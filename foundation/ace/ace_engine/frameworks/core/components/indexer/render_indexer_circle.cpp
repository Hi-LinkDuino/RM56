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

#include "core/components/indexer/render_indexer_circle.h"

#include "core/components/arc/render_arc.h"
#include "core/components/indexer/render_indexer_item.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RenderIndexerCircle::Create()
{
    LOGI("[indexer] Create RenderIndexerCircle");
    return AceType::MakeRefPtr<RenderIndexerCircle>();
}

void RenderIndexerCircle::Update(const RefPtr<Component>& component)
{
    LOGD("[indexer] Update");
    RenderIndexer::Update(component);

    auto context = GetContext().Upgrade();
    if (context) {
        hotRgnSize_ = context->NormalizeToPx(Dimension(hotRgnSize_, DimensionUnit::VP));
    }
    if (itemCount_ <= INDEXER_COLLAPSE_ITEM_COUNT) {
        curStatus_ = IndexerItemStatus::COLLAPSE;
        collapseItemCount_ = itemCount_;
    }

    RefPtr<IndexerComponent> componentIndex = AceType::DynamicCast<IndexerComponent>(component);
    if (componentIndex) {
        itemMaxCount_ = std::clamp(componentIndex->GetMaxShowCount(), 1, INDEXER_ITEM_MAX_COUNT + 1);
        hasCollapseItem_ = componentIndex->HasCollapseItem();
        perItemExtent_ = DOUBLE * M_PI / itemMaxCount_;
        indexerChangeEvent_ =
            AceAsyncEvent<void(const std::string&)>::Create(componentIndex->GetIndexerChange(), context_);
    }

    LOGI("[indexer] hotRgnSize:%{public}lf, CollapseCount:%{public}d, TotalCount:%{public}d, perItemExtent:%{public}lf",
        hotRgnSize_, collapseItemCount_, itemCount_, perItemExtent_);
}

void RenderIndexerCircle::PerformLayout()
{
    LOGD("[indexer] PerformLayout RenderIndexerCircle");
    if (GetChildren().empty()) {
        return;
    }
    UpdateItems();
    if (isInitFocus_) {
        InitFocusedItem();
    }
    if (IsValidArc()) {
        arc_->SetStartAngle(arcHeadPosition_);
        arc_->SetSweepAngle(arcTailPosition_ - arcHeadPosition_);
    }
    const LayoutParam& layoutSetByParent = GetLayoutParam();
    LayoutParam childrenLayout = layoutSetByParent;
    childrenLayout.SetMinSize(Size(0.0, 0.0));
    for (const auto& item : GetChildren()) {
        item->Layout(childrenLayout);
    }

    // calculate self and children size
    Size sizeMax = GetChildren().front()->GetLayoutSize(); // get arc layout size first
    SetLayoutSize(sizeMax);

    if (items_.empty()) {
        LOGE("[indexer] invalid children count:%{public}u", static_cast<int32_t>(GetChildren().size()));
        return;
    }

    // check whether the layout size was valid
    outerRadius_ = sizeMax.Width() * HALF;
    if (LessOrEqual(outerRadius_, itemSize_ * DOUBLE)) {
        LOGE("[indexer] invalid outerRadius_:%{public}lf, itemSize_:%{public}lf", outerRadius_, itemSize_);
        return;
    }

    if (curStatus_ != IndexerItemStatus::EXPAND && itemCount_ > collapseItemCount_) {
        return;
    }

    ResetItemsPosition();
    InitBubbleBox(sizeMax);
    InitIndicatorBox();
}

void RenderIndexerCircle::SetItemPosition(const RefPtr<RenderNode>& item, int32_t positionIndex)
{
    if (positionIndex >= 0 && positionIndex <= INDEXER_ITEM_MAX_COUNT) {
        double itemRadius = outerRadius_ - itemSize_ * HALF;
        Offset position;
        position.SetX(itemRadius + itemRadius * itemInfo_[positionIndex][TABLE_POSITION_X]);
        position.SetY(itemRadius + itemRadius * itemInfo_[positionIndex][TABLE_POSITION_Y]);
        item->SetPosition(position);
    }
}

void RenderIndexerCircle::SetItemPosition(const RefPtr<RenderNode>& item, double angle)
{
    double itemRadius = outerRadius_ - itemSize_ * HALF;
    Offset position;
    position.SetX(itemRadius + itemRadius * sin(M_PI_2 + angle));
    position.SetY(itemRadius - itemRadius * cos(M_PI_2 + angle));
    item->SetPosition(position);
}

