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

#include "input_button_component.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "font/ui_font_header.h"
#include "js_app_context.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"
#include "product_adapter.h"
#include "ui_button.h"

namespace OHOS {
namespace ACELite {
bool InputButtonComponent::CreateNativeViews()
{
    fontSize_ = ProductAdapter::GetDefaultFontSize();
    // set font family
    if (!CopyFontFamily(fontFamily_, ProductAdapter::GetDefaultFontFamilyName())) {
        return false;
    }
    const uint8_t defaultWidth = 100;
    const uint8_t defaultHeight = 50;
    button_.SetWidth(defaultWidth);
    button_.SetHeight(defaultHeight);
    button_.SetStateForStyle(UIButton::ButtonState::PRESSED);
    button_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    button_.SetStateForStyle(UIButton::ButtonState::RELEASED);
    button_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    return true;
}

void InputButtonComponent::ReleaseNativeViews()
{
    if (clickListener_ != nullptr) {
        delete clickListener_;
        clickListener_ = nullptr;
    }
    ACE_FREE(textValue_);
    ACE_FREE(fontFamily_);
    ACE_FREE(normalBackGroundImg_);
    ACE_FREE(pressedBackGroundImg_);
}

bool InputButtonComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    switch (attrKeyId) {
        case K_VALUE:
            ACE_FREE(textValue_);
            textValue_ = MallocStringOf(attrValue);
            return true;
        default:
            return false;
    }
}

bool InputButtonComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    bool result = true;
    uint16_t styleKey = GetStylePropNameId(style);
    switch (styleKey) {
        case K_FONT_SIZE:
            result = SetFontSize(style);
            break;
        case K_FONT_FAMILY:
            if (IsStyleValueTypeString(style)) {
                // the style would be freed after this method
                result = CopyFontFamily(fontFamily_, GetStyleStrValue(style));
            } else {
                result = false;
            }
            break;
        case K_TEXT_ALIGN:
            result = SetTextAlign(style);
            break;
        case K_COLOR:
            result = SetColor(*style);
            break;
        case K_BACKGROUND_COLOR:
            result = SetBackgroundColor(*style);
            break;
        case K_BACKGROUND_IMAGE:
            result = HandleBackgroundImg(*style, pressedBackGroundImg_, normalBackGroundImg_);
            break;
        case K_PADDING:
        // fall through
        case K_PADDING_BOTTOM:
        // fall through
        case K_PADDING_LEFT:
        // fall through
        case K_PADDING_RIGHT:
        // fall through
        case K_PADDING_TOP:
        // fall through
        case K_BORDER_BOTTOM_WIDTH:
        // fall through
        case K_BORDER_LEFT_WIDTH:
        // fall through
        case K_BORDER_RIGHT_WIDTH:
        // fall through
        case K_BORDER_TOP_WIDTH:
        // fall through
        case K_BORDER_WIDTH:
        // fall through
        case K_BORDER_BOTTOM_COLOR:
        // fall through
        case K_BORDER_LEFT_COLOR:
        // fall through
        case K_BORDER_RIGHT_COLOR:
        // fall through
        case K_BORDER_TOP_COLOR:
        // fall through
        case K_BORDER_COLOR:
        // fall through
        case K_BORDER_RADIUS: {
            button_.SetStateForStyle(UIButton::ButtonState::PRESSED);
            bool pressResult = ApplyCommonStyle(button_, style);
            button_.SetStateForStyle(UIButton::ButtonState::RELEASED);
            bool releaseResult = ApplyCommonStyle(button_, style);
            result = pressResult && releaseResult;
            break;
        }
        default:
            result = false;
            break;
    }
    return result;
}

bool InputButtonComponent::SetBackgroundColor(const AppStyleItem &style)
{
    if (style.GetPseudoClassType() != PSEUDO_CLASS_ACTIVE) {
        // let parent handle the normal state background color style,
        // will be set for released state style of ui button
        button_.SetStateForStyle(UIButton::ButtonState::PRESSED);
        Component::SetBackgroundColor(button_, style);
        button_.SetStateForStyle(UIButton::ButtonState::RELEASED);
        Component::SetBackgroundColor(button_, style);
        return true;
    }
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    if (!GetStyleColorValue(&style, color, alpha)) {
        return false;
    }
    pressedBackGroundColorValue_ = color;
    pressedOpacityValue_ = alpha;
    return true;
}

bool InputButtonComponent::SetColor(const AppStyleItem &style)
{
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    if (GetStyleColorValue(&style, color, alpha)) {
        button_.SetLabelStyle(STYLE_TEXT_COLOR, GetRGBColor(color).full);
        button_.SetLabelStyle(STYLE_TEXT_OPA, alpha);
        return true;
    }
    return false;
}

bool InputButtonComponent::SetTextAlign(const AppStyleItem *style)
{
    if ((!IsStyleValueTypeString(style)) || (!GetStyleStrValue(style))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the type of style align is error");
        return false;
    }
    uint16_t valueId = KeyParser::ParseKeyId(GetStyleStrValue(style), GetStyleStrValueLen(style));
    switch (valueId) {
        case K_LEFT:
            button_.SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT);
            return true;
        case K_CENTER:
            button_.SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
            return true;
        case K_RIGHT:
            button_.SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT);
            return true;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "the value of text-align in input component is error");
            return false;
    }
}

bool InputButtonComponent::SetFontSize(const AppStyleItem *style)
{
    fontSize_ = GetStylePixelValue(style);
    return true;
}

void InputButtonComponent::PostRender()
{
    if (textValue_ != nullptr && fontFamily_ != nullptr) {
        button_.SetFont(fontFamily_, fontSize_);
        button_.SetText(textValue_);
    }
    if (clickListener_ != nullptr) {
        button_.SetOnClickListener(clickListener_);
    }
    if (pressedBackGroundColorValue_ >= 0) {
        button_.SetStateForStyle(UIButton::ButtonState::PRESSED);
        button_.SetStyle(STYLE_BACKGROUND_COLOR, GetRGBColor(pressedBackGroundColorValue_).full);
        button_.SetStyle(STYLE_BACKGROUND_OPA, pressedOpacityValue_);
    }
    if (normalBackGroundImg_ != nullptr || pressedBackGroundImg_ != nullptr) {
        // make sure the normal and pressed image same in case user only set one of them
        char *normalImg = (normalBackGroundImg_ == nullptr) ? pressedBackGroundImg_ : normalBackGroundImg_;
        char *pressedImg = (pressedBackGroundImg_ == nullptr) ? normalBackGroundImg_ : pressedBackGroundImg_;
        button_.SetImageSrc(normalImg, pressedImg);
    }
    ACE_FREE(normalBackGroundImg_);
    ACE_FREE(pressedBackGroundImg_);
}

void InputButtonComponent::PostUpdate(uint16_t attrKeyId)
{
    if (!KeyParser::IsKeyValid(attrKeyId)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "input component post update check args failed");
        return;
    }
    switch (attrKeyId) {
        case K_VALUE:
            if (textValue_ != nullptr) {
                button_.SetText(textValue_);
            }
            break;
        case K_FONT_SIZE:
        case K_FONT_FAMILY:
            if (fontFamily_ != nullptr) {
                button_.SetFont(fontFamily_, fontSize_);
            }
            break;
        default:
            break;
    }
}
} // namespace ACELite
} // namespace OHOS
