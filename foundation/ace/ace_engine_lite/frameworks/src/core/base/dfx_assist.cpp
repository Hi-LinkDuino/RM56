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

#include "dfx_assist.h"
#include "ace_log.h"
#include "jerryscript-core.h"
#include "presets/console_log_impl.h"

namespace OHOS {
namespace ACELite {
void DfxAssist::DumpErrorCode(const jerry_value_t errorValue)
{
    const uint16_t errorMsgMaxLength = 128;
    if (!jerry_is_feature_enabled(JERRY_FEATURE_ERROR_MESSAGES)) {
        return;
    }

    jerry_value_t errValue = jerry_get_value_from_error(errorValue, false);
    jerry_value_t errStrVal = jerry_value_to_string(errValue);
    jerry_release_value(errValue);

    if (jerry_value_is_error(errStrVal)) {
        /* Avoid recursive error throws. */
        HILOG_ERROR(HILOG_MODULE_ACE, "[Error value to string failed]");
        jerry_release_value(errStrVal);
        return;
    }

    jerry_size_t errStrSize = jerry_get_utf8_string_size(errStrVal);
    if (errStrSize >= errorMsgMaxLength) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[Error message too long]");
        jerry_release_value(errStrVal);
        return;
    }

    jerry_char_t *errStrBuffer = static_cast<jerry_char_t *>(ace_malloc(sizeof(jerry_char_t) * (errStrSize + 1)));
    if (errStrBuffer == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc memory failed!");
        jerry_release_value(errStrVal);
        return;
    }
    jerry_size_t stringEnd = jerry_string_to_utf8_char_buffer(errStrVal, errStrBuffer, errStrSize);
    errStrBuffer[stringEnd] = '\0';
    // output to platform trace
    HILOG_ERROR(HILOG_MODULE_ACE, " [JS Error]: %{public}s", reinterpret_cast<char *>(errStrBuffer));
    // output to user console
    LogString(LogLevel::LOG_LEVEL_ERR, "[JS Exception]: ");
    LogString(LogLevel::LOG_LEVEL_ERR, reinterpret_cast<char *>(errStrBuffer));
    // output line separator to trigger trace output
    LogString(LogLevel::LOG_LEVEL_ERR, "\n");
    ace_free(errStrBuffer);
    errStrBuffer = nullptr;
    jerry_release_value(errStrVal);
}

void DfxAssist::DumpErrorMessage(const jerry_value_t errorValue)
{
    const uint16_t stackMsgMaxLength = 256;
    const uint8_t arrMaxLength = 32;
    jerry_value_t stackStr = jerry_create_string((const jerry_char_t *) "stack");
    jerry_value_t errorVal = jerry_get_value_from_error(errorValue, false);

    jerry_value_t backtraceVal = jerry_get_property(errorVal, stackStr);
    jerry_release_value(stackStr);
    jerry_release_value(errorVal);

    if (jerry_value_is_error(backtraceVal) || !(jerry_value_is_array(backtraceVal))) {
        jerry_release_value(backtraceVal);
        return;
    }

    uint32_t length = jerry_get_array_length(backtraceVal);
    if (length == 0) {
        jerry_release_value(backtraceVal);
        return;
    }
    if (length > arrMaxLength) {
        length = arrMaxLength;
    }

    jerry_char_t *errStrBuffer = static_cast<jerry_char_t *>(ace_malloc(sizeof(jerry_char_t) * (stackMsgMaxLength)));
    if (errStrBuffer == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc memory failed!");
        jerry_release_value(backtraceVal);
        return;
    }
    HILOG_ERROR(HILOG_MODULE_ACE, "[Exception backtrace]:");
    for (uint32_t i = 0; i < length; i++) {
        jerry_value_t itemVal = jerry_get_property_by_index(backtraceVal, i);
        jerry_size_t strSize = 0;
        if (!jerry_value_is_error(itemVal) && jerry_value_is_string(itemVal)) {
            strSize = jerry_get_utf8_string_size(itemVal);
        }

        if (strSize >= stackMsgMaxLength) {
            HILOG_ERROR(HILOG_MODULE_ACE, "%{public}hhu: [Backtrace string too long]", i);
        } else {
            jerry_size_t stringEnd = jerry_string_to_utf8_char_buffer(itemVal, errStrBuffer, strSize);
            errStrBuffer[stringEnd] = '\0';
            HILOG_ERROR(HILOG_MODULE_ACE, "%{public}u: %{public}s", i, reinterpret_cast<char *>(errStrBuffer));
        }
        jerry_release_value(itemVal);
    }
    ace_free(errStrBuffer);
    errStrBuffer = nullptr;
    jerry_release_value(backtraceVal);
}
} // namespace ACELite
} // namespace OHOS
