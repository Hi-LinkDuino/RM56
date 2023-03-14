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

#ifndef HISTREAMER_AU_SERVER_SINK_PLUGIN_H
#define HISTREAMER_AU_SERVER_SINK_PLUGIN_H

#include <atomic>
#include <memory>

#include "audio_info.h"
#include "audio_renderer.h"
#include "foundation/osal/thread/mutex.h"
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/interface/audio_sink_plugin.h"
#include "timestamp.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libswresample/swresample.h"
#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace AuSrSinkPlugin {
class AudioServerSinkPlugin : public Plugin::AudioSinkPlugin {
public:
    explicit AudioServerSinkPlugin(std::string name);

    ~AudioServerSinkPlugin() override;

    Status Init() override;

    Status Deinit() override;

    Status Prepare() override;

    Status Reset() override;

    Status Start() override;

    Status Stop() override;

    Status GetParameter(Tag tag, ValueType& para) override;

    Status SetParameter(Tag tag, const ValueType& para) override;

    std::shared_ptr<Allocator> GetAllocator() override
    {
        return nullptr;
    }

    Status SetCallback(Callback* cb) override
    {
        return Status::OK;
    }

    Status GetMute(bool& mute) override
    {
        return Status::OK;
    }

    Status SetMute(bool mute) override
    {
        return Status::OK;
    }

    Status GetVolume(float& volume) override;

    Status SetVolume(float volume) override;

    Status GetSpeed(float& speed) override
    {
        return Status::OK;
    }

    Status SetSpeed(float speed) override
    {
        return Status::OK;
    }

    Status Pause() override;

    Status Resume() override;

    Status GetLatency(uint64_t& hstTime) override;

    Status GetFrameSize(size_t& size) override
    {
        return Status::OK;
    }

    Status GetFrameCount(uint32_t& count) override
    {
        return Status::OK;
    }

    Status Write(const std::shared_ptr<Buffer>& input) override;

    Status Flush() override;

    Status Drain() override;
private:
    void ReleaseRender();
    bool StopRender();
    bool AssignSampleRateIfSupported(uint32_t sampleRate);
    bool AssignChannelNumIfSupported(uint32_t channelNum);
    bool AssignSampleFmtIfSupported(AudioSampleFormat sampleFormat);

    OSAL::Mutex renderMutex_ {};
    std::unique_ptr<AudioStandard::AudioRenderer> audioRenderer_;
    AudioStandard::AudioRendererParams rendererParams_ {};

    bool fmtSupported_ {false};
    AVSampleFormat reSrcFfFmt_ {AV_SAMPLE_FMT_NONE};
    const AudioStandard::AudioSampleFormat reStdDestFmt_ {AudioStandard::AudioSampleFormat::SAMPLE_S16LE};
    const AVSampleFormat reFfDestFmt_ {AV_SAMPLE_FMT_S16};
    AudioChannelLayout channelLayout_ {};
    uint32_t channels_ {};
    uint32_t samplesPerFrame_ {};
    uint32_t sampleRate_ {};
    int64_t bitRate_ {0};
    bool needReformat_ {false};
    std::shared_ptr<SwrContext> swrCtx_ {nullptr};
    std::vector<uint8_t> resampleCache_ {};
    std::vector<uint8_t*> resampleChannelAddr_ {};
};
} // AuSrSinkPlugin
} // Plugin
} // Media
} // OHOS
#endif // HISTREAMER_AU_SERVER_SINK_PLUGIN_H
