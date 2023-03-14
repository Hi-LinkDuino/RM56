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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_EDITING_VALUE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_EDITING_VALUE_H

#include <functional>
#include <string>

#include "core/common/ime/text_selection.h"

namespace OHOS::Ace {

class JsonValue;

using TextManipulation = std::function<void(std::wstring&)>;

struct TextEditingValue {
    void ParseFromJson(const JsonValue& json);
    std::string ToJsonString() const;

    bool operator==(const TextEditingValue& other) const;
    bool operator!=(const TextEditingValue& other) const;

    std::wstring GetWideText() const;

    /**
     * @brief Selection offset of text is designed for 2 bytes per offset, so if glyphs out range of UTF-16 BMP,
     * like emoji, will be counted as 2 offsets. For example, here is a string contains emoji [smile]:
     *     abc[smile]
     * The offset between 'c' and '[smile]' is 3, and the offset after '[smile]' will be 5.
     *
     * Internal implementation: We recognize this case by judging UTF-16 characters. If an code unit is out of
     * UTF-16 BMP, then offset to move will for it will be two.
     */
    void MoveLeft();
    void MoveRight();
    void MoveToPosition(int32_t position);
    void UpdateSelection(int32_t both);
    void UpdateSelection(int32_t start, int32_t end);

    /**
     * @brief Manipulate the text by [manipulation] func, meanwhile update the selection.
     *
     * @param[in] manipulation The function for manipulation.
     */
    void SelectionAwareTextManipulation(const TextManipulation& manipulation);
    std::string GetBeforeSelection() const;
    std::string GetSelectedText() const;
    std::string GetSelectedText(const TextSelection& textSelection) const;
    std::string GetAfterSelection() const;

    // Delete text of start to end.
    void Delete(int32_t start, int32_t end);

    std::string text;
    std::string hint;
    TextSelection selection;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_EDITING_VALUE_H
