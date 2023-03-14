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
 * @file wifi_device_config.h
 *
 * @brief Defines the Wi-Fi station configuration.
 *
 * The Wi-Fi station configuration includes the security type and data length. \n
 *
 * @since 7
 */

#ifndef HARMONY_OS_LITE_WIFI_DEVICE_CONFIG_H
#define HARMONY_OS_LITE_WIFI_DEVICE_CONFIG_H

/**
 * @brief Indicates the maximum number of Wi-Fi station configurations that can be added using {@link AddDeviceConfig}.
 *
 * If the maximum number is reached, an error will be returned. In this case, you must delete at least one
 * configuration before you can add new ones. \n
 */
#define WIFI_MAX_CONFIG_SIZE 10
/**
 * @brief Indicates the value of <b>networkId</b> when the configuration file is unavailable.
 *
 * Generally, the configuration file is unavailable because the configuration matching the <b>networkId</b> is
 * uninitialized. \n
 */
#define WIFI_CONFIG_INVALID (-1)
/**
 * @brief Indicates the maximum length of a Wi-Fi SSID.
 *
 * The maximum length is 32, and the last bit is reserved and set to <b>\0</b>. \n
 */
#define WIFI_MAX_SSID_LEN 33 // 32 + \0
/**
 * @brief Indicates the maximum length of a Wi-Fi key.
 *
 * The maximum length is 64, and the last bit is reserved and set to <b>\0</b>. \n
 */
#define WIFI_MAX_KEY_LEN 65 // 64 + \0
/**
 * @brief Indicates the maximum length of a Wi-Fi MAC address or a Wi-Fi BSSID.
 *
 */
#define WIFI_MAC_LEN 6

/**
 * @brief Indicates the maximum length of a Wi-Fi PSK.
 *
 */
#define WIFI_PSK_LEN 32

/**
 * @brief Indicates the maximum number of DNS servers.
 *
 * A maximum of two DNS servers are allowed. \n
 */
#define WIFI_MAX_DNS_NUM 2

/**
 * @brief Indicates the maximum length of a device name.
 *
 */
#define DEVICE_NAME_LEN 128

/**
 * @brief Enumerates Wi-Fi security types.
 *
 * @since 7
 */
typedef enum {
    /** Invalid security type */
    WIFI_SEC_TYPE_INVALID = -1,
    /** Open */
    WIFI_SEC_TYPE_OPEN,
    /** Wired Equivalent Privacy (WEP) */
    WIFI_SEC_TYPE_WEP,
    /** Pre-shared key (PSK) */
    WIFI_SEC_TYPE_PSK,
    /** Simultaneous Authentication of Equals (SAE) */
    WIFI_SEC_TYPE_SAE,
} WifiSecurityType;

/**
 * @brief Enumerates psk encryption types.
 *
 * @since 7
 */
typedef enum {
    /** Indicates that the ascii type of psk encryption type */
    WIFI_PSK_TYPE_ASCII = 0,
    /** Indicates that the hex type of psk encryption type */
    WIFI_PSK_TYPE_HEX,
} WifiPskType;

/**
 * @brief Defines the IP configuration of the Wi-Fi device.
 *
 * The IP configuration is mainly used for connecting to the device. \n
 *
 * @since 3
 */
typedef struct {
    /** IP address of the Wi-Fi device */
    unsigned int ipAddress;
    /** Gateway of the Wi-Fi device */
    unsigned int gateway;
    /** DNS server addresses for the Wi-Fi device */
    unsigned int dnsServers[WIFI_MAX_DNS_NUM];
    /** Subnet mask of the Wi-Fi device */
    unsigned int netmask;
} IpConfig;

/**
 * @brief Enumerates IP address types for the Wi-Fi device.
 *
 * @since 3
 */
typedef enum {
    /** Static IP address */
    STATIC_IP,
    /** IP address dynamically assigned by DHCP */
    DHCP,
    /** Unknown IP address type */
    UNKNOWN
} IpType;

/**
 * @brief Represents the Wi-Fi station configuration used to connect to a specified Wi-Fi device.
 *
 * @since 7
 */
typedef struct WifiDeviceConfig {
    /** Service set ID (SSID). For its length, see {@link WIFI_MAX_SSID_LEN}. */
    char ssid[WIFI_MAX_SSID_LEN];
    /** Basic service set ID (BSSID). For its length, see {@link WIFI_MAC_LEN}. */
    unsigned char bssid[WIFI_MAC_LEN];
    /** Key. For its length, see {@link WIFI_MAX_KEY_LEN}. */
    char preSharedKey[WIFI_MAX_KEY_LEN];
    /** Security type. It is defined in {@link WifiSecurityType}. */
    int securityType;
    /** Allocated <b>networkId</b> */
    int netId;
    /** Frequency */
    unsigned int freq;
    /** PSK type, see {@link WifiPskType}. */
    int wapiPskType;
    /** IP address type */
    IpType ipType;
    /** Static IP address */
    IpConfig staticIp;
    /* 1 for hidden config */
    int isHiddenSsid;
} WifiDeviceConfig;

/**
 * @brief Enumerates Wi-Fi scan types.
 *
 * @since 7
 */
typedef enum {
    /** A scan based on a specified frequency. */
    WIFI_FREQ_SCAN,
    /** A scan based on a specified SSID. */
    WIFI_SSID_SCAN,
    /** A scan based on a specified BSSID. */
    WIFI_BSSID_SCAN,
    /** A scan based on a specified frequency band. */
    WIFI_BAND_SCAN
} WifiScanType;

/**
 * @brief Represents the Wi-Fi station configuration used to connect to a specified Wi-Fi device.
 *
 * @since 7
 */
typedef struct {
    /** Service set ID (SSID). Its maximum length is defined by {@link WIFI_MAX_SSID_LEN}. */
    char ssid[WIFI_MAX_SSID_LEN];
    /** Length of the SSID. */
    char ssidLen;
    /** Basic service set ID (BSSID). Its length is defined by {@link WIFI_MAC_LEN}. */
    char bssid[WIFI_MAC_LEN];
    /** Frequency. */
    int freqs;
    /** Frequency band. */
    int band;
    /** Wi-Fi scan type, which is defined by {@link WifiScanType}. */
    WifiScanType scanType;
} WifiScanParams;

/**
 * @brief IP info
 *
 * @since 7
 */
typedef struct {
    unsigned int ipAddress;
    unsigned int netMask;
    unsigned int netGate;
    unsigned int dns1;
    unsigned int dns2;
    unsigned int serverAddress;
    int leaseDuration;
} IpInfo;
#endif // HARMONY_OS_LITE_WIFI_DEVICE_CONFIG_H
/** @} */
