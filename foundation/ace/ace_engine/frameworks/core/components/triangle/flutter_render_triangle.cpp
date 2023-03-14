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

#include "core/components/triangle/flutter_render_triangle.h"

#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

void FlutterRenderTriangle::Paint(RenderContext& context, const Offset& offset)
{
    data_.SetRadius(NormalizeToPx(1.0_vp)); // round radius of triangle is 1dp
    if (!data_.Calculate(offset.GetX(), offset.GetY())) {
        LOGE("calculate triangle points failed!");
        return;
    }
    auto canvas = ScopedCanvas::Create(context);
    fml::RefPtr<flutter::CanvasPath> path = flutter::CanvasPath::Create();
    const auto& arc1 = data_.GetOutArc1();
    const auto& arc2 = data_.GetOutArc2();
    const auto& arc3 = data_.GetOutArc3();
    path->moveTo(arc1.GetStartPoint().GetX(), arc1.GetStartPoint().GetY());
    path->arcTo(arc1.GetLeft(), arc1.GetTop(), arc1.GetRight(), arc1.GetBottom(),
        arc1.GetStartAngle(), arc1.GetSweepAngle(), false);
    path->lineTo(arc2.GetStartPoint().GetX(), arc2.GetStartPoint().GetY());
    path->arcTo(arc2.GetLeft(), arc2.GetTop(), arc2.GetRight(), arc2.GetBottom(),
        arc2.GetStartAngle(), arc2.GetSweepAngle(), false);
    path->lineTo(arc3.GetStartPoint().GetX(), arc3.GetStartPoint().GetY());
    path->arcTo(arc3.GetLeft(), arc3.GetTop(), arc3.GetRight(), arc3.GetBottom(),
        arc3.GetStartAngle(), arc3.GetSweepAngle(), false);
    path->close();

    flutter::Paint paint;
    paint.paint()->setARGB(color_.GetAlpha(), color_.GetRed(), color_.GetGreen(), color_.GetBlue());
    paint.paint()->setAntiAlias(true);
    flutter::PaintData paintData;
    canvas->drawPath(path.get(), paint, paintData);
}

} // namespace OHOS::Ace