void RenderIndexerCircle::ResetItemsPosition()
{
    if (!IsValidArc() || GetChildren().empty()) {
        return;
    }
    arcHeadOffset_ = 0.0;
    int32_t count = 0;
    for (auto item : items_) {
        if (!NeedProcess(item)) {
            item->SetVisible(false);
            continue;
        }
        SetItemPosition(item, count);
        item->SetVisible(true);
        ++count;
    }

    arcMaxLen_ = count == itemMaxCount_ ? M_PI * DOUBLE : (count - 1) * perItemExtent_;
    if (curStatus_ != IndexerItemStatus::ANIMATION) {
        arc_->SetSweepAngle(arcMaxLen_);
    }
    if (!hasCollapseItem_) {
        return;
    }

    currentCount_ = count - 1;
    if (currentCount_ <= collapseItemCount_) {
        auto collapse = GetChildren().back();
        if (collapse) {
            collapse->SetVisible(false);
        }
        arc_->SetSweepAngle((currentCount_ - 1) * perItemExtent_);
        return;
    }

    SetCollapseItemPosition(itemInfo_[itemMaxCount_ - 1][TABLE_ANGLE]);
    if (curStatus_ != IndexerItemStatus::ANIMATION) {
        arcTailPosition_ = arcMaxLen_ + arcHeadPosition_;
        SetCollapseItemPosition(arcTailPosition_);
    }
}

bool RenderIndexerCircle::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!IsValidArc()) {
        LOGE("[indexer] TouchTest arc render node is invalid");
        return false;
    }
    if (GetDisableTouchEvent() || disabled_) {
        return false;
    }
    // Since the paintRect is relative to parent, use parent local point to perform touch test.
    if (arc_->IsInRegion(parentLocalPoint - GetPaintRect().GetOffset())) {
        // Calculates the local point location and coordinate offset in this node.
        const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
        const auto coordinateOffset = globalPoint - localPoint;
        globalPoint_ = globalPoint;
        OnTouchTestHit(coordinateOffset, touchRestrict, result);
        return true;
    } else {
        if (IsValidBubbleBox() && bubbleController_ && bubbleController_->IsRunning()) {
            bubbleController_->Finish();
        }
    }
    return true;
}

void RenderIndexerCircle::HandleTouchDown(const TouchEventInfo& info)
{
    LOGI("[indexer] HandleTouchDown.");
    if (curStatus_ == IndexerItemStatus::ANIMATION) {
        return;
    }
    if (info.GetTouches().empty()) {
        return;
    }
    Offset position = info.GetTouches().front().GetLocalLocation();
    LOGD("[indexer] x:%{public}lf, y:%{public}lf, curStatus_:%{public}d, nextStatus_:%{public}d", position.GetX(),
        position.GetY(), curStatus_, nextStatus_);

    if (CollapseItemHitTest(position)) {
        curStatus_ == IndexerItemStatus::EXPAND ? CollapseItems() : ExpandItems();
        return;
    }

    touching_ = true;
    if (curStatus_ == IndexerItemStatus::EXPAND) {
        int32_t item = GetNearestItem(position, false);
        if (item <= INVALID_INDEX || item >= itemCount_ - 1) {
            LOGE("[indexer] item index %{public}d is not valid", item);
            return;
        }

        if (ChangeLanguage(item)) {
            LOGI("[indexer] : change to show another language");
            touching_ = false;
            return;
        }
        SetItemsFocused(item);
    } else {
        TouchOnCollapseArc(position);
    }
}

bool RenderIndexerCircle::NeedChangeIndexer(int32_t index)
{
    if (curStatus_ != IndexerItemStatus::EXPAND) {
        return false;
    }
    auto indexerItem = GetSpecificItem(index);
    if (!indexerItem) {
        return false;
    }

    // skip # and *
    if (indexerItem->GetSectionText() == StringUtils::Str16ToStr8(INDEXER_STR_SHARP) ||
        indexerItem->GetSectionText() == StringUtils::Str16ToStr8(INDEXER_STR_COLLECT)) {
        return false;
    }
    return indexerItem->GetItemType() != curItemType_;
}

void RenderIndexerCircle::HandleTouchUp(const TouchEventInfo& info)
{
    LOGI("[indexer] HandleTouchUp.");
    curHitItem_ = -1;
    touching_ = false;
}

void RenderIndexerCircle::HandleTouchMove(const TouchEventInfo& info)
{
    if (!touching_) {
        LOGD("[indexer] not in touching.");
        return;
    }
    if (info.GetTouches().empty()) {
        return;
    }
    Offset position = info.GetTouches().front().GetLocalLocation();
    if (curStatus_ == IndexerItemStatus::EXPAND) {
        int32_t curItem = GetNearestItem(position, true);
        if (curItem == INVALID_INDEX || curItem >= itemCount_ - 1) {
            return;
        }
        if (curItem != curHitItem_) {
            curHitItem_ = curItem;
            SetItemsFocused(curItem);
        }
    } else if (curStatus_ == IndexerItemStatus::COLLAPSE) {
        TouchOnCollapseArc(position);
    } else {
        return;
    }
}

