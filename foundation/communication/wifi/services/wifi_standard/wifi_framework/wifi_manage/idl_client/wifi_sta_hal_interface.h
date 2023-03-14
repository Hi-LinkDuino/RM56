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

#ifndef OHOS_WIFI_STA_HAL_INTERFACE_H
#define OHOS_WIFI_STA_HAL_INTERFACE_H

#include <string>
#include <vector>
#include "wifi_base_hal_interface.h"
#include "wifi_event_callback.h"
#include "wifi_idl_struct.h"
#include "wifi_scan_param.h"
#include "wifi_idl_define.h"

namespace OHOS {
namespace Wifi {
class WifiStaHalInterface : public WifiBaseHalInterface {
public:
    /**
     * @Description Get the Instance object.
     *
     * @return WifiStaHalInterface&
     */
    static WifiStaHalInterface &GetInstance(void);

    /**
     * @Description Open Wifi.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo StartWifi(void);

    /**
     * @Description Close Wifi.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo StopWifi(void);

    /**
     * @Description Connect Wifi.
     *
     * @param networkId
     * @return WifiErrorNo
     */
    WifiErrorNo Connect(int networkId);

    /**
     * @Description Reconnect Wifi.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo Reconnect(void);

    /**
     * @Description Reassociate Wifi.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo Reassociate(void);

    /**
     * @Description Disconnect Wifi.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo Disconnect(void);

    /**
     * @Description Obtaining the STA Support Capability.
     *
     * @param capabilities
     * @return WifiErrorNo
     */
    WifiErrorNo GetStaCapabilities(unsigned int &capabilities);

    /**
     * @Description Obtaining the MAC Address of a STA.
     *
     * @param mac
     * @return WifiErrorNo
     */
    WifiErrorNo GetStaDeviceMacAddress(std::string &mac);

    /**
     * @Description Obtains the frequencies supported by a specified frequency band.
     *
     * @param band
     * @param frequencies
     * @return WifiErrorNo
     */
    WifiErrorNo GetSupportFrequencies(int band, std::vector<int> &frequencies);

    /**
     * @Description Sets the MAC address of the Wi-Fi connection.
     *
     * @param mac
     * @return WifiErrorNo
     */
    WifiErrorNo SetConnectMacAddr(const std::string &mac);

    /**
     * @Description Sets the MAC address for Wi-Fi scanning.
     *
     * @param mac
     * @return WifiErrorNo
     */
    WifiErrorNo SetScanMacAddress(const std::string &mac);

    /**
     * @Description Disconnect the BSSID of the last roaming subscriber.
     *
     * @param mac
     * @return WifiErrorNo
     */
    WifiErrorNo DisconnectLastRoamingBssid(const std::string &mac);

    /**
     * @Description Get total supported feature, and call user can
     *              determine whether support a feature.
     *
     * @param feature
     * @return WifiErrorNo
     */
    WifiErrorNo GetSupportFeature(long &feature);

    /**
     * @Description Send instructions to the Wi-Fi driver or chip.
     *
     * @param request
     * @return WifiErrorNo
     */
    WifiErrorNo SendRequest(const WifiStaRequest &request);

    /**
     * @Description Set the Wi-Fi transmit power.
     *
     * @param power
     * @return WifiErrorNo
     */
    WifiErrorNo SetTxPower(int power);

    /**
     * @Description Scan by specified parameter.
     *
     * @param scanParam
     * @return WifiErrorNo
     */
    WifiErrorNo Scan(const WifiScanParam &scanParam);

    /**
     * @Description Obtain the scanning result.
     *
     * @param scanResults
     * @return WifiErrorNo
     */
    WifiErrorNo QueryScanInfos(std::vector<InterScanInfo> &scanInfos);

    /**
     * @Description Initiate PNO scanning.
     *
     * @param scanParam
     * @return WifiErrorNo
     */
    WifiErrorNo StartPnoScan(const WifiPnoScanParam &scanParam);

    /**
     * @Description Stop PNO Scanning.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo StopPnoScan(void);

    /**
     * @Description Deleting a Network.
     *
     * @param networkId
     * @return WifiErrorNo
     */
    WifiErrorNo RemoveDevice(int networkId);

    /**
     * @Description Clears the network configuration information saved by wpa_supplicant.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo ClearDeviceConfig(void) const;

    /**
     * @Description Request to obtain the next network ID.
     *
     * @param networkId
     * @return WifiErrorNo
     */
    WifiErrorNo GetNextNetworkId(int &networkId);

    /**
     * @Description Enable a network.
     *
     * @param networkId
     * @return WifiErrorNo
     */
    WifiErrorNo EnableNetwork(int networkId);

    /**
     * @Description Disable a network.
     *
     * @param networkId
     * @return WifiErrorNo
     */
    WifiErrorNo DisableNetwork(int networkId);

    /**
     * @Description Setting the network.
     *
     * @param networkId
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo SetDeviceConfig(int networkId, const WifiIdlDeviceConfig &config);

    /**
     * @Description Obtaining Network Configurations.
     *
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo GetDeviceConfig(WifiIdlGetDeviceConfig &config);

    /**
     * @Description Save network config.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo SaveDeviceConfig(void);

    /**
     * @Description Registering the Sta Event Callback.
     *
     * @param callback
     * @return WifiErrorNo
     */
    WifiErrorNo RegisterStaEventCallback(const WifiEventCallback &callback);

    /**
     * @Description Enabling WPS in PBC Mode.
     *
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo StartWpsPbcMode(const WifiIdlWpsConfig &config);

    /**
     * @Description Enable PIN mode WPS.
     *
     * @param config
     * @param pinCode
     * @return WifiErrorNo
     */
    WifiErrorNo StartWpsPinMode(const WifiIdlWpsConfig &config, int &pinCode);

    /**
     * @Description Close wps.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo StopWps(void);

    /**
     * @Description Obtains the roaming support capability.
     *
     * @param capability
     * @return WifiErrorNo
     */
    WifiErrorNo GetRoamingCapabilities(WifiIdlRoamCapability &capability);

    /**
     * @Description Setting Roaming Configurations.
     *
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo SetRoamConfig(const WifiIdlRoamConfig &config);

    /**
     * @Description Wpa_s disable/enable(0/1) automatic reconnection.
     *
     * @param enable
     * @return WifiErrorNo
     */
    WifiErrorNo WpaAutoConnect(int enable);

    /**
     * @Description Clearing the wpa Blocklist.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo WpaBlocklistClear();

    /**
     * @Description Obtaining the Network List.
     *
     * @param networkList
     * @return WifiErrorNo
     */
    WifiErrorNo GetNetworkList(std::vector<WifiWpaNetworkInfo> &networkList);

    /**
     * @Description Get current connect signal info, rssi, linkspeed, noise ...
     *
     * @param endBssid - peer end bssid, i.e. linked ap's bssid
     * @param info - signal info
     * @return WifiErrorNo
     */
    WifiErrorNo GetConnectSignalInfo(const std::string &endBssid, WifiWpaSignalInfo &info);

    /**
     * @Description Get register callback objects
     *
     * @return const WifiEventCallback& - register sta callback objects
     */
    const WifiEventCallback &GetCallbackInst(void) const;

private:
    WifiEventCallback mStaCallback;
};
}  // namespace Wifi
}  // namespace OHOS

#endif