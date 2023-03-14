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

#ifndef OHOS_WIFI_MOCK_P2P_SERVICE_H
#define OHOS_WIFI_MOCK_P2P_SERVICE_H

#include "wifi_p2p_msg.h"
#include "ip2p_service.h"
#include "ip2p_service_callbacks.h"

namespace OHOS {
namespace Wifi {
const int TEST_ONE_THOUSAND = 1000;
const int TEST_TWO_THOUSAND = 2000;
class WifiMockP2pService : public IP2pService {
public:
    ErrCode EnableP2p();
    ErrCode DisableP2p();
    ErrCode DiscoverDevices();
    ErrCode StopDiscoverDevices();
    ErrCode DiscoverServices();
    ErrCode StopDiscoverServices();
    ErrCode RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request);
    ErrCode PutLocalP2pService(const WifiP2pServiceInfo &srvInfo);
    ErrCode DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo);
    ErrCode StartP2pListen(int period, int interval);
    ErrCode StopP2pListen();
    ErrCode CreateGroup(const WifiP2pConfig &config);
    ErrCode RemoveGroup();
    ErrCode DeleteGroup(const WifiP2pGroupInfo &group);
    ErrCode P2pConnect(const WifiP2pConfig &config);
    ErrCode P2pCancelConnect();
    ErrCode QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo);
    ErrCode GetCurrentGroup(WifiP2pGroupInfo &group);
    ErrCode GetP2pEnableStatus(int &status);
    ErrCode GetP2pDiscoverStatus(int &status);
    ErrCode GetP2pConnectedStatus(int &status);
    ErrCode QueryP2pDevices(std::vector<WifiP2pDevice> &devives);
    ErrCode QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups);
    ErrCode QueryP2pServices(std::vector<WifiP2pServiceInfo> &services);
    ErrCode RegisterP2pServiceCallbacks(const IP2pServiceCallbacks &callbacks);

public:
    IP2pServiceCallbacks mCallback;
};
}  // namespace Wifi
}  // namespace OHOS
#endif