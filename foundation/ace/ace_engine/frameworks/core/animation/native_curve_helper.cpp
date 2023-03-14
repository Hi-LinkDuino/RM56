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

#include "core/animation/native_curve_helper.h"

#include "core/animation/cubic_curve.h"
#include "core/animation/spring_curve.h"

namespace OHOS::Ace {

Rosen::RSAnimationTimingCurve NativeCurveHelper::ToNativeCurve(const RefPtr<Curve>& curve)
{
    if (AceType::InstanceOf<LinearCurve>(curve)) {
        return Rosen::RSAnimationTimingCurve::LINEAR;
    } else if (auto cubicCurve = AceType::DynamicCast<CubicCurve>(curve)) {
        return Rosen::RSAnimationTimingCurve::CreateCubicCurve(
            cubicCurve->x0_, cubicCurve->y0_, cubicCurve->x1_, cubicCurve->y1_);
    } else if (auto springCurve = AceType::DynamicCast<SpringCurve>(curve)) {
        return Rosen::RSAnimationTimingCurve::CreateSpringCurve(springCurve->velocity_, springCurve->mass_,
            springCurve->stiffness_, springCurve->damping_);
    } else if (auto customCurve = AceType::DynamicCast<CustomCurve>(curve)) {
        return Rosen::RSAnimationTimingCurve::CreateCustomCurve(customCurve->interpolateFunc_);
    } else {
        return Rosen::RSAnimationTimingCurve::CreateCustomCurve(
            [weak = WeakPtr<Curve>(curve)](float fraction) -> float {
                auto curve = weak.Upgrade();
                if (curve == nullptr) {
                    LOGE("transform to native curve failed, curve is null!");
                    return 1.0f;
                }

                return curve->MoveInternal(fraction);
            });
    }
}

Rosen::RSMotionPathOption NativeCurveHelper::ToNativeMotionPathOption(const MotionPathOption& option)
{
    auto motionOption = Rosen::RSMotionPathOption(option.GetPath());
    motionOption.SetBeginFraction(option.GetBegin());
    motionOption.SetEndFraction(option.GetEnd());
    motionOption.SetRotationMode(
        option.GetRotate() ? Rosen::RotationMode::ROTATE_AUTO : Rosen::RotationMode::ROTATE_NONE);
    return motionOption;
}

} // namespace OHOS::Ace
