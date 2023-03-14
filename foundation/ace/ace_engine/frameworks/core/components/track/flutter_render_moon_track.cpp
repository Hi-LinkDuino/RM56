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

#include "flutter_render_moon_track.h"

#include "flutter/lib/ui/painting/path.h"

#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

void FlutterRenderMoonTrack::Paint(RenderContext& context, const Offset& offset)
{
    Size canvasSize = GetLayoutSize();
    Offset center = offset + Offset(canvasSize.Width() / 2, canvasSize.Height() / 2);
    double radius = std::min(canvasSize.Width(), canvasSize.Height()) / 2;
    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setAntiAlias(true);
    paint.paint()->setColor(GetSelectColor().GetValue());
    paint.paint()->setStyle(SkPaint::kFill_Style);
    auto canvas = ScopedCanvas::Create(context);
    auto path = flutter::CanvasPath::Create();

    SkPaint backgroundPaint;
    backgroundPaint.setStyle(SkPaint::kFill_Style);
    backgroundPaint.setAntiAlias(true);
    backgroundPaint.setColor(GetBackgroundColor().GetValue());
    canvas->canvas()->drawCircle(center.GetX(), center.GetY(), radius, backgroundPaint);

    if (GetTotalRatio() <= 0.5) {
        path->moveTo(center.GetX(), center.GetY() - radius);
        path->addArc(center.GetX() - radius, center.GetY() - radius, center.GetX() + radius, center.GetY() + radius,
            M_PI / 2, M_PI);
        double progressOffset = radius - radius * GetTotalRatio() / 0.5;
        path->addArc(center.GetX() - progressOffset, center.GetY() - radius, center.GetX() + progressOffset,
            center.GetY() + radius, 3 * M_PI / 2, -M_PI);
        canvas->drawPath(path.get(), paint, paintData);
    } else {
        double progressOffset = radius * (GetTotalRatio() - 0.5) / 0.5;
        path->moveTo(center.GetX(), center.GetY() - radius);
        path->addArc(center.GetX() - radius, center.GetY() - radius, center.GetX() + radius, center.GetY() + radius,
            M_PI / 2, M_PI);

        path->addArc(center.GetX() - progressOffset, center.GetY() - radius, center.GetX() + progressOffset,
            center.GetY() + radius, 3 * M_PI / 2, M_PI);
        canvas->drawPath(path.get(), paint, paintData);
    }
}

} // namespace OHOS::Ace