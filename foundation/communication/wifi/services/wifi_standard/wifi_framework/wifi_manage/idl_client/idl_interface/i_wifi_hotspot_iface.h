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

#ifndef OHOS_IDL_IWIFI_HOTSPOT_IFACE_H
#define OHOS_IDL_IWIFI_HOTSPOT_IFACE_H

#include <stdint.h>
#include "wifi_error_no.h"
#include "i_wifi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Sets the AP event callback function.
 *
 * @param callback
 */
void SetWifiApEventCallback(IWifiApEventCallback callback);

/**
 * @Description Obtains the AP event callback structure.
 *
 * @return IWifiApEventCallback*
 */
IWifiApEventCallback *GetWifiApEventCallback(void);

/**
 * @Description Start Ap.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StartSoftAp(void);

/**
 * @Description Close Ap.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopSoftAp(void);

/**
 * @Description Setting the startup configuration items of the hostapd.
 *
 * @param config - HostapdConfig object's point.
 * @return WifiErrorNo
 */
WifiErrorNo SetHostapdConfig(HostapdConfig *config);

/**
 * @Description Obtains information about all connected STAs.
 *
 * @param infos
 * @param size
 * @return WifiErrorNo
 */
WifiErrorNo GetStaInfos(char *infos, int32_t *size);

/**
 * @Description To set the blocklist filtering in AP mode to prohibit the MAC
 *              address connection.
 *
 * @param mac - Mac address.
 * @param lenMac - Mac string length.
 * @return WifiErrorNo
 */
WifiErrorNo SetMacFilter(unsigned char *mac, int lenMac);

/**
 * @Description This command is used to set blocklist filtering in AP mode and delete
 *              a specified MAC address from the blocklist.
 *
 * @param mac - Mac address.
 * @param lenMac - Mac string length.
 * @return WifiErrorNo
 */
WifiErrorNo DelMacFilter(unsigned char *mac, int lenMac);

/**
 * @Description Disconnect the STA with a specified MAC address.
 *
 * @param mac - Mac address.
 * @param lenMac - Mac string length.
 * @return WifiErrorNo
 */
WifiErrorNo DisassociateSta(unsigned char *mac, int lenMac);

/**
 * @Description Obtains the hotspot frequency supported by a specified frequency band.
 *
 * @param band - Band type.
 * @param frequencies - Numeric group pointer of the int type.
 * @param size - Size of the memory to which the frequencies point and the
 *               number of obtained data.
 * @return WifiErrorNo
 */
WifiErrorNo GetValidFrequenciesForBand(int32_t band, int *frequencies, int32_t *size);

/**
 * @Description Setting the Wi-Fi Country Code.
 *
 * @param code
 * @return WifiErrorNo
 */
WifiErrorNo SetCountryCode(const char *code);

/**
 * @Description Disconnect the STA connection based on the MAC address.
 *
 * @param mac - MAC address of the STA to be disconnected.
 * @return WifiErrorNo
 */
WifiErrorNo DisconnectStaByMac(const char *mac);

/**
 * @Description Information about the disconnected or connected STA.
 *
 * @param callback
 * @return WifiErrorNo
 */
WifiErrorNo RegisterAsscociatedEvent(IWifiApEventCallback callback);

#ifdef __cplusplus
}
#endif
#endif