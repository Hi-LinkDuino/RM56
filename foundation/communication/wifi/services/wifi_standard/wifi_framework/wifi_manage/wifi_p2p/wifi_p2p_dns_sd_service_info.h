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
#ifndef OHOS_WIFI_P2P_DNS_SERVICE_INFO_H
#define OHOS_WIFI_P2P_DNS_SERVICE_INFO_H

#include <string>
#include <map>

#include "wifi_p2p_msg.h"
#include "wifi_p2p_dns_txt_record.h"

namespace OHOS {
namespace Wifi {
class WifiP2pDnsSdServiceInfo : public WifiP2pServiceInfo {
    friend class WifiP2pDnsSdServiceInfoTest;

public:
    enum { VERSION_1 = 1, DNS_PTR_TYPE = 12, DNS_TXT_TYPE = 16 };
    /**
     * @Description Construct a new WifiP2pDnsSdServiceInfo object.
     *
     * @param queryList list of query character
     */
    explicit WifiP2pDnsSdServiceInfo(const std::vector<std::string> &queryList);

    /**
     * @Description Destroy the WifiP2pDnsSdServiceInfo object.
     *
     */
    ~WifiP2pDnsSdServiceInfo() = default;

    /**
     * @Description Creating a dns service.
     *
     * @param instanceName - name of the dns service
     * @param serviceType - dns service type name
     * @param txtMap - TXT character table of the dns service
     * @param svrName - service name
     * @return WifiP2pDnsSdServiceInfo
     */
    static WifiP2pDnsSdServiceInfo Create(const std::string &instanceName, const std::string &serviceType,
        const std::map<std::string, std::string> &txtMap, const std::string &svrName);

    /**
     * @Description Create service discovery request query string.
     *
     * @param dnsName - DNS character name
     * @param dnsType - DNS service type
     * @param version - DNS service version
     * @return std::string
     */
    static std::string BuildRequest(const std::string &dnsName, int dnsType, int version);

private:
    /**
     * @Description Construct a new WifiP2pDnsSdServiceInfo object.
     *
     */
    WifiP2pDnsSdServiceInfo();

    /**
     * @Description Creating a PTR query.
     *
     * @param instanceName - instance name
     * @param serviceType - DNS service type
     * @param svrName - service name
     * @return std::string
     */
    static std::string BuildPtrServiceQuery(
        const std::string &instanceName, const std::string &serviceType, const std::string &svrName);

    /**
     * @Description Creating a TXT query.
     *
     * @param instanceName - instance name
     * @param serviceType - DNS service type
     * @param txtRecord - TXT record object
     * @param svrName - service name
     * @return std::string
     */
    static std::string BuildTxtServiceQuery(const std::string &instanceName, const std::string &serviceType,
        const WifiP2PDnsTxtRecord &txtRecord, const std::string &svrName);

    /**
     * @Description DNS name resolution.
     *
     * @param dnsName - DNS name
     * @return std::string
     */
    static std::string TurnDnsNameToStream(const std::string &dnsName);
};
}  // namespace Wifi
}  // namespace OHOS

#endif