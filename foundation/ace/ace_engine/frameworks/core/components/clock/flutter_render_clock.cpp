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

#include "core/components/clock/flutter_render_clock.h"

#include "flutter/common/task_runners.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkMaskFilter.h"

#include "base/json/json_util.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {
namespace {

constexpr double HOUR_ANGLE_UNIT = 360.0 / 12;
constexpr double MINUTE_ANGLE_UNIT = 360.0 / 60;
constexpr double SECOND_ANGLE_UNIT = 360.0 / 60;
constexpr int32_t INTERVAL_OF_USECOND = 1000000;
constexpr int32_t MICROSECONDS_OF_MILLISECOND = 1000;

} // namespace

void FlutterRenderClock::Paint(RenderContext& context, const Offset& offset)
{
    auto renderOffset = paintOffset_ + offset;

    // paint clock face and digit
    context.PaintChild(renderClockFace_, renderOffset);
    Offset rotateCenter = renderOffset + Offset(drawSize_.Width() / 2.0, drawSize_.Height() / 2.0);
    if (declaration_->GetShowDigit()) {
        RenderDigit(context, rotateCenter);
    }

    // paint clock hand
    context.PaintChild(renderClockHand_, renderOffset);
    auto flutterRenderClockHand = AceType::DynamicCast<FlutterRenderClockHand>(renderClockHand_);
    flutterRenderClockHand->RequestRenderForNextSecond();
}

void FlutterRenderClock::RenderDigit(RenderContext& context, const Offset& center)
{
    if (radians_.size() != digitRenderNodes_.size()) {
        LOGE("Size of [radians] and [digitRenderNodes] does not match! Please check!");
        return;
    }
    double innerRadius = drawSize_.Width() * digitRadiusRatio_ / 2.0;
    for (size_t i = 0; i < radians_.size(); i++) {
        auto halfDigitWidth = digitRenderNodes_[i]->GetLayoutSize().Width() / 2.0;
        auto halfDigitHeight = digitRenderNodes_[i]->GetLayoutSize().Height() / 2.0;
        auto digitOffset = Offset(center.GetX() + sin(radians_[i]) * innerRadius - halfDigitWidth,
            center.GetY() - cos(radians_[i]) * innerRadius - halfDigitHeight);
        context.PaintChild(digitRenderNodes_[i], digitOffset);
    }
}

void FlutterRenderClockHand::RenderHand(RenderContext& context, const Offset& offset,
    const RefPtr<RenderImage>& renderHand, const Offset& rotateCenter, double rotateAngle)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    canvas->canvas()->rotate(rotateAngle, rotateCenter.GetX(), rotateCenter.GetY());
    context.PaintChild(renderHand, offset);
}

void FlutterRenderClockHand::RequestRenderForNextSecond()
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
                    return;
                }
            },
            TaskExecutor::TaskType::UI, delayTime);
        return;
    }
    pipelineContext->AddNodesToNotifyOnPreDraw(Claim(this));
}

void FlutterRenderClockHand::Paint(RenderContext& context, const Offset& offset)
{
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
    auto handOffset = offset + Offset((clockSize.Width() - renderHourHand_->GetLayoutSize().Width()) / 2.0, 0.0);
    Offset rotateCenter = offset + Offset(clockSize.Width() / 2.0, clockSize.Height() / 2.0);

    RenderHand(context, handOffset, renderMinuteHand_, rotateCenter, timeOfNow.minute_ * MINUTE_ANGLE_UNIT);
    RenderHand(context, handOffset, renderHourHand_, rotateCenter, timeOfNow.hour12_ * HOUR_ANGLE_UNIT);
    RenderHand(context, handOffset, renderSecondHand_, rotateCenter, timeOfNow.second_ * SECOND_ANGLE_UNIT);
}

void FlutterRenderClockHand::OnAppShow()
{
    RenderNode::OnAppShow();
    MarkNeedRender();
    RequestRenderForNextSecond();
}

void FlutterRenderClockHand::OnPreDraw()
{
    onPreDraw_ = true;
    stableCnt_ = 0; // if onPreDraw called, clock maybe slide unstably, stableCnt_ should be setted to 0.
    MarkNeedRender();
    RequestRenderForNextSecond();
}

} // namespace OHOS::Ace