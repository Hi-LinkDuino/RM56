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

#include "core/animation/friction_motion.h"

#include "base/log/log.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {
namespace {

constexpr float UNIT_CONVERT = 1000.0f;
constexpr float FRICTION_SCALE = -4.2f;
constexpr float DEFAULT_THRESHOLD = 0.75f;

} // namespace

FrictionMotion::FrictionMotion(double friction, double initPosition, double initVelocity)
{
    Reset(friction, initPosition, initVelocity);
}

void FrictionMotion::Reset(double friction, double initPosition, double initVelocity, double threshold)
{
    if (!IsValid(friction)) {
        return;
    }
    friction_ = friction * FRICTION_SCALE;
    initVelocity_ = std::abs(initVelocity);
    currentTime_ = 0.0;
    initPosition_ = initPosition;
    valueThreshold_ = DEFAULT_THRESHOLD;
    velocityThreshold_ = valueThreshold_ * threshold;

    if (NearZero(initVelocity_)) {
        signum_ = 0.0;
        finalTime_ = 0.0;
    } else {
        signum_ = GreatNotEqual(initVelocity, 0.0) ? 1.0 : -1.0;
        finalTime_ = UNIT_CONVERT * std::log(velocityThreshold_ / initVelocity_) / friction_;
    }
    finalTime_ = std::max(finalTime_, 0.0);
    finalPosition_ = GetPosition(finalTime_ / UNIT_CONVERT);
}

bool FrictionMotion::IsValid(double friction) const
{
    if (friction < 0.0 || NearZero(friction)) {
        LOGE("Invalid friction:%{public}lf.", friction);
        return false;
    }
    return true;
}

void FrictionMotion::Move(float offsetTime)
{
    // change millisecond to second.
    currentTime_ = offsetTime / UNIT_CONVERT;
}

double FrictionMotion::GetPosition(float offsetTime) const
{
    return initPosition_ + signum_ * (initVelocity_ / friction_) * std::expm1(friction_ * offsetTime);
}

double FrictionMotion::GetVelocityByFinalPosition(double final, double threshold) const
{
    return  valueThreshold_ * threshold * signum_ - (final - initPosition_) * friction_;
}

double FrictionMotion::GetVelocity(float offsetTime) const
{
    return signum_ * initVelocity_ * std::exp(friction_ * offsetTime);
}

double FrictionMotion::GetCurrentPosition()
{
    return GetPosition(currentTime_);
}

double FrictionMotion::GetCurrentVelocity()
{
    return GetVelocity(currentTime_);
}

bool FrictionMotion::IsCompleted()
{
    return NearZero(GetCurrentVelocity(), velocityThreshold_) ||
        NearEqual(finalPosition_, GetCurrentPosition(), 1.0);
}

double FrictionMotion::GetFinalPosition() const
{
    return finalPosition_;
}

bool FrictionMotion::GetTimeByPosition(double position, double& time) const
{
    time = 0.0;
    if (NearZero(initVelocity_)) {
        return false;
    }

    double rangeStart = 0.0;
    double rangeEnd = finalPosition_;
    if (finalPosition_ < 0.0) {
        rangeStart = finalPosition_;
        rangeEnd = 0.0;
    }
    if (position < rangeStart || position > rangeEnd) {
        return false;
    }
    // Deduced by formula of Func(GetPosition)
    time = std::log(position * friction_ / initVelocity_ + 1.0) / friction_;
    return true;
}

} // namespace OHOS::Ace
