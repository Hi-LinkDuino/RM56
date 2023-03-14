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

#ifndef OHOS_I_WIFI_HOTSPOT_CALLBACK_H
#define OHOS_I_WIFI_HOTSPOT_CALLBACK_H

#include <string>
#include <string_ex.h>
#include <iremote_broker.h>
#include "message_parcel.h"
#include "message_option.h"
#include "wifi_ap_msg.h"
#include "define.h"

namespace OHOS {
namespace Wifi {
class IWifiHotspotCallback : public IRemoteBroker {
public:
    /**
     * @Description Deal Hotspot state change message
     *
     * @param state - Hotspot state
     */
    virtual void OnHotspotStateChanged(int state) = 0;

    /**
     * @Description Deal Hotspot accept a connection message
     *
     * @param info - The connected station info
     */
    virtual void OnHotspotStaJoin(const StationInfo &info) = 0;

    /**
     * @Description Deal Hotspot lost a connection message
     *
     * @param info - The disconnect station info
     */
    virtual void OnHotspotStaLeave(const StationInfo &info) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.wifi.IWifiHotspotCallback");
};
}  // namespace Wifi
}  // namespace OHOS
#endif