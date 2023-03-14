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

#if !defined(OHOS_LITE) && defined(RECORDER_SUPPORT) && defined(VIDEO_SUPPORT)

#define HST_LOG_TAG "VideoCapturePlugin"

#include "video_capture_plugin.h"
#include <algorithm>
#include <cmath>
#include "foundation/log.h"
#include "foundation/pre_defines.h"
#include "plugin/common/plugin_time.h"
#include "utils/constants.h"

namespace {
// register plugins
using namespace OHOS::Media::Plugin;
using namespace VideoCapture;

Status VideoCaptureRegister(const std::shared_ptr<Register> &reg)
{
    SourcePluginDef definition;
    definition.name = "VideoCapture";
    definition.description = "Video capture from audio service";
    definition.rank = 100; // 100: max rank
    definition.inputType = SrcInputType::VID_SURFACE_YUV;
    definition.creator = [](const std::string& name) -> std::shared_ptr<SourcePlugin> {
        return std::make_shared<VideoCapturePlugin>(name);
    };
    Capability outCaps(OHOS::Media::MEDIA_MIME_VIDEO_RAW);
    outCaps.AppendDiscreteKeys<VideoPixelFormat>(
        Capability::Key::VIDEO_PIXEL_FORMAT, {VideoPixelFormat::NV21});
    definition.outCaps.push_back(outCaps);
    // add es outCaps later
    return reg->AddPlugin(definition);
}
PLUGIN_DEFINITION(StdVideoCapture, LicenseType::APACHE_V2, VideoCaptureRegister, [] {});
}

namespace OHOS {
namespace Media {
namespace Plugin {
namespace VideoCapture {
constexpr int32_t DEFAULT_SURFACE_QUEUE_SIZE = 6;
constexpr int32_t DEFAULT_SURFACE_SIZE = 1024 * 1024;
constexpr int32_t DEFAULT_VIDEO_WIDTH = 1920;
constexpr int32_t DEFAULT_VIDEO_HEIGHT = 1080;
constexpr int32_t DEFAULT_STRIDE_ALIGN = 16;

VideoCapturePlugin::VideoCapturePlugin(std::string name)
    : SourcePlugin(std::move(name)),
      width_(DEFAULT_VIDEO_WIDTH),
      height_(DEFAULT_VIDEO_HEIGHT)
{
    MEDIA_LOG_D("IN");
}

VideoCapturePlugin::~VideoCapturePlugin()
{
    MEDIA_LOG_D("IN");
}

Status VideoCapturePlugin::Init()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status VideoCapturePlugin::Deinit()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

void VideoCapturePlugin::ConfigSurfaceConsumer()
{
    auto ret = surfaceConsumer_->SetUserData("video_width", std::to_string(width_));
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_E("set video width fail");
    }
    ret = surfaceConsumer_->SetUserData("video_height", std::to_string(height_));
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_E("set video height fail");
    }
    ret = surfaceConsumer_->SetQueueSize(DEFAULT_SURFACE_QUEUE_SIZE);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_E("set queue size fail");
    }
    ret = surfaceConsumer_->SetUserData("surface_size", std::to_string(DEFAULT_SURFACE_SIZE));
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_E("set surface size fail");
    }
    ret = surfaceConsumer_->SetDefaultWidthAndHeight(width_, height_);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_E("set surface width and height fail");
    }
}

Status VideoCapturePlugin::Prepare()
{
    MEDIA_LOG_D("IN");
    surfaceConsumer_ = Surface::CreateSurfaceAsConsumer();
    if (!surfaceConsumer_) {
        MEDIA_LOG_E("CreateSurfaceAsConsumer() fail");
        return Status::ERROR_UNKNOWN;
    }
    sptr<IBufferConsumerListener> consumerListener = new (std::nothrow) SurfaceConsumerListener(*this);
    if (!consumerListener) {
        MEDIA_LOG_E("Malloc SurfaceConsumerListener fail");
        return Status::ERROR_NO_MEMORY;
    }
    if (surfaceConsumer_->RegisterConsumerListener(consumerListener) != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_E("RegisterConsumerListener() fail");
        return Status::ERROR_UNKNOWN;
    }
    sptr<IBufferProducer> bufferProducer = surfaceConsumer_->GetProducer();
    if (!bufferProducer) {
        MEDIA_LOG_E("Malloc GetProducer fail");
        return Status::ERROR_UNKNOWN;
    }
    surfaceProducer_ = Surface::CreateSurfaceAsProducer(bufferProducer);
    if (!surfaceProducer_) {
        MEDIA_LOG_E("CreateSurfaceAsProducer() fail");
        return Status::ERROR_UNKNOWN;
    }
    ConfigSurfaceConsumer();
    return Status::OK;
}

Status VideoCapturePlugin::Reset()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status VideoCapturePlugin::Start()
{
    MEDIA_LOG_D("IN");
    OSAL::ScopedLock lock(mutex_);
    if (isStop_.load()) {
        if (curTimestampNs_ < stopTimestampNs_) {
            MEDIA_LOG_E("Get wrong audio time");
        }
        totalPauseTimeNs_ += std::fabs(curTimestampNs_ - stopTimestampNs_);
        isStop_ = false;
    }
    return Status::OK;
}

Status VideoCapturePlugin::Stop()
{
    MEDIA_LOG_D("IN");
    OSAL::ScopedLock lock(mutex_);
    if (!isStop_.load()) {
        stopTimestampNs_ = curTimestampNs_;
        isStop_ = true;
    }
    return Status::OK;
}

Status VideoCapturePlugin::GetParameter(Tag tag, ValueType& value)
{
    MEDIA_LOG_D("IN");
    switch (tag) {
        case Tag::VIDEO_SURFACE: {
            value = surfaceProducer_;
            break;
        }
        default:
            MEDIA_LOG_I("Unknown key");
            break;
    }
    return Status::OK;
}

Status VideoCapturePlugin::SetParameter(Tag tag, const ValueType& value)
{
    switch (tag) {
        case Tag::VIDEO_HEIGHT: {
            if (value.SameTypeWith(typeid(uint32_t))) {
                height_ = Plugin::AnyCast<uint32_t>(value);
            }
            break;
        }
        case Tag::VIDEO_WIDTH: {
            if (value.SameTypeWith(typeid(uint32_t))) {
                width_ = Plugin::AnyCast<uint32_t>(value);
            }
            break;
        }
        case Tag::VIDEO_CAPTURE_RATE: {
            if (value.SameTypeWith(typeid(double))) {
                captureRate_ = Plugin::AnyCast<double>(value);
            }
            break;
        }
        case Tag::VIDEO_PIXEL_FORMAT: {
            if (value.SameTypeWith(typeid(VideoPixelFormat))) {
                pixelFormat_ = Plugin::AnyCast<VideoPixelFormat>(value);
            }
            break;
        }
        default:
            MEDIA_LOG_I("Unknown key");
            break;
    }
    return Status::OK;
}

std::shared_ptr<Allocator> VideoCapturePlugin::GetAllocator()
{
    MEDIA_LOG_D("IN");
    return nullptr;
}

Status VideoCapturePlugin::SetCallback(Callback* cb)
{
    MEDIA_LOG_D("IN");
    UNUSED_VARIABLE(cb);
    return Status::ERROR_UNIMPLEMENTED;
}

Status VideoCapturePlugin::SetSource(std::shared_ptr<MediaSource> source)
{
    UNUSED_VARIABLE(source);
    return Status::ERROR_UNIMPLEMENTED;
}

Status VideoCapturePlugin::AcquireSurfaceBuffer()
{
    auto ret = surfaceConsumer_->AcquireBuffer(surfaceBuffer_, fence_, timestamp_, damage_);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_E("surfaceConsumer AcquireBuffer() fail: " PUBLIC_LOG_U32, ret);
        return Status::ERROR_UNKNOWN;
    }
    ret = surfaceBuffer_->ExtraGet("dataSize", bufferSize_);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK || bufferSize_ <= 0) {
        MEDIA_LOG_E("surfaceBuffer get data size fail: " PUBLIC_LOG_U32, ret);
        return Status::ERROR_UNKNOWN;
    }
    ret = surfaceBuffer_->ExtraGet("isKeyFrame", isKeyFrame_);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        MEDIA_LOG_E("surfaceBuffer get isKeyFrame fail: " PUBLIC_LOG_U32, ret);
        return Status::ERROR_UNKNOWN;
    }
    int64_t pts;
    ret = surfaceBuffer_->ExtraGet("timeStamp", pts);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK || pts < 0) {
        MEDIA_LOG_E("surfaceBuffer get data size fail: " PUBLIC_LOG_U32, ret);
        return Status::ERROR_UNKNOWN;
    }
    if (static_cast<uint64_t>(pts) < curTimestampNs_) {
        MEDIA_LOG_W("Get wrong timestamp from surface buffer");
    }
    curTimestampNs_ = static_cast<uint64_t>(pts);
    return Status::OK;
}

