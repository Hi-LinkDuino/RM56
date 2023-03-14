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

#include "core/components/flex/render_flex.h"

#include <algorithm>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/flex/flex_component.h"
#include "core/components/scroll/render_single_child_scroll.h"
#include "core/pipeline/base/position_layout_utils.h"

namespace OHOS::Ace {
namespace {

const static int32_t PLATFORM_VERSION_FIVE = 5;

inline FlexDirection FlipAxis(FlexDirection direction)
{
    if (direction == FlexDirection::ROW || direction == FlexDirection::ROW_REVERSE) {
        return FlexDirection::COLUMN;
    } else {
        return FlexDirection::ROW;
    }
}

double GetMainAxisValue(const Size& size, FlexDirection direction)
{
    return direction == FlexDirection::ROW || direction == FlexDirection::ROW_REVERSE ? size.Width() : size.Height();
}

inline bool IsNonRelativePosition(PositionType pos)
{
    return ((pos != PositionType::RELATIVE) && (pos != PositionType::SEMI_RELATIVE) && (pos != PositionType::OFFSET));
}

} // namespace

RefPtr<RenderNode> RenderFlex::Create()
{
    return AceType::MakeRefPtr<RenderFlex>();
}

void RenderFlex::Update(const RefPtr<Component>& component)
{
    const RefPtr<FlexComponent> flex = AceType::DynamicCast<FlexComponent>(component);
    if (!flex) {
        return;
    }
    direction_ = flex->GetDirection();
    mainAxisAlign_ = flex->GetMainAxisAlign();
    crossAxisAlign_ = flex->GetCrossAxisAlign();
    mainAxisSize_ = flex->GetMainAxisSize();
    crossAxisSize_ = flex->GetCrossAxisSize();
    stretchToParent_ = flex->IsStretchToParent();
    useViewPort_ = flex->GetUseViewPortFlag();
    containsNavigation_ = flex->ContainsNavigation();
    overflow_ = flex->GetOverflow();
    SetTextDirection(flex->GetTextDirection());
    alignPtr_ = flex->GetAlignDeclarationPtr();

    auto context = GetContext().Upgrade();
    if (context) {
        space_ = context->NormalizeToPx(flex->GetSpace());
        inspectorSpace_ = flex->GetSpace();
        useOldLayoutVersion_ = context->GetMinPlatformVersion() <= PLATFORM_VERSION_FIVE;
        isDeclarative_ = context->GetIsDeclarative();
        if (GreatNotEqual(space_, 0.0)) {
            mainAxisAlign_ = FlexAlign::SPACE_CUSOMIZATION;
        }
    }
    UpdateAccessibilityAttr();
    MarkNeedLayout();
}

void RenderFlex::UpdateAccessibilityAttr()
{
    auto refPtr = accessibilityNode_.Upgrade();
    if (!refPtr) {
        return;
    }
    RefPtr<RenderSingleChildScroll> scroll;
    auto parent = GetParent().Upgrade();
    while (parent) {
        scroll = AceType::DynamicCast<RenderSingleChildScroll>(parent);
        if (scroll) {
            break;
        }
        parent = parent->GetParent().Upgrade();
    }
    if (!scroll || !scroll->GetAccessibilityNode().Upgrade()) {
        return;
    }
    if (scroll->GetAccessibilityNode().Upgrade()->GetNodeId() != refPtr->GetNodeId()) {
        return;
    }

    refPtr->SetScrollableState(true);
    refPtr->SetActionScrollForward([weakScroll = AceType::WeakClaim(RawPtr(scroll))]() {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            LOGD("Trigger ScrollForward by Accessibility.");
            scroll->ScrollPage(false, true);
            return true;
        }
        return false;
    });
    refPtr->SetActionScrollBackward([weakScroll = AceType::WeakClaim(RawPtr(scroll))]() {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            LOGD("Trigger ScrollBackward by Accessibility.");
            scroll->ScrollPage(true, true);
            return true;
        }
        return false;
    });
    refPtr->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
    refPtr->AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
    scrollNode = scroll;
}

void RenderFlex::OnPaintFinish()
{
    auto refPtr = accessibilityNode_.Upgrade();
    if (!refPtr || !scrollNode) {
        return;
    }
    auto collectionInfo = refPtr->GetCollectionInfo();
    collectionInfo.rows = static_cast<int32_t>(GetChildren().size());
    collectionInfo.columns = 1;
    refPtr->SetCollectionInfo(collectionInfo);
    Rect itemRect;
    Rect viewPortRect(scrollNode->GetGlobalOffset(), scrollNode->GetChildViewPort());
    for (const auto& item : GetChildren()) {
        auto node = item->GetAccessibilityNode().Upgrade();
        if (!node) {
            continue;
        }
        bool visible = GetVisible();
        if (visible) {
            itemRect.SetSize(item->GetLayoutSize());
            itemRect.SetOffset(item->GetGlobalOffset());
            visible = itemRect.IsIntersectWith(viewPortRect);
        }
        item->SetAccessibilityVisible(visible);
        if (visible) {
            Rect clampRect = itemRect.Constrain(viewPortRect);
            if (clampRect != itemRect) {
                item->SetAccessibilityRect(clampRect);
            }
        } else {
            item->NotifyPaintFinish();
        }
    }
}

LayoutParam RenderFlex::MakeStretchInnerLayoutParam(const RefPtr<RenderNode>& item) const
{
    // must be called in the second time layout, so that crossSize_ is determined.
    LayoutParam innerLayout;
    double crossAxisLimit = GetStretchCrossLimit();
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        innerLayout.SetFixedSize(Size(GetMainSize(item), crossAxisLimit));
    } else {
        innerLayout.SetFixedSize(Size(crossAxisLimit, GetMainSize(item)));
    }
    return innerLayout;
}

