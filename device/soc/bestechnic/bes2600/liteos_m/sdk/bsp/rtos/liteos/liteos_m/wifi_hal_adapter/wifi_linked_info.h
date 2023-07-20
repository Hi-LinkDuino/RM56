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
 * @file wifi_linked_info.h
 *
 * @brief Defines the data structure and macro of the Wi-Fi connection information.
 *
 * @since 7
 */

#ifndef HARMONY_OS_LITE_WIFI_LINKED_INFO_H
#define HARMONY_OS_LITE_WIFI_LINKED_INFO_H
#include "wifi_device_config.h"

/**
 * @brief Enumerates Wi-Fi connection states.
 *
 * @since 7
 */
typedef enum {
    /** Disconnected */
    WIFI_DISCONNECTED,
    /** Connected */
    WIFI_CONNECTED
} WifiConnState;

/**
 * @brief Represents the Wi-Fi connection information.
 *
 * This refers to the information about the hotspot connected to this station. The information is obtained using
 * {@link GetLinkedInfo}.
 *
 * @since 7
 */
typedef struct {
    /** Service set ID (SSID). For its length, see {@link WIFI_MAX_SSID_LEN}. */
    char ssid[WIFI_MAX_SSID_LEN];
    /** Basic service set ID (BSSID). For its length, see {@link WIFI_MAC_LEN}. */
    unsigned char bssid[WIFI_MAC_LEN];
    /** Received signal strength indicator (RSSI) */
    int rssi;
    /** Wi-Fi band information of hotspot */
    int band;
    /** Wi-Fi frequency information of hotspot */
    int frequency;
    /** Wi-Fi connection state, which is defined in {@link WifiConnState} */
    WifiConnState connState;
    /** Reason for Wi-Fi disconnection */
    unsigned short disconnectedReason;
    /** IP address of the connected hotspot */
    unsigned int ipAddress;
} WifiLinkedInfo;
#endif // HARMONY_OS_LITE_WIFI_LINKED_INFO_H
/** @} */
