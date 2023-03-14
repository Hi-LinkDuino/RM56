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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_BASE_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/common/properties/animation_option.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

// base class for animatable object
template<class T>
class AnimatableBase : public T {
public:
    using RenderNodeAnimationCallback = std::function<void()>;

    AnimatableBase() = default;
    virtual ~AnimatableBase() = default;
    virtual void MoveTo(const T& endValue) = 0;

    void AnimateTo(const T& beginValue, const T& endValue)
    {
        if (endValue == endValue_) {
            return;
        }
        endValue_ = endValue;
        if (!animationOption_.IsValid()) {
            MoveTo(endValue);
            return;
        }
        ResetController();
        if (!animationController_) {
            animationController_ = AceType::MakeRefPtr<Animator>(context_);
        }
        // create animation
        const auto& animation = CreateAnimation(beginValue, endValue);
        if (animation) {
            animationController_->AddInterpolator(animation);
        }
        // apply animation options
        ApplyAnimationOptions();
    }

    void SetEvaluator(const RefPtr<Evaluator<T>>& evaluator)
    {
        evaluator_ = evaluator;
    }

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        context_ = context;
        animationCallback_ = callback;
    }

    void SetAnimationStopCallback(RenderNodeAnimationCallback&& callback)
    {
        stopCallback_ = std::move(callback);
    }

    const AnimationOption& GetAnimationOption() const
    {
        return animationOption_;
    }

    void SetAnimationOption(const AnimationOption& option)
    {
        animationOption_ = option;
    }

    Animator::Status GetAnimationStatus() const
    {
        if (!animationController_) {
            return Animator::Status::IDLE;
        }
        return animationController_->GetStatus();
    }

protected:
    virtual void OnAnimationCallback(const T& value)
    {
        if (animationCallback_) {
            animationCallback_();
        }
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

    RefPtr<Animation<T>> CreateAnimation(const T& begin, const T& end)
    {
        RefPtr<CurveAnimation<T>> animation =
            AceType::MakeRefPtr<CurveAnimation<T>>(begin, end, animationOption_.GetCurve());
        animation->AddListener(std::bind(&AnimatableBase::OnAnimationCallback, this, std::placeholders::_1));
        if (evaluator_) {
            animation->SetEvaluator(evaluator_);
        }
        return animation;
    }

    void ApplyAnimationOptions()
    {
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

protected:
    AnimationOption animationOption_;
    RefPtr<Animator> animationController_;
    WeakPtr<PipelineContext> context_;
    RenderNodeAnimationCallback animationCallback_;
    RenderNodeAnimationCallback stopCallback_;
    RefPtr<Evaluator<T>> evaluator_;
    T endValue_ {};
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_BASE_H
