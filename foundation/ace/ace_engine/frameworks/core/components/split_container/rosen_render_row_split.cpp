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

#include "core/components/split_container/rosen_render_row_split.h"

#include "include/core/SkPaint.h"

#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

namespace {

constexpr uint32_t ROW_SPLIT_COLOR = 0xFF707B7C;

} // namespace

void RosenRenderRowSplit::Paint(RenderContext& context, const Offset& offset)
{
    auto children = GetChildren();
    double dividerLength = GetPaintRect().Height();
    int32_t index = 0;
    for (const auto& item : children) {
        if (!item->GetVisible()) {
            continue;
        }
        context.PaintChild(item, offset);
        if (index != 0) {
            PaintDivider(context, offset + item->GetPosition(), dividerLength);
        }
        index++;
    }
}

void RosenRenderRowSplit::PaintDivider(RenderContext& context, const Offset& offset, double height)
{
    double startPointX = offset.GetX();
    double startPointY = offset.GetY();
    double endPointX = startPointX;
    double endPointY = startPointY + height;
    SkPaint paint;
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    paint.setColor(ROW_SPLIT_COLOR);
    paint.setStrokeWidth(DEFAULT_SPLIT_HEIGHT);
    canvas->drawLine(startPointX, startPointY, endPointX, endPointY, paint);
}

} // namespace OHOS::Ace