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

#ifndef OHOS_WIFI_HAL_STA_INTERFACE_H
#define OHOS_WIFI_HAL_STA_INTERFACE_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include "wifi_hal_define.h"
#include "wifi_hal_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Start WiFi.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Start(void);

/**
 * @Description Stop WiFi.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Stop(void);

/**
 * @Description Stop Wifi, No reference counting check.
 *
 * @return WifiErrorNo
 */
WifiErrorNo ForceStop(void);

/**
 * @Description Start wpa_supplicant.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StartSupplicant(void);

/**
 * @Description Stop wpa_supplicant.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopSupplicant(void);

/**
 * @Description Connect wpa_supplicant.
 *
 * @return WifiErrorNo
 */
WifiErrorNo ConnectSupplicant(void);

/**
 * @Description Disconnect wpa_supplicant.
 *
 * @return WifiErrorNo
 */
WifiErrorNo DisconnectSupplicant(void);

/**
 * @Description Send a request to wpa_supplicant.
 *
 * @param buf - Request information array.
 * @param bufSize - Request message size.
 * @return WifiErrorNo
 */
WifiErrorNo RequestToSupplicant(const unsigned char *buf, int32_t bufSize);

/**
 * @Description Start Scanning.
 *
 * @param settings - Scanning parameters.
 * @return WifiErrorNo
 */
WifiErrorNo StartScan(const ScanSettings *settings);

/**
 * @Description Obtain the scanning result.
 *
 * @param results - Scan result array.
 * @param size - Size of the scan result array and returns the actual size of the
 *               scan result array.
 * @return WifiErrorNo
 */
WifiErrorNo GetScanInfos(ScanInfo *results, int *size);

/**
 * @Description Start PNO scanning.
 *
 * @param settings - PNO scanning parameters.
 * @return WifiErrorNo
 */
WifiErrorNo StartPnoScan(const PnoScanSettings *settings);

/**
 * @Description Stop PNO scanning.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopPnoScan(void);

/**
 * @Description Wi-Fi connect.
 *
 * @param networkId - Network ID.
 * @return WifiErrorNo
 */
WifiErrorNo Connect(int networkId);

/**
 * @Description Wi-Fi reconnect.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Reconnect(void);

/**
 * @Description Wi-Fi reassociation.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Reassociate(void);

/**
 * @Description Disconnecting a Wi-Fi Connection.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Disconnect(void);

/**
 * @Description Turn on/off power save mode for the interface.
 *
 * @param enable
 * @return WifiErrorNo
 */
WifiErrorNo SetPowerSave(int enable);

/**
 * @Description Obtaining the STA Support Capability.
 *
 * @param capabilities - STA capability.
 * @return WifiErrorNo
 */
WifiErrorNo GetStaCapabilities(int32_t *capabilities);

/**
 * @Description Obtaining the MAC Address of a STA.
 *
 * @param mac - Mac Message.
 * @param lenMac - Size of the array input when the MAC address is requested and get
 *                 mac message array size.
 * @return WifiErrorNo
 */
WifiErrorNo GetDeviceMacAddress(unsigned char *mac, int *lenMac);

/**
 * @Description Obtains the frequencies supported by a specified frequency band.
 *
 * @param band - Frequency band.
 * @param frequencies - Frequency array information.
 * @param size - Request FrequencyArray Size and get the size of the frequency array.
 * @return WifiErrorNo
 */
WifiErrorNo GetFrequencies(int32_t band, int *frequencies, int32_t *size);

/**
 * @Description Sets the MAC address of the Wi-Fi connection.
 *
 * @param mac - Mac Address.
 * @param lenMac - Mac Address array size.
 * @return WifiErrorNo
 */
WifiErrorNo SetAssocMacAddr(const unsigned char *mac, int lenMac);

/**
 * @Description Sets the MAC address for Wi-Fi scanning.
 *
 * @param mac - Mac Address.
 * @param lenMac - Mac Address array size.
 * @return WifiErrorNo
 */
WifiErrorNo SetScanningMacAddress(const unsigned char *mac, int lenMac);

