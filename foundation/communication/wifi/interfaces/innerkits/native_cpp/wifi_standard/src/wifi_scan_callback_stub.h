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
#ifndef OHOS_I_WIFI_SCAN_CALLBACK_STUB_H
#define OHOS_I_WIFI_SCAN_CALLBACK_STUB_H

#include "iremote_stub.h"
#include "i_wifi_scan_callback.h"

namespace OHOS {
namespace Wifi {
class WifiScanCallbackStub : public IRemoteStub<IWifiScanCallback> {
public:
    WifiScanCallbackStub();
    virtual ~WifiScanCallbackStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    void OnWifiScanStateChanged(int state) override;
    void RegisterCallBack(const sptr<IWifiScanCallback> &userCallback);
    bool IsRemoteDied() const;
    void SetRemoteDied(bool val);

private:
    int RemoteOnWifiScanStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply);
    sptr<IWifiScanCallback> userCallback_;

    bool mRemoteDied;
};
}  // namespace Wifi
}  // namespace OHOS
#endif