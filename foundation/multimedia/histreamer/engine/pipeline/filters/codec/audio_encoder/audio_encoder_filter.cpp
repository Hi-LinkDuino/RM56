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

#ifdef RECORDER_SUPPORT

#define HST_LOG_TAG "AudioEncoderFilter"

#include "audio_encoder_filter.h"
#include "factory/filter_factory.h"
#include "foundation/cpp_ext/memory_ext.h"
#include "osal/utils/util.h"
#include "utils/steady_clock.h"

#define DEFAULT_OUT_BUFFER_POOL_SIZE 5
#define MAX_OUT_DECODED_DATA_SIZE_PER_FRAME 20 * 1024 // 20kB

namespace OHOS {
namespace Media {
namespace Pipeline {
static AutoRegisterFilter<AudioEncoderFilter> g_registerFilterHelper("builtin.recorder.audioencoder");

AudioEncoderFilter::AudioEncoderFilter(const std::string& name) : CodecFilterBase(name)
{
    filterType_ = FilterType::AUDIO_ENCODER;
    MEDIA_LOG_D("audio encoder ctor called");
}

AudioEncoderFilter::~AudioEncoderFilter()
{
    MEDIA_LOG_D("audio encoder dtor called");
    Release();
}

ErrorCode AudioEncoderFilter::Start()
{
    MEDIA_LOG_I("audio encoder start called");
    if (state_ != FilterState::READY && state_ != FilterState::PAUSED) {
        MEDIA_LOG_W("call encoder start() when state is not ready or working");
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    rb_->SetActive(true);
    return FilterBase::Start();
}

ErrorCode AudioEncoderFilter::SetAudioEncoder(int32_t sourceId, std::shared_ptr<Plugin::Meta> encoderMeta)
{
    std::string mime;
    FALSE_RETURN_V_MSG_E(encoderMeta->GetString(Plugin::MetaID::MIME, mime), ErrorCode::ERROR_INVALID_PARAMETER_VALUE,
                         "encoder meta must contains mime");
    mime_ = mime;
    encoderMeta_ = std::move(encoderMeta);
    return ErrorCode::SUCCESS;
}

bool AudioEncoderFilter::Negotiate(const std::string& inPort,
                                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                                   Plugin::Capability& negotiatedCap,
                                   const Plugin::TagMap& upstreamParams,
                                   Plugin::TagMap& downstreamParams)
{
    PROFILE_BEGIN("Audio Encoder Negotiate begin");
    FALSE_RETURN_V_MSG_E(state_ == FilterState::PREPARING, false, "not preparing when negotiate");
    auto targetOutPort = GetRouteOutPort(inPort);
    FALSE_RETURN_V_MSG_E(targetOutPort != nullptr, false, "out port not found");
    std::shared_ptr<Plugin::PluginInfo> selectedPluginInfo = nullptr;
    bool atLeastOutCapMatched = false;
    auto candidatePlugins = FindAvailablePlugins(*upstreamCap, Plugin::PluginType::CODEC);
    for (const auto& candidate : candidatePlugins) {
        FALSE_LOG_MSG(!candidate.first->outCaps.empty(), "encoder plugin must have out caps");
        for (const auto& outCap : candidate.first->outCaps) { // each codec plugin should have at least one out cap
            Plugin::Meta tmpMeta;
            if (outCap.mime != mime_ || !MergeMetaWithCapability(*encoderMeta_, outCap, tmpMeta)) {
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
        "can't find available encoder plugin with " PUBLIC_LOG_S, Capability2String(*upstreamCap).c_str());
    auto res = UpdateAndInitPluginByInfo<Plugin::Codec>(plugin_, pluginInfo_, selectedPluginInfo,
        [](const std::string& name)-> std::shared_ptr<Plugin::Codec> {
        return Plugin::PluginManager::Instance().CreateCodecPlugin(name);
    });
    negotiatedCap = *upstreamCap;
    plugin_->SetDataCallback(this);
    PROFILE_END("audio encoder negotiate end");
    return res;
}

uint32_t AudioEncoderFilter::CalculateBufferSize(const std::shared_ptr<const Plugin::Meta>& meta)
{
    Plugin::ValueType value;
    if (plugin_->GetParameter(Plugin::Tag::AUDIO_SAMPLE_PER_FRAME, value) != Plugin::Status::OK ||
        !value.SameTypeWith(typeid(uint32_t))) {
        MEDIA_LOG_E("Get samplePerFrame from plugin fail");
        return 0;
    }
    auto samplesPerFrame = Plugin::AnyCast<uint32_t>(value);
    uint32_t channels;
    if (!meta->GetUint32(Plugin::MetaID::AUDIO_CHANNELS, channels)) {
        return 0;
    }
    Plugin::AudioSampleFormat format;
    if (!meta->GetData<Plugin::AudioSampleFormat>(Plugin::MetaID::AUDIO_SAMPLE_FORMAT, format)) {
        return 0;
    }
    return GetBytesPerSample(format) * samplesPerFrame * channels;
}

bool AudioEncoderFilter::Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    PROFILE_BEGIN("Audio encoder configure begin");
    MEDIA_LOG_I("receive upstream meta " PUBLIC_LOG_S, Meta2String(*upstreamMeta).c_str());
    FALSE_RETURN_V_MSG_E(plugin_ != nullptr && pluginInfo_ != nullptr, false,
        "can't configure encoder when no plugin available");
    auto thisMeta = std::make_shared<Plugin::Meta>();
    // todo how to decide the caps ?
    FALSE_RETURN_V_MSG_E(MergeMetaWithCapability(*upstreamMeta, pluginInfo_->outCaps[0], *thisMeta), false,
        "can't configure encoder plugin since meta is not compatible with negotiated caps");
    auto targetOutPort = GetRouteOutPort(inPort);
    FALSE_RETURN_V_MSG_E(targetOutPort != nullptr, false, "encoder out port is not found");
    auto err = ConfigureToStartPluginLocked(thisMeta);
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("encoder configure error");
        OnEvent({name_, EventType::EVENT_ERROR, err});
        return false;
    }
    FAIL_LOG(UpdateMetaFromPlugin(*thisMeta));
    FALSE_RETURN_V_MSG_E(targetOutPort->Configure(thisMeta), false, "fail to configure downstream");
    state_ = FilterState::READY;
    OnEvent({name_, EventType::EVENT_READY});
    MEDIA_LOG_I("audio encoder send EVENT_READY");
    PROFILE_END("Audio encoder configure end");
    return true;
}

ErrorCode AudioEncoderFilter::ConfigureToStartPluginLocked(const std::shared_ptr<const Plugin::Meta>& meta)
{
    FAIL_RETURN_MSG(ConfigPluginWithMeta(*plugin_, *meta), "configure encoder plugin error");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Prepare()), "encoder prepare failed");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Start()), "encoder start failed");

    uint32_t bufferCnt = 0;
    if (GetPluginParameterLocked(Tag::REQUIRED_OUT_BUFFER_CNT, bufferCnt) != ErrorCode::SUCCESS) {
        bufferCnt = DEFAULT_OUT_BUFFER_POOL_SIZE;
    }
    // 每次重新创建bufferPool
    outBufferPool_ = std::make_shared<BufferPool<AVBuffer>>(bufferCnt);
    frameSize_ = CalculateBufferSize(meta);
    if (frameSize_ == 0) {
        frameSize_ = MAX_OUT_DECODED_DATA_SIZE_PER_FRAME;
    }
    auto outAllocator = plugin_->GetAllocator();
    if (outAllocator == nullptr) {
        MEDIA_LOG_I("plugin doest not support out allocator, using framework allocator");
        outBufferPool_->Init(frameSize_);
    } else {
        MEDIA_LOG_I("using plugin output allocator");
        for (size_t cnt = 0; cnt < bufferCnt; cnt++) {
            auto buf = CppExt::make_unique<AVBuffer>();
            buf->AllocMemory(outAllocator, frameSize_);
            outBufferPool_->Append(std::move(buf));
        }
    }
    rb_ = CppExt::make_unique<RingBuffer>(frameSize_ * 10); // 最大缓存10帧
    FALSE_RETURN_V_MSG_E(rb_ != nullptr, ErrorCode::ERROR_NO_MEMORY, "create ring buffer failed");
    rb_->Init();
    cahceBuffer_ = std::make_shared<AVBuffer>(Plugin::BufferMetaType::AUDIO);
    FALSE_RETURN_V_MSG_E(cahceBuffer_->AllocMemory(nullptr, frameSize_) != nullptr, ErrorCode::ERROR_NO_MEMORY,
                         "alloc cache mem failed");
    return ErrorCode::SUCCESS;
}

