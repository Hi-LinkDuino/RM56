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

#include "gfx_utils/file.h"
#include "gfx_utils/mem_api.h"
#include "imgdecode/file_img_decoder.h"
#include "imgdecode/image_load.h"
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_types.h"
#include "../draw/draw_utils.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
FileImgDecoder& FileImgDecoder::GetInstance()
{
    static FileImgDecoder instance;
    return instance;
}

RetCode FileImgDecoder::Open(ImgResDsc& dsc)
{
#ifdef _WIN32
    int32_t fd = open(dsc.path, O_RDONLY | O_BINARY);
#else
    int32_t fd = open(dsc.path, O_RDONLY);
#endif
    if (fd == -1) {
        return RetCode::FAIL;
    }
    dsc.fd = fd;

    dsc.imgInfo.data = nullptr;
    dsc.inCache_ = false;
    uint8_t colorMode = dsc.imgInfo.header.colorMode;
    if (IsImgValidMode(colorMode)) {
        return RetCode::OK;
    } else {
        return RetCode::FAIL;
    }
}

RetCode FileImgDecoder::Close(ImgResDsc& dsc)
{
    if (dsc.imgInfo.data != nullptr) {
        ImageCacheFreeAlignment(dsc.imgInfo);
        // ImageCacheFree(dsc.imgInfo);
        dsc.imgInfo.data = nullptr;
    }
    if (dsc.fd && (dsc.fd != -1)) {
        close(dsc.fd);
        dsc.fd = -1;
    }

    return RetCode::OK;
}

RetCode FileImgDecoder::GetHeader(ImgResDsc& dsc)
{
    int32_t fd;
    int32_t readCount;
#ifdef _WIN32
    fd = open(dsc.path, O_BINARY);
#else
    fd = open(dsc.path, O_RDONLY);
#endif
    if (fd == -1) {
        return RetCode::FAIL;
    }

    readCount = read(fd, &dsc.imgInfo.header, sizeof(ImageHeader));
    close(fd);
    dsc.fd = -1;
    if (readCount != sizeof(ImageHeader)) {
        dsc.imgInfo.header.width = 0;
        dsc.imgInfo.header.height = 0;
        dsc.imgInfo.header.colorMode = UNKNOWN;
        return RetCode::FAIL;
    }

    return RetCode::OK;
}

RetCode FileImgDecoder::ReadLine(ImgResDsc& dsc, const Point& start, int16_t len, uint8_t* buf)
{
    if (IsImgValidMode(dsc.imgInfo.header.colorMode)) {
        return ReadLineTrueColor(dsc, start, len, buf);
    }
    return RetCode::FAIL;
}

RetCode FileImgDecoder::ReadToCache(ImgResDsc& dsc)
{
    struct stat info;
    if (!dsc.inCache_) {
        lseek(dsc.fd, 0, SEEK_SET);
        int32_t readCount = read(dsc.fd, &dsc.imgInfo.header, sizeof(ImageHeader));
        if (readCount != sizeof(ImageHeader)) {
            return RetCode::FAIL;
        }

        int32_t ret = fstat(dsc.fd, &info);
        if (ret != 0) {
            return RetCode::FAIL;
        }
        uint32_t pxCount = info.st_size - readCount;
        if (dsc.imgInfo.data != nullptr) {
            ImageCacheFreeAlignment(dsc.imgInfo);
            dsc.imgInfo.data = nullptr;
        }

        bool readSuccess = false;
        if (dsc.imgInfo.header.compressMode != COMPRESS_MODE_NONE) {
            readSuccess = ImageLoad::GetImageInfo(dsc.fd, pxCount, dsc.imgInfo);
        } else {
            dsc.imgInfo.dataSize = pxCount;
            dsc.imgInfo.data = reinterpret_cast<uint8_t*>(ImageCacheMalloc(dsc.imgInfo, ALIGN_ADDRESS_NUM));
            if (dsc.imgInfo.data == nullptr) {
                //2023年4月12日：已经失败了，应该真实地反映到外部
                //return RetCode::OK;
                return RetCode::FAIL;
            }
            uint8_t* tmp = const_cast<uint8_t*>(dsc.imgInfo.data);
            readSuccess = (static_cast<int32_t>(pxCount) == read(dsc.fd, reinterpret_cast<void*>(tmp), pxCount));
            if(readSuccess){
                ImageHeader imageHeader = dsc.imgInfo.header;
                uint16_t imageWidth = imageHeader.width;
                uint16_t imageHeight = imageHeader.height;
                if(dsc.imgInfo.header.colorMode==L8){
                    uint32_t indexLength = imageWidth*imageHeight;
                    uint16_t clutCount = pxCount - indexLength;
                    uint8_t* clut = (uint8_t*)malloc(clutCount);
                    memset(clut,0,clutCount);
                    uint8_t* clutIndex = tmp+indexLength;
                    memcpy(clut,clutIndex,clutCount);
                    dsc.imgInfo.userData = clut;
                }
            }
        }

        if (!readSuccess) {
            ImageCacheFreeAlignment(dsc.imgInfo);
            dsc.imgInfo.data = nullptr;
            dsc.imgInfo.dataSize = 0;
            close(dsc.fd);
            dsc.fd = -1;
            //2023年4月12日：已经失败了，应该真实地反映到外部
            //return RetCode::OK;
            return RetCode::FAIL;
        }
        dsc.inCache_ = true;
        close(dsc.fd);
        dsc.fd = -1;
    }

    return RetCode::OK;
}

RetCode FileImgDecoder::ReadLineTrueColor(ImgResDsc& dsc, const Point& start, int16_t len, uint8_t* buf)
{
    uint8_t pxSizeInBit = DrawUtils::GetPxSizeByColorMode(dsc.imgInfo.header.colorMode);
    off_t res;

    uint32_t pos = ((start.y * dsc.imgInfo.header.width + start.x) * pxSizeInBit) >> BYTE_TO_BIT_SHIFT;
    pos += sizeof(ImageHeader); /* Skip the header */
    res = lseek(dsc.fd, pos, SEEK_SET);
    if (res == -1) {
        return RetCode::FAIL;
    }
    uint32_t btr = len * (pxSizeInBit >> BYTE_TO_BIT_SHIFT);
    int32_t br = read(dsc.fd, buf, btr);
    if ((br == -1) || (btr != static_cast<uint32_t>(br))) {
        return RetCode::FAIL;
    }

    return RetCode::OK;
}
} // namespace OHOS
