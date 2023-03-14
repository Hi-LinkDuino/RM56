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

#include "datetime_ex.h"
#include <chrono>
#include <cmath>
#include "utils_log.h"

using namespace std::chrono;
using namespace std;

namespace OHOS {

int64_t GetSecondsSince1970ToNow()
{
    using secondtype = duration<int64_t>;
    time_point<system_clock, secondtype> totalSeconds = time_point_cast<secondtype>(system_clock::now());
    return totalSeconds.time_since_epoch().count();
}

int64_t GetSecondsSince1970ToPointTime(struct tm inputTm)
{
    time_t inputTime = mktime(&inputTm);
    if (inputTime == -1) {
        UTILS_LOGE("mktime failed.");
        return -1;
    }
    system_clock::time_point pointTime = system_clock::from_time_t(inputTime);
    using secondtype = duration<int64_t>;
    time_point<system_clock, secondtype> totalSeconds = time_point_cast<secondtype>(pointTime);
    return totalSeconds.time_since_epoch().count();
}

int64_t GetSecondsBetween(struct tm inputTm1, struct tm inputTm2)
{
    int64_t second1 = GetSecondsSince1970ToPointTime(inputTm1);
    int64_t second2 = GetSecondsSince1970ToPointTime(inputTm2);
    return second1 >= second2 ? (second1 - second2) : (second2 - second1);
}

int64_t GetDaysSince1970ToNow()
{
    typedef duration<int64_t, std::ratio<SECONDS_PER_DAY>> dayType;
    time_point<system_clock, dayType> totalDays = time_point_cast<dayType>(system_clock::now());
    return totalDays.time_since_epoch().count();
}

bool GetSystemCurrentTime(struct tm* curTime)
{
    if (curTime == nullptr) {
        return false;
    }

    auto tt = system_clock::to_time_t(system_clock::now());
    struct tm* timeResult = nullptr;
    timeResult = localtime_r(&tt, curTime);
    return (timeResult != nullptr);
}

bool GetLocalTimeZone(int& timezone)
{
    auto t1 = system_clock::to_time_t(system_clock::now());
    auto t2 = t1;
    struct tm curTime1 = {0};
    struct tm curTime2 = {0};
    struct tm* localTime = localtime_r(&t1, &curTime1);
    struct tm* gmTime = gmtime_r(&t2, &curTime2);
    if ((localTime == nullptr) || (gmTime == nullptr)) {
        return false;
    }

    t1 = mktime(&curTime1);
    t2 = mktime(&curTime2);
    if ((t1 == -1) || (t2 == -1)) {
        UTILS_LOGE("mktime current time failed.");
        return false;
    }

    timezone = (static_cast<int>(t1 - t2)) / SECONDS_PER_HOUR;
    return true;
}

int64_t GetTickCount()
{
    return GetMicroTickCount() / SEC_TO_MILLISEC;
}

int64_t GetMicroTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * SEC_TO_MICROSEC + ts.tv_nsec / MICROSEC_TO_NANOSEC);
}

} // OHOS

