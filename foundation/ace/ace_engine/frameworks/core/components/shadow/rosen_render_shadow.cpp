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

#include "core/components/shadow/rosen_render_shadow.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkMaskFilter.h"
#include "third_party/skia/include/core/SkRRect.h"

#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr uint8_t SHADOW_COLOR_ALPHA = 77;
constexpr float SHADOW_BLUR_RADIUS = 20.0f;
constexpr Dimension SHADOW_OFFSET = 10.0_vp;

} // namespace

void RosenRenderShadow::Paint(RenderContext& context, const Offset& offset)
{
    if (!isDisplay_) {
        return;
    }

    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("canvas fetch failed");
        return;
    }

    if (isNeedClip_) {
        canvas->clipRect(SkRect::MakeXYWH(clipRect_.GetOffset().GetX() - NormalizeToPx(SHADOW_OFFSET) / 2,
                             clipRect_.GetOffset().GetY(), clipRect_.Width() + NormalizeToPx(SHADOW_OFFSET),
                             clipRect_.Height() + NormalizeToPx(SHADOW_OFFSET)),
            true);
    }

    double radiusX = NormalizeToPx(rrect_.GetCorner().bottomLeftRadius.GetX());
    double radiusY = NormalizeToPx(rrect_.GetCorner().bottomLeftRadius.GetY());

    SkPaint paint;
    paint.setColor(SkColorSetARGB(SHADOW_COLOR_ALPHA, 0, 0, 0));
    paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    SkRect rect = SkRect::MakeXYWH(offset_.GetX(), offset_.GetY(), rrect_.Width(), rrect_.Height());
    canvas->clipRRect(SkRRect::MakeRectXY(rect, radiusX, radiusY), SkClipOp::kDifference, true);
    rect = SkRect::MakeXYWH(
        offset_.GetX(), offset_.GetY() + NormalizeToPx(SHADOW_OFFSET), rrect_.Width(), rrect_.Height());
    canvas->drawRRect(SkRRect::MakeRectXY(rect, radiusX, radiusY), paint);
}

} // namespace OHOS::Ace
