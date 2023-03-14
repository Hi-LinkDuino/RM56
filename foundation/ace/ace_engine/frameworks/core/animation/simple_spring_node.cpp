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

#include "core/animation/simple_spring_node.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t FRAME_SCHEDULED = 16;
constexpr int64_t NANOS_TO_MILLS = 1000000;

} // namespace

SimpleSpringNode::SimpleSpringNode(const WeakPtr<PipelineContext>& context, int32_t index, double value)
    : SpringNode(index), context_(context), value_(value)
{
    springProperty_ = AceType::MakeRefPtr<SpringProperty>();
    spring_ = AceType::MakeRefPtr<SpringMotion>(0.0, 0.0, 0.0, springProperty_);
    spring_->SetAccuracy(valueAccuracy_);
    auto&& callback = [weak = AceType::WeakClaim(this)](uint64_t duration) {
        auto node = weak.Upgrade();
        if (!node || !node->adapter_) {
            LOGE("Empty node or adapter, skip tick callback.");
            return;
        }
        auto context = node->context_.Upgrade();
        if (!context) {
            return;
        }
        node->adapter_->TickAnimation(context->GetTimeFromExternalTimer());
    };
    scheduler_ = AceType::MakeRefPtr<Scheduler>(callback, context);
}

void SimpleSpringNode::SetValue(double value)
{
    SpringNode::SetValue(value);
    value_ = value;
    OnUpdate(value_, velocity_);
    NotifyUpdateListener(value_, velocity_);
    NotifyNext(value_, velocity_);
}

void SimpleSpringNode::NotifyNext(double endValue, double initVelocity)
{
    if (!adapter_) {
        LOGE("Notify next failed. Adapter is null. index: %{public}d", index_);
        return;
    }
    if (this != AceType::RawPtr(adapter_->GetControlNode())) {
        return;
    }
    auto currentNode = adapter_->GetNext(AceType::WeakClaim(this).Upgrade());
    while (currentNode) {
        currentNode->EndToValue(endValue, initVelocity);
        currentNode = adapter_->GetNext(currentNode);
    }
}

void SimpleSpringNode::EndToValue(double endValue, double velocity)
{
    if (!adapter_) {
        LOGE("End to value failed. adapter is null. index: %{public}d", index_);
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("End to value failed, context is null. index: %{public}d", index_);
        return;
    }
    SpringNode::EndToValue(endValue, velocity);
    if (isRunning_) {
        LOGD("EndToValue when running. index: %{public}d, endValue: %{public}.3lf, velocity: %{public}.3lf", index_,
            endValue, velocity);
        auto controlNode = adapter_->GetControlNode();
        if (!controlNode) {
            LOGE("End to value failed. control node is null. index: %{public}d", index_);
            return;
        }
        if (controlNode->IsAnimateToEnd()) {
            startTime_ = context->GetTimeFromExternalTimer() - static_cast<uint64_t>(FRAME_SCHEDULED);
        } else {
            startTime_ = context->GetTimeFromExternalTimer() - static_cast<uint64_t>(GetFrameDelta() * FRAME_SCHEDULED);
        }
        spring_->SetAccuracy(valueAccuracy_);
        spring_->Reset(value_, endValue, velocity_, springProperty_);
    } else {
        LOGD("EndToValue first time. index: %{public}d, endValue: %{public}.3lf, velocity: %{public}.3lf", index_,
            endValue, velocity);
        startTime_ = context->GetTimeFromExternalTimer();
        isRunning_ = true;
        spring_->SetAccuracy(valueAccuracy_);
        spring_->Reset(value_, endValue, velocity_, springProperty_);
        OnAnimation();
    }
    NotifyNext(endValue, velocity);
}

void SimpleSpringNode::OnAnimation()
{
    if (!isRunning_) {
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Animate failed, context is null. index: %{public}d", index_);
        return;
    }
    if (!spring_) {
        LOGE("Animate failed, spring is null. index: %{public}d", index_);
        return;
    }
    int64_t delta = static_cast<int64_t>(context->GetTimeFromExternalTimer()) - static_cast<int64_t>(startTime_);
    spring_->OnTimestampChanged(static_cast<double>(delta) / static_cast<double>(NANOS_TO_MILLS), 0.0f, false);
    value_ = spring_->GetCurrentPosition();
    velocity_ = spring_->GetCurrentVelocity();
    if (spring_->IsCompleted()) {
        isRunning_ = false;
        value_ = spring_->GetEndValue();
        velocity_ = 0.0;
        // Node's animation update callback
        OnUpdate(value_, velocity_);
        NotifyUpdateListener(value_, velocity_);
        OnEnd(value_);
        scheduler_->Stop();
    } else {
        OnUpdate(value_, velocity_);
        NotifyUpdateListener(value_, velocity_);
        scheduler_->Start();
    }
}

void SimpleSpringNode::Cancel()
{
    scheduler_->Stop();
    isRunning_ = false;
    velocity_ = 0.0;
    OnEnd(value_);
}

void SimpleSpringNode::ResetNode(double value, double velocity)
{
    LOGD("ResetNode. value: %{public}.3lf, velocity: %{public}.3lf", value, velocity);
    value_ = value;
    velocity_ = velocity;
    OnUpdate(value_, velocity_);
    NotifyUpdateListener(value_, velocity_);
}

void SimpleSpringNode::TransferParams(double stiffness, double damping)
{
    if (springProperty_) {
        springProperty_->SetStiffness(stiffness);
        springProperty_->SetDamping(damping);
    }
}

} // namespace OHOS::Ace