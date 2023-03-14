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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_EDIT_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_EDIT_CONTROLLER_H

#include <string>

#include "base/utils/value_change_notifier.h"
#include "core/common/ime/text_editing_value.h"

namespace OHOS::Ace {

class TextEditController : public ValueChangeNotifier<TextEditingValue> {
public:
    void SetText(const std::string& newText, bool needFireChangeEvent = true)
    {
        auto value = GetValue();
        value.text = newText;
        // Default set selection to the end of text is more consistent with the intuition of user.
        value.selection.Update(value.GetWideText().length());
        SetValue(std::move(value), needFireChangeEvent);
    }

    void SetHint(const std::string& hint)
    {
        auto value = GetValue();
        value.hint = hint;
        SetValue(std::move(value));
    }

    void SetSelection(const TextSelection& selection)
    {
        auto value = GetValue();
        value.selection = selection;
        SetValue(std::move(value));
    }

    void Clear()
    {
        SetValue(TextEditingValue());
    }

    const std::string& GetText() const
    {
        return GetValue().text;
    }

    const TextSelection& GetSelection() const
    {
        return GetValue().selection;
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_EDIT_CONTROLLER_H