int32_t RenderIndexerCircle::GetTouchedItemIndex(const Offset& touchPosition)
{
    if (NearZero(itemSize_)) {
        LOGE("[indexer] Invalid Item size:%{public}lf", itemSize_);
        return INVALID_INDEX;
    }

    double touchAngle = atan2(touchPosition.GetY() - outerRadius_, touchPosition.GetX() - outerRadius_);
    if (touchPosition.GetY() - outerRadius_ < 0.0) {
        touchAngle += DOUBLE * M_PI;
    }
    touchAngle += arcHeadPosition_;

    int32_t itemIndexInIndexer = static_cast<int32_t>(touchAngle / perItemExtent_);
    LOGI("[indexer] HandleTouched section index:%{public}d, touchAngle:%{public}lf", itemIndexInIndexer, touchAngle);
    return GetItemIndex(itemIndexInIndexer);
}

void RenderIndexerCircle::InitIndexTable()
{
    // init information table
    LOGI("[indexer] InitIndexTable Count:%{public}d, MaxCount:%{public}d", itemCount_, itemMaxCount_);
    for (int32_t i = 0; i < itemMaxCount_; ++i) {
        itemInfo_[i][TABLE_ANGLE] = INDEXER_ARC_BEGIN + i * perItemExtent_;
        itemInfo_[i][TABLE_POSITION_X] = sin(M_PI_2 + itemInfo_[i][TABLE_ANGLE]);
        itemInfo_[i][TABLE_POSITION_Y] = -cos(M_PI_2 + itemInfo_[i][TABLE_ANGLE]);
    }
}

void RenderIndexerCircle::HandleListMoveItems(int32_t curHeadIndex)
{
    if (curStatus_ != IndexerItemStatus::EXPAND) {
        return;
    }
    if (lastHeadIndex_ != curHeadIndex) {
        CollapseItems();
    }
    LOGD("[indexer] HandleListMoveItems focusedItem_:%{public}d,  curHeadIndex:%{public}d", focusedItem_, curHeadIndex);
}

void RenderIndexerCircle::InitBubbleBox(const Size& size)
{
    if (IsValidBubbleBox()) {
        Offset bubblePosition;
        bubblePosition.SetX(
            size.Width() * HALF - NormalizeToPx(Dimension(BUBBLE_BOX_SIZE_CIRCLE * HALF, DimensionUnit::VP)));
        bubblePosition.SetY(
            size.Height() * HALF * HALF - NormalizeToPx(Dimension(BUBBLE_BOX_SIZE_CIRCLE * HALF, DimensionUnit::VP)));
        bubbleDisplay_->SetPosition(bubblePosition);
    }
}

void RenderIndexerCircle::InitIndicatorBox()
{
    if (IsValidIndicatorBox()) {
        Offset boxPosition;
        double angle = INDEXER_ARC_BEGIN + (collapseItemCount_ - 1) * perItemExtent_;
        double itemRadius = outerRadius_ - itemSize_ * HALF;
        boxPosition.SetX(itemRadius + itemRadius * sin(M_PI_2 + angle));
        boxPosition.SetY(itemRadius - itemRadius * cos(M_PI_2 + angle));
        indicatorBox_->SetPosition(boxPosition);
        indicatorBox_->SetColor(Color::TRANSPARENT, false);
        itemColorEnabled_ = true;
    }
}

bool RenderIndexerCircle::IsValidIndicatorBox()
{
    if (!indicatorBox_ && !GetChildren().empty()) {
        auto iter = GetChildren().begin();
        std::advance(iter, 1);
        indicatorBox_ = iter == GetChildren().end() ? nullptr : AceType::DynamicCast<RenderBox>(*iter);
        if (!indicatorBox_) {
            LOGE("[indexer] IsValidIndicatorBox. indicatorBox_ ptr is NULL.");
            return false;
        }
    }
    return true;
}

bool RenderIndexerCircle::IsValidBubbleBox()
{
    if (!bubbleEnabled_ || GetChildren().empty()) {
        return false;
    }
    if (!bubbleDisplay_) {
        auto iter = GetChildren().begin();
        std::advance(iter, 2);
        bubbleDisplay_ = iter == GetChildren().end() ? nullptr : AceType::DynamicCast<RenderDisplay>(*iter);
        if (!bubbleDisplay_ || bubbleDisplay_->GetChildren().empty()) {
            LOGE("[indexer] IsValidBubbleBox. bubbleBox_ ptr is NULL.");
            return false;
        }
        bubbleBox_ = AceType::DynamicCast<RenderBox>(bubbleDisplay_->GetChildren().front());
        if (!bubbleBox_) {
            return false;
        }
    }
    return true;
}

bool RenderIndexerCircle::IsValidArc()
{
    if (!arc_) {
        arc_ = GetChildren().empty() ? nullptr : AceType::DynamicCast<RenderArc>(GetChildren().front());
        if (!arc_) {
            LOGE("[indexer] IsValidArc. Arc ptr is NULL.");
            return false;
        }
    }
    return true;
}