LayoutParam RenderFlex::MakeLayoutParamWithLimit(double minMainLimit, double maxMainLimit, bool isStretch) const
{
    LayoutParam innerLayout;
    double minCrossLimit = 0.0;
    double maxCrossLimit = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE)
                               ? GetLayoutParam().GetMaxSize().Height()
                               : GetLayoutParam().GetMaxSize().Width();
    if (isStretch) {
        minCrossLimit = GetStretchCrossLimit();
        maxCrossLimit = minCrossLimit;
    }
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        innerLayout.SetMinSize(Size(minMainLimit, minCrossLimit));
        innerLayout.SetMaxSize(Size(maxMainLimit, maxCrossLimit));
    } else {
        innerLayout.SetMinSize(Size(minCrossLimit, minMainLimit));
        innerLayout.SetMaxSize(Size(maxCrossLimit, maxMainLimit));
    }
    return innerLayout;
}

LayoutParam RenderFlex::MakeConstrainedLayoutParam(
    double mainFlexExtent, const LayoutParam& constraints, bool isStretch, bool supportZero) const
{
    LayoutParam innerLayout;
    if (LessNotEqual(mainFlexExtent, 0.0)) {
        innerLayout.SetMaxSize(GetLayoutParam().GetMaxSize());
    } else if (GreatNotEqual(mainFlexExtent, 0.0)) {
        innerLayout = MakeLayoutParamWithLimit(mainFlexExtent, mainFlexExtent, isStretch);
    } else {
        if (supportZero) {
            innerLayout = MakeLayoutParamWithLimit(mainFlexExtent, mainFlexExtent, isStretch);
        } else {
            innerLayout.SetMaxSize(GetLayoutParam().GetMaxSize());
        }
    }
    innerLayout.SetMaxSize(constraints.Constrain(innerLayout.GetMaxSize()));
    innerLayout.SetMinSize(constraints.Constrain(innerLayout.GetMinSize()));
    // SetHasUsedPercentFlag is to tell box not to use constraints
    innerLayout.SetHasUsedConstraints(true);
    return innerLayout;
}

double RenderFlex::GetStretchCrossLimit() const
{
    Size maxLayoutParam = GetLayoutParam().GetMaxSize();
    double crossAxisLimit = 0.0;
    if (!stretchToParent_) {
        crossAxisLimit = crossSize_;
    } else if (!isCrossInfinite_ || !useViewPort_) {
        crossAxisLimit = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE)
                             ? maxLayoutParam.Height()
                             : maxLayoutParam.Width();
    } else {
        crossAxisLimit = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE)
                             ? viewPort_.Height()
                             : viewPort_.Width();
    }
    return crossAxisLimit;
}

void RenderFlex::PerformLayout()
{
    if (GetChildren().empty()) {
        SetLayoutSize(Size());
        return;
    }

    auto context = GetContext().Upgrade();
    if (!context) {
        return;
    }
    if (alignPtr_ != nullptr) {
        context->AddAlignDeclarationNode(AceType::Claim(this));
    }

    // init properties.
    InitFlexProperties();
    if (layoutMode_ == FlexLayoutMode::FLEX_WEIGHT_MODE) {
        PerformLayoutInWeightMode();
    } else if (maxDisplayIndex_ > 1) {
        PerformLayoutInIndexMode();
    } else {
        PerformLayoutInItemMode();
    }
    ClearChildrenLists();

    if (alignPtr_ != nullptr) {
        auto& nodeList = context->GetAlignDeclarationNodeList();
        PerformItemAlign(nodeList);
    }
}

void RenderFlex::PerformLayoutInWeightMode()
{
    double maxMainSize = GetMainAxisValue(GetLayoutParam().GetMaxSize(), direction_);
    if (NearEqual(maxMainSize, Size::INFINITE_SIZE)) {
        LOGW("not supported infinite size");
        return;
    }
    BaselineProperties baselineProperties;
    LayoutParam innerLayout;
    double allocatedSize = allocatedSize_;
    for (const auto& child : relativeNodes_) {
        if (LessOrEqual(child->GetFlexWeight(), 0.0)) {
            child->Layout(GetLayoutParam());
            ResizeByItem(child, allocatedSize);
            CheckSizeValidity(child);
            CheckBaselineProperties(child, baselineProperties);
        }
    }
    maxMainSize -= allocatedSize_;
    // if remain size less than zero, adjust it to zero
    if (!useOldLayoutVersion_ && LessNotEqual(maxMainSize, 0.0)) {
        maxMainSize = 0.0;
    }
    // totalFlexWeight_ is guard in InitFlexProperties() so it won't be zero
    auto spacePerWeight = maxMainSize / totalFlexWeight_;
    bool isExceed = false;
    auto iter = magicNodes_.rbegin();
    // Calculate innerLayoutParam for each magic node
    while (iter != magicNodes_.rend()) {
        auto& layoutList = (*iter).second;
        for (auto& node : layoutList) {
            auto child = node.node;
            if (LessOrEqual(child->GetFlexWeight(), 0.0)) {
                continue;
            }
            auto childFlexSize = spacePerWeight * child->GetFlexWeight();
            auto flexItem = AceType::DynamicCast<RenderFlexItem>(child);
            if (flexItem) {
                innerLayout = MakeConstrainedLayoutParam(
                    childFlexSize, flexItem->GetNormalizedConstraints(), false, !useOldLayoutVersion_);
            } else {
                innerLayout = MakeLayoutParamWithLimit(childFlexSize, childFlexSize, false);
            }
            // If min constraint is larger than flexSize, total size must exceed.
            isExceed = isExceed || GetMainAxisValue(innerLayout.GetMaxSize(), direction_) > childFlexSize;
            node.innerLayout = innerLayout;
        }
        if (!isExceed) {
            iter++;
        } else if (magicNodes_.size() <= 1) {
            break;
        } else {
            // Hide nodes, reset properties and start next loop
            totalFlexWeight_ -= magicWeightMaps_[(*magicNodes_.begin()).first];
            spacePerWeight = maxMainSize / totalFlexWeight_;
            isExceed = false;
            magicNodes_.erase(magicNodes_.begin());
            iter = magicNodes_.rbegin();
        }
    }
    // Layout magic nodes
    LayoutMagicNodes(baselineProperties);
    if (crossAxisAlign_ == FlexAlign::STRETCH) {
        RelayoutForStretchMagicNode();
    }
    LayoutAbsoluteChildren();
    LayoutHiddenNodes();
    Size layoutSize = GetConstrainedSize(GetMainAxisValue(GetLayoutParam().GetMaxSize(), direction_));
    SetLayoutSize(layoutSize);
    mainSize_ = GetMainAxisValue(layoutSize, direction_);
    crossSize_ = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) ? layoutSize.Height()
                                                                                                : layoutSize.Width();
    DetermineItemsPosition(baselineProperties);
}

void RenderFlex::LayoutMagicNodes(BaselineProperties& baselineProperties)
{
    double allocatedSize = allocatedSize_;
    for (const auto& magicNode : magicNodes_) {
        auto nodeList = magicNode.second;
        for (const auto& child : nodeList) {
            if (LessOrEqual(child.node->GetFlexWeight(), 0.0)) {
                continue;
            }
            child.node->Layout(child.innerLayout);
            child.node->SetVisible(true);
            ResizeByItem(child.node, allocatedSize);
            CheckSizeValidity(child.node);
            CheckBaselineProperties(child.node, baselineProperties);
        }
    }
}

void RenderFlex::RelayoutForStretchMagicNode()
{
    LayoutParam innerLayout;
    for (const auto& magicNodeMap : magicNodes_) {
        auto nodeList = magicNodeMap.second;
        for (const auto& node : nodeList) {
            auto child = node.node;
            auto flexItem = AceType::DynamicCast<RenderFlexItem>(child);
            if (!flexItem) {
                innerLayout = MakeLayoutParamWithLimit(GetMainSize(child), GetMainSize(child), true);
            } else if (flexItem->GetStretchFlag()) {
                innerLayout =
                    MakeConstrainedLayoutParam(GetMainSize(flexItem), flexItem->GetNormalizedConstraints(), true);
            } else {
                // FlexItem cannot be stretched, skip it.
                continue;
            }
            child->Layout(innerLayout);
            // stretch only need to adjust crossSize
            crossSize_ = std::max(crossSize_, GetCrossSize(child));
        }
    }
}

void RenderFlex::PerformLayoutInIndexMode()
{
    auto maxMainSize = GetMainAxisValue(GetLayoutParam().GetMaxSize(), direction_);
    LayoutParam innerLayout;
    innerLayout.SetMaxSize(GetLayoutParam().GetMaxSize());
    FlexItemProperties flexItemProperties;
    for (auto iter = magicNodes_.rbegin(); iter != magicNodes_.rend();) {
        auto nodeList = (*iter).second;
        for (const auto& node : nodeList) {
            auto child = node.node;
            child->Layout(innerLayout);
            allocatedSize_ += GetMainSize(child);
            allocatedSize_ += space_;
        }
        if ((allocatedSize_ - space_) > maxMainSize) {
            for (const auto& node : nodeList) {
                auto child = node.node;
                allocatedSize_ -= GetMainSize(child);
                allocatedSize_ -= space_;
            }
            break;
        }
        // If not fill all the main size, record the node and continue the loop.
        for (const auto& node : nodeList) {
            auto child = node.node;
            CheckSizeValidity(child);
            child->SetVisible(true);
            auto flexItem = AceType::DynamicCast<RenderFlexItem>(child);
            if (flexItem && GreatNotEqual(flexItem->GetFlexGrow(), 0.0)) {
                flexItemProperties.totalGrow += flexItem->GetFlexGrow();
                flexItemProperties.lastGrowChild = flexItem;
            }
            crossSize_ = std::max(crossSize_, GetCrossSize(child));
        }
        if (NearEqual(allocatedSize_, maxMainSize)) {
            break;
        }
        iter++;
    }
    // Second Layout for grow/stretch
    if (crossAxisAlign_ == FlexAlign::STRETCH || flexItemProperties.totalGrow > 0) {
        RelayoutForStretchFlexNode(flexItemProperties);
    }
    LayoutHiddenNodes();
    LayoutAbsoluteChildren();
    allocatedSize_ -= space_;
    Size layoutSize;
    if (!isDeclarative_ || mainAxisSize_ == MainAxisSize::MAX) {
        layoutSize = GetConstrainedSize(maxMainSize);
    } else {
        layoutSize = GetConstrainedSize(allocatedSize_);
    }
    SetLayoutSize(layoutSize);
    mainSize_ = GetMainAxisValue(layoutSize, direction_);
    crossSize_ = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) ? layoutSize.Height()
                                                                                                : layoutSize.Width();
    BaselineProperties baselineProperties;
    DetermineItemsPosition(baselineProperties);
}

void RenderFlex::RelayoutForStretchFlexNode(const FlexItemProperties& flexItemProperties)
{
    auto remainSpace = GetMainAxisValue(GetLayoutParam().GetMaxSize(), direction_) - allocatedSize_;
    // only grow applied in display index mode.
    double spacePerFlex = 0.0;
    if (GreatNotEqual(flexItemProperties.totalGrow, 0.0)) {
        spacePerFlex = remainSpace / flexItemProperties.totalGrow;
    }
    double allocatedFlexSpace = 0.0;
    BaselineProperties baselineProperties;
    for (const auto& item : displayNodes_) {
        auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
        if (!flexItem) {
            if (crossAxisAlign_ == FlexAlign::STRETCH) {
                item->Layout(MakeStretchInnerLayoutParam(item));
                // stretch only need to adjust cross size
                crossSize_ = std::max(crossSize_, GetCrossSize(item));
            }
            continue;
        }
        if (GreatNotEqual(flexItem->GetFlexGrow(), 0.0)) {
            double flexSize = 0.0;
            flexSize = flexItem == flexItemProperties.lastGrowChild ? remainSpace - allocatedFlexSpace
                                                                    : spacePerFlex * flexItem->GetFlexGrow();
            RelayoutFlexItem(flexItem, flexSize, baselineProperties, allocatedFlexSpace);
        } else if (crossAxisAlign_ == FlexAlign::STRETCH && flexItem->GetStretchFlag()) {
            flexItem->Layout(
                MakeConstrainedLayoutParam(GetMainSize(flexItem), flexItem->GetNormalizedConstraints(), true));
            crossSize_ = std::max(crossSize_, GetCrossSize(flexItem));
        } else {
            // not stretch or grow, continue.
            continue;
        }
    }
}

void RenderFlex::LayoutHiddenNodes()
{
    LayoutParam innerLayout = LayoutParam(Size(), Size());
    for (const auto& child : relativeNodes_) {
        if (displayNodes_.find(child) != displayNodes_.end()) {
            continue;
        }
        child->SetVisible(false);
        child->Layout(innerLayout);
    }
}

void RenderFlex::PerformLayoutInItemMode()
{
    LayoutParam innerLayout;
    innerLayout.SetMaxSize(GetLayoutParam().GetMaxSize());
    FlexItemProperties flexItemProperties;
    BaselineProperties baselineProperties;
    double allocatedSize = allocatedSize_;
    // first time layout
    for (const auto& item : relativeNodes_) {
        auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
        if (!flexItem || flexItem->IsHidden()) {
            item->Layout(innerLayout);
        } else {
            LayoutFlexItem(flexItem, flexItemProperties);
        }
        if (item == GetChildren().front() && containsNavigation_) {
            navigationMainSize_ = GetMainAxisValue(item->GetLayoutSize(), direction_);
        }
        ResizeByItem(item, allocatedSize);
        CheckSizeValidity(item);
        CheckBaselineProperties(item, baselineProperties);
    }
    // second time layout
    double mainViewPort = GetMainAxisValue(viewPort_, direction_);
    bool useViewPort = useViewPort_ && !viewPort_.IsInfinite() && (allocatedSize_ < mainViewPort);
    auto mainAxisSize = mainAxisSize_;
    if (!isMainInfinite_ || useViewPort) {
        ResizeItems(flexItemProperties, baselineProperties);
    } else if (!infinityLayoutNodes_.empty()) {
        if (allocatedSize_ < mainViewPort) {
            allocatedSize_ = Size::INFINITE_SIZE;
        } else {
            double availableMainSize = GetAvailableMainSize();
            for (auto& infinityItem : infinityLayoutNodes_) {
                LayoutInfinityChild(infinityItem, availableMainSize, baselineProperties);
            }
            mainAxisSize = MainAxisSize::MIN;
        }
    } else if (allocatedSize_ >= mainViewPort && crossAxisAlign_ == FlexAlign::STRETCH) {
        // Children size larger than viewPort, second layout only do stretch.
        for (const auto& item : relativeNodes_) {
            // If Item has set width/height in main axis, not need to stretch.
            auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
            if (flexItem && !flexItem->GetStretchFlag()) {
                continue;
            }
            item->Layout(MakeStretchInnerLayoutParam(item));
        }
    } else {
        for (const auto& item : stretchNodes_) {
            item->Layout(MakeStretchInnerLayoutParam(item));
        }
        mainAxisSize = MainAxisSize::MIN;
    }
    LayoutAbsoluteChildren();
    // get layout size and set positions
    DetermineSelfSize(mainAxisSize, useViewPort);
    DetermineItemsPosition(baselineProperties);
}

void RenderFlex::ResizeItems(const FlexItemProperties& flexItemProps, BaselineProperties& baselineProps)
{
    double availableMainSize = GetAvailableMainSize();
    if (flexItemProps.totalGrow > 0 && availableMainSize > allocatedSize_ && !isDeclarative_) {
        mainAxisSize_ = MainAxisSize::MAX;
    }
    // remainSpace should be (availableMainSize - allocatedSize_), and do not remain space when MainAxisSize::MIN.
    // Handle infinity children specially, because allocatedSize_ not include infinity child.
    double remainSpace =
        (mainAxisSize_ == MainAxisSize::MIN && availableMainSize >= allocatedSize_ && infinityLayoutNodes_.empty())
            ? 0.0
            : availableMainSize - allocatedSize_;
    double infiniteLayoutSize = availableMainSize;
    if (!infinityLayoutNodes_.empty()) {
        if (remainSpace > 0.0) {
            infiniteLayoutSize = remainSpace / infinityLayoutNodes_.size();
            remainSpace = 0.0;
        } else {
            remainSpace -= infiniteLayoutSize;
        }
    }
    double spacePerFlex = 0.0;
    double allocatedFlexSpace = 0.0;
    double (*getFlex)(const RefPtr<RenderFlexItem>&) = nullptr;
    RefPtr<RenderFlexItem> lastChild;
    if (remainSpace > 0) {
        getFlex = [](const RefPtr<RenderFlexItem>& item) { return item->GetFlexGrow(); };
        spacePerFlex = NearZero(flexItemProps.totalGrow) ? 0.0 : remainSpace / flexItemProps.totalGrow;
        lastChild = flexItemProps.lastGrowChild;
    } else {
        getFlex = [](const RefPtr<RenderFlexItem>& item) { return item->GetFlexShrink(); };
        spacePerFlex = NearZero(flexItemProps.totalShrink) ? 0.0 : remainSpace / flexItemProps.totalShrink;
        lastChild = flexItemProps.lastShrinkChild;
    }
    // In second layout, do not need to check the size validity, they are all checked.
    for (const auto& item : relativeNodes_) {
        if (infinityLayoutNodes_.find(item) != infinityLayoutNodes_.end()) {
            LayoutInfinityChild(item, infiniteLayoutSize, baselineProps);
            continue;
        }
        auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
        if (!flexItem || flexItem->IsHidden()) {
            if (crossAxisAlign_ == FlexAlign::STRETCH) {
                item->Layout(MakeStretchInnerLayoutParam(item));
            }
            continue;
        }
        double itemFlex = getFlex(flexItem);
        double flexSize = (flexItem == lastChild) ? (remainSpace - allocatedFlexSpace)
                                                  : ((remainSpace > 0) ? spacePerFlex * itemFlex
                                                                       : spacePerFlex * itemFlex * GetMainSize(item));
        RelayoutFlexItem(flexItem, flexSize, baselineProps, allocatedFlexSpace);
    }
}

void RenderFlex::DetermineSelfSize(MainAxisSize mainAxisSize, bool useViewPort)
{
    double maxMainSize = GetMainAxisValue(GetLayoutParam().GetMaxSize(), direction_);
    double mainViewPort = GetMainAxisValue(viewPort_, direction_);
    if (NearEqual(maxMainSize, Size::INFINITE_SIZE)) {
        // If max size of layoutParam is infinity, use children's allocated size as max size.
        maxMainSize = allocatedSize_;
    }
    allocatedSize_ -= space_;
    // useViewPort means that it is the root flex, should be as large as viewPort.
    Size layoutSize = (mainAxisSize == MainAxisSize::MIN) ? GetConstrainedSize(allocatedSize_)
                      : useViewPort                       ? GetConstrainedSize(mainViewPort)
                                                          : GetConstrainedSize(maxMainSize);
    if (useViewPort && !absoluteNodes_.empty()) {
        layoutSize = GetConstrainedSize(mainViewPort);
    }
    isChildOverflow_ = allocatedSize_ > GetMainAxisValue(layoutSize, direction_);
    SetLayoutSize(layoutSize);
    mainSize_ = GetMainAxisValue(layoutSize, direction_);
    crossSize_ = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) ? layoutSize.Height()
                                                                                                : layoutSize.Width();
}

void RenderFlex::DetermineItemsPosition(const BaselineProperties& baselineProperties)
{
    double remainSpace = (mainSize_ - allocatedSize_) > 0.0 ? (mainSize_ - allocatedSize_) : 0.0;
    double frontSpace = 0.0;
    double betweenSpace = 0.0;
    // make use of remain space
    CalculateSpace(remainSpace, frontSpace, betweenSpace);
    // position elements
    PlaceChildren(frontSpace, betweenSpace, baselineProperties);
}

void RenderFlex::CalculateSpace(double remainSpace, double& frontSpace, double& betweenSpace) const
{
    if (NearZero(remainSpace) && mainAxisAlign_ != FlexAlign::SPACE_CUSOMIZATION) {
        return;
    }
    switch (mainAxisAlign_) {
        case FlexAlign::FLEX_START:
            frontSpace = 0.0;
            betweenSpace = 0.0;
            break;
        case FlexAlign::FLEX_END:
            frontSpace = remainSpace;
            betweenSpace = 0.0;
            break;
        case FlexAlign::CENTER:
            frontSpace = remainSpace / 2.0;
            betweenSpace = 0.0;
            break;
        case FlexAlign::SPACE_BETWEEN:
            frontSpace = 0.0;
            betweenSpace = validSizeCount_ > 1 ? remainSpace / (validSizeCount_ - 1) : 0.0;
            break;
        case FlexAlign::SPACE_AROUND:
            betweenSpace = validSizeCount_ > 0 ? remainSpace / validSizeCount_ : 0.0;
            frontSpace = betweenSpace / 2.0;
            break;
        case FlexAlign::SPACE_EVENLY:
            betweenSpace = validSizeCount_ > 0 ? remainSpace / (validSizeCount_ + 1) : 0.0;
            frontSpace = betweenSpace;
            break;
        case FlexAlign::SPACE_CUSOMIZATION:
            betweenSpace = space_;
            frontSpace = 0.0;
            break;
        default:
            break;
    }
}

void RenderFlex::PlaceChildren(double frontSpace, double betweenSpace, const BaselineProperties& baselineProperties)
{
    double childMainPos = IsStartTopLeft(direction_, GetTextDirection()) ? frontSpace : mainSize_ - frontSpace;
    double childCrossPos = 0.0;
    for (const auto& item : GetChildren()) {
        if (item->IsIgnored()) {
            continue;
        }
        auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
        if (flexItem && flexItem->IsHidden() && !flexItem->GetLayoutSize().IsValid()) {
            continue;
        }
        if (IsNonRelativePosition(item->GetPositionType())) {
            Offset absoluteOffset = PositionLayoutUtils::GetAbsoluteOffset(Claim(this), item);
            item->SetAbsolutePosition(absoluteOffset);
            continue;
        }
        auto alignItem = GetSelfAlign(item);
        auto textDirection = AdjustTextDirectionByDir();
        switch (alignItem) {
            case FlexAlign::FLEX_START:
            case FlexAlign::FLEX_END:
                childCrossPos =
                    (IsStartTopLeft(FlipAxis(direction_), textDirection) == (alignItem == FlexAlign::FLEX_START))
                        ? 0.0
                        : (crossSize_ - GetCrossSize(item));
                break;
            case FlexAlign::CENTER:
                childCrossPos = (crossSize_ / 2.0) - (GetCrossSize(item) / 2.0);
                break;
            case FlexAlign::STRETCH:
                childCrossPos =
                    IsStartTopLeft(FlipAxis(direction_), textDirection) ? 0.0 : (crossSize_ - GetCrossSize(item));
                break;
            case FlexAlign::BASELINE:
                childCrossPos = 0.0;
                if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
                    double distance = item->GetBaselineDistance(textBaseline_);
                    childCrossPos = baselineProperties.maxBaselineDistance - distance;
                }
                break;
            default:
                break;
        }
        Offset offset;
        if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
            if (item->GetPositionType() == PositionType::SEMI_RELATIVE) {
                childMainPos = 0.0;
            }
            offset = Offset(childMainPos, childCrossPos);
        } else {
            offset =
                Offset((item->GetPositionType() == PositionType::SEMI_RELATIVE) ? 0.0 : childCrossPos, childMainPos);
        }

        if (!IsStartTopLeft(direction_, GetTextDirection())) {
            if (direction_ != FlexDirection::COLUMN_REVERSE) {
                offset.SetX(offset.GetX() - GetMainSize(item));
            } else {
                offset.SetY(offset.GetY() - GetMainSize(item));
            }
            item->SetPosition(offset);
            childMainPos -= GetMainSize(item) + betweenSpace;
        } else {
            item->SetPosition(offset);
            childMainPos += GetMainSize(item) + betweenSpace;
        }
    }
}

void RenderFlex::LayoutFlexItem(RefPtr<RenderFlexItem>& flexItem, FlexItemProperties& flexItemProperties)
{
    double itemShrink = flexItem->GetFlexShrink();
    double itemGrow = flexItem->GetFlexGrow();
    double itemBasis = flexItem->GetFlexBasisToPx();
    if (flexItem->MustStretch()) {
        stretchNodes_.emplace_back(flexItem);
    }
    if (itemGrow > 0) {
        flexItemProperties.lastGrowChild = flexItem;
    }
    if (itemShrink > 0) {
        flexItemProperties.lastShrinkChild = flexItem;
    }
    LayoutParam innerLayout;
    if (GreatNotEqual(itemBasis, 0.0)) {
        innerLayout = MakeLayoutParamWithLimit(itemBasis, itemBasis, false);
    } else {
        innerLayout.SetMaxSize(GetLayoutParam().GetMaxSize());
    }
    // first time layout flex item
    flexItem->Layout(innerLayout);
    flexItemProperties.totalShrink += itemShrink * GetMainSize(flexItem);
    flexItemProperties.totalGrow += itemGrow;
}

void RenderFlex::RelayoutFlexItem(const RefPtr<RenderFlexItem>& flexItem, double flexSize,
    BaselineProperties& baselineProps, double& allocatedFlexSpace)
{
    bool canItemStretch = flexItem->MustStretch() || ((GetSelfAlign(flexItem) == FlexAlign::STRETCH) &&
                                                         (flexItem->GetStretchFlag()) && (relativeNodes_.size() > 1));
    // less or equal than api 5
    if (useOldLayoutVersion_) {
        canItemStretch =
            flexItem->MustStretch() || ((GetSelfAlign(flexItem) == FlexAlign::STRETCH) && (flexItem->GetStretchFlag()));
    }

    if (NearZero(flexSize) && !canItemStretch) {
        return;
    }
    auto mainFlexExtent = flexSize + GetMainSize(flexItem);
    auto childMainContent = GetMainAxisValue(flexItem->GetContentSize(), direction_);
    if (childMainContent > mainFlexExtent) {
        mainFlexExtent = childMainContent;
    }
    allocatedSize_ -= GetMainSize(flexItem);
    auto innerLayout = MakeConstrainedLayoutParam(mainFlexExtent, flexItem->GetNormalizedConstraints(), canItemStretch,
        flexItem->MustStretch() || !useOldLayoutVersion_);
    if (flexItem->MustStretch()) {
        auto crossStretch = crossAxisSize_ == CrossAxisSize::MAX
                                ? GetMainAxisValue(GetLayoutParam().GetMaxSize(), FlipAxis(direction_))
                                : crossSize_;
        auto innerMax = innerLayout.GetMaxSize();
        if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
            innerMax.SetHeight(crossStretch);
        } else {
            innerMax.SetWidth(crossStretch);
        }
        innerLayout.SetMaxSize(innerMax);
        innerLayout.SetMinSize(innerMax);
    }
    flexItem->Layout(innerLayout);
    allocatedFlexSpace += flexSize;
    allocatedSize_ -= space_;
    double allocatedSize = allocatedSize_;
    ResizeByItem(flexItem, allocatedSize);
    CheckBaselineProperties(flexItem, baselineProps);
}

void RenderFlex::LayoutInfinityChild(const RefPtr<RenderNode>& item, double mainSize, BaselineProperties& baselineProps)
{
    auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
    bool isStretch = false;
    if (flexItem) {
        isStretch = (crossAxisAlign_ == FlexAlign::STRETCH) && (flexItem->GetStretchFlag());
    } else {
        isStretch = crossAxisAlign_ == FlexAlign::STRETCH;
    }
    LayoutParam innerLayout = MakeLayoutParamWithLimit(mainSize, mainSize, isStretch);
    item->Layout(innerLayout);
    double allocatedSize = allocatedSize_;
    ResizeByItem(item, allocatedSize);
    CheckBaselineProperties(item, baselineProps);
}

void RenderFlex::LayoutAbsoluteChildren()
{
    if (absoluteNodes_.empty()) {
        return;
    }
    for (const auto& item : absoluteNodes_) {
        item->Layout(GetLayoutParam());
    }
}

void RenderFlex::CheckBaselineProperties(const RefPtr<RenderNode>& item, BaselineProperties& baselineProperties)
{
    auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
    bool isChildBaselineAlign = flexItem ? flexItem->GetAlignSelf() == FlexAlign::BASELINE : false;
    if (crossAxisAlign_ == FlexAlign::BASELINE || isChildBaselineAlign) {
        double distance = item->GetBaselineDistance(textBaseline_);
        baselineProperties.maxBaselineDistance = std::max(baselineProperties.maxBaselineDistance, distance);
        baselineProperties.maxDistanceAboveBaseline = std::max(baselineProperties.maxDistanceAboveBaseline, distance);
        baselineProperties.maxDistanceBelowBaseline =
            std::max(baselineProperties.maxDistanceBelowBaseline, GetCrossSize(item) - distance);
        if (crossAxisAlign_ == FlexAlign::BASELINE) {
            crossSize_ = baselineProperties.maxDistanceAboveBaseline + baselineProperties.maxDistanceBelowBaseline;
        }
    }
}

double RenderFlex::GetBaselineDistance(TextBaseline baseline)
{
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        // in row, use default children baseline defined in render node.
        return GetHighestChildBaseline(baseline);
    } else {
        // in column, just get the first child baseline
        return GetFirstChildBaseline(baseline);
    }
}

Size RenderFlex::GetChildViewPort()
{
    if (containsNavigation_) {
        double width = viewPort_.Width();
        double height = viewPort_.Height();
        if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
            width -= navigationMainSize_;
        } else {
            height -= navigationMainSize_;
        }
        return Size(width, height);
    } else {
        return viewPort_;
    }
}

void RenderFlex::InitFlexProperties()
{
    mainSize_ = 0.0;
    crossSize_ = 0.0;
    allocatedSize_ = 0.0;
    validSizeCount_ = 0;
    layoutMode_ = FlexLayoutMode::FLEX_WEIGHT_MODE;
    totalFlexWeight_ = 0.0;
    maxDisplayIndex_ = 0;
    if (direction_ == FlexDirection::ROW) {
        isMainInfinite_ = GetLayoutParam().GetMaxSize().IsWidthInfinite();
        isCrossInfinite_ = GetLayoutParam().GetMaxSize().IsHeightInfinite();
    } else {
        isMainInfinite_ = GetLayoutParam().GetMaxSize().IsHeightInfinite();
        isCrossInfinite_ = GetLayoutParam().GetMaxSize().IsWidthInfinite();
    }
    // determine the flex layout mode
    TravelChildrenFlexProps();
}

void RenderFlex::TravelChildrenFlexProps()
{
    for (const auto& child : GetChildren()) {
        child->SetVisible(true);
        if (IsNonRelativePosition(child->GetPositionType())) {
            absoluteNodes_.insert(child);
            continue;
        }
        maxDisplayIndex_ = std::max(child->GetDisplayIndex(), maxDisplayIndex_);
        relativeNodes_.emplace_back(child);
        MagicLayoutNode node;
        node.node = child;
        auto idx = child->GetDisplayIndex();
        if (magicNodes_.find(idx) != magicNodes_.end()) {
            magicNodes_[idx].emplace_back(node);
            magicWeightMaps_[idx] += child->GetFlexWeight();
        } else {
            std::list<MagicLayoutNode> nodes;
            nodes.emplace_back(node);
            magicNodes_[idx] = nodes;
            magicWeightMaps_[idx] = child->GetFlexWeight();
        }
        // FLEX_WEIGHT_MODE now active if one child has set flexWeight
        totalFlexWeight_ += child->GetFlexWeight();
    }
    layoutMode_ =
        LessOrEqual(totalFlexWeight_, 0.0) ? FlexLayoutMode::FLEX_ITEM_MODE : FlexLayoutMode::FLEX_WEIGHT_MODE;
    if (relativeNodes_.empty() && !absoluteNodes_.empty()) {
        layoutMode_ = FlexLayoutMode::FLEX_ITEM_MODE;
    }
}

void RenderFlex::ClearChildrenLists()
{
    infinityLayoutNodes_.clear();
    absoluteNodes_.clear();
    relativeNodes_.clear();
    magicNodes_.clear();
    magicWeightMaps_.clear();
    displayNodes_.clear();
    stretchNodes_.clear();
}

void RenderFlex::ResizeByItem(const RefPtr<RenderNode>& item, double &allocatedSize)
{
    double mainSize = GetMainSize(item);
    if (NearEqual(mainSize, Size::INFINITE_SIZE)) {
        mainSize = 0.0;
        // push infinite nodes
        infinityLayoutNodes_.insert(item);
    }
    if (IsNonRelativePosition(item->GetPositionType())) {
        return;
    }

    crossSize_ = std::max(crossSize_, GetCrossSize(item));
    // Semi relative and variable allocatedSize is used for grid container.
    if ((item->GetPositionType() == PositionType::SEMI_RELATIVE) &&
        (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE)) {
        allocatedSize_ = std::max(allocatedSize_, mainSize);
        allocatedSize = mainSize;
    } else {
        allocatedSize_ += mainSize;
        allocatedSize_ += space_;
        allocatedSize += mainSize;
        allocatedSize += space_;
    }
}

void RenderFlex::CheckSizeValidity(const RefPtr<RenderNode>& item)
{
    if (item->IsIgnored() || IsNonRelativePosition(item->GetPositionType())) {
        return;
    }
    if (!item->GetLayoutSize().IsValid()) {
        auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
        if (flexItem && flexItem->IsHidden()) {
            return;
        }
    }
    ++validSizeCount_;
    displayNodes_.insert(item);
}

double RenderFlex::GetAvailableMainSize()
{
    double maxMainSize = 0.0;
    if (!isMainInfinite_ || !useViewPort_) {
        maxMainSize = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE)
                          ? GetLayoutParam().GetMaxSize().Width()
                          : GetLayoutParam().GetMaxSize().Height();
    } else {
        maxMainSize = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE)
                          ? viewPort_.Width()
                          : viewPort_.Height();
    }
    return maxMainSize;
}

double RenderFlex::GetMainSize(const RefPtr<RenderNode>& item) const
{
    double size;
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        size = item->GetLayoutSize().Width();
        if (item->GetPositionType() == PositionType::SEMI_RELATIVE) {
            Offset absoluteOffset = PositionLayoutUtils::GetAbsoluteOffset(Claim(const_cast<RenderFlex*>(this)), item);
            size += absoluteOffset.GetX();
        }
    } else {
        size = item->GetLayoutSize().Height();
    }
    return size;
}

double RenderFlex::GetCrossSize(const RefPtr<RenderNode>& item) const
{
    double size;
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        size = item->GetLayoutSize().Height();
    } else {
        size = item->GetLayoutSize().Width();
        if (item->GetPositionType() == PositionType::SEMI_RELATIVE) {
            Offset absoluteOffset = PositionLayoutUtils::GetAbsoluteOffset(Claim(const_cast<RenderFlex*>(this)), item);
            size += absoluteOffset.GetX();
        }
    }
    return size;
}

bool RenderFlex::IsStartTopLeft(FlexDirection direction, TextDirection textDirection) const
{
    switch (direction) {
        case FlexDirection::ROW:
            return textDirection == TextDirection::LTR;
        case FlexDirection::ROW_REVERSE:
            return textDirection == TextDirection::RTL;
        case FlexDirection::COLUMN:
            return true;
        case FlexDirection::COLUMN_REVERSE:
            return false;
        default:
            return true;
    }
}

Size RenderFlex::GetConstrainedSize(double mainSize)
{
    if ((stretchToParent_ && crossAxisAlign_ == FlexAlign::STRETCH) || (crossAxisSize_ == CrossAxisSize::MAX)) {
        if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
            return GetLayoutParam().Constrain(Size(mainSize, GetLayoutParam().GetMaxSize().Height()));
        } else {
            return GetLayoutParam().Constrain(Size(GetLayoutParam().GetMaxSize().Width(), mainSize));
        }
    }
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        return GetLayoutParam().Constrain(Size(mainSize, crossSize_));
    } else {
        return GetLayoutParam().Constrain(Size(crossSize_, mainSize));
    }
}

