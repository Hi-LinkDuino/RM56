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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATION_OPTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATION_OPTION_H

#include <functional>
#include <string>
#include <unordered_map>

#include "core/animation/animation_pub.h"
#include "core/animation/curve.h"

namespace OHOS::Ace {

class AnimationOption final {
public:
    AnimationOption() = default;
    ~AnimationOption() = default;

    void SetDuration(int32_t duration)
    {
        duration_ = duration;
    }

    int32_t GetDuration() const
    {
        return duration_;
    }

    void SetDelay(int32_t delay)
    {
        delay_ = delay;
    }

    int32_t GetDelay() const
    {
        return delay_;
    }

    void SetCurve(const RefPtr<Curve>& curve)
    {
        if (!curve) {
            LOGE("set curve failed. curve is null.");
            return;
        }
        curve_ = curve;
    }

    void SetIteration(int32_t iteration)
    {
        iteration_ = iteration;
    }

    int32_t GetIteration() const
    {
        return iteration_;
    }

    void SetTempo(float tempo)
    {
        if (tempo < 0.0f) {
            return;
        }
        tempo_ = tempo;
    }

    float GetTempo() const
    {
        return tempo_;
    }

    void SetAnimationDirection(AnimationDirection direction)
    {
        direction_ = direction;
    }

    AnimationDirection GetAnimationDirection() const
    {
        return direction_;
    }

    const RefPtr<Curve>& GetCurve() const
    {
        return curve_;
    }

    void SetFillMode(const FillMode& fillMode)
    {
        fillMode_ = fillMode;
    }

    FillMode GetFillMode() const
    {
        return fillMode_;
    }

    void SetOnFinishEvent(const std::function<void()>& onFinishEvent)
    {
        onFinishEvent_ = onFinishEvent;
    }

    const std::function<void()>& GetOnFinishEvent() const
    {
        return onFinishEvent_;
    }

    bool IsValid() const
    {
        return (GetDuration() > 0 || GetAllowRunningAsynchronously());
    }

    void SetAllowRunningAsynchronously(bool runAsync)
    {
        allowRunningAsynchronously_ = runAsync;
    }

    bool GetAllowRunningAsynchronously() const
    {
        return allowRunningAsynchronously_;
    }

private:
    int32_t duration_ = 0;
    int32_t delay_ = 0;
    int32_t iteration_ = 1;
    float tempo_ = 1.0f;
    FillMode fillMode_ = FillMode::FORWARDS;
    bool allowRunningAsynchronously_ = false;

    RefPtr<Curve> curve_;
    std::function<void()> onFinishEvent_;
    AnimationDirection direction_ = AnimationDirection::NORMAL;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATION_OPTION_H
