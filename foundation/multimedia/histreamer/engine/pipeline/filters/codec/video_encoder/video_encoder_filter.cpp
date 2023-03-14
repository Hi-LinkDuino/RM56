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

#if defined(RECORDER_SUPPORT) && defined(VIDEO_SUPPORT)

#define HST_LOG_TAG "VideoEncoderFilter"

#include "video_encoder_filter.h"
#include "foundation/cpp_ext/memory_ext.h"
#include "factory/filter_factory.h"
#include "foundation/log.h"
#include "osal/utils/util.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_video_tags.h"
#include "utils/constants.h"
#include "utils/steady_clock.h"
#include "pipeline/core/plugin_attr_desc.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
const uint32_t DEFAULT_IN_BUFFER_POOL_SIZE = 8;
const uint32_t DEFAULT_OUT_BUFFER_POOL_SIZE = 8;
const float VIDEO_PIX_DEPTH = 1.5;
static uint32_t VIDEO_ALIGN_SIZE = 16;
static uint32_t DEFAULT_TRY_DECODE_TIME = 10;

static AutoRegisterFilter<VideoEncoderFilter> g_registerFilterHelper("builtin.recorder.videoencoder");

class VideoEncoderFilter::DataCallbackImpl : public Plugin::DataCallbackHelper {
public:
    explicit DataCallbackImpl(VideoEncoderFilter& filter) : encFilter_(filter)
    {
    }

    ~DataCallbackImpl() override = default;

    void OnInputBufferDone(const std::shared_ptr<Plugin::Buffer>& input) override
    {
        encFilter_.OnInputBufferDone(input);
    }

    void OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& output) override
    {
        encFilter_.OnOutputBufferDone(output);
    }

private:
    VideoEncoderFilter& encFilter_;
};

VideoEncoderFilter::VideoEncoderFilter(const std::string& name)
    : CodecFilterBase(name), dataCallback_(new DataCallbackImpl(*this))
{
    MEDIA_LOG_I("video encoder ctor called");
    filterType_ = FilterType::VIDEO_ENCODER;
    vencFormat_.width = 0;
    vencFormat_.height = 0;
    vencFormat_.bitRate = -1;
}

VideoEncoderFilter::~VideoEncoderFilter()
{
    MEDIA_LOG_I("video encoder dtor called");
    isStop_ = true;
    if (plugin_) {
        plugin_->Stop();
        plugin_->Deinit();
    }
    if (handleFrameTask_) {
        handleFrameTask_->Stop();
        handleFrameTask_.reset();
    }
    if (inBufQue_) {
        inBufQue_->SetActive(false);
        inBufQue_.reset();
    }
    if (pushTask_) {
        pushTask_->Stop();
        pushTask_.reset();
    }
    if (outBufQue_) {
        outBufQue_->SetActive(false);
        outBufQue_.reset();
    }
    if (dataCallback_) {
        delete dataCallback_;
        dataCallback_ = nullptr;
    }
}

