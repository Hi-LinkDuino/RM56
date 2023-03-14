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

#include "core/components/common/painter/flutter_scroll_bar_painter.h"

#include <cmath>

namespace OHOS::Ace {
namespace {

constexpr double FULL_ALPHA = 255.0;

} // namespace

void FlutterScrollBarPainter::PaintBar(flutter::Canvas* canvas, const Offset& offset, const Rect& paintRect,
    const RefPtr<ScrollBar>& scrollBar, const Offset& globalOffset, int32_t alpha)
{
    if ((canvas != nullptr) && scrollBar && scrollBar->NeedScrollBar()) {
        if (scrollBar->GetShapeMode() == ShapeMode::RECT) {
            PaintRectBar(canvas, offset, scrollBar, alpha);
        } else {
            PaintCircleBar(canvas, globalOffset, paintRect, scrollBar);
        }
    }
}

void FlutterScrollBarPainter::PaintCircleBar(
    flutter::Canvas* canvas, const Offset& offset, const Rect& paintRect, const RefPtr<ScrollBar>& scrollBar)
{
    auto skCanvas = canvas->canvas();
    if (skCanvas && scrollBar->GetTrickSweepAngle() > 0.0) {
        SkPaint paint;
        paint.setBlendMode(SkBlendMode::kSrcOver);
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        paint.setStrokeWidth(scrollBar->GetNormalWidthToPx());
        paint.setStrokeCap(SkPaint::kRound_Cap);

        auto rootSize = scrollBar->GetRootSize();
        // do not draw scrollbar when central is out of scroll viewport
        if (rootSize.Height() * SK_ScalarHalf < offset.GetY() ||
            rootSize.Height() * SK_ScalarHalf > offset.GetY() + paintRect.Height()) {
            return;
        }
        double scrollBarWidth = scrollBar->GetNormalWidthToPx();
        double diameter = rootSize.Width() - scrollBarWidth;
        SkRect arcRect = SkRect::MakeXYWH(scrollBarWidth * SK_ScalarHalf,
                                          scrollBarWidth * SK_ScalarHalf, diameter, diameter);
        // paint background
        double deltaAngle = scrollBar->GetBottomAngle() - scrollBar->GetTopAngle();
        paint.setColor(scrollBar->GetBackgroundColor().GetValue());
        if (scrollBar->GetPositionMode() == PositionMode::LEFT) {
            skCanvas->drawArc(arcRect, deltaAngle * SK_ScalarHalf - STRAIGHT_ANGLE, -deltaAngle, false, paint);
        } else {
            skCanvas->drawArc(arcRect, -deltaAngle * SK_ScalarHalf, deltaAngle, false, paint);
        }

        // paint foreground
        paint.setColor(scrollBar->GetForegroundColor().GetValue());
        skCanvas->drawArc(arcRect, scrollBar->GetTrickStartAngle(), scrollBar->GetTrickSweepAngle(), false, paint);
    }
}

void FlutterScrollBarPainter::PaintRectBar(
    flutter::Canvas* canvas, const Offset& offset, const RefPtr<ScrollBar>& scrollBar, int32_t alpha)
{
    auto skCanvas = canvas->canvas();
    Rect activeRect = scrollBar->GetActiveRect();
    Rect barRect = scrollBar->GetBarRect();
    if (skCanvas && !NearZero(activeRect.Height()) && !NearZero(barRect.Height())) {
        SkPaint paint;
        paint.setBlendMode(SkBlendMode::kSrcOver);
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
        paint.setStrokeCap(SkPaint::kRound_Cap);

        // paint background
        SkRect backgroundRect = SkRect::MakeLTRB(barRect.Left(), barRect.Top(), barRect.Right(), barRect.Bottom());
        paint.setColor(scrollBar->GetBackgroundColor().GetValue());
        double filletRadius = backgroundRect.width() * SK_ScalarHalf;
        skCanvas->drawRoundRect(backgroundRect, filletRadius, filletRadius, paint);

        // paint foreground
        paint.setColor(scrollBar->GetForegroundColor().BlendOpacity(alpha / FULL_ALPHA).GetValue());
        SkRect foregroundRect =
            SkRect::MakeLTRB(activeRect.Left(), activeRect.Top(), activeRect.Right(), activeRect.Bottom());
        skCanvas->drawRoundRect(foregroundRect, filletRadius, filletRadius, paint);
    }
}

} // namespace OHOS::Ace