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

#include "base/utils/time_util.h"

#include <sys/time.h>

#include "base/log/log.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {
namespace {

constexpr int64_t SEC_TO_MICROSEC = 1000000;
constexpr int64_t SEC_TO_NANOSEC = 1000000000;
constexpr int64_t MICROSEC_TO_NANOSEC = 1000;
constexpr int32_t HOURS_WEST_GEOGRAPHICAL_LOWER_LIMIT = -12;
constexpr int32_t HOURS_WEST_LOWER_LIMIT = -14;
constexpr int32_t HOURS_WEST_UPPER_LIMIT = 12;
constexpr int32_t TOTAL_SECONDS_OF_DAY = 24 * 60 * 60;
constexpr int32_t TOTAL_SECONDS_OF_HOUR = 60 * 60;
constexpr int32_t TOTAL_SECONDS_OF_MINUTE = 60;
constexpr int32_t TOTAL_MINUTE_OF_HOUR = 60;
constexpr int32_t TWENTY_FOUR_HOUR_BASE = 24;
constexpr int32_t TWELVE_HOUR_BASE = 12;
constexpr int32_t DAY_TIME_LOWER_LIMIT = 6;
constexpr int32_t DAY_TIME_UPPER_LIMIT = 18;

} // namespace

int64_t GetMicroTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * SEC_TO_MICROSEC + ts.tv_nsec / MICROSEC_TO_NANOSEC);
}

int64_t GetSysTimestamp()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * SEC_TO_NANOSEC + ts.tv_nsec;
}


TimeOfNow GetTimeOfNow(double hoursWest)
{
    struct timeval currentTime;
    struct timezone timeZone;
    gettimeofday(&currentTime, &timeZone);

    TimeOfNow timeOfNow(hoursWest);
    int32_t minutesWest = timeZone.tz_minuteswest;
    if (IsHoursWestValid(timeOfNow.hoursWest_)) {
        minutesWest = Round(TOTAL_MINUTE_OF_HOUR * timeOfNow.hoursWest_);
    } else {
        // when [hoursWest] is invalid, set current time zone to [hoursWest].
        if (!NearEqual(timeOfNow.hoursWest_, DBL_MAX)) { // default value of hoursWest_ is DBL_MAX
            LOGW("hoursWest [%{public}lf] is invalid, use current time zone.", timeOfNow.hoursWest_);
        }
        timeOfNow.hoursWest_ = static_cast<double>(timeZone.tz_minuteswest) / static_cast<double>(TOTAL_MINUTE_OF_HOUR);
    }
    int secondsOfToday = currentTime.tv_sec % TOTAL_SECONDS_OF_DAY - minutesWest * TOTAL_SECONDS_OF_MINUTE;
    if (secondsOfToday < 0) {
        secondsOfToday += TOTAL_SECONDS_OF_DAY;
    }
    timeOfNow.minute_ = (secondsOfToday / TOTAL_SECONDS_OF_MINUTE) % TOTAL_MINUTE_OF_HOUR +
                        static_cast<double>(secondsOfToday % TOTAL_SECONDS_OF_MINUTE) / TOTAL_SECONDS_OF_MINUTE;
    timeOfNow.hour24_ =
        (secondsOfToday / TOTAL_SECONDS_OF_HOUR) % TWENTY_FOUR_HOUR_BASE + timeOfNow.minute_ / TOTAL_MINUTE_OF_HOUR;
    timeOfNow.hour12_ =
        (timeOfNow.hour24_ < TWELVE_HOUR_BASE) ? timeOfNow.hour24_ : (timeOfNow.hour24_ - TWELVE_HOUR_BASE);
    timeOfNow.second_ = secondsOfToday % TOTAL_SECONDS_OF_MINUTE;
    timeOfNow.timeUsec_ = currentTime.tv_usec;
    return timeOfNow;
}

bool IsHoursWestValid(double& hoursWest)
{
    // valid hoursWest is within [-14, 12]
    bool isValid = GreatOrEqual(hoursWest, HOURS_WEST_LOWER_LIMIT) && LessOrEqual(hoursWest, HOURS_WEST_UPPER_LIMIT);
    if (!isValid) {
        return false;
    }
    // Theoretically, the time zone range should be [-12, +12], but some countries and regions that cross the
    // International Date Line use -13(UTC+13) and -14(UTC+14) to keep the whole country or region at the same date.
    bool isSpecialTimeZone = LessNotEqual(hoursWest, HOURS_WEST_GEOGRAPHICAL_LOWER_LIMIT);
    if (isSpecialTimeZone) {
        hoursWest += TWENTY_FOUR_HOUR_BASE;
    }
    return true;
}

bool IsDayTime(const TimeOfNow& timeOfNow)
{
    return GreatOrEqual(timeOfNow.hour24_, DAY_TIME_LOWER_LIMIT) &&
           LessNotEqual(timeOfNow.hour24_, DAY_TIME_UPPER_LIMIT);
}

} // namespace OHOS::Ace
