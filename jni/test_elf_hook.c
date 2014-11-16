
#include "log.h"
#include "hook_elf.h"
#include <sys/stat.h>

static int mystat(const char *f, struct stat *s) {
    LOGD("%s: %s\n", __func__, f);
    return 998;
}

int main(int argc, char *argv[]) {
    struct stat fs;

    // hook stat imported by myself, old stat is returned
    hook_elf(NULL, "stat", (int) mystat);

    return stat("/data/local/tmp", &fs);
}

