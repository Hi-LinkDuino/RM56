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

#define HST_LOG_TAG "LiteAACDecoderPlugin"

#include "lite_aac_decoder_plugin.h"
#include <cstring>
#include <map>
#include <set>
#include "utils/constants.h"
#include "foundation/log.h"
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_caps_builder.h"
#include "plugin/interface/codec_plugin.h"
#include "securec.h"

namespace {
using namespace OHOS::Media::Plugin;
void UpdatePluginDefinition(CodecPluginDef& definition);
constexpr int8_t RANK_MAX = 100;
constexpr unsigned int BUFFER_ITEM_CNT = 6;

std::shared_ptr<CodecPlugin> LiteAACDecoderCreator(const std::string& name)
{
    return std::make_shared<LiteAacPlugin::LiteAACDecoderPlugin>(name);
}

Status RegisterDecoderPlugin(const std::shared_ptr<Register>& reg)
{
    MEDIA_LOG_D("RegisterDecoderPlugin called.");
    if (!reg) {
        MEDIA_LOG_E("RegisterDecoderPlugin failed due to null pointer for reg.");
        return Status::ERROR_INVALID_PARAMETER;
    }

    CodecPluginDef definition;
    definition.name      = "LiteAACDecoderPlugin";
    definition.codecType = CodecType::AUDIO_DECODER;
    definition.rank      = RANK_MAX;
    definition.creator   = LiteAACDecoderCreator;
    UpdatePluginDefinition(definition);
    if (reg->AddPlugin(definition) != Status::OK) {
        MEDIA_LOG_W("register lite aac decoder plugin failed");
    }
    return Status::OK;
}

void UpdateInCaps(CodecPluginDef& definition)
{
    CapabilityBuilder capBuilder;
    capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_AAC);
    capBuilder.SetAudioSampleRateList({96000, 88200, 64000, 48000, 44100, 32000, 24000,
                                        22050, 16000, 12000, 11025, 8000, 7350});
    capBuilder.SetAudioChannelLayoutList({AudioChannelLayout::MONO,
                                          AudioChannelLayout::STEREO});
    capBuilder.SetAudioSampleFormatList({AudioSampleFormat::S16});
    capBuilder.SetAudioAacProfileList({AudioAacProfile::LC});
    capBuilder.SetAudioAacStreamFormatList({AudioAacStreamFormat::MP4ADTS});
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

PLUGIN_DEFINITION(LiteAACDecoder, LicenseType::APACHE_V2, RegisterDecoderPlugin, [] {});

namespace OHOS {
namespace Media {
namespace Plugin {
namespace LiteAacPlugin {
constexpr unsigned int AAC_DECODER_SAMPLE_SIZE = sizeof(signed short);
constexpr unsigned int PCM_SIZE = 2048 * AAC_DECODER_SAMPLE_SIZE;
constexpr unsigned int FRAME_MAX_COUNT = 16 * 1024;

LiteAACDecoderPlugin::LiteAACDecoderPlugin(std::string name)
    : CodecPlugin(std::move(name)),
      audioParameter_{ { Tag::REQUIRED_OUT_BUFFER_CNT, (unsigned int)BUFFER_ITEM_CNT } },
      inBuffer_ {nullptr},
      outBuffer_ {nullptr}
{
    MEDIA_LOG_I("LiteAACDecoderPlugin, plugin name: " PUBLIC_LOG_S, pluginName_.c_str());
}

LiteAACDecoderPlugin::~LiteAACDecoderPlugin()
{
    MEDIA_LOG_I("~LiteAACDecoderPlugin");
}

Status LiteAACDecoderPlugin::Init()
{
    return Status::OK;
}

Status LiteAACDecoderPlugin::Deinit()
{
    Reset();
    return Status::OK;
}

Status LiteAACDecoderPlugin::SetParameter(Tag tag, const ValueType& value)
{
    audioParameter_.insert(std::make_pair(tag, value));
    return Status::OK;
}

Status LiteAACDecoderPlugin::GetParameter(Tag tag, ValueType& value)
{
    auto item = audioParameter_.find(tag);
    if (item != audioParameter_.end()) {
        value = item->second;
        return Status::OK;
    }
    return Status::ERROR_INVALID_PARAMETER;
}

Status LiteAACDecoderPlugin::Prepare()
{
    OSAL::ScopedLock lock(avMutex_);
    Status ret = AudioDecoderAACMp4Open();
    return ret;
}

Status LiteAACDecoderPlugin::Reset()
{
    OSAL::ScopedLock lock(avMutex_);
    AudioDecoderAACMp4Close();
    audioParameter_.clear();
    if (outBuffer_) {
        outBuffer_.reset();
        outBuffer_ = nullptr;
    }
    return Status::OK;
}

Status LiteAACDecoderPlugin::Start()
{
    return Status::OK;
}

Status LiteAACDecoderPlugin::Stop()
{
    OSAL::ScopedLock lock(avMutex_);
    AudioDecoderAACMp4Close();
    if (outBuffer_) {
        outBuffer_.reset();
        outBuffer_ = nullptr;
    }
    return Status::OK;
}

std::shared_ptr<Allocator> LiteAACDecoderPlugin::GetAllocator()
{
    return nullptr;
}

Status LiteAACDecoderPlugin::SetCallback(Callback* cb)
{
    return Status::OK;
}

Status LiteAACDecoderPlugin::SetDataCallback(DataCallback* dataCallback)
{
    dataCb_ = dataCallback;
    return Status::OK;
}

Status LiteAACDecoderPlugin::QueueInputBuffer(const std::shared_ptr<Buffer> &inputBuffer, int32_t timeoutMs)
{
    MEDIA_LOG_DD("queue input buffer");
    if (inBuffer_ != nullptr) {
        return Status::ERROR_TIMED_OUT;
    }
    (void)timeoutMs;
    if (inputBuffer->IsEmpty() && !(inputBuffer->flag & BUFFER_FLAG_EOS)) {
        MEDIA_LOG_E("decoder does not support fd buffer");
        return Status::ERROR_INVALID_DATA;
    }
    inBuffer_ = inputBuffer;
    return Status::OK;
}

Status LiteAACDecoderPlugin::QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffers, int32_t timeoutMs)
{
    MEDIA_LOG_DD("queue out put");
    (void)timeoutMs;
    if (outBuffer_ != nullptr) {
        return Status::ERROR_TIMED_OUT;
    }
    if (!outputBuffers) {
        return Status::ERROR_INVALID_PARAMETER;
    }
    outBuffer_ = outputBuffers;
    return SendOutputBuffer();
}

Status LiteAACDecoderPlugin::SendOutputBuffer()
{
    MEDIA_LOG_D("SendOutputBuffer");
    Status status = ReceiveBuffer();
    inBuffer_.reset();
    inBuffer_ = nullptr;
    if (status == Status::OK || status == Status::END_OF_STREAM) {
        dataCb_->OnOutputBufferDone(outBuffer_);
    }
    outBuffer_.reset();
    outBuffer_ = nullptr;

    return status;
}

Status LiteAACDecoderPlugin::ReceiveBufferLocked(const std::shared_ptr<Buffer> &ioInfo)
{
    Status status;
    if (inBuffer_->IsEmpty() && (inBuffer_->flag & BUFFER_FLAG_EOS) != 0) {
        ioInfo->GetMemory()->Reset();
        ioInfo->flag = BUFFER_FLAG_EOS;
        status = Status::END_OF_STREAM;
    } else {
        status = AudioDecoderAACMp4Process(inBuffer_, ioInfo);
    }
    return status;
}

Status LiteAACDecoderPlugin::ReceiveBuffer()
{
    if (inBuffer_ == nullptr) {
        return Status::ERROR_NOT_ENOUGH_DATA;
    }
    std::shared_ptr<Buffer> ioInfo = outBuffer_;
    if ((ioInfo == nullptr) || ioInfo->IsEmpty() || (ioInfo->GetBufferMeta()->GetType() != BufferMetaType::AUDIO)) {
        MEDIA_LOG_W("no buffer");
        return Status::ERROR_NO_MEMORY;
    }
    Status status;
    {
        OSAL::ScopedLock lock(avMutex_);
        status = ReceiveBufferLocked(ioInfo);
    }
    return status;
}

Status LiteAACDecoderPlugin::Flush()
{
    OSAL::ScopedLock lock(avMutex_);
    outBuffer_.reset();
    outBuffer_ = nullptr;
    return Status::OK;
}

Status LiteAACDecoderPlugin::AudioDecoderAACMp4Open()
{
    int ret = aac_decoder_init();
    return ret == -1 ? Status::ERROR_UNKNOWN : Status::OK;
}

void LiteAACDecoderPlugin::AudioDecoderAACMp4Close()
{
    aac_decoder_close();
}

Status LiteAACDecoderPlugin::AudioDecoderAACMp4Process(std::shared_ptr<Buffer> inBuffer,
    std::shared_ptr<Buffer> outBuffer)
{
    if (inBuffer == nullptr || outBuffer == nullptr) {
        return Status::ERROR_UNKNOWN;
    }
    auto inData  = inBuffer->GetMemory();
    auto outData = outBuffer->GetMemory();
    const uint8_t *FramePtr = const_cast<uint8_t *>(inData->GetReadOnlyData());
    size_t inputLength = inData->GetSize();
    unsigned int remain_size = inputLength;
    int err;

    err = aac_decoder_input_data(const_cast<unsigned char**>(&FramePtr),
        static_cast<size_t *>(&inputLength), &remain_size);
    if (err != 0) {
        MEDIA_LOG_E("aac_decoder_input_data error is " PUBLIC_LOG_D32, err);
        return Status::ERROR_UNKNOWN;
    }
    aac_stream_info_t  *aacInfo_ = aac_decoder_get_meida_info();
    if (aacInfo_ == NULL) {
        MEDIA_LOG_E("aacInfo_ get error");
        return Status::ERROR_UNKNOWN;
    }
    unsigned int packet_length = static_cast<unsigned int>(aacInfo_->frame_size) *
        static_cast<unsigned int>(aacInfo_->channel_num) * AAC_DECODER_SAMPLE_SIZE;
    if ((outData->GetCapacity() >= packet_length) && (packet_length <= PCM_SIZE)) {
        err = aac_decoder_get_frame(reinterpret_cast<signed short *>(outData->GetWritableAddr(packet_length, 0)),
            PCM_SIZE / 2, 0); // 2
        if (err != 0) {
            MEDIA_LOG_E("aac_decoder_get_frame error is " PUBLIC_LOG_D32 " remain_size " PUBLIC_LOG_D32, err,
                remain_size);
            return Status::ERROR_UNKNOWN;
        }
    } else {
        MEDIA_LOG_E("outData not enough memory");
        return Status::ERROR_NO_MEMORY;
    }
    return Status::OK;
}
} // LiteAacPlugin
} // namespace Plugin
} // namespace Media
} // namespace OHOS