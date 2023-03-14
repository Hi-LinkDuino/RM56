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

#ifndef HISTREAMER_UTAUDIOSINKTEST1_H
#define HISTREAMER_UTAUDIOSINKTEST1_H

#include "plugin/interface/audio_sink_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
class UtAudioSinkTest1 : public AudioSinkPlugin {
public:
    explicit UtAudioSinkTest1(std::string name) : AudioSinkPlugin(std::move(name))
    {
    }

    ~UtAudioSinkTest1() override = default;

    Status GetMute(bool &mute) override;

    Status SetMute(bool mute) override;

    Status GetVolume(float &volume) override;

    Status SetVolume(float volume) override;

    Status GetSpeed(float &speed) override;

    Status SetSpeed(float speed) override;

    Status Pause() override;

    Status Resume() override;

    Status GetLatency(uint64_t &ms) override;

    Status GetFrameSize(size_t &size) override;

    Status GetFrameCount(uint32_t &count) override;

    Status Write(const std::shared_ptr<Buffer> &input) override;

    Status Flush() override;

    Status Drain() override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_UTAUDIOSINKTEST1_H
