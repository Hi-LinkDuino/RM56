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
#ifndef OHOS_I_WIFI_P2P_CALLBACK_H
#define OHOS_I_WIFI_P2P_CALLBACK_H

#include <string>
#include <iremote_stub.h>
#include "message_parcel.h"
#include "message_option.h"
#include "wifi_p2p_msg.h"
#include "define.h"
#include "wifi_errcode.h"

namespace OHOS {
namespace Wifi {
class IWifiP2pCallback : public IRemoteBroker {
public:
    /**
     * @Description Deal p2p state change message.
     *
     * @param state - P2P_STATE_IDLE/P2P_STATE_STARTED/P2P_STATE_CLOSED
     */
    virtual void OnP2pStateChanged(int state) = 0;

    /**
     * @Description Persistent Group Changed Resources
     *
     */
    virtual void OnP2pPersistentGroupsChanged(void) = 0;

    /**
     * @Description The device is changed.
     *
     * @param device - WifiP2pDevice object
     */
    virtual void OnP2pThisDeviceChanged(const WifiP2pDevice &device) = 0;

    /**
     * @Description If the discover P2P device information is updated, all the
     *        latest WifiP2P devices are reported.
     *
     * @param device - std::vector<WifiP2pDevice> object
     */
    virtual void OnP2pPeersChanged(const std::vector<WifiP2pDevice> &device) = 0;

    /**
     * @Description This event is triggered when the discovered services are updated.
     *
     * @param srvInfo - std::vector<WifiP2pServiceInfo> object
     */
    virtual void OnP2pServicesChanged(const std::vector<WifiP2pServiceInfo> &srvInfo) = 0;

    /**
     * @Description Connection status change.
     *
     * @param info - WifiP2pLinkedInfo object
     */
    virtual void OnP2pConnectionChanged(const WifiP2pLinkedInfo &info) = 0;

    /**
     * @Description Discover status change.
     *
     * @param isChange - true : change, false : not change
     */
    virtual void OnP2pDiscoveryChanged(bool isChange) = 0;

    /**
     * @Description P2p callback result.
     *
     * @param action - DiscoverDevices/StopDiscoverDevices/DiscoverServices/StopDiscoverServices
     *                 /PutLocalP2pService/StartP2pListen/StopP2pListen/CreateGroup/RemoveGroup
     *                 /DeleteGroup/P2pConnect/P2pCancelConnect
     * @param code   - Return code
     */
    virtual void OnP2pActionResult(P2pActionCallback action, ErrCode code) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.wifi.IWifiP2pCallback");
};
}  // namespace Wifi
}  // namespace OHOS
#endif