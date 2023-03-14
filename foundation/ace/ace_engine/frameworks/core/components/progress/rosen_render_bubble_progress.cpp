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

#include "core/components/progress/rosen_render_bubble_progress.h"

#include "third_party/skia/include/core/SkPaint.h"

#include "base/geometry/offset.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {
constexpr Color DARK_COLOR = Color(0xFF333333);
constexpr Color LIGHT_COLOR = Color(0xFF8A8A8A);
} // namespace

void RosenRenderBubbleProgress::Paint(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("canvas is null ptr");
        return;
    }
    SkPaint subCirclePaint;
    subCirclePaint.setAntiAlias(true);
    subCirclePaint.setColor(Color(0xFF8A8A8A).GetValue());

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
    for (auto& center : subCircleCenter_) {
        if (index == lightToDarkIndex) {
            subCirclePaint.setColor(lightToDark_.GetValue());
        } else if (index == darkToLightIndex) {
            subCirclePaint.setColor(darkToLight_.GetValue());
        } else if (index == darkIndex_1 || index == darkIndex_2) {
            subCirclePaint.setColor(DARK_COLOR.GetValue());
        } else {
            subCirclePaint.setColor(LIGHT_COLOR.GetValue());
        }
        canvas->drawCircle(
            (offset + center).GetX(), (offset + center).GetY(), maxCircleRadius_, subCirclePaint);
        index++;
    }
}
} // namespace OHOS::Ace