/**
 * @Description Disconnect the BSSID of the last roaming subscriber.
 *
 * @param mac - Mac Address.
 * @param lenMac - Mac Address array size.
 * @return WifiErrorNo
 */
WifiErrorNo DeauthLastRoamingBssid(const unsigned char *mac, int lenMac);

/**
 * @Description Get total supported feature.
 *
 * @param feature - Attributes.
 * @return WifiErrorNo
 */
WifiErrorNo GetSupportFeature(long *feature);

/**
 * @Description Send instructions to the Wi-Fi driver or chip.
 *
 * @param ifname
 * @param cmdid - Commond ID.
 * @param buf
 * @param bufSize
 * @return WifiErrorNo
 */
WifiErrorNo RunCmd(const char *ifname, int32_t cmdid, const unsigned char *buf, int32_t bufSize);

/**
 * @Description Set the Wi-Fi transmit power.
 *
 * @param power - Transmit power.
 * @return WifiErrorNo
 */
WifiErrorNo SetWifiTxPower(int32_t power);

/**
 * @Description - Deleting a Network.
 *
 * @param networkId - Network id.
 * @return WifiErrorNo
 */
WifiErrorNo RemoveNetwork(int networkId);

/**
 * @Description Send Add_network to the WPA and return the network ID.
 *
 * @param networkId - Network id.
 * @return WifiErrorNo
 */
WifiErrorNo AddNetwork(int *networkId);

/**
 * @Description Enable a network.
 *
 * @param networkId - Network id.
 * @return WifiErrorNo
 */
WifiErrorNo EnableNetwork(int networkId);

/**
 * @Description Disable a network.
 *
 * @param networkId - Network id.
 * @return WifiErrorNo
 */
WifiErrorNo DisableNetwork(int networkId);

/**
 * @Description Setting the network.
 *
 * @param networkId - Network ID.
 * @param confs - Network configuration array to be set.
 * @param size - Size of the network configuration array to be set.
 * @return WifiErrorNo
 */
WifiErrorNo SetNetwork(int networkId, const SetNetworkConfig *confs, int size);

/**
 * @Description Save the network.
 *
 * @return WifiErrorNo
 */
WifiErrorNo SaveNetworkConfig(void);

/**
 * @Description Enabling WPS in PBC Mode.
 *
 * @param param
 * @return WifiErrorNo
 */
WifiErrorNo StartWpsPbcMode(const WifiWpsParam *param);

/**
 * @Description Enable PIN mode WPS.
 *
 * @param param
 * @param pinCode
 * @return WifiErrorNo
 */
WifiErrorNo StartWpsPinMode(const WifiWpsParam *param, int *pinCode);

/**
 * @Description Stop wps.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopWps(void);

/**
 * @Description Obtains the roaming support capability.
 *
 * @param capability - Obtains the roaming support capability.
 * @return WifiErrorNo
 */
WifiErrorNo GetRoamingCapabilities(WifiRoamCapability *capability);

/**
 * @Description Setting Roaming Configurations.
 *
 * @param blocklist - Ssid list.
 * @param blocksize - Block size.
 * @param trustlist - Ssid list.
 * @param trustsize - trust size.
 * @return WifiErrorNo
 */
WifiErrorNo SetRoamConfig(char **blocklist, int blocksize, char **trustlist, int trustsize);

/**
 * @Description Set country code.
 *
 * @param countryCode - Country code.
 * @return WifiErrorNo
 */
WifiErrorNo WpaSetCountryCode(const char *countryCode);

/**
 * @Description Get country code.
 *
 * @param countryCode - Country code.
 * @param codeSize - Size of the memory applied for by the address specified by countryCode.
 * @return WifiErrorNo
 */
WifiErrorNo WpaGetCountryCode(char *countryCode, int codeSize);

/**
 * @Description WpaGetNetWork Info.
 *
 * @param conf
 * @return WifiErrorNo
 */
WifiErrorNo WpaGetNetWork(GetNetworkConfig *conf);

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
 * @param WifiNetworkInfo - Array pointer of the network info structure type.
 * @param size - Number of infos that can be stored in the memory to which
 *               the network info pointer points.
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