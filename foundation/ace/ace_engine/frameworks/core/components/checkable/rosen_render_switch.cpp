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

#include "core/components/checkable/rosen_render_switch.h"

#include "render_service_client/core/ui/rs_node.h"
#include "third_party/skia/include/core/SkRRect.h"

#include "core/components/checkable/checkable_component.h"
#include "core/components/common/painter/rosen_checkable_painter.h"
#include "core/components/common/painter/rosen_universal_painter.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

void RosenRenderSwitch::Paint(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    Offset paintOffset = offset + paintPosition_;
    SkPaint trackPaint;
    SkPaint paint;
    trackPaint.setAntiAlias(true);
    paint.setAntiAlias(true);

    double originX = paintOffset.GetX();
    double originY = paintOffset.GetY();
    double pointOriginX = paintOffset.GetX() + currentPointOriginX_;
    double pointOriginY = paintOffset.GetY() + NormalizeToPx(pointPadding_);

    uint32_t trackColor = 0;
    uint32_t pointColor = 0;
    paintTrackSize_ = switchSize_;
    SetPaintStyle(originX, originY, trackColor, pointColor, trackPaint);
    if (IsPhone() && onFocus_) {
        RequestFocusBorder(paintOffset, switchSize_, switchSize_.Height() / 2.0);
    }

    // paint track rect
    if (!isSwitchDuringAnimation_) {
#ifdef OHOS_PLATFORM
        auto recordingCanvas = static_cast<Rosen::RSRecordingCanvas*>(canvas);
        recordingCanvas->SaveAlpha();
        recordingCanvas->MultiplyAlpha(ConfigureOpacity(disabled_));
        PaintTrack(canvas, trackPaint, originX, originY, trackColor);
        recordingCanvas->RestoreAlpha();
#else
        PaintTrack(canvas, trackPaint, originX, originY, trackColor);
#endif
    } else {
        // current status is during the switch on/off
        DrawTrackAnimation(paintOffset, canvas, trackPaint);
    }

    // paint center point
    PaintCenterPoint(canvas, paint, pointOriginX, pointOriginY, pointColor);

    // paint text
    if (renderTextOn_ && renderTextOff_ && showText_) {
        PaintText(Offset(pointOriginX, pointOriginY), context);
    }
}

void RosenRenderSwitch::PaintTrack(
    SkCanvas* canvas, SkPaint& trackPaint, double originX, double originY, uint32_t trackColor) const
{
    auto& skPaint = trackPaint;
    SkRRect rrect;
    double trackRadius = switchSize_.Height() / 2.0;
    rrect =
        SkRRect::MakeRectXY({ originX, originY, paintTrackSize_.Width() + originX, paintTrackSize_.Height() + originY },
            trackRadius, trackRadius);
    skPaint.setAntiAlias(true);
    skPaint.setColor(trackColor);
    canvas->drawRRect(rrect, skPaint);
}

void RosenRenderSwitch::PaintCenterPoint(
    SkCanvas* canvas, SkPaint& paint, double pointOriginX, double pointOriginY, uint32_t pointColor) const
{
    auto& skPaint = paint;
    SkRRect rrect;
    skPaint.setAntiAlias(true);
    skPaint.setColor(pointColor);
    rrect = SkRRect::MakeRectXY(
        { pointOriginX, pointOriginY, rawPointSize_.Width() + pointOriginX, rawPointSize_.Height() + pointOriginY },
        pointRadius_, pointRadius_);
    canvas->drawRRect(rrect, skPaint);
    double shadowBorderWidth = NormalizeToPx(shadowWidth_);
    SetStrokeWidth(shadowBorderWidth, paint);
    skPaint.setColor(shadowColor_);
    rrect = SkRRect::MakeRectXY({ pointOriginX - shadowBorderWidth, pointOriginY - shadowBorderWidth,
                                    rawPointSize_.Width() + pointOriginX + shadowBorderWidth,
                                    rawPointSize_.Height() + pointOriginY + shadowBorderWidth },
        pointRadius_ + shadowBorderWidth, pointRadius_ + shadowBorderWidth);
    canvas->drawRRect(rrect, skPaint);
}

void RosenRenderSwitch::DrawTrackAnimation(const Offset& paintOffset, SkCanvas* canvas, SkPaint& trackPaint) const
{
    if (!canvas) {
        LOGE("DrawTrackAnimation Paint canvas is null");
        return;
    }
    double trackRadius = switchSize_.Height() / 2.0;
    switch (uiStatus_) {
        case UIStatus::OFF_TO_ON:
        case UIStatus::ON_TO_OFF:
            DrawTrackOffAndOn(paintOffset, trackRadius, canvas, trackPaint);
            break;
        default:
            LOGE("DrawTrackAnimation current uiStatus is not correct");
            break;
    }
}

void RosenRenderSwitch::DrawTrackOffAndOn(
    const Offset& paintOffset, double trackRadius, SkCanvas* canvas, SkPaint& trackPaint) const
{
    auto& skPaint = trackPaint;
    auto pointPadding = NormalizeToPx(pointPadding_);
    double originX = paintOffset.GetX();
    double originY = paintOffset.GetY();
    double pointOriginX = paintOffset.GetX() + currentPointOriginX_;
    double pointOriginY = paintOffset.GetY() + pointPadding;
    // paint the bottom track rect first
    SkRRect rrectUpper;
    SkRRect rrectBottom;
    skPaint.setAntiAlias(true);
    rrectBottom =
        SkRRect::MakeRectXY({ originX, originY, paintTrackSize_.Width() + originX, paintTrackSize_.Height() + originY },
            trackRadius, trackRadius);
    skPaint.setColor(inactiveColor_);
    canvas->drawRRect(rrectBottom, skPaint);

    // paint the upper track rect
    auto width = switchSize_.Width() - rawPointSize_.Width() - pointPadding * 3.0;
    if (NearZero(width)) {
        LOGE("the width is zero");
        return;
    }
    double ratio = (currentPointOriginX_ - pointPadding * 2.0) / width;
    double trackStartDelta = pointPadding;
    double trackStartX = originX + pointPadding - (trackStartDelta * ratio);
    double trackEnd = (rawPointSize_.Width() + pointOriginX) + pointPadding * ratio;
    double trackHeightDelta = paintTrackSize_.Height() + originY - (rawPointSize_.Height() + pointOriginY);
    double radiusDelta = trackRadius - pointRadius_;
    double trackStartY = pointOriginY - (pointPadding * ratio);
    rrectUpper = SkRRect::MakeRectXY(
        { trackStartX, trackStartY, trackEnd, rawPointSize_.Height() + pointOriginY + trackHeightDelta * ratio },
        pointRadius_ + radiusDelta * ratio, pointRadius_ + radiusDelta * ratio);
    skPaint.setColor(activeColor_);
    canvas->drawRRect(rrectUpper, skPaint);
}

void RosenRenderSwitch::SetPaintStyle(
    double& originX, double& originY, uint32_t& trackColor, uint32_t& pointColor, SkPaint& trackPaint)
{
    auto borderWidth = NormalizeToPx(borderWidth_);
    switch (uiStatus_) {
        case UIStatus::SELECTED:
            trackColor = activeColor_;
            pointColor = pointColor_;
            isSwitchDuringAnimation_ = false;
            break;
        case UIStatus::UNSELECTED: {
            if (backgroundSolid_) {
                trackColor = inactiveColor_;
                pointColor = pointColor_;
            } else {
                SetStrokeWidth(static_cast<float>(borderWidth), trackPaint);
                double strokeOffset = borderWidth / 2.0;
                paintTrackSize_.SetWidth(switchSize_.Width() - borderWidth);
                paintTrackSize_.SetHeight(switchSize_.Height() - borderWidth);
                originX += strokeOffset;
                originY += strokeOffset;
                trackColor = inactiveColor_;
                pointColor = inactiveColor_;
            }
            isSwitchDuringAnimation_ = false;
            break;
        }
        case UIStatus::FOCUS: { // focus of unselected
            SetStrokeWidth(static_cast<float>(borderWidth), trackPaint);
            double strokeOffset = borderWidth / 2.0;
            paintTrackSize_.SetWidth(switchSize_.Width() - borderWidth);
            paintTrackSize_.SetHeight(switchSize_.Height() - borderWidth);
            originX += strokeOffset;
            originY += strokeOffset;
            trackColor = focusColor_;
            pointColor = focusColor_;
            isSwitchDuringAnimation_ = false;
            break;
        }
        case UIStatus::OFF_TO_ON:
        case UIStatus::ON_TO_OFF: {
            isSwitchDuringAnimation_ = true;
            pointColor = pointColor_;
            break;
        }
        default:
            LOGE("unknown ui status");
            break;
    }
}

Size RosenRenderSwitch::CalculateTextSize(const std::string& text, RefPtr<RenderText>& renderText)
{
    if (!renderText) {
        LOGW("the render text is nullptr");
        return Size();
    }
    renderText->SetTextData(text);
    renderText->PerformLayout();
    return renderText->GetLayoutSize();
}

} // namespace OHOS::Ace
