/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_EVENT_MANAGER_ADAPT_H
#define OHOS_EVENT_MANAGER_ADAPT_H

#include <functional>
#include <map>
#include <mutex>

#include "common_event.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"
#include "dm_log.h"
#include "matching_skills.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
using CommomEventCallback = std::function<void(int32_t)>;

struct CommomEventCallbackNode {
    int32_t  input_;
    CommomEventCallback callback_;
};

class DmCommonEventManager {
DECLARE_SINGLE_INSTANCE_BASE(DmCommonEventManager);
public:
    bool SubscribeServiceEvent(const std::string &event, const CommomEventCallback callback);
    bool UnsubscribeServiceEvent(const std::string &event);
    class EventSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        EventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscribeInfo, const CommomEventCallback &callback,
            const std::string &event) : EventFwk::CommonEventSubscriber(subscribeInfo),
            callback_(callback), event_(event) {}
        void OnReceiveEvent(const EventFwk::CommonEventData &data);
    private:
        CommomEventCallback callback_;
        std::string event_;
    };
private:
    DmCommonEventManager();
    ~DmCommonEventManager();
private:
    static void DealCallback(void);
private:
    static std::mutex callbackQueueMutex_;
    static std::mutex eventSubscriberMutex_;
    static std::condition_variable notEmpty_;
    static std::list<CommomEventCallbackNode> callbackQueue_;
    std::map<std::string, std::shared_ptr<EventSubscriber>> dmEventSubscriber_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_EVENT_MANAGER_ADAPT_H
