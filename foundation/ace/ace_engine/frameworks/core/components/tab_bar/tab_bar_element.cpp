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

#include "core/components/tab_bar/tab_bar_element.h"

#include "base/utils/system_properties.h"
#include "core/components/tab_bar/render_tab_bar.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {

RefPtr<RenderNode> TabBarElement::CreateRenderNode()
{
    RefPtr<RenderNode> node = ComponentGroupElement::CreateRenderNode();

    RefPtr<RenderTabBar> tabBar = AceType::DynamicCast<RenderTabBar>(node);
    if (tabBar) {
        tabBar->RegisterCallback([weakTabBarElement = AceType::WeakClaim(this)](int32_t index) {
            auto tabBar = weakTabBarElement.Upgrade();
            if (tabBar) {
                tabBar->UpdateElement(index);
            }
        });
    }
    return node;
}

void TabBarElement::UpdateElement(int32_t index)
{
    if (controller_) {
        int32_t preIndex = controller_->GetIndex();
        controller_->SetIndex(index);
        int32_t curIndex = controller_->GetIndex();
        if (preIndex != curIndex) {
            LOGD("TabBar change from %{public}d to %{public}d", preIndex, curIndex);
            controller_->ChangeDispatch(curIndex);
        }
        controller_->OnTabBarChanged(index);
        UpdateIndex(curIndex);
    }
}

void TabBarElement::Update()
{
    if (component_) {
        RefPtr<TabBarComponent> tabBar = AceType::DynamicCast<TabBarComponent>(component_);
        if (!tabBar) {
            LOGE("TabBarElement::Update: get TabBarComponent failed!");
            return;
        }
        indicatorStyle_ = tabBar->GetIndicator();
        focusIndicatorStyle_ = tabBar->GetFocusIndicator();
        tabs_.clear();
        tabBar->BuildItems(tabs_);
        vertical_ = tabBar->IsVertical();
        auto controller = tabBar->GetController();
        if (controller && (controller_ != controller)) {
            // Get index from old controller before replace.
            if (!controller->IsIndexDefined() && controller_) {
                controller->SetIndexWithoutChangeContent(controller_->GetIndex());
            }
            controller_ = controller;
        }
        if (!controller_) {
            LOGE("fail to get controller");
            return;
        }
        controller_->SetBarElement(AceType::Claim(this));
        auto domChangeEvent = AceAsyncEvent<void(uint32_t)>::Create(tabBar->GetDomChangeEventId(), GetContext());
        if (domChangeEvent) {
            domChangeEvent(controller_->GetIndex());
        }
    }
    ComponentGroupElement::Update();
}

bool TabBarElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    if (vertical_ == vertical) {
        if (GoToNextFocus(reverse)) {
            int32_t index = std::distance(focusNodes_.begin(), itLastFocusNode_);
            UpdateElement(index);
            return true;
        }
    }
    return false;
}

void TabBarElement::UpdateIndex(int32_t index)
{
    RefPtr<RenderTabBar> tabBar = AceType::DynamicCast<RenderTabBar>(renderNode_);
    if (tabBar) {
        tabBar->SetIndex(index);
    }
}

void TabBarElement::PerformBuild()
{
    if (tabs_.empty()) {
        LOGD("tabs is empty");
        ComponentGroupElement::PerformBuild();
        return;
    }
    auto itChild = children_.begin();
    auto itChildEnd = children_.end();
    auto itComponent = tabs_.begin();
    auto itComponentEnd = tabs_.end();

    while (itChild != itChildEnd && itComponent != itComponentEnd) {
        UpdateChild(*(itChild++), *(itComponent++));
    }
    // delete useless element.
    while (itChild != itChildEnd) {
        UpdateChild(*(itChild++), nullptr);
    }
    // update component.
    while (itComponent != itComponentEnd) {
        UpdateChild(nullptr, *(itComponent++));
    }
    GetRenderNode()->MarkNeedLayout();
}

void TabBarElement::OnFocus()
{
    LOGD("TabBar element OnFocus");
    RefPtr<RenderTabBar> tabBar = AceType::DynamicCast<RenderTabBar>(renderNode_);
    if (tabBar) {
        tabBar->HandleFocusEvent(true);
        if (focusIndicatorStyle_) {
            tabBar->UpdateIndicatorStyle(focusIndicatorStyle_);
        }
    }
    if (controller_) {
        int32_t index = controller_->GetIndex();
        int32_t size = static_cast<int32_t>(focusNodes_.size());
        if (size > 0) {
            size--;
        }
        index = std::clamp(index, 0, size);
        itLastFocusNode_ = focusNodes_.begin();
        std::advance(itLastFocusNode_, index);
    }

    return FocusGroup::OnFocus();
}

void TabBarElement::OnBlur()
{
    LOGD("TabBar element Onblur");
    RefPtr<RenderTabBar> tabBar = AceType::DynamicCast<RenderTabBar>(renderNode_);
    if (tabBar) {
        tabBar->HandleFocusEvent(false);
        if (indicatorStyle_) {
            tabBar->UpdateIndicatorStyle(indicatorStyle_);
        }
    }
    return FocusGroup::OnBlur();
}

} // namespace OHOS::Ace