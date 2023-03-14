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

#ifndef OHOS_WIFI_HOTSPOT_CALLBACK_PROXY_H
#define OHOS_WIFI_HOTSPOT_CALLBACK_PROXY_H

#include "i_wifi_hotspot_callback.h"
#include "iremote_proxy.h"
#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
class WifiHotspotCallbackProxy : public IRemoteProxy<IWifiHotspotCallback> {
public:
    explicit WifiHotspotCallbackProxy(const sptr<IRemoteObject> &remote);
    ~WifiHotspotCallbackProxy() = default;

    /**
     * @Description Deal Hotspot state change message
     *
     * @param state - Hotspot state
     */
    void OnHotspotStateChanged(int state) override;

    /**
     * @Description Deal Hotspot accept a connection message
     *
     * @param info - The connected station info
     */
    void OnHotspotStaJoin(const StationInfo &info) override;

    /**
     * @Description Deal Hotspot lost a connection message
     *
     * @param info - The disconnect station info
     */
    void OnHotspotStaLeave(const StationInfo &info) override;

private:
    static inline BrokerDelegator<WifiHotspotCallbackProxy> g_delegator;
};
}  // namespace Wifi
}  // namespace OHOS
#endif