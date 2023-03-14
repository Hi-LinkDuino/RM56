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

#include "core/animation/scroll_motion.h"

#include "base/log/log.h"

namespace OHOS::Ace {
namespace {

constexpr double DEFAULT_SPRING_MASS = 1.0;
constexpr double DEFAULT_SPRING_STIFFNESS = 500.0;
constexpr double DEFAULT_SPRING_DAMPING = 45.0;

} // namespace

ScrollMotion::ScrollMotion(double position, double velocity, const ExtentPair& extent,
    const ExtentPair& initExtent, const RefPtr<SpringProperty>& spring)
    : position_(position), velocity_(velocity), leadingExtent_(extent.Leading()), trailingExtent_(extent.Trailing())
{
    if (spring && spring->IsValid()) {
        spring_ = spring;
    } else {
        LOGW("construct scroll motion error. empty or invalid spring property, use default spring property.");
        spring_ =
            AceType::MakeRefPtr<SpringProperty>(DEFAULT_SPRING_MASS, DEFAULT_SPRING_STIFFNESS, DEFAULT_SPRING_DAMPING);
    }

    if (position > initExtent.Trailing()) {
        springMotion_ = MakeOverScrollMotion(position, velocity);
    } else if (position < initExtent.Leading()) {
        springMotion_ = MakeUnderScrollMotion(position, velocity);
    } else {
        LOGW("No available motion.");
    }
}

bool ScrollMotion::IsValid() const
{
    return springMotion_ != nullptr;
}

void ScrollMotion::Move(float offsetTime)
{
    // SpringMotion is null when not OverScoll&UnderScroll.
    if (!springMotion_) {
        return;
    }
    springMotion_->Move(offsetTime);
}

double ScrollMotion::GetCurrentPosition()
{
    if (!springMotion_) {
        // do not move yet, return init position.
        return position_;
    }
    return springMotion_->GetCurrentPosition();
}

double ScrollMotion::GetCurrentVelocity()
{
    if (!springMotion_) {
        // do not move yet, return init velocity.
        return velocity_;
    }
    return springMotion_->GetCurrentVelocity();
}

bool ScrollMotion::IsCompleted()
{
    if (!springMotion_) {
        // do not move yet
        return false;
    }
    return springMotion_->IsCompleted();
}

} // namespace OHOS::Ace