ErrorCode VideoEncoderFilter::Start()
{
    MEDIA_LOG_D("video encoder start called");
    if (state_ != FilterState::READY && state_ != FilterState::PAUSED) {
        MEDIA_LOG_W("call encoder start() when state_ is not ready or working");
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    isStop_ = false;
    return FilterBase::Start();
}

ErrorCode VideoEncoderFilter::Prepare()
{
    MEDIA_LOG_D("video encoder prepare called");
    if (state_ != FilterState::INITIALIZED) {
        MEDIA_LOG_W("encoder filter is not in init state_");
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    if (!outBufQue_) {
        outBufQue_ = std::make_shared<BlockingQueue<AVBufferPtr>>("vdecFilterOutBufQue", DEFAULT_OUT_BUFFER_POOL_SIZE);
    } else {
        outBufQue_->SetActive(true);
    }
    if (!pushTask_) {
        pushTask_ = std::make_shared<OSAL::Task>("vecPushThread");
        pushTask_->RegisterHandler([this] { FinishFrame(); });
    }
    if (!inBufQue_) {
        inBufQue_ = std::make_shared<BlockingQueue<AVBufferPtr>>("vecFilterInBufQue", DEFAULT_IN_BUFFER_POOL_SIZE);
    } else {
        inBufQue_->SetActive(true);
    }
    if (!handleFrameTask_) {
        handleFrameTask_ = std::make_shared<OSAL::Task>("vecHandleFrameThread");
        handleFrameTask_->RegisterHandler([this] { HandleFrame(); });
    }
    return FilterBase::Prepare();
}

ErrorCode VideoEncoderFilter::SetVideoEncoder(int32_t sourceId, std::shared_ptr<Plugin::Meta> encoderMeta)
{
    std::string mime;
    FALSE_RETURN_V_MSG_E(encoderMeta->GetString(Plugin::MetaID::MIME, mime), ErrorCode::ERROR_INVALID_PARAMETER_VALUE,
                         "encoder meta must contains mime");
    vencFormat_.mime = mime;
    codecMeta_ = std::move(encoderMeta);
    return ErrorCode::SUCCESS;
}

bool VideoEncoderFilter::Negotiate(const std::string& inPort,
                                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                                   Plugin::Capability& negotiatedCap,
                                   const Plugin::TagMap& upstreamParams,
                                   Plugin::TagMap& downstreamParams)
{
    PROFILE_BEGIN("video encoder negotiate start");
    if (state_ != FilterState::PREPARING) {
        MEDIA_LOG_W("encoder filter is not in preparing when negotiate");
        return false;
    }
    auto targetOutPort = GetRouteOutPort(inPort);
    FALSE_RETURN_V_MSG_E(targetOutPort != nullptr, false, "out port not found");
    std::shared_ptr<Plugin::PluginInfo> selectedPluginInfo = nullptr;
    bool atLeastOutCapMatched = false;
    auto candidatePlugins = FindAvailablePlugins(*upstreamCap, Plugin::PluginType::CODEC);
    for (const auto& candidate : candidatePlugins) {
        FALSE_LOG_MSG(!candidate.first->outCaps.empty(), "encoder plugin must have out caps");
        for (const auto& outCap : candidate.first->outCaps) { // each codec plugin should have at least one out cap
            Plugin::Meta tmpMeta;
            if (outCap.mime != vencFormat_.mime ||
                !MergeMetaWithCapability(*codecMeta_, outCap, tmpMeta)) {
                continue;
            }
            auto thisOut = std::make_shared<Plugin::Capability>();
            if (!MergeCapabilityKeys(*upstreamCap, outCap, *thisOut)) {
                MEDIA_LOG_I("one cap of plugin " PUBLIC_LOG_S " mismatch upstream cap", candidate.first->name.c_str());
                continue;
            }
            atLeastOutCapMatched = true;
            thisOut->mime = outCap.mime;
            if (targetOutPort->Negotiate(thisOut, capNegWithDownstream_, upstreamParams, downstreamParams)) {
                capNegWithUpstream_ = candidate.second;
                selectedPluginInfo = candidate.first;
                MEDIA_LOG_I("use plugin " PUBLIC_LOG_S, candidate.first->name.c_str());
                MEDIA_LOG_I("neg upstream cap " PUBLIC_LOG_S, Capability2String(capNegWithUpstream_).c_str());
                MEDIA_LOG_I("neg downstream cap " PUBLIC_LOG_S, Capability2String(capNegWithDownstream_).c_str());
                break;
            }
        }
        if (selectedPluginInfo != nullptr) {
            break;
        }
    }
    FALSE_RETURN_V_MSG_E(atLeastOutCapMatched && selectedPluginInfo != nullptr, false,
                         "can't find available encoder plugin with " PUBLIC_LOG_S,
                         Capability2String(*upstreamCap).c_str());
    auto res = UpdateAndInitPluginByInfo<Plugin::Codec>(plugin_, pluginInfo_, selectedPluginInfo,
        [](const std::string& name)-> std::shared_ptr<Plugin::Codec> {
        return Plugin::PluginManager::Instance().CreateCodecPlugin(name);
    });
    negotiatedCap = *upstreamCap;
    PROFILE_END("video encoder negotiate end");
    return res;
}

bool VideoEncoderFilter::Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    PROFILE_BEGIN("video encoder configure start");
    MEDIA_LOG_I("receive upstream meta " PUBLIC_LOG_S, Meta2String(*upstreamMeta).c_str());
    if (plugin_ == nullptr || pluginInfo_ == nullptr) {
        MEDIA_LOG_E("cannot configure encoder when no plugin available");
        return false;
    }
    auto thisMeta = std::make_shared<Plugin::Meta>();
    if (!MergeMetaWithCapability(*upstreamMeta, pluginInfo_->outCaps[0], *thisMeta)) {
        MEDIA_LOG_E("cannot configure encoder plugin since meta is not compatible with negotiated caps");
        return false;
    }
    auto targetOutPort = GetRouteOutPort(inPort);
    if (targetOutPort == nullptr) {
        MEDIA_LOG_E("encoder out port is not found");
        return false;
    }
    auto err = ConfigureNoLocked(thisMeta);
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("encoder configure error");
        Event event {
            .srcFilter = name_,
            .type = EventType::EVENT_ERROR,
            .param = err,
        };
        OnEvent(event);
        return false;
    }
    FAIL_LOG(UpdateMetaFromPlugin(*thisMeta));
    if (!targetOutPort->Configure(thisMeta)) {
        MEDIA_LOG_E("encoder filter downstream Configure failed");
        return false;
    }
    state_ = FilterState::READY;
    Event event {
        .srcFilter = name_,
        .type = EventType::EVENT_READY,
    };
    OnEvent(event);
    MEDIA_LOG_I("video encoder send EVENT_READY");
    PROFILE_END("video encoder configure end");
    return true;
}

ErrorCode VideoEncoderFilter::AllocateOutputBuffers()
{
    uint32_t bufferCnt = 0;
    if (GetPluginParameterLocked(Tag::REQUIRED_OUT_BUFFER_CNT, bufferCnt) != ErrorCode::SUCCESS) {
        bufferCnt = DEFAULT_OUT_BUFFER_POOL_SIZE;
    }
    outBufPool_ = std::make_shared<BufferPool<AVBuffer>>(bufferCnt);
    // YUV420: size = stride * height * 1.5
    uint32_t bufferSize = 0;
    uint32_t stride = Plugin::AlignUp(vencFormat_.width, VIDEO_ALIGN_SIZE);
    if (vencFormat_.format == Plugin::VideoPixelFormat::YUV420P ||
        vencFormat_.format == Plugin::VideoPixelFormat::NV21 ||
        vencFormat_.format == Plugin::VideoPixelFormat::NV12) {
        bufferSize = static_cast<uint32_t>(Plugin::AlignUp(stride, VIDEO_ALIGN_SIZE) *
                                           Plugin::AlignUp(vencFormat_.height, VIDEO_ALIGN_SIZE) * VIDEO_PIX_DEPTH);
        MEDIA_LOG_D("Output buffer size: " PUBLIC_LOG_U32, bufferSize);
    } else {
        // need to check video sink support and calc buffer size
        MEDIA_LOG_E("Unsupported video pixel format: " PUBLIC_LOG_S, GetVideoPixelFormatNameStr(vencFormat_.format));
        return ErrorCode::ERROR_UNIMPLEMENTED;
    }
    auto outAllocator = plugin_->GetAllocator(); // zero copy need change to use sink allocator
    if (outAllocator == nullptr) {
        MEDIA_LOG_I("plugin doest not support out allocator, using framework allocator");
        outBufPool_->Init(bufferSize, Plugin::BufferMetaType::VIDEO);
    } else {
        MEDIA_LOG_I("using plugin output allocator");
        for (size_t cnt = 0; cnt < bufferCnt; cnt++) {
            auto buf = CppExt::make_unique<AVBuffer>(Plugin::BufferMetaType::VIDEO);
            buf->AllocMemory(outAllocator, bufferSize);
            outBufPool_->Append(std::move(buf));
        }
    }
    return ErrorCode::SUCCESS;
}

ErrorCode VideoEncoderFilter::SetVideoEncoderFormat(const std::shared_ptr<const Plugin::Meta>& meta)
{
    if (!meta->GetData<Plugin::VideoPixelFormat>(Plugin::MetaID::VIDEO_PIXEL_FORMAT, vencFormat_.format)) {
        MEDIA_LOG_E("Get video pixel format fail");
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    if (!meta->GetUint32(Plugin::MetaID::VIDEO_WIDTH, vencFormat_.width)) {
        MEDIA_LOG_E("Get video width fail");
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    if (!meta->GetUint32(Plugin::MetaID::VIDEO_HEIGHT, vencFormat_.height)) {
        MEDIA_LOG_E("Get video width height");
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    if (!meta->GetInt64(Plugin::MetaID::MEDIA_BITRATE, vencFormat_.bitRate)) {
        MEDIA_LOG_D("Do not have codec bit rate");
    }
    if (!meta->GetUint32(Plugin::MetaID::VIDEO_FRAME_RATE, vencFormat_.frameRate)) {
        MEDIA_LOG_D("Do not have codec frame rate");
    }
    if (!meta->GetData(Plugin::MetaID::MIME, vencFormat_.mime)) {
        MEDIA_LOG_D("Do not have codec mime");
    }
    // Optional: codec extra data
    if (!meta->GetData<std::vector<uint8_t>>(Plugin::MetaID::MEDIA_CODEC_CONFIG, vencFormat_.codecConfig)) {
        MEDIA_LOG_D("Do not have codec extra data");
    }
    return ErrorCode::SUCCESS;
}

ErrorCode VideoEncoderFilter::ConfigurePluginParams()
{
    FALSE_RETURN_V_MSG_W(SetPluginParameterLocked(Tag::VIDEO_WIDTH, vencFormat_.width) == ErrorCode::SUCCESS,
                         ErrorCode::ERROR_UNKNOWN, "Set width to plugin fail");
    FALSE_RETURN_V_MSG_W(SetPluginParameterLocked(Tag::VIDEO_HEIGHT, vencFormat_.height) == ErrorCode::SUCCESS,
                         ErrorCode::ERROR_UNKNOWN, "Set height to plugin fail");
    FALSE_RETURN_V_MSG_W(SetPluginParameterLocked(Tag::VIDEO_PIXEL_FORMAT, vencFormat_.format) == ErrorCode::SUCCESS,
                         ErrorCode::ERROR_UNKNOWN, "Set pixel format to plugin fail");
    if (vencFormat_.bitRate > 0) {
        auto ret = SetPluginParameterLocked(Tag::MEDIA_BITRATE, vencFormat_.bitRate);
        FALSE_RETURN_V_MSG_W(ret == ErrorCode::SUCCESS, ErrorCode::ERROR_UNKNOWN,
                             "Set bitrate to plugin fail");
    }
    if (vencFormat_.frameRate > 0) {
        auto ret = SetPluginParameterLocked(Tag::VIDEO_FRAME_RATE, vencFormat_.frameRate);
        FALSE_RETURN_V_MSG_W(ret == ErrorCode::SUCCESS, ErrorCode::ERROR_UNKNOWN,
                             "Set framerate to plugin fail");
    }
    if (codecMeta_->GetData<Plugin::VideoH264Profile>(Plugin::MetaID::VIDEO_H264_PROFILE, vencFormat_.profile)) {
        auto ret = SetPluginParameterLocked(Tag::VIDEO_H264_PROFILE, vencFormat_.profile);
        FALSE_RETURN_V_MSG_W(ret == ErrorCode::SUCCESS, ErrorCode::ERROR_UNKNOWN,
                             "Set profile to plugin fail");
    }
    if (codecMeta_->GetUint32(Plugin::MetaID::VIDEO_H264_LEVEL, vencFormat_.level)) {
        auto ret = SetPluginParameterLocked(Tag::VIDEO_H264_LEVEL, vencFormat_.level);
        FALSE_RETURN_V_MSG_W(ret == ErrorCode::SUCCESS, ErrorCode::ERROR_UNKNOWN,
                             "Set level to plugin fail");
    }
    // Optional: codec extra data
    if (vencFormat_.codecConfig.size() > 0) {
        if (SetPluginParameterLocked(Tag::MEDIA_CODEC_CONFIG, std::move(vencFormat_.codecConfig)) !=
            ErrorCode::SUCCESS) {
            MEDIA_LOG_W("Set extradata to plugin fail");
        }
    }
    MEDIA_LOG_D("ConfigurePluginParams success, mime: " PUBLIC_LOG_S ", width: " PUBLIC_LOG_U32 ", height: "
                PUBLIC_LOG_U32 ", format: " PUBLIC_LOG_S ", bitRate: " PUBLIC_LOG_D64 ", frameRate: " PUBLIC_LOG_U32,
                vencFormat_.mime.c_str(), vencFormat_.width, vencFormat_.height,
                GetVideoPixelFormatNameStr(vencFormat_.format), vencFormat_.bitRate, vencFormat_.frameRate);
    return ErrorCode::SUCCESS;
}

ErrorCode VideoEncoderFilter::ConfigurePluginOutputBuffers()
{
    ErrorCode err = ErrorCode::SUCCESS;
    while (!outBufPool_->Empty()) {
        auto ptr = outBufPool_->AllocateBuffer();
        if (ptr == nullptr) {
            MEDIA_LOG_W("cannot allocate buffer in buffer pool");
            continue;
        }
        err = TranslatePluginStatus(plugin_->QueueOutputBuffer(ptr, -1));
        if (err != ErrorCode::SUCCESS) {
            MEDIA_LOG_E("queue output buffer error");
        }
    }
    return err;
}

ErrorCode VideoEncoderFilter::ConfigurePlugin()
{
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->SetDataCallback(dataCallback_)),
        "Set plugin callback fail");
    FAIL_RETURN_MSG(ConfigurePluginParams(), "Configure plugin params error");
    FAIL_RETURN_MSG(ConfigurePluginOutputBuffers(), "Configure plugin output buffers error");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Prepare()), "Prepare plugin fail");
    return TranslatePluginStatus(plugin_->Start());
}

ErrorCode VideoEncoderFilter::ConfigureNoLocked(const std::shared_ptr<const Plugin::Meta>& meta)
{
    MEDIA_LOG_D("video encoder configure called");
    FAIL_RETURN_MSG(SetVideoEncoderFormat(meta), "Set video encoder format fail");
    FAIL_RETURN_MSG(AllocateOutputBuffers(), "Alloc output buffers fail");
    FAIL_RETURN_MSG(ConfigurePlugin(), "Config plugin fail");
    if (handleFrameTask_) {
        handleFrameTask_->Start();
    }
    if (pushTask_) {
        pushTask_->Start();
    }
    return ErrorCode::SUCCESS;
}

ErrorCode VideoEncoderFilter::PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset)
{
    if (state_ != FilterState::READY && state_ != FilterState::PAUSED && state_ != FilterState::RUNNING) {
        MEDIA_LOG_W("pushing data to encoder when state_ is " PUBLIC_LOG_D32, static_cast<int>(state_.load()));
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    if (isFlushing_) {
        MEDIA_LOG_I("encoder is flushing, discarding this data from port " PUBLIC_LOG_S, inPort.c_str());
        return ErrorCode::SUCCESS;
    }
    inBufQue_->Push(buffer);
    return ErrorCode::SUCCESS;
}

void VideoEncoderFilter::FlushStart()
{
    MEDIA_LOG_I("FlushStart entered");
    isFlushing_ = true;
    if (inBufQue_) {
        inBufQue_->SetActive(false);
    }
    if (handleFrameTask_) {
        handleFrameTask_->PauseAsync();
    }
    if (outBufQue_) {
        outBufQue_->SetActive(false);
    }
    if (pushTask_) {
        pushTask_->PauseAsync();
    }
    if (plugin_ != nullptr) {
        auto err = TranslatePluginStatus(plugin_->Flush());
        if (err != ErrorCode::SUCCESS) {
            MEDIA_LOG_E("encoder plugin flush error");
        }
    }
}

void VideoEncoderFilter::FlushEnd()
{
    MEDIA_LOG_I("FlushEnd entered");
    isFlushing_ = false;
    if (inBufQue_) {
        inBufQue_->SetActive(true);
    }
    if (handleFrameTask_) {
        handleFrameTask_->Start();
    }
    if (outBufQue_) {
        outBufQue_->SetActive(true);
    }
    if (pushTask_) {
        pushTask_->Start();
    }
    if (plugin_) {
        ConfigurePluginOutputBuffers();
    }
}

ErrorCode VideoEncoderFilter::Stop()
{
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Flush()), "Flush plugin fail");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Stop()), "Stop plugin fail");
    isStop_ = true;
    outBufQue_->SetActive(false);
    if (pushTask_) {
        pushTask_->Pause();
    }
    inBufQue_->SetActive(false);
    if (handleFrameTask_) {
        handleFrameTask_->Pause();
    }
    outBufPool_.reset();
    MEDIA_LOG_I("Stop success");
    return FilterBase::Stop();
}

