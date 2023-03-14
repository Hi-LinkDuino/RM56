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

#include "core/components/panel/render_sliding_panel.h"

#include "base/json/json_util.h"
#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "core/animation/curve.h"
#include "core/animation/curves.h"
#include "core/animation/keyframe.h"
#include "core/animation/keyframe_animation.h"
#include "core/animation/spring_animation.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/drag_bar/render_drag_bar.h"
#include "core/components/panel/sliding_panel_component.h"
#include "core/components/root/render_root.h"
#include "core/components/stack/stack_element.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t ANIMATION_BASE_DURATION = 256;
constexpr uint32_t FIND_MAX_COUNT = 64;
constexpr Dimension BLANK_MIN_HEIGHT = 8.0_vp;
constexpr Dimension DRAG_BAR_HEIGHT = 8.0_vp;
constexpr Dimension DRAG_UP_THRESHOLD = 48.0_vp;
constexpr double CONTENT_MIN_TOLERANCE = 0.01;
constexpr double VELOCITY_THRESHOLD = 1000.0; // Move 1000px per second.

int32_t GetAnimationDuration(double delta, double dragRange)
{
    if (NearZero(dragRange)) {
        return 0;
    }
    return static_cast<int32_t>(((std::abs(delta) / dragRange) + 1.0) * ANIMATION_BASE_DURATION);
}

template<class T>
RefPtr<T> FindChildOfClass(const RefPtr<RenderNode>& parent)
{
    // BFS to find child in tree.
    uint32_t findCount = 0;
    auto searchQueue = parent->GetChildren(); // copy children to a queue
    while (++findCount <= FIND_MAX_COUNT && !searchQueue.empty()) {
        const auto child = searchQueue.front();
        searchQueue.pop_front();
        if (!child) {
            continue;
        }
        if (AceType::InstanceOf<T>(child)) {
            return AceType::DynamicCast<T>(child);
        }
        searchQueue.insert(searchQueue.end(), child->GetChildren().begin(), child->GetChildren().end());
    }
    return RefPtr<T>();
}

} // namespace

std::string SlidingPanelSizeChangeEvent::ToJSONString() const
{
    auto jsonResult = JsonUtil::Create(false);
    jsonResult->Put("height", height_);
    jsonResult->Put("width", width_);
    auto result = JsonUtil::Create(true);
    result->Put("size", jsonResult);
    std::string modeStr;
    if (mode_ == PanelMode::FULL) {
        modeStr = "full";
    } else if (mode_ == PanelMode::HALF) {
        modeStr = "half";
    } else {
        modeStr = "mini";
    }
    result->Put("mode", modeStr.c_str());

    return std::string(R"("sizechange",)").append(result->ToString());
}

RefPtr<RenderNode> RenderSlidingPanel::Create()
{
    return AceType::MakeRefPtr<RenderSlidingPanel>();
}

void RenderSlidingPanel::Update(const RefPtr<Component>& component)
{
    auto slidingPanel = AceType::DynamicCast<SlidingPanelComponent>(component);
    if (!slidingPanel) {
        LOGE("slidingPanel update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    InitializeRecognizer();
    if (isFirstUpdate_) {
        isFirstUpdate_ = false;
        boxForBlank_ = RenderBox::Create();
        AddChild(boxForBlank_, 0);
        boxForBlank_->Attach(GetContext());
        animator_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    hasBoxStyle_ = slidingPanel->HasBoxStyle();
    previousMode_ = mode_;
    if ((slidingPanel->GetMode() != PanelMode::AUTO && previousMode_ != slidingPanel->GetMode()) ||
        type_ != slidingPanel->GetType()) {
        isFirstLayout_ = true;
    }
    mode_ = slidingPanel->GetMode() == PanelMode::AUTO ? PanelMode::FULL : slidingPanel->GetMode();
    type_ = slidingPanel->GetType();
    fullHeight_ = slidingPanel->GetFullHeight();
    halfHeight_ = slidingPanel->GetHalfHeight();
    miniHeight_ = slidingPanel->GetMiniHeight();
    panelId_ = slidingPanel->GetPanelId();
    onSizeChange_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
        slidingPanel->GetOnSizeChanged(), context_);
    MarkNeedLayout();
}

void RenderSlidingPanel::OnPaintFinish()
{
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    auto manager = context->GetAccessibilityManager();
    if (manager) {
        auto node = manager->GetAccessibilityNodeById(panelId_);
        if (!node) {
            LOGE("node is null");
            return;
        }

        auto viewScale = context->GetViewScale();
        Size size = GetLayoutSize() * viewScale;
        Offset globalOffset = GetGlobalOffset() * viewScale;
        double height = blankHeight_ * viewScale;

        // blankHeight_ is the height from screen top to panel top
        node->SetWidth(size.Width());
        node->SetHeight(size.Height() - height);
        node->SetLeft(globalOffset.GetX());
        node->SetTop(height);
        node->SetVisible(true);
    }
#endif
}

void RenderSlidingPanel::PerformLayout()
{
    if (!InitializeLayoutProps()) {
        return;
    }
    auto maxSize = GetLayoutParam().GetMaxSize();
    // SlidingPanelComponent's size is as large as the root's.
    if (maxSize.IsInfinite()) {
        SetLayoutSize(viewPort_);
    } else {
        SetLayoutSize(maxSize);
    }
    if (isFirstLayout_) {
        blankHeight_ = GetLayoutParam().GetMaxSize().Height();
        AnimateTo(defaultBlankHeights_[mode_], mode_);
        if (previousMode_ != mode_) {
            FireSizeChangeEvent();
        }
    } else {
        InnerLayout();
    }
    isFirstLayout_ = false;
    previousSize_ = maxSize;
}

bool RenderSlidingPanel::InitializeLayoutProps()
{
    // Only 2 children are allowed in RenderSlidingPanel
    if (GetChildren().empty() || GetChildren().size() != 2) {
        LOGE("Children size wrong in slide panel modal");
        return false;
    }
    if (!dragBar_) {
        dragBar_ = FindChildOfClass<RenderDragBar>(GetChildren().back());
        if (!dragBar_) {
            LOGE("No drag bar in sliding panel. Cause error");
            return false;
        }
    }
    auto maxSize = GetLayoutParam().GetMaxSize();
    if (defaultBlankHeights_.empty() || previousSize_ != maxSize) {
        defaultBlankHeights_[PanelMode::MINI] = miniHeight_.second
                                                    ? maxSize.Height() - NormalizeToPx(miniHeight_.first)
                                                    : maxSize.Height() - NormalizeToPx(DRAG_UP_THRESHOLD);
        defaultBlankHeights_[PanelMode::HALF] =
            halfHeight_.second ? maxSize.Height() - NormalizeToPx(halfHeight_.first) : maxSize.Height() / 2;
        defaultBlankHeights_[PanelMode::FULL] =
            fullHeight_.second ? maxSize.Height() - NormalizeToPx(fullHeight_.first) : NormalizeToPx(BLANK_MIN_HEIGHT);
        CheckHeightValidity();
        isFirstLayout_ = true;
        fullHalfBoundary_ = defaultBlankHeights_[PanelMode::FULL] +
            (defaultBlankHeights_[PanelMode::HALF] - defaultBlankHeights_[PanelMode::FULL]) / 2.0;
        halfMiniBoundary_ = defaultBlankHeights_[PanelMode::HALF] +
            (defaultBlankHeights_[PanelMode::MINI] - defaultBlankHeights_[PanelMode::HALF]) / 2.0;
        fullMiniBoundary_ = defaultBlankHeights_[PanelMode::FULL] +
            (defaultBlankHeights_[PanelMode::MINI] - defaultBlankHeights_[PanelMode::FULL]) / 2.0;
    }
    minBlankHeight_ = NormalizeToPx(BLANK_MIN_HEIGHT);
    if (dragBar_ && !(dragBar_->HasClickArrowCallback())) {
        SetDragBarCallBack();
    }
    return true;
}

void RenderSlidingPanel::CheckHeightValidity()
{
    auto minBlank = NormalizeToPx(BLANK_MIN_HEIGHT);
    auto maxBlank = GetLayoutParam().GetMaxSize().Height() - NormalizeToPx(DRAG_BAR_HEIGHT);
    defaultBlankHeights_[PanelMode::MINI] = std::clamp(defaultBlankHeights_[PanelMode::MINI], minBlank, maxBlank);
    defaultBlankHeights_[PanelMode::HALF] = std::clamp(defaultBlankHeights_[PanelMode::HALF], minBlank, maxBlank);
    defaultBlankHeights_[PanelMode::FULL] = std::clamp(defaultBlankHeights_[PanelMode::FULL], minBlank, maxBlank);
}

void RenderSlidingPanel::SetDragBarCallBack()
{
    dragBar_->SetClickArrowCallback([weak = WeakClaim(this)]() {
        auto panel = weak.Upgrade();
        if (!panel) {
            LOGE("panel is nullptr");
            return;
        }
        auto dragBar = panel->GetDragBar();
        if (!dragBar) {
            LOGE("dragBar is nullptr");
            return;
        }
        panel->previousMode_ = panel->mode_;
        if (panel->mode_ == PanelMode::MINI) {
            panel->mode_ = panel->type_ == PanelType::MINI_BAR ? PanelMode::FULL : PanelMode::HALF;
        } else if (panel->mode_ == PanelMode::FULL) {
            panel->mode_ = panel->type_ == PanelType::MINI_BAR ? PanelMode::MINI : PanelMode::HALF;
        } else {
            LOGD("not support click in half mode");
        }
        panel->AnimateTo(panel->defaultBlankHeights_[panel->mode_], panel->mode_);
        if (panel->previousMode_ != panel->mode_) {
            panel->FireSizeChangeEvent();
        }
    });
}

void RenderSlidingPanel::FireSizeChangeEvent()
{
    if (!onSizeChange_) {
        return;
    }
    double height =
        std::floor(GetLayoutSize().Height() - defaultBlankHeights_[mode_] - dragBar_->GetLayoutSize().Height());
    double width = std::floor(GetLayoutSize().Width());
    onSizeChange_(std::make_shared<SlidingPanelSizeChangeEvent>(mode_, width, height));
}

void RenderSlidingPanel::InnerLayout()
{
    LayoutParam innerLayout;
    auto maxSize = GetLayoutParam().GetMaxSize();
    auto columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::PANEL);
    columnInfo->GetParent()->BuildColumnWidth();
    auto maxWidth = columnInfo->GetWidth() + 2 * NormalizeToPx(columnInfo->GetParent()->GetGutterWidth());
    if (GridSystemManager::GetInstance().GetCurrentSize() <= GridSizeType::SM) {
        maxWidth = maxSize.Width();
    }
    innerLayout.SetFixedSize(Size(maxWidth, blankHeight_));
    boxForBlank_->Layout(innerLayout);
    boxForBlank_->SetPosition(Offset::Zero());
    auto contentHeight = maxSize.Height() - blankHeight_;
    innerLayout.SetMinSize(Size());
    innerLayout.SetMaxSize(Size(maxWidth, contentHeight));
    auto boxForContent = GetChildren().back();
    boxForContent->Layout(innerLayout);
    auto centerX = (maxSize.Width() - maxWidth) / 2;
    boxForContent->SetPosition(Offset(centerX, blankHeight_));
    double viewPortHeight = maxSize.Height() - blankHeight_ - dragBar_->GetLayoutSize().Height();
    viewPort_ = Size(maxWidth, viewPortHeight);
}

void RenderSlidingPanel::ResetContentHeight()
{
    AnimateTo(dragStartBlankHeight_, mode_);
}

void RenderSlidingPanel::OnAnimationStop()
{
    isAnimating_ = false;
}

void RenderSlidingPanel::InitializeRecognizer()
{
    if (!dragDetector_) {
        dragDetector_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
        dragDetector_->SetOnDragStart([weak = WeakClaim(this)](const DragStartInfo& startInfo) {
            auto panel = weak.Upgrade();
            if (panel) {
                panel->HandleDragStart(startInfo.GetLocalLocation());
            }
        });
        dragDetector_->SetOnDragUpdate([weakDrag = AceType::WeakClaim(this)](const DragUpdateInfo& info) {
            auto panel = weakDrag.Upgrade();
            if (panel) {
                panel->HandleDragUpdate(info.GetLocalLocation());
            }
        });
        dragDetector_->SetOnDragEnd([weakDrag = AceType::WeakClaim(this)](const DragEndInfo& info) {
            auto panel = weakDrag.Upgrade();
            if (panel) {
                panel->HandleDragEnd(info.GetLocalLocation(), info.GetMainVelocity());
            }
        });
    }
}

bool RenderSlidingPanel::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    // Forbidden vertical swipe of all children in not full mode.
    TouchRestrict newRestrict = touchRestrict;
    if (mode_ != PanelMode::FULL) {
        newRestrict.UpdateForbiddenType(TouchRestrict::SWIPE_VERTICAL);
    }
    if (!GetChildren().empty()) {
        touchRect_ = GetChildren().back()->GetPaintRect();
    }
    return RenderNode::TouchTest(globalPoint, parentLocalPoint, newRestrict, result);
}

void RenderSlidingPanel::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (isAnimating_) {
        return;
    }
    if (dragDetector_) {
        dragDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDetector_);
    }
}

void RenderSlidingPanel::UpdateTouchRect()
{
    touchRect_ = GetPaintRect();
    touchRectList_.emplace_back(touchRect_);
    SetTouchRectList(touchRectList_);
    if (GetChildren().size() < 2) {
        return;
    }
    touchRect_ = GetChildren().back()->GetPaintRect();
    touchRectList_.emplace_back(touchRect_);
    SetTouchRectList(touchRectList_);
}

void RenderSlidingPanel::HandleDragStart(const Offset& startPoint)
{
    if (isAnimating_) {
        return;
    }
    dragStartPoint_ = startPoint;
    dragStartBlankHeight_ = blankHeight_;
}

void RenderSlidingPanel::HandleDragUpdate(const Offset& currentPoint)
{
    if (isAnimating_) {
        return;
    }
    double targetBlankHeight = dragStartBlankHeight_ + currentPoint.GetY() - dragStartPoint_.GetY();

    if (targetBlankHeight < minBlankHeight_) {
        targetBlankHeight = minBlankHeight_;
    }
    // Do not drag to make page content height <= zero.
    double maxBlankHeight = GetLayoutSize().Height() - CONTENT_MIN_TOLERANCE;
    if (dragBar_) {
        maxBlankHeight -= dragBar_->GetLayoutSize().Height();
    }
    if (targetBlankHeight > maxBlankHeight) {
        targetBlankHeight = maxBlankHeight;
    }
    blankHeight_ = targetBlankHeight;
    MarkNeedLayout();
}

void RenderSlidingPanel::HandleDragEnd(const Offset& endPoint, double velocity)
{
    if (isAnimating_) {
        return;
    }
    previousMode_ = mode_;
    auto dragStart = dragStartPoint_.GetY();
    auto dragLen = endPoint.GetY() - dragStart;
    switch (type_) {
        case PanelType::MINI_BAR: { // FULL & MINI
            CalculateModeTypeMini(dragLen, velocity);
            break;
        }
        case PanelType::FOLDABLE_BAR: { // FULL & HALF & MINI
            CalculateModeTypeFold(dragLen, velocity);
            break;
        }
        case PanelType::TEMP_DISPLAY: { // FULL & HALF
            CalculateModeTypeTemp(dragLen, velocity);
            break;
        }
        default: {
            LOGE("Unsupported type:%{public}d", type_);
            return;
        }
    }
    AnimateTo(defaultBlankHeights_[mode_], mode_);
    if (previousMode_ != mode_) {
        FireSizeChangeEvent();
    }
}

void RenderSlidingPanel::CalculateModeTypeMini(double dragLen, double velocity) // FULL & MINI
{
    double currentPostion = defaultBlankHeights_[mode_] + dragLen;
    if (std::abs(velocity) < VELOCITY_THRESHOLD) {
        // Drag velocity not reached to threshold, mode based on the location.
        if (currentPostion < fullMiniBoundary_) {
            mode_ = PanelMode::FULL;
        } else {
            mode_ = PanelMode::MINI;
        }
    } else {
        // Drag velocity reached to threshold, mode based on the drag direction.
        if (velocity > 0.0) {
            mode_ = PanelMode::MINI;
        } else {
            mode_ = PanelMode::FULL;
        }
    }
}

void RenderSlidingPanel::CalculateModeTypeFold(double dragLen, double velocity) // FULL & HALF & MINI
{
    double currentPostion = defaultBlankHeights_[mode_] + dragLen;
    if (std::abs(velocity) < VELOCITY_THRESHOLD) {
        // Drag velocity not reached to threshold, mode based on the location.
        if (currentPostion < fullHalfBoundary_) {
            mode_ = PanelMode::FULL;
        } else if (currentPostion < halfMiniBoundary_) {
            mode_ = PanelMode::HALF;
        } else {
            mode_ = PanelMode::MINI;
        }
    } else {
        // Drag velocity reached to threshold, mode based on the drag direction.
        if (velocity > 0.0) {
            if (currentPostion < defaultBlankHeights_[PanelMode::HALF]) {
                mode_ = PanelMode::HALF;
            } else {
                mode_ = PanelMode::MINI;
            }
        } else {
            if (currentPostion > defaultBlankHeights_[PanelMode::HALF]) {
                mode_ = PanelMode::HALF;
            } else {
                mode_ = PanelMode::FULL;
            }
        }
    }
}

void RenderSlidingPanel::CalculateModeTypeTemp(double dragLen, double velocity) // FULL & HALF
{
    double currentPostion = defaultBlankHeights_[mode_] + dragLen;
    if (std::abs(velocity) < VELOCITY_THRESHOLD) {
        // Drag velocity not reached to threshold, mode based on the location.
        if (currentPostion < fullHalfBoundary_) {
            mode_ = PanelMode::FULL;
        } else {
            mode_ = PanelMode::HALF;
        }
    } else {
        // Drag velocity reached to threshold, mode based on the drag direction.
        if (velocity > 0.0) {
            mode_ = PanelMode::HALF;
        } else {
            mode_ = PanelMode::FULL;
        }
    }
}

void RenderSlidingPanel::AppendBlankHeightAnimation(double blankHeight, PanelMode mode)
{
    if (LessNotEqual(blankHeight_, 0.0) || NearEqual(blankHeight_, blankHeight)) {
        LOGE("Append animation failed. Blank height less than zero.");
        return;
    }

    // Mass: 1.0, stiffness: 100.0f, damping: 20.0f. For Critical Damped.
    auto springProperty = AceType::MakeRefPtr<SpringProperty>(1.0f, 100.0f, 20.0f);
    auto heightAnimation = AceType::MakeRefPtr<SpringAnimation>(springProperty);
    heightAnimation->AddListener([weak = AceType::WeakClaim(this),
        start = blankHeight_, end = blankHeight, mode](const double& value) {
        auto panel = weak.Upgrade();
        if (!panel) {
            LOGE("Panel is null.");
            return;
        }
        if (value > 1.0) {
            panel->dragBar_->ShowInPanelMode(mode);
        }
        panel->blankHeight_ = start + (end - start) * value;
        panel->MarkNeedLayout();
    });
    animator_->AddInterpolator(heightAnimation);
}

void RenderSlidingPanel::AnimateTo(double blankHeight, PanelMode mode)
{
    isAnimating_ = true;
    animator_->ClearInterpolators();
    animator_->ClearAllListeners();
    if (animator_->IsRunning()) {
        animator_->Stop();
    }
    animator_->AddStopListener([weak = WeakClaim(this), mode]() {
        auto panel = weak.Upgrade();
        if (panel && panel->dragBar_) {
            panel->OnAnimationStop();
            panel->dragBar_->ShowInPanelMode(mode);
        }
    });
    AppendBlankHeightAnimation(blankHeight, mode);
    auto dragRange = GetLayoutParam().GetMaxSize().Height();
    animator_->SetDuration(GetAnimationDuration(blankHeight - blankHeight_, dragRange));
    animator_->SetFillMode(FillMode::FORWARDS);
    animator_->Forward();
}

void RenderSlidingPanel::Dump()
{
    DumpLog::GetInstance().AddDesc(std::string("PanelMode:").append(std::to_string(static_cast<int32_t>(mode_))));
    DumpLog::GetInstance().AddDesc(std::string("blankHeight: ").append(std::to_string(blankHeight_)));
}

} // namespace OHOS::Ace
