/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_LOG_H
#define HCS_COMPILER_LOG_H

#include <stdio.h>
#include "hcs_types.h"

#ifdef DEBUG
#define HCS_DEBUG(fmt, args...) fprintf(stdout, fmt"\n", ##args)
#else
#define HCS_DEBUG(fmt, args...) do {} while (0)
#endif

#ifdef OS_LINUX
#define ERROR_COLOR_PREFIX "\033[31m"
#define ERROR_COLOR_END    "\033[0m"
#else
#define ERROR_COLOR_PREFIX
#define ERROR_COLOR_END
#endif

#define HCS_LOG_PRINT(fmt, args...)  fprintf(stdout, ERROR_COLOR_PREFIX fmt ERROR_COLOR_END"\n", ##args)

#define HCS_PRINT(fmt, args...)  fprintf(stdout, fmt, ##args)

#define HCS_INFO(fmt, args...)  fprintf(stdout, fmt"\n", ##args)

#define HCS_ERROR(fmt, args...) HCS_LOG_PRINT("Error: " fmt, ##args)

#define HCS_WARNING(fmt, args...) HCS_LOG_PRINT("Warning: " fmt, ##args)

#define HCS_OBJECT_PR(prefix, object, fmt, args...)                         \
    do {                                                                    \
        HCS_LOG_PRINT(prefix": %s:%u\n\t" fmt"\n",                         \
                    object ? ((ParserObjectBase*)object)->src : "unknown",  \
                    object ? ((ParserObjectBase*)object)->lineno : 0,       \
                    ##args);                                                \
    } while (0)

#define HCS_OBJECT_ERROR(object, fmt, args...) HCS_OBJECT_PR("Error", object, fmt, ##args)

#define HCS_OBJECT_WARNING(object, fmt, args...) HCS_OBJECT_PR("Warning", object, fmt, ##args)

#define PRINTF_CHECK_AND_RETURN(printRes)                                   \
    do {                                                                    \
        if ((printRes) < 0) {                                               \
            HCS_ERROR("Error:%s(%d), sprintf_s failed",                     \
                __FUNCTION__, __LINE__);                                    \
            return EOUTPUT;                                                 \
        }                                                                   \
    } while (0)

#define OUTPUT_CHECK_AND_RETURN(printRes)                                   \
    do {                                                                    \
        if ((printRes) < 0) {                                               \
            HCS_ERROR("Error:%s(%d), output write failed",                  \
                __FUNCTION__, __LINE__);                                    \
            return EOUTPUT;                                                 \
        }                                                                   \
    } while (0)


#endif // HCS_COMPILER_LOG_H
