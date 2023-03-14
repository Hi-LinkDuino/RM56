/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "stylemgr/app_style_sheet.h"
#include "condition_arbitrator.h"
#include "js_fwk_common.h"
#include "scope_js_value.h"

namespace OHOS {
namespace ACELite {
void AppStyleSheet::Reset()
{
    if (idSelectors_ != nullptr) {
        idSelectors_->Reset();
        delete idSelectors_;
        idSelectors_ = nullptr;
    }

    if (classSelectors_ != nullptr) {
        classSelectors_->Reset();
        delete classSelectors_;
        classSelectors_ = nullptr;
    }

    if (keyFrameSelectors_ != nullptr) {
        keyFrameSelectors_->Reset();
        delete keyFrameSelectors_;
        keyFrameSelectors_ = nullptr;
    }
}

void AppStyleSheet::InitSheet(jerry_value_t styleSheetObj)
{
    if (jerry_value_is_undefined(styleSheetObj)) {
        return;
    }

    // clear previous style sheet list
    Reset();

    // initializa all media query selectors first, as all media query selectors have higher priority
    InitMediaSelectors(styleSheetObj);

    // initialize all other normal selectors
    InitNormalSelectors(styleSheetObj);
}

void AppStyleSheet::InitMediaSelectors(const jerry_value_t styleSheetObj)
{
    // check if media query exists in style sheet
    const char * const mediaQueryArrayKeyName = "@media";
    jerry_value_t propNameV =
            jerry_create_string(reinterpret_cast<jerry_char_t *>(const_cast<char *>(mediaQueryArrayKeyName)));
    ScopeJSValue propNameP(propNameV);
    if (!HasOwnProperty(styleSheetObj, propNameV)) {
        // no media querry items
        return;
    }

    ScopeJSValue mediaQueryArrayValue(jerry_get_property(styleSheetObj, propNameV));
    if (!jerry_value_is_array(mediaQueryArrayValue.Obain())) {
        return;
    }

    uint32_t conditionCounts = jerry_get_array_length(mediaQueryArrayValue.Obain());
    const uint32_t maxConditionCount = 256;
    if (conditionCounts == 0 || conditionCounts > maxConditionCount) {
        return;
    }
    for (uint32_t index = 0; index < conditionCounts; index++) {
        // get the array item @ index position
        jerry_value_t mediaArrayItemV = jerry_get_property_by_index(mediaQueryArrayValue.Obain(), index);
        ScopeJSValue mediaArrayItemP(mediaArrayItemV);
        // handle single one
        HandleSingleMediaItem(mediaArrayItemV);
    }
}

void AppStyleSheet::HandleSingleMediaItem(const jerry_value_t mediaItem)
{
    const char * const conditionKeyName = "condition";
    jerry_value_t conditionKeyV = jerry_create_string(reinterpret_cast<const jerry_char_t *>(conditionKeyName));
    ScopeJSValue conditionKeyP(conditionKeyV);
    if (!JerryHasProperty(mediaItem, conditionKeyV)) {
        return;
    }
    // get condition string
    jerry_value_t conditionStrJSValue = jerry_get_property(mediaItem, conditionKeyV);
    ScopeJSValue autoReleaseV(conditionStrJSValue);
    char *conditionStr = MallocStringOf(conditionStrJSValue);
    if (conditionStr == nullptr) {
        return;
    }
    ConditionArbitrator arbitrator;
    bool isMatched = arbitrator.Decide(conditionStr);
    ace_free(conditionStr);
    conditionStr = nullptr;
    if (!isMatched) {
        // the current media item is not matching with current device environment
        return;
    }

    // matched, parse the item through normal way
    InitNormalSelectors(mediaItem, true);
}

void AppStyleSheet::InitNormalSelectors(const jerry_value_t styleSheetObj, bool overwrite)
{
    // init all id selectors
    const char * const attrIdSelectors = "idSelectors";
    jerry_value_t propName = jerry_create_string(reinterpret_cast<jerry_char_t *>(const_cast<char *>(attrIdSelectors)));
    jerry_value_t propValue = UNDEFINED;
    if (HasOwnProperty(styleSheetObj, propName)) {
        propValue = jerry_get_property(styleSheetObj, propName);
        InitSelectors(&idSelectors_, propValue, false, overwrite);
        jerry_release_value(propValue);
    }
    jerry_release_value(propName);

    // init all class selectors
    const char * const attrClassSelectors = "classSelectors";
    propName = jerry_create_string(reinterpret_cast<jerry_char_t *>(const_cast<char *>(attrClassSelectors)));
    if (HasOwnProperty(styleSheetObj, propName)) {
        propValue = jerry_get_property(styleSheetObj, propName);
        InitSelectors(&classSelectors_, propValue, false, overwrite);
        jerry_release_value(propValue);
    }
    jerry_release_value(propName);

    // init styleSheet
    const char * const keyFrames = "@keyframes";
    propName = jerry_create_string(reinterpret_cast<jerry_char_t *>(const_cast<char *>(keyFrames)));
    if (HasOwnProperty(styleSheetObj, propName)) {
        propValue = jerry_get_property(styleSheetObj, propName);
        InitSelectors(&keyFrameSelectors_, propValue, true, overwrite);
        jerry_release_value(propValue);
    }
    jerry_release_value(propName);
}

void AppStyleSheet::InitSelectors(AppStyleList **selectorsList, jerry_value_t selectorsObj,
                                  bool isKeyFrames, bool overwrite)
{
    if (jerry_value_is_undefined(selectorsObj)) {
        return;
    }

    if ((*selectorsList) == nullptr) {
        (*selectorsList) = new AppStyleList();
        if ((*selectorsList) == nullptr) {
            // malloc failed
            return;
        }
    }

    jerry_value_t styleKeys = jerry_get_object_keys(selectorsObj);
    uint16_t styleKeySize = jerry_get_array_length(styleKeys);
    for (uint16_t index = 0; index < styleKeySize; index++) {
        /* convert style key into char */
        jerry_value_t styleKey = jerry_get_property_by_index(styleKeys, index);
        jerry_value_t styleValue = jerry_get_property(selectorsObj, styleKey);
        AppStyle *newStyle = AppStyle::GenerateFromJS(styleKey, styleValue, isKeyFrames);
        if (newStyle != nullptr) {
            AppStyle *existOne = (*selectorsList)->GetExistStyle(newStyle->GetStyleName());
            if (existOne != nullptr) {
                AppStyle::CombineStyles(*existOne, *newStyle, overwrite);
                delete newStyle; // combined to exist one, no used
                newStyle = nullptr;
            } else {
                (*selectorsList)->AddStyle(newStyle);
            }
        }
        ReleaseJerryValue(styleKey, styleValue, VA_ARG_END_FLAG);
    }

    jerry_release_value(styleKeys);
}

AppStyle *AppStyleSheet::GetStyleFromIDSelectors(const char * const name) const
{
    return GetStyleFromSelectors(idSelectors_, name);
}

AppStyle *AppStyleSheet::GetStyleFromClassSelectors(const char * const name) const
{
    return GetStyleFromSelectors(classSelectors_, name);
}

AppStyle *AppStyleSheet::GetStyleFromKeyFramesSelectors(const char * const name) const
{
    return GetStyleFromSelectors(keyFrameSelectors_, name);
}

AppStyle *AppStyleSheet::GetStyleFromSelectors(AppStyleList *selectors, const char * const name) const
{
    if ((selectors == nullptr) || (name == nullptr) || strlen(name) == 0) {
        return nullptr;
    }

    AppStyle *first = const_cast<AppStyle *>(selectors->GetFirst());
    while (first != nullptr) {
        const char *styleName = first->GetStyleName();
        if (styleName != nullptr && !strcmp(name, styleName)) {
            return first;
        }
        first = const_cast<AppStyle *>(first->GetNext());
    }

    return first;
}
} // namespace ACELite
} // namespace OHOS
