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

#include "gfx_utils/color.h"
namespace OHOS {
ColorType Color::GetMixColor(ColorType c1, ColorType c2, uint8_t mix)
{
    ColorType ret;
    /* COLOR_DEPTH is 16 or 32 */
    // 8: right move 8 bit. 255: 2^8-1
    ret.red = (static_cast<uint16_t>(c1.red) * mix + (static_cast<uint16_t>(c2.red) * (255 ^ mix))) >> 8;
    // 8: right move 8 bit. 255: 2^8-1
    ret.green = (static_cast<uint16_t>(c1.green) * mix + (static_cast<uint16_t>(c2.green) * (255 ^ mix))) >> 8;
    // 8: right move 8 bit. 255: 2^8-1
    ret.blue = (static_cast<uint16_t>(c1.blue) * mix + (static_cast<uint16_t>(c2.blue) * (255 ^ mix))) >> 8;
#if COLOR_DEPTH == 32
    ret.alpha = 0xFF;
#endif
    return ret;
}

ColorType Color::GetColorFromRGB(uint8_t r8, uint8_t g8, uint8_t b8)
{
    return GetColorFromRGBA(r8, g8, b8, 0xFF);
}

ColorType Color::GetColorFromRGBA(uint8_t r8, uint8_t g8, uint8_t b8, uint8_t alpha)
{
    ColorType rColor;
#if COLOR_DEPTH == 16
    rColor.red = r8 >> 3;   // 3: shift right 3 places
    rColor.blue = b8 >> 3;  // 3: shift right 3 places
    rColor.green = g8 >> 2; // 2: shift right 2 places
#elif COLOR_DEPTH == 24
    rColor.full =  (r8 << 16) | (g8 << 8) | (b8);
#elif COLOR_DEPTH == 32
    // 24, 16, 8: shift right places
    rColor.full = (alpha << 24) | (r8 << 16) | (g8 << 8) | (b8);
#endif
    return rColor;
}

uint32_t Color::ColorTo32(ColorType color)
{
#if COLOR_DEPTH == 16
    Color32 ret;
    ret.red = color.red << 3;     /* (2^8 - 1)/(2^5 - 1) = 255/31 = 8 */
    ret.green = color.green << 2; /* (2^8 - 1)/(2^6 - 1) = 255/63 = 4 */
    ret.blue = color.blue << 3;   /* (2^8 - 1)/(2^5 - 1) = 255/31 = 8 */
    ret.alpha = 0xFF;
    return ret.full;
#elif COLOR_DEPTH == 24
    Color32 ret;
    ret.red = color.red;
    ret.green = color.green;
    ret.blue = color.blue;
    ret.alpha = 0xFF;
    return ret.full;
#elif COLOR_DEPTH == 32
    return color.full;
#endif
}

uint32_t Color::ColorTo32(Color16 color, uint8_t alpha)
{
    Color32 ret;
    /*
     * when 16-bitmap image is tansformed to 32-bitmap,
     * R should shift left 3 bits,
     * G should shift left 2 bits,
     * B should shift left 3 bits,
     */
    ret.red = color.red << 3;     /* (2^8 - 1)/(2^5 - 1) = 255/31 = 8 */
    ret.green = color.green << 2; /* (2^8 - 1)/(2^6 - 1) = 255/63 = 4 */
    ret.blue = color.blue << 3;   /* (2^8 - 1)/(2^5 - 1) = 255/31 = 8 */
    ret.alpha = alpha;
    return ret.full;
}

uint16_t Color::ColorTo16(Color32 color)
{
    /*
     * when 32-bitmap image is tansformed to 16-bitmap,
     * R should shift right 3 bits,
     * G should shift right 2 bits,
     * B should shift right 3 bits,
     */
    Color16 rColor;
    rColor.red = color.red >> 3;
    rColor.green = color.green >> 2;
    rColor.blue = color.blue >> 3;
    return rColor.full;
}

ColorType Color::White()
{
    return GetColorFromRGB(0xFF, 0xFF, 0xFF);
}

ColorType Color::Silver()
{
    return GetColorFromRGB(0xC0, 0xC0, 0xC0);
}

ColorType Color::Gray()
{
    return GetColorFromRGB(0x80, 0x80, 0x80);
}

ColorType Color::Black()
{
    return GetColorFromRGB(0x00, 0x00, 0x00);
}

ColorType Color::Red()
{
    return GetColorFromRGB(0xFF, 0x00, 0x00);
}

ColorType Color::Maroon()
{
    return GetColorFromRGB(0x80, 0x00, 0x00);
}

ColorType Color::Yellow()
{
    return GetColorFromRGB(0xFF, 0xFF, 0x00);
}

ColorType Color::Olive()
{
    return GetColorFromRGB(0x80, 0x80, 0x00);
}

ColorType Color::Lime()
{
    return GetColorFromRGB(0x00, 0xFF, 0x00);
}

ColorType Color::Green()
{
    return GetColorFromRGB(0x00, 0xFF, 0x00);
}

ColorType Color::Cyan()
{
    return GetColorFromRGB(0x00, 0xFF, 0xFF);
}

ColorType Color::Aqua()
{
    return GetColorFromRGB(0x00, 0xFF, 0xFF);
}

ColorType Color::Teal()
{
    return GetColorFromRGB(0x00, 0x80, 0x80);
}

ColorType Color::Blue()
{
    return GetColorFromRGB(0x00, 0x00, 0xFF);
}

ColorType Color::Navy()
{
    return GetColorFromRGB(0x00, 0x00, 0x80);
}

ColorType Color::Magenta()
{
    return GetColorFromRGB(0xFF, 0x00, 0xFF);
}

ColorType Color::Purple()
{
    return GetColorFromRGB(0x80, 0x00, 0x80);
}

ColorType Color::Orange()
{
    return GetColorFromRGB(0xFF, 0xA5, 0x00);
}
} // namespace OHOS
