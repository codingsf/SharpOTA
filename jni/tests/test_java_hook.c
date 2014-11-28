
#include "log.h"
#include "hook_java.h"
#include <jni.h>
#include <string.h>

static jint testJavaHook_add(JNIEnv *env, jobject thiz, jint a, jint b) {
    LOGV("testJavaHook_add: a = %d, b = %d", a, b);
    return 2 * (a + b);
}

static jint testJavaHook_sub(JNIEnv *env, jclass clz, jint a, jint b) {
    LOGV("testJavaHook_sub: a = %d, b = %d", a, b);
    return 2 * (a - b);
}

int Java_org_stagex_hooksdk_TestActivity_testJavaHook(JNIEnv *env, jobject thiz) {
    int rc = 0;
    struct hook_java_args args;

    memset(&args, 0, sizeof(args));
    args.clz = "org/stagex/hooksdk/TestActivity";
    args.mtd = "testJavaHook_add";
    args.sig = "(II)I";
    args.func = testJavaHook_add;
    rc |= hook_java(env, &args);
    args.mtd = "testJavaHook_sub";
    args.sig = "(II)I";
    args.func = testJavaHook_sub;
    rc |= hook_java(env, &args);
    return rc;
}

