#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OK 0
#define ERR -1

#define P_ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#define P_MIN(a, b) ((a) < (b) ? (a) : (b))
#define P_MAX(a, b) ((a) > (b) ? (a) : (b))

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({ const typeof( ((type *)0)->member) *__mptr = (ptr); \
(type *)( (char *)__mptr - offsetof(type,member) ); })
#endif

#define LOG_D(fmt, ...) printf("D %s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#define LOG_I(fmt, ...) printf("I %s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#define LOG_W(fmt, ...) printf("W %s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#define LOG_E(fmt, ...) printf("E %s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#define LOG_ENTER() printf("D %s:%d %s enter\r\n", __FILE__, __LINE__, __func__)
#define LOG_LEAVE() printf("D %s:%d %s leave\r\n", __FILE__, __LINE__, __func__)
#define HDF_LOGD(fmt, ...) printf("D " fmt "\r\n", ##__VA_ARGS__)
#define HDF_LOGI(fmt, ...) printf("I " fmt "\r\n", ##__VA_ARGS__)
#define HDF_LOGW(fmt, ...) printf("W " fmt "\r\n", ##__VA_ARGS__)
#define HDF_LOGE(fmt, ...) printf("E " fmt "\r\n", ##__VA_ARGS__)

#define RETURN_VAL_IF_FAIL(cond, val)                                      \
    do {                                                                   \
        if (!(cond)) {                                                     \
            printf("E %s:%d '%s' failed.\r\n", __FILE__, __LINE__, #cond); \
            return val;                                                    \
        }                                                                  \
    } while (0)

#define RETURN_IF_FAIL(cond)                                               \
    do {                                                                   \
        if (!(cond)) {                                                     \
            printf("E %s:%d '%s' failed.\r\n", __FILE__, __LINE__, #cond); \
            return;                                                        \
        }                                                                  \
    } while (0)

#define DBG_ASSERT(cond)                                                          \
    do {                                                                          \
        if (!(cond)) {                                                            \
            printf("E %s:%d '%s' assert failed.\r\n", __FILE__, __LINE__, #cond); \
            while (1)                                                             \
                ;                                                                 \
        }                                                                         \
    } while (0)

typedef void *DevHandle;

#ifdef __cplusplus
}
#endif

#endif
