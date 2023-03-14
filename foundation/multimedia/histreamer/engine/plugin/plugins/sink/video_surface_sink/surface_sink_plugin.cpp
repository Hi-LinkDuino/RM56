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

#define HST_LOG_TAG "SurfaceSinkPlugin"

#include "surface_sink_plugin.h"
#include <functional>
#include <algorithm>
#include "string_ex.h"
#include "param_wrapper.h"
#include "surface_buffer_impl.h"
#include "securec.h"
#include "foundation/log.h"
#include "utils/constants.h"
#include "plugin/common/surface_memory.h"

namespace {
using namespace OHOS::Media::Plugin;
using namespace VidSurfaceSinkPlugin;
constexpr uint32_t DEFAULT_WIDTH = 640;
constexpr uint32_t DEFAULT_HEIGHT = 480;
constexpr uint32_t DEFAULT_BUFFER_NUM = 10;
constexpr int32_t DEFAULT_STRIDE_ALIGN = 8;

std::shared_ptr<VideoSinkPlugin> VideoSinkPluginCreator(const std::string& name)
{
    return std::make_shared<SurfaceSinkPlugin>(name);
}

Status SurfaceSinkRegister(const std::shared_ptr<Register>& reg)
{
    VideoSinkPluginDef definition;
    definition.name = "surface_sink";
    definition.rank = 100; // 100
    Capability cap(OHOS::Media::MEDIA_MIME_VIDEO_RAW);
    cap.AppendDiscreteKeys<VideoPixelFormat>(
        Capability::Key::VIDEO_PIXEL_FORMAT,
        {VideoPixelFormat::RGBA, VideoPixelFormat::NV21});
    definition.inCaps.emplace_back(cap);
    definition.creator = VideoSinkPluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(StdVideoSurfaceSink, LicenseType::APACHE_V2, SurfaceSinkRegister, [] {});
} // namespace

namespace OHOS {
namespace Media {
namespace Plugin {
namespace VidSurfaceSinkPlugin {
static PixelFormat TranslatePixelFormat(const VideoPixelFormat pixelFormat)
{
    PixelFormat surfaceFormat = PixelFormat::PIXEL_FMT_BUTT;
    switch (pixelFormat) {
        case VideoPixelFormat::YUV420P:
            surfaceFormat = PixelFormat::PIXEL_FMT_YCBCR_420_P;
            break;
        case VideoPixelFormat::YUYV422:
            surfaceFormat = PixelFormat::PIXEL_FMT_YUYV_422_PKG;
            break;
        case VideoPixelFormat::RGBA:
            surfaceFormat = PixelFormat::PIXEL_FMT_RGBA_8888;
            break;
        case VideoPixelFormat::BGRA:
            surfaceFormat = PixelFormat::PIXEL_FMT_BGRA_8888;
            break;
        case VideoPixelFormat::YUV422P:
            surfaceFormat = PixelFormat::PIXEL_FMT_YUV_422_I;
            break;
        case VideoPixelFormat::ARGB:
        case VideoPixelFormat::ABGR:
        case VideoPixelFormat::RGB24:
        case VideoPixelFormat::BGR24:
        case VideoPixelFormat::YUV444P:
        case VideoPixelFormat::YUV410P:
        case VideoPixelFormat::YUV411P:
        case VideoPixelFormat::GRAY8:
        case VideoPixelFormat::MONOWHITE:
        case VideoPixelFormat::MONOBLACK:
        case VideoPixelFormat::PAL8:
        case VideoPixelFormat::YUVJ420P:
        case VideoPixelFormat::YUVJ422P:
        case VideoPixelFormat::YUVJ444P:
            break;
        case VideoPixelFormat::NV12:
            surfaceFormat = PixelFormat::PIXEL_FMT_YCBCR_420_SP;
            break;
        case VideoPixelFormat::NV21:
            surfaceFormat = PixelFormat::PIXEL_FMT_YCRCB_420_SP;
            break;
        default:
            break;
    }
    return surfaceFormat;
}

SurfaceSinkPlugin::SurfaceSinkPlugin(std::string name)
    : VideoSinkPlugin(std::move(name)),
      width_(DEFAULT_WIDTH),
      height_(DEFAULT_HEIGHT),
      pixelFormat_(VideoPixelFormat::NV21),
      maxSurfaceNum_(DEFAULT_BUFFER_NUM),
      needConvFormat(false)
{
}

Status SurfaceSinkPlugin::Init()
{
    std::weak_ptr<SurfaceSinkPlugin> weakPtr(shared_from_this());
    // must get the surface_ from app
    if (surface_ == nullptr) {
        OSAL::ScopedLock lock(mutex_);
        surfaceCond_.Wait(lock, [this] { return surface_ != nullptr; });
    }
#ifdef DUMP_RAW_DATA
    dumpFd_ = std::fopen("./vsink_out.dat", "wb");
#endif
    MEDIA_LOG_D("get surface success");
    return Status::OK;
}

Status SurfaceSinkPlugin::Deinit()
{
#ifdef DUMP_RAW_DATA
    if (dumpFd_) {
        std::fclose(dumpFd_);
        dumpFd_ = nullptr;
    }
#endif
    surface_ = nullptr;
    return Status::OK;
}

Status SurfaceSinkPlugin::Prepare()
{
    FALSE_RETURN_V_MSG_E(surface_ != nullptr && mAllocator_ != nullptr,
                         Status::ERROR_UNKNOWN, "need surface config first");
    FALSE_RETURN_V_MSG_E(surface_->SetQueueSize(maxSurfaceNum_) == OHOS::SurfaceError::SURFACE_ERROR_OK,
                         Status::ERROR_UNKNOWN, "surface SetQueueSize fail");
    PixelFormat pluginFmt = TranslatePixelFormat(pixelFormat_);
    if (pluginFmt == PixelFormat::PIXEL_FMT_BUTT) {
        MEDIA_LOG_E("surface can not support pixel format: " PUBLIC_LOG_U32, pixelFormat_);
        return Status::ERROR_UNKNOWN;
    }
    const std::string surfaceFmtStr = "SURFACE_FORMAT";
    std::string formatStr = surface_->GetUserData(surfaceFmtStr);
    PixelFormat surfaceFmt;
    if (formatStr == std::to_string(PixelFormat::PIXEL_FMT_RGBA_8888)) {
        surfaceFmt = PixelFormat::PIXEL_FMT_RGBA_8888;
    } else {
        surfaceFmt = PixelFormat::PIXEL_FMT_YCRCB_420_SP;
    }
    if (pluginFmt != surfaceFmt) {
        MEDIA_LOG_D("plugin format: " PUBLIC_LOG_U32 " is diff from surface format: " PUBLIC_LOG_U32,
                    static_cast<uint32_t>(pluginFmt), static_cast<uint32_t>(surfaceFmt));
        // need to convert pixel format when write
        needConvFormat = true;
    }
    int32_t usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
    mAllocator_->Config(static_cast<int32_t>(width_), static_cast<int32_t>(height_), usage, surfaceFmt,
                        DEFAULT_STRIDE_ALIGN, 0);
    MEDIA_LOG_D("Prepare success");
    return Status::OK;
}

Status SurfaceSinkPlugin::Reset()
{
    MEDIA_LOG_D("Reset success");
#ifdef DUMP_RAW_DATA
    if (dumpFd_) {
        std::fclose(dumpFd_);
        dumpFd_ = nullptr;
    }
#endif
    return Status::OK;
}

Status SurfaceSinkPlugin::Start()
{
    MEDIA_LOG_D("Start success");
    return Status::OK;
}

Status SurfaceSinkPlugin::Stop()
{
    MEDIA_LOG_D("Stop success");
    if (surface_) {
        surface_->CleanCache();
    }
    return Status::OK;
}

Status SurfaceSinkPlugin::GetParameter(Tag tag, ValueType& value)
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status SurfaceSinkPlugin::SetParameter(Tag tag, const ValueType& value)
{
    OSAL::ScopedLock lock(mutex_);
    switch (tag) {
        case Tag::VIDEO_WIDTH: {
            if (value.SameTypeWith(typeid(uint32_t))) {
                width_ = Plugin::AnyCast<uint32_t>(value);
                MEDIA_LOG_D("width_: " PUBLIC_LOG_U32, width_);
            }
            break;
        }
        case Tag::VIDEO_HEIGHT: {
            if (value.SameTypeWith(typeid(uint32_t))) {
                height_ = Plugin::AnyCast<uint32_t>(value);
                MEDIA_LOG_D("height_: " PUBLIC_LOG_U32, height_);
            }
            break;
        }
        case Tag::VIDEO_PIXEL_FORMAT: {
            if (value.SameTypeWith(typeid(VideoPixelFormat))) {
                pixelFormat_ = Plugin::AnyCast<VideoPixelFormat>(value);
                MEDIA_LOG_D("pixelFormat: " PUBLIC_LOG_U32, static_cast<uint32_t>(pixelFormat_));
            }
            break;
        }
        case Tag::VIDEO_SURFACE: {
            if (value.SameTypeWith(typeid(sptr<Surface>))) {
                surface_ = Plugin::AnyCast<sptr<Surface>>(value);
                if (!surface_) {
                    MEDIA_LOG_E("surface is null");
                    return Status::ERROR_INVALID_PARAMETER;
                }
                mAllocator_ = std::make_shared<SurfaceAllocator>(surface_);
                surfaceCond_.NotifyAll();
            }
            break;
        }
        case Tag::VIDEO_MAX_SURFACE_NUM: {
            if (value.SameTypeWith(typeid(uint32_t))) {
                auto bufferNum = Plugin::AnyCast<uint32_t>(value);
                if (bufferNum < DEFAULT_BUFFER_NUM) {
                    maxSurfaceNum_ = bufferNum;
                }
                MEDIA_LOG_D("maxSurfaceNum_: " PUBLIC_LOG_U32, maxSurfaceNum_);
            }
            break;
        }
        default:
            MEDIA_LOG_I("Unknown key");
            break;
    }
    return Status::OK;
}

std::shared_ptr<Allocator> SurfaceSinkPlugin::GetAllocator()
{
    return mAllocator_;
}

Status SurfaceSinkPlugin::SetCallback(Callback* cb)
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status SurfaceSinkPlugin::Pause()
{
    return Status::OK;
}

Status SurfaceSinkPlugin::Resume()
{
    return Status::OK;
}

Status SurfaceSinkPlugin::UpdateSurfaceBuffer(sptr<SurfaceBuffer> surfaceBuffer, int32_t fence)
{
    FALSE_RETURN_V_MSG_E(surfaceBuffer != nullptr, Status::ERROR_NULL_POINTER, "surfaceBuffer is NULL");
#ifdef DUMP_RAW_DATA
    if (dumpFd_ && surfaceBuffer->GetVirAddr()) {
        std::fwrite(reinterpret_cast<const char*>(surfaceBuffer->GetVirAddr()),
                    surfaceBuffer->GetSize(), 1, dumpFd_);
    }
#endif
    FALSE_RETURN_V_MSG_E(needConvFormat == false, Status::ERROR_UNIMPLEMENTED, "Need to convert format");
    OHOS::BufferFlushConfig flushConfig = {
        {0, 0, surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight()},
    };
    auto res = surface_->FlushBuffer(surfaceBuffer, fence, flushConfig);
    if (res != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_W("surface FlushBuffer fail: " PUBLIC_LOG_D32, res);
        res = surface_->CancelBuffer(surfaceBuffer);
        FALSE_RETURN_V_MSG_W(res == OHOS::SurfaceError::SURFACE_ERROR_OK, Status::ERROR_UNKNOWN,
                             "surface CancelBuffer fail: " PUBLIC_LOG_D32, res);
    }
    return Status::OK;
}

Status SurfaceSinkPlugin::Write(const std::shared_ptr<Buffer>& inputInfo)
{
    MEDIA_LOG_D("SurfaceSink write begin");
    if (inputInfo == nullptr || inputInfo->IsEmpty()) {
        return Status::ERROR_INVALID_PARAMETER;
    }
    auto memory = inputInfo->GetMemory();
    FALSE_RETURN_V_MSG_E(memory != nullptr, Status::ERROR_NULL_POINTER, "GetMemory fail");
    FALSE_RETURN_V_MSG_E(memory->GetMemoryType() == MemoryType::SURFACE_BUFFER, Status::ERROR_INVALID_PARAMETER,
                         "memory type is not SURFACE_BUFFER");
    std::shared_ptr<SurfaceMemory> surfaceMemory = ReinterpretPointerCast<SurfaceMemory>(memory);
    auto ret = UpdateSurfaceBuffer(surfaceMemory->GetSurfaceBuffer(), surfaceMemory->GetFlushFence());
    if (ret != Status::OK) {
        MEDIA_LOG_W("UpdateSurfaceBuffer fail: " PUBLIC_LOG_U32, ret);
    }
    // After surface buffer sending to surface, we need to clear sptr.
    // So that we can request again surface buffer on the same SurfaceMemory
    surfaceMemory->ReleaseSurfaceBuffer();
    MEDIA_LOG_D("SurfaceSink write success");
    return Status::OK;
}

Status SurfaceSinkPlugin::Flush()
{
    return Status::OK;
}

Status SurfaceSinkPlugin::GetLatency(uint64_t& nanoSec)
{
    nanoSec = 10; // 10 ns
    return Status::OK;
}
} // namespace VidSurfaceSinkPlugin
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif