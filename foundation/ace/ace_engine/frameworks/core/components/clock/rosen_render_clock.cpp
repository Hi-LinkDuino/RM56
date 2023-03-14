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

#include "core/components/clock/rosen_render_clock.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkMaskFilter.h"

#include "base/json/json_util.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {
constexpr double HOUR_ANGLE_UNIT = 360.0 / 12;
constexpr double MINUTE_ANGLE_UNIT = 360.0 / 60;
constexpr double SECOND_ANGLE_UNIT = 360.0 / 60;
constexpr int32_t INTERVAL_OF_USECOND = 1000000;
constexpr int32_t MICROSECONDS_OF_MILLISECOND = 1000;
} // namespace

void RosenRenderClock::Paint(RenderContext& context, const Offset& offset)
{
    auto renderOffset = paintOffset_ + offset;

    // paint clock face and digit
    context.PaintChild(renderClockFace_, renderOffset);
    if (declaration_->GetShowDigit()) {
        RenderDigit(context, offset);
    }

    // paint clock hand
    context.PaintChild(renderClockHand_, renderOffset);
    auto rosenRenderClockHand = AceType::DynamicCast<RosenRenderClockHand>(renderClockHand_);
    rosenRenderClockHand->RequestRenderForNextSecond();
}

void RosenRenderClock::PerformLayout()
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
        double innerRadius = drawSize_.Width() * digitRadiusRatio_ / 2.0;
        Offset center = paintOffset_ + Offset(drawSize_.Width() / 2.0, drawSize_.Height() / 2.0);
        int i = 0;
        for (const auto& renderDigit : digitRenderNodes_) {
            renderDigit->Layout(textLayoutParam);
            auto halfDigitWidth = renderDigit->GetLayoutSize().Width() / 2.0;
            auto halfDigitHeight = renderDigit->GetLayoutSize().Height() / 2.0;
            auto digitOffset = Offset(center.GetX() + sin(radians_[i]) * innerRadius - halfDigitWidth,
                                      center.GetY() - cos(radians_[i]) * innerRadius - halfDigitHeight);
            renderDigit->SetPosition(digitOffset);
            ++i;
        }
    }

    LayoutParam layoutParam = GetLayoutParam();
    layoutParam.SetMaxSize(drawSize_);
    renderClockHand_->Layout(layoutParam);
    renderClockHand_->MarkNeedRender();
    paintOffset_ = Alignment::GetAlignPosition(GetLayoutSize(), drawSize_, Alignment::CENTER);
}

void RosenRenderClock::RenderDigit(RenderContext& context, const Offset& offset)
{
    for (size_t i = 0; i < radians_.size(); i++) {
        context.PaintChild(digitRenderNodes_[i], offset);
    }
}

void RosenRenderClockHand::LayoutHand(const Offset& offset,
    const RefPtr<RenderImage>& renderHand, double rotateAngle)
{
    renderHand->SetRotate(rotateAngle);
    renderHand->SetPosition(offset);
    renderHand->MarkNeedRender();
}

