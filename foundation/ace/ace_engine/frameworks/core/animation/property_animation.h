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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATION_H

#include <functional>
#include <list>

#include "core/animation/curve.h"
#include "core/animation/animatable.h"
#include "core/animation/interpolator.h"

namespace OHOS::Ace {

class PropertyAnimation : public Interpolator {
    DECLARE_ACE_TYPE(PropertyAnimation, Interpolator);

public:
    using PropCallback = std::function<void(const RefPtr<Animatable>&)>;
    explicit PropertyAnimation(AnimatableType type);
    PropertyAnimation() = delete;
    ~PropertyAnimation() override = default;
    void SetStart(const RefPtr<Animatable>& animatable);
    void AddAnimatable(const RefPtr<Animatable>& animatable);
    const std::list<RefPtr<Animatable>>& GetAnimatable() const
    {
        return animatables_;
    }
    void SetCurve(const RefPtr<Curve>& curve);
    void AddListener(const PropCallback& callback)
    {
        animateTo_ = callback;
    }
    const RefPtr<Animatable>& GetInit() const
    {
        return init_;
    }
    void SetInit(const RefPtr<Animatable>& init)
    {
        init_ = init;
    }

private:
    void OnNormalizedTimestampChanged(float normalized, bool reverse) override;
    void OnInitNotify(float normalizedTime, bool reverse) override;
    RefPtr<Curve> GetCurve() override;
    void Calculate(float keyTime);
    void TriggerFrame(const RefPtr<Animatable>& start, const RefPtr<Animatable>& end, float time);

    template<class T>
    void Next(const RefPtr<Animatable>& start, const RefPtr<Animatable>& end, float time);

    AnimatableType type_;
    std::list<RefPtr<Animatable>> animatables_;
    PropCallback animateTo_ = nullptr;
    RefPtr<Animatable> init_;
};

using PropAnimationMap = std::map<AnimatableType, RefPtr<PropertyAnimation>>;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATION_H