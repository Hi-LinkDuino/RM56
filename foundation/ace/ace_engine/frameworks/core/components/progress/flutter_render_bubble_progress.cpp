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

#include "core/components/progress/flutter_render_bubble_progress.h"

#include "flutter/lib/ui/ui_dart_state.h"
#include "third_party/skia/include/core/SkMaskFilter.h"

#include "core/pipeline/base/scoped_canvas_state.h"
#include "core/pipeline/layers/offset_layer.h"

namespace OHOS::Ace {
namespace {

constexpr Color DARK_COLOR = Color(0xFF333333);
constexpr Color LIGHT_COLOR = Color(0xFF8A8A8A);

} // namespace

void FlutterRenderBubbleProgress::Paint(RenderContext& context, const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("canvas is null ptr");
        return;
    }
    flutter::Paint subCirclePaint;

    subCirclePaint.paint()->setAntiAlias(true);
    subCirclePaint.paint()->setColor(Color(0xFF8A8A8A).GetValue());

    int32_t index = 0;
    static const int32_t threshold = 7;
    static const int32_t maxBubbleCount = 8;
    int32_t lightToDarkIndex = 7 - step_;
    int32_t darkIndex_1 =
        lightToDarkIndex + 1 > threshold ? (lightToDarkIndex + 1) % maxBubbleCount : lightToDarkIndex + 1;
    int32_t darkIndex_2 =
        lightToDarkIndex + 2 > threshold ? (lightToDarkIndex + 2) % maxBubbleCount : lightToDarkIndex + 2;
    int32_t darkToLightIndex =
        lightToDarkIndex + 3 > threshold ? (lightToDarkIndex + 3) % maxBubbleCount : lightToDarkIndex + 3;
    flutter::PaintData subCirclePaintData;
    for (auto& center : subCircleCenter_) {
        if (index == lightToDarkIndex) {
            subCirclePaint.paint()->setColor(lightToDark_.GetValue());
        } else if (index == darkToLightIndex) {
            subCirclePaint.paint()->setColor(darkToLight_.GetValue());
        } else if (index == darkIndex_1 || index == darkIndex_2) {
            subCirclePaint.paint()->setColor(DARK_COLOR.GetValue());
        } else {
            subCirclePaint.paint()->setColor(LIGHT_COLOR.GetValue());
        }
        canvas->drawCircle(
            (offset + center).GetX(), (offset + center).GetY(), maxCircleRadius_, subCirclePaint, subCirclePaintData);
        index++;
    }
}

RenderLayer FlutterRenderBubbleProgress::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::OffsetLayer>();
    }
    return AceType::RawPtr(layer_);
}

} // namespace OHOS::Ace