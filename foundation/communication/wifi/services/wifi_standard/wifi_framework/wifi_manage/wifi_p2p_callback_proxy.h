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
#ifndef OHOS_WIFI_P2P_CALLBACK_PROXY_H
#define OHOS_WIFI_P2P_CALLBACK_PROXY_H

#include "i_wifi_p2p_callback.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Wifi {
class WifiP2pCallbackProxy : public IRemoteProxy<IWifiP2pCallback> {
public:
    explicit WifiP2pCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~WifiP2pCallbackProxy();

    /**
     * @Description Deal p2p state change message
     *
     * @param state - P2P_STATE_IDLE/P2P_STATE_STARTED/P2P_STATE_CLOSED
     */
    void OnP2pStateChanged(int state) override;

    /**
     * @Description Persistent Group Changed Resources
     *
     */
    void OnP2pPersistentGroupsChanged() override;

    /**
     * @Description The device is changed.
     *
     * @param device - WifiP2pDevice object
     */
    void OnP2pThisDeviceChanged(const WifiP2pDevice &device) override;

    /**
     * @Description If the discover P2P device information is updated, all the
     *        latest WifiP2P devices are reported.
     *
     * @param device - std::vector<WifiP2pDevice> object
     */
    void OnP2pPeersChanged(const std::vector<WifiP2pDevice> &device) override;

    /**
     * @Description This event is triggered when the discovered services are updated.
     *
     * @param srvInfo - std::vector<WifiP2pServiceInfo> object
     */
    void OnP2pServicesChanged(const std::vector<WifiP2pServiceInfo> &srvInfo) override;

    /**
     * @Description Connection status change
     *
     * @param info - WifiP2pLinkedInfo object
     */
    void OnP2pConnectionChanged(const WifiP2pLinkedInfo &info) override;

    /**
     * @Description Discover status change
     *
     * @param isChange - true : change, false : not change
     */
    void OnP2pDiscoveryChanged(bool isChange) override;

    /**
     * @Description P2p callback result
     *
     * @param action - DiscoverDevices/StopDiscoverDevices/DiscoverServices/StopDiscoverServices
     *                 /PutLocalP2pService/StartP2pListen/StopP2pListen/CreateGroup/RemoveGroup
     *                 /DeleteGroup/P2pConnect/P2pCancelConnect
     * @param code   - Return code
     */
    void OnP2pActionResult(P2pActionCallback action, ErrCode code) override;

private:
    void WriteWifiP2pDeviceData(MessageParcel &data, const WifiP2pDevice &device);

private:
    static inline BrokerDelegator<WifiP2pCallbackProxy> delegator;
};
}  // namespace Wifi
}  // namespace OHOS
#endif