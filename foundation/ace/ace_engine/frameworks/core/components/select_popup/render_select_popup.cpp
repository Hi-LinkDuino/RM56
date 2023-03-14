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

#include "core/components/select_popup/render_select_popup.h"

#include "core/components/flex/flex_component.h"
#include "core/components/flex/render_flex.h"
#include "core/components/option/render_option.h"
#include "core/components/select/select_theme.h"
#include "core/components/select_popup/select_popup_component.h"
#include "core/components/stack/stack_element.h"
#include "core/components/text/render_text.h"
#include "core/gestures/raw_recognizer.h"

namespace OHOS::Ace {
namespace {
constexpr int32_t DEFAULT_DISTANCE = 5;
} // namespace

RenderSelectPopup::RenderSelectPopup()
{
    rawDetector_ = AceType::MakeRefPtr<RawRecognizer>();
    rawDetector_->SetOnTouchDown([weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto selectPopup = weak.Upgrade();
        if (selectPopup) {
            selectPopup->ProcessTouchDown(info);
            selectPopup->HandleRawEvent(info.GetTouches().front().GetLocalLocation());
        }
    });
}

void RenderSelectPopup::OnPaintFinish()
{
    auto pipeline = context_.Upgrade();
    if (!selectPopup_ || !selectPopup_->GetNode() || !pipeline || !renderPositioned_) {
        LOGE("can not get accessibility node of select popup or pipeline is null or positional is null.");
        return;
    }
    auto node = selectPopup_->GetNode();
    auto viewScale = pipeline->GetViewScale();
    auto leftTop = renderPositioned_->GetGlobalOffset();
    node->SetLeft(leftTop.GetX() * viewScale);
    node->SetTop(leftTop.GetY() * viewScale);
    auto size = renderPositioned_->GetLayoutSize();
    node->SetWidth(size.Width() * viewScale);
    node->SetHeight(size.Height() * viewScale);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto parentNode = node->GetParentNode();
    if (parentNode && parentNode->GetTag() == "menu") {
        parentNode->SetLeft(leftTop.GetX() * viewScale);
        parentNode->SetTop(leftTop.GetY() * viewScale);
        parentNode->SetWidth(size.Width() * viewScale);
        parentNode->SetHeight(size.Height() * viewScale);
    }
#endif
    node->SetEnabledState(true);
    node->SetCheckableState(false);
    node->SetClickableState(false);
    node->SetFocusableState(false);
}

void RenderSelectPopup::Update(const RefPtr<Component>& component)
{
    auto popup = AceType::DynamicCast<SelectPopupComponent>(component);
    if (!popup || !popup->GetTheme()) {
        LOGE("select: input component is null or not SelectPopupComponent.");
        return;
    }

    theme_ = popup->GetTheme();
    tvBackColor_ = theme_->GetTvBackColor();
    selectPopup_ = popup;
    popup->GetPopupLayout(selectLeftTop_, selectRightBottom_);
    optionSize_ = popup->GetOptionSize();
    rrectSize_ = popup->GetPopupRRectSize();
    optionInterval_ = theme_->GetOptionInterval();
    minWidth_ = popup->GetPopupMinWidth();
    horizontalSpacing_ = popup->GetHorizontalSpacing();
    verticalSpacing_ = popup->GetVerticalSpacing();
    contentSpacing_ = popup->GetContentSpacing();
    isFullScreen_ = popup->IsFullScreen();
    isContextMenu_ = popup->IsContextMenu();
    MarkNeedLayout();
}

void RenderSelectPopup::AdjustTvChildVerticalLayout(const Size& size, double& y, double& height)
{
    double bottomSpace = globalRightBottom_.GetY() - selectLeftTop_.GetY();
    if (bottomSpace >= size.Height() + verticalSpacing_.Value()) {
        y = selectLeftTop_.GetY();
        height = size.Height();
        return;
    }
    bottomSpace = globalRightBottom_.GetY();
    if (bottomSpace >= size.Height() + 2.0 * verticalSpacing_.Value()) {
        height = size.Height();
        y = bottomSpace - verticalSpacing_.Value() - height;
        return;
    }
    height = bottomSpace - 2.0 * verticalSpacing_.Value();
    y = verticalSpacing_.Value();
}

void RenderSelectPopup::AdjustTvChildHorizontalLayout(const Size& size, double& x, double& width)
{
    double leftSpace = selectLeftTop_.GetX();
    double rightSpace = globalRightBottom_.GetX() - selectRightBottom_.GetX();
    if (selectPopup_ && selectPopup_->GetTextDirection() == TextDirection::RTL) {
        if (leftSpace >= contentSpacing_.Value() + size.Width() + horizontalSpacing_.Value()) {
            width = size.Width();
            x = selectLeftTop_.GetX() - contentSpacing_.Value() - width;
            return;
        }
        if (rightSpace <= leftSpace) {
            width = leftSpace - contentSpacing_.Value() - horizontalSpacing_.Value();
            x = horizontalSpacing_.Value();
            return;
        }
        x = selectRightBottom_.GetX() + contentSpacing_.Value();
        if (rightSpace >= contentSpacing_.Value() + size.Width() + horizontalSpacing_.Value()) {
            width = size.Width();
            return;
        }
        width = rightSpace - contentSpacing_.Value() - horizontalSpacing_.Value();
        return;
    }

    if (rightSpace >= contentSpacing_.Value() + size.Width() + horizontalSpacing_.Value()) {
        width = size.Width();
        x = selectRightBottom_.GetX() + contentSpacing_.Value();
        return;
    }
    if (leftSpace <= rightSpace) {
        width = rightSpace - contentSpacing_.Value() - horizontalSpacing_.Value();
        x = selectRightBottom_.GetX() + contentSpacing_.Value();
        return;
    }
    if (leftSpace >= contentSpacing_.Value() + size.Width() + horizontalSpacing_.Value()) {
        width = size.Width();
        x = selectLeftTop_.GetX() - contentSpacing_.Value() - width;
        return;
    }
    width = leftSpace - contentSpacing_.Value() - horizontalSpacing_.Value();
    x = horizontalSpacing_.Value();
}

void RenderSelectPopup::AdjustChildVerticalLayout(const Size& size, double& y, double& height)
{
    if (selectPopup_ && selectPopup_->IsMenu() && selectPopup_->IsTV()) {
        AdjustTvChildVerticalLayout(size, y, height);
        y -= NormalizeToPx(optionInterval_);
        y -= NormalizeToPx(rrectSize_);
        return;
    }

    double bottomSpace = globalRightBottom_.GetY() - selectRightBottom_.GetY();
    double topSpace = selectLeftTop_.GetY();
    // think about top padding and bottom padding
    if (bottomSpace >= size.Height() + 2.0 * normalPadding_) {
        y = selectRightBottom_.GetY() + normalPadding_;
        height = size.Height();
        return;
    }

    if (bottomSpace >= topSpace) {
        y = selectRightBottom_.GetY() + normalPadding_;
        // think about top padding and bottom padding
        height = bottomSpace - 2.0 * normalPadding_;
        return;
    }

    // think about top padding and bottom padding
    if (topSpace >= size.Height() + 2.0 * normalPadding_) {
        height = size.Height();
        y = selectLeftTop_.GetY() - normalPadding_ - height;
        return;
    }

    y = normalPadding_;
    // think about top padding and bottom padding
    height = topSpace - 2.0 * normalPadding_;
}

void RenderSelectPopup::AdjustChildHorizontalLayout(const Size& size, double& x, double& width)
{
    double rightSpace = globalRightBottom_.GetX() - selectLeftTop_.GetX();
    if (selectRightBottom_ != selectLeftTop_) {
        if (selectPopup_ && selectPopup_->IsMenu() && selectPopup_->IsTV()) {
            AdjustTvChildHorizontalLayout(size, x, width);
            return;
        }
        if (selectPopup_ && selectPopup_->GetTextDirection() == TextDirection::RTL) {
            // align right for select popup dialog
            double space = selectRightBottom_.GetX();
            width = (space > size.Width() + normalPadding_ ? size.Width() : space - normalPadding_);
            x = space - width;
        } else {
            x = selectLeftTop_.GetX();
            width = (rightSpace > size.Width() + normalPadding_ ? size.Width() : rightSpace - normalPadding_);
        }
        return;
    }

    double leftSpace = selectLeftTop_.GetX();
    // think about left padding and right padding
    if (rightSpace >= size.Width() + 2.0 * normalPadding_) {
        x = selectLeftTop_.GetX() + normalPadding_;
        width = size.Width();
        return;
    }

    if (rightSpace >= leftSpace) {
        x = selectLeftTop_.GetX() + normalPadding_;
        // think about left padding and right padding
        width = rightSpace - 2.0 * normalPadding_;
        return;
    }

    // think about left padding and right padding
    if (leftSpace >= size.Width() + 2.0 * normalPadding_) {
        width = size.Width();
        x = selectLeftTop_.GetX() - normalPadding_ - width;
        return;
    }

    x = normalPadding_;
    // think about left padding and right padding
    width = leftSpace - 2.0 * normalPadding_;
}

void RenderSelectPopup::AdjustChildLayout(Size& size)
{
    double x = 0.0;
    double width = 0.0;
    AdjustChildHorizontalLayout(size, x, width);
    double y = 0.0;
    double height = 0.0;
    AdjustChildVerticalLayout(size, y, height);
    childLayoutParam_.SetFixedSize(Size(width, height));
    size.SetWidth(width);
    size.SetHeight(height);
    childPosition_.SetX(x);
    childPosition_.SetY(y);
}

void RenderSelectPopup::CreateAnimation()
{
    if (animationCreated_) {
        return;
    }
    if (selectPopup_) {
        // When the popup is used for contextmenu, add the animation, the same with menu.
        CreatePopupAnimation(selectPopup_->IsMenu() || selectPopup_->IsContextMenu());
    }
    animationCreated_ = true;
}

void RenderSelectPopup::CreatePopupAnimation(bool isMenu)
{
    if (!selectPopup_ || !selectPopup_->GetTheme()) {
        LOGE("select theme or select popup component is null.");
        return;
    }

    auto theme = selectPopup_->GetTheme();
    auto showScaleAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(0.9f, 1.0f, Curves::FRICTION);
    auto showAlphaAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, 1.0f, Curves::FAST_OUT_SLOW_IN);
    TweenOption showOption;
    showOption.SetDuration(theme->GetShowTime(isMenu));
    showOption.SetTransformFloatAnimation(AnimationType::SCALE, showScaleAnimation);
    showOption.SetOpacityAnimation(showAlphaAnimation);
    selectPopup_->SetShowOption(showOption);

    auto hideScaleAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 0.9f, Curves::FRICTION);
    auto hideAlphaAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 0.0f, Curves::FAST_OUT_SLOW_IN);
    TweenOption hideOption;
    hideOption.SetDuration(theme->GetHideTime(isMenu));
    hideOption.SetTransformFloatAnimation(AnimationType::SCALE, hideScaleAnimation);
    hideOption.SetOpacityAnimation(hideAlphaAnimation);
    selectPopup_->SetHideOption(hideOption);
}

void RenderSelectPopup::PerformLayout()
{
    if (ScreenDirectionSwitched()) {
        return;
    }
    if (!renderRoot_ || !renderScroll_ || (isFullScreen_ && !renderPositioned_)) {
        LOGE("render child is null.");
        return;
    }
    verticalSpacing_ = Dimension(NormalizeToPx(verticalSpacing_), DimensionUnit::PX);
    horizontalSpacing_ = Dimension(NormalizeToPx(horizontalSpacing_), DimensionUnit::PX);
    contentSpacing_ = Dimension(NormalizeToPx(contentSpacing_), DimensionUnit::PX);
    normalPadding_ = NormalizeToPx(rrectSize_);
    globalRightBottom_ = Offset() + renderRoot_->GetLayoutSize();
    double outPadding = NormalizeToPx(4.0_vp); // the out padding is 4dp from doc.
    Size totalSize;
    double fixHeight = 0.0;
    if (renderTitleBox_) {
        renderTitleBox_->Layout(LayoutParam());
        totalSize = renderTitleBox_->GetLayoutSize();
        // add 8.0dp delta width for title so that it will show full title.
        totalSize.SetWidth(totalSize.Width() + NormalizeToPx(8.0_vp));
        fixHeight = totalSize.Height();
    }
    for (const auto& option : renderOptions_) {
        if (selectLeftTop_ != selectRightBottom_ && selectPopup_ && !selectPopup_->IsMenu()) {
            option->SetMaxWidth(
                (selectPopup_->GetTextDirection() == TextDirection::RTL
                        ? selectRightBottom_.GetX() - normalPadding_ - 2.0 * outPadding
                        : globalRightBottom_.GetX() - selectLeftTop_.GetX() - normalPadding_ - 2.0 * outPadding));
        } else if (selectLeftTop_ == selectRightBottom_ && selectPopup_ && selectPopup_->IsMenu()) {
            auto leftSpace = selectLeftTop_.GetX();
            auto rightSpace = globalRightBottom_.GetX() - leftSpace;
            leftSpace -= 2.0 * normalPadding_ + 2.0 * outPadding;
            rightSpace -= 2.0 * normalPadding_ + 2.0 * outPadding;
            option->SetMaxWidth(std::max(leftSpace, rightSpace));
        }

        option->Layout(LayoutParam());
        if (option->GetLayoutSize().Width() > totalSize.Width()) {
            totalSize.SetWidth(option->GetLayoutSize().Width());
        }
        totalSize.AddHeight(option->GetLayoutSize().Height());
    }
    if (totalSize.Width() < NormalizeToPx(minWidth_)) {
        totalSize.SetWidth(NormalizeToPx(minWidth_));
    }
    totalSize.AddHeight(2.0 * outPadding);
    totalSize.AddWidth(2.0 * outPadding);

    AdjustChildLayout(totalSize);
    for (const auto& option : renderOptions_) {
        // leave the space of 8.0dp for interval of innner and out box.
        option->SetFixedWidth(totalSize.Width() - 2.0 * outPadding);
    }
    if (isFullScreen_) {
        renderPositioned_->SetPosition(childPosition_);
        renderPositioned_->Layout(childLayoutParam_);
    } else {
        if (GetChildren().front()) {
            GetChildren().front()->Layout(childLayoutParam_);
        }
    }
    LayoutParam scrollLayout;
    scrollLayout.SetFixedSize(
        Size(totalSize.Width() - 2.0 * outPadding, totalSize.Height() - fixHeight - 2.0 * outPadding));
    renderScroll_->Layout(scrollLayout);
    if (renderTitleBox_) {
        LayoutParam layout;
        layout.SetFixedSize(Size(totalSize.Width() - 2.0 * outPadding, renderTitleBox_->GetLayoutSize().Height()));
        renderTitleBox_->Layout(layout);
    }

    if (isFullScreen_) {
        SetLayoutSize(GetLayoutParam().GetMaxSize());
        touchRegion_ = TouchRegion(
            renderPositioned_->GetPosition(), renderPositioned_->GetPosition() + renderPositioned_->GetLayoutSize());
    } else {
        if (GetChildren().front()) {
            SetLayoutSize(GetChildren().front()->GetLayoutSize());
        }
        touchRegion_ = TouchRegion(GetPosition(), GetPosition() + GetLayoutSize());
    }

    CreateAnimation();
}

void RenderSelectPopup::HandleRawEvent(const Offset& clickPosition)
{
    LOGD("Handle Raw Event, Position is %{public}s.", clickPosition.ToString().c_str());
    if (touchRegion_.ContainsInRegion(clickPosition.GetX(), clickPosition.GetY())) {
        LOGI("Contains the touch region.");
        return;
    }

    if (!selectPopup_) {
        return;
    }
    if (isContextMenu_) {
        LOGI("Hide the contextmenu.");
        selectPopup_->CloseContextMenu();
        return;
    }
    selectPopup_->HideDialog(SELECT_INVALID_INDEX);
}

void RenderSelectPopup::ProcessTouchDown(const TouchEventInfo& info)
{
    LOGI("ProcessTouchDown");
    auto touches = info.GetTouches();
    if (touches.empty()) {
        LOGE("touch event info is empty.");
        return;
    }

    auto clickPosition = touches.front().GetLocalLocation();
    if (!touchRegion_.ContainsInRegion(clickPosition.GetX(), clickPosition.GetY())) {
        LOGI("Do not contains the touch region.");
        return;
    }

    firstFingerDownOffset_ = touches.front().GetGlobalLocation();
}

void RenderSelectPopup::ProcessTouchUp(const TouchEventInfo& info)
{
    LOGI("ProcessTouchUp");
    auto touches = info.GetTouches();
    if (touches.empty()) {
        LOGE("touch event info is empty.");
        return;
    }

    auto clickPosition = touches.front().GetLocalLocation();
    if (!touchRegion_.ContainsInRegion(clickPosition.GetX(), clickPosition.GetY())) {
        LOGI("Do not contains the touch region.");
        return;
    }

    if (selectPopup_->GetSelectOptions().empty()) {
        return;
    }

    auto firstOption = selectPopup_->GetSelectOptions().front();
    if (!firstOption->GetCustomComponent()) {
        return;
    }

    auto offset = touches.front().GetGlobalLocation();
    firstFingerUpOffset_ = offset;
    if ((offset - firstFingerDownOffset_).GetDistance() <= DEFAULT_DISTANCE) {
        if (isContextMenu_) {
            selectPopup_->CloseContextMenu();
        } else {
            selectPopup_->HideDialog(SELECT_INVALID_INDEX);
        }
        firstFingerDownOffset_ = Offset();
    }
}

