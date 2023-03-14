/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * @brief Defines Base renderer
 * @since 1.0
 * @version 1.0.
 */

#include "render_base.h"

namespace OHOS {
void RenderBase::Attach(RenderPixfmtRgbaBlend& ren)
{
    pixfmtType_ = &ren;
    clipBox_.SetRect(0, 0, ren.GetWidth() - 1, ren.GetHeight() - 1);
}

bool RenderBase::ClipBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    Rect32 cb(x1, y1, x2, y2);
    cb.Normalize();
    if (cb.Intersect(cb, Rect32(0, 0, GetWidth() - 1, GetHeight() - 1))) {
        clipBox_.SetRect(cb.GetLeft(), cb.GetTop(), cb.GetRight(), cb.GetBottom());
        return true;
    }
    ClipBoxNaked(1, 1, 0, 0);
    return false;
}

void RenderBase::ResetClipping(bool visibility)
{
    if (visibility) {
        ClipBoxNaked(0, 0, GetWidth() - 1, GetHeight() - 1);
    } else {
        ClipBoxNaked(1, 1, 0, 0);
    }
}

void RenderBase::ClipBoxNaked(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    clipBox_.SetRect(x1, y1, x2, y2);
}

void RenderBase::Clear(const Rgba8T& color)
{
    if (GetWidth()) {
        for (uint32_t y = 0; y < GetHeight(); y++) {
            pixfmtType_->CopyHLine(0, y, GetWidth(), color);
        }
    }
}

void RenderBase::BlendHLine(int32_t x1, int32_t y, int32_t x2, const Rgba8T& color, uint8_t cover)
{
    if (x1 > x2) {
        int32_t swapTemp = x1;
        x1 = x2;
        x2 = swapTemp;
    }
    if (y > GetYMax() || y < GetYMin() || x1 > GetXMax() || x2 < GetXMin()) {
        return;
    }
    if (x1 < GetXMin()) {
        x1 = GetXMin();
    }

    if (x2 > GetXMax()) {
        x2 = GetXMax();
    }
    pixfmtType_->BlendHLine(x1, y, x2 - x1 + 1, color, cover);
}

void RenderBase::BlendSolidHSpan(int32_t x, int32_t y, int32_t len, const Rgba8T& color, const uint8_t* covers)
{
    if (y > GetYMax() || y < GetYMin()) {
        return;
    }
    if (x < GetXMin()) {
        len -= GetXMin() - x;
        if (len <= 0) {
            return;
        }
        covers += GetXMin() - x;
        x = GetXMin();
    }
    if (x + len > GetXMax()) {
        len = GetXMax() - x + 1;
        if (len <= 0) {
            return;
        }
    }
    pixfmtType_->BlendSolidHSpan(x, y, len, color, covers);
}

void RenderBase::CopyColorHSpan(int32_t x, int32_t y, int32_t len, const Rgba8T* colors)
{
    const uint8_t* covers = nullptr;
    if (!ColorHSpanHandler(x, y, len, colors, covers)) {
        return;
    }
    pixfmtType_->CopyColorHSpan(x, y, len, colors);
}

void RenderBase::BlendColorHSpan(int32_t x, int32_t y, int32_t len, const Rgba8T* colors, const uint8_t* covers,
                                 uint8_t cover)
{
    if (!ColorHSpanHandler(x, y, len, colors, covers)) {
        return;
    }
    pixfmtType_->BlendColorHSpan(x, y, len, colors, covers, cover);
}

bool RenderBase::ColorHSpanHandler(int32_t& x, int32_t& y, int32_t& len, const Rgba8T*& colors, const uint8_t*& covers)
{
    if (y > GetYMax() || y < GetYMin()) {
        return false;
    }
    if (x < GetXMin()) {
        int32_t d = GetXMin() - x;
        len -= d;
        if (len <= 0) {
            return false;
        }
        if (covers) {
            covers += d;
        }
        colors += d;
        x = GetXMin();
    }
    if (x + len > GetXMax()) {
        len = GetXMax() - x + 1;
        if (len <= 0) {
            return false;
        }
    }
    return true;
}
} // namespace OHOS
