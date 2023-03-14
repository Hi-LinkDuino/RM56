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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_THEME_STYLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_THEME_STYLE_H

#include <type_traits>
#include <unordered_map>
#include <variant>

#include "base/geometry/dimension.h"
#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "base/resource/internal_resource.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme_utils.h"

namespace OHOS::Ace {

class ThemeStyle;
class ThemeUtils;
class StateResource;

using ResRawValue = std::variant<Color, Dimension, int32_t, uint32_t, double, InternalResource::ResourceId, std::string,
    RefPtr<ThemeStyle>, RefPtr<StateResource>>;
using BlendAlpha = std::variant<double, uint32_t>;

enum class ThemeConstantsType {
    // Error type for check if key is match with value
    ERROR = -1,
    COLOR = 0,
    DIMENSION,
    INT,
    DOUBLE,
    RESOURCE_ID,
    REFERENCE,
    REFERENCE_ATTR,
    STRING,
    PATTERN,
    STATE_RESOURCE,
    THEME,
};

struct ResValueWrapper {

    template<typename T>
    std::pair<bool, T> GetValue(const T& errorValue) const
    {
        auto valuePtr = std::get_if<T>(&value);
        if (!valuePtr) {
            return std::make_pair(false, errorValue);
        }
        return std::make_pair(true, *valuePtr);
    }

    ThemeConstantsType type { ThemeConstantsType::COLOR };
    ResRawValue value;
    // Whether style value is public to app.
    bool isPublic { false };
    // Extra alpha needs to blend with color(uint means an ID reference, double means an alpha value).
    BlendAlpha blendAlpha { 1.0 };
};

using ThemeConstantsMap = std::unordered_map<uint32_t, ResValueWrapper>;

class ThemeStyle : public virtual AceType {
    DECLARE_ACE_TYPE(ThemeStyle, AceType);

public:
    ThemeStyle() = default;
    ~ThemeStyle() override
    {
        attributes_.clear();
    }

    void SetName(const std::string& name)
    {
        name_ = name;
    }

    std::string GetName() const
    {
        return name_;
    }

    virtual void ParseContent() {}

    void SetAttributes(const std::unordered_map<std::string, ResValueWrapper>& attributes)
    {
        attributes_ = attributes;
    }

    const std::unordered_map<std::string, ResValueWrapper>& GetAttributes() const
    {
        return attributes_;
    }

    bool HasAttr(const std::string& attr) const
    {
        return attributes_.find(attr) != attributes_.end();
    }

    void SetAttr(const std::string& attr, const ResValueWrapper& value)
    {
        attributes_[attr] = value;
    }

    template<typename T>
    T GetAttr(const std::string& attr, const T& errorValue) const
    {
        auto findIter = attributes_.find(attr);
        if (findIter == attributes_.end()) {
            LOGE("style %{public}s not contains %{public}s!", name_.c_str(), attr.c_str());
            return errorValue;
        }
        const auto& valueWrapper = findIter->second;
        // parse attr reference.
        if (valueWrapper.type == ThemeConstantsType::REFERENCE_ATTR) {
            auto parent = parentStyle_.Upgrade();
            if (!parent) {
                return errorValue;
            }
            auto parseResult = ThemeUtils::ParseThemeIdReference(valueWrapper.GetValue<std::string>("").second);
            if (!parseResult.parseSuccess || parseResult.isIdRef) {
                return errorValue;
            }
            return parent->GetAttr<T>(parseResult.refAttr, errorValue);
        }
        auto valuePair = valueWrapper.GetValue<T>(errorValue);
        if (!valuePair.first) {
            LOGE("style %{public}s get attr %{public}s error!", name_.c_str(), attr.c_str());
        }
        return valuePair.second;
    }

protected:
    std::string name_;
    std::unordered_map<std::string, ResValueWrapper> attributes_;
    WeakPtr<ThemeStyle> parentStyle_;
};

class StateResource : public virtual AceType {
    DECLARE_ACE_TYPE(StateResource, AceType);

public:
    StateResource() = default;
    ~StateResource() override = default;

    void SetName(const std::string& name)
    {
        name_ = name;
    }

    void SetStateValue(uint32_t state, const ResValueWrapper& value)
    {
        states_[state] = value;
    }

    template<typename T>
    T GetState(uint32_t state, const T& defaultValue) const
    {
        auto findIter = states_.find(state);
        if (findIter == states_.end()) {
            return defaultValue;
        }
        auto valuePair = findIter->second.GetValue<T>(defaultValue);
        if (!valuePair.first) {
            LOGE("style %{public}s get state %{public}u error!", name_.c_str(), state);
        }
        return valuePair.second;
    }

private:
    std::string name_;
    std::unordered_map<uint32_t, ResValueWrapper> states_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_THEME_STYLE_H
