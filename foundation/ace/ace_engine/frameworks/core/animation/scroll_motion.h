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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SCROLL_MOTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SCROLL_MOTION_H

#include "core/animation/friction_motion.h"
#include "core/animation/motion.h"
#include "core/animation/spring_motion.h"

namespace OHOS::Ace {

class ACE_EXPORT ExtentPair {
public:
    ExtentPair(double leading, double trailing) : leading_(leading), trailing_(trailing) {}
    ExtentPair() = delete;
    ~ExtentPair() = default;

    double Leading() const
    {
        return leading_;
    }

    double Trailing() const
    {
        return trailing_;
    }

private:
    double leading_ = 0.0;
    double trailing_ = 0.0;
};

class ACE_EXPORT ScrollMotion : public Motion {
public:
    ScrollMotion(double position, double velocity, const ExtentPair& extent, const ExtentPair& initExtent,
        const RefPtr<SpringProperty>& spring);
    ~ScrollMotion() override = default;

    double GetCurrentPosition() override;
    double GetCurrentVelocity() override;
    bool IsCompleted() override;

    // Perform motion in each timestamp
    void Move(float offsetTime) override;

    bool IsValid() const;

private:
    RefPtr<ScrollSpringMotion> MakeUnderScrollMotion(double position, double velocity)
    {
        return AceType::MakeRefPtr<ScrollSpringMotion>(position, leadingExtent_, velocity, spring_);
    }

    RefPtr<ScrollSpringMotion> MakeOverScrollMotion(double position, double velocity)
    {
        return AceType::MakeRefPtr<ScrollSpringMotion>(position, trailingExtent_, velocity, spring_);
    }

    // init position
    const double position_ { 0.0 };
    // init velocity
    const double velocity_ { 0.0 };
    double leadingExtent_ { 0.0 };
    double trailingExtent_ { 0.0 };
    RefPtr<SpringProperty> spring_;
    RefPtr<ScrollSpringMotion> springMotion_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SCROLL_MOTION_H
