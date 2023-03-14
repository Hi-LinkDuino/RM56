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

#include "core/components/text_field/text_field_element.h"

#include "core/components/text_field/text_field_component.h"
#include "core/components/text_field/text_field_controller.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t KEYBOARD_HEIGHT = 200;

} // namespace

TextFieldElement::~TextFieldElement()
{
    auto context = context_.Upgrade();
    if (context) {
        context->UnregisterSurfaceChangedCallback(callbackId_);
    }
}

void TextFieldElement::Update()
{
    RenderElement::Update();

    auto labelTarget = AceType::DynamicCast<LabelTarget>(component_);
    if (labelTarget) {
        auto trigger = labelTarget->GetTrigger();
        if (trigger) {
            auto weak = AceType::WeakClaim(this);
            trigger->clickHandler_ = [weak]() {
                auto textField = weak.Upgrade();
                if (textField) {
                    textField->RequestKeyboard(true);
                }
            };
        }
    }

    auto textField = AceType::DynamicCast<TextFieldComponent>(component_);
    if (!textField) {
        return;
    }

    if (textField->GetTextFieldController()) {
        textField->GetTextFieldController()->SetHandler(AceType::WeakClaim(this));
    }
    enabled_ = textField->IsEnabled();

    // If auto focus, request keyboard immediately.
    if (textField->GetAutoFocus()) {
        RequestKeyboard(true);
    }

    auto context = context_.Upgrade();
    if (context && callbackId_ <= 0) {
        callbackId_ = context->RegisterSurfaceChangedCallback(
            [weak = WeakClaim(this)](int32_t width, int32_t height, int32_t oldWidth, int32_t oldHeight) {
                auto textField = weak.Upgrade();
                if (textField) {
                    textField->OnSurfaceChanged(width, height, oldWidth, oldHeight);
                }
            });
    }
}

RefPtr<RenderNode> TextFieldElement::CreateRenderNode()
{
    RefPtr<RenderNode> node = RenderElement::CreateRenderNode();

    auto renderNode = AceType::DynamicCast<RenderTextField>(node);
    if (renderNode) {
        renderNode->RegisterTapCallback([wp = AceType::WeakClaim(this)]() {
            auto sp = wp.Upgrade();
            if (sp) {
                return sp->RequestKeyboard();
            }
            return false;
        });
        renderNode->SetNextFocusEvent([wp = AceType::WeakClaim(this)]() {
            auto sp = wp.Upgrade();
            if (sp) {
                auto pipeline = sp->context_.Upgrade();
                if (!pipeline) {
                    LOGW("pipeline is null.");
                    return;
                }
                sp->isNextAction_ = true;

                KeyEvent keyEvent(KeyCode::KEY_DPAD_DOWN, KeyAction::UP);
                if (!pipeline->OnKeyEvent(keyEvent)) {
                    sp->CloseKeyboard();
                } else {
                    // below textfield will auto open keyboard
                    KeyEvent keyEventEnter(KeyCode::KEY_ENTER, KeyAction::UP);
                    pipeline->OnKeyEvent(keyEventEnter);
                }
            }
        });

        renderNode->SetOnOverlayFocusChange([wp = AceType::WeakClaim(this)](bool isFocus) {
            auto sp = wp.Upgrade();
            if (sp) {
                if (!isFocus && !sp->isRequestFocus_) {
                    sp->CloseKeyboard();
                }
            }
        });

        renderNode->SetOnIsCurrentFocus([wp = AceType::WeakClaim(this)]() -> bool {
            auto sp = wp.Upgrade();
            if (sp) {
                return sp->IsCurrentFocus();
            }
            return false;
        });
    }
    return node;
}

bool TextFieldElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if (!enabled_) {
        return false;
    }

    if (editingMode_ && FocusNode::OnKeyEvent(keyEvent)) {
        return true;
    }

    if (editingMode_) {
        auto textField = DynamicCast<RenderTextField>(renderNode_);
        if (textField && textField->OnKeyEvent(keyEvent)) {
            return true;
        }
    }

    if (keyEvent.action != KeyAction::UP) {
        return false;
    }

    switch (keyEvent.code) {
        case KeyCode::KEY_BACK:
        case KeyCode::KEY_ESCAPE: {
            bool editingMode = editingMode_;
            CloseKeyboard();
            // If not editingMode, mark the keyevent unhandled to let navigator pop page..
            return editingMode;
        }
        case KeyCode::KEY_ENTER:
        case KeyCode::KEY_NUMPAD_ENTER:
        case KeyCode::KEY_DPAD_CENTER:
            RequestKeyboard(true);
            return true;
        case KeyCode::KEY_DPAD_LEFT:
        case KeyCode::KEY_DPAD_RIGHT:
        case KeyCode::KEY_DPAD_UP:
        case KeyCode::KEY_DPAD_DOWN: {
            bool result = editingMode_ && !isNextAction_;
            isNextAction_ = false;
            return result;
        }
        default:
            return false;
    }
}

void TextFieldElement::OnFocus()
{
    if (!enabled_) {
        return;
    }
    FocusNode::OnFocus();
    renderNode_->ChangeStatus(RenderStatus::FOCUS);
}

void TextFieldElement::OnBlur()
{
    if (!enabled_) {
        return;
    }
    if (renderNode_) {
        renderNode_->ChangeStatus(RenderStatus::BLUR);
    }
    auto textField = DynamicCast<RenderTextField>(renderNode_);
    if (textField) {
        textField->StopTwinkling();
    }
    CloseKeyboard();
    FocusNode::OnBlur();
}

void TextFieldElement::OnSurfaceChanged(int32_t width, int32_t height, int32_t oldWidth, int32_t oldHeight)
{
    // If height of surface append beyond 200, we think soft keyboard is closed.
    if (oldWidth == width && height - oldHeight > KEYBOARD_HEIGHT) {
        editingMode_ = false;
    }
}

void TextFieldElement::CloseKeyboard(bool forceClose)
{
    isRequestFocus_ = false;
    auto textField = DynamicCast<RenderTextField>(renderNode_);
    if (textField) {
        if (textField->CloseKeyboard(forceClose)) {
            editingMode_ = false;
        }
    }
}

bool TextFieldElement::RequestKeyboard(bool needStartTwinkling)
{
    if (!enabled_) {
        return false;
    }
    isRequestFocus_ = true;
    if (RequestFocusImmediately()) {
        auto textField = DynamicCast<RenderTextField>(renderNode_);
        if (textField) {
            if (textField->RequestKeyboard(!editingMode_, needStartTwinkling)) {
                editingMode_ = true;
            }
        }
        return true;
    } else {
        isRequestFocus_ = false;
        return false;
    }
}

void TextFieldElement::ShowError(const std::string& errorText)
{
    auto textField = DynamicCast<RenderTextField>(renderNode_);
    if (textField) {
        textField->ShowError(errorText);
    }
}

void TextFieldElement::Delete()
{
    auto textField = DynamicCast<RenderTextField>(renderNode_);
    if (!textField) {
        return;
    }
    auto value = textField->GetEditingValue();
    if (value.text.empty()) {
        RequestKeyboard(true);
        return;
    }
    if (editingMode_) {
        auto start = value.selection.GetStart();
        auto end = value.selection.GetEnd();
        if (start >= 0 && end > 0) {
            textField->Delete(start == end ? start - 1 : start, end);
        }
    } else {
        textField->Delete(value.GetWideText().size() - 1, value.GetWideText().size());
    }
    RequestKeyboard(true);
}

void TextFieldElement::Insert(const std::string& args)
{
    auto textField = DynamicCast<RenderTextField>(renderNode_);
    if (!textField) {
        return;
    }
    textField->Insert(args);
}

} // namespace OHOS::Ace
