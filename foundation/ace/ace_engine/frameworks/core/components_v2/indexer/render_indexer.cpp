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

#include "core/components_v2/indexer/render_indexer.h"

#include "base/log/event_report.h"
#include "core/components/arc/render_arc.h"
#include "core/components/text/render_text.h"

namespace OHOS::Ace::V2 {
namespace {
constexpr Dimension FOCUS_PADDING = 2.0_vp;
constexpr int32_t PADDIN_SIZE = 2;
} // namespace

RefPtr<RenderNode> RenderIndexer::Create()
{
    return AceType::MakeRefPtr<RenderIndexer>();
}

RenderIndexer::RenderIndexer()
{
    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    touchRecognizer_->SetOnTouchDown([wp = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->HandleTouchDown(info);
        }
    });
    touchRecognizer_->SetOnTouchUp([wp = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->HandleTouchUp(info);
        }
    });
    touchRecognizer_->SetOnTouchMove([wp = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->HandleTouchMove(info);
        }
    });
}

void RenderIndexer::Update(const RefPtr<Component>& component)
{
    RefPtr<IndexerComponent> indexerComponent = AceType::DynamicCast<IndexerComponent>(component);
    if (!indexerComponent) {
        LOGE("[indexer] Update Get component failed");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    nonItemCount_ = indexerComponent->GetNonItemCount();
    bubbleEnabled_ = indexerComponent->IsBubbleEnabled();
    popupListEnabled_ = indexerComponent->IsPopupListEnabled();
    bubbleText_ = indexerComponent->GetBubbleTextComponent();
    focusedItem_ = indexerComponent->GetSelectedIndex();
    alignStyle_ = indexerComponent->GetAlignStyle();
    color_ = indexerComponent->GetBubbleBackgroundColor();
    valueArray_ = indexerComponent->GetArrayValue();

    // update item information
    auto context = GetContext().Upgrade();
    if (context) {
        itemSize_ = context->NormalizeToPx(indexerComponent->GetItemSize());
    }
    itemSizeRender_ = itemSize_;
    itemCount_ = indexerComponent->GetItemCount();
    LOGI("[indexer] Init data, itemSizeRender_:%{public}lf, itemCount_:%{public}d", itemSizeRender_, itemCount_);

    if (IsValidBubbleBox() && !bubbleBox_->GetChildren().empty()) {
        auto text = AceType::DynamicCast<RenderText>(bubbleBox_->GetChildren().front());
        auto item = GetSpecificItem(focusedItem_);
        if (bubbleText_ && text && item) {
            bubbleText_->SetData(item->GetSectionText());
            text->Update(bubbleText_);
            text->PerformLayout();
        }
    }
    MarkNeedLayout();
    selectedEventFun_ = AceSyncEvent<void(const std::shared_ptr<IndexerEventInfo>&)>::
        Create(indexerComponent->GetSelectedEvent(), context_);
    requestPopupDataEventFun_ = indexerComponent->GetRequestPopupDataFunc();
}

void RenderIndexer::PerformLayout()
{
    UpdateItems();
    if (itemCount_ <= 0) {
        SetLayoutSize(Size());
        return;
    }

    // calculate self and children size,
    if (NearZero(itemSize_)) {
        LOGE("[indexer] Invalid Item size:%{public}lf", itemSize_);
        return;
    }

    // calculate the size of the items
    const LayoutParam& layoutSetByParent = GetLayoutParam();
    Size sizeMax = layoutSetByParent.GetMaxSize();
    if (LessOrEqual(paddingY_ * DOUBLE + itemCount_ * itemSize_, sizeMax.Height())) {
        itemSizeRender_ = itemSize_;
    } else {
        itemSizeRender_ = (sizeMax.Height() - paddingY_ * DOUBLE) / itemCount_;
    }

    InitFocusedItem();
    LayoutParam childrenLayout;
    childrenLayout.SetMinSize(Size(0.0, 0.0));
    for (const auto& item : GetChildren()) {
        item->Layout(childrenLayout);
    }

    // then set the position of children
    Offset position;
    int32_t count = 0;
    for (const auto& item : GetChildren()) {
        if (!AceType::InstanceOf<RenderIndexerItem>(item)) {
            continue;
        }
        position.SetX(paddingX_);
        position.SetY(paddingY_ + count * itemSizeRender_);
        item->SetPosition(position);
        count++;
    }

    double indexerWidth = paddingX_ + itemSizeRender_ + paddingX_;
    double indexerHeight = paddingY_ + count * itemSizeRender_ + paddingY_;
    SetLayoutSize(Size(indexerWidth, indexerHeight));
    // layout bubble and popup list.
    LayoutPopup();
}

void RenderIndexer::UpdateItems()
{
    if (nonItemCount_ + static_cast<int32_t>(items_.size()) == static_cast<int32_t>(GetChildren().size())) {
        LOGI("[indexer] no need update Items");
        return;
    }
    items_.clear();
    for (auto item : GetChildren()) {
        if (AceType::InstanceOf<RenderIndexerItem>(item)) {
            items_.push_back(item);
        }
    }
    LOGI("[indexer] items nums : %{public}d", static_cast<int32_t>(items_.size()));
}

void RenderIndexer::InitFocusedItem()
{
    for (auto item : items_) {
        RefPtr<RenderIndexerItem> indexerItem = AceType::DynamicCast<RenderIndexerItem>(item);
        if (indexerItem) {
            indexerItem->SetClicked(false);
        }
    }

    if (focusedItem_ >= 0) {
        auto item = GetSpecificItem(focusedItem_);
        if (item) {
            item->SetClicked(true);
        }
    }
}

void RenderIndexer::LayoutPopup()
{
    if (IsValidBubbleBox()) {
        Offset bubblePosition;
        if (alignStyle_ == AlignStyle::RIGHT) {
            bubblePosition.SetX(NormalizeToPx(-BUBBLE_POSITION_X) - bubbleDisplay_->GetLayoutSize().Width());
            bubblePosition.SetY(NormalizeToPx(BUBBLE_POSITION_Y));
        } else {
            bubblePosition.SetX(NormalizeToPx(BUBBLE_POSITION_X) + GetLayoutSize().Width());
            bubblePosition.SetY(NormalizeToPx(BUBBLE_POSITION_Y));
        }

        bubbleDisplay_->SetPosition(bubblePosition);
#ifdef ENABLE_ROSEN_BACKEND
        bubbleDisplay_->SetPaintOutOfParent(true);
#endif
    }

    if (IsValidPopupList()) {
        Offset popupPosition;
        popupPosition.SetX(bubbleDisplay_->GetPosition().GetX());
        popupPosition.SetY(bubbleDisplay_->GetPosition().GetY() + bubbleDisplay_->GetLayoutSize().Height());
        popupListDisplay_->SetPosition(popupPosition);
#ifdef ENABLE_ROSEN_BACKEND
        popupListDisplay_->SetPaintOutOfParent(true);
#endif
    }
}

void RenderIndexer::HandleTouchDown(const TouchEventInfo& info)
{
    if (touchBubbleDisplay || touchPopupListDisplay) {
        LOGI("touch down bubble or popup list.");
        if (IsValidBubbleBox()) {
            bubbleDisplay_->UpdateOpacity(DEFAULT_OPACITY);
        }

        if (IsValidPopupList()) {
            popupListDisplay_->UpdateOpacity(DEFAULT_OPACITY);
        }

        if (bubbleController_) {
            bubbleController_->Pause();
        }
        return;
    }

    if (info.GetTouches().empty()) {
        return;
    }

    touchPostion_ = info.GetTouches().front().GetLocalLocation();
    LOGI("[indexer] item is HandleTouchDown x:%{public}lf, y:%{public}lf", touchPostion_.GetX(), touchPostion_.GetY());
    HandleTouched(touchPostion_);
    clicked_ = true;

    MarkNeedLayout();
}

void RenderIndexer::HandleTouchUp(const TouchEventInfo& info)
{
    if ((touchBubbleDisplay || touchPopupListDisplay) && bubbleController_) {
        LOGI("touch up bubble or popup list.");
        bubbleController_->UpdatePlayedTime(0);
        bubbleController_->Resume();
        return;
    }

    if (info.GetTouches().empty()) {
        return;
    }

    touchPostion_ = info.GetTouches().front().GetLocalLocation();
    LOGI("[indexer] item is HandleTouchUp x:%{public}lf, y:%{public}lf", touchPostion_.GetX(), touchPostion_.GetY());
    HandleTouched(touchPostion_);
    if (clicked_) {
        clicked_ = false;
        MarkNeedLayout();
    }
}

void RenderIndexer::HandleTouchMove(const TouchEventInfo& info)
{
    if (touchBubbleDisplay || touchPopupListDisplay) {
        return;
    }

    if (info.GetTouches().empty()) {
        return;
    }

    touchPostion_ = info.GetTouches().front().GetLocalLocation();
    LOGI("[indexer] item is HandleTouchMove x:%{public}lf, y:%{public}lf", touchPostion_.GetX(), touchPostion_.GetY());
    HandleTouched(touchPostion_);
    if (clicked_) {
        clicked_ = true;
        MarkNeedLayout();
    }
}

bool RenderIndexer::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (GetDisableTouchEvent() || disabled_) {
        return false;
    }
    auto focusedNode = GetSpecificItem(focusedItem_);
    if (focusedNode) {
        focusedNode->SetFocused(false);
    }

    // reset touch flag
    touchBubbleDisplay = false;
    touchPopupListDisplay = false;
    Rect bubbleRect;
    Rect popupListRect;

    auto isBubbleRect = GetBubbleRect(bubbleRect);
    if (!isBubbleRect) {
        LOGW("bubble rect wrong.");
    }

    auto isPopupListRect = GetPopupListRect(popupListRect);
    if (!isPopupListRect) {
        LOGW("popup list rect wrong.");
    }

    // Since the paintRect is relative to parent, use parent local point to perform touch test.
    if (GetPaintRect().IsInRegion(parentLocalPoint)) {
        LOGI("touch in indexer");

        // Calculates the local point location and coordinate offset in this node.
        const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
        const auto coordinateOffset = globalPoint - localPoint;
        globalPoint_ = globalPoint;
        OnTouchTestHit(coordinateOffset, touchRestrict, result);
        return true;
    } else if (isBubbleRect && bubbleRect.IsInRegion(globalPoint)) {
        LOGI("touch in bubble display");
        touchBubbleDisplay = true;

        // Calculates the local point location and coordinate offset in bubble.
        const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
        const auto coordinateOffset = globalPoint - localPoint;
        globalPoint_ = globalPoint;
        OnTouchTestHit(coordinateOffset, touchRestrict, result);
        return RenderNode::TouchTest(globalPoint, parentLocalPoint, touchRestrict, result);
    } else if (isPopupListRect && popupListRect.IsInRegion(globalPoint)) {
        LOGI("touch in popupList display");
        touchPopupListDisplay = true;

        // Calculates the local point location and coordinate offset in bubble.
        const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
        const auto coordinateOffset = globalPoint - localPoint;
        globalPoint_ = globalPoint;
        OnTouchTestHit(coordinateOffset, touchRestrict, result);
        return RenderNode::TouchTest(globalPoint, parentLocalPoint, touchRestrict, result);
    } else {
        LOGI("touch in other display");
        if (IsValidBubbleBox() && bubbleController_ && bubbleController_->IsRunning()) {
            bubbleController_->Finish();
        }
    }
    return false;
}

void RenderIndexer::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (touchRecognizer_) {
        touchRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(touchRecognizer_);
    }
}

void RenderIndexer::HandleTouched(const Offset& touchPosition)
{
    int32_t index = GetTouchedItemIndex(touchPosition);
    if (index > -1) {
        MoveSectionWithIndexer(index);
    }
}

int32_t RenderIndexer::GetTouchedItemIndex(const Offset& touchPosition)
{
    double position = touchPosition.GetY();
    if (position < paddingY_) {
        return -1;
    }

    if (NearZero(itemSizeRender_)) {
        LOGE("[indexer] Invalid Item size:%{public}lf", itemSizeRender_);
        return -1;
    }

    int32_t index = static_cast<int32_t>((position - paddingY_) / itemSizeRender_);
    LOGI("[indexer] GetTouchedItemIndex section index:%{public}d", index);
    return GetItemIndex(index);
}

void RenderIndexer::UpdateBubbleText()
{
    if (IsValidBubbleBox() && !bubbleBox_->GetChildren().empty()) {
        // update bubble border
        RefPtr<Decoration> decoration = bubbleBox_->GetBackDecoration();
        Border border = decoration->GetBorder();
        Radius radius;
        if (popupListEnabled_) {
            radius = Radius(Dimension(ZERO_RADIUS, DimensionUnit::VP));
        } else {
            radius = Radius(Dimension(BUBBLE_BOX_RADIUS, DimensionUnit::VP));
        }
        border.SetBottomLeftRadius(radius);
        border.SetBottomRightRadius(radius);
        decoration->SetBorder(border);

        auto text = AceType::DynamicCast<RenderText>(bubbleBox_->GetChildren().front());
        auto item = GetSpecificItem(focusedItem_);
        if (bubbleText_ && text && item) {
            bubbleText_->SetData(item->GetSectionText());
            text->Update(bubbleText_);
            text->PerformLayout();
            BeginBubbleAnimation();
        }
    }
}

void RenderIndexer::BuildBubbleAnimation()
{
    if (!IsValidBubbleBox()) {
        LOGI("bubble box is invalid");
        return;
    }
    if (!bubbleController_) {
        bubbleController_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    bubbleController_->ClearInterpolators();
    bubbleController_->ClearAllListeners();
    auto weak = AceType::WeakClaim(this);
    bubbleController_->AddStopListener([weak]() {
        auto indexer = weak.Upgrade();
        if (!indexer) {
            return;
        }

        if (indexer->IsValidBubbleBox()) {
            indexer->bubbleDisplay_->UpdateOpacity(ZERO_OPACITY);
        }

        if (indexer->IsValidPopupList()) {
            indexer->popupListDisplay_->UpdateOpacity(ZERO_OPACITY);
        }
    });

    // build and start animation
    auto animation = AceType::MakeRefPtr<KeyframeAnimation<uint8_t>>();
    auto startFrame = AceType::MakeRefPtr<Keyframe<uint8_t>>(KEYFRAME_BEGIN, DEFAULT_OPACITY);
    auto midFrame = AceType::MakeRefPtr<Keyframe<uint8_t>>(KEYFRAME_HALF, DEFAULT_OPACITY);
    auto endFrame = AceType::MakeRefPtr<Keyframe<uint8_t>>(KEYFRAME_END, ZERO_OPACITY);
    midFrame->SetCurve(Curves::DECELE);
    endFrame->SetCurve(Curves::DECELE);
    animation->AddKeyframe(startFrame);
    animation->AddKeyframe(midFrame);
    animation->AddKeyframe(endFrame);
    animation->AddListener([weak](uint8_t value) {
        auto indexer = weak.Upgrade();
        if (!indexer) {
            LOGW("indexer error %s", AceType::TypeName(indexer));
            return;
        }

        if (indexer->IsValidBubbleBox()) {
            indexer->bubbleDisplay_->UpdateOpacity(value);
        }

        // add popup list animation
        if (indexer->IsValidPopupList()) {
            indexer->popupListDisplay_->UpdateOpacity(value);
        }
    });

    bubbleController_->AddInterpolator(animation);
    bubbleController_->SetDuration(INDEXER_BUBBLE_ANIMATION_DURATION);
}

void RenderIndexer::BeginBubbleAnimation()
{
    if (!IsValidBubbleBox()) {
        LOGI("bubble box is invalid");
        return;
    }
    if (!bubbleController_) {
        BuildBubbleAnimation();
    }
    if (bubbleController_->IsRunning()) {
        bubbleController_->Finish();
    }
    bubbleController_->Play();
}

bool RenderIndexer::IsValidBubbleBox()
{
    if (!bubbleEnabled_ || GetChildren().empty()) {
        return false;
    }
    if (!bubbleDisplay_) {
        bubbleDisplay_ = AceType::DynamicCast<RenderDisplay>(GetChildren().front());
        if (!bubbleDisplay_ || bubbleDisplay_->GetChildren().empty()) {
            return false;
        }
        bubbleBox_ = AceType::DynamicCast<RenderBox>(bubbleDisplay_->GetChildren().front());
        if (!bubbleBox_) {
            return false;
        }
    }
    return true;
}

bool RenderIndexer::IsValidPopupList()
{
    if (!bubbleEnabled_ || !popupListEnabled_ || GetChildren().empty()) {
        return false;
    }
    if (!popupListDisplay_) {
        popupListDisplay_ = AceType::DynamicCast<RenderDisplay>(GetChildren().back());
        if (!popupListDisplay_ || popupListDisplay_->GetChildren().empty()) {
            return false;
        }
        popupList_ = AceType::DynamicCast<RenderPopupList>(popupListDisplay_->GetChildren().front());
        if (!popupList_) {
            return false;
        }
    }
    return true;
}

int32_t RenderIndexer::GetItemIndex(int32_t index)
{
    if (items_.empty()) {
        return -1;
    }
    int32_t itemIndexInList = -1;
    RefPtr<RenderNode> itemNode;
    auto iter = items_.begin();
    std::advance(iter, index);
    if (iter == items_.end()) {
        itemNode = items_.back();
    } else {
        itemNode = *iter;
    }
    RefPtr<RenderIndexerItem> indexerItem = AceType::DynamicCast<RenderIndexerItem>(itemNode);
    if (indexerItem) {
        itemIndexInList = indexerItem->GetSectionIndex();
    }
    LOGI("[indexer] GetItemIndex index:%{public}d indexInList:%{public}d", index, itemIndexInList);
    return itemIndexInList;
}

void RenderIndexer::HandleFocusAnimation(const Size& size, const Offset& offset)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Pipeline context upgrade fail!");
        return;
    }
    if (!context->GetRenderFocusAnimation()) {
        LOGE("focusAnimation is null!");
        return;
    }

    double focusPadding = NormalizeToPx(FOCUS_PADDING);
    context->ShowFocusAnimation(RRect::MakeRRect(Rect(Offset(), size - Size(focusPadding, focusPadding) * PADDIN_SIZE),
                                    focusPadding, focusPadding),
        Color::BLUE, offset + Offset(focusPadding, focusPadding));
}

void RenderIndexer::MoveSectionWithIndexer(int32_t curSection)
{
    if (focusedItem_ == curSection) {
        LOGI("Current focused item already is:%{public}d", curSection);

        // click the same letter multiple times.
        if (bubbleController_ && !bubbleController_->IsRunning()) {
            bubbleController_->Play();
        }
        return;
    }

    RefPtr<RenderIndexerItem> curItem = GetSpecificItem(curSection);
    if (!curItem || !NeedProcess(curItem)) {
        LOGW("[indexer] invalid indexer item");
        return;
    }

    // change to correct index
    BeginFocusAnimation(focusedItem_, curSection);
    curItem->SetClicked(true); // Make cur clicked item focus.
    UpdateCurrentSectionItem(curSection);

    // Make pre focused item blur.
    RefPtr<RenderIndexerItem> preItem = GetSpecificItem(focusedItem_);
    if (preItem) {
        preItem->SetClicked(false);
    }
    focusedItem_ = curSection;
    MoveList(curItem->GetSectionIndex());
    UpdateBubbleText();
}

void RenderIndexer::MoveList(int32_t index)
{
    if (index < 0) {
        LOGE("[indexer] invalid item indexer");
        return;
    }

    // trigger onSelected Method
    OnSelected(index);

    // trigger onRequestPopupData Method
    OnRequestPopupData(index);
}

RefPtr<RenderIndexerItem> RenderIndexer::GetSpecificItem(int32_t index) const
{
    auto iter = items_.begin();
    std::advance(iter, index);
    if (iter != items_.end()) {
        return AceType::DynamicCast<RenderIndexerItem>(*iter);
    }
    return nullptr;
}

void RenderIndexer::OnSelected(int32_t selected) const
{
    if (selectedEventFun_) {
        auto event = std::make_shared<IndexerEventInfo>(selected);
        if (event) {
            selectedEventFun_(event);
        }
    }
}

void RenderIndexer::OnRequestPopupData(int32_t selected)
{
    if (requestPopupDataEventFun_) {
        auto event = std::make_shared<IndexerEventInfo>(selected);
        if (event) {
            auto popupData = requestPopupDataEventFun_(event);
            if (popupList_) {
                popupList_->OnRequestPopupDataSelected(popupData);
            }

            // switch bubble style
            if (popupData.size() == 0) {
                popupListEnabled_ = false;
            } else {
                popupListEnabled_ = true;
            }
        }
    }
}

bool RenderIndexer::GetBubbleRect(Rect& rect)
{
    if (!IsValidBubbleBox()) {
        return false;
    }

    Offset bubbleOffset = bubbleDisplay_->GetGlobalOffset();
    Size bubbleSize = bubbleDisplay_->GetLayoutSize();

    rect.SetOffset(bubbleOffset);
    rect.SetSize(bubbleSize);
    return true;
}

bool RenderIndexer::GetPopupListRect(Rect& rect)
{
    if (!IsValidPopupList()) {
        return false;
    }
    Offset popupListOffset = popupListDisplay_->GetGlobalOffset();
    Size popupListSize = popupListDisplay_->GetLayoutSize();

    rect.SetOffset(popupListOffset);
    rect.SetSize(popupListSize);
    return true;
}
} // namespace OHOS::Ace::V2
