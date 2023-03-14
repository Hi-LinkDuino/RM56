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

#include "core/components/semi_modal/render_semi_modal.h"

#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "core/animation/curve.h"
#include "core/animation/curves.h"
#include "core/animation/keyframe.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/drag_bar/render_drag_bar.h"
#include "core/components/root/render_root.h"
#include "core/components/semi_modal/semi_modal_component.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t ANIMATION_BASE_DURATION = 256;
constexpr double CONTENT_DEFAULT_RATIO = 0.67;
constexpr double KEYBOARD_HEIGHT_RATIO = 0.18;
constexpr Dimension BLANK_MIN_HEIGHT = 8.0_vp;
constexpr Dimension DRAG_UP_THRESHOLD = 48.0_vp;
constexpr Dimension DRAG_BAR_ARROW_BIAS = 1.0_vp;
constexpr double CONTENT_MIN_TOLERANCE = 0.01;

int32_t GetAnimationDuration(double delta, double dragRange)
{
    if (NearZero(dragRange)) {
        return 0;
    }
    return static_cast<int32_t>(((std::abs(delta) / dragRange) + 1.0) * ANIMATION_BASE_DURATION);
}

} // namespace

RefPtr<RenderNode> RenderSemiModal::Create()
{
    return AceType::MakeRefPtr<RenderSemiModal>();
}

void RenderSemiModal::Update(const RefPtr<Component>& component)
{
    auto semiModal = AceType::DynamicCast<SemiModalComponent>(component);
    if (!semiModal) {
        LOGE("RenderDragBar update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    InitializeRecognizer();
    isFullScreen_ = semiModal->IsFullScreen();
    if (GreatNotEqual(semiModal->GetModalHeight(), 0.0)) {
        hasInputHeight_ = true;
        inputHeight_ = semiModal->GetModalHeight();
    }
    if (isFirstUpdate_) {
        isFirstUpdate_ = false;
        boxForBlank_ = RenderBox::Create();
        AddChild(boxForBlank_, 0);
        boxForBlank_->Attach(GetContext());
        animator_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    MarkNeedLayout();
}

bool RenderSemiModal::IsPageReady() const
{
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("Cannot get context!");
        return false;
    }
    const auto& pageStack = context->GetLastStack();
    if (!pageStack) {
        LOGE("Cannot get pageStack!");
        return false;
    }
    const auto& pageStackNode = pageStack->GetRenderNode();
    if (!pageStackNode) {
        LOGE("Cannot get pageStackNode!");
        return false;
    }
    return true;
}

void RenderSemiModal::PerformLayout()
{
    // Only 2 children are allowed in RenderSemiModal
    if (GetChildren().empty() || GetChildren().size() != 2) {
        LOGE("Children size wrong in semi modal");
        return;
    }
    if (!dragBar_) {
        dragBar_ = FindChildOfClass<RenderDragBar>(GetChildren().back());
    }
    if (dragBar_ && !(dragBar_->HasClickArrowCallback())) {
        dragBar_->SetClickArrowCallback([weak = WeakClaim(this)]() {
            auto semiModal = weak.Upgrade();
            if (semiModal) {
                semiModal->AnimateToExitApp();
            }
        });
    }
    // SemiModalComponent's size is as large as the root's.
    auto maxSize = GetLayoutParam().GetMaxSize();
    auto columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::PANEL);
    columnInfo->GetParent()->BuildColumnWidth();
    maxWidth_ = columnInfo->GetWidth() + 2 * NormalizeToPx(columnInfo->GetParent()->GetGutterWidth());
    if (GridSystemManager::GetInstance().GetCurrentSize() <= GridSizeType::SM || isFullScreen_) {
        maxWidth_ = maxSize.Width();
    }
    SetLayoutSize(maxSize);
    double viewPortHeight = maxSize.Height() - navigationHeight_ - blankHeight_;
    if (dragBar_) {
        viewPortHeight -= dragBar_->GetLayoutSize().Height();
    }
    viewPort_ = Size(maxWidth_, viewPortHeight);
    // Layout content region
    if (isFirstLayout_) {
        // First layout, calculate default height.
        FirstLayout();
    } else {
        InnerLayout();
    }
    double verticalPosition = 0.0;
    for (const auto& child : GetChildren()) {
        double horizontalPosition = (maxSize.Width() - child->GetLayoutSize().Width()) / 2;
        child->SetPosition(Offset(horizontalPosition, verticalPosition));
        verticalPosition += child->GetLayoutSize().Height();
    }
    blankTouchRegion_ = boxForBlank_->GetPaintRect();
    blankHeight_ = boxForBlank_->GetPaintRect().Height();
    UpdateMinBlankHeight();
    forbiddenSwipe_ = GreatNotEqual(blankHeight_, minBlankHeight_);
    if (isFirstLayout_) {
        ShowUpAnimation();
    }
    // Set isFirstLayout_ true if page not ready.
    isFirstLayout_ = !IsPageReady();
}

void RenderSemiModal::InnerLayout()
{
    // Not first layout, use blankHeight_ control all children's size and layout.

    // update blankHeight_ when device orientation changed
    if (orientation_ != SystemProperties::GetDevcieOrientation()) {
        updateMinBlank_ = true;
        UpdateMinBlankHeight();
        blankHeight_ = UpdateTargetBlankHeight(blankHeight_);
        orientation_ = SystemProperties::GetDevcieOrientation();
    }

    auto maxSize = GetLayoutParam().GetMaxSize();
    auto boxForContent = GetChildren().back();
    if (updateDefaultBlank_) {
        UpdateDefaultBlankHeight();
        if (isAnimatingToDefault_ && !isExit_) {
            // Update animation target position.
            AnimateTo(defaultBlankHeight_);
        }
        updateDefaultBlank_ = false;
    }
    LayoutParam innerLayoutParam;
    innerLayoutParam.SetFixedSize(Size(maxSize.Width(), blankHeight_));
    boxForBlank_->Layout(innerLayoutParam);
    innerLayoutParam.SetMaxSize(Size(maxWidth_, maxSize.Height() - blankHeight_));
    innerLayoutParam.SetMinSize(Size());
    boxForContent->Layout(innerLayoutParam);
}

void RenderSemiModal::UpdateMinBlankHeight()
{
    if (!updateMinBlank_ || isAnimating_ || !IsPageReady()) {
        return;
    }
    auto contentHeight = GetScrollContentHeight();
    if (dragBar_) {
        contentHeight += dragBar_->GetLayoutSize().Height();
    }
    contentHeight += navigationHeight_;
    minBlankHeight_ = GetLayoutParam().GetMaxSize().Height() - contentHeight;
    double minHeight = NormalizeToPx(BLANK_MIN_HEIGHT) + statusBarHeight_;
    if (minBlankHeight_ < minHeight) {
        minBlankHeight_ = minHeight;
    }
    updateMinBlank_ = false;
    UpdateDragImg();
}

void RenderSemiModal::UpdateDefaultBlankHeight()
{
    auto contentHeight = GetScrollContentHeight();
    if (dragBar_) {
        contentHeight += dragBar_->GetLayoutSize().Height();
    }
    contentHeight += navigationHeight_;
    auto maxSize = GetLayoutParam().GetMaxSize();
    if (!hasInputHeight_) {
        defaultBlankHeight_ =
            maxSize.Height() - std::min(contentHeight, GetLayoutParam().GetMaxSize().Height() * CONTENT_DEFAULT_RATIO);
    } else {
        defaultBlankHeight_ = maxSize.Height() - std::min(contentHeight, inputHeight_);
    }
}

void RenderSemiModal::OnNavigationBarHeightChanged(const Dimension& height)
{
    if (NearEqual(navigationHeight_, NormalizeToPx(height))) {
        return;
    }
    double delta = navigationHeight_ - NormalizeToPx(height);
    navigationHeight_ = NormalizeToPx(height);
    // Page size and blankHeight value change opposite
    MovePage(delta * -1.0);
    if (isAnimatingToDefault_ || isFullScreen_) {
        updateMinBlank_ = true;
        updateDefaultBlank_ = true;
        MarkNeedLayout();
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("OnNavigationBarHeightChanged failed, context is null");
        return;
    }
    updateMinBlank_ = true;
    updateDefaultBlank_ = true;
    if (abs(delta) < context->GetRootHeight() * KEYBOARD_HEIGHT_RATIO) {
        // Is not a keyboard changing.
        blankHeight_ = std::max(blankHeight_ + delta, statusBarHeight_);
    } else {
        // Is a keyboard change.
        if (LessNotEqual(delta, 0.0)) {
            // keyboard slide in, remember old blank value
            normalBlankHeight_ = blankHeight_;
            blankHeight_ = std::clamp(blankHeight_ + delta, statusBarHeight_, minBlankHeight_);
        } else {
            // keyboard slide out, restore old blank value
            blankHeight_ = normalBlankHeight_;
        }
    }
    MarkNeedLayout();
}

void RenderSemiModal::OnStatusBarHeightChanged(const Dimension& height)
{
    if (NearEqual(statusBarHeight_, NormalizeToPx(height)) || isFullScreen_) {
        return;
    }
    statusBarHeight_ = NormalizeToPx(height);
    updateMinBlank_ = true;
    MarkNeedLayout();
}

void RenderSemiModal::LayoutFullScreen(const RefPtr<RenderNode>& boxForContent)
{
    if (dragBar_) {
        dragBar_->SetFullScreenMode(true);
    }
    // blank is 0.0, content is full screen.
    boxForContent->Layout(GetLayoutParam());
    LayoutParam innerLayoutParam;
    innerLayoutParam.SetFixedSize(Size(0.0, 0.0));
    boxForBlank_->Layout(innerLayoutParam);
}

void RenderSemiModal::FirstLayout()
{
    auto boxForContent = GetChildren().back();
    auto maxSize = GetLayoutParam().GetMaxSize();
    LayoutParam innerLayoutParam;
    if (!hasInputHeight_) {
        innerLayoutParam.SetMaxSize(Size(maxWidth_, maxSize.Height() * CONTENT_DEFAULT_RATIO));
    } else {
        innerLayoutParam.SetMaxSize(Size(maxWidth_, inputHeight_));
    }
    innerLayoutParam.SetMinSize(Size());
    boxForContent->Layout(innerLayoutParam);
    auto contentHeight = GetScrollContentHeight();
    if (dragBar_) {
        dragBar_->ShowArrow(false);
        contentHeight += dragBar_->GetLayoutSize().Height();
    }
    contentHeight += navigationHeight_;
    double maxHeight = hasInputHeight_ ? inputHeight_ : maxSize.Height() * CONTENT_DEFAULT_RATIO;
    if (contentHeight < maxHeight) {
        innerLayoutParam.SetMaxSize(Size(maxWidth_, contentHeight));
        boxForContent->Layout(innerLayoutParam);
    }
    defaultBlankHeight_ = maxSize.Height() - boxForContent->GetLayoutSize().Height();
    // First layout, animate from maxHeight to defaultBlankHeight_.
    innerLayoutParam.SetFixedSize(Size(maxSize.Width(), maxSize.Height()));
    boxForBlank_->Layout(innerLayoutParam);
}

double RenderSemiModal::GetScrollContentHeight()
{
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("Cannot get context!");
        return 0.0;
    }
    const auto& pageStack = context->GetLastStack();
    if (!pageStack) {
        LOGE("Cannot get pageStack!");
        return 0.0;
    }
    const auto& pageStackNode = pageStack->GetRenderNode();
    auto page = cachedPage_.Upgrade();
    if (page && pageStackNode != page) {
        // Page has changed, use first cached page height.
        return cachedContentHeight_;
    }
    if (!pageStackNode) {
        LOGE("Cannot get pageStackNode!");
        return 0.0;
    }
    auto scrollNode = FindChildOfClass<RenderSingleChildScroll>(pageStackNode);
    if (scrollNode) {
        const auto& child = scrollNode->GetChildren().front();
        if (child) {
            cachedPage_ = pageStackNode;
            cachedContentHeight_ = child->GetLayoutSize().Height() + scrollNode->GetPosition().GetY();
            return cachedContentHeight_;
        }
    }
    LOGE("Cannot get content size!");
    return 0.0;
}

void RenderSemiModal::ResetContentHeight()
{
    if (isExit_) {
        LOGI("semi modal is exit, skip reset content height.");
        return;
    }
    // Reset blank height.
    AnimateTo(dragStartBlankHeight_);
}

void RenderSemiModal::ExtendContentHeight()
{
    if (NearEqual(blankHeight_, minBlankHeight_)) {
        return;
    }
    if (isExit_) {
        LOGI("semi modal is exit, skip extend content height.");
        return;
    }
    AnimateTo(minBlankHeight_);
    animator_->AddStopListener([weak = WeakClaim(this)]() {
        auto semiModal = weak.Upgrade();
        if (semiModal) {
            semiModal->OnExtendAnimationEnd();
        }
    });
}

void RenderSemiModal::OnExtendAnimationEnd()
{
    blankHeight_ = minBlankHeight_;
    dragBar_->ShowArrow(true);
    forbiddenSwipe_ = false;
}

void RenderSemiModal::OnAnimationStop()
{
    isAnimating_ = false;
    isAnimatingToDefault_ = false;
    if (!updateMinBlank_) {
        // if minBlank need update, delay UpdateDragImg.
        UpdateDragImg();
    }
}

void RenderSemiModal::InitializeRecognizer()
{
    if (!clickDetector_) {
        clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickDetector_->SetOnClick([weak = WeakClaim(this)](const ClickInfo& info) {
            auto modal = weak.Upgrade();
            if (modal) {
                modal->HandleClick(info.GetLocalLocation());
            }
        });
    }
    if (!dragDetector_) {
        dragDetector_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
        dragDetector_->SetOnDragStart([weak = WeakClaim(this)](const DragStartInfo& startInfo) {
            auto semiModal = weak.Upgrade();
            if (semiModal) {
                semiModal->HandleDragStart(startInfo.GetLocalLocation());
            }
        });
        dragDetector_->SetOnDragUpdate([weakDrag = AceType::WeakClaim(this)](const DragUpdateInfo& info) {
            auto semiModal = weakDrag.Upgrade();
            if (semiModal) {
                semiModal->HandleDragUpdate(info.GetLocalLocation());
            }
        });
        dragDetector_->SetOnDragEnd([weakDrag = AceType::WeakClaim(this)](const DragEndInfo& info) {
            auto semiModal = weakDrag.Upgrade();
            if (semiModal) {
                semiModal->HandleDragEnd(info.GetLocalLocation());
            }
        });
    }
}

bool RenderSemiModal::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    // Forbidden vertical swipe of all children.
    TouchRestrict newRestrict = touchRestrict;
    if (!isFullScreen_ && forbiddenSwipe_) {
        newRestrict.UpdateForbiddenType(TouchRestrict::SWIPE_VERTICAL);
    }
    return RenderNode::TouchTest(globalPoint, parentLocalPoint, newRestrict, result);
}

void RenderSemiModal::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (isFullScreen_ || isAnimating_) {
        return;
    }
    if (clickDetector_) {
        clickDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(clickDetector_);
    }
    if (dragDetector_) {
        dragDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDetector_);
    }
}

void RenderSemiModal::HandleClick(const Offset& clickPosition) {}

void RenderSemiModal::AnimateToExitApp()
{
    if (isExit_) {
        LOGI("semi modal is exit, skip exit app.");
        return;
    }
    AnimateTo(GetLayoutSize().Height());

    animator_->AddStopListener([weak = context_, semiWeak = AceType::WeakClaim(this)]() {
        auto context = weak.Upgrade();
        if (context) {
            // force finish
            context->Finish(false);
        }
        auto semi = semiWeak.Upgrade();
        if (semi) {
            semi->isExit_ = false;
        }
    });
    isExit_ = true;
    auto root = AceType::DynamicCast<RenderRoot>(GetParent().Upgrade());
    if (!root) {
        LOGE("Exit app animation failed. root is null.");
        return;
    }
    root->AnimateToHide(animator_->GetDuration());
}

void RenderSemiModal::HandleDragStart(const Offset& startPoint)
{
    if (isAnimating_) {
        return;
    }
    dragStartPoint_ = startPoint;
    canHandleDrag_ = dragStartPoint_.GetY() >= blankHeight_;
    dragStartBlankHeight_ = blankHeight_;
}

double RenderSemiModal::UpdateTargetBlankHeight(double oldHeight)
{
    if (oldHeight < minBlankHeight_) {
        oldHeight = minBlankHeight_;
    }

    // Do not drag to make page content height <= zero
    double maxBlankHeight = GetLayoutSize().Height() - navigationHeight_ - CONTENT_MIN_TOLERANCE;
    if (dragBar_) {
        maxBlankHeight -= dragBar_->GetLayoutSize().Height();
    }
    if (oldHeight > maxBlankHeight) {
        oldHeight = maxBlankHeight;
    }
    return oldHeight;
}

void RenderSemiModal::HandleDragUpdate(const Offset& currentPoint)
{
    if (!canHandleDrag_ || isAnimating_) {
        return;
    }
    double targetBlankHeight = dragStartBlankHeight_ + currentPoint.GetY() - dragStartPoint_.GetY();
    blankHeight_ = UpdateTargetBlankHeight(targetBlankHeight);
    UpdateDragImg();
    MarkNeedLayout();
}

void RenderSemiModal::UpdateDragImg()
{
    if (!dragBar_) {
        return;
    }
    auto min = minBlankHeight_ - NormalizeToPx(DRAG_BAR_ARROW_BIAS);
    auto max = minBlankHeight_ + NormalizeToPx(DRAG_BAR_ARROW_BIAS);
    if (blankHeight_ < max && blankHeight_ > min) {
        dragBar_->ShowArrow(true);
    } else {
        dragBar_->ShowArrow(false);
    }
}

void RenderSemiModal::HandleDragEnd(const Offset& endPoint)
{
    if (!canHandleDrag_ || isAnimating_) {
        return;
    }
    auto dragStart = dragStartPoint_.GetY();
    double targetBlankHeight = dragStartBlankHeight_ + endPoint.GetY() - dragStart;
    if (targetBlankHeight <= minBlankHeight_) {
        ExtendContentHeight();
        return;
    }
    auto contentHeight = GetScrollContentHeight();
    if (dragBar_) {
        contentHeight += dragBar_->GetLayoutSize().Height();
    }
    contentHeight += navigationHeight_;
    double dragDownThreshold = 0.0;
    if (!hasInputHeight_) {
        dragDownThreshold =
            std::min(contentHeight, GetLayoutParam().GetMaxSize().Height() * CONTENT_DEFAULT_RATIO) / 2.0;
    } else {
        dragDownThreshold = (GetLayoutSize().Height() - std::min(contentHeight, inputHeight_)) / 2.0;
    }
    if (endPoint.GetY() < dragStart) {
        // Handle drag up.
        bool extendToMax = dragStart - endPoint.GetY() >= NormalizeToPx(DRAG_UP_THRESHOLD);
        if (extendToMax) {
            ExtendContentHeight();
        } else {
            ResetContentHeight();
        }
    } else if (endPoint.GetY() >= dragStart) {
        // Handle drag down.
        bool exitApp = endPoint.GetY() - dragStart >= dragDownThreshold;
        if (exitApp) {
            AnimateToExitApp();
        } else {
            ResetContentHeight();
        }
    }
}

void RenderSemiModal::AppendContentRadiusAnimation(double radius)
{
    // drag bar radius
    auto contentRender = AceType::DynamicCast<RenderBox>(GetChildren().back());
    if (!contentRender) {
        LOGE("Append content radius animation failed. content render is null");
        return;
    }
    auto backDecoration = contentRender->GetBackDecoration();
    auto border = backDecoration->GetBorder();
    auto topLeftRadius = border.TopLeftRadius();
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Append content radius animation failed. context is null");
        return;
    }
    auto keyframeFrom = AceType::MakeRefPtr<Keyframe<double>>(0.0, context->NormalizeToPx(topLeftRadius.GetX()));
    auto keyframeTo = AceType::MakeRefPtr<Keyframe<double>>(1.0, radius);
    auto radiusAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    radiusAnimation->AddKeyframe(keyframeFrom);
    radiusAnimation->AddKeyframe(keyframeTo);
    radiusAnimation->SetCurve(Curves::FRICTION);
    radiusAnimation->AddListener([semiWeak = AceType::WeakClaim(this)](const double& radius) {
        auto semi = semiWeak.Upgrade();
        if (!semi) {
            LOGE("Semi modal is null.");
            return;
        }
        auto contentRender = AceType::DynamicCast<RenderBox>(semi->GetChildren().back());
        if (!contentRender) {
            LOGE("Content Render is null.");
            return;
        }
        auto backDecoration = contentRender->GetBackDecoration();
        if (!backDecoration) {
            LOGE("Back Decoration is null.");
            return;
        }
        auto border = backDecoration->GetBorder();
        border.SetTopLeftRadius(Radius(radius));
        border.SetTopRightRadius(Radius(radius));
        backDecoration->SetBorder(border);
        contentRender->SetBackDecoration(backDecoration);
    });
    animator_->AddInterpolator(radiusAnimation);
}

void RenderSemiModal::AppendBlankHeightAnimation(double blankHeight)
{
    if (LessNotEqual(blankHeight_, 0.0)) {
        LOGE("Append animation failed. Blank height less than zero.");
        return;
    }
    // blankHeight_ -> blankHeight
    auto keyframeFrom = AceType::MakeRefPtr<Keyframe<double>>(0.0, blankHeight_);
    auto keyframeTo = AceType::MakeRefPtr<Keyframe<double>>(1.0, blankHeight);
    auto heightAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    heightAnimation->AddKeyframe(keyframeFrom);
    heightAnimation->AddKeyframe(keyframeTo);
    heightAnimation->SetCurve(Curves::FRICTION);

    heightAnimation->AddListener([semiWeak = AceType::WeakClaim(this)](const double& height) {
        auto semi = semiWeak.Upgrade();
        if (!semi) {
            LOGE("Semi modal is null.");
            return;
        }
        if (LessNotEqual(height, 0.0)) {
            LOGE("Height less than zero, do not animate it.");
            return;
        }
        semi->blankHeight_ = height;
        semi->MarkNeedLayout();
    });
    animator_->AddInterpolator(heightAnimation);
}

void RenderSemiModal::AnimateToFullWindow(int32_t duration)
{
    if (!animator_) {
        LOGE("animate failed. animator is null.");
        return;
    }
    isFullScreen_ = true;
    if (NearZero(blankHeight_)) {
        LOGE("already full window, no need to animate.");
        return;
    }
    if (animator_->IsRunning()) {
        animator_->Finish();
    }
    animator_->ClearInterpolators();
    animator_->ClearAllListeners();
    AppendBlankHeightAnimation(0.0);
    AppendContentRadiusAnimation(0.0);
    if (duration == -1) {
        auto dragRange = GetLayoutParam().GetMaxSize().Height();
        duration = GetAnimationDuration(blankHeight_, dragRange);
    }
    animator_->SetDuration(duration);
    animator_->SetFillMode(FillMode::FORWARDS);
    animator_->Forward();

    if (!dragBar_) {
        LOGE("Animate render drag bar failed. render drag bar is null.");
        return;
    }
    dragBar_->AnimateToStatusBarPadding(duration);
}

void RenderSemiModal::AnimateTo(double blankHeight)
{
    if (isExit_) {
        LOGE("semi modal is exit, skip animate to.");
        return;
    }
    isAnimating_ = true;
    animator_->ClearInterpolators();
    animator_->ClearAllListeners();
    if (animator_->IsRunning()) {
        animator_->Stop();
    }
    animator_->AddStopListener([weak = WeakClaim(this)]() {
        auto semiModal = weak.Upgrade();
        if (semiModal) {
            semiModal->OnAnimationStop();
        }
    });
    AppendBlankHeightAnimation(blankHeight);
    auto dragRange = GetLayoutParam().GetMaxSize().Height();
    animator_->SetDuration(GetAnimationDuration(blankHeight - blankHeight_, dragRange));
    animator_->SetFillMode(FillMode::FORWARDS);
    animator_->Forward();
}

void RenderSemiModal::ShowUpAnimation()
{
    if (isExit_) {
        LOGI("semi modal is exit, skip show up animation.");
        return;
    }
    if (isFullScreen_) {
        auto context = context_.Upgrade();
        if (!context) {
            LOGE("Show up animation failed. context is null.");
            return;
        }
        context->RequestFullWindow(-1);
    } else {
        isAnimatingToDefault_ = true;
        AnimateTo(defaultBlankHeight_);
        auto root = AceType::DynamicCast<RenderRoot>(GetParent().Upgrade());
        if (!root) {
            LOGE("Show up animation failed. root is null.");
            return;
        }
        root->AnimateToShow(animator_->GetDuration());
    }
}

void RenderSemiModal::Dump()
{
    DumpLog::GetInstance().AddDesc(std::string("navigationHeight: ").append(std::to_string(navigationHeight_)));
    DumpLog::GetInstance().AddDesc(std::string("statusBarHeight: ").append(std::to_string(statusBarHeight_)));
    DumpLog::GetInstance().AddDesc(std::string("blankHeight: ").append(std::to_string(blankHeight_)));
    DumpLog::GetInstance().AddDesc(std::string("defaultBlankHeight: ").append(std::to_string(defaultBlankHeight_)));
    DumpLog::GetInstance().AddDesc(std::string("minBlankHeight: ").append(std::to_string(minBlankHeight_)));
    DumpLog::GetInstance().AddDesc(std::string("normalBlankHeight: ").append(std::to_string(normalBlankHeight_)));
}

void RenderSemiModal::MovePage(double delta)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Move failed, context is null");
        return;
    }
    if (!dragBar_) {
        LOGE("Move failed, drag Bar is null");
        return;
    }
    auto parent = dragBar_->GetParent().Upgrade();
    if (!parent) {
        LOGE("Move failed, drag Bar's parent is null");
        return;
    }

    auto iter = parent->GetChildren().begin();
    // get flex child in second position.
    advance(iter, 1);
    if (iter == parent->GetChildren().end()) {
        LOGE("Move failed, flex is null");
        return;
    }
    auto render = (*iter);
    if (!render) {
        LOGE("Move failed, render is null");
        return;
    }
    double newHeight = render->GetGlobalOffset().GetY() + render->GetPaintRect().Height() - delta;
    context->MovePage(Offset(render->GetPaintRect().Width(), newHeight), delta);
}

} // namespace OHOS::Ace
