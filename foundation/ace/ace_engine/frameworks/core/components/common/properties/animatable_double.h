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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_DOUBLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_DOUBLE_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/common/properties/animation_option.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

using RenderNodeAnimationCallback = std::function<void()>;

/*
 * AnimatableDouble is a double with AnimationOption and Animator.
 */
class AnimatableDouble final {
public:
    AnimatableDouble() = default;
    explicit AnimatableDouble(double value, const AnimationOption& option = AnimationOption())
        : value_(value), animationOption_(option)
    {}
    ~AnimatableDouble() = default;

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        context_ = context;
        animationCallback_ = callback;
    }

    void SetAnimationStopCallback(const RenderNodeAnimationCallback& callback)
    {
        stopCallback_ = callback;
    }

    double GetValue() const
    {
        return value_;
    }

    void SetValue(double value)
    {
        value_ = value;
    }

    const AnimationOption& GetAnimationOption() const
    {
        return animationOption_;
    }

    void SetAnimationOption(const AnimationOption& option)
    {
        animationOption_ = option;
    }

    AnimatableDouble& operator=(double newValue)
    {
        ResetAnimatableDouble();
        value_ = newValue;
        return *this;
    }

    AnimatableDouble& operator=(const AnimatableDouble& newValue)
    {
        SetAnimationOption(newValue.GetAnimationOption());
        auto context = context_.Upgrade();
        if (!context || !animationCallback_) {
            SetValue(newValue.GetValue());
            return *this;
        }
        AnimationOption explicitAnim = context->GetExplicitAnimationOption();
        if (explicitAnim.IsValid()) {
            SetAnimationOption(explicitAnim);
            AnimateTo(newValue.GetValue());
        } else if (animationOption_.IsValid()) {
            AnimateTo(newValue.GetValue());
        } else {
            ResetController();
            SetValue(newValue.GetValue());
        }
        isFirstAssign_ = false;
        return *this;
    }

    void MoveTo(double target)
    {
        SetValue(target);
        isFirstAssign_ = false;
    }

    Animator::Status GetAnimationStatus() const
    {
        if (!animationController_) {
            return Animator::Status::IDLE;
        }
        return animationController_->GetStatus();
    }

private:
    void AnimateTo(double endValue)
    {
        if (isFirstAssign_) {
            LOGD("AnimateTo with first assign. endValue: %{public}.2f", endValue);
            isFirstAssign_ = false;
            SetValue(endValue);
            return;
        }
        if (NearEqual(value_, endValue)) {
            LOGD("AnimateTo with same value. endValue: %{public}.2f", endValue);
            return;
        }
        ResetController();
        if (!animationController_) {
            animationController_ = AceType::MakeRefPtr<Animator>(context_);
        }
        RefPtr<CurveAnimation<double>> animation =
            AceType::MakeRefPtr<CurveAnimation<double>>(value_, endValue, animationOption_.GetCurve());
        animation->AddListener(std::bind(&AnimatableDouble::OnAnimationCallback, this, std::placeholders::_1));

        animationController_->AddInterpolator(animation);
        auto onFinishEvent = animationOption_.GetOnFinishEvent();
        if (onFinishEvent) {
            animationController_->AddStopListener([onFinishEvent, weakContext = context_] {
                auto context = weakContext.Upgrade();
                if (context) {
                    context->PostAsyncEvent(onFinishEvent);
                } else {
                    LOGE("the context is null");
                }
            });
        }
        if (stopCallback_) {
            animationController_->AddStopListener(stopCallback_);
        }
        animationController_->SetDuration(animationOption_.GetDuration());
        animationController_->SetStartDelay(animationOption_.GetDelay());
        animationController_->SetIteration(animationOption_.GetIteration());
        animationController_->SetTempo(animationOption_.GetTempo());
        animationController_->SetAnimationDirection(animationOption_.GetAnimationDirection());
        animationController_->SetFillMode(FillMode::FORWARDS);
        animationController_->SetAllowRunningAsynchronously(animationOption_.GetAllowRunningAsynchronously());
        animationController_->Play();
    }

    void ResetController()
    {
        if (animationController_) {
            if (!animationController_->IsStopped()) {
                animationController_->Stop();
            }
            animationController_->ClearInterpolators();
            animationController_->ClearAllListeners();
            animationController_.Reset();
        }
    }

    void OnAnimationCallback(const double& value)
    {
        SetValue(value);
        if (animationCallback_) {
            animationCallback_();
        }
    }

    void ResetAnimatableDouble()
    {
        isFirstAssign_ = true;
        animationOption_ = AnimationOption();
        animationController_ = nullptr;
        context_ = nullptr;
        animationCallback_ = nullptr;
        stopCallback_ = nullptr;
    }

private:
    double value_;
    bool isFirstAssign_ = true;
    AnimationOption animationOption_;
    RefPtr<Animator> animationController_;
    WeakPtr<PipelineContext> context_;
    RenderNodeAnimationCallback animationCallback_;
    RenderNodeAnimationCallback stopCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_DOUBLE_H