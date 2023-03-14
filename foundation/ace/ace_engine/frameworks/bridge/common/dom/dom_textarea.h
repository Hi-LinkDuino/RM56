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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TEXTAREA_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TEXTAREA_H

#include "core/components/text_field/text_field_component.h"
#include "core/components/text_field/textfield_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_input.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMTextarea final : public DOMNode {
    DECLARE_ACE_TYPE(DOMTextarea, DOMNode);

public:
    using DOMTextareaMap = std::unordered_map<std::string, void (*)(const std::string&, DOMTextarea&)>;

    DOMTextarea(NodeId nodeId, const std::string& nodeName);
    ~DOMTextarea() override = default;

    void InitializeStyle() override;
    void OnRequestFocus(bool shouldFocus) override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return textAreaChild_;
    }

    void SetInputOptions(const std::vector<InputOption>& inputOptions)
    {
        inputOptions_ = inputOptions;
    }

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;
    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;

private:
    void UpdateDecoration();

    bool hasBoxRadius_ = false;
    RefPtr<TextFieldComponent> textAreaChild_;
    TextStyle textStyle_;

    // Callback id for when text is changed.
    EventMarker onTextChange_;
    Radius defaultRadius_;

    std::vector<InputOption> inputOptions_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TEXTAREA_H
