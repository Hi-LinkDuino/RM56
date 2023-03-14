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

#include "core/components/tab_bar/tab_controller.h"

#include "core/components/tab_bar/render_tab_bar.h"
#include "core/components/tab_bar/tab_bar_element.h"
#include "core/components/tab_bar/tab_content_element.h"

namespace OHOS::Ace {

RefPtr<TabController> TabController::GetController(int32_t id)
{
    return AceType::MakeRefPtr<TabController>(id);
}

TabController::TabController(int32_t id)
{
    id_ = id;
}

void TabController::ValidateIndex(int32_t maxIndex)
{
    if (pageReady_ && index_ > maxIndex) {
        index_ = 0;
        if (barElement_.Upgrade()) {
            auto tabBar = AceType::DynamicCast<TabBarElement>(barElement_.Upgrade());
            if (tabBar) {
                tabBar->UpdateIndex(0);
            }
        }
    }
}

void TabController::SetPageReady(bool ready)
{
    pageReady_ = ready;
}

void TabController::SetIndex(int32_t index)
{
    if (index_ == index || index < 0) {
        LOGI("SetIndex: Input index is not valid, %{public}d, %{public}d", index_, index);
        return;
    }
    indexDefined_ = true;

    index_ = index;
    if (contentElement_.Upgrade()) {
        auto tabContent = AceType::DynamicCast<TabContentElement>(contentElement_.Upgrade());
        if (tabContent) {
            tabContent->ChangeByBar(index);
        }
    }
}

void TabController::SetInitialIndex(int32_t index)
{
    if (initialIndex_ == index || index < 0) {
        LOGI("SetInitialIndex: Input index is not valid, %{public}d, %{public}d", initialIndex_, index);
        return;
    }
    initialIndex_ = index;
}

void TabController::SetIndexWithoutChangeContent(int32_t index)
{
    if (index_ == index || index < 0) {
        LOGI("SetIndexWithoutChangeContent: Input index is not valid, %{public}d, %{public}d", index_, index);
        return;
    }
    indexDefined_ = true;
    index_ = index;
}

void TabController::SetPendingIndex(int32_t index)
{
    if (pendingIndex_ == index || index < 0) {
        LOGI("SetPendingIndex: Input index is not valid, %{public}d, %{public}d", pendingIndex_, index);
        return;
    }
    pendingIndex_ = index;
    indexDefined_ = false;
}

void TabController::SetIndexByController(int32_t index, bool blockEvent)
{
    if (index_ == index || index < 0) {
        LOGI("SetIndexByController: Input index is not valid, %{public}d, %{public}d", index_, index);
        return;
    }
    if (index >= totalCount_) {
        LOGI("index is large than total, %{public}d, %{public}d", index, totalCount_);
        SetPendingIndex(index);
        return;
    }
    indexDefined_ = true;
    if (barElement_.Upgrade()) {
        auto tabBar = AceType::DynamicCast<TabBarElement>(barElement_.Upgrade());
        if (tabBar) {
            auto renderTabBar = AceType::DynamicCast<RenderTabBar>(tabBar->GetRenderNode());
            if (renderTabBar && renderTabBar->GetTabsSize() < index) {
                LOGW("Input index is not valid.");
                return;
            }
            tabBar->UpdateIndex(index);
        }
    }

    index_ = index;
    if (contentElement_.Upgrade()) {
        auto tabContent = AceType::DynamicCast<TabContentElement>(contentElement_.Upgrade());
        if (tabContent) {
            tabContent->ChangeByBar(index, blockEvent);
        }
    }
}

void TabController::ChangeDispatch(int32_t index)
{
    if (contentElement_.Upgrade()) {
        LOGD("tab controller dispatch domChange event");
        auto tabContent = AceType::DynamicCast<TabContentElement>(contentElement_.Upgrade());
        if (tabContent) {
            tabContent->ChangeDispatch(index);
        }
    }
}

void TabController::SetIndexByScrollContent(int32_t index)
{
    if (index_ == index || index < 0) {
        LOGI("SetIndexByScrollContent: Input index is not valid, %{public}d, %{public}d", index_, index);
        return;
    }
    indexDefined_ = true;
    index_ = index;
    if (barElement_.Upgrade()) {
        auto tabBar = AceType::DynamicCast<TabBarElement>(barElement_.Upgrade());
        if (tabBar) {
            tabBar->UpdateIndex(index);
        }
    }
}

void TabController::SetContentElement(const RefPtr<Element>& contentElement)
{
    contentElement_ = contentElement;
}

void TabController::SetBarElement(const RefPtr<Element>& barElement)
{
    barElement_ = barElement;
}

int32_t TabController::GetId() const
{
    return id_;
}

int32_t TabController::GetIndex() const
{
    return index_;
}

bool TabController::IsIndexDefined() const
{
    return indexDefined_;
}

} // namespace OHOS::Ace
