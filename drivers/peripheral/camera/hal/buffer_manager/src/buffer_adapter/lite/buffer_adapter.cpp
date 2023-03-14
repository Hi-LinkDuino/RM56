
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

#include "buffer_adapter.h"

namespace OHOS::Camera {
RetCode BufferAdapter::SurfaceBufferToCameraBuffer(const OHOS::SurfaceBuffer* surfaceBuffer,
    const std::shared_ptr<OHOS::Surface>& surface,
    const std::shared_ptr<IBuffer>& buffer)
{
    CHECK_IF_EQUAL_RETURN_VALUE(buffer == nullptr || surfaceBuffer == nullptr, true, RC_ERROR);
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(buffer->GetSourceType(),
        CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL, RC_ERROR);

    buffer->SetPhyAddress(surfaceBuffer->GetPhyAddr());
    buffer->SetStride(surface->GetStride());
    buffer->SetWidth(surface->GetWidth());
    buffer->SetHeight(surface->GetHeight());
    int32_t format = static_cast<int32_t>(PixelFormatToCameraFormat(surface->GetFormat()));
    buffer->SetFormat(format);
    buffer->SetUsage(CameraUsageToGrallocUsage(surface->GetUsage()));
    buffer->SetSize(static_cast<uint32_t>(surfaceBuffer->GetSize()));
    if (surfaceBuffer->GetVirAddr() != nullptr) {
        buffer->SetVirAddress(surfaceBuffer->GetVirAddr());
    }

    return RC_OK;
}

RetCode BufferAdapter::SetExtInfoToSurfaceBuffer(const std::shared_ptr<IBuffer>& buffer,
    const std::shared_ptr<OHOS::SurfaceBuffer>& surfaceBuffer)
{
    CHECK_IF_EQUAL_RETURN_VALUE(buffer == nullptr || surfaceBuffer == nullptr, true, RC_ERROR);
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(buffer->GetSourceType(),
        CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL, RC_ERROR);

    EsFrameInfo info = buffer->GetEsFrameInfo();
    if (info.size != -1) {
        surfaceBuffer->SetInt32(FRAME_SIZE_TAG, info.size);
    }
    return RC_OK;
}

uint32_t BufferAdapter::PixelFormatToCameraFormat(const uint32_t format)
{
    uint32_t cameraFormat = CAMERA_FORMAT_INVALID;
    switch (format) {
        case IMAGE_PIXEL_FORMAT_RGB565:
            cameraFormat = CAMERA_FORMAT_RGB_565;
            break;
        case IMAGE_PIXEL_FORMAT_ARGB1555:
            cameraFormat = CAMERA_FORMAT_RGBA_5551;
            break;
        case IMAGE_PIXEL_FORMAT_ARGB8888:
            cameraFormat = CAMERA_FORMAT_RGBA_8888;
            break;
        case IMAGE_PIXEL_FORMAT_NV12:
            cameraFormat = CAMERA_FORMAT_YCBCR_420_SP;
            break;
        case IMAGE_PIXEL_FORMAT_NV21:
            cameraFormat = CAMERA_FORMAT_YCRCB_420_SP;
            break;
        case IMAGE_PIXEL_FORMAT_YUV420:
            cameraFormat = CAMERA_FORMAT_YCBCR_420_P;
            break;
        case IMAGE_PIXEL_FORMAT_YVU420:
            cameraFormat = CAMERA_FORMAT_YCRCB_420_P;
            break;
        default:
            cameraFormat = CAMERA_FORMAT_INVALID;
    }

    return cameraFormat;
}

uint32_t BufferAdapter::CameraFormatToPixelFormat(const uint32_t cameraFormat)
{
    uint32_t format = IMAGE_PIXEL_FORMAT_NONE;
    switch (cameraFormat) {
        case CAMERA_FORMAT_RGB_565:
            format = IMAGE_PIXEL_FORMAT_RGB565;
            break;
        case CAMERA_FORMAT_RGBA_5551:
            format = IMAGE_PIXEL_FORMAT_ARGB1555;
            break;
        case CAMERA_FORMAT_RGBA_8888:
            format = IMAGE_PIXEL_FORMAT_ARGB8888;
            break;
        case CAMERA_FORMAT_YCBCR_420_SP:
            format = IMAGE_PIXEL_FORMAT_NV12;
            break;
        case CAMERA_FORMAT_YCRCB_420_SP:
            format = IMAGE_PIXEL_FORMAT_NV21;
            break;
        case CAMERA_FORMAT_YCBCR_420_P:
            format = IMAGE_PIXEL_FORMAT_YUV420;
            break;
        case CAMERA_FORMAT_YCRCB_420_P:
            format = IMAGE_PIXEL_FORMAT_YVU420;
            break;
        default:
            format = IMAGE_PIXEL_FORMAT_NONE;
            break;
    }

    return format;
}

uint64_t BufferAdapter::CameraUsageToGrallocUsage(const uint64_t cameraUsage)
{
    uint64_t grallocUsage = 0;
    uint64_t test = 1;
    const uint32_t BYTE = 8; // 8 bits per byte
    for (uint32_t i = 0; i < sizeof(cameraUsage) * BYTE; i++) {
        switch (cameraUsage & (test << i)) {
            case CAMERA_USAGE_MEM_SHARE:
                grallocUsage |= BUFFER_CONSUMER_USAGE_SORTWARE;
                break;
            case CAMERA_USAGE_MEM_MMZ:
                grallocUsage |= BUFFER_CONSUMER_USAGE_HARDWARE_PRODUCER_CACHE;
                break;
            case CAMERA_USAGE_MEM_MMZ_CACHE:
                grallocUsage |= BUFFER_CONSUMER_USAGE_HARDWARE_CONSUMER_CACHE;
                break;
            default:
                break;
        }
    }

    return grallocUsage;
}

uint64_t BufferAdapter::GrallocUsageToCameraUsage(const uint64_t usage)
{
    uint64_t cameraUsage = 0;
    uint64_t test = 1;
    const uint32_t BYTE = 8; // 8 bits per byte
    for (uint32_t i = 0; i < sizeof(usage) * BYTE; i++) {
        switch (usage & (test << i)) {
            case BUFFER_CONSUMER_USAGE_SORTWARE:
                cameraUsage |= CAMERA_USAGE_MEM_SHARE;
                break;
            case BUFFER_CONSUMER_USAGE_HARDWARE:
            case BUFFER_CONSUMER_USAGE_HARDWARE_PRODUCER_CACHE:
                cameraUsage |= CAMERA_USAGE_MEM_MMZ;
                break;
            case BUFFER_CONSUMER_USAGE_HARDWARE_CONSUMER_CACHE:
                cameraUsage |= CAMERA_USAGE_MEM_MMZ_CACHE;
                break;
            default:
                break;
        }
    }

    return cameraUsage;
}
} // namespace OHOS::Camera
