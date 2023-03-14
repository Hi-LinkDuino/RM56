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

#include "core/components/drag_bar/render_drag_bar.h"

#include "core/animation/curve.h"
#include "core/animation/curve_animation.h"
#include "core/animation/curves.h"
#include "core/animation/keyframe.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/drag_bar/drag_bar_component.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension DRAG_ICON_WIDTH = 64.0_vp;
constexpr Dimension DRAG_ICON_HEIGHT = 24.0_vp;
constexpr Dimension HOT_REGION_WIDTH = 64.0_vp;
constexpr Dimension HOT_REGION_HEIGHT = 24.0_vp;
constexpr Dimension PAINT_WIDTH = 56.0_vp;
constexpr Dimension PAINT_HEIGHT = 8.0_vp;
constexpr Dimension MAX_DRAG_X = 10.0_vp;
constexpr Dimension MAX_DRAG_Y = 4.0_vp;
constexpr double DRAG_X_RATIO = 0.4;
constexpr double DRAG_Y_RATIO = 0.2;
constexpr double SCALE_ICON = 1.15;
constexpr double SCALE_WIDTH = 1.5 / SCALE_ICON;
constexpr int32_t DOWN_DURATION = 150;
constexpr int32_t RESET_DURATION = 250;
constexpr int32_t STYLE_DURATION = 200;

// For DragBar Shrink State Point.
const Offset POINT_L_SHRINK = Offset(17.0, 15.0); // Left Point position.
const Offset POINT_C_SHRINK = Offset(32.0, 9.0);  // Center Point position.
const Offset POINT_R_SHRINK = Offset(47.0, 15.0); // Right Point position.

// For DragBar Initial State Point.
const Offset POINT_L_INITIAL = Offset(18.0, 12.0); // Left Point position.
const Offset POINT_C_INITIAL = Offset(32.0, 12.0); // Center Point position.
const Offset POINT_R_INITIAL = Offset(46.0, 12.0); // Right Point position.

// For DragBar Expand State Point.
const Offset POINT_L_EXPAND = Offset(17.0, 9.0);  // Left Point position.
const Offset POINT_C_EXPAND = Offset(32.0, 15.0); // Center Point position.
const Offset POINT_R_EXPAND = Offset(47.0, 9.0);  // Right Point position.

const Size DRAG_BAR_SIZE = Size(64.0, 24.0);

constexpr double OPACITY_DEFAULT = 0.2;

} // namespace

void RenderDragBar::Update(const RefPtr<Component>& component)
{
    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    if (!barTouchController_) {
        barTouchController_ = AceType::MakeRefPtr<Animator>(GetContext());
        auto touchAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(1.0, SCALE_ICON, Curves::SHARP);
        touchAnimation->AddListener([weak = AceType::WeakClaim(this)](double value) {
            auto dragBar = weak.Upgrade();
            if (dragBar) {
                dragBar->scaleIcon_ = value;
                dragBar->scaleWidth_ = 1.0 + (value - 1.0) / (SCALE_ICON - 1.0) * (SCALE_WIDTH - 1.0);
                dragBar->MarkNeedRender();
            }
        });
        barTouchController_->ClearInterpolators();
        barTouchController_->AddInterpolator(touchAnimation);
        barTouchController_->SetFillMode(FillMode::FORWARDS);
    }
    if (!barRangeController_) {
        barRangeController_ = AceType::MakeRefPtr<Animator>(GetContext());
        barRangeController_->SetFillMode(FillMode::FORWARDS);
        barRangeController_->SetDuration(RESET_DURATION);
    }
    if (!barStyleController_) {
        barStyleController_ = AceType::MakeRefPtr<Animator>(GetContext());
        barStyleController_->SetFillMode(FillMode::FORWARDS);
        barStyleController_->SetDuration(STYLE_DURATION);
    }
    hotRegionHeight_ = HOT_REGION_HEIGHT;
    dragRangeX_ = NormalizeToPx(MAX_DRAG_X);
    dragRangeY_ = NormalizeToPx(MAX_DRAG_Y);
    alpha_ = std::floor(UINT8_MAX * OPACITY_DEFAULT);
    auto dragBar = AceType::DynamicCast<DragBarComponent>(component);
    if (dragBar) {
        showMode_ = dragBar->GetPanelMode();
        hasDragBar_ = dragBar->HasDragBar();
    }
    InitializeRecognizer();
    UpdateDrawPoint();
    MarkNeedLayout();
}

void RenderDragBar::PerformLayout()
{
    Size dragBarSize;
    if (hasDragBar_) {
        dragBarSize = Size(NormalizeToPx(HOT_REGION_WIDTH), NormalizeToPx(hotRegionHeight_));
    }
    if (fullScreenMode_) {
        dragBarSize = Size(GetLayoutParam().GetMaxSize().Width(), NormalizeToPx(hotRegionHeight_));
    }
    auto realSize = Size(NormalizeToPx(DRAG_ICON_WIDTH), NormalizeToPx(DRAG_ICON_HEIGHT));
    iconOffset_ = Alignment::GetAlignPosition(dragBarSize, realSize, Alignment::CENTER);
    scaleX_ = realSize.Width() / DRAG_BAR_SIZE.Width();
    scaleY_ = realSize.Height() / DRAG_BAR_SIZE.Height();
    SetLayoutSize(dragBarSize);
    imgTouchRegion_ = GetPaintRect();
}

void RenderDragBar::ShowArrow(bool show)
{
    PanelMode mode = PanelMode::HALF;
    if (show) {
        mode = PanelMode::FULL;
    } else {
        mode = PanelMode::HALF;
    }
    ShowInPanelMode(mode);
}

void RenderDragBar::ShowInPanelMode(PanelMode mode)
{
    if (showMode_ == mode) {
        return;
    }
    showMode_ = mode;
    UpdateDrawPoint();
    MarkNeedRender();
}

void RenderDragBar::UpdateDrawPoint()
{
    Offset leftPoint, centerPoint, rightPoint;
    switch (showMode_) {
        case PanelMode::MINI:
            leftPoint = POINT_L_SHRINK;
            centerPoint = POINT_C_SHRINK;
            rightPoint = POINT_R_SHRINK;
            break;
        case PanelMode::HALF:
            leftPoint = POINT_L_INITIAL;
            centerPoint = POINT_C_INITIAL;
            rightPoint = POINT_R_INITIAL;
            break;
        case PanelMode::FULL:
            leftPoint = POINT_L_EXPAND;
            centerPoint = POINT_C_EXPAND;
            rightPoint = POINT_R_EXPAND;
            break;
        default:
            LOGE("Unsupported Show Mode:%{public}d", showMode_);
            return;
    }
    if (barLeftPoint_ == Offset()) {
        // No need to do animation when first time to display.
        barLeftPoint_ = leftPoint;
        barCenterPoint_ = centerPoint;
        barRightPoint_ = rightPoint;
    } else {
        DoStyleAnimation(leftPoint, centerPoint, rightPoint);
    }
}

void RenderDragBar::DoStyleAnimation(const Offset& left, const Offset& center, const Offset& right)
{
    if (barStyleController_->IsRunning()) {
        barStyleController_->Stop();
    }
    auto leftAnimation = AceType::MakeRefPtr<CurveAnimation<Offset>>(barLeftPoint_, left, Curves::SHARP);
    leftAnimation->AddListener([weak = AceType::WeakClaim(this)](const Offset& value) {
        auto dragBar = weak.Upgrade();
        if (dragBar) {
            dragBar->barLeftPoint_ = value;
            dragBar->MarkNeedRender();
        }
    });
    auto centerAnimation = AceType::MakeRefPtr<CurveAnimation<Offset>>(barCenterPoint_, center, Curves::SHARP);
    centerAnimation->AddListener([weak = AceType::WeakClaim(this)](const Offset& value) {
        auto dragBar = weak.Upgrade();
        if (dragBar) {
            dragBar->barCenterPoint_ = value;
        }
    });
    auto rightAnimation = AceType::MakeRefPtr<CurveAnimation<Offset>>(barRightPoint_, right, Curves::SHARP);
    rightAnimation->AddListener([weak = AceType::WeakClaim(this)](const Offset& value) {
        auto dragBar = weak.Upgrade();
        if (dragBar) {
            dragBar->barRightPoint_ = value;
        }
    });
    barStyleController_->ClearInterpolators();
    barStyleController_->AddInterpolator(leftAnimation);
    barStyleController_->AddInterpolator(centerAnimation);
    barStyleController_->AddInterpolator(rightAnimation);
    barStyleController_->Play();
}

void RenderDragBar::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (fullScreenMode_) {
        return;
    }
    if (clickDetector_) {
        clickDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(clickDetector_);
    }
    if (touchDetector_) {
        touchDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(touchDetector_);
    }
}

void RenderDragBar::InitializeRecognizer()
{
    if (!clickDetector_) {
        clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickDetector_->SetOnClick([weak = WeakClaim(this)](const ClickInfo& info) {
            auto dragBar = weak.Upgrade();
            if (dragBar) {
                dragBar->HandleClick(info.GetLocalLocation());
            }
        });
    }
    if (!touchDetector_) {
        touchDetector_ = AceType::MakeRefPtr<RawRecognizer>();
        touchDetector_->SetOnTouchDown([weak = AceType::WeakClaim(this)](const TouchEventInfo& info) {
            auto dragBar = weak.Upgrade();
            if (dragBar && !info.GetTouches().empty()) {
                dragBar->HandleTouchDown(info.GetTouches().front().GetGlobalLocation());
            }
        });
        touchDetector_->SetOnTouchMove([weakDrag = AceType::WeakClaim(this)](const TouchEventInfo& info) {
            auto dragBar = weakDrag.Upgrade();
            if (dragBar && !info.GetTouches().empty()) {
                dragBar->HandleTouchMove(info.GetTouches().front().GetGlobalLocation());
            }
        });
        touchDetector_->SetOnTouchUp([weakDrag = AceType::WeakClaim(this)](const TouchEventInfo& info) {
            auto dragBar = weakDrag.Upgrade();
            if (dragBar) {
                dragBar->HandleTouchUp();
            }
        });
        touchDetector_->SetOnTouchCancel([weakDrag = AceType::WeakClaim(this)](const TouchEventInfo& info) {
            auto dragBar = weakDrag.Upgrade();
            if (dragBar) {
                dragBar->HandleTouchUp();
            }
        });
    }
}

void RenderDragBar::HandleClick(const Offset& clickPosition)
{
    if (!clickArrowCallback_) {
        return;
    }
    clickArrowCallback_();
}

void RenderDragBar::HandleTouchDown(const Offset& downPoint)
{
    // Display the click-to-magnify effect.
    downPoint_ = downPoint;
    barTouchController_->SetDuration(DOWN_DURATION);
    barTouchController_->Forward();
}

void RenderDragBar::HandleTouchMove(const Offset& movePoint)
{
    // Display the dragging offset effect.
    Offset distance = movePoint - downPoint_;
    Offset dragOffset;
    dragOffset.SetX(std::clamp(distance.GetX() * DRAG_X_RATIO, -dragRangeX_, dragRangeX_));
    dragOffset.SetY(std::clamp(distance.GetY() * DRAG_Y_RATIO, -dragRangeY_, dragRangeY_));
    if (dragOffset_ != dragOffset) {
        dragOffset_ = dragOffset;
        MarkNeedRender();
    }
}

void RenderDragBar::HandleTouchUp()
{
    // Restore the click-to-magnify effect.
    barTouchController_->SetDuration(RESET_DURATION);
    barTouchController_->Backward();

    // Restore the dragging offset effect.
    if (dragOffset_ == Offset()) {
        return; // No need to back to center with animation.
    }
    auto dragAnimation = AceType::MakeRefPtr<CurveAnimation<Offset>>(dragOffset_, Offset(), Curves::SHARP);
    dragAnimation->AddListener([weak = AceType::WeakClaim(this)](Offset value) {
        auto dragBar = weak.Upgrade();
        if (dragBar) {
            dragBar->dragOffset_ = value;
            dragBar->MarkNeedRender();
        }
    });
    barRangeController_->ClearInterpolators();
    barRangeController_->AddInterpolator(dragAnimation);
    barRangeController_->Play();
}

void RenderDragBar::FadingOut()
{
    auto keyframeFrom = AceType::MakeRefPtr<Keyframe<double>>(0.0, 1.0);
    auto keyframeTo = AceType::MakeRefPtr<Keyframe<double>>(1.0, 0.0);
    auto animation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    animation->AddKeyframe(keyframeFrom);
    animation->AddKeyframe(keyframeTo);
    animation->AddListener([weak = AceType::WeakClaim(this)](const double& opacity) {
        auto dragBar = weak.Upgrade();
        if (dragBar) {
            dragBar->opacity_ = opacity;
        }
    });
    animator_->AddInterpolator(animation);
}

void RenderDragBar::Stretching()
{
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("Animate to status bar failed. context is null.");
        return;
    }
    auto keyframeFrom = AceType::MakeRefPtr<Keyframe<double>>(0.0, hotRegionHeight_.Value());
    auto keyframeTo = AceType::MakeRefPtr<Keyframe<double>>(1.0, context->GetStatusBarHeight());
    auto animation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    animation->AddKeyframe(keyframeFrom);
    animation->AddKeyframe(keyframeTo);
    animation->AddListener([weak = AceType::WeakClaim(this)](const double& height) {
        auto dragBar = weak.Upgrade();
        if (!dragBar) {
            return;
        }
        dragBar->hotRegionHeight_.SetValue(height);
        dragBar->MarkNeedLayout();
    });
    animator_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto dragBar = weak.Upgrade();
        if (!dragBar) {
            return;
        }
        dragBar->SetFullScreenMode(true);
        dragBar->MarkNeedLayout();
    });
    animator_->AddInterpolator(animation);
}

void RenderDragBar::AnimateToStatusBarPadding(int32_t duration)
{
    if (animator_->IsRunning()) {
        animator_->Finish();
    }
    animator_->ClearAllListeners();
    animator_->ClearInterpolators();
    Stretching();
    FadingOut();
    animator_->SetFillMode(FillMode::FORWARDS);
    animator_->SetDuration(duration);
    animator_->Forward();
    SetDisableTouchEvent(true);
}

void RenderDragBar::SetStatusBarHeight(double height)
{
    // Reset full window animation.
    if (animator_->IsRunning()) {
        animator_->ClearAllListeners();
        animator_->ClearInterpolators();
        Stretching();
        FadingOut();
    }
    if (GreatOrEqual(height, 0.0) && !NearEqual(statusBarHeight_.Value(), height)) {
        statusBarHeight_.SetValue(height);
        MarkNeedLayout();
    }
}

void RenderDragBar::OnMouseHoverEnterTest()
{
    MarkNeedRender();
}

void RenderDragBar::OnMouseHoverExitTest()
{
    MarkNeedRender();
}

void RenderDragBar::OnPaintFinish()
{
    if (GetLayoutSize().IsEmpty()) {
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    if (renderStatus_ == RenderStatus::FOCUS) {
        Offset offset = GetPosition();
        Size size = Size(NormalizeToPx(PAINT_WIDTH), NormalizeToPx(PAINT_HEIGHT));
        auto alignOffset = Alignment::GetAlignPosition(GetLayoutSize(), size, Alignment::CENTER);
        Offset globalOffset = GetGlobalOffset() + alignOffset;
        RRect rrect = RRect::MakeRect(Rect(offset, size));
        auto radius = Radius(size.Height() / 2);
        rrect.SetCorner({ radius, radius, radius, radius });
        context->ShowFocusAnimation(rrect, Color::BLUE, globalOffset);
    }
}

void RenderDragBar::OnStatusChanged(RenderStatus renderStatus)
{
    if (renderStatus_ != renderStatus) {
        renderStatus_ = renderStatus;
        MarkNeedRender();
    }
}

} // namespace OHOS::Ace
