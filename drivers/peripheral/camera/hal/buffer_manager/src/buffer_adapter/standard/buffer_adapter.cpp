
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
RetCode BufferAdapter::SurfaceBufferToCameraBuffer(const OHOS::sptr<OHOS::SurfaceBuffer>& surfaceBuffer,
    const std::shared_ptr<IBuffer>& buffer)
{
    CHECK_IF_EQUAL_RETURN_VALUE(buffer == nullptr || surfaceBuffer == nullptr, true, RC_ERROR);
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(buffer->GetSourceType(),
        CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL, RC_ERROR);
    BufferHandle* bufHandle = surfaceBuffer->GetBufferHandle();
    if (bufHandle == nullptr) {
        CAMERA_LOGE("bufHandle is null cannot convert");
        return RC_ERROR;
    }

    buffer->SetPhyAddress(bufHandle->phyAddr);
    buffer->SetFileDescriptor(bufHandle->fd);
    buffer->SetStride(bufHandle->stride);
    buffer->SetWidth(bufHandle->width);
    buffer->SetHeight(bufHandle->height);
    int32_t format = static_cast<int32_t>(PixelFormatToCameraFormat(static_cast<PixelFormat>(bufHandle->format)));
    buffer->SetFormat(format);
    buffer->SetUsage(CameraUsageToGrallocUsage(bufHandle->usage));
    buffer->SetSize(static_cast<uint32_t>(bufHandle->size));
    if (bufHandle->virAddr != nullptr) {
        buffer->SetVirAddress(bufHandle->virAddr);
    }

    return RC_OK;
}

RetCode BufferAdapter::CameraBufferToSurfaceBuffer(const std::shared_ptr<IBuffer>& buffer,
                                                   const OHOS::sptr<OHOS::SurfaceBuffer>& surfaceBuffer)
{
    CHECK_IF_EQUAL_RETURN_VALUE(buffer == nullptr || surfaceBuffer == nullptr, true, RC_ERROR);
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(buffer->GetSourceType(),
        CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL, RC_ERROR);

    BufferHandle* pHandle = surfaceBuffer->GetBufferHandle();
    if (pHandle == nullptr) {
        CAMERA_LOGE("pHandle is null cannot convert");
        return RC_ERROR;
    }
    pHandle->phyAddr = buffer->GetPhyAddress();
    pHandle->fd = buffer->GetFileDescriptor();
    pHandle->stride = static_cast<int32_t>(buffer->GetStride());
    pHandle->width = static_cast<int32_t>(buffer->GetWidth());
    pHandle->height = static_cast<int32_t>(buffer->GetHeight());
    pHandle->format = CameraFormatToPixelFormat(buffer->GetFormat());
    pHandle->usage =
        static_cast<uint64_t>(GrallocUsageToCameraUsage(buffer->GetUsage()));
    pHandle->virAddr = buffer->GetVirAddress();
    pHandle->size = static_cast<int32_t>(buffer->GetSize());

    return RC_OK;
}

RetCode BufferAdapter::SetExtInfoToSurfaceBuffer(const std::shared_ptr<IBuffer>& buffer,
    const OHOS::sptr<OHOS::SurfaceBuffer>& surfaceBuffer)
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

