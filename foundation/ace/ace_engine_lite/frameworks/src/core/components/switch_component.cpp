/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "switch_component.h"
#include "ace_log.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
SwitchComponent::SwitchComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
      changeListener_(nullptr),
      clickListener_(nullptr)
{
    SetComponentName(K_SWITCH);
}

SwitchComponent::~SwitchComponent()
{
    if (changeListener_) {
        delete changeListener_;
        changeListener_ = nullptr;
    }
    if (clickListener_) {
        delete clickListener_;
        clickListener_ = nullptr;
    }
}

bool SwitchComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    uint16_t stylePropNameId = GetStylePropNameId(style);
    if (!KeyParser::IsKeyValid(stylePropNameId)) {
        return false;
    }

    if (stylePropNameId == K_BACKGROUND_COLOR) {
        uint32_t color = 0;
        uint8_t alpha = OPA_OPAQUE;
        if (GetStyleColorValue(style, color, alpha)) {
            toggleButton_.SetStyle(STYLE_BACKGROUND_COLOR, GetRGBColor(color).full);
            toggleButton_.SetStyle(STYLE_BACKGROUND_OPA, alpha);
            return true;
        }
    }

    return false;
}

bool SwitchComponent::CreateNativeViews()
{
    toggleButton_.SetState(false);
    toggleButton_.SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    toggleButton_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    return true;
}


UIView *SwitchComponent::GetComponentRootView() const
{
    return const_cast<UIToggleButton *>(&toggleButton_);
}

bool SwitchComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    if (attrKeyId == K_CHECKED) {
        toggleButton_.SetState(BoolOf(attrValue));
        return true;
    }

    return false;
}

bool SwitchComponent::RegisterPrivateEventListener(uint16_t eventTypeId,
                                                   jerry_value_t funcValue,
                                                   bool isStopPropagation)
{
    if (eventTypeId == K_CHANGE) {
        changeListener_ = new StateChangeListener(funcValue);
        if (changeListener_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create change listener failed");
            return false;
        }
        return true;
    }
    if (eventTypeId == K_CLICK) {
        clickListener_ = new ViewOnClickListener(GetViewModel(), funcValue, isStopPropagation);
        if (clickListener_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create click listener failed");
            return false;
        }
        return true;
    }
    return false;
}

void SwitchComponent::PostRender()
{
    // the change event should be dealed after checked attribute
    if (changeListener_) {
        changeListener_->SetState(toggleButton_.GetState());
        toggleButton_.SetOnChangeListener(changeListener_);
        if (clickListener_ == nullptr) {
            // trigger changeEvent
            clickListener_ = new ViewOnClickListener(GetViewModel(), UNDEFINED, true);
            if (clickListener_ == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "create click listener failed");
                return;
            }
        }
    }
    if (clickListener_) {
        clickListener_->SetComponentListener(changeListener_);
        toggleButton_.SetOnClickListener(clickListener_);
    }
}
} // namespace ACELite
} // namespace OHOS
