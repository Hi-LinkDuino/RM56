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

#include "core/animation/bilateral_spring_node.h"

namespace OHOS::Ace {
namespace {

constexpr int64_t NANOS_TO_MILLS = 1000000;

} // namespace

void BilateralSpringNode::NotifyNext(double endValue, double initVelocity)
{
    if (!adapter_) {
        LOGE("Notify next failed. adapter is null.");
        return;
    }
    if (this != AceType::RawPtr(adapter_->GetControlNode()) || adapter_->GetSize() < 2) {
        return;
    }

    int32_t controlIndex = adapter_->GetSize() / 2;
    RefPtr<SpringNode> node;
    for (int32_t index = 1; index <= controlIndex; index++) {
        if (controlIndex + index < adapter_->GetSize()) {
            node = adapter_->GetNode(controlIndex + index);
            if (node) {
                node->EndToValue(endValue, initVelocity);
            }
        }
        if (controlIndex - index >= 0) {
            node = adapter_->GetNode(controlIndex - index);
            if (node) {
                node->EndToValue(endValue, initVelocity);
            }
        }
    }
}

void BilateralSpringNode::OnAnimation()
{
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
    spring_->OnTimestampChanged(static_cast<double>(delta * 1.0) / static_cast<double>(NANOS_TO_MILLS), 0.0f, false);
    double simpleValue = spring_->GetCurrentPosition();
    FixValue(simpleValue);
    DoCollision();
    velocity_ = spring_->GetCurrentVelocity();
    if (spring_->IsCompleted() || spring_->IsCompleted(simpleValue, velocity_)) {
        isRunning_ = false;
        value_ = spring_->GetEndValue();
        velocity_ = 0.0;
        // Node's animation update callback
        OnUpdate(value_, velocity_);
        NotifyUpdateListener(value_, velocity_);
        OnEnd(value_);
        if (scheduler_->IsActive()) {
            scheduler_->Stop();
        }
        LOGD("Animation. index: %{public}d complete at value: %{public}.3lf", index_, value_);
    } else {
        OnUpdate(value_, velocity_);
        NotifyUpdateListener(value_, velocity_);
        if (!scheduler_->IsActive()) {
            scheduler_->Start();
        }
    }
}

void BilateralSpringNode::DoCollision()
{
    if (!switchCollision_ || !adapter_) {
        return;
    }
    if (adapter_->GetControlIndex() > GetIndex()) {
        // up / left node
        auto next = adapter_->GetNode(GetIndex() + 1);
        if (next) {
            double nextBasePosition = next->GetValue() + decoration_;
            value_ = std::clamp(value_, nextBasePosition - maxDecoration_, nextBasePosition - minDecoration_);
        }
    }
    if (adapter_->GetControlIndex() < GetIndex()) {
        // down / right node
        auto prev = adapter_->GetNode(GetIndex() - 1);
        if (prev) {
            double prevBasePosition = prev->GetValue() - decoration_;
            value_ = std::clamp(value_, prevBasePosition + minDecoration_, prevBasePosition + maxDecoration_);
        }
    }
}

void BilateralSpringNode::FixValue(double springValue)
{
    if (fixMode_) {
        SimpleFixedValue(springValue);
    } else {
        value_ = springValue;
    }
}

void BilateralSpringNode::SimpleFixedValue(double springValue)
{
    if (!fixMode_) {
        value_ = springValue;
        return;
    }
    double delta = springValue - lastTheoryValue_;
    lastTheoryValue_ = springValue;

    if (GreatOrEqual(std::abs(delta), 1.0)) {
        // The displacement between two frames is greater than 1 pixel, using engine data
        value_ = springValue;
    } else {
        // The displacement between two frames is less than 1 pixel, and +1 processing is performed
        if (delta > 0.0) {
            value_++;
        } else {
            value_--;
        }
    }
}

void BilateralSpringNode::SetDeltaValue(double delta)
{
    SimpleSpringNode::SetDeltaValue(delta);
    DoCollision();
}

} // namespace OHOS::Ace