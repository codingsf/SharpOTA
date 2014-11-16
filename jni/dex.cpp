
#include "dex.h"
#include "log.h"
#include <android_runtime/AndroidRuntime.h>

int dex_invoke_main(const char* dexPath, const char* dexOptDir, const char* className, const char* methodName, int argc, char *argv[]) {
    JNIEnv* env;
    jclass stringClass, classLoaderClass, dexClassLoaderClass, targetClass;
    jmethodID getSystemClassLoaderMethod, dexClassLoaderContructor, loadClassMethod, targetMethod;
    jobject systemClassLoaderObject, dexClassLoaderObject;
    jstring dexPathString, dexOptDirString, classNameString, tmpString;    
    jobjectArray stringArray;

    env = android::AndroidRuntime::getJNIEnv();
    if (env == NULL) {
        LOGE("JNIEnv == NULL!\n");
        return -1;
    }

    stringClass = env->FindClass("java/lang/String");
    classLoaderClass = env->FindClass("java/lang/ClassLoader");
    // XXX: will this fail in ART?
    dexClassLoaderClass = env->FindClass("dalvik/system/DexClassLoader");
    getSystemClassLoaderMethod = env->GetStaticMethodID(classLoaderClass, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    systemClassLoaderObject = env->CallStaticObjectMethod(classLoaderClass, getSystemClassLoaderMethod);

    dexClassLoaderContructor = env->GetMethodID(dexClassLoaderClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    dexPathString = env->NewStringUTF(dexPath);
    dexOptDirString = env->NewStringUTF(dexOptDir);
    dexClassLoaderObject = env->NewObject(dexClassLoaderClass, dexClassLoaderContructor, dexPathString, dexOptDirString, NULL, systemClassLoaderObject);

    loadClassMethod = env->GetMethodID(dexClassLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    classNameString = env->NewStringUTF(className);
    targetClass = (jclass)env->CallObjectMethod(dexClassLoaderObject, loadClassMethod, classNameString);
    if (!targetClass) {
        // TODO: many local ref leak
        LOGE("load class %s failed", className);
        return -1;
    }

    targetMethod = env->GetStaticMethodID(targetClass, methodName, "([Ljava/lang/String;)V");
    if (!targetMethod) {
        // TODO: many local ref leak
        LOGE("get static method ID %s failed", methodName);
        return -1;
    }

    if (argc > 0) {
        stringArray = env->NewObjectArray(argc, stringClass, NULL);
        for (int i = 0; i < argc; i++) {
            tmpString = env->NewStringUTF(argv[i]);
            env->SetObjectArrayElement(stringArray, i, tmpString);
        }
    } else {
        stringArray = NULL;
    }

    LOGD("call %s.%s() bgn\n", className, methodName);
    env->CallStaticVoidMethod(targetClass, targetMethod, stringArray);
    LOGD("call %s.%s() end\n", className, methodName);

    // TODO: many local ref leak

    return 0;
}

