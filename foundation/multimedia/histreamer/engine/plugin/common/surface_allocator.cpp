/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#if !defined(OHOS_LITE) && defined(VIDEO_SUPPORT)

#define HST_LOG_TAG "SurfaceAllocator"

#include "surface_allocator.h"
#include "foundation/log.h"
#include "display_type.h"
#include "sync_fence.h"

namespace OHOS {
namespace Media {
namespace Plugin {
constexpr int32_t DEFAULT_SURFACE_WIDTH = 640;
constexpr int32_t DEFAULT_SURFACE_HEIGHT = 480;
constexpr int32_t DEFAULT_SURFACE_STRIDE_ALIGN = 8;

SurfaceAllocator::SurfaceAllocator(sptr<Surface> surface)
    : Allocator(MemoryType::SURFACE_BUFFER),
      surface_(surface)
{
    requestConfig_ = {
        DEFAULT_SURFACE_WIDTH, DEFAULT_SURFACE_HEIGHT, DEFAULT_SURFACE_STRIDE_ALIGN,
        PixelFormat ::PIXEL_FMT_RGBA_8888, HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA, 0};
}

sptr<SurfaceBuffer> SurfaceAllocator::AllocSurfaceBuffer(size_t size)
{
    (void)size;
    if (surface_ == nullptr) {
        MEDIA_LOG_E("surface is nullptr");
        return nullptr;
    }
    MEDIA_LOG_D("width: " PUBLIC_LOG_D32 ", height :" PUBLIC_LOG_D32 ", align: " PUBLIC_LOG_D32
                ", format: " PUBLIC_LOG_D32 ", usage: " PUBLIC_LOG_D32 ", timeout: " PUBLIC_LOG_D32,
                requestConfig_.width, requestConfig_.height, requestConfig_.strideAlignment, requestConfig_.format,
                requestConfig_.usage, requestConfig_.timeout);
    OHOS::sptr<OHOS::SurfaceBuffer> surfaceBuffer = nullptr;
    int32_t releaseFence = -1;
    auto ret = surface_->RequestBuffer(surfaceBuffer, releaseFence, requestConfig_);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK || surfaceBuffer == nullptr) {
        if (ret == OHOS::SurfaceError::SURFACE_ERROR_NO_BUFFER) {
            MEDIA_LOG_E("buffer queue is no more buffers");
        } else {
            MEDIA_LOG_E("surface RequestBuffer fail, ret: " PUBLIC_LOG_U64, static_cast<uint64_t>(ret));
        }
        return nullptr;
    }
    sptr<SyncFence> autoFence = new(std::nothrow) SyncFence(releaseFence);
    if (autoFence != nullptr) {
        autoFence->Wait(100); // 100ms
    }
    MEDIA_LOG_D("request surface buffer success, releaseFence: " PUBLIC_LOG_D32, releaseFence);
    return surfaceBuffer;
}

void* SurfaceAllocator::Alloc(size_t size)
{
    return nullptr;
}

void SurfaceAllocator::Free(void* ptr) // NOLINT: void*
{
    (void)ptr;
}

void SurfaceAllocator::Config(int32_t width, int32_t height, int32_t usage, int32_t format, int32_t strideAlign,
                              int32_t timeout)
{
    requestConfig_ = {
        width, height, strideAlign, format, usage, timeout
    };
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif