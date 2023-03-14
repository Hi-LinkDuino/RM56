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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOGGLE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOGGLE_H

#include "core/components/padding/padding_component.h"
#include "core/components/text/text_component.h"
#include "core/components/toggle/toggle_component.h"
#include "core/components/toggle/toggle_theme.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMToggle final : public DOMNode {
    DECLARE_ACE_TYPE(DOMToggle, DOMNode);

public:
    DOMToggle(NodeId nodeId, const std::string& nodeName);
    ~DOMToggle() override = default;

    void InitializeStyle() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return toggleChild_;
    }

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;

private:
    void PrepareDisabledStyle();
    void ResetColorStyle();

    RefPtr<ToggleComponent> toggleChild_;
    RefPtr<TextComponent> textChild_;
    RefPtr<PaddingComponent> paddingChild_;
    RefPtr<ToggleTheme> toggleTheme_;

    TextStyle textStyle_;
    Color backgroundColor_;
    Color checkedColor_;
    Color textColor_;
    double blendOpacity_ = 1.0;
    bool isDisabled_ = false;
    bool colorChanged_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOGGLE_H
