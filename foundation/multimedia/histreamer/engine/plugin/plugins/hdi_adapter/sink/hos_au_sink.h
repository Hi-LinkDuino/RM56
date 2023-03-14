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

#ifndef HISTREAMER_HDI_SINK_H
#define HISTREAMER_HDI_SINK_H

#include <atomic>
#include <vector>
#include "audio_manager.h"
#include "audio_types.h"
#include "foundation/osal/thread/condition_variable.h"
#include "foundation/osal/thread/mutex.h"
#include "foundation/osal/thread/task.h"
#include "plugin/interface/audio_sink_plugin.h"

struct AudioAdapter;
struct AudioRender;
struct AudioPort;
namespace OHOS {
namespace Media {
namespace Plugin {
namespace HosLite {
class RingBuffer;

class HdiSink : public std::enable_shared_from_this<HdiSink>, public AudioSinkPlugin {
public:
    explicit HdiSink(std::string name);

    ~HdiSink() override = default;

    Status Init() override;

    Status Deinit() override;

    Status Prepare() override;

    Status Reset() override;

    Status Start() override;

    Status Stop() override;

    Status GetParameter(Tag tag, ValueType& value) override;

    Status SetParameter(Tag tag, const ValueType& value) override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override;

    Status GetMute(bool& mute) override;

    Status SetMute(bool mute) override;

    Status GetVolume(float& volume) override;

    Status SetVolume(float volume) override;

    Status GetSpeed(float& speed) override;

    Status SetSpeed(float speed) override;

    Status Pause() override;

    Status Resume() override;

    Status GetLatency(uint64_t& hstTime) override;

    Status GetFrameSize(size_t& size) override;

    Status GetFrameCount(uint32_t& count) override;

    Status Write(const std::shared_ptr<Buffer>& input) override;

    Status Flush() override;

    Status Drain() override;

private:
    Status ReleaseRender();

    void Deinterleave8(uint8_t* inData, uint8_t* outData, int32_t frameCnt);

    void Deinterleave16(uint8_t* inData, uint8_t* outData, int32_t frameCnt);

    void Deinterleave32(uint8_t* inData, uint8_t* outData, int32_t frameCnt);

    bool HandleInterleaveData(uint8_t* origData, int32_t frameCnt);

    void RenderFrame(const std::shared_ptr<Buffer>& input);

    Status ProcessInputSampleFormat(const ValueType& value);

private:
    OSAL::Mutex renderMutex_ {};
    AudioManager* audioManager_ {nullptr};
    AudioAdapterDescriptor adapterDescriptor_ {};
    AudioAdapter* audioAdapter_ {nullptr};
    AudioRender* audioRender_ {nullptr};
    AudioDeviceDescriptor deviceDescriptor_ {};
    AudioSampleAttributes sampleAttributes_ {};
    bool isInputInterleaved_{false};
    AudioChannelMask channelMask_{AUDIO_CHANNEL_MONO};
    Callback* eventCallback_ {};
    std::vector<uint8_t> cacheData_;
    bool usingDefaultInCaps_ {true}; // if true pass hdi with S16P pcm data and convert input into non-interleaved
    std::atomic<bool> processing_;
    OSAL::ConditionVariable renderCond_;
};
} // namespace HosLite
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif
