/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef void *DevHandle;

uint32_t GetSysTime(void);

uint32_t SysTimeDiff(uint32_t start);

void DelayMs(uint32_t ms);

void DelayUs(uint32_t us);

/* platform log module */
#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARN 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

/* log low level interfaces */
#define LOG_LEVEL LOG_LEVEL_INFO

#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
#define LOG_D(fmt, ...) print_string("D %s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#define LOG_DA(n, a, l) print_array(n, a, l)
#else
#define LOG_D(fmt, ...)
#define LOG_DA(n, a, l)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_INFO)
#define LOG_I(fmt, ...) print_string("I %s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#define LOG_IA(n, a, l) print_array(n, a, l)
#else
#define LOG_I(fmt, ...)
#define LOG_IA(n, a, l)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_WARN)
#define LOG_W(fmt, ...) print_string("W %s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#define LOG_WA(n, a, l) print_array(n, a, l)
#else
#define LOG_W(fmt, ...)
#define LOG_WA(n, a, l)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
#define LOG_E(fmt, ...) print_string("E %s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#define LOG_EA(n, a, l) print_array(n, a, l)
#else
#define LOG_E(fmt, ...)
#define LOG_EA(n, a, l)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
#define LOG_ENTER() print_string("D %s:%d %s enter\r\n", __FILE__, __LINE__, __func__)
#define LOG_LEAVE() print_string("D %s:%d %s leave\r\n", __FILE__, __LINE__, __func__)
#else
#define LOG_ENTER()
#define LOG_LEAVE()
#endif

#define RETURN_VAL_IF_FAIL(cond, val)     \
    do {                                  \
        if (!(cond)) {                    \
            LOG_E("'%s' failed.", #cond); \
            return val;                   \
        }                                 \
    } while (0)

#define RETURN_IF_FAIL(cond)              \
    do {                                  \
        if (!(cond)) {                    \
            LOG_E("'%s' failed.", #cond); \
            return;                       \
        }                                 \
    } while (0)

#define DBG_ASSERT(cond)                                                                \
    do {                                                                                \
        if (!(cond)) {                                                                  \
            print_string("E/%s:%d '%s' assert failed.\r\n", __FILE__, __LINE__, #cond); \
            while (1)                                                                   \
                ;                                                                       \
        }                                                                               \
    } while (0)

void print_string(const char *fmt, ...);

void print_array(const char *name, const void *array, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
