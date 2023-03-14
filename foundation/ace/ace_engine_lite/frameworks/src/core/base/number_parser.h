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

#ifndef OHOS_ACELITE_NUMBER_PARSER_H
#define OHOS_ACELITE_NUMBER_PARSER_H

#include <cstdint>
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
/**
 * Utils class for number formating operations.
 */
class NumberParser final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(NumberParser);
    NumberParser() = delete;
    ~NumberParser() = delete;

    /**
     * @brief ParseDoubleValue convert one string to number(double) value
     * @param percentStr the under processing string
     * @param strLength the given string length
     * @param outValue the converted result
     * @return the process result, false for converting failed, true for success, caller
     * should check the returned value before using outValue.
     */
    static bool ParseDoubleValue(const char *str, uint16_t strLength, double &outValue);

    /**
     * @brief ParsePercentValue convert percent string to number value
     * @param percentStr the under processing percent string, for example "10%"
     * @param strLength the given percent string length
     * @param outValue the converted result
     * @return the process result, false for converting failed, true for success, caller
     * should check the returned value before using outValue.
     */
    static bool ParsePercentValue(const char *percentStr, uint16_t strLength, float &outValue);

private:
    /**
     * @brief IsValidString check if the input target string is valid
     * @param str the given under processing string
     * @param strLength the given string's length
     * @return true for valid string, false for not
     */
    static bool IsValidString(const char *str, uint16_t strLength);
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_NUMBER_PARSER_H
