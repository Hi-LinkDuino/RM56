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

#include "core/components/root/root_element.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/container_modal/container_modal_element.h"
#include "core/components/dialog_modal/dialog_modal_element.h"
#include "core/components/root/render_root.h"
#include "core/components/root/root_component.h"
#include "core/components/semi_modal/semi_modal_element.h"

namespace OHOS::Ace {

void RootElement::PerformBuild()
{
    const RefPtr<RootComponent> root = AceType::DynamicCast<RootComponent>(component_);
    if (root) {
        active_ = true;
        auto first = children_.begin();
        const auto& stageElement = (first == children_.end()) ? nullptr : *first;
        UpdateChild(stageElement, root->GetChild());
    }
}

RefPtr<Element> RootElement::GetOverlayElement(WindowModal windowModal) const
{
    if (windowModal == WindowModal::SEMI_MODAL || windowModal == WindowModal::SEMI_MODAL_FULL_SCREEN) {
        auto semiModal = AceType::DynamicCast<SemiModalElement>(GetFirstChild());
        if (!semiModal) {
            LOGE("Get overlay element failed. SemiModal element is null!");
            return RefPtr<OverlayElement>();
        }
        return semiModal->GetOverlayElement();
    } else if (windowModal == WindowModal::DIALOG_MODAL) {
        auto dialogModal = AceType::DynamicCast<DialogModalElement>(GetFirstChild());
        if (!dialogModal) {
            LOGE("Get overlay element failed. DialogModal element is null!");
            return RefPtr<OverlayElement>();
        }
        return dialogModal->GetOverlayElement();
    } else if (windowModal == WindowModal::CONTAINER_MODAL) {
        auto containerModal = AceType::DynamicCast<ContainerModalElement>(GetFirstChild());
        if (!containerModal) {
            LOGE("Get overlay element failed. containerModal element is null!");
            return RefPtr<OverlayElement>();
        }
        return containerModal->GetOverlayElement();
    } else {
        auto stack = GetFirstChild();
        if (!stack) {
            return RefPtr<OverlayElement>();
        }
        auto child = stack->GetChildren();
        if (child.size() > 1) {
            auto it = child.begin();
            it++;
            return AceType::DynamicCast<OverlayElement>(*it);
        }
        return RefPtr<OverlayElement>();
    }
}

void RootElement::HandleSpecifiedKey(const KeyEvent& keyEvent)
{
    const auto& context = context_.Upgrade();
    if (!context) {
        LOGE("Handle specified key failed. context is null!");
        return;
    }
    WindowModal windowModal = context->GetWindowModal();
    if ((windowModal == WindowModal::DIALOG_MODAL) || (windowModal == WindowModal::SEMI_MODAL)) {
        bool isFullWindow = false;
        if (windowModal == WindowModal::SEMI_MODAL) {
            auto semiModal = AceType::DynamicCast<SemiModalElement>(GetFirstChild());
            if (!semiModal) {
                LOGE("Handle specified key failed. SemiModal element is null!");
                return;
            }
            isFullWindow = semiModal->IsFullWindow();
        }
        if (keyEvent.code == KeyCode::KEY_HOME && keyEvent.action == KeyAction::UP && !isFullWindow) {
            const auto& render = AceType::DynamicCast<RenderRoot>(renderNode_);
            if (render) {
                render->SetBgColor(Color::TRANSPARENT);
            }
        }
    }
}

} // namespace OHOS::Ace
