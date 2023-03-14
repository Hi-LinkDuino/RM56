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
#ifndef MEDIA_PIPELINE_AUDIO_CAPTURE_FILTER_H
#define MEDIA_PIPELINE_AUDIO_CAPTURE_FILTER_H

#ifdef RECORDER_SUPPORT

#include <memory>
#include <string>

#include "pipeline/filters/common/buffer_calibration/buffer_calibration.h"
#include "osal/thread/task.h"
#include "osal/utils/util.h"
#include "pipeline/core/error_code.h"
#include "pipeline/core/filter_base.h"
#include "plugin/core/plugin_manager.h"
#include "pipeline/core/type_define.h"
#include "plugin/interface/source_plugin.h"
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_time.h"
#include "utils/constants.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class AudioCaptureFilter : public FilterBase {
public:
    explicit AudioCaptureFilter(const std::string& name);
    ~AudioCaptureFilter() override;

    std::vector<WorkMode> GetWorkModes() override;
    ErrorCode SetParameter(int32_t key, const Plugin::Any& value) override;
    ErrorCode GetParameter(int32_t key, Plugin::Any& value) override;
    ErrorCode Prepare() override;
    ErrorCode Start() override;
    ErrorCode Stop() override;
    ErrorCode Pause() override;
    ErrorCode Resume() override;
    ErrorCode SendEos();
private:
    void InitPorts() override;
    ErrorCode InitAndConfigWithMeta(const std::shared_ptr<Plugin::Meta>& audioMeta);
    void ReadLoop();
    ErrorCode CreatePlugin(const std::shared_ptr<Plugin::PluginInfo>& info, const std::string& name,
                           Plugin::PluginManager& manager);
    ErrorCode FindPlugin();
    bool DoNegotiate(const CapabilitySet& outCaps);
    bool CheckSampleRate(const Plugin::Capability& cap);
    bool CheckChannels(const Plugin::Capability& cap);
    bool CheckSampleFormat(const Plugin::Capability& cap);
    ErrorCode DoConfigure();
    void SendBuffer(const std::shared_ptr<AVBuffer>& buffer);
    void PickPreferSampleFmt(const std::shared_ptr<Plugin::Meta>& meta, const Plugin::ValueType& val);
    std::shared_ptr<Plugin::Meta> PickPreferParameters();

    std::shared_ptr<OSAL::Task> taskPtr_ {nullptr};
    std::shared_ptr<Plugin::Source> plugin_ {nullptr};
    std::shared_ptr<Allocator> pluginAllocator_ {nullptr};
    std::shared_ptr<Plugin::PluginInfo> pluginInfo_ {nullptr};
    Plugin::SrcInputType inputType_ {};
    bool inputTypeSpecified_ {false};
    uint32_t sampleRate_ {0};
    bool sampleRateSpecified_ {false};
    uint32_t channelNum_ {0};
    bool channelNumSpecified_ {false};
    int64_t bitRate_ {0};
    bool bitRateSpecified_ {false};
    uint32_t appTokenId_ {0};
    bool appTokenIdSpecified_ {false};
    int32_t appUid_ {0};
    bool appUidSpecified_ {false};
    Plugin::AudioSampleFormat sampleFormat_ {OHOS::Media::Plugin::AudioSampleFormat::S16};
    bool sampleFormatSpecified_ {false};
    Plugin::AudioChannelLayout channelLayout_ {OHOS::Media::Plugin::AudioChannelLayout::STEREO};
    bool channelLayoutSpecified_ {false};
    Capability capNegWithDownstream_ {};

    std::unique_ptr<BufferCalibration> bufferCalibration_ {};
    bool refreshTotalPauseTime_ {false};
    int64_t latestBufferTime_ {HST_TIME_NONE};
    int64_t latestPausedTime_ {HST_TIME_NONE};
    int64_t totalPausedTime_ {0};
    std::atomic<bool> eos_ {false};
    OSAL::Mutex pushDataMutex_ {};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif
#endif // MEDIA_PIPELINE_AUDIO_CAPTURE_FILTER_H

