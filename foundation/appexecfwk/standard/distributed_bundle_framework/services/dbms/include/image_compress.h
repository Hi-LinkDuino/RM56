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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_COMPRESS_H
#define FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_COMPRESS_H

#include <stdio.h>
#include <cstdlib>
#include <string>
#include <cmath>

#include "png.h"
#include "jpeglib.h"
#include "image_buffer.h"

namespace OHOS {
namespace AppExecFwk {
class ImageCompress {
public:
    ImageCompress() = default;
    ~ImageCompress();
    double CalRatio(std::string fileName);
    bool NeedCompress(std::string fileName);
    bool InitPngFile(std::shared_ptr<ImageBuffer>& imageBuffer, png_structp& png, png_infop& info);
    int32_t DecodePngFile(std::string fileName, std::shared_ptr<ImageBuffer>& imageBuffer);
    int32_t EncodePngFile(std::shared_ptr<ImageBuffer>& imageBuffer);
    int32_t ResizeRGBAImage(std::shared_ptr<ImageBuffer>& imageBufferIn, std::shared_ptr<ImageBuffer>& imageBufferOut);
    int32_t DecodeJPGFile(std::string fileName, std::shared_ptr<ImageBuffer>& imageBuffer);
    int32_t EncodeJPGFile(std::shared_ptr<ImageBuffer>& imageBuffer);
    int32_t ResizeRGBImage(std::shared_ptr<ImageBuffer>& imageBufferIn, std::shared_ptr<ImageBuffer>& imageBufferOut);
    std::shared_ptr<ImageBuffer> CompressImage(std::string inFileName);
    void ReleasePngPointer(png_bytepp& rowPointers, uint32_t height);
    bool MallocPngPointer(png_bytepp& rowPointers, uint32_t height, uint32_t strides);
    bool DoubleEqual(double left, double right);
    bool IsPathValid(std::string& fileName);
    static void PngToBuffer(png_structp png_ptr, png_bytep data, png_size_t lenght);
};
}
}

#endif // FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_COMPRESS_H