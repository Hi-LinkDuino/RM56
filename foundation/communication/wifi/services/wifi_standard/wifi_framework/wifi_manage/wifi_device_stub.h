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

#ifndef OHOS_WIFI_DEVICE_STUB_H
#define OHOS_WIFI_DEVICE_STUB_H

#include <map>
#include "iremote_stub.h"
#include "i_wifi_device.h"
#include "i_wifi_device_callback.h"

namespace OHOS {
namespace Wifi {
class WifiDeviceStub : public IRemoteStub<IWifiDevice> {
public:
    WifiDeviceStub();
    virtual ~WifiDeviceStub();

    using handleFunc = void (WifiDeviceStub::*)(uint32_t code, MessageParcel &data, MessageParcel &reply);
    using HandleFuncMap = std::map<int, handleFunc>;

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    void InitHandleMap();
    void OnEnableWifi(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnDisableWifi(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnInitWifiProtect(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetWifiProtectRef(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnPutWifiProtectRef(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnAddDeviceConfig(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnUpdateDeviceConfig(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnRemoveDevice(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnRemoveAllDevice(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetDeviceConfigs(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnEnableDeviceConfig(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnDisableDeviceConfig(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnConnectTo(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnConnect2To(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnReConnect(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnReAssociate(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnDisconnect(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnStartWps(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnCancelWps(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnIsWifiActive(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetWifiState(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetLinkedInfo(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetIpInfo(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnSetCountryCode(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetCountryCode(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnRegisterCallBack(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetSignalLevel(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetSupportedFeatures(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnGetDeviceMacAdd(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnIsWifiConnected(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnSetLowLatencyMode(uint32_t code, MessageParcel &data, MessageParcel &reply);

private:
    void ReadWifiDeviceConfig(MessageParcel &data, WifiDeviceConfig &config);
    void ReadIpAddress(MessageParcel &data, WifiIpAddress &address);
    void WriteWifiDeviceConfig(MessageParcel &reply, const WifiDeviceConfig &config);
    void WriteIpAddress(MessageParcel &reply, const WifiIpAddress &address);

private:
    HandleFuncMap handleFuncMap;
    sptr<IWifiDeviceCallBack> callback_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    bool mSingleCallback;
};
}  // namespace Wifi
}  // namespace OHOS
#endif