ErrorCode AudioEncoderFilter::PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset)
{
    const static int8_t maxRetryCnt = 3; // max retry times of handling one frame
    if (state_ != FilterState::READY && state_ != FilterState::PAUSED && state_ != FilterState::RUNNING) {
        MEDIA_LOG_W("pushing data to encoder when state is " PUBLIC_LOG_D32, static_cast<int>(state_.load()));
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    auto inputMemory = buffer->GetMemory();
    if (inputMemory && inputMemory->GetSize() > 0) {
        rb_->WriteBuffer(const_cast<uint8_t *>(inputMemory->GetReadOnlyData()), inputMemory->GetSize());
    }
    bool shouldDrainRb = buffer->flag & BUFFER_FLAG_EOS;
    bool isRbDrained = false;
    for (auto available = rb_->GetSize(); (available >= frameSize_) || (shouldDrainRb && !isRbDrained);
         available = rb_->GetSize()) {
        cahceBuffer_->Reset();
        auto encodeSize = std::min(available, frameSize_);
        if (encodeSize > 0) { // ring buffer has buffer available
            if (rb_->ReadBuffer(cahceBuffer_->GetMemory()->GetWritableAddr(encodeSize), encodeSize) != encodeSize) {
                MEDIA_LOG_E("Read data from ring buffer fail");
                return ErrorCode::ERROR_UNKNOWN;
            }
        } else { // EOS
            cahceBuffer_->flag |= BUFFER_FLAG_EOS;
            isRbDrained = true;
        }
        ErrorCode handleFrameRes;
        int8_t retryCnt = 0;
        do {
            handleFrameRes = HandleFrame(cahceBuffer_);
            while (FinishFrame() == ErrorCode::SUCCESS) {
                MEDIA_LOG_DD("finish frame");
            }
            retryCnt++;
            if (retryCnt >= maxRetryCnt) { // if retry cnt exceeds we will drop this frame
                break;
            }
            // if timed out or returns again we should try again
        } while (handleFrameRes == ErrorCode::ERROR_TIMED_OUT || handleFrameRes == ErrorCode::ERROR_AGAIN);
    }
    return ErrorCode::SUCCESS;
}

ErrorCode AudioEncoderFilter::Stop()
{
    MEDIA_LOG_I("AudioEncoderFilter stop start.");
    // 先改变底层状态 然后停掉上层线程 否则会产生死锁
    if (plugin_ != nullptr) {
        FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Flush()), "encoder flush error");
        FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Stop()), "encoder stop error");
    }
    if (rb_) {
        rb_->SetActive(false);
    }
    MEDIA_LOG_I("AudioEncoderFilter stop end.");
    return FilterBase::Stop();
}

ErrorCode AudioEncoderFilter::Release()
{
    if (plugin_) {
        plugin_->Stop();
        plugin_->Deinit();
    }
    return ErrorCode::SUCCESS;
}

ErrorCode AudioEncoderFilter::HandleFrame(const std::shared_ptr<AVBuffer>& buffer)
{
    MEDIA_LOG_DD("HandleFrame called");
    auto ret = TranslatePluginStatus(plugin_->QueueInputBuffer(buffer, 0));
    FALSE_LOG_MSG(ret == ErrorCode::SUCCESS || ret == ErrorCode::ERROR_TIMED_OUT,
                    "Queue input buffer to plugin fail: " PUBLIC_LOG_D32, ret);
    return ret;
}

ErrorCode AudioEncoderFilter::FinishFrame()
{
    MEDIA_LOG_DD("begin finish frame");
    auto outBuffer = outBufferPool_->AllocateAppendBufferNonBlocking();
    FALSE_RETURN_V_MSG_E(outBuffer != nullptr, ErrorCode::ERROR_NO_MEMORY, "Get out buffer from buffer pool fail");
    outBuffer->Reset();
    auto status = plugin_->QueueOutputBuffer(outBuffer, 0);
    if (status != Plugin::Status::OK && status != Plugin::Status::END_OF_STREAM) {
        if (status != Plugin::Status::ERROR_NOT_ENOUGH_DATA) {
            MEDIA_LOG_E("Queue output buffer to plugin fail: " PUBLIC_LOG_D32, static_cast<int32_t>((status)));
        }
    }
    MEDIA_LOG_DD("end finish frame");
    return TranslatePluginStatus(status);
}

void AudioEncoderFilter::OnInputBufferDone(const std::shared_ptr<Plugin::Buffer>& input)
{
    MEDIA_LOG_DD("AudioEncoderFilter::OnInputBufferDone");
}

void AudioEncoderFilter::OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& output)
{
    FALSE_RETURN(output != nullptr);

    // push to port
    auto oPort = outPorts_[0];
    if (oPort->GetWorkMode() == WorkMode::PUSH) {
        oPort->PushData(output, -1);
    } else {
        MEDIA_LOG_W("encoder out port works in pull mode");
    }

    // 释放buffer 如果没有被缓存使其回到buffer pool 如果被sink缓存 则从buffer pool拿其他的buffer
    std::const_pointer_cast<Plugin::Buffer>(output).reset();
}
} // Pipeline
} // Media
} // OHOS
#endif // RECORDER_SUPPORT