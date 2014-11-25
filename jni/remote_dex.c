
#include "log.h"
#include "dex.h"
#include "hook_elf.h"
#include <sys/stat.h>
#include <unistd.h>

#define HOOKSDK_DEX_FILE "/data/data/org.stagex.hooksdk/files/hooksdk.jar"
#define HOOKSDK_DEX_CLASS "org/stagex/hooksdk/remote/HookMain"
#define HOOKSDK_DEX_ENTRY "main"

/*
args:
/data/data/org.stagex.hooksdk/share/hooksdk.jar     <-- path to HookSDK remote JAR
/data/data/org.stagex.hooksdk/share                 <-- shared dex cache dir, world rwx, done by HookSDK Java
org/stagex/hooksdk/remote/HookMain                  <-- HookSDK remote JAR entry class
main                                                <-- HookSDK remote JAR entry method
a                                                   <-- args
b
c

*/

static int g_libc_stat = 0;

static int hsdk_stat_stub(const char *path, struct stat *st) {
    int rc;

    rc = ((int (*)(const char *, struct stat *)) g_libc_stat)(path, st);
    if (rc)
        return rc;
    if (!strcmp(path, HOOKSDK_DEX_FILE)) {
        st->st_uid = getuid();
        st->st_gid = getgid();
    }
    return rc;
}

int HookSDKMain(int argc, char *argv[]) {
    int rc, i;

    LOGD("argc = %d", argc);
    if (argc > 0) {
        for (i = 0; i < argc; i++) {
            LOGD("argv[%d] = %s", i, argv[i]);
        }
    }

    if (argc < 4)
        return -1;
    int _argc = argc - 4;
    char *_argv[_argc + 1];
    for (i = 4; i < argc; i++)
        _argv[i - 4] = argv[i];
    _argv[_argc] = NULL;

    // DexClassLoader implementation need DEX file has the same UID/GID as the caller process
    // this is how we have
    g_libc_stat = hook_elf("/system/lib/libdvm.so", "stat", (int) hsdk_stat_stub);

    // call our DEX
    rc = dex_invoke_main(
        argv[0],
        argv[1],
        argv[2],
        argv[3],
        _argc,
        _argv);

    // restore stat
    hook_elf("/system/lib/libdvm.so", "stat", g_libc_stat);

    return rc;
}

