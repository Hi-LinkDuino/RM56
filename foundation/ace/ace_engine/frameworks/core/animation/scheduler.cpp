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

#include "core/animation/scheduler.h"

#include "base/log/log.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

void Scheduler::Start()
{
    if (isRunning_) {
        LOGW("Already running, no need to start again.");
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Start failed, context is null.");
        return;
    }
    isRunning_ = true;
    startupTimestamp_ = context->GetTimeFromExternalTimer();
    scheduleId_ = static_cast<int32_t>(context->AddScheduleTask(AceType::Claim(this)));
}

void Scheduler::Stop()
{
    if (!isRunning_) {
        LOGD("Already stopped, no need to stop again.");
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Stop failed, context is null.");
        return;
    }
    isRunning_ = false;
    context->RemoveScheduleTask(scheduleId_);
    scheduleId_ = 0;
}

void Scheduler::OnFrame(uint64_t nanoTimestamp)
{
    if (!isRunning_) {
        LOGD("Already stopped, no need to send frame event.");
        return;
    }

    // Refresh the startup time every frame.
    uint64_t elapsedTimeMs = (nanoTimestamp - startupTimestamp_) / 1000000;
    startupTimestamp_ += elapsedTimeMs * 1000000;

    // Consume previous schedule as default.
    scheduleId_ = 0;
    if (callback_) {
        // Need to convert nanoseconds to milliseconds
        callback_(elapsedTimeMs);
    }

    // Schedule next frame task.
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Schedule next frame task failed, context is null.");
        return;
    }
    if (IsActive()) {
        scheduleId_ = static_cast<int32_t>(context->AddScheduleTask(AceType::Claim(this)));
    }
}

bool Scheduler::Animate(const AnimationOption& option, const RefPtr<Curve>& curve,
    const std::function<void()> propertyCallback, const std::function<void()>& finishCallBack)
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGE("Failed to animate asynchronously, context is null!");
        return false;
    }

    return context->Animate(option, curve, propertyCallback, finishCallBack);
}

void Scheduler::OpenImplicitAnimation(const AnimationOption& option, const RefPtr<Curve>& curve,
    const std::function<void()>& finishCallBack)
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGE("Failed to open implicit animation, context is null!");
        return;
    }

    return context->OpenImplicitAnimation(option, curve, finishCallBack);
}

bool Scheduler::CloseImplicitAnimation()
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGE("Failed to close implicit animation, context is null!");
        return false;
    }

    return context->CloseImplicitAnimation();
}

void Scheduler::AddKeyFrame(
    float fraction, const RefPtr<Curve>& curve, const std::function<void()>& propertyCallback)
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGE("Failed to add keyframe, context is null!");
        return;
    }

    return context->AddKeyFrame(fraction, curve, propertyCallback);
}

void Scheduler::AddKeyFrame(float fraction, const std::function<void()>& propertyCallback)
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGE("Failed to add keyframe, context is null!");
        return;
    }

    return context->AddKeyFrame(fraction, propertyCallback);
}

} // namespace OHOS::Ace
