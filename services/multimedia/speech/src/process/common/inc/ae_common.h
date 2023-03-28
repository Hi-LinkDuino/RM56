/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef AE_COMMON_H
#define AE_COMMON_H

//#define AE_PROFILE_ENABLE

#include <stdio.h>

#ifndef VQE_SIMULATE
#include "hal_trace.h"
#include "hal_timer.h"
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

/*
 * You can modify this (for example with "#define LOG_NDEBUG 0"
 * at the top of your source file) to change that behavior.
 */
#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

/*
 * This is the local tag used for the following simplified
 * logging macros.  You can change this preprocessor definition
 * before using the other macros to change the tag.
 */
#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

#ifdef VQE_SIMULATE
#ifdef __cplusplus
extern "C" {
#endif
void __ae_log_print(int num, const char *tag, const char *fmt, ...);
#ifdef __cplusplus
}
#endif
#else
#ifdef AUDIO_DEBUG
#define __ae_log_print(tag, fmt, ...)
#elif defined __TWS__
#define __ae_log_print(tag, fmt, ...) TRACE(HAL_TRACE_LEVEL_0, tag, fmt, ##__VA_ARGS__)
#else
#define __ae_log_print(attr, tag, fmt, ...) TRACE(attr, fmt, ##__VA_ARGS__)
#endif
#endif

#if LOG_NDEBUG
#define AE_TRACE(...) ((void)0)
#else
#define AE_TRACE(num,fmt, ...) __ae_log_print(num+1,LOG_TAG, fmt, ##__VA_ARGS__)
#endif

#ifdef VQE_SIMULATE
#define AE_ASSERT(num,cond, fmt, ...) \
do { \
    if (!(cond)) { \
      __ae_log_print(num+1,LOG_TAG, fmt, ##__VA_ARGS__); \
        while(1); \
    } \
} while(0)
#else
#define AE_ASSERT(num,cond, fmt, ...) ASSERT(cond, fmt, ##__VA_ARGS__)
#endif

#ifdef AE_PROFILE_ENABLE
#define AE_PROFILE_ENTER() \
    uint32_t start_ticks = hal_fast_sys_timer_get();

#define AE_PROFILE_EXIT() \
    uint32_t end_ticks = hal_fast_sys_timer_get(); \
    AE_TRACE(2,"[%s] takes %d us", __FUNCTION__, FAST_TICKS_TO_US(end_ticks - start_ticks));
#else
#define AE_PROFILE_ENTER()
#define AE_PROFILE_EXIT()
#endif

#endif
