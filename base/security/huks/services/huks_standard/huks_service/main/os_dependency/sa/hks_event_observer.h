/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HKS_EVENT_OBSERVER_H
#define HKS_EVENT_OBSERVER_H

#include <memory>

#include "common_event_manager.h"
#include "common_event_subscriber.h"

namespace OHOS {
namespace Security {
namespace Hks {
class SystemEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    explicit SystemEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo);
    ~SystemEventSubscriber() = default;
    virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data);
};

class SystemEventObserver {
public:
    SystemEventObserver() = default;
    ~SystemEventObserver();
    static bool SubscribeSystemEvent();
    static bool UnSubscribeSystemEvent();

private:
    static std::shared_ptr<SystemEventSubscriber> systemEventSubscriber_;
};
} // namespace Hks
} // namespace Security
} // namespace OHOS

#endif  // HKS_EVENT_OBSERVER_H
