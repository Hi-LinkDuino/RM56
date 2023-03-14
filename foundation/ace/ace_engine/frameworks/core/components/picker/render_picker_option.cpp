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

#include "core/components/picker/render_picker_option.h"

#include "base/log/event_report.h"
#include "base/utils/system_properties.h"
#include "core/components/picker/picker_option_component.h"
#include "core/components/text/render_text.h"

namespace OHOS::Ace {
namespace {

const Dimension PRESS_INTERVAL = 4.0_vp;
const Dimension PRESS_RADIUS = 8.0_vp;
const Dimension FOCUS_AUTO_DIFF = 2.0_vp;
const Dimension FOCUS_RADIUS_AUTO_DIFF = 3.0_vp;
const Color PRESS_COLOR(0x19000000);
const Color HOVER_COLOR(0x0C000000);

} // namespace

RenderPickerOption::RenderPickerOption()
{
    if (SystemProperties::GetDeviceType() != DeviceType::PHONE) {
        return;
    }

    pressDecoration_ = AceType::MakeRefPtr<Decoration>();
    pressDecoration_->SetBackgroundColor(PRESS_COLOR);
    pressDecoration_->SetBorderRadius(Radius(PRESS_RADIUS));

    hoverDecoration_ = AceType::MakeRefPtr<Decoration>();
    hoverDecoration_->SetBackgroundColor(HOVER_COLOR);
    hoverDecoration_->SetBorderRadius(Radius(PRESS_RADIUS));
}

void RenderPickerOption::Update(const RefPtr<Component>& component)
{
    auto option = AceType::DynamicCast<PickerOptionComponent>(component);
    if (!option) {
        LOGE("input component is incorrect type or null.");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    auto theme = option->GetTheme();
    if (!theme) {
        LOGE("option theme is null.");
        EventReport::SendComponentException(ComponentExcepType::GET_THEME_ERR);
        return;
    }

    if (pressDecoration_) {
        pressDecoration_->SetBackgroundColor(theme->GetPressColor());
    }
    if (hoverDecoration_) {
        hoverDecoration_->SetBackgroundColor(theme->GetHoverColor());
    }
    optionSize_ = theme->GetOptionSize(option->GetSelected());
    if (!NearZero(NormalizeToPx(option->GetFixHeight()))) {
        optionSize_.SetHeight(NormalizeToPx(option->GetFixHeight()));
    }
    optionSizeUnit_ = theme->GetOptionSizeUnit();
    optionPadding_ = theme->GetOptionPadding();
    textComponent_ = option->GetTextComponent();
    boxComponent_ = option->GetBoxComponent();
    selectedStyle_ = theme->GetOptionStyle(true, false);
    focusStyle_ = theme->GetOptionStyle(true, true);
    focusColor_ = theme->GetFocusColor();
    rrectRadius_ = theme->GetFocusRadius();
    selectedDecoration_ = theme->GetOptionDecoration(false);
    focusDecoration_ = theme->GetOptionDecoration(true);
    index_ = option->GetIndex();
    text_ = option->GetText();
    selected_ = option->GetSelected();
    autoLayout_ = option->GetAutoLayout();
    alignTop_ = option->GetAlignTop();
    alignBottom_ = option->GetAlignBottom();
    MarkNeedLayout();
}

bool RenderPickerOption::GetSelected() const
{
    return selected_;
}

void RenderPickerOption::UpdateValue(uint32_t newIndex, const std::string& newText)
{
    index_ = newIndex;
    text_ = newText;
    if (!textComponent_) {
        LOGE("text component is null in picker option.");
        return;
    }

    if (textComponent_->GetData() == text_) {
        LOGE("The text does not change and does not need to be updated.");
        return; // needless to update
    }

    textComponent_->SetData(text_);
    if (!renderText_) {
        LOGE("render text is null in picker option.");
        return;
    }
    renderText_->Update(textComponent_);
}

void RenderPickerOption::OnTouchTestHit(const Offset&, const TouchRestrict&, TouchTestResult& result)
{
    if (!selected_ && !autoLayout_) {
        return;
    }

    if (!pressDetect_) {
        pressDetect_ = AceType::MakeRefPtr<PressRecognizer>(context_);
        pressDetect_->SetOnPress([weak = AceType::WeakClaim(this)] (const PressInfo&) {
            auto refPtr = weak.Upgrade();
            if (!refPtr) {
                return;
            }
            refPtr->StartPressAnimation(true);
        });
        pressDetect_->SetOnPressCancel([weak = AceType::WeakClaim(this)] {
            auto refPtr = weak.Upgrade();
            if (!refPtr) {
                return;
            }
            refPtr->StartPressAnimation(false);
        });
    }
    result.emplace_back(pressDetect_);
}

void RenderPickerOption::OnMouseHoverEnterTest()
{
    if (!selected_ || disabled_) {
        return;
    }
    StartHoverAnimation(true);
}

void RenderPickerOption::OnMouseHoverExitTest()
{
    if (!selected_ || disabled_) {
        return;
    }
    StartHoverAnimation(false);
}

void RenderPickerOption::UpdateBackgroundDecoration(const Color& color)
{
    if (!pressDecoration_) {
        return;
    }
    pressDecoration_->SetBackgroundColor(GetEventEffectColor());
    boxComponent_->SetBackDecoration(pressDecoration_);
    renderBox_->Update(boxComponent_);
    MarkNeedRender();
}

void RenderPickerOption::ResetMouseController()
{
    if (!mouseAnimationController_) {
        mouseAnimationController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    if (mouseAnimationController_->IsRunning()) {
        mouseAnimationController_->Stop();
    }
    mouseAnimationController_->ClearInterpolators();
    mouseAnimationController_->ClearAllListeners();
}

bool RenderPickerOption::ResetHoverAnimation(bool isEnter)
{
    RefPtr<PickerTheme> theme = GetTheme<PickerTheme>();
    if (!theme) {
        LOGE("picker option theme invalid");
        return false;
    }
    if (!mouseAnimationController_) {
        ResetMouseController();
    }

    auto hoverColor = theme->GetHoverColor();
    Color bgColor = GetEventEffectColor();
    if (selectedDecoration_) {
        bgColor = selectedDecoration_->GetBackgroundColor();
    }
    RefPtr<KeyframeAnimation<Color>> animation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    if (isEnter) {
        // hover enter
        CreateMouseAnimation(animation, bgColor, bgColor.BlendColor(hoverColor));
        animation->SetCurve(Curves::FRICTION);
    } else {
        // from hover to normal
        CreateMouseAnimation(animation, GetEventEffectColor(), bgColor);
        if (GetEventEffectColor() == bgColor.BlendColor(hoverColor)) {
            animation->SetCurve(Curves::FRICTION);
        } else {
            animation->SetCurve(Curves::FAST_OUT_SLOW_IN);
        }
    }
    mouseAnimationController_->SetDuration(HOVER_DURATION);
    mouseAnimationController_->AddInterpolator(animation);
    mouseAnimationController_->SetFillMode(FillMode::FORWARDS);
    return true;
}

bool RenderPickerOption::ResetPressAnimation(bool isDown)
{
    RefPtr<PickerTheme> theme = GetTheme<PickerTheme>();
    if (!theme) {
        LOGE("picker option theme invalid");
        return false;
    }
    if (!mouseAnimationController_) {
        ResetMouseController();
    }

    auto hoverColor = theme->GetHoverColor();
    auto pressColor = theme->GetPressColor();
    Color bgColor = GetEventEffectColor();
    if (selectedDecoration_) {
        bgColor = selectedDecoration_->GetBackgroundColor();
    }
    RefPtr<KeyframeAnimation<Color>> animation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();

    if (isDown) {
        if (mouseState_ == MouseState::HOVER) {
            // from hover to press
            CreateMouseAnimation(animation, GetEventEffectColor(), bgColor.BlendColor(pressColor));
        } else {
            // from normal to press
            CreateMouseAnimation(animation, bgColor, bgColor.BlendColor(pressColor));
        }
    } else {
        if (mouseState_ == MouseState::HOVER) {
            // from press to hover
            CreateMouseAnimation(animation, GetEventEffectColor(), bgColor.BlendColor(hoverColor));
        } else {
            // from press to normal
            CreateMouseAnimation(animation, GetEventEffectColor(), bgColor);
        }
    }
    mouseAnimationController_->SetDuration(PRESS_DURATION);
    mouseAnimationController_->AddInterpolator(animation);
    mouseAnimationController_->SetFillMode(FillMode::FORWARDS);
    return true;
}

void RenderPickerOption::StartHoverAnimation(bool isEnter)
{
    ResetMouseController();
    SetHoverAndPressCallback([weakNode = AceType::WeakClaim(this)](const Color& color) {
        auto node = weakNode.Upgrade();
        if (node) {
            node->UpdateBackgroundDecoration(color);
        }
    });
    if (mouseAnimationController_ && ResetHoverAnimation(isEnter)) {
        mouseAnimationController_->Forward();
    }
}

void RenderPickerOption::StartPressAnimation(bool isDown)
{
    ResetMouseController();
    SetHoverAndPressCallback([weakNode = AceType::WeakClaim(this)](const Color& color) {
        auto node = weakNode.Upgrade();
        if (node) {
            node->UpdateBackgroundDecoration(color);
        }
    });
    if (mouseAnimationController_ && ResetPressAnimation(isDown)) {
        mouseAnimationController_->Forward();
    }
}

void RenderPickerOption::UpdateTextFocus(bool focus)
{
    hasTextFocus_ = focus;

    if (renderText_ && textComponent_) {
        if (focus) {
            textComponent_->SetTextStyle(focusStyle_);
        } else {
            textComponent_->SetTextStyle(selectedStyle_);
        }
        renderText_->Update(textComponent_);
    }
}

void RenderPickerOption::UpdatePhoneFocus(bool focus)
{
    if (SystemProperties::GetDeviceType() != DeviceType::PHONE) {
        return;
    }

    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        LOGE("pipeline is null.");
        return;
    }

    if (focus) {
        hasAnimate_ = true;
        auto size = realSize_;
        auto offset = GetGlobalOffset();
        double radiusValue = NormalizeToPx(PRESS_RADIUS) - NormalizeToPx(FOCUS_RADIUS_AUTO_DIFF);
        Radius pxRadius(radiusValue, radiusValue);
        double yOffsetDiff = NormalizeToPx(PRESS_INTERVAL) + NormalizeToPx(FOCUS_AUTO_DIFF);
        double xOffsetDiff = NormalizeToPx(FOCUS_AUTO_DIFF);
        double xSizeDiff = 2.0 * xOffsetDiff;
        double ySizeDiff = 2.0 * yOffsetDiff;
        size = size - Size(xSizeDiff, ySizeDiff);
        offset = offset + Size(xOffsetDiff, yOffsetDiff);
        pipeline->ShowFocusAnimation(
            RRect::MakeRRect(Rect(Offset(0, 0), size), pxRadius), focusColor_, offset);
    } else {
        hasAnimate_ = false;
    }
}

void RenderPickerOption::UpdateFocus(bool focus)
{
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        UpdateTextFocus(focus);
        UpdatePhoneFocus(focus);
        return;
    }

    if (renderText_ && renderBox_ && textComponent_ && boxComponent_ && focusDecoration_ && selectedDecoration_) {
        if (focus) {
            textComponent_->SetTextStyle(focusStyle_);
            boxComponent_->SetBackDecoration(focusDecoration_);
        } else {
            textComponent_->SetTextStyle(selectedStyle_);
            boxComponent_->SetBackDecoration(selectedDecoration_);
        }
        renderText_->Update(textComponent_);
        renderBox_->Update(boxComponent_);
    } else {
        LOGE("inner params has null.");
    }

    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        LOGE("pipeline is null.");
        return;
    }

    if (focus) {
        hasAnimate_ = true;
        Radius pxRadius(NormalizeToPx(rrectRadius_.GetX()), NormalizeToPx(rrectRadius_.GetY()));
        pipeline->ShowFocusAnimation(
            RRect::MakeRRect(Rect(Offset(0, 0), realSize_), pxRadius), focusColor_, GetGlobalOffset());
    } else {
        hasAnimate_ = false;
    }
}

void RenderPickerOption::RefreshFocus()
{
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        UpdatePhoneFocus(hasAnimate_);
        return;
    }

    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        LOGE("pipeline is null.");
        return;
    }

    if (hasAnimate_) {
        Radius pxRadius(NormalizeToPx(rrectRadius_.GetX()), NormalizeToPx(rrectRadius_.GetY()));
        pipeline->ShowFocusAnimation(
            RRect::MakeRRect(Rect(Offset(0, 0), realSize_), pxRadius), focusColor_, GetGlobalOffset());
    }
}

void RenderPickerOption::UpdateScrollDelta(double delta)
{
    deltaSize_ = delta;
    MarkNeedLayout();
}

double RenderPickerOption::LayoutBox()
{
    LayoutParam boxLayout;
    if (SystemProperties::GetDeviceType() == DeviceType::PHONE && selected_ && !autoLayout_) {
        auto pressInterval = NormalizeToPx(PRESS_INTERVAL);
        auto boxSize = realSize_;
        boxSize.SetHeight(boxSize.Height() - 2.0 * pressInterval);
        boxLayout.SetFixedSize(boxSize);
        renderBox_->SetPosition(Offset(0.0, pressInterval));
        renderBox_->Layout(boxLayout);
        return pressInterval;
    } else {
        boxLayout.SetFixedSize(realSize_);
        renderBox_->SetPosition(Offset(0.0, 0.0));
        renderBox_->Layout(boxLayout);
        return 0.0;
    }
}

void RenderPickerOption::PerformLayout()
{
    if (!renderBox_ || !renderText_) {
        LOGE("render text or render box is null.");
        return;
    }

    renderText_->Layout(GetLayoutParam());
    Size textSize = renderText_->GetLayoutSize();
    realPadding_ = NormalizeToPx(Dimension(optionPadding_, optionSizeUnit_));

    if (autoLayout_) {
        realSize_ = renderText_->GetLayoutSize();
    } else {
        realSize_.SetWidth(NormalizeToPx(Dimension(optionSize_.Width(), optionSizeUnit_)));
        realSize_.SetHeight(NormalizeToPx(Dimension(optionSize_.Height(), optionSizeUnit_)));
    }

    if (realSize_.Width() - textSize.Width() < realPadding_) {
        realSize_.SetWidth(textSize.Width() + realPadding_);
    }
    if (realSize_.Height() - textSize.Height() < realPadding_) {
        realSize_.SetHeight(textSize.Height() + realPadding_);
    }

    double maxWidth = GetLayoutParam().GetMaxSize().Width();
    if (realSize_.Width() > maxWidth) {
        realSize_.SetWidth(maxWidth);
    }
    if (textSize.Width() > maxWidth - realPadding_) {
        textSize.SetWidth(maxWidth - realPadding_);
    }

    auto pressInterval = LayoutBox();

    LayoutParam textLayout;
    textLayout.SetFixedSize(textSize);
    double textX = (realSize_.Width() - textSize.Width()) / 2.0; // place center
    if (textComponent_ && textComponent_->GetTextDirection() == TextDirection::RTL) {
        textX = realSize_.Width() - realPadding_ / 2.0 - textSize.Width(); // place right; right padding is half
    }
    double textY = (realSize_.Height() - textSize.Height()) / 2.0; // place center
    if (alignTop_) {
        textY = 0.0;                                               // place top
    } else if (alignBottom_) {
        textY = realSize_.Height() - textSize.Height();            // place bottom
    }
    textY += deltaSize_;                                           // think about delta of scroll action.
    textY -= pressInterval;
    renderText_->SetPosition(Offset(textX, textY));
    renderText_->Layout(textLayout);

    SetLayoutSize(realSize_);
}

void RenderPickerOption::OnPaintFinish()
{
    if (!autoLayout_ && !selected_) {
        return;
    }

    RefreshFocus();
}

void RenderPickerOption::UpdateRenders()
{
    ClearRenders();
    GetRenders();
}

void RenderPickerOption::GetRenders(const RefPtr<RenderNode>& render)
{
    if (!render) {
        LOGE("render node is null.");
        return;
    }

    if (AceType::InstanceOf<RenderText>(render)) {
        renderText_ = AceType::DynamicCast<RenderText>(render);
        return;
    }

    if (AceType::InstanceOf<RenderBox>(render)) {
        renderBox_ = AceType::DynamicCast<RenderBox>(render);
    }

    for (const auto& child : render->GetChildren()) {
        GetRenders(child);
    }
}

void RenderPickerOption::GetRenders()
{
    GetRenders(AceType::Claim(this));
}

void RenderPickerOption::ClearRenders()
{
    renderText_ = nullptr;
    renderBox_ = nullptr;
}

} // namespace OHOS::Ace
