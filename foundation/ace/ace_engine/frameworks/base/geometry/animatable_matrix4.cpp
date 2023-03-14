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

#include "base/geometry/animatable_matrix4.h"

#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

AnimatableMatrix4& AnimatableMatrix4::operator=(const Matrix4& newMatrix4)
{
    ResetAnimatableMatrix();
    Matrix4& matrix4 = *this;
    matrix4 = newMatrix4;
    return *this;
}

AnimatableMatrix4& AnimatableMatrix4::operator=(const AnimatableMatrix4& newMatrix4)
{
    SetAnimationOption(newMatrix4.GetAnimationOption());
    auto pipelineContext = context_.Upgrade();
    if (!animationCallback_ || !pipelineContext) {
        Matrix4& matrix4 = *this;
        matrix4 = newMatrix4;
        return *this;
    }
    AnimationOption explicitAnim = pipelineContext->GetExplicitAnimationOption();
    if (explicitAnim.IsValid()) {
        SetAnimationOption(explicitAnim);
        AnimateTo(newMatrix4);
    } else if (animationOption_.IsValid()) {
        AnimateTo(newMatrix4);
    } else {
        ResetController();
        Matrix4& matrix4 = *this;
        matrix4 = newMatrix4;
    }
    isFirstAssign_ = false;
    return *this;
}

void AnimatableMatrix4::MoveTo(const Matrix4& target)
{
    Matrix4& matrix4 = *this;
    matrix4 = target;
    isFirstAssign_ = false;
}

void AnimatableMatrix4::AnimateTo(const Matrix4& endValue)
{
    if (isFirstAssign_) {
        LOGD("AnimateTo with first assign. endValue: %{public}s", endValue.ToString().c_str());
        isFirstAssign_ = false;
        Matrix4& matrix4 = *this;
        matrix4 = endValue;
        return;
    }

    if (*this == endValue && !evaluator_) {
        return;
    }
    ResetController();
    if (!animationController_) {
        animationController_ = AceType::MakeRefPtr<Animator>(context_);
    }

    TransformOperation operationInit;
    TransformOperation operationEnd;
    operationInit.type_ = TransformOperationType::MATRIX;
    operationEnd.type_ = TransformOperationType::MATRIX;
    operationInit.matrix4_ = static_cast<Matrix4>(*this);
    operationEnd.matrix4_ = endValue;
    RefPtr<CurveAnimation<TransformOperation>> animation = AceType::MakeRefPtr<CurveAnimation<TransformOperation>>(
        operationInit, operationEnd, animationOption_.GetCurve());
    if (evaluator_) {
        animation->SetEvaluator(evaluator_);
    }
    animation->AddListener(std::bind(&AnimatableMatrix4::OnAnimationCallback, this, std::placeholders::_1));
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

void AnimatableMatrix4::ResetController()
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

void AnimatableMatrix4::OnAnimationCallback(const TransformOperation& value)
{
    Matrix4& matrix4 = *this;
    matrix4 = value.matrix4_;
    if (animationCallback_) {
        animationCallback_();
    }
}

void AnimatableMatrix4::ResetAnimatableMatrix()
{
    isFirstAssign_ = true;
    animationOption_ = AnimationOption();
    animationController_ = nullptr;
    context_ = nullptr;
    animationCallback_ = nullptr;
}

} // namespace OHOS::Ace
