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

#include "frameworks/bridge/common/dom/dom_navigation_bar.h"

#include "base/log/event_report.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

const char EMPHASIZE[] = "emphasize";
NavigationBarType GetType(const std::string& value)
{
    return value == EMPHASIZE ? NavigationBarType::EMPHASIZE : NavigationBarType::NORMAL;
}

} // namespace

DomNavigationBar::DomNavigationBar(NodeId nodeId, const std::string& nodeName)
    : DOMNode(nodeId, nodeName),
      navigationBarComponent_(AceType::MakeRefPtr<NavigationBarComponent>(std::to_string(nodeId), nodeName)),
      navigationBarData_(navigationBarComponent_->GetData())
{
    if (IsRightToLeft()) {
        navigationBarComponent_->SetTextDirection(TextDirection::RTL);
    }
}

void DomNavigationBar::InitializeStyle()
{
    auto theme = GetTheme<NavigationBarTheme>();
    navigationBarData_->theme = theme;
    navigationBarData_->subTitleColor = theme->GetSubTitleColor();
    navigationBarData_->titleColor = theme->GetTitleColor();
}

void DomNavigationBar::ResetInitializedStyle()
{
    InitializeStyle();
}

#ifndef WEARABLE_PRODUCT
void DomNavigationBar::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!child || !navigationBarData_) {
        return;
    }

    auto menu = AceType::DynamicCast<MenuComponent>(child->GetSpecializedComponent());
    if (menu) {
        navigationBarData_->menu = nullptr;
        navigationBarData_->allMenuItems.clear();
        return;
    }
    auto tabBar = AceType::DynamicCast<TabBarComponent>(child->GetSpecializedComponent());
    if (tabBar) {
        navigationBarData_->tabBar = nullptr;
        return;
    }

    auto selectPopup = AceType::DynamicCast<SelectComponent>(child->GetSpecializedComponent());
    if (selectPopup) {
        navigationBarData_->selectPopup = nullptr;
    } else {
        LOGW("navigation bar child node type not support");
    }
}

void DomNavigationBar::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child || !navigationBarComponent_) {
        return;
    }

    auto menu = AceType::DynamicCast<MenuComponent>(child->GetSpecializedComponent());
    if (menu) {
        navigationBarData_->menu = menu;
        return;
    }
    auto tabBar = AceType::DynamicCast<TabBarComponent>(child->GetSpecializedComponent());
    if (tabBar) {
        navigationBarData_->tabBar = tabBar;
        return;
    }

    auto selectPopup = AceType::DynamicCast<SelectComponent>(child->GetSpecializedComponent());
    if (selectPopup) {
        navigationBarData_->selectPopup = selectPopup;
    } else {
        LOGW("navigation bar child node type not support");
    }
}
#endif

void DomNavigationBar::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (!navigationBarData_->controller) {
        LOGE("get navigation bar controller failed");
        EventReport::SendComponentException(ComponentExcepType::NAVIGATION_BAR_ERR);
        return;
    }
    if (method == DOM_NAVIGATION_BAR_METHOD_SHOW) {
        navigationBarData_->controller->Show();
    } else if (method == DOM_NAVIGATION_BAR_METHOD_HIDE) {
        navigationBarData_->controller->Hide();
    } else {
        LOGE("call not support method: %{private}s", method.c_str());
    }
}

void DomNavigationBar::PrepareSpecializedComponent()
{
    navigationBarData_->imageFill = GetImageFill();
}

bool DomNavigationBar::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, const RefPtr<NavigationBarData>&)> attrOperators[] = {
        { DOM_NAVIGATION_BAR_BACK_ENABLED,
            [](const std::string& val, const RefPtr<NavigationBarData>& navigationBar) {
                navigationBar->backEnabled = StringToBool(val);
            } },
        { DOM_NAVIGATION_BAR_END_ICON,
            [](const std::string& val, const RefPtr<NavigationBarData>& navigationBar) {
                navigationBar->endIcon = val;
            } },
        { DOM_NAVIGATION_BAR_HEADER, [](const std::string& val,
                                     const RefPtr<NavigationBarData>& navigationBar) { navigationBar->header = val; } },
        { DOM_NAVIGATION_BAR_LOGO,
            [](const std::string& val, const RefPtr<NavigationBarData>& navigationBar) { navigationBar->logo = val; } },
        { DOM_NAVIGATION_BAR_START_ICON,
            [](const std::string& val, const RefPtr<NavigationBarData>& navigationBar) {
                navigationBar->startIcon = val;
            } },
        { DOM_NAVIGATION_BAR_SUBTITLE,
            [](const std::string& val, const RefPtr<NavigationBarData>& navigationBar) {
                navigationBar->subTitle = val;
            } },
        { DOM_NAVIGATION_BAR_TITLE, [](const std::string& val,
                                    const RefPtr<NavigationBarData>& navigationBar) { navigationBar->title = val; } },
        { DOM_NAVIGATION_BAR_TYPE,
            [](const std::string& val, const RefPtr<NavigationBarData>& navigationBar) {
                navigationBar->type = GetType(val);
            } },
    };
    auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        attrOperators[operatorIter].value(attr.second, navigationBarData_);
        return true;
    }
    return false;
}

bool DomNavigationBar::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, const DomNavigationBar&)> navigationBarStyleOperators[] = {
        { DOM_NAVIGATION_BAR_SUBTITLE_COLOR,
            [](const std::string& val, const DomNavigationBar& node) {
                node.navigationBarData_->subTitleColor = node.ParseColor(val);
            } },
        { DOM_NAVIGATION_BAR_TITLE_COLOR,
            [](const std::string& val, const DomNavigationBar& node) {
                node.navigationBarData_->titleColor = node.ParseColor(val);
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(navigationBarStyleOperators, ArraySize(navigationBarStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        navigationBarStyleOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

bool DomNavigationBar::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_NAVIGATION_BAR_EVENT_BACK_CLICK) {
        navigationBarData_->backClickMarker = EventMarker(GetNodeIdForEvent(), event, pageId);
    } else if (event == DOM_NAVIGATION_BAR_EVENT_START_CLICK) {
        navigationBarData_->startClickMarker = EventMarker(GetNodeIdForEvent(), event, pageId);
    } else if (event == DOM_NAVIGATION_BAR_EVENT_END_CLICK) {
        navigationBarData_->endClickMarker = EventMarker(GetNodeIdForEvent(), event, pageId);
    } else {
        return false;
    }
    return true;
}

} // namespace OHOS::Ace::Framework
