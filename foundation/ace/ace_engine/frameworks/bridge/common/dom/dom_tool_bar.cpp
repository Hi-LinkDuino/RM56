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

#include "frameworks/bridge/common/dom/dom_tool_bar.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMToolBar::DOMToolBar(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    toolBarChild_ = AceType::MakeRefPtr<ToolBarComponent>(std::list<RefPtr<Component>>());
}

void DOMToolBar::InitializeStyle()
{
    RefPtr<ToolBarTheme> theme = GetTheme<ToolBarTheme>();
    if (!theme || !declaration_) {
        LOGE("ToolBarTheme or declaration is null");
        return;
    }
    declaration_->GetBackDecoration()->SetBackgroundColor(theme->GetToolBarBgColor());
    declaration_->SetHasDecorationStyle(true);
}

void DOMToolBar::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    AddChildNode(child, slot, false);
}

void DOMToolBar::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    Rebuild();
}

void DOMToolBar::Rebuild()
{
    toolBarChild_->ClearChildren();
    menuChildren_.clear();

    if (GetChildList().empty()) {
        return;
    }

    for (const auto& pos : GetChildList()) {
        AddChildNode(pos, TOOL_BAR_INVALID_INDEX, true);
    }
}

void DOMToolBar::AddChildNode(const RefPtr<DOMNode>& child, int32_t slot, bool isRebuild)
{
    if (!child) {
        LOGE("Child is nullptr, add node failed, slot:%{public}d.", slot);
        return;
    }

    RefPtr<DOMToolBarItem> domToolBarItem = AceType::DynamicCast<DOMToolBarItem>(child);
    if (!domToolBarItem || !domToolBarItem->GetSpecializedComponent()) {
        return;
    }
    if (isRebuild) {
        domToolBarItem->SetIsEndItem(false);
        domToolBarItem->MarkNeedUpdate();
    }

    if (toolBarChild_->GetChildren().size() > TOOL_BAR_DEFAULT_SIZE) {
        menuChildren_.emplace_back(domToolBarItem->BuildOptionComponent());
        return;
    }

    domToolBarItem->SetPosition(GetPosition());
    if (toolBarChild_->GetChildren().size() == TOOL_BAR_DEFAULT_SIZE) {
        if (preToolBarItem_) {
            RefPtr<DOMToolBarItem> preDomToolBarItem = AceType::DynamicCast<DOMToolBarItem>(preToolBarItem_);
            if (preDomToolBarItem) {
                RefPtr<ToolBarItemComponent> toolBarItemComponent =
                    AceType::DynamicCast<ToolBarItemComponent>(preDomToolBarItem->GetSpecializedComponent());
                if (toolBarItemComponent) {
                    preDomToolBarItem->SetIsEndItem(true);
                    toolBarItemComponent->SetOptionChildrenCallBack([this]() { return GetMenuChildren(); });
                    toolBarItemComponent->SetClickedEventId(EventMarker());
                    menuChildren_.emplace_back(preDomToolBarItem->BuildOptionComponent());
                }
            }
        }
        menuChildren_.emplace_back(domToolBarItem->BuildOptionComponent());
        preToolBarItem_ = nullptr;
        return;
    }

    if (slot == TOOL_BAR_INVALID_INDEX) {
        toolBarChild_->AppendChild(child->GetRootComponent());
    } else {
        toolBarChild_->InsertChild(slot, child->GetRootComponent());
    }
    preToolBarItem_ = child;
}

void DOMToolBar::PrepareSpecializedComponent()
{
    if (toolBarChild_) {
        toolBarChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    }
}

} // namespace OHOS::Ace::Framework
