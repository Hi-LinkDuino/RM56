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

#include "input_checkbox_component.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "js_app_context.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
InputCheckboxComponent::~InputCheckboxComponent()
{
    if (changeListener_ != nullptr) {
        delete changeListener_;
        changeListener_ = nullptr;
    }
    if (clickListener_ != nullptr) {
        delete clickListener_;
        clickListener_ = nullptr;
    }
    ACE_FREE(normalBackGroundImg_);
    ACE_FREE(pressedBackGroundImg_);
}

bool InputCheckboxComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    switch (attrKeyId) {
        case K_CHECKED:
            // set the state of checkbox and radioButton
            if (jerry_value_is_boolean(attrValue)) {
                bool checked = jerry_get_boolean_value(attrValue);
                UICheckBox::UICheckBoxState state =
                    checked ? UICheckBox::UICheckBoxState::SELECTED : UICheckBox::UICheckBoxState::UNSELECTED;
                checkbox_.SetState(state);
                return true;
            }
            HILOG_ERROR(HILOG_MODULE_ACE, "the checkbox checked is error value");
            return false;
        default:
            return false;
    }
}

bool InputCheckboxComponent::RegisterPrivateEventListener(uint16_t eventTypeId,
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
    HILOG_DEBUG(HILOG_MODULE_ACE, "go to common click event");
    return false;
}

bool InputCheckboxComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    uint16_t styleKey = GetStylePropNameId(style);
    if (styleKey == K_BACKGROUND_IMAGE) {
        return HandleBackgroundImg(*style, pressedBackGroundImg_, normalBackGroundImg_);
    }

    return false;
}

void InputCheckboxComponent::PostRender()
{
    DealEvent();
    ACE_FREE(normalBackGroundImg_);
    ACE_FREE(pressedBackGroundImg_);
}

void InputCheckboxComponent::DealEvent()
{
    if (normalBackGroundImg_ != nullptr || pressedBackGroundImg_ != nullptr) {
        // make sure the normal and pressed image same in case user only set one of them
        char *normalImg = (normalBackGroundImg_ == nullptr) ? pressedBackGroundImg_ : normalBackGroundImg_;
        char *pressedImg = (pressedBackGroundImg_ == nullptr) ? normalBackGroundImg_ : pressedBackGroundImg_;
        checkbox_.SetImages(pressedImg, normalImg);
    }
    if ((clickListener_ == nullptr) && (changeListener_ == nullptr)) {
        return;
    }
    if (changeListener_ != nullptr) {
        bool state = (checkbox_.GetState() == UICheckBox::UICheckBoxState::SELECTED);
        changeListener_->SetState(state);
        checkbox_.SetOnChangeListener(changeListener_);
    }

    if (clickListener_ == nullptr) {
        // trigger changeEvent
        clickListener_ = new ViewOnClickListener(GetViewModel(), UNDEFINED, true);
        if (clickListener_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create click listener failed");
            return;
        }
    }

    clickListener_->SetComponentListener(changeListener_);
    checkbox_.SetOnClickListener(clickListener_);
}
} // namespace ACELite
} // namespace OHOS
