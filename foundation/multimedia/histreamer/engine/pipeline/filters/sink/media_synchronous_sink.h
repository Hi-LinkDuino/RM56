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

#ifndef HISTREAMER_PIPELINE_MEDIA_SYNC_SINK_H
#define HISTREAMER_PIPELINE_MEDIA_SYNC_SINK_H
#include <functional>
#include "pipeline/core/filter_base.h"
#include "pipeline/core/media_sync_manager.h"
namespace OHOS {
namespace Media {
namespace Pipeline {
class MediaSynchronousSink : public FilterBase, public IMediaSynchronizer {
public:
    explicit MediaSynchronousSink(const std::string& name) : FilterBase(name) {}
    ~MediaSynchronousSink() override;
    void WaitAllPrerolled(bool shouldWait) final;
    int8_t GetPriority() final;

    void NotifyAllPrerolled() final;

    void Init(EventReceiver *receiver, FilterCallback *callback) override;

    std::string GetSynchronizerName() override
    {
        return name_;
    }
protected:
    virtual ErrorCode DoSyncWrite(const AVBufferPtr& buffer) = 0;
    virtual ErrorCode WriteToPluginRefTimeSync(const AVBufferPtr& buffer);
    virtual void ResetSyncInfo() = 0;

    void ResetPrerollReported();
    void UpdateMediaTimeRange(const Plugin::Meta& meta);

    int8_t syncerPriority_ {IMediaSynchronizer::NONE};
    bool hasReportedPreroll_ {false};
    std::atomic<bool> waitForPrerolled_ {false};
    OSAL::Mutex prerollMutex_ {};
    OSAL::ConditionVariable prerollCond_ {};

    int64_t waitPrerolledTimeout_ {0};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_PIPELINE_MEDIA_SYNC_SINK_H
