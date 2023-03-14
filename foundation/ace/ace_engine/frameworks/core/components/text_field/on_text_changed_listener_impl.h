/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_ON_TEXT_CHANGED_LISTENER_IMPL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_ON_TEXT_CHANGED_LISTENER_IMPL_H

#include "input_method_controller.h"

#include "core/components/text_field/render_text_field.h"

namespace OHOS::Ace {

class OnTextChangedListenerImpl : public MiscServices::OnTextChangedListener {
public:
    explicit OnTextChangedListenerImpl(const WeakPtr<RenderTextField>& field) : field_(field) {}
    ~OnTextChangedListenerImpl() override = default;

    void InsertText(const std::u16string& text) override;
    void DeleteBackward(int32_t length) override;
    void DeleteForward(int32_t length) override;
    void SetKeyboardStatus(bool status) override;
    void SendKeyEventFromInputMethod(const MiscServices::KeyEvent& event) override;
    void SendKeyboardInfo(const MiscServices::KeyboardInfo& info) override;
    void MoveCursor(MiscServices::Direction direction) override;

private:
    void PostTaskToUI(const std::function<void()>& task);
    void HandleKeyboardStatus(MiscServices::KeyboardStatus status);
    void HandleFunctionKey(MiscServices::FunctionKey functionKey);

    WeakPtr<RenderTextField> field_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_ON_TEXT_CHANGED_LISTENER_IMPL_H
