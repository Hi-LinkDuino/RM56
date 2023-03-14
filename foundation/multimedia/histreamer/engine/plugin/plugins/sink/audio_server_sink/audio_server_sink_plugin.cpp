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

#define HST_LOG_TAG "AudioServerSinkPlugin"

#include "audio_server_sink_plugin.h"
#include <functional>
#include "audio_errors.h"
#include "foundation/cpp_ext/algorithm_ext.h"
#include "foundation/log.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "foundation/osal/utils/util.h"
#include "plugin/common/plugin_time.h"
#include "utils/constants.h"
#include "pipeline/core/plugin_attr_desc.h"

namespace {
using namespace OHOS::Media::Plugin;
constexpr uint32_t DEFAULT_OUTPUT_CHANNELS = 2;
constexpr AudioChannelLayout DEFAULT_OUTPUT_CHANNEL_LAYOUT = AudioChannelLayout::STEREO;

const std::pair<OHOS::AudioStandard::AudioSamplingRate, uint32_t> g_auSampleRateMap[] = {
    {OHOS::AudioStandard::SAMPLE_RATE_8000, 8000},
    {OHOS::AudioStandard::SAMPLE_RATE_11025, 11025},
    {OHOS::AudioStandard::SAMPLE_RATE_12000, 12000},
    {OHOS::AudioStandard::SAMPLE_RATE_16000, 16000},
    {OHOS::AudioStandard::SAMPLE_RATE_22050, 22050},
    {OHOS::AudioStandard::SAMPLE_RATE_24000, 24000},
    {OHOS::AudioStandard::SAMPLE_RATE_32000, 32000},
    {OHOS::AudioStandard::SAMPLE_RATE_44100, 44100},
    {OHOS::AudioStandard::SAMPLE_RATE_48000, 48000},
    {OHOS::AudioStandard::SAMPLE_RATE_64000, 64000},
    {OHOS::AudioStandard::SAMPLE_RATE_96000, 96000},
};

const std::vector<std::tuple<AudioSampleFormat, OHOS::AudioStandard::AudioSampleFormat, AVSampleFormat>> g_aduFmtMap = {
    {AudioSampleFormat::S8, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::U8, OHOS::AudioStandard::AudioSampleFormat::SAMPLE_U8, AV_SAMPLE_FMT_U8},
    {AudioSampleFormat::S8P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::U8P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_U8P},
    {AudioSampleFormat::S16, OHOS::AudioStandard::AudioSampleFormat::SAMPLE_S16LE, AV_SAMPLE_FMT_S16},
    {AudioSampleFormat::U16, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::S16P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_S16P},
    {AudioSampleFormat::U16P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::S24, OHOS::AudioStandard::AudioSampleFormat::SAMPLE_S24LE, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::U24, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::S24P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::U24P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::S32, OHOS::AudioStandard::AudioSampleFormat::SAMPLE_S32LE, AV_SAMPLE_FMT_S32},
    {AudioSampleFormat::U32, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::S32P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_S32P},
    {AudioSampleFormat::U32P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::F32, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_FLT},
    {AudioSampleFormat::F32P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_FLTP},
    {AudioSampleFormat::F64, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_DBL},
    {AudioSampleFormat::F64P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_DBLP},
    {AudioSampleFormat::S64, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_S64},
    {AudioSampleFormat::U64, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
    {AudioSampleFormat::S64P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_S64P},
    {AudioSampleFormat::U64P, OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH, AV_SAMPLE_FMT_NONE},
};

const std::pair<OHOS::AudioStandard::AudioChannel, uint32_t> g_auChannelsMap[] = {
    {OHOS::AudioStandard::MONO, 1},
    {OHOS::AudioStandard::STEREO, 2},
};

bool SampleRateEnum2Num (OHOS::AudioStandard::AudioSamplingRate enumVal, uint32_t& numVal)
{
    for (const auto& item : g_auSampleRateMap) {
        if (item.first == enumVal) {
            numVal = item.second;
            return true;
        }
    }
    numVal = 0;
    return false;
}

bool SampleRateNum2Enum (uint32_t numVal, OHOS::AudioStandard::AudioSamplingRate& enumVal)
{
    for (const auto& item : g_auSampleRateMap) {
        if (item.second == numVal) {
            enumVal = item.first;
            return true;
        }
    }
    return false;
}

bool ChannelNumNum2Enum(uint32_t numVal, OHOS::AudioStandard::AudioChannel& enumVal)
{
    for (const auto& item : g_auChannelsMap) {
        if (item.second == numVal) {
            enumVal = item.first;
            return true;
        }
    }
    return false;
}

std::shared_ptr<AudioSinkPlugin> AudioServerSinkPluginCreater(const std::string& name)
{
    return std::make_shared<OHOS::Media::Plugin::AuSrSinkPlugin::AudioServerSinkPlugin>(name);
}

void UpdateSupportedSampleRate(Capability& inCaps)
{
    auto supportedSampleRateList = OHOS::AudioStandard::AudioRenderer::GetSupportedSamplingRates();
    if (!supportedSampleRateList.empty()) {
        DiscreteCapability<uint32_t> values;
        for (const auto& rate : supportedSampleRateList) {
            uint32_t sampleRate = 0;
            if (SampleRateEnum2Num(rate, sampleRate)) {
                values.push_back(sampleRate);
            }
        }
        if (!values.empty()) {
            inCaps.AppendDiscreteKeys<uint32_t>(Capability::Key::AUDIO_SAMPLE_RATE, values);
        }
    }
}

void UpdateSupportedSampleFormat(Capability& inCaps)
{
    DiscreteCapability<AudioSampleFormat> values(g_aduFmtMap.size());
    for (const auto& item : g_aduFmtMap) {
        if (std::get<1>(item) != OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH ||
            std::get<2>(item) != AV_SAMPLE_FMT_NONE) {
            values.emplace_back(std::get<0>(item));
        }
    }
    inCaps.AppendDiscreteKeys(Capability::Key::AUDIO_SAMPLE_FORMAT, values);
}

Status AudioServerSinkRegister(const std::shared_ptr<Register>& reg)
{
    AudioSinkPluginDef definition;
    definition.name = "AudioServerSink";
    definition.description = "Audio sink for audio server of media standard";
    definition.rank = 100; // 100: max rank
    definition.creator = AudioServerSinkPluginCreater;
    Capability inCaps(OHOS::Media::MEDIA_MIME_AUDIO_RAW);
    UpdateSupportedSampleRate(inCaps);
    UpdateSupportedSampleFormat(inCaps);
    definition.inCaps.push_back(inCaps);
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(AudioServerSink, LicenseType::APACHE_V2, AudioServerSinkRegister, [] {});

inline void ResetAudioRendererParams(OHOS::AudioStandard::AudioRendererParams& param)
{
    using namespace OHOS::AudioStandard;
    param.sampleFormat = INVALID_WIDTH;
    param.sampleRate = SAMPLE_RATE_8000;
    param.channelCount = MONO;
    param.encodingType = ENCODING_INVALID;
}
} // namespace

namespace OHOS {
namespace Media {
namespace Plugin {
namespace AuSrSinkPlugin {
using namespace OHOS::Media::Plugin;

AudioServerSinkPlugin::AudioServerSinkPlugin(std::string name)
    : Plugin::AudioSinkPlugin(std::move(name)), audioRenderer_(nullptr)
{
    rendererParams_.encodingType = AudioStandard::ENCODING_PCM;
}

AudioServerSinkPlugin::~AudioServerSinkPlugin()
{
    MEDIA_LOG_I("~AudioServerSinkPlugin() entered.");
    ReleaseRender();
}

Status AudioServerSinkPlugin::Init()
{
    MEDIA_LOG_I("Init entered.");
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRenderer_ == nullptr) {
        audioRenderer_ = AudioStandard::AudioRenderer::Create(AudioStandard::AudioStreamType::STREAM_MUSIC);
        if (audioRenderer_ == nullptr) {
            MEDIA_LOG_E("Create audioRenderer_ fail");
            return Status::ERROR_UNKNOWN;
        }
    }
    return Status::OK;
}

void AudioServerSinkPlugin::ReleaseRender()
{
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRenderer_ != nullptr && audioRenderer_->GetStatus() != AudioStandard::RendererState::RENDERER_RELEASED) {
        if (!audioRenderer_->Release()) {
            MEDIA_LOG_W("release audio render failed");
            return;
        }
    }
    audioRenderer_.reset();
}

Status AudioServerSinkPlugin::Deinit()
{
    MEDIA_LOG_I("Deinit entered.");
    ReleaseRender();
    return Status::OK;
}

Status AudioServerSinkPlugin::Prepare()
{
    MEDIA_LOG_I("Prepare entered.");
    FALSE_RETURN_V_MSG_E(fmtSupported_, Status::ERROR_INVALID_PARAMETER, "sample fmt is not supported");
    auto types = AudioStandard::AudioRenderer::GetSupportedEncodingTypes();
    if (!CppExt::AnyOf(types.begin(), types.end(), [](AudioStandard::AudioEncodingType tmp) -> bool {
        return tmp == AudioStandard::ENCODING_PCM;
    })) {
        MEDIA_LOG_E("audio renderer do not support pcm encoding");
        return Status::ERROR_INVALID_PARAMETER;
    }
    MEDIA_LOG_I("set param with fmt " PUBLIC_LOG_D32 " sampleRate " PUBLIC_LOG_D32 " channel " PUBLIC_LOG_D32
        " encode type " PUBLIC_LOG_D32,
        rendererParams_.sampleFormat, rendererParams_.sampleRate, rendererParams_.channelCount,
        rendererParams_.encodingType);
    {
        OSAL::ScopedLock lock(renderMutex_);
        auto ret = audioRenderer_->SetParams(rendererParams_);
        if (ret != AudioStandard::SUCCESS) {
            MEDIA_LOG_E("audio renderer SetParams() fail with " PUBLIC_LOG_D32, ret);
            return Status::ERROR_UNKNOWN;
        }
    }
    if (needReformat_) {
        auto resampleSize = av_samples_get_buffer_size(nullptr, channels_, samplesPerFrame_, reFfDestFmt_, 0);
        resampleCache_ .reserve(resampleSize);
        resampleChannelAddr_.reserve(channels_);
        auto tmp = resampleChannelAddr_.data();
        av_samples_fill_arrays(tmp, nullptr, resampleCache_.data(), channels_, samplesPerFrame_, reFfDestFmt_, 0);
        SwrContext* swrContext = swr_alloc();
        FALSE_RETURN_V_MSG_E(swrContext != nullptr, Status::ERROR_NO_MEMORY, "cannot allocate swr context");
        swrContext = swr_alloc_set_opts(swrContext, static_cast<int64_t>(channelLayout_), reFfDestFmt_, sampleRate_,
            static_cast<int64_t>(channelLayout_), reSrcFfFmt_, sampleRate_, 0, nullptr);
        FALSE_RETURN_V_MSG_E(swr_init(swrContext) == 0, Status::ERROR_UNKNOWN, "swr init error");
        swrCtx_ = std::shared_ptr<SwrContext>(swrContext, [](SwrContext* ptr) {
            if (ptr) {
                swr_free(&ptr);
            }
        });
    }
    return Status::OK;
}

bool AudioServerSinkPlugin::StopRender()
{
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRenderer_) {
        return audioRenderer_->Stop();
    }
    return true;
}

Status AudioServerSinkPlugin::Reset()
{
    MEDIA_LOG_I("Reset entered.");
    if (!StopRender()) {
        MEDIA_LOG_E("stop render error");
        return Status::ERROR_UNKNOWN;
    }
    ResetAudioRendererParams(rendererParams_);
    fmtSupported_ = false;
    reSrcFfFmt_ = AV_SAMPLE_FMT_NONE;
    channels_ = 0;
    bitRate_ = 0;
    sampleRate_ = 0;
    samplesPerFrame_ = 0;
    needReformat_ = false;
    swrCtx_.reset();
    return Status::OK;
}

Status AudioServerSinkPlugin::Start()
{
    MEDIA_LOG_I("Start entered.");
    bool ret = false;
    OSAL::ScopedLock lock(renderMutex_);
    {
        if (audioRenderer_ == nullptr) {
            return Status::ERROR_WRONG_STATE;
        }
        ret = audioRenderer_->Start();
    }
    if (ret) {
        MEDIA_LOG_I("audioRenderer_ Start() success");
        return Status::OK;
    } else {
        MEDIA_LOG_E("audioRenderer_ Start() fail");
    }
    return Status::ERROR_UNKNOWN;
}

Status AudioServerSinkPlugin::Stop()
{
    MEDIA_LOG_I("Stop entered.");
    if (StopRender()) {
        MEDIA_LOG_I("stop render success");
        return Status::OK;
    } else {
        MEDIA_LOG_E("stop render failed");
    }
    return Status::ERROR_UNKNOWN;
}

Status AudioServerSinkPlugin::GetParameter(Tag tag, ValueType& para)
{
    MEDIA_LOG_I("GetParameter entered, key: " PUBLIC_LOG_S, Pipeline::Tag2String(tag));
    AudioStandard::AudioRendererParams params;
    OSAL::ScopedLock lock(renderMutex_);
    switch (tag) {
        case Tag::AUDIO_SAMPLE_RATE:
            if (audioRenderer_ && audioRenderer_->GetParams(params) == AudioStandard::SUCCESS) {
                if (params.sampleRate != rendererParams_.sampleRate) {
                    MEDIA_LOG_W("samplingRate has changed from " PUBLIC_LOG_U32 " to " PUBLIC_LOG_U32,
                                rendererParams_.sampleRate, params.sampleRate);
                }
                para = params.sampleRate;
            }
            break;
        case Tag::AUDIO_OUTPUT_CHANNELS:
            para = DEFAULT_OUTPUT_CHANNELS; // get the real output channels from audio server here
            MEDIA_LOG_I("Get outputChannels: " PUBLIC_LOG_U32, DEFAULT_OUTPUT_CHANNELS);
            break;
        case Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT:
            para = DEFAULT_OUTPUT_CHANNEL_LAYOUT; // get the real output channel layout from audio server here
            MEDIA_LOG_I("Get outputChannelLayout: " PUBLIC_LOG_U64, DEFAULT_OUTPUT_CHANNEL_LAYOUT);
            break;
        case Tag::MEDIA_BITRATE:
            para = bitRate_;
            break;
        case Tag::AUDIO_SAMPLE_FORMAT:
            if (audioRenderer_ && audioRenderer_->GetParams(params) == AudioStandard::SUCCESS) {
                if (params.sampleFormat != rendererParams_.sampleFormat) {
                    MEDIA_LOG_W("sampleFormat has changed from " PUBLIC_LOG_U32 " to " PUBLIC_LOG_U32,
                                rendererParams_.sampleFormat, params.sampleFormat);
                }
                para = params.sampleFormat;
            }
            break;
        default:
            MEDIA_LOG_I("Unknown key");
            break;
    }
    return Status::OK;
}

bool AudioServerSinkPlugin::AssignSampleRateIfSupported(uint32_t sampleRate)
{
    sampleRate_ = sampleRate;
    AudioStandard::AudioSamplingRate aRate = AudioStandard::SAMPLE_RATE_8000;
    if (!SampleRateNum2Enum(sampleRate, aRate)) {
        MEDIA_LOG_E("sample rate " PUBLIC_LOG_U32 "not supported", sampleRate);
        return false;
    }
    auto supportedSampleRateList = OHOS::AudioStandard::AudioRenderer::GetSupportedSamplingRates();
    if (supportedSampleRateList.empty()) {
        MEDIA_LOG_E("GetSupportedSamplingRates() fail");
        return false;
    }
    for (const auto& rate : supportedSampleRateList) {
        if (rate == aRate) {
            rendererParams_.sampleRate = rate;
            MEDIA_LOG_D("sampleRate: " PUBLIC_LOG_U32, rendererParams_.sampleRate);
            return true;
        }
    }
    return false;
}

bool AudioServerSinkPlugin::AssignChannelNumIfSupported(uint32_t channelNum)
{
    AudioStandard::AudioChannel aChannel = AudioStandard::MONO;
    if (!ChannelNumNum2Enum(channelNum, aChannel)) {
        MEDIA_LOG_E("sample rate " PUBLIC_LOG_U32 "not supported", channelNum);
        return false;
    }
    auto supportedChannelsList = OHOS::AudioStandard::AudioRenderer::GetSupportedChannels();
    if (supportedChannelsList.empty()) {
        MEDIA_LOG_E("GetSupportedChannels() fail");
        return false;
    }
    for (const auto& channel : supportedChannelsList) {
        if (channel == aChannel) {
            rendererParams_.channelCount = channel;
            MEDIA_LOG_D("channelCount: " PUBLIC_LOG_U32, rendererParams_.channelCount);
            return true;
        }
    }
    return false;
}

bool AudioServerSinkPlugin::AssignSampleFmtIfSupported(Plugin::AudioSampleFormat sampleFormat)
{
    const auto& item = std::find_if(g_aduFmtMap.begin(), g_aduFmtMap.end(), [&sampleFormat] (const auto& tmp) -> bool {
        return std::get<0>(tmp) == sampleFormat;
    });
    auto stdFmt = std::get<1>(*item);
    if (stdFmt == OHOS::AudioStandard::AudioSampleFormat::INVALID_WIDTH) {
        if (std::get<2>(*item) == AV_SAMPLE_FMT_NONE) { // secondary param
            fmtSupported_ = false;
            return false;
        } else {
            fmtSupported_ = true;
            needReformat_ = true;
            reSrcFfFmt_ = std::get<2>(*item);
            rendererParams_.sampleFormat = reStdDestFmt_;
        }
    } else {
        auto supportedFmts = OHOS::AudioStandard::AudioRenderer::GetSupportedFormats();
        if (CppExt::AnyOf(supportedFmts.begin(), supportedFmts.end(), [&stdFmt](const auto& tmp) -> bool {
            return tmp == stdFmt;
        })) {
            fmtSupported_ = true;
            needReformat_ = false;
            rendererParams_.sampleFormat = stdFmt;
        } else {
            fmtSupported_ = false;
            needReformat_ = false;
            return false;
        }
    }
    return true;
}

Status AudioServerSinkPlugin::SetParameter(Tag tag, const ValueType& para)
{
    MEDIA_LOG_I("SetParameter entered, key: " PUBLIC_LOG_S, Pipeline::Tag2String(tag));
    switch (tag) {
        case Tag::AUDIO_SAMPLE_RATE:
            FALSE_RETURN_V_MSG_E(para.SameTypeWith(typeid(uint32_t)), Status::ERROR_MISMATCHED_TYPE,
                "sample rate type should be uint32_t");
            FALSE_RETURN_V_MSG_E(AssignSampleRateIfSupported(Plugin::AnyCast<uint32_t>(para)),
                Status::ERROR_INVALID_PARAMETER, "sampleRate isn't supported");
            break;
        case Tag::AUDIO_OUTPUT_CHANNELS:
            FALSE_RETURN_V_MSG_E(para.SameTypeWith(typeid(uint32_t)), Status::ERROR_MISMATCHED_TYPE,
                "channels type should be uint32_t");
            channels_ = Plugin::AnyCast<uint32_t>(para);
            MEDIA_LOG_I("Set outputChannels: " PUBLIC_LOG_U32, channels_);
            FALSE_RETURN_V_MSG_E(AssignChannelNumIfSupported(channels_), Status::ERROR_INVALID_PARAMETER,
                "channel isn't supported");
            break;
        case Tag::MEDIA_BITRATE:
            FALSE_RETURN_V_MSG_E(para.SameTypeWith(typeid(int64_t)), Status::ERROR_MISMATCHED_TYPE,
                "bit rate type should be int64_t");
            bitRate_ = Plugin::AnyCast<int64_t>(para);
            break;
        case Tag::AUDIO_SAMPLE_FORMAT:
            FALSE_RETURN_V_MSG_E(para.SameTypeWith(typeid(AudioSampleFormat)), Status::ERROR_MISMATCHED_TYPE,
                "AudioSampleFormat type should be AudioSampleFormat");
            FALSE_RETURN_V_MSG_E(AssignSampleFmtIfSupported(Plugin::AnyCast<AudioSampleFormat>(para)),
                Status::ERROR_INVALID_PARAMETER, "sampleFmt isn't supported by audio renderer or resample lib");
            break;
        case Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT:
            FALSE_RETURN_V_MSG_E(para.SameTypeWith(typeid(AudioChannelLayout)), Status::ERROR_MISMATCHED_TYPE,
                "channel layout type should be AudioChannelLayout");
            channelLayout_ = Plugin::AnyCast<AudioChannelLayout>(para);
            MEDIA_LOG_I("Set outputChannelLayout: " PUBLIC_LOG_U64, channelLayout_);
            break;
        case Tag::AUDIO_SAMPLE_PER_FRAME:
            FALSE_RETURN_V_MSG_E(para.SameTypeWith(typeid(uint32_t)), Status::ERROR_MISMATCHED_TYPE,
                "SAMPLE_PER_FRAME type should be uint32_t");
            samplesPerFrame_ = Plugin::AnyCast<uint32_t>(para);
            break;
        default:
            MEDIA_LOG_I("Unknown key");
            break;
    }
    return Status::OK;
}

Status AudioServerSinkPlugin::GetVolume(float& volume)
{
    MEDIA_LOG_I("GetVolume entered.");
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRenderer_ != nullptr) {
        volume = audioRenderer_->GetVolume();
        return Status::OK;
    }
    return Status::ERROR_WRONG_STATE;
}

Status AudioServerSinkPlugin::SetVolume(float volume)
{
    MEDIA_LOG_I("SetVolume entered.");
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRenderer_ != nullptr) {
        int32_t ret = audioRenderer_->SetVolume(volume);
        if (ret != OHOS::AudioStandard::SUCCESS) {
            MEDIA_LOG_E("set volume failed with code " PUBLIC_LOG_D32, ret);
            return Status::ERROR_UNKNOWN;
        }
        return Status::OK;
    }
    return Status::ERROR_WRONG_STATE;
}

Status AudioServerSinkPlugin::Resume()
{
    MEDIA_LOG_I("Resume entered.");
    return Start();
}

Status AudioServerSinkPlugin::Pause()
{
    MEDIA_LOG_I("Pause entered.");
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRenderer_ != nullptr) {
        if (audioRenderer_->Pause()) {
            MEDIA_LOG_I("audio renderer pause success");
            return Status::OK;
        } else {
            MEDIA_LOG_E("audio renderer pause fail");
        }
    }
    return Status::ERROR_WRONG_STATE;
}

Status AudioServerSinkPlugin::GetLatency(uint64_t& hstTime)
{
    hstTime = 0; // set latency as 0 since latency of audio system is not reliable
    return Status::OK;
}

Status AudioServerSinkPlugin::Write(const std::shared_ptr<Buffer>& input)
{
    FALSE_RETURN_V_MSG_W(input != nullptr && !input->IsEmpty(), Status::OK, "Receive empty buffer."); // return ok
    auto mem = input->GetMemory();
    auto* buffer = const_cast<uint8_t *>(mem->GetReadOnlyData());
    size_t length = mem->GetSize();
    if (needReformat_ && length > 0) {
        size_t lineSize = mem->GetSize() / channels_;
        std::vector<const uint8_t*> tmpInput(channels_);
        tmpInput[0] = mem->GetReadOnlyData();
        if (av_sample_fmt_is_planar(reSrcFfFmt_)) {
            for (size_t i = 1; i < channels_; ++i) {
                tmpInput[i] = tmpInput[i-1] + lineSize;
            }
        }
        auto samples = lineSize / av_get_bytes_per_sample(reSrcFfFmt_);
        auto res = swr_convert(swrCtx_.get(), resampleChannelAddr_.data(), samples, tmpInput.data(), samples);
        if (res < 0) {
            MEDIA_LOG_E("resample input failed");
            length = 0;
        } else {
            buffer = resampleCache_.data();
            length = res * av_get_bytes_per_sample(reFfDestFmt_) * channels_;
        }
    }
    MEDIA_LOG_DD("write data size " PUBLIC_LOG_ZU, length);
    int32_t ret = 0;
    OSAL::ScopedLock lock(renderMutex_);
    FALSE_RETURN_V(audioRenderer_ != nullptr, Status::ERROR_WRONG_STATE);
    for (; length > 0;) {
        ret = audioRenderer_->Write(buffer, length);
        MEDIA_LOG_DD("written data size " PUBLIC_LOG_D32, ret);
        if (ret < 0) {
            MEDIA_LOG_E("Write data error " PUBLIC_LOG_D32, ret);
            break;
        } else if (static_cast<size_t>(ret) < length) {
            OSAL::SleepFor(5); // 5ms
        }
        buffer += ret;
        length -= ret;
    }
    return ret >= 0 ? Status::OK : Status::ERROR_UNKNOWN;
}

Status AudioServerSinkPlugin::Flush()
{
    MEDIA_LOG_I("Flush entered.");
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRenderer_ == nullptr) {
        return Status::ERROR_WRONG_STATE;
    }
    if (audioRenderer_->Flush()) {
        MEDIA_LOG_I("audioRenderer_ Flush() success");
        return Status::OK;
    }
    MEDIA_LOG_E("audioRenderer_ Flush() fail");
    return Status::ERROR_UNKNOWN;
}

Status  AudioServerSinkPlugin::Drain()
{
    MEDIA_LOG_I("Drain entered.");
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRenderer_ == nullptr) {
        return Status::ERROR_WRONG_STATE;
    }
    auto res = audioRenderer_->Drain();
    uint64_t latency = 0;
    audioRenderer_->GetLatency(latency);
    latency /= 1000; // cast into ms
    if (!res || latency > 50) { // latency too large
        MEDIA_LOG_W("drain failed or latency is too large, will sleep " PUBLIC_LOG_U64 " ms, aka. latency", latency);
        OSAL::SleepFor(latency);
    }
    MEDIA_LOG_I("audioRenderer_ Drain() success");
    return Status::OK;
}
} // namespace AuSrSinkPlugin
} // Plugin
} // namespace Media
} // namespace OHOS