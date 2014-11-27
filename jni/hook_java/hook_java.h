
#ifndef _HOOK_JAVA_H_
#define _HOOK_JAVA_H_

#ifdef __cplusplus
extern "C" {
#endif

struct hook_java_args {
    const char *clz;        // 
    const char *mtd;        //
    const char *sig;        //
    void *old;              // old struct Method
    void *prev;             //
    void *post;             //
    void *func;             //
};

void *get_java_vm();
void *get_jni_env();
int hook_java(void *, struct hook_java_args *);

#ifdef __cplusplus
}
#endif

#endif

