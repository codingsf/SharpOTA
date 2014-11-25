
#include "log.h"

void __attribute__((constructor)) TestCtor() {
    LOGV("%s called", __func__);
}

int TestMain(int argc, char *argv[]) {
    int i;

    LOGV("argc = %d", argc);
    if (argc > 0) {
        for (i = 0; i < argc; i++) {
            LOGV("argv[%d] = %s", i, argv[i]);
        }
    }

    return 0;
}

