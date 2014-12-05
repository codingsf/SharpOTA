
#include "log.h"
#include "shellcode.h"
#include "hook_util.h"
#include <dlfcn.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include <asm/ptrace.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERESTARTSYS 512
#define ERESTARTNOINTR  513
#define ERESTARTNOHAND  514 /* restart if no handler.. */
#define ERESTART_RESTARTBLOCK 516 /* restart by calling sys_restart_syscall */

// see payload.S
extern int inject_bgn;
extern int inject_end;

static const char *linker_path = "/system/bin/linker";
static const char *libc_path = "/system/lib/libc.so";

static void usage(const char *self) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "%s <pid> <sopath> <soentry> ...\n", self);
    exit(1);
}

static int ptrace_readdata(pid_t pid, int src, char *dst, size_t size) {
    long *ptr, val;
    size_t i, r;

    ptr = (long *) dst;
    for (i = 0; i < size / sizeof(long); i++) {
        val = ptrace(PTRACE_PEEKDATA, pid, (void *) src, NULL);
        if (val == -1 && errno)
            return -1;
        *ptr++ = val;
        src += sizeof(long);
    }
    r = size % sizeof(long);
    if (r) {
        val = ptrace(PTRACE_PEEKDATA, pid, (void *) src, 0);
        if (val == -1 && errno)
            return -1;
        memcpy(ptr, &val, r);
    }

    return 0;
}

static int ptrace_writedata(pid_t pid, int dst, const char *src, size_t size) {
    int rc;
    long *ptr, val;
    size_t i, r;

    ptr = (long *) src;
    for (i = 0; i < size / sizeof(long); i++) {
        val = *ptr++;
        rc = ptrace(PTRACE_POKEDATA, pid, (void *) dst, (void *) val);
        if (rc < 0)
            return -1;
        dst += sizeof(long);
    }
    r = size % sizeof(long);
    if (r) {
        val = ptrace(PTRACE_PEEKDATA, pid, (void *) dst, 0);
        if (val == -1 && errno)
            return -1;
        memcpy(ptr, &val, r);
        rc = ptrace(PTRACE_POKEDATA, pid, (void *) dst, (void *) val);
        if (rc < 0)
            return -1;
    }

    return 0;
}

static int ptrace_call(int pid, int addr, int argc, int *argv, struct pt_regs *reg) {
    int rc, i;

    // R0-R3
    for (i = 0; i < 4 && i < argc; i++) {
        reg->uregs[i] = argv[i];
    }
    // stack
    if (i < argc) {
        reg->ARM_sp -= (argc - i) * sizeof(int);
        rc = ptrace_writedata(pid, reg->ARM_sp, (const char *) &argv[i], (argc - i) * sizeof(int));
        if (rc < 0)
            return -1;
    }
    //
    reg->ARM_pc = addr;
    if (reg->ARM_pc & 1) {
        // target address is THUMB mode, set THUMB flag
        reg->ARM_pc &= ~1;
        reg->ARM_cpsr |= PSR_T_BIT;
    } else {
        // target address is ARM mode, clear THUMB flag
        reg->ARM_cpsr &= ~PSR_T_BIT;
    }
    // make sure break after call
    reg->ARM_lr = 0;
    rc = ptrace(PTRACE_SETREGS, pid, NULL, reg);
    if (rc < 0)
        return -1;
    rc = ptrace(PTRACE_CONT, pid, NULL, 0);
    if (rc < 0)
        return -1;
    rc = waitpid(pid, NULL, WUNTRACED);
    if (rc < 0)
        return -1;

    return 0;
}

static int inject(pid_t pid, const char *sopath, const char *soentry, int argc, char *argv[]) {
    int rc, i, off;
    int sopath_len, soentry_len;
    size_t code_size, args_size;
    int mmap_ptr, dlopen_ptr, dlsym_ptr, dlclose_ptr;
    struct pt_regs old_regs, tmp_regs;
    int mmap_args[6];
    int mapped;
    char payload[0x4000];
    struct hook_args *ha;

    // basic check
    if ((sopath == NULL) || (soentry == NULL) ||
        (argc < 0) || (argc > 0 && argv == NULL)) {
        LOGE("[-] invalid parameters\n");
        return -1;
    }
    sopath_len = strlen(sopath);
    soentry_len = strlen(soentry);
    if (!sopath_len || !soentry_len ||
        (sopath_len > 259) || (soentry_len > 31)) {
        LOGE("[-] invalid parameters\n");
        return -1;
    }
    // calculate various size
    code_size = (int) &inject_end - (int) &inject_bgn;
    args_size = sizeof(struct hook_args);
    args_size += argc * sizeof(int);
    for (i = 0; i < argc; i++) {
        if (argv[i] == NULL) {
            LOGE("[-] invalid parameters\n");
            return -1;
        }
        args_size += strlen(argv[i]);
        args_size += 1;
    }
    if (args_size & 3)
        args_size += 4 - (args_size & 3);
    if (code_size + args_size > 0x4000 - 4) {
        LOGE("[-] invalid parameters\n");
        return -1;
    }

    // resolve symbols
    mmap_ptr = get_remote_addr(pid, libc_path, (int) mmap);
    dlopen_ptr = get_remote_addr(pid, linker_path, (int) dlopen);
    dlsym_ptr = get_remote_addr(pid, linker_path, (int) dlsym);
    dlclose_ptr = get_remote_addr(pid, linker_path, (int) dlclose);
    if (!mmap_ptr || !dlopen_ptr ||
        !dlsym_ptr || !dlclose_ptr) {
        LOGE("[-] resolve symbols\n");
        return -2;
    }
    // LOGD("[+] remote mmap = %x\n", mmap_ptr);
    // LOGD("[+] remote dlopen = %x\n", dlopen_ptr);
    // LOGD("[+] remote dlsym = %x\n", dlsym_ptr);
    // LOGD("[+] remote dlclose = %x\n", dlclose_ptr);

    // attach
    rc = ptrace(PTRACE_ATTACH, pid, NULL, 0);
    if (rc < 0) {
        LOGE("[-] ptrace ATTACH\n");
        return -3;
    }
    rc = waitpid(pid, NULL, WUNTRACED);
    if (rc < 0) {
        LOGE("[-] ptrace ATTACH: waitpid: %s\n", strerror(errno));
        return -4;
    }
    // getregs
    rc = ptrace(PTRACE_GETREGS, pid, NULL, &old_regs);
    if (rc < 0) {
        LOGE("[-] ptrace GETREGS\n");
        return -5;
    }
    // invoke mmap
    memcpy(&tmp_regs, &old_regs, sizeof(tmp_regs));
    mmap_args[0] = 0;
    mmap_args[1] = 0x4000;
    mmap_args[2] = PROT_READ | PROT_WRITE | PROT_EXEC;
    mmap_args[3] = MAP_ANONYMOUS | MAP_PRIVATE;
    mmap_args[4] = 0;
    mmap_args[5] = 0;
    rc = ptrace_call(pid, mmap_ptr, 6, mmap_args, &tmp_regs);
    if (rc < 0) {
        LOGE("[-] remote mmap\n");
        return -6;
    }
    // construct parameters for dl*
    rc = ptrace(PTRACE_GETREGS, pid, NULL, &tmp_regs);
    if (rc < 0) {
        LOGE("[-] ptrace GETREGS\n");
        return -7;
    }
    mapped = tmp_regs.ARM_r0;
    if (mapped == -1) {
        LOGE("[-] remote mmap\n");
        return -8;
    }
    /*
    remote payload layout
        ------        <- CODE END
          ^     HIGH
          |
          |           <- CODE BGN(ARGS END)
          |
          |
          |           <- ARGS BGN(SP)
          |
        -----   
    */
    // zero out
    memset(payload, 0, sizeof(payload));
    // copy code
    memcpy(payload + sizeof(payload) - code_size, &inject_bgn, code_size);
    // copy args
    ha = (struct hook_args *) (payload + sizeof(payload) - code_size - args_size);
    ha->dlopen = dlopen_ptr;
    ha->dlsym = dlsym_ptr;
    ha->dlclose = dlclose_ptr;
    strcpy(ha->sopath, sopath);
    strcpy(ha->soentry, soentry);
    if (old_regs.ARM_r0 == -ERESTARTNOHAND ||
        old_regs.ARM_r0 == -ERESTARTSYS ||
        old_regs.ARM_r0 == -ERESTARTNOINTR ||
        old_regs.ARM_r0 == -ERESTART_RESTARTBLOCK ) {
        old_regs.ARM_r0 = old_regs.ARM_ORIG_r0;
        old_regs.ARM_r7 = __NR_restart_syscall;
        if (old_regs.ARM_cpsr & PSR_T_BIT) {
            old_regs.ARM_pc -= 2;
        } else {
            old_regs.ARM_pc -=4;
        }
    }
    memcpy(&ha->regs, &old_regs, sizeof(ha->regs));
    ha->argc = argc;
    off = sizeof(payload) - code_size - args_size + sizeof(struct hook_args) + argc * sizeof(int);
    for (i = 0; i < argc; i++) {
        ha->argv[i] = mapped + off;
        strcpy(payload + off, argv[i]);
        off += strlen(argv[i]);
        off += 1;
    }
    // 
    rc = ptrace_writedata(pid, mapped, payload, sizeof(payload));
    if (rc < 0) {
        LOGE("[-] remote payload write\n");
        return -9;
    }
    // invoke payload
    memcpy(&tmp_regs, &old_regs, sizeof(tmp_regs));
    tmp_regs.ARM_r0 = mapped + sizeof(payload) - code_size - args_size;
    tmp_regs.ARM_sp = tmp_regs.ARM_r0 & ~3;
    tmp_regs.ARM_pc = mapped + sizeof(payload) - code_size;
    rc = ptrace(PTRACE_SETREGS, pid, NULL, &tmp_regs);
    if (rc < 0) {
        LOGE("[-] ptrace SETREGS\n");
        return -10;
    }
    // detach
    rc = ptrace(PTRACE_DETACH, pid, NULL, 0);
    if (rc < 0) {
        LOGE("[-] ptrace DETACH\n");
        return -12;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 4)
        usage(argv[0]);
    int i, _argc = argc - 4;
    char *_argv[_argc + 1];

    for (i = 4; i < argc; i++)
        _argv[i - 4] = argv[i];
    _argv[_argc] = NULL;
    return inject(strtol(argv[1], NULL, 10), argv[2], argv[3], _argc, _argv);
}

