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

#include "core/components/checkable/render_switch.h"

#include "base/log/event_report.h"
#include "core/common/font_manager.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {
namespace {

#ifdef WEARABLE_PRODUCT
constexpr int32_t DEFAULT_SWITCH_ANIMATION_DURATION = 250;
#else
constexpr int32_t DEFAULT_SWITCH_ANIMATION_DURATION = 450;
#endif
constexpr Dimension DEFAULT_SWITCH_POINT_PADDING = 2.0_vp;

} // namespace

RenderSwitch::~RenderSwitch()
{
#ifndef WEARABLE_PRODUCT
    UnSubscribeMultiModal();
#endif
    auto context = context_.Upgrade();
    if (context) {
        context->RemoveFontNode(WeakClaim(this));
        auto fontManager = context->GetFontManager();
        if (fontManager) {
            fontManager->RemoveVariationNode(WeakClaim(this));
        }
    }
}

void RenderSwitch::Update(const RefPtr<Component>& component)
{
    RenderCheckable::Update(component);
    auto switchComponent = AceType::DynamicCast<SwitchComponent>(component);
    if (!switchComponent) {
        LOGE("cast to switch component failed");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    component_ = switchComponent;
    showText_ = switchComponent->GetShowText();
    if (showText_) {
        if (!renderTextOn_ || !renderTextOff_) {
            InitRenderText();
        }
        UpdateRenderText(switchComponent);
    }

    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(GetContext());
        auto weak = AceType::WeakClaim(this);
        controller_->AddStopListener(Animator::StatusCallback([weak]() {
            auto switchComponent = weak.Upgrade();
            if (switchComponent) {
                switchComponent->OnAnimationStop();
            }
        }));
    }
    backgroundSolid_ = switchComponent->IsBackgroundSolid();
    pointPadding_ = DEFAULT_SWITCH_POINT_PADDING;

    if (switchComponent->GetUpdateType() == UpdateType::ALL) {
        checked_ = switchComponent->GetValue();
    }
    oldChecked_ = checked_;
    needReverse_ = (switchComponent->GetTextDirection() == TextDirection::RTL);
    auto theme = GetTheme<SwitchTheme>();
    if (theme) {
        borderWidth_ = theme->GetBorderWidth();
    }
    UpdateUIStatus();
    HandleDrag();
    UpdateAccessibilityAttr();
#ifndef WEARABLE_PRODUCT
    if (!component_->GetMultimodalProperties().IsUnavailable()) {
        PrepareMultiModalEvent();
        SubscribeMultiModal();
    }
#endif
}

void RenderSwitch::UpdateAccessibilityAttr()
{
    std::string text;
    if (component_ && showText_) {
        if (checked_) {
            text = textOn_;
        } else {
            text = textOff_;
        }
    }
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->SetText(text);
    accessibilityNode->SetCheckedState(checked_);
    if (accessibilityNode->GetClicked()) {
        accessibilityNode->SetClicked(false);
        auto context = context_.Upgrade();
        if (context) {
            AccessibilityEvent switchEvent;
            switchEvent.nodeId = accessibilityNode->GetNodeId();
            switchEvent.eventType = "click";
            context->SendEventToAccessibility(switchEvent);
        }
    }
}

void RenderSwitch::InitRenderText()
{
    LayoutParam innerLayout;
    innerLayout.SetMaxSize(Size(Size::INFINITE_SIZE, Size::INFINITE_SIZE));

    textOnComponent_ = AceType::MakeRefPtr<TextComponent>(textOn_);
    renderTextOn_ = AceType::DynamicCast<RenderText>(textOnComponent_->CreateRenderNode());
    renderTextOn_->Attach(GetContext());
    renderTextOn_->Update(textOnComponent_);
    renderTextOn_->SetLayoutParam(innerLayout);

    textOffComponent_ = AceType::MakeRefPtr<TextComponent>(textOff_);
    renderTextOff_ = AceType::DynamicCast<RenderText>(textOffComponent_->CreateRenderNode());
    renderTextOff_->Attach(GetContext());
    renderTextOff_->Update(textOffComponent_);
    renderTextOff_->SetLayoutParam(innerLayout);

    auto context = context_.Upgrade();
    if (context) {
        auto fontManager = context->GetFontManager();
        if (fontManager) {
            fontManager->AddVariationNode(WeakClaim(this));
        }
    }
}

