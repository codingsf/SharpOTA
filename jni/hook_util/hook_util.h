
#ifndef _HOOK_UTIL_H_
#define _HOOK_UTIL_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int get_module_base(pid_t, const char *);
int get_remote_addr(pid_t, const char *, int);
void dump_maps(pid_t);

#ifdef __cplusplus
}
#endif

#endif

