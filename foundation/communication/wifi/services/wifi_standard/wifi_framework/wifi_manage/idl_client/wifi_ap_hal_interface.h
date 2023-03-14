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

#ifndef OHOS_WIFI_AP_HAL_INTERFACE_H
#define OHOS_WIFI_AP_HAL_INTERFACE_H

#include <string>
#include <vector>
#include "wifi_msg.h"
#include "i_wifi_hotspot_iface.h"
#include "wifi_base_hal_interface.h"

namespace OHOS {
namespace Wifi {
class WifiApHalInterface : public WifiBaseHalInterface {
public:
    /**
     * @Description Get the Instance object.
     *
     * @return WifiApHalInterface&
     */
    static WifiApHalInterface &GetInstance(void);

    /**
     * @Description Start Ap.
     *
     * @return WifiErrorNo
     */
    virtual WifiErrorNo StartAp(void);

    /**
     * @Description Close Ap.
     *
     * @return WifiErrorNo
     */
    virtual WifiErrorNo StopAp(void);

    /**
     * @Description Setting SoftAP Configurations.
     *
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo SetSoftApConfig(const HotspotConfig &config);

    /**
     * @Description Obtains information about all connected STAs.
     *
     * @param result - Returns the obtained STA information list.
     * @return WifiErrorNo
     */
    WifiErrorNo GetStationList(std::vector<std::string> &result);

    /**
     * @Description To set the blocklist filtering in AP mode to prohibit
     *              the MAC address connection.
     *
     * @param mac - Blocklisted address.
     * @return WifiErrorNo
     */
    WifiErrorNo AddBlockByMac(const std::string &mac);

    /**
     * @Description To set blocklist filtering in AP mode and delete a
     *              specified MAC address from the blocklist.
     *
     * @param mac - Blocklisted address.
     * @return WifiErrorNo
     */
    WifiErrorNo DelBlockByMac(const std::string &mac);

    /**
     * @Description Disconnect the STA with a specified MAC address.
     *
     * @param mac - Address information.
     * @return WifiErrorNo
     */
    WifiErrorNo RemoveStation(const std::string &mac);

    /**
     * @Description Obtains the hotspot frequency supported by a
     *              specified frequency band.
     *
     * @param band - Frequency band.
     * @param frequencies - Frequency list.
     * @return WifiErrorNo
     */
    WifiErrorNo GetFrequenciesByBand(int band, std::vector<int> &frequencies);

    /**
     * @Description Listening to Wi-Fi disconnection or connection events
     *              of the STA in AP mode.
     *
     * @param callback - Structure of the callback function for AP
     *                   registration events.
     * @return WifiErrorNo
     */
    WifiErrorNo RegisterApEvent(IWifiApMonitorEventCallback callback);

    /**
     * @Description Sets the Wi-Fi country code.
     *
     * @param code
     * @return WifiErrorNo
     */
    WifiErrorNo SetWifiCountryCode(const std::string &code);

    /**
     * @Description Disconnect STAs based on MAC addresses.
     *
     * @param mac
     * @return WifiErrorNo
     */
    WifiErrorNo DisconnectStaByMac(const std::string &mac);

    /**
     * @Description Get the Ap Callback Inst object
     *
     * @return const IWifiApMonitorEventCallback& - register ap callback objects
     */
    const IWifiApMonitorEventCallback &GetApCallbackInst(void) const;

private:
    IWifiApMonitorEventCallback mApCallback;
};
}  // namespace Wifi
}  // namespace OHOS

#endif