void RenderIndexerCircle::UpdateCurrentSectionItem(int32_t curSection)
{
    if (curStatus_ != IndexerItemStatus::COLLAPSE) {
        return;
    }

    // calculate drag angle
    int32_t index = GetPositionItemIndex(curSection);
    double offsetAngle = -(index + 1 - collapseItemCount_) * perItemExtent_;
    offsetAngle = std::min(offsetAngle, 0.0);

    // make indicator box visible and item background color disabled
    if (LessNotEqual(offsetAngle, 0.0)) {
        if (focusController_ && focusController_->IsRunning()) {
            focusController_->Finish();
        }
        if (IsValidIndicatorBox()) {
            indicatorBox_->SetColor(Color(INDEXER_CIRCLE_ACTIVE_BG_COLOR), false);
            itemColorEnabled_ = false;
        }
    }

    LOGD("[indexer] UpdateCurrentSectionItem curSection:%{public}d, offsetAngle:%{public}lf itemCount_:%{public}d, "
         "collapseItemCount_:%{public}d arcHeadPosition_:%{public}lf, collapseItemCount_:%{public}lf",
        curSection, offsetAngle, itemCount_, collapseItemCount_, arcHeadPosition_, arcTailPosition_);
    if (!NearEqual(offsetAngle, arcHeadOffset_)) {
        BeginCollapseScrollAnimation(arcHeadOffset_, offsetAngle);
    }
}

void RenderIndexerCircle::CollapseItems()
{
    LOGI("[indexer] CollapseItems");
    if (currentCount_ <= collapseItemCount_) {
        LOGD("[indexer] current show item count(%{public}d), no need to collapse", currentCount_);
        return;
    }

    if (collapseScrollController_ && collapseScrollController_->IsRunning()) {
        itemColorEnabled_ = true;
        collapseScrollController_->Finish();
    }

    // finish focus change animation first
    if (focusController_ && focusController_->IsRunning()) {
        focusController_->Finish();
    }
    BeginArcAnimation(true);
}

void RenderIndexerCircle::ExpandItems()
{
    LOGI("[indexer] ExpandItems");
    if (IsValidIndicatorBox()) {
        indicatorBox_->SetColor(Color::TRANSPARENT, false);
        itemColorEnabled_ = true;
        UpdateItemBackground(DEFAULT_OPACITY_IN_PERCENT, focusedItem_);
    }
    BeginArcAnimation(false);
}

void RenderIndexerCircle::BuildArcAnimation()
{
    if (!IsValidArc()) {
        LOGE("[indexer] BuildArcAnimation arc render node is invalid");
        return;
    }
    if (!collapseController_) {
        collapseController_ = AceType::MakeRefPtr<Animator>(GetContext());
    } else {
        collapseController_->ClearInterpolators();
        collapseController_->ClearAllListeners();
    }

    double collapseTail = arcHeadPosition_ + collapseItemCount_ * perItemExtent_;
    auto tailStartFrame = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_BEGIN, collapseTail);
    auto tailEndFrame = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_END, arcHeadPosition_ + arcMaxLen_);
    tailEndFrame->SetCurve(Curves::FAST_OUT_SLOW_IN);

    auto tailInterpolator = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    tailInterpolator->AddKeyframe(tailStartFrame);
    tailInterpolator->AddKeyframe(tailEndFrame);
    tailInterpolator->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto indexer = weak.Upgrade();
        if (indexer) {
            indexer->UpdateArcTail(value);
        }
    });

    collapseController_->AddInterpolator(tailInterpolator);
    auto weak = AceType::WeakClaim(this);
    collapseController_->AddStopListener([weak]() {
        auto indexer = weak.Upgrade();
        if (indexer) {
            indexer->HandleArcAnimationStop();
        }
    });
    collapseController_->AddStartListener([weak]() {
        auto indexer = weak.Upgrade();
        if (indexer) {
            indexer->HandleArcAnimationStart();
        }
    });
    collapseController_->SetDuration(INDEXER_ANIMATION_DURATION);
    collapseController_->SetFillMode(FillMode::FORWARDS);
}

void RenderIndexerCircle::BeginArcAnimation(bool collapse)
{
    if (!IsValidArc()) {
        LOGE("[indexer] BeginArcAnimation arc render node is invalid");
        return;
    }
    if (!collapseController_) {
        BuildArcAnimation();
    }
    if (collapseController_->IsRunning()) {
        collapseController_->Finish();
    }
    collapse ? collapseController_->Backward() : collapseController_->Forward();
}

void RenderIndexerCircle::HandleArcAnimationStart()
{
    nextStatus_ = curStatus_ == IndexerItemStatus::EXPAND ? IndexerItemStatus::COLLAPSE : IndexerItemStatus::EXPAND;
    curStatus_ = IndexerItemStatus::ANIMATION;
}

void RenderIndexerCircle::HandleArcAnimationStop()
{
    curStatus_ = nextStatus_;
}

