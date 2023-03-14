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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_FRICTION_MOTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_FRICTION_MOTION_H

#include "core/animation/motion.h"

namespace OHOS::Ace {

constexpr float DEFAULT_MULTIPLIER = 60.0f;

class ACE_EXPORT FrictionMotion : public Motion {
public:
    FrictionMotion(double friction, double initPosition, double initVelocity);
    ~FrictionMotion() override = default;

    // Called when do a new friction motion
    void Reset(double friction, double initPosition, double initVelocity, double threshold = DEFAULT_MULTIPLIER);

    double GetCurrentPosition() override;
    double GetCurrentVelocity() override;
    bool IsCompleted() override;

    double GetVelocityByFinalPosition(double final, double threshold = DEFAULT_MULTIPLIER) const;
    // Get the Position and Velocity by offsetTime(Unit:second).
    double GetPosition(float offsetTime) const;
    double GetVelocity(float offsetTime) const;

    // Trigger motion in each vsync timestamp(Unit:millisecond).
    void Move(float offsetTime) override;

    double GetFinalPosition() const;
    bool GetTimeByPosition(double position, double& time) const;

private:
    bool IsValid(double friction) const;

    double friction_ { 1.0 };
    double initVelocity_ { 0.0 };
    double signum_ { 0.0 }; // Rules: V < 0 [-1], V = 0 [0], V > 0 [1].
    double currentTime_ { 0.0 };
    double valueThreshold_ { 0.0 };
    double velocityThreshold_ { 0.0 };
    double finalTime_ { 0.0 };
    double finalPosition_ { 0.0 };
    double initPosition_ { 0.0 };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_FRICTION_MOTION_H
