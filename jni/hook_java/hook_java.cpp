
#include "hook_java.h"
#include <cutils/properties.h>
#include <android_runtime/AndroidRuntime.h>

extern "C" int hook_dvm(JNIEnv *, struct hook_java_args *);
extern "C" int hook_art(JNIEnv *, struct hook_java_args *);

void *get_java_vm() {
    return android::AndroidRuntime::getJavaVM();
}

void *get_jni_env() {
    return android::AndroidRuntime::getJNIEnv();
}

int hook_java(void *env, struct hook_java_args *args) {
    char val[PROPERTY_VALUE_MAX];

    property_get("persist.sys.dalvik.vm.lib", val, "");
    if (!strcmp(val, "libdvm.so") || *val == 0)
        return hook_dvm((JNIEnv *) env, args);
    if (!strcmp(val, "libart.so"))
        return hook_art((JNIEnv *) env, args);
    return -1;
}

