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

#include "stylemgr/app_style_manager.h"
#include "ace_log.h"
#include "component.h"
#include "js_app_context.h"

namespace OHOS {
namespace ACELite {
const char * const AppStyleManager::ID = "id";
const char * const AppStyleManager::ATTR_STATIC_STYLE = "staticStyle";
const char * const AppStyleManager::ATTR_DYNAMIC_STYLE = "dynamicStyle";
const char * const AppStyleManager::ATTR_STATIC_CLASS = "staticClass";
AppStyleManager::AppStyleManager() : styleSheet_(nullptr), flags_(nullptr)
{
    const uint8_t divisor = 2;
    flagsLen_ = (KEYWORDS_MAX & 1) ? (KEYWORDS_MAX / divisor) + 1 : (KEYWORDS_MAX / divisor);
    // prepare jerry string value in advance to less the overhead
    idStrValue_ = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ID));
    attrsStrValue_ = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ATTR_ATTRS));
    staticStyleStrValue_ = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ATTR_STATIC_STYLE));
    dynamicStyleStrValue_ = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ATTR_DYNAMIC_STYLE));
    staticClassStrValue_ = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ATTR_STATIC_CLASS));
}

AppStyleManager::~AppStyleManager()
{
    ACE_FREE(flags_);
    if (styleSheet_ != nullptr) {
        styleSheet_->Reset();
        delete styleSheet_;
        styleSheet_ = nullptr;
    }
    ReleaseJerryValue(idStrValue_, attrsStrValue_, dynamicStyleStrValue_, staticStyleStrValue_, staticClassStrValue_,
                      VA_ARG_END_FLAG);
}

void AppStyleManager::Prepare()
{
    if (flags_ != nullptr) {
        return;
    }
    if (flagsLen_ == 0) {
        return;
    }

    flags_ = static_cast<StyleFlag *>(ace_malloc(flagsLen_ * sizeof(StyleFlag)));
    if (flags_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc flags data buffer failed");
    }
}

const AppStyleSheet *AppStyleManager::GetStyleSheet()
{
    return styleSheet_;
}

void AppStyleManager::ApplyComponentStyles(const jerry_value_t options, Component& curr)
{
    ResetFlags();
    HandleStaticStyle(options, curr);
    HandleDynamicStyle(options, curr);
    HandleIDSelectors(options, curr);
    HandleClassSelectors(options, curr);
}

void AppStyleManager::ResetFlags()
{
    if (flags_ == nullptr) {
        return;
    }

    for (uint16_t i = 0; i < flagsLen_; i++) {
        flags_[i].u8Byte = 0;
    }
}

void AppStyleManager::InitStyleSheet(jerry_value_t jsStyleSheetObj)
{
    if (jerry_value_is_undefined(jsStyleSheetObj) || jerry_value_is_error(jsStyleSheetObj)) {
        return;
    }

    if (styleSheet_ == nullptr) {
        styleSheet_ = new AppStyleSheet();
    }

    if (styleSheet_ != nullptr) {
        styleSheet_->InitSheet(jsStyleSheetObj);
    }
}

// return value must be released by caller
jerry_value_t AppStyleManager::GetStyleObjFromOptions(jerry_value_t options, const jerry_value_t name)
{
    jerry_value_t value = UNDEFINED;
    if (jerry_value_is_undefined(options) || jerry_value_is_undefined(name)) {
        return value;
    }

    jerry_value_t valueContainer = options;
    bool containerNeedReleased = false;
    if (name == idStrValue_) {
        // get attrs object from options
        if (!HasOwnProperty(options, attrsStrValue_)) {
            // if no attrs in options, return undefined directly
            return value;
        }
        valueContainer = jerry_get_property(options, attrsStrValue_);
        // mark that the valueContainer need to be released when getting out of scope
        containerNeedReleased = true;
    }

    // check if container object have target property, if not, just return undefined
    if (HasOwnProperty(valueContainer, name)) {
        value = jerry_get_property(valueContainer, name);
    }

    if (containerNeedReleased) {
        jerry_release_value(valueContainer);
    }
    return value;
}

void AppStyleManager::MarkStandardFlag(uint16_t styleItemNameId)
{
    uint16_t index = styleItemNameId >> 1;
    if (styleItemNameId & 1) {
        flags_[index].u8Bits.standardEven_ = 1;
    } else {
        flags_[index].u8Bits.standardOdd_ = 1;
    }
}

void AppStyleManager::HandleStaticStyle(const jerry_value_t options, Component &curr)
{
    jerry_value_t staticStyleValue = GetStyleObjFromOptions(options, staticStyleStrValue_);
    if (jerry_value_is_undefined(staticStyleValue)) {
        return;
    }

    jerry_value_t propKeys = jerry_get_object_keys(staticStyleValue);
    uint32_t propKeySize = jerry_get_array_length(propKeys);
    for (uint32_t index = 0; index < propKeySize; index++) {
        jerry_value_t propKey = jerry_get_property_by_index(propKeys, index);
        jerry_value_t propValue = jerry_get_property(staticStyleValue, propKey);
        AppStyleItem *newStyleItem = AppStyleItem::GenerateFromJSValue(propKey, propValue);
        if (newStyleItem != nullptr) {
            curr.ApplyStyle(newStyleItem);
            MarkStandardFlag(newStyleItem->GetPropNameId());
            delete newStyleItem;
            newStyleItem = nullptr;
        }
        ReleaseJerryValue(propKey, propValue, VA_ARG_END_FLAG);
    }
    ReleaseJerryValue(propKeys, staticStyleValue, VA_ARG_END_FLAG);
}

void AppStyleManager::HandleDynamicStyle(const jerry_value_t options, Component &curr)
{
    jerry_value_t dynamicStyleValue = GetStyleObjFromOptions(options, dynamicStyleStrValue_);
    if (jerry_value_is_undefined(dynamicStyleValue)) {
        return;
    }

    jerry_value_t propKeys = jerry_get_object_keys(dynamicStyleValue);
    uint32_t propKeySize = jerry_get_array_length(propKeys);
    for (uint32_t index = 0; index < propKeySize; index++) {
        jerry_value_t propKey = jerry_get_property_by_index(propKeys, index);
        jerry_value_t propValue = jerry_get_property(dynamicStyleValue, propKey);
        // parse expression and new watcher for it
        if (jerry_value_is_function(propValue)) {
            jerry_value_t expressionValue;
            if (curr.IsFreeze()) {
                expressionValue = JSFunction::Call(propValue, curr.GetViewModel(), nullptr, 0);
            } else {
#if (FEATURE_LAZY_LOADING_MODULE == 1)
                expressionValue = CallJSFunction(propValue, curr.GetNativeElement(), nullptr, 0);
                JsAppContext *context = JsAppContext::GetInstance();
                LazyLoadManager *lazyLoadManager = const_cast<LazyLoadManager *>(context->GetLazyLoadManager());
                lazyLoadManager->AddLazyLoadWatcher(curr.GetNativeElement(), propKey, propValue);
#else
                expressionValue = curr.AddWatcherItem(propKey, propValue);
#endif
            }
            // the expression's calculating result should be used, not the function value itself
            AppStyleItem *newStyleItem = AppStyleItem::GenerateFromJSValue(propKey, expressionValue);
            if (newStyleItem != nullptr) {
                curr.ApplyStyle(newStyleItem);
                MarkStandardFlag(newStyleItem->GetPropNameId());
                delete newStyleItem;
                newStyleItem = nullptr;
            }
            if (expressionValue != propValue) {
                // generally, ParseExpression() will calculate the expression function's value and return
                // the result back, so it's need to be released
                jerry_release_value(expressionValue);
            }
        }
        ReleaseJerryValue(propKey, propValue, VA_ARG_END_FLAG);
    }
    ReleaseJerryValue(propKeys, dynamicStyleValue, VA_ARG_END_FLAG);
}

void AppStyleManager::HandleSingleSelector(const jerry_value_t prop, const jerry_value_t type, Component &curr)
{
    if ((styleSheet_ == nullptr) || (jerry_value_is_undefined(prop)) || (jerry_value_is_undefined(type))) {
        return;
    }

    uint16_t strLen = 0;
    char *valueBuffer = MallocStringOf(prop, &strLen);
    if (valueBuffer == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "convert id value to char failed, will be dropped");
        return;
    }

    do {
        // malloc successfully but length is not correct, give up
        if (strLen == 0) {
            break;
        }

        AppStyle *style = nullptr;
        if (type == idStrValue_) {
            style = styleSheet_->GetStyleFromIDSelectors(valueBuffer);
        } else if (type == staticClassStrValue_) {
            style = styleSheet_->GetStyleFromClassSelectors(valueBuffer);
        } else {
            HILOG_WARN(HILOG_MODULE_ACE, "not supported yet");
        }
        if (style == nullptr) {
            // malloc failed or not supported
            break;
        }
        ApplySingleStyle(*style, curr);
    } while (0);

    // free value buffer
    ace_free(valueBuffer);
    valueBuffer = nullptr;
}

