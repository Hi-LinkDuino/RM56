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
#include "wifi_logger.h"
#include "wifi_p2p_upnp_service_response.h"

namespace OHOS {
namespace Wifi {
WifiP2pUpnpServiceResponse::WifiP2pUpnpServiceResponse(
    P2pServiceStatus status, int tranId, std::vector<unsigned char> data)
    : WifiP2pServiceResponse(P2pServicerProtocolType::SERVICE_TYPE_UP_NP, status, tranId, data), version(-1), uniqueServNames()
{}

WifiP2pUpnpServiceResponse WifiP2pUpnpServiceResponse::Create(
    P2pServiceStatus status, int tranId, std::vector<unsigned char> data)
{
    if (status != P2pServiceStatus::PSRS_SUCCESS) {
        std::vector<unsigned char> nullData;
        return WifiP2pUpnpServiceResponse(status, tranId, nullData);
    } else {
        return WifiP2pUpnpServiceResponse(status, tranId, data);
    }
}

int WifiP2pUpnpServiceResponse::GetVersion() const
{
    return version;
}

const std::vector<std::string> &WifiP2pUpnpServiceResponse::GetUniqueServNames() const
{
    return uniqueServNames;
}

bool WifiP2pUpnpServiceResponse::ParseData()
{
    if (responseData.size() < 1) {
        return false;
    }
    version = responseData.at(0) & 0xff;
    auto lastIt = responseData.begin() + 1;
    auto it = responseData.begin() + 1;
    for (; it != responseData.end(); ) {
        if (*it == ',') {
            uniqueServNames.push_back(std::string(lastIt, it));
            lastIt = it + 1;
        } else if (*it == ';') {
            uniqueServNames.push_back(std::string(lastIt, it));
            lastIt = it + 1;
            int svrNameLength = static_cast<int>(*lastIt);
            lastIt++;
            it = lastIt + svrNameLength;
            mSvrName = std::string(lastIt, it);
            return true;
        }
        ++it;
    }
    uniqueServNames.push_back(std::string(lastIt, it));
    return true;
}
}  // namespace Wifi
}  // namespace OHOS