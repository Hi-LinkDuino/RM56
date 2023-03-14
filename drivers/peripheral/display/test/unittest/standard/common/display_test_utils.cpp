/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <cmath>
#include <vector>
#include "display_test.h"
#include "display_type.h"
#include "buffer_handle.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
constexpr uint8_t BITS_PER_BYTE = 8;

static uint32_t BGRAToRGBA(uint32_t bgra)
{
    uint32_t rgba = 0;
    const int32_t twoByteOffset = 16;

    rgba |= (bgra & 0x0000ff00) << twoByteOffset; // get red then move to rgba
    rgba |= (bgra & 0x00ff0000);                  // get green
    rgba |= (bgra & 0xff000000) >> twoByteOffset; // get blue then move to rgba
    rgba |= (bgra & 0x000000ff);                  // get alpha
    return rgba;
}

static int32_t GetPixelFormatBpp(PixelFormat format)
{
    const int32_t bppRgba8888 = 32;
    switch (format) {
        case PIXEL_FMT_RGBA_8888:
            return bppRgba8888;
        case PIXEL_FMT_BGRA_8888:
            return bppRgba8888;
        default:
            return -1;
    }
}

void SaveFile(const char *fileName, uint8_t *data, int size)
{
    int fileFd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    int hasWriten = write(fileFd, data, size);
    DISPLAY_TEST_LOGD("SaveFile hasWriten %d", hasWriten);
    close(fileFd);
}

static uint32_t ConverToRGBA(PixelFormat fmt, uint32_t color)
{
    switch (fmt) {
        case PIXEL_FMT_BGRA_8888:
            return BGRAToRGBA(color);
        case PIXEL_FMT_RGBA_8888:
            return color;
        default:
            DISPLAY_TEST_LOGE("the fmt can not covert %d", fmt);
    }
    return color;
}

uint32_t GetPixelValue(const BufferHandle &handle, int x, int y)
{
    const int32_t pixelBytes = 4;
    int32_t bpp = GetPixelFormatBpp((PixelFormat)handle.format);
    DISPLAY_TEST_CHK_RETURN((bpp <= 0), 0, DISPLAY_TEST_LOGE("CheckPixel do not support format %d", handle.format));
    DISPLAY_TEST_CHK_RETURN((handle.virAddr == nullptr), 0,
        DISPLAY_TEST_LOGE("CheckPixel viraddr is null must map it"));
    DISPLAY_TEST_CHK_RETURN((x < 0 || x >= handle.width), 0,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter x:%d width:%d", x, handle.width));
    DISPLAY_TEST_CHK_RETURN((y < 0 || y >= handle.height), 0,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter y:%d height:%d", y, handle.height));

    int32_t position = y * handle.width + x;
    if ((position * pixelBytes) > handle.size) {
        DISPLAY_TEST_LOGE("the pixel postion outside\n");
    }
    uint32_t *pixel = reinterpret_cast<uint32_t *>(handle.virAddr) + position;
    return *pixel;
}

uint32_t GetUint32(uint32_t value)
{
    uint32_t dst;
    uint8_t *data = reinterpret_cast<uint8_t *>(&dst);
    for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
        *(data + i) = (value >> ((sizeof(uint32_t) - i - 1) * BITS_PER_BYTE)) & 0xff;
    }
    return dst;
}

uint32_t CheckPixel(const BufferHandle &handle, int x, int y, uint32_t color)
{
    const int32_t pixelBytes = 4;
    int32_t bpp = GetPixelFormatBpp((PixelFormat)handle.format);
    DISPLAY_TEST_CHK_RETURN((bpp <= 0), 0, DISPLAY_TEST_LOGE("CheckPixel do not support format %d", handle.format));
    DISPLAY_TEST_CHK_RETURN((handle.virAddr == nullptr), 0,
        DISPLAY_TEST_LOGE("CheckPixel viraddr is null must map it"));
    DISPLAY_TEST_CHK_RETURN((x < 0 || x >= handle.width), 0,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter x:%d width:%d", x, handle.width));
    DISPLAY_TEST_CHK_RETURN((y < 0 || y >= handle.height), 0,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter y:%d height:%d", y, handle.height));

    int32_t position = y * handle.width + x;
    if ((position * pixelBytes) > handle.size) {
        DISPLAY_TEST_LOGE("the pixel postion outside\n");
    }
    uint32_t *pixel = reinterpret_cast<uint32_t *>(handle.virAddr) + position;
    uint32_t checkColor = ConverToRGBA(static_cast<PixelFormat>(handle.format), GetUint32(*pixel));
    if (checkColor != color) {
        DISPLAY_TEST_LOGD("the pixel color not match vAddr:%p position:%d pixel:%08x color:%08x", handle.virAddr,
            position, checkColor, color);
        DISPLAY_TEST_LOGD("x:%d y:%d width:%d", x, y, handle.width);
        SaveFile("/data/display_test_bitmap_", static_cast<uint8_t *>(handle.virAddr), handle.size);
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

void SetUint32(uint32_t &dst, uint32_t value)
{
    constexpr uint8_t BITS_PER_BYTE = 8;
    uint8_t *data = reinterpret_cast<uint8_t *>(&dst);
    for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
        *(data + i) = (value >> ((sizeof(uint32_t) - i - 1) * BITS_PER_BYTE)) & 0xff;
    }
}

void SetPixel(const BufferHandle &handle, int x, int y, uint32_t color)
{
    constexpr int32_t pixelBytes = 4;
    constexpr int32_t bpp = 32;
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((bpp <= 0),
        DISPLAY_TEST_LOGE("CheckPixel do not support format %d", handle.format));
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((handle.virAddr == nullptr),
        DISPLAY_TEST_LOGE("CheckPixel viraddr is null must map it"));
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((x < 0 || x >= handle.width),
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter x:%d width:%d", x, handle.width));
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((y < 0 || y >= handle.height),
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter y:%d height:%d", y, handle.height));

    int32_t position = y * handle.width + x;
    if ((position * pixelBytes) > handle.size) {
        DISPLAY_TEST_LOGE("the pixel postion outside\n");
    }
    uint32_t *pixel = reinterpret_cast<uint32_t *>(handle.virAddr) + position;
    SetUint32(*pixel, color);
}

void ClearColor(const BufferHandle &handle, uint32_t color)
{
    for (int32_t x = 0; x < handle.width; x++) {
        for (int32_t y = 0; y < handle.height; y++) {
            SetPixel(handle, x, y, color);
        }
    }
}

void ClearColorRect(const BufferHandle &handle, uint32_t color, IRect &rect)
{
    DISPLAY_TEST_LOGD("x %d, y %d w %d h %d color %x ", rect.x, rect.y, rect.w, rect.h, color);
    for (int32_t x = 0; x < rect.w; x++) {
        for (int32_t y = 0; y < rect.h; y++) {
            SetPixel(handle, x + rect.x, y + rect.y, color);
        }
    }
}

std::vector<IRect> SplitBuffer(const BufferHandle &handle, std::vector<uint32_t> &colors)
{
    std::vector<IRect> splitRects;
    if (colors.empty()) {
        DISPLAY_TEST_LOGD("the colors empty");
    }
    const uint32_t rowNum = sqrt(colors.size());
    const uint32_t colNum = rowNum;
    if (colNum == 0) {
        DISPLAY_TEST_LOGD("rowNum and colNum are zero");
        return splitRects;
    }
    const uint32_t cellWidth = handle.width / rowNum;
    const uint32_t cellHeight = handle.height / colNum;
    IRect rect = { 0, 0, cellWidth, cellHeight };
    DISPLAY_TEST_LOGD("rowNum %u, colNum %u cellWidth %u cellHeight %u", rowNum, colNum, cellWidth, cellHeight);
    uint32_t count = 0;
    for (uint32_t x = 0; x < rowNum; x++) {
        for (uint32_t y = 0; y < colNum; y++) {
            rect.x = x * cellWidth;
            rect.y = y * cellHeight;
            ClearColorRect(handle, colors[count++], rect);
            splitRects.push_back(rect);
        }
    }
    SaveFile("/data/splitbuffer_data_", static_cast<uint8_t *>(handle.virAddr), handle.size);
    return splitRects;
}
} // OHOS
} // HDI
} // DISPLAY
} // TEST
