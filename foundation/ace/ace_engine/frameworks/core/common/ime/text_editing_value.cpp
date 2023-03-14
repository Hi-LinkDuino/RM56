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

#include "core/common/ime/text_editing_value.h"

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/utils/macros.h"
#include "base/utils/string_utils.h"

namespace OHOS::Ace {
namespace {

// Negotiated fields with Java
const char TEXT[] = "text";
const char HINT[] = "hint";
const char SELECTION_START[] = "selectionStart";
const char SELECTION_END[] = "selectionEnd";

} // namespace

void TextEditingValue::ParseFromJson(const JsonValue& json)
{
    text = json.GetString(TEXT);
    hint = json.GetString(HINT);
    selection.baseOffset = json.GetInt(SELECTION_START, -1);
    selection.extentOffset = json.GetInt(SELECTION_END, -1);
}

std::string TextEditingValue::ToJsonString() const
{
    auto json = JsonUtil::Create(true);
    json->Put(TEXT, text.c_str());
    json->Put(HINT, hint.c_str());
    json->Put(SELECTION_START, selection.baseOffset);
    json->Put(SELECTION_END, selection.extentOffset);
    return json->ToString();
}

bool TextEditingValue::operator==(const TextEditingValue& other) const
{
    if (selection != other.selection) {
        return false;
    }

    return text == other.text && hint == other.hint;
}

bool TextEditingValue::operator!=(const TextEditingValue& other) const
{
    return !operator==(other);
}

std::wstring TextEditingValue::GetWideText() const
{
    return StringUtils::ToWstring(text);
}

void TextEditingValue::MoveLeft()
{
    if (selection.extentOffset <= 1) {
        selection.Update(0);
        return;
    }

    auto utf16Text = StringUtils::Str8ToStr16(text);
    int32_t prevCharIndex = std::min(selection.extentOffset - 1, static_cast<int32_t>(utf16Text.length()));
    selection.Update(StringUtils::NotInUtf16Bmp(utf16Text[prevCharIndex]) ? prevCharIndex - 1 : prevCharIndex);
}

void TextEditingValue::MoveRight()
{
    auto utf16Text = StringUtils::Str8ToStr16(text);
    if (static_cast<size_t>(selection.extentOffset) >= utf16Text.length() - 1) {
        selection.Update(utf16Text.length());
        return;
    }

    int32_t nextCharIndex = selection.extentOffset;
    selection.Update(StringUtils::NotInUtf16Bmp(utf16Text[nextCharIndex])
                         ? std::min(static_cast<int32_t>(utf16Text.length()), nextCharIndex + 2)
                         : nextCharIndex + 1);
}

void TextEditingValue::MoveToPosition(int32_t position)
{
    if (position < 0) {
        selection.Update(0);
        return;
    }
    auto utf16Text = StringUtils::Str8ToStr16(text);
    if (static_cast<size_t>(position) >= utf16Text.length()) {
        selection.Update(utf16Text.length());
        return;
    }
    selection.Update(position);
}

void TextEditingValue::UpdateSelection(int32_t both)
{
    UpdateSelection(both, both);
}

void TextEditingValue::UpdateSelection(int32_t start, int32_t end)
{
    if (start < 0) {
        start = 0;
    }
    if (static_cast<size_t>(end) > GetWideText().length()) {
        end = static_cast<int32_t>(GetWideText().length());
    }
    selection.Update(start, end);
}

void TextEditingValue::SelectionAwareTextManipulation(const TextManipulation& manipulation)
{
    if (!manipulation) {
        return;
    }

    auto wideText = GetWideText();
    int32_t start = selection.GetStart();
    int32_t end = selection.GetEnd();
    if (static_cast<size_t>(end) > wideText.length() || start > end) {
        LOGD("Illegal selection for manipulate: start %{public}d, end %{public}d", start, end);
        return;
    }

    if ((start <= 0) && (end <= 0)) {
        manipulation(wideText);
    } else {
        std::wstring beforeSelection;
        if ((start > 0) && (static_cast<size_t>(start) <= wideText.length())) {
            beforeSelection = wideText.substr(0, start);
            manipulation(beforeSelection);
        }

        std::wstring inSelection;
        if (start != end) {
            inSelection = wideText.substr(start, end - start);
            manipulation(inSelection);
        }

        std::wstring afterSelection;
        size_t lenLeft = wideText.length() - static_cast<size_t>(end);
        if (lenLeft > 0) {
            afterSelection = wideText.substr(end, lenLeft);
            manipulation(afterSelection);
        }

        wideText = beforeSelection + inSelection + afterSelection;
        if (selection.baseOffset > selection.extentOffset) {
            selection.Update(beforeSelection.length() + inSelection.length(), beforeSelection.length());
        } else {
            selection.Update(beforeSelection.length(), beforeSelection.length() + inSelection.length());
        }
    }

    text = StringUtils::ToString(wideText);
}

std::string TextEditingValue::GetBeforeSelection() const
{
    auto wideText = GetWideText();
    int32_t start = selection.GetStart();
    if (static_cast<size_t>(start) > wideText.length()) {
        LOGD("Illegal selection for GetBeforeSelection: start %{public}d", start);
        return "";
    }

    std::string beforeText;
    if (start > 0) {
        std::wstring beforeSelection = wideText.substr(0, start);
        beforeText = StringUtils::ToString(beforeSelection);
    }
    return beforeText;
}

std::string TextEditingValue::GetSelectedText() const
{
    auto wideText = GetWideText();
    int32_t start = selection.GetStart();
    int32_t end = selection.GetEnd();
    if (static_cast<size_t>(end) > wideText.length() || start > end) {
        LOGD("Illegal selection for GetSelectedText: start %{public}d, end %{public}d", start, end);
        return "";
    }

    std::string selectedText;
    if (start < 0) {
        start = 0;
    }
    if (end > 0 && start != end) {
        std::wstring inSelection = wideText.substr(start, end - start);
        selectedText = StringUtils::ToString(inSelection);
    }
    return selectedText;
}

std::string TextEditingValue::GetSelectedText(const TextSelection& textSelection) const
{
    auto wideText = GetWideText();
    int32_t start = textSelection.GetStart();
    int32_t end = textSelection.GetEnd();
    std::string selectedText;
    if (start < 0) {
        start = 0;
    }
    if (static_cast<size_t>(end) > wideText.length()) {
        end = static_cast<int32_t>(wideText.length());
    }

    if (end > 0 && start < end) {
        std::wstring inSelection = wideText.substr(start, end - start);
        selectedText = StringUtils::ToString(inSelection);
    }
    return selectedText;
}

std::string TextEditingValue::GetAfterSelection() const
{
    auto wideText = GetWideText();
    int32_t end = selection.GetEnd();
    if (static_cast<size_t>(end) > wideText.length()) {
        LOGD("Illegal selection for GetAfterSelection: start %{public}d", end);
        return "";
    }

    std::string afterText;
    if (end <= 0) {
        afterText = StringUtils::ToString(wideText);
    } else {
        std::wstring afterSelection = wideText.substr(end);
        afterText = StringUtils::ToString(afterSelection);
    }
    return afterText;
}

void TextEditingValue::Delete(int32_t start, int32_t end)
{
    auto wideText = GetWideText();
    auto length = (int32_t)wideText.length();
    int32_t startPos = std::max(std::min(start, end), 0);
    int32_t endPos = std::min(std::max(start, end), length);
    if (startPos >= endPos) {
        return;
    }

    auto textAfterDelete = wideText.substr(0, startPos) + wideText.substr(endPos);
    text = StringUtils::ToString(textAfterDelete);
    selection.Update(startPos);
}

} // namespace OHOS::Ace
