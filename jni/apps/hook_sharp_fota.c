
#include "log.h"
#include "hook_java.h"
#include <cutils/properties.h>
#include <jni.h>
#include <string.h>

// return false to fake not roaming
static jboolean jp_co_sharp_android_FotaApp_FOTA_Utility_isRoaming_Z(JNIEnv *env, jobject thiz) {
    LOGV("emulate isRoaming = false");
    return JNI_FALSE;
}

// return 2 to fake mobile data is connected
static jint com_nttdocomo_android_fota_DmcUtil_getDataState_I(JNIEnv *env, jobject thiz) {
    LOGV("emulate getDataState = 2");
    return 2;
}

// return false to fake not roaming
static jboolean com_nttdocomo_android_fota_DmcUtil_getRoaming_Z(JNIEnv *env, jobject thiz) {
    LOGV("emulate getRoaming = false");
    return JNI_FALSE;
}

// return true to fake a Docomo UIM inserted
static jboolean com_nttdocomo_android_fota_DmcIpl_isDocomoUIM_Z(JNIEnv *env, jobject thiz) {
    LOGV("emulate isDocomoUIM = true");
    return JNI_TRUE;
}

// return true to fake mobile data used
static jboolean com_nttdocomo_android_fota_DmcIpl_checkNetworkDataState_ZZ(JNIEnv *env, jobject thiz, jboolean jb) {
    LOGV("emulate checkNetworkDataState = true");
    return JNI_TRUE;
}

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

static struct hook_java_args sbm303sh_stubs[] = {
    { .clz = "jp/co/sharp/android/FotaApp/FOTA_Utility", .mtd = "isRoaming", .sig = "()Z", .func = jp_co_sharp_android_FotaApp_FOTA_Utility_isRoaming_Z },
    { .clz = 0 },
};

static struct hook_java_args sh04f_stubs[] = {
    { .clz = "com/nttdocomo/android/fota/DmcUtil", .mtd = "getDataState", .sig = "()I", .func = com_nttdocomo_android_fota_DmcUtil_getDataState_I },
    { .clz = "com/nttdocomo/android/fota/DmcUtil", .mtd = "getRoaming", .sig = "()Z", .func = com_nttdocomo_android_fota_DmcUtil_getRoaming_Z },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "checkNetworkDataState", .sig = "(Z)Z", .func = com_nttdocomo_android_fota_DmcIpl_checkNetworkDataState_ZZ },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "isDocomoUIM", .sig = "()Z", .func = com_nttdocomo_android_fota_DmcIpl_isDocomoUIM_Z },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "iplGetFotaNetworkSettings", .sig = "()I", .func = com_nttdocomo_android_fota_DmcIpl_iplGetFotaNetworkSettings_I },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "iplSetFotaNetworkSettings", .sig = "()I", .func = com_nttdocomo_android_fota_DmcIpl_iplSetFotaNetworkSettings_I },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "iplRestoreFotaNetworkSettings", .sig = "()V", .func = com_nttdocomo_android_fota_DmcIpl_iplRestoreFotaNetworkSettings_V },
    { .clz = "com/nttdocomo/android/fota/DmcIpl", .mtd = "isDocomoUIM", .sig = "()Z", .func = com_nttdocomo_android_fota_DmcIpl_isDocomoUIM_Z },
    { .clz = 0 },
};

struct sharp_model_info {
    const char *model;
    struct hook_java_args *stubs;
};

static struct sharp_model_info mtab[] = {
    { .model = "SBM303SH", .stubs = sbm303sh_stubs },
    { .model = "SH-04F", .stubs = sh04f_stubs },
};

int SharpMain(int argc, char *argv[]) {
    int rc, i;
    JNIEnv *env = NULL;
    char model[PROPERTY_VALUE_MAX];

    env = get_jni_env();
    if (env == NULL) {
        LOGE("JNIEnv == NULL");
        return -1;
    }
    property_get("ro.product.model", model, "");
    for (i = 0; i < sizeof(mtab) / sizeof(mtab[0]); i++) {
        struct sharp_model_info *info = &mtab[i];

        if (!strcmp(info->model, model)) {
            struct hook_java_args *args;

            for (args = info->stubs; args->clz; args++) {
                rc = hook_java(env, args);
                if (rc) {
                    LOGE("hook_java failed: L%s;->%s%s", args->clz, args->mtd, args->sig);
                    break;
                }
            }

            return rc;
        }
    }

    return -1;
}