void VideoCapturePlugin::SetVideoBufferMeta(std::shared_ptr<BufferMeta>& bufferMeta)
{
    std::shared_ptr<VideoBufferMeta> videoMeta = std::dynamic_pointer_cast<VideoBufferMeta>(bufferMeta);
    videoMeta->width = width_;
    videoMeta->height = height_;
    videoMeta->videoPixelFormat = VideoPixelFormat::NV21;
    size_t lineSize = AlignUp(width_, DEFAULT_STRIDE_ALIGN);
    if ((lineSize / 2) % DEFAULT_STRIDE_ALIGN) { // 2
        lineSize = AlignUp(width_, DEFAULT_STRIDE_ALIGN * 2); // 2
    }
    videoMeta->stride.emplace_back(lineSize); // lineSize[0]
    videoMeta->stride.emplace_back(lineSize / 2); // lineSize[1], 2
    videoMeta->stride.emplace_back(lineSize / 2); // lineSize[2], 2
    videoMeta->planes = videoMeta->stride.size();
}

Status VideoCapturePlugin::Read(std::shared_ptr<Buffer>& buffer, size_t expectedLen)
{
    OHOS::Media::OSAL::ScopedLock lock(mutex_);
    if (!buffer) {
        return Status::ERROR_INVALID_PARAMETER;
    }
    auto bufferMeta = buffer->GetBufferMeta();
    if (!bufferMeta || bufferMeta->GetType() != BufferMetaType::VIDEO || surfaceConsumer_ == nullptr) {
        return Status::ERROR_INVALID_PARAMETER;
    }
    std::shared_ptr<Memory> bufData;
    if (buffer->IsEmpty()) {
        bufData = buffer->AllocMemory(GetAllocator(), expectedLen);
    } else {
        bufData = buffer->GetMemory();
    }
    if (bufData->GetMemoryType() != MemoryType::VIRTUAL_ADDR || bufData->GetCapacity() <= 0) {
        return Status::ERROR_NO_MEMORY;
    }
    readCond_.Wait(lock, [this] { return bufferCnt_ > 0 || isStop_.load(); });
    if (isStop_.load()) {
        MEDIA_LOG_I("flush or EOS, skip read buffer");
        return Status::END_OF_STREAM;
    }
    auto ret = AcquireSurfaceBuffer();
    if (ret != Status::OK) {
        MEDIA_LOG_E("AcquireSurfaceBuffer fail: " PUBLIC_LOG_D32, ret);
        return ret;
    }
    auto writeSize = bufData->Write(static_cast<const uint8_t*>(surfaceBuffer_->GetVirAddr()), bufferSize_);
    if (static_cast<int32_t>(writeSize) != bufferSize_) {
        MEDIA_LOG_E("write buffer data fail");
        return Status::ERROR_UNKNOWN;
    }
    Ns2HstTime(curTimestampNs_ - totalPauseTimeNs_, reinterpret_cast<int64_t &>(buffer->pts));
    SetVideoBufferMeta(bufferMeta);
    bufferCnt_--;
    return Status::OK;
}

Status VideoCapturePlugin::GetSize(size_t& size)
{
    if (bufferSize_ == 0) {
        return Status::ERROR_INVALID_PARAMETER;
    }
    size = bufferSize_;
    MEDIA_LOG_D("bufferSize_: " PUBLIC_LOG_ZU, size);
    return Status::OK;
}

Seekable VideoCapturePlugin::GetSeekable()
{
    return Seekable::UNSEEKABLE;
}

Status VideoCapturePlugin::SeekTo(uint64_t offset)
{
    UNUSED_VARIABLE(offset);
    return Status::ERROR_UNIMPLEMENTED;
}

void VideoCapturePlugin::SurfaceConsumerListener::OnBufferAvailable()
{
    return owner_.OnBufferAvailable();
}

void VideoCapturePlugin::OnBufferAvailable()
{
    if (!surfaceConsumer_) {
        return;
    }
    OSAL::ScopedLock lock(mutex_);
    bufferCnt_++;
    if (bufferCnt_ == 1) {
        readCond_.NotifyAll();
    }
}
} // namespace VideoCapture
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif