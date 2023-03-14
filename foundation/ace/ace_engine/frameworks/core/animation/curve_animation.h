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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CURVE_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CURVE_ANIMATION_H

#include "core/animation/animation.h"
#include "core/animation/curve.h"
#include "core/animation/curves.h"
#include "core/animation/evaluator.h"

namespace OHOS::Ace {

template<typename T>
class CurveAnimation : public Animation<T> {
public:
    CurveAnimation(const T& begin, const T& end, const RefPtr<Curve>& curve)
        : begin_(begin), end_(end), currentValue_(begin)
    {
        if (curve) {
            curve_ = curve;
        } else {
            LOGW("curve is empty when construct animation, use EASE as default.");
            curve_ = Curves::EASE;
        }
        reverseCurve_ = curve_;
    }

    ~CurveAnimation() override = default;

    const T& GetValue() const override
    {
        return currentValue_;
    }

    void SetCurve(const RefPtr<Curve>& curve) override
    {
        if (!curve) {
            LOGE("set curve failed. curve is null.");
            return;
        }
        curve_ = curve;
        reverseCurve_ = curve;
    }

    void SetReverseCurve(const RefPtr<Curve>& reverseCurve)
    {
        if (!reverseCurve) {
            LOGE("set reverse curve failed. reverse curve is null.");
            return;
        }
        reverseCurve_ = reverseCurve;
    }

    void SetEvaluator(const RefPtr<Evaluator<T>>& evaluator)
    {
        if (evaluator) {
            isSupportedRunningAsync_ = false;
            evaluator_ = evaluator;
        }
    }

    bool IsSupportedRunningAsynchronously() override
    {
        return isSupportedRunningAsync_;
    }

    bool RunAsync(const WeakPtr<Scheduler>& weakScheduler, const AnimationOption& option,
        const std::function<void()> prepareCallback = nullptr,
        const std::function<void()> finishCallback = nullptr) override
    {
        if (!isSupportedRunningAsync_) {
            LOGE("can not run animation asynchronously with nonlinear evaluator.");
            return false;
        }

        return Interpolator::RunAsync(weakScheduler, option, prepareCallback, finishCallback);
    }

    RefPtr<Curve> GetCurve() override
    {
        return curve_ != nullptr ? curve_ : AceType::DynamicCast<Curve>(Curves::EASE_IN_OUT);
    }

private:
    // Use the normalized time [0.0, 1.0] to get the current data value.
    // Map the value of the curve motion to the real data range.
    void Calculate(float time, const RefPtr<Curve>& curve)
    {
        if (NearZero(time)) {
            currentValue_ = begin_;
            return;
        }
        if (NearEqual(time, 1.0)) {
            currentValue_ = end_;
            return;
        }
        currentValue_ = evaluator_->Evaluate(begin_, end_, curve->Move(time));
    }

    void OnNormalizedTimestampChanged(float normalized, bool reverse) override
    {
        if (normalized < NORMALIZED_DURATION_MIN || normalized > NORMALIZED_DURATION_MAX) {
            LOGE("normalized time check failed. normalized: %{public}f", normalized);
            return;
        }
        Calculate(normalized, reverse ? reverseCurve_ : curve_);
        ValueListenable<T>::NotifyListener(currentValue_);
    }

    T begin_;
    T end_;
    T currentValue_;
    bool isSupportedRunningAsync_ { true };
    RefPtr<Curve> curve_;
    RefPtr<Curve> reverseCurve_;
    RefPtr<Evaluator<T>> evaluator_ = AceType::MakeRefPtr<LinearEvaluator<T>>();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CURVE_ANIMATION_H
