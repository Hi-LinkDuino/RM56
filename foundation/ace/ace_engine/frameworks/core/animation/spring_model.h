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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_MODEL_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

enum class SpringModelType {
    CRITICAL_DAMPED, // critical damped calculation model
    UNDER_DAMPED,    // under damped calculation model
    OVER_DAMPED,     // over damping calculation model
};

class SpringProperty : public AceType {
    DECLARE_ACE_TYPE(SpringProperty, AceType);

public:
    SpringProperty(double mass, double stiffness, double damping)
        : mass_(mass), stiffness_(stiffness), damping_(damping)
    {}

    SpringProperty() = default;

    ~SpringProperty() override = default;

    void SetMass(double mass);

    double Mass() const;

    void SetStiffness(double stiffness);

    double Stiffness() const;

    void SetDamping(double damping);

    double Damping() const;

    bool IsValid() const;

private:
    // Default stiffness of spring.
    static constexpr double DEFAULT_STIFFNESS = 228.0;
    // Default damping of spring.
    static constexpr double DEFAULT_DAMPING = 30.0;
    // Default mass of spring
    static constexpr double DEFAULT_MASS = 1.0;

    // the mass of the spring.
    double mass_ = DEFAULT_MASS;
    // the stiffness of spring, generally, a spring stiffness is constant.
    double stiffness_ = DEFAULT_STIFFNESS;
    // damping ratio of spring
    double damping_ = DEFAULT_DAMPING;
};

class SpringModel : public AceType {
    DECLARE_ACE_TYPE(SpringModel, AceType);

public:
    // calculate position, the unit of time is second.
    virtual double Position(double time) const = 0;

    // calculate velocity, the unit of time is second.
    virtual double Velocity(double time) const = 0;

    // get current calculation type.
    virtual SpringModelType GetType() const = 0;

    /**
     * Judge the type of the spring and get the calculation according to the type.
     */
    static RefPtr<SpringModel> Build(double distance, double velocity, const RefPtr<SpringProperty>& spring);
};

// Critical Damped calculation model.
class CriticalDampedModel : public SpringModel {
    DECLARE_ACE_TYPE(CriticalDampedModel, SpringModel);

public:
    CriticalDampedModel(double distance, double velocity, const RefPtr<SpringProperty>& spring);

    ~CriticalDampedModel() override = default;

    double Position(double time) const override;

    double Velocity(double time) const override;

    SpringModelType GetType() const override;

private:
    double r_ = 0.0;
    double c1_ = 0.0;
    double c2_ = 0.0;
};

// Overdamping calculation model.
class OverdampedModel : public SpringModel {
    DECLARE_ACE_TYPE(OverdampedModel, SpringModel);

public:
    OverdampedModel(double distance, double velocity, const RefPtr<SpringProperty>& spring);

    ~OverdampedModel() override = default;

    double Position(double time) const override;

    double Velocity(double time) const override;

    SpringModelType GetType() const override;

private:
    double r1_ = 0.0;
    double r2_ = 0.0;
    double c1_ = 0.0;
    double c2_ = 0.0;
};

// Underdamped calculation model
class UnderdampedModel : public SpringModel {
    DECLARE_ACE_TYPE(UnderdampedModel, SpringModel);

public:
    UnderdampedModel(double distance, double velocity, const RefPtr<SpringProperty>& spring);

    ~UnderdampedModel() override = default;

    double Position(double time) const override;

    double Velocity(double time) const override;

    SpringModelType GetType() const override;

private:
    double w_ = 0.0;
    double r_ = 0.0;
    double c1_ = 0.0;
    double c2_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_MODEL_H
