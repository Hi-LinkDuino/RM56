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
#include "wifi_p2p_upnp_service_request.h"
#include "wifi_p2p_msg.h"
#include "wifi_p2p_upnp_service_info.h"
#include "securec.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("WifiP2pUpnpServiceRequest");
namespace OHOS {
namespace Wifi {
WifiP2pUpnpServiceRequest::WifiP2pUpnpServiceRequest()
    : WifiP2pServiceRequest(P2pServicerProtocolType::SERVICE_TYPE_UP_NP, std::string(""))
{}

WifiP2pUpnpServiceRequest::WifiP2pUpnpServiceRequest(std::string query)
    : WifiP2pServiceRequest(P2pServicerProtocolType::SERVICE_TYPE_UP_NP, query)
{}

WifiP2pUpnpServiceRequest WifiP2pUpnpServiceRequest::Create()
{
    return WifiP2pUpnpServiceRequest();
}

WifiP2pUpnpServiceRequest WifiP2pUpnpServiceRequest::Create(std::string searchTarget)
{
    std::string query;
    char version[4] = {'\0'};
    if (searchTarget.empty()) {
        WIFI_LOGE("failed, search target cann't be nullptr, something wrong may happen.");
    }
    if (snprintf_s(version, sizeof(version), sizeof(version) - 1, "%02x ", WifiP2pUpnpServiceInfo::VERSION_1_0) < 0) {
        WIFI_LOGE("ifiP2pUpnpServiceRequest::Create snprintf_s failed.");
        WifiP2pUpnpServiceRequest retErr;
        return retErr;
    }
    query.append(std::string(version));
    query.append(WifiP2pServiceInfo::Bin2HexStr(searchTarget));
    WifiP2pUpnpServiceRequest ret(query);

    return ret;
}
}  // namespace Wifi
}  // namespace OHOS