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

#include "properties.h"

#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#include <ctime>
#include <cerrno>
#include <securec.h>

#include "log_time_stamp.h"
#include "hilog_trace.h"
#include "hilog_inner.h"
#include "hilog/log.h"
#include "hilog_common.h"
#include "hilog_input_socket_client.h"
#include "vsnprintf_s_p.h"

using namespace std;
static RegisterFunc g_registerFunc = nullptr;
static atomic_int g_hiLogGetIdCallCount = 0;
static const char P_LIMIT_TAG[] = "LOGLIMITP";
#ifdef DEBUG
static const int MAX_PATH_LEN = 1024;
#endif
static const int DEFAULT_QUOTA = 13050;
static const int LOG_FLOWCTRL_QUOTA_STR_LEN = 6;
int HiLogRegisterGetIdFun(RegisterFunc registerFunc)
{
    if (g_registerFunc != nullptr) {
        return -1;
    }
    g_registerFunc = registerFunc;
    return 0;
}

void HiLogUnregisterGetIdFun(RegisterFunc registerFunc)
{
    if (g_registerFunc != registerFunc) {
        return;
    }

    g_registerFunc = nullptr;
    while (atomic_load(&g_hiLogGetIdCallCount) != 0) {
        /* do nothing, just wait current callback return */
    }

    return;
}

static uint16_t GetFinalLevel(unsigned int domain, const std::string& tag)
{
    uint16_t domainLevel = GetDomainLevel(domain);
    uint16_t tagLevel = GetTagLevel(tag);
    uint16_t globalLevel = GetGlobalLevel();
    uint16_t maxLevel = LOG_LEVEL_MIN;
    maxLevel = (maxLevel < domainLevel) ? domainLevel : maxLevel;
    maxLevel = (maxLevel < tagLevel) ? tagLevel : maxLevel;
    maxLevel = (maxLevel < globalLevel) ? globalLevel : maxLevel;
    return maxLevel;
}

static uint32_t ParseProcessQuota()
{
    uint32_t proQuota = DEFAULT_QUOTA;
    std::string proName = GetProgName();
    static constexpr char flowCtrlQuotaFile[] = "/system/etc/hilog_flowcontrol_quota.conf";
    std::ifstream ifs(flowCtrlQuotaFile, std::ifstream::in);
    if (!ifs.is_open()) {
        return proQuota;
    }
    std::string line;
    while (!ifs.eof()) {
        getline(ifs, line);
        if (line.empty() || line.at(0) == '#') {
            continue;
        }
        std::string processName;
        std::string processQuotaValue;
        std::size_t processNameEnd = line.find_first_of(" ");
        if (processNameEnd == std::string::npos) {
            continue;
        }
        processName = line.substr(0, processNameEnd);
        if (++processNameEnd >= line.size()) {
            continue;
        }
        if (proName == processName) {
            int ret = 0;
            processQuotaValue = line.substr(processNameEnd, LOG_FLOWCTRL_QUOTA_STR_LEN);
            char quotaValue[LOG_FLOWCTRL_QUOTA_STR_LEN];
            ret = strcpy_s(quotaValue, LOG_FLOWCTRL_QUOTA_STR_LEN, processQuotaValue.c_str());
            if (ret != 0) {
                break;
            }
            ret = sscanf_s(quotaValue, "%d", &proQuota);
            if (ret <= 0) {
                cout << "invalid quota config" << endl;
            }
            break;
        }
    }
    ifs.close();
    return proQuota;
}

static int HiLogFlowCtrlProcess(int len, uint16_t logType, bool debug)
{
    if (logType == LOG_APP || !IsProcessSwitchOn() || debug) {
        return 0;
    }
    static uint32_t processQuota = DEFAULT_QUOTA;
    static atomic_int gSumLen = 0;
    static atomic_int gDropped = 0;
    LogTimeStamp startTime(0, 0);
    static atomic<LogTimeStamp> gStartTime(startTime);
    static std::atomic_flag isFirstFlag = ATOMIC_FLAG_INIT;
    if (!isFirstFlag.test_and_set()) {
        processQuota = ParseProcessQuota();
    }
    LogTimeStamp tsStart = atomic_load(&gStartTime);
    LogTimeStamp tsNow(CLOCK_MONOTONIC);
    /* in statistic period(1 second) */
    if ((tsNow -= tsStart) <= LogTimeStamp(1)) {
        uint32_t sumLen = (uint32_t)atomic_load(&gSumLen);
        if (sumLen > processQuota) { /* over quota, -1 means don't print */
            atomic_fetch_add_explicit(&gDropped, 1, memory_order_relaxed);
            return -1;
        } else { /* under quota, 0 means do print */
            atomic_fetch_add_explicit(&gSumLen, len, memory_order_relaxed);
            return 0;
        }
    } else  { /* new statistic period, return how many lines were dropped */
        int dropped = atomic_exchange_explicit(&gDropped, 0, memory_order_relaxed);
        atomic_store(&gStartTime, tsNow);
        atomic_store(&gSumLen, len);
        return dropped;
    }
    return 0;
}

