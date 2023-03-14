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

#include "core/components/option/flutter_render_option.h"

#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {
namespace {

const Dimension ROUND_RADIUS_PHONE = 12.0_vp;
const Dimension ROUND_RADIUS_TV = 8.0_vp;

} // namespace

void FlutterRenderOption::Paint(RenderContext& context, const Offset& offset)
{
    PaintBackground(context, offset);
    PaintLine(context, offset);
    RenderNode::Paint(context, offset);
}

void FlutterRenderOption::PaintBackground(RenderContext& context, const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);

    auto size = GetLayoutSize();
    double radius = NormalizeToPx((isTv_ ? ROUND_RADIUS_TV : ROUND_RADIUS_PHONE));
    double diff = 0.0;
    if (data_ && data_->GetTheme()) {
        auto theme = data_->GetTheme();
        diff = NormalizeToPx(theme->GetOptionInterval());
    }


    fml::RefPtr<flutter::CanvasPath> path = flutter::CanvasPath::Create();
    auto tempRect = SkRect::MakeXYWH(offset.GetX() + diff, offset.GetY() + diff,
        size.Width() - 2.0 * diff, size.Height() - 2.0 * diff);
    auto skRRect = SkRRect::MakeRectXY(tempRect, radius, radius);
    flutter::RRect rrect;
    rrect.sk_rrect = skRRect;
    rrect.is_null = false;
    path->addRRect(rrect);

    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setARGB(backColor_.GetAlpha(),
        backColor_.GetRed(), backColor_.GetGreen(), backColor_.GetBlue());
    canvas->drawPath(path.get(), paint, paintData);
    paint.paint()->setColor(GetEventEffectColor().GetValue());
    canvas->drawPath(path.get(), paint, paintData);
}

void FlutterRenderOption::PaintLine(RenderContext& context, const Offset& offset)
{
    if (isTv_) {
        return;
    }

    if (!needLine_) {
        return;
    }

    auto text = GetRenderText(AceType::Claim(this));
    if (!text) {
        return;
    }

    auto thisOffset = GetGlobalOffset();
    auto textOffset = text->GetGlobalOffset();
    double left = textOffset.GetX() - thisOffset.GetX();
    double right = left + text->GetLayoutSize().Width();
    double top = 0.0;
    double bottom = 1.0; // line width is 1.0px

    left += offset.GetX();
    right += offset.GetX();
    top += offset.GetY();
    bottom += offset.GetY();

    auto canvas = ScopedCanvas::Create(context);

    fml::RefPtr<flutter::CanvasPath> path = flutter::CanvasPath::Create();
    path->addRect(static_cast<float>(left), static_cast<float>(top),
        static_cast<float>(right), static_cast<float>(bottom));

    flutter::Paint paint;
    paint.paint()->setARGB(lineColor_.GetAlpha(), lineColor_.GetRed(), lineColor_.GetGreen(), lineColor_.GetBlue());
    paint.paint()->setAntiAlias(true);
    flutter::PaintData paintData;
    canvas->drawPath(path.get(), paint, paintData);

}

} // namespace OHOS::Ace
