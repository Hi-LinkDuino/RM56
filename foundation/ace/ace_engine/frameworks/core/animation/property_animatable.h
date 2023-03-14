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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATABLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATABLE_H

#include <map>

#include "base/memory/ace_type.h"
#include "core/animation/animation.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"

namespace OHOS::Ace {

enum class PropertyAnimatableType {
    PROPERTY_WIDTH,
    PROPERTY_HEIGHT,
    PROPERTY_BACK_DECORATION_COLOR,
    PROPERTY_FRONT_DECORATION_COLOR,
    PROPERTY_BACKGROUND_POSITION,
    PROPERTY_OFFSET_X,
    PROPERTY_OFFSET_Y,
    PROPERTY_BORDER_RADIUS,
};

template<class T>
class TypePropertyAnimatable {
public:
    using Setter = std::function<void(T value)>;
    using Getter = std::function<T(void)>;
    using SetterMap = std::map<PropertyAnimatableType, Setter>;
    using GetterMap = std::map<PropertyAnimatableType, Getter>;
    using Type = T;
};

using FloatPropertyAnimatable = TypePropertyAnimatable<float>;
using ColorPropertyAnimatable = TypePropertyAnimatable<Color>;
using BackgroundPositionPropertyAnimatable = TypePropertyAnimatable<BackgroundImagePosition>;

using PropertyAnimationFloat = RefPtr<Animation<float>>;
using PropertyAnimationFloatMap = std::map<PropertyAnimatableType, PropertyAnimationFloat>;

class PropertyAnimatable : public virtual AceType {
    DECLARE_ACE_TYPE(PropertyAnimatable, AceType);

public:
    template<class U, class V>
    static bool AddPropertyAnimation(const RefPtr<PropertyAnimatable>& propertyAnimatable,
        PropertyAnimatableType property, RefPtr<Animation<V>>& animation, typename U::Type& initValue)
    {
        U u {};
        if (!propertyAnimatable) {
            LOGE("Create property animation failed. animatable is null.");
            return false;
        }
        if (!animation) {
            LOGE("Create property animation failed. animation is null.");
            return false;
        }
        LOGD("Create Property Animation. property: %{public}d.", property);
        typename U::SetterMap setterMap = propertyAnimatable->GetPropertySetterMap(u);
        typename U::GetterMap getterMap = propertyAnimatable->GetPropertyGetterMap(u);

        auto setterIter = setterMap.find(property);
        if (setterIter == setterMap.end()) {
            LOGW("Create property animation failed. no setter found for property: %{public}d", property);
            return false;
        }
        auto& setter = setterIter->second;
        if (!setter) {
            LOGE("Create property animation failed. setter is null for property: %{public}d", property);
            return false;
        }
        auto getterIter = getterMap.find(property);
        if (getterIter == getterMap.end()) {
            LOGW("Create property animation failed. no getter found for property: %{public}d", property);
            return false;
        }
        auto& getter = getterIter->second;
        if (!getter) {
            LOGE("Create property animation failed. getter is null for property: %{public}d", property);
            return false;
        }
        animation->AddListener(setter);
        initValue = getter();
        return true;
    }

private:
    const FloatPropertyAnimatable::SetterMap GetPropertySetterMap(FloatPropertyAnimatable&)
    {
        return GetFloatPropertySetterMap();
    };
    const FloatPropertyAnimatable::GetterMap GetPropertyGetterMap(FloatPropertyAnimatable&)
    {
        return GetFloatPropertyGetterMap();
    };
    const ColorPropertyAnimatable::SetterMap GetPropertySetterMap(ColorPropertyAnimatable&)
    {
        return GetColorPropertySetterMap();
    };
    const ColorPropertyAnimatable::GetterMap GetPropertyGetterMap(ColorPropertyAnimatable&)
    {
        return GetColorPropertyGetterMap();
    };
    const BackgroundPositionPropertyAnimatable::SetterMap GetPropertySetterMap(BackgroundPositionPropertyAnimatable&)
    {
        return GetBackgroundPositionPropertySetterMap();
    };
    const BackgroundPositionPropertyAnimatable::GetterMap GetPropertyGetterMap(BackgroundPositionPropertyAnimatable&)
    {
        return GetBackgroundPositionPropertyGetterMap();
    };
    virtual FloatPropertyAnimatable::SetterMap GetFloatPropertySetterMap()
    {
        return FloatPropertyAnimatable::SetterMap();
    };
    virtual FloatPropertyAnimatable::GetterMap GetFloatPropertyGetterMap()
    {
        return FloatPropertyAnimatable::GetterMap();
    };
    virtual ColorPropertyAnimatable::SetterMap GetColorPropertySetterMap()
    {
        return ColorPropertyAnimatable::SetterMap();
    };
    virtual ColorPropertyAnimatable::GetterMap GetColorPropertyGetterMap()
    {
        return ColorPropertyAnimatable::GetterMap();
    };
    virtual BackgroundPositionPropertyAnimatable::SetterMap GetBackgroundPositionPropertySetterMap()
    {
        return BackgroundPositionPropertyAnimatable::SetterMap();
    };
    virtual BackgroundPositionPropertyAnimatable::GetterMap GetBackgroundPositionPropertyGetterMap()
    {
        return BackgroundPositionPropertyAnimatable::GetterMap();
    };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATABLE_H
