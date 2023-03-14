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

#ifndef GRAPHIC_LITE_PIXEL_FORMAT_UTILS
#define GRAPHIC_LITE_PIXEL_FORMAT_UTILS

#include <stdint.h>

namespace OHOS {
union PF_ARGB1555 {
    struct {
        uint16_t blue : 5;
        uint16_t green : 5;
        uint16_t red : 5;
        uint16_t alpha : 1;
    };
    uint16_t full;
};

union PF_ARGB8888 {
    struct {
        uint32_t blue : 8;
        uint32_t green : 8;
        uint32_t red : 8;
        uint32_t alpha : 8;
    };
    uint32_t full;
};

/**
 * @brief Enumerates a pixel format.
 */
enum ImagePixelFormat {
    /** Invalid pixel format */
    IMAGE_PIXEL_FORMAT_NONE = 0,
    /** RGB565 pixel format */
    IMAGE_PIXEL_FORMAT_RGB565 = 101,
    /** ARGB555 pixel format */
    IMAGE_PIXEL_FORMAT_ARGB1555,
    /** RGB888 pixel format */
    IMAGE_PIXEL_FORMAT_RGB888,
    /** ARGB8888 pixel format */
    IMAGE_PIXEL_FORMAT_ARGB8888,
    /** YUYV pixel format */
    IMAGE_PIXEL_FORMAT_YUYV = 201,
    /** YVYU pixel format */
    IMAGE_PIXEL_FORMAT_YVYU,
    /** UYVY pixel format */
    IMAGE_PIXEL_FORMAT_UYVY,
    /** VYUY pixel format */
    IMAGE_PIXEL_FORMAT_VYUY,
    /** AYUV pixel format */
    IMAGE_PIXEL_FORMAT_AYUV,
    /** YUV410 pixel format */
    IMAGE_PIXEL_FORMAT_YUV410,
    /** YVU410 pixel format */
    IMAGE_PIXEL_FORMAT_YVU410,
    /** YUV411 pixel format */
    IMAGE_PIXEL_FORMAT_YUV411,
    /** YVU411 pixel format */
    IMAGE_PIXEL_FORMAT_YVU411,
    /** YUV420 pixel format */
    IMAGE_PIXEL_FORMAT_YUV420,
    /** YVU420 pixel format */
    IMAGE_PIXEL_FORMAT_YVU420,
    /** YUV422 pixel format */
    IMAGE_PIXEL_FORMAT_YUV422,
    /** YVU422 pixel format */
    IMAGE_PIXEL_FORMAT_YVU422,
    /** YUV444 pixel format */
    IMAGE_PIXEL_FORMAT_YUV444,
    /** YVU444 pixel format */
    IMAGE_PIXEL_FORMAT_YVU444,
    /** NV12 pixel format */
    IMAGE_PIXEL_FORMAT_NV12 = 301,
    /** NV21 pixel format */
    IMAGE_PIXEL_FORMAT_NV21,
    /** NV16 pixel format */
    IMAGE_PIXEL_FORMAT_NV16,
    /** NV61 pixel format */
    IMAGE_PIXEL_FORMAT_NV61
};

class PixelFormatUtils {
public:
    static bool BppOfPixelFormat(ImagePixelFormat pixelFormat, int16_t& bpp);
    static uint16_t ARGB8888ToARGB1555(uint32_t color);
    static uint32_t ARGB1555ToARGB8888(uint16_t color);
};
} // namespace OHOS
#endif
