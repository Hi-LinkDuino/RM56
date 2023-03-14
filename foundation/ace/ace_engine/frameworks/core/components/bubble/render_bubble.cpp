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

#include "core/components/bubble/render_bubble.h"

#include "base/geometry/offset.h"
#include "base/log/event_report.h"
#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "core/accessibility/accessibility_utils.h"
#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/bubble/bubble_element.h"
#include "core/components/stack/stack_element.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension ARROW_WIDTH = 32.0_vp;
constexpr Dimension GRID_MARGIN_PORTRAIT = 48.0_vp;
constexpr Dimension GRID_SPACING = 24.0_vp;
constexpr Dimension GRID_SPACING_TOTAL = 232.0_vp;
constexpr Dimension HORIZON_SPACING_WITH_SCREEN = 6.0_vp;
constexpr int32_t GRID_NUMBER_LANDSCAPE = 8;
constexpr int32_t BUBBLR_GRID_MAX_LANDSCAPE = 6;

} // namespace

const Dimension RenderBubble::BUBBLE_SPACING(8.0, DimensionUnit::VP);

RenderBubble::RenderBubble()
{
    rawDetector_ = AceType::MakeRefPtr<RawRecognizer>();
    rawDetector_->SetOnTouchDown([weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto bubble = weak.Upgrade();
        if (bubble) {
            bubble->HandleTouch();
        }
    });
}

void RenderBubble::Update(const RefPtr<Component>& component)
{
    const auto bubble = AceType::DynamicCast<BubbleComponent>(component);
    if (!bubble) {
        LOGE("RenderBubble update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    if (!bubble->GetPopupParam()) {
        return;
    }
    bubbleComponent_ = bubble;
    maskColor_ = bubble->GetPopupParam()->GetMaskColor();
    backgroundColor_ = bubble->GetPopupParam()->GetBackgroundColor();
    placement_ = bubble->GetPopupParam()->GetPlacement();
    onVisibilityChange_ =
        AceAsyncEvent<void(const std::string&)>::Create(bubble->GetPopupParam()->GetOnVisibilityChange(), context_);
    isShow_ = bubble->GetPopupParam()->IsShow();
    enableArrow_ = bubble->GetPopupParam()->EnableArrow();
    padding_ = bubble->GetPopupParam()->GetPadding();
    margin_ = bubble->GetPopupParam()->GetMargin();
    border_ = bubble->GetPopupParam()->GetBorder();
    arrowOffset_ = bubble->GetPopupParam()->GetArrowOffset();
    targetId_ = bubble->GetPopupParam()->GetTargetId();
    weakStack_ = bubble->GetWeakStack();
    useCustom_ = bubble->GetPopupParam()->IsUseCustom();
    SetDisableTouchEvent(bubble->IsDisabledStatus());
    SetInterceptTouchEvent(bubbleComponent_->GetPopupParam()->HasAction() || bubble->IsDisabledStatus());

    // When app is hide and there is no button in popup, pop popup.
    auto context = context_.Upgrade();
    if (context) {
        context->SetPopupEventHandler([weak = WeakClaim(this)] {
            auto bubble = weak.Upgrade();
            if (bubble) {
                auto bubbleComponent = bubble->bubbleComponent_;
                if (bubbleComponent && !bubbleComponent->GetPopupParam()->HasAction()) {
                    bubble->PopBubble();
                }
            }
        });
    }

    MarkNeedLayout();
}

void RenderBubble::UpdateAccessibilityInfo(Size size, Offset offset)
{
    if (!bubbleComponent_) {
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("RenderBubble context is null");
        return;
    }
    auto viewScale = context->GetViewScale();
    if (NearZero(viewScale)) {
        LOGE("RenderBubble viewScale is zero.");
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGE("RenderBubble accessibilityManager is null");
        return;
    }
    auto nodeId = StringUtils::StringToInt(bubbleComponent_->GetId());
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(nodeId);
    if (!accessibilityNode) {
        LOGE("RenderBubble accessibilityNode is null.");
        return;
    }
    accessibilityNode->SetWidth((size.Width()) * viewScale);
    accessibilityNode->SetHeight((size.Height()) * viewScale);
    accessibilityNode->SetLeft((offset.GetX()) * viewScale);
    accessibilityNode->SetTop((offset.GetY()) * viewScale);
    accessibilityNode->SetLongClickableState(true);
    accessibilityNode->SetClickableState(false);

    accessibilityNode->AddSupportAction(AceAction::ACTION_LONG_CLICK);
    accessibilityNode->SetActionLongClickImpl([weakPtr = WeakClaim(this)]() {
        const auto& bubble = weakPtr.Upgrade();
        if (bubble) {
            bubble->PopBubble();
        }
    });
}

void RenderBubble::PerformLayout()
{
    InitTargetSizeAndPosition();
    SetLayoutSize(GetLayoutParam().GetMaxSize());
    LayoutParam innerLayout = GetLayoutParam();
    if (!useCustom_) {
        if (SystemProperties::GetDevcieOrientation() == DeviceOrientation::PORTRAIT) {
            innerLayout.SetMaxSize(Size(innerLayout.GetMaxSize().Width() - NormalizeToPx(GRID_MARGIN_PORTRAIT),
                innerLayout.GetMaxSize().Height()));
        } else {
            static const int32_t gridGaps = 5;
            double colWidth =
                (innerLayout.GetMaxSize().Width() - NormalizeToPx(GRID_SPACING_TOTAL)) / GRID_NUMBER_LANDSCAPE;
            innerLayout.SetMaxSize(Size(colWidth * BUBBLR_GRID_MAX_LANDSCAPE + NormalizeToPx(GRID_SPACING) * gridGaps,
                innerLayout.GetMaxSize().Height()));
        }
    }
    if (!GetChildren().empty()) {
        const auto& child = GetChildren().front();
        child->Layout(innerLayout);
        childSize_ = child->GetLayoutSize();
        UpdateBorderRadius();
        childOffset_ = GetChildPosition(childSize_);
        child->SetPosition(childOffset_);
        UpdateAccessibilityInfo(childSize_, childOffset_);
    }
}

void RenderBubble::InitTargetSizeAndPosition()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto targetElement = context->GetComposedElementById(targetId_);
    if (!targetElement) {
        LOGE("Get target element by target id return null");
        isShow_ = false;
        return;
    }
    auto targetRender = targetElement->GetRenderNode();
    if (!targetRender) {
        return;
    }
    targetSize_ = targetRender->GetLayoutSize();
    targetOffset_ = targetRender->GetOffsetToPage();
    if (bubbleComponent_ && bubbleComponent_->GetPopupParam()) {
        auto targetMargin = bubbleComponent_->GetPopupParam()->GetTargetMargin();
        targetSize_ -= targetMargin.GetLayoutSizeInPx(context->GetDipScale());
        targetOffset_ += targetMargin.GetOffsetInPx(context->GetDipScale());
    }
}

void RenderBubble::InitArrowState()
{
    if (!enableArrow_) {
        showTopArrow_ = false;
        showBottomArrow_ = false;
        return;
    }

    double arrowWidth = NormalizeToPx(ARROW_WIDTH);
    showTopArrow_ = GreatOrEqual(
        childSize_.Width() -
            std::max(NormalizePercentToPx(padding_.Left(), false), NormalizeToPx(border_.TopLeftRadius().GetX())) -
            std::max(NormalizePercentToPx(padding_.Right(), false), NormalizeToPx(border_.TopRightRadius().GetX())),
        arrowWidth);
    showBottomArrow_ = GreatOrEqual(
        childSize_.Width() -
            std::max(NormalizePercentToPx(padding_.Left(), false), NormalizeToPx(border_.BottomLeftRadius().GetX())) -
            std::max(NormalizePercentToPx(padding_.Right(), false), NormalizeToPx(border_.BottomRightRadius().GetX())),
        arrowWidth);
}

Offset RenderBubble::GetChildPosition(const Size& childSize)
{
    InitArrowState();
    double scaledBubbleSpacing = NormalizeToPx(BUBBLE_SPACING);
    Offset bottomPosition = Offset(targetOffset_.GetX() + (targetSize_.Width() - childSize.Width()) / 2.0,
        targetOffset_.GetY() + targetSize_.Height() + scaledBubbleSpacing + NormalizePercentToPx(margin_.Top(), true));
    if (showBottomArrow_) {
        bottomPosition += Offset(0.0, scaledBubbleSpacing);
    }
    Offset topPosition = Offset(targetOffset_.GetX() + (targetSize_.Width() - childSize.Width()) / 2.0,
        targetOffset_.GetY() - childSize.Height() - scaledBubbleSpacing - NormalizePercentToPx(margin_.Bottom(), true));
    if (showTopArrow_) {
        topPosition += Offset(0.0, -scaledBubbleSpacing);
    }
    Offset topArrowPosition = Offset(targetOffset_.GetX() + targetSize_.Width() / 2.0,
        targetOffset_.GetY() - scaledBubbleSpacing - NormalizePercentToPx(margin_.Bottom(), true));
    Offset bottomArrowPosition = Offset(targetOffset_.GetX() + targetSize_.Width() / 2.0,
        targetOffset_.GetY() + targetSize_.Height() + scaledBubbleSpacing + NormalizePercentToPx(margin_.Top(), true));
    Offset originOffset =
        GetPositionWithPlacement(childSize, topPosition, bottomPosition, topArrowPosition, bottomArrowPosition);
    Offset childPosition = originOffset;
    arrowPlacement_ = placement_;

    // Fit popup to screen range.
    ErrorPositionType errorType = GetErrorPositionType(childPosition, childSize);
    if (errorType == ErrorPositionType::NORMAL) {
        return childPosition;
    }
    // If childPosition is error, adjust bubble to bottom.
    if (placement_ != Placement::TOP || errorType == ErrorPositionType::TOP_LEFT_ERROR) {
        childPosition = FitToScreen(bottomPosition, childSize);
        arrowPosition_ = bottomArrowPosition + (childPosition - bottomPosition);
        arrowPlacement_ = Placement::BOTTOM;
        if (GetErrorPositionType(childPosition, childSize) == ErrorPositionType::NORMAL) {
            return childPosition;
        }
    }
    // If childPosition is error, adjust bubble to top.
    childPosition = FitToScreen(topPosition, childSize);
    arrowPosition_ = topArrowPosition + (childPosition - topPosition);
    arrowPlacement_ = Placement::TOP;
    if (GetErrorPositionType(childPosition, childSize) == ErrorPositionType::NORMAL) {
        return childPosition;
    }
    // If childPosition is error, adjust bubble to origin position.
    arrowPlacement_ = placement_;
    arrowPosition_ = arrowPlacement_ == Placement::TOP ? topArrowPosition : bottomArrowPosition;
    return originOffset;
}

Offset RenderBubble::GetPositionWithPlacement(const Size& childSize, const Offset& topPosition,
    const Offset& bottomPosition, const Offset& topArrowPosition, const Offset& bottomArrowPosition)
{
    Offset childPosition;
    double bubbleSpacing = NormalizeToPx(BUBBLE_SPACING);
    switch (placement_) {
        case Placement::LEFT:
            childPosition = Offset(
                targetOffset_.GetX() - childSize.Width() - bubbleSpacing - NormalizePercentToPx(margin_.Right(), false),
                targetOffset_.GetY() + targetSize_.Height() / 2.0 - childSize.Height() / 2.0);
            break;
        case Placement::RIGHT:
            childPosition = Offset(targetOffset_.GetX() + targetSize_.Width() + bubbleSpacing +
                                       NormalizePercentToPx(margin_.Left(), false),
                targetOffset_.GetY() + targetSize_.Height() / 2.0 - childSize.Height() / 2.0);
            break;
        case Placement::TOP:
            childPosition = topPosition;
            arrowPosition_ = topArrowPosition;
            break;
        case Placement::BOTTOM:
            childPosition = bottomPosition;
            arrowPosition_ = bottomArrowPosition;
            break;
        case Placement::TOP_LEFT:
            childPosition =
                Offset(targetOffset_.GetX() - childSize.Width() - NormalizePercentToPx(margin_.Right(), false),
                    targetOffset_.GetY() - childSize.Height() - NormalizePercentToPx(margin_.Bottom(), true));
            break;
        case Placement::TOP_RIGHT:
            childPosition =
                Offset(targetOffset_.GetX() + targetSize_.Width() + NormalizePercentToPx(margin_.Left(), false),
                    targetOffset_.GetY() - childSize.Height() - NormalizePercentToPx(margin_.Bottom(), true));
            break;
        case Placement::BOTTOM_LEFT:
            childPosition =
                Offset(targetOffset_.GetX() - childSize.Width() - NormalizePercentToPx(margin_.Right(), false),
                    targetOffset_.GetY() + targetSize_.Height() + NormalizePercentToPx(margin_.Top(), true));
            break;
        case Placement::BOTTOM_RIGHT:
            childPosition =
                Offset(targetOffset_.GetX() + targetSize_.Width() + NormalizePercentToPx(margin_.Left(), false),
                    targetOffset_.GetY() + targetSize_.Height() + NormalizePercentToPx(margin_.Top(), true));
            break;
        default:
            break;
    }
    return childPosition;
}

Offset RenderBubble::FitToScreen(const Offset& fitPosition, const Size& childSize)
{
    auto validation = GetErrorPositionType(fitPosition, childSize);
    if (validation == ErrorPositionType::NORMAL) {
        return fitPosition;
    }
    Offset childPosition = fitPosition;
    double horizonSpacing = NormalizeToPx(HORIZON_SPACING_WITH_SCREEN);
    if (validation == ErrorPositionType::TOP_LEFT_ERROR) {
        childPosition.SetX(horizonSpacing);
    } else {
        childPosition.SetX(GetLayoutSize().Width() - childSize.Width() - horizonSpacing);
    }
    return childPosition;
}

RenderBubble::ErrorPositionType RenderBubble::GetErrorPositionType(const Offset& childOffset, const Size& childSize)
{
    double horizonSpacing = NormalizeToPx(HORIZON_SPACING_WITH_SCREEN);
    TouchRegion validRegion = TouchRegion(
        Offset(horizonSpacing, 0.0), Offset(GetLayoutSize().Width() - horizonSpacing, GetLayoutSize().Height()));
    if (!validRegion.ContainsInRegion(childOffset.GetX(), childOffset.GetY())) {
        return ErrorPositionType::TOP_LEFT_ERROR;
    }
    if (!validRegion.ContainsInRegion(
            childOffset.GetX() + childSize.Width(), childOffset.GetY() + childSize.Height())) {
        return ErrorPositionType::BOTTOM_RIGHT_ERROR;
    }
    return ErrorPositionType::NORMAL;
}

void RenderBubble::OnHiddenChanged(bool hidden)
{
    if (!bubbleComponent_ || !bubbleComponent_->GetPopupParam()) {
        return;
    }
    // When page is hidden and there is no button in popup, pop bubble.
    if (hidden && !bubbleComponent_->GetPopupParam()->HasAction()) {
        PopBubble();
    }
}

void RenderBubble::HandleTouch()
{
    if (!bubbleComponent_ || !bubbleComponent_->GetPopupParam()) {
        return;
    }
    if (!bubbleComponent_->GetPopupParam()->HasAction()) {
        PopBubble();
        UpdateAccessibilityInfo(Size(), Offset());
    }
}

void RenderBubble::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    rawDetector_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(rawDetector_);
}

bool RenderBubble::PopBubble()
{
    auto stackElement = weakStack_.Upgrade();
    if (!stackElement) {
        return false;
    }
    stackElement->PopPopup(bubbleComponent_->GetId());
    auto stateChangeEvent = bubbleComponent_->GetStateChangeEvent();
    if (stateChangeEvent) {
        stateChangeEvent(false);
    }

    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    const auto& accessibilityManager = context->GetAccessibilityManager();
    if (accessibilityManager) {
        accessibilityManager->RemoveAccessibilityNodeById(StringUtils::StringToInt(bubbleComponent_->GetId()));
    }
#else
    const auto& accessibilityManager = context->GetAccessibilityManager();
    if (accessibilityManager) {
        auto bubbleNodeId = StringUtils::StringToInt(bubbleComponent_->GetId());
        auto node = accessibilityManager->GetAccessibilityNodeById(bubbleNodeId);
        if (node) {
            auto children = node->GetChildList();
            for (auto& child : children) {
                child->ClearRect();
            }
        }
    }
#endif
    return true;
}

void RenderBubble::FirePopEvent()
{
    if (onVisibilityChange_) {
        std::string param = std::string("\"visibilitychange\",{\"visibility\":").append("false}");
        onVisibilityChange_(param);
    }
}

bool RenderBubble::HandleMouseEvent(const MouseEvent& event)
{
    if (event.button != MouseButton::NONE_BUTTON && event.button != MouseButton::LEFT_BUTTON &&
        event.action == MouseAction::PRESS) {
        HandleTouch();
    }
    return true;
}

} // namespace OHOS::Ace