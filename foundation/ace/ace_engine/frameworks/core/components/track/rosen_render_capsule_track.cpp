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

#include "rosen_render_capsule_track.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"

#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
void RosenRenderCapsuleTrack::DrawShape(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    Size canvasSize = GetLayoutSize();
    double progressHeight = canvasSize.Height();
    double progressWidth = canvasSize.Width();
    Size progressSize = Size(progressWidth, progressHeight);
    double rrectRadius = progressSize.Height() / 2.0;

    SkPaint paint;
    paint.setColor(GetBackgroundColor().GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setAntiAlias(true);
    SkRRect rRect;

    rRect.setRectXY(SkRect::MakeIWH(progressSize.Width(), progressSize.Height()), rrectRadius, rrectRadius);

    rRect.offset(offset.GetX(), offset.GetY());
    canvas->drawRRect(rRect, paint);
}

void RosenRenderCapsuleTrack::DrawCapsuleProgressAnimation(RenderContext& context,
                                                           const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    double offsetX = offset.GetX();
    double offsetY = offset.GetY();

    Size canvasSize = GetLayoutSize();
    double capsuleHeight = canvasSize.Height();
    double capsuleWidth  = canvasSize.Width();
    Size progressSize = Size(capsuleWidth, capsuleHeight);

    double radius = progressSize.Height() / 2.0;
    
    double progressWidth = progressSize.Width()*GetTotalRatio();

    SkPath path;
    path.addArc({ offsetX, offsetY, progressSize.Height() + offsetX, progressSize.Height() + offsetY }, 90, 180);
    if (LessNotEqual(progressWidth, radius)) {
        path.addArc({ progressWidth + offsetX, offsetY, progressSize.Height() - progressWidth + offsetX,
                        progressSize.Height() + offsetY },
            270, -180);
    } else if (GreatNotEqual(progressWidth, progressSize.Width() - radius)) {
        path.addRect(
            { radius + offsetX, offsetY, progressSize.Width() - radius + offsetX, progressSize.Height() + offsetY });
        path.addArc({ (progressSize.Width() - radius) * 2.0 - progressWidth + offsetX, offsetY, progressWidth + offsetX,
                        progressSize.Height() + offsetY },
            270, 180);
    } else {
        path.addRect({ radius + offsetX, offsetY, progressWidth + offsetX, progressSize.Height() + offsetY });
    }

    SkPaint paint;
    paint.setColor(GetSelectColor().GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setAntiAlias(true);
    canvas->drawPath(path, paint);
}

void RosenRenderCapsuleTrack::DrawCapsuleProgressVerticalAnimation(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
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

    double radius = progressSize.Width() / 2.0;
    double progressWidth = progressSize.Height()*GetTotalRatio();

    SkPath path;
    path.addArc({ offsetX, offsetY, progressSize.Width() + offsetX, progressSize.Width() + offsetY }, 0, -180);
    if (LessNotEqual(progressWidth, radius)) {
        path.addArc({ offsetX, offsetY + progressWidth, progressSize.Width() + offsetX,
                        progressSize.Width() - progressWidth + offsetY },
            180, 180);
    } else if (GreatNotEqual(progressWidth, progressSize.Height() - radius)) {
        path.addRect(
            { offsetX, offsetY + radius, progressSize.Width() + offsetX, progressSize.Height() - radius + offsetY });
        path.addArc({ offsetX, offsetY + (progressSize.Height() - radius) * 2.0 - progressWidth,
                        progressSize.Width() + offsetX, progressWidth + offsetY },
            180, -180);
    } else {
        path.addRect({ offsetX, radius + offsetY, offsetX + progressSize.Width(), progressWidth + offsetY });
    }

    SkPaint paint;
    paint.setColor(GetSelectColor().GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setAntiAlias(true);
    canvas->drawPath(path, paint);
}

void RosenRenderCapsuleTrack::Paint(RenderContext& context, const Offset& offset)
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