void RosenRenderClockHand::RequestRenderForNextSecond()
{
    auto timeOfNow = GetTimeOfNow(hoursWest_);
    auto timeUsec = timeOfNow.timeUsec_;
    // 1 second = 1000 millisecond = 1000000 microsecond.
    // Millisecond is abbreviated as msec. Microsecond is abbreviated as usec.
    // unit of [delayTime] is msec, unit of [tv_usec] is usec
    // when [tv_usec] is 000100, (INTERVAL_OF_USECOND - timeUsec) / MICROSECONDS_OF_MILLISECOND = 999 msec
    // which will cause the delay task still arriving in current second, because 999000 + 000100 = 999100 < 1 second
    // so add an additional millisecond to modify the loss of precision during division
    int32_t delayTime = (INTERVAL_OF_USECOND - timeUsec) / MICROSECONDS_OF_MILLISECOND + 1; // millisecond

    auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        return;
    }
    auto isInScreen = pipelineContext->IsWindowInScreen();
    if (IsAppShow() && (isInScreen || onPreDraw_)) {
        if (onPreDraw_ && isInScreen) {
            onPreDraw_ = false;
        }
        if (onPreDraw_ && !isInScreen) {
            // onPreDraw && !isScreen means view slide not stable, add stableCnt
            ++stableCnt_;
            // deal with when slide out of the screen so quickly that onPreDraw can not be setted false.
            // if (onPreDraw_ && !isInScreen) constants more than 2 second, means slide stable, set onPreDraw to false.
            if (stableCnt_ > 2) {
                onPreDraw_ = false;
                stableCnt_ = 0;
            }
        }
        pipelineContext->GetTaskExecutor()->PostDelayedTask(
            [wp = WeakClaim(this)] {
                auto renderClockHand = wp.Upgrade();
                if (!renderClockHand) {
                    LOGE("RenderClockHand is null!");
                    return;
                }
                auto pipelineContext = renderClockHand->GetContext().Upgrade();
                if (!pipelineContext) {
                    LOGE("PipelineContext is null when RequestRenderForNextSecond");
                    return;
                }
                renderClockHand->RequestRenderForNextSecond();
                bool needRender = !(renderClockHand->GetHidden() || !renderClockHand->GetVisible());
                if (needRender) {
                    renderClockHand->MarkNeedRender();
                    renderClockHand->MarkNeedLayout();
                    return;
                }
            },
            TaskExecutor::TaskType::UI, delayTime);
        return;
    }
    pipelineContext->AddNodesToNotifyOnPreDraw(Claim(this));
}

void RosenRenderClockHand::PerformLayout()
{
    RenderClockHand::PerformLayout();
    auto timeOfNow = GetTimeOfNow(hoursWest_);
    // case [10] means that time travels from light to dark, case [01] means that time travels from dark to light
    uint8_t dayNightStatus = (static_cast<uint8_t>(isDay_) << 1) | static_cast<uint8_t>(IsDayTime(timeOfNow));
    switch (dayNightStatus) {
        case 0b10:
            dayToNightCallback_();
            return;
        case 0b01:
            nightToDayCallback_();
            return;
        default:
            break;
    }
    auto minute = static_cast<int32_t>(timeOfNow.minute_);
    if (curMinute_ != minute) {
        curMinute_ = minute;
        accessibilityTimeCallback_(timeOfNow.hour24_, timeOfNow.minute_);
    }

    auto pipelineContext = context_.Upgrade();
    if (pipelineContext && (pipelineContext->IsJsCard()) && onHourCallback_) {
        auto hour = static_cast<int32_t>(timeOfNow.hour24_);
        if (curHour_ != hour) {
            curHour_ = hour;
            auto json = JsonUtil::Create(true);
            json->Put("hour", curHour_);
            onHourCallback_(json->ToString());
        }
    }
    if (GetHidden() || !GetVisible() || !isAppOnShow_) {
        return;
    }
    auto clockSize = GetLayoutSize();
    auto handOffset = Offset((clockSize.Width() - renderHourHand_->GetLayoutSize().Width()) / 2.0, 0.0);
    LayoutHand(handOffset, renderMinuteHand_, timeOfNow.minute_ * MINUTE_ANGLE_UNIT);
    LayoutHand(handOffset, renderHourHand_, timeOfNow.hour12_ * HOUR_ANGLE_UNIT);
    LayoutHand(handOffset, renderSecondHand_, timeOfNow.second_ * SECOND_ANGLE_UNIT);
}

void RosenRenderClockHand::Paint(RenderContext& context, const Offset& offset)
{
    context.PaintChild(renderMinuteHand_, offset);
    context.PaintChild(renderHourHand_, offset);
    context.PaintChild(renderSecondHand_, offset);
}

void RosenRenderClockHand::OnAppShow()
{
    RenderNode::OnAppShow();
    MarkNeedRender();
    RequestRenderForNextSecond();
}

void RosenRenderClockHand::OnPreDraw()
{
    onPreDraw_ = true;
    stableCnt_ = 0; // if onPreDraw called, clock maybe slide unstably, stableCnt_ should be setted to 0.
    MarkNeedRender();
    RequestRenderForNextSecond();
}
} // namespace OHOS::Ace