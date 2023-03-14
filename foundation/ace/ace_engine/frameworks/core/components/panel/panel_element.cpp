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

#include "core/components/panel/panel_element.h"

#include "core/components/panel/panel_component.h"
#include "core/components/panel/sliding_panel_component.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {

void PanelElement::PerformBuild()
{
    RefPtr<PanelComponent> panel = AceType::DynamicCast<PanelComponent>(component_);
    if (!panel) {
        return;
    }
    panel_ = panel;
    const auto& controller = panel->GetPanelController();
    if (!controller) {
        return;
    }
    controller->SetShowPanelImpl([weak = AceType::WeakClaim(this)]() {
        auto panel = weak.Upgrade();
        if (panel) {
            panel->ShowPanel();
        }
    });
    controller->SetClosePanelImpl([weak = AceType::WeakClaim(this)]() {
        auto panel = weak.Upgrade();
        if (panel) {
            panel->ClosePanel();
        }
    });
}

void PanelElement::ShowPanel()
{
    if (!panel_) {
        return;
    }
    const auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto stackElement = context->GetLastStack();
    if (!stackElement) {
        return;
    }
    auto slidingPanel = SlidingPanelComponent::Create(panel_);
    stackElement->PushPanel(slidingPanel, panel_->GetPanelType() == PanelType::TEMP_DISPLAY);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto manager = context->GetAccessibilityManager();
    if (manager) {
        auto node = manager->GetAccessibilityNodeById(StringUtils::StringToInt(panel_->GetId()));
        if (!node) {
            return;
        }
        node->SetZIndexToChild(stackElement->GetChildrenSize());
        manager->ClearNodeRectInfo(node, false);
    }
#endif
}

void PanelElement::ClosePanel()
{
    const auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    const auto& lastStack = context->GetLastStack();
    if (!lastStack) {
        return;
    }
    lastStack->PopPanel();
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto manager = context->GetAccessibilityManager();
    if (manager) {
        auto node = manager->GetAccessibilityNodeById(StringUtils::StringToInt(panel_->GetId()));
        if (!node) {
            return;
        }
        node->SetZIndexToChild(0);
        manager->ClearNodeRectInfo(node, true);
    }
#endif
}

} // namespace OHOS::Ace