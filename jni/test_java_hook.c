
#include "log.h"
#include "hook_java.h"
#include <jni.h>
#include <string.h>

static jint testJavaHook_add(JNIEnv *env, jobject thiz, jint a, jint b) {
    LOGV("testJavaHook_add: a = %d, b = %d", a, b);
    return 2 * (a + b);
}

int Java_org_stagex_hooksdk_TestActivity_testJavaHook(JNIEnv *env, jobject thiz) {
    struct hook_java_args args;

    memset(&args, 0, sizeof(args));
    args.clz = "org/stagex/hooksdk/TestActivity";
    args.mtd = "testJavaHook_add";
    args.sig = "(II)I";
    args.func = testJavaHook_add;

    return hook_java(env, &args);
}

