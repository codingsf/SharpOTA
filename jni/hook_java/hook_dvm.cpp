
#include "log.h"
#include "hook_java.h"
#include "hook_dvm.h"
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dlfcn.h>
#include <pthread.h>

// XXX: this is tested only with Android 4.4.4

static int g_load_dvm = 0;
static pthread_mutex_t g_load_dvm_lock = PTHREAD_MUTEX_INITIALIZER;

static dvmUseJNIBridge_t dvmUseJNIBridge;
static dvmCallJNIMethod_t dvmCallJNIMethod;

static int loadDVM() {
    void *h;

    if (g_load_dvm)
        return 0;
    pthread_mutex_lock(&g_load_dvm_lock);
    h = dlopen("libdvm.so", RTLD_NOW);
    if (h) {
        dvmUseJNIBridge = (dvmUseJNIBridge_t) dlsym(h, "_Z15dvmUseJNIBridgeP6MethodPv");
        dvmCallJNIMethod = (dvmCallJNIMethod_t) dlsym(h, "_Z16dvmCallJNIMethodPKjP6JValuePK6MethodP6Thread");
        if (dvmUseJNIBridge &&
            dvmCallJNIMethod)
            g_load_dvm = 1;
        else
            g_load_dvm = 0;
    } else {
        LOGE("dlopen: %s", strerror(errno));
    }
    pthread_mutex_unlock(&g_load_dvm_lock);

    return g_load_dvm == 1 ? 0 : -1;
}

// XXX: version aware?
// count include return value on my SH-04F 4.4.4
static int getMethodArgsSize(const char *shorty) {
    int count = 0;

    for (;;) {
        switch ((*shorty++)) {
        case '\0':
            return count;
        case 'D':
        case 'J': {
            count += 2;
            break;
        }
        default: {
            count += 1;
            break;
        }
        }
    }

    return count;
}

#ifdef __arm__
// from vm/arch/arm/HintsEABI.cpp
static u4 dvmPlatformInvokeHints(const char* shorty)
{
    const char* sig = shorty;
    int padFlags, jniHints;
    char sigByte;
    int stackOffset, padMask;

    stackOffset = padFlags = 0;
    padMask = 0x00000001;

    /* Skip past the return type */
    sig++;

    while (true) {
        sigByte = *(sig++);

        if (sigByte == '\0')
            break;

        if (sigByte == 'D' || sigByte == 'J') {
            if ((stackOffset & 1) != 0) {
                padFlags |= padMask;
                stackOffset++;
                padMask <<= 1;
            }
            stackOffset += 2;
            padMask <<= 2;
        } else {
            stackOffset++;
            padMask <<= 1;
        }
    }

    jniHints = 0;

    if (stackOffset > DALVIK_JNI_COUNT_SHIFT) {
        /* too big for "fast" version */
        jniHints = DALVIK_JNI_NO_ARG_INFO;
    } else {
        assert((padFlags & (0xffffffff << DALVIK_JNI_COUNT_SHIFT)) == 0);
        stackOffset -= 2;           // r2/r3 holds first two items
        if (stackOffset < 0)
            stackOffset = 0;
        jniHints |= ((stackOffset+1) / 2) << DALVIK_JNI_COUNT_SHIFT;
        jniHints |= padFlags;
    }

    return jniHints;
}
#else
#error "Implement me!"
#endif

// from vm/Native.cpp
static int dvmComputeJniArgInfo(const char* shorty)
{
    const char* sig = shorty;
    int returnType, jniArgInfo;
    u4 hints;

    /* The first shorty character is the return type. */
    switch (*(sig++)) {
    case 'V':
        returnType = DALVIK_JNI_RETURN_VOID;
        break;
    case 'F':
        returnType = DALVIK_JNI_RETURN_FLOAT;
        break;
    case 'D':
        returnType = DALVIK_JNI_RETURN_DOUBLE;
        break;
    case 'J':
        returnType = DALVIK_JNI_RETURN_S8;
        break;
    case 'Z':
    case 'B':
        returnType = DALVIK_JNI_RETURN_S1;
        break;
    case 'C':
        returnType = DALVIK_JNI_RETURN_U2;
        break;
    case 'S':
        returnType = DALVIK_JNI_RETURN_S2;
        break;
    default:
        returnType = DALVIK_JNI_RETURN_S4;
        break;
    }

    jniArgInfo = returnType << DALVIK_JNI_RETURN_SHIFT;

    hints = dvmPlatformInvokeHints(shorty);

    if (hints & DALVIK_JNI_NO_ARG_INFO) {
        jniArgInfo |= DALVIK_JNI_NO_ARG_INFO;
    } else {
        assert((hints & DALVIK_JNI_RETURN_MASK) == 0);
        jniArgInfo |= hints;
    }

    return jniArgInfo;
}

static jclass hsdk_find_clz1(JNIEnv *env, const char *clz) {
    jclass ret;

    ret = env->FindClass(clz);
    if (ret == NULL) {
        if (env->ExceptionOccurred())
            env->ExceptionClear();
    }
    return ret;
}

static jclass hsdk_find_clz2(JNIEnv *env, const char *clz) {
    jstring className;
    jclass clz_ApplicationLoaders;
    jfieldID fid_gApplicationLoaders;
    jobject obj_gApplicationLoaders;
    jfieldID fid_mLoaders;
    jobject obj_mLoaders;
    jclass clz_Map;
    jmethodID mid_Map_values;
    jobject obj_mLoaders_values;
    jclass clz_Collection;
    jmethodID mid_Collection_toArray;
    jobjectArray obj_mLoaders_array;
    jint i, nloaders;

    clz_ApplicationLoaders = env->FindClass("android/app/ApplicationLoaders");
    if (clz_ApplicationLoaders == NULL) {
        LOGD("hook_dvm: find_clz2 %d", __LINE__);
        if (env->ExceptionOccurred())
            env->ExceptionClear();
        return NULL;
    }
    fid_gApplicationLoaders = env->GetStaticFieldID(clz_ApplicationLoaders, "gApplicationLoaders", "Landroid/app/ApplicationLoaders;");
    if (fid_gApplicationLoaders == NULL) {
        LOGD("hook_dvm: find_clz2 %d", __LINE__);
        if (env->ExceptionOccurred())
            env->ExceptionClear();
        env->DeleteLocalRef(clz_ApplicationLoaders);
        return NULL;
    }
    fid_mLoaders = env->GetFieldID(clz_ApplicationLoaders, "mLoaders", "Ljava/util/Map;");
    if (fid_mLoaders == NULL) {
        if (env->ExceptionOccurred())
            env->ExceptionClear();
    }
    // try again with ArrayMap
    fid_mLoaders = env->GetFieldID(clz_ApplicationLoaders, "mLoaders", "Landroid/util/ArrayMap;");
    if (fid_mLoaders == NULL) {
        if (env->ExceptionOccurred())
            env->ExceptionClear();
    }
    env->DeleteLocalRef(clz_ApplicationLoaders);
    if (fid_mLoaders == NULL) {
        LOGD("hook_dvm: find_clz2 %d", __LINE__);
        return NULL;
    }
    obj_gApplicationLoaders = env->GetStaticObjectField(clz_ApplicationLoaders, fid_gApplicationLoaders);
    if (obj_gApplicationLoaders == NULL) {
        LOGD("hook_dvm: find_clz2 %d", __LINE__);
        return NULL;
    }
    obj_mLoaders = env->GetObjectField(obj_gApplicationLoaders, fid_mLoaders);
    if (obj_mLoaders == NULL) {
        LOGD("hook_dvm: find_clz2 %d", __LINE__);
        return NULL;
    }
    clz_Map = env->GetObjectClass(obj_mLoaders);
    mid_Map_values = env->GetMethodID(clz_Map, "values", "()Ljava/util/Collection;");
    obj_mLoaders_values = env->CallObjectMethod(obj_mLoaders, mid_Map_values);
    env->DeleteLocalRef(clz_Map);
    if (obj_mLoaders_values == NULL) {
        LOGD("hook_dvm: find_clz2 %d", __LINE__);
        return NULL;
    }
    clz_Collection = env->GetObjectClass(obj_mLoaders_values);
    mid_Collection_toArray = env->GetMethodID(clz_Collection, "toArray", "()[Ljava/lang/Object;");
    obj_mLoaders_array = (jobjectArray) env->CallObjectMethod(obj_mLoaders_values, mid_Collection_toArray);
    env->DeleteLocalRef(clz_Collection);
    if (obj_mLoaders_array == NULL) {
        LOGD("hook_dvm: find_clz2 %d", __LINE__);
        return NULL;
    }
    nloaders = env->GetArrayLength(obj_mLoaders_array);
    if (nloaders > 0) {
        className = env->NewStringUTF(clz);
        for(i = 0 ; i < nloaders ; i ++) {
            jobject obj_cl;
            jclass clz_cl, ret;
            jmethodID mid_cl_loadClass;

            obj_cl = env->GetObjectArrayElement(obj_mLoaders_array, i);
            if (obj_cl == NULL)
                continue;
            clz_cl = env->GetObjectClass(obj_cl);
            mid_cl_loadClass = env->GetMethodID(clz_cl, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
            ret = (jclass) env->CallObjectMethod(obj_cl, mid_cl_loadClass, className);
            env->DeleteLocalRef(clz_cl);
            if (env->ExceptionOccurred()) {
                env->ExceptionClear();
                continue;
            }
            if (ret)
                return ret;
        }
    }

    return NULL;
}

static jclass hsdk_find_clz3(JNIEnv *env, const char *clz) {
    jclass ret = NULL;
    jclass clz_Class;
    jmethodID mid_Class_forName;

    clz_Class = env->FindClass("java/lang/Class");
    if (clz_Class == NULL) {
        LOGD("hook_dvm: find_clz3 %d", __LINE__);
        if (env->ExceptionOccurred())
            env->ExceptionClear();
        return NULL;
    }
    mid_Class_forName = env->GetStaticMethodID(clz_Class, "forName", "(Ljava/lang/String;)Ljava/lang/Class;");
    if (mid_Class_forName == NULL) {
        LOGD("hook_dvm: find_clz3 %d", __LINE__);
        if (env->ExceptionOccurred())
            env->ExceptionClear();
    } else {
        ret = (jclass) env->CallStaticObjectMethod(clz_Class, mid_Class_forName,
            env->NewStringUTF(clz));
    }
    env->DeleteLocalRef(clz_Class);
    if (ret == NULL) {
        LOGD("hook_dvm: find_clz3 %d", __LINE__);
        if (env->ExceptionOccurred())
            env->ExceptionClear();
        return NULL;
    }
    return ret;
}

static jclass hsdk_find_clz(JNIEnv *env, const char *clz) {
    jclass classClz;
    int i, len;
    char *dot;

    dot = strdup(clz);
    if (!dot)
        return NULL;
    len = strlen(dot);
    for (i = 0; i < len; i++) {
        if (dot[i] == '/')
            dot[i] = '.';
    }
    classClz = hsdk_find_clz1(env, clz);
    if (classClz == NULL) {
        classClz = hsdk_find_clz2(env, dot);
        if (classClz == NULL)
            classClz = hsdk_find_clz3(env, dot);
    }
    free(dot);
    return classClz;
}

static void hsdk_bridge_func(const u4 *args, JValue *pResult,
    const Method *method, struct Thread *self) {

}

extern "C" int hook_dvm(JNIEnv *env, struct hook_java_args *args) {
    jclass clz;
    jmethodID jm;
    bool isStatic = false;
    struct hook_java_args *ha = NULL;
    Method *method;
    int argsSize;

    if (loadDVM()) {
        LOGE("hook_dvm: loadDVM() failed");
        return -1;
    }
    if ((args->prev || args->post) && args->func) {
        LOGE("hook_dvm: invalid argument");
        return -1;
    }
    clz = hsdk_find_clz(env, args->clz);
    if (clz == NULL) {
        LOGE("hook_dvm: class `%s\' not found", args->clz);
        return -1;
    }
    jm = env->GetMethodID(clz, args->mtd, args->sig);
    if (jm == NULL) {
        if (env->ExceptionOccurred())
            env->ExceptionClear();
        jm = env->GetStaticMethodID(clz, args->mtd, args->sig);
        if (env->ExceptionOccurred())
            env->ExceptionClear();
        if (jm == NULL) {
            LOGE("hook_dvm: method `%s\' with sig `%s\' not found", args->mtd, args->sig);
            env->DeleteLocalRef(clz);
            return -1;
        }
        isStatic = true;
    }
    env->DeleteLocalRef(clz);
    method = (Method *) jm;
    if ((method->nativeFunc == hsdk_bridge_func) ||
        (method->nativeFunc == dvmCallJNIMethod && method->insns == args->func)) {
        LOGD("hook_dvm: already hooked");
        return 0;
    }
    // hook
    args->old = calloc(1, sizeof(*method));
    if (args->old)
        memcpy(args->old, jm, sizeof(*method));
    if (!args->func) {
        ha = (struct hook_java_args *) calloc(1, sizeof(*ha));
        if (!ha) {
            LOGE("hook_dvm: calloc failed");
            return -1;
        }
        ha->clz = strdup(args->clz);
        ha->mtd = strdup(args->mtd);
        ha->sig = strdup(args->sig);
        ha->prev = args->prev;
        ha->post = args->post;
    }
    argsSize = getMethodArgsSize(method->shorty);
    if (isStatic)
        argsSize += 1;
    method->registersSize = argsSize;
    method->insSize = argsSize;
    method->outsSize = 0;
    method->jniArgInfo = dvmComputeJniArgInfo(method->shorty);
    if (!args->func) {
        method->insns = (const u2*) ha;
        method->nativeFunc = hsdk_bridge_func;
    } else {
        dvmUseJNIBridge(method, args->func);
    }
    SET_METHOD_FLAG(method, ACC_NATIVE);
    if (args->func)
        LOGD("hook_dvm: L%s;%s->%s => %p", args->clz, args->mtd, args->sig, args->func);

    return 0;
}

