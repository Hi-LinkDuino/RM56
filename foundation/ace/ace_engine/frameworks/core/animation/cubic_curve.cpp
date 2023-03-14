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

#include "core/animation/cubic_curve.h"

namespace OHOS::Ace {

CubicCurve::CubicCurve(float x0, float y0, float x1, float y1)
    : x0_(x0), y0_(y0), x1_(x1), y1_(y1)
{}

float CubicCurve::MoveInternal(float time)
{
    // let P0 = (0,0), P3 = (1,1)
    float start = 0.0f;
    float end = 1.0f;
    while (true) {
        float midpoint = (start + end) / 2;
        float estimate = CalculateCubic(x0_, x1_, midpoint);

        if (NearEqual(time, estimate, cubicErrorBound_)) {
            return CalculateCubic(y0_, y1_, midpoint);
        }

        if (estimate < time) {
            start = midpoint;
        } else {
            end = midpoint;
        }
    }
}

const std::string CubicCurve::ToString()
{
    std::string curveString("cubic-bezier");
    std::string comma(",");
    curveString.append(std::string("(") + std::to_string(x0_) + comma + std::to_string(y0_)
        + comma + std::to_string(x1_) + comma + std::to_string(y1_) + std::string(")"));
    return curveString;
}

float CubicCurve::CalculateCubic(float a, float b, float m)
{
    return 3.0f * a * (1.0f - m) * (1.0f - m) * m + 3.0f * b * (1.0f - m) * m * m + m * m * m;
}

} // namespace OHOS::Ace