void RenderIndexerCircle::BeginCollapseScrollAnimation(double originOffset, double targetOffset)
{
    if (!collapseScrollController_) {
        collapseScrollController_ = AceType::MakeRefPtr<Animator>(GetContext());
    }

    if (collapseScrollController_->IsRunning()) {
        collapseScrollController_->Finish();
    }
    collapseScrollController_->ClearInterpolators();
    collapseScrollController_->ClearAllListeners();
    collapseScrollController_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto indexer = weak.Upgrade();
        if (indexer) {
            indexer->HandleCollapseScrollAnimationStop();
        }
    });

    InitCollapseScrollInterpolator(originOffset, targetOffset);
    collapseScrollController_->SetDuration(INDEXER_ANIMATION_DURATION);
    collapseScrollController_->Play();
}

void RenderIndexerCircle::HandleCollapseScrollAnimationStop()
{
    if (IsValidIndicatorBox() && NearEqual(0.0, arcHeadOffset_)) {
        indicatorBox_->SetColor(Color::TRANSPARENT, false);
        itemColorEnabled_ = true;
        UpdateItemBackground(DEFAULT_OPACITY_IN_PERCENT, focusedItem_);
    }
}

void RenderIndexerCircle::BeginFocusAnimation(int32_t originIndex, int32_t targetIndex)
{
    if (!NearEqual(0.0, arcHeadOffset_) && !itemColorEnabled_) {
        return;
    }

    // stop and reset animator
    if (!focusController_) {
        focusController_ = AceType::MakeRefPtr<Animator>(GetContext());
    }

    if (focusController_->IsRunning()) {
        focusController_->Finish();
        SetIndexerItemStatus(false, originIndex, targetIndex);
    }
    focusController_->ClearInterpolators();
    focusController_->ClearAllListeners();

    SetIndexerItemStatus(true, originIndex, targetIndex);

    auto weak = AceType::WeakClaim(this);
    focusController_->AddStopListener([weak, originIndex, targetIndex]() {
        auto indexer = weak.Upgrade();
        if (indexer) {
            indexer->SetIndexerItemStatus(false, originIndex, targetIndex);
        }
    });

    InitFocusInterpolator(ZERO_OPACITY_IN_PERCENT, DEFAULT_OPACITY_IN_PERCENT, originIndex, targetIndex);
    focusController_->SetDuration(INDEXER_ANIMATION_DURATION);
    focusController_->Play();
}

void RenderIndexerCircle::SetIndexerItemStatus(bool inAnimation, int32_t originIndex, int32_t targetIndex)
{
    RefPtr<RenderIndexerItem> focusItem = GetSpecificItem(targetIndex);
    RefPtr<RenderIndexerItem> preFocusItem = GetSpecificItem(originIndex);
    if (!focusItem || !preFocusItem) {
        return;
    }
    focusItem->MarkItemInAnimation(inAnimation);
    preFocusItem->MarkItemInAnimation(inAnimation);

    if (!inAnimation) {
        focusItem->UpdateItemStyle();
        preFocusItem->UpdateItemStyle();
    }
}

void RenderIndexerCircle::UpdateItemsPosition(double arcOffset)
{
    if (curStatus_ == IndexerItemStatus::EXPAND) {
        return;
    }

    RotateItems(arcOffset);
    SetItemsVisible(arcHeadPosition_ - arcOffset, arcTailPosition_ - perItemExtent_ - arcOffset);
    SetCollapseItemPosition(itemInfo_[collapseItemCount_][TABLE_ANGLE]);
    MarkNeedRender();
}

void RenderIndexerCircle::UpdateItemBackground(double value, int32_t originIndex, int32_t targetIndex)
{
    UpdateItemBackground(value, targetIndex);
    UpdateItemBackground(DEFAULT_OPACITY_IN_PERCENT - value, originIndex);
}

void RenderIndexerCircle::UpdateItemBackground(double value, int32_t index)
{
    RefPtr<RenderIndexerItem> item = GetSpecificItem(index);
    if (item) {
        item->UpdateItemBackground(Color(INDEXER_CIRCLE_ACTIVE_BG_COLOR).BlendOpacity(value));
    }
}

void RenderIndexerCircle::InitCollapseScrollInterpolator(double origin, double target)
{
    auto startFrame = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_BEGIN, origin);
    auto endFrame = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_END, target);
    endFrame->SetCurve(Curves::FAST_OUT_SLOW_IN);

    auto animation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    animation->AddKeyframe(startFrame);
    animation->AddKeyframe(endFrame);
    animation->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto indexer = weak.Upgrade();
        if (indexer) {
            indexer->UpdateItemsPosition(value);
        }
    });

    collapseScrollController_->AddInterpolator(animation);
}

void RenderIndexerCircle::InitFocusInterpolator(double origin, double target, int32_t originIndex, int32_t targetIndex)
{
    auto startFrame = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_BEGIN, origin);
    auto endFrame = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_END, target);
    endFrame->SetCurve(Curves::FAST_OUT_SLOW_IN);

    auto weak = AceType::WeakClaim(this);
    auto animation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    animation->AddKeyframe(startFrame);
    animation->AddKeyframe(endFrame);
    animation->AddListener([weak, originIndex, targetIndex](double value) {
        auto indexer = weak.Upgrade();
        if (indexer) {
            indexer->UpdateItemBackground(value, originIndex, targetIndex);
        }
    });

    focusController_->AddInterpolator(animation);
}

