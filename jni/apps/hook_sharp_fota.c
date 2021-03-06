
#include "log.h"
#include "hook_java.h"
#include <cutils/properties.h>
#include <string.h>
#include <jni.h>
#include <dlfcn.h>

// return 0 means good
static jint com_nttdocomo_android_fota_DmcIpl_iplGetFotaNetworkSettings_I(JNIEnv *env, jobject thiz) {
    LOGV("emulate iplGetFotaNetworkSettings = 0");
    return 0;
}

// return 0 means good, avoid swithing to "Software Update" APN
static jint com_nttdocomo_android_fota_DmcIpl_iplSetFotaNetworkSettings_I(JNIEnv *env, jobject thiz) {
    LOGV("emulate iplSetFotaNetworkSettings = 0");
    return 0;
}

// restore from "Software Update" APN
static void com_nttdocomo_android_fota_DmcIpl_iplRestoreFotaNetworkSettings_V(JNIEnv *env, jobject thiz) {
    LOGV("emulate iplRestoreFotaNetworkSettings");
}

#if 1
// isRooted() => false
static jboolean com_nttdocomo_android_fota_DmcUtil_isRooted_Z(JNIEnv *env, jobject thiz) {
    return 0;
}
#endif

#if 0
static jboolean java_io_File_exists_Z(JNIEnv *env, jobject thiz) {
    jclass clz_File;
    jmethodID mid_exists;
    jfieldID fid_path;
    jobject obj_path;
    jboolean ret = 0;

    LOGV("enter File.exists()");
    if (thiz == NULL)
        return 0;
    clz_File = (*env)->GetObjectClass(env, thiz);
    if (clz_File == NULL) {
        if ((*env)->ExceptionOccurred(env))
            (*env)->ExceptionClear(env);
        return 0;
    }
    mid_exists = (*env)->GetMethodID(env, clz_File, "exists", "()Z");
    ret = (*env)->CallBooleanMethod(env, thiz, mid_exists);
    fid_path = (*env)->GetFieldID(env, clz_File, "path", "Ljava/lang/String;");
    obj_path = (*env)->GetObjectField(env, thiz, fid_path);
    if (obj_path) {
        const char *cstr_path;

        cstr_path = (*env)->GetStringUTFChars(env, obj_path, NULL);
        if (cstr_path) {
            LOGV("origin File.exists() returned %d", ret);
            if (!strcmp(cstr_path, "/system/bin/su") ||
                !strcmp(cstr_path, "/system/xbin/su"))
                ret = 0;
            (*env)->ReleaseStringUTFChars(env, obj_path, cstr_path);
            LOGV("hooked File.exists() returned %d", ret);
        }    
    }
    (*env)->DeleteLocalRef(env, obj_path);
    (*env)->DeleteLocalRef(env, fid_path);
    (*env)->DeleteLocalRef(env, mid_exists);
    return ret;
}
#endif

// return DATA_CONNECTED
static jint android_telephony_TelephonyManager_getDataState_I(JNIEnv *env, jobject thiz) {
    return 2;
}

// return SIM_STATE_READY
static jint android_telephony_TelephonyManager_getSimState_I(JNIEnv *env, jobject thiz) {
    return 5;
}

// return false
static jboolean android_telephony_TelephonyManager_isNetworkRoaming_Z(JNIEnv *env, jobject thiz) {
    return 0;
}

// return DOCOMO
static jstring android_telephony_TelephonyManager_getSimOperator_L_dcm(JNIEnv *env, jobject thiz) {
    return (*env)->NewStringUTF(env, "44010");
}

// return SoftBank
static jstring android_telephony_TelephonyManager_getSimOperator_L_sbm(JNIEnv *env, jobject thiz) {
    return (*env)->NewStringUTF(env, "44020");
}

// return DOCOMO
static jstring android_telephony_TelephonyManager_getNetworkOperator_L_dcm(JNIEnv *env, jobject thiz) {
    return (*env)->NewStringUTF(env, "44010");
}

// return SoftBank
static jstring android_telephony_TelephonyManager_getNetworkOperator_L_sbm(JNIEnv *env, jobject thiz) {
    return (*env)->NewStringUTF(env, "44020");
}

// return a real DOCOMO phone number
static jstring android_telephony_TelephonyManager_getLine1Number_L_dcm(JNIEnv *env, jobject thiz) {
    // const char *no = "09026553215";
    const char *no = "08020080582";
    LOGV("emualte getLine1Number: %s", no);
    return (*env)->NewStringUTF(env, no);
}

// return a real SoftBank phone number
static jstring android_telephony_TelephonyManager_getLine1Number_L_sbm(JNIEnv *env, jobject thiz) {
    const char *no = "09060385755";
    LOGV("emualte getLine1Number: %s", no);
    return (*env)->NewStringUTF(env, no);
}

// return a real DOCOMO IMSI
static jstring android_telephony_TelephonyManager_getSubscriberId_L_dcm(JNIEnv *env, jobject thiz) {
    // const char *imsi = "440103114861967";
    const char *imsi = "440103068936867";
    LOGV("emualte getSubscriberId: %s", imsi);
    return (*env)->NewStringUTF(env, imsi);
}

// return a real SoftBank IMSI
static jstring android_telephony_TelephonyManager_getSubscriberId_L_sbm(JNIEnv *env, jobject thiz) {
    const char *imsi = "440201020071369";
    LOGV("emualte getSubscriberId: %s", imsi);
    return (*env)->NewStringUTF(env, imsi);
}

static struct hook_java_args gen_dcm_osv_stubs[] = {
    { .clz = "android/telephony/TelephonyManager", .mtd = "getDataState", .sig = "()I", .func = android_telephony_TelephonyManager_getDataState_I },
    { .clz = "android/telephony/TelephonyManager", .mtd = "isNetworkRoaming", .sig = "()Z", .func = android_telephony_TelephonyManager_isNetworkRoaming_Z },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getLine1Number", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getLine1Number_L_dcm },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getNetworkOperator", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getNetworkOperator_L_dcm },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSimOperator", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getSimOperator_L_dcm},
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSimState", .sig = "()I", .func = android_telephony_TelephonyManager_getSimState_I },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSubscriberId", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getSubscriberId_L_dcm },
    { .clz = "com/nttdocomo/android/osv/Utils", .mtd = "isRooted", .sig = "()Z", .func = com_nttdocomo_android_fota_DmcUtil_isRooted_Z },
    { .clz = 0 },
};

static struct hook_java_args gen_dcm_fota_stubs[] = {
    { .clz = "android/telephony/TelephonyManager", .mtd = "getDataState", .sig = "()I", .func = android_telephony_TelephonyManager_getDataState_I },
    { .clz = "android/telephony/TelephonyManager", .mtd = "isNetworkRoaming", .sig = "()Z", .func = android_telephony_TelephonyManager_isNetworkRoaming_Z },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getLine1Number", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getLine1Number_L_dcm },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getNetworkOperator", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getNetworkOperator_L_dcm },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSimOperator", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getSimOperator_L_dcm},
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSimState", .sig = "()I", .func = android_telephony_TelephonyManager_getSimState_I },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSubscriberId", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getSubscriberId_L_dcm },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "iplGetFotaNetworkSettings", .sig = "()I", .func = com_nttdocomo_android_fota_DmcIpl_iplGetFotaNetworkSettings_I },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "iplSetFotaNetworkSettings", .sig = "()I", .func = com_nttdocomo_android_fota_DmcIpl_iplSetFotaNetworkSettings_I },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "iplRestoreFotaNetworkSettings", .sig = "()V", .func = com_nttdocomo_android_fota_DmcIpl_iplRestoreFotaNetworkSettings_V },
    { .clz = "com/nttdocomo/android/fota/DmcUtil", .mtd = "isRooted", .sig = "()Z", .func = com_nttdocomo_android_fota_DmcUtil_isRooted_Z },
    { .clz = 0 },
};

