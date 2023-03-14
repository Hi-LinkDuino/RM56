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

#define HST_LOG_TAG "HdiSinkPlugin"

#include "hos_au_sink.h"
#include <memory>
#include "audio_adapter.h"
#include "audio_manager.h"
#include "foundation/log.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "foundation/osal/utils/util.h"
#include "foundation/pre_defines.h"
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_time.h"
#include "plugins/hdi_adapter/utils/hdi_au_utils.h"
#include "securec.h"
#include "utils/constants.h"

namespace {
using namespace OHOS::Media::Plugin;
using namespace OHOS::Media::Plugin::HosLite;
using PluginSampleFmt = OHOS::Media::Plugin::AudioSampleFormat;
constexpr int32_t MAX_RETRY_CNT = 3;
constexpr int32_t RETRY_INTERVAL = 100; // 100ms
constexpr int32_t HI_ERR_VI_BUF_FULL = 0xA016800F;
constexpr int32_t RANK100 = 100;
constexpr int32_t PCM_CHAN_CNT = 2;
static std::map<std::string, std::pair<uint32_t, bool>> g_sinkInfos;

Status LoadAndInitAdapter(AudioManager* audioManager, AudioAdapterDescriptor* descriptor, AudioAdapter** adapter)
{
    if (audioManager == nullptr) {
        MEDIA_LOG_E("no audio manager when load adapter");
        return Status::ERROR_UNKNOWN;
    }
    if (adapter == nullptr) {
        MEDIA_LOG_E("**adapter null ptr");
        return Status::ERROR_INVALID_PARAMETER;
    }
    if (audioManager->LoadAdapter(audioManager, descriptor, adapter) < 0) {
        *adapter = nullptr;
        MEDIA_LOG_W("failed to load adapter " PUBLIC_LOG_S, descriptor->adapterName);
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    if (*adapter == nullptr) {
        MEDIA_LOG_E("no available adapter after load adapter");
        return Status::ERROR_UNKNOWN;
    }

    int32_t retryCnt = 0;
    do {
        if ((*adapter)->InitAllPorts(*adapter) != 0) {
            OHOS::Media::OSAL::SleepFor(RETRY_INTERVAL);
        } else {
            break;
        }
        MEDIA_LOG_I("retry init port on adapter " PUBLIC_LOG_S, descriptor->adapterName);
    } while (++retryCnt < MAX_RETRY_CNT);
    if (retryCnt >= MAX_RETRY_CNT) {
        MEDIA_LOG_W("cannot init port on adapter " PUBLIC_LOG_S " after retry " PUBLIC_LOG_D32 " times",
                    descriptor->adapterName, retryCnt);
        audioManager->UnloadAdapter(audioManager, *adapter);
        *adapter = nullptr;
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

void UpdatePluginCapWithPortCap(const AudioPortCapability& portCap, Capability& pluginCap)
{
    std::vector<PluginSampleFmt> sampleFormats;
    for (size_t cnt = 0; cnt < portCap.supportSampleFormatNum; cnt++) {
        PluginSampleFmt tmp;
        if (OHOS::Media::Plugin::HosLite::HdiAuFormat2PluginFormat(portCap.supportSampleFormats[cnt], tmp)) {
            sampleFormats.emplace_back(tmp);
        }
    }
    if (!sampleFormats.empty()) {
        if (sampleFormats.size() == 1) {
            pluginCap.AppendFixedKey<PluginSampleFmt>(Capability::Key::AUDIO_SAMPLE_FORMAT, sampleFormats[0]);
        } else {
            pluginCap.AppendDiscreteKeys<PluginSampleFmt>(Capability::Key::AUDIO_SAMPLE_FORMAT, sampleFormats);
        }
    }
    auto pluginSampleRates = OHOS::Media::Plugin::HosLite::HdiSampleRatesMask2PluginRates(portCap.sampleRateMasks);
    if (!pluginSampleRates.empty()) {
        if (pluginSampleRates.size() == 1) {
            pluginCap.AppendFixedKey<uint32_t>(Capability::Key::AUDIO_SAMPLE_RATE, pluginSampleRates[0]);
        } else {
            pluginCap.AppendDiscreteKeys<uint32_t>(Capability::Key::AUDIO_SAMPLE_RATE, pluginSampleRates);
        }
    }
    AudioChannelLayout pluginLayout;
    if (OHOS::Media::Plugin::HosLite::HdiMask2PluginChannelLayout(portCap.channelMasks, pluginLayout)) {
        pluginCap.AppendFixedKey(Capability::Key::AUDIO_CHANNEL_LAYOUT, pluginLayout);
    }
    if (portCap.channelCount > 0) {
        pluginCap.AppendIntervalKey<uint32_t>(Capability::Key::AUDIO_CHANNELS, 1, portCap.channelCount);
    }
}

std::shared_ptr<AudioSinkPlugin> AudioSinkPluginCreator(const std::string& name)
{
    return std::make_shared<OHOS::Media::Plugin::HosLite::HdiSink>(name);
}

void RegisterOutportOnAdapter(const std::shared_ptr<Register>& reg, const AudioAdapterDescriptor& desc,
                              AudioAdapter* adapter)
{
    CapabilitySet adapterCapabilities;
    bool usingDefaultCaps = false;
    uint32_t pIndex = 0;
    for (uint32_t portIndex = 0; portIndex < desc.portNum; portIndex++) {
        if (desc.ports[portIndex].dir != PORT_OUT) {
            continue;
        }
        Capability capability(OHOS::Media::MEDIA_MIME_AUDIO_RAW);
        AudioPortCapability portCap{0};
        if (adapter->GetPortCapability != nullptr &&
            adapter->GetPortCapability(adapter, &desc.ports[portIndex], &portCap) == 0) {
            UpdatePluginCapWithPortCap(portCap, capability);
            usingDefaultCaps = false;
        } else {
            MEDIA_LOG_W("query port capability failed when registering plugin, set audio sample format as S16/S16P");
            capability.AppendDiscreteKeys<PluginSampleFmt>(Capability::Key::AUDIO_SAMPLE_FORMAT,
                                                           {PluginSampleFmt::S16, PluginSampleFmt::S16P});
            usingDefaultCaps = true;
        }
        adapterCapabilities.emplace_back(capability);
        pIndex = portIndex;
        break; // only take the first out port
    }
    if (adapterCapabilities.empty()) {
        return;
    }
    AudioSinkPluginDef sinkPluginDef;
    sinkPluginDef.creator = AudioSinkPluginCreator;
    sinkPluginDef.name = desc.adapterName;
    sinkPluginDef.inCaps = adapterCapabilities;
    sinkPluginDef.rank = RANK100;
    if (reg->AddPlugin(sinkPluginDef) == Status::OK) {
        g_sinkInfos[sinkPluginDef.name] = std::make_pair(pIndex, usingDefaultCaps);
        MEDIA_LOG_D("register plugin " PUBLIC_LOG_S " succ.", desc.adapterName);
    } else {
        MEDIA_LOG_W("register plugin " PUBLIC_LOG_S " failed", desc.adapterName);
    }
}

Status RegisterHdiSinkPlugins(const std::shared_ptr<Register>& reg)
{
    auto audioManager = GetAudioManagerFuncs();
    if (audioManager == nullptr) {
        MEDIA_LOG_E("cannot find audio manager funcs");
        return Status::ERROR_UNKNOWN;
    }
    int32_t adapterSize = 0;
    AudioAdapterDescriptor* descriptors = nullptr;
    int32_t ret = audioManager->GetAllAdapters(audioManager, &descriptors, &adapterSize);
    if (ret != 0 || adapterSize == 0) {
        MEDIA_LOG_E("cannot find available audio adapter");
        return Status::OK;
    }
    for (int32_t index = 0; index < adapterSize; index++) {
        AudioAdapter* adapter = nullptr;
        const auto& desc = descriptors[index];
        if (LoadAndInitAdapter(audioManager, &descriptors[index], &adapter) != Status::OK) {
            continue;
        }
        RegisterOutportOnAdapter(reg, desc, adapter);
        audioManager->UnloadAdapter(audioManager, adapter);
    }
    return Status::OK;
}

void UnRegisterAudioDecoderPlugin()
{
    g_sinkInfos.clear();
}

template <typename T>
inline Status AssignIfCastSuccess(T& lvalue, const Any& anyValue, const char* tagName)
{
    if (anyValue.SameTypeWith(typeid(T))) {
        lvalue = AnyCast<const T&>(anyValue);
        MEDIA_LOG_I("AssignIfCastSuccess found " PUBLIC_LOG_S, tagName);
        return Status::OK;
    } else {
        MEDIA_LOG_W("tag:" PUBLIC_LOG_S " value type mismatch", tagName);
        return Status::ERROR_MISMATCHED_TYPE;
    }
}

int32_t CalculateBufferSize(const AudioSampleAttributes& attributes)
{
    return attributes.frameSize * attributes.period;
}

PLUGIN_DEFINITION(HdiAuSink, LicenseType::APACHE_V2, RegisterHdiSinkPlugins, UnRegisterAudioDecoderPlugin);
} // namespace
namespace OHOS {
namespace Media {
namespace Plugin {
namespace HosLite {
using namespace OHOS::Media::Plugin;

HdiSink::HdiSink(std::string name)
    : AudioSinkPlugin(std::move(name)), audioManager_(nullptr), cacheData_(), processing_(false), renderCond_()
{
    // default is media
    sampleAttributes_.type = AUDIO_IN_MEDIA;
    if (g_sinkInfos.count(pluginName_) != 0) {
        usingDefaultInCaps_ = g_sinkInfos[pluginName_].second;
    }
}

Status HdiSink::Init()
{
    MEDIA_LOG_I("Init entered.");
    audioManager_ = GetAudioManagerFuncs();
    if (audioManager_ == nullptr) {
        MEDIA_LOG_E("Init error due to audioManager nullptr");
        return Status::ERROR_UNKNOWN;
    }
    int32_t adapterSize = 0;
    AudioAdapterDescriptor* descriptors = nullptr;
    int32_t ret = audioManager_->GetAllAdapters(audioManager_, &descriptors, &adapterSize);
    if (ret != 0 || adapterSize == 0) {
        MEDIA_LOG_E("cannot find available audio adapter");
        return Status::ERROR_UNKNOWN;
    }
    for (int32_t index = 0; index < adapterSize; index++) {
        const auto& desc = descriptors[index];
        if (pluginName_ != desc.adapterName) {
            continue;
        }
        if (LoadAndInitAdapter(audioManager_, &descriptors[index], &audioAdapter_) != Status::OK) {
            continue;
        }
        adapterDescriptor_ = descriptors[index];
        break;
    }
    if (audioAdapter_ == nullptr) {
        MEDIA_LOG_E("cannot find adapter with name " PUBLIC_LOG_S, pluginName_.c_str());
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Media::Plugin::Status HdiSink::ReleaseRender()
{
    {
        OSAL::ScopedLock lock(renderMutex_);
        if (audioAdapter_ != nullptr && audioRender_ != nullptr) {
            audioAdapter_->DestroyRender(audioAdapter_, audioRender_);
            audioRender_ = nullptr;
        }
    }
    return Status::OK;
}

Status HdiSink::Deinit()
{
    MEDIA_LOG_I("Deinit entered.");
    Stop();
    // release all resources
    ReleaseRender();
    if (audioManager_ != nullptr) {
        if (audioAdapter_ != nullptr) {
            audioManager_->UnloadAdapter(audioManager_, audioAdapter_);
            audioAdapter_ = nullptr;
        }
        audioManager_ = nullptr;
    }
    return Status::OK;
}

Status HdiSink::ProcessInputSampleFormat(const ValueType& value)
{
    AudioSampleFormat format;
    auto ret = AssignIfCastSuccess<AudioSampleFormat>(format, value, "audioSampleFormat");
    if (ret != Status::OK) {
        return ret;
    }
    if (PluginAuFormat2HdiAttrs(format, sampleAttributes_)) {
        // if using default in caps e.g. S16/S16P always pass non-interleaved pcm data to hdi
        // otherwise using the specified format
        if (usingDefaultInCaps_) {
            if (sampleAttributes_.interleaved) {
                isInputInterleaved_ = true;
                sampleAttributes_.interleaved = false;
            } else {
                isInputInterleaved_ = false;
            }
        }
        return Status::OK;
    } else {
        MEDIA_LOG_E("audioSampleFormat mismatch");
        return Status::ERROR_MISMATCHED_TYPE;
    }
}

Status HdiSink::SetParameter(Tag tag, const ValueType& value)
{
    switch (tag) {
        case Tag::AUDIO_CHANNELS:
            return AssignIfCastSuccess<uint32_t>(sampleAttributes_.channelCount, value, "channel");
        case Tag::AUDIO_SAMPLE_RATE:
            return AssignIfCastSuccess<uint32_t>(sampleAttributes_.sampleRate, value, "sampleRate");
        case Tag::AUDIO_SAMPLE_FORMAT:
            return ProcessInputSampleFormat(value);
        case Tag::AUDIO_SAMPLE_PER_FRAME:
            return AssignIfCastSuccess<uint32_t>(sampleAttributes_.period, value, "samples per frame");
        case Tag::AUDIO_CHANNEL_LAYOUT: {
            AudioChannelLayout layout;
            auto ret = AssignIfCastSuccess<AudioChannelLayout>(layout, value, "audioChannelLayout");
            if (ret != Status::OK) {
                return ret;
            }
            if (PluginChannelLayout2HdiMask(layout, channelMask_)) {
                return Status::OK;
            } else {
                MEDIA_LOG_E("audioChannelLayout mismatch");
                return Status::ERROR_MISMATCHED_TYPE;
            }
        }
        default:
            MEDIA_LOG_I("receive one parameter with unconcern tag, ignore it");
    }
    return Status::OK;
}

Status HdiSink::GetParameter(Tag tag, ValueType& value)
{
    UNUSED_VARIABLE(tag);
    UNUSED_VARIABLE(value);
    return Status::ERROR_UNIMPLEMENTED;
}

Status HdiSink::Prepare()
{
    MEDIA_LOG_I("Prepare entered.");
    sampleAttributes_.frameSize = GetPcmBytes(sampleAttributes_.format) * sampleAttributes_.channelCount;
    sampleAttributes_.startThreshold = sampleAttributes_.period * sampleAttributes_.frameSize;
    sampleAttributes_.stopThreshold = INT32_MAX;
    sampleAttributes_.silenceThreshold = 0;
    if (g_sinkInfos.count(pluginName_) == 0) {
        MEDIA_LOG_E("cannot find out port");
        return Status::ERROR_UNKNOWN;
    }
    deviceDescriptor_.portId = g_sinkInfos[pluginName_].first;
    deviceDescriptor_.pins = PIN_OUT_SPEAKER;
    deviceDescriptor_.desc = nullptr;

    MEDIA_LOG_I("create render: " PUBLIC_LOG_S ", port: " PUBLIC_LOG_D32 ":\ncategory " PUBLIC_LOG
                "s,\nchannels " PUBLIC_LOG_D32 ", sampleRate " PUBLIC_LOG_D32 ",\n"
                " audioChannelMask " PUBLIC_LOG "x, format " PUBLIC_LOG_D32 ",\nisSignedData " PUBLIC_LOG
                "d, interleaved " PUBLIC_LOG_D32 ",\nperiod " PUBLIC_LOG_U32 ", frameSize " PUBLIC_LOG_U32,
                adapterDescriptor_.adapterName, deviceDescriptor_.portId,
                (sampleAttributes_.type == AUDIO_IN_MEDIA) ? "media" : "communication", sampleAttributes_.channelCount,
                sampleAttributes_.sampleRate, channelMask_, sampleAttributes_.format, sampleAttributes_.isSignedData,
                sampleAttributes_.interleaved, sampleAttributes_.period, sampleAttributes_.frameSize);
    {
        OSAL::ScopedLock lock(renderMutex_);
        auto ret = audioAdapter_->CreateRender(audioAdapter_, &deviceDescriptor_, &sampleAttributes_, &audioRender_);
        if (ret != 0) {
            MEDIA_LOG_E("cannot create render with error code " PUBLIC_LOG_U64 "x",
                        static_cast<uint64_t>(ret));
            audioRender_ = nullptr;
            return Status::ERROR_UNKNOWN;
        }
    }
    MEDIA_LOG_I("create audio render successfully");
    if (sampleAttributes_.channelCount == PCM_CHAN_CNT && usingDefaultInCaps_ && isInputInterleaved_) {
        cacheData_.resize(CalculateBufferSize(sampleAttributes_));
    }
    return Status::OK;
}

Status HdiSink::Reset()
{
    MEDIA_LOG_I("Reset entered.");
    ReleaseRender();
    (void)memset_s(&sampleAttributes_, sizeof(sampleAttributes_), 0, sizeof(sampleAttributes_));
    (void)memset_s(&deviceDescriptor_, sizeof(deviceDescriptor_), 0, sizeof(deviceDescriptor_));
    isInputInterleaved_ = false;
    processing_ = false;
    channelMask_ = AUDIO_CHANNEL_MONO;

    return Status::OK;
}

Status HdiSink::Start()
{
    MEDIA_LOG_I("Start entered.");
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_E("no available render");
        return Status::ERROR_UNKNOWN;
    }

    if (audioRender_->control.Start(audioRender_) != 0) {
        MEDIA_LOG_E("audio render start error");
        return Status::ERROR_UNKNOWN;
    }
    processing_ = true;
    return Status::OK;
}

Status HdiSink::Stop()
{
    MEDIA_LOG_I("Stop Entered");
    OSAL::ScopedLock lock(renderMutex_);
    processing_ = false;
    renderCond_.NotifyOne();
    if (audioRender_ == nullptr) {
        MEDIA_LOG_E("no available render");
        return Status::OK;
    }
    if (audioRender_->control.Stop(audioRender_) != 0) {
        MEDIA_LOG_E("audio render stop error");
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_D("Stop Exited");
    return Status::OK;
}

std::shared_ptr<Allocator> HdiSink::GetAllocator()
{
    return nullptr;
}

Status HdiSink::SetCallback(Callback* cb)
{
    eventCallback_ = cb;
    return Status::OK;
}

Status HdiSink::GetMute(bool& mute)
{
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_W("no render available, get mute must be called after prepared");
        return Status::ERROR_WRONG_STATE;
    }
    if (audioRender_->volume.GetMute(audioRender_, &mute) != 0) {
        MEDIA_LOG_E("get mute failed");
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Status HdiSink::SetMute(bool mute)
{
    // todo when to set mute
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_W("no render available, set mute must be called after prepare");
        return Status::ERROR_WRONG_STATE;
    }
    if (audioRender_->volume.SetMute(audioRender_, mute) != 0) {
        MEDIA_LOG_E("set mute failed");
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Status HdiSink::GetVolume(float& volume)
{
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_W("no render available, get volume must be called after prepare");
        return Status::ERROR_WRONG_STATE;
    }
    if (audioRender_->volume.GetVolume(audioRender_, &volume) != 0) {
        MEDIA_LOG_E("get volume failed");
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Status HdiSink::SetVolume(float volume)
{
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_W("no render available, set volume must be called after prepare");
        return Status::ERROR_WRONG_STATE;
    }
    constexpr float maxVolume = 1.0f;
    auto relVolume = volume * maxVolume;
    if (audioRender_->volume.SetVolume(audioRender_, relVolume) != 0) {
        MEDIA_LOG_E("set volume failed");
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_W("set volume to " PUBLIC_LOG ".3f", relVolume);
    return Status::OK;
}

Status HdiSink::GetSpeed(float& speed)
{
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_W("no render available, get speed must be called after prepare");
        return Status::ERROR_WRONG_STATE;
    }
    if (audioRender_->GetRenderSpeed(audioRender_, &speed) != 0) {
        MEDIA_LOG_E("get speed failed");
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Status HdiSink::SetSpeed(float speed)
{
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_W("no render available, set speed must be called after prepare");
        return Status::ERROR_WRONG_STATE;
    }
    if (audioRender_->SetRenderSpeed(audioRender_, speed) != 0) {
        MEDIA_LOG_E("set speed failed");
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Status HdiSink::Pause()
{
    MEDIA_LOG_I("Pause Entered");
    OSAL::ScopedLock lock(renderMutex_);
    processing_ = false;
    renderCond_.NotifyOne();
    if (audioRender_ != nullptr && audioRender_->control.Pause(audioRender_) != 0) {
        MEDIA_LOG_E("pause failed");
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Status HdiSink::Resume()
{
    MEDIA_LOG_I("Resume Entered");
    OSAL::ScopedLock lock(renderMutex_);
    processing_ = true;
    if (audioRender_ != nullptr && audioRender_->control.Resume(audioRender_) != 0) {
        MEDIA_LOG_E("resume failed");
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Status HdiSink::GetLatency(uint64_t& hstTime)
{
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_W("no render available, get latency must be called after prepare");
        return Status::ERROR_WRONG_STATE;
    }
    uint32_t tmp;
    if (audioRender_->GetLatency(audioRender_, &tmp) != 0) {
        MEDIA_LOG_E("get latency failed");
        return Status::ERROR_UNKNOWN;
    }
    int64_t latency = 0;
    if (Ms2HstTime(tmp, latency)) {
        hstTime = latency;
        return Status::OK;
    } else {
        MEDIA_LOG_E("time convert overflow");
        return Status::ERROR_UNKNOWN;
    }
}

Status HdiSink::GetFrameSize(size_t& size)
{
    UNUSED_VARIABLE(size);
    return Status::ERROR_UNIMPLEMENTED;
}

Status HdiSink::GetFrameCount(uint32_t& count)
{
    UNUSED_VARIABLE(count);
    return Status::ERROR_UNIMPLEMENTED;
}

Status HdiSink::Write(const std::shared_ptr<Buffer>& input)
{
    MEDIA_LOG_DD("Write begin.");
    if (!input) {
        MEDIA_LOG_E("Write failed due to nullptr.");
        return Status::ERROR_INVALID_PARAMETER;
    }
    if (!input->IsEmpty()) {
        RenderFrame(input);
    }
    MEDIA_LOG_DD("Write finished.");
    return Status::OK;
}

Status HdiSink::Flush()
{
    MEDIA_LOG_I("Flush Entered");
    OSAL::ScopedLock lock(renderMutex_);
    if (audioRender_ == nullptr) {
        MEDIA_LOG_E("no render available, flush must be called after prepare");
        return Status::ERROR_WRONG_STATE;
    }
    if (audioRender_->control.Flush(audioRender_) != 0) {
        MEDIA_LOG_E("audio render flush error");
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_I("Flush Exited.");
    return Status::OK;
}

Status HdiSink::Drain()
{
    MEDIA_LOG_I("Drain Entered");
    constexpr int waitTimeForPlaybackCompleteMs = 60; // 60
    OHOS::Media::OSAL::SleepFor(waitTimeForPlaybackCompleteMs);
    MEDIA_LOG_I("Drain Exited.");
    return Status::OK;
}

template <typename T>
static void Deinterleave(T inData, T outData, int32_t frameCnt)
{
    int32_t frameSize = frameCnt / PCM_CHAN_CNT;
    for (int i = 0; i < PCM_CHAN_CNT; i++) {
        for (int j = 0; j < frameSize; j++) {
            outData[i * frameSize + j] = inData[j * PCM_CHAN_CNT + i];
        }
    }
}

void HdiSink::Deinterleave16(uint8_t* inData, uint8_t* outData, int32_t frameCnt)
{
    if (sampleAttributes_.isSignedData == true) {
        Deinterleave(reinterpret_cast<int16_t*>(inData), reinterpret_cast<int16_t*>(outData), frameCnt);
    } else {
        Deinterleave(reinterpret_cast<uint16_t*>(inData), reinterpret_cast<uint16_t*>(outData), frameCnt);
    }
}

void HdiSink::Deinterleave8(uint8_t* inData, uint8_t* outData, int32_t frameCnt)
{
    if (sampleAttributes_.isSignedData == true) {
        Deinterleave(reinterpret_cast<int8_t*>(inData), reinterpret_cast<int8_t*>(outData), frameCnt);
    } else {
        Deinterleave(inData, outData, frameCnt);
    }
}

void HdiSink::Deinterleave32(uint8_t* inData, uint8_t* outData, int32_t frameCnt)
{
    if (sampleAttributes_.isSignedData == true) {
        Deinterleave(reinterpret_cast<int32_t*>(inData), reinterpret_cast<int32_t*>(outData), frameCnt);
    } else {
        Deinterleave(reinterpret_cast<uint32_t*>(inData), reinterpret_cast<uint32_t*>(outData), frameCnt);
    }
}

bool HdiSink::HandleInterleaveData(uint8_t* origData, int32_t frameCnt)
{
    if (sampleAttributes_.channelCount != PCM_CHAN_CNT || !usingDefaultInCaps_ || !isInputInterleaved_) {
        return false;
    }
    bool isHandled = true;
    switch (sampleAttributes_.format) {
        case AUDIO_FORMAT_PCM_16_BIT:
            Deinterleave16(origData, cacheData_.data(), frameCnt);
            break;
        default:
            isHandled = false;
            break;
    }
    return isHandled;
}

void HdiSink::RenderFrame(const std::shared_ptr<Buffer>& input)
{
    MEDIA_LOG_DD("RenderFrame started");
    auto mem = input->GetMemory();
    auto frame = const_cast<uint8_t*>(mem->GetReadOnlyData());
    bool dataInterleaved = false;
    for (size_t remainingBytes = mem->GetSize(); processing_.load() && remainingBytes > 0;) {
        int32_t ret = 0;
        uint64_t renderSize = 0;
        {
            OSAL::ScopedLock lock(renderMutex_);
            if (audioRender_ == nullptr) {
                break;
            }
            if (!dataInterleaved && HandleInterleaveData(frame, remainingBytes / PCM_CHAN_CNT)) {
                frame = cacheData_.data();
                dataInterleaved = true;
            }
            ret = audioRender_->RenderFrame(audioRender_, frame, remainingBytes, &renderSize);
        }
        if (ret != 0) {
            if (ret == HI_ERR_VI_BUF_FULL) {
                MEDIA_LOG_DD("renderFrame buffer full"); // do not log this info
                constexpr int timeoutMs = 5;
                OSAL::ScopedLock lock(renderMutex_);
                renderCond_.WaitFor(lock, timeoutMs, [this] { return processing_.load() == false; });
            } else {
                MEDIA_LOG_E("renderFrame buffer error " PUBLIC_LOG_D32, ret);
                break;
            }
        }
        frame += renderSize;
        remainingBytes -= renderSize;
    }
}
} // namespace HosLite
} // namespace Plugin
} // namespace Media
} // namespace OHOS
