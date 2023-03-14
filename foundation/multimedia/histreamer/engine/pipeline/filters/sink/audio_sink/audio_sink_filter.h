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

#ifndef MEDIA_PIPELINE_AUDIO_SINK_FILTER_H
#define MEDIA_PIPELINE_AUDIO_SINK_FILTER_H

#include <atomic>

#include "osal/thread/condition_variable.h"
#include "osal/thread/mutex.h"
#include "pipeline/core/error_code.h"
#include "pipeline/core/filter_base.h"
#include "pipeline/filters/sink/media_synchronous_sink.h"
#include "plugin/core/audio_sink.h"
#include "plugin/core/plugin_info.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class AudioSinkFilter : public std::enable_shared_from_this<AudioSinkFilter>, public MediaSynchronousSink {
public:
    explicit AudioSinkFilter(const std::string& name);
    ~AudioSinkFilter() override;

    void Init(EventReceiver* receiver, FilterCallback* callback) override;

    ErrorCode SetParameter(int32_t key, const Plugin::Any& value) override;

    ErrorCode GetParameter(int32_t key, Plugin::Any& value) override;

    bool Negotiate(const std::string& inPort,
                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::TagMap& upstreamParams,
                   Plugin::TagMap& downstreamParams) override;

    bool Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta) override;

    /**
     *
     * @param inPort
     * @param buffer
     * @param offset always ignore this parameter
     * @return
     */
    ErrorCode PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset) override;

    ErrorCode Start() override;
    ErrorCode Stop() override;

    ErrorCode Pause() override;
    ErrorCode Resume() override;

    void FlushStart() override;
    void FlushEnd() override;
    ErrorCode SetVolume(float volume);

protected:
    ErrorCode DoSyncWrite(const AVBufferPtr& buffer) override;

    void ResetSyncInfo() override;

private:
    ErrorCode SetPluginParameter(Tag tag, const Plugin::ValueType& value);
    ErrorCode ConfigureToPreparePlugin(const std::shared_ptr<const Plugin::Meta>& meta);
    ErrorCode SetVolumeToPlugin();

    int64_t latestBufferPts_ {HST_TIME_NONE};
    int64_t latestBufferDuration_ {0};
    int64_t frameCnt_ {0};

    std::atomic<bool> pushThreadIsBlocking {false};
    bool isFlushing {false};
    OSAL::ConditionVariable startWorkingCondition_ {};
    OSAL::Mutex mutex_ {};

    std::shared_ptr<Plugin::AudioSink> plugin_ {};
    float volume_ {-1.0f}; // default invalid value
    int64_t reportAnchorDuration_ {0};
    int64_t lastReportedClockTime_ {HST_TIME_NONE};

    bool forceUpdateTimeAnchorNextTime_ {false};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif