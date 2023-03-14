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

#ifndef GRAPHIC_LITE_DRAW_RECT_H
#define GRAPHIC_LITE_DRAW_RECT_H

#include "gfx_utils/heap_base.h"
#include "gfx_utils/rect.h"
#include "gfx_utils/style.h"
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_buffer.h"

namespace OHOS {
/** @brief Class for drawing rectangle. */
class DrawRect : public HeapBase {
public:
    DrawRect() {}
    ~DrawRect() {}

    static void Draw(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                     const Style& style, OpacityType opaScale);

private:
    static void DrawRectRadiusWithoutBorder(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                            const Style& style, OpacityType opaScale);

    static void DrawRectBorderWithoutRadius(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                            const Style& style, OpacityType opaScale);

    static void DrawRectRadiusEqualBorder(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                          const Style& style, OpacityType opaScale);

    static void DrawRectRadiusSmallThanBorder(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                              const Style& style, OpacityType opaScale);

    static void DrawRectRadiusBiggerThanBorder(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                               const Style& style, OpacityType opaScale);

    static void DrawRectRadiusWithoutBorderCon1(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                                const Style& style, OpacityType opaScale);

    static void DrawRectRadiusWithoutBorderCon2(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                                const Style& style, OpacityType opaScale);

    static void DrawRectRadiusWithoutBorderCon3(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                                const Style& style, OpacityType opaScale);

    static void DrawRectRadiusWithoutBorderCon4(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                                const Style& style, OpacityType opaScale);

    static void DrawRectRadiusBiggerThanBorderCon1(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                                   const Style& style, OpacityType opaScale);

    static void DrawRectRadiusBiggerThanBorderCon2(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                                   const Style& style, OpacityType opaScale);

    static void DrawRectRadiusBiggerThanBorderCon3(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                                   const Style& style, OpacityType opaScale);

    static void DrawRectRadiusBiggerThanBorderCon4(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& dirtyRect,
                                                   const Style& style, OpacityType opaScale);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_DRAW_RECT_H
