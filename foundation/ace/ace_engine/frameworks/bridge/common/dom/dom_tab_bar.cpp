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

#include "frameworks/bridge/common/dom/dom_tab_bar.h"

#include "core/components/tab_bar/tab_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMTabBar::DOMTabBar(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    std::list<RefPtr<Component>> tabBars;
    RefPtr<TabController> controller;
    tabBarIndicator_ = AceType::MakeRefPtr<TabBarIndicatorComponent>();
    tabBarChild_ = AceType::MakeRefPtr<TabBarComponent>(tabBars, controller, tabBarIndicator_);
}

void DOMTabBar::InitializeStyle()
{
    RefPtr<TabTheme> theme = GetTheme<TabTheme>();
    if (!theme) {
        return;
    }
    if (boxComponent_) {
        boxComponent_->SetColor(theme->GetBackgroundColor());
    }
    auto paddingDimension = theme->GetPadding();
    padding_ = Edge(paddingDimension.Value(), 0.0, paddingDimension.Value(), 0.0, paddingDimension.Unit());
}

bool DOMTabBar::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_TAB_BAR_MODE) {
        tabBarMode_ = ConvertStrToTabBarMode(attr.second);
        return true;
    }
    return false;
}

bool DOMTabBar::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const std::unordered_map<std::string, void (*)(const std::string&, DOMTabBar&)> styleOperators = {
        { DOM_PADDING,
            [](const std::string& val, DOMTabBar& node) {
                node.padding_ = node.ParseEdge(val);
            } },
        { DOM_PADDING_END,
            [](const std::string& val, DOMTabBar& node) {
                if (node.IsRightToLeft()) {
                    node.padding_.SetLeft(node.ParseDimension(val));
                } else {
                    node.padding_.SetRight(node.ParseDimension(val));
                }
            } },
        { DOM_PADDING_LEFT,
            [](const std::string& val, DOMTabBar& node) {
                node.padding_.SetLeft(node.ParseDimension(val));
            } },
        { DOM_PADDING_RIGHT,
            [](const std::string& val, DOMTabBar& node) {
                node.padding_.SetRight(node.ParseDimension(val));
            } },
        { DOM_PADDING_START,
            [](const std::string& val, DOMTabBar& node) {
                if (node.IsRightToLeft()) {
                    node.padding_.SetRight(node.ParseDimension(val));
                } else {
                    node.padding_.SetLeft(node.ParseDimension(val));
                }
            } },
    };
    auto operatorIter = styleOperators.find(style.first);
    if (operatorIter != styleOperators.end()) {
        operatorIter->second(style.second, *this);
        return true;
    }
    return false;
}

void DOMTabBar::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child) {
        LOGE("Child is nullptr, add node failed, slot:%{public}d.", slot);
        return;
    }
    if (tabBarChild_) {
        LOGD("DOMTabBar Add Child in slot(=%{public}d)", slot);
        tabBarChild_->InsertChild(slot, child->GetRootComponent());
    }
}

void DOMTabBar::UpdateIndex(uint32_t currentIndex)
{
    uint32_t index = 0;
    for (const auto& childNode : GetChildList()) {
        if (index == currentIndex) {
            OnChildActive(childNode, true);
        } else if (index == lastIndex_) {
            OnChildActive(childNode, false);
        }
        index++;
    }
    lastIndex_ = currentIndex;
}

void DOMTabBar::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    if (!parentNode) {
        LOGE("parentNode is nullptr, mount node failed");
        return;
    }
    if (parentNode->GetTag() == DOM_NODE_TAG_TABS) {
        const auto& parentNodeTmp = AceType::DynamicCast<DOMTabs>(parentNode);
        if (!parentNodeTmp) {
            LOGE("DynamicCast DOMTabs failed");
            return;
        }
        lastIndex_ = parentNodeTmp->GetTabIndex();
        controllerId_ = parentNodeTmp->GetTabControllerId();
        const auto& controller = parentNodeTmp->GetTabController();
        controller->SetIndexWithoutChangeContent(static_cast<int32_t>(lastIndex_));
        tabBarChild_->SetController(controller);
        PrepareChangeListener();
    }
}

