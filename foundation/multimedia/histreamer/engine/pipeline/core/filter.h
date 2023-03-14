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

#ifndef HISTREAMER_PIPELINE_CORE_FILTER_H
#define HISTREAMER_PIPELINE_CORE_FILTER_H
#include <functional>
#include <list>
#include <memory>

#include "filter_callback.h"
#include "error_code.h"
#include "utils/constants.h"
#include "i_media_sync_center.h"
#include "event.h"
#include "parameter.h"
#include "port.h"
#include "plugin/core/plugin_meta.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class Filter;

using PFilter = std::shared_ptr<Filter>;
using PPort = std::shared_ptr<Port>;
using PInPort = std::shared_ptr<InPort>;
using POutPort = std::shared_ptr<OutPort>;
using PairPort = std::pair<std::string, std::string>;

enum class FilterState {
    CREATED,     // Filter created
    INITIALIZED, // Init called
    PREPARING,   // Prepare called
    READY,       // Ready Event reported
    RUNNING,     // Start called
    PAUSED,      // Pause called
};

// EventReceiver:
//   1. Port使用此接口传递事件给Filter
//   2. Filter使用此接口传递事件给Pipeline
//   3. Sub Filter使用此接口传递事件给 Parent Filter
class EventReceiver {
public:
    virtual ~EventReceiver() = default;
    virtual void OnEvent(const Event& event) = 0;
};

// InfoTransfer:
//   Port使用此接口从Filter获取信息 或 传递信息给Filter
class InfoTransfer : public EventReceiver {
public:
    virtual const std::string& GetName() = 0;
    virtual std::vector<WorkMode> GetWorkModes() = 0; // OutPort调用

    // InPort调用此接口确定是否要继续往后协商
    virtual bool Negotiate(const std::string& inPort,
                           const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                           Plugin::Capability& negotiatedCap,
                           const Plugin::TagMap& upstreamParams,
                           Plugin::TagMap& downstreamParams)
    {
        return false;
    }

    virtual bool Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
    {
        return false;
    }

    /**
     * push data to this filter
     *
     * @param inPort in port
     * @param buffer in buffer
     * @param offset means the offset from the start of the stream between Filter.Start and Filter.Stop. -1 means ignore
     * this parameter.
     * @return
     */
    virtual ErrorCode PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset) = 0; // InPort调用
    virtual ErrorCode PullData(const std::string& outPort, uint64_t offset, size_t size,
                               AVBufferPtr& data) = 0; // OutPort调用
    virtual const EventReceiver* GetOwnerPipeline() const = 0;
};

class Filter : public InfoTransfer {
public:
    ~Filter() override = default;
    virtual void Init(EventReceiver* receiver, FilterCallback* callback) = 0;
    virtual PInPort GetInPort(const std::string& name) = 0;
    virtual POutPort GetOutPort(const std::string& name) = 0;

    virtual ErrorCode Prepare() = 0;
    virtual ErrorCode Start() = 0;
    virtual ErrorCode Pause() = 0;
    virtual ErrorCode Resume() = 0;
    virtual ErrorCode Stop() = 0;

    // 清除缓存
    virtual void FlushStart() = 0;
    virtual void FlushEnd() = 0;

    // Filter的使用者可以直接调用Filter的SetParameter接口设置参数
    virtual ErrorCode SetParameter(int32_t key, const Plugin::Any& value) = 0;
    virtual ErrorCode GetParameter(int32_t key, Plugin::Any& value) = 0;

    virtual void UnlinkPrevFilters() = 0;
    virtual std::vector<Filter*> GetNextFilters() = 0;
    virtual std::vector<Filter*> GetPreFilters() = 0;
    virtual void SetSyncCenter(std::weak_ptr<IMediaSyncCenter> mediaSyncCenter) = 0;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PIPELINE_CORE_FILTER_H
