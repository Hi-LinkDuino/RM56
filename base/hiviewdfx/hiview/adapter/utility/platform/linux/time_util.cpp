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

#include "time_util.h"
#include <chrono>
#include <sys/time.h>
#include <unistd.h>

namespace OHOS {
namespace HiviewDFX {
namespace TimeUtil {
time_t StrToTimeStamp(const std::string& tmStr, const std::string& format)
{
    std::string stTime = tmStr;
    struct tm tmFormat { 0 };
    strptime(stTime.c_str(), format.c_str(), &tmFormat);
    tmFormat.tm_isdst = -1;
    return mktime(&tmFormat);
}

uint64_t GenerateTimestamp()
{
    struct timeval now { 0 };
    if (gettimeofday(&now, nullptr) == -1) {
        return 0;
    }
    return (now.tv_sec * SEC_TO_MICROSEC + now.tv_usec);
}

void Sleep(unsigned int seconds)
{
    sleep(seconds);
}

int GetMillSecOfSec()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count() % SEC_TO_MILLISEC;
}

uint64_t GetMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

std::string TimestampFormatToDate(time_t timeStamp, const std::string& format)
{
    char date[MAX_TIME_BUFF] = {0};
    struct tm result {};
    if (localtime_r(&timeStamp, &result) != nullptr) {
        strftime(date, MAX_TIME_BUFF, format.c_str(), &result);
    }
    return std::string(date);
}

std::string GetTimeZone()
{
    struct timeval tv;
    struct timezone tz;
    if (gettimeofday(&tv, &tz) != 0) {
        return "";
    }
    int tzHour = (-tz.tz_minuteswest) / SECONDS_PER_MINUTE;
    std::string timeZone = (tzHour >= 0) ? "+" : "-";
    int absTzHour = std::abs(tzHour);
    if (absTzHour < 10) { // less than 10 hour
        timeZone.append("0");
    }
    timeZone.append(std::to_string(absTzHour));

    int tzMin = (-tz.tz_minuteswest) % SECONDS_PER_MINUTE;
    int absTzMin = std::abs(tzMin);
    if (absTzMin < 10) { // less than 10 minute
        timeZone.append("0");
    }
    timeZone.append(std::to_string(absTzMin));
    return timeZone;
}

int64_t Get0ClockStampMs()
{
    time_t now = std::time(nullptr);
    int64_t zero = now;
    struct tm *l = std::localtime(&now);
    if (l != nullptr) {
        l->tm_hour = 0;
        l->tm_min = 0;
        l->tm_sec = 0;
        zero = std::mktime(l) * SEC_TO_MILLISEC;  // time is 00:00:00
    }
    return zero;
}
} // namespace TimeUtil
} // namespace HiviewDFX
} // namespace OHOS
