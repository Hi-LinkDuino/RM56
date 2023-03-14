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

#include "imgdecode/image_load.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"

namespace {
const uint8_t BITMAP_ZIP_LEN = 3;
const uint8_t BITMAP_MID_BIT = 1;
const uint8_t BITMAP_LOW_BIT = 2;
const uint32_t BITMAP_ZIP24_FLAG = 0x456789;
const uint32_t BITMAP_ZIP_FLAG = 0x23456789;
const uint32_t BITMAP_ALPHA_MASK = 0xFF000000;
const uint32_t BITMAP_MAXCON_PIXNUM = 0xCB100;
const uint32_t MOVE_HIGH = 16;
const uint32_t MOVE_LOW = 8;
} // namespace

namespace OHOS {
bool ImageLoad::CreateImage(ImageInfo& imageInfo)
{
    uint32_t bytePerPixel = 4;
    ImageHeader& imageHeader = imageInfo.header;

    switch (imageHeader.colorMode) {
        case ARGB8888:
            bytePerPixel = 4; // 4 bytes per pixel
            break;
        case RGB888:
            bytePerPixel = 3; // 3 bytes per pixel
            break;
        case RGB565:
            bytePerPixel = 2; // 2 bytes per pixel
            break;
        default:
            GRAPHIC_LOGE("CreateImage invalid colorMode.");
            return false;
    }

    imageInfo.dataSize = imageHeader.width * imageHeader.height * bytePerPixel;
    imageInfo.data = static_cast<uint8_t*>(ImageCacheMalloc(imageInfo));
    if (imageInfo.data == nullptr) {
        GRAPHIC_LOGE("ImageCacheMalloc error.");
        return false;
    }

    return true;
}

bool ImageLoad::UncompressImageInZip(ImageInfo& imageInfo, uint8_t* buffer, uint32_t size)
{
    if (!CreateImage(imageInfo)) {
        GRAPHIC_LOGE("Create image error.");
        return false;
    }

    if (imageInfo.header.colorMode == RGB888) {
        return Unzip24Image(buffer, size, imageInfo);
    } else {
        return UnzipImage(buffer, size, imageInfo);
    }
}

bool ImageLoad::UnzipImage(uint8_t* imageBuffer, uint32_t size, ImageInfo& imageInfo)
{
    uint32_t value = 0;
    uint32_t count = 0;

    if ((imageBuffer == nullptr) || (size == 0)) {
        GRAPHIC_LOGE("imageHeader is null.");
        return false;
    }

    uint32_t* source = reinterpret_cast<uint32_t*>(imageBuffer);
    uint32_t* sourceEnd = reinterpret_cast<uint32_t*>(imageBuffer + size);
    uint32_t* dest = nullptr;
    uint32_t* destEnd = nullptr;

    dest = reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(imageInfo.data));
    destEnd = reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(imageInfo.data) + imageInfo.dataSize);

    while ((source < sourceEnd) && (dest < destEnd)) {
        if (*source != BITMAP_ZIP_FLAG) {
            *dest++ = *source++;
        } else {
            source++;
            value = *source++;
            count = *source++;
            if (destEnd < count + dest) {
                break;
            }

            while (count--) {
                *dest++ = value;
            }
        }
    }

    if (dest == destEnd) {
        return true;
    }
    ImageCacheFree(imageInfo);
    imageInfo.data = nullptr;
    return false;
}

bool ImageLoad::Unzip24Image(uint8_t* imageBuffer, uint32_t size, ImageInfo& imageInfo)
{
    if ((imageBuffer == nullptr) || (size == 0)) {
        GRAPHIC_LOGE("imageHeader is null.");
        return false;
    }

    uint8_t* source = reinterpret_cast<uint8_t*>(imageBuffer);
    uint8_t* sourceEnd = reinterpret_cast<uint8_t*>(imageBuffer + size);
    uint32_t* dest = reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(imageInfo.data));
    uint32_t* destEnd = reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(imageInfo.data) + imageInfo.dataSize);
    while ((source < sourceEnd) && (dest < destEnd)) {
        uint32_t count = 0;
        uint32_t value = 0;

        // Little endian
        value = ((*source)) + (*(source + BITMAP_MID_BIT) << MOVE_LOW) + (*(source + BITMAP_LOW_BIT) << MOVE_HIGH);
        source = source + BITMAP_ZIP_LEN;
        if (value != BITMAP_ZIP24_FLAG) {
            *dest = value | BITMAP_ALPHA_MASK;
            dest++;
        } else {
            value = 0;
            value = ((*source)) + (*(source + BITMAP_MID_BIT) << MOVE_LOW) + (*(source + BITMAP_LOW_BIT) << MOVE_HIGH);
            source = source + BITMAP_ZIP_LEN;

            count = 0;
            count = ((*source)) + (*(source + BITMAP_MID_BIT) << MOVE_LOW) + (*(source + BITMAP_LOW_BIT) << MOVE_HIGH);
            source = source + BITMAP_ZIP_LEN;

            if (count > BITMAP_MAXCON_PIXNUM) {
                *dest = BITMAP_ZIP24_FLAG | BITMAP_ALPHA_MASK;
                dest++;
                *dest = value | BITMAP_ALPHA_MASK;
                dest++;
                *dest = count | BITMAP_ALPHA_MASK;
                dest++;
                continue;
            }
            if (static_cast<uintptr_t>(destEnd - dest) < static_cast<uintptr_t>(count)) {
                break;
            }
            while (count--) {
                *dest = value | BITMAP_ALPHA_MASK;
                dest++;
            }
        }
    }

    if (dest == destEnd) {
        return true;
    }
    ImageCacheFree(imageInfo);
    imageInfo.data = nullptr;
    return false;
}

bool ImageLoad::UnZip2ImageInfo(ImageInfo& imageInfo, uint8_t* buffer, uint32_t size)
{
    switch (imageInfo.header.compressMode) {
        case COMPRESS_MODE__ZIP_ALG:
            return UncompressImageInZip(imageInfo, buffer, size);
        default:
            return false;
    }
}

bool ImageLoad::GetImageInfo(int32_t fd, uint32_t size, ImageInfo& imageInfo)
{
    if (size == 0) {
        return false;
    }

    uint8_t* buffer = reinterpret_cast<uint8_t*>(UIMalloc(size));
    if (buffer == nullptr) {
        return false;
    }

    if (read(fd, buffer, size) != static_cast<int32_t>(size)) {
        UIFree(buffer);
        GRAPHIC_LOGE("SeekImageFile error.");
        return false;
    }
    bool ret = UnZip2ImageInfo(imageInfo, buffer, size);
    UIFree(buffer);
    return ret;
}
} // namespace OHOS
