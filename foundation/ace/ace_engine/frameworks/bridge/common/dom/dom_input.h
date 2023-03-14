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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_INPUT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_INPUT_H

#include "core/components/common/properties/border.h"
#include "core/components/common/properties/input_option.h"
#include "core/components/checkable/checkable_component.h"
#include "core/pipeline/base/component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/dom/form_value.h"

namespace OHOS::Ace::Framework {

class DOMInput final : public DOMNode, public FormValue {
    DECLARE_ACE_TYPE(DOMInput, DOMNode, FormValue);

public:
    DOMInput(NodeId nodeId, const std::string& nodeName);
    ~DOMInput() override;

    void CallSpecializedMethod(const std::string& method, const std::string& args) override;
    void OnRequestFocus(bool shouldFocus) override;
    RefPtr<Component> GetSpecializedComponent() override;

    void SetInputOptions(const std::vector<InputOption>& inputOptions)
    {
        inputOptions_ = inputOptions;
    }

    bool CheckPseduo(RefPtr<Decoration> decoration) const;

protected:
    void OnMounted(const RefPtr<DOMNode>& parentNode) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

    void PrepareSpecializedComponent() override;

    void ResetInitializedStyle() override;

    void OnReset() override;

protected:
    void UpdateSpecializedComponent();

    void CreateSpecializedComponent();

    void UpdateSpecializedComponentStyle();

    void AddSpecializedComponentEvent();

    template<typename T, typename S>
    void InitCheckable()
    {
        const auto& theme = GetTheme<S>();
        const auto& checkableComponent = AceType::DynamicCast<T>(inputChild_);
        if (theme && checkableComponent) {
            checkableComponent->ApplyTheme(theme);
            SetWidth(theme->GetWidth());
            SetHeight(theme->GetHeight());
        }
    }

private:
    void PrepareCheckedListener();
    void CheckSubmitAndResetType();
    void SetFormValueListener();

    void HandlePadding(RefPtr<Component> component, RefPtr<BoxComponent> boxComponent,
        const std::map<std::string, std::string>& styles);

    RefPtr<Component> inputChild_;
    WeakPtr<DOMNode> formNode_;
    std::pair<std::string, bool> type_ = { "text", false };

    std::map<std::string, std::string> inputAttrs_;
    std::map<std::string, std::string> inputStyles_;
    std::vector<std::string> tempEvent_;
    EventMarker checkableChangeMarker_;
    int32_t pageId_ = -1; // invalid id.
    Border boxBorder_;
    std::vector<InputOption> inputOptions_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_INPUT_H
