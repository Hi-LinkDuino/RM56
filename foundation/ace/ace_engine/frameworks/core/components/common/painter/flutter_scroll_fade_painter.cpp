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

#include "core/components/common/painter/flutter_scroll_fade_painter.h"

#include "flutter/lib/ui/text/paragraph_builder.h"

#include "base/geometry/rect.h"
#include "base/log/log.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr double WIDTH_TO_HEIGHT_FACTOR = 0.20096; // (3.0 / 4.0) * (2.0 - std::sqrt(3));
constexpr double PI_OVER_2 = 1.5707963; // 3.1415926 / 2.0;

} // namespace

void FlutterScrollFadePainter::Paint(flutter::Canvas* canvas, const Size& size, const Offset& offset)
{
    if (canvas == nullptr || NearZero(opacity_) || size.IsEmpty()) {
        return;
    }

    LOGD("ScrollFadePainter: opacity_(%{public}lf), scaleFactor(%{public}lf)", opacity_, scaleFactor_);
    double baseGlowScale = size.Width() > size.Height() ? size.Height() / size.Width() : 1.0;
    double radius = size.Width() * 3.0 / 2.0;
    double height = std::min(size.Height(), size.Width() * WIDTH_TO_HEIGHT_FACTOR);
    double scaleH = scaleFactor_ * baseGlowScale;
    const auto& clipRect = Rect(Offset::Zero(), Size(size.Width(), height));
    Offset center = Offset(size.Width() / 2.0, height - radius);
    LOGD("center(%{public}lf, %{public}lf), radius(%{public}lf)", center.GetX(), center.GetY(), radius);

    flutter::Paint painter;
    flutter::PaintData paintData;
    painter.paint()->setColor(color_.GetValue());
    painter.paint()->setAlphaf(opacity_);
    painter.paint()->setBlendMode(SkBlendMode::kSrcOver);

    canvas->save();
    canvas->scale(1.0, scaleH);
    canvas->clipRect(clipRect.Left(), clipRect.Top(), clipRect.Right(), clipRect.Bottom(), SkClipOp::kIntersect);
    canvas->drawCircle(center.GetX(), center.GetY(), radius, painter, paintData);
    canvas->restore();
}

void FlutterScrollFadePainter::PaintSide(RenderContext& context, const Size& size, const Offset& offset)
{
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    if (!renderContext) {
        return;
    }
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        return;
    }
    switch (direction_) {
        case OverScrollDirection::UP:
            canvas->save();
            canvas->translate(offset.GetX(), offset.GetY());
            Paint(canvas, size, offset);
            canvas->restore();
            break;
        case OverScrollDirection::DOWN:
            canvas->save();
            canvas->translate(offset.GetX(), offset.GetY());
            canvas->translate(0.0, size.Height());
            canvas->scale(1.0, -1.0);
            Paint(canvas, size, offset);
            canvas->restore();
            break;
        case OverScrollDirection::LEFT:
            canvas->save();
            canvas->translate(offset.GetX(), offset.GetY());
            canvas->rotate(PI_OVER_2);
            canvas->scale(1.0, -1.0);
            Paint(canvas, Size(size.Height(), size.Width()), offset);
            canvas->restore();
            break;
        case OverScrollDirection::RIGHT:
            canvas->save();
            canvas->translate(offset.GetX(), offset.GetY());
            canvas->translate(size.Width(), 0.0);
            canvas->rotate(PI_OVER_2);
            Paint(canvas, Size(size.Height(), size.Width()), offset);
            canvas->restore();
            break;
        default:
            break;
    }
}

} // namespace OHOS::Ace