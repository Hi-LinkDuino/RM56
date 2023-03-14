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
#ifndef OHOS_WIFI_DEVICE_CALLBACK_STUB_H
#define OHOS_WIFI_DEVICE_CALLBACK_STUB_H

#include "iremote_stub.h"
#include "iremote_object.h"
#include "i_wifi_device_callback.h"


namespace OHOS {
namespace Wifi {
class WifiDeviceCallBackStub : public IRemoteStub<IWifiDeviceCallBack> {
public:
    WifiDeviceCallBackStub();
    virtual ~WifiDeviceCallBackStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    void OnWifiStateChanged(int state) override;
    void OnWifiConnectionChanged(int state, const WifiLinkedInfo &info) override;
    void OnWifiRssiChanged(int rssi) override;
    void OnWifiWpsStateChanged(int state, const std::string &pinCode) override;
    void OnStreamChanged(int direction) override;
    void RegisterUserCallBack(const sptr<IWifiDeviceCallBack> &callBack);
    bool IsRemoteDied() const;
    void SetRemoteDied(bool val);

private:
    int RemoteOnWifiStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    int RemoteOnWifiConnectionChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    int RemoteOnWifiRssiChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    int RemoteOnWifiWpsStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    int RemoteOnStreamChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);

    sptr<IWifiDeviceCallBack> callback_;

    bool mRemoteDied;
};
}  // namespace Wifi
}  // namespace OHOS
#endif