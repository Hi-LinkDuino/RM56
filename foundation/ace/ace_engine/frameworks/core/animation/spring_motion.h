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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_MOTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_MOTION_H

#include <cmath>

#include "base/utils/utils.h"
#include "core/animation/motion.h"
#include "core/animation/spring_model.h"

namespace OHOS::Ace {

class ACE_EXPORT SpringMotion : public Motion {
    DECLARE_ACE_TYPE(SpringMotion, Motion);

public:
    SpringMotion(double start, double end, double velocity, const RefPtr<SpringProperty>& spring);

    ~SpringMotion() override = default;

    void Move(float offsetTime) override;

    SpringModelType GetType();

    double GetCurrentPosition() override;

    double GetCurrentVelocity() override;

    double GetEndValue() const;

    bool IsCompleted(double value, double velocity) const;

    bool IsCompleted() override;

    void SetAccuracy(double accuracy);

    void Reset(double start, double end, double velocity, const RefPtr<SpringProperty>& spring);

protected:
    double endPosition_ = 0.0;
    double currentPosition_ = 0.0;
    double currentVelocity_ = 0.0;
    double accuracy_ = NEAR_ZERO;
    RefPtr<SpringModel> model_;

private:
    // the threshold close to zero
    static constexpr double NEAR_ZERO = 0.001;
};

class ScrollSpringMotion : public SpringMotion {
    DECLARE_ACE_TYPE(ScrollSpringMotion, SpringMotion);

public:
    ScrollSpringMotion(double start, double end, double velocity, const RefPtr<SpringProperty>& spring);

    ~ScrollSpringMotion() override = default;

    void Move(float offsetTime) override;

    bool IsCompleted() override;

    double GetCurrentPosition() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_MOTION_H
