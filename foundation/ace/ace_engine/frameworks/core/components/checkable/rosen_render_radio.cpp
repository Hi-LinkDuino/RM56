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

#include "core/components/checkable/rosen_render_radio.h"

#include "core/components/checkable/checkable_component.h"
#include "core/components/common/painter/rosen_universal_painter.h"
#include "core/pipeline/base/rosen_render_context.h"
#include "render_service_client/core/ui/rs_node.h"

namespace OHOS::Ace {

void RosenRenderRadio::Paint(RenderContext& context, const Offset& offset)
{
    auto paintOffset = offset + paintPosition_;
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
#ifdef OHOS_PLATFORM
    auto recordingCanvas = static_cast<Rosen::RSRecordingCanvas*>(canvas);
    recordingCanvas->MultiplyAlpha(ConfigureOpacity(disabled_));
#endif
    SkPaint paint;
    paint.setAntiAlias(true);
    if (IsPhone() && onFocus_) {
        RequestFocusBorder(paintOffset, drawSize_, drawSize_.Width() / 2.0);
    }
    double centerX = outCircleRadius_ + paintOffset.GetX();
    double centerY = outCircleRadius_ + paintOffset.GetY();
    auto borderWidth = NormalizeToPx(borderWidth_);
    switch (uiStatus_) {
        case UIStatus::SELECTED:
            // draw stroke border
            paint.setAntiAlias(true);
            paint.setColor(activeColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ * totalScale_, paint);

            // draw shadow
            if (!NearZero(pointScale_) && !NearEqual(pointScale_, 1.0)) {
                paint.setColor(shadowColor_);
                canvas->drawCircle(
                    centerX, centerY, outCircleRadius_ * pointScale_ + NormalizeToPx(shadowWidth_), paint);
            }

            // draw inner circle
            paint.setColor(pointColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ * pointScale_, paint);
            break;
        case UIStatus::UNSELECTED:
            // draw inner circle
            paint.setColor(inactivePointColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ - borderWidth / 2.0, paint);

            // draw border with unselected color
            paint.setColor(inactiveColor_);
            SetStrokeWidth(borderWidth, paint);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ - borderWidth / 2.0, paint);
            break;
        case UIStatus::FOCUS: // focus of unselected
            // draw inner circle
            paint.setColor(inactivePointColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ - borderWidth / 2.0, paint);

            // draw border with focus color
            SetStrokeWidth(borderWidth, paint);
            paint.setColor(focusColor_);
            canvas->drawCircle(centerX, centerY, outCircleRadius_ - borderWidth / 2.0, paint);
            break;
        default:
            LOGE("unknown ui status");
            break;
    }
}

} // namespace OHOS::Ace
