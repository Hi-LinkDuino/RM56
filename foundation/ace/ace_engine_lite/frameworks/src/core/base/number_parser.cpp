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
#include "number_parser.h"
#include <cstdlib>
#include <cstring>
#include "ace_log.h"

namespace OHOS {
namespace ACELite {
/**
 * @brief ParseDoubleValue convert one string to number(int32_t) value
 * @param percentStr the under processing string
 * @param strLength the given string length
 * @param outValue the converted result
 * @return the process result, false for converting failed, true for success, caller
 * should check the returned value before using outValue.
 */
bool NumberParser::ParseDoubleValue(const char *str, uint16_t strLength, double &outValue)
{
    if (!IsValidString(str, strLength)) {
        return false;
    }

    outValue = strtod(str, nullptr);
    return true;
}

/**
 * @brief ParsePercentValue convert percent string to number value
 * @param percentStr the under processing percent string, for example "10%"
 * @param strLength the given percent string length
 * @param outValue the converted result
 * @return the process result, false for converting failed, true for success, caller
 * should check the returned value before using outValue.
 */
bool NumberParser::ParsePercentValue(const char *percentStr, uint16_t strLength, float &outValue)
{
    constexpr uint16_t minLength = 2;
    // check input parameters
    if (!(IsValidString(percentStr, strLength)) || (strLength < minLength)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "parse percent value failed, invalid input parameters");
        return false;
    }
    size_t originalStrLen = strlen(percentStr);
    uint16_t ensuredStrLength = (strLength > originalStrLen) ? originalStrLen : strLength;
    // only support max 15 bytes length string
    constexpr uint16_t maxLength = 16;
    if (ensuredStrLength > maxLength) {
        HILOG_ERROR(HILOG_MODULE_ACE, "parse percent value failed, input string too long [%{public}d]",
                    ensuredStrLength);
        return false;
    }
    // the latest charater must be %, and the first digit must not be "."
    if ((percentStr[0] == '.') || (percentStr[ensuredStrLength - 1] != '%')) {
        return false;
    }
    // check if the string is one valid number format
    uint16_t index = (percentStr[0] == '-') ? 1 : 0;
    constexpr uint16_t minNegativeLength = minLength + 1;
    if ((index == 1) && (ensuredStrLength < minNegativeLength)) {
        // for negative number, the min length is 3
        return false;
    }
    while ((index < ensuredStrLength) && (percentStr[index] != '%')) {
        if ((percentStr[index] != '.') && ((percentStr[index] < '0') || (percentStr[index] > '9'))) {
            // give up the parsing, it's not digit format
            return false;
        }
        index++;
    }
    // do the parsing
    outValue = static_cast<float>(strtod(percentStr, nullptr));
    return true;
}

/**
 * @brief IsValidString check if the input target string is valid
 * @param str the given under processing string
 * @param strLength the given string's length
 * @return true for valid string, false for not
 */
bool NumberParser::IsValidString(const char *str, uint16_t strLength)
{
    return ((str != nullptr) && (strLength != 0));
}
} // namespace ACELite
} // namespace OHOS