void VideoEncoderFilter::HandleFrame()
{
    MEDIA_LOG_D("HandleFrame called");
    auto oneBuffer = inBufQue_->Pop();
    if (oneBuffer == nullptr) {
        MEDIA_LOG_W("encoder find nullptr in esBufferQ");
        return;
    }
    HandleOneFrame(oneBuffer);
}

void VideoEncoderFilter::HandleOneFrame(const std::shared_ptr<AVBuffer>& data)
{
    MEDIA_LOG_D("HandleOneFrame called");
    Plugin::Status ret;
    do {
        if (isStop_ || isFlushing_) {
            MEDIA_LOG_D("filter stop, exit...");
            break;
        }
        ret = plugin_->QueueInputBuffer(data, -1);
        if (ret == Plugin::Status::OK) {
            break;
        }
        MEDIA_LOG_DD("Send data to plugin error: " PUBLIC_LOG_D32, ret);
        OSAL::SleepFor(DEFAULT_TRY_DECODE_TIME);
    } while (1);
}

void VideoEncoderFilter::FinishFrame()
{
    MEDIA_LOG_D("begin finish frame");
    auto ptr = outBufQue_->Pop();
    if (ptr) {
        auto oPort = outPorts_[0];
        if (oPort->GetWorkMode() == WorkMode::PUSH) {
            oPort->PushData(ptr, -1);
        } else {
            MEDIA_LOG_W("encoder out port works in pull mode");
        }
        ptr.reset();
        auto oPtr = outBufPool_->AllocateBuffer();
        if (oPtr != nullptr) {
            oPtr->Reset();
            plugin_->QueueOutputBuffer(oPtr, 0);
        }
    }
    MEDIA_LOG_D("end finish frame");
}

void VideoEncoderFilter::OnInputBufferDone(const std::shared_ptr<Plugin::Buffer>& buffer)
{
    // do nothing since we has no input buffer pool
}

void VideoEncoderFilter::OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& buffer)
{
    outBufQue_->Push(buffer);
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif