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

#ifndef OHOS_WIFI_SCAN_STUB_H
#define OHOS_WIFI_SCAN_STUB_H

#include <map>
#include "iremote_stub.h"
#include "i_wifi_scan.h"

namespace OHOS {
namespace Wifi {
class WifiScanStub : public IRemoteStub<IWifiScan> {
public:
    WifiScanStub();
    virtual ~WifiScanStub() override;

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    bool IsSingleCallback() const;
    void SetSingleCallback(const bool isSingleCallback);
protected:
    sptr<IWifiScanCallback> GetCallback() const;

private:
    int OnSetScanControlInfo(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int OnScan(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int OnScanByParams(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int OnIsWifiClosedScan(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int OnGetScanInfoList(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int OnRegisterCallBack(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int OnGetSupportedFeatures(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    sptr<IWifiScanCallback> callback_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;

    bool mSingleCallback;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