void RenderSwitch::HandleDrag()
{
    if (!disabled_ && !dragRecognizer_) {
        dragRecognizer_ = AceType::MakeRefPtr<HorizontalDragRecognizer>();
        dragRecognizer_->SetOnDragStart([weak = AceType::WeakClaim(this)](const DragStartInfo& info) {
            auto renderSwitch = weak.Upgrade();
            if (renderSwitch) {
                renderSwitch->HandleDragStart(info.GetLocalLocation());
            }
        });
        dragRecognizer_->SetOnDragUpdate([weak = AceType::WeakClaim(this)](const DragUpdateInfo& info) {
            auto renderSwitch = weak.Upgrade();
            if (renderSwitch) {
                renderSwitch->HandleDragUpdate(info.GetLocalLocation());
            }
        });
        dragRecognizer_->SetOnDragEnd([weak = AceType::WeakClaim(this)](const DragEndInfo& info) {
            auto renderSwitch = weak.Upgrade();
            if (renderSwitch) {
                renderSwitch->HandleDragEnd(info.GetLocalLocation());
            }
        });
    } else if (disabled_ && dragRecognizer_) {
        dragRecognizer_ = nullptr;
    }
}

void RenderSwitch::PerformLayout()
{
    if (showText_) {
        width_ = NormalizeToPx(defaultWidth_);
        height_ = NormalizeToPx(defaultHeight_);
        textOnSize_ = CalculateTextSize(textOn_, renderTextOn_);
        textOffSize_ = CalculateTextSize(textOff_, renderTextOff_);
    } else {
        RenderCheckable::InitSize();
    }
    RenderCheckable::CalculateSize();
    double maxTextWidth = std::max(textOnSize_.Width(), textOffSize_.Width());
    double pointHeight = std::max(textOnSize_.Height(), drawSize_.Height());
    auto pointTextPadding = NormalizeToPx(pointTextPadding_);
    rawPointSize_ = Size(std::max(maxTextWidth + pointTextPadding * 2, pointHeight), pointHeight);
    bool pointUseTextHeight = (rawPointSize_.Height() > drawSize_.Height());
    bool pointUseHeightAsWidth = (rawPointSize_.Width() == drawSize_.Height());
    auto pointPadding = NormalizeToPx(pointPadding_);
    if (showText_) {
        if (pointUseTextHeight) {
            pointHeight += pointPadding * 2.0;
        }
        switchSize_ =
            Size(pointUseHeightAsWidth ? drawSize_.Width() : (rawPointSize_.Width() + pointPadding) * aspectRatio_,
                pointHeight);
        pointRadius_ = pointHeight / 2.0 - pointPadding;
    } else {
        switchSize_ = drawSize_;
        pointRadius_ = drawSize_.Height() / 2.0 - pointPadding;
    }
    rawPointSize_ -=
        Size(pointUseHeightAsWidth ? pointPadding * 2.0 : 0.0, pointUseTextHeight ? 0.0 : pointPadding * 2.0);

    Size layoutSize;
    if (switchSize_.Width() > width_) {
        layoutSize = GetLayoutParam().Constrain(Size(switchSize_.Width() + NormalizeToPx(hotZoneHorizontalPadding_) * 2,
            std::max(switchSize_.Height(), height_) + NormalizeToPx(hotZoneVerticalPadding_) * 2));
    } else {
        layoutSize = GetLayoutParam().Constrain(Size(width_, height_));
    }
    SetLayoutSize(layoutSize);
    double widthOverflow = layoutSize.Width() - switchSize_.Width();
    paintPosition_ = Alignment::GetAlignPosition(layoutSize, switchSize_, Alignment::CENTER) +
                     Offset(widthOverflow > 0.0 ? 0.0 : widthOverflow - NormalizeToPx(hotZoneHorizontalPadding_), 0.0);
    InitCurrentPointPosition();
    UpdateAccessibilityPosition();
}

void RenderSwitch::HandleDragStart(const Offset& updatePoint)
{
    dragStartPosition_ = updatePoint.GetX();
    oldChecked_ = checked_;
}

void RenderSwitch::HandleDragUpdate(const OHOS::Ace::Offset& updatePoint)
{
    OnDrag(updatePoint);
    MarkNeedRender();
}

void RenderSwitch::HandleDragEnd(const OHOS::Ace::Offset& updatePoint)
{
    OnDrag(updatePoint);
    dragStartPosition_ = 0.0;
    if (changeEvent_ && (oldChecked_ != checked_)) {
        std::string res = checked_ ? "true" : "false";
        changeEvent_(std::string("\"change\",{\"checked\":").append(res.append("},null")));
    }
    bool isNeedCallback = (oldChecked_ != checked_);
    oldChecked_ = checked_;
    InitCurrentPointPosition();
    UpdateUIStatus();
    if (onChange_ && isNeedCallback) {
        onChange_(checked_);
    }
    MarkNeedRender();
}

void RenderSwitch::UpdateRenderText(const RefPtr<SwitchComponent>& switchComponent)
{
    auto context = context_.Upgrade();
    if (context) {
        if (textStyle_.IsAllowScale() || textStyle_.GetFontSize().Unit() == DimensionUnit::FP) {
            context->AddFontNode(AceType::WeakClaim(this));
        }
    }

    textOn_ = switchComponent->GetTextOn();
    textOff_ = switchComponent->GetTextOff();
    textColorOn_ = switchComponent->GetTextColorOn();
    textColorOff_ = switchComponent->GetTextColorOff();
    pointTextPadding_ = switchComponent->GetTextPadding();

    textStyle_ = switchComponent->GetTextStyle();
    textStyle_.SetTextColor(textColorOn_);
    textOnComponent_->SetData(textOn_);
    textOnComponent_->SetTextStyle(textStyle_);
    renderTextOn_->Update(textOnComponent_);

    textStyle_.SetTextColor(textColorOff_);
    textOffComponent_->SetData(textOff_);
    textOffComponent_->SetTextStyle(textStyle_);
    renderTextOff_->Update(textOffComponent_);
}

void RenderSwitch::OnDrag(const Offset& updatePoint)
{
    InitCurrentPointPosition();
    auto pointPadding = NormalizeToPx(pointPadding_);
    double pointTrackLength = switchSize_.Width() - rawPointSize_.Width() - pointPadding * 2;
    double halfTrackLength = pointTrackLength / 2.0;
    pointPositionDelta_ = updatePoint.GetX() - dragStartPosition_;
    // let A = [needReverse_], B = [oldChecked_], C = [moveRight], D = [effectiveMove],
    // and [A'] represents the reverse of [A].
    // there are four situations in which the checked status needs to be reversed:
    // 1. A'B'CD, 2. A'BC'D, 3. AB'C'D, 4. ABCD,
    // so [needChangeStatus] = A'B'CD + ABCD + A'BC'D + AB'C'D = (A'B' + AB)CD + (A'B + AB')C'D
    bool effectiveMove = abs(pointPositionDelta_) > halfTrackLength;
    bool moveRight = pointPositionDelta_ > 0.0;
    bool needChangeStatus = ((needReverse_ == oldChecked_) && moveRight && effectiveMove) ||
                            ((needReverse_ != oldChecked_) && !moveRight && effectiveMove);
    checked_ = needChangeStatus != oldChecked_;     // reverse [checked_] status if [needChangeStatus] is true

    // let A = [needReverse_], B = [oldChecked_], C = [moveRight], D = [excessiveMove],
    // and [A'] represents the reverse of [A].
    // there are eight situations in which [pointPositionDelta_] needs to be constrained to zero:
    // 1. A'B'C'D', 2. A'B'C'D, 3. ABC'D', 4. ABC'D, 5. A'BCD', 6. A'BCD, 7. AB'CD', 8. AB'CD,
    // so [constrainDeltaToZero] = (A'B'C'D' + A'B'C'D) +( ABC'D' + ABC'D) + (A'BCD' + A'BCD) + (AB'CD' + AB'CD)
    //                           = A'B'C' + ABC' + A'BC + AB'C = (A'B' + AB)C' + (A'B + AB')C
    bool excessiveMove = abs(pointPositionDelta_) > pointTrackLength;
    bool constrainDeltaToZero =
        (moveRight && (needReverse_ != oldChecked_)) || (!moveRight && (needReverse_ == oldChecked_));
    // there are four situations in which the absolute value of [pointPositionDelta_] needs to be constrained to
    // trackLength: 1. A'BC'D, 2. AB'C'D, 3. ABCD, 4. A'B'CD,
    // so [constrainDeltaToTrackLength] = A'BC'D + AB'C'D + ABCD + A'B'CD = (A'BC' + AB'C' + ABC + A'B'C)D
    //                                  = ((A'B + AB')C' + (AB + A'B')C)D
    bool constrainDeltaToTrackLength = excessiveMove &&
        ((!moveRight && (needReverse_ != oldChecked_)) || (moveRight && (needReverse_ == oldChecked_)));
    double deltaCoefficient = moveRight ? 1.0 : -1.0;
    if (constrainDeltaToZero) {
        pointPositionDelta_ = 0.0;
    } else if (constrainDeltaToTrackLength) {
        pointPositionDelta_ = deltaCoefficient * pointTrackLength;
    }
    currentPointOriginX_ += pointPositionDelta_;
}

void RenderSwitch::HandleClick()
{
    UpdateAnimation();
    controller_->Play();

    if (clickEvent_) {
        clickEvent_();
    }
}

void RenderSwitch::PaintText(const Offset& textOffset, RenderContext& context) const
{
    auto paintRenderText = oldChecked_ ? renderTextOn_ : renderTextOff_;
    auto textSize = paintRenderText->GetLayoutSize();
    textSize.SetHeight(textSize.Height() > rawPointSize_.Height() ? rawPointSize_.Height() : textSize.Height());
    auto textPos = Alignment::GetAlignPosition(rawPointSize_, textSize, Alignment::CENTER);
    paintRenderText->Paint(context, textOffset + textPos);
}

void RenderSwitch::OnAnimationStop()
{
    // after the animation stopped,we need to update the check status
    RenderCheckable::HandleClick();
    UpdateAccessibilityAttr();
    oldChecked_ = checked_;
    InitCurrentPointPosition();
}

void RenderSwitch::UpdateAnimation()
{
    double from = 0.0;
    double to = 0.0;
    auto pointPadding = NormalizeToPx(pointPadding_);
    if ((oldChecked_ && !needReverse_) || (!oldChecked_ && needReverse_)) {
        from = switchSize_.Width() - pointPadding - rawPointSize_.Width();
        to = pointPadding;
    } else {
        from = pointPadding;
        to = switchSize_.Width() - pointPadding - rawPointSize_.Width();
    }

    if (translate_) {
        controller_->RemoveInterpolator(translate_);
    }
    translate_ = AceType::MakeRefPtr<CurveAnimation<double>>(from, to, Curves::FRICTION);
    auto weak = AceType::WeakClaim(this);
    translate_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto switchComp = weak.Upgrade();
        if (switchComp) {
            Offset data(value, 0.0);
            switchComp->UpdatePointPosition(data);
        }
    }));
    controller_->SetDuration(DEFAULT_SWITCH_ANIMATION_DURATION);
    controller_->AddInterpolator(translate_);
}

void RenderSwitch::UpdatePointPosition(const Offset& updatePoint)
{
    InitCurrentPointPosition();
    auto pointPadding = NormalizeToPx(pointPadding_);
    double pointTrackLength = switchSize_.Width() - rawPointSize_.Width() - pointPadding * 2;
    double movingOffset = updatePoint.GetX();
    if (movingOffset < 0.0) {
        movingOffset = 0.0;
    } else if (movingOffset > pointTrackLength) {
        movingOffset = pointTrackLength;
    }
    if ((oldChecked_ && !needReverse_) || (!oldChecked_ && needReverse_)) {
        currentPointOriginX_ = movingOffset;
        uiStatus_ = needReverse_ ? UIStatus::OFF_TO_ON : UIStatus::ON_TO_OFF;
    } else {
        uiStatus_ = needReverse_ ? UIStatus::ON_TO_OFF : UIStatus::OFF_TO_ON;
        currentPointOriginX_ += movingOffset;
    }
    MarkNeedRender();
}

#ifndef WEARABLE_PRODUCT
void RenderSwitch::PrepareMultiModalEvent()
{
    if (!multimodalSwitchEvent_) {
        multimodalSwitchEvent_ = [weak = WeakClaim(this)](const AceMultimodalEvent& event) {
            auto renderSwitch = weak.Upgrade();
            static const int32_t switchOn = 1;
            static const int32_t switchOff = 2;
            if (renderSwitch) {
                if (event.GetVoice().action == switchOn) {
                    renderSwitch->OpenSwitch();
                } else if (event.GetVoice().action == switchOff) {
                    renderSwitch->CloseSwitch();
                }
            }
        };
    }
}

bool RenderSwitch::SubscribeMultiModal()
{
    if (isSubscribeMultimodal_) {
        return true;
    }
    if (multiModalScene_.Invalid()) {
        const auto pipelineContext = GetContext().Upgrade();
        if (!pipelineContext) {
            LOGW("the pipeline context is null");
            return false;
        }
        const auto multimodalManager = pipelineContext->GetMultiModalManager();
        if (!multimodalManager) {
            LOGW("the multimodal manager is null");
            return false;
        }
        const auto scene = multimodalManager->GetCurrentMultiModalScene();
        switchEvent_ = VoiceEvent(component_->GetMultimodalProperties().voiceLabel, SceneLabel::SWITCH);
        scene->SubscribeVoiceEvent(switchEvent_, multimodalSwitchEvent_);

        multiModalScene_ = scene;
        isSubscribeMultimodal_ = true;
    }
    return true;
}

bool RenderSwitch::UnSubscribeMultiModal()
{
    if (!isSubscribeMultimodal_) {
        return true;
    }
    auto multiModalScene = multiModalScene_.Upgrade();
    if (!multiModalScene) {
        LOGE("fail to destroy multimodal event due to multiModalScene is null");
        return false;
    }
    if (!switchEvent_.GetVoiceContent().empty()) {
        multiModalScene->UnSubscribeVoiceEvent(switchEvent_);
    }
    isSubscribeMultimodal_ = false;
    return true;
}
#endif

void RenderSwitch::OpenSwitch()
{
    if (!oldChecked_) {
        HandleClick();
    }
}

void RenderSwitch::CloseSwitch()
{
    if (oldChecked_) {
        HandleClick();
    }
}

void RenderSwitch::InitCurrentPointPosition()
{
    auto pointPadding = NormalizeToPx(pointPadding_);
    if (needReverse_) {
        currentPointOriginX_ =
            oldChecked_ ? pointPadding : (switchSize_.Width() - pointPadding - rawPointSize_.Width());
    } else {
        currentPointOriginX_ =
            oldChecked_ ? (switchSize_.Width() - pointPadding - rawPointSize_.Width()) : pointPadding;
    }
}

} // namespace OHOS::Ace
