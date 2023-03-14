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
#ifndef OHOS_IP2P_SERVICE_CALLBACK_H
#define OHOS_IP2P_SERVICE_CALLBACK_H

#include <functional>
#include "wifi_errcode.h"
#include "wifi_p2p_msg.h"

namespace OHOS {
namespace Wifi {
/* The callbacks of P2P service provided */
struct IP2pServiceCallbacks {
    /* The event of status change. */
    std::function<void(P2pState)> OnP2pStateChangedEvent;
    /* Report the latest devices discovery information. */
    std::function<void(const std::vector<WifiP2pDevice> &)> OnP2pPeersChangedEvent;
    /* Report the latest services discovery information. */
    std::function<void(const std::vector<WifiP2pServiceInfo> &)> OnP2pServicesChangedEvent;
    /* The event of connection status change. */
    std::function<void(const WifiP2pLinkedInfo &)> OnP2pConnectionChangedEvent;
    /* The event of this device configure has change */
    std::function<void(const WifiP2pDevice &)> OnP2pThisDeviceChangedEvent;
    /* The event of discovery status change */
    std::function<void(bool)> OnP2pDiscoveryChangedEvent;
    /* The event of groups configure has change */
    std::function<void()> OnP2pGroupsChangedEvent;
    /* The result returned by the asynchronous interface */
    std::function<void(P2pActionCallback, ErrCode)> OnP2pActionResultEvent;
    std::function<void(P2pServicerProtocolType, const std::vector<unsigned char> &, const WifiP2pDevice &)>
        OnP2pServiceAvailable;
    std::function<void(const std::string &, const std::string &, const WifiP2pDevice &)> OnP2pDnsSdServiceAvailable;
    std::function<void(const std::string &, const std::map<std::string, std::string> &, const WifiP2pDevice &)>
        OnP2pDnsSdTxtRecordAvailable;
    std::function<void(const std::vector<std::string> &, const WifiP2pDevice &)> OnP2pUpnpServiceAvailable;
};
}  // namespace Wifi
}  // namespace OHOS
#endif  // OHOS_IP2P_SERVICE_CALLBACK_H