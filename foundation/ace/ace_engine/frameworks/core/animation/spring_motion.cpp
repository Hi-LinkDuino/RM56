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

#include "core/animation/spring_motion.h"

#include "base/log/log.h"

namespace OHOS::Ace {
namespace {

constexpr float UNIT_CONVERT = 1000.0f;

} // namespace

SpringMotion::SpringMotion(double start, double end, double velocity, const RefPtr<SpringProperty>& spring)
{
    Reset(start, end, velocity, spring);
}

SpringModelType SpringMotion::GetType()
{
    return model_->GetType();
}

double SpringMotion::GetCurrentPosition()
{
    return currentPosition_;
}

double SpringMotion::GetCurrentVelocity()
{
    return currentVelocity_;
}

double SpringMotion::GetEndValue() const
{
    return endPosition_;
}

bool SpringMotion::IsCompleted(double value, double velocity) const
{
    return NearZero(value - endPosition_, accuracy_) && NearZero(velocity, accuracy_);
}

bool SpringMotion::IsCompleted()
{
    return IsCompleted(currentPosition_, currentVelocity_);
}

void SpringMotion::SetAccuracy(double accuracy)
{
    accuracy_ = std::abs(accuracy);
}

void SpringMotion::Reset(double start, double end, double velocity, const RefPtr<SpringProperty>& spring)
{
    currentPosition_ = start;
    currentVelocity_ = velocity;
    endPosition_ = end;
    model_ = SpringModel::Build(start - end, velocity, spring);
}

void SpringMotion::Move(float offsetTime)
{
    // change millisecond to second.
    float offsetTimeInSecond = offsetTime / UNIT_CONVERT;
    currentPosition_ = endPosition_ + model_->Position(offsetTimeInSecond);
    currentVelocity_ = model_->Velocity(offsetTimeInSecond);
    if (IsCompleted()) {
        currentPosition_ = endPosition_;
        currentVelocity_ = 0.0;
    }
}

ScrollSpringMotion::ScrollSpringMotion(double start, double end, double velocity, const RefPtr<SpringProperty>& spring)
    : SpringMotion(start, end, velocity, spring)
{}

bool ScrollSpringMotion::IsCompleted()
{
    return NearZero(currentPosition_ - endPosition_, accuracy_);
}

double ScrollSpringMotion::GetCurrentPosition()
{
    return currentPosition_;
}

void ScrollSpringMotion::Move(float offsetTime)
{
    // change millisecond to second.
    float offsetTimeInSecond = offsetTime / UNIT_CONVERT;
    currentPosition_ = endPosition_ + model_->Position(offsetTimeInSecond);
    if (IsCompleted()) {
        currentPosition_ = endPosition_;
    }
}

} // namespace OHOS::Ace