#ifdef DEBUG
static size_t GetExecutablePath(char *processdir, char *processname, size_t len)
{
    char* path_end = nullptr;
    if (readlink("/proc/self/exe", processdir, len) <= 0)
        return -1;
    path_end = strrchr(processdir, '/');
    if (path_end == nullptr)
        return -1;
    ++path_end;
    if (strncpy_s(processname, MAX_PATH_LEN, path_end, MAX_PATH_LEN - 1)) {
        return 0;
    }
    *path_end = '\0';
    return (size_t)(path_end - processdir);
}
#endif

int HiLogPrintArgs(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
    const char *fmt, va_list ap)
{
#ifdef DEBUG
    char dir[MAX_PATH_LEN] = {0};
    char name[MAX_PATH_LEN] = {0};
    (void)GetExecutablePath(dir, name, MAX_PATH_LEN);
    if (strcmp(name, "hilog_test") != 0) {
        return -1;
    }
#endif

    int ret;
    char buf[MAX_LOG_LEN] = {0};
    char *logBuf = buf;
    int traceBufLen = 0;
    HilogMsg header = {0};
    bool debug = false;
    bool priv = HILOG_DEFAULT_PRIVACY;

    if (tag == nullptr) {
        return -1;
    }

    if (!HiLogIsLoggable(domain, tag, level)) {
        return -1;
    }

    /* print traceid */
    if (g_registerFunc != nullptr) {
        uint64_t chainId = 0;
        uint32_t flag = 0;
        uint64_t spanId = 0;
        uint64_t parentSpanId = 0;
        int ret = -1;  /* default value -1: invalid trace id */
        atomic_fetch_add_explicit(&g_hiLogGetIdCallCount, 1, memory_order_relaxed);
        RegisterFunc func = g_registerFunc;
        if (g_registerFunc != nullptr) {
            ret = func(&chainId, &flag, &spanId, &parentSpanId);
        }
        atomic_fetch_sub_explicit(&g_hiLogGetIdCallCount, 1, memory_order_relaxed);
        if (ret == 0) {  /* 0: trace id with span id */
            traceBufLen = snprintf_s(logBuf, MAX_LOG_LEN, MAX_LOG_LEN - 1, "[%llx, %llx, %llx] ",
                (unsigned long long)chainId, (unsigned long long)spanId, (unsigned long long)parentSpanId);
        } else if (ret != -1) {  /* trace id without span id, -1: invalid trace id */
            traceBufLen = snprintf_s(logBuf, MAX_LOG_LEN, MAX_LOG_LEN - 1, "[%llx] ",
                (unsigned long long)chainId);
        }
        if (traceBufLen > 0) {
            logBuf += traceBufLen;
        } else {
            traceBufLen = 0;
        }
    }

    /* format log string */
    debug = IsDebugOn();
    priv = (!debug) && IsPrivateSwitchOn();

#ifdef __clang__
/* code specific to clang compiler */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif __GNUC__
/* code for GNU C compiler */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
    ret = vsnprintfp_s(logBuf, MAX_LOG_LEN - traceBufLen, MAX_LOG_LEN - traceBufLen - 1, priv, fmt, ap);
#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

    /* fill header info */
    auto tagLen = strnlen(tag, MAX_TAG_LEN - 1);
    auto logLen = strnlen(buf, MAX_LOG_LEN - 1);
    header.type = type;
    header.level = level;
#ifndef __RECV_MSG_WITH_UCRED_
    header.pid = getpid();
#endif
    header.tid = static_cast<uint32_t>(syscall(SYS_gettid));
    header.domain = domain;

    /* flow control */
    ret = HiLogFlowCtrlProcess(tagLen + logLen, type, debug);
    if (ret < 0) {
        return ret;
    } else if (ret > 0) {
        char dropLogBuf[MAX_LOG_LEN] = {0};
        if (snprintf_s(dropLogBuf, MAX_LOG_LEN, MAX_LOG_LEN - 1, "%d line(s) dropped!", ret) == EOK) {
            HilogWriteLogMessage(&header, P_LIMIT_TAG, strlen(P_LIMIT_TAG) + 1, dropLogBuf,
                strnlen(dropLogBuf, MAX_LOG_LEN - 1) + 1);
        }
    }

    return HilogWriteLogMessage(&header, tag, tagLen + 1, buf, logLen + 1);
}

int HiLogPrint(LogType type, LogLevel level, unsigned int domain, const char *tag, const char *fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = HiLogPrintArgs(type, level, domain, tag, fmt, ap);
    va_end(ap);
    return ret;
}

bool HiLogIsLoggable(unsigned int domain, const char *tag, LogLevel level)
{
    if ((level <= LOG_LEVEL_MIN) || (level >= LOG_LEVEL_MAX) || tag == nullptr) {
        return false;
    }
    if (level < GetFinalLevel(domain, tag)) {
        return false;
    }
    return true;
}
