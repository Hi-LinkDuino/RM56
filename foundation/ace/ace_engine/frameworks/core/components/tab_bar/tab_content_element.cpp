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

#include "core/components/tab_bar/tab_content_element.h"

#include <algorithm>

#include "core/components/tab_bar/render_tab_content.h"

namespace OHOS::Ace {

TabContentElement::TabContentElement(const std::list<RefPtr<Component>>& contents)
{
    contents_ = contents;
}

RefPtr<RenderNode> TabContentElement::CreateRenderNode()
{
    RefPtr<RenderNode> node = ComponentGroupElement::CreateRenderNode();
    RefPtr<RenderTabContent> tabContent = AceType::DynamicCast<RenderTabContent>(node);
    if (tabContent) {
        tabContent->RegisterCallback([weakTabContentElement = AceType::WeakClaim(this)](int32_t index) {
            auto tabContent = weakTabContentElement.Upgrade();
            if (tabContent) {
                tabContent->ChangeByContent(index);
            }
        });
        tabContent->RegisterRequireCallback([weakTabContentElement = AceType::WeakClaim(this)](int32_t index) {
            auto tabContent = weakTabContentElement.Upgrade();
            if (tabContent) {
                tabContent->PrepareContent(index);
            }
        });
    }
    return node;
}

void TabContentElement::UpdateLastFocusNode()
{
    if (IsCurrentFocus()) {
        auto focusNode = GetCurrentFocusNode();
        if (!focusNode || !focusNode->IsFocusable()) {
            LostFocus();
            itLastFocusNode_ = focusNodes_.end();
        } else {
            focusNode->RequestFocusImmediately();
        }
    } else {
        itLastFocusNode_ = focusNodes_.end();
    }
}

void TabContentElement::ChangeByContent(int32_t index)
{
    if (controller_) {
        controller_->SetIndexByScrollContent(index);
    }
    lastIndex_ = index;
    UpdateLastFocusNode();
}

void TabContentElement::ChangeByBar(int32_t index, bool isFromController)
{
    LOGD("change content by tab bar index:%{public}d", index);
    newBarIndex_ = index;
    fromController_ = isFromController;
    MarkDirty();
}

void TabContentElement::ChangeDispatch(int32_t index)
{
    LOGD("change content by tab bar index:%{public}d", index);
    auto content = AceType::DynamicCast<RenderTabContent>(GetRenderNode());
    if (content) {
        content->FireDomChangeEvent(index);
    }
}

void TabContentElement::PrepareContent(int32_t index)
{
    LOGD("request prepareContent new index:%{public}d", index);
    newIndex_ = index;
    MarkDirty();
}

void TabContentElement::Update()
{
    if (NeedUpdate()) {
        RefPtr<TabContentComponent> tabContent = AceType::DynamicCast<TabContentComponent>(component_);
        if (!tabContent) {
            LOGE("Get tabContent failed");
            return;
        }
        contents_ = tabContent->GetChildren();
        auto controller = tabContent->GetController();
        if (controller && (controller_ != controller)) {
            // Get index from old controller before replace.
            if (!controller->IsIndexDefined() && controller_) {
                controller->SetIndexWithoutChangeContent(controller_->GetIndex());
            }
            controller_ = controller;
        }
        if (!controller_) {
            LOGE("Get controller failed");
            return;
        }
        controller_->SetContentElement(AceType::Claim(this));
        if (controller_->GetIndex() >= static_cast<int32_t>(contents_.size())) {
            controller_->SetIndexWithoutChangeContent(static_cast<int32_t>(contents_.size()) - 1);
        }
        controller_->SetTotalCount(static_cast<int32_t>(contents_.size()));
        newComponentBuild_ = true;
    }
    ComponentGroupElement::Update();
}

void TabContentElement::PerformBuild()
{
    if (contents_.empty()) {
        LOGD("contents is empty");
        ComponentGroupElement::PerformBuild();
        return;
    }
    RefPtr<RenderTabContent> tabContent = AceType::DynamicCast<RenderTabContent>(renderNode_);
    if (!tabContent || !controller_ || !renderNode_) {
        LOGW("tabContent or controller is null.");
        return;
    }

    auto it = contents_.begin();
    // if have new content requested by drag, build the new child, else build current child
    int32_t target = newIndex_ >= 0 ? newIndex_ : controller_->GetIndex();
    LOGD("TabContentElement::PerformBuild: target: %{public}d", target);
    std::advance(it, target);
    if (it == contents_.end()) {
        LOGE("no content at index %{public}d.", target);
        return;
    }
    auto childIter = childMap_.find(target);
    if (childIter == childMap_.end()) {
        auto newChild = UpdateChild(nullptr, *it);
        focusIndexMap_.emplace(target);
        childMap_.emplace(target, newChild);
        auto renderChild = newChild->GetRenderNode();
        if (renderChild) {
            tabContent->AddChildContent(target, renderChild);
        }
        renderNode_->MarkNeedLayout();
    } else if (NeedUpdate()) {
        UpdateChild(childIter->second, *it);
    }

    // process for new content requested by drag
    if (target == newIndex_) {
        tabContent->UpdateDragPosition(newIndex_);
        newIndex_ = -1;
    }
    // process for new content requested by tab bar
    if (target == newBarIndex_) {
        tabContent->ChangeScroll(newBarIndex_, fromController_);
        UpdateLastFocusNode();
        newBarIndex_ = -1;
    }
    lastIndex_ = target;
    fromController_ = false;
}

void TabContentElement::OnFocus()
{
    // invoke callback
    FocusNode::OnFocus();

    auto focusNode = GetCurrentFocusNode();
    if (!focusNode) {
        LOGE("GetCurrentFocusNode failed");
        return;
    }

    if (focusNode->RequestFocusImmediately()) {
        itLastFocusNode_ = std::find(focusNodes_.begin(), focusNodes_.end(), focusNode);
    } else {
        LOGE("RequestFocusImmediately failed");
    }
}

bool TabContentElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    return false;
}

bool TabContentElement::IsFocusable() const
{
    auto focusNode = GetCurrentFocusNode();
    if (focusNode) {
        return focusNode->IsFocusable();
    }
    return false;
}

RefPtr<FocusNode> TabContentElement::GetCurrentFocusNode() const
{
    if (!controller_) {
        LOGE("controller is nullptr");
        return nullptr;
    }

    auto focusIndexIter = focusIndexMap_.find(controller_->GetIndex());
    if (focusIndexIter == focusIndexMap_.end()) {
        return nullptr;
    }
    auto pos = focusNodes_.begin();
    std::advance(pos, *focusIndexIter);
    if (pos == focusNodes_.end()) {
        return nullptr;
    }
    return AceType::DynamicCast<FocusNode>(*pos);
}

RefPtr<Element> TabContentElement::GetTabContentChild(int32_t index) const
{
    auto childIter = childMap_.find(index);
    if (childIter == childMap_.end()) {
        return nullptr;
    }
    return childIter->second;
}

} // namespace OHOS::Ace