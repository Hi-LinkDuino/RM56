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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_ATTRIBUTE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_ATTRIBUTE_H

#include "core/components/common/layout/constants.h"
#include "core/event/multimodal/voice_event.h"
#include "frameworks/core/components/transform/click_spring_effect.h"

namespace OHOS::Ace {

enum class AttributeTag {
    COMMON_ATTR = 0,
    COMMON_DISABLED_ATTR,
    COMMON_FOCUSABLE_ATTR,
    COMMON_TOUCHABLE_ATTR,
    COMMON_DATA_ATTR,
    COMMON_CLICK_EFFECT_ATTR,
    COMMON_RENDER_ATTR,
    COMMON_MULTIMODAL_ATTR,
    SPECIALIZED_ATTR,
    UNKNOWN,
    DEFAULT,
};

struct Attribute {
    bool IsValid() const
    {
        return tag != AttributeTag::UNKNOWN;
    }

    bool IsShared() const
    {
        return isShared;
    }

    bool isShared = true;
    AttributeTag tag = AttributeTag::DEFAULT;
};

struct CommonAttribute : Attribute {
    std::string id;
    std::string style;
    std::string className;
    std::string ref;
    bool isRightToLeft = false;
    TextDirection direction = TextDirection::AUTO;
};

struct CommonDisabledAttribute : Attribute {
    bool disabled = false;
};

struct CommonFocusableAttribute : Attribute {
    std::pair<bool, bool> focusable = { false, false };
};

struct CommonTouchableAttribute : Attribute {
    bool touchable = true;
};

struct CommonDataAttribute : Attribute {
    std::string data;
};

struct CommonClickEffectAttribute : Attribute {
    ClickSpringEffectType clickEffect = ClickSpringEffectType::NONE;
};

struct CommonRenderAttribute : Attribute {
    std::string forAttr;
    std::string ifAttr;
    std::string show;
};

struct CommonMultimodalAttribute : Attribute {
    std::string voiceLabel;
    std::string subscriptLabel;
    bool useSubscript = false;
    SceneLabel scene = SceneLabel::PAGE;

    bool IsUnavailable() const
    {
        return voiceLabel.empty() && !useSubscript;
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_ATTRIBUTE_H
