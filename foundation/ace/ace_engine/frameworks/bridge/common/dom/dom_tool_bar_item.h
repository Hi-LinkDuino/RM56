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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOOL_BAR_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOOL_BAR_ITEM_H

#include "core/components/button/button_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/image/image_component.h"
#include "core/components/option/option_component.h"
#include "core/components/text/text_component.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/tool_bar/tool_bar_item_component.h"
#include "core/components/tool_bar/tool_bar_theme.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMToolBarItem final : public DOMNode {
    DECLARE_ACE_TYPE(DOMToolBarItem, DOMNode);

public:
    DOMToolBarItem(NodeId nodeId, const std::string& nodeName);
    ~DOMToolBarItem() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        if (icon_.empty() && value_.empty()) {
            return nullptr;
        }
        return toolBarItemChild_;
    }

    void SetIsEndItem(bool isEndItem)
    {
        isEndItem_ = isEndItem;
        PrepareSpecializedComponent();
    }

    void SetPosition(PositionType positionType)
    {
        toolBarItemChild_->SetPositionType(positionType);
    }

    RefPtr<OptionComponent> BuildOptionComponent();
    void InitializeStyle() override;

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;

private:
    void InitImageStyle();
    void InitTextStyle();
    void InitializedToolBarItemChild();
    void BuildEndItemComponent(std::list<RefPtr<Component>>& children);
    void BuildCommonComponent(std::list<RefPtr<Component>>& children);
    const RefPtr<Component> SetPadding(const RefPtr<Component>& component, Edge&& edge);

    RefPtr<ToolBarItemComponent> toolBarItemChild_;
    RefPtr<TextComponent> textChild_;
    RefPtr<ImageComponent> imageChild_;
    TextStyle textStyle_;
    RefPtr<ToolBarTheme> theme_;

    EventMarker clickEventId_;
    bool isEndItem_ = false;
    std::string value_;
    std::string icon_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOOL_BAR_ITEM_H