void AppStyleManager::HandleSelectors(const jerry_value_t options, const jerry_value_t type, Component &curr)
{
    if (styleSheet_ == nullptr) {
        return;
    }

    jerry_value_t propValue = GetStyleObjFromOptions(options, type);
    if (jerry_value_is_undefined(propValue)) {
        return;
    }

    if (!jerry_value_is_array(propValue)) {
        // if is not array, just handle it
        HandleSingleSelector(propValue, type, curr);
    } else {
        // support array
        uint32_t propCount = jerry_get_array_length(propValue);
        if (propCount > 0 && propCount < UINT8_MAX) {
            // do reverse traversal to ensure style override correctly
            while (propCount > 0) {
                jerry_value_t property = jerry_get_property_by_index(propValue, (propCount - 1));
                HandleSingleSelector(property, type, curr);
                jerry_release_value(property);
                propCount--;
            }
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "invalid prop count");
        }
    }
    jerry_release_value(propValue);
}

void AppStyleManager::HandleIDSelectors(const jerry_value_t options, Component &curr)
{
    HandleSelectors(options, idStrValue_, curr);
}

void AppStyleManager::HandleClassSelectors(const jerry_value_t options, Component &curr)
{
    HandleSelectors(options, staticClassStrValue_, curr);
}

bool AppStyleManager::isStandardExist(uint16_t styleItemNameId)
{
    uint16_t index = styleItemNameId >> 1;
    if (styleItemNameId & 1) {
        return flags_[index].u8Bits.standardEven_;
    }

    return  flags_[index].u8Bits.standardOdd_;
}

bool AppStyleManager::isPseudoExist(uint16_t styleItemNameId, uint16_t type)
{
    uint16_t index = styleItemNameId >> 1;
    if (styleItemNameId & 1) {
        if (type == PSEUDO_CLASS_ACTIVE) {
            return flags_[index].u8Bits.pseudoActiveEven_;
        }
        return flags_[index].u8Bits.pseudoCheckedEven_;
    }
    if (type == PSEUDO_CLASS_ACTIVE) {
        return flags_[index].u8Bits.pseudoActiveOdd_;
    }
    return flags_[index].u8Bits.pseudoCheckedOdd_;
}

void AppStyleManager::MarkPseudoFlag(uint16_t styleItemNameId, uint16_t type)
{
    uint16_t index = styleItemNameId >> 1;
    if (styleItemNameId & 1) {
        if (type == PSEUDO_CLASS_ACTIVE) {
            flags_[index].u8Bits.pseudoActiveEven_ = 1;
        } else {
            flags_[index].u8Bits.pseudoCheckedEven_ = 1;
        }
    } else {
        if (type == PSEUDO_CLASS_ACTIVE) {
            flags_[index].u8Bits.pseudoActiveOdd_ = 1;
        } else {
            flags_[index].u8Bits.pseudoCheckedOdd_ = 1;
        }
    }
}

void AppStyleManager::ApplySingleStyle(AppStyle &appStyle, Component &curr)
{
    // go through id style's all items, apply it to component
    const AppStyleItem *styleItem = appStyle.GetFirst();
    while (styleItem != nullptr) {
        uint16_t styleItemNameId = styleItem->GetPropNameId();
        uint8_t pseudoClassType = styleItem->GetPseudoClassType();
        // inline style not exists and pseudo class type is normal, set normal style
        if (pseudoClassType == 0 && !isStandardExist(styleItemNameId)) {
            MarkStandardFlag(styleItemNameId);
            curr.ApplyStyle(styleItem);
            styleItem = styleItem->GetNext();
            continue;
        }
        // pseudo class type exists and no other same pseudo style has been applied, set pseudo class style
        if (pseudoClassType != 0 && !isPseudoExist(styleItemNameId, pseudoClassType)) {
            MarkPseudoFlag(styleItemNameId, pseudoClassType);
            curr.ApplyStyle(styleItem);
        }
        styleItem = styleItem->GetNext();
    }
}
} // namespace ACELite
} // namespace OHOS
