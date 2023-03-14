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

#include "dhcp_event_subscriber.h"

#include "wifi_logger.h"
#include "dhcp_client_service_impl.h"

DEFINE_WIFILOG_DHCP_LABEL("DhcpEventSubscriber");

using namespace OHOS::EventFwk;

namespace OHOS {
namespace Wifi {
void DhcpEventSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    std::lock_guard<std::mutex> lock(onReceivedLock_);
    int code = data.GetCode();
    std::string strData = data.GetData();
    WIFI_LOGI("OnReceiveEvent() GetCode:%{public}d, GetData:%{private}s.", code, strData.c_str());
    if (DhcpClientServiceImpl::DhcpEventResultHandle(code, strData) != DHCP_OPT_SUCCESS) {
        WIFI_LOGE("OnReceiveEvent() DhcpEventResultHandle failed!");
        return;
    }
}
}  // namespace Wifi
}  // namespace OHOS