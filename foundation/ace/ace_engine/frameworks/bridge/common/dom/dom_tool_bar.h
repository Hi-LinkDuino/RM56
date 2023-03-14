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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOOL_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOOL_BAR_H

#include "core/components/option/option_component.h"
#include "core/components/tool_bar/tool_bar_component.h"
#include "core/components/tool_bar/tool_bar_item_component.h"
#include "frameworks/bridge/common/dom/dom_tool_bar_item.h"

namespace OHOS::Ace::Framework {

constexpr int32_t TOOL_BAR_DEFAULT_SIZE = 5;
constexpr int32_t TOOL_BAR_INVALID_INDEX = -1;

class DOMToolBar final : public DOMNode {
    DECLARE_ACE_TYPE(DOMToolBar, DOMNode);

public:
    DOMToolBar(NodeId nodeId, const std::string& nodeName);
    ~DOMToolBar() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return toolBarChild_;
    }

    const std::list<RefPtr<Component>>& GetMenuChildren() const
    {
        return menuChildren_;
    }
    void InitializeStyle() override;

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void PrepareSpecializedComponent() override;

private:
    void AddChildNode(const RefPtr<DOMNode>& child, int32_t slot, bool isRebuild);
    void Rebuild();

    RefPtr<ToolBarComponent> toolBarChild_;
    std::list<RefPtr<Component>> menuChildren_;
    RefPtr<DOMNode> preToolBarItem_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TOOL_BAR_H
