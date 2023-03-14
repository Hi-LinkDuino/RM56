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
#ifndef HIVIEW_BASE_EVENT_DISPATCHER_H
#define HIVIEW_BASE_EVENT_DISPATCHER_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "event.h"
#include "plugin.h"

namespace OHOS {
namespace HiviewDFX {
class EventDispatcher {
public:
    EventDispatcher(){};
    ~EventDispatcher() = default;

    void AddInterestType(uint32_t type);
    void ClearInvalidListeners();
    void DispatchEvent(Event event);
    void RegisterListener(std::weak_ptr<EventListener> listener);
    void RegisterListener(std::weak_ptr<Plugin> listener);

protected:
    bool channelPluginFind(std::string name, int32_t type);
    std::set<uint32_t> types_;
    std::map<uint32_t, std::list<std::weak_ptr<EventListener>>> channelMapper_;
    std::map<uint32_t, std::list<std::weak_ptr<Plugin>>> channelPlugin_;

private:
    std::mutex lock_;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEW_BASE_EVENT_DISPATCHER_H