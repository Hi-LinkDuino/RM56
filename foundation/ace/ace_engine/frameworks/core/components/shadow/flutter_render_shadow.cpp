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

#include "core/components/shadow/flutter_render_shadow.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkMaskFilter.h"

#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {
namespace {

constexpr uint8_t SHADOW_COLOR_ALPHA = 77;
constexpr float SHADOW_BLUR_RADIUS = 20.0f;
constexpr Dimension SHADOW_OFFSET = 10.0_vp;

} // namespace

using namespace Flutter;

RenderLayer FlutterRenderShadow::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<OffsetLayer>(0.0, 0.0);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderShadow::Paint(RenderContext& context, const Offset& offset)
{
    if (!isDisplay_) {
        return;
    }

    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("canvas fetch failed");
        return;
    }
    SkCanvas* skCanvas = canvas->canvas();
    if (skCanvas == nullptr) {
        LOGE("skCanvas fetch failed");
        return;
    }

    if (isNeedClip_) {
        skCanvas->clipRect(SkRect::MakeXYWH(clipRect_.GetOffset().GetX() - NormalizeToPx(SHADOW_OFFSET) / 2,
            clipRect_.GetOffset().GetY(), clipRect_.Width() + NormalizeToPx(SHADOW_OFFSET),
            clipRect_.Height() + NormalizeToPx(SHADOW_OFFSET)), true);
    }

    double radiusX = NormalizeToPx(rrect_.GetCorner().bottomLeftRadius.GetX());
    double radiusY = NormalizeToPx(rrect_.GetCorner().bottomLeftRadius.GetY());

    SkPaint paint;
    paint.setColor(SkColorSetARGB(SHADOW_COLOR_ALPHA, 0, 0, 0));
    paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    SkRect rect = SkRect::MakeXYWH(offset_.GetX(), offset_.GetY(), rrect_.Width(), rrect_.Height());
    skCanvas->clipRRect(SkRRect::MakeRectXY(rect, radiusX, radiusY), SkClipOp::kDifference, true);
    rect = SkRect::MakeXYWH(
        offset_.GetX(), offset_.GetY() + NormalizeToPx(SHADOW_OFFSET), rrect_.Width(), rrect_.Height());
    skCanvas->drawRRect(SkRRect::MakeRectXY(rect, radiusX, radiusY), paint);
}

} // namespace OHOS::Ace
