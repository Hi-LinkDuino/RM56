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

#include "core/components/search/rosen_render_search.h"

#include "third_party/skia/include/core/SkRRect.h"

#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension ICON_HEIGHT = 16.0_vp;
constexpr Dimension SEARCH_SPACING = 2.0_vp;
constexpr double SEARCH_DIVIDER_WIDTH = 1.0;
constexpr Dimension OFFSET_FOR_FOCUS = 4.0_vp;
const Color SEARCH_DIVIDER_COLOR = Color(0x33000000);

} // namespace

void RosenRenderSearch::Paint(RenderContext& context, const Offset& offset)
{
    RenderNode::Paint(context, offset);

    // Paint close icon.
    if (showCloseIcon_ && renderCloseIcon_) {
        renderCloseIcon_->Paint(context, offset + closeIconRect_.GetOffset());
    }

    // Paint divider.
    if (renderSearchBox_) {
        SkCanvas* canvas = GetSkCanvas(context);
        SkPaint paint;
        if (canvas != nullptr) {
            double dividerVerticalOffset = (GetLayoutSize().Height() - NormalizeToPx(ICON_HEIGHT)) / 2.0;
            Offset dividerOffset = Offset(searchTextRect_.GetOffset().GetX(), dividerVerticalOffset);
            if (needReverse_) {
                dividerOffset = searchTextRect_.GetOffset() +
                                Offset(searchTextRect_.Width() - SEARCH_DIVIDER_WIDTH, dividerVerticalOffset);
            }
            dividerOffset += offset;
            Rect rect(dividerOffset, Size(SEARCH_DIVIDER_WIDTH, NormalizeToPx(ICON_HEIGHT)));
            canvas->save();
            paint.setColor(SEARCH_DIVIDER_COLOR.GetValue());
            canvas->drawRect(SkRect::MakeXYWH(rect.Left(), rect.Top(), rect.Width(), rect.Height()), paint);
            canvas->restore();
        }

        // Paint search text.
        searchBoxOffset_ =
            searchTextRect_.GetOffset() + Offset(NormalizeToPx(SEARCH_SPACING) + SEARCH_DIVIDER_WIDTH, 0.0);
        if (needReverse_) {
            searchBoxOffset_ = searchTextRect_.GetOffset() + Offset(NormalizeToPx(SEARCH_SPACING), 0.0);
        }
        auto buttonText = renderSearchBox_->GetFirstChild();
        if (buttonText) {
            auto textSize = buttonText->GetLayoutSize();
            double yOffset = (searchTextRect_.Height() - textSize.Height()) / 2;
            double xOffset = (searchTextRect_.Width() - textSize.Width()) / 2;
            buttonText->Paint(context, offset + searchBoxOffset_ + Offset(xOffset, yOffset));
        }
    }

    PaintFocus();
    PaintOverlayForHoverAndPress(context, offset);
}

void RosenRenderSearch::PaintFocus()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    Size size;
    Offset offset;
    if (focusRender_ == SearchNodeType::NONE) {
        return;
    }
    if (focusRender_ == SearchNodeType::IMAGE) {
        if (renderCloseIcon_) {
            size = closeIconHotZoneRect_.GetSize();
            offset = closeIconHotZoneRect_.GetOffset();
        }
    } else if (focusRender_ == SearchNodeType::BUTTON) {
        if (renderSearchBox_) {
            size = renderSearchBox_->GetLayoutSize();
            offset = searchBoxOffset_;
        }
    }

    if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
        double offsetForFocus = NormalizeToPx(OFFSET_FOR_FOCUS);
        size -= Size(offsetForFocus * 2.0, offsetForFocus * 2.0);
        offset += Offset(offsetForFocus, offsetForFocus);
    }
    RRect rrect = RRect::MakeRect(Rect(offset, size));
    Radius radius = Radius(size.Height() / 2.0);
    rrect.SetCorner({ radius, radius, radius, radius });
    context->ShowFocusAnimation(rrect, Color::BLUE, GetGlobalOffset() + offset);
}

void RosenRenderSearch::PaintOverlayForHoverAndPress(RenderContext& context, const Offset& offset)
{
    if (overlayColor_ == Color::TRANSPARENT) {
        return;
    }

    SkCanvas* canvas = GetSkCanvas(context);
    if (canvas == nullptr) {
        LOGE("canvas is null!");
        return;
    }
    canvas->save();
    SkPaint paint;
    // Background overlay 10% opacity black when hover.
    paint.setColor(overlayColor_.GetValue());

    Offset rectOffset;
    Size rectSize;
    double searchSpacing = NormalizeToPx(SEARCH_SPACING);
    if (showCloseIcon_ && hoverOrPressRender_ == SearchNodeType::IMAGE) {
        rectOffset = closeIconHotZoneRect_.GetOffset();
        rectSize = closeIconHotZoneRect_.GetSize();
    } else if (hoverOrPressRender_ == SearchNodeType::BUTTON) {
        rectOffset = searchTextRect_.GetOffset() + Offset(SEARCH_DIVIDER_WIDTH, 0.0) + Offset(searchSpacing, 0.0);
        rectSize = searchTextRect_.GetSize() - Size(SEARCH_DIVIDER_WIDTH, 0.0) - Size(searchSpacing * 2.0, 0.0);
    }

    Border border;
    border.SetBorderRadius(Radius(rectSize.Height() / 2.0));
    canvas->drawRRect(MakeRRect(rectOffset + offset, rectSize, border), paint);
    canvas->restore();
}

SkRRect RosenRenderSearch::MakeRRect(const Offset& offset, const Size& size, const Border& border)
{
    SkRect rect = SkRect::MakeXYWH(offset.GetX(), offset.GetY(), size.Width(), size.Height());
    SkRRect rrect = SkRRect::MakeEmpty();
    SkVector rectRadii[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    rectRadii[SkRRect::kUpperLeft_Corner] =
        SkPoint::Make(NormalizeToPx(border.TopLeftRadius().GetX()), NormalizeToPx(border.TopLeftRadius().GetY()));
    rectRadii[SkRRect::kUpperRight_Corner] =
        SkPoint::Make(NormalizeToPx(border.TopRightRadius().GetX()), NormalizeToPx(border.TopRightRadius().GetY()));
    rectRadii[SkRRect::kLowerRight_Corner] = SkPoint::Make(
        NormalizeToPx(border.BottomRightRadius().GetX()), NormalizeToPx(border.BottomRightRadius().GetY()));
    rectRadii[SkRRect::kLowerLeft_Corner] =
        SkPoint::Make(NormalizeToPx(border.BottomLeftRadius().GetX()), NormalizeToPx(border.BottomLeftRadius().GetY()));
    rrect.setRectRadii(rect, rectRadii);
    return rrect;
}

SkCanvas* RosenRenderSearch::GetSkCanvas(RenderContext& context)
{
    auto renderContext = AceType::DynamicCast<RosenRenderContext>(&context);
    if (!renderContext) {
        LOGE("render context is null");
        return nullptr;
    }
    auto canvas = renderContext->GetCanvas();
    if (!canvas) {
        return nullptr;
    }
    return canvas;
}

} // namespace OHOS::Ace