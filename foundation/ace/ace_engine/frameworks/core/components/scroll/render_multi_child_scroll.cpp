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

#include "core/components/scroll/render_multi_child_scroll.h"

#include "core/animation/curve.h"
#include "core/animation/curve_animation.h"
#include "core/common/vibrator/vibrator_proxy.h"
#include "core/components/box/render_box.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr double NONE_SCROLL_POSITION = -1.0;
constexpr double MIN_EXTENT = 5.0;
constexpr double EXTENT_RATIO = 2.0;
constexpr int32_t ANIMATION_DURATION = 200;
constexpr double LIST_INCONTINUOUS_ROTATION_SENSITYVITY_NORMAL = 0.6;
constexpr double LIST_CONTINUOUS_ROTATION_SENSITYVITY_NORMAL = 1.0;
constexpr double LIST_ITEMCENTER_ROTATION_THRESHOLD = 0.7;
constexpr int32_t COMPATIBLE_VERSION = 6;

#ifdef WEARABLE_PRODUCT
const std::string& VIBRATOR_TYPE_WATCH_CROWN_STRENGTH3 = "watchhaptic.crown.strength3";
#endif

} // namespace

void RenderMultiChildScroll::AddChild(const RefPtr<RenderList>& child)
{
    RenderNode::AddChild(child);
}

LayoutParam RenderMultiChildScroll::MakeInnerLayoutParam() const
{
    LayoutParam layout;
    if (axis_ == Axis::VERTICAL) {
        layout.SetMinSize(Size(viewPort_.Width(), layout.GetMinSize().Height()));
        layout.SetMaxSize(Size(viewPort_.Width(), layout.GetMaxSize().Height()));
    } else {
        layout.SetMinSize(Size(layout.GetMinSize().Width(), viewPort_.Height()));
        layout.SetMaxSize(Size(layout.GetMaxSize().Width(), viewPort_.Height()));
    }
    return layout;
}

void RenderMultiChildScroll::ProcessScrollExtent()
{
    // When the scrollBar is off, return
    if (!scrollBar_ || !scrollBar_->NeedScrollBar()) {
        return;
    }

    if (NearEqual(scrollBarExtent_, 0.0)) {
        scrollBarExtent_ = mainScrollExtent_;
        return;
    }

    if (mainScrollExtent_ - scrollBarExtent_ > MIN_EXTENT && !scrollBarExtentFlag_) {
        LOGD("ScrollBar is animating.");
        scrollBarExtentFlag_ = true;
        auto animation =
            AceType::MakeRefPtr<CurveAnimation<double>>(scrollBarExtent_, mainScrollExtent_, Curves::SHARP);
        animation->AddListener([weakScroll = AceType::WeakClaim(this)](double value) {
            auto scroll = weakScroll.Upgrade();
            if (scroll) {
                scroll->SetMainScrollExtentForBar(value);
                scroll->MarkNeedLayout(true);
            }
        });
        if (animateController_) {
            animateController_->ClearInterpolators();
            animateController_->SetDuration(ANIMATION_DURATION);

            // add the new animation
            animateController_->AddInterpolator(animation);
            animateController_->Play();
        }
    } else if (!scrollBarExtentFlag_) {
        scrollBarExtent_ = mainScrollExtent_;
    }
}

bool RenderMultiChildScroll::CalculateMainScrollExtent()
{
    Size itemSize; // Calculate all children layout size.
    for (const auto& child : GetChildren()) {
        itemSize += child->GetLayoutSize();
    }

    bool isScrollable = false;
    if (axis_ == Axis::VERTICAL) {
        double paddingVertical = NormalizeToPx(padding_.Top()) + NormalizeToPx(padding_.Bottom());
        mainScrollExtent_ = itemSize.Height() + paddingVertical + outBoundaryExtent_;
        ProcessScrollExtent();
        if (mainScrollExtent_ > viewPort_.Height()) {
            isScrollable = true;
        }
    } else {
        double paddingHorizontal = NormalizeToPx(padding_.Left()) + NormalizeToPx(padding_.Right());
        mainScrollExtent_ = itemSize.Width() + paddingHorizontal + outBoundaryExtent_;
        scrollBarExtent_ = mainScrollExtent_;
        if (mainScrollExtent_ > viewPort_.Width()) {
            isScrollable = true;
        }
    }

    // If not scrollable, reset scrollable_ to null.
    if (!isScrollable) {
        if (scrollable_) {
            scrollable_->MarkAvailable(false);
            if (scrollable_->Idle() && GetMainOffset(currentOffset_) > 0.0) {
                scrollEffect_->ProcessScrollOver(0.0);
            }
        }
        if (scrollBar_) {
            scrollBar_->SetScrollable(false);
        }
        if (positionController_) {
            positionController_->SetNonScrollable();
        }
    } else {
        if (scrollable_ && scrollable_->Available()) {
            if (scrollable_->Idle() && GetMainOffset(currentOffset_) > mainScrollExtent_ - GetMainSize(viewPort_)) {
                // scroll to bottom
                scrollEffect_->ProcessScrollOver(0.0);
            }
        } else {
            if (scrollable_) {
                scrollable_->MarkAvailable(true);
            }
        }
        if (scrollBar_) {
            scrollBar_->SetScrollable(true);
        }
    }

    return isScrollable;
}

void RenderMultiChildScroll::JumpToIndex(int32_t index, int32_t source)
{
    if (GetChildren().empty()) {
        LOGE("no list in scroll");
        return;
    }
    auto listBase = AceType::DynamicCast<RenderList>(GetChildren().front());
    if (!listBase) {
        LOGE("no list to jump");
        return;
    }

    double position = listBase->CalculateItemPosition(index, ScrollType::SCROLL_INDEX);
    if (position < 0.0) {
        LOGE("no this index: %{public}d", index);
        return;
    }
    LOGI("jump to index:%{public}d position:%{public}lf", index, position);
    if (CalculateMainScrollExtent()) {
        RenderScroll::JumpToPosition(position, source);
    } else {
        LOGW("Current is not allow to jump index.");
    }
}

void RenderMultiChildScroll::JumpToPosition(double position, int32_t source)
{
    if (GetChildren().empty()) {
        LOGE("no list in scroll");
        return;
    }
    auto listBase = AceType::DynamicCast<RenderList>(GetChildren().front());
    if (!listBase) {
        LOGE("no list to jump");
        return;
    }
    listBase->CalculateItemPosition(position);
    LOGI("jump to position:%{public}lf", position);
    if (CalculateMainScrollExtent()) {
        RenderScroll::JumpToPosition(position, source);
    } else {
        LOGW("Current is not allow to jump position.");
    }
}

void RenderMultiChildScroll::UpdateEdgeEffect(const RefPtr<ListComponent>& listComponent)
{
    auto newEffect = listComponent->GetScrollEffect();
    if (scrollEffect_ != newEffect) {
        scrollEffect_ = newEffect;
        if (scrollEffect_) {
            ResetEdgeEffect();
        }
    }
}

void RenderMultiChildScroll::UpdateGradient(const RefPtr<ListComponent>& listComponent)
{
    gradientWidth_ = listComponent->GetGradientWidth();
    backgroundColor_ = listComponent->GetBackgroundColor();
}

