
#include "log.h"
#include "hook_java.h"
#include <jni.h>
#include <string.h>

// return 2 to fake mobile data is connected
static jint com_nttdocomo_android_fota_DmcUtil_getDataState(JNIEnv *env, jobject thiz) {
    LOGV("emulate getDataState = 2");
    return 2;
}

// return false to fake not roaming
static jboolean com_nttdocomo_android_fota_DmcUtil_getRoaming(JNIEnv *env, jobject thiz) {
    LOGV("emulate getRoaming = false");
    return JNI_FALSE;
}

// return true to fake a Docomo UIM inserted
static jboolean com_nttdocomo_android_fota_DmcIpl_isDocomoUIM(JNIEnv *env, jobject thiz) {
    LOGV("emulate isDocomoUIM = true");
    return JNI_TRUE;
}

// return true to fake mobile data used
static jboolean com_nttdocomo_android_fota_DmcIpl_checkNetworkDataState(JNIEnv *env, jobject thiz, jboolean jb) {
    LOGV("emulate checkNetworkDataState = true");
    return JNI_TRUE;
}

// return 0 means good
static jint com_nttdocomo_android_fota_DmcIpl_iplGetFotaNetworkSettings(JNIEnv *env, jobject thiz) {
    LOGV("emulate iplGetFotaNetworkSettings = 0");
    return 0;
}

// return 0 means good, avoid swithing to "Software Update" APN
static jint com_nttdocomo_android_fota_DmcIpl_iplSetFotaNetworkSettings(JNIEnv *env, jobject thiz) {
    LOGV("emulate iplSetFotaNetworkSettings = 0");
    return 0;
}

// restore from "Software Update" APN
static void com_nttdocomo_android_fota_DmcIpl_iplRestoreFotaNetworkSettings(JNIEnv *env, jobject thiz) {
    LOGV("emulate iplRestoreFotaNetworkSettings");
}

int DocomoMain(int argc, char *argv[]) {
    int rc;
    JNIEnv *env = NULL;
    struct hook_java_args args;

    env = get_jni_env();
    memset(&args, 0, sizeof(args));
    args.clz = "com/nttdocomo/android/fota/DmcUtil";
    args.mtd = "getDataState";
    args.sig = "()I";
    args.func = com_nttdocomo_android_fota_DmcUtil_getDataState;
    rc = hook_java(env, &args);
    if (rc) {
        LOGE("[-] %s;->%s%s", args.clz, args.mtd, args.sig);
        return rc;
    }
    args.mtd = "getRoaming";
    args.sig = "()Z";
    args.func = com_nttdocomo_android_fota_DmcUtil_getRoaming;
    rc = hook_java(env, &args);
    if (rc) {
        LOGE("[-] %s;->%s%s", args.clz, args.mtd, args.sig);
        return rc;
    }
    args.clz = "com/nttdocomo/android/fota/DmcIpl";
    args.mtd = "isDocomoUIM";
    args.sig = "()Z";
    args.func = com_nttdocomo_android_fota_DmcIpl_isDocomoUIM;
    rc = hook_java(env, &args);
    if (rc) {
        LOGE("[-] %s;->%s%s", args.clz, args.mtd, args.sig);
        return rc;
    }
    args.mtd = "checkNetworkDataState";
    args.sig = "(Z)Z";
    args.func = com_nttdocomo_android_fota_DmcIpl_checkNetworkDataState;
    rc = hook_java(env, &args);
    if (rc) {
        LOGE("[-] %s;->%s%s", args.clz, args.mtd, args.sig);
        return rc;
    }
    args.mtd = "iplGetFotaNetworkSettings";
    args.sig = "()I";
    args.func = com_nttdocomo_android_fota_DmcIpl_iplGetFotaNetworkSettings;
    rc = hook_java(env, &args);
    if (rc) {
        LOGE("[-] %s;->%s%s", args.clz, args.mtd, args.sig);
        return rc;
    }
    args.mtd = "iplSetFotaNetworkSettings";
    args.sig = "()I";
    args.func = com_nttdocomo_android_fota_DmcIpl_iplSetFotaNetworkSettings;
    rc = hook_java(env, &args);
    if (rc) {
        LOGE("[-] %s;->%s%s", args.clz, args.mtd, args.sig);
        return rc;
    }
    args.mtd = "iplRestoreFotaNetworkSettings";
    args.sig = "()V";
    args.func = com_nttdocomo_android_fota_DmcIpl_iplRestoreFotaNetworkSettings;
    rc = hook_java(env, &args);
    if (rc) {
        LOGE("[-] %s;->%s%s", args.clz, args.mtd, args.sig);
        return rc;
    }

    return 0;
}

int SoftBankMain(int argc, char *argv[]) {
    return -1;
}

