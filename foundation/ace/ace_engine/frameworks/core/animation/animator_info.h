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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATOR_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATOR_INFO_H

#include "base/memory/ace_type.h"
#include "core/animation/animator.h"
#include "core/animation/curve.h"

namespace OHOS::Ace::Framework {

using AnimatorEventFunc = std::function<void(const float&)>;

class AnimatorInfo : public AceType {

public:
    AnimatorInfo() = default;
    ~AnimatorInfo() override = default;

    void SetAnimator(const RefPtr<Animator>& animator)
    {
        if (!animator) {
            LOGE("set animator failed. animator is null.");
            return;
        }
        animator_ = animator;
    }

    RefPtr<Animator> GetAnimator() const
    {
        return animator_;
    }

    void SetCurve(const RefPtr<Curve>& curve)
    {
        if (!curve) {
            LOGE("set curve failed. curve is null.");
            return;
        }
        curve_ = curve;
    }

    const RefPtr<Curve>& GetCurve() const
    {
        return curve_;
    }

    void SetDelay(int32_t delay)
    {
        delay_ = delay;
    }

    int32_t GetDelay() const
    {
        return delay_;
    }

    void SetIteration(int32_t iteration)
    {
        iteration_ = iteration;
    }

    int32_t GetIteration() const
    {
        return iteration_;
    }

    void SetFillMode(FillMode fillMode)
    {
        fillMode_ = fillMode;
    }

    FillMode GetFillMode() const
    {
        return fillMode_;
    }

    void SetPlayMode(AnimationDirection playMode)
    {
        playMode_ = playMode;
    }

    AnimationDirection GetPlayMode() const
    {
        return playMode_;
    }

    void SetAnimatorMotion(const RefPtr<Motion>& motion)
    {
        motion_ = motion;
    }

    const RefPtr<Motion>& GetAnimatorMotion() const
    {
        return motion_;
    }

    void SetDuration(int32_t duration)
    {
        duration_ = duration;
    }

    // Duration in millisecond.
    int32_t GetDuration() const
    {
        return duration_;
    }

    void SetFrameEvent(const AnimatorEventFunc& frameEvent)
    {
        frameEvent_ = frameEvent;
    }

    const AnimatorEventFunc& GetFrameEvent() const
    {
        return frameEvent_;
    }

    void OnJsEngineDestroy()
    {
        frameEvent_ = nullptr;
    }

private:
    RefPtr<Animator> animator_;
    RefPtr<Curve> curve_;
    FillMode fillMode_ = FillMode::FORWARDS;
    AnimationDirection playMode_ = AnimationDirection::NORMAL;
    RefPtr<Motion> motion_;
    AnimatorEventFunc frameEvent_;
    int32_t duration_ = 0;
    int32_t delay_ = 0;
    int32_t iteration_ = 1;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATOR_INFO_H