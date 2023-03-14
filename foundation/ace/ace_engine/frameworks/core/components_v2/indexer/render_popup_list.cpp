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

#include "core/components_v2/indexer/render_popup_list.h"

#include "base/log/log.h"
#include "core/animation/bilateral_spring_node.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/scroll/render_scroll.h"
#include "core/components/scroll/render_single_child_scroll.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "core/components_v2/indexer/popup_list_component.h"
#include "core/components_v2/indexer/popup_list_item_component.h"

namespace OHOS::Ace::V2 {
constexpr double VIEW_PORT_SCALE = 3.0;

void RenderPopupList::Update(const RefPtr<Component>& component)
{
    component_ = AceType::DynamicCast<PopupListComponent>(component);
    ACE_DCHECK(component_);

    boxComponent_ = AceType::DynamicCast<BoxComponent>(component_->GetChildren().front());
    ACE_DCHECK(boxComponent_);

    popupSelectedEventFun_ = AceAsyncEvent<void(const std::shared_ptr<IndexerEventInfo>&)>::
        Create(component_->GetPopupSelectedEvent(), context_);

    auto axis = component_->GetDirection();

    if (scrollable_) {
        scrollable_->SetAxis(axis);
    } else {
        auto callback = [weak = AceType::WeakClaim(this)](double offset, int32_t source) {
            auto renderPopupList = weak.Upgrade();
            if (!renderPopupList) {
                return false;
            }

            Offset delta;
            delta.SetX(0.0);
            delta.SetY(offset);

            renderPopupList->AdjustOffset(delta, source);
            return renderPopupList->UpdateScrollPosition(delta.GetY(), source);
        };
        scrollable_ = AceType::MakeRefPtr<Scrollable>(callback, axis);
        scrollable_->Initialize(context_);
    }

    // only support spring
    if (component_->GetEdgeEffect() == EdgeEffect::SPRING) {
        if (!scrollEffect_ || scrollEffect_->GetEdgeEffect() != EdgeEffect::SPRING) {
            scrollEffect_ = AceType::MakeRefPtr<ScrollSpringEffect>();
            ResetEdgeEffect();
        }
    } else {
        scrollEffect_ = nullptr;
    }

    MarkNeedLayout();
}

void RenderPopupList::PerformLayout()
{
    // Check validation of layout size
    if (NearZero(viewPortWidth_) || NearZero(viewPortHeight_)) {
        LOGW("Cannot layout using invalid view port");
        return;
    }

    const auto maxSize = Size(viewPortWidth_, viewPortHeight_);
    const auto minSize = GetLayoutParam().GetMinSize();
    const auto innerLayout = LayoutParam(maxSize, minSize);
    double curMainPos = LayoutOrRecycleCurrentItems(innerLayout);

    for (size_t newIndex = startIndex_ + items_.size(); curMainPos < endMainPos_; ++newIndex) {
        auto child = RequestAndLayoutNewItem(newIndex, innerLayout);
        if (!child) {
            startIndex_ = std::min(startIndex_, datas_.size());
            break;
        }
        curMainPos += child->GetLayoutSize().Height();
    }

    bool noEdgeEffect = (scrollable_ && scrollable_->IsAnimationNotRunning()) || !scrollEffect_;
    // Check if reach the end of popup list
    reachEnd_ = LessOrEqual(curMainPos, viewPortHeight_);
    if (noEdgeEffect && reachEnd_) {
        // Adjust end of popup list to match the end of layout
        currentOffset_ += viewPortHeight_ - curMainPos;
        curMainPos = viewPortHeight_;
    }

    // Try to request new items at start if needed
    for (; currentOffset_ > startMainPos_ && startIndex_ > 0; --startIndex_) {
        auto child = RequestAndLayoutNewItem(startIndex_ - 1, innerLayout);
        if (!child) {
            break;
        }
        currentOffset_ -= child->GetLayoutSize().Height();
    }

    // Check if reach the start of list
    reachStart_ = GreatOrEqual(currentOffset_, 0.0);
    if (noEdgeEffect && reachStart_) {
        curMainPos -= currentOffset_;
        currentOffset_ = 0;
    }

    // Check if disable or enable scrollable
    CalculateMainScrollExtent(curMainPos);

    // Set position for each child
    SetItemsPosition();

    // Set layout size of popup list component itself
    SetLayoutSize(GetLayoutParam().Constrain(Size(viewPortWidth_, viewPortHeight_)));
}

bool RenderPopupList::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (GetDisableTouchEvent() || disabled_) {
        return false;
    }

    // calculate touch point in which item.
    CalTouchPoint(globalPoint, popupSelected_);

    // call js function
    OnPopupSelected(popupSelected_);

    // Calculates the local point location and coordinate offset in this node.
    const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
    const auto coordinateOffset = globalPoint - localPoint;
    globalPoint_ = globalPoint;
    OnTouchTestHit(coordinateOffset, touchRestrict, result);

    return true;
}

void RenderPopupList::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!GetVisible()) {
        return;
    }

    // Disable scroll while expand all items
    if (!scrollable_ || !scrollable_->Available()) {
        LOGE("[popup list] OnTouchTestHit scrollable_ not available");
        return;
    }

    scrollable_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(scrollable_);
}

void RenderPopupList::OnRequestPopupDataSelected(std::vector<std::string>& data)
{
    if (!GetChildren().empty()) {
        ClearChildren();
    }
    items_.clear();
    datas_.clear();

    // load string data
    datas_.assign(data.begin(), data.end());

    // calculate view port size
    viewPortWidth_ = SystemProperties::Vp2Px(POPUP_BOX_SIZE);
    viewPortHeight_ = ApplyLayoutParam();

    LOGI("size of datas %{public}zu view port height %{public}.2f ", datas_.size(), viewPortHeight_);

    // create render node
    if (data.size() == 0) {
        boxComponent_->SetWidth(POPUP_ZERO_SIZE);
        boxComponent_->SetHeight(POPUP_ZERO_SIZE);
    } else {
        boxComponent_->SetWidth(viewPortWidth_);
        boxComponent_->SetHeight(viewPortHeight_);
    }
    if (!renderBox_) {
        renderBox_ = AceType::DynamicCast<RenderBox>(RenderBox::Create());
    }
    AddChild(renderBox_);
    renderBox_->Attach(GetContext());
    renderBox_->Update(boxComponent_);
    renderBox_->PerformLayout();
    renderBox_->MarkNeedRender();
    MarkNeedLayout();
}

void RenderPopupList::OnPopupSelected(int32_t selected) const
{
    if (popupSelectedEventFun_) {
        auto event = std::make_shared<IndexerEventInfo>(selected);
        if (event) {
            popupSelectedEventFun_(event);
        }
    }
}

void RenderPopupList::CalTouchPoint(const Point& globalPoint, int32_t& selected)
{
    selected = INVALID_POPUP_SELECTED;
    std::list<RefPtr<RenderPopupListItem>>::iterator iter;
    int32_t index = 0;
    for (iter = items_.begin(); iter != items_.end(); ++iter) {
        auto renderItem = *iter;
        std::unique_ptr<Rect> rect = std::make_unique<Rect>(renderItem->GetGlobalOffset(),
                                        renderItem->GetLayoutSize());
        if (rect->IsInRegion(globalPoint)) {
            selected = index + static_cast<int32_t>(startIndex_);
            renderItem->UpdateBoxSelected();
        } else {
            renderItem->UpdateBoxNormal();
        }
        index++;
    }
}

double RenderPopupList::ApplyLayoutParam()
{
    double viewPortSize = 0.0;
    if (datas_.size() > POPUP_ITEM_VIEW_MAX_COUNT) {
        viewPortSize = SystemProperties::Vp2Px(POPUP_BOX_SIZE) * POPUP_ITEM_VIEW_MAX_COUNT + POPUP_BORDER_RADIUS_SIZE;
    } else {
        viewPortSize = SystemProperties::Vp2Px(POPUP_BOX_SIZE) * datas_.size() + POPUP_BORDER_RADIUS_SIZE;
    }

    // start position and end position of scrollable
    startMainPos_ = -viewPortSize;
    endMainPos_ = startMainPos_ + (viewPortSize * VIEW_PORT_SCALE);

    return viewPortSize;
}

double RenderPopupList::LayoutOrRecycleCurrentItems(const LayoutParam& layoutParam)
{
    double curMainPos = currentOffset_;
    size_t curIndex = startIndex_;
    for (auto it = items_.begin(); it != items_.end(); ++curIndex) {
        const auto& child = *(it);
        if (LessOrEqual(curMainPos, endMainPos_)) {
            child->Layout(layoutParam);
            curMainPos += child->GetLayoutSize().Height();
            if (GreatOrEqual(curMainPos, startMainPos_)) {
                ++it;
                continue;
            }
            currentOffset_ = curMainPos;
            startIndex_ = curIndex + 1;
        }

        it = items_.erase(it);
    }
    return curMainPos;
}

RefPtr<RenderPopupListItem> RenderPopupList::RequestAndLayoutNewItem(size_t index, const LayoutParam& layoutParam)
{
    if (index >= datas_.size()) {
        return nullptr;
    }

    RefPtr<PopupListItemComponent> itemComponent = AceType::MakeRefPtr<PopupListItemComponent>(datas_[index]);
    RefPtr<RenderPopupListItem> renderItem = AceType::DynamicCast<RenderPopupListItem>(RenderPopupListItem::Create());
    AddChild(renderItem);
    if (renderItem) {
        renderItem->Attach(GetContext());
        renderItem->Update(itemComponent);
        renderItem->Layout(layoutParam);
        if (index < startIndex_) {
            items_.emplace_front(renderItem);
        } else {
            items_.emplace_back(renderItem);
        }
    }

    return renderItem;
}

void RenderPopupList::CalculateMainScrollExtent(double curMainPos)
{
    // check current is out of boundary
    isOutOfBoundary_ = LessNotEqual(curMainPos, viewPortHeight_) || GreatNotEqual(currentOffset_, 0.0);
    // content length
    mainScrollExtent_ = curMainPos - currentOffset_;
    // disable scroll when content length less than mainSize
    if (scrollable_) {
        scrollable_->MarkAvailable(GreatOrEqual(mainScrollExtent_, viewPortHeight_));
    }
}

void RenderPopupList::SetItemsPosition()
{
    double curMainPos = currentOffset_;

    for (const auto& child : items_) {
        double childHeight = child->GetLayoutSize().Height();
        child->SetPosition(Offset(0.0, curMainPos));
        curMainPos += childHeight;
    }
}

double RenderPopupList::GetCurrentPosition() const
{
    return currentOffset_;
}

bool RenderPopupList::IsOutOfBoundary() const
{
    return isOutOfBoundary_;
}

void RenderPopupList::AdjustOffset(Offset& delta, int32_t source)
{
    // when scrollEffect equal to none, no need to adjust offset
    if (!scrollEffect_) {
        return;
    }

    if (delta.IsZero() || source == SCROLL_FROM_ANIMATION || source == SCROLL_FROM_ANIMATION_SPRING) {
        return;
    }

    double offset = delta.GetY();
    if (NearZero(viewPortHeight_) || NearZero(offset)) {
        return;
    }

    double maxScrollExtent = mainScrollExtent_ - viewPortHeight_;
    double overscrollPastStart = 0.0;
    double overscrollPastEnd = 0.0;
    double overscrollPast = 0.0;
    bool easing = false;

    overscrollPastStart = std::max(GetCurrentPosition(), 0.0);
    overscrollPastEnd = std::max(-GetCurrentPosition() - maxScrollExtent, 0.0);
    // do not adjust offset if direction oppsite from the overScroll direction when out of boundary
    if ((overscrollPastStart > 0.0 && offset < 0.0) || (overscrollPastEnd > 0.0 && offset > 0.0)) {
        return;
    }
    easing = (overscrollPastStart > 0.0 && offset > 0.0) || (overscrollPastEnd > 0.0 && offset < 0.0);

    overscrollPast = std::max(overscrollPastStart, overscrollPastEnd);
    double friction = easing ? RenderScroll::CalculateFriction((overscrollPast - std::abs(offset)) / viewPortHeight_)
                             : RenderScroll::CalculateFriction(overscrollPast / viewPortHeight_);
    double direction = offset / std::abs(offset);
    offset = direction * RenderScroll::CalculateOffsetByFriction(overscrollPast, std::abs(offset), friction);
    // set delta after adjust
    delta.SetY(offset);
}

bool RenderPopupList::UpdateScrollPosition(double offset, int32_t source)
{
    if (source == SCROLL_FROM_START) {
        return true;
    }

    if (NearZero(offset)) {
        return true;
    }

    if (reachStart_ && reachEnd_) {
        return false;
    }

    if (offset > 0.0) {
        if (reachStart_ && !scrollEffect_) {
            return false;
        }
        reachEnd_ = false;
    } else {
        if (reachEnd_ && !scrollEffect_) {
            return false;
        }
        reachStart_ = false;
    }

    currentOffset_ += offset;
    MarkNeedLayout(true);
    return true;
}

void RenderPopupList::ResetEdgeEffect()
{
    if (!scrollEffect_) {
        LOGE("ResetEdgeEffect failed, scrollEffect_ is nullptr");
        return;
    }

    scrollEffect_->SetCurrentPositionCallback([weak = AceType::WeakClaim(this)]() {
        auto popuplist = weak.Upgrade();
        if (popuplist) {
            return popuplist->GetCurrentPosition();
        }
        return 0.0;
    });
    scrollEffect_->SetLeadingCallback([weak = AceType::WeakClaim(this)]() {
        auto popuplist = weak.Upgrade();
        if (popuplist) {
            return popuplist->GetLayoutSize().Height() - popuplist->mainScrollExtent_;
        }
        return 0.0;
    });

    scrollEffect_->SetTrailingCallback([weak = AceType::WeakClaim(this)]() { return 0.0; });
    scrollEffect_->SetInitLeadingCallback([weak = AceType::WeakClaim(this)]() {
        auto popuplist = weak.Upgrade();
        if (popuplist) {
            return popuplist->GetLayoutSize().Height() - popuplist->mainScrollExtent_;
        }
        return 0.0;
    });
    scrollEffect_->SetInitTrailingCallback([weak = AceType::WeakClaim(this)]() { return 0.0; });
    scrollEffect_->SetScrollNode(AceType::WeakClaim(this));
    SetEdgeEffectAttribute();
    scrollEffect_->InitialEdgeEffect();
}

void RenderPopupList::SetEdgeEffectAttribute()
{
    if (scrollEffect_ && scrollable_) {
        scrollEffect_->SetScrollable(scrollable_);
        scrollEffect_->RegisterSpringCallback();
        if (scrollEffect_->IsSpringEffect()) {
            scrollable_->SetOutBoundaryCallback([weakScroll = AceType::WeakClaim(this)]() {
                auto scroll = weakScroll.Upgrade();
                if (scroll) {
                    return scroll->IsOutOfBoundary();
                }
                return false;
            });
        }
    }
}
} // namespace OHOS::Ace::V2
