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
#ifndef POWERMGR_POWER_MGR_TIME_UTIL_H
#define POWERMGR_POWER_MGR_TIME_UTIL_H

#include <stdint.h>

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum {
    NSEC_PER_MIN = 60000000000LL,
    NSEC_PER_SEC = 1000000000LL,
    NSEC_PER_MSEC = 1000000LL,
    NSEC_PER_USEC = 1000LL,
    USEC_PER_SEC = 1000000LL,
    USEC_PER_MSEC = 1000LL,
    MSEC_PER_DAY = 86400000LL,
    MSEC_PER_HOUR = 3600000LL,
    MSEC_PER_MIN = 60000LL,
    MSEC_PER_SEC = 1000LL,
    SEC_PER_MIN = 60LL,
    SEC_PER_HOUR = 3600LL,
    SEC_PER_DAY = 86400LL,
    MIN_PER_HOUR = 60LL,
    HOUR_PER_DAY = 24LL,
};

static inline int64_t NsecToUsec(int64_t nsec)
{
    return nsec / NSEC_PER_USEC;
}

static inline int64_t NsecToMsec(int64_t nsec)
{
    return nsec / NSEC_PER_MSEC;
}

static inline int64_t UsecToMsec(int64_t usec)
{
    return usec / USEC_PER_MSEC;
}

static inline int64_t MsecToNsec(int64_t msec)
{
    return msec * NSEC_PER_MSEC;
}

static inline int64_t MsecToUsec(int64_t msec)
{
    return msec * USEC_PER_MSEC;
}

static inline int64_t MsecToSec(int64_t msec)
{
    return msec / MSEC_PER_SEC;
}

static inline int64_t MsecToMin(int64_t msec)
{
    return msec / MSEC_PER_MIN;
}

static inline int64_t MsecToHour(int64_t msec)
{
    return msec / MSEC_PER_HOUR;
}

static inline int64_t SecToNsec(int64_t sec)
{
    return sec * NSEC_PER_SEC;
}

static inline int64_t SecToMsec(int64_t sec)
{
    return sec * MSEC_PER_SEC;
}

static inline int64_t MinToNsec(int64_t mins)
{
    return mins * NSEC_PER_MIN;
}

static inline int64_t MinToMsec(int64_t mins)
{
    return mins * MSEC_PER_MIN;
}

static inline int64_t HourToMsec(int64_t hour)
{
    return hour * MSEC_PER_HOUR;
}

static inline int64_t DayToMsec(int64_t day)
{
    return day * MSEC_PER_DAY;
}

int64_t GetCurrentTimeMsec(clockid_t clkId);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_POWER_MGR_TIME_UTIL_H
