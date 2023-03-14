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

#ifndef OHOS_IDL_IWIFI_CHIP_H
#define OHOS_IDL_IWIFI_CHIP_H

#include "wifi_error_no.h"
#include "i_wifi_chip_event_callback.h"
#include "i_wifi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVENTS_IFACE_ADD_DEL_NUM 2
#define EVENTS_NUM_4 4

/**
 * @Description Sets the chip event callback function.
 *
 * @param callback - Callback function pointer structure.
 */
void SetWifiChipEventCallback(IWifiChipEventCallback callback);

/**
 * @Description Obtains the callback structure of the chip event.
 *
 * @return IWifiChipEventCallback*
 */
IWifiChipEventCallback *GetWifiChipEventCallback(void);

/**
 * @Description Obtains the chip ID.
 *
 * @param id
 * @return WifiErrorNo
 */
WifiErrorNo GetChipId(int32_t *id);

/**
 * @Description Creating a Device Interface in STA/AP/P2P/NAN Mode.
 *
 * @param type
 * @param iface
 * @return WifiErrorNo
 */
WifiErrorNo CreateIface(int32_t type, IWifiIface *iface);

/**
 * @Description Obtains the network device interface object based on
 *              the device interface name.
 *
 * @param ifname
 * @param iface
 * @return WifiErrorNo
 */
WifiErrorNo GetIface(const char *ifname, IWifiIface *iface);

/**
 * @Description Obtains the interface type. Obtains the device interface name.
 *
 * @param type
 * @param ifaces
 * @param size - ifaces array size.
 * @return WifiErrorNo
 */
WifiErrorNo GetIfaceNames(int32_t type, char *ifaces, int32_t size);

/**
 * @Description Delete a network device interface object by device interface name.
 *
 * @param ifname
 * @return WifiErrorNo
 */
WifiErrorNo RemoveIface(const char *ifname);

/**
 * @Description Obtains chip capabilities.
 *
 * @param capabilities
 * @return WifiErrorNo
 */
WifiErrorNo GetCapabilities(uint32_t *capabilities);

/**
 * @Description Obtains the joint mode supported by the chip, for example,
 *              sta+sta/sta+p2p/sta+ap/sta+nan/ap+nan.
 *
 * @param modes
 * @param size
 * @return WifiErrorNo
 */
WifiErrorNo GetSupportedComboModes(int32_t *modes, int32_t *size);

/**
 * @Description Configure the current joint mode of the chip.
 *
 * @param mode
 * @return WifiErrorNo
 */
WifiErrorNo ConfigComboModes(int32_t mode);

/**
 * @Description Gets the current federation mode.
 *
 * @param id
 * @return WifiErrorNo
 */
WifiErrorNo GetComboModes(int32_t *id);

/**
 * @Description Registering a Wi-Fi Chip Event.
 *
 * @param callback - Callback function pointer structure.
 * @return WifiErrorNo
 */
WifiErrorNo RegisterEventCallback(IWifiChipEventCallback callback);

/**
 * @Description Requesting the debugging information of the firmware chip.
 *
 * @param bytes - Debug info size.
 * @param size - Size of the memory to which bytes points.
 * @return WifiErrorNo
 */
WifiErrorNo RequestFirmwareDebugDump(unsigned char *bytes, int32_t *size);

/**
 * @Description Setting the Low Latency Mode.
 *
 * @param mode
 * @return WifiErrorNo
 */
WifiErrorNo SetPowerMode(uint8_t mode);

#ifdef __cplusplus
}
#endif
#endif