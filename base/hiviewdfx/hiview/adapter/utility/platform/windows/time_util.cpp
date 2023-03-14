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
#include <iomanip>
#include <sstream>
#include <thread>

namespace OHOS {
namespace HiviewDFX {
namespace TimeUtil {
time_t StrToTimeStamp(const std::string& tmStr, const std::string& format)
{
    std::istringstream input(tmStr.c_str());
    struct tm tmFormat = { 0 };
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(&tmFormat, format.c_str());
    if (input.fail()) {
        return 0;
    }

    return mktime(&tmFormat);
}

uint64_t GetTimeOfDay()
{
    struct timespec now = { 0, 0 };
    int err = timespec_get(&now, TIME_UTC);
    if (err == 0) {
        return 0;
    }
    return static_cast<uint64_t>(now.tv_sec);
}

uint64_t GenerateTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    return static_cast<uint64_t>(secs.count());
}

uint64_t GetMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

void Sleep(unsigned int seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

int GetMillSecOfSec()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count() % SEC_TO_MILLISEC;
}

std::string GetTimeZone()
{
    std::string timeZone = "";
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
