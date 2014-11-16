
#include "log.h"
#include "hook_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int get_module_base(pid_t pid, const char *soname) {
    char fn[64];
    char line[512];
    char sn[260];
    FILE *fp;
    char *test;
    int rc, ret = 0;

    if (pid <= 0)
        snprintf(fn, sizeof(fn), "/proc/self/maps");
    else
        snprintf(fn, sizeof(fn), "/proc/%d/maps", pid);
    if (soname == NULL) {
        memset(sn, 0, sizeof(sn));
        rc = readlink("/proc/self/exe", sn, sizeof(sn));
        if (rc < 0)
            return ret;
    } else {
        strncpy(sn, soname, sizeof(sn));
    }
    fp = fopen(fn, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, sn)) {
                test = strtok(line, "-");
                if (test) {
                    ret = (int) strtoul(test, NULL, 16);
                    if (ret == 0x8000)
                        ret = 0;
                    break;
                }
            }
        }
        fclose(fp);
    }

    return ret;
}

int get_remote_addr(pid_t pid, const char *soname, int addr) {
    int la, ra;

    la = get_module_base(0, soname);
    ra = get_module_base(pid, soname);
    return addr + ra - la;
}

unsigned int elf_hash(const unsigned char *sym) {
    const unsigned char *tmp = (const unsigned char *) sym;
    unsigned h = 0, g;

    while (*tmp) {
        h = (h << 4) + *tmp++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }

    return h;
}

void dump_maps(pid_t pid) {
    char fn[260];
    FILE *fp;
    char line[512];

    if (pid <= 0)
        snprintf(fn, sizeof(fn), "/proc/self/maps");
    else
        snprintf(fn, sizeof(fn), "/proc/%d/maps", pid);
    fp = fopen(fn, "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            LOGV("%s", line);
        }
        fclose(fp);
    }
}

