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

#ifndef OHOS_WIFI_HOTSPOT_CALLBACK_STUB_H
#define OHOS_WIFI_HOTSPOT_CALLBACK_STUB_H

#include "iremote_stub.h"
#include "i_wifi_hotspot_callback.h"

namespace OHOS {
namespace Wifi {
class WifiHotspotCallbackStub : public IRemoteStub<IWifiHotspotCallback> {
public:
    WifiHotspotCallbackStub();
    virtual ~WifiHotspotCallbackStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    void RegisterCallBack(const sptr<IWifiHotspotCallback> &userCallback);
    void OnHotspotStateChanged(int state) override;
    void OnHotspotStaJoin(const StationInfo &info) override;
    void OnHotspotStaLeave(const StationInfo &info) override;
    bool IsRemoteDied() const;
    void SetRemoteDied(bool val);

private:
    int RemoteOnHotspotStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    int RemoteOnHotspotStaJoin(uint32_t code, MessageParcel &data, MessageParcel &reply);
    int RemoteOnHotspotStaLeave(uint32_t code, MessageParcel &data, MessageParcel &reply);

private:
    sptr<IWifiHotspotCallback> userCallback_;

    bool mRemoteDied;
};
}  // namespace Wifi
}  // namespace OHOS
#endif