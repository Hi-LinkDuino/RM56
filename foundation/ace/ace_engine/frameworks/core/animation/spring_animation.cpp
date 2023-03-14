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

#include "core/animation/spring_animation.h"

namespace OHOS::Ace {
namespace {

constexpr float DEFAULT_VALUE_THRESHOLD = 0.001f;
constexpr float VELOCITY_THRESHOLD_RATIO = 25.0f;
constexpr float DEFAULT_START_POSITION = 0.0f;
constexpr float DEFAULT_END_POSITION = 1.0f;
constexpr float DEFAULT_START_VELOCITY = 0.0f;
constexpr int32_t DEFAULT_ESTIMATE_STEPS = 100;

} // namespace

SpringAnimation::SpringAnimation(const RefPtr<SpringProperty>& property)
{
    property_ = property;
    valueThreshold_ = (DEFAULT_END_POSITION - DEFAULT_START_POSITION) * DEFAULT_VALUE_THRESHOLD;
    SetEndPosition(DEFAULT_END_POSITION, DEFAULT_START_VELOCITY);
}

SpringAnimation::SpringAnimation(const RefPtr<SpringProperty>& property, float velocity)
{
    property_ = property;
    valueThreshold_ = (DEFAULT_END_POSITION - DEFAULT_START_POSITION) * DEFAULT_VALUE_THRESHOLD;
    SetEndPosition(DEFAULT_END_POSITION, velocity);
}

SpringAnimation::SpringAnimation(const RefPtr<SpringProperty>& property, float velocity, float valueThreshold)
{
    property_ = property;
    valueThreshold_ = valueThreshold;
    SetEndPosition(DEFAULT_END_POSITION, velocity);
}

void SpringAnimation::SetEndPosition(float endPosition, float startVelocity)
{
    startPosition_ = 0.0f;
    endPosition_ = endPosition;
    currentVelocity_ = startVelocity;
    currentPosition_ = startPosition_;
    velocityThreshold_ = valueThreshold_ * VELOCITY_THRESHOLD_RATIO;
    solution_ = SpringModel::Build(endPosition_, startVelocity, property_);
    InitEstimateDuration();
}

void SpringAnimation::UpdatePosition(float normalized)
{
    currentPosition_ = endPosition_ - solution_->Position(normalized * estimateDuration_);
    currentVelocity_ = solution_->Velocity(normalized * estimateDuration_);
    if (NearEqual(currentPosition_, endPosition_, valueThreshold_) &&
        NearZero(currentVelocity_, velocityThreshold_)) {
        currentPosition_ = endPosition_;
        currentVelocity_ = 0.0f;
    }
    LOGD("Time:%{public}f Position:%{public}f Velocity:%{public}f", normalized, currentPosition_, currentVelocity_);
}

void SpringAnimation::InitEstimateDuration()
{
    float position = 0.0f;
    float velocity = 0.0f;
    float time = 1.0f / DEFAULT_ESTIMATE_STEPS;
    for (int32_t i = 1; i < DEFAULT_ESTIMATE_STEPS; ++i) {
        position = endPosition_ - solution_->Position(time * i);
        velocity = solution_->Velocity(time * i);
        if (NearEqual(position, endPosition_, valueThreshold_) && NearZero(velocity, velocityThreshold_)) {
            estimateDuration_ = time * i;
            break;
        }
    }
}

RefPtr<Curve> SpringAnimation::GetCurve()
{
    return MakeRefPtr<CustomCurve>([weak = AceType::WeakClaim(this)](float fraction) -> float {
        auto animation = weak.Upgrade();
        if (animation == nullptr) {
            LOGE("create spring curve failed, animation is null!");
            return NORMALIZED_DURATION_MAX;
        }

        animation->UpdatePosition(fraction);
        return animation->currentPosition_;
    });
}

} // namespace OHOS::Ace
