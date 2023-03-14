/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGIN_COMMON_VIDEO_TAGS_H
#define HISTREAMER_PLUGIN_COMMON_VIDEO_TAGS_H

#include <cstdint> // NOLINT: using uint32_t

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @enum Video Pixel Format.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct VideoPixelFormat : uint32_t {
    UNKNOWN,
    YUV410P,   ///< planar YUV 4:1:0, 1 Cr & Cb sample per 4x4 Y samples
    YUV411P,   ///< planar YUV 4:1:1, 1 Cr & Cb sample per 4x1 Y samples
    YUV420P,   ///< planar YUV 4:2:0, 1 Cr & Cb sample per 2x2 Y samples
    NV12,      ///< semi-planar YUV 4:2:0, UVUV...
    NV21,      ///< semi-planar YUV 4:2:0, VUVU...
    YUYV422,   ///< packed YUV 4:2:2, Y0 Cb Y1 Cr
    YUV422P,   ///< planar YUV 4:2:2, 1 Cr & Cb sample per 2x1 Y samples
    YUV444P,   ///< planar YUV 4:4:4, 1 Cr & Cb sample per 1x1 Y samples
    RGBA,      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
    RGB24,     ///< packed RGB 8:8:8, RGBRGB...
    BGR24,     ///< packed RGB 8:8:8, BGRBGR...
    PAL8,      ///< 8 bit with AV_PIX_FMT_RGB32 palette
    GRAY8,     ///< Y
    MONOWHITE, ///< Y, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
    MONOBLACK, ///< Y, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
    YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG)
    YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG)
    YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG)
};

/**
 * @enum Video H264/AVC profile.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct VideoH264Profile : uint32_t {
    UNKNOWN,
    BASELINE,  ///< Baseline profile
    MAIN,      ///< Main profile
    EXTENDED,  ///< Extended profile
    HIGH,      ///< High profile
    HIGH10,    ///< High 10 profile
    HIGH422,   ///< High 4:2:2 profile
    HIGH444,   ///< High 4:4:4 profile
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_COMMON_VIDEO_TAGS_H
