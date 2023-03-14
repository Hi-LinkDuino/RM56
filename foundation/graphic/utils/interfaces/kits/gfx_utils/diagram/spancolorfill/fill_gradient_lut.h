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
 * @file fill_gradient_lut.h
 * @brief Defines Gradient mode
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_FILL_GRADIENT_LUT_H
#define GRAPHIC_LITE_FILL_GRADIENT_LUT_H

#include "gfx_utils/color.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_dda_line.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_plaindata_array.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_range_adapter.h"
#include "gfx_utils/diagram/spancolorfill/fill_interpolator.h"
#include "gfx_utils/vector.h"
namespace OHOS {
const uint32_t COLOR_PROFILE_SIZE = 4;
const uint16_t COLOR_LUT_SIZE = 512;
/**
* @brief According to remove_all,add_color,and build_lut,
* build the color gradient process, start, end and middle gradient colors
* @param The parameters are colorinterpolator color interpolator and colorlutsize color cell size
* @since 1.0
* @version 1.0
*/
class FillGradientLut {
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
public:
    FillGradientLut() : colorType_(COLOR_LUT_SIZE), colorLutSize_(COLOR_LUT_SIZE), colorProfile_(COLOR_PROFILE_SIZE) {}
    /**
     * @brief Remove all colors
     * @since 1.0
     * @version 1.0
     */
    void RemoveAll()
    {
        colorProfile_.Clear();
    }

    /**
     * @brief Add the color and position you want to gradient during the gradient process
     * @param offset (0-1)
     * @param color Added color
     * @since 1.0
     * @version 1.0
     */
    void AddColor(float offset, const Rgba8T& color)
    {
        colorProfile_.PushBack(ColorPoint(offset, color));
    }

    /**
     * @brief Building a color_typ array from gradient colors
     * Array length 0-255
     * The contents of the array are distributed on the array according to the gradient color
     * @since 1.0
     * @version 1.0
     */
    void BuildLut()
    {
        /*
         * For gradient color array memory quick sort
         */
        QuickSort(colorProfile_, OffsetLess);
        colorProfile_.ReSize(RemoveDuplicates(colorProfile_, OffsetEqual));
        if (colorProfile_.Size() > 1) {
            uint32_t index;
            uint32_t start = colorProfile_[0].offset * colorLutSize_;
            uint32_t end;
            Rgba8T color = colorProfile_[0].color;

            /*
             * Assign initial color calculation to colorprofile [0]
             */
            for (index = 0; index < start; index++) {
                colorType_[index] = color;
            }
            /*
             * From 1 to colorprofile Interpolation color calculation between size ()
             */
            for (index = 1; index < colorProfile_.Size(); index++) {
                end = colorProfile_[index].offset * colorLutSize_;
                ColorInterpolator ci(colorProfile_[index - 1].color,
                                     colorProfile_[index].color,
                                     end - start + 1);
                while (start < end) {
                    colorType_[start] = ci.GetColor();
                    ++ci;
                    ++start;
                }
            }
            color = colorProfile_.End()->color;
            /*
             * Give end color to colorprofile last
             */
            for (; end < colorType_.GetSize(); end++) {
                color = colorProfile_.End()->color;
                colorType_[end] = color;
            }
        }
    }

    /**
     * @brief size Returns the size of the colorprofile
     */
    uint32_t GetSize()
    {
        return colorLutSize_;
    }

    /**
     * @brief Override [] operator
     */
    const Rgba8T& operator[](uint32_t i) const
    {
        return colorType_[i];
    }
private:
    struct ColorPoint {
        float offset;
        Rgba8T color;

        ColorPoint() {}
        /**
         * @brief Input parameter
         * @param offsetValue (0-1)
         * @param color_ Added color
         */
        ColorPoint(float offsetValue, const Rgba8T& colorValue)
            : offset(offsetValue), color(colorValue)
        {
            if (offset < 0.0) {
                offset = 0.0;
            }
            if (offset > 1.0) {
                offset = 1.0;
            }
        }
    };

    /**
     * @brief OffsetLess Returns the comparison result that the offset of a is smaller than that of B
     * @param colorPoint1 struct ColorPoint struct
     * @param colorPoint2 struct ColorPoint struct
     * @return true: colorpoint1 precedes colorpoint2 false colorpoint1 follows colorpoint2
     */
    static bool OffsetLess(const ColorPoint& colorPoint1, const ColorPoint& colorPoint2)
    {
        return colorPoint1.offset < colorPoint2.offset;
    }

    /**
     * @brief OffsetEqual Returns a comparison result in
     * which the offset of comparison a is equal to that of comparison b
     * @param colorPoint1 struct ColorPoint struct
     * @param colorPoint2 struct ColorPoint struct
     * @return Return true: the offsets of colorpoint1 and colorpoint2 are equal,
     * and false: the offsets of a and B are not equal
     */
    static bool OffsetEqual(const ColorPoint& colorPoint1, const ColorPoint& colorPoint2)
    {
        return colorPoint1.offset == colorPoint2.offset;
    }
    Graphic::Vector<ColorPoint> colorProfile_;
    GeometryPlainDataArray<Rgba8T> colorType_;
    uint16_t colorLutSize_;
#endif
};
} // namespace OHOS
#endif
