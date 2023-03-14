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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_DATE_UTIL_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_DATE_UTIL_H

#include <cstdint>

namespace OHOS::Ace {

struct Date {
    Date() = default;
    Date(uint32_t year, uint32_t month, uint32_t day) : year(year), month(month), day(day) {}
    ~Date() = default;

    static Date Current();
    static int32_t DayOfMonth(int32_t year, int32_t month);
    static bool IsLeapYear(int32_t year);
    static int32_t CalculateWeekDay(int32_t year, int32_t month, int32_t day);

    uint32_t year = 1900;
    uint32_t month = 0;
    uint32_t day = 1;
    uint32_t week = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_DATE_UTIL_H
