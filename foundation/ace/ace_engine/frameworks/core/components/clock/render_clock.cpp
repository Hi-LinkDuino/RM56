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

#include "core/components/clock/render_clock.h"

#include <cmath>

#include "base/i18n/localization.h"
#include "core/components/clock/clock_component.h"
#include "core/components/image/image_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr double HOUR_HAND_RATIO = 0.062; // [width-of-clock-hand-image] / [width-of-clock-face-image]
constexpr double RADIAN_FOR_ONE_HOUR = 30 * 3.1415926 / 180.0;
constexpr double DIGIT_SIZE_RATIO_UPPER_BOUND = 1.0 / 7.0;
constexpr double DIGIT_RADIUS_RATIO_UPPER_BOUND = 1.0;
constexpr double EPSILON = 0.000001;
constexpr int32_t TOTAL_HOURS_OF_ANALOG_CLOCK = 12;

void UseDaySourceIfEmpty(const std::string& daySource, std::string& nightSource)
{
    if (nightSource.empty()) {
        nightSource = daySource;
    }
}

std::function<void()> GenerateClockHandLoadFailCallback(const RefPtr<RenderImage>& renderClockHand, double hoursWest,
    const std::string& handType, InternalResource::ResourceId resourceIdForDay,
    InternalResource::ResourceId resourceIdForNight)
{
    auto clockHandLoadFailCallback = [wp = AceType::WeakClaim(AceType::RawPtr(renderClockHand)), hoursWest, handType,
        resourceIdForDay, resourceIdForNight]() {
        auto renderClockHand = wp.Upgrade();
        if (!renderClockHand) {
            LOGE("renderClockHand upgrade fail when try handle clock hand load fail event");
            return;
        }
        RefPtr<ImageComponent> imageComponent = AceType::MakeRefPtr<ImageComponent>("");
        auto resourceId = IsDayTime(GetTimeOfNow(hoursWest)) ? resourceIdForDay : resourceIdForNight;
        imageComponent->SetResourceId(resourceId);
        renderClockHand->Update(imageComponent);
        LOGE("clock hand load fail event triggered, using internal clock hand source, handType: %{public}s",
            handType.c_str());
    };
    return clockHandLoadFailCallback;
}

} // namespace

RenderClock::RenderClock()
{
    for (int32_t i = 1; i <= TOTAL_HOURS_OF_ANALOG_CLOCK; i++) {
        auto digitStr = Localization::GetInstance()->NumberFormat(i);
        auto textComponent = AceType::MakeRefPtr<TextComponent>(digitStr);
        auto renderDigit = AceType::DynamicCast<RenderText>(RenderText::Create());
        AddChild(renderDigit);
        digitRenderNodes_.emplace_back(renderDigit);
        digitComponentNodes_.emplace_back(textComponent);
        radians_.emplace_back(i * RADIAN_FOR_ONE_HOUR);
    }

    renderClockFace_ = AceType::DynamicCast<RenderImage>(RenderImage::Create());
    AddChild(renderClockFace_);

    renderClockHand_ = AceType::DynamicCast<RenderClockHand>(RenderClockHand::Create());
    renderClockHand_->SetDayToNightCallback([wp = WeakClaim(this)]() {
        auto renderClock = wp.Upgrade();
        if (renderClock) {
            renderClock->UseNightConfig();
            renderClock->MarkNeedLayout();
        }
    });
    renderClockHand_->SetNightToDayCallback([wp = WeakClaim(this)]() {
        auto renderClock = wp.Upgrade();
        if (renderClock) {
            renderClock->UseDayConfig();
            renderClock->MarkNeedLayout();
        }
    });
    renderClockHand_->SetAccessibilityTimeCallback([wp = WeakClaim(this)](double hour, double minute) {
        auto renderClock = wp.Upgrade();
        if (renderClock) {
            renderClock->UpdateAccessibilityInfo(hour, minute);
        }
    });
    renderHourHand_ = AceType::DynamicCast<RenderImage>(RenderImage::Create());
    renderMinuteHand_ = AceType::DynamicCast<RenderImage>(RenderImage::Create());
    renderSecondHand_ = AceType::DynamicCast<RenderImage>(RenderImage::Create());
    AddChild(renderClockHand_);
}

void RenderClock::Update(const RefPtr<Component>& component)
{
    RefPtr<ClockComponent> clockComponent = AceType::DynamicCast<ClockComponent>(component);
    if (clockComponent == nullptr) {
        LOGE("clock component is null!");
        return;
    }
    declaration_ = clockComponent->GetDeclaration();
    if (declaration_ == nullptr) {
        LOGE("clock declaration is null!");
        return;
    }
    auto inputDigitSizeRatio = declaration_->GetDigitSizeRatio();
    digitSizeRatio_ = InRegion(EPSILON, DIGIT_SIZE_RATIO_UPPER_BOUND, inputDigitSizeRatio) ? inputDigitSizeRatio
                                                                                           : digitSizeRatio_;
    auto inputDigitRadiusRatio = declaration_->GetDigitRadiusRatio();
    digitRadiusRatio_ = InRegion(EPSILON, DIGIT_RADIUS_RATIO_UPPER_BOUND, inputDigitRadiusRatio)
                            ? inputDigitRadiusRatio
                            : digitRadiusRatio_;

    // update attributes and styles for night mode
    clockFaceNightSrc_ = declaration_->GetClockFaceNightSrc();
    hourHandNightSrc_ = declaration_->GetHourHandNightSrc();
    minuteHandNightSrc_ = declaration_->GetMinuteHandNightSrc();
    secondHandNightSrc_ = declaration_->GetSecondHandNightSrc();
    digitColorNight_ = declaration_->GetDigitColorNight();

    CheckNightConfig();

    double hoursWest = declaration_->GetHoursWest();
    auto timeOfNow = GetTimeOfNow(hoursWest);
    IsDayTime(timeOfNow) ? UseDayConfig() : UseNightConfig();
    renderHourHand_->SetLoadFailCallback(GenerateClockHandLoadFailCallback(renderHourHand_, hoursWest, "hour",
        InternalResource::ResourceId::FA_CLOCK_WIDGET_HOUR,
        InternalResource::ResourceId::FA_BLACK_CLOCK_WIDGET_HOUR));
    renderMinuteHand_->SetLoadFailCallback(GenerateClockHandLoadFailCallback(renderMinuteHand_, hoursWest, "minute",
        InternalResource::ResourceId::FA_CLOCK_WIDGET_MINUTE,
        InternalResource::ResourceId::FA_BLACK_CLOCK_WIDGET_MINUTE));
    renderSecondHand_->SetLoadFailCallback(GenerateClockHandLoadFailCallback(renderSecondHand_, hoursWest, "second",
        InternalResource::ResourceId::FA_CLOCK_WIDGET_SECOND,
        InternalResource::ResourceId::FA_BLACK_CLOCK_WIDGET_SECOND));

    renderClockHand_->SetHoursWest(declaration_->GetHoursWest());
    // update accessibility text when hour changed
    UpdateAccessibilityInfo(timeOfNow.hour24_, timeOfNow.minute_);
    renderClockHand_->Attach(GetContext());
    renderClockHand_->SetOnHourCallback(
        AceAsyncEvent<void(const std::string&)>::Create(declaration_->GetOnHourChangeEvent(), context_));
    if (!setScreenCallback_) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        context->AddScreenOnEvent([wp = WeakPtr<RenderClockHand>(renderClockHand_)]() {
            auto renderClockHand = wp.Upgrade();
            if (renderClockHand) {
                renderClockHand->SetNeedStop(false);
            }
        });
        context->AddScreenOffEvent([wp = WeakPtr<RenderClockHand>(renderClockHand_)]() {
            auto renderClockHand = wp.Upgrade();
            if (renderClockHand) {
                renderClockHand->SetNeedStop(true);
            }
        });
        setScreenCallback_ = true;
    }
    MarkNeedLayout();
}

void RenderClock::UseDayConfig()
{
    UpdateRenderImage(renderClockFace_, declaration_->GetClockFaceSrc());

    UpdateRenderImage(renderHourHand_, declaration_->GetHourHandSrc());
    renderClockHand_->SetHourHand(renderHourHand_);
    UpdateRenderImage(renderMinuteHand_, declaration_->GetMinuteHandSrc());
    renderClockHand_->SetMinuteHand(renderMinuteHand_);
    UpdateRenderImage(renderSecondHand_, declaration_->GetSecondHandSrc());
    renderClockHand_->SetSecondHand(renderSecondHand_);
    renderClockHand_->SetIsDay(true);
}

void RenderClock::UseNightConfig()
{
    UpdateRenderImage(renderClockFace_, clockFaceNightSrc_);

    UpdateRenderImage(renderHourHand_, hourHandNightSrc_);
    renderClockHand_->SetHourHand(renderHourHand_);
    UpdateRenderImage(renderMinuteHand_, minuteHandNightSrc_);
    renderClockHand_->SetMinuteHand(renderMinuteHand_);
    UpdateRenderImage(renderSecondHand_, secondHandNightSrc_);
    renderClockHand_->SetSecondHand(renderSecondHand_);
    renderClockHand_->SetIsDay(false);
}

void RenderClock::CheckNightConfig()
{
    UseDaySourceIfEmpty(declaration_->GetClockFaceSrc(), clockFaceNightSrc_);
    UseDaySourceIfEmpty(declaration_->GetHourHandSrc(), hourHandNightSrc_);
    UseDaySourceIfEmpty(declaration_->GetMinuteHandSrc(), minuteHandNightSrc_);
    UseDaySourceIfEmpty(declaration_->GetSecondHandSrc(), secondHandNightSrc_);
    if (digitColorNight_ == Color::TRANSPARENT) {
        digitColorNight_ = declaration_->GetDigitColor();
    }
}

void RenderClock::UpdateAccessibilityInfo(double hour, double minute)
{
    auto node = GetAccessibilityNode().Upgrade();
    if (!node) {
        return;
    }
    std::string prefix = minute < 10 ? "0" : "";
    std::string minuteToString =  prefix.append(std::to_string(static_cast<int32_t>(minute)));
    std::string content = std::to_string(static_cast<int32_t>(hour)).append(":").append(minuteToString);
    node->SetText(content);

    auto context = context_.Upgrade();
    if (context) {
        AccessibilityEvent accessibilityEvent;
        accessibilityEvent.nodeId = node->GetNodeId();
        accessibilityEvent.eventType = "textchange";
        context->SendEventToAccessibility(accessibilityEvent);
    }
}

void RenderClock::UpdateRenderText(double digitSize, const Color& digitColor)
{
    if (digitComponentNodes_.size() != digitRenderNodes_.size()) {
        LOGE("Size of [digitComponentNodes] and [digitRenderNodes] does not match! Please check!");
        return;
    }
    TextStyle textStyle;
    textStyle.SetAllowScale(false);
    textStyle.SetTextColor(digitColor);
    textStyle.SetFontSize(Dimension(digitSize));
    textStyle.SetFontFamilies(declaration_->GetFontFamilies());

    for (size_t i = 0; i < digitRenderNodes_.size(); i++) {
        const auto& textComponent = digitComponentNodes_[i];
        textComponent->SetTextStyle(textStyle);
        textComponent->SetFocusColor(digitColor);
        digitRenderNodes_[i]->Attach(GetContext());
        digitRenderNodes_[i]->Update(textComponent);
    }
}

void RenderClock::UpdateRenderImage(RefPtr<RenderImage>& renderImage, const std::string& imageSrc)
{
    RefPtr<ImageComponent> imageComponent = AceType::MakeRefPtr<ImageComponent>(imageSrc);
    renderImage->Attach(GetContext());
    renderImage->Update(imageComponent);
}

void RenderClock::PerformLayout()
{
    defaultSize_ = Dimension(NormalizeToPx(defaultSize_), DimensionUnit::PX);
    CalculateLayoutSize();
    SetLayoutSize(GetLayoutParam().Constrain(drawSize_));
    LayoutClockImage(renderClockFace_, drawSize_);
    auto textColor = renderClockHand_->GetIsDay() ? declaration_->GetDigitColor() : digitColorNight_;
    if (declaration_->GetShowDigit()) {
        LayoutParam textLayoutParam = GetLayoutParam();
        textLayoutParam.SetMinSize(Size());
        UpdateRenderText(drawSize_.Width() * digitSizeRatio_, textColor);
        for (const auto& renderDigit : digitRenderNodes_) {
            renderDigit->Layout(textLayoutParam);
        }
    }

    LayoutParam layoutParam = GetLayoutParam();
    layoutParam.SetMaxSize(drawSize_);
    renderClockHand_->Layout(layoutParam);

    paintOffset_ = Alignment::GetAlignPosition(GetLayoutSize(), drawSize_, Alignment::CENTER);
}

void RenderClock::CalculateLayoutSize()
{
    auto maxSize = GetLayoutParam().GetMaxSize();
    if (!maxSize.IsValid()) {
        LOGE("LayoutParam invalid!");
        return;
    }
    uint8_t infiniteStatus =
        (static_cast<uint8_t>(maxSize.IsWidthInfinite()) << 1) | static_cast<uint8_t>(maxSize.IsHeightInfinite());
    drawSize_ = maxSize;
    switch (infiniteStatus) {
        case 0b00: // both width and height are valid
            break;
        case 0b01: // width is valid but height is infinite
            drawSize_.SetHeight(defaultSize_.Value());
            break;
        case 0b10: // height is valid but width is infinite
            drawSize_.SetWidth(defaultSize_.Value());
            break;
        case 0b11: // both width and height are infinite
        default:
            drawSize_ = Size(defaultSize_.Value(), defaultSize_.Value());
            break;
    }
    double shorterEdge = std::min(drawSize_.Width(), drawSize_.Height());
    drawSize_ = Size(shorterEdge, shorterEdge);
}

void RenderClock::LayoutClockImage(const RefPtr<RenderImage>& renderImage, const Size& imageComponentSize)
{
    LayoutParam imageLayoutParam;
    imageLayoutParam.SetFixedSize(imageComponentSize);
    renderImage->SetImageComponentSize(imageComponentSize);
    renderImage->Layout(imageLayoutParam);
}

void RenderClockHand::PerformLayout()
{
    SetLayoutSize(GetLayoutParam().GetMaxSize());
    auto clockSize = GetLayoutSize();
    RenderClock::LayoutClockImage(renderHourHand_, Size(clockSize.Width() * HOUR_HAND_RATIO, clockSize.Height()));
    RenderClock::LayoutClockImage(renderMinuteHand_, Size(clockSize.Width() * HOUR_HAND_RATIO, clockSize.Height()));
    RenderClock::LayoutClockImage(renderSecondHand_, Size(clockSize.Width() * HOUR_HAND_RATIO, clockSize.Height()));
}

} // namespace OHOS::Ace