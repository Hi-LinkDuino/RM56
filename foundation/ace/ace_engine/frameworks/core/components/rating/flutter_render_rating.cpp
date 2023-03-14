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

#include "core/components/rating/flutter_render_rating.h"

#include "flutter/common/task_runners.h"
#include "third_party/skia/include/core/SkClipOp.h"
#include "third_party/skia/include/core/SkMaskFilter.h"

#include "core/components/align/render_align.h"
#include "core/components/common/painter/flutter_universal_painter.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

void FlutterRenderRating::Paint(RenderContext& context, const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    double offsetDeltaX = (GetLayoutSize().Width() - ratingSize_.Width()) / 2.0;
    double offsetDeltaY = (GetLayoutSize().Height() - ratingSize_.Height()) / 2.0;
    offsetDelta_ = offset + Offset(offsetDeltaX, offsetDeltaY);
    double imageVerticalOffset = ratingSize_.Width() * paddingVertical_.Value() / defaultHeight_.Value() / starNum_;
    Offset starOffset = GetStarOffset(imageVerticalOffset);
    if (!isIndicator_ && operationEvent_ == OperationEvent::RATING_KEY_EVENT && !NearZero(imageVerticalOffset)) {
        Offset animationOffset = starOffset + Offset(offsetDeltaX, offsetDeltaY);
        PaintFocus(animationOffset + offset, singleWidth_ / 2 - imageVerticalOffset,
            Size(singleWidth_ - imageVerticalOffset * 2, singleWidth_ - imageVerticalOffset * 2), context);
        double focusRadius = singleWidth_ - imageVerticalOffset * 2;
        RequestFocusAnimation(animationOffset + GetGlobalOffset(), Size(focusRadius, focusRadius), focusRadius);
    }
    if (IsPhone() && !isIndicator_) {
        PaintHoverRect(canvas);
    }
    PaintRatingBar(context, canvas);
    if (IsPhone() && !isIndicator_ && operationEvent_ == OperationEvent::RATING_KEY_EVENT &&
        operationEvent_ != OperationEvent::RATING_MOUSE_EVENT) {
        // total width of focus border is twice the width of [focusBorderWidth_], border-width + padding-width
        RequestFocusAnimationForPhone();
    }
}

void FlutterRenderRating::PaintHoverRect(const ScopedCanvas& canvas)
{
    Size hoverSize = Size(singleWidth_, ratingSize_.Height());
    for (auto& item : hoverColorMap_) {
        if (item.second == Color::TRANSPARENT) {
            continue;
        }
        FlutterUniversalPainter::DrawHoverBackground(canvas->canvas(),
            Rect(Offset(item.first * singleWidth_, 0.0) + offsetDelta_, hoverSize),
            item.second.GetValue(), focusBorderRadius_.Value());
    }
}

void FlutterRenderRating::PaintRatingBar(RenderContext& context, const ScopedCanvas& canvas)
{
    double imageVerticalOffset = ratingSize_.Width() * paddingVertical_.Value() / defaultHeight_.Value() / starNum_;
    auto imageOffset = offsetDelta_ + Offset(0.0, imageVerticalOffset);
    if (paintForeground_) {
        // In RTL environment, foreground image may draw three stars and be clipped to display 2.5 stars eventually.
        // [foregroundDrawOffset] is the position to start drawing foreground image.
        // [foregroundClipOffset] is the position to start clipping foreground image zone.
        Offset foregroundDrawOffset = imageOffset;
        Offset foregroundClipOffset = imageOffset;
        Size foregroundSize = Size(foregroundImageWidth_, ratingSize_.Height());
        if (needReverse_) {
            foregroundDrawOffset += Offset((starNum_ - ceil(drawScore_)) * singleWidth_, 0.0);
            foregroundClipOffset += Offset(secondaryImageWidth_ + backgroundImageWidth_, 0.0);
        }

        auto foregroundProperties =
            ImageAreaProperties(ceil(drawScore_), foregroundSize, foregroundDrawOffset, foregroundClipOffset);
        PaintImageArea(context, renderForeground_, canvas, foregroundProperties);
    }

    if (paintSecondary_) {
        Offset secondaryClipOffset = imageOffset;
        Offset secondaryDrawOffset = imageOffset;
        if (needReverse_) {
            secondaryClipOffset += Offset((starNum_ - ceil(drawScore_)) * singleWidth_, 0.0);
            secondaryDrawOffset = secondaryClipOffset;
        } else {
            secondaryClipOffset += Offset(foregroundImageWidth_, 0.0);
            secondaryDrawOffset += Offset(floor(drawScore_) * singleWidth_, 0.0);
        }
        Size secondarySize = Size(secondaryImageWidth_, ratingSize_.Height());

        auto secondaryProperties = ImageAreaProperties(1, secondarySize, secondaryDrawOffset, secondaryClipOffset);
        PaintImageArea(context, renderSecondary_, canvas, secondaryProperties);
    }

    if (paintBackground_) {
        Offset backgroundDrawOffset =
            needReverse_ ? imageOffset : imageOffset + Offset(foregroundImageWidth_ + secondaryImageWidth_, 0.0);
        Offset backgroundClipOffset = backgroundDrawOffset;
        Size backgroundSize = Size(backgroundImageWidth_, ratingSize_.Height());

        auto backgroundProperties = ImageAreaProperties(
            starNum_ - floor(drawScore_), backgroundSize, backgroundDrawOffset, backgroundClipOffset);
        PaintImageArea(context, renderBackground_, canvas, backgroundProperties);
    }
}

void FlutterRenderRating::PaintImageArea(RenderContext& context, const RefPtr<RenderImage>& renderImage,
    const ScopedCanvas& canvas, const ImageAreaProperties& properties)
{
    canvas->save();
    auto clipOffset = properties.clipOffset;
    auto drawOffset = properties.drawOffset;
    auto imageAreaSize = properties.imageAreaSize;
    canvas->clipRect(clipOffset.GetX(), clipOffset.GetY(), clipOffset.GetX() + imageAreaSize.Width(),
        clipOffset.GetY() + imageAreaSize.Height(), SkClipOp::kIntersect, true);
    for (int i = 0; i < properties.repeatNum; ++i) {
        renderImage->RenderWithContext(context, drawOffset + Offset(singleWidth_ * i, 0.0));
    }
    canvas->restore();
}

void FlutterRenderRating::PaintFocus(
    const Offset& offset, double rRectRadius, const Size& boardSize, RenderContext& context)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    flutter::Paint paint;
    flutter::PaintData paintData;
    flutter::RRect rRect;
    paint.paint()->setColor(RATING_FOCUS_BOARD_COLOR);
    paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    paint.paint()->setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kSolid_SkBlurStyle, 1.0));
    rRect.sk_rrect.setRectXY(SkRect::MakeIWH(boardSize.Width(), boardSize.Height()), rRectRadius, rRectRadius);
    rRect.sk_rrect.offset(offset.GetX(), offset.GetY());
    canvas->drawRRect(rRect, paint, paintData);
}

} // namespace OHOS::Ace