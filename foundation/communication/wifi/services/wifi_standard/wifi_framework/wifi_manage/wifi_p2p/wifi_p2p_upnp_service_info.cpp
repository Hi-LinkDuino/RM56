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
#include "wifi_p2p_upnp_service_info.h"
#include "securec.h"

namespace OHOS {
namespace Wifi {
WifiP2pUpnpServiceInfo::WifiP2pUpnpServiceInfo(std::vector<std::string> &queryList) : WifiP2pServiceInfo(queryList)
{}

WifiP2pUpnpServiceInfo WifiP2pUpnpServiceInfo::Create(const std::string &uuid, const std::string &device,
    const std::vector<std::string> &services, const std::string &svrName)
{
    std::vector<std::string> upnpInfo;
    upnpInfo.push_back(BuildWpaQuery(uuid, std::string(""), svrName));
    upnpInfo.push_back(BuildWpaQuery(uuid, std::string("upnp:rootdevice"), svrName));
    upnpInfo.push_back(BuildWpaQuery(uuid, device, svrName));
    for (unsigned long i = 0; i < services.size(); ++i) {
        std::string service = services.at(i);
        upnpInfo.push_back(BuildWpaQuery(uuid, service, svrName));
    }
    WifiP2pUpnpServiceInfo upnpServInfo(upnpInfo);
    upnpServInfo.SetServiceName(svrName);
    upnpServInfo.SetServicerProtocolType(P2pServicerProtocolType::SERVICE_TYPE_UP_NP);
    return upnpServInfo;
}

std::string WifiP2pUpnpServiceInfo::BuildWpaQuery(
    const std::string &uuid, const std::string &data, const std::string &svrName)
{
    std::string query;
    query.append("upnp ");
    char version[4] = {'\0'};
    if (snprintf_s(version, sizeof(version), sizeof(version) - 1, "%02x ", VERSION_1_0) < 0) {
        return "";
    }
    query.append(std::string(version));
    query.append(std::string("uuid:"));
    query.append(uuid);
    if (!data.empty()) {
        query.append(std::string("::"));
        query.append(data);
    }
    if (!svrName.empty()) {
        query.append(std::string(";"));
        query.append(1, static_cast<unsigned char>(svrName.length()));
        query.append(svrName);
    }
    return query;
}
}  // namespace Wifi
}  // namespace OHOS
