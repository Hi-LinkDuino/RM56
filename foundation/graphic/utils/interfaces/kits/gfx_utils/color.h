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

/**
 * @addtogroup UI_Utils
 * @{
 *
 * @brief Defines basic UI utils.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file color.h
 *
 * @brief Defines color attributes for the graphics system and implements common color functions.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_COLOR_H
#define GRAPHIC_LITE_COLOR_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/heap_base.h"
#include "graphic_config.h"
#include "graphic_math.h"
#if ENABLE_ARM_MATH
#include "arm_math.h"
#endif
namespace OHOS {
const float PURPLE_MIN = 380.0f;
const float PURPLE_MIDDLE = 420.0f;
const float PURPLE_MAX = 440.0f;
const float BLUE_MAX = 490.0f;
const float CYAN_MAX = 510.0f;
const float GREEN_MAX = 580.0f;
const float ORANGE_MAX = 645.0f;
const float RED_MIN = 700.0f;
const float RED_MAX = 780.0f;
const float FIXED_VALUE = 0.3f;
const float COLOR_CONVERT = 255.0f;

using OpacityType = uint8_t;
/**
 * @brief Enumerates opacity values.
 */
enum {
    /** The opacity is 0. */
    OPA_TRANSPARENT = 0,
    /** The opacity is 100%. */
    OPA_OPAQUE = 255,
};

/**
 * @brief Defines the color attribute when the color depth is <b>16</b>.
 */
typedef union {
    struct {
        /** Blue */
        uint16_t blue : 5;
        /** Green */
        uint16_t green : 6;
        /** Red */
        uint16_t red : 5;
    };
    /** Full RGB data */
    uint16_t full;
} Color16;

/**
 * @brief Defines the color attribute when the color depth is <b>24</b>.
 */
typedef union {
    struct {
        /** Blue */
        uint8_t blue;
        /** Green */
        uint8_t green;
        /** Red */
        uint8_t red;
        /** Alpha (how opaque each pixel is) */
        uint8_t alpha;
    };
    /** Full RGB data */
    uint32_t full;
} Color24;

/**
 * @brief Defines the color attribute when the color depth is <b>32</b>.
 */
typedef union {
    struct {
        /** Blue */
        uint8_t blue;
        /** Green */
        uint8_t green;
        /** Red */
        uint8_t red;
        /** Alpha (how opaque each pixel is) */
        uint8_t alpha;
    };
    /** Full RGB data */
    uint32_t full;
} Color32;

#if COLOR_DEPTH == 16
using ColorType = Color16;
#elif COLOR_DEPTH == 24
using ColorType = Color24;
#elif COLOR_DEPTH == 32
using ColorType = Color32;
#else
#    error "Invalid COLOR_DEPTH, Set it to 16, 24 or 32!"
#endif

struct OrderBgra {
    enum Bgra {
        BLUE = 0,
        GREEN = 1,
        RED = 2,
        ALPHA = 3
    };
};

/**
 * @brief Rgba.
 *
 * Color order: red, green, blue, transparency.
 * Note that the colors in this support floating-point processing.
 * @see Rgba
 * @since 1.0
 * @version 1.0
 */
struct Rgba {
    float red;
    float green;
    float blue;
    float alpha;

    Rgba() {}

    /**
     * @brief Rgba Constructor.
     *
     * @param Red value, green value, blue value, alpha transparency.
     * @since 1.0
     * @version 1.0
     */
    Rgba(float red, float green, float blue, float alpha = 1.0f)
        : red(red), green(green), blue(blue), alpha(alpha) {}

    /**
     * @brief Rgba Constructor.
     *
     * @param Color is the RGBA object, alpha, and transparency.
     * @since 1.0
     * @version 1.0.
     */
    Rgba(const Rgba& color, float alpha)
        : red(color.red),
          green(color.green),
          blue(color.blue),
          alpha(alpha) {}

