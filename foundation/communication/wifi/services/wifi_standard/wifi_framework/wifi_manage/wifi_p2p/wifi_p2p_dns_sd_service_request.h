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
#ifndef OHOS_WIFI_P2P_DNS_SERVICE_REQUEST_H
#define OHOS_WIFI_P2P_DNS_SERVICE_REQUEST_H

#include "wifi_p2p_msg.h"
#include "p2p_macro.h"

namespace OHOS {
namespace Wifi {
class WifiP2pDnsSdServiceRequest : public WifiP2pServiceRequest {
    FRIEND_GTEST(WifiP2pDnsSdServiceRequest);
public:
    /**
     * @Description Construct a default service discovery request instance.
     *
     * @return WifiP2pDnsSdServiceRequest
     */
    static WifiP2pDnsSdServiceRequest Create();

    /**
     * @Description Construct a default service discovery request instance using a service type.
     *
     * @param serviceType - DNS service type
     * @return WifiP2pDnsSdServiceRequest
     */
    static WifiP2pDnsSdServiceRequest Create(const std::string &serviceType);

    /**
     * @Description Constructing a service discovery request instance.
     *
     * @param instanceName - DNS service name
     * @param serviceType - DNS service type
     * @return WifiP2pDnsSdServiceRequest
     */
    static WifiP2pDnsSdServiceRequest Create(const std::string &instanceName, const std::string &serviceType);

private:
    /**
     * @Description Construction method.
     *
     * @param query - query string
     */
    explicit WifiP2pDnsSdServiceRequest(const std::string &query);

    /**
     * @Description Construction method.
     *
     */
    WifiP2pDnsSdServiceRequest();
    /**
     * @Description Construction method.
     *
     * @param dnsQuery - query string
     * @param dnsType - DNS type
     * @param version - DNS version
     */
    WifiP2pDnsSdServiceRequest(const std::string &dnsQuery, int dnsType, int version);
    /**
     * @Description Destroy the WifiP2pDnsSdServiceRequest object.
     *
     */
    ~WifiP2pDnsSdServiceRequest() = default;
};
}  // namespace Wifi
}  // namespace OHOS
#endif