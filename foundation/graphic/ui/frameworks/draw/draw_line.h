/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_DRAW_LINE_H
#define GRAPHIC_LITE_DRAW_LINE_H

#include "gfx_utils/color.h"
#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_buffer.h"

namespace OHOS {
class DrawLine : public HeapBase {
public:
    static void Draw(BufferInfo& gfxDstBuffer, const Point& start, const Point& end, const Rect& mask,
        int16_t width, const ColorType& color, OpacityType opacity);

private:
    static void DrawVerticalLine(BufferInfo& gfxDstBuffer, const Point& start, const Point& end,
        const Rect& mask, int16_t width, const ColorType& color, OpacityType opacity);

    static void DrawHorizontalLine(BufferInfo& gfxDstBuffer, const Point& start, const Point& end,
        const Rect& mask, int16_t width, const ColorType& color, OpacityType opacity);

    static void DrawWuLine(BufferInfo& gfxDstBuffer, const Point& start, const Point& end,
        const Rect& mask, int16_t width, const ColorType& color, OpacityType opacity);

    static void DrawThinWuLine(BufferInfo& gfxDstBuffer, const Point& start, const Point& end, const Rect& mask,
        int16_t width, const ColorType& color, OpacityType opacity);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_DRAW_LINE_H
