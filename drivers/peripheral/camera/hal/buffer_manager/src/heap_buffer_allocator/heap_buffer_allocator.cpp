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

#include "heap_buffer_allocator.h"
#include "image_buffer.h"

namespace OHOS::Camera {
HeapBufferAllocator::HeapBufferAllocator()
{
    CAMERA_LOGD("buffer allocator construct, instance = %{public}p", this);
}

HeapBufferAllocator::~HeapBufferAllocator() {}

RetCode HeapBufferAllocator::Init()
{
    return RC_OK;
}

std::shared_ptr<IBuffer> HeapBufferAllocator::AllocBuffer(const uint32_t width,
                                                          const uint32_t height,
                                                          const uint64_t usage,
                                                          const uint32_t format)
{
    uint32_t size = CalculateSize(width, height, usage, format);
    char* heap = nullptr;
    if (size > 0) {
        heap = new (std::nothrow) char[size];
    }
    if (heap == nullptr) {
        CAMERA_LOGE("Alloc buffer failed");
        return nullptr;
    }
    std::shared_ptr<IBuffer> buffer = std::make_shared<ImageBuffer>(sourceType_);
    if (buffer != nullptr) {
        buffer->SetSize(size);
        buffer->SetUsage(usage);
        buffer->SetVirAddress(heap);
        buffer->SetStride(width);
        buffer->SetWidth(width);
        buffer->SetHeight(height);
        buffer->SetFormat(format);
        CAMERA_LOGD("Alloc buffer succeed to shared memory segment size:%{public}d.", size);
    } else {
        delete[] heap;
        heap = nullptr;
        CAMERA_LOGE("Alloc buffer failed to shared memory segment.");
    }
    return buffer;
}

RetCode HeapBufferAllocator::FreeBuffer(std::shared_ptr<IBuffer>& buffer)
{
    if (buffer->GetSourceType() != sourceType_) {
        return RC_ERROR;
    }

    char* addr = reinterpret_cast<char*>(buffer->GetVirAddress());
    if (addr != nullptr) {
        delete[] addr;
        addr = nullptr;
    }
    buffer->Free();
    return RC_OK;
}

RetCode HeapBufferAllocator::MapBuffer(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

RetCode HeapBufferAllocator::UnmapBuffer(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

RetCode HeapBufferAllocator::FlushCache(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

RetCode HeapBufferAllocator::InvalidateCache(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

uint32_t HeapBufferAllocator::CalculateSize(const uint32_t width,
                                            const uint32_t height,
                                            const uint64_t usage,
                                            const uint32_t format) const
{
    (void)usage;
    switch (format) {
        case CAMERA_FORMAT_RGB_565:
        case CAMERA_FORMAT_RGBA_5658:
        case CAMERA_FORMAT_RGBX_4444:
        case CAMERA_FORMAT_RGBA_4444:
        case CAMERA_FORMAT_RGB_444:
        case CAMERA_FORMAT_RGBX_5551:
        case CAMERA_FORMAT_RGBA_5551:
        case CAMERA_FORMAT_RGB_555:
        case CAMERA_FORMAT_RGBX_8888:
        case CAMERA_FORMAT_RGBA_8888:
        case CAMERA_FORMAT_RGB_888:
        case CAMERA_FORMAT_BGR_565:
        case CAMERA_FORMAT_BGRX_4444:
        case CAMERA_FORMAT_BGRA_4444:
        case CAMERA_FORMAT_BGRX_5551:
        case CAMERA_FORMAT_BGRA_5551:
        case CAMERA_FORMAT_BGRX_8888:
        case CAMERA_FORMAT_BGRA_8888:
            break;
        case CAMERA_FORMAT_YCBCR_420_SP:
        case CAMERA_FORMAT_YCRCB_420_SP:
        case CAMERA_FORMAT_YCRCB_422_P:
        case CAMERA_FORMAT_YCBCR_420_P:
        case CAMERA_FORMAT_YCRCB_420_P:
        /*
            Fixed calculation formula of yuv
            yuv420 size= w * h * 3 / 2
        */
            return width * height * 3 / 2; // 3:Fixed calculated value of yuv 2:Fixed calculated value of yuv
            break;
        case CAMERA_FORMAT_YCBCR_422_P:
        case CAMERA_FORMAT_YUV_422_I:
        case CAMERA_FORMAT_YCBCR_422_SP:
        case CAMERA_FORMAT_YCRCB_422_SP:
        case CAMERA_FORMAT_YUYV_422_PKG:
        case CAMERA_FORMAT_UYVY_422_PKG:
        case CAMERA_FORMAT_YVYU_422_PKG:
        case CAMERA_FORMAT_VYUY_422_PKG:
        /*
            Fixed calculation formula of yuv
            yuv422 size= w * h * 2
        */
            return width * height * 2; // 2:Fixed calculated value of yuv
            break;
        default:
            break;
    }
    return 0;
}
REGISTER_BUFFER_ALLOCATOR(HeapBufferAllocator, CAMERA_BUFFER_SOURCE_TYPE_HEAP);
} // namespace OHOS::Camera

