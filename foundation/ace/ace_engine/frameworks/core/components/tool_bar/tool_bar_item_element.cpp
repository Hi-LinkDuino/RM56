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

#include "core/components/tool_bar/tool_bar_item_element.h"

#include "base/geometry/dimension.h"
#include "base/geometry/size.h"
#include "core/components/tool_bar/render_tool_bar_item.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

void ToolBarItemElement::Prepare(const WeakPtr<Element>& parent)
{
    RenderElement::Prepare(parent);
    if (renderNode_) {
        auto renderToolBarItem = AceType::DynamicCast<RenderToolBarItem>(renderNode_);
        if (renderToolBarItem) {
            renderToolBarItem->Initialize();
            renderToolBarItem->SetTargetMenuCallBack(
                [weak = WeakClaim(this)](const Offset& leftTop, const Offset& rightBottom) {
                    auto toolBarItemElement = weak.Upgrade();
                    if (toolBarItemElement) {
                        toolBarItemElement->OnTargetCallback(leftTop, rightBottom);
                    }
                });
        }
    }
}

void ToolBarItemElement::Update()
{
    if (popup_) {
        popup_->ClearAllOptions();
        popup_ = nullptr;
    }
    RefPtr<ToolBarItemComponent> toolBarItemComponent = AceType::DynamicCast<ToolBarItemComponent>(component_);
    if (!toolBarItemComponent) {
        LOGE("ToolBarItemElement::Update: get ToolBarItemComponent failed!");
        return;
    }
    isEndItem_ = toolBarItemComponent->GetIsEndItem();
    if (isEndItem_) {
        optionChildrenCallBack_ = toolBarItemComponent->GetOptionChildrenCallBack();
        if (optionChildrenCallBack_ && renderNode_) {
            auto renderToolBarItem = AceType::DynamicCast<RenderToolBarItem>(renderNode_);
            if (renderToolBarItem) {
                IniteSelectPopup();
                renderToolBarItem->Initialize();
            }
        }
    }
    SoleChildElement::Update();
}

void ToolBarItemElement::IniteSelectPopup()
{
    if (!popup_) {
        popup_ = AceType::MakeRefPtr<SelectPopupComponent>();
        popup_->InitTheme(GetThemeManager());
    }
    if (optionChildrenCallBack_) {
        std::list<RefPtr<Component>> children = optionChildrenCallBack_();
        if (children.empty()) {
            return;
        }
        for (const auto& pos : children) {
            RefPtr<OptionComponent> option = AceType::DynamicCast<OptionComponent>(pos);
            if (option) {
                option->InitTheme(GetThemeManager());
                popup_->AppendSelectOption(option);
            }
        }
        popup_->SetOptionClickedCallback([weak = WeakClaim(this)](std::size_t index) {
            auto refPtr = weak.Upgrade();
            if (!refPtr) {
                return;
            }
            refPtr->OnOptionCallback(index);
        });
    }
}

void ToolBarItemElement::OnOptionCallback(std::size_t index)
{
    if (popup_) {
        auto option = popup_->GetSelectOption(index);
        if (!option) {
            LOGE("option of the index is null.");
            return;
        }
        auto onClick = AceAsyncEvent<void()>::Create(option->GetClickEventForToolBarItem(), context_);
        if (onClick) {
            onClick();
        }
    }
}

void ToolBarItemElement::OnTargetCallback(const Offset& leftTop, const Offset& rightBottom)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null.");
        return;
    }

    auto stack = context->GetLastStack();
    if (!stack) {
        LOGE("can not get last stack.");
        return;
    }

    if (!popup_) {
        LOGE("can not get popup component.");
        return;
    }
    popup_->ShowDialog(stack, leftTop, rightBottom, false);
}

void ToolBarItemElement::OnFocus()
{
    const auto& renderToolBarItem = AceType::DynamicCast<RenderToolBarItem>(renderNode_);
    if (!renderToolBarItem) {
        return;
    }
    renderToolBarItem->ChangeStatus(RenderStatus::FOCUS);
    renderToolBarItem->SetFocusEventFlag(true);
    renderToolBarItem->MarkNeedLayout();
}

void ToolBarItemElement::OnBlur()
{
    const auto& renderToolBarItem = AceType::DynamicCast<RenderToolBarItem>(renderNode_);
    if (!renderToolBarItem) {
        return;
    }
    renderToolBarItem->SetFocusEventFlag(false);
    renderToolBarItem->MarkNeedLayout();
    auto focusAnimation = renderToolBarItem->GetFocusAnimation();
    if (!focusAnimation) {
        return;
    }
    focusAnimation->CancelFocusAnimation();
}

void ToolBarItemElement::OnClick()
{
    const auto& renderToolBarItem = AceType::DynamicCast<RenderToolBarItem>(renderNode_);
    if (!renderToolBarItem) {
        return;
    }
    renderToolBarItem->HandleClickEvent();
}

} // namespace OHOS::Ace