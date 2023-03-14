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

#include "base/utils/date_util.h"

#include <chrono>
#include <ctime>

namespace OHOS::Ace {

Date Date::Current()
{
    Date date;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto local = std::localtime(&now);
    if (local == nullptr) {
        return date;
    }
    date.year = static_cast<uint32_t>(local->tm_year) + 1900; // local date start from 1900
    date.month = static_cast<uint32_t>(local->tm_mon) + 1;    // local month start from 0 to 11, need add one.
    date.day = static_cast<uint32_t>(local->tm_mday);
    date.week = static_cast<uint32_t>(local->tm_wday);
    return date;
}

bool Date::IsLeapYear(int32_t year)
{
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

int32_t Date::DayOfMonth(int32_t year, int32_t month)
{
    int32_t day = 0;
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            day = 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            day = 30;
            break;
        case 2:
            day = IsLeapYear(year) ? 29 : 28;
            break;
        default:
            day = 0;
    }
    return day;
}

int32_t Date::CalculateWeekDay(int32_t year, int32_t month, int32_t day)
{
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }

    // Day of the week calculation formula
    return (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
}

} // namespace OHOS::Ace
