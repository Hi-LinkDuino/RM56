/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

/**
 * @addtogroup wifiservice
 * @{
 *
 * @brief Provides functions for the Wi-Fi station and hotspot modes.
 *
 * You can use this module to enable and disable the Wi-Fi station or hotspot mode, connect to and disconnect from a
 * station or hotspot, query the station or hotspot status, and listen for events. \n
 *
 * @since 7
 */

/**
 * @file wifi_hotspot.h
 *
 * @brief Provides capabilities to enable and disable the hotspot mode, connect to and disconnect from a hotspot, query
 * the hotspot status, and listen for events.
 *
 * @since 7
 */

#ifndef HARMONY_OS_LITE_WIFI_HOTSPOT_H
#define HARMONY_OS_LITE_WIFI_HOTSPOT_H
#include "wifi_device.h"
#include "wifi_error_code.h"
#include "wifi_hotspot_config.h"
#include "wifi_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the default channel of the hotspot mode.
 */
#define HOTSPOT_DEFAULT_CHANNEL 6

/**
 * @brief Enables the hotspot mode.
 *
 * Before using this function, you need to invoke {@link SetHotspotConfig} and set at least the SSID, security type,
 * and key. \n
 *
 * @return Returns {@link WIFI_SUCCESS} if the hotspot mode is enabled; returns an error code defined in
 * {@link WifiErrorCode} otherwise.
 * @since 7
 */
WifiErrorCode EnableHotspot(void);

/**
 * @brief Disables the hotspot mode.
 *
 * @return Returns {@link WIFI_SUCCESS} if the hotspot mode is disabled; returns an error code defined in
 * {@link WifiErrorCode} otherwise.
 * @since 7
 */
WifiErrorCode DisableHotspot(void);

/**
 * @brief Sets a specified hotspot configuration.
 *
 * The hotspot configuration includes the SSID, security type, and key. The configuration set overwrites the existing
 * configuration and takes effect after the hotspot mode is re-enabled. \n
 * Before enabling the hotspot mode for the first time, you must call this function. \n
 *
 * @param config Indicates the hotspot configuration to set.
 * @return Returns {@link WIFI_SUCCESS} if the hotspot configuration is set; returns an error code defined in
 * {@link WifiErrorCode} otherwise.
 * @since 7
 */
WifiErrorCode SetHotspotConfig(const HotspotConfig *config);

/**
 * @brief Obtains a specified hotspot configuration.
 *
 * The hotspot configuration includes the SSID, security type, and key. \n
 *
 * @param result Indicates the obtained hotspot configuration.
 * @return Returns {@link WIFI_SUCCESS} if the hotspot configuration is obtained; returns an error code defined in
 * {@link WifiErrorCode} otherwise.
 * @since 7
 */
WifiErrorCode GetHotspotConfig(HotspotConfig *result);

/**
 * @brief Checks whether the hotspot mode is enabled.
 *
 * @return Returns {@link WIFI_HOTSPOT_ACTIVE} if the hotspot mode is enabled; returns {@link WIFI_HOTSPOT_NOT_ACTIVE}
 * otherwise.
 * @since 7
 */
int IsHotspotActive(void);

/**
 * @brief Obtains an array of stations connected to this hotspot.
 *
 * The station information is defined in {@link StationInfo}. \n
 *
 * @param result Indicates the array of stations connected to this hotspot. The array is requested and released by the
 * caller. The value must be greater than or equal to {@link WIFI_MAX_STA_NUM}.
 * @param size Indicates the size of the array.
 * @return Returns {@link WIFI_SUCCESS} if the array of stations connected to this hotspot is obtained; returns an error
 *  code defined in {@link WifiErrorCode} otherwise.
 * @since 7
 */
WifiErrorCode GetStationList(StationInfo *result, unsigned int *size);

/**
 * @brief Disconnects from the station with a specified MAC address.
 *
 * @param mac Indicates the pointer to the MAC address of the station.
 * @param macLen Indicates the length of the MAC address of the station.
 * @return Returns {@link WIFI_SUCCESS} if the function is successfully called;
 * returns an error code defined in {@link WifiErrorCode} otherwise.
 * @since 7
 */
WifiErrorCode DisassociateSta(unsigned char *mac, int macLen);

/**
 * @brief Adds the hotspot transmit power to the beacon.
 *
 * After the transmit power is added, the beacon must contain specified IEs. If the minimum transmit power
 * <b>0xFFFFFFFF</b> is added, the beacon does not contain the IEs. \n
 * The transmit power is added to the <b>ie</b> field only, exerting no impacts on the transmit power. \n
 * @param power Indicates the transmit power to add.
 * @return Returns {@link WIFI_SUCCESS} if the function is successfully called; returns an error code defined
 * in {@link WifiErrorCode} otherwise.
 * @since 7
 */
WifiErrorCode AddTxPowerInfo(int power);

#ifdef __cplusplus
}
#endif

#endif // HARMONY_OS_LITE_WIFI_HOTSPOT_H_
/** @} */
