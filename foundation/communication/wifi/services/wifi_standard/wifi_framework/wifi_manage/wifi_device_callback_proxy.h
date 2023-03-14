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
#ifndef OHOS_WIFI_DEVICE_CALLBACK_PROXY_H
#define OHOS_WIFI_DEVICE_CALLBACK_PROXY_H

#include "i_wifi_device_callback.h"
#include "iremote_proxy.h"
#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
class WifiDeviceCallBackProxy : public IRemoteProxy<IWifiDeviceCallBack> {
public:
    explicit WifiDeviceCallBackProxy(const sptr<IRemoteObject> &remote);

    virtual ~WifiDeviceCallBackProxy() {}

    /**
     * @Description Deal wifi state change message
     *
     * @param state - Wifi State
     */
    void OnWifiStateChanged(int state) override;

    /**
     * @Description Deal wifi connection state change message
     *
     * @param state - Wifi connect state
     * @param info - WifiLinkedInfo object
     */
    void OnWifiConnectionChanged(int state, const WifiLinkedInfo &info) override;

    /**
     * @Description Deal wifi rssi change message
     *
     * @param rssi - rssi
     */
    void OnWifiRssiChanged(int rssi) override;

    /**
     * @Description Deal wps state change message
     *
     * @param state - wps state
     * @param pinCode - when wps pin mode, open wps successfully, return the pin code
     */
    void OnWifiWpsStateChanged(int state, const std::string &pinCode) override;

    /**
     * @Description Deal stream change message
     *
     * @param direction - stream direction
     */
    void OnStreamChanged(int direction) override;

private:
    static inline BrokerDelegator<WifiDeviceCallBackProxy> g_delegator;
};
}  // namespace Wifi
}  // namespace OHOS
#endif