FlexAlign RenderFlex::GetSelfAlign(const RefPtr<RenderNode>& item) const
{
    auto flexItem = AceType::DynamicCast<RenderFlexItem>(item);
    if (flexItem) {
        auto alignSelf = flexItem->GetAlignSelf();
        return alignSelf == FlexAlign::AUTO ? crossAxisAlign_ : alignSelf;
    }
    return crossAxisAlign_;
}

TextDirection RenderFlex::AdjustTextDirectionByDir()
{
    auto textDir = GetTextDirection();
    if (direction_ == FlexDirection::ROW_REVERSE) {
        textDir = GetTextDirection() == TextDirection::RTL ? TextDirection::LTR : TextDirection::RTL;
    }
    return textDir;
}

void RenderFlex::OnChildRemoved(const RefPtr<RenderNode>& child)
{
    if (child) {
        child->SetAccessibilityVisible(false);
        child->ClearAccessibilityRect();
    }
    MarkNeedLayout();
}

void RenderFlex::ClearRenderObject()
{
    RenderNode::ClearRenderObject();
    direction_ = FlexDirection::ROW;
    mainAxisAlign_ = FlexAlign::FLEX_START;
    crossAxisAlign_ = FlexAlign::FLEX_START;
    mainAxisSize_ = MainAxisSize::MAX;
    crossAxisSize_ = CrossAxisSize::MIN;
    textBaseline_ = TextBaseline::ALPHABETIC;
    layoutMode_ = FlexLayoutMode::FLEX_ITEM_MODE;
    stretchToParent_ = false;
    mainSize_ = 0.0;
    crossSize_ = 0.0;
    allocatedSize_ = 0.0;
    infinityLayoutNodes_.clear();
    absoluteNodes_.clear();
    relativeNodes_.clear();
    magicNodes_.clear();
    magicWeightMaps_.clear();
    displayNodes_.clear();
    stretchNodes_.clear();
    scrollNode = nullptr;
    isMainInfinite_ = false;
    isCrossInfinite_ = false;
    useViewPort_ = false;
    containsNavigation_ = false;
    navigationMainSize_ = 0.0;
    validSizeCount_ = 0;
    totalFlexWeight_ = 0.0;
    maxDisplayIndex_ = 0;
    space_ = 0.0;
    alignPtr_ = nullptr;
}

bool RenderFlex::MaybeRelease()
{
    auto context = GetContext().Upgrade();
    if (context && context->GetRenderFactory() && context->GetRenderFactory()->GetRenderFlexFactory()->Recycle(this)) {
        ClearRenderObject();
        return false;
    }
    return true;
}

bool RenderFlex::GetAlignDeclarationOffset(AlignDeclarationPtr alignDeclarationPtr, Offset& offset) const
{
    if (alignPtr_ != alignDeclarationPtr) {
        return RenderNode::GetAlignDeclarationOffset(alignDeclarationPtr, offset);
    }
    if (alignDeclarationPtr->GetDeclarationType() == AlignDeclaration::DeclarationType::HORIZONTAL) {
        switch (alignDeclarationPtr->GetHorizontalAlign()) {
            case HorizontalAlign::START:
                break;
            case HorizontalAlign::CENTER: {
                offset = offset + Offset(GetLayoutSize().Width() / 2, 0);
                break;
            }
            case HorizontalAlign::END: {
                offset = offset + Offset(GetLayoutSize().Width(), 0);
                break;
            }
            default:
                break;
        }
        offset.SetY(0.0);
    } else {
        switch (alignDeclarationPtr->GetVerticalAlign()) {
            case VerticalAlign::TOP:
                break;
            case VerticalAlign::CENTER: {
                offset = offset + Offset(0, GetLayoutSize().Height() / 2);
                break;
            }
            case VerticalAlign::BOTTOM: {
                offset = offset + Offset(0, GetLayoutSize().Height());
                break;
            }
            case VerticalAlign::BASELINE:
            case VerticalAlign::NONE:
                return false;
            default:
                break;
        }
        offset.SetX(0.0);
    }
    return true;
}

void RenderFlex::PerformItemAlign(std::list<RefPtr<RenderNode>>& nodelist)
{
    auto item = nodelist.begin();

    while (item != nodelist.end()) {
        if (*item == AceType::Claim(this)) {
            nodelist.erase(item);
            return;
        }
        const RefPtr<RenderBox> box = AceType::DynamicCast<RenderBox>(*item);
        if (!box) {
            nodelist.clear();
            LOGE("PerformItemAlign error");
            return;
        }
        if (box->GetAlignDeclarationPtr() != alignPtr_) {
            item++;
            continue;
        }
        box->CalculateAlignDeclaration();
        item = nodelist.erase(item);
    }
}

void RenderFlex::Dump()
{
    DumpLog::GetInstance().AddDesc(std::string("Direction: ")
                                       .append(std::to_string(static_cast<int32_t>(direction_)))
                                       .append(", Mode: ")
                                       .append(std::to_string(static_cast<int32_t>(layoutMode_)))
                                       .append(", MainAlign: ")
                                       .append(std::to_string(static_cast<int32_t>(mainAxisAlign_)))
                                       .append(", CrossAlign: ")
                                       .append(std::to_string(static_cast<int32_t>(crossAxisAlign_)))
                                       .append(", MainAxisSize: ")
                                       .append(std::to_string(static_cast<int32_t>(mainAxisSize_)))
                                       .append(", CrossAxisSize: ")
                                       .append(std::to_string(static_cast<int32_t>(crossAxisSize_))));
}

bool RenderFlex::CheckIfNeedLayoutAgain()
{
    if (NeedLayout()) {
        return true;
    }
    return layoutMode_ != FlexLayoutMode::FLEX_WEIGHT_MODE;
}

} // namespace OHOS::Ace
