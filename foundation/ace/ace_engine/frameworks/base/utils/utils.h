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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_UTILS_H

#include <chrono>
#include <cmath>
#include <cstdint>

namespace OHOS::Ace {

template<typename T, std::size_t N>
constexpr std::size_t ArraySize(T (&)[N]) noexcept
{
    return N;
}

template<typename T, int32_t N>
T ConvertIntToEnum(int32_t index, const T (&values)[N], T defaultValue)
{
    if (index >= 0 && index < N) {
        return values[index];
    }
    return defaultValue;
}

inline bool NearEqual(const double left, const double right, const double epsilon)
{
    return (std::abs(left - right) <= epsilon);
}

inline bool NearZero(const double value, const double epsilon)
{
    return NearEqual(value, 0.0, epsilon);
}

inline bool NearEqual(const double left, const double right)
{
    static constexpr double epsilon = 0.000001f;
    return NearEqual(left, right, epsilon);
}

inline bool NearZero(const double left)
{
    static constexpr double epsilon = 0.000001f;
    return NearZero(left, epsilon);
}

inline bool LessOrEqual(double left, double right)
{
    static constexpr double epsilon = 0.000001f;
    return (left - right) < epsilon;
}

inline bool LessNotEqual(double left, double right)
{
    static constexpr double epsilon = -0.000001f;
    return (left - right) < epsilon;
}

inline bool GreatOrEqual(double left, double right)
{
    static constexpr double epsilon = -0.000001f;
    return (left - right) > epsilon;
}

inline bool GreatNotEqual(double left, double right)
{
    static constexpr double epsilon = 0.000001f;
    return (left - right) > epsilon;
}

inline double Round(double rawNum)
{
    static constexpr double epsilon = 0.000001f;
    return std::round(rawNum + epsilon);
}

inline bool InRegion(double lowerBound, double upperBound, double destNum)
{
    return LessOrEqual(lowerBound, destNum) && LessOrEqual(destNum, upperBound);
}

inline uint64_t GetMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

inline uint64_t GetNanoseconds()
{
    auto now = std::chrono::system_clock::now();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
    return nanoseconds.count();
}

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_UTILS_H
