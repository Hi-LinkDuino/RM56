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

#include "core/components/text_field/on_text_changed_listener_impl.h"

namespace OHOS::Ace {

void OnTextChangedListenerImpl::InsertText(const std::u16string& text)
{
    if (text.length() <= 0) {
        LOGE("the text is null");
        return;
    }

    auto task = [textField = field_, text] {
        auto client = textField.Upgrade();
        if (!client) {
            LOGE("text field is null");
            return;
        }
        ContainerScope scope(client->instanceId_);
        auto value = client->GetEditingValue();
        auto textEditingValue = std::make_shared<TextEditingValue>();
        textEditingValue->text =
            value.GetBeforeSelection() + StringUtils::Str16ToStr8(text) + value.GetAfterSelection();
        textEditingValue->UpdateSelection(std::max(value.selection.GetStart(), 0) + text.length());
        client->UpdateEditingValue(textEditingValue, true);
    };
    PostTaskToUI(task);
}

void OnTextChangedListenerImpl::DeleteBackward(int32_t length)
{
    LOGI("[OnTextChangedListenerImpl] DeleteBackward length: %{public}d", length);
    if (length <= 0) {
        LOGE("Delete nothing.");
        return;
    }

    auto task = [textField = field_, length] {
        auto client = textField.Upgrade();
        if (!client) {
            LOGE("text field is null");
            return;
        }
        ContainerScope scope(client->instanceId_);
        auto value = client->GetEditingValue();
        auto start = value.selection.GetStart();
        auto end = value.selection.GetEnd();
        auto textEditingValue = std::make_shared<TextEditingValue>();
        textEditingValue->text = value.text;
        textEditingValue->UpdateSelection(start, end);
        textEditingValue->Delete(start, start == end ? end + length : end);
        client->UpdateEditingValue(textEditingValue, true);
    };
    PostTaskToUI(task);
}

void OnTextChangedListenerImpl::DeleteForward(int32_t length)
{
    LOGI("[OnTextChangedListenerImpl] DeleteForward length: %{public}d", length);
    if (length <= 0) {
        LOGE("Delete nothing.");
        return;
    }

    auto task = [textField = field_, length] {
        auto client = textField.Upgrade();
        if (!client) {
            LOGE("text field is null");
            return;
        }
        ContainerScope scope(client->instanceId_);
        auto value = client->GetEditingValue();
        auto start = value.selection.GetStart();
        auto end = value.selection.GetEnd();
        auto textEditingValue = std::make_shared<TextEditingValue>();
        textEditingValue->text = value.text;
        textEditingValue->UpdateSelection(start, end);
        textEditingValue->Delete(start == end ? start - length : start, end);
        client->UpdateEditingValue(textEditingValue, true);
    };
    PostTaskToUI(task);
}

void OnTextChangedListenerImpl::SetKeyboardStatus(bool status)
{
    LOGI("[OnTextChangedListenerImpl] SetKeyboardStatus status: %{public}d", status);
    auto task = [textField = field_, status] {
        auto client = textField.Upgrade();
        if (client) {
            ContainerScope scope(client->instanceId_);
            client->SetInputMethodStatus(status);
        }
    };
    PostTaskToUI(task);
}

void OnTextChangedListenerImpl::SendKeyEventFromInputMethod(const MiscServices::KeyEvent& event) {}

void OnTextChangedListenerImpl::SendKeyboardInfo(const MiscServices::KeyboardInfo& info)
{
    HandleKeyboardStatus(info.GetKeyboardStatus());
    HandleFunctionKey(info.GetFunctionKey());
}

void OnTextChangedListenerImpl::HandleKeyboardStatus(MiscServices::KeyboardStatus status)
{
    LOGI("[OnTextChangedListenerImpl] HandleKeyboardStatus status: %{public}d", status);
    if (status == MiscServices::KeyboardStatus::NONE) {
        return;
    }
    SetKeyboardStatus(status == MiscServices::KeyboardStatus::SHOW);
}

void OnTextChangedListenerImpl::HandleFunctionKey(MiscServices::FunctionKey functionKey)
{
    auto task = [textField = field_, functionKey] {
        auto client = textField.Upgrade();
        if (!client) {
            LOGE("text field is null");
            return;
        }
        ContainerScope scope(client->instanceId_);
        TextInputAction action_ = static_cast<TextInputAction>(functionKey);
        switch (action_) {
            case TextInputAction::DONE:
            case TextInputAction::NEXT:
            case TextInputAction::SEARCH:
            case TextInputAction::SEND:
            case TextInputAction::GO:
                client->PerformAction(action_);
                break;
            default:
                LOGE("TextInputAction  is not support: %{public}d", action_);
                break;
        }
    };
    PostTaskToUI(task);
}

void OnTextChangedListenerImpl::MoveCursor(MiscServices::Direction direction)
{
    auto task = [textField = field_, direction] {
        auto client = textField.Upgrade();
        if (!client) {
            return;
        }
        ContainerScope scope(client->instanceId_);
        switch (direction) {
            case MiscServices::Direction::UP:
                client->CursorMoveUp();
                break;
            case MiscServices::Direction::DOWN:
                client->CursorMoveDown();
                break;
            case MiscServices::Direction::LEFT:
                client->CursorMoveLeft();
                break;
            case MiscServices::Direction::RIGHT:
                client->CursorMoveRight();
                break;
            default:
                LOGE("direction is not support: %{public}d", direction);
                break;
        }
    };
    PostTaskToUI(task);
}

void OnTextChangedListenerImpl::PostTaskToUI(const std::function<void()>& task)
{
    if (!task) {
        LOGE("task is empty");
        return;
    }
    auto renderTextField = field_.Upgrade();
    if (!renderTextField) {
        LOGE("text field is null");
        return;
    }

    auto context = renderTextField->GetContext().Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }

    auto taskExecutor = context->GetTaskExecutor();
    if (!taskExecutor) {
        LOGE("task executor is null");
        return;
    }

    taskExecutor->PostTask(task, TaskExecutor::TaskType::UI);
}

} // namespace OHOS::Ace
