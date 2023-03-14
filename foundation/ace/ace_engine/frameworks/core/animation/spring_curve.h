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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_CURVE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_CURVE_H

#include "core/animation/curve.h"
#include "core/animation/spring_motion.h"

namespace OHOS::Ace {

class ACE_EXPORT SpringCurve : public Curve {
    DECLARE_ACE_TYPE(SpringCurve, Curve);

public:
    SpringCurve(float velocity, float mass, float stiffness, float damping);
    ~SpringCurve() override = default;

    float MoveInternal(float time) override;
    const std::string ToString() override;

private:
    void SetEndPosition(float endPosition, float startVelocity);
    void InitEstimateDuration();

    float estimateDuration_ = 1.0f;
    float startPosition_ = 0.0f;
    float endPosition_ = 0.0f;
    float currentVelocity_ = 0.0f;
    float currentPosition_ = 0.0f;
    float velocityThreshold_ = 0.001f;
    float valueThreshold_ = 0.001f;
    float velocity_ = 0.0f;
    float mass_ = 0.0f;
    float stiffness_ = 0.0f;
    float damping_ = 0.0f;
    RefPtr<SpringProperty> property_; // Contain: mass & stiffness & damping
    RefPtr<SpringModel> solution_; // Maybe: CriticalDamped or Overdamped or Underdamped

    friend class NativeCurveHelper;
};

} // // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_CURVE_H