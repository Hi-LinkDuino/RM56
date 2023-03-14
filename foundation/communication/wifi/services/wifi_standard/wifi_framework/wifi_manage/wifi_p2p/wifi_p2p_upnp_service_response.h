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
#ifndef OHOS_WIFI_P2P_UPNP_SERVICE_RESPONSE_H
#define OHOS_WIFI_P2P_UPNP_SERVICE_RESPONSE_H

#include "wifi_p2p_msg.h"

namespace OHOS {
namespace Wifi {
    class WifiP2pUpnpServiceResponse : public WifiP2pServiceResponse {
    public:
        /**
         * @Description Construct a new WifiP2pUpnpServiceResponse object.
         *
         * @param status - status code of the service response
         * @param tranId - transmission ID of the service response
         * @param data - data of the service response
         * @return WifiP2pUpnpServiceResponse
         */
        static WifiP2pUpnpServiceResponse Create(
            P2pServiceStatus status, int tranId, std::vector<unsigned char> data);
        /**
         * @Description Get the version object.
         *
         * @return int
         */
        int GetVersion() const;
        /**
         * @Description Get the uniqueServNames object.
         *
         * @return const std::vector<std::string>&
         */
        const std::vector<std::string> &GetUniqueServNames() const;
        /**
         * @Description Parses data and converts it to uniqueServNames.
         *
         * @return true - parse success
         * @return false - parse failed
         */
        bool ParseData();
        /**
         * @Description Destroy the WifiP2pUpnpServiceResponse object.
         *
         */
        ~WifiP2pUpnpServiceResponse() = default;

    protected:
        /**
         * @Description Construct a new WifiP2pUpnpServiceResponse object.
         *
         * @param status - status code of the service response
         * @param tranId - transmission ID of the service response
         * @param data - data of the service response
         */
        WifiP2pUpnpServiceResponse(P2pServiceStatus status, int tranId, std::vector<unsigned char> data);

    private:
        int version;
        std::vector<std::string> uniqueServNames;
    };
} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_WIFI_P2P_UPNP_SERVICE_RESPONSE_H */