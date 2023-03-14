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

#ifndef OHOS_WIFI_HAL_CHIP_INTERFACE_H
#define OHOS_WIFI_HAL_CHIP_INTERFACE_H

#include "wifi_hal_define.h"
#include "wifi_hal_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_CODE_POS_OVERFLOW (-2)

/**
 * @Description Obtains the chip object by ID.
 *
 * @param id
 * @param chip
 * @return WifiErrorNo
 */
WifiErrorNo GetWifiChip(uint8_t id, WifiChip *chip);

/**
 * @Description Obtains the ID of a Wi-Fi chip.
 *
 * @param ids - Chip ID array.
 * @param size - Size of the input chip ID array.
 * @return WifiErrorNo
 */
WifiErrorNo GetWifiChipIds(uint8_t *ids, int32_t *size);

/**
 * @Description Obtains the chip ID.
 *
 * @param id - Indicates the chip ID.
 * @return WifiErrorNo
 */
WifiErrorNo GetChipId(int32_t *id);

/**
 * @Description Creating a Device Interface in STA/AP/P2P/NAN Mode.
 *
 * @param type
 * @param iface - Device interface object.
 * @return WifiErrorNo
 */
WifiErrorNo CreateIface(int32_t type, WifiIface *iface);

/**
 * @Description Obtains the network device interface object based.
 *
 * @param ifname - Device Name.
 * @param iface - Device interface object.
 * @return WifiErrorNo
 */
WifiErrorNo GetIface(const char *ifname, WifiIface *iface);

/**
 * @Description Obtains the interface type. Obtains the device.
 *
 * @param type - Device type.
 * @param ifaces - Device Interface Name.
 * @param size - ifaces size.
 * @return WifiErrorNo
 */
WifiErrorNo GetIfaceNames(int32_t type, char *ifaces, int32_t size);

/**
 * @Description Delete a network device interface object by device interface name.
 *
 * @param ifname - Device Name.
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
 * @Description Obtains the joint mode supported by the chip, as shown
 *              in (sta+sta/sta+p2p/sta+ap/sta+nan/ap+nan and so on).
 *
 * @param modes - Pattern array.
 * @param size - Incoming Pattern Array Size and Returns the actual size of the
 *               obtained pattern array.
 * @return WifiErrorNo
 */
WifiErrorNo GetSupportedComboModes(int32_t *modes, int32_t *size);

/**
 * @Description Configure the current joint mode of the chip.
 *
 * @param mode - Mode id.
 * @return WifiErrorNo
 */
WifiErrorNo ConfigComboModes(int32_t mode);

/**
 * @Description Gets the current federation mode.
 *
 * @param id - Current Mode.
 * @return WifiErrorNo
 */
WifiErrorNo GetComboModes(int32_t *id);

/**
 * @Description Requesting the debugging information of the firmware chip.
 *
 * @param bytes - Debug info.
 * @param size - Size of the input debug information array and Returns the actual
 *               size of debug information.
 * @return WifiErrorNo
 */
WifiErrorNo RequestFirmwareDebugDump(unsigned char *bytes, int32_t *size);

/**
 * @Description Setting the Low Latency Mode.
 *
 * @param mode - Low-latency mode.
 * @return WifiErrorNo
 */
WifiErrorNo SetPowerMode(uint8_t mode);

#ifdef __cplusplus
}
#endif
#endif