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

#include "core/components/drag_bar/rosen_render_drag_bar.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/common/painter/rosen_universal_painter.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/drag_bar/drag_bar_theme.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t HOVER_COLOR = 0x0c000000;
constexpr Dimension HOVER_RADIUS = 8.0_vp;
constexpr Dimension PAINT_WIDTH = 60.0_vp;
constexpr Dimension PAINT_HEIGHT = 16.0_vp;

} // namespace

void RosenRenderDragBar::Paint(RenderContext& context, const Offset& offset)
{
    if (GetLayoutSize().IsEmpty()) {
        return;
    }
    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }
    if (NearZero(opacity_)) {
        return;
    }

    SkPaint skPaint;
    skPaint.setAntiAlias(true);
    if (mouseState_ == MouseState::HOVER) {
        auto size = Size(NormalizeToPx(PAINT_WIDTH), NormalizeToPx(PAINT_HEIGHT));
        auto alignOffset = Alignment::GetAlignPosition(GetLayoutSize(), size, Alignment::CENTER);
        RosenUniversalPainter::DrawHoverBackground(
            canvas, Rect(offset + alignOffset, size), HOVER_COLOR, NormalizeToPx(HOVER_RADIUS));
    }
    skPaint.setAlpha(alpha_ * opacity_);
    auto theme = GetTheme<DragBarTheme>();
    if (theme) {
        barColor_ = theme->GetBarColor();
    }
    skPaint.setColor(barColor_.GetValue());
    skPaint.setStyle(SkPaint::Style::kStroke_Style);
    skPaint.setStrokeWidth(barWidth_ * scaleY_ * scaleWidth_);
    skPaint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    Offset totalOffset = offset + iconOffset_ + dragOffset_;
    SkPath path;
    path.moveTo(SkDoubleToScalar(barLeftPoint_.GetX() * scaleX_ + totalOffset.GetX()),
        SkDoubleToScalar(barLeftPoint_.GetY() * scaleY_ + totalOffset.GetY()));
    path.lineTo(SkDoubleToScalar(barCenterPoint_.GetX() * scaleX_ + totalOffset.GetX()),
        SkDoubleToScalar(barCenterPoint_.GetY() * scaleY_ + totalOffset.GetY()));
    path.lineTo(SkDoubleToScalar(barRightPoint_.GetX() * scaleX_ + totalOffset.GetX()),
        SkDoubleToScalar(barRightPoint_.GetY() * scaleY_ + totalOffset.GetY()));
    if (!NearEqual(scaleIcon_, 1.0)) {
        canvas->scale(scaleIcon_, scaleIcon_);
        Size translate = GetLayoutSize() * ((scaleIcon_ - 1.0) / 2.0);
        canvas->translate(-translate.Width(), -translate.Height());
    }
    canvas->drawPath(path, skPaint);
}

} // namespace OHOS::Ace
