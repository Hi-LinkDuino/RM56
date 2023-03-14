/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_WIFI_P2P_STUB_H
#define OHOS_WIFI_P2P_STUB_H

#include <map>
#include "iremote_stub.h"
#include "message_parcel.h"
#include "message_option.h"
#include "i_wifi_p2p.h"

namespace OHOS {
namespace Wifi {
class WifiP2pStub : public IRemoteStub<IWifiP2p> {
public:
    using handleFunc = void (WifiP2pStub::*)(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    using HandleFuncMap = std::map<int, handleFunc>;

    WifiP2pStub();

    virtual ~WifiP2pStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    bool IsSingleCallback() const;
    void SetSingleCallback(const bool isSingleCallback);
protected:
    sptr<IWifiP2pCallback> GetCallback() const;

private:
    void InitHandleMap(void);
    void OnEnableP2p(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnDisableP2p(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnDiscoverDevices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnStopDiscoverDevices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnDiscoverServices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnStopDiscoverServices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnRequestService(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnPutLocalP2pService(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnDeleteLocalP2pService(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnStartP2pListen(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnStopP2pListen(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnCreateGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnRemoveGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnDeleteGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnP2pConnect(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnP2pCancelConnect(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnQueryP2pLinkedInfo(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetCurrentGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetP2pEnableStatus(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetP2pDiscoverStatus(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetP2pConnectedStatus(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnQueryP2pDevices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnQueryP2pGroups(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnQueryP2pServices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnRegisterCallBack(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetSupportedFeatures(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSetP2pDeviceName(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSetP2pWfdInfo(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dRequestGcIp(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dSharedlinkIncrease(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dSharedlinkDecrease(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dCreateGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dRemoveGcGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dConnect(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dConfigIPAddr(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dReleaseIPAddr(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dGetRecommendChannel(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dGetChannelListFor5G(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dGetSelfWifiCfgInfo(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHid2dSetPeerWifiCfgInfo(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    bool ReadWifiP2pServiceInfo(MessageParcel &data, WifiP2pServiceInfo &info);
    bool ReadWifiP2pServiceRequest(MessageParcel &data, WifiP2pDevice &device, WifiP2pServiceRequest &request);
    void WriteWifiP2pServiceInfo(MessageParcel &reply, const WifiP2pServiceInfo &info);
    void ReadWifiP2pDeviceData(MessageParcel &data, WifiP2pDevice &device);
    void WriteWifiP2pDeviceData(MessageParcel &reply, const WifiP2pDevice &device);
    bool ReadWifiP2pGroupData(MessageParcel &data, WifiP2pGroupInfo &info);
    void WriteWifiP2pGroupData(MessageParcel &reply, const WifiP2pGroupInfo &info);
    void ReadWifiP2pConfigData(MessageParcel &data, WifiP2pConfig &config);

private:
    HandleFuncMap handleFuncMap;
    sptr<IWifiP2pCallback> callback_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    bool mSingleCallback;
};
}  // namespace Wifi
}  // namespace OHOS
#endif