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

#include "core/components/track/flutter_render_capsule_track.h"

#include "core/pipeline/base/scoped_canvas_state.h"

#include "flutter/lib/ui/painting/path.h"

namespace OHOS::Ace {
void FlutterRenderCapsuleTrack::DrawShape(RenderContext& context, const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    Size canvasSize = GetLayoutSize();
    double capsuleHeight = canvasSize.Height();
    double capsuleWidth = canvasSize.Width();
    Size progressSize = Size(capsuleWidth, capsuleHeight);
    double rrectRadius = progressSize.Height() / 2.0;

    flutter::Paint paint;
    paint.paint()->setColor(GetBackgroundColor().GetValue());
    paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    paint.paint()->setAntiAlias(true);
    flutter::RRect rRect;
    flutter::PaintData paintData;

    rRect.sk_rrect.setRectXY(SkRect::MakeIWH(progressSize.Width(), progressSize.Height()), rrectRadius, rrectRadius);

    rRect.sk_rrect.offset(offset.GetX(), offset.GetY());
    canvas->drawRRect(rRect, paint, paintData);
}

void FlutterRenderCapsuleTrack::DrawCapsuleProgressAnimation(RenderContext& context,
                                                             const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    double offsetX = offset.GetX();
    double offsetY = offset.GetY();
    Size canvasSize = GetLayoutSize();
    double capsuleHeight = canvasSize.Height();
    double capsuleWidth = canvasSize.Width();
    Size progressSize = Size(capsuleWidth, capsuleHeight);

    double radius = progressSize.Height() / 2.0;

    double progressWidth = progressSize.Width()*GetTotalRatio();

    auto path = flutter::CanvasPath::Create();
    path->addArc(offsetX, offsetY, progressSize.Height() + offsetX,
        progressSize.Height() + offsetY, M_PI * 0.5, M_PI);
    if (LessNotEqual(progressWidth, radius)) {
        path->addArc(progressWidth + offsetX, offsetY,
            progressSize.Height() - progressWidth + offsetX,
            progressSize.Height() + offsetY, M_PI * 1.5, -M_PI);
    } else if (GreatNotEqual(progressWidth, progressSize.Width() - radius)) {
        path->addRect(radius + offsetX, offsetY,
            progressSize.Width() - radius + offsetX,
            progressSize.Height() + offsetY);
        path->addArc((progressSize.Width() - radius) * 2.0 - progressWidth + offsetX,
            offsetY,
            progressWidth + offsetX,
            progressSize.Height() + offsetY,
            M_PI * 1.5, M_PI);
    } else {
        path->addRect(radius + offsetX,
                      offsetY,
                      progressWidth + offsetX,
                      progressSize.Height() + offsetY);
    }

    flutter::Paint paint;
    paint.paint()->setColor(GetSelectColor().GetValue());
    paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    paint.paint()->setAntiAlias(true);
    flutter::PaintData paintData;
    canvas->drawPath(path.get(), paint, paintData);
}

void FlutterRenderCapsuleTrack::DrawCapsuleProgressVerticalAnimation(RenderContext& context, const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    double offsetX = offset.GetX();
    double offsetY = offset.GetY();
    Size canvasSize = GetLayoutSize();
    double height_ = canvasSize.Height();
    double width_ = canvasSize.Width();
    Size progressSize_ = Size(width_, height_);
    double progressWidth_ = progressSize_.Height()*GetTotalRatio();
    double radius = progressSize_.Width() / 2.0;
    auto path = flutter::CanvasPath::Create();

    path->addArc(offsetX, offsetY, progressSize_.Width() + offsetX, progressSize_.Width() + offsetY, 0, -M_PI);
    if (LessNotEqual(progressWidth_, radius)) {
        path->addArc(offsetX, offsetY + progressWidth_, progressSize_.Width() + offsetX,
            progressSize_.Width() - progressWidth_ + offsetY, M_PI, M_PI);
    } else if (GreatNotEqual(progressWidth_, progressSize_.Height() - radius)) {
        path->addRect(offsetX, offsetY + radius, progressSize_.Width() + offsetX,
            progressSize_.Height() - radius + offsetY);
        path->addArc(offsetX, offsetY + (progressSize_.Height() - radius) * 2.0 - progressWidth_,
            progressSize_.Width() + offsetX, progressWidth_ + offsetY, M_PI, -M_PI);
    } else {
        path->addRect(offsetX, radius + offsetY, offsetX + progressSize_.Width(), progressWidth_ + offsetY);
    }

    flutter::Paint paint;
    paint.paint()->setColor(GetSelectColor().GetValue());
    paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    paint.paint()->setAntiAlias(true);
    flutter::PaintData paintData;
    canvas->drawPath(path.get(), paint, paintData);
}

void FlutterRenderCapsuleTrack::Paint(RenderContext& context, const Offset& offset)
{
    DrawShape(context, offset);
    Size canvasSize = GetLayoutSize();
    double height = canvasSize.Height();
    double width = canvasSize.Width();
    if (width >= height) {
        DrawCapsuleProgressAnimation(context, offset);
    } else {
        DrawCapsuleProgressVerticalAnimation(context, offset);
    }
}
} // namespace OHOS::Ace