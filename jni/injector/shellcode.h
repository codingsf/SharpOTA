
#ifndef _SHELLCODE_H_
#define _SHELLCODE_H_

#include <asm/ptrace.h>

struct hook_args {
    int dlopen;
    int dlsym;
    int dlclose;
    char sopath[260];
    int sohandle;
    char soentry[32];
    struct pt_regs regs;
    int argc;
    int argv[0];
};

#endif

