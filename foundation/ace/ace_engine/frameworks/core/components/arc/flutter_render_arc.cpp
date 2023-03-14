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

#include "core/components/arc/flutter_render_arc.h"

#include "third_party/skia/include/core/SkMaskFilter.h"

namespace OHOS::Ace {

void FlutterRenderArc::Paint(RenderContext& context, const Offset& offset)
{
    if (!IsAcrValid()) {
        LOGE("IsAcrValid is false.");
        return;
    }

    if (!IsArcShow()) {
        LOGE("IsAcrShow is false.");
        return;
    }

    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }

    double widthHalf = width_ / ARC_RADIUS_TO_DIAMETER;
    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setAntiAlias(true);
    paint.paint()->setColor(color_.GetValue());
    paint.paint()->setStrokeWidth(width_);
    paint.paint()->setStrokeCap(SkPaint::kRound_Cap);

    if (!NearZero(width_)) {
        paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
        if (!NearZero(shadowWidth_)) {
            paint.paint()->setMaskFilter(
                SkMaskFilter::MakeBlur(SkBlurStyle::kSolid_SkBlurStyle, static_cast<float>(shadowWidth_)));
        }
        canvas->drawArc(offset.GetX() + widthHalf, offset.GetY() + widthHalf,
            offset.GetX() + arcRadiusX_ * ARC_RADIUS_TO_DIAMETER - widthHalf,
            offset.GetY() + arcRadiusY_ * ARC_RADIUS_TO_DIAMETER - widthHalf, startAngle_, sweepAngle_, false, paint,
            paintData);
    } else {
        paint.paint()->setStyle(SkPaint::Style::kFill_Style);
        canvas->drawArc(offset.GetX(), offset.GetY(), offset.GetX() + arcRadiusX_ * ARC_RADIUS_TO_DIAMETER,
            offset.GetY() + arcRadiusY_ * ARC_RADIUS_TO_DIAMETER, startAngle_, sweepAngle_, true, paint, paintData);
    }
}

} // namespace OHOS::Ace
