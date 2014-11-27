
#include "hook_java.h"

struct JNIEnv;

extern "C" int hook_art(JNIEnv *env, struct hook_java_args *args) {
    return -1;
}

