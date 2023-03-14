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
#ifndef OHOS_WIFI_P2P_DNS_SD_SERVICE_RESPONSE_H
#define OHOS_WIFI_P2P_DNS_SD_SERVICE_RESPONSE_H

#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include "wifi_p2p_msg.h"
#include "p2p_macro.h"

namespace OHOS {
namespace Wifi {
class WifiP2pDnsSdServiceResponse : public WifiP2pServiceResponse {
    FRIEND_GTEST(WifiP2pDnsSdServiceResponse);
public:
    /**
     * @Description Construct a new WifiP2pDnsSdServiceResponse object.
     *
     * @param status - service status
     * @param tranId - transmission ID
     * @param data - detailed data of the service response
     */
    WifiP2pDnsSdServiceResponse(
        P2pServiceStatus status, int tranId, const std::vector<unsigned char> &data);

    /**
     * @Description Construct a new WifiP2pDnsSdServiceResponse object.
     *
     * @param resp - base class object of the DNS service response
     */
    explicit WifiP2pDnsSdServiceResponse(const WifiP2pServiceResponse &resp);

    /**
     * @Description Destroy the WifiP2pDnsSdServiceResponse object.
     *
     */
    ~WifiP2pDnsSdServiceResponse() = default;

    /**
     * @Description Parses the data in the service discovery response.
     *
     * @return true - parses success
     * @return false - parses failed
     */
    bool ParseData();

    /**
     * @Description Get the dnsQueryName object.
     *
     * @return const std::string&
     */
    const std::string &GetQueryName() const;

    /**
     * @Description Get the instanceName object.
     *
     * @return const std::string&
     */
    const std::string &GetInstanceName() const;

    /**
     * @Description Get the dnsType object.
     *
     * @return int
     */
    int GetDnsType() const;

    /**
     * @Description Get the version object.
     *
     * @return int
     */
    int GetVersion() const;

    /**
     * @Description Get the txtRecord object.
     *
     * @return const std::map<std::string, std::string>&
     */
    const std::map<std::string, std::string> &GetTxtRecord() const;
private:
    /**
     * @Description Resolve DNS name data.
     *
     * @param istream - stream instance of service discovery response content
     * @param dnsName - instance of saving DNS name resolution
     * @return true - success
     * @return false - failed
     */
    bool FetchDnsName(std::istringstream &istream, std::string &dnsName);

    /**
     * @Description Parsing TXT data.
     *
     * @param istream - stream instance of service discovery response content
     * @return true - success
     * @return false - failed
     */
    bool FetchTxtData(std::istringstream &istream);

    /**
     * @Description Initialization.
     *
     */
    void Init();

    std::string dnsQueryName;
    std::string instanceName;
    int dnsType;
    int version;
    std::map<std::string, std::string> txtRecord;
    std::map<unsigned char, std::string> sVmpack;
};

} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_WIFI_P2P_DNS_SD_SERVICE_RESPONSE_H */