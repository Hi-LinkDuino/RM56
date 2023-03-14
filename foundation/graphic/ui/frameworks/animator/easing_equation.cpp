/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "animator/easing_equation.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
double EasingEquation::overshoot_ = 1.7; // The empirical value commonly used in easing equation

void EasingEquation::SetBackOvershoot(double overshoot)
{
    if ((overshoot >= OVERSHOOT_MIN) && (overshoot <= OVERSHOOT_MAX)) {
        overshoot_ = overshoot;
    }
}

int16_t EasingEquation::BackEaseIn(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        double t = -(static_cast<double>(curTime) / durationTime);
        double x = -t * t * ((overshoot_ + 1) * t + overshoot_);
        return static_cast<int16_t>((x * (static_cast<int32_t>(endPos) - startPos)) + startPos);
    }

    return endPos;
}

int16_t EasingEquation::BackEaseOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        double t = static_cast<double>(curTime) / durationTime;
        t -= 1.0;
        double x = t * t * ((overshoot_ + 1) * t + overshoot_) + 1;
        return static_cast<int16_t>((x * (static_cast<int32_t>(endPos) - startPos)) + startPos);
    }

    return endPos;
}

int16_t EasingEquation::BackEaseInOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    uint16_t halfTime = durationTime >> 1;
    int16_t halfStep = (endPos >> 1) + (startPos >> 1);
    if (curTime < halfTime) {
        return BackEaseIn(startPos, halfStep, curTime, halfTime);
    }
    return BackEaseOut(halfStep, endPos, curTime - halfTime, halfTime);
}

/* 1 - sqrt(1 - t^2) */
int16_t EasingEquation::CircEaseIn(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int32_t t = (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;
        uint32_t x = INTERPOLATION_RANGE - static_cast<int32_t>(Sqrt(INTERPOLATION_RANGE_SQUARE - t * t));
        return static_cast<int16_t>(((x * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

/* sqrt(1 - (1 - t)^2) */
int16_t EasingEquation::CircEaseOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int32_t t = INTERPOLATION_RANGE - (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;
        uint32_t x = static_cast<uint32_t>(Sqrt(INTERPOLATION_RANGE_SQUARE - t * t));
        return static_cast<int16_t>(((x * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

int16_t EasingEquation::CircEaseInOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    uint16_t halfTime = durationTime >> 1;
    int16_t halfStep = (endPos >> 1) + (startPos >> 1);
    if (curTime < halfTime) {
        return CircEaseIn(startPos, halfStep, curTime, halfTime);
    }
    return CircEaseOut(halfStep, endPos, curTime - halfTime, halfTime);
}

/* t^3 */
int16_t EasingEquation::CubicEaseIn(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int32_t t = (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;
        int16_t x = (t * t * t) >> (INTERPOLATION_RANGE_OFFSET << 1);
        return static_cast<int16_t>(((x * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

/* 1 - (1 - t)^3 */
int16_t EasingEquation::CubicEaseOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int32_t t = (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;
        t = INTERPOLATION_RANGE - t;
        int16_t x = INTERPOLATION_RANGE - ((t * t * t) >> (INTERPOLATION_RANGE_OFFSET << 1));
        return static_cast<int16_t>(((x * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

int16_t EasingEquation::CubicEaseInOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    uint16_t halfTime = durationTime >> 1;
    int16_t halfStep = (endPos >> 1) + (startPos >> 1);
    if (curTime < halfTime) {
        return CubicEaseIn(startPos, halfStep, curTime, halfTime);
    }
    return CubicEaseOut(halfStep, endPos, curTime - halfTime, halfTime);
}

int16_t EasingEquation::LinearEaseNone(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int32_t t = (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;
        return static_cast<int16_t>(((t * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

/* t^2 */
int16_t EasingEquation::QuadEaseIn(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int32_t t = (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;
        int16_t x = (t * t) >> INTERPOLATION_RANGE_OFFSET;
        return static_cast<int16_t>(((x * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

/* 1 - (1 - t)^2 */
int16_t EasingEquation::QuadEaseOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int32_t t = INTERPOLATION_RANGE - (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;
        int16_t x = INTERPOLATION_RANGE - ((t * t) >> INTERPOLATION_RANGE_OFFSET);
        return static_cast<int16_t>(((x * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

int16_t EasingEquation::QuadEaseInOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    uint16_t halfTime = durationTime >> 1;
    int16_t halfStep = (endPos >> 1) + (startPos >> 1);
    if (curTime < halfTime) {
        return QuadEaseIn(startPos, halfStep, curTime, halfTime);
    }
    return QuadEaseOut(halfStep, endPos, curTime - halfTime, halfTime);
}

/* t^5 */
int16_t EasingEquation::QuintEaseIn(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int64_t t = (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;

        /* 4: the fourth power of t */
        int16_t x = (t * t * t * t * t) >> (INTERPOLATION_RANGE_OFFSET * 4);
        return static_cast<int16_t>(((x * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

/* 1 - (1 - t)^5 */
int16_t EasingEquation::QuintEaseOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int64_t t = (curTime << INTERPOLATION_RANGE_OFFSET) / durationTime;
        t = INTERPOLATION_RANGE - t;

        /* 4: the fourth power of t */
        int16_t x = INTERPOLATION_RANGE - ((t * t * t * t * t) >> (INTERPOLATION_RANGE_OFFSET * 4));
        return static_cast<int16_t>(((x * (static_cast<int32_t>(endPos) - startPos)) >> INTERPOLATION_RANGE_OFFSET) +
                                    startPos);
    }

    return endPos;
}

int16_t EasingEquation::QuintEaseInOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    uint16_t halfTime = durationTime >> 1;
    int16_t halfStep = (endPos >> 1) + (startPos >> 1);
    if (curTime < halfTime) {
        return QuintEaseIn(startPos, halfStep, curTime, halfTime);
    }
    return QuintEaseOut(halfStep, endPos, curTime - halfTime, halfTime);
}

int16_t EasingEquation::SineEaseIn(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int16_t t = (curTime * QUARTER_IN_DEGREE) / durationTime - QUARTER_IN_DEGREE;
        float x = Sin(t) + 1;
        return static_cast<int16_t>(x * (endPos - startPos)) + startPos;
    }

    return endPos;
}

int16_t EasingEquation::SineEaseOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    if (curTime < durationTime) {
        int16_t t = (curTime * QUARTER_IN_DEGREE) / durationTime;
        float x = Sin(t);
        return static_cast<int16_t>(x * (endPos - startPos)) + startPos;
    }

    return endPos;
}

int16_t EasingEquation::SineEaseInOut(int16_t startPos, int16_t endPos, uint16_t curTime, uint16_t durationTime)
{
    uint16_t halfTime = durationTime >> 1;
    int16_t halfStep = (endPos >> 1) + (startPos >> 1);
    if (curTime < halfTime) {
        return SineEaseIn(startPos, halfStep, curTime, halfTime);
    }
    return SineEaseOut(halfStep, endPos, curTime - halfTime, halfTime);
}
} // namespace OHOS
