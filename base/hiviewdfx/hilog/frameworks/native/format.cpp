/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#include "format.h"
#include "hilog/log.h"
#include "hilogtool_msg.h"
#include "hilog_common.h"


#include <cstring>
#include <iostream>
#include <ctime>
#include <securec.h>

namespace OHOS {
namespace HiviewDFX {
namespace {
constexpr int HILOG_COLOR_BLUE = 75;
constexpr int HILOG_COLOR_DEFAULT = 231;
constexpr int HILOG_COLOR_GREEN = 40;
constexpr int HILOG_COLOR_ORANGE = 166;
constexpr int HILOG_COLOR_RED = 196;
constexpr int HILOG_COLOR_YELLOW = 226;

const char* ParsedFromLevel(uint16_t level)
{
    switch (level) {
        case LOG_DEBUG:
            return "D";
        case LOG_INFO:
            return "I";
        case LOG_WARN:
            return "W";
        case LOG_ERROR:
            return "E";
        case LOG_FATAL:
            return "F";
        default:
            return " ";
    }
}

int ColorFromLevel(uint16_t level)
{
    switch (level) {
        case LOG_DEBUG:
            return HILOG_COLOR_BLUE;
        case LOG_INFO:
            return HILOG_COLOR_GREEN;
        case LOG_WARN:
            return HILOG_COLOR_ORANGE;
        case LOG_ERROR:
            return HILOG_COLOR_YELLOW;
        case LOG_FATAL:
            return HILOG_COLOR_RED;
        default:
            return HILOG_COLOR_DEFAULT;
    }
}
} // anoymous namespace

int HilogShowTimeBuffer(char* buffer, int bufLen, uint32_t showFormat,
    const HilogShowFormatBuffer& contentOut)
{
    time_t now = contentOut.tv_sec;
    unsigned long nsecTime = contentOut.tv_nsec;
    size_t timeLen = 0;
    int ret = 0;
    nsecTime = (now < 0) ? (NSEC - nsecTime) : nsecTime;
    if ((showFormat & (1 << EPOCH_SHOWFORMAT)) || (showFormat & (1 << MONOTONIC_SHOWFORMAT))) {
        if ((bufLen - 1) > 0) {
            ret = snprintf_s(buffer, bufLen, bufLen - 1,
                (showFormat & (1 << MONOTONIC_SHOWFORMAT)) ? "%6lld" : "%19lld", (long long)now);
        }
        timeLen += ((ret > 0) ? ret : 0);
    } else {
        struct tm tmLocal;
        if (localtime_r(&now, &tmLocal) == nullptr) {
            return 0;
        }
        timeLen = strftime(buffer, bufLen, "%m-%d %H:%M:%S", &tmLocal);
        timeLen = strlen(buffer);
        if (showFormat & (1 << YEAR_SHOWFORMAT)) {
            timeLen = strftime(buffer, bufLen, "%Y-%m-%d %H:%M:%S", &tmLocal);
            timeLen = strlen(buffer);
        }
        if (showFormat & (1 << ZONE_SHOWFORMAT)) {
            timeLen = strftime(buffer, bufLen, "%z %m-%d %H:%M:%S", &tmLocal);
            timeLen = strlen(buffer);
        }
    }
    if (showFormat & (1 << TIME_NSEC_SHOWFORMAT)) {
        if ((bufLen - timeLen - 1) > 0) {
            ret = snprintf_s(buffer + timeLen, bufLen - timeLen, bufLen - timeLen - 1,
                ".%09lu", nsecTime);
        }
        timeLen += ((ret > 0) ? ret : 0);
    } else if (showFormat & (1 << TIME_USEC_SHOWFORMAT)) {
        if ((bufLen - timeLen - 1) > 0) {
            ret = snprintf_s(buffer + timeLen, bufLen - timeLen, bufLen - timeLen - 1,
                ".%06llu", nsecTime / NS2US);
        }
        timeLen += ((ret > 0) ? ret : 0);
    } else {
        if ((bufLen - timeLen - 1) > 0) {
            ret = snprintf_s(buffer + timeLen, bufLen - timeLen, bufLen - timeLen - 1,
                ".%03llu", nsecTime / NS2MS);
        }
        timeLen += ((ret > 0) ? ret : 0);
    }
    return timeLen;
}

void HilogShowBuffer(char* buffer, int bufLen, const HilogShowFormatBuffer& contentOut, uint32_t showFormat)
{
    int logLen = 0;
    int ret = 0;
    if (buffer == nullptr) {
        return;
    }
    if (showFormat & (1 << COLOR_SHOWFORMAT)) {
        if ((bufLen - logLen - 1) > 0) {
            ret = snprintf_s(buffer, bufLen, bufLen - logLen - 1,
                "\x1B[38;5;%dm", ColorFromLevel(contentOut.level));
        }
        logLen += ((ret > 0) ? ret : 0);
    }
    logLen += HilogShowTimeBuffer(buffer + logLen, bufLen - logLen, showFormat, contentOut);
    if ((bufLen - logLen - 1) > 0) {
        ret = snprintf_s(buffer + logLen, bufLen - logLen, bufLen - logLen - 1,
            " %5d", contentOut.pid);
    }
    logLen += ((ret > 0) ? ret : 0);
    if ((bufLen - logLen - 1) > 0) {
        ret = snprintf_s(buffer + logLen, bufLen - logLen, bufLen - logLen - 1,
            " %5d", contentOut.tid);
    }
    logLen += ((ret > 0) ? ret : 0);
    if ((bufLen - logLen - 1) > 0) {
        ret = snprintf_s(buffer + logLen, bufLen - logLen, bufLen - logLen - 1,
            " %s ", ParsedFromLevel(contentOut.level));
    }
    logLen += ((ret > 0) ? ret : 0);
    if ((bufLen - logLen - 1) > 0) {
        ret = snprintf_s(buffer + logLen, bufLen - logLen, bufLen - logLen - 1,
            "%05x/%s:", contentOut.domain & 0xFFFFF, contentOut.data);
    }
    logLen += ((ret > 0) ? ret : 0);
    if ((bufLen - logLen - 1) > 0) {
        ret = snprintf_s(buffer + logLen, bufLen - logLen, bufLen - logLen - 1,
            " %s", contentOut.data + contentOut.tag_len);
    }
    logLen += ((ret > 0) ? ret : 0);
    if (showFormat & (1 << COLOR_SHOWFORMAT)) {
        const char suffixColor[] = "\x1B[0m";
        if (strcpy_s(buffer + logLen, sizeof(suffixColor), suffixColor) != 0) {
            return;
        }
        logLen += strlen(suffixColor);
    }
}
} // namespace HiviewDFX
} // namespace OHOS