void RenderMultiChildScroll::Update(const RefPtr<Component>& component)
{
    auto listComponent = AceType::DynamicCast<ListComponent>(component);
    if (!listComponent) {
        LOGE("component is not a ListComponent");
        return;
    }

    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("context is nullptr");
        return;
    }
    if (context->IsJsCard()) {
        cacheExtent_ = (std::numeric_limits<double>::max)();
    }

    scrollVibrate_ = listComponent->GetScrollVibrate();
    if (scrollVibrate_ && !vibrator_ && context) {
        vibrator_ = VibratorProxy::GetInstance().GetVibrator(context->GetTaskExecutor());
    }

    rotationVibrate_ = listComponent->IsRotationVibrate();
    if (rotationVibrate_ && !vibrator_ && context) {
        vibrator_ = VibratorProxy::GetInstance().GetVibrator(context->GetTaskExecutor());
    }

    if (listComponent->IsInRefresh()) {
        auto parent = GetParent().Upgrade();
        while (parent) {
            auto refresh = AceType::DynamicCast<RenderRefresh>(parent);
            if (refresh) {
                refreshParent_ = AceType::WeakClaim(AceType::RawPtr(refresh));
                break;
            }
            parent = parent->GetParent().Upgrade();
        }
    }

    auto axis = listComponent->GetDirection() == FlexDirection::COLUMN ? Axis::VERTICAL : Axis::HORIZONTAL;
    if (axis_ != axis) {
        axis_ = axis;
        ResetScrollable();
    }

    if (scrollable_) {
        scrollable_->SetOverSpringProperty(listComponent->OverSpringProperty());
        scrollable_->MarkNeedCenterFix(listComponent->GetSupportItemCenter());
    }

    // sync scrollpage from List child
    SetScrollPage(listComponent->GetScrollPage());

    // Update its child.
    auto children = GetChildren();
    if (!children.empty()) {
        auto listNode = children.front();
        if (listNode) {
            listNode->Update(component);
            listNode->Attach(GetContext());
        }
    }

    UpdateGradient(listComponent);
    UpdateEdgeEffect(listComponent);

    auto newController = listComponent->GetPositionController();
    if (positionController_ != newController) {
        positionController_ = newController;
        if (positionController_) {
            positionController_->SetScrollEvent(ScrollEvent::SCROLL_TOP,
                AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(listComponent->GetOnScrollTop(),
                GetContext()));
            positionController_->SetScrollEvent(ScrollEvent::SCROLL_BOTTOM,
                AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(listComponent->GetOnScrollBottom(),
                GetContext()));
            positionController_->SetScrollEvent(ScrollEvent::SCROLL_TOUCHUP,
                AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(listComponent->GetOnScrollTouchUp(),
                GetContext()));
            positionController_->SetScrollEvent(ScrollEvent::SCROLL_END,
                AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(listComponent->GetOnScrollEnd(),
                GetContext()));
            positionController_->SetScrollEvent(ScrollEvent::SCROLL_POSITION,
                AceAsyncEvent<void(std::shared_ptr<ScrollEventInfo>&)>::Create(listComponent->GetOnScroll(),
                GetContext()));
            positionController_->SetScrollNode(AceType::WeakClaim(this));
            LOGD("initial position: %{public}lf, %{public}lf", currentOffset_.GetX(), currentOffset_.GetY());
        }
    }
    if (positionController_) {
        initialIndex_ = positionController_->GetInitialIndex();
        initialOffset_ = positionController_->GetInitialOffset();
    }

    if (!animateController_) {
        animateController_ = AceType::MakeRefPtr<Animator>(GetContext());
        animateController_->AddStopListener([weakScroll = AceType::WeakClaim(this)]() {
            auto scroll = weakScroll.Upgrade();
            if (scroll) {
                scroll->scrollBarExtentFlag_ = false;
            }
        });
    }

    auto scrollBar = listComponent->GetScrollBar();
    InitScrollBar(scrollBar);
    rightToLeft_ = listComponent->GetRightToLeft();

    // This should be put after setting positionController_.
    RenderScroll::Update(component);
}

bool RenderMultiChildScroll::ReachMaxCount() const
{
    bool reached = true;
    for (const auto& child : GetChildren()) {
        auto listBase = AceType::DynamicCast<RenderList>(child);
        if (!listBase) {
            continue;
        }
        int32_t currentIndex = listBase->GetCurrentMaxIndex();
        int32_t maxCount = listBase->GetMaxCount();
        if (maxCount <= 0 || currentIndex != (maxCount - 1)) {
            reached = false;
            break;
        }
    }
    return reached;
}

void RenderMultiChildScroll::OnPredictLayout(int64_t deadline)
{
    int32_t childrenSize = static_cast<int32_t>(GetChildren().size());
    if (currentIndex_ < 0 || currentIndex_ >= childrenSize || childrenSize == 0) {
        LOGE("invalid current index: %{public}d, size is: %{public}d", currentIndex_, childrenSize);
        return;
    }
    // only in list widget enabled build next
    auto child = GetChildren().front();
    auto listBase = AceType::DynamicCast<RenderList>(child);
    if (listBase) {
        Offset lastOffset = Offset::Zero() - currentOffset_ - correctedDelta_;
        Offset curOffset = lastOffset;
        double mainOffset = GetMainOffset(curOffset);
        double mainExtent = GetMainSize(viewPort_);
        double layoutHead = 0.0;
        double layoutTail = mainExtent;
        if (IsRowReverse()) {
            layoutHead = layoutHead - cacheExtent_ + mainOffset;
            layoutTail = layoutTail + cacheExtent_ + mainOffset;
        } else {
            layoutHead = layoutHead - cacheExtent_ - mainOffset;
            layoutTail = layoutTail + cacheExtent_ - mainOffset;
        }
        listBase->BuildNextItem(layoutHead, layoutTail, curOffset, viewPort_);
    }
}

void RenderMultiChildScroll::LayoutChild(
    const RefPtr<RenderNode>& child, const Offset& position, double start, double end)
{
    auto listBase = AceType::DynamicCast<RenderList>(child);
    if (listBase) {
        listBase->ResetLayoutRange(start, end, position, viewPort_);
        listBase->SetLayoutParam(GetLayoutParam());
        listBase->SetNeedLayout(true);
        listBase->OnLayout();
    }
}