    /**
     * @brief Clear, color transparency set to 0.
     *
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& Clear()
    {
        red = 0;
        green = 0;
        blue = 0;
        alpha = 0;
        return *this;
    }

    /**
     * @brief Fully transparent.
     *
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& Transparent()
    {
        alpha = 0;
        return *this;
    }

    /**
     * @brief Set transparency.
     *
     * @param Alpha transparency.
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& Opacity(float alphaValue)
    {
        if (alphaValue < 0) {
            alpha = 0;
        } else if (alphaValue > 1) {
            alpha = 1;
        } else {
            alpha = alphaValue;
        }
        return *this;
    }

    /**
     * @brief Get transparency.
     *
     * @return Return transparency.
     * @since 1.0
     * @version 1.0
     */
    float Opacity() const
    {
        return alpha;
    }

    /**
     * @brief Double subtraction.
     *
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& Demultiply()
    {
        if (alpha == 0) {
            red = 0;
            green = 0;
            blue = 0;
        } else {
            float alphaValue = 1.0f / alpha;
            red *= alphaValue;
            green *= alphaValue;
            blue *= alphaValue;
        }
        return *this;
    }
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    /**
     * @brief Gradual change.
     *
     * @param RGBA is the RGBA object and K is the coefficient of variation.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba Gradient(Rgba rgba, float k) const
    {
        Rgba ret;
        ret.red = red + (rgba.red - red) * k;
        ret.green = green + (rgba.green - green) * k;
        ret.blue = blue + (rgba.blue - blue) * k;
        ret.alpha = alpha+ (rgba.alpha - alpha) * k;
        return ret;
    }
#endif
    /**
     * @brief Overloaded operator += function.
     *
     * @param RGBA is a reference to the RGBA object.
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& operator+=(const Rgba& rgba)
    {
        red += rgba.red;
        green += rgba.green;
        blue += rgba.blue;
        alpha += rgba.alpha;
        return *this;
    }

    /**
     * @brief Overloaded operator *= function.
     *
     * @param Multiplyvalue is the coefficient of multiplication.
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& operator*=(float multiplyValue)
    {
        red *= multiplyValue;
        green *= multiplyValue;
        blue *= multiplyValue;
        alpha *= multiplyValue;
        return *this;
    }

    static Rgba NoColor()
    {
        return Rgba(0, 0, 0, 0);
    }

    /**
     * @brief Is the wavelength purple.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsPurpleWave(float waveLength)
    {
        if (waveLength >= PURPLE_MIN && waveLength <= PURPLE_MAX) {
            return Rgba(-1.0f * (waveLength - PURPLE_MAX) / (PURPLE_MAX - PURPLE_MIN), 0.0f, 1.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength blue.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsBlueWave(float waveLength)
    {
        if (waveLength >= PURPLE_MAX && waveLength <= BLUE_MAX) {
            return Rgba(0.0f, (waveLength - PURPLE_MAX) / (BLUE_MAX - PURPLE_MAX), 1.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength cyan.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsCyanWave(float waveLength)
    {
        if (waveLength >= BLUE_MAX && waveLength <= CYAN_MAX) {
            return Rgba(0.0f, 1.0f, -1.0f * (waveLength - CYAN_MAX) / (CYAN_MAX - BLUE_MAX));
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength green.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsGreenWave(float waveLength)
    {
        if (waveLength >= CYAN_MAX && waveLength <= GREEN_MAX) {
            return Rgba((waveLength - CYAN_MAX) / (GREEN_MAX - CYAN_MAX), 1.0f, 0.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength orange.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsOrangeWave(float waveLength)
    {
        if (waveLength >= GREEN_MAX && waveLength <= ORANGE_MAX) {
            return Rgba(1.0f, -1.0f * (waveLength - ORANGE_MAX) / (ORANGE_MAX - GREEN_MAX), 0.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength red.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsRedWave(float waveLength)
    {
        if (waveLength >= ORANGE_MAX && waveLength <= RED_MAX) {
            return Rgba(1.0f, 0.0f, 0.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Initialize color based on wavelength.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    static Rgba InitColorByWaveLength(float waveLength);
};

inline Rgba Rgba::InitColorByWaveLength(float waveLength)
{
    Rgba rgba(0.0f, 0.0f, 0.0f);
    rgba += IsPurpleWave(waveLength);
    rgba += IsBlueWave(waveLength);
    rgba += IsCyanWave(waveLength);
    rgba += IsGreenWave(waveLength);
    rgba += IsOrangeWave(waveLength);
    rgba += IsRedWave(waveLength);
    return rgba;
}

/**
 * @brief Rgba8T color sequence conversion.
 *
 * Color order: red, green, blue, transparency.
 *
 * @see Rgba8T
 * @since 1.0
 * @version 1.0
 */
struct Rgba8T {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;

    enum BaseScale {
        BASE_SHIFT = 8,
        BASE_SCALE = 1 << BASE_SHIFT,
        BASE_MASK = BASE_SCALE - 1,
        BASE_MSB = 1 << (BASE_SHIFT - 1)
    };

    Rgba8T() {}

    /**
     * @brief Rgba8T Constructor
     *
     * @param Red value, green value, blue value, alpha transparency
     * @since 1.0
     * @version 1.0
     */
    Rgba8T(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha = BASE_MASK)
        : red(uint8_t(red)),
        green(uint8_t(green)),
        blue(uint8_t(blue)),
        alpha(uint8_t(alpha)) {}

    /**
     * @brief Rgba8T Constructor
     *
     * @param Color is a reference to the RGBA object
     * @since 1.0
     * @version 1.0
     */
    Rgba8T(const Rgba& color)
    {
        Convert(*this, color);
    }

    /**
     * @brief Rgba8T Constructor
     *
     * @param Color is the reference of the rgba8t object, and alpha is the transparency
     * @since 1.0
     * @version 1.0
     */
    Rgba8T(const Rgba8T& color, uint32_t alpha)
        : red(color.red),
        green(color.green),
        blue(color.blue),
        alpha(uint8_t(alpha)) {}

    /**
     * @brief Rgba8T Constructor
     *
     * @param Color is a reference to the rgba8t < T > object
     * @return Rgba8T
     * @since 1.0
     * @version 1.0
     */
    Rgba8T(const Rgba8T& color)
    {
        Convert(*this, color);
    }

    /**
     * @brief Overloaded RGBA function
     *
     * @return Returns the RGBA object
     * @since 1.0
     * @version 1.0
     */
    operator Rgba() const
    {
        Rgba color;
        Convert(color, *this);
        return color;
    }

    /**
     * @brief Assign the color value in RGBA to rgba8t <linear>
     *
     * @param DST is the reference of rgba8t <linear> object, and Src is the
     *  constant reference of RGBA object
     * @since 1.0
     * @version 1.0
     */
    static void Convert(Rgba8T& dst, const Rgba& src)
    {
        dst.red = uint8_t(MATH_UROUND(src.red * BASE_MASK));
        dst.green = uint8_t(MATH_UROUND(src.green * BASE_MASK));
        dst.blue = uint8_t(MATH_UROUND(src.blue * BASE_MASK));
        dst.alpha = uint8_t(MATH_UROUND(src.alpha * BASE_MASK));
    }

    /**
     * @brief Assign the color value in RGBA to rgba8t <linear>
     *
     * @param DST is the reference of rgba8t <linear> object, and Src is the
     *  constant reference of RGBA object
     * @since 1.0
     * @version 1.0
     */
    static void Convert(Rgba& dst, const Rgba8T& src)
    {
        dst.red = static_cast<float>(src.red) / BASE_MASK;
        dst.green = static_cast<float>(src.green) / BASE_MASK;
        dst.blue = static_cast<float>(src.blue) / BASE_MASK;
        dst.alpha = static_cast<float>(src.alpha) / BASE_MASK;
    }

