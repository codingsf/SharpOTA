
/*
 important: this must match struct hook_args in shellcode.h 
*/
#define OFFSET_dlopen       0
#define OFFSET_dlsym        4
#define OFFSET_dlclose      8
#define OFFSET_sopath       12
#define OFFSET_sohandle     272
#define OFFSET_soentry      276
#define OFFSET_regs         308
#define OFFSET_cpsr         372
#define OFFSET_argc         380
#define OFFSET_argv         384 

.ARM

.global inject_bgn
.global inject_end

inject_bgn:
    mov r5, r0
    add r0, r5, #OFFSET_sopath
    mov r1, #0
    ldr r4, [r5, #OFFSET_dlopen]
    blx r4
    add r4, r5, #OFFSET_sohandle
    str r0, [r4]
    cmp r0, #0
    beq fail_dlopen
    add r1, r5, #OFFSET_soentry
    ldr r4, [r5, #OFFSET_dlsym]
    blx r4
    mov r4, r0
    cmp r0, #0
    beq fail_dlsym
    ldr r0, [r5, #OFFSET_argc]
    add r1, r5, #OFFSET_argv
    blx r4
inject_done:
    ldr r0, [r5, #OFFSET_cpsr]
    msr cpsr, r0
    add sp, r5, #OFFSET_regs
    ldmfd sp, {r0-pc}
    
fail_dlsym:
    ldr r0, [r5, #OFFSET_sohandle]
    ldr r4, [r5, #OFFSET_dlclose]
    blx r4
fail_dlopen:
    b    inject_done

inject_end:

