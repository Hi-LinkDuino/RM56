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

#include "core/components/slider/flutter_render_circle_block.h"

#include "flutter/lib/ui/ui_dart_state.h"
#include "third_party/skia/include/core/SkMaskFilter.h"

#include "base/utils/system_properties.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {
namespace {

constexpr double BORDER_WEIGHT = 0.33;
constexpr double HALF = 0.5;
constexpr Dimension HOVER_RADIUS = 16.0_vp;

} // namespace

void FlutterRenderCircleBlock::Update(const RefPtr<Component>& component)
{
    RenderBlock::Update(component);
}

void FlutterRenderCircleBlock::Paint(RenderContext& context, const Offset& offset)
{
    LOGD("Slider::Paint_offset position x:%{public}lf, y:%{public}lf radiusScale:%{public}lf", offset.GetX(),
        offset.GetY(), radiusScale_);
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("canvas fetch failed");
        return;
    }
    SkCanvas* skCanvas = canvas->canvas();
    if (!skCanvas) {
        LOGE("skCanvas fetch failed");
        return;
    }

    if (isHover_) {
        SkPaint hoverPaint;
        hoverPaint.setColor(GetHoverColor().GetValue());
        double hoverRadius = NormalizeToPx(HOVER_RADIUS);
        skCanvas->drawCircle(offset.GetX(), offset.GetY(), hoverRadius, hoverPaint);
    }

    double radius = NormalizeToPx(blockSize_) * HALF * radiusScale_;
    if (GetFocus() && SystemProperties::GetDeviceType() == DeviceType::TV) {
        SkPaint blockPaint;
        // the color value, when the slider get focus
        static const uint8_t lightBlueA = 255;
        static const uint8_t lightBlueR = 70;
        static const uint8_t lightBlueG = 150;
        static const uint8_t lightBlueB = 236;
        blockPaint.setColor(SkColorSetARGB(lightBlueA, lightBlueR, lightBlueG, lightBlueB));
        skCanvas->drawCircle(offset.GetX(), offset.GetY(), radius, blockPaint);
    } else {
        SkPaint blockPaint;
        blockPaint.setColor(SkColorSetARGB(GetBlockColor().GetAlpha(), GetBlockColor().GetRed(),
            GetBlockColor().GetGreen(), GetBlockColor().GetBlue()));
        skCanvas->drawCircle(offset.GetX(), offset.GetY(), radius, blockPaint);
    }

    // Draw block border
    SkPaint borderPaint;
    // use this color to reduce the loss at corner.
    static const uint8_t alpha = 13;
    borderPaint.setColor(SkColorSetARGB(alpha, 0, 0, 0));
    borderPaint.setStyle(SkPaint::Style::kStroke_Style);
    borderPaint.setAntiAlias(true);
    borderPaint.setStrokeWidth(BORDER_WEIGHT);
    skCanvas->drawCircle(offset.GetX(), offset.GetY(), radius, borderPaint);

    PaintShadow(skCanvas, offset, radius);
}

void FlutterRenderCircleBlock::PaintShadow(SkCanvas* skCanvas, const Offset& offset, double radius)
{
    if (!skCanvas) {
        return;
    }
    double diameter = radius * 2.0;
    // Draw block border shadow
    SkPaint borderShadowPaint;
    // use this color to reduce the loss at corner.
    static const uint8_t shadowAlpha = 30;
    borderShadowPaint.setColor(SkColorSetARGB(shadowAlpha, 0, 0, 0));
    borderShadowPaint.setStyle(SkPaint::Style::kFill_Style);
    borderShadowPaint.setAntiAlias(true);
    borderShadowPaint.setStrokeWidth(BORDER_WEIGHT);
    SkRect rect = SkRect::MakeXYWH(offset.GetX() - radius, offset.GetY() - radius, diameter, diameter);
    skCanvas->clipRRect(SkRRect::MakeRectXY(rect, radius, radius), SkClipOp::kDifference, true);
    // Draw first circle for shadow.
    skCanvas->drawCircle(offset.GetX(), offset.GetY() + BORDER_WEIGHT, radius, borderShadowPaint);
    // Draw second circle for shadow.
    skCanvas->drawCircle(offset.GetX(), offset.GetY() + BORDER_WEIGHT * 2.0, radius, borderShadowPaint);
}

} // namespace OHOS::Ace