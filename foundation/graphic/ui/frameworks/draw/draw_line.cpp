/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "draw/draw_line.h"
#include "draw/draw_utils.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
#define INCREASE_ACC(acc, accTemp, adj, step, dir)           \
    do {                                                     \
        (accTemp) = (acc);                                   \
        (acc) += (adj);                                      \
        if ((acc) <= (accTemp)) {                            \
            (step) += (dir);                                 \
        }                                                    \
    } while (0)

#define SWAP_START_END(sx, sy, ex, ey, dx, dy, dir)          \
    do {                                                     \
        if ((dy) >= (dx)) {                                  \
            if ((sy) > (ey)) {                               \
                SWAP_POINTS((sx), (ex), (sy), (ey));         \
            }                                                \
            if ((ex) < (sx)) {                               \
                (dir) = -1;                                  \
            }                                                \
        } else {                                             \
            if ((sx) < (ex)) {                               \
                SWAP_POINTS((sx), (ex), (sy), (ey));         \
            }                                                \
            if ((ey) < (sy)) {                               \
                (dir) = -1;                                  \
            }                                                \
        }                                                    \
    } while (0)

#define SWAP_IF_Y_LARGER(x1, x2, y1, y2)                     \
    if ((y1) > (y2)) {                                       \
        SWAP_POINTS((x1), (x2), (y1), (y2));                 \
    }

#define SWAP_IF_X_SMALLER(x1, x2, y1, y2)                    \
    if ((x1) < (x2)) {                                       \
        SWAP_POINTS((x1), (x2), (y1), (y2));                 \
    }

void DrawLine::Draw(BufferInfo& gfxDstBuffer,
                    const Point& start,
                    const Point& end,
                    const Rect& mask,
                    int16_t width,
                    const ColorType& color,
                    OpacityType opacity)
{
    if ((width == 0) || (opacity == OPA_TRANSPARENT)) {
        return;
    }

    int16_t yTop;
    int16_t yBottom;

    if (start.y < end.y) {
        yTop = start.y - width / 2;  // 2: half
        yBottom = end.y + width / 2; // 2: half
    } else {
        yTop = end.y - width / 2;      // 2: half
        yBottom = start.y + width / 2; // 2: half
    }

    if ((yBottom < mask.GetTop()) || (yTop > mask.GetBottom())) {
        return;
    }

    if (start.y == end.y) {
        DrawHorizontalLine(gfxDstBuffer, start, end, mask, width, color, opacity);
    } else if (start.x == end.x) {
        DrawVerticalLine(gfxDstBuffer, start, end, mask, width, color, opacity);
    } else {
        DrawWuLine(gfxDstBuffer, start, end, mask, width, color, opacity);
    }
}

void DrawLine::DrawVerticalLine(BufferInfo& gfxDstBuffer,
                                const Point& start,
                                const Point& end,
                                const Rect& mask,
                                int16_t width,
                                const ColorType& color,
                                OpacityType opacity)
{
    Rect rect;

    if (start.y < end.y) {
        rect.SetX(start.x - width / 2); // 2: half
        rect.SetY(start.y);
        rect.SetWidth(width);
        rect.SetHeight(end.y - start.y + 1);
    } else {
        rect.SetX(end.x - width / 2); // 2: half
        rect.SetY(end.y);
        rect.SetWidth(width);
        rect.SetHeight(start.y - end.y + 1);
    }

    DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, rect, mask, color, opacity);
}

void DrawLine::DrawHorizontalLine(BufferInfo& gfxDstBuffer, const Point& start,
                                  const Point& end,
                                  const Rect& mask,
                                  int16_t width,
                                  const ColorType& color,
                                  OpacityType opacity)
{
    Rect rect;

    if (start.x < end.x) {
        rect.SetX(start.x);
        rect.SetY(start.y - width / 2); // 2: half
        rect.SetWidth(end.x - start.x + 1);
        rect.SetHeight(width);
    } else {
        rect.SetX(end.x);
        rect.SetY(end.y - width / 2); // 2: half
        rect.SetWidth(start.x - end.x + 1);
        rect.SetHeight(width);
    }

    DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, rect, mask, color, opacity);
}

void DrawLine::DrawWuLine(BufferInfo& gfxDstBuffer, const Point& start, const Point& end,
    const Rect& mask, int16_t width, const ColorType& color, OpacityType opacity)
{
    if (width <= 2) { // 2 : thin line width
        DrawThinWuLine(gfxDstBuffer, start, end, mask, width, color, opacity);
        return;
    }

    int16_t sx = start.x;
    int16_t sy = start.y;
    int16_t ex = end.x;
    int16_t ey = end.y;
    uint16_t dx = MATH_ABS(ex - sx);
    uint16_t dy = MATH_ABS(ey - sy);
    int8_t dir = 1;
    SWAP_START_END(sx, sy, ex, ey, dx, dy, dir);

    // calculate four vertex ordered according to dy and dx
    float plot = -static_cast<float>(ex - sx) / static_cast<float>(ey - sy);
    float offset = 1 / (1 + plot * plot);
    offset = Sqrt(offset) * width / 2; // 2: half
    float x0 = sx + offset;
    float y0 = sy + (x0 - sx) * plot;
    float x1 = sx - offset;
    float y1 = sy + (x1 - sx) * plot;
    float x2 = ex + offset;
    float y2 = ey + (x2 - ex) * plot;
    float x3 = ex - offset;
    float y3 = ey + (x3 - ex) * plot;
    int16_t x0Int = MATH_ROUND(x0);
    int16_t y0Int = MATH_ROUND(y0);
    int16_t x1Int = MATH_ROUND(x1);
    int16_t y1Int = MATH_ROUND(y1);
    int16_t x2Int = MATH_ROUND(x2);
    int16_t y2Int = MATH_ROUND(y2);
    int16_t x3Int = MATH_ROUND(x3);
    int16_t y3Int = MATH_ROUND(y3);
    // width is longer than distance between start point and end point, need swap direction of line.
    if (dx * dx + dy * dy < width * width) {
        if ((dx == 1) && (dy == 1)) {
            DrawThinWuLine(gfxDstBuffer, { x0Int, y0Int }, { x3Int, y3Int }, mask, 2, color, opacity); // 2 : line width
            return;
        }
        dx = MATH_ABS(x0Int - x1Int);
        dy = MATH_ABS(y0Int - y1Int);
        if (dy == dx) {
            dir = -dir;
        }
    }
    if (dy >= dx) {
        SWAP_IF_Y_LARGER(x0Int, x1Int, y0Int, y1Int);
        SWAP_IF_Y_LARGER(x1Int, x2Int, y1Int, y2Int);
        SWAP_IF_Y_LARGER(x2Int, x3Int, y2Int, y3Int);
        SWAP_IF_Y_LARGER(x0Int, x1Int, y0Int, y1Int);
        SWAP_IF_Y_LARGER(x1Int, x2Int, y1Int, y2Int);
        SWAP_IF_Y_LARGER(x0Int, x1Int, y0Int, y1Int);
        if (dir == -1) {
            SWAP_IF_X_SMALLER(x1Int, x0Int, y1Int, y0Int);
            SWAP_IF_X_SMALLER(x3Int, x2Int, y3Int, y2Int);
        } else {
            SWAP_IF_X_SMALLER(x0Int, x1Int, y0Int, y1Int);
            SWAP_IF_X_SMALLER(x2Int, x3Int, y2Int, y3Int);
        }
    } else {
        SWAP_IF_X_SMALLER(x0Int, x1Int, y0Int, y1Int);
        SWAP_IF_X_SMALLER(x1Int, x2Int, y1Int, y2Int);
        SWAP_IF_X_SMALLER(x2Int, x3Int, y2Int, y3Int);
        SWAP_IF_X_SMALLER(x0Int, x1Int, y0Int, y1Int);
        SWAP_IF_X_SMALLER(x1Int, x2Int, y1Int, y2Int);
        SWAP_IF_X_SMALLER(x0Int, x1Int, y0Int, y1Int);
        if (dir == 1) {
            SWAP_IF_Y_LARGER(x1Int, x0Int, y1Int, y0Int);
            SWAP_IF_Y_LARGER(x3Int, x2Int, y3Int, y2Int);
        } else {
            SWAP_IF_Y_LARGER(x0Int, x1Int, y0Int, y1Int);
            SWAP_IF_Y_LARGER(x2Int, x3Int, y2Int, y3Int);
        }
    }

    uint64_t adj0;
    uint16_t accTemp0;
    uint16_t acc0 = 0;
    uint64_t adj1;
    uint16_t accTemp1;
    uint16_t acc1 = 0;
    uint16_t accTemp2;
    uint16_t acc2 = 0;

    int16_t endPoints0[MAX_LINE_WIDTH] = { 0 };
    int16_t endPoints1[MAX_LINE_WIDTH] = { 0 };
    int16_t temp0 = 0;
    int16_t temp1 = 0;
    int16_t edge0 = 0;
    int16_t edge1 = 0;
    Rect rect;

    // sort points
    if (dy >= dx) {
        adj0 = static_cast<uint64_t>(dx << SHIFT_16) / static_cast<uint64_t>(dy);
        adj1 = static_cast<uint64_t>(MATH_ABS(y1Int - y0Int) << SHIFT_16) /
            static_cast<uint64_t>(MATH_ABS(x1Int - x0Int));
        if (adj1 != 0) {
            // draw top line
            dx = MATH_ABS(x1Int - x0Int);
            sx = x0Int;
            sy = y0Int;
            DrawUtils::GetInstance()->DrawPixel(gfxDstBuffer, x0Int, y0Int, mask, color, opacity);
            while (--dx) {
                accTemp1 = acc1;
                acc1 += adj1;
                if (acc1 <= accTemp1) {
                    if (sy - y0Int < MAX_LINE_WIDTH) {
                        endPoints0[sy - y0Int] = sx;
                    }
                    sy++;
                }
                sx -= dir;
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx, sy, mask, color, opacity,
                    (acc1 >> SHIFT_8) ^ OPA_OPAQUE);
            }
            if (sy - y0Int < MAX_LINE_WIDTH) {
                endPoints0[sy - y0Int] = sx - dir;
            }

            // draw botttom line
            acc1 = 0;
            dx = MATH_ABS(x3Int - x2Int);
            sy = y3Int;
            sx = x3Int;
            DrawUtils::GetInstance()->DrawPixel(gfxDstBuffer, x3Int, y3Int, mask, color, opacity);
            while (--dx) {
                accTemp1 = acc1;
                acc1 += adj1;
                if (acc1 <= accTemp1) {
                    if (temp1 < MAX_LINE_WIDTH) {
                        endPoints1[temp1++] = sx;
                    }
                    sy--;
                }
                sx += dir;
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx, sy, mask, color, opacity,
                                                          (acc1 >> SHIFT_8) ^ OPA_OPAQUE);
            }
            if (temp1 < MAX_LINE_WIDTH) {
                endPoints1[temp1++] = sx + dir;
            }
        } else {
            /* If y0 is equal to y1, draw two horizontal lines as the top line and bottom line. */
            rect.SetRect(MATH_MIN(x0Int, x1Int), y0Int, MATH_MAX(x0Int, x1Int), y1Int);
            DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, rect, mask, color, opacity);
            rect.SetRect(MATH_MIN(x2Int, x3Int), y3Int, MATH_MAX(x2Int, x3Int), y2Int);
            DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, rect, mask, color, opacity);
        }

        sx = x0Int;
        sy = y0Int + 1;
        dy = MATH_ABS(y3Int - y0Int);
        if (dy == 0) {
            return;
        }
        int16_t sxTemp = x1Int;
        while (--dy) {
            if (sy <= y1Int) {
                INCREASE_ACC(acc0, accTemp0, adj0, sx, dir);
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx + dir, sy, mask,
                                                          color, opacity, acc0 >> SHIFT_8);
                if (temp0 < MAX_LINE_WIDTH) {
                    edge0 = endPoints0[temp0++];
                }
                edge1 = sx;
            } else if (sy < y2Int) {
                INCREASE_ACC(acc0, accTemp0, adj0, sx, dir);
                INCREASE_ACC(acc2, accTemp2, adj0, sxTemp, dir);
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx + dir, sy, mask,
                                                          color, opacity, acc0 >> SHIFT_8);
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sxTemp, sy, mask, color, opacity,
                                                          (acc2 >> SHIFT_8) ^ OPA_OPAQUE);
                edge0 = sxTemp + dir;
                edge1 = sx;
            } else if (sy < y3Int) {
                INCREASE_ACC(acc2, accTemp2, adj0, sxTemp, dir);
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sxTemp, sy, mask, color, opacity,
                                                          (acc2 >> SHIFT_8) ^ OPA_OPAQUE);
                edge0 = sxTemp + dir;
                if (temp1 > 0) {
                    edge1 = endPoints1[--temp1];
                }
            }
            if ((dir < 0) && (edge0 > edge1)) {
                SWAP_INT16(edge0, edge1);
            }
            rect.SetRect(edge0, sy, edge1, sy);
            DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, rect, mask, color, opacity);
            sy++;
        }
    } else {
        adj0 = static_cast<uint64_t>(dy << SHIFT_16) / static_cast<uint64_t>(dx);
        adj1 = static_cast<uint64_t>(MATH_ABS(x1Int - x0Int) << SHIFT_16) /
            static_cast<uint64_t>(MATH_ABS(y1Int - y0Int));
        if (adj1 != 0) {
            // draw top line
            dy = MATH_ABS(y1Int - y0Int);
            sx = x0Int;
            sy = y0Int;
            DrawUtils::GetInstance()->DrawPixel(gfxDstBuffer, sx, sy, mask, color, opacity);
            while (--dy) {
                accTemp1 = acc1;
                acc1 += adj1;
                if (acc1 <= accTemp1) {
                    if (x0Int - sx < MAX_LINE_WIDTH) {
                        endPoints0[x0Int - sx] = sy;
                    }
                    sx--;
                }
                sy -= dir;
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx, sy, mask, color, opacity,
                                                          (acc1 >> SHIFT_8) ^ OPA_OPAQUE);
            }
            if (x0Int - sx < MAX_LINE_WIDTH) {
                endPoints0[x0Int - sx] = sy - dir;
            }

            // draw botttom line
            acc1 = 0;
            dy = MATH_ABS(y3Int - y2Int);
            sy = y3Int;
            sx = x3Int;
            while (--dy) {
                accTemp1 = acc1;
                acc1 += adj1;
                if (acc1 <= accTemp1) {
                    if (temp1 < MAX_LINE_WIDTH) {
                        endPoints1[temp1++] = sy;
                    }
                    sx++;
                }
                sy += dir;
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx, sy, mask, color, opacity,
                                                          (acc1 >> SHIFT_8) ^ OPA_OPAQUE);
            }
            DrawUtils::GetInstance()->DrawPixel(gfxDstBuffer, x3Int, y3Int, mask, color, opacity);
            if (temp1 < MAX_LINE_WIDTH) {
                endPoints1[temp1++] = sy + dir;
            }
        } else {
            /* If x0 is equal to x1, draw two vertical lines as the top line and bottom line. */
            rect.SetRect(x1Int, MATH_MIN(y0Int, y1Int), x0Int, MATH_MAX(y0Int, y1Int));
            DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, rect, mask, color, opacity);
            rect.SetRect(x3Int, MATH_MIN(y2Int, y3Int), x2Int, MATH_MAX(y2Int, y3Int));
            DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, rect, mask, color, opacity);
        }

        sx = x0Int - 1;
        sy = y0Int;
        dx = MATH_ABS(x3Int - x0Int);
        int16_t syTemp = y1Int;
        if (dx == 0) {
            return;
        }
        while (--dx) {
            if (sx >= x1Int) {
                INCREASE_ACC(acc0, accTemp0, adj0, sy, dir);
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx, sy + dir, mask,
                                                          color, opacity, acc0 >> SHIFT_8);
                if (temp0 < MAX_LINE_WIDTH) {
                    edge0 = endPoints0[temp0++];
                }
                edge1 = sy;
            } else if (sx > x2Int) {
                INCREASE_ACC(acc0, accTemp0, adj0, sy, dir);
                INCREASE_ACC(acc2, accTemp2, adj0, syTemp, dir);
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx, sy + dir, mask,
                                                          color, opacity, acc0 >> SHIFT_8);
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx, syTemp, mask, color,
                                                          opacity, (acc2 >> SHIFT_8) ^ OPA_OPAQUE);
                edge0 = syTemp + dir;
                edge1 = sy;
            } else if (sx > x3Int) {
                INCREASE_ACC(acc2, accTemp2, adj0, syTemp, dir);
                DrawUtils::GetInstance()->DrawPixelInLine(gfxDstBuffer, sx, syTemp, mask, color, opacity,
                                                          (acc2 >> SHIFT_8) ^ OPA_OPAQUE);
                edge0 = syTemp + dir;
                if (temp1 > 0) {
                    edge1 = endPoints1[--temp1];
                }
            }
            if ((dir < 0) && (edge0 > edge1)) {
                SWAP_INT16(edge0, edge1);
            }
            rect.SetRect(sx, edge0, sx, edge1);
            DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, rect, mask, color, opacity);
            sx--;
        }
    }
}

void DrawLine::DrawThinWuLine(BufferInfo& gfxDstBuffer, const Point& start, const Point& end,
    const Rect& mask, int16_t width, const ColorType& color, OpacityType opacity)
{
    int16_t sx = start.x;
    int16_t sy = start.y;
    int16_t ex = end.x;
    int16_t ey = end.y;
    uint16_t dx = MATH_ABS(ex - sx);
    uint16_t dy = MATH_ABS(ey - sy);
    uint64_t adj;
    uint16_t accTemp;
    uint16_t acc = 0;
    int8_t dir = 1;
    SWAP_START_END(sx, sy, ex, ey, dx, dy, dir);
    if (dy >= dx) {
        adj = static_cast<uint64_t>(dx << SHIFT_16) / static_cast<uint64_t>(dy);
        while (dy--) {
            INCREASE_ACC(acc, accTemp, adj, sx, dir);
            sy++;
            if (width == 1) {
                DrawUtils::GetInstance()->DrawAdjPixelInLine(gfxDstBuffer, sx, sy, sx + dir, sy, mask,
                                                             color, opacity, acc >> SHIFT_8);
            } else {
                DrawUtils::GetInstance()->DrawVerPixelInLine(gfxDstBuffer, sx, sy, dir, mask,
                                                             color, opacity, acc >> SHIFT_8);
            }
        }
    } else {
        adj = static_cast<uint64_t>(dy << SHIFT_16) / static_cast<uint64_t>(dx);
        while (dx--) {
            INCREASE_ACC(acc, accTemp, adj, sy, dir);
            sx--;
            if (width == 1) {
                DrawUtils::GetInstance()->DrawAdjPixelInLine(gfxDstBuffer, sx, sy, sx, sy + dir, mask,
                                                             color, opacity, acc >> SHIFT_8);
            } else {
                DrawUtils::GetInstance()->DrawHorPixelInLine(gfxDstBuffer, sx, sy, dir, mask,
                                                             color, opacity, acc >> SHIFT_8);
            }
        }
    }
}
} // namespace OHOS
