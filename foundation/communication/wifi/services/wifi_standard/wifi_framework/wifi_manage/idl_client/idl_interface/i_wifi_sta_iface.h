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

#ifndef OHOS_IDL_IWIFI_STA_IFACE_H
#define OHOS_IDL_IWIFI_STA_IFACE_H

#include "wifi_error_no.h"
#include "i_wifi_event_callback.h"
#include "i_wifi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Sets the Wi-Fi event callback function.
 *
 * @param callback
 */
void SetWifiEventCallback(IWifiEventCallback callback);

/**
 * @Description Obtains the Wi-Fi event callback structure.
 *
 * @return IWifiEventCallback*
 */
IWifiEventCallback *GetWifiEventCallback(void);

/**
 * @Description Obtaining the STA Support Capability.
 *
 * @param capabilities
 * @return WifiErrorNo
 */
WifiErrorNo GetStaCapabilities(int32_t *capabilities);

/**
 * @Description Obtaining the MAC Address of a STA.
 *
 * @param mac - Mac address.
 * @param lenMac - Size of the memory to which the MAC address points.
 * @return WifiErrorNo
 */
WifiErrorNo GetDeviceMacAddress(unsigned char *mac, int *lenMac);

/**
 * @Description Obtains the frequencies supported by a specified frequency band.
 *
 * @param band - Band type.
 * @param frequencies - Numeric group pointer of the int type.
 * @param size - Size of the memory to which the frequencies point and the
 *               number of obtained data.
 * @return WifiErrorNo
 */
WifiErrorNo GetFrequencies(int32_t band, int *frequencies, int32_t *size);

/**
 * @Description Sets the MAC address of the Wi-Fi connection.
 *
 * @param mac - Mac address.
 * @param lenMac - Mac string length.
 * @return WifiErrorNo
 */
WifiErrorNo SetAssocMacAddr(unsigned char *mac, int lenMac);

/**
 * @Description Sets the MAC address for Wi-Fi scanning.
 *
 * @param mac - Mac address.
 * @param lenMac - Mac string length.
 * @return WifiErrorNo
 */
WifiErrorNo SetScanningMacAddress(unsigned char *mac, int lenMac);

/**
 * @Description Disconnect the BSSID of the last roaming subscriber.
 *
 * @param mac - Mac address.
 * @param lenMac - Mac string length.
 * @return WifiErrorNo
 */
WifiErrorNo DeauthLastRoamingBssid(unsigned char *mac, int lenMac);

/**
 * @Description Get total supported feature.
 *
 * @param feature
 * @return WifiErrorNo
 */
WifiErrorNo GetSupportFeature(long *feature);

/**
 * @Description Send instructions to the Wi-Fi driver or chip.
 *
 * @param ifname
 * @param cmdId
 * @param buf
 * @param bufSize
 * @return WifiErrorNo
 */
WifiErrorNo RunCmd(const char *ifname, int32_t cmdId, unsigned char *buf, int32_t bufSize);

/**
 * @Description Set the Wi-Fi transmit power.
 *
 * @param power
 * @return WifiErrorNo
 */
WifiErrorNo SetWifiTxPower(int32_t power);

/**
 * @Description Deleting a Network.
 *
 * @param networkId
 * @return WifiErrorNo
 */
WifiErrorNo RemoveNetwork(int networkId);

/**
 * @Description Add a network. If the network is successfully added, the
 *              HAL returns the network ID.
 *
 * @param networkId
 * @return WifiErrorNo
 */
WifiErrorNo AddNetwork(int *networkId);

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
 * @Description Set the Network object
 *
 * @param networkId
 * @param confs
 * @param size
 * @return WifiErrorNo
 */
WifiErrorNo SetNetwork(int networkId, SetNetworkConfig *confs, int size);

/**
 * @Description WpaGetNetwork Info.
 *
 * @param confs
 * @return WifiErrorNo
 */
WifiErrorNo WpaGetNetwork(GetNetworkConfig *confs);

/**
 * @Description Save the network.
 *
 * @return WifiErrorNo
 */
WifiErrorNo SaveNetworkConfig(void);

/**
 * @Description Start Scanning.
 *
 * @param settings
 * @return WifiErrorNo
 */
WifiErrorNo StartScan(const ScanSettings *settings);

/**
 * @Description Obtain the scanning result, the caller needs to release the return
 *              pointer if it is not NULL
 *
 * @param results
 * @param size
 * @return ScanResult pointer
 */
ScanInfo* GetScanInfos(int *size);

/**
 * @Description Initiate PNO scanning.
 *
 * @param settings
 * @return WifiErrorNo
 */
WifiErrorNo StartPnoScan(const PnoScanSettings *settings);

/**
 * @Description Stop PNO Scanning.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopPnoScan(void);

/**
 * @Description Registering the Sta Event Callback.
 *
 * @param callback
 * @return WifiErrorNo
 */
WifiErrorNo RegisterStaEventCallback(IWifiEventCallback callback);

/**
 * @Description Enabling WPS in PBC Mode.
 *
 * @param param
 * @return WifiErrorNo
 */
WifiErrorNo StartWpsPbcMode(WifiWpsParam *param);

/**
 * @Description Enable PIN mode WPS.
 *
 * @param param
 * @param pinCode
 * @return WifiErrorNo
 */
WifiErrorNo StartWpsPinMode(WifiWpsParam *param, int *pinCode);

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
WifiErrorNo GetRoamingCapabilities(WifiRoamCapability *capability);

/**
 * @Description Setting Roaming Configurations.
 *
 * @param blocklist
 * @param blocksize
 * @param trustlist
 * @param trustsize
 * @return WifiErrorNo
 */
WifiErrorNo SetRoamConfig(char **blocklist, int blocksize, char **trustlist, int trustsize);

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
WifiErrorNo WpaBlocklistClear(void);

/**
 * @Description Obtaining the Network List.
 *
 * @param networkList
 * @param size
 * @return WifiErrorNo
 */
WifiErrorNo GetNetworkList(WifiNetworkInfo *infos, int *size);

/**
 * @Description Get current connect signal info, rssi, linkspeed, noise ...
 *
 * @param endBssid - peer end bssid, i.e. linked ap's bssid
 * @param info - signal info
 * @return WifiErrorNo
 */
WifiErrorNo GetConnectSignalInfo(const char *endBssid, WpaSignalInfo *info);

#ifdef __cplusplus
}
#endif
#endif