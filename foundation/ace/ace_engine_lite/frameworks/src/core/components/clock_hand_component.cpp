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

#include "acelite_config.h"

#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)

#include "ace_log.h"
#include "ace_mem_base.h"
#include "clock_hand_component.h"
#include "js_app_context.h"
#include "key_parser.h"
#include "keys.h"
#include "ui_image_view.h"

namespace OHOS {
namespace ACELite {
ClockHandComponent::ClockHandComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
      options_(options),
      layoutType_(LayoutType::IMAGE),
      clockHandView_(nullptr),
      pivotX_(0),
      pivotY_(0),
      fillColor_(0),
      opacity_(255),
      handType_(nullptr),
      posX_(0),
      posY_(0),
      width_(0),
      height_(0)
{
    SetComponentName(K_CLOCK_HAND);
}

bool ClockHandComponent::CreateNativeViews()
{
    // Get clock hand layout type
    layoutType_ = GetLayoutType();
    if (layoutType_ == LayoutType::RECT) {
        clockHandView_ = new UIView();
    } else {
        clockHandView_ = new UIImageView();
    }

    if (clockHandView_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent: create native view failed!");
        return false;
    }
    return true;
}

void ClockHandComponent::ReleaseNativeViews()
{
    if (clockHandView_) {
        delete clockHandView_;
        clockHandView_ = nullptr;
    }

    if (handType_) {
        ace_free(handType_);
        handType_ = nullptr;
    }
}

inline UIView *ClockHandComponent::GetComponentRootView() const
{
    return clockHandView_;
}

void ClockHandComponent::PostRender()
{
    const char * const stylePivotX = "pivot-x";
    const char * const stylePivotY = "pivot-y";

    // Verify hand type
    if ((handType_ == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent: hand type invalid!");
        return;
    }

    // Set params to current js object
    jerry_value_t pivotXValue = jerry_create_number(pivotX_);
    jerry_value_t pivotYValue = jerry_create_number(pivotY_);
    uint16_t typeId = KeyParser::ParseKeyId(handType_, strlen(handType_));
    jerry_value_t typeIdValue = jerry_create_number(typeId);

    jerry_value_t nativeObj = GetNativeElement();
    if (jerry_value_is_undefined(nativeObj)) {
        ReleaseJerryValue(pivotXValue, pivotYValue, typeIdValue, VA_ARG_END_FLAG);
        return;
    }
    SetNamedProperty(nativeObj, stylePivotX, pivotXValue);
    SetNamedProperty(nativeObj, stylePivotY, pivotYValue);
    SetNamedProperty(nativeObj, ATTR_TYPE, typeIdValue);

    jerry_value_t isImage;
    if (layoutType_ == LayoutType::RECT) {
        isImage = jerry_create_boolean(false);
        jerry_value_t fillColorValue = jerry_create_number(fillColor_);
        SetNamedProperty(nativeObj, COMMON_STYLE_COLOR, fillColorValue);
        jerry_value_t opacityValue = jerry_create_number(opacity_);
        SetNamedProperty(nativeObj, COMMON_STYLE_OPACITY, opacityValue);
    } else {
        isImage = jerry_create_boolean(true);
    }
    SetNamedProperty(nativeObj, CLOCK_HAND_IS_IMAGE, isImage);
}

bool ClockHandComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    uint16_t styleNameId = GetStylePropNameId(style);
    if (!KeyParser::IsKeyValid(styleNameId)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent: id invalid when apply private style!");
        return false;
    }

    switch (styleNameId) {
        case K_PIVOT_X:
            pivotX_ = GetStyleNumValue(style);
            break;
        case K_PIVOT_Y:
            pivotY_ = GetStyleNumValue(style);
            break;
        case K_COLOR:
            fillColor_ = GetStyleNumValue(style);
            break;
        case K_OPACITY:
            opacity_ = GetStyleNumValue(style);
            break;
        case K_BES_HAND_HEIGHT:
            height_ = GetStyleNumValue(style);
            SetViewHeightOrWidth(false);
            break;
        case K_BES_HAND_WIDTH:
            width_= GetStyleNumValue(style);
            SetViewHeightOrWidth(true);
            break;
        default:
            HILOG_DEBUG(HILOG_MODULE_ACE, "ClockHandComponent: style invalid when apply private style!");
            return false;
    }
    return true;
}

void ClockHandComponent::SetViewHeightOrWidth(bool isWidth){
    if(clockHandView_ == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent:clockHandView: is nullptr");
        return;
    }
    if(isWidth){
        clockHandView_->SetWidth(width_);
    }else{
        clockHandView_->SetHeight(height_);
    }
}

bool ClockHandComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    if ((clockHandView_ == nullptr) || !KeyParser::IsKeyValid(attrKeyId) || IS_UNDEFINED(attrValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent: set private attribute failed!");
        return false;
    }

    bool setResult = true;
    switch (attrKeyId) {
        case K_SRC: {
            uint16_t length = 0;
            char *attrValueStr = MallocStringOf(attrValue, &length);
            if (attrValueStr == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent: image path invalid!");
                setResult = false;
                break;
            }

            if (length != 0) {
                UIImageView *image = reinterpret_cast<UIImageView *>(clockHandView_);
                if (image == nullptr) {
                    ace_free(attrValueStr);
                    attrValueStr = nullptr;
                    HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent: reinterpret_cast UIImageView failed!");
                    setResult = false;
                    break;
                }
                char *imagePath = const_cast<char *>(ParseImageSrc(attrValue));
                if (imagePath != nullptr) {
                    image->SetSrc(imagePath);
                    ace_free(imagePath);
                    imagePath = nullptr;
                }
            }

            ace_free(attrValueStr);
            attrValueStr = nullptr;
            break;
        }
        case K_TYPE:
            ACE_FREE(handType_);
            handType_ = MallocStringOf(attrValue);
            break;
        case K_BES_LAYOUT_TYPE:
            ace_free(layoutTypeStr_);
            layoutTypeStr_ = MallocStringOf(attrValue);
            SetLayoutType();
            break;
        case K_BES_POS_X:
            posX_ = IntegerOf(attrValue);
            SetViewPosition(true);
            break;
        case K_BES_POS_Y:
            posY_ = IntegerOf(attrValue);
            SetViewPosition(false);
            break;
        default:
            setResult = false;
            break;
    }
    return setResult;
}

void ClockHandComponent::SetViewPosition(bool isPosX){
    if(clockHandView_ == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent:clockHandView: is nullptr");
        return;
    }
    if(layoutType_ == LayoutType::IMAGE){
       UIImageView *image = reinterpret_cast<UIImageView *>(clockHandView_);
       if (image == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent: reinterpret_cast UIImageView failed!");          
        return;
       }
       if(isPosX){
           image->SetX(posX_);
       }else{
           image->SetY(posY_);
       } 
    }else{
        if(isPosX){
           clockHandView_->SetX(posX_);
        }else{
           clockHandView_->SetY(posY_);
        } 
    
    }
}

void ClockHandComponent::SetLayoutType()
{
    if(layoutTypeStr_ == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "SetLayoutType: layoutTypeStr is nullptr");
        return;
    }
    if("rect" == layoutTypeStr_){
        layoutType_ = LayoutType::RECT;
    }else{
        layoutType_ = LayoutType::IMAGE;
    }

