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

#include "core/common/ime/text_input_formatter.h"

#include "base/utils/macros.h"
#include "base/utils/string_utils.h"
#include "core/common/ime/text_editing_value.h"

namespace OHOS::Ace {

BlackListCharsFormatter::BlackListCharsFormatter(std::wregex&& regex) : regex_(regex) {}

void BlackListCharsFormatter::Format(const TextEditingValue& oldValue, TextEditingValue& newValue) const
{
    newValue.SelectionAwareTextManipulation(
        [this](std::wstring& manipulateText) { manipulateText = std::regex_replace(manipulateText, regex_, L""); });
}

// Only allow \d.-e
NumberFormatter::NumberFormatter() : BlackListCharsFormatter(std::wregex(L"[^\\d.\\-e]+")) {}

// Only allow \d-+
PhoneNumberFormatter::PhoneNumberFormatter() : BlackListCharsFormatter(std::wregex(L"[^\\d\\-\\+]+")) {}

// Only allow \d\w-_.@
EmailFormatter::EmailFormatter() : BlackListCharsFormatter(std::wregex(L"[^\\d\\w\\-\\._@]+")) {}

// Allow \d\w-_.~!*'();:@&=+$,/?#[]
UriFormatter::UriFormatter() : BlackListCharsFormatter(std::wregex(L"[^\\d\\w-_.~!*'();:@&=+$,/?#[\\]]+")) {}

SingleLineFormatter::SingleLineFormatter() : BlackListCharsFormatter(std::wregex(L"\n")) {}

void LengthLimitingFormatter::Format(const TextEditingValue& oldValue, TextEditingValue& newValue) const
{
    auto text = newValue.GetWideText();
    if (text.length() > limit_) {
        int32_t exceedLen = static_cast<int32_t>(text.length()) - static_cast<int32_t>(limit_);
        int32_t removeBeforeExtent = std::min(exceedLen, newValue.selection.extentOffset);
        int32_t removeAfterExtent = exceedLen - removeBeforeExtent;
        if (removeBeforeExtent > 0) {
            int32_t eraseStart = newValue.selection.extentOffset - removeBeforeExtent;
            int32_t preCharIndex = eraseStart - 1;
            if (preCharIndex >= 0 && preCharIndex < static_cast<int32_t>(text.length()) &&
                StringUtils::NotInBmp(text[preCharIndex])) {
                --eraseStart;
                ++exceedLen;
                ++removeBeforeExtent;
            }
            if (static_cast<size_t>(eraseStart + exceedLen) > text.length()) {
                return;
            }
            text.erase(eraseStart, removeBeforeExtent);
            newValue.selection.Update(eraseStart);
        }
        if (removeAfterExtent > 0) {
            int32_t eraseStart = text.length() - removeAfterExtent;
            auto preCharIndex = eraseStart - 1;
            if (preCharIndex >= 0 && preCharIndex < static_cast<int32_t>(text.length()) &&
                StringUtils::NotInBmp(text[preCharIndex])) {
                --eraseStart;
            }
            if (eraseStart >= 0 && (static_cast<int32_t>(text.length()) - eraseStart > 0)) {
                text.erase(eraseStart);
            }
        }
        if (exceedLen > 0) {
            newValue.text = StringUtils::ToString(text);
        }
    }
}

} // namespace OHOS::Ace