void DOMTabBar::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!child) {
        LOGE("Child is nullptr, remove node failed.");
        return;
    }
    if (tabBarChild_) {
        LOGD("DOMTabBar remove child");
        tabBarChild_->RemoveChild(child->GetRootComponent());
    }
}

void DOMTabBar::ResetInitializedStyle()
{
    if (!boxComponent_) {
        LOGE("BoxComponent is null, reset style failed.");
        return;
    }
    RefPtr<TabTheme> theme = GetTheme<TabTheme>();
    if (theme) {
        tabBarChild_->InitStyle(theme);
        if (vertical_) {
            if (LessOrEqual(GetWidth().Value(), 0.0)) {
                boxComponent_->SetWidth(theme->GetDefaultWidth().Value(), theme->GetDefaultWidth().Unit());
            }
        } else {
            if (LessOrEqual(GetHeight().Value(), 0.0)) {
                boxComponent_->SetHeight(theme->GetDefaultHeight().Value(), theme->GetDefaultHeight().Unit());
            }
        }
    }
}

void DOMTabBar::PrepareSpecializedComponent()
{
    tabBarChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    tabBarChild_->SetMode(tabBarMode_);
    const auto& parentNodeTmp = AceType::DynamicCast<DOMTabs>(parentNode_.Upgrade());
    if (parentNodeTmp) {
        vertical_ = parentNodeTmp->IsVertical();
    }
    tabBarChild_->SetVertical(vertical_);
    ResetInitializedStyle();
    tabBarChild_->SetPadding(padding_);
}

void DOMTabBar::PrepareChangeListener()
{
    // used for initailzed the active tabBarItem
    auto weak = AceType::WeakClaim(this);
    auto changeCallback = [weak](uint32_t currentIndex) {
        auto tabBarNode = weak.Upgrade();
        if (!tabBarNode) {
            LOGE("get dom node failed!");
            return;
        }
        tabBarNode->UpdateIndex(currentIndex);
    };
    auto changeMarker = BackEndEventManager<void(uint32_t)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(uint32_t)>::GetInstance().BindBackendEvent(changeMarker, changeCallback);
    tabBarChild_->SetDomChangeEventId(changeMarker);
}

void DOMTabBar::OnChildActive(const RefPtr<DOMNode>& node, bool active)
{
    node->OnActive(active);
    for (const auto& childNode : node->GetChildList()) {
        childNode->OnActive(active);
    }
}

Edge DOMTabBar::ParseEdge(const std::string& value) const
{
    Edge edge;
    std::vector<std::string> offsets;
    StringUtils::StringSpliter(value, ' ', offsets);
    switch (offsets.size()) {
        case 1:
            edge.SetLeft(ParseDimension(offsets[0]));
            edge.SetRight(ParseDimension(offsets[0]));
            edge.SetTop(ParseDimension(offsets[0]));
            edge.SetBottom(ParseDimension(offsets[0]));
            break;
        case 2:
            edge.SetLeft(ParseDimension(offsets[0]));
            edge.SetRight(ParseDimension(offsets[1]));
            edge.SetTop(ParseDimension(offsets[0]));
            edge.SetBottom(ParseDimension(offsets[0]));
            break;
        case 3:
            edge.SetLeft(ParseDimension(offsets[1]));
            edge.SetRight(ParseDimension(offsets[1]));
            edge.SetTop(ParseDimension(offsets[0]));
            edge.SetBottom(ParseDimension(offsets[2]));
            break;
        case 4:
            edge.SetLeft(ParseDimension(offsets[3]));
            edge.SetRight(ParseDimension(offsets[1]));
            edge.SetTop(ParseDimension(offsets[0]));
            edge.SetBottom(ParseDimension(offsets[2]));
            break;
        default:
            break;
    }
    return edge;
}

} // namespace OHOS::Ace::Framework
