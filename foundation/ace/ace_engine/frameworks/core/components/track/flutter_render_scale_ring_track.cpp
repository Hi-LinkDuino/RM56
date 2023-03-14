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

#include "core/components/track/flutter_render_scale_ring_track.h"

#include "flutter/lib/ui/ui_dart_state.h"
#include "third_party/skia/include/effects/Sk1DPathEffect.h"

#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {
namespace {

void DrawScaleArc(RenderContext& context, const RenderRingInfo& trackInfo)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    flutter::Paint paint;
    flutter::PaintData paintData;
    SkPath path;
    path.addRRect(SkRRect::MakeRectXY(SkRect::MakeWH(trackInfo.scaleStrokeWidth, trackInfo.thickness),
        trackInfo.thickness / 2.0, trackInfo.thickness / 2.0));
    double pathDistance = 2.0 * M_PI *
                          (trackInfo.radius + (NearEqual(trackInfo.clockwise, 1.0) ? trackInfo.thickness : 0.0)) /
                          trackInfo.totalScaleNumber;
    paint.paint()->setPathEffect(
        SkPath1DPathEffect::Make(path, static_cast<SkScalar>(pathDistance), 0.0f, SkPath1DPathEffect::kRotate_Style));
    paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
    paint.paint()->setStrokeWidth(trackInfo.thickness);
    paint.paint()->setAntiAlias(true);
    paint.paint()->setColor(trackInfo.color.GetValue());

    static int32_t totalDegree = 360;
    double radiusPrecision = trackInfo.thickness;
    if (trackInfo.clockwise != 1) {
        radiusPrecision = 0.0;
    }
    canvas->drawArc(trackInfo.center.GetX() - trackInfo.radius - radiusPrecision,
        trackInfo.center.GetY() - trackInfo.radius - radiusPrecision,
        trackInfo.center.GetX() + trackInfo.radius + radiusPrecision,
        trackInfo.center.GetY() + trackInfo.radius + radiusPrecision,
        M_PI * (trackInfo.clockwise * (trackInfo.startDegree / (totalDegree / 2.0)) - 0.5),
        M_PI * 2 * (trackInfo.clockwise * trackInfo.sweepDegree / totalDegree), false, paint, paintData);
}
} // namespace

void FlutterRenderScaleRingTrack::Paint(RenderContext& context, const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    RenderRingInfo data = paintData_;
    data.center = offset + Offset(GetLayoutSize().Width() / 2.0, GetLayoutSize().Height() / 2.0);
    data.radius = (GetLayoutSize().Width() / 2.0) - data.thickness;

    if (data.totalScaleNumber == 0) {
        LOGE("Scale number error. Use default number(120)");
        data.totalScaleNumber = 120;
    }
    data.color = GetBackgroundColor();
    DrawScaleArc(context, data);

    data.color = GetCachedColor();
    data.sweepDegree = paintData_.sweepDegree * GetCachedRatio();
    DrawScaleArc(context, data);

    data.color = GetSelectColor();
    data.sweepDegree = paintData_.sweepDegree * GetTotalRatio();
    DrawScaleArc(context, data);
}

} // namespace OHOS::Ace
