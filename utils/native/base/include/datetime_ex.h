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

#ifndef DATETIME_EX_H
#define DATETIME_EX_H

#include <ctime>
#include <cstdint>
namespace OHOS {

constexpr int64_t SEC_TO_NANOSEC = 1000000000;
constexpr int64_t SEC_TO_MICROSEC = 1000000;
constexpr int64_t SEC_TO_MILLISEC = 1000;
constexpr int64_t MILLISEC_TO_NANOSEC = 1000000;
constexpr int64_t MICROSEC_TO_NANOSEC = 1000;

constexpr int SECONDS_PER_HOUR = 3600; // 60 * 60
constexpr int SECONDS_PER_DAY = 86400; // 60 * 60 * 24

static constexpr inline int64_t SecToNanosec(int64_t sec)
{
    return sec * SEC_TO_NANOSEC;
}

static constexpr inline int64_t MillisecToNanosec(int64_t millise)
{
    return millise * MILLISEC_TO_NANOSEC;
}

static constexpr inline int64_t MicrosecToNanosec(int64_t microsec)
{
    return microsec * MICROSEC_TO_NANOSEC;
}

static constexpr inline int64_t NanosecToSec(int64_t nanosec)
{
    return nanosec / SEC_TO_NANOSEC;
}

static constexpr inline int64_t NanosecToMillisec(int64_t nanosec)
{
    return nanosec / MILLISEC_TO_NANOSEC;
}

static constexpr inline int64_t NanosecToMicrosec(int64_t nanosec)
{
    return nanosec / MICROSEC_TO_NANOSEC;
}

/**
 * The GetSecondsSince1970ToNow function get seconds from 1970 to now
 */
int64_t GetSecondsSince1970ToNow();

/**
 * The GetSecondsSince1970ToPointTime function get seconds from 1970 to inputtime
 */
int64_t GetSecondsSince1970ToPointTime(struct tm inputTm);

/**
 * The GetSecondsBetween function get seconds between inputTm1 and inputTm2.
 */
int64_t GetSecondsBetween(struct tm inputTm1, struct tm inputTm2);

/**
 * The GetDaysSince1970ToNow function get days from 1970 to now.
 */
int64_t GetDaysSince1970ToNow();

/**
 * The GetLocalTimeZone function get current timezone,
 * return true if get success, else false.
 */
bool GetLocalTimeZone(int& timezone);

/**
* The GetSystemCurrentTime function get current time,
* return true if get success, else false.
*/
bool GetSystemCurrentTime(struct tm* curTime);

/**
* The GetTickCount function get current milliseconds since the system was started.
*/
int64_t GetTickCount();

/**
* The GetMicroTickCount function get current microseconds since the system was started.
*/
int64_t GetMicroTickCount();
}

#endif
