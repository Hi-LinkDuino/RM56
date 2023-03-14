/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_FOUNDATION_LOG_H
#define HISTREAMER_FOUNDATION_LOG_H

#include <cinttypes>
#include <string>

// If file name and line number is need, #define HST_DEBUG at the beginning of the cpp file.
#define HST_DEBUG
#ifdef HST_DEBUG
inline std::string HstGetFileName(std::string file)
{
    if (file == "") {
        return "Unknown File";
    }
    return file.substr(file.find_last_of("/\\") + 1);
}
#endif

#ifdef MEDIA_OHOS
#ifndef LOG_DOMAIN
#define LOG_DOMAIN 0xD002B00
#endif
#ifndef LOG_TAG
#define LOG_TAG "HiStreamer"
#endif
#include "hilog/log.h"
#else
#include "log_adapter.h"
#endif

#ifndef HST_LOG_TAG
#define HST_LOG_TAG "NULL"
#endif

#if defined(MEDIA_OHOS)
#define PUBLIC_LOG "%{public}"
#else
#define PUBLIC_LOG "%"
#endif

#define PUBLIC_LOG_C PUBLIC_LOG "c"
#define PUBLIC_LOG_S PUBLIC_LOG "s"
#define PUBLIC_LOG_D8 PUBLIC_LOG PRId8
#define PUBLIC_LOG_D16 PUBLIC_LOG PRId16
#define PUBLIC_LOG_D32 PUBLIC_LOG PRId32
#define PUBLIC_LOG_D64 PUBLIC_LOG PRId64
#define PUBLIC_LOG_U8 PUBLIC_LOG PRIu8
#define PUBLIC_LOG_U16 PUBLIC_LOG PRIu16
#define PUBLIC_LOG_U32 PUBLIC_LOG PRIu32
#define PUBLIC_LOG_U64 PUBLIC_LOG PRIu64
#define PUBLIC_LOG_F PUBLIC_LOG "f"
#define PUBLIC_LOG_P PUBLIC_LOG "p"
#define PUBLIC_LOG_ZU PUBLIC_LOG "zu"

#ifdef MEDIA_OHOS
#ifndef HST_DEBUG
#define HST_DECORATOR_HILOG(op, fmt, args...)                                               \
    do {                                                                                    \
        op(LOG_CORE, PUBLIC_LOG_S ":" fmt, HST_LOG_TAG, ##args);                            \
    } while (0)
#else
#define HST_DECORATOR_HILOG(op, fmt, args...)                                                                          \
    do {                                                                                                               \
        std::string file(__FILE__);                                                                                    \
        std::string bareFile = HstGetFileName(file);                                                                   \
        op(LOG_CORE, "(" PUBLIC_LOG_S ", " PUBLIC_LOG_D32 "): " fmt, bareFile.c_str(), __LINE__, ##args);              \
    } while (0)
#endif

#define MEDIA_LOG_D(fmt, ...) HST_DECORATOR_HILOG(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_I(fmt, ...) HST_DECORATOR_HILOG(HILOG_INFO, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_W(fmt, ...) HST_DECORATOR_HILOG(HILOG_WARN, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_E(fmt, ...) HST_DECORATOR_HILOG(HILOG_ERROR, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_F(fmt, ...) HST_DECORATOR_HILOG(HILOG_FATAL, fmt, ##__VA_ARGS__)
#endif


// Control the MEDIA_LOG_D.
// If MEDIA_LOG_D is needed, #define MEDIA_LOG_DEBUG 1 at the beginning of the cpp file.
#ifndef MEDIA_LOG_DEBUG
#define MEDIA_LOG_DEBUG 0
#endif

#if !MEDIA_LOG_DEBUG
#undef MEDIA_LOG_D
#define MEDIA_LOG_D(msg, ...) ((void)0)
#endif

// Control the debug detail logs MEDIA_LOG_DD.
// If MEDIA_LOG_DD is needed, #define MEDIA_LOG_DEBUG_DETAIL 1 at the beginning of the cpp file.
#ifndef MEDIA_LOG_DEBUG_DETAIL
#define MEDIA_LOG_DEBUG_DETAIL 0
#endif

#if !MEDIA_LOG_DEBUG_DETAIL
#undef MEDIA_LOG_DD
#define MEDIA_LOG_DD(msg, ...) ((void)0)
#else
#undef MEDIA_LOG_DD
#define MEDIA_LOG_DD MEDIA_LOG_D
#endif

#ifndef NOK_RETURN
#define NOK_RETURN(exec)                                                                                               \
    do {                                                                                                               \
        Status returnValue = (exec);                                                                                   \
        if (returnValue != Status::OK) {                                                                               \
            MEDIA_LOG_E("NOK_RETURN on Status(" PUBLIC_LOG_D32 ").", returnValue);                                     \
            return returnValue;                                                                                        \
        }                                                                                                              \
    } while (0)
#endif

#ifndef NOK_LOG
#define NOK_LOG(exec)                                                                                                  \
    do {                                                                                                               \
        Status returnValue = (exec);                                                                                   \
        if (returnValue != Status::OK) {                                                                               \
            MEDIA_LOG_E("NOK_LOG on Status(" PUBLIC_LOG_D32 ").", returnValue);                                        \
        }                                                                                                              \
    } while (0)
#endif

// If exec not return zero, then record the error code, especially when call system C function.
#ifndef NZERO_LOG
#define NZERO_LOG(exec)                                                                                                \
    do {                                                                                                               \
        int returnValue = (exec);                                                                                      \
        if (returnValue != 0) {                                                                                        \
            MEDIA_LOG_E("NZERO_LOG when call (" #exec "), return " PUBLIC_LOG_D32, returnValue);                       \
        }                                                                                                              \
    } while (0)
#endif

#ifndef NZERO_RETURN
#define NZERO_RETURN(exec)                                                                                             \
    do {                                                                                                               \
        int returnValue = (exec);                                                                                      \
        if (returnValue != 0) {                                                                                        \
            MEDIA_LOG_E("NZERO_LOG when call (" #exec "), return " PUBLIC_LOG_D32, returnValue);                       \
            return returnValue;                                                                                        \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_RETURN
#define FALSE_RETURN(exec)                                                                                             \
    do {                                                                                                               \
        bool returnValue = (exec);                                                                                     \
        if (!returnValue) {                                                                                            \
            MEDIA_LOG_E("FALSE_RETURN " #exec);                                                                        \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_RETURN_W
#define FALSE_RETURN_W(exec)                                                                                           \
    do {                                                                                                               \
        bool returnValue = (exec);                                                                                     \
        if (!returnValue) {                                                                                            \
            MEDIA_LOG_W("FALSE_RETURN " #exec);                                                                        \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_RETURN_V
#define FALSE_RETURN_V(exec, ret)                                                                                      \
    do {                                                                                                               \
        bool returnValue = (exec);                                                                                     \
        if (!returnValue) {                                                                                            \
            MEDIA_LOG_E("FALSE_RETURN_V " #exec);                                                                      \
            return ret;                                                                                                \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_RETURN_V_W
#define FALSE_RETURN_V_W(exec, ret)                                                                                    \
    do {                                                                                                               \
        bool returnValue = (exec);                                                                                     \
        if (!returnValue) {                                                                                            \
            MEDIA_LOG_W("FALSE_RETURN_V_W " #exec);                                                                    \
            return ret;                                                                                                \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_RETURN_MSG
#define FALSE_RETURN_MSG(exec, fmt, args...)                                                                           \
    do {                                                                                                               \
        bool returnValue = (exec);                                                                                     \
        if (!returnValue) {                                                                                            \
            MEDIA_LOG_E(fmt, ##args);                                                                                  \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_RETURN_V_MSG_IMPL
#define FALSE_RETURN_V_MSG_IMPL(loglevel, exec, ret, fmt, args...)                                                     \
    do {                                                                                                               \
        bool returnValue = (exec);                                                                                     \
        if (!returnValue) {                                                                                            \
            loglevel(fmt, ##args);                                                                                     \
            return ret;                                                                                                \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_RETURN_V_MSG
#define FALSE_RETURN_V_MSG(exec, ret, fmt, args...) FALSE_RETURN_V_MSG_IMPL(MEDIA_LOG_E, exec, ret, fmt, ##args)
#endif

#ifndef FALSE_RETURN_V_MSG_W
#define FALSE_RETURN_V_MSG_W(exec, ret, fmt, args...) FALSE_RETURN_V_MSG_IMPL(MEDIA_LOG_W, exec, ret, fmt, ##args)
#endif

#ifndef FALSE_RETURN_V_MSG_E
#define FALSE_RETURN_V_MSG_E(exec, ret, fmt, args...) FALSE_RETURN_V_MSG_IMPL(MEDIA_LOG_E, exec, ret, fmt, ##args)
#endif

#ifndef FALSE_LOG
#define FALSE_LOG(exec)                                                                                                \
    do {                                                                                                               \
        bool returnValue = (exec);                                                                                     \
        if (!returnValue) {                                                                                            \
            MEDIA_LOG_E("FALSE_LOG: " #exec);                                                                          \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_LOG_MSG_IMPL
#define FALSE_LOG_MSG_IMPL(loglevel, exec, fmt, args...)                                                               \
    do {                                                                                                               \
        bool returnValue = (exec);                                                                                     \
        if (!returnValue) {                                                                                            \
            loglevel(fmt, ##args);                                                                                     \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FALSE_LOG_MSG
#define FALSE_LOG_MSG(exec, fmt, args...) FALSE_LOG_MSG_IMPL(MEDIA_LOG_E, exec, fmt, ##args)
#endif

#ifndef FALSE_LOG_MSG_W
#define FALSE_LOG_MSG_W(exec, fmt, args...) FALSE_LOG_MSG_IMPL(MEDIA_LOG_W, exec, fmt, ##args)
#endif

#endif // HISTREAMER_FOUNDATION_LOG_H