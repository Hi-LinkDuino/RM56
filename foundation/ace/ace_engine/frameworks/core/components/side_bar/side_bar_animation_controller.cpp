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

#include "core/components/side_bar/side_bar_animation_controller.h"

#include "core/components/side_bar/render_side_bar_container.h"
#include "core/components/side_bar/side_bar_container_component.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t SLIDE_TRANSLATE_DURATION = 400;
constexpr double RATIO_NEGATIVE = -1.0;
constexpr double RATIO_ZERO = 0;

void UpdateSideBarPosition(const WeakPtr<SideBarAnimationController>& weak, double value)
{
    auto animationController = weak.Upgrade();
    if (!animationController) {
        return;
    }

    auto sidebar = animationController->GetRenderSideBarContainer().Upgrade();
    if (!sidebar) {
        return;
    }

    auto renderSideBarContainer = AceType::DynamicCast<RenderSideBarContainer>(sidebar);
    if (!renderSideBarContainer) {
        return;
    }
    auto curValue = value * renderSideBarContainer->GetSidebarWidth();
    renderSideBarContainer->UpdateElementPosition(curValue);
}

}

SideBarAnimation::SideBarAnimation(const WeakPtr<PipelineContext>& context, double start,
                                   double end, int delay,
                                   int duration, const RefPtr<Curve>& curve,
                                   AnimationCallback&& callback)
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

void SideBarAnimation::Play()
{
    if (!controller_ || !animation_) {
        return;
    }
    controller_->ClearInterpolators();
    controller_->AddInterpolator(animation_);
    controller_->Play();
}

void SideBarAnimation::Stop()
{
    if (controller_) {
        controller_->Finish();
    }
}

void SideBarAnimationController::StopAnimation()
{
    StopHideAnimation();
    StopShowAnimation();
}

void SideBarAnimationController::PlaySideBarContainerToAnimation(SideStatus status)
{
    if (!isAnimationCreated_) {
        CreateAniamtion();
    }

    StopHideAnimation();
    if (rightToLeftAnimation_) {
        rightToLeftAnimation_->ClearStopListenerCallback();
    }

    StopShowAnimation();
    if (leftToRightAnimation_) {
        leftToRightAnimation_->ClearStopListenerCallback();
    }
    if (status == SideStatus::HIDDEN) {
        if (rightToLeftAnimation_ && stopCallback_) {
            rightToLeftAnimation_->AddStopListenerCallback(stopCallback_);
            PlayHideAnimation();
        }
    } else {
        if (leftToRightAnimation_ && stopCallback_) {
            leftToRightAnimation_->AddStopListenerCallback(stopCallback_);
            PlayShowAnimation();
        }
    }
}

void SideBarAnimationController::CreateAniamtion()
{
    auto weak = AceType::WeakClaim(this);

    leftToRightAnimation_ = AceType::MakeRefPtr<SideBarAnimation>(context_, RATIO_NEGATIVE,
        RATIO_ZERO, 0, SLIDE_TRANSLATE_DURATION, Curves::FRICTION, [weak](double value) {
            UpdateSideBarPosition(weak, value);
        });

    rightToLeftAnimation_ = AceType::MakeRefPtr<SideBarAnimation>(context_, RATIO_ZERO,
        RATIO_NEGATIVE, 0, SLIDE_TRANSLATE_DURATION, Curves::FRICTION, [weak](double value) {
            UpdateSideBarPosition(weak, value);
        });

    isAnimationCreated_ = true;
}

void SideBarAnimationController::PlayHideAnimation()
{
    if (rightToLeftAnimation_) {
        rightToLeftAnimation_->Play();
    }
}

void SideBarAnimationController::StopHideAnimation()
{
    if (rightToLeftAnimation_) {
        rightToLeftAnimation_->Stop();
    }
}

void SideBarAnimationController::PlayShowAnimation()
{
    if (leftToRightAnimation_) {
        leftToRightAnimation_->Play();
    }
}

void SideBarAnimationController::StopShowAnimation()
{
    if (leftToRightAnimation_) {
        leftToRightAnimation_->Stop();
    }
}

} // namespace OHOS::Ace