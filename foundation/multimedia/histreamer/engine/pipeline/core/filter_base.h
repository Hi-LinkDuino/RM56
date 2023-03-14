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

#ifndef HISTREAMER_PIPELINE_CORE_FILTER_BASE_H
#define HISTREAMER_PIPELINE_CORE_FILTER_BASE_H
#include <atomic>
#include <functional>
#include <list>
#include <memory>

#include "foundation/pre_defines.h"
#include "filter.h"
#include "error_code.h"
#include "utils/constants.h"
#include "event.h"
#include "filter_type.h"
#include "plugin/core/plugin_info.h"
#include "plugin/core/base.h"
#include "port.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class FilterBase : public Filter, public Plugin::CallbackWrap {
public:
    explicit FilterBase(std::string name);
    ~FilterBase() override = default;
    void Init(EventReceiver* receiver, FilterCallback* callback) override;
    PInPort GetInPort(const std::string& name) override;
    POutPort GetOutPort(const std::string& name) override;
    const std::string& GetName() override
    {
        return name_;
    }
    const EventReceiver* GetOwnerPipeline() const override
    {
        return eventReceiver_;
    }

    ErrorCode Prepare() override;
    ErrorCode Start() override;
    ErrorCode Pause() override;
    ErrorCode Stop() override;
    ErrorCode Resume() override
    {
        return Start();
    }
    void FlushStart() override
    {
    }
    void FlushEnd() override
    {
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

    void UnlinkPrevFilters() override;

    std::vector<Filter*> GetNextFilters() override;

    std::vector<Filter*> GetPreFilters() override;

    ErrorCode PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset) override;
    ErrorCode PullData(const std::string& outPort, uint64_t offset, size_t size, AVBufferPtr& data) override;
    std::vector<WorkMode> GetWorkModes() override
    {
        return {WorkMode::PUSH};
    }

    // Port调用此方法向Filter报告事件
    void OnEvent(const Event& event) override;

    void SetSyncCenter(std::weak_ptr<IMediaSyncCenter> syncCenter) final;
protected:
    virtual void InitPorts();

    ErrorCode ConfigPluginWithMeta(Plugin::Base& plugin, const Plugin::Meta& meta);

    std::string NamePort(const std::string& mime);

    /**
     * 获取routemap中 outPortName对应的inport
     *
     * @param outPortName outport name
     * @return null if not exists
     */
    PInPort GetRouteInPort(const std::string& outPortName);

    /**
     * 获取routemap中 inPortName对应的outport
     *
     * @param inPortName inport name
     * @return null if not exists
     */
    POutPort GetRouteOutPort(const std::string& inPortName);

    template<typename T>
    static bool UpdateAndInitPluginByInfo(std::shared_ptr<T>& plugin, std::shared_ptr<Plugin::PluginInfo>& pluginInfo,
        const std::shared_ptr<Plugin::PluginInfo>& selectedPluginInfo,
        const std::function<std::shared_ptr<T>(const std::string&)>& pluginCreator);

protected:
    void OnEvent(const Plugin::PluginEvent &event) override;
    std::string name_;
    std::atomic<FilterState> state_;
    EventReceiver* eventReceiver_;
    FilterCallback* callback_;
    std::vector<PFilter> children_ {};
    std::vector<PInPort> inPorts_ {};
    std::vector<POutPort> outPorts_ {};
    std::vector<PairPort> routeMap_ {}; // inport -> outport

    std::map<std::string, uint32_t> mediaTypeCntMap_ {};

    FilterType filterType_ {FilterType::NONE};

    std::shared_ptr<Plugin::PluginInfo> pluginInfo_{};

    std::weak_ptr<IMediaSyncCenter> syncCenter_ {};
private:
    template <typename T>
    static T FindPort(const std::vector<T>& list, const std::string& name);
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif
