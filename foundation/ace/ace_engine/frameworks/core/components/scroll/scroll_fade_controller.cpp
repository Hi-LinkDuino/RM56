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

#include "core/components/scroll/scroll_fade_controller.h"

#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t RECEDE_TIME = 600;       // milliseconds
constexpr int32_t PULL_TIME = 167;         // milliseconds
constexpr int32_t PULL_HOLD_TIME = 167;    // milliseconds
constexpr int32_t PULL_RECEDE_TIME = 1000; // milliseconds
constexpr double MIN_VELOCITY = 100.0;
constexpr double MAX_VELOCITY = 10000.0;
constexpr double MIN_OPACITY = 0.15;
constexpr double MAX_OPACITY = 0.5;
constexpr double DEFAULT_OPACITY = 0.3;
constexpr double MAX_SCALE_SIZE = 1.0;
constexpr double VELOCITY_FACTOR = 0.00006;
constexpr double WIDTH_TO_HEIGHT_FACTOR = 0.20096; // (3.0 / 4.0) * (2.0 - std::sqrt(3));
constexpr double SCALE_SIZE_BASE = 0.025;
constexpr double SCALE_VELOCITY_FACTOR = 7.5e-7;
constexpr double DURATION_BASE = 0.15;
constexpr double DURATION_VELOCITY_FACTOR = 0.02;
constexpr double OVER_DISTANCE_SCALE = 200.0;
constexpr double PULL_OPACITY_FACTOR = 3.2;
constexpr double SCALE_CALCULATOR_FACTOR = 0.7;

} // namespace

ScrollFadeController::ScrollFadeController(const WeakPtr<PipelineContext>& context) : context_(context)
{
    Initialize();
}

ScrollFadeController::~ScrollFadeController()
{
    // If animation still runs, force stop it.
    if (controller_ && !controller_->IsStopped()) {
        controller_->Stop();
    }
    pullTask_.Cancel();
}

void ScrollFadeController::Initialize()
{
    decele_ = AceType::MakeRefPtr<CurveAnimation<double>>(0.0, 1.0, Curves::DECELE);
    auto weak = AceType::WeakClaim(this);
    decele_->AddListener([weak](double value) {
        auto controller = weak.Upgrade();
        if (controller) {
            controller->DecelerateListener(value);
        }
    });

    controller_ = AceType::MakeRefPtr<Animator>(context_);
    controller_->AddInterpolator(decele_);
    controller_->AddStopListener([weak]() {
        auto controller = weak.Upgrade();
        if (controller) {
            controller->ChangeState();
        }
    });
}

void ScrollFadeController::DecelerateListener(double value)
{
    opacity_ = opacityFloor_ + value * (opacityCeil_ - opacityFloor_);
    scaleSize_ = scaleSizeFloor_ + value * (scaleSizeCeil_ - scaleSizeFloor_);
    if (callback_) {
        callback_(opacity_, scaleSize_);
    }
}

void ScrollFadeController::ProcessAbsorb(double velocity)
{
    LOGD("ProcessAbsorb enter: velocity(%{public}lf)", velocity);
    if (velocity < 0.0 || state_ == OverScrollState::PULL) {
        return;
    }
    pullTask_.Cancel();
    velocity = std::clamp(velocity, MIN_VELOCITY, MAX_VELOCITY);
    opacityFloor_ = state_ == OverScrollState::IDLE ? DEFAULT_OPACITY : opacity_;
    opacityCeil_ = std::clamp(velocity * VELOCITY_FACTOR, opacityFloor_, MAX_OPACITY);
    scaleSizeFloor_ = scaleSize_;
    scaleSizeCeil_ = std::min(SCALE_SIZE_BASE + SCALE_VELOCITY_FACTOR * velocity * velocity, MAX_SCALE_SIZE);
    if (controller_) {
        controller_->SetDuration(std::round(DURATION_BASE + velocity * DURATION_VELOCITY_FACTOR));
        controller_->Play();
        state_ = OverScrollState::ABSORB;
    }
}

void ScrollFadeController::ProcessPull(double overDistance, double mainAxisExtent, double crossAxisExtent)
{
    LOGD("ProcessPull enter:overDistance(%{public}lf), mainAxisExtent(%{public}lf), crossAxisExtent(%{public}lf)",
        overDistance, mainAxisExtent, crossAxisExtent);
    pullTask_.Cancel();
    opacityFloor_ = std::min(MIN_OPACITY + opacity_, DEFAULT_OPACITY);
    opacityCeil_ = NearZero(mainAxisExtent)
                       ? MAX_OPACITY
                       : std::min(opacity_ + overDistance / mainAxisExtent * PULL_OPACITY_FACTOR, MAX_OPACITY);
    double height = std::min(mainAxisExtent, crossAxisExtent * WIDTH_TO_HEIGHT_FACTOR);
    pullDistance_ += overDistance / OVER_DISTANCE_SCALE;
    scaleSizeFloor_ = scaleSize_;
    double scaleByPullDistance = SCALE_CALCULATOR_FACTOR * std::sqrt(pullDistance_ * height);
    scaleSizeCeil_ = NearZero(scaleSize_) ? MAX_SCALE_SIZE
                                          : std::max(MAX_SCALE_SIZE - MAX_SCALE_SIZE / scaleByPullDistance, scaleSize_);
    LOGD("opacityFloor_(%{public}lf), opacityCeil_(%{public}lf)", opacityFloor_, opacityCeil_);
    LOGD("scaleSizeFloor_(%{public}lf), scaleSizeCeil_(%{public}lf)", scaleSizeFloor_, scaleSizeCeil_);
    if (controller_) {
        controller_->SetDuration(PULL_TIME);
        if (state_ != OverScrollState::PULL) {
            controller_->Play();
            state_ = OverScrollState::PULL;
        } else {
            controller_->Stop();
            controller_->Play();
        }
    }

    SchedulePullHoldTask();
}

void ScrollFadeController::ProcessRecede(int32_t duration)
{
    LOGD("ProcessRecede enter:duration(%{public}d)", duration);
    if (state_ == OverScrollState::RECEDE || state_ == OverScrollState::IDLE) {
        LOGD("current state do not support recede:state_(%{public}d)", state_);
        return;
    }
    pullTask_.Cancel();
    opacityFloor_ = opacity_;
    opacityCeil_ = 0.0;
    scaleSizeFloor_ = scaleSize_;
    scaleSizeCeil_ = 0.0;
    if (controller_) {
        controller_->SetDuration(duration);
        controller_->Play();
        state_ = OverScrollState::RECEDE;
    }
}

void ScrollFadeController::ChangeState()
{
    LOGD("ChangeState enter: old(%{public}d)", state_);
    switch (state_) {
        case OverScrollState::RECEDE:
            state_ = OverScrollState::IDLE;
            pullDistance_ = 0.0;
            break;
        case OverScrollState::ABSORB:
            ProcessRecede(RECEDE_TIME);
            break;
        case OverScrollState::PULL:
        case OverScrollState::IDLE:
            break;
        default:
            break;
    }
}

void ScrollFadeController::SchedulePullHoldTask()
{
    const auto context = context_.Upgrade();
    if (!context) {
        LOGW("No context exists.");
        return;
    }

    if (!context->GetTaskExecutor()) {
        LOGW("context has no task executor.");
        return;
    }

    pullTask_.Reset([weak = WeakClaim(this)] {
        auto client = weak.Upgrade();
        if (client) {
            client->ProcessRecede(PULL_RECEDE_TIME);
        }
    });
    context->GetTaskExecutor()->PostDelayedTask(pullTask_, TaskExecutor::TaskType::UI, PULL_HOLD_TIME);
}

} // namespace OHOS::Ace