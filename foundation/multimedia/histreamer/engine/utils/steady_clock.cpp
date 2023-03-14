/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "SteadyClock"

#include "steady_clock.h"
#include "foundation/log.h"

namespace OHOS {
namespace Media {
namespace {
using namespace std::chrono;
}

int64_t SteadyClock::GetCurrentTimeMs()
{
    return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

int64_t SteadyClock::GetCurrentTimeNanoSec()
{
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}
SteadyClock::SteadyClock() : begin_(high_resolution_clock::now())
{
    MEDIA_LOG_D("ctor called.");
}

void SteadyClock::Reset()
{
    MEDIA_LOG_D("Reset timer.");
    begin_ = high_resolution_clock::now();
}

int64_t SteadyClock::ElapsedMilliseconds()
{
    return duration_cast<milliseconds>(high_resolution_clock::now() - begin_).count();
}

int64_t SteadyClock::ElapsedMicroseconds()
{
    return duration_cast<microseconds>(high_resolution_clock::now() - begin_).count();
}

int64_t SteadyClock::ElapsedNanoseconds()
{
    return duration_cast<nanoseconds>(high_resolution_clock::now() - begin_).count();
}

int64_t SteadyClock::ElapsedSeconds()
{
    return duration_cast<seconds>(high_resolution_clock::now() - begin_).count();
}
} // namespace Media
} // namespace OHOS