static struct hook_java_args gen_sbm_fota_stubs[] = {
    { .clz = "android/telephony/TelephonyManager", .mtd = "getDataState", .sig = "()I", .func = android_telephony_TelephonyManager_getDataState_I },
    { .clz = "android/telephony/TelephonyManager", .mtd = "isNetworkRoaming", .sig = "()Z", .func = android_telephony_TelephonyManager_isNetworkRoaming_Z },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getLine1Number", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getLine1Number_L_sbm },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getNetworkOperator", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getNetworkOperator_L_sbm },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSimOperator", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getSimOperator_L_sbm},
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSimState", .sig = "()I", .func = android_telephony_TelephonyManager_getSimState_I },
    { .clz = "android/telephony/TelephonyManager", .mtd = "getSubscriberId", .sig = "()Ljava/lang/String;", .func = android_telephony_TelephonyManager_getSubscriberId_L_sbm },
    { .clz = 0 },
};

struct sharp_model_info {
    const char *model;
    struct hook_java_args *stubs;
};

static struct sharp_model_info mtab_fota[] = {
    { .model = "SBM302SH", .stubs = gen_sbm_fota_stubs },
    { .model = "SBM303SH", .stubs = gen_sbm_fota_stubs },
    { .model = "304SH", .stubs = gen_sbm_fota_stubs },
    { .model = "305SH", .stubs = gen_sbm_fota_stubs },
    { .model = "402SH", .stubs = gen_sbm_fota_stubs },
    { .model = "SH-06E", .stubs = gen_dcm_fota_stubs },
    { .model = "SH-01F", .stubs = gen_dcm_fota_stubs },
    { .model = "SH-02F", .stubs = gen_dcm_fota_stubs },
    { .model = "SH-03F", .stubs = gen_dcm_fota_stubs },
    { .model = "SH-04F", .stubs = gen_dcm_fota_stubs },
    { .model = "SH-05F", .stubs = gen_dcm_fota_stubs },
    { .model = "SH-06F", .stubs = gen_dcm_fota_stubs },
    { .model = "SH-01G", .stubs = gen_dcm_fota_stubs },
    { .model = "SH-02G", .stubs = gen_dcm_fota_stubs },
};

static struct sharp_model_info mtab_osv[] = {
    { .model = "SH-06E", .stubs = gen_dcm_osv_stubs },
    { .model = "SH-01F", .stubs = gen_dcm_osv_stubs },
    { .model = "SH-02F", .stubs = gen_dcm_osv_stubs },
    { .model = "SH-03F", .stubs = gen_dcm_osv_stubs },
    { .model = "SH-04F", .stubs = gen_dcm_osv_stubs },
    { .model = "SH-05F", .stubs = gen_dcm_osv_stubs },
    { .model = "SH-06F", .stubs = gen_dcm_osv_stubs },
    { .model = "SH-01G", .stubs = gen_dcm_osv_stubs },
    { .model = "SH-02G", .stubs = gen_dcm_osv_stubs },
};

int SharpMain(int argc, char *argv[]) {
    int rc, i, n;
    struct sharp_model_info *mtab;
    JNIEnv *env = NULL;
    char model[PROPERTY_VALUE_MAX];

    if (argc != 1) {
        LOGE("invalid args");
        return -1;
    }
    if (!strcmp(argv[0], "com.nttdocomo.android.fota") ||
        !strcmp(argv[0], "jp.co.sharp.android.FotaApp") ||
        !strcmp(argv[0], "com.redbend.dmClient")) {
        mtab = mtab_fota;
        n = sizeof(mtab_fota) / sizeof(*mtab);
    } else if (!strcmp(argv[0], "com.nttdocomo.android.osv")) {
        mtab = mtab_osv;
        n = sizeof(mtab_osv) / sizeof(*mtab);
    } else {
        LOGE("invalid args");
        return -1;
    }
    env = get_jni_env();
    if (env == NULL) {
        LOGE("JNIEnv == NULL");
        return -1;
    }
    property_get("ro.product.model", model, "");
    for (i = 0; i < n; i++) {
        struct sharp_model_info *info = &mtab[i];
        int ret = 0;

        if (!strcmp(info->model, model)) {
            struct hook_java_args *args;

            for (args = info->stubs; args->clz; args++) {
                rc = hook_java(env, args);
                ret |= rc;
                if (rc) {
                    LOGE("hook_java failed: L%s;->%s%s", args->clz, args->mtd, args->sig);
                    continue;
                }
            }

            return ret;
        }
    }

    return -1;
}