void RenderIndexerCircle::UpdateArcTail(double value)
{
    if (!IsValidArc()) {
        LOGE("[indexer] UpdateArcTail arc render node is invalid");
        return;
    }
    LOGD("[indexer] UpdateArcTail value:%{public}lf, arcHeadPosition_:%{public}lf, sweepAngle(%{public}lf)", value,
        arcHeadPosition_, arc_->GetSweepAngle());
    if (focusedItem_ >= itemCount_ - 1) {
        LOGE("[indexer] UpdateArcTail. Invalid focusedItem:%{public}d", focusedItem_);
        return;
    }

    arcTailPosition_ = value;
    arc_->SetSweepAngle(arcTailPosition_ - arcHeadPosition_);

    // calculate focused item index on circle
    int32_t index = GetPositionItemIndex(focusedItem_);
    double focusAngle = itemInfo_[index][TABLE_ANGLE];
    double expectFocusAngle = arcTailPosition_ - perItemExtent_;
    double offsetAngle = expectFocusAngle - focusAngle;

    LOGD("[indexer]focusItem(%{public}d), index(%{public}d), focusAngle(%{public}lf), expectAngle(%{public}lf)",
        focusedItem_, index, focusAngle, expectFocusAngle);
    if (focusAngle > expectFocusAngle) {
        // items need move
        RotateItems(offsetAngle); // update items position
        SetItemsVisible(focusAngle - (arcTailPosition_ - arcHeadPosition_ - perItemExtent_), focusAngle);
    } else {
        // items do not need move, reset position directly
        ResetItemsPosition();
        SetItemsVisible(arcHeadPosition_, arcTailPosition_ - perItemExtent_);
    }
    SetCollapseItemPosition(value);
    MarkNeedRender();
}

void RenderIndexerCircle::SetCollapseItemPosition(double position)
{
    if (GetChildren().empty()) {
        return;
    }
    auto item = GetChildren().back();
    if (!item) {
        return;
    }

    double finalPosition = position;
    LOGD("[indexer] SetCollapseItemPosition position:%{public}lf", finalPosition);
    if (itemCount_ <= collapseItemCount_) {
        item->SetVisible(false);
        return;
    } else {
        item->SetVisible(true);
    }

    if (finalPosition < itemInfo_[collapseItemCount_][TABLE_ANGLE]) {
        finalPosition = itemInfo_[collapseItemCount_][TABLE_ANGLE];
    }

    if (position > itemInfo_[itemMaxCount_ - 1][TABLE_ANGLE]) {
        finalPosition = itemInfo_[itemMaxCount_ - 1][TABLE_ANGLE];
    }

    SetItemPosition(item, finalPosition);
    collapseItemPosition_ = finalPosition;

    // rotate the collapse item ">"
    bool collapse = false;
    if (curStatus_ == IndexerItemStatus::EXPAND) {
        collapse = true;
    }
    if (curStatus_ == IndexerItemStatus::ANIMATION && nextStatus_ == IndexerItemStatus::EXPAND &&
        NearEqual(collapseItemPosition_, itemInfo_[itemMaxCount_ - 1][TABLE_ANGLE])) {
        collapse = true;
    }
    // rotate angle
    double rotate = collapseItemPosition_ + M_PI_2;
    if (collapse) {
        rotate += M_PI;
    }

    RefPtr<RenderIndexerItem> collapseItem = AceType::DynamicCast<RenderIndexerItem>(GetChildren().back());
    if (collapseItem) {
        collapseItem->SetRotate(rotate * INDEXER_ANGLE_TO_RADIAN / M_PI);
    }
}

bool RenderIndexerCircle::CollapseItemHitTest(const Offset& position)
{
    // do not exist a collapse item
    if (GetChildren().empty() || !hasCollapseItem_) {
        return false;
    }
    // exist a collapse item but not visible
    auto collapse = GetChildren().back();
    if (collapse && !collapse->GetVisible()) {
        return false;
    }

    double hotRgnRadius = hotRgnSize_ * HALF;
    double touchAngle = GetPositionAngle(position);
    double touchRadiusTail = hotRgnRadius / (outerRadius_ - itemSize_ * HALF);

    if (touchAngle < collapseItemPosition_ - perItemExtent_ * HALF ||
        touchAngle > collapseItemPosition_ + touchRadiusTail) {
        LOGI("[indexer] CollapseItemHitTest, NOT HIT");
        return false;
    }

    double centerX = outerRadius_ + (outerRadius_ - itemSize_ * HALF) * sin(M_PI_2 + collapseItemPosition_);
    double centerY = outerRadius_ - (outerRadius_ - itemSize_ * HALF) * cos(M_PI_2 + collapseItemPosition_);
    double distance = pow(position.GetX() - centerX, 2) + pow(position.GetY() - centerY, 2);

    LOGD("[indexer] HitTest %s cx:%{public}lf cy:%{public}lf dis:%{public}lf pos:%{public}lf hotRgnRadius:%{public}lf",
        position.ToString().c_str(), centerX, centerY, distance, collapseItemPosition_, hotRgnRadius);

    if (distance > hotRgnRadius * hotRgnRadius) {
        LOGI("[indexer] CollapseItemHitTest, NOT HIT");
        return false;
    } else {
        LOGI("[indexer] CollapseItemHitTest, HIT");
        return true;
    }
}

