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

#ifndef GRAPHIC_LITE_IMAGE_LOAD_H
#define GRAPHIC_LITE_IMAGE_LOAD_H

#include "gfx_utils/image_info.h"

namespace OHOS {
enum CompressMode : uint8_t {
    COMPRESS_MODE_NONE = 0,
    COMPRESS_MODE__ZIP_ALG,
    COMPRESS_MODE_BITMAP_ALG,
    COMPRESS_MODE_BLOCK_ALG,
};

class ImageLoad {
public:
    static bool GetImageInfo(int32_t fd, uint32_t size, ImageInfo& imageInfo);

private:
    ImageLoad() = delete;
    ~ImageLoad() = delete;

    static bool UnZip2ImageInfo(ImageInfo& imageInfo, uint8_t* buffer, uint32_t size);
    static bool UncompressImageInZip(ImageInfo& imageInfo, uint8_t* buffer, uint32_t size);
    static bool CreateImage(ImageInfo& imageInfo);
    static bool Unzip24Image(uint8_t* imageBuffer, uint32_t size, ImageInfo& imageInfo);
    static bool UnzipImage(uint8_t* imageBuffer, uint32_t size, ImageInfo& imageInfo);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_IMAGE_LOAD_H