    if(layoutTypeStr_ != nullptr){
        ace_free(layoutTypeStr_);
        layoutTypeStr_ = nullptr;
    }
}

void ClockHandComponent::SetNamedProperty(jerry_value_t obj, const char * const name, jerry_value_t value)
{
    jerry_value_t propName = jerry_create_string((const jerry_char_t *)name);
    jerry_release_value(jerry_set_property(obj, propName, value));
    jerry_release_value(propName);
    jerry_release_value(value);
}

char *ClockHandComponent::GetStringAttrByName(const char * const name)
{
    if (jerry_value_is_undefined(options_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ClockHandComponent: options undefined!");
        return nullptr;
    }

    char *strValue = nullptr;
    jerry_value_t attrsPropName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ATTR_ATTRS));
    jerry_value_t attrsPropValue = jerry_get_property(options_, attrsPropName);
    if (!jerry_value_is_undefined(attrsPropValue)) {
        jerry_value_t attrPropName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(name));
        jerry_value_t attrPropValue = jerry_get_property(attrsPropValue, attrPropName);
        if (jerry_value_is_string(attrPropValue)) {
            strValue = MallocStringOf(attrPropValue);
        } else {
            HILOG_INFO(HILOG_MODULE_ACE, "ClockHandComponent: attribute: %{public}s does not exits", name);
        }
        jerry_release_value(attrPropValue);
        jerry_release_value(attrPropName);
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "ClockHandComponent: attrs undefined");
    }
    jerry_release_value(attrsPropValue);
    jerry_release_value(attrsPropName);

    return strValue;
}

ClockHandComponent::LayoutType ClockHandComponent::GetLayoutType()
{
    LayoutType type = LayoutType::RECT;
    char *handSrc = GetStringAttrByName(ATTR_SRC);
    if (handSrc != nullptr) {
        type = LayoutType::IMAGE;
        ace_free(handSrc);
        handSrc = nullptr;
    }
    return type;
}
} // namespace ACELite
} // namespace OHOS

#endif // FEATURE_COMPONENT_ANALOG_CLOCK
