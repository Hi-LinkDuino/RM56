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

#ifndef HISTREAMER_SDL_AU_SINK_PLUGIN_H
#define HISTREAMER_SDL_AU_SINK_PLUGIN_H

#include <atomic>
#include "SDL.h"
#include "plugin/interface/audio_sink_plugin.h"
#include "plugin/common/plugin_audio_tags.h"
#include "utils/ring_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Sdl {
class SdlAudioSinkPlugin : public std::enable_shared_from_this<SdlAudioSinkPlugin>, public AudioSinkPlugin {
public:
    explicit SdlAudioSinkPlugin(std::string name);
    ~SdlAudioSinkPlugin() override = default;

    Status Init() override;

    Status Deinit() override;

    Status Prepare() override;

    Status Reset() override;

    Status Start() override;

    Status Stop() override;

    Status GetParameter(Tag tag, ValueType &value) override;

    Status SetParameter(Tag tag, const ValueType &value) override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override;

    // audio sink

    Status GetMute(bool &mute) override;

    Status SetMute(bool mute) override;

    Status GetVolume(float &volume) override;

    Status SetVolume(float volume) override;

    Status GetSpeed(float &speed) override;

    Status SetSpeed(float speed) override;

    Status Pause() override;

    Status Resume() override;

    Status GetLatency(uint64_t &nanoSec) override;

    Status GetFrameSize(size_t &size) override;

    Status GetFrameCount(uint32_t &count) override;

    Status Write(const std::shared_ptr<Buffer> &input) override;

    Status Flush() override;

    Status Drain() override;
private:
    void AudioCallback(void* userdata, uint8_t* stream, int len); // NOLINT: void*

    bool needResample_ {false};
    std::vector<uint8_t> resampleCache_ {};
    std::vector<uint8_t*> resampleChannelAddr_ {};
    std::vector<uint8_t> mixCache_ {};
    std::unique_ptr<RingBuffer> rb {};
    size_t srcFrameSize_ {};
    SDL_AudioSpec wantedSpec_ {};
    uint32_t channels_ {2}; // 2: STEREO
    uint32_t sampleRate_ {0};
    uint32_t samplesPerFrame_ {0};
    uint64_t channelLayout_ {0};
    AudioSampleFormat audioFormat_ {AudioSampleFormat::NONE};
    std::shared_ptr<SwrContext> swrCtx_ {nullptr};
    int volume_;
    const AVSampleFormat reFfDestFmt_ {AV_SAMPLE_FMT_S16};
    AVSampleFormat reSrcFfFmt_ {AV_SAMPLE_FMT_NONE};
};
} // namespace Sdl
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_SDL_AU_SINK_PLUGIN_H
