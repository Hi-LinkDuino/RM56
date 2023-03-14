/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CUBIC_CURVE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CUBIC_CURVE_H

#include "core/animation/curve.h"

namespace OHOS::Ace {
class NativeCurveHelper;

// Third-order bezier curve. Formula as follows:
// B(m) = (1-m)^3*P0 + 3m(1-m)^2*P1 + 3m^2*P2 + m^3*P3，
// where P0 = (0,0), P1 = (x0_, y0_), P2 = (x1_, y1_),  P3 = (1,1)
// so Bx(m) = 3m(1-m)^2*x0_ + 3m^2*x1_ + m^3
//    By(m) = 3m(1-m)^2*y0_ + 3m^2*y1_ + m^3
// in function MoveInternal, assume time as Bx(m), we let Bx(m) approaching time, and we can get m and the output By(m)
class ACE_EXPORT CubicCurve : public Curve {
    DECLARE_ACE_TYPE(CubicCurve, Curve);

public:
    CubicCurve(float x0, float y0, float x1, float y1);
    ~CubicCurve() override = default;

    float MoveInternal(float time) override;
    const std::string ToString() override;

private:
    // Bx(m) or By(m) = 3m(1-m)^2*a + 3m^2*b + m^3, where a = x0_ ,b = x1_ or a = y0_ ,b = y1_
    static float CalculateCubic(float a, float b, float m);

    float cubicErrorBound_ = 0.001f; // Control curve accuracy
    float x0_;                       // X-axis of the first point (P1)
    float y0_;                       // Y-axis of the first point (P1)
    float x1_;                       // X-axis of the second point (P2)
    float y1_;                       // Y-axis of the second point (P2)

    friend class NativeCurveHelper;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CUBIC_CURVE_H
