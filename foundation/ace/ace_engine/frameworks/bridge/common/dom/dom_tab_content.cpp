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

#include "frameworks/bridge/common/dom/dom_tab_content.h"

#include "frameworks/bridge/common/dom/dom_tab_bar.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMTabContent::DOMTabContent(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    std::list<RefPtr<Component>> tabContents;
    RefPtr<TabController> controller;
    tabContentChild_ = AceType::MakeRefPtr<TabContentComponent>(tabContents, controller);
}

bool DOMTabContent::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_TAB_CONTENT_SCROLLABEL) {
        scrollable_ = StringToBool(attr.second);
        tabContentChild_->SetScrollable(scrollable_);
        return true;
    }
    return false;
}

void DOMTabContent::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child) {
        LOGE("Child is nullptr, add node failed, slot:%{public}d.", slot);
        return;
    }
    if (tabContentChild_) {
        tabContentChild_->InsertChild(slot, child->GetRootComponent());
    }
}

void DOMTabContent::PrepareChangeListener(const RefPtr<DOMTabs>& parentNode)
{
    auto weak = AceType::WeakClaim(this);
    WeakPtr<DOMTabs> weakParent = parentNode;
    auto changeCallback = [weak, weakParent](uint32_t currentIndex) {
        auto domNode = weak.Upgrade();
        if (!domNode) {
            LOGE("get dom node failed!");
            return;
        }

        auto parentRef = weakParent.Upgrade();
        if (!parentRef) {
            LOGE("get parent node failed!");
            return;
        }
        for (const auto& node : parentRef->GetChildList()) {
            if (node->GetTag() == DOM_NODE_TAG_TAB_BAR) {
                const auto& tabBarNode = AceType::DynamicCast<DOMTabBar>(node);
                if (tabBarNode) {
                    tabBarNode->UpdateIndex(currentIndex);
                    break;
                }
            }
        }
    };
    auto changeMarker = BackEndEventManager<void(uint32_t)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(uint32_t)>::GetInstance().BindBackendEvent(changeMarker, changeCallback);
    tabContentChild_->SetDomChangeEventId(changeMarker);
}

void DOMTabContent::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    if (!parentNode) {
        return;
    }
    if (parentNode->GetTag() == DOM_NODE_TAG_TABS) {
        const auto& parentNodeTmp = AceType::DynamicCast<DOMTabs>(parentNode);
        if (!parentNodeTmp) {
            return;
        }
        index_ = parentNodeTmp->GetTabIndex();
        controllerId_ = parentNodeTmp->GetTabControllerId();
        const auto& controller = parentNodeTmp->GetTabController();
        controller->SetIndexWithoutChangeContent(static_cast<int32_t>(index_));
        tabContentChild_->SetController(controller);

        vertical_ = parentNodeTmp->IsVertical();
        tabContentChild_->SetVertical(vertical_);

        changeEventId_ = EventMarker(
            parentNodeTmp->GetTabEventId(), parentNodeTmp->GetTabEventType(), parentNodeTmp->GetTabPageId());
        if (parentNodeTmp->GetTabEventType() == DOM_CHANGE) {
            tabContentChild_->SetChangeEventId(changeEventId_);
        }
        PrepareChangeListener(parentNodeTmp);
    }
}

void DOMTabContent::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!child) {
        LOGE("Child is nullptr, remove node failed.");
        return;
    }
    if (tabContentChild_) {
        tabContentChild_->RemoveChild(child->GetRootComponent());
    }
}

void DOMTabContent::PrepareSpecializedComponent()
{
    if (tabContentChild_) {
        tabContentChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    }
}

} // namespace OHOS::Ace::Framework
