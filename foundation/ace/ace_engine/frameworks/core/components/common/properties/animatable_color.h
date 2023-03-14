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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_COLOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_COLOR_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/common/properties/animation_option.h"
#include "core/components/common/properties/color.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

using RenderNodeAnimationCallback = std::function<void()>;

/*
 * AnimatableColor is a Color with AnimationOption and Animator.
 */
class AnimatableColor final : public Color {
public:
    AnimatableColor() = default;
    explicit AnimatableColor(uint32_t value, const AnimationOption& option = AnimationOption()) : Color(value)
    {
        animationOption_ = option;
    }

    explicit AnimatableColor(const Color& color, const AnimationOption& option = AnimationOption())
        : Color(color.GetValue())
    {
        animationOption_ = option;
    }

    ~AnimatableColor() = default;

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        context_ = context;
        animationCallback_ = callback;
    }

    const AnimationOption& GetAnimationOption() const
    {
        return animationOption_;
    }

    void SetAnimationOption(const AnimationOption& option)
    {
        animationOption_ = option;
    }

    AnimatableColor& operator=(const AnimatableColor& newColor)
    {
        SetAnimationOption(newColor.GetAnimationOption());
        auto pipelineContext = context_.Upgrade();
        if (!pipelineContext || !animationCallback_) {
            SetValue(newColor.GetValue());
            return *this;
        }
        AnimationOption explicitAnim = pipelineContext->GetExplicitAnimationOption();
        if (explicitAnim.IsValid()) {
            SetAnimationOption(explicitAnim);
            AnimateTo(newColor.GetValue());
        } else if (animationOption_.IsValid()) {
            AnimateTo(newColor.GetValue());
        } else {
            ResetController();
            SetValue(newColor.GetValue());
        }
        isFirstAssign_ = false;
        return *this;
    }

private:
    void AnimateTo(uint32_t endValue)
    {
        if (isFirstAssign_) {
            isFirstAssign_ = false;
            SetValue(endValue);
            return;
        }
        if (GetValue() == endValue) {
            return;
        }
        ResetController();
        if (!animationController_) {
            animationController_ = AceType::MakeRefPtr<Animator>(context_);
        }

        RefPtr<CurveAnimation<Color>> colorAnimation =
            AceType::MakeRefPtr<CurveAnimation<Color>>(Color(GetValue()), Color(endValue), animationOption_.GetCurve());
        colorAnimation->AddListener(std::bind(&AnimatableColor::OnAnimationCallback, this, std::placeholders::_1));

        animationController_->AddInterpolator(colorAnimation);
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

    void OnAnimationCallback(const Color& color)
    {
        SetValue(color.GetValue());
        if (animationCallback_) {
            animationCallback_();
        }
    }

private:
    bool isFirstAssign_ = true;
    AnimationOption animationOption_;
    RefPtr<Animator> animationController_;
    WeakPtr<PipelineContext> context_;
    RenderNodeAnimationCallback animationCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_COLOR_H
