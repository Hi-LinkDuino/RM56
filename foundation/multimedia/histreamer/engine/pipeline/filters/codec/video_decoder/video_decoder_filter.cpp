/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifdef VIDEO_SUPPORT

#define HST_LOG_TAG "VideoDecoderFilter"

#include "video_decoder_filter.h"
#include "factory/filter_factory.h"
#include "filters/common/dump_buffer.h"
#include "foundation/cpp_ext/memory_ext.h"
#include "foundation/log.h"
#include "osal/utils/util.h"
#include "pipeline/filters/codec/codec_filter_factory.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_video_tags.h"
#include "plugin/common/surface_allocator.h"
#include "utils/constants.h"
#include "utils/steady_clock.h"

namespace {
const uint32_t DEFAULT_IN_BUFFER_POOL_SIZE = 8;
const uint32_t DEFAULT_OUT_BUFFER_POOL_SIZE = 8;
const float VIDEO_PIX_DEPTH = 1.5;
const uint32_t VIDEO_ALIGN_SIZE = 16;
}

namespace OHOS {
namespace Media {
namespace Pipeline {
VideoDecoderFilter::VideoDecoderFilter(const std::string& name, std::shared_ptr<CodecMode>& codecMode)
    : CodecFilterBase(name)
{
    MEDIA_LOG_I("video decoder ctor called");
    filterType_ = FilterType::VIDEO_DECODER;
    bufferMetaType_ = Plugin::BufferMetaType::VIDEO;
    codecMode_ = codecMode;
}

VideoDecoderFilter::~VideoDecoderFilter()
{
    MEDIA_LOG_D("video decoder dtor called");
    if (plugin_) {
        plugin_->Stop();
        plugin_->Deinit();
    }
    (void)codecMode_->Release();
}

ErrorCode VideoDecoderFilter::Prepare()
{
    MEDIA_LOG_I("video decoder prepare called.");
    codecMode_->SetBufferPoolSize(static_cast<uint32_t>(DEFAULT_IN_BUFFER_POOL_SIZE),
                                  static_cast<uint32_t>(DEFAULT_OUT_BUFFER_POOL_SIZE));
    (void)codecMode_->Prepare();
    return CodecFilterBase::Prepare();
}

ErrorCode VideoDecoderFilter::Start()
{
    return CodecFilterBase::Start();
}

ErrorCode VideoDecoderFilter::Stop()
{
    MEDIA_LOG_D("video decoder stop start.");
    FAIL_RETURN(CodecFilterBase::Stop());
    MEDIA_LOG_D("video decoder stop end.");
    return ErrorCode::SUCCESS;
}

void VideoDecoderFilter::FlushStart()
{
    MEDIA_LOG_I("video decoder FlushStart entered.");
    codecMode_->FlushStart();
    CodecFilterBase::FlushStart();
}

void VideoDecoderFilter::FlushEnd()
{
    MEDIA_LOG_I("video decoder FlushEnd entered");
    isFlushing_ = false;
    codecMode_->FlushEnd();
}

bool VideoDecoderFilter::Configure(const std::string &inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    PROFILE_BEGIN("audio decoder configure begin");
    FALSE_RETURN_V(CodecFilterBase::Configure(inPort, upstreamMeta), false);
    PROFILE_END("audio decoder configure end");
    return true;
}

ErrorCode VideoDecoderFilter::PushData(const std::string &inPort, const AVBufferPtr& buffer, int64_t offset)
{
    if (state_ != FilterState::READY && state_ != FilterState::PAUSED && state_ != FilterState::RUNNING) {
        MEDIA_LOG_W("pushing data to decoder when state is " PUBLIC_LOG_D32, static_cast<int>(state_.load()));
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    if (isFlushing_) {
        MEDIA_LOG_I("video decoder is flushing, discarding this data from port " PUBLIC_LOG_S, inPort.c_str());
        return ErrorCode::SUCCESS;
    }
    DUMP_BUFFER2FILE("decoder_input.data", buffer);
    return codecMode_->PushData(inPort, buffer, offset);
}

bool VideoDecoderFilter::Negotiate(const std::string& inPort,
                                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                                   Plugin::Capability& negotiatedCap,
                                   const Plugin::TagMap& upstreamParams,
                                   Plugin::TagMap& downstreamParams)
{
    FALSE_RETURN_V(CodecFilterBase::Negotiate(inPort, upstreamCap, negotiatedCap, upstreamParams, downstreamParams),
                   false);
    MEDIA_LOG_D("video decoder negotiate end");
    return true;
}

uint32_t VideoDecoderFilter::GetOutBufferPoolSize()
{
    return DEFAULT_OUT_BUFFER_POOL_SIZE;
}

uint32_t VideoDecoderFilter::CalculateBufferSize(const std::shared_ptr<const OHOS::Media::Plugin::Meta>& meta)
{
    uint32_t bufferSize = 0;
    uint32_t vdecWidth;
    uint32_t vdecHeight;
    Plugin::VideoPixelFormat vdecFormat;

    if (!meta->GetUint32(Plugin::MetaID::VIDEO_WIDTH, vdecWidth)) {
        MEDIA_LOG_E("Get video width fail");
        return 0;
    }
    if (!meta->GetUint32(Plugin::MetaID::VIDEO_HEIGHT, vdecHeight)) {
        MEDIA_LOG_E("Get video width height");
        return 0;
    }
    if (!meta->GetData<Plugin::VideoPixelFormat>(Plugin::MetaID::VIDEO_PIXEL_FORMAT, vdecFormat)) {
        MEDIA_LOG_E("Get video pixel format fail");
        return 0;
    }
    // YUV420: size = stride * height * 1.5
    uint32_t stride = Plugin::AlignUp(vdecWidth, VIDEO_ALIGN_SIZE);
    if (vdecFormat == Plugin::VideoPixelFormat::YUV420P ||
        vdecFormat == Plugin::VideoPixelFormat::NV21 ||
        vdecFormat == Plugin::VideoPixelFormat::NV12) {
        bufferSize = static_cast<uint32_t>(Plugin::AlignUp(stride, VIDEO_ALIGN_SIZE) *
                                           Plugin::AlignUp(vdecHeight, VIDEO_ALIGN_SIZE) * VIDEO_PIX_DEPTH);
        MEDIA_LOG_D("YUV output buffer size: " PUBLIC_LOG_U32, bufferSize);
    } else if (vdecFormat == Plugin::VideoPixelFormat::RGBA ||
               vdecFormat == Plugin::VideoPixelFormat::ARGB ||
               vdecFormat == Plugin::VideoPixelFormat::ABGR ||
               vdecFormat == Plugin::VideoPixelFormat::BGRA) {
        bufferSize = static_cast<uint32_t>(Plugin::AlignUp(stride, VIDEO_ALIGN_SIZE) *
                                            Plugin::AlignUp(vdecHeight, VIDEO_ALIGN_SIZE) * 4); // 4: 32bit
        MEDIA_LOG_D("RGBA output buffer size: " PUBLIC_LOG_U32, bufferSize);
    } else {
        // need to check video sink support and calc buffer size
        MEDIA_LOG_E("Unsupported video pixel format: " PUBLIC_LOG_U32, vdecFormat);
    }
    return bufferSize;
}

Plugin::TagMap VideoDecoderFilter::GetNegotiateParams(const Plugin::TagMap& upstreamParams)
{
    // video, need to get the max buffer num from plugin capability when use hdi as codec plugin interfaces
    Plugin::TagMap proposeParams = upstreamParams;
    proposeParams.Insert<Plugin::Tag::VIDEO_MAX_SURFACE_NUM>(DEFAULT_OUT_BUFFER_POOL_SIZE);
    return proposeParams;
}

std::shared_ptr<Allocator> VideoDecoderFilter::GetAllocator()
{
#ifndef OHOS_LITE
    // Use sink allocator first, zero copy while passing data
    Plugin::Tag tag = Plugin::Tag::BUFFER_ALLOCATOR;
    auto ite = sinkParams_.Find(tag);
    if (ite != std::end(sinkParams_)) {
        if (ite->second.SameTypeWith(typeid(std::shared_ptr<Plugin::SurfaceAllocator>))) {
            MEDIA_LOG_D("Get SurfaceAllocator from sink");
            return Plugin::AnyCast<std::shared_ptr<Plugin::SurfaceAllocator>>(ite->second);
        }
    }
#endif
    return plugin_->GetAllocator();
}

void VideoDecoderFilter::UpdateParams(const std::shared_ptr<const Plugin::Meta>& upMeta,
                                      std::shared_ptr<Plugin::Meta>& meta)
{
    MEDIA_LOG_D("video not need implement this func.");
}

void VideoDecoderFilter::OnInputBufferDone(const std::shared_ptr<Plugin::Buffer>& input)
{
    MEDIA_LOG_DD("VideoDecoderFilter::OnInputBufferDone");
}

void VideoDecoderFilter::OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& output)
{
    codecMode_->OnOutputBufferDone(output);
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif