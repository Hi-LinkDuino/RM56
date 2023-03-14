/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HILOG_COMMON_H
#define HILOG_COMMON_H

#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

#define SOCKET_FILE_DIR "/dev/unix/socket/"
#define INPUT_SOCKET_NAME "hilogInput"
#define INPUT_SOCKET SOCKET_FILE_DIR INPUT_SOCKET_NAME
#define CONTROL_SOCKET_NAME "hilogControl"
#define CONTROL_SOCKET SOCKET_FILE_DIR CONTROL_SOCKET_NAME
#define HILOG_FILE_DIR "/data/log/hilog/"
#define SYSTEM_BG_STUNE "/dev/stune/system-background/cgroup.procs"
#define SYSTEM_BG_CPUSET "/dev/cpuset/system-background/cgroup.procs"
#define SYSTEM_BG_BLKIO "/dev/blkio/system-background/cgroup.procs"

#define SENDIDN 0    // hilogd: reached end of log;  hilogtool: exit log reading
#define SENDIDA 1    // hilogd & hilogtool: normal log reading
#define SENDIDS 2    // hilogd: notify for new data; hilogtool: block and wait for new data
#define MAX_LOG_LEN 1024  /* maximum length of a log, include '\0' */
#define MAX_TAG_LEN 32  /* log tag size, include '\0' */
#define MAX_DOMAINS 5
#define MAX_TAGS 10
#define MAX_PIDS 5
#define RET_SUCCESS 0
#define RET_FAIL (-1)
#define IS_ONE(number, n) ((number>>n)&0x01)
#define ONE_KB (1UL<<10)
#define ONE_MB (1UL<<20)
#define ONE_GB (1UL<<30)
#define ONE_TB (1ULL<<40)
const long long NSEC = 1000000000LL;
const long long US = 1000000LL;
const long long NS2US = 1000LL;
const long long NS2MS = 1000000LL;
const size_t MIN_BUFFER_SIZE = 65536;
const size_t MAX_BUFFER_SIZE = 1UL<<30;
const uint32_t MAX_PERSISTER_BUFFER_SIZE = 64 * 1024;
constexpr int MIN_LOG_FILE_NUM = 2;
constexpr int MAX_LOG_FILE_NUM = 1000;
const int MSG_MAX_LEN = 2048;
constexpr uint64_t PRIME = 0x100000001B3ull;
constexpr uint64_t BASIS = 0xCBF29CE484222325ull;
/*
 * header of log message from libhilog to hilogd
 */
using HilogMsg = struct __attribute__((__packed__)) {
    uint16_t len;
    uint16_t version : 3;
    uint16_t type : 4;  /* APP,CORE,INIT,SEC etc */
    uint16_t level : 3;
    uint16_t tag_len : 6; /* include '\0' */
    uint32_t tv_sec;
    uint32_t tv_nsec;
    uint32_t pid;
    uint32_t tid;
    uint32_t domain;
    char tag[]; /* shall be end with '\0' */
};

using HilogShowFormatBuffer = struct {
    uint16_t length;
    uint16_t level;
    uint16_t type;
    uint16_t tag_len;
    uint32_t pid;
    uint32_t tid;
    uint32_t domain;
    uint32_t tv_sec;
    uint32_t tv_nsec;
    const char* data;
};

template <typename T>
using OptRef = std::optional<std::reference_wrapper<T>>;

template <typename T>
using OptCRef = std::optional<std::reference_wrapper<const T>>;

#define CONTENT_LEN(pMsg) (pMsg->len - sizeof(HilogMsg) - pMsg->tag_len) /* include '\0' */
#define CONTENT_PTR(pMsg) (pMsg->tag + pMsg->tag_len)

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#if defined(__GNUC__) && (__GNUC__ >= 4)
    #define HILOG_PUBLIC_API __attribute__((visibility ("default")))
    #define HILOG_LOCAL_API __attribute__((visibility("hidden")))
#else
    #define HILOG_PUBLIC_API
    #define HILOG_LOCAL_API
#endif

/*
 * ********************************************
 *  Error codes list
 *  Error codes _values_ are pinned down.
 * ********************************************
*/
typedef enum {
    ERR_LOG_LEVEL_INVALID  = -2,
    ERR_LOG_TYPE_INVALID = -3,
    ERR_QUERY_LEVEL_INVALID = -4,
    ERR_QUERY_DOMAIN_INVALID = -5,
    ERR_QUERY_TAG_INVALID = -6,
    ERR_QUERY_PID_INVALID = -7,
    ERR_QUERY_TYPE_INVALID = -8,
    ERR_BUFF_SIZE_EXP = -9,
    ERR_LOG_CONTENT_NULL = -10,
    ERR_LOG_PERSIST_FILE_SIZE_INVALID = -11,
    ERR_LOG_PERSIST_FILE_NAME_INVALID = -12,
    ERR_LOG_PERSIST_COMPRESS_BUFFER_EXP = -13,
    ERR_LOG_PERSIST_DIR_OPEN_FAIL = -14,
    ERR_LOG_PERSIST_COMPRESS_INIT_FAIL = -15,
    ERR_LOG_PERSIST_FILE_OPEN_FAIL = -16,
    ERR_LOG_PERSIST_MMAP_FAIL = -17,
    ERR_LOG_PERSIST_JOBID_FAIL = -18,
    ERR_DOMAIN_INVALID = -19,
    ERR_MEM_ALLOC_FAIL = -20,
    ERR_MSG_LEN_INVALID = -21,
    ERR_PRIVATE_SWITCH_VALUE_INVALID = -22,
    ERR_COMMAND_NOT_FOUND = -23,
    ERR_FORMAT_INVALID = -24,
    ERR_LOG_PERSIST_FILE_PATH_INVALID = -25,
    ERR_PERSIST_INFO_OPEN_FAIL = -26,
    ERR_FLOWCONTROL_CONF_OPEN_FAIL = -27,
    ERR_LOG_PERSIST_JOBID_INVALID = -28,
    ERR_FLOWCTRL_SWITCH_VALUE_INVALID = -29,
    ERR_BUFF_SIZE_INVALID = -30,
    ERR_COMMAND_INVALID = -31,
    ERR_LOG_PERSIST_TASK_FAIL = -32,
    ERR_KMSG_SWITCH_VALUE_INVALID = -33,
    ERR_LOG_FILE_NUM_INVALID = -34,
} ErrorCode;

inline void HilogPrintError(int errnum)
{
    constexpr int bufSize = 1024;
    char buf[bufSize] = { 0 };
    strerror_r(errnum, buf, bufSize);
    std::cerr << "Errno: " << errnum << ", " << buf << "\n";
}

#endif /* HILOG_COMMON_H */
