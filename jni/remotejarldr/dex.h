
#ifndef _DEX_H_
#define _DEX_H_

#ifdef __cplusplus
extern "C" {
#endif

int dex_invoke_main(const char* dexPath, const char* dexOptDir, const char* className, const char* methodName, int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif

