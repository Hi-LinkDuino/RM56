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

#include "core/components/option/render_option.h"

#include "base/log/log.h"
#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "core/components/scroll/render_scroll.h"
#include "core/components/select_popup/render_select_popup.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

const Dimension VERTICAL_INTERVAL_PHONE = 14.4_vp;
const Dimension HORIZONTAL_INTERVAL_PHONE = 12.0_vp;
const Dimension HORIZONTAL_DISTANCE_PHONE = 8.0_vp;
const Dimension ROUND_RADIUS_PHONE = 12.0_vp;
const Dimension ROUND_RADIUS_TV = 8.0_vp;

} // namespace

RefPtr<RenderText> RenderOption::GetRenderText(const RefPtr<RenderNode>& render) const
{
    if (!render) {
        return nullptr;
    }
    if (AceType::InstanceOf<RenderText>(render)) {
        return AceType::DynamicCast<RenderText>(render);
    }
    for (const auto& child : render->GetChildren()) {
        auto text = GetRenderText(child);
        if (text) {
            return text;
        }
    }
    return nullptr;
}

RefPtr<RenderImage> RenderOption::GetRenderImage(const RefPtr<RenderNode>& render) const
{
    if (!render) {
        return nullptr;
    }
    if (AceType::InstanceOf<RenderImage>(render)) {
        return AceType::DynamicCast<RenderImage>(render);
    }
    for (const auto& child : render->GetChildren()) {
        auto image = GetRenderImage(child);
        if (image) {
            return image;
        }
    }
    return nullptr;
}

bool RenderOption::OnBack()
{
    if (!data_) {
        return false;
    }

    auto clickCallback = data_->GetClickedCallback();
    if (!clickCallback) {
        return false;
    }

    clickCallback(SELECT_INVALID_INDEX);
    return true;
}

void RenderOption::OnFocus(bool focus)
{
    if (!data_) {
        return;
    }

    // lost focus => just update status.
    if (!focus) {
        data_->SetFocused(false);
        data_->SetSelected(false);
        UpdateStatus();
        return;
    }

    // is not auto focus on popup dialog
    if (data_->GetIndex() != 0 || focusJumped_) {
        data_->SetFocused(true);
        UpdateStatus();
        AdjustScrollPosition();
        return;
    }

    // auto focus on popup dialog
    focusJumped_ = true;
    auto options = GetAllOptions();
    for (const auto& option : options) {
        if (!option->GetSelected()) {
            continue;
        }
        // auto focus on selected option.
        if (AceType::RawPtr(option) == this) {
            // auto jump to self which index is 0 and selected.
            data_->SetFocused(true);
            UpdateStatus();
            return;
        }
        option->RequestFocus();
        return;
    }

    data_->SetFocused(true);
    UpdateStatus();
}

void RenderOption::RequestFocus()
{
    auto node = weakNode_.Upgrade();
    if (!node) {
        return;
    }
    node->RequestFocus();
}

void RenderOption::GetAllOptions(std::list<RefPtr<RenderOption>>& result, const RefPtr<RenderNode>& parent) const
{
    if (!parent) {
        return;
    }

    auto option = AceType::DynamicCast<RenderOption>(parent);
    if (option) {
        result.emplace_back(option);
    }

    for (const auto& child : parent->GetChildren()) {
        GetAllOptions(result, child);
    }
}

std::list<RefPtr<RenderOption>> RenderOption::GetAllOptions() const
{
    std::list<RefPtr<RenderOption>> result;
    RefPtr<RenderNode> parent = GetParent().Upgrade();
    while (parent && !AceType::InstanceOf<RenderBox>(parent)) {
        parent = parent->GetParent().Upgrade();
    }
    GetAllOptions(result, parent);
    return result;
}

void RenderOption::OnClick(bool focus)
{
    if (!data_ || data_->GetDisabled()) {
        return;
    }

    auto options = GetAllOptions();
    for (const auto& option : options) {
        option->OnSelect(data_->GetIndex());
    }

    auto clickCallback = data_->GetClickedCallback();
    if (!clickCallback) {
        return;
    }

    clickCallback(data_->GetIndex());

    if (onClickEvent_) {
        onClickEvent_();
    }
}

void RenderOption::OnSelect(uint32_t selectIndex)
{
    if (!data_) {
        return;
    }

    if (data_->GetIndex() == selectIndex) {
        data_->SetClicked(false);
        data_->SetSelected(true);
    } else {
        data_->SetSelected(false);
    }
    UpdateStatus();
}

void RenderOption::OnTouch(bool down)
{
    if (!data_ || data_->GetDisabled() || data_->GetCustomComponent()) {
        return;
    }

    data_->SetClicked(down);
    UpdateStatus();
    Color endColor;
    if (down) {
        endColor = clickedColor_;
    } else if (hovered_) {
        endColor = hoveredColor_;
    } else {
        endColor = Color::TRANSPARENT;
    }
    PlayEventEffectAnimation(endColor, PRESS_DURATION);
}

void RenderOption::OnMouseHoverEnterTest()
{
    if (!data_ || data_->GetDisabled()) {
        return;
    }

    hovered_ = true;
    UpdateStatus();
    PlayEventEffectAnimation(hoveredColor_, HOVER_DURATION);
}

void RenderOption::OnMouseHoverExitTest()
{
    if (!data_ || data_->GetDisabled()) {
        return;
    }

    hovered_ = false;
    UpdateStatus();
    PlayEventEffectAnimation(Color::TRANSPARENT, HOVER_DURATION, true);
}

void RenderOption::UpdateStatus()
{
    UpdateSelfStatus();
    UpdateDownStatus();
}

void RenderOption::UpdateSelfStatus()
{
    if (!data_ || data_->GetDisabled()) {
        return;
    }
    // tv focus > press(clicked) > hover > select > normal
    if (isTv_ && data_->GetFocused()) {
        UpdateTvFocusedStatus();
        return;
    }
    if (data_->GetClicked()) {
        UpdateClickedStatus();
        return;
    }
    if (hovered_) {
        UpdateHoveredStatus();
        return;
    }
    if (data_->GetSelected()) {
        UpdateSelectedStatus();
        return;
    }

    UpdateOthersStatus();
    return;
}

void RenderOption::UpdateDownStatus()
{
    auto downOption = GetDownOption();
    if (!downOption) {
        return;
    }
    downOption->UpdateSelfStatus();
}

void RenderOption::UpdateClickedStatus()
{
    needLine_ = false;
    if (isTv_) {
        UpdateFocusedText();
    } else {
        UpdateNormalText();
    }
    MarkNeedRender();
}

void RenderOption::UpdateHoveredStatus()
{
    needLine_ = false;
    UpdateNormalText();
    MarkNeedRender();
}

void RenderOption::UpdateSelectedStatus()
{
    if (!data_ || !data_->GetTheme()) {
        return;
    }
    backColor_ = data_->GetSelectedBackgroundColor();
    needLine_ = false;
    UpdateSelectedText();
    MarkNeedRender();
}

void RenderOption::UpdateTvFocusedStatus()
{
    if (!data_ || !data_->GetTheme()) {
        return;
    }
    auto theme = data_->GetTheme();
    backColor_ = theme->GetClickedColor();
    UpdateFocusedText();
    MarkNeedRender();
}

void RenderOption::UpdateOthersStatus()
{
    auto pipe = context_.Upgrade();
    if (!data_ || !pipe) {
        return;
    }
    backColor_ = isTv_ ? Color(0x33FFFFFF) : data_->GetBackgroundColor();
    auto upOption = GetUpOption();
    needLine_ = (!(data_->GetFocused() && pipe->IsKeyEvent()) && upOption && upOption->IsNormalStatus());
    UpdateNormalText();
    MarkNeedRender();
}

RefPtr<RenderOption> RenderOption::GetUpOption() const
{
    auto options = GetAllOptions();
    RefPtr<RenderOption> topOption;
    for (auto it = options.begin(); it != options.end(); ++it) {
        if (AceType::RawPtr(*it) == this) {
            return topOption;
        }
        topOption = *it;
    }
    return nullptr;
}

RefPtr<RenderOption> RenderOption::GetDownOption() const
{
    auto options = GetAllOptions();
    for (auto it = options.begin(); it != options.end(); ++it) {
        if (AceType::RawPtr(*it) != this) {
            continue;
        }
        ++it;
        if (it == options.end()) {
            return nullptr;
        }
        return *it;
    }
    return nullptr;
}

bool RenderOption::IsNormalStatus() const
{
    auto pipe = context_.Upgrade();
    if (!data_ || !pipe) {
        return false;
    }

    return (!data_->GetClicked() && !hovered_ && !data_->GetSelected() &&
        !(data_->GetFocused() && pipe->IsKeyEvent()));
}

void RenderOption::UpdateNormalText()
{
    UpdateTextColor(false, false);
}

void RenderOption::UpdateSelectedText()
{
    UpdateTextColor(true, false);
}

void RenderOption::UpdateFocusedText()
{
    UpdateTextColor(false, true);
}

void RenderOption::UpdateTextColor(bool selected, bool focused)
{
    if (!data_) {
        return;
    }
    auto theme = data_->GetTheme();
    if (!theme) {
        return;
    }
    auto component = data_->GetText();
    if (!component) {
        return;
    }
    auto render = GetRenderText(AceType::Claim(this));
    if (!render) {
        return;
    }
    auto style = component->GetTextStyle();

    auto context = context_.Upgrade();
    if (context->GetIsDeclarative()) {
        if (focused) {
            style.SetTextColor(Color(0xE6000000));
            component->SetFocusColor(style.GetTextColor());
        }
    } else {
        if (focused) {
            style.SetTextColor(Color(0xE6000000));
            component->SetFocusColor(style.GetTextColor());
        } else if (selected) {
            style.SetTextColor(theme->GetSelectedColorText());
            component->SetFocusColor(style.GetTextColor());
        } else {
            style.SetTextColor(theme->GetFontColor());
            component->SetFocusColor(style.GetTextColor());
        }
    }
    component->SetTextStyle(style);
    render->Update(component);
}

void RenderOption::AdjustScrollPosition()
{
    RefPtr<RenderNode> render = GetParent().Upgrade();
    while (render && !AceType::InstanceOf<RenderScroll>(render)) {
        render = render->GetParent().Upgrade();
    }
    if (!render) {
        return;
    }
    auto scroll = AceType::DynamicCast<RenderScroll>(render);
    while (render && !AceType::InstanceOf<RenderSelectPopup>(render)) {
        render = render->GetParent().Upgrade();
    }
    if (!render) {
        return;
    }
    auto popup = AceType::DynamicCast<RenderSelectPopup>(render);
    auto scrollTop = scroll->GetCurrentPosition();
    auto scrollBottom = scrollTop + scroll->GetLayoutSize().Height();
    auto scrollHeight = scroll->GetLayoutSize().Height();
    auto optionTop = GetPosition().GetY();
    auto optionBottom = optionTop + GetLayoutSize().Height();
    auto optionHeight = GetLayoutSize().Height();
    if (scrollHeight < optionHeight) {
        auto center = (optionTop + optionBottom) / 2.0;
        scroll->JumpToPosition(center - scrollHeight / 2.0);
        popup->MarkNeedRender();
        return;
    }
    double pos = 0.0;
    if (optionTop < scrollTop + optionHeight) {
        pos = optionTop >= optionHeight ? optionTop - optionHeight : 0;
    } else if (scrollBottom - optionHeight < optionBottom) {
        pos = optionBottom + optionHeight - scrollHeight;
    } else {
        return;
    }
    if (pos <= optionTop && optionBottom <= pos + scrollHeight) {
        scroll->JumpToPosition(pos);
        popup->MarkNeedRender();
        return;
    }
    if (pos > optionTop) {
        scroll->JumpToPosition(optionTop);
        popup->MarkNeedRender();
        return;
    }
    scroll->JumpToPosition(optionBottom - scrollHeight);
    popup->MarkNeedRender();
}

RenderOption::~RenderOption()
{
    UpdateAccessibilityInfo(Size(0.0, 0.0), Offset(0.0, 0.0), false);
}

void RenderOption::InitClickEvent()
{
    if (click_) {
        return;
    }
    click_ = AceType::MakeRefPtr<ClickRecognizer>();
    auto weak = AceType::WeakClaim(this);
    click_->SetOnClick([weak](const ClickInfo&) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        ref->OnClick(false);
    });
}

void RenderOption::InitTouchEvent()
{
    if (touch_) {
        return;
    }
    touch_ = AceType::MakeRefPtr<RawRecognizer>();
    auto weak = AceType::WeakClaim(this);
    touch_->SetOnTouchDown([weak](const TouchEventInfo&) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        ref->OnTouch(true);
    });
    touch_->SetOnTouchUp([weak](const TouchEventInfo&) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        ref->OnTouch(false);
    });
    touch_->SetOnTouchCancel([weak](const TouchEventInfo&) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        ref->OnTouch(false);
    });
}

void RenderOption::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    LOGD("RenderOption::OnTouchTestHit(%{public}lf, %{public}lf).", coordinateOffset.GetX(), coordinateOffset.GetY());
    InitClickEvent();
    InitTouchEvent();
    click_->SetCoordinateOffset(coordinateOffset);
    touch_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(click_);
    result.emplace_back(touch_);
}

void RenderOption::Update(const RefPtr<Component>& component)
{
    data_ = AceType::DynamicCast<OptionComponent>(component);
    if (!data_ || !data_->GetTheme()) {
        return;
    }
    if (!eventEffectController_) {
        eventEffectController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    auto theme = data_->GetTheme();
    lineColor_  = theme->GetLineColor();
    clickedColor_ = theme->GetClickedColor();
    hoveredColor_ = theme->GetHoverColor();
    onClickEvent_ = AceAsyncEvent<void()>::Create(data_->GetClickEvent(), context_);
    UpdateStatus();
    MarkNeedLayout();
}

void RenderOption::OnPaintFinish()
{
    Size size = GetLayoutSize();
    Offset offset = GetGlobalOffset();
    bool isSelected = false;
    if (data_) {
        isSelected = data_->GetSelected();
    }
    UpdateAccessibilityInfo(size, offset, isSelected);

    // update focus
    auto pipeline = context_.Upgrade();
    if (!pipeline || !data_ || !data_->GetTheme()) {
        LOGE("pipeline or box or data component or theme is null.");
        return;
    }
    if (!data_->GetFocused()) {
        return;
    }
    auto theme = data_->GetTheme();
    Radius radius(NormalizeToPx((isTv_ ? ROUND_RADIUS_TV : ROUND_RADIUS_PHONE)));
    auto diff = NormalizeToPx(theme->GetOptionInterval());
    offset = GetGlobalOffset() + Size(diff, diff);
    size = GetLayoutSize() - Size(diff, diff) * 2; // left top diff and right bottom diff.
    pipeline->ShowFocusAnimation(
        RRect::MakeRRect(Rect(Offset(0, 0), size), radius), theme->GetClickedColor(), offset, true);
}

void RenderOption::UpdateAccessibilityInfo(Size size, Offset offset, bool isSelected)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    if (!data_) {
        return;
    }
    auto viewScale = context->GetViewScale();
    if (NearZero(viewScale)) {
        LOGW("GetGlobalPositionById viewScale is zero.");
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGW("RenderOption accessibilityManager is null.");
        return;
    }
    auto nodeId = StringUtils::StringToInt(data_->GetId());
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(nodeId);
    if (!accessibilityNode) {
        LOGW("RenderOption accessibilityNode is null.");
        return;
    }

    PositionInfo positionInfo = { (size.Width()) * viewScale, (size.Height()) * viewScale, (offset.GetX()) * viewScale,
        (offset.GetY()) * viewScale };
    accessibilityNode->SetPositionInfo(positionInfo);
    if (data_ && data_->GetText()) {
        auto text = data_->GetText();
        accessibilityNode->SetText(text->GetData());
    }
    accessibilityNode->SetSelectedState(isSelected);
}

void RenderOption::LayoutText(const RefPtr<RenderText>& text)
{
    double verInterval = NormalizeToPx(VERTICAL_INTERVAL_PHONE);
    double horInterval = NormalizeToPx(HORIZONTAL_INTERVAL_PHONE);

    double minWidth = minWidth_ - horInterval * 2.0; // left + right interval
    if (LessOrEqual(minWidth, 0.0)) {
        minWidth = 0.0;
    }
    double maxWidth = maxWidth_ - horInterval * 2.0; // left + right interval
    if (LessOrEqual(maxWidth, 0.0)) {
        maxWidth = 0.0;
    }

    LayoutParam layout;
    layout.SetMinWidth(minWidth);
    layout.SetMaxWidth(maxWidth);
    text->Layout(layout);
    auto size = text->GetLayoutSize();

    size.AddWidth(horInterval * 2.0);
    size.AddHeight(verInterval * 2.0);
    text->SetPosition(Offset(horInterval, verInterval));
    SetLayoutSize(size);
}

void RenderOption::LayoutTextImage(const RefPtr<RenderText>& text, const RefPtr<RenderImage>& image)
{
    double verInterval = NormalizeToPx(VERTICAL_INTERVAL_PHONE);
    double horInterval = NormalizeToPx(HORIZONTAL_INTERVAL_PHONE);
    double horDistance = NormalizeToPx(HORIZONTAL_DISTANCE_PHONE);

    image->Layout(LayoutParam());
    auto imageSize = image->GetLayoutSize();

    // left interval + right interval + distance between elements
    double minWidth = minWidth_ - horInterval * 2.0 - horDistance - imageSize.Width();
    if (LessOrEqual(minWidth, 0.0)) {
        minWidth = 0.0;
    }
    double maxWidth = maxWidth_ - horInterval * 2.0 - horDistance - imageSize.Width();
    if (LessOrEqual(maxWidth, 0.0)) {
        maxWidth = 0.0;
    }

    LayoutParam layout;
    layout.SetMinWidth(minWidth);
    layout.SetMaxWidth(maxWidth);
    text->Layout(layout);
    auto textSize = text->GetLayoutSize();

    auto size = textSize;
    size.AddWidth(horInterval * 2.0 + horDistance + imageSize.Width());
    size.AddHeight(verInterval * 2.0);
    SetLayoutSize(size);

    double yImage = (size.Height() - imageSize.Height()) / 2.0; // place center
    if (IsRTL()) {
        text->SetPosition(Offset(horInterval, verInterval));
        image->SetPosition(Offset(size.Width() - horInterval - imageSize.Width(), yImage));
    } else {
        image->SetPosition(Offset(horInterval, yImage));
        text->SetPosition(Offset(size.Width() - horInterval - textSize.Width(), verInterval));
    }
}

bool RenderOption::IsRTL() const
{
    if (!data_) {
        return false;
    }

    if (data_->GetTextDirection() == TextDirection::RTL) {
        return true;
    }

    return false;
}

void RenderOption::PerformLayout()
{
    if (data_->GetCustomComponent()) {
        auto child = GetLastChild();
        if (!child) {
            LOGE("child is null.");
            return;
        }

        auto layoutParam = LayoutParam(GetLayoutParam().GetMaxSize(), Size());
        child->Layout(layoutParam);
        SetLayoutSize(child->GetLayoutSize());
        return;
    }

    auto text = GetRenderText(AceType::Claim(this));
    if (!text) {
        LOGE("render text is null.");
        return;
    }

    auto image = GetRenderImage(AceType::Claim(this));
    if (image) {
        LayoutTextImage(text, image);
    } else {
        LayoutText(text);
    }
}

void RenderOption::PlayEventEffectAnimation(const Color& endColor, int32_t duration, bool isHoverExists)
{
    if (!eventEffectController_->IsStopped()) {
        eventEffectController_->Stop();
    }
    RefPtr<KeyframeAnimation<Color>> colorAnimation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    CreateMouseAnimation(colorAnimation, GetEventEffectColor(), endColor);
    if (duration == HOVER_DURATION) {
        colorAnimation->SetCurve(Curves::FRICTION);
    }
    if (isHoverExists && GetEventEffectColor().GetValue() < hoveredColor_.GetValue()) {
        colorAnimation->SetCurve(Curves::FAST_OUT_SLOW_IN);
    }
    eventEffectController_->ClearInterpolators();
    eventEffectController_->ClearStopListeners();
    eventEffectController_->AddInterpolator(colorAnimation);
    eventEffectController_->SetDuration(duration);
    eventEffectController_->SetFillMode(FillMode::FORWARDS);
    eventEffectController_->AddStopListener([weakNode = AceType::WeakClaim(this)]() {
        auto renderOption = weakNode.Upgrade();
        if (renderOption) {
            renderOption->UpdateStatus();
        }
    });
    eventEffectController_->Forward();
}

} // namespace OHOS::Ace