uint32_t BufferAdapter::PixelFormatToCameraFormat(const PixelFormat format)
{
    uint32_t cameraFormat = CAMERA_FORMAT_INVALID;
    switch (format) {
        case PIXEL_FMT_YUV_422_I:
            cameraFormat = CAMERA_FORMAT_YUV_422_I;
            break;
        case PIXEL_FMT_YCBCR_422_SP:
            cameraFormat = CAMERA_FORMAT_YCBCR_422_SP;
            break;
        case PIXEL_FMT_YCRCB_422_SP:
            cameraFormat = CAMERA_FORMAT_YCBCR_422_SP;
            break;
        case PIXEL_FMT_YCBCR_420_SP:
            cameraFormat = CAMERA_FORMAT_YCBCR_420_SP;
            break;
        case PIXEL_FMT_YCRCB_420_SP:
            cameraFormat = CAMERA_FORMAT_YCRCB_420_SP;
            break;
        case PIXEL_FMT_YCBCR_422_P:
            cameraFormat = CAMERA_FORMAT_YCBCR_422_P;
            break;
        case PIXEL_FMT_YCRCB_422_P:
            cameraFormat = CAMERA_FORMAT_YCRCB_422_P;
            break;
        case PIXEL_FMT_YCBCR_420_P:
            cameraFormat = CAMERA_FORMAT_YCBCR_420_P;
            break;
        case PIXEL_FMT_YCRCB_420_P:
            cameraFormat = CAMERA_FORMAT_YCRCB_420_P;
            break;
        case PIXEL_FMT_YUYV_422_PKG:
            cameraFormat = CAMERA_FORMAT_YCBCR_422_SP;
            break;
        case PIXEL_FMT_UYVY_422_PKG:
            cameraFormat = CAMERA_FORMAT_UYVY_422_PKG;
            break;
        case PIXEL_FMT_YVYU_422_PKG:
            cameraFormat = CAMERA_FORMAT_YVYU_422_PKG;
            break;
        case PIXEL_FMT_VYUY_422_PKG:
            cameraFormat = CAMERA_FORMAT_VYUY_422_PKG;
            break;
        case PIXEL_FMT_RGBA_8888:
            cameraFormat = CAMERA_FORMAT_RGBA_8888;
            break;
        default:
            cameraFormat = CAMERA_FORMAT_INVALID;
            break;
    }

    return cameraFormat;
}

PixelFormat BufferAdapter::CameraFormatToPixelFormat(const uint32_t cameraFormat)
{
    PixelFormat format = PIXEL_FMT_BUTT;
    switch (cameraFormat) {
        case CAMERA_FORMAT_YUV_422_I:
            format = PIXEL_FMT_YUV_422_I;
            break;
        case CAMERA_FORMAT_YCBCR_422_SP:
            format = PIXEL_FMT_YCBCR_422_SP;
            break;
        case CAMERA_FORMAT_YCRCB_422_SP:
            format = PIXEL_FMT_YCRCB_422_SP;
            break;
        case CAMERA_FORMAT_YCBCR_420_SP:
            format = PIXEL_FMT_YCBCR_420_SP;
            break;
        case CAMERA_FORMAT_YCRCB_420_SP:
            format = PIXEL_FMT_YCRCB_420_SP;
            break;
        case CAMERA_FORMAT_YCBCR_422_P:
            format = PIXEL_FMT_YCBCR_422_P;
            break;
        case CAMERA_FORMAT_YCRCB_422_P:
            format = PIXEL_FMT_YCRCB_422_P;
            break;
        case CAMERA_FORMAT_YCBCR_420_P:
            format = PIXEL_FMT_YCBCR_420_P;
            break;
        case CAMERA_FORMAT_YCRCB_420_P:
            format = PIXEL_FMT_YCRCB_420_P;
            break;
        case CAMERA_FORMAT_YUYV_422_PKG:
            format = PIXEL_FMT_YUYV_422_PKG;
            break;
        case CAMERA_FORMAT_UYVY_422_PKG:
            format = PIXEL_FMT_UYVY_422_PKG;
            break;
        case CAMERA_FORMAT_YVYU_422_PKG:
            format = PIXEL_FMT_YVYU_422_PKG;
            break;
        case CAMERA_FORMAT_VYUY_422_PKG:
            format = PIXEL_FMT_VYUY_422_PKG;
            break;
        case CAMERA_FORMAT_RGBA_8888:
            format = PIXEL_FMT_RGBA_8888;
            break;
        default:
            format = PIXEL_FMT_BUTT;
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
            case CAMERA_USAGE_SW_READ_OFTEN:
                grallocUsage |= HBM_USE_CPU_READ;
                break;
            case CAMERA_USAGE_SW_WRITE_OFTEN:
                grallocUsage |= HBM_USE_CPU_WRITE;
                break;
            case CAMERA_USAGE_MEM_DMA:
                grallocUsage |= HBM_USE_MEM_DMA;
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
            case HBM_USE_CPU_READ:
                cameraUsage |= CAMERA_USAGE_SW_READ_OFTEN;
                break;
            case HBM_USE_CPU_WRITE:
                cameraUsage |= CAMERA_USAGE_SW_WRITE_OFTEN;
                break;
            case HBM_USE_MEM_DMA:
                cameraUsage |= CAMERA_USAGE_MEM_DMA;
                break;
            default:
                break;
        }
    }

    return cameraUsage;
}
} // namespace OHOS::Camera
