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
#ifndef GRAPHIC_LITE_FILL_PATTERN_RGBA_H
#define GRAPHIC_LITE_FILL_PATTERN_RGBA_H

#include <gfx_utils/image_info.h>
#include "gfx_utils/color.h"
#include "fill_base.h"
/**
 * @file span_pattern_rgba.h
 * @brief Defines Scan line of pattern
 * @since 1.0
 * @version 1.0
 */

namespace OHOS {
class FillPatternRgba : public SpanBase {
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
public:
    FillPatternRgba() {}

    FillPatternRgba(const ImageInfo* image, PatternRepeatMode patternRepeat, float startX, float startY)
        : patternRepeat_(patternRepeat)
    {
        if (image->header.colorMode == ARGB8888) {
            patternImage_ = reinterpret_cast<Color32*>(const_cast<uint8_t*>(image->data));
            patternImageheigth_ = image->header.height;
            patternImagewidth_ = image->header.width;
            patternStartX_ = startX;
            patternStartY_ = startY;
        }
    }

    void Attach(const ImageInfo* image, PatternRepeatMode patternRepeat, float startX, float startY)
    {
        patternRepeat_ = patternRepeat;
        if (image->header.colorMode == ARGB8888) {
            patternImage_ = reinterpret_cast<Color32*>(const_cast<uint8_t*>(image->data));
            patternImageheigth_ = image->header.height;
            patternImagewidth_ = image->header.width;
            patternStartX_ = startX;
            patternStartY_ = startY;
        }
    }

    /**
     * @brief black
     * @return black
     */
    Color32 NoColor() const
    {
        Color32 fillColor;
        fillColor.red = 0;
        fillColor.green = 0;
        fillColor.blue = 0;
        fillColor.alpha = MAX_COLOR_NUM;
        return fillColor;
    }

    /**
     * @brief Prepare Ready for render_scanlines_aa in
     */
    void Prepare() {}

    void Generate(Rgba8T* span, int32_t x, int32_t y, uint32_t len)
    {
        y = y - patternStartY_;
        x = x - patternStartX_;
        for (; len; --len, span++, x++) {
            if (patternRepeat_ == NO_REPEAT) {
                if (x >= patternImagewidth_ ||
                        y >= patternImageheigth_) {
                    ChangeColor(span, NoColor());
                } else {
                    ChangeColor(span, patternImage_[patternImagewidth_ * y + x]);
                }
            }

            if (patternRepeat_ == REPEAT) {
                x = x % patternImagewidth_;
                y = y % patternImageheigth_;
                if (x >= patternImagewidth_ || y >= patternImageheigth_) {
                    ChangeColor(span, NoColor());
                } else {
                    ChangeColor(span, patternImage_[patternImagewidth_ * y + x]);
                }
            }

            if (patternRepeat_ == REPEAT_X) {
                x = x % patternImagewidth_;
                if (y >= patternImageheigth_) {
                    ChangeColor(span, NoColor());
                } else {
                    y = y % patternImageheigth_;
                    if (x >= patternImagewidth_ || y >= patternImageheigth_) {
                        ChangeColor(span, NoColor());
                    } else {
                        ChangeColor(span, patternImage_[patternImagewidth_ * y + x]);
                    }
                }
            }

            if (patternRepeat_ == REPEAT_Y) {
                y = y % patternImageheigth_;
                if (x >= patternImageheigth_) {
                    ChangeColor(span, NoColor());
                } else {
                    x = x % patternImagewidth_;
                    if (x >= patternImagewidth_ ||
                       y >= patternImageheigth_) {
                        ChangeColor(span, NoColor());
                    } else {
                        ChangeColor(span, patternImage_[patternImagewidth_ * y + x]);
                    }
                }
            }
        }
    }
private:
    PatternRepeatMode patternRepeat_;
    const Color32* patternImage_;
    uint16_t patternImageheigth_;
    uint16_t patternImagewidth_;
    float patternStartX_;
    float patternStartY_;

    void ChangeColor(Rgba8T* color, Color32 colorType)
    {
        color->red = colorType.red;
        color->green = colorType.green;
        color->blue = colorType.blue;
        color->alpha = colorType.alpha;
    }
#endif
};
} // namespace OHOS
#endif
