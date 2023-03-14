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
#include<string>
#include "image_buffer.h"

#include "app_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace AppExecFwk {
ImageBuffer::~ImageBuffer()
{
    APP_LOGI("ImageBuffer: ~ImageBuffer");
}

bool ImageBuffer::MallocImageMap(int components)
{
    if (width <= 0 || height <= 0) {
        APP_LOGE("ImageBuffer: get image width and height first");
        return false;
    }
    uint32_t rowStride = width * components;
    imageDataPointer = std::make_unique<unsigned char[]>(rowStride * height);
    return true;
}

int32_t ImageBuffer::SetCompressData(unsigned char* buffer, int32_t length)
{
    compressDataBuffer = std::make_unique<unsigned char[]>(length);
    unsigned char* dataBuffer = compressDataBuffer.get();
    if (memcpy_s(dataBuffer, length, buffer, length) != EOK) {
        return -1;
    }
    return length;
}
}
}