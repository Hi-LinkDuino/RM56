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
 * @file common_gamma_lut.h
 *
 * @brief Gamma correction lookup table, sRGB lookup table, sRGB conversion class
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_COMMON_GAMMA_LUT_H
#define GRAPHIC_LITE_COMMON_GAMMA_LUT_H

#include <cmath>

#include "common_gamma_functions.h"
#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/graphic_math.h"
namespace OHOS {
const int32_t TABLE_SIZE = 256;
const int32_t TABLE_BOUNDARY = 255;
const int32_t TABLE_SIZE_HALF = 128;
const int32_t TABLE_SIZE_QURATER = 64;
const int32_t TABLE_SIZE_EIGHTH = 32;
const int32_t TABLE_SIZE_ONE_SIXTEENTH = 16;
const int32_t TABLE_SIZE_ONE_THIRTYTWO = 8;
const int32_t TABLE_SIZE_ONE_SIXTHFOUR = 4;
const int32_t TABLE_SIZE_ONE_ONETWENTYEIGHT = 2;
const int32_t TABLE_SIZE_ONE_TWOFIFTYSIX = 1;
const int32_t MOVE_BIT = 8;
const float HALF = 0.5f;
const float S_RGB_VALUE = 255.0f;
const float RGB_16_MAX = 65535.0f;
const int32_t ALPHA_MAX = 255;
/**
 * @brief SRGB lookup class
 *
 * @see SrgbLutBase
 * @since 1.0
 * @version 1.0
 */
template <class LinearType>
class SrgbLutBase {
public:
    /**
     * @brief Find the color value in dirtable_ according to the subscript
     * @param Index is the subscript value
     * @return LinearType
     * @since 1.0
     * @version 1.0
     */
    LinearType GetDirTableValue(uint8_t index) const
    {
        return dirTable_[index];
    }

    /**
     * @brief Find color values based on linevalue values
     * @param Linevalue is a linear color value
     * @return Returns a color value of type uint8_t
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetInverseTableValue(LinearType lineValue) const
    {
        // Half search
        uint8_t value = 0;
        if (lineValue > inverseTable_[TABLE_SIZE_HALF]) {
            value = TABLE_SIZE_HALF;
        }
        if (lineValue > inverseTable_[value + TABLE_SIZE_QURATER]) {
            value += TABLE_SIZE_QURATER;
        }
        if (lineValue > inverseTable_[value + TABLE_SIZE_EIGHTH]) {
            value += TABLE_SIZE_EIGHTH;
        }
        if (lineValue > inverseTable_[value + TABLE_SIZE_ONE_SIXTEENTH]) {
            value += TABLE_SIZE_ONE_SIXTEENTH;
        }
        if (lineValue > inverseTable_[value + TABLE_SIZE_ONE_THIRTYTWO]) {
            value += TABLE_SIZE_ONE_THIRTYTWO;
        }
        if (lineValue > inverseTable_[value + TABLE_SIZE_ONE_SIXTHFOUR]) {
            value += TABLE_SIZE_ONE_SIXTHFOUR;
        }
        if (lineValue > inverseTable_[value + TABLE_SIZE_ONE_ONETWENTYEIGHT]) {
            value += TABLE_SIZE_ONE_ONETWENTYEIGHT;
        }
        if (lineValue > inverseTable_[value + TABLE_SIZE_ONE_TWOFIFTYSIX]) {
            value += TABLE_SIZE_ONE_TWOFIFTYSIX;
        }
        return value;
    }

protected:
    LinearType dirTable_[TABLE_SIZE];
    LinearType inverseTable_[TABLE_SIZE];

    SrgbLutBase() {}
};

template <class LinearType>
class SrgbLut;

/**
 * @brief SRGB lookup class, float type
 *
 * @see SrgbLut
 * @since 1.0
 * @version 1.0
 */
template <>
class SrgbLut<float> : public SrgbLutBase<float> {
public:
    SrgbLut()
    {
        // Generate lookup table
        dirTable_[0] = 0;
        inverseTable_[0] = 0;
        for (uint32_t i = 1; i <= TABLE_BOUNDARY; ++i) {
            // Floating point RGB range is within [0,1].
            dirTable_[i] = float(SrgbToLinear(i / S_RGB_VALUE));
            inverseTable_[i] = float(SrgbToLinear((i - HALF) / S_RGB_VALUE));
        }
    }
};

/**
 * @brief SRGB lookup class, uint16_t type
 *
 * @see SrgbLut
 * @since 1.0
 * @version 1.0
 */
template <>
class SrgbLut<uint16_t> : public SrgbLutBase<uint16_t> {
public:
    SrgbLut()
    {
        // Generate lookup table
        dirTable_[0] = 0;
        inverseTable_[0] = 0;
        for (int32_t i = 1; i <= TABLE_BOUNDARY; ++i) {
            // The 16 bit RGB range is within [065535].
            dirTable_[i] = MATH_UROUND(RGB_16_MAX * SrgbToLinear(i / S_RGB_VALUE));
            inverseTable_[i] = MATH_UROUND(RGB_16_MAX * SrgbToLinear((i - HALF) / S_RGB_VALUE));
        }
    }
};

/**
 * @brief SRGB lookup class, uint8_t type
 *
 * @see SrgbLut
 * @since 1.0
 * @version 1.0
 */
template <>
class SrgbLut<uint8_t> : public SrgbLutBase<uint8_t> {
public:
    SrgbLut()
    {
        // Generate lookup table
        dirTable_[0] = 0;
        inverseTable_[0] = 0;
        for (int32_t i = 1; i <= TABLE_BOUNDARY; ++i) {
            // 8-bit RGB is processed by a simple bidirectional lookup table.
            dirTable_[i] = MATH_UROUND(S_RGB_VALUE * SrgbToLinear(i / S_RGB_VALUE));
            inverseTable_[i] = MATH_UROUND(S_RGB_VALUE * LinearToSrgb(i / S_RGB_VALUE));
        }
    }

    /**
     * @brief Find the color value in inverseTable_ according to the subscript
     * @param Index is the subscript value
     * @return Returns a color value of type uint8_t
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetInverseTableValue(uint8_t index) const
    {
        // In this case, the inverse transform is a simple search.
        return inverseTable_[index];
    }
};

/**
 * @brief SRGB transformation class
 *
 * @see StandardRgbConvBase
 * @since 1.0
 * @version 1.0
 */
template <class T>
class StandardRgbConvBase {
public:
    /**
     * @brief From sRGB value to RGB value
     * @param srgb is sRGB type
     * @return Returns a color value of type RGB
     * @since 1.0
     * @version 1.0
     */
    static T RgbFromSrgb(uint8_t srgb)
    {
        return lut_.GetDirTableValue(srgb);
    }

    /**
     * @brief From RGB value to sRGB value
     * @param rgb is of type RGB
     * @return Returns a color value of type sRGB
     * @since 1.0
     * @version 1.0
     */
    static uint8_t RgbToStandardRgb(T rgb)
    {
        return lut_.GetInverseTableValue(rgb);
    }

private:
    static SrgbLut<T> lut_;
};

template <class T>
SrgbLut<T> StandardRgbConvBase<T>::lut_;

template <class T>
class StandardRgbConv;

/**
 * @brief Standardrgbconv conversion class, float type
 *
 * @see StandardRgbConv
 * @since 1.0
 * @version 1.0
 */
template <>
class StandardRgbConv<float> : public StandardRgbConvBase<float> {
public:
    /**
     * @brief From alpha value to sRGB value
     * @param alphaValue is the alpha value
     * @return Returns a color value of type sRGB
     * @since 1.0
     * @version 1.0
     */
    static uint8_t AlphaToStandardRgb(float alphaValue)
    {
        if (alphaValue < 0) {
            return 0;
        }
        if (alphaValue > 1) {
            return ALPHA_MAX;
        }
        return uint8_t(HALF + alphaValue * ALPHA_MAX);
    }

    /**
     * @brief From sRGB value to alpha value
     * @param Srgbvalue is sRGB value
     * @return Returns the alpha value
     * @since 1.0
     * @version 1.0
     */
    static float AlphaFromSrgb(uint8_t srgbValue)
    {
        static const float y = 1 / S_RGB_VALUE;
        return float(srgbValue * y);
    }
};

/**
 * @brief StandardRgbConv conversion class, uint16_t type
 *
 * @see StandardRgbConv
 * @since 1.0
 * @version 1.0
 */
template <>
class StandardRgbConv<uint16_t> : public StandardRgbConvBase<uint16_t> {
public:
    /**
     * @brief From alpha value to sRGB value
     * @param alphaValue is the alpha value
     * @return Returns a color value of type sRGB
     * @since 1.0
     * @version 1.0
     */
    static uint8_t AlphaToStandardRgb(uint16_t alphaValue)
    {
        return alphaValue >> MOVE_BIT;
    }

    /**
     * @brief From sRGB value to alpha value
     * @param Srgbvalue is sRGB value
     * @return Returns the alpha value
     * @since 1.0
     * @version 1.0
     */
    static uint16_t AlphaFromSrgb(uint8_t srgbValue)
    {
        return (srgbValue << MOVE_BIT) | srgbValue;
    }
};

/**
 * @brief StandardRgbConv Conversion class, uint8_t type
 *
 * @see StandardRgbConv
 * @since 1.0
 * @version 1.0
 */
template <>
class StandardRgbConv<uint8_t> : public StandardRgbConvBase<uint8_t> {
public:
    /**
     * @brief From sRGB value to alpha value
     * @param srgbValueis sRGB value
     * @return Returns the alpha value
     * @since 1.0
     * @version 1.0
     */
    static uint8_t AlphaFromSrgb(uint8_t srgbValue)
    {
        return srgbValue;
    }

    /**
     * @brief From alpha value to sRGB value
     * @param alphaValue is the alpha value
     * @return Returns a color value of type sRGB
     * @since 1.0
     * @version 1.0
     */
    static uint8_t AlphaToStandardRgb(uint8_t alphaValue)
    {
        return alphaValue;
    }
};
} // namespace OHOS
#endif