void RenderIndexerCircle::TouchOnCollapseArc(const Offset& position)
{
    LOGD("[indexer] TouchOnCollapseArc item is curStatus_:%{public}d", curStatus_);
    if (curStatus_ != IndexerItemStatus::COLLAPSE) {
        return;
    }

    double touchAngle = GetPositionAngle(position);
    double touchRadius = hotRgnSize_ / (outerRadius_ - itemSize_ * HALF) * HALF;

    LOGD("[indexer] TouchOnCollapseArc item is touchAngle:%{public}lf, touchRadius:%{public}lf, "
         "arcHeadPosition_:%{public}lf, arcTailPosition_:%{public}lf",
        touchAngle, touchRadius, arcHeadPosition_, arcTailPosition_);
    if (touchAngle < arcHeadPosition_ - touchRadius || touchAngle > arcTailPosition_ - perItemExtent_ + touchRadius) {
        return;
    }

    // focus
    int32_t curItem = GetNearestItem(touchAngle);
    curItem = GetActualItemIndex(curItem);
    LOGD("[indexer] TouchOnCollapseArc item is curItem:%{public}d, itemCount_:%{public}d", curItem, itemCount_);
    // to except collapse item
    if (curItem == INVALID_INDEX || curItem >= itemCount_ - 1) {
        return;
    }
    if (curItem != curHitItem_) {
        curHitItem_ = curItem;
        SetItemsFocused(curItem);
    }

    MarkNeedRender();
}

void RenderIndexerCircle::RotateItems(double arcOffset)
{
    LOGD("[indexer] RotateItems arcOffset:%{public}lf", arcOffset);
    int32_t count = 0;
    for (auto item : items_) {
        // to except collapse item
        if (count == itemCount_ - 1 || count >= itemMaxCount_) {
            break;
        }

        if (!NeedProcess(item)) {
            continue;
        }

        // calculate item position
        SetItemPosition(item, itemInfo_[count][TABLE_ANGLE] + arcOffset);
        ++count;
    }
    arcHeadOffset_ = arcOffset;
}

void RenderIndexerCircle::SetItemsVisible(double arcHead, double arcTail)
{
    if (curStatus_ == IndexerItemStatus::EXPAND) {
        for (auto item : items_) {
            item->SetVisible(true);
        }
        return;
    }

    int32_t count = 0;
    double visibleHead = arcHead - perItemExtent_ * HALF * HALF;
    double visibleTail = arcTail + perItemExtent_ * HALF * HALF;
    for (auto item : items_) {
        if (count >= itemCount_ - 1 || count >= itemMaxCount_) {
            break;
        }
        if (!NeedProcess(item)) {
            continue;
        }
        item->SetVisible(itemInfo_[count][TABLE_ANGLE] > visibleHead && itemInfo_[count][TABLE_ANGLE] < visibleTail);
        ++count;
    }
}

int32_t RenderIndexerCircle::GetPositionItemIndex(int32_t index)
{
    int32_t positionIndex = 0;
    int32_t count = 0;
    for (auto item : items_) {
        if (count == index) {
            break;
        }
        if (NeedProcess(item)) {
            positionIndex++;
        }
        count++;
    }

    return positionIndex;
}

int32_t RenderIndexerCircle::GetActualItemIndex(int32_t positionIndex)
{
    int32_t index = 0;
    int32_t count = 0;
    for (auto item : items_) {
        if (NeedProcess(item)) {
            count++;
        }
        if (positionIndex == count - 1) {
            break;
        }
        index++;
    }
    return index;
}

void RenderIndexerCircle::HandleRotation(double value)
{
    rotationStepValue_ += value;
    if (GreatOrEqual(rotationStepValue_, ROTATION_THRESHOLD)) {
        // ShowPrevious
        for (int32_t i = focusedItem_ - 1; i >= 0; i--) {
            auto indexerItem = GetSpecificItem(i);
            if (indexerItem && indexerItem->GetKeyCount() > 0 && NeedProcess(indexerItem)) {
                SetItemsFocused(i);
                break;
            }
        }
        rotationStepValue_ = 0.0;
    } else if (LessOrEqual(rotationStepValue_, -ROTATION_THRESHOLD)) {
        // ShowNext
        for (int32_t i = focusedItem_ + 1; i < itemCount_; i++) {
            auto indexerItem = GetSpecificItem(i);
            if (indexerItem && indexerItem->GetKeyCount() > 0 && NeedProcess(indexerItem)) {
                SetItemsFocused(i);
                break;
            }
        }
        rotationStepValue_ = 0.0;
    }
}

bool RenderIndexerCircle::RotationTest(const RotationEvent& event)
{
    if (curStatus_ != IndexerItemStatus::EXPAND) {
        return false;
    }
    HandleRotation(event.value);
    return true;
}

int32_t RenderIndexerCircle::GetNearestItem(const Offset& position, bool move)
{
    double touchAngle = GetPositionAngle(position);
    int32_t positionIndex = round((touchAngle - arcHeadPosition_ - arcHeadOffset_) / perItemExtent_);

    // calculate actual index of indexer by position
    int32_t ret = GetActualItemIndex(positionIndex);
    LOGD("[indexer] ExpandItems touchAngle:%{public}lf, arcHeadPosition_:%{public}lf, arcHeadOffset_:%{public}lf",
        touchAngle, arcHeadPosition_, arcHeadOffset_);
    if (ret < 0 || ret >= itemCount_) {
        LOGW("[indexer] GetNearestItem failed, ret:%{public}d", ret);
        return INVALID_INDEX;
    }

    double hotRgnRadius = hotRgnSize_ * HALF;
    double itemCenterRadius = outerRadius_ - itemSize_ * HALF;
    if (!move) {
        double centerX = outerRadius_ + itemCenterRadius * itemInfo_[positionIndex][TABLE_POSITION_X];
        double centerY = outerRadius_ + itemCenterRadius * itemInfo_[positionIndex][TABLE_POSITION_Y];
        double distance = (position.GetX() - centerX) * (position.GetX() - centerX) +
                          (position.GetY() - centerY) * (position.GetY() - centerY);
        if (distance > hotRgnRadius * hotRgnRadius) {
            return INVALID_INDEX;
        }
    }
    LOGD("[indexer] GetNearestItem ret:%{public}d", ret);
    return ret;
}

int32_t RenderIndexerCircle::GetNearestItem(double position)
{
    return round((position - arcHeadPosition_ - arcHeadOffset_) / perItemExtent_);
}

bool RenderIndexerCircle::NeedRotation() const
{
    return curStatus_ == IndexerItemStatus::EXPAND;
}

bool RenderIndexerCircle::NeedProcess(const RefPtr<RenderNode>& item) const
{
    auto indexerItem = AceType::DynamicCast<RenderIndexerItem>(item);
    if (indexerItem && (indexerItem->GetItemType() == curItemType_ || indexerItem->IsItemPrimary())) {
        return true;
    }
    return false;
}

bool RenderIndexerCircle::ChangeLanguage(int32_t index)
{
    if (!NeedChangeIndexer(index)) {
        return false;
    }
    curItemType_ = 1 - curItemType_;
    FireIndexerChangeEvent();
    SetItemsFocused(index, true, false);
    MarkNeedLayout(true);
    return true;
}

double RenderIndexerCircle::GetPositionAngle(const Offset& position)
{
    double touchAngle = atan2(position.GetY() - outerRadius_, position.GetX() - outerRadius_);
    double itemRadius = itemSize_ / (outerRadius_ - itemSize_ * HALF) * HALF;
    if (touchAngle < arcHeadPosition_ - itemRadius) {
        touchAngle += M_PI * DOUBLE;
    }
    return touchAngle;
}

void RenderIndexerCircle::SetItemsFocused(int32_t index, bool force, bool smooth)
{
    if (index < 0 || index >= itemCount_ - 1) {
        LOGE("Invalid index:%{public}d", index);
        return;
    }

    RefPtr<RenderIndexerItem> curItem = GetSpecificItem(index);
    if (!curItem || curItem->GetKeyCount() <= 0) {
        LOGE("Invalid indexer item:%{public}d", index);
        return;
    }
    if (focusedItem_ == index) {
        if (force) {
            curItem->SetClicked(true);
            MoveList(curItem->GetSectionIndex());
        }
        return;
    }

    // change to correct index and move list to correct position
    if (smooth) {
        BeginFocusAnimation(focusedItem_, index);
    }
    if (IsValidIndicatorBox()) {
        indicatorBox_->SetColor(Color::TRANSPARENT, false);
        itemColorEnabled_ = true;
    }
    curItem->SetClicked(true);
    MoveList(curItem->GetSectionIndex());

    // Make pre clicked item blur.
    auto preItem = GetSpecificItem(focusedItem_);
    if (preItem) {
        preItem->SetClicked(false);
    }
    focusedItem_ = index;
    isInitFocus_ = false;
    UpdateBubbleText();
}

void RenderIndexerCircle::FireIndexerChangeEvent() const
{
    if (indexerChangeEvent_) {
        LOGI("indexerChangeEvent_, curItemType_ = %{public}d.", curItemType_);
        std::string param =
            std::string(R"("indexerchange",{"local":)").append(curItemType_ == 0 ? "true" : "false").append("},null");
        indexerChangeEvent_(param);
    }
}

} // namespace OHOS::Ace
