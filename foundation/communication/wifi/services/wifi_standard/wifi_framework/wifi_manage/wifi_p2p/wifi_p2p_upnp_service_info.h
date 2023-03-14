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
#ifndef OHOS_WIFI_P2P_UPNP_SERVICE_INFO_H
#define OHOS_WIFI_P2P_UPNP_SERVICE_INFO_H

#include "wifi_p2p_msg.h"
#include "p2p_macro.h"

namespace OHOS {
namespace Wifi {
    class WifiP2pUpnpServiceInfo : public WifiP2pServiceInfo {
    FRIEND_GTEST(WifiP2pUpnpServiceInfo);
    public:
        static const int VERSION_1_0 = 0x10;
        /**
         * @Description Construct a new WifiP2pUpnpServiceInfo object.
         *
         * @param uuid - string of uuid
         * @param device - string of device
         * @param services - string list of services
         * @param svrName - service name
         * @return WifiP2pUpnpServiceInfo
         */
        static WifiP2pUpnpServiceInfo Create(const std::string &uuid, const std::string &device,
                                                  const std::vector<std::string> &services, const std::string &svrName);
        /**
         * @Description Destroy the WifiP2pUpnpServiceInfo object.
         *
         */
        ~WifiP2pUpnpServiceInfo() = default;

    private:
        /**
         * @Description Construct the character string used to supplicant command.
         *
         * @param uuid - string of uuid
         * @param data - data to be converted
         * @param svrName - service name
         * @return std::string
         */
        static std::string BuildWpaQuery(const std::string &uuid, const std::string &data, const std::string &svrName);

        /**
         * @Description Construct a new WifiP2pUpnpServiceInfo object.
         *
         * @param queryList - the character string list used to supplicant command
         */
        explicit WifiP2pUpnpServiceInfo(std::vector<std::string> &queryList);
    };
} // namespace Wifi
} // namespace OHOS


#endif