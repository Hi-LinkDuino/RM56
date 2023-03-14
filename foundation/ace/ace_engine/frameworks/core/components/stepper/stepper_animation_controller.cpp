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

#include "core/components/stepper/stepper_animation_controller.h"

#include "core/components/stepper/render_stepper.h"

namespace OHOS::Ace {
namespace {

constexpr double MAX_OPACITY = 255.0;
constexpr double MIN_OPACITY = 0.0;
constexpr int32_t FROM_INDEX_OPACITY_DURATION = 400;
constexpr int32_t TO_INDEX_OPACITY_DURATION = 700;

constexpr int32_t TRANSLATE_DURATION = 700;
constexpr double RATIO_NEGATIVE = -0.5;
constexpr double RATIO_ZERO = 0;
constexpr double RATIO_POSITIVE = 0.5;

void UpdateStepperItemOpacity(const WeakPtr<StepperAnimationController>& weak, double value, bool isFromIndex)
{
    auto animationController = weak.Upgrade();
    if (!animationController) {
        return;
    }

    auto stepper = animationController->GetRenderStepper().Upgrade();
    if (!stepper) {
        return;
    }

    auto renderStepper = AceType::DynamicCast<RenderStepper>(stepper);
    if (!renderStepper) {
        return;
    }

    auto curValue = (uint8_t)value;
    if (isFromIndex) {
        renderStepper->UpdateItemOpacity(curValue, renderStepper->GetFromIndex());
    } else {
        renderStepper->UpdateItemOpacity(curValue, renderStepper->GetToIndex());
    }
}

void UpdateStepperItemPosition(const WeakPtr<StepperAnimationController>& weak, double value, bool isFromIndex)
{
    auto animationController = weak.Upgrade();
    if (!animationController) {
        return;
    }

    auto stepper = animationController->GetRenderStepper().Upgrade();
    if (!stepper) {
        return;
    }

    auto renderStepper = AceType::DynamicCast<RenderStepper>(stepper);
    if (!renderStepper) {
        return;
    }

    auto curValue = value * renderStepper->GetStepperWidth();
    if (isFromIndex) {
        renderStepper->UpdateItemPosition(curValue, renderStepper->GetFromIndex());
    } else {
        renderStepper->UpdateItemPosition(curValue, renderStepper->GetToIndex());
    }
}

} // namespace

StepperAnimation::StepperAnimation(const WeakPtr<PipelineContext>& context, double start, double end, int delay,
    int duration, const RefPtr<Curve>& curve, const AnimationCallback& callback)
    : start_(start), end_(end), delay_(delay), duration_(duration)
{
    curve_ = curve;
    callback_ = callback;
    context_ = context;
    animation_ = AceType::MakeRefPtr<CurveAnimation<double>>(start_, end_, curve_);
    animation_->AddListener(Animation<double>::ValueCallback(callback_));
    controller_ = AceType::MakeRefPtr<Animator>(context_);
    controller_->SetDuration(duration_);
    controller_->SetStartDelay(delay_);
}

void StepperAnimation::Play()
{
    if (!controller_ || !animation_) {
        return;
    }
    controller_->ClearInterpolators();
    controller_->AddInterpolator(animation_);
    controller_->Play();
}

void StepperAnimation::Stop()
{
    if (controller_) {
        controller_->Finish();
    }
}

void StepperAnimationController::CreateAnimation()
{
    CreateOpacityAnimation();
    CreateTranslateAnimation();
}

void StepperAnimationController::CreateOpacityAnimation()
{
    auto weak = AceType::WeakClaim(this);
    fromIndexOpacityAnimation_ = AceType::MakeRefPtr<StepperAnimation>(context_, MAX_OPACITY, MIN_OPACITY,
        0, FROM_INDEX_OPACITY_DURATION, Curves::FRICTION, [weak](double value) {
            UpdateStepperItemOpacity(weak, value, true);
        });

    toIndexOpacityAnimation_ = AceType::MakeRefPtr<StepperAnimation>(context_, MIN_OPACITY, MAX_OPACITY,
        0, TO_INDEX_OPACITY_DURATION, Curves::SMOOTH, [weak](double value) {
            UpdateStepperItemOpacity(weak, value, false);
        });
}

void StepperAnimationController::CreateTranslateAnimation()
{
    auto weak = AceType::WeakClaim(this);
    fromIndexTranslateAnimationNext_ = AceType::MakeRefPtr<StepperAnimation>(context_, RATIO_ZERO,
        RATIO_NEGATIVE, 0, TRANSLATE_DURATION, Curves::FRICTION, [weak](double value) {
            UpdateStepperItemPosition(weak, value, true);
        });

    toIndexTranslateAnimationNext_ = AceType::MakeRefPtr<StepperAnimation>(context_, RATIO_POSITIVE,
        RATIO_ZERO, 0, TRANSLATE_DURATION, Curves::FAST_OUT_SLOW_IN, [weak](double value) {
            UpdateStepperItemPosition(weak, value, false);
        });

    fromIndexTranslateAnimationBack_ = AceType::MakeRefPtr<StepperAnimation>(context_, RATIO_ZERO,
        RATIO_POSITIVE, 0, TRANSLATE_DURATION, Curves::FRICTION, [weak](double value) {
            UpdateStepperItemPosition(weak, value, true);
        });

    toIndexTranslateAnimationBack_ = AceType::MakeRefPtr<StepperAnimation>(context_, RATIO_NEGATIVE,
        RATIO_ZERO, 0, TRANSLATE_DURATION, Curves::FAST_OUT_SLOW_IN, [weak](double value) {
            UpdateStepperItemPosition(weak, value, false);
        });
}

void StepperAnimationController::PlayNextAnimation()
{
    if (fromIndexOpacityAnimation_) {
        fromIndexOpacityAnimation_->Play();
    }
    if (toIndexOpacityAnimation_) {
        toIndexOpacityAnimation_->Play();
    }
    if (fromIndexTranslateAnimationNext_) {
        fromIndexTranslateAnimationNext_->Play();
    }
    if (toIndexTranslateAnimationNext_) {
        toIndexTranslateAnimationNext_->Play();
    }
}

void StepperAnimationController::StopNextAnimation()
{
    if (fromIndexOpacityAnimation_) {
        fromIndexOpacityAnimation_->Stop();
    }
    if (toIndexOpacityAnimation_) {
        toIndexOpacityAnimation_->Stop();
    }
    if (fromIndexTranslateAnimationNext_) {
        fromIndexTranslateAnimationNext_->Stop();
    }
    if (toIndexTranslateAnimationNext_) {
        toIndexTranslateAnimationNext_->Stop();
    }
}

void StepperAnimationController::PlayBackAnimation()
{
    if (fromIndexOpacityAnimation_) {
        fromIndexOpacityAnimation_->Play();
    }
    if (toIndexOpacityAnimation_) {
        toIndexOpacityAnimation_->Play();
    }
    if (fromIndexTranslateAnimationBack_) {
        fromIndexTranslateAnimationBack_->Play();
    }
    if (toIndexTranslateAnimationBack_) {
        toIndexTranslateAnimationBack_->Play();
    }
}

void StepperAnimationController::StopBackAnimation()
{
    if (fromIndexOpacityAnimation_) {
        fromIndexOpacityAnimation_->Stop();
    }
    if (toIndexOpacityAnimation_) {
        toIndexOpacityAnimation_->Stop();
    }
    if (fromIndexTranslateAnimationBack_) {
        fromIndexTranslateAnimationBack_->Stop();
    }
    if (toIndexTranslateAnimationBack_) {
        toIndexTranslateAnimationBack_->Stop();
    }
}

void StepperAnimationController::PlayStepperToAnimation(int32_t fromIndex, int32_t toIndex, bool reverse)
{
    if (!isAnimationCreated_) {
        CreateAnimation();
        isAnimationCreated_ = true;
    }

    StopNextAnimation();
    if (toIndexTranslateAnimationNext_) {
        toIndexTranslateAnimationNext_->ClearStopListenerCallback();
    }

    StopBackAnimation();
    if (toIndexTranslateAnimationBack_) {
        toIndexTranslateAnimationBack_->ClearStopListenerCallback();
    }

    if (toIndex > fromIndex) {
        if (toIndexTranslateAnimationNext_ && animationStopCallback_) {
            toIndexTranslateAnimationNext_->AddStopListenerCallback(animationStopCallback_);
        }
        PlayNextAnimation();
    } else {
        if (toIndexTranslateAnimationBack_ && animationStopCallback_) {
            toIndexTranslateAnimationBack_->AddStopListenerCallback(animationStopCallback_);
        }
        PlayBackAnimation();
    }
}

} // namespace OHOS::Ace
