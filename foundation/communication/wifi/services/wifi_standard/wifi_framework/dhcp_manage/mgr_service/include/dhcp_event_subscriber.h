/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_DHCP_EVENT_SUBSCRIBER_H
#define OHOS_DHCP_EVENT_SUBSCRIBER_H

#include <memory>
#include "common_event_subscriber.h"
#include "common_event_data.h"

namespace OHOS {
namespace Wifi {
class DhcpEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    explicit DhcpEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscribeInfo)
        : CommonEventSubscriber(subscribeInfo)
    {}

    ~DhcpEventSubscriber()
    {}
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override;

private:
    std::mutex onReceivedLock_;
};
}  // namespace Wifi
}  // namespace OHOS
#endif