    static inline uint8_t FromFloat(float value)
    {
        return uint8_t(MATH_UROUND(value * BASE_MASK));
    }

    static inline uint8_t EmptyValue()
    {
        return 0;
    }

    inline bool IsTransparent() const
    {
        return alpha == 0;
    }

    inline bool IsOpaque() const
    {
        return alpha == BASE_MASK;
    }

    static inline uint8_t Multiply(uint8_t valueA, uint8_t valueB)
    {
#if ENABLE_ARM_MATH
        uint32_t uint32_t = __SMUAD(valueA, valueB) + BASE_MSB;
#else
        uint32_t uint32_t = valueA * valueB + BASE_MSB;
#endif
        return uint8_t(((uint32_t >> BASE_SHIFT) + uint32_t) >> BASE_SHIFT);
    }

    static uint8_t DividMultiply(uint8_t valueA, uint8_t valueB)
    {
        if (valueA * valueB == 0) {
            return 0;
        } else if (valueA >= valueB) {
            return BASE_MASK;
        } else {
            if (valueB == 0) {
                valueB = 1;
            }

#if ENABLE_ARM_MATH
            return uint8_t(__UDIV(__SMUAD(valueA, BASE_MASK) + (valueB >> 1), valueB));
#else
            return uint8_t((valueA * BASE_MASK + (valueB >> 1)) / valueB);
#endif
        }
    }

    static inline uint8_t MultCover(uint8_t valueA, uint8_t coverValue)
    {
        return Multiply(valueA, coverValue);
    }

    static inline uint8_t ScaleCover(uint8_t coverValue, uint8_t value)
    {
        return Multiply(value, coverValue);
    }

    static inline uint8_t Prelerp(uint8_t valueP, uint8_t valueQ, uint8_t valueA)
    {
        return valueP + valueQ - Multiply(valueP, valueA);
    }

    static inline uint8_t Lerp(uint8_t valueP, uint8_t valueQ, uint8_t valueA)
    {
#if ENABLE_ARM_MATH
        int32_t t = __SMUAD((valueQ - valueP), valueA) + BASE_MSB - (valueP > valueQ);
#else
        int32_t t = (valueQ - valueP) * valueA + BASE_MSB - (valueP > valueQ);

#endif
        return uint8_t(valueP + (((t >> BASE_SHIFT) + t) >> BASE_SHIFT));
    }

    /**
     * @brief Set transparency
     *
     * @param Alpha is transparency
     * @return Returns a reference to the rgba8t object
     * @since 1.0
     * @version 1.0
     */
    Rgba8T& Opacity(float alphaValue)
    {
        if (alphaValue < 0) {
            alpha = 0;
        } else if (alphaValue > 1) {
            alpha = 1;
        } else {
            alpha = static_cast<uint8_t>(MATH_UROUND(alphaValue * float(BASE_MASK)));
        }
        return *this;
    }

    float Opacity() const
    {
        return alpha / BASE_MASK;
    }
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    /**
     * @brief Gradient, calculate the new rgba8t object according to the change coefficient
     *
     * @param Color is the reference of the rgba8t object, and K is the coefficient of variation
     * @return Returns the rgba8t object
     * @since 1.0
     * @version 1.0
     */
    inline Rgba8T Gradient(const Rgba8T& color, float k) const
    {
        Rgba8T ret;
        uint32_t increaseK = MATH_UROUND(k * BASE_MASK);
        ret.red = Lerp(red, color.red, increaseK);
        ret.green = Lerp(green, color.green, increaseK);
        ret.blue = Lerp(blue, color.blue, increaseK);
        ret.alpha = Lerp(alpha, color.alpha, increaseK);
        return ret;
    }
#endif
    static Rgba8T NoColor()
    {
        return Rgba8T(0, 0, 0, 0);
    }
};

/**
 * @brief Converts colors in different formats and defines common colors.
 *
 * @since 1.0
 * @version 1.0
 */
class Color : public HeapBase {
public:
    /**
     * @brief Mixes two colors (color 1 and color 2) based on a specified opacity.
     *
     * @param c1 Indicates color 1.
     * @param c2 Indicates color 2.
     * @param mix Indicates the alpha, that is, how opaque each pixel is.

     * @return Returns the color data after mixing.
     * @since 1.0
     * @version 1.0
     */
    static ColorType GetMixColor(ColorType c1, ColorType c2, uint8_t mix);

    /**
     * @brief Obtains the color based on the RGB color value.
     *
     * @param r8 Indicates the intensity of red.
     * @param g8 Indicates the intensity of green.
     * @param b8 Indicates the intensity of blue.
     *
     * @return Returns the color data generated.
     * @since 1.0
     * @version 1.0
     */
    static ColorType GetColorFromRGB(uint8_t r8, uint8_t g8, uint8_t b8);

    /**
     * @brief Obtains the color based on the RGBA color value.
     *
     * @param r8 Indicates the intensity of red.
     * @param g8 Indicates the intensity of green.
     * @param b8 Indicates the intensity of blue.
     * @param alpha Indicates the alpha, that is, how opaque each pixel is.
     *
     * @return Returns the color data generated.
     * @since 1.0
     * @version 1.0
     */
    static ColorType GetColorFromRGBA(uint8_t r8, uint8_t g8, uint8_t b8, uint8_t alpha);

    /**
     * @brief Converts color data into the RGBA8888 format.
     *
     * The color data definition varies according to the color depth.
     *
     * @param color Indicates the color data, which is defined by {@link ColorType}.
     * @return Returns the RGBA8888 color data.
     * @since 1.0
     * @version 1.0
     */
    static uint32_t ColorTo32(ColorType color);

    /**
     * @brief Converts color data with the 16-bit color depth into the RGBA8888 format.
     *
     * @param color Indicates the color data with the 16-bit color depth, which is defined by {@link Color16}.
     * @param alpha Indicates the alpha, that is, how opaque each pixel is.
     * @return Returns the RGBA8888 color data.
     * @since 1.0
     * @version 1.0
     */
    static uint32_t ColorTo32(Color16 color, uint8_t alpha);

    /**
     * @brief Converts color data from the RGBA8888 format into the RGB565 format.
     *
     * @param color Indicates the color data with the 32-bit color depth, which is defined by {@link Color32}.
     * @return Returns the RGB565 color data.
     * @since 1.0
     * @version 1.0
     */
    static uint16_t ColorTo16(Color32 color);

    /**
     * @brief Obtains the color data of white.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType White();

    /**
     * @brief Obtains the color data of silver.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Silver();

    /**
     * @brief Obtains the color data of gray.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Gray();

    /**
     * @brief Obtains the color data of black.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Black();

    /**
     * @brief Obtains the color data of red.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Red();

    /**
     * @brief Obtains the color data of maroon.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Maroon();

    /**
     * @brief Obtains the color data of yellow.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Yellow();

    /**
     * @brief Obtains the color data of olive.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Olive();

    /**
     * @brief Obtains the color data of lime.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Lime();

    /**
     * @brief Obtains the color data of green.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Green();

    /**
     * @brief Obtains the color data of cyan.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Cyan();

    /**
     * @brief Obtains the color data of aqua.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Aqua();

    /**
     * @brief Obtains the color data of teal.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Teal();

    /**
     * @brief Obtains the color data of blue.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Blue();

    /**
     * @brief Obtains the color data of navy.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Navy();

    /**
     * @brief Obtains the color data of magenta.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Magenta();

    /**
     * @brief Obtains the color data of purple.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Purple();

    /**
     * @brief Obtains the color data of orange.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Orange();
};
} // namespace OHOS
#endif
