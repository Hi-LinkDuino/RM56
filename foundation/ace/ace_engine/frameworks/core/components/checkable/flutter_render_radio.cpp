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

#include "core/components/checkable/flutter_render_radio.h"

#include "core/components/checkable/checkable_component.h"
#include "core/components/common/painter/flutter_universal_painter.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

void FlutterRenderRadio::Paint(RenderContext& context, const Offset& offset)
{
    auto paintOffset = offset + paintPosition_;
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    flutter::Paint paint;
    paint.paint()->setAntiAlias(true);
    auto* skPaint = paint.paint();
    if (skPaint == nullptr) {
        LOGE("the sk paint is nullptr");
        return;
    }
    if (IsPhone() && onFocus_) {
        RequestFocusBorder(paintOffset, drawSize_, drawSize_.Width() / 2.0);
    }
    flutter::PaintData paintData;
    double centerX = outCircleRadius_ + paintOffset.GetX();
    double centerY = outCircleRadius_ + paintOffset.GetY();
    auto borderWidth = NormalizeToPx(borderWidth_);
    switch (uiStatus_) {
        case UIStatus::SELECTED:
            // draw stroke border
            skPaint->setAntiAlias(true);
            skPaint->setColor(activeColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ * totalScale_, paint, paintData);

            // draw shadow
            if (!NearZero(pointScale_) && !NearEqual(pointScale_, 1.0)) {
                skPaint->setColor(shadowColor_);
                canvas->drawCircle(
                    centerX, centerY, outCircleRadius_ * pointScale_ + NormalizeToPx(shadowWidth_), paint, paintData);
            }

            // draw inner circle
            skPaint->setColor(pointColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ * pointScale_, paint, paintData);
            break;
        case UIStatus::UNSELECTED:
            // draw inner circle
            skPaint->setColor(inactivePointColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ - borderWidth / 2.0, paint, paintData);

            // draw border with unselected color
            skPaint->setColor(inactiveColor_);
            SetStrokeWidth(borderWidth, paint);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ - borderWidth / 2.0, paint, paintData);
            break;
        case UIStatus::FOCUS: // focus of unselected
            // draw inner circle
            skPaint->setColor(inactivePointColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ - borderWidth / 2.0, paint, paintData);

            // draw border with focus color
            SetStrokeWidth(borderWidth, paint);
            skPaint->setColor(focusColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ - borderWidth / 2.0, paint, paintData);
            break;
        default:
            LOGE("unknown ui status");
            break;
    }
}

} // namespace OHOS::Ace
