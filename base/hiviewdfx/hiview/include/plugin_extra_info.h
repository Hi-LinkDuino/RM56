/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef HIVIEW_PUBLIC_PLUGIN_EXTRA_INFO_DEFINES_H
#define HIVIEW_PUBLIC_PLUGIN_EXTRA_INFO_DEFINES_H
#include <string>
#include <vector>
#include <list>

#include "event.h"

namespace OHOS {
namespace HiviewDFX {
static const std::string DISTRIBUTED_COMMUNICATOR_PLUGIN = "DistributedCommunicatorPlugin";
struct PluginCapacityInfo {
    std::string name_; // plugin name
    std::vector<std::string> capacities_; // capacity list
    std::string type_; // reserved
    std::vector<std::string> accessAuth_; // reserved
};
enum CapacityEventId : uint32_t {
    CAP_PUBLISH = 0x5a,
    CAP_OBTAIN
};

class CapacityPublishEvent : public Event {
public:
    CapacityPublishEvent(const std::string &sender, const PluginCapacityInfo &pluginCapacityInfo)
        : Event(sender), pluginCapacity_(pluginCapacityInfo) {};
    virtual ~CapacityPublishEvent() {};
    PluginCapacityInfo pluginCapacity_;
};

class CapacityObtainEvent : public Event {
public:
    CapacityObtainEvent(const std::string &plugin, const std::string capacity)
        : Event(plugin), plugin_(plugin), capacity_(capacity) {};
    virtual ~CapacityObtainEvent() {};
    std::string plugin_;
    std::string capacity_;
    std::list<std::string> deviceList_;
};
} // end of namespace HiviewDFX
} // end of namespace OHOS
#endif
