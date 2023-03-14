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

#ifndef UTILITY_TIME_UTIL_H
#define UTILITY_TIME_UTIL_H

#include <cstdint>
#include <ctime>
#include <string>

namespace OHOS {
namespace HiviewDFX {
namespace TimeUtil {
constexpr int64_t SEC_TO_NANOSEC = 1000000000;
constexpr int64_t SEC_TO_MICROSEC = 1000000;
constexpr int64_t SEC_TO_MILLISEC = 1000;
constexpr int64_t MILLISEC_TO_NANOSEC = 1000000;
constexpr int64_t MICROSEC_TO_NANOSEC = 1000;
constexpr int SECONDS_PER_MINUTE = 60; // 60 seconds
constexpr int SECONDS_PER_HOUR = 3600; // 60 * 60
constexpr int SECONDS_PER_DAY = 86400; // 60 * 60 * 24
constexpr int MAX_TIME_BUFF = 64; // 64 : for example 2021-05-27-01-01-01
uint64_t GetNanoTime();
uint64_t GetTimeOfDay();
uint64_t GenerateTimestamp();
uint64_t GetMilliseconds();
time_t StrToTimeStamp(const std::string &tmStr, const std::string& format);
void Sleep(unsigned int seconds);
int GetMillSecOfSec();
std::string TimestampFormatToDate(time_t timeStamp, const std::string& format);
std::string GetTimeZone();
int64_t Get0ClockStampMs();
} // namespace TimeUtil
} // namespace HiviewDFX
} // namespace OHOS
#endif // UTILITY_TIME_UTIL_H