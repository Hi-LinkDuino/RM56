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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_BUFFER_H
#define FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_BUFFER_H

#include <cstring>

namespace OHOS {
namespace AppExecFwk {
typedef unsigned char ImageData;
typedef unsigned char* ImageRow;
typedef unsigned char** ImageMap;

class ImageBuffer {
public:
    ImageBuffer() = default;
    ~ImageBuffer();
    bool MallocImageMap(int components);
    uint32_t GetWidth() const
    {
        return width; 
    }
    void SetWidth(uint32_t width)
    {
        this->width = width;
    }
    uint32_t GetHeight() const
    {
        return height;
    }
    void SetHeight(uint32_t height)
    {
        this->height = height;
    }
    uint32_t GetFileSize() const
    {
        return fileSize;
    }
    void SetFileSize(uint32_t fileSize)
    {
        this->fileSize = fileSize;
    }
    uint32_t GetComponents() const
    {
        return components;
    }
    void SetComponents(uint32_t components)
    {
        this->components = components;
    }
    uint32_t GetPngComponents() const
    {
        return pngComponents;
    }
    void SetPngComponents(uint32_t pngComponents)
    {
        this->pngComponents = pngComponents;
    }
    double GetRatio() const
    {
        return ratio;
    }
    void SetRatio(double ratio)
    {
        this->ratio = ratio;
    }
    ImageData GetColorType() const
    {
        return colorType;
    }
    void SetColorType(ImageData colorType)
    {
        this->colorType = colorType;
    }
    ImageData GetBitDepth() const
    {
        return bitDepth;
    }
    void SetBitDepth(ImageData bitDepth)
    {
        this->bitDepth = bitDepth;
    }
    std::unique_ptr<unsigned char[]>& GetImageDataPointer()
    { 
        return imageDataPointer;
    }
    void SetImageType(const std::string& type)
    {
        imageType = type;
    }
    const std::string& GetImageType() const
    {
        return imageType;
    }
    std::unique_ptr<unsigned char[]>& GetCompressDataBuffer()
    {
        return compressDataBuffer;
    }
    int32_t GetCompressSize() const
    {
        return compressSize;
    }
    void SetCompressSize(int32_t compressSize)
    {
        this->compressSize = compressSize;
    }
    int32_t SetCompressData(unsigned char* buffer, int32_t length); 

private:
    uint32_t width;
    uint32_t height;
    uint32_t fileSize;
    uint32_t components;
    uint32_t pngComponents;
    double ratio;
    ImageData colorType;
    ImageData bitDepth;
    std::unique_ptr<unsigned char[]> imageDataPointer = nullptr;
    std::unique_ptr<unsigned char[]> compressDataBuffer = nullptr;
    std::string imageType;
    int32_t compressSize;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_IMAGE_BUFFER_H