/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_UTILS_H

#include <cmath>
#include <string>

#include "base/utils/utils.h"
#include "base/utils/string_utils.h"
#include "core/common/ime/text_selection.h"

namespace OHOS::Ace::TextUtils {

enum class CharType {
    NUMBER = 0,
    LETTER,
    OTHERS,
    NONE
};

inline CharType GetCharType(const std::wstring& value, int32_t position)
{
    if (position < 0 || position >= static_cast<int32_t>(value.size())) {
        return CharType::NONE;
    }

    CharType result = CharType::OTHERS;
    auto positionChar = value[position];
    if (positionChar >= '0' && positionChar <= '9') {
        result = CharType::NUMBER;
    } else if ((positionChar >= 'a' && positionChar <= 'z') || (positionChar >= 'A' && positionChar <= 'Z')) {
        result = CharType::LETTER;
    }
    return result;
}

inline TextSelection GetRangeOfSameType(const std::string& str, int32_t position)
{
    TextSelection selection = TextSelection(position, position);
    auto wstring = StringUtils::ToWstring(str);
    int32_t length = static_cast<int32_t>(wstring.size());
    CharType type = GetCharType(wstring, position);
    if (type != CharType::NUMBER && type != CharType::LETTER) {
        selection.extentOffset = position + 1;
        return selection;
    }

    // find start position.
    for (int32_t i = position - 1; i >= 0; --i) {
        if (GetCharType(wstring, i) != type) {
            selection.baseOffset = i + 1;
            break;
        }
        if (i == 0) {
            selection.baseOffset = 0;
        }
    }

    // find end position.
    for (int32_t i = position; i < length; ++i) {
        if (GetCharType(wstring, i) != type) {
            selection.extentOffset = i;
            break;
        }
        if (i == (length - 1)) {
            selection.extentOffset = length;
        }
    }
    return selection;
}

} // namespace OHOS::Ace::TextUtils

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_UTILS_H
