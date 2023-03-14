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
 * @file renderer_base.h
 * @brief Defines Base renderer
 * @since 1.0
 * @version 1.0.
 */

#ifndef GRAPHIC_LITE_RENDER_BASE_H
#define GRAPHIC_LITE_RENDER_BASE_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "render/render_buffer.h"
#include "render/render_pixfmt_rgba_blend.h"
#include "gfx_utils/color.h"
namespace OHOS {
class RenderBase {
public:
    RenderBase() : pixfmtType_(nullptr), clipBox_(1, 1, 0, 0) {}

    explicit RenderBase(RenderPixfmtRgbaBlend& ren)
        : pixfmtType_(&ren), clipBox_(0, 0, ren.GetWidth() - 1, ren.GetHeight() - 1) {}

    /**
     * @brief afferent pixfmt_type param.
     */
    void Attach(RenderPixfmtRgbaBlend& ren);

    /**
     * @brief Gets the width to render.
     */
    uint32_t GetWidth() const
    {
        return pixfmtType_->GetWidth();
    }

    /**
     * @brief Gets the height to render.
     */
    uint32_t GetHeight() const
    {
        return pixfmtType_->GetHeight();
    }

    /**
     * @brief ClipBox Sets the size of the clipping box.
     * @param x1 rectangular starting point x coordinate.
     * @param y1 rectangular starting point y coordinate.
     * @param x2 rectangular diagonal x coordinate.
     * @param y2 rectangular diagonal y coordinate.
     * @return Returns whether the rectangle was created successfully.
     */
    bool ClipBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    /**
     * @brief ResetClipping Set the size of the clipping box according to the visibility,
     *  The visibility is set to normal, and the invisibility is set to one pixel.
     * @param visibility Visible.
     */
    void ResetClipping(bool visibility);

    /**
     * @brief ClipBoxNaked Sets the size of the box.
     * @param x1 rectangular starting point x coordinate.
     * @param y1 rectangular starting point y coordinate.
     * @param x2 rectangular starting point x coordinate.
     * @param y2 rectangular starting point y coordinate.
     */
    void ClipBoxNaked(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    /**
     * @brief inbox determines whether the point (x, y) is within the rectangle.
     * @param x coordinate of x point.
     * @param y coordinate of y point.
     * @return Return whether.
     */
    bool Inbox(int32_t x, int32_t y) const
    {
        return x >= clipBox_.GetLeft() && y >= clipBox_.GetTop() &&
               x <= clipBox_.GetRight() && y <= clipBox_.GetBottom();
    }

    /**
     * @brief GetClipBox Get cut box.
     * @return Return to the corresponding box.
     */
    const Rect32& GetClipBox() const
    {
        return clipBox_;
    }

    /**
     * @return Returns the minimum value of the x-axis of the box.
     */
    int32_t GetXMin() const
    {
        return clipBox_.GetLeft();
    }

    /**
     * @return Returns the minimum value of the y-axis of the box.
     */
    int32_t GetYMin() const
    {
        return clipBox_.GetTop();
    }

    /**
     * @return Returns the maximum value of the x-axis of the box.
     */
    int32_t GetXMax() const
    {
        return clipBox_.GetRight();
    }

    /**
     * @return Returns the maximum value of the Y-axis of the box.
     */
    int32_t GetYMax() const
    {
        return clipBox_.GetBottom();
    }

    /**
     * @brief Clear clear the pixels in the rectangle under getwidth() width and getheight() height with color.
     * @param color colour.
     */
    void Clear(const Rgba8T& color);

    /**
     * @brief BlendHLine Render scanlines within a certain range of the X axis on the Y axis.
     * @param x1 Scan line left coordinate.
     * @param y  Scan line Y-axis coordinates.
     * @param x2 Scan line right coordinate.
     * @param c The color of the rendered scanline.
     * @param colors Scan line corresponding color array.
     */
    void BlendHLine(int32_t x1, int32_t y, int32_t x2, const Rgba8T& color, uint8_t cover);

    /**
     * @brief BlendSolidHSpan Renders a scan line of a solid line within a certain range of the Y axis.
     * @param x scan line X-axis start coordinate.
     * @param y scan line Y-axis start coordinate.
     * @param len Scan line length.
     * @param c The color of the rendered scanline.
     * @param colors Scan line corresponding color array.
     */
    void BlendSolidHSpan(int32_t x, int32_t y, int32_t len, const Rgba8T& color, const uint8_t* covers);

    /**
     * @brief CopyColorHSpan Copy colors within a certain Y-axis range
     * @param x Scan line X-axis start coordinate
     * @param y Scan line Y-axis start coordinate
     * @param len Scan line length.
     * @param colors Scan line corresponding color array.
     */
    void CopyColorHSpan(int32_t x, int32_t y, int32_t len, const Rgba8T* colors);

    /**
     * @brief BlendColorHSpan Mix colors within a certain height.
     * @param x Scan line X-axis start coordinate
     * @param y Scan line Y-axis start coordinate
     * @param len Scan line length.
     * @param colors Scan line corresponding color array.
     * @param covers Scan line corresponding coverage array.
     * @param cover Coverage
     */
    void BlendColorHSpan(int32_t x, int32_t y, int32_t len, const Rgba8T* colors, const uint8_t* covers,
                         uint8_t cover = COVER_FULL);

    /**
     * @brief ColorHSpanHandler Handle ColorHSpan params.
     * @param x Scan line X-axis start coordinate
     * @param y Scan line Y-axis start coordinate
     * @param len Scan line length.
     * @param colors Scan line corresponding color array.
     * @param covers Scan line corresponding coverage array.
     * @return Returns true if should run the follow programs.
     */
    bool ColorHSpanHandler(int32_t& x, int32_t& y, int32_t& len, const Rgba8T*& colors, const uint8_t*& covers);

private:
    RenderPixfmtRgbaBlend* pixfmtType_;
    Rect32 clipBox_;
};
} // namespace OHOS
#endif
