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
#include "mock_sta_network_check.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("MockStaNetworkCheck");

namespace OHOS {
namespace Wifi {
ErrCode MockStaNetworkCheck::InitNetCheckThread()
{
    WIFI_LOGD("Enter MockDhcpService::[%{public}s].", __FUNCTION__);
    return WIFI_OPT_SUCCESS;
}
void MockStaNetworkCheck::SignalNetCheckThread()
{
    WIFI_LOGD("Enter MockDhcpService::[%{public}s].", __FUNCTION__);
}
void MockStaNetworkCheck::StopNetCheckThread()
{
    WIFI_LOGD("Enter MockDhcpService::[%{public}s].", __FUNCTION__);
}
}  // namespace Wifi
}  // namespace OHOS