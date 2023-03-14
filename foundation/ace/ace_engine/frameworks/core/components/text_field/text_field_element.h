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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_TEXT_FIELD_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_TEXT_FIELD_ELEMENT_H

#include "base/utils/label_target.h"
#include "core/components/text_field/render_text_field.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class TextFieldElement : public RenderElement, public FocusNode {
    DECLARE_ACE_TYPE(TextFieldElement, RenderElement, FocusNode);

public:
    ~TextFieldElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    void Update() override;
    void OnFocus() override;
    void OnBlur() override;
    bool OnKeyEvent(const KeyEvent& keyEvent) override;

    bool RequestKeyboard(bool needStartTwinkling = false);
    void CloseKeyboard(bool forceClose = false);
    void ShowError(const std::string& errorText);
    void Delete();
    void Insert(const std::string& args);

private:
    void OnSurfaceChanged(int32_t width, int32_t height, int32_t oldWidth, int32_t oldHeight);

    bool enabled_ = true;
    bool editingMode_ = false;
    bool isNextAction_ = false;
    bool isRequestFocus_ = false;
    int32_t callbackId_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_TEXT_FIELD_ELEMENT_H
