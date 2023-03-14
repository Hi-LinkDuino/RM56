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

#include "number_format_module.h"
#include "ace_log.h"
#if (FEATURE_NUMBER_FORMAT == 1)
#include "locale_util.h"
#include "string.h"
#include "key_parser.h"
#include "keys.h"
#include "types.h"
namespace OHOS {
namespace ACELite {
using namespace I18N;
constexpr jerry_object_native_info_t NumberFormatModule::gcCallback;
void NumberFormatModule::Init(jerry_value_t intlHandle)
{
    jerry_value_t constructor = jerry_create_external_function(CreateNumberFormat);
    jerry_value_t format = jerry_create_external_function(Format);
    jerry_value_t baseObj = jerry_create_object();
    JerrySetNamedProperty(baseObj, "format", format);
    JerrySetNamedProperty(constructor, "prototype", baseObj);
    JerrySetNamedProperty(intlHandle, "NumberFormat", constructor);
    ReleaseJerryValue(constructor, format, baseObj, VA_ARG_END_FLAG);
}

jerry_value_t NumberFormatModule::CreateNumberFormat(const jerry_value_t func,
                                                     const jerry_value_t context,
                                                     const jerry_value_t args[],
                                                     const jerry_length_t argsNum)
{
    if (!jerry_value_is_constructor(func)) {
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>
                                  ("use new to create NumberFormat"));
    }
    NumberFormatModule *numModule = new NumberFormatModule();
    if (numModule == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>("memory is not enough"));
    }
    char *value = nullptr;
    if (argsNum >= 1) {
        value = MallocStringOf(args[0]);
    }
    numModule->localeInfo_ = LocaleUtil::GetLocaleInfo(value);
    ACE_FREE(value);
    if (numModule->localeInfo_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "get system info failed");
        delete numModule;
        numModule = nullptr;
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>("get locale info failed"));
    }
    int status = 0;
    numModule->formatter_ = new NumberFormat(*(numModule->localeInfo_), status);
    if ((numModule->formatter_ == nullptr) || (status == I18nStatus::IERROR)) {
        delete numModule;
        numModule = nullptr;
        HILOG_ERROR(HILOG_MODULE_ACE, "init resource failed");
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>("init resource failed"));
    }
    const char *result = nullptr;
    const uint8_t maxArgLen = 2;
    if (argsNum >= maxArgLen) {
        // set the number style
        jerry_value_t keys = jerry_get_object_keys(args[1]);
        uint8_t size = jerry_get_array_length(keys);
        for (uint8_t index = 0; index < size; index++) {
            jerry_value_t key = jerry_get_property_by_index(keys, index);
            result = SetStyle(*numModule, key, args[1]);
            jerry_release_value(key);
        }
        jerry_release_value(keys);
        if (result != nullptr) {
            delete numModule;
            numModule = nullptr;
            return jerry_create_error(JERRY_ERROR_RANGE, reinterpret_cast<const jerry_char_t *>(result));
        }
    }
    jerry_set_object_native_pointer(context, numModule, &gcCallback);
    return UNDEFINED;
}

jerry_value_t NumberFormatModule::Format(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    if ((argsNum < 1) || (!jerry_value_is_number(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the args num in number format is error");
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>("format invalid value"));
    }
    double number = jerry_get_number_value(args[0]);
    NumberFormatModule *numModule = nullptr;
    if ((!jerry_get_object_native_pointer(context, reinterpret_cast<void* *>(&numModule), &gcCallback)) ||
        (numModule == nullptr) || (numModule->formatter_ == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "get the object number formatter failed");
        return UNDEFINED;
    }
    std::string result;
    int status = 0;
    if (numModule->isUsingGroup_) {
        result = numModule->formatter_->Format(number, numModule->formatType_, status);
    } else {
        result = numModule->formatter_->FormatNoGroup(number, numModule->formatType_, status);
    }
    if (status == I18nStatus::IERROR) {
        HILOG_ERROR(HILOG_MODULE_ACE, "format number failed");
        return UNDEFINED;
    }
    if (!result.empty()) {
        return jerry_create_string(reinterpret_cast<const jerry_char_t*>(result.c_str()));
    }
    return UNDEFINED;
}

void NumberFormatModule::DeleteNumberFormat(void *nativePointer)
{
    NumberFormatModule *format = reinterpret_cast<NumberFormatModule *>(nativePointer);
    if (format != nullptr) {
        delete format;
        format = nullptr;
    }
}

const char *NumberFormatModule::SetStyle(NumberFormatModule &numModel,
                                         const jerry_value_t key,
                                         const jerry_value_t style)
{
    uint16_t styleLen = 0;
    char *styleName = MallocStringOf(key, &styleLen);
    uint32_t styleNameId = KeyParser::ParseKeyId(styleName, styleLen);
    ACE_FREE(styleName);
    if (!KeyParser::IsKeyValid(styleNameId)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the style name is invalid");
        return nullptr;
    }
    jerry_value_t styleValue = jerry_get_property(style, key);
    const char *result = nullptr;
    switch (styleNameId) {
        case K_STYLE: {
            if (!numModel.SetNumberStyle(styleValue)) {
                result = "the style is invalid";
            }
            break;
        }
        case K_USEGROUP: {
            numModel.SetUsingGroup(styleValue);
            break;
        }
        case K_MINIMUMFRACTIONDIGITS: {
            int16_t number = IntegerOf(styleValue);
            const uint8_t maxMiniFraction = 20;
            if ((number < 0) || (number > maxMiniFraction)) {
                HILOG_ERROR(HILOG_MODULE_ACE, "the minimumFractionDigits is invalid");
                result = "the minimumFractionDigits is invalid";
            }
            if (numModel.formatType_ == NumberFormatType::DECIMAL) {
                numModel.formatter_->SetMinDecimalLength(number);
            }
            break;
        }
        case K_MAXIMUMFRACTIONDIGITS: {
            if (!numModel.SetMaximumFractionDigits(styleValue)) {
                result = "the maximumFractionDigits is invalid";
            }
            break;
        }
        default:
            break;
    }
    jerry_release_value(styleValue);
    return result;
}

void NumberFormatModule::SetUsingGroup(jerry_value_t styleValue)
{
    if (!jerry_value_is_boolean(styleValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "set the useGroup value failed");
        return;
    }
    isUsingGroup_ = jerry_get_boolean_value(styleValue);
}

bool NumberFormatModule::SetMaximumFractionDigits(jerry_value_t styleValue)
{
    int16_t number = IntegerOf(styleValue);
    const uint8_t maxMaxFraction = 20;
    if ((number < 0) || (number > maxMaxFraction)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the maximumFractionDigits is out of range");
        return false;
    }
    const int16_t defaultMaxmumFractionDigits = 3;
    if (number < defaultMaxmumFractionDigits) {
        HILOG_WARN(HILOG_MODULE_ACE, "the maximumFractionDigits is too small");
    } else {
        if (formatType_ == NumberFormatType::PERCENT) {
            // set maxDecimalLength in percent is invalid
            HILOG_ERROR(HILOG_MODULE_ACE, "set maximumFractionDigits failed in percent");
            return true;
        }
        formatter_->SetMaxDecimalLength(number);
    }
    return true;
}

bool NumberFormatModule::SetNumberStyle(jerry_value_t styleValue)
{
    char *type = MallocStringOf(styleValue);
    if ((type != nullptr) && (!strcmp(type, "percent"))) {
        formatType_ = NumberFormatType::PERCENT;
    } else if ((type == nullptr) || (!strcmp(type, "decimal"))) {
        formatType_ = NumberFormatType::DECIMAL;
    } else {
        ACE_FREE(type);
        return false;
    }
    ACE_FREE(type);
    return true;
}
}
}
#endif