bool RenderMultiChildScroll::LayoutChild(
    const RefPtr<RenderNode>& curChild, int32_t curIndex, const RefPtr<RenderNode>& lastChild)
{
    Offset lastOffset = Offset::Zero() - currentOffset_;
    Size lastSize;
    if (lastChild) {
        lastOffset = lastChild->GetPosition();
        lastSize = lastChild->GetLayoutSize();
    }

    Offset curOffset = lastOffset + lastSize;
    double mainOffset = GetMainOffset(curOffset);
    double mainExtent = GetMainSize(viewPort_);

    // The following children are not visible.
    if (lastChild && mainOffset >= mainExtent + cacheExtent_) {
        return false;
    }

    // The last child become invisible, change the current index.
    if (mainOffset <= -cacheExtent_) {
        currentIndex_ = curIndex;
    }

    double layoutHead = 0.0;
    double layoutTail = mainExtent;
    if (IsRowReverse()) {
        layoutHead = layoutHead - cacheExtent_ + mainOffset;
        layoutTail = layoutTail + cacheExtent_ + mainOffset;
    } else {
        layoutHead = layoutHead - cacheExtent_ - mainOffset;
        layoutTail = layoutTail + cacheExtent_ - mainOffset;
    }
    LayoutChild(curChild, curOffset, layoutHead, layoutTail);

    return true;
}

void RenderMultiChildScroll::LayoutChild()
{
    int32_t childrenSize = static_cast<int32_t>(GetChildren().size());
    if (currentIndex_ < 0 || currentIndex_ >= childrenSize) {
        LOGE("invalid current index: %{public}d", currentIndex_);
        return;
    }

    // currentIndex_ is 0 at the beginning.
    int32_t currentIndex = 0;
    RefPtr<RenderNode> lastChild;
    for (const auto& child : GetChildren()) {
        if (currentIndex >= currentIndex_) {
            if (!LayoutChild(child, currentIndex, lastChild)) {
                LOGE("layout child failed, index:%{public}d", currentIndex);
                break;
            }
            lastChild = child;
        }
        currentIndex++;
    }
}

void RenderMultiChildScroll::PerformLayout()
{
    auto context = context_.Upgrade();
    if (context && context->GetMinPlatformVersion() < COMPATIBLE_VERSION) {
        // List Layout Screen Remaining Space
        viewPort_ = GetLayoutParam().GetMaxSize();
        SetLayoutSize(viewPort_);
        if (NearEqual(viewPort_.Width(), Size::INFINITE_SIZE) || NearEqual(viewPort_.Height(), Size::INFINITE_SIZE)) {
            LOGW("The main or cross size is INFINITE, wait for the determined value");
            return;
        }
    } else {
        // List determines its own layout size based on children.
        if (GetLayoutParam().GetMaxSize().IsInfinite()) {
            ExtendViewPort(); // Extend the view port for layout more items.
        } else {
            viewPort_ = GetLayoutParam().GetMaxSize();
        }
    }

    LOGD("ViewPort:%{public}s Offset:%{public}s", viewPort_.ToString().c_str(), currentOffset_.ToString().c_str());
    offsetBeforeLayout_ = GetMainOffset(currentOffset_);
    LayoutChild();
    CalculateMainScrollExtent();
    ApplyGradientColor();

    if (IsReadyToJump()) {
        if (!NearEqual(initialOffset_, effectOffset_)) {
            JumpToPosition(initialOffset_);
            effectOffset_ = initialOffset_;
            LOGI("Effect initialOffset_:%{public}lf %{public}s", effectOffset_, currentOffset_.ToString().c_str());
        }
        if (NearZero(initialOffset_) && initialIndex_ > 0 && initialIndex_ != effectIndex_) {
            JumpToIndex(initialIndex_);
            effectIndex_ = initialIndex_;
            LOGI("Effect initialIndex_:%{public}d %{public}s", effectIndex_, currentOffset_.ToString().c_str());
        }
    }

    if (scrollable_->Available()) {
        ValidateOffset(SCROLL_FROM_NONE);
    } else {
        currentOffset_ = Offset::Zero();
    }

    if (!context || context->GetMinPlatformVersion() >= COMPATIBLE_VERSION) {
        if (GetLayoutParam().GetMaxSize().IsInfinite()) {
            // If not set the main axis length: wrap content.
            Rect rect;
            for (const auto& child : GetChildren()) {
                rect.IsValid() ? rect.CombineRect(child->GetPaintRect()) : rect = child->GetPaintRect();
            }
            viewPort_ = rect.GetSize();
        }
        SetLayoutSize(viewPort_);
    }
}

void RenderMultiChildScroll::ExtendViewPort()
{
    if (GreatNotEqual(GetMainSize(GetLayoutSize()), GetMainSize(viewPort_))) {
        if (axis_ == Axis::HORIZONTAL) {
            viewPort_.SetWidth(GetLayoutSize().Width() * EXTENT_RATIO);
        } else {
            viewPort_.SetHeight(GetLayoutSize().Height() * EXTENT_RATIO);
        }
    } else {
        if (axis_ == Axis::HORIZONTAL) {
            viewPort_.SetWidth(viewPort_.Width() * EXTENT_RATIO);
        } else {
            viewPort_.SetHeight(viewPort_.Height() * EXTENT_RATIO);
        }
    }
}

bool RenderMultiChildScroll::HandleCrashTop()
{
#ifdef WEARABLE_PRODUCT
    if (scrollVibrate_ && vibrator_) {
        vibrator_->Vibrate(VIBRATOR_TYPE_WATCH_CROWN_STRENGTH3);
    }
    if (rotationVibrate_ && IsFromRotate() && vibrator_) {
        vibrator_->Vibrate(VIBRATOR_TYPE_WATCH_CROWN_STRENGTH3);
    }
#endif
    return RenderScroll::HandleCrashTop();
}
bool RenderMultiChildScroll::HandleCrashBottom()
{
#ifdef WEARABLE_PRODUCT
    if (scrollVibrate_ && vibrator_) {
        vibrator_->Vibrate(VIBRATOR_TYPE_WATCH_CROWN_STRENGTH3);
    }
    if (rotationVibrate_ && IsFromRotate() && vibrator_) {
        vibrator_->Vibrate(VIBRATOR_TYPE_WATCH_CROWN_STRENGTH3);
    }
#endif
    return RenderScroll::HandleCrashBottom();
}

bool RenderMultiChildScroll::IsReadyToJump() const
{
    bool ready = false;
    for (const auto& child : GetChildren()) {
        auto listBase = AceType::DynamicCast<RenderList>(child);
        if (!listBase) {
            continue;
        }
        int32_t maxCount = listBase->GetMaxCount();
        if (listBase->IsPageReady() || (initialIndex_ != 0 && initialIndex_ < maxCount) ||
            (!NearZero(initialOffset_) && LessNotEqual(initialOffset_, mainScrollExtent_))) {
            ready = true;
            break;
        }
    }
    return ready;
}

void RenderMultiChildScroll::ApplyGradientColor()
{
    if (scrollable_ && !scrollable_->Available()) {
        LOGD("list not scrollable");
        return;
    }

    if (!gradientWidth_.IsValid()) {
        LOGD("not config gradient width");
        return;
    }

    auto box = AceType::DynamicCast<RenderBox>(GetParent().Upgrade());
    if (!box) {
        LOGE("parent is not box");
        return;
    }

    // workaround: set needLayout_ to true to make sure box will not mark flex to be layout
    // and add box to dirty layout set manually at the end.
    box->SetNeedLayout(true);

    Dimension widthPx = Dimension(viewPort_.Width());
    Dimension heightPx = Dimension(viewPort_.Height());
    Dimension gradientWidthPx = Dimension(NormalizeToPx(gradientWidth_));

    Dimension mainPx;
    Gradient gradient = Gradient();
    if (axis_ == Axis::HORIZONTAL) {
        mainPx = widthPx;
        gradient.SetDirection(GradientDirection::RIGHT);
    } else {
        mainPx = heightPx;
        gradient.SetDirection(GradientDirection::BOTTOM);
    }

    if (!IsAtTop()) {
        LOGD("apply top");
        GradientColor start;
        start.SetColor(backgroundColor_);
        start.SetDimension(Dimension(0.0));

        GradientColor end;
        Color endColor = backgroundColor_;
        endColor = endColor.ChangeAlpha(0);
        end.SetColor(endColor);
        end.SetDimension(gradientWidthPx);

        gradient.AddColor(start);
        gradient.AddColor(end);
    }

    if (!IsAtBottom()) {
        LOGD("apply bottom");
        GradientColor start;
        Color startColor = backgroundColor_;
        startColor = startColor.ChangeAlpha(0);
        start.SetColor(startColor);
        start.SetDimension(mainPx - gradientWidthPx);

        GradientColor end;
        end.SetColor(backgroundColor_);
        end.SetDimension(mainPx);

        gradient.AddColor(start);
        gradient.AddColor(end);
    }

    auto frontDecoration = box->GetFrontDecoration();
    if (!frontDecoration) {
        frontDecoration = AceType::MakeRefPtr<Decoration>();
    }
    frontDecoration->SetGradient(gradient);
    box->SetFrontDecoration(frontDecoration);

    auto pipelineContext = context_.Upgrade();
    if (pipelineContext) {
        pipelineContext->AddDirtyLayoutNode(box);
    }
}

void RenderMultiChildScroll::MoveItemToViewPort(double position, double size, double effectOffset)
{
    if (SystemProperties::GetDeviceType() != DeviceType::TV && SystemProperties::GetDeviceType() != DeviceType::PHONE) {
        return;
    }
    double beginPosition = CalculateBeginPositionInViewPort(position, size, effectOffset);
    if (beginPosition >= 0.0 && mainScrollExtent_ >= GetMainSize(viewPort_)) {
        beginPosition = std::clamp(beginPosition, 0.0, mainScrollExtent_ - GetMainSize(viewPort_));
    }
    ScrollToPosition(beginPosition, SCROLL_FROM_FOCUS_JUMP, false);
}

double RenderMultiChildScroll::CalculateBeginPositionInViewPort(double position, double size, double effectOffset)
{
    double gradientWidth = NormalizeToPx(gradientWidth_);
    double viewPortSize = GetMainSize(viewPort_);
    double offset = GetMainOffset(currentOffset_);
    double viewMin = offset;
    double viewMax = offset + viewPortSize - size;

    if (!IsAtTop()) {
        viewMin = offset + gradientWidth;
    }
    if (!IsAtBottom()) {
        viewMax = offset + viewPortSize - size - gradientWidth;
    }

    if (GreatOrEqual(viewMin, viewMax)) {
        return NONE_SCROLL_POSITION;
    }

    if (position < viewMin) {
        return std::max(position - gradientWidth - effectOffset, 0.0);
    }
    if (position > viewMax) {
        return std::max(position - viewMax + offset + effectOffset, 0.0);
    }
    return NONE_SCROLL_POSITION;
}

void RenderMultiChildScroll::ScrollToEdge(ScrollEdgeType scrollEdgeType, bool smooth)
{
    if (GetChildren().empty()) {
        LOGE("no list in scroll");
        return;
    }
    auto renderList = AceType::DynamicCast<RenderList>(GetChildren().front());
    if (!renderList) {
        LOGE("no list to jump");
        return;
    }

    double position = 0.0;
    if (scrollEdgeType == ScrollEdgeType::SCROLL_TOP) {
        position = renderList->CalculateItemPosition(0, ScrollType::SCROLL_TOP);
    } else {
        position = renderList->CalculateItemPosition(0, ScrollType::SCROLL_BOTTOM);
    }
    if (position < 0.0) {
        LOGE("Get edge position failed.");
        return;
    }
    CalculateMainScrollExtent();
    LOGI("Scroll to position:%{public}lf %{public}d", position, smooth);
    ScrollToPosition(position, SCROLL_FROM_FOCUS_JUMP, smooth);
}

bool RenderMultiChildScroll::ScrollToPosition(double position, int32_t source, bool smooth)
{
    if (NearEqual(position, NONE_SCROLL_POSITION)) {
        return false;
    }

    double distance = position - GetMainOffset(currentOffset_);
    if (NearZero(distance)) {
        return false;
    }
    position = std::max(position, 0.0);
    if (smooth) {
        ScrollBy(distance, distance, smooth);
    } else {
        JumpToPosition(std::max(position, 0.0), source);
    }
    return true;
}

bool RenderMultiChildScroll::ScrollPage(bool reverse, bool smooth, const std::function<void()>& onFinish)
{
    if (GetChildren().empty()) {
        LOGE("no list in scroll");
        return false;
    }
    auto renderList = AceType::DynamicCast<RenderList>(GetChildren().front());
    if (!renderList) {
        LOGE("no list to jump");
        return false;
    }
    double position = 0.0;
    if (reverse) {
        position = renderList->CalculateItemPosition(0, ScrollType::SCROLL_PAGE_UP);
    } else {
        position = renderList->CalculateItemPosition(0, ScrollType::SCROLL_PAGE_DOWN);
    }
    if (position < 0.0) {
        LOGE("Get page:%{public}d position failed.", reverse);
        return false;
    }
    CalculateMainScrollExtent();
    LOGI("Scroll to position:%{public}lf %{public}d", position, smooth);
    return ScrollToPosition(position, SCROLL_FROM_FOCUS_JUMP, smooth);
}

bool RenderMultiChildScroll::OnRotation(const RotationEvent& event)
{
    if (positionController_ && !positionController_->IsScrollNeedRotation()) {
        LOGE("OnRotation, current indexer is expand");
        return false;
    }
    float rotateValue = event.value; // value of rotation, means pixels(vp)
    HandleRotate(rotateValue, axis_ == Axis::VERTICAL);
    return true;
}

void RenderMultiChildScroll::HandleRotate(double rotateValue, bool isVertical)
{
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }
    auto listBase = AceType::DynamicCast<RenderList>(GetFirstChild());
    if (!listBase) {
        LOGE("no rotatable list");
        return;
    }

    if (listBase->GetOnRotateCallback()) {
        RotationEvent event = {rotateValue};
        (listBase->GetOnRotateCallback())(event);
    }

    double value = context->NormalizeToPx(Dimension(rotateValue, DimensionUnit::VP)) * (-1.0);
    if (listBase->IsSupportScale()) {
        value *= LIST_INCONTINUOUS_ROTATION_SENSITYVITY_NORMAL;
    } else {
        value *= LIST_CONTINUOUS_ROTATION_SENSITYVITY_NORMAL;
    }
    if (listBase->GetSupportItemCenter()) {
        auto childItem = listBase->GetChildByIndex(listBase->GetCenterIndex());
        auto centerItem = RenderListItem::GetRenderListItem(childItem);
        if (centerItem) {
            accumulatedRotationValue_ += value;
            Size itemSize = centerItem->GetLayoutSize();
            double threshold = 0.0;
            if (isVertical) {
                threshold = itemSize.Height() * LIST_ITEMCENTER_ROTATION_THRESHOLD;
            } else {
                threshold = itemSize.Width() * LIST_ITEMCENTER_ROTATION_THRESHOLD;
            }
            if (InRegion(-threshold, threshold, accumulatedRotationValue_)) {
                return;
            }
            value = accumulatedRotationValue_;
            accumulatedRotationValue_ = 0.0;
        }

        double destPosition = -GetMainOffset(currentOffset_) - value;
        double fixPosition = GetFixPositionOnWatch(destPosition, -GetMainOffset(currentOffset_));
        value -= fixPosition - destPosition;
        AnimateTo(GetCurrentPosition() + value, ANIMATION_DURATION, Curves::FRICTION);
    } else {
        // Vertical or horizontal, different axis
        Offset delta;
        if (isVertical) {
            delta.SetX(0.0);
            delta.SetY(value);
        } else {
            delta.SetX(value);
            delta.SetY(0.0);
        }
        UpdateOffset(delta, SCROLL_FROM_ROTATE);
    }
}

// notify start position in global main axis
void RenderMultiChildScroll::NotifyDragStart(double startPosition)
{
    for (const auto& child : GetChildren()) {
        auto listBase = AceType::DynamicCast<RenderList>(child);
        if (!listBase) {
            continue;
        }
        listBase->NotifyDragStart(startPosition);
    }
}

// notify drag offset in global main axis
void RenderMultiChildScroll::NotifyDragUpdate(double dragOffset, int32_t source)
{
    for (const auto& child : GetChildren()) {
        auto listBase = AceType::DynamicCast<RenderList>(child);
        if (!listBase) {
            continue;
        }
        listBase->NotifyDragUpdate(dragOffset);
        // switch chain control node in flush chain animation
        double delta = listBase->FlushChainAnimation();
        // fix currentOffset_ after switch control node.
        if (axis_ == Axis::HORIZONTAL) {
            currentOffset_ += Offset(-delta, 0.0);
        } else {
            currentOffset_ += Offset(0.0, -delta);
        }
        if (!NearZero(delta)) {
            LOGD("Fix offset when drag update. currentOffset: %{public}s, delta: %{public}.1lf",
                currentOffset_.ToString().c_str(), delta);
        }
    }
}

void RenderMultiChildScroll::ProcessScrollOverCallback(double velocity)
{
    for (const auto& child : GetChildren()) {
        auto listBase = AceType::DynamicCast<RenderList>(child);
        if (!listBase) {
            continue;
        }
        // switch chain control node when notify scroll over
        listBase->NotifyScrollOver(velocity, IsOutOfTopBoundary(), IsOutOfBottomBoundary());
        double delta = listBase->FlushChainAnimation();
        // fix currentOffset_ after switch control node.
        if (axis_ == Axis::HORIZONTAL) {
            currentOffset_ += Offset(-delta, 0.0);
        } else {
            currentOffset_ += Offset(0.0, -delta);
        }
        if (!NearZero(delta)) {
            LOGD("Fix offset when scroll over. currentOffset: %{public}s, delta: %{public}.1lf",
                currentOffset_.ToString().c_str(), delta);
        }
    }
}

double RenderMultiChildScroll::GetMainScrollExtent() const
{
    const double mainScrollExtent = RenderScroll::GetMainScrollExtent();
    auto child = GetFirstChild();
    auto listBase = AceType::DynamicCast<RenderList>(child);
    if (listBase) {
        return mainScrollExtent + listBase->GetTailAnimationValue() + listBase->GetHeadAnimationValue();
    } else {
        return mainScrollExtent;
    }
}

double RenderMultiChildScroll::GetFixPositionOnWatch(double destination, double current)
{
    LOGD("current(%{public}lf), distance(%{public}lf), viewport(%{public}lf)",
        GetMainOffset(currentOffset_), destination - current, GetMainSize(viewPort_));
    auto listBase = AceType::DynamicCast<RenderList>(GetLastChild());
    if (!listBase) {
        return destination;
    }

    // find centerIndex
    int32_t centerIndex = -1;
    double itemSize = 0.0;
    double itemPosition = 0.0;
    double listPosition = GetMainOffset(currentOffset_) - destination + current;
    if (GreatNotEqual(destination, current)) {
        // scroll to top direction
        centerIndex = listBase->EstimateIndexByPosition(listPosition + GetMainSize(viewPort_) * HALF_ITEM_SIZE);
        itemPosition = listBase->GetItemPosition(centerIndex);
        itemSize = listBase->GetItemPosition(centerIndex + 1) - itemPosition;
    } else {
        // scroll to bottom direction
        listBase->CalculateItemPosition(listPosition);
        double centerPosition = listPosition + GetMainSize(viewPort_) * HALF_ITEM_SIZE;
        for (const auto &itemPair : listBase->GetItems()) {
            int32_t index = itemPair.first;
            auto item = RenderListItem::GetRenderListItem(itemPair.second);
            double start = listBase->GetItemPosition(index);
            double end = start + listBase->GetMainSize(item->GetLayoutSize());

            if (start < centerPosition && end > centerPosition) {
                centerIndex = index;
                itemSize = GetMainSize(item->GetLayoutSize());
                itemPosition = item->GetPositionInList();
                break;
            }
        }
    }
    if (centerIndex == -1) {
        LOGW("invalid center index");
        return destination;
    }

    // calculate destination position after fix
    double center = listPosition + GetMainSize(viewPort_) * HALF_ITEM_SIZE;
    double itemCenterPosition = itemPosition + itemSize * HALF_ITEM_SIZE;
    return destination + center - itemCenterPosition;
}

bool RenderMultiChildScroll::IsOutOfBottomBoundary()
{
    double headOffset = GetMainOffset(currentOffset_);
    double tailOffset = mainScrollExtent_;
    auto child = GetLastChild();
    auto listBase = AceType::DynamicCast<RenderList>(child);
    if (listBase) {
        tailOffset = mainScrollExtent_ + listBase->GetTailAnimationValue();
        LOGD("IsOutOfBottomBoundary. offset_: %{public}.3lf, tailOffset: %{public}.3lf, viewPort: %{public}.3lf, "
             "mainScrollExtent_: %{public}.3lf, ReachMaxCount: %{public}d",
            GetMainOffset(currentOffset_), tailOffset, GetMainSize(viewPort_), mainScrollExtent_, ReachMaxCount());
    }
    if (IsRowReverse()) {
        return headOffset <= (GetMainSize(viewPort_) - tailOffset) && ReachMaxCount();
    } else {
        return headOffset >= (tailOffset - GetMainSize(viewPort_)) && ReachMaxCount();
    }
}

bool RenderMultiChildScroll::IsOutOfTopBoundary()
{
    double headOffset = GetMainOffset(currentOffset_);
    auto child = GetFirstChild();
    auto listBase = AceType::DynamicCast<RenderList>(child);
    if (listBase) {
        headOffset = GetMainOffset(currentOffset_) - listBase->GetHeadAnimationValue();
        LOGD("IsOutOfTopBoundary. offset_: %{public}.3lf, headOffset: %{public}.3lf", GetMainOffset(currentOffset_),
            headOffset);
    }
    if (IsRowReverse()) {
        return headOffset >= 0.0;
    } else {
        return headOffset <= 0.0;
    }
}

} // namespace OHOS::Ace