void RenderSelectPopup::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!dragDetector_) {
        dragDetector_ = AceType::MakeRefPtr<FreeDragRecognizer>();
    }
    if (!longPressDetector_) {
        longPressDetector_ = AceType::MakeRefPtr<LongPressRecognizer>(context_);
    }
    if (!clickDetector_) {
        clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
    }

    rawDetector_->SetOnTouchUp([weak = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        auto ref = weak.Upgrade();
        if (!ref) {
            LOGE("renderSelectPopup upgrade fail.");
            return;
        }

        ref->ProcessTouchUp(info);
    });

    rawDetector_->SetOnTouchCancel([weak = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        auto ref = weak.Upgrade();
        if (!ref) {
            LOGE("renderSelectPopup upgrade fail.");
            return;
        }

        ref->ProcessTouchUp(info);
    });

    rawDetector_->SetCoordinateOffset(coordinateOffset);
    dragDetector_->SetCoordinateOffset(coordinateOffset);
    longPressDetector_->SetCoordinateOffset(coordinateOffset);
    clickDetector_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(rawDetector_);
    result.emplace_back(dragDetector_);
    result.emplace_back(longPressDetector_);
    result.emplace_back(clickDetector_);
}

void RenderSelectPopup::GetReferenceRenders()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto pageElement = context->GetLastStack();
    if (!pageElement) {
        return;
    }
    renderRoot_ = pageElement->GetRenderNode();
    renderOptions_.clear();
    GetReferenceRenders(AceType::Claim(this));
}

void RenderSelectPopup::GetReferenceRenders(const RefPtr<RenderNode>& render)
{
    if (!render) {
        return;
    }

    if (AceType::InstanceOf<RenderBox>(render)) {
        auto boxParent = render->GetParent().Upgrade();
        if (boxParent && AceType::InstanceOf<RenderFlex>(boxParent)) {
            renderTitleBox_ = AceType::DynamicCast<RenderBox>(render);
            return;
        }
    }

    if (AceType::InstanceOf<RenderOption>(render)) {
        renderOptions_.emplace_back(AceType::DynamicCast<RenderOption>(render));
        return;
    }

    if (AceType::InstanceOf<RenderPositioned>(render)) {
        renderPositioned_ = AceType::DynamicCast<RenderPositioned>(render);
    }

    if (AceType::InstanceOf<RenderScroll>(render)) {
        renderScroll_ = AceType::DynamicCast<RenderScroll>(render);
    }

    for (const auto& child : render->GetChildren()) {
        GetReferenceRenders(child);
    }
}

void RenderSelectPopup::ClearReferenceRenders()
{
    renderRoot_ = nullptr;
    renderPositioned_ = nullptr;
    renderTitleBox_ = nullptr;
    renderScroll_ = nullptr;
    renderOptions_.clear();
}

bool RenderSelectPopup::ScreenDirectionSwitched()
{
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return false;
    }

    bool check = GreatNotEqual(pipeline->GetRootWidth(), pipeline->GetRootHeight());
    bool switched = (!screenHorizontal_ && !screenVertical_) ? false : (check ? screenVertical_ : screenHorizontal_);
    screenHorizontal_ = check;
    screenVertical_ = !check;
    if (switched && selectPopup_) {
        selectPopup_->HideDialog(SELECT_INVALID_INDEX);
    }
    return switched;
}

bool RenderSelectPopup::HandleMouseEvent(const MouseEvent& event)
{
    if (event.button != MouseButton::NONE_BUTTON && event.button != MouseButton::LEFT_BUTTON &&
        event.action == MouseAction::PRESS) {
        HandleRawEvent({ event.x, event.y });
    }
    return true;
}

} // namespace OHOS::Ace
