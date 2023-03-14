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

#include "base/geometry/animatable_dimension.h"

#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
AnimatableDimension& AnimatableDimension::operator=(const Dimension& newDimension)
{
    ResetAnimatableDimension();
    Dimension& dimension = *this;
    dimension = newDimension;
    return *this;
}

AnimatableDimension& AnimatableDimension::operator=(const CalcDimension& newDimension)
{
    ResetAnimatableDimension();
    CalcDimension& dimension = *this;
    dimension = newDimension;
    return *this;
}

AnimatableDimension& AnimatableDimension::operator=(const AnimatableDimension& newDimension)
{
    SetUnit(newDimension.Unit());
    SetAnimationOption(newDimension.GetAnimationOption());
    auto pipelineContext = context_.Upgrade();
    if (!animationCallback_ || !pipelineContext) {
        if (newDimension.Unit() == DimensionUnit::CALC) {
            SetCalcValue(newDimension.CalcValue());
        } else {
            SetValue(newDimension.Value());
        }
        return *this;
    }
    AnimationOption explicitAnim = pipelineContext->GetExplicitAnimationOption();
    if (explicitAnim.IsValid()) {
        SetAnimationOption(explicitAnim);
        AnimateTo(newDimension.Value());
    } else if (animationOption_.IsValid()) {
        AnimateTo(newDimension.Value());
    } else {
        ResetController();
        if (newDimension.Unit() == DimensionUnit::CALC) {
            SetCalcValue(newDimension.CalcValue());
        } else {
            SetValue(newDimension.Value());
        }
    }
    isFirstAssign_ = false;
    return *this;
}

void AnimatableDimension::AnimateTo(double endValue)
{
    if (isFirstAssign_) {
        LOGD("AnimateTo with first assign. endValue: %{public}.2f", endValue);
        isFirstAssign_ = false;
        SetValue(endValue);
        return;
    }
    if (NearEqual(Value(), endValue) && !evaluator_) {
        return;
    }
    ResetController();
    if (!animationController_) {
        animationController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    RefPtr<CurveAnimation<double>> animation =
        AceType::MakeRefPtr<CurveAnimation<double>>(Value(), endValue, animationOption_.GetCurve());
    if (evaluator_) {
        animation->SetEvaluator(evaluator_);
    }
    animation->AddListener(std::bind(&AnimatableDimension::OnAnimationCallback, this, std::placeholders::_1));

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

void AnimatableDimension::ResetController()
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

void AnimatableDimension::OnAnimationCallback(double value)
{
    SetValue(value);
    if (animationCallback_) {
        animationCallback_();
    }
}

void AnimatableDimension::MoveTo(double target)
{
    SetValue(target);
    isFirstAssign_ = false;
}

void AnimatableDimension::ResetAnimatableDimension()
{
    isFirstAssign_ = true;
    animationOption_ = AnimationOption();
    animationController_ = nullptr;
    context_ = nullptr;
    animationCallback_ = nullptr;
}
} // namespace OHOS::Ace