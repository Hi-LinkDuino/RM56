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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_ANIMATION_H

#include "core/animation/animation.h"
#include "core/animation/interpolator.h"
#include "core/animation/spring_motion.h"

namespace OHOS::Ace {

// Spring interpolator for elastic animation. Reference config:Mass(1.0) stiffness(100.0) damping(15.0).
class SpringAnimation : public Animation<float> {
    DECLARE_ACE_TYPE(SpringAnimation, Animation);

public:
    explicit SpringAnimation(const RefPtr<SpringProperty>& property);
    SpringAnimation(const RefPtr<SpringProperty>& property, float velocity);
    SpringAnimation(const RefPtr<SpringProperty>& property, float velocity, float valueThreshold);
    ~SpringAnimation() override = default;

    const float& GetValue() const override
    {
        return currentPosition_;
    }

private:
    void SetEndPosition(float endPosition, float startVelocity);

    void UpdatePosition(float normalized);

    void InitEstimateDuration();

    void OnNormalizedTimestampChanged(float normalized, bool reverse) override
    {
        if (normalized < NORMALIZED_DURATION_MIN || normalized > NORMALIZED_DURATION_MAX) {
            LOGE("normalized time check failed. normalized: %{public}f", normalized);
            return;
        }
        UpdatePosition(normalized);
        NotifyListener(currentPosition_);
    }

    RefPtr<Curve> GetCurve() override;

    float estimateDuration_ = 1.0f;
    float currentPosition_ = 0.0f;
    float currentVelocity_ = 0.0f;
    float valueThreshold_ = 0.001f;
    float velocityThreshold_ = 0.001f;
    float startPosition_ = 0.0f;
    float endPosition_ = 0.0f;
    RefPtr<SpringProperty> property_; // Contain: mass & stiffness & damping
    RefPtr<SpringModel> solution_; // Maybe: CriticalDamped or Overdamped or Underdamped
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_ANIMATION_H
