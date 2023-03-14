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

#include "core/animation/spring_model.h"

#include "base/log/log.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {
namespace {

constexpr double HIGH_RATIO = 4.0;
constexpr double LOW_RATIO = 2.0;

} // namespace

bool SpringProperty::IsValid() const
{
    if (LessOrEqual(mass_, 0.0) || LessOrEqual(stiffness_, 0.0) || LessOrEqual(damping_, 0.0)) {
        return false;
    }
    return true;
}

void SpringProperty::SetMass(double mass)
{
    if (mass > 0.0) {
        mass_ = mass;
    }
}

double SpringProperty::Mass() const
{
    return mass_;
}

void SpringProperty::SetStiffness(double stiffness)
{
    if (stiffness > 0.0) {
        stiffness_ = stiffness;
    }
}

double SpringProperty::Stiffness() const
{
    return stiffness_;
}

void SpringProperty::SetDamping(double damping)
{
    if (damping > 0.0) {
        damping_ = damping;
    }
}

double SpringProperty::Damping() const
{
    return damping_;
}

RefPtr<SpringModel> SpringModel::Build(double distance, double velocity, const RefPtr<SpringProperty>& spring)
{
    if (!spring || !spring->IsValid()) {
        LOGE("SpringProperty can not be nullptr.");
        return nullptr;
    } else {
        double cmk = spring->Damping() * spring->Damping() - HIGH_RATIO * spring->Mass() * spring->Stiffness();
        if (NearZero(cmk)) {
            if (NearZero(distance)) {
                LOGE("create CriticalDamped failed, distance can not be zero.");
                return nullptr;
            }
            return AceType::MakeRefPtr<CriticalDampedModel>(distance, velocity, spring);
        } else if (cmk > 0.0) {
            return AceType::MakeRefPtr<OverdampedModel>(distance, velocity, spring);
        } else {
            return AceType::MakeRefPtr<UnderdampedModel>(distance, velocity, spring);
        }
    }
}

// Overdamping calculation model.
CriticalDampedModel::CriticalDampedModel(double distance, double velocity, const RefPtr<SpringProperty>& spring)
{
    if (spring && spring->IsValid() && !NearZero(distance)) {
        r_ = -spring->Damping() / (LOW_RATIO * spring->Mass());
        c1_ = distance;
        c2_ = velocity / (r_ * distance);
    }
}

double CriticalDampedModel::Position(double time) const
{
    return (c1_ + c2_ * time) * exp(r_ * time);
}

double CriticalDampedModel::Velocity(double time) const
{
    const double power = exp(r_ * time);
    return r_ * (c1_ + c2_ * time) * power + c2_ * power;
}

SpringModelType CriticalDampedModel::GetType() const
{
    return SpringModelType::CRITICAL_DAMPED;
}

// Overdamping calculation model.
OverdampedModel::OverdampedModel(double distance, double velocity, const RefPtr<SpringProperty>& spring)
{
    if (spring && spring->IsValid()) {
        double cmk = spring->Damping() * spring->Damping() - HIGH_RATIO * spring->Mass() * spring->Stiffness();
        r1_ = (-spring->Damping() - sqrt(cmk)) / (LOW_RATIO * spring->Mass());
        r2_ = (-spring->Damping() + sqrt(cmk)) / (LOW_RATIO * spring->Mass());
        if (!NearEqual(r2_, r1_)) {
            c2_ = (velocity - r1_ * distance) / (r2_ - r1_);
            c1_ = distance - c2_;
        }
    }
}

double OverdampedModel::Position(double time) const
{
    return c1_ * exp(r1_ * time) + c2_ * exp(r2_ * time);
}

double OverdampedModel::Velocity(double time) const
{
    return c1_ * r1_ * exp(r1_ * time) + c2_ * r2_ * exp(r2_ * time);
}

SpringModelType OverdampedModel::GetType() const
{
    return SpringModelType::OVER_DAMPED;
}

// Underdamped calculation model
UnderdampedModel::UnderdampedModel(double distance, double velocity, const RefPtr<SpringProperty>& spring)
{
    if (spring && spring->IsValid()) {
        w_ = sqrt(HIGH_RATIO * spring->Mass() * spring->Stiffness() - spring->Damping() * spring->Damping()) /
             (LOW_RATIO * spring->Mass());
        r_ = -(spring->Damping() / LOW_RATIO * spring->Mass());
        c1_ = distance;
        if (!NearEqual(w_, 0.0)) {
            c2_ = (velocity - r_ * distance) / w_;
        }
    }
}

double UnderdampedModel::Position(double time) const
{
    return exp(r_ * time) * (c1_ * cos(w_ * time) + c2_ * sin(w_ * time));
}

double UnderdampedModel::Velocity(double time) const
{
    double power = exp(r_ * time);
    double cosine = cos(w_ * time);
    double sine = sin(w_ * time);
    return power * (c2_ * w_ * cosine - c1_ * w_ * sine) + r_ * power * (c2_ * sine + c1_ * cosine);
}

SpringModelType UnderdampedModel::GetType() const
{
    return SpringModelType::UNDER_DAMPED;
}

} // namespace OHOS::Ace