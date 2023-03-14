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

#include "core/components/scroll/render_single_child_scroll.h"

#include "core/common/text_field_manager.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t MAX_CHILD_SIZE = 1;

} // namespace

void RenderSingleChildScroll::Update(const RefPtr<Component>& component)
{
    RefPtr<ScrollComponent> scroll = AceType::DynamicCast<ScrollComponent>(component);
    if (!scroll) {
        return;
    }

    enable_ = scroll->GetEnable();

    auto axis = scroll->GetAxisDirection();
    if (axis_ != axis) {
        axis_ = axis;
        ResetScrollable();
        InitScrollBarProxy();
    }
    padding_ = scroll->GetPadding();
    scrollPage_ = scroll->GetScrollPage();

    positionController_ = scroll->GetScrollPositionController();
    if (positionController_) {
        positionController_->SetScrollNode(AceType::WeakClaim(this));
        positionController_->SetScrollEvent(ScrollEvent::SCROLL_TOP,
            AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(scroll->GetOnScrollEdge(), GetContext()));
        positionController_->SetScrollEvent(ScrollEvent::SCROLL_EDGE,
            AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(scroll->GetOnScrollEdge(), GetContext()));
        positionController_->SetScrollEvent(ScrollEvent::SCROLL_END,
            AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(scroll->GetOnScrollEnd(), GetContext()));
        positionController_->SetScrollEvent(ScrollEvent::SCROLL_POSITION,
            AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(scroll->GetOnScroll(), GetContext()));
        positionController_->SetScrollNode(AceType::WeakClaim(this));
        LOGD("initial position: %{public}lf, %{public}lf", currentOffset_.GetX(), currentOffset_.GetY());
    }
    // In dialog, scroll is not takeBoundary, use this flag to determine.
    TakeBoundary(scroll->IsTakeBoundary());

    auto scrollBar = scroll->GetScrollBar();
    InitScrollBar(scrollBar);

    // This should be put after setting positionController_.
    RenderScroll::Update(component);
    UpdateAccessibilityAttr();

    // Update edge effect.
    isEffectSetted_ = scroll->IsEffectSetted();
    auto newEffect = scroll->GetScrollEffect();
    if (scrollEffect_ != newEffect) {
        scrollEffect_ = newEffect;
        if (scrollEffect_) {
            ResetEdgeEffect();
        }
    }
}

LayoutParam RenderSingleChildScroll::MakeInnerLayoutParam() const
{
    LayoutParam layout;
    if (!enable_) {
        layout.SetMaxSize(Size(viewPort_.Width(), viewPort_.Height()));
    } else if (axis_ == Axis::VERTICAL) {
        layout.SetMaxSize(Size(viewPort_.Width(), layout.GetMaxSize().Height()));
    } else {
        layout.SetMaxSize(Size(layout.GetMaxSize().Width(), viewPort_.Height()));
    }
    return layout;
}

bool RenderSingleChildScroll::IsUseOnly()
{
    return true;
}

bool RenderSingleChildScroll::CalculateMainScrollExtent(const Size& itemSize)
{
    bool isScrollable = false;
    if (axis_ == Axis::VERTICAL) {
        mainScrollExtent_ = itemSize.Height() + NormalizeToPx(padding_.Top()) + NormalizeToPx(padding_.Bottom());
        if (mainScrollExtent_ > viewPort_.Height()) {
            isScrollable = true;
        }
    } else {
        mainScrollExtent_ = itemSize.Width() + NormalizeToPx(padding_.Left()) + NormalizeToPx(padding_.Right());
        if (mainScrollExtent_ > viewPort_.Width()) {
            isScrollable = true;
        }
    }

    // If not scrollable, reset scrollable_ to null.
    if (!isScrollable) {
        if (scrollable_) {
            scrollable_->MarkAvailable(false);
            if (scrollable_->Idle() && GetMainOffset(currentOffset_) > 0.0) {
                LOGD("jump to top");
                JumpToPosition(0.0);
            }
        }
    } else {
        if (scrollable_ && scrollable_->Available()) {
            if (scrollable_->Idle() && GetMainOffset(currentOffset_) > mainScrollExtent_ - GetMainSize(viewPort_)) {
                // scroll to bottom
                LOGD("jump to bottom");
                JumpToPosition(mainScrollExtent_ - GetMainSize(viewPort_));
            }
        } else {
            if (scrollable_) {
                scrollable_->MarkAvailable(true);
            }
        }
    }

    if (scrollBar_) {
        scrollBar_->SetScrollable(isScrollable);
    }

    return isScrollable;
}

void RenderSingleChildScroll::MoveChildToViewPort(
    const Size& size, const Offset& childOffset, const Offset& effectOffset)
{
    LOGD("MoveChildToViewPort %{public}s %{public}s", size.ToString().c_str(), childOffset.ToString().c_str());
    auto selfOffset = GetGlobalOffset();
    auto itemActualRect = Rect(childOffset, size);
    auto viewRect = Rect(selfOffset, viewPort_);

    // rect is in viewport
    if (itemActualRect.IsWrappedBy(viewRect)) {
        return;
    }

    double childPosition = GetMainOffset(childOffset);
    double viewMin = GetMainOffset(selfOffset);
    double viewMax = GetMainOffset(selfOffset + viewPort_);
    double effectSize = GetMainOffset(effectOffset);
    double childSize = GetMainSize(size);
    double viewPortSize = GetMainSize(viewPort_);

    double moveDelta = 0.0;
    if (viewPortSize <= childSize) {
        return;
    }

    if (childPosition < viewMin) {
        moveDelta = childPosition - viewMin - effectSize;
    } else if (childPosition + childSize > viewMax) {
        moveDelta = childPosition + childSize + effectSize - viewMax;
    }
    JumpToPosition(GetCurrentPosition() + moveDelta);
}

void RenderSingleChildScroll::PerformLayout()
{
    if (GetChildren().size() != MAX_CHILD_SIZE) {
        LOGE("render Scroll perform layout with %{public}zu children", GetChildren().size());
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }

    viewPort_ = GetLayoutParam().GetMaxSize() > viewPort_ ? viewPort_ : GetLayoutParam().GetMaxSize();

    Size paddingSize = padding_.GetLayoutSizeInPx(context->GetDipScale());
    Offset paddingOffset = padding_.GetOffsetInPx(context->GetDipScale());

    auto child = GetChildren().front();

    LayoutParam layout;
    layout = MakeInnerLayoutParam();
    child->Layout(layout);

    // Get layout result of child.
    Size itemSize = child->GetLayoutSize();
    // Calculate with padding.
    if (!NearZero(paddingSize.Width()) || !NearZero(paddingSize.Height())) {
        layout.SetFixedSize(itemSize - paddingSize);
        // Layout again with new param.
        child->Layout(layout);
    }
    itemSize = child->GetLayoutSize();
    LOGD("child size after padding: %{public}lf, %{public}lf", itemSize.Width(), itemSize.Height());
    auto currentChildMainSize = GetMainSize(child->GetLayoutSize());
    // Mark need force layout with parent if child size changed in semi and dialog window modal.
    if (!NearEqual(childLastMainSize_, -std::numeric_limits<double>::max()) &&
        !NearEqual(currentChildMainSize, childLastMainSize_) && !context->IsFullScreenModal()) {
        PostForceMakeNeedLayout();
    }
    childLastMainSize_ = currentChildMainSize;

    SetLayoutSize(GetLayoutParam().Constrain(itemSize > viewPort_ ? viewPort_ : itemSize));

    auto textFieldManager = AceType::DynamicCast<TextFieldManager>(context->GetTextFieldManager());
    if (textFieldManager && moveStatus_.first && axis_ == Axis::VERTICAL) {
        moveDistance_ = textFieldManager->GetClickPosition().GetY() - viewPort_.Height();
        currentOffset_.SetY(moveDistance_);
        moveStatus_.first = false;
    }

    if (textFieldManager && moveStatus_.second && !moveStatus_.first && axis_ == Axis::VERTICAL) {
        currentOffset_.SetY(0 - moveDistance_);
        moveStatus_.second = false;
        moveDistance_ = 0;
    }
    // Get main direction scrollable extent.
    bool isScrollable = CalculateMainScrollExtent(itemSize);
    scrollBarExtent_ = mainScrollExtent_;
    if (isScrollable) {
        ValidateOffset(SCROLL_FROM_NONE);
    } else {
        currentOffset_ = Offset::Zero();
    }
    child->SetPosition(Offset::Zero() - currentOffset_ + paddingOffset);
    LOGD("child position:%{public}s", child->GetPosition().ToString().c_str());

    currentBottomOffset_ = axis_ == Axis::VERTICAL ? currentOffset_ + Offset(0.0, viewPort_.Height())
                                                   : currentOffset_ + Offset(viewPort_.Width(), 0.0);
}

void RenderSingleChildScroll::PostForceMakeNeedLayout()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = AceType::WeakClaim(this)] {
            auto scroll = weak.Upgrade();
            if (!scroll) {
                return;
            }
            scroll->MarkNeedLayout(false, true);
        },
        TaskExecutor::TaskType::UI);
}

void RenderSingleChildScroll::UpdateAccessibilityAttr()
{
    auto refPtr = accessibilityNode_.Upgrade();
    if (!refPtr) {
        LOGW("Get accessibility node failed.");
        return;
    }
    refPtr->SetScrollableState(true);
    refPtr->SetActionScrollForward([weakScroll = AceType::WeakClaim(this)]() {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            LOGI("Trigger ScrollForward by Accessibility.");
            scroll->ScrollPage(false, true);
            return true;
        }
        return false;
    });
    refPtr->SetActionScrollBackward([weakScroll = AceType::WeakClaim(this)]() {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            LOGI("Trigger ScrollBackward by Accessibility.");
            scroll->ScrollPage(true, true);
            return true;
        }
        return false;
    });
}

void RenderSingleChildScroll::AdjustTouchRestrict(TouchRestrict& touchRestrict)
{
    // If edge effect is setted, do not adjust touch restrict.
    if (isEffectSetted_) {
        return;
    }

    if (currentOffset_.IsZero()) {
        if (axis_ == Axis::VERTICAL) {
            touchRestrict.forbiddenType |= TouchRestrict::SWIPE_DOWN;
        } else {
            touchRestrict.forbiddenType |= TouchRestrict::SWIPE_RIGHT;
        }
    }
}

} // namespace OHOS::Ace
