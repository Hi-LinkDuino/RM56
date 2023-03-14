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

#include "core/components/stack/render_stack.h"

#include "base/utils/utils.h"
#include "core/components/positioned/render_positioned.h"
#include "core/components/stack/stack_component.h"
#include "core/pipeline/base/position_layout_utils.h"

namespace OHOS::Ace {

void RenderStack::Update(const RefPtr<Component>& component)
{
    const auto stack = AceType::DynamicCast<StackComponent>(component);
    if (!stack) {
        return;
    }
    align_ = stack->GetAlignment();
    fit_ = stack->GetStackFit();
    overflow_ = stack->GetOverflow();
    mainStackSize_ = stack->GetMainStackSize();
    MarkNeedLayout();
}

void RenderStack::PerformLayout()
{
    Size maxSize = GetLayoutParam().GetMaxSize();
    bool hasNonPositionedItem = false;
    if (GetChildren().empty()) {
        LOGD("RenderStack: No child in Stack. Use max size of LayoutParam.");
        SetLayoutSize(maxSize);
        return;
    }
    LayoutParam innerLayout;
    // layout children
    RefPtr<RenderNode> firstChild;
    std::list<RefPtr<RenderNode>> percentChild;
    for (const auto& item : GetChildren()) {
        if (item->GetIsPercentSize()) {
            percentChild.emplace_back(item);
        }
        auto positionedItem = AceType::DynamicCast<RenderPositioned>(item);
        if (!positionedItem) {
            hasNonPositionedItem = true;
            innerLayout = MakeNonPositionedInnerLayoutParam(firstChild);
            item->Layout(innerLayout);
        } else {
            innerLayout = MakePositionedInnerLayoutParam(positionedItem, firstChild);
            positionedItem->Layout(innerLayout);
        }
        if (!firstChild) {
            firstChild = item;
        }
    }
    // determine the stack size
    DetermineStackSize(hasNonPositionedItem);

    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        auto layoutParam = GetLayoutParam();
        layoutParam.SetMaxSize(GetLayoutSize());
        SetLayoutParam(layoutParam);
    }

    // secondnary layout for percentchild
    for (const auto& item : percentChild) {
        innerLayout.SetMaxSize(GetLayoutSize());
        item->Layout(innerLayout);
    }

    SetChildrenStatus();
    // place children
    for (const auto& item : GetChildren()) {
        auto positionedItem = AceType::DynamicCast<RenderPositioned>(item);
        if (!positionedItem) {
            if (item->GetPositionType() == PositionType::ABSOLUTE) {
                auto itemOffset = PositionLayoutUtils::GetAbsoluteOffset(Claim(this), item);
                item->SetAbsolutePosition(itemOffset);
                continue;
            }
            item->SetPosition(GetNonPositionedChildOffset(item->GetLayoutSize()));
            continue;
        }
        Offset offset = GetPositionedChildOffset(positionedItem);
        if (offset.GetX() < 0.0 || offset.GetY() < 0.0 ||
            offset.GetX() + positionedItem->GetLayoutSize().Width() > GetLayoutSize().Width() ||
            offset.GetY() + positionedItem->GetLayoutSize().Height() > GetLayoutSize().Height()) {
            isChildOverflow_ = true;
        }
        positionedItem->SetPosition(offset);
    }
}

void RenderStack::DetermineStackSize(bool hasNonPositioned)
{
    Size maxSize = GetLayoutParam().GetMaxSize();
    if (maxSize.IsWidthInfinite()) {
        maxSize.SetWidth(viewPort_.Width());
    }
    if (maxSize.IsHeightInfinite()) {
        maxSize.SetHeight(viewPort_.Height());
    }

    if (mainStackSize_ == MainStackSize::MAX && !maxSize.IsInfinite()) {
        SetLayoutSize(maxSize);
        return;
    }
    double width = GetLayoutParam().GetMinSize().Width();
    double height = GetLayoutParam().GetMinSize().Height();
    double maxX = 0.0;
    double maxY = 0.0;
    double lastChildWidth = width;
    double lastChildHeight = height;
    for (const auto& item : GetChildren()) {
        if (item->GetIsPercentSize()) {
            continue;
        }
        double constrainedWidth = std::clamp(item->GetLayoutSize().Width(), GetLayoutParam().GetMinSize().Width(),
            GetLayoutParam().GetMaxSize().Width());
        double constrainedHeight = std::clamp(item->GetLayoutSize().Height(), GetLayoutParam().GetMinSize().Height(),
            GetLayoutParam().GetMaxSize().Height());
        width = std::max(width, constrainedWidth);
        height = std::max(height, constrainedHeight);
        lastChildWidth = constrainedWidth;
        lastChildHeight = constrainedHeight;
        maxX = std::max(maxX, item->GetLayoutSize().Width() + NormalizePercentToPx(item->GetLeft(), false));
        maxY = std::max(maxY, item->GetLayoutSize().Height() + NormalizePercentToPx(item->GetTop(), true));
    }
    for (const auto& item : GetChildren()) {
        if (item->GetIsPercentSize()) {
            if (maxX == 0 || maxY == 0) {
                double constrainedWidth = std::clamp(item->GetLayoutSize().Width(),
                    GetLayoutParam().GetMinSize().Width(), GetLayoutParam().GetMaxSize().Width());
                double constrainedHeight = std::clamp(item->GetLayoutSize().Height(),
                    GetLayoutParam().GetMinSize().Height(), GetLayoutParam().GetMaxSize().Height());
                width = std::max(width, constrainedWidth);
                height = std::max(height, constrainedHeight);
                lastChildWidth = constrainedWidth;
                lastChildHeight = constrainedHeight;
                maxX = std::max(maxX, item->GetLayoutSize().Width() + NormalizePercentToPx(item->GetLeft(), false));
                maxY = std::max(maxY, item->GetLayoutSize().Height() + NormalizePercentToPx(item->GetTop(), true));
            }
        }
    }
    if (mainStackSize_ == MainStackSize::NORMAL && !hasNonPositioned && !maxSize.IsInfinite()) {
        SetLayoutSize(maxSize);
        return;
    }
    // Usually used in SemiModal for determining current height.
    if (mainStackSize_ == MainStackSize::LAST_CHILD_HEIGHT) {
        SetLayoutSize(Size(maxSize.Width(), lastChildHeight));
        return;
    }
    if (mainStackSize_ == MainStackSize::MATCH_CHILDREN) {
        SetLayoutSize(GetLayoutParam().Constrain(Size(maxX, maxY)));
        return;
    }
    if (mainStackSize_ == MainStackSize::MAX_X) {
        auto maxSizeX = maxSize.Width();
        SetLayoutSize(Size(maxSizeX, maxY));
        return;
    }
    if (mainStackSize_ == MainStackSize::MAX_Y) {
        auto maxSizeY = maxSize.Height();
        SetLayoutSize(Size(maxX, maxSizeY));
        return;
    }
    SetLayoutSize(Size(width, height));
}

LayoutParam RenderStack::MakeNonPositionedInnerLayoutParam(const RefPtr<RenderNode>& firstChild) const
{
    LayoutParam innerLayout;
    switch (fit_) {
        case StackFit::STRETCH:
            innerLayout.SetFixedSize(GetLayoutParam().GetMaxSize());
            break;
        case StackFit::KEEP:
            innerLayout.SetMaxSize(GetLayoutParam().GetMaxSize());
            break;
        case StackFit::INHERIT:
            innerLayout = GetLayoutParam();
            break;
        case StackFit::FIRST_CHILD:
            innerLayout = GetLayoutParam();
            if (firstChild) {
                innerLayout.SetMaxSize(firstChild->GetLayoutSize());
            }
            break;
        default:
            LOGD("RenderStack: No such StackFit support. Use KEEP.");
            innerLayout.SetMaxSize(GetLayoutParam().GetMaxSize());
            break;
    }
    return innerLayout;
}

LayoutParam RenderStack::MakePositionedInnerLayoutParam(
    const RefPtr<RenderPositioned>& item, const RefPtr<RenderNode>& firstChild) const
{
    LayoutParam innerLayout;
    double width = std::clamp(item->GetWidth(), innerLayout.GetMinSize().Width(), innerLayout.GetMaxSize().Width());
    double height = std::clamp(item->GetHeight(), innerLayout.GetMinSize().Height(), innerLayout.GetMaxSize().Height());
    if (!NearZero(width) && !NearZero(height)) {
        innerLayout.SetFixedSize(Size(width, height));
    } else if (!NearZero(width)) {
        innerLayout.SetMinSize(Size(width, innerLayout.GetMinSize().Height()));
        innerLayout.SetMaxSize(Size(width, innerLayout.GetMaxSize().Height()));
    } else if (!NearZero(height)) {
        innerLayout.SetMinSize(Size(innerLayout.GetMinSize().Width(), height));
        innerLayout.SetMaxSize(Size(innerLayout.GetMaxSize().Width(), height));
    } else {
        LOGD("RenderStack: No width or height set in positioned component. Make NonpositionedInnerLayoutParam.");
        innerLayout = MakeNonPositionedInnerLayoutParam(firstChild);
    }
    return innerLayout;
}

Offset RenderStack::GetNonPositionedChildOffset(const Size& childSize)
{
    return Alignment::GetAlignPosition(GetLayoutSize(), childSize, align_);
}

Offset RenderStack::GetPositionedChildOffset(const RefPtr<RenderPositioned>& item)
{
    double deltaX = 0.0;
    if (item->HasLeft()) {
        deltaX = NormalizePercentToPx(item->GetLeft(), false);
    } else if (item->HasRight()) {
        deltaX =
            GetLayoutSize().Width() - NormalizePercentToPx(item->GetRight(), false) - item->GetLayoutSize().Width();
    } else {
        deltaX = GetNonPositionedChildOffset(item->GetLayoutSize()).GetX();
    }
    double deltaY = 0.0;
    if (item->HasTop()) {
        deltaY = NormalizePercentToPx(item->GetTop(), true);
    } else if (item->HasBottom()) {
        deltaY =
            GetLayoutSize().Height() - NormalizePercentToPx(item->GetBottom(), true) - item->GetLayoutSize().Height();
    } else {
        deltaY = GetNonPositionedChildOffset(item->GetLayoutSize()).GetY();
    }
    return Offset(deltaX, deltaY);
}

void RenderStack::OnAttachContext()
{
    RenderNode::OnAttachContext();
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        SetExclusiveEventForChild(true);
    }
}

} // namespace OHOS::Ace
