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
#include "wifi_p2p_dns_sd_service_request.h"
#include "wifi_p2p_dns_sd_service_info.h"

namespace OHOS {
namespace Wifi {
WifiP2pDnsSdServiceRequest::WifiP2pDnsSdServiceRequest(const std::string &query)
    : WifiP2pServiceRequest(P2pServicerProtocolType::SERVICE_TYPE_BONJOUR, query)
{}
WifiP2pDnsSdServiceRequest::WifiP2pDnsSdServiceRequest()
    : WifiP2pServiceRequest(P2pServicerProtocolType::SERVICE_TYPE_BONJOUR, std::string(""))
{}

WifiP2pDnsSdServiceRequest::WifiP2pDnsSdServiceRequest(const std::string &dnsQuery, int dnsType, int version)
    : WifiP2pServiceRequest(P2pServicerProtocolType::SERVICE_TYPE_BONJOUR,
          WifiP2pDnsSdServiceInfo::BuildRequest(dnsQuery, dnsType, version))
{}

WifiP2pDnsSdServiceRequest WifiP2pDnsSdServiceRequest::Create()
{
    WifiP2pDnsSdServiceRequest req;
    return req;
}

WifiP2pDnsSdServiceRequest WifiP2pDnsSdServiceRequest::Create(const std::string &serviceType)
{
    WifiP2pDnsSdServiceRequest req(
        serviceType + ".local.", WifiP2pDnsSdServiceInfo::DNS_PTR_TYPE, WifiP2pDnsSdServiceInfo::VERSION_1);
    return req;
}

WifiP2pDnsSdServiceRequest WifiP2pDnsSdServiceRequest::Create(
    const std::string &instanceName, const std::string &serviceType)
{
    WifiP2pDnsSdServiceRequest req(instanceName + "." + serviceType + ".local.",
        WifiP2pDnsSdServiceInfo::DNS_TXT_TYPE,
        WifiP2pDnsSdServiceInfo::VERSION_1);
    return req;
}
}  // namespace Wifi
}  // namespace OHOS