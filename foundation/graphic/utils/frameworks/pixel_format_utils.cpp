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

#include "gfx_utils/pixel_format_utils.h"

namespace OHOS {
static struct {
    ImagePixelFormat pixelFormat;
    int16_t bpp;
} g_mapBpp[] = {
    {IMAGE_PIXEL_FORMAT_RGB565, 2},
    {IMAGE_PIXEL_FORMAT_ARGB1555, 2},
    {IMAGE_PIXEL_FORMAT_RGB888, 3},
    {IMAGE_PIXEL_FORMAT_ARGB8888, 4},
};

bool PixelFormatUtils::BppOfPixelFormat(ImagePixelFormat pixelFormat, int16_t& bpp)
{
    int16_t len = sizeof(g_mapBpp) / sizeof(g_mapBpp[0]);
    for (int16_t i = 0; i < len; i++) {
        if (pixelFormat == g_mapBpp[i].pixelFormat) {
            bpp = g_mapBpp[i].bpp;
            return true;
        }
    }
    return false;
}

uint16_t PixelFormatUtils::ARGB8888ToARGB1555(uint32_t color)
{
    PF_ARGB1555 ret;
    PF_ARGB8888 in;
    in.full = color;
    ret.alpha = in.alpha ? 1 : 0;
    /*
     * when 32-bitmap image is tansformed to 16-bitmap
     * R should shift right 3 bits
     * G should shift right 3 bits
     * B should shift right 3 bits
     */
    ret.red = in.red >> 3;
    ret.green = in.green >> 3;
    ret.blue = in.blue >> 3;
    return ret.full;
}

uint32_t PixelFormatUtils::ARGB1555ToARGB8888(uint16_t color)
{
    PF_ARGB8888 ret;
    PF_ARGB1555 in;
    in.full = color;
    ret.alpha = in.alpha ? 0xFF : 0x0;
    /*
     * when 16-bitmap image is tansformed to 32-bitmap
     * R should shift left 3 bits
     * G should shift left 3 bits
     * B should shift left 3 bits
     */
    ret.red = in.red << 3;
    ret.green = in.green << 3;
    ret.blue = in.blue << 3;
    return ret.full;
}
} // namespace OHOS
