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

#define HST_LOG_TAG "Minimp3DecoderPlugin"

#include "minimp3_decoder_plugin.h"
#include <cstring>
#include <map>
#include <set>
#include "utils/constants.h"

#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_caps_builder.h"
#include "plugin/interface/codec_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Minimp3 {
namespace {
    constexpr uint32_t MP3_384_SAMPLES_PER_FRAME  = 384;  // 384
    constexpr uint32_t MP3_576_SAMPLES_PER_FRAME  = 576;  // 576
    constexpr uint32_t MP3_MAX_SAMPLES_PER_FRAME  = 1152; // 1152
    constexpr uint32_t BUFFER_ITEM_CNT            = 6;    // 6
    constexpr uint32_t MAX_RANK                   = 100;  // 100
}

Minimp3DecoderPlugin::Minimp3DecoderPlugin(std::string name)
    : CodecPlugin(std::move(name)),
      samplesPerFrame_(0),
      channels_(0),
      mp3Parameter_()
{
    FALSE_LOG(memset_s(&mp3DecoderAttr_, sizeof(mp3DecoderAttr_), 0x00, sizeof(AudioDecoderMp3Attr)) == 0);
    FALSE_LOG(memset_s(&minimp3DecoderImpl_, sizeof(minimp3DecoderImpl_), 0x00, sizeof(Minimp3DemuxerOp)) == 0);
    MEDIA_LOG_I("Minimp3DecoderPlugin, plugin name: " PUBLIC_LOG_S, pluginName_.c_str());
}

Minimp3DecoderPlugin::~Minimp3DecoderPlugin()
{
    MEDIA_LOG_I("~Minimp3DecoderPlugin, plugin name: " PUBLIC_LOG_S, pluginName_.c_str());
}

Status Minimp3DecoderPlugin::Init()
{
    minimp3DecoderImpl_ = MiniMp3GetOpt();
    AudioDecoderMp3Open();
    mp3Parameter_[Tag::REQUIRED_OUT_BUFFER_CNT] = BUFFER_ITEM_CNT;
    mp3Parameter_[Tag::AUDIO_SAMPLE_PER_FRAME] = MP3_MAX_SAMPLES_PER_FRAME;
    return Status::OK;
}

Status Minimp3DecoderPlugin::Deinit()
{
    AudioDecoderMp3Close();
    return Status::OK;
}

Status Minimp3DecoderPlugin::SetParameter(Tag tag, const ValueType& value)
{
    mp3Parameter_.insert(std::make_pair(tag, value));
    return Status::OK;
}

Status Minimp3DecoderPlugin::GetParameter(Tag tag, ValueType& value)
{
    auto res = mp3Parameter_.find(tag);
    if (res != mp3Parameter_.end()) {
        value = res->second;
        return Status::OK;
    }
    return Status::ERROR_INVALID_PARAMETER;
}

Status Minimp3DecoderPlugin::Prepare()
{
    if (mp3Parameter_.find(Tag::AUDIO_CHANNELS) != mp3Parameter_.end()) {
        channels_ = AnyCast<uint32_t>((mp3Parameter_.find(Tag::AUDIO_CHANNELS))->second);
    }
    if (mp3Parameter_.find(Tag::AUDIO_SAMPLE_PER_FRAME) != mp3Parameter_.end()) {
        samplesPerFrame_ = AnyCast<uint32_t>(mp3Parameter_.find(Tag::AUDIO_SAMPLE_PER_FRAME)->second);
    }
    if (samplesPerFrame_ != MP3_384_SAMPLES_PER_FRAME && samplesPerFrame_ != MP3_576_SAMPLES_PER_FRAME &&
        samplesPerFrame_ != MP3_MAX_SAMPLES_PER_FRAME) {
        return Status::ERROR_INVALID_PARAMETER;
    }
    MEDIA_LOG_I("channels_ = " PUBLIC_LOG_D32 " samplesPerFrame_ = " PUBLIC_LOG_D32, channels_, samplesPerFrame_);
    return Status::OK;
}

Status Minimp3DecoderPlugin::Reset()
{
    mp3Parameter_.clear();
    return Status::OK;
}

Status Minimp3DecoderPlugin::Start()
{
    return Status::OK;
}

Status Minimp3DecoderPlugin::Stop()
{
    return Status::OK;
}

std::shared_ptr<Allocator> Minimp3DecoderPlugin::GetAllocator()
{
    return nullptr;
}

Status Minimp3DecoderPlugin::SetCallback(Callback* cb)
{
    return Status::OK;
}

Status Minimp3DecoderPlugin::GetPcmDataProcess(const std::shared_ptr<Buffer>& inputBuffer,
                                               std::shared_ptr<Buffer>& outputBuffer)
{
    if (inputBuffer == nullptr) {
        return Status::ERROR_NOT_ENOUGH_DATA;
    }
    if (outputBuffer == nullptr || outputBuffer->IsEmpty()) {
        MEDIA_LOG_W("outputBuffer nullptr warning");
        return Status::ERROR_INVALID_PARAMETER;
    }
    if (inputBuffer->IsEmpty() && (inputBuffer->flag & BUFFER_FLAG_EOS) != 0) {
        MEDIA_LOG_I("eos received");
        outputBuffer->GetMemory()->Reset();
        outputBuffer->flag |= BUFFER_FLAG_EOS;
        return Status::END_OF_STREAM;
    }
    return AudioDecoderMp3Process(inputBuffer_, outputBuffer);
}

Status Minimp3DecoderPlugin::QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffers, int32_t timeoutMs)
{
    MEDIA_LOG_DD("queue output buffer");
    (void)timeoutMs;
    if (!outputBuffers) {
        return Status::ERROR_INVALID_PARAMETER;
    }
    outputBuffer_ = outputBuffers;
    return SendOutputBuffer();
}

Status Minimp3DecoderPlugin::SendOutputBuffer()
{
    MEDIA_LOG_DD("send output buffer");
    OSAL::ScopedLock lock(ioMutex_);
    Status status = GetPcmDataProcess(inputBuffer_, outputBuffer_);
    inputBuffer_.reset();
    inputBuffer_ = nullptr;
    if (status == Status::OK || status == Status::END_OF_STREAM) {
        dataCb_->OnOutputBufferDone(outputBuffer_);
    }
    outputBuffer_.reset();
    outputBuffer_ = nullptr;
    return status;
}

Status Minimp3DecoderPlugin::QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs)
{
    MEDIA_LOG_DD("queue input buffer");
    (void)timeoutMs;
    if (inputBuffer->IsEmpty() && !(inputBuffer->flag & BUFFER_FLAG_EOS)) {
        MEDIA_LOG_E("decoder does not support fd buffer");
        return Status::ERROR_INVALID_DATA;
    } else {
        inputBuffer_ = inputBuffer;
        return Status::OK;
    }
}

Status Minimp3DecoderPlugin::Flush()
{
    return Status::OK;
}

Status Minimp3DecoderPlugin::SetDataCallback(DataCallback* dataCallback)
{
    dataCb_ = dataCallback;
    return Status::OK;
}

void Minimp3DecoderPlugin::AudioDecoderMp3Open()
{
    Minimp3WrapperMp3decInit(&mp3DecoderAttr_.mp3DecoderHandle);
}

int Minimp3DecoderPlugin::AudioDecoderMp3Close()
{
    return 0;
}

Status Minimp3DecoderPlugin::AudioDecoderMp3Process(std::shared_ptr<Buffer> inBuffer, std::shared_ptr<Buffer> outBuffer)
{
    auto inData  = inBuffer->GetMemory();
    auto outData = outBuffer->GetMemory();
    Minimp3WrapperMp3decFrameInfo frameInfo;

    uint32_t probePcmLength = samplesPerFrame_ * sizeof(Mp3DecoderSample) * channels_;
    if (outData->GetCapacity() < probePcmLength) {
        return Status::ERROR_UNKNOWN;
    }
    int16_t *pcmPtr = (int16_t *)outData->GetWritableAddr(probePcmLength, 0);
    int sampleCount = minimp3DecoderImpl_.decoderFrame(&mp3DecoderAttr_.mp3DecoderHandle, inData->GetReadOnlyData(),
                                                       inData->GetSize(), pcmPtr, &frameInfo);
    outBuffer->pts = inBuffer->pts;
    if (sampleCount > 0) {
        if (frameInfo.frame_bytes) {
            return Status::OK;
        }
    } else if (sampleCount == 0) {
        return Status::OK;
    }
    return Status::ERROR_UNKNOWN;
}

namespace {
void UpdatePluginDefinition(CodecPluginDef& definition);
std::shared_ptr<CodecPlugin> Minimp3DecoderCreator(const std::string& name)
{
    return std::make_shared<Minimp3DecoderPlugin>(name);
}

Status RegisterDecoderPlugin(const std::shared_ptr<Register>& reg)
{
    MEDIA_LOG_I("RegisterPlugins called.");
    if (!reg) {
        MEDIA_LOG_E("RegisterPlugins failed due to nullptr pointer for reg.");
        return Status::ERROR_INVALID_PARAMETER;
    }

    CodecPluginDef definition;
    definition.name      = "Minimp3DecoderPlugin";
    definition.codecType = CodecType::AUDIO_DECODER;
    definition.rank      = MAX_RANK;
    definition.creator   = Minimp3DecoderCreator;
    UpdatePluginDefinition(definition);
    if (reg->AddPlugin(definition) != Status::OK) {
        MEDIA_LOG_W("register minimp3 decoder plugin failed");
    }
    return Status::OK;
}

void UpdateInCaps(CodecPluginDef& definition)
{
    CapabilityBuilder capBuilder;
    capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_MPEG)
        .SetAudioMpegVersion(1)
        .SetAudioMpegLayerRange(1, 3); // 3
    DiscreteCapability<uint32_t> values = {8000, 16000, 22050, 44100, 48000, 32000}; // 8000, 16000 etc. sample rates
    capBuilder.SetAudioSampleRateList(values);
    DiscreteCapability<AudioChannelLayout> channelLayoutValues = {
        AudioChannelLayout::MONO, AudioChannelLayout::STEREO};
    capBuilder.SetAudioChannelLayoutList(channelLayoutValues);
    definition.inCaps.push_back(capBuilder.Build());
}

void UpdateOutCaps(CodecPluginDef& definition)
{
    CapabilityBuilder capBuilder;
    capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_RAW);
    capBuilder.SetAudioSampleFormatList({AudioSampleFormat::S16});
    definition.outCaps.emplace_back(capBuilder.Build());
}

void UpdatePluginDefinition(CodecPluginDef& definition)
{
    UpdateInCaps(definition);
    UpdateOutCaps(definition);
}
}

PLUGIN_DEFINITION(Minimp3Decoder, LicenseType::CC0, RegisterDecoderPlugin, [] {});
} // namespace Minimp3
} // namespace Plugin
} // namespace Media
} // namespace OHOS