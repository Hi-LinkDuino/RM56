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

#ifndef MEDIA_PIPELINE_VIDEO_SINK_FILTER_H
#define MEDIA_PIPELINE_VIDEO_SINK_FILTER_H

#ifdef VIDEO_SUPPORT

#include <atomic>
#if !defined(OHOS_LITE) && defined(VIDEO_SUPPORT)
#include "refbase.h"
#include "surface/surface.h"
#endif
#include "osal/thread/condition_variable.h"
#include "osal/thread/mutex.h"
#include "osal/thread/task.h"
#include "utils/blocking_queue.h"
#include "pipeline/core/error_code.h"
#include "pipeline/core/filter_base.h"
#include "plugin/core/plugin_info.h"
#include "plugin/core/video_sink.h"
#include "pipeline/filters/sink/media_synchronous_sink.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class VideoSinkFilter : public MediaSynchronousSink {
public:
    explicit VideoSinkFilter(const std::string& name);
    ~VideoSinkFilter() override;

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

#if !defined(OHOS_LITE) && defined(VIDEO_SUPPORT)
    ErrorCode SetVideoSurface(sptr<Surface> surface);
#endif

protected:
    ErrorCode DoSyncWrite(const AVBufferPtr &buffer) override;

    void ResetSyncInfo() override;

private:
    ErrorCode ConfigurePluginParams(const std::shared_ptr<const Plugin::Meta>& meta);
    ErrorCode ConfigurePluginToStartNoLocked(const std::shared_ptr<const Plugin::Meta>& meta);
    bool CreateVideoSinkPlugin(const std::shared_ptr<Plugin::PluginInfo>& selectedPluginInfo);
    void HandleNegotiateParams(const Plugin::TagMap& upstreamParams, Plugin::TagMap& downstreamParams);
    void RenderFrame();
    bool CheckBufferLatenessMayWait(AVBufferPtr buffer);
    std::shared_ptr<OHOS::Media::BlockingQueue<AVBufferPtr>> inBufQueue_ {nullptr};
    std::shared_ptr<OHOS::Media::OSAL::Task> renderTask_ {nullptr};
    std::atomic<bool> pushThreadIsBlocking_ {false};
    bool isFlushing_ {false};
    OSAL::ConditionVariable startWorkingCondition_ {};
    OSAL::Mutex mutex_;
#ifndef OHOS_LITE
    sptr<Surface> surface_;
#endif
    std::shared_ptr<Plugin::VideoSink> plugin_ {nullptr};
    int64_t frameCnt_ {0};
    int64_t refreshTime_ {0};
    bool isFirstFrame_ {true};
    uint32_t frameRate_ {0};
    bool forceRenderNextFrame_ {false};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif
#endif // MEDIA_PIPELINE_VIDEO_SINK_FILTER_H