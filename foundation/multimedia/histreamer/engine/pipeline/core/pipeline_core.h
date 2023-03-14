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

#ifndef HISTREAMER_PIPELINE_CORE_PIPELINE_CORE_H
#define HISTREAMER_PIPELINE_CORE_PIPELINE_CORE_H

#include <algorithm>
#include <atomic>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include <stack>

#include "filter_base.h"
#include "error_code.h"
#include "osal/thread/mutex.h"
#include "plugin/common/plugin_types.h"
#include "plugin/core/plugin_meta.h"
#include "pipeline.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class MetaBundle {
public:
    MetaBundle() = default;
    ~MetaBundle() = default;
    std::shared_ptr<const Plugin::Meta> GetGlobalMeta()
    {
        return globalMeta_;
    }

    std::shared_ptr<const Plugin::Meta> GeTrackMeta(int32_t trackId);

    void UpdateGlobalMeta(const Plugin::Meta& meta);

    void UpdateTrackMeta(const Plugin::Meta& meta);

private:
    std::shared_ptr<Plugin::Meta> globalMeta_;
    std::vector<std::shared_ptr<Plugin::Meta>> trackMeta_;
};

class PipelineCore : public Pipeline {
public:
    explicit PipelineCore(const std::string& name = "pipeline_core");

    ~PipelineCore() override = default;

    const std::string& GetName() override;

    const EventReceiver* GetOwnerPipeline() const override;

    void Init(EventReceiver* receiver, FilterCallback* callback) override;

    ErrorCode Prepare() override;

    ErrorCode Start() override;

    ErrorCode Pause() override;

    ErrorCode Resume() override;

    ErrorCode Stop() override;

    void FlushStart() override;

    void FlushEnd() override;

    FilterState GetState();

    ErrorCode AddFilters(std::initializer_list<Filter*> filtersIn) override;
    ErrorCode RemoveFilter(Filter* filter) override;
    ErrorCode RemoveFilterChain(Filter* firstFilter) override;
    ErrorCode LinkFilters(std::initializer_list<Filter*> filters) override;
    ErrorCode LinkPorts(std::shared_ptr<OutPort> port1, std::shared_ptr<InPort> port2) override;

    void OnEvent(const Event& event) override;

    void UnlinkPrevFilters() override
    {
    }
    std::vector<Filter*> GetNextFilters() override
    {
        return {};
    }
    std::vector<Filter*> GetPreFilters() override
    {
        return {};
    }
    ErrorCode PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset) override
    {
        UNUSED_VARIABLE(inPort);
        UNUSED_VARIABLE(buffer);
        UNUSED_VARIABLE(offset);
        return ErrorCode::ERROR_UNIMPLEMENTED;
    }
    ErrorCode PullData(const std::string& outPort, uint64_t offset, size_t size, AVBufferPtr& data) override
    {
        UNUSED_VARIABLE(outPort);
        UNUSED_VARIABLE(offset);
        UNUSED_VARIABLE(size);
        UNUSED_VARIABLE(data);
        return ErrorCode::ERROR_UNIMPLEMENTED;
    }
    std::vector<WorkMode> GetWorkModes() override
    {
        return {WorkMode::PUSH};
    }

    PInPort GetInPort(const std::string& name) override
    {
        UNUSED_VARIABLE(name);
        return nullptr;
    }
    POutPort GetOutPort(const std::string& name) override
    {
        UNUSED_VARIABLE(name);
        return nullptr;
    }

    ErrorCode SetParameter(int32_t key, const Plugin::Any& value) override
    {
        UNUSED_VARIABLE(key);
        UNUSED_VARIABLE(value);
        return ErrorCode::ERROR_UNIMPLEMENTED;
    }
    ErrorCode GetParameter(int32_t key, Plugin::Any& value) override
    {
        UNUSED_VARIABLE(key);
        UNUSED_VARIABLE(value);
        return ErrorCode::ERROR_UNIMPLEMENTED;
    }

    void SetSyncCenter(std::weak_ptr<IMediaSyncCenter> syncCenter) final;

    void InitFilters(const std::vector<Filter*>& filters);

    std::shared_ptr<MetaBundle> GetMetaBundle()
    {
        return metaBundle_;
    }

private:
    void ReorderFilters();

    void NotifyEvent(const Event& event);

    std::string name_;
    size_t readyEventCnt_ {0};
    FilterState state_ {FilterState::CREATED};
    OSAL::Mutex mutex_ {};
    std::vector<Filter*> filters_ {};
    EventReceiver* eventReceiver_;
    FilterCallback* filterCallback_;
    std::shared_ptr<MetaBundle> metaBundle_;
    std::vector<Filter*> filtersToRemove_ {};
    std::weak_ptr<IMediaSyncCenter> syncCenter_ {};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PIPELINE_CORE_PIPELINE_CORE_H
