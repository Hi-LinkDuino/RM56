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

#include "localization_module.h"
#include "ace_event_error_code.h"
#if (FEATURE_LOCALIZATION_MODULE == 1)
#include "ace_log.h"
#include "js_fwk_common.h"
#include <stdlib.h>

namespace OHOS {
namespace ACELite {
void LocalizationModule::Init()
{
    jerry_value_t globalContext = jerry_get_global_object();
    const char * const name = "ViewModel";
    jerry_value_t propertyName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(name));
    if (JerryHasProperty(globalContext, propertyName)) {
        // get the prototype of AbilitySlice
        jerry_value_t currentApp = jerry_get_property(globalContext, propertyName);
        jerry_value_t protoType = jerryx_get_property_str(currentApp, "prototype");
        // register $t to the prototype of abilitySlice
        jerry_value_t functionHandle = jerry_create_external_function(Translate);
        const char * const propName = "$t";
        JerrySetNamedProperty(protoType, propName, functionHandle);
        // register $tc to the prototype of abilitySlice
#ifdef LOCALIZATION_PLURAL
        jerry_value_t pluralHandle = jerry_create_external_function(TranslatePlural);
        const char * const pluralFuncName = "$tc";
        JerrySetNamedProperty(protoType, pluralFuncName, pluralHandle);
        jerry_release_value(pluralHandle);
#endif // LOCALIZATION_PLURAL
        ReleaseJerryValue(functionHandle, protoType, currentApp, VA_ARG_END_FLAG);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "app is not create.");
    }
    ReleaseJerryValue(propertyName, globalContext, VA_ARG_END_FLAG);
}

void LocalizationModule::Clear()
{
    if (parser_ != nullptr) {
        delete parser_;
        parser_ = nullptr;
    }
}

#ifdef LOCALIZATION_PLURAL
jerry_value_t LocalizationModule::GetValueByKey(const jerry_value_t *args, const jerry_length_t argsNum, bool isPlural)
#else
jerry_value_t LocalizationModule::GetValueByKey(const jerry_value_t *args, const jerry_length_t argsNum)
#endif
{
    if (argsNum == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetValueByKey failed: args number error");
        return UNDEFINED;
    }
    LocalizationModule *localization = GetInstance();
    if (localization->parser_ == nullptr) {
        localization->parser_ = new CJSONParser();
        if (localization->parser_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create json parser failed");
            return UNDEFINED;
        }
        if (!localization->parser_->Init()) {
            ACE_ERROR_CODE_PRINT(EXCE_ACE_LOCALIZATION_FAILED, EXCE_ACE_LOCALIZATION_SYSTEM_LANGUAGE_NOT_INITIALIZED);
            localization->Clear(); // parser init failed, release the resource of this module
            HILOG_ERROR(HILOG_MODULE_ACE, "GetValueByKey failed: parser initialize error");
            return UNDEFINED;
        }
    }
    // if system language origion info changed, cache new file to psram/ram
    CJSONParser::LanguageState change = localization->parser_->ChangeLanguage();
    if ((change == CJSONParser::LanguageState::LANGUAGE_CHANGED) ||
        (change == CJSONParser::LanguageState::LANGUAGE_FAIL)) {
        if (!localization->parser_->CacheFile()) {
            HILOG_ERROR(HILOG_MODULE_ACE, "cache file failed");
        }
    }
    uint16_t paramStrLength = 0;
    char *param = MallocStringOf(args[0], &paramStrLength);
    if (param == nullptr || paramStrLength == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetValueByKey failed: parse key to string error");
        return UNDEFINED;
    }
    jerry_value_t resultProp = localization->parser_->GetValue(param, args, argsNum);
    if (!jerry_value_is_string(resultProp) && !jerry_value_is_number(resultProp)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "GetValueByKey failed: the final result error, keyLen[%{public}d]", strlen(param));
    }
    ace_free(param);
    param = nullptr;
    return resultProp;
}
} // namespace ACELite
} // namespace OHOS
#endif // LOCALIZATION_MODULE
