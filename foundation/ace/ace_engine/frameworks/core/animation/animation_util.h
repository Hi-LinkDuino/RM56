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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_UTIL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_UTIL_H

#include <string>

#include "core/animation/animatable_data.h"
#include "core/components/common/properties/shadow.h"
#include "core/components/common/properties/tween_option.h"
#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/utils/transform_convertor.h"

namespace OHOS::Ace {

class AnimationUtil final {
public:
    const PropAnimationMap& GetPropAnimationMap() const
    {
        return propAnimationMap_;
    }

    const TransformConvertor& GetTransformConvertor() const
    {
        return transformConvertor_;
    }

    const std::string& GetAnimationName() const
    {
        return animationName_;
    }

    void Clear()
    {
        transformConvertor_.ClearAnimations();
        shadowKeyframes_.clear();
        propAnimationMap_.clear();
    }

    void ParseAnimationStyle(const std::vector<std::unordered_map<std::string, std::string>>& animationKeyframes,
        const RefPtr<Declaration>& declaration, const RefPtr<ThemeConstants> themeConstants);

private:
    void KeyframesAddKeyFrame(const std::string& keyStyle, const std::string& value, const std::string& timeStr);
    template<class T>
    void AddAnimatable(const T& value, double time, AnimatableType);
    RefPtr<ThemeConstants> GetThemeConstants() const;
    Color ParseColor(const std::string& value, uint32_t maskAlpha = COLOR_ALPHA_MASK) const;
    double ParseDouble(const std::string& value) const;
    Dimension ParseDimension(const std::string& value) const;

    template<typename T>
    T ParseThemeReference(const std::string& value, std::function<T()>&& noRefFunc,
        std::function<T(uint32_t refId)>&& idRefFunc, const T& errorValue) const
    {
        const auto& parseResult = ThemeUtils::ParseThemeIdReference(value, GetThemeConstants());
        if (!parseResult.parseSuccess) {
            return noRefFunc();
        }
        auto themeConstants = GetThemeConstants();
        if (!themeConstants) {
            return errorValue;
        }
        // Refer to a theme id resource.
        if (parseResult.isIdRef) {
            return idRefFunc(parseResult.id);
        }
        // Refer to a theme attribute.
        auto themeStyle = themeConstants->GetThemeStyle();
        if (!themeStyle) {
            return errorValue;
        }
        return themeStyle->GetAttr<T>(parseResult.refAttr, errorValue);
    }

private:
    std::string animationName_;
    PropAnimationMap propAnimationMap_;
    TransformConvertor transformConvertor_;
    std::map<std::string, Shadow> shadowKeyframes_;
    bool isRightToLeft = false;
    RefPtr<ThemeConstants> themeConstants_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_UTIL_H
