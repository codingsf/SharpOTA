
#ifndef _LOG_H_
#define _LOG_H_

#include <android/log.h>
#include <stdio.h>

#define DEBUG 1

#define LOGX(X, ...) do { \
        if (DEBUG) { \
            fprintf(stderr, __VA_ARGS__); fflush(stderr); \
            __android_log_print(X, TAG, __VA_ARGS__); \
        } \
    } while (0)
#define LOGD(...) LOGX(ANDROID_LOG_DEBUG, __VA_ARGS__)
#define LOGW(...) LOGX(ANDROID_LOG_WARN, __VA_ARGS__)
#define LOGE(...) LOGX(ANDROID_LOG_ERROR, __VA_ARGS__)
#define LOGV(...) LOGX(ANDROID_LOG_VERBOSE, __VA_ARGS__)

#endif

