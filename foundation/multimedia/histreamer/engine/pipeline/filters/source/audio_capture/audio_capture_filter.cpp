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

#define HST_LOG_TAG "AudioCaptureFilter"

#include "audio_capture_filter.h"
#include "common/plugin_utils.h"
#include "factory/filter_factory.h"
#include "foundation/cpp_ext/algorithm_ext.h"
#include "foundation/log.h"
#include "pipeline/core/plugin_attr_desc.h"
#include "pipeline/filters/common/buffer_calibration/audio_buffer_calibration.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
using namespace Plugin;

static AutoRegisterFilter<AudioCaptureFilter> g_registerFilterHelper("builtin.recorder.audiocapture");

AudioCaptureFilter::AudioCaptureFilter(const std::string& name)
    : FilterBase(name),
      taskPtr_(nullptr),
      plugin_(nullptr),
      pluginAllocator_(nullptr),
      pluginInfo_(nullptr),
      bufferCalibration_(new AudioBufferCalibration)
{
    filterType_ = FilterType::CAPTURE_SOURCE;
    MEDIA_LOG_D("ctor called");
}

AudioCaptureFilter::~AudioCaptureFilter()
{
    MEDIA_LOG_D("dtor called");
    if (taskPtr_) {
        taskPtr_->Stop();
    }
    if (plugin_) {
        plugin_->Deinit();
    }
}

std::vector<WorkMode> AudioCaptureFilter::GetWorkModes()
{
    return {WorkMode::PUSH};
}

ErrorCode AudioCaptureFilter::InitAndConfigWithMeta(const std::shared_ptr<Plugin::Meta>& audioMeta)
{
    MEDIA_LOG_D("IN");
    if (appTokenIdSpecified_) {
        NOK_LOG(plugin_->SetParameter(Tag::APP_TOKEN_ID, appTokenId_));
    }
    if (appUidSpecified_) {
        NOK_LOG(plugin_->SetParameter(Tag::APP_UID, appUid_));
    }
    ErrorCode err = TranslatePluginStatus(plugin_->Init());
    if (err != ErrorCode::SUCCESS) {
        return err;
    }
    plugin_->SetCallback(this);
    pluginAllocator_ = plugin_->GetAllocator();
    uint32_t tmp = 0;
    if (audioMeta->GetUint32(MetaID::AUDIO_SAMPLE_RATE, tmp)) {
        MEDIA_LOG_I("configure plugin with sample rate " PUBLIC_LOG_U32, tmp);
        bufferCalibration_->SetParam(Tag::AUDIO_SAMPLE_RATE, tmp);
        err = TranslatePluginStatus(plugin_->SetParameter(Tag::AUDIO_SAMPLE_RATE, tmp));
        if (err != ErrorCode::SUCCESS) {
            return err;
        }
    }
    if (audioMeta->GetUint32(MetaID::AUDIO_CHANNELS, tmp)) {
        MEDIA_LOG_I("configure plugin with channel " PUBLIC_LOG_U32, tmp);
        bufferCalibration_->SetParam(Tag::AUDIO_CHANNELS, tmp);
        err = TranslatePluginStatus(plugin_->SetParameter(Tag::AUDIO_CHANNELS, channelNum_));
        if (err != ErrorCode::SUCCESS) {
            return err;
        }
    }
    int64_t bitRate = 0;
    if (audioMeta->GetInt64(MetaID::MEDIA_BITRATE, bitRate)) {
        MEDIA_LOG_I("configure plugin with bitrate " PUBLIC_LOG_D64, bitRate);
        err = TranslatePluginStatus(plugin_->SetParameter(Tag::MEDIA_BITRATE, bitRate));
        if (err != ErrorCode::SUCCESS) {
            return err;
        }
    }
    Plugin::AudioSampleFormat sampleFormat = Plugin::AudioSampleFormat::S16;
    if (audioMeta->GetData<Plugin::AudioSampleFormat>(MetaID::AUDIO_SAMPLE_FORMAT, sampleFormat)) {
        bufferCalibration_->SetParam(Tag::AUDIO_SAMPLE_FORMAT, sampleFormat);
        MEDIA_LOG_I("configure plugin with sampleFormat " PUBLIC_LOG_S, GetAudSampleFmtNameStr(sampleFormat));
        return TranslatePluginStatus(plugin_->SetParameter(Tag::AUDIO_SAMPLE_FORMAT, sampleFormat));
    }
    return ErrorCode::SUCCESS;
}

ErrorCode AudioCaptureFilter::SetParameter(int32_t key, const Plugin::Any& value)
{
    auto tag = static_cast<OHOS::Media::Plugin::Tag>(key);
    switch (tag) {
        case Tag::SRC_INPUT_TYPE:
            inputTypeSpecified_ = AssignParameterIfMatch(tag, inputType_, value);
            break;
        case Tag::AUDIO_SAMPLE_RATE:
            sampleRateSpecified_ = AssignParameterIfMatch(tag, sampleRate_, value);
            break;
        case Tag::AUDIO_CHANNELS:
            channelNumSpecified_ = AssignParameterIfMatch(tag, channelNum_, value);
            break;
        case Tag::MEDIA_BITRATE:
            bitRateSpecified_ = AssignParameterIfMatch(tag, bitRate_, value);
            break;
        case Tag::AUDIO_SAMPLE_FORMAT:
            sampleFormatSpecified_ = AssignParameterIfMatch(tag, sampleFormat_, value);
            break;
        case Tag::AUDIO_CHANNEL_LAYOUT:
            channelLayoutSpecified_ = AssignParameterIfMatch(tag, channelLayout_, value);
            break;
        case Tag::APP_TOKEN_ID:
            appTokenIdSpecified_ = AssignParameterIfMatch(tag, appTokenId_, value);
            break;
        case Tag::APP_UID:
            appUidSpecified_ = AssignParameterIfMatch(tag, appUid_, value);
            break;
        default:
            MEDIA_LOG_W("SetParameter: unknown key " PUBLIC_LOG_S "(" PUBLIC_LOG_D32 ")", GetTagStrName(tag), key);
            break;
    }
    return ErrorCode::SUCCESS;
}

ErrorCode AudioCaptureFilter::GetParameter(int32_t key, Plugin::Any& value)
{
    Tag tag = static_cast<Plugin::Tag>(key);
    switch (tag) {
        case Tag::SRC_INPUT_TYPE: {
            value = inputType_;
            break;
        }
        case Tag::AUDIO_SAMPLE_RATE: {
            value = sampleRate_;
            break;
        }
        case Tag::AUDIO_CHANNELS: {
            value = channelNum_;
            break;
        }
        case Tag::MEDIA_BITRATE: {
            value = bitRate_;
            break;
        }
        case Tag::AUDIO_SAMPLE_FORMAT: {
            value = sampleFormat_;
            break;
        }
        default:
            MEDIA_LOG_W("Unknown key " PUBLIC_LOG_S, GetTagStrName(tag));
            break;
    }
    return ErrorCode::SUCCESS;
}

void AudioCaptureFilter::PickPreferSampleFmt(const std::shared_ptr<Plugin::Meta>& meta, const Plugin::ValueType& val)
{
    static constexpr AudioSampleFormat preferFmt = AudioSampleFormat::S16;
    bool pickPreferFmt = false;
    if (val.SameTypeWith(typeid(FixedCapability<AudioSampleFormat>)) &&
        preferFmt == Plugin::AnyCast<FixedCapability<AudioSampleFormat>>(val)) {
        pickPreferFmt = true;
    } else if (val.SameTypeWith(typeid(DiscreteCapability<AudioSampleFormat>))) {
        const auto* fmts = Plugin::AnyCast<DiscreteCapability<AudioSampleFormat>>(&val);
        pickPreferFmt = CppExt::AnyOf(fmts->begin(), fmts->end(), [&](AudioSampleFormat tmp) -> bool {
            return preferFmt == tmp;
        });
    }
    if (pickPreferFmt) {
        meta->SetData(MetaID::AUDIO_SAMPLE_FORMAT, preferFmt);
    }
}

std::shared_ptr<Plugin::Meta> AudioCaptureFilter::PickPreferParameters()
{
    auto preferMeta = std::make_shared<Plugin::Meta>();
    if (capNegWithDownstream_.keys.count(Capability::Key::AUDIO_SAMPLE_FORMAT)) {
        PickPreferSampleFmt(preferMeta, capNegWithDownstream_.keys.at(Capability::Key::AUDIO_SAMPLE_FORMAT));
    }
    return preferMeta;
}

ErrorCode AudioCaptureFilter::DoConfigure()
{
    auto preferMeta = PickPreferParameters();
    auto audioMeta = std::make_shared<Plugin::Meta>();
    if (!MergeMetaWithCapability(*preferMeta, capNegWithDownstream_, *audioMeta)) {
        MEDIA_LOG_E("cannot find available capability of plugin " PUBLIC_LOG_S, pluginInfo_->name.c_str());
        return ErrorCode::ERROR_UNKNOWN;
    }
    if (!outPorts_[0]->Configure(audioMeta)) {
        MEDIA_LOG_E("Configure downstream fail with " PUBLIC_LOG_S, Meta2String(*audioMeta).c_str());
        return ErrorCode::ERROR_UNKNOWN;
    }
    return InitAndConfigWithMeta(audioMeta);
}

ErrorCode AudioCaptureFilter::Prepare()
{
    MEDIA_LOG_I("Prepare entered.");
    if (!taskPtr_) {
        taskPtr_ = std::make_shared<OSAL::Task>("DataReader");
        taskPtr_->RegisterHandler([this] { ReadLoop(); });
    }
    ErrorCode err = FindPlugin();
    if (err != ErrorCode::SUCCESS || !plugin_) {
        MEDIA_LOG_E("Find plugin fail");
        return err;
    }
    err = DoConfigure();
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("DoConfigure fail");
        return err;
    }
    err = TranslatePluginStatus(plugin_->Prepare());
    if (err == ErrorCode::SUCCESS) {
        MEDIA_LOG_D("media source send EVENT_READY");
        OnEvent(Event{name_, EventType::EVENT_READY, {}});
    }
    return err;
}

ErrorCode AudioCaptureFilter::Start()
{
    MEDIA_LOG_I("Start entered.");
    eos_ = false;
    auto res = ErrorCode::SUCCESS;
    bufferCalibration_->Enable();
    // start plugin firstly
    if (plugin_) {
        res = TranslatePluginStatus(plugin_->Start());
    } else {
        res = ErrorCode::ERROR_INVALID_OPERATION;
    }
    FALSE_RETURN_V_MSG_E(res == ErrorCode::SUCCESS, res, "start plugin failed");
    // start task secondly
    if (taskPtr_) {
        taskPtr_->Start();
    }
    return res;
}

ErrorCode AudioCaptureFilter::Stop()
{
    MEDIA_LOG_I("Stop entered.");
    FilterBase::Stop();
    // stop task firstly
    if (taskPtr_) {
        taskPtr_->Stop();
    }
    bufferCalibration_->Reset();
    latestBufferTime_ = HST_TIME_NONE;
    latestPausedTime_ = HST_TIME_NONE;
    totalPausedTime_ = 0;
    refreshTotalPauseTime_ = false;
    // stop plugin secondly
    ErrorCode ret = ErrorCode::SUCCESS;
    if (plugin_) {
        ret = TranslatePluginStatus(plugin_->Stop());
    }
    return ret;
}

ErrorCode AudioCaptureFilter::Pause()
{
    MEDIA_LOG_I("Pause entered.");
    FilterBase::Pause();
    if (taskPtr_) {
        taskPtr_->Pause();
    }
    latestPausedTime_ = latestBufferTime_;
    ErrorCode ret = ErrorCode::SUCCESS;
    if (plugin_) {
        ret = TranslatePluginStatus(plugin_->Stop());
    }
    return ret;
}

ErrorCode AudioCaptureFilter::Resume()
{
    MEDIA_LOG_I("Resume entered.");
    if (state_ == FilterState::PAUSED) {
        refreshTotalPauseTime_ = true;
    }
    state_ = FilterState::RUNNING;
    if (taskPtr_) {
        taskPtr_->Start();
    }
    return plugin_ ? TranslatePluginStatus(plugin_->Start()) : ErrorCode::ERROR_INVALID_OPERATION;
}

ErrorCode AudioCaptureFilter::SendEos()
{
    MEDIA_LOG_I("SendEos entered.");
    auto buf = std::make_shared<AVBuffer>();
    buf->flag |= BUFFER_FLAG_EOS;
    SendBuffer(buf);
    eos_ = true;
    return ErrorCode::SUCCESS;
}

void AudioCaptureFilter::InitPorts()
{
    MEDIA_LOG_D("IN");
    auto outPort = std::make_shared<OutPort>(this);
    outPorts_.push_back(outPort);
}

void AudioCaptureFilter::ReadLoop()
{
    if (eos_.load()) {
        return;
    }
    size_t bufferSize = 0;
    auto ret = plugin_->GetSize(bufferSize);
    if (ret != Status::OK || bufferSize <= 0) {
        MEDIA_LOG_E("Get plugin buffer size fail");
        return;
    }
    AVBufferPtr bufferPtr = std::make_shared<AVBuffer>(BufferMetaType::AUDIO);
    ret = plugin_->Read(bufferPtr, bufferSize);
    if (ret == Status::ERROR_AGAIN) {
        MEDIA_LOG_D("plugin read return again");
        return;
    }
    if (ret != Status::OK) {
        SendEos();
        return;
    }
    latestBufferTime_ = bufferPtr->pts;
    if (refreshTotalPauseTime_) {
        if (latestPausedTime_ != HST_TIME_NONE && latestBufferTime_ > latestPausedTime_) {
            totalPausedTime_ += latestBufferTime_ - latestPausedTime_;
        }
        refreshTotalPauseTime_ = false;
    }
    bufferPtr->pts -= totalPausedTime_;
    SendBuffer(bufferPtr);
}

ErrorCode AudioCaptureFilter::CreatePlugin(const std::shared_ptr<PluginInfo>& info, const std::string& name,
                                           PluginManager& manager)
{
    if ((plugin_ != nullptr) && (pluginInfo_ != nullptr)) {
        if (info->name == pluginInfo_->name && TranslatePluginStatus(plugin_->Reset()) == ErrorCode::SUCCESS) {
            MEDIA_LOG_I("Reuse last plugin: " PUBLIC_LOG_S, name.c_str());
            return ErrorCode::SUCCESS;
        }
        if (TranslatePluginStatus(plugin_->Deinit()) != ErrorCode::SUCCESS) {
            MEDIA_LOG_E("Deinit last plugin: " PUBLIC_LOG_S " error", pluginInfo_->name.c_str());
        }
    }
    plugin_ = manager.CreateSourcePlugin(name);
    if (plugin_ == nullptr) {
        MEDIA_LOG_E("PluginManager CreatePlugin " PUBLIC_LOG_S " fail", name.c_str());
        return ErrorCode::ERROR_UNKNOWN;
    }
    pluginInfo_ = info;
    MEDIA_LOG_I("Create new plugin: " PUBLIC_LOG_S " success", pluginInfo_->name.c_str());
    return ErrorCode::SUCCESS;
}

bool AudioCaptureFilter::CheckSampleRate(const Plugin::Capability& cap)
{
    if (!sampleRateSpecified_) {
        return true;
    }
    for (const auto& pairKey : cap.keys) {
        if (pairKey.first != Capability::Key::AUDIO_SAMPLE_RATE ||
            !pairKey.second.SameTypeWith(typeid(DiscreteCapability<uint32_t>))) {
            continue;
        }
        auto supportedSampleRateList = Plugin::AnyCast<DiscreteCapability<uint32_t>>(pairKey.second);
        for (const auto& rate : supportedSampleRateList) {
            if (rate == sampleRate_) {
                return true;
            }
        }
    }
    return false;
}

bool AudioCaptureFilter::CheckChannels(const Plugin::Capability& cap)
{
    if (!channelNumSpecified_) {
        return true;
    }
    for (const auto& pairKey : cap.keys) {
        if (pairKey.first != Capability::Key::AUDIO_CHANNELS ||
            !pairKey.second.SameTypeWith(typeid(DiscreteCapability<uint32_t>))) {
            continue;
        }
        auto supportedChannelsList = Plugin::AnyCast<DiscreteCapability<uint32_t>>(pairKey.second);
        for (const auto& channel : supportedChannelsList) {
            if (channel == channelNum_) {
                return true;
            }
        }
    }
    return false;
}

bool AudioCaptureFilter::CheckSampleFormat(const Plugin::Capability& cap)
{
    if (!channelNumSpecified_) {
        return true;
    }
    for (const auto& pairKey : cap.keys) {
        if (pairKey.first != Capability::Key::AUDIO_SAMPLE_FORMAT ||
            !pairKey.second.SameTypeWith(typeid(DiscreteCapability<Plugin::AudioSampleFormat>))) {
            continue;
        }
        auto supportedSampleFormatList =
            Plugin::AnyCast<DiscreteCapability<Plugin::AudioSampleFormat>>(pairKey.second);
        for (const auto& fmt : supportedSampleFormatList) {
            if (fmt == sampleFormat_) {
                return true;
            }
        }
    }
    return false;
}

bool AudioCaptureFilter::DoNegotiate(const CapabilitySet &outCaps)
{
    if (outCaps.empty()) {
        MEDIA_LOG_E("audio capture plugin must have out caps");
        return false;
    }
    for (const auto& outCap : outCaps) {
        if (!CheckSampleRate(outCap) || !CheckChannels(outCap) || !CheckSampleFormat(outCap)) {
            continue;
        }
        auto thisOut = std::make_shared<Plugin::Capability>();
        *thisOut = outCap;
        if (sampleFormatSpecified_) {
            thisOut->keys[Capability::Key::AUDIO_SAMPLE_FORMAT] = sampleFormat_;
        }
        if (sampleRateSpecified_) {
            thisOut->keys[Capability::Key::AUDIO_SAMPLE_RATE] = sampleRate_;
        }
        if (channelNumSpecified_) {
            thisOut->keys[Capability::Key::AUDIO_CHANNELS] = channelNum_;
        }
        if (channelNum_ == 1) {
            thisOut->keys[Capability::Key::AUDIO_CHANNEL_LAYOUT] = AudioChannelLayout::MONO;
        } else if (channelNum_ == 2) { // 2 stereo
            thisOut->keys[Capability::Key::AUDIO_CHANNEL_LAYOUT] = AudioChannelLayout::STEREO;
        } else if (channelLayoutSpecified_) {
            thisOut->keys[Capability::Key::AUDIO_CHANNEL_LAYOUT] = channelLayout_;
        } else {
            MEDIA_LOG_W("audio channel layout is unknown");
        }
        if (bitRateSpecified_) {
            thisOut->keys[Capability::Key::MEDIA_BITRATE] = bitRate_;
        }
        Plugin::TagMap upstreamParams;
        Plugin::TagMap downstreamParams;
        if (outPorts_[0]->Negotiate(thisOut, capNegWithDownstream_, upstreamParams, downstreamParams)) {
            MEDIA_LOG_I("Negotiate success");
            return true;
        }
    }
    return false;
}

ErrorCode AudioCaptureFilter::FindPlugin()
{
    if (!inputTypeSpecified_) {
        MEDIA_LOG_E("Must set input type first");
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    PluginManager& pluginManager = PluginManager::Instance();
    std::set<std::string> nameList = pluginManager.ListPlugins(PluginType::SOURCE);
    for (const std::string& name : nameList) {
        std::shared_ptr<PluginInfo> info = pluginManager.GetPluginInfo(PluginType::SOURCE, name);
        MEDIA_LOG_I("name: " PUBLIC_LOG_S ", info->name: " PUBLIC_LOG_S, name.c_str(), info->name.c_str());
        auto val = info->extra[PLUGIN_INFO_EXTRA_INPUT_TYPE];
        if (val.SameTypeWith(typeid(Plugin::SrcInputType))) {
            auto supportInputType = OHOS::Media::Plugin::AnyCast<Plugin::SrcInputType>(val);
            if (inputType_ == supportInputType && DoNegotiate(info->outCaps) &&
                CreatePlugin(info, name, pluginManager) == ErrorCode::SUCCESS) {
                MEDIA_LOG_I("CreatePlugin " PUBLIC_LOG_S " success", name_.c_str());
                return ErrorCode::SUCCESS;
            }
        }
    }
    MEDIA_LOG_I("Cannot find any plugin");
    return ErrorCode::ERROR_UNSUPPORTED_FORMAT;
}

void AudioCaptureFilter::SendBuffer(const std::shared_ptr<AVBuffer>& buffer)
{
    OSAL::ScopedLock lock(pushDataMutex_);
    if (!eos_) {
        bufferCalibration_->CorrectBuffer(buffer);
        outPorts_[0]->PushData(buffer, -1);
    }
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // RECORDER_SUPPORT