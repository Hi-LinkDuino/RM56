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

#include "frameworks/bridge/common/dom/dom_navigation_menu.h"

#include "frameworks/bridge/common/dom/dom_navigation_bar.h"

namespace OHOS::Ace::Framework {

DOMNavigationMenu::DOMNavigationMenu(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    menuChild_ = AceType::MakeRefPtr<MenuComponent>(std::to_string(nodeId), nodeName);
    menuChild_->SetTextDirection((IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR));
}

void DOMNavigationMenu::InitializeStyle()
{
    menuChild_->InitTheme(GetThemeManager());
}

void DOMNavigationMenu::ResetInitializedStyle()
{
    InitializeStyle();
}

void DOMNavigationMenu::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child) {
        return;
    }
    auto option = AceType::DynamicCast<OptionComponent>(child->GetSpecializedComponent());
    if (!option) {
        return;
    }
    if (navigationBar_) {
        navigationBar_->GetData()->allMenuItems.push_back(option);
        UpdateNavigationBar();
    }
}

void DOMNavigationMenu::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!child) {
        return;
    }
    auto option = AceType::DynamicCast<OptionComponent>(child->GetSpecializedComponent());
    if (!option) {
        return;
    }
    if (navigationBar_) {
        navigationBar_->GetData()->allMenuItems.remove(option);
        UpdateNavigationBar();
    }
}

void DOMNavigationMenu::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    auto parentDom = AceType::DynamicCast<DomNavigationBar>(parentNode);
    if (!parentDom) {
        return;
    }

    RefPtr<Component> component = parentDom->GetSpecializedComponent();
    navigationBar_ = AceType::DynamicCast<NavigationBarComponent>(component);
}

bool DOMNavigationMenu::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_SELECTED) {
        menuChild_->SetOnSuccess(EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

void DOMNavigationMenu::UpdateNavigationBar()
{
    navigationBar_->MarkNeedUpdate();
    auto pipelineContext = pipelineContext_.Upgrade();
    if (!pipelineContext) {
        LOGE("pipelineContext_ is nullptr");
        return;
    }
    pipelineContext->ScheduleUpdate(GetParentNode()->GetRootComponent());
}

} // namespace OHOS::Ace::Framework