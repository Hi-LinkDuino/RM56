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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_DATA_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_DATA_H

#include "base/geometry/dimension.h"
#include "core/animation/animatable.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/shadow.h"

namespace OHOS::Ace {

template<typename T>
class AnimatableData : public Animatable {
    DECLARE_ACE_TYPE(AnimatableData, Animatable);

public:
    static T Blend(const T& start, const T& end, float process)
    {
        return start + (end - start) * process;
    }

    // do not add explicit for this constructor.
    explicit AnimatableData(const T& value) : value_(value) {}
    ~AnimatableData() = default;

    virtual T Animate(const T& start, const T& end, float process)
    {
        return Blend(start, end, process);
    }

    const T& GetValue() const
    {
        return value_;
    }

    void SetValue(const T& value)
    {
        value_ = value;
    }

protected:
    T value_;
};

template<>
BorderStyle AnimatableData<BorderStyle>::Blend(const BorderStyle& start, const BorderStyle& end, float process);

template<>
Shadow AnimatableData<Shadow>::Blend(const Shadow& start, const Shadow& end, float process);

template<>
BackgroundImageSize AnimatableData<BackgroundImageSize>::Blend(
    const BackgroundImageSize& start, const BackgroundImageSize& end, float process);

template<>
Dimension AnimatableData<Dimension>::Blend(const Dimension& start, const Dimension& end, float process);

template<>
Color AnimatableData<Color>::Blend(const Color& start, const Color& end, float process);

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_DATA_H