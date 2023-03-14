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
#ifndef OHOS_WIFI_P2P_CALLBACK_STUB_H
#define OHOS_WIFI_P2P_CALLBACK_STUB_H

#include <map>
#include "iremote_stub.h"
#include "i_wifi_p2p_callback.h"

namespace OHOS {
namespace Wifi {
class WifiP2pCallbackStub : public IRemoteStub<IWifiP2pCallback> {
public:
    WifiP2pCallbackStub();
    virtual ~WifiP2pCallbackStub();

    using handleFunc = void (WifiP2pCallbackStub::*)(uint32_t code, MessageParcel &data, MessageParcel &reply);
    using HandleFuncMap = std::map<int, handleFunc>;

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

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

    void RegisterCallBack(const sptr<IWifiP2pCallback> &userCallback);
    bool IsRemoteDied() const;
    void SetRemoteDied(bool val);

private:
    void InitHandleMap();
    void ReadWifiP2pDeviceData(MessageParcel &data, WifiP2pDevice &device);
    void RemoteOnP2pStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void RemoteOnP2pPersistentGroupsChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void RemoteOnP2pThisDeviceChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void RemoteOnP2pPeersChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void RemoteOnP2pServicesChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void RemoteOnP2pConnectionChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void RemoteOnP2pDiscoveryChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void RemoteOnP2pActionResult(uint32_t code, MessageParcel &data, MessageParcel &reply);

private:
    HandleFuncMap handleFuncMap;
    sptr<IWifiP2pCallback> userCallback_;
    bool mRemoteDied;
};
}  // namespace Wifi
}  // namespace OHOS
#endif