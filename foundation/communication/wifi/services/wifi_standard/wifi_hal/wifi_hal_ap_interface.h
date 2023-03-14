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

#ifndef OHOS_WIFI_HAL_AP_INTERFACE_H
#define OHOS_WIFI_HAL_AP_INTERFACE_H

#include "wifi_hal_define.h"
#include "wifi_hal_struct.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Start Ap.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StartSoftAp(void);

/**
 * @Description Start Hostapd.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StartHostapd(void);

/**
 * @Description Init hostapd hal module.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StartHostapdHal(void);

/**
 * @Description Stop Ap.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopSoftAp(void);

/**
 * @Description Stop hostapd.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopHostapd(void);

/**
 * @Description Release hostapd hal.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopHostapdHal(void);

/**
 * @Description Obtains information about all connected STAs.
 *
 * @param infos - Connected STA information array.
 * @param size - Obtains the size of all sta information arrays and Size of the
 *               obtained sta information array.
 * @return WifiErrorNo
 */
WifiErrorNo GetStaInfos(char *infos, int32_t *size);

/**
 * @Description Setting the AP Country Code.
 *
 * @param code - Country code.
 * @return WifiErrorNo
 */
WifiErrorNo SetCountryCode(const char *code);

/**
 * @Description Setting the startup configuration items of the hostapd.
 *
 * @param config - Hostapd startup configuration.
 * @return WifiErrorNo
 */
WifiErrorNo SetHostapdConfig(HostapdConfig *config);

/**
 * @Description To set the blocklist filtering in AP mode to prohibit
 *              the MAC address connection.
 *
 * @param mac - Blocklisted MAC address.
 * @param lenMac - Blocklist MAC address length.
 * @return WifiErrorNo
 */
WifiErrorNo SetMacFilter(const unsigned char *mac, int lenMac);

/**
 * @Description To set blocklist filtering in AP mode and delete a specified MAC
 *              address from the blocklist.
 *
 * @param mac - Blocklisted MAC address.
 * @param lenMac - Blocklist MAC address length.
 * @return WifiErrorNo
 */
WifiErrorNo DelMacFilter(const unsigned char *mac, int lenMac);

/**
 * @Description Disconnect the STA with a specified MAC address.
 *
 * @param mac - Blocklisted MAC address.
 * @param lenMac - Blocklist MAC address length.
 * @return WifiErrorNo
 */
WifiErrorNo DisassociateSta(const unsigned char *mac, int lenMac);

/**
 * @Description Obtains the hotspot frequency supported by a specified
 *              frequency band.
 *
 * @param band - Specified frequency band.
 * @param frequencies - Frequency array.
 * @param size - Frequency array memory size and Returns the size of the frequency array.
 * @return WifiErrorNo
 */
WifiErrorNo GetValidFrequenciesForBand(int32_t band, int *frequencies, int32_t *size);

#ifdef __cplusplus
}
#endif
#endif