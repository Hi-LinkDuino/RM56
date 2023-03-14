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

#include "core/components/divider/rosen_render_divider.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"

#include "core/components/divider/render_divider.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

void RosenRenderDivider::Paint(RenderContext& context, const Offset& offset)
{
    if (NearZero(constrainStrokeWidth_) || NearZero(dividerLength_)) {
        return;
    }
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("canvas fetch failed");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(dividerColor_.GetValue());
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setStrokeWidth(constrainStrokeWidth_);

    switch (lineCap_) {
        case LineCap::BUTT:
        case LineCap::SQUARE:
            paint.setStrokeCap(SkPaint::Cap::kSquare_Cap);
            break;
        case LineCap::ROUND:
            paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
            break;
        default:
            break;
    }
    dividerLength_ =
        vertical_ ? GetLayoutSize().Height() - constrainStrokeWidth_ : GetLayoutSize().Width() - constrainStrokeWidth_;

    auto startPointX = offset.GetX() + constrainStrokeWidth_ / 2;
    auto startPointY = offset.GetY() + constrainStrokeWidth_ / 2;
    if (vertical_) {
        canvas->drawLine(startPointX, startPointY, startPointX, startPointY + dividerLength_, paint);
    } else {
        canvas->drawLine(startPointX, startPointY, startPointX + dividerLength_, startPointY, paint);
    }
}

} // namespace OHOS::Ace