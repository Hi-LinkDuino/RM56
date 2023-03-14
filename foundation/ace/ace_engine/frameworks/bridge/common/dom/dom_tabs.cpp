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

#include "frameworks/bridge/common/dom/dom_tabs.h"

#include "frameworks/bridge/common/dom/dom_tab_bar.h"
#include "frameworks/bridge/common/dom/dom_tab_content.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

// global variable to track the tabs' controllerId
static uint32_t g_domNodeControllerId = 0;

uint32_t DOMTabs::GetGlobalTabControllerId()
{
    return ++g_domNodeControllerId;
}

DOMTabs::DOMTabs(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    flexChild_ = CreateChild();
    tabControllerId_ = GetGlobalTabControllerId();
    tabController_ = TabController::GetController(tabControllerId_);
}

RefPtr<FlexComponent> DOMTabs::CreateChild() const
{
    RefPtr<FlexComponent> child;
    if (vertical_) {
        child = AceType::MakeRefPtr<RowComponent>(
            FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    } else {
        child = AceType::MakeRefPtr<ColumnComponent>(
            FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    }
    if (IsRightToLeft()) {
        child->SetTextDirection(TextDirection::RTL);
    } else {
        child->SetTextDirection(TextDirection::LTR);
    }
    return child;
}

bool DOMTabs::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == TAB_INDEX) {
        tabIndex_ = std::max(0, StringToInt(attr.second));
        LOGD("DOMTabs Index: %{public}u ControllerId: %{public}u", tabIndex_, tabControllerId_);
        if (tabController_) {
            tabController_->SetIndexByController(tabIndex_);
        }
        for (const auto& domChild : GetChildList()) {
            if (AceType::InstanceOf<DOMTabBar>(domChild)) {
                AceType::DynamicCast<DOMTabBar>(domChild)->UpdateIndex(tabIndex_);
                break;
            }
        }
        return true;
    } else if (attr.first == TAB_IS_VERTICAL) {
        if (vertical_ != StringToBool(attr.second)) {
            vertical_ = StringToBool(attr.second);
            auto newChild = CreateChild();
            for (const auto& child : flexChild_->GetChildren()) {
                newChild->AppendChild(child);
            }
            flexChild_ = newChild;
            for (const auto& domChild : GetChildList()) {
                if (AceType::InstanceOf<DOMTabBar>(domChild)) {
                    AceType::DynamicCast<DOMTabBar>(domChild)->SetVertical(vertical_);
                }
                if (AceType::InstanceOf<DOMTabContent>(domChild)) {
                    AceType::DynamicCast<DOMTabContent>(domChild)->SetVertical(vertical_);
                }
            }
        }
        return true;
    }
    return false;
}

bool DOMTabs::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_CHANGE) {
        tabPageId_ = static_cast<uint32_t>(pageId);
        tabEventId_ = GetNodeIdForEvent();
        tabEventType_ = event;
        LOGD("DOMTabs AddEvent pageId:%{private}u EventId:%{private}s EventType:%{private}s",
            tabPageId_, tabEventId_.c_str(), tabEventType_.c_str());
        return true;
    }
    return false;
}

void DOMTabs::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child) {
        LOGE("Child is nullptr, add node failed, slot:%{public}d.", slot);
        return;
    }
    flexChild_->InsertChild(slot, child->GetRootComponent());
}

void DOMTabs::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!child) {
        LOGE("Child is nullptr, remove node failed.");
        return;
    }
    flexChild_->RemoveChild(child->GetRootComponent());
}

} // namespace OHOS::Ace::Framework
