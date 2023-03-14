/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
 * @addtogroup SoftBus
 * @{
 *
 * @brief Provides high-speed, secure communication between devices.
 *
 * This module implements unified distributed communication capability management between nearby devices, and provides
 * link-independent device discovery and transmission interfaces to support service publishing and data transmission.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file softbus_common.h
 *
 * @brief Declares common APIs for the Intelligent Soft Bus.
 *
 * This file provides common functions and constants for each submodule of the Intelligent Soft Bus, including: \n
 *
 * <ul>
 * <li>Constants such as the network ID length</li>
 * <li>Functions such as that for initializing the Intelligent Soft Bus client</li>
 * </ul>
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef SOFTBUS_CLIENT_COMMON_H
#define SOFTBUS_CLIENT_COMMON_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Indicates the length of the Bluetooth device MAC address in string format,
 * including the terminating null character <b>\0</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define BT_MAC_LEN 18

/**
 * @brief Indicates the length of the network ID string, including the terminating null character <b>\0</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define NETWORK_ID_BUF_LEN 65

/**
 * @brief Indicates the length of the UDID string, including the terminating null character <b>\0</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define UDID_BUF_LEN 65

/**
 * @brief Indicates the length of the UDID hash value.
 *
 * @since 1.0
 * @version 1.0
 */
#define UDID_HASH_LEN 32

/**
 * @brief Indicates the length of the UUID string, including the terminating null character <b>\0</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define UUID_BUF_LEN 65

/**
 * @brief Indicates the maximum length of an IP address in string format,
 * including the terminating null character <b>\0</b>. IPv6 addresses are supported.
 *
 * @since 1.0
 * @version 1.0
 */
#define IP_STR_MAX_LEN 46

/**
 * @brief Indicates the maximum length of the account hash code in <b>IDiscoveryCallback</b>.
 *
 */
#define MAX_ACCOUNT_HASH_LEN 96

/**
 * @brief Indicates the maximum length of the capability data in <b>PublishInfo</b> and <b>SubscribeInfo</b>.
 *
 */
#define MAX_CAPABILITYDATA_LEN 513

/**
 * @brief Indicates the maximum length of the custom data in <b>IDiscoveryCallback</b>.
 *
 */
#define DISC_MAX_CUST_DATA_LEN 219

/**
 * @brief Indicates the maximum number of capabilities contained in the bitmap in <b>IDiscoveryCallback</b>.
 *
 */
#define DISC_MAX_CAPABILITY_NUM 2

/**
 * @brief Indicates the maximum length of the device name in <b>IDiscoveryCallback</b>.
 *
 */
#define DISC_MAX_DEVICE_NAME_LEN 65

/**
 * @brief Indicates the maximum length of the device ID in <b>IDiscoveryCallback</b>.
 *
 */
#define DISC_MAX_DEVICE_ID_LEN 96

/**
 * @brief Enumerates {@link ConnectionAddr} types of a device that is added to a LNN.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    CONNECTION_ADDR_WLAN = 0, /**< WLAN */
    CONNECTION_ADDR_BR,       /**< BR */
    CONNECTION_ADDR_BLE,      /**< BLE */
    CONNECTION_ADDR_ETH,      /**< Ethernet */
    CONNECTION_ADDR_MAX       /**< Invalid type */
} ConnectionAddrType;
/**
 * @brief Defines the address of a device that is added to a LNN.
 * For details, see {@link ConnectionAddr}.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    /**< Address type. This field is used to explain the <b>info</b> field. */
    ConnectionAddrType type;
    /**< Connection address information */
    union {
        /**< BR address */
        struct BrAddr {
            char brMac[BT_MAC_LEN];   /**< BR MAC address in string format */
        } br;
        /**< BLE address */
        struct BleAddr {
            char bleMac[BT_MAC_LEN];  /**< BLE MAC address in string format */
            uint8_t udidHash[UDID_HASH_LEN];  /**< udid hash value */
        } ble;
        /**< IPv4 or IPv6 address */
        struct IpAddr {
            /**
             * IP address in string format. It can be an IPv4 address written in dotted decimal notation
             * or an IPv6 address written in hexadecimal colon-separated notation.
             */
            char ip[IP_STR_MAX_LEN];
            uint16_t port;            /**< Port number represented by the host byte order */
        } ip;
    } info;
    char peerUid[MAX_ACCOUNT_HASH_LEN];
} ConnectionAddr;

/**
 * @brief Enumerates the modes in which services are published.
 *
 */
typedef enum  {
    /* Passive */
    DISCOVER_MODE_PASSIVE = 0x55,
    /* Proactive */
    DISCOVER_MODE_ACTIVE  = 0xAA
} DiscoverMode;

/**
 * @brief Enumerates media, such as Bluetooth, Wi-Fi, and USB, used for publishing services.
 *
 * Currently, only <b>COAP</b> is supported.
 * When <b>AUTO</b> is selected, all the supported media will be called automatically.
 */
typedef enum {
    /** Automatic medium selection */
    AUTO = 0,
    /** Bluetooth */
    BLE = 1,
    /** Wi-Fi */
    COAP = 2,
    /** USB */
    USB = 3,
    /** HiLink */
    COAP1 = 4,
    MEDIUM_BUTT
} ExchanageMedium;

/**
 * @brief Enumerates frequencies for publishing services.
 *
 * This enumeration applies only to Bluetooth and is not supported currently.
 */
typedef enum {
    /** Low */
    LOW = 0,
    /** Medium */
    MID = 1,
    /** High */
    HIGH = 2,
    /** Super-high */
    SUPER_HIGH = 3,
    FREQ_BUTT
} ExchangeFreq;

/**
 * @brief Enumerates supported capabilities published by a device.
 *
 */
typedef enum {
    /** MeeTime */
    HICALL_CAPABILITY_BITMAP = 0,
    /** Video reverse connection in the smart domain */
    PROFILE_CAPABILITY_BITMAP = 1,
    /** Gallery in Vision */
    HOMEVISIONPIC_CAPABILITY_BITMAP = 2,
    /** cast+ */
    CASTPLUS_CAPABILITY_BITMAP,
    /** Input method in Vision */
    AA_CAPABILITY_BITMAP,
    /** Device virtualization tool package */
    DVKIT_CAPABILITY_BITMAP,
    /** Distributed middleware */
    DDMP_CAPABILITY_BITMAP,
    /** Osd capability */
    OSD_CAPABILITY_BITMAP
} DataBitMap;

/**
 * @brief Defines the mapping between supported capabilities and bitmaps.
 *
 */
typedef struct {
    /** Bitmaps. For details, see {@link DataBitMap}. */
    DataBitMap bitmap;
    /** Capability. For details, see {@link g_capabilityMap}. */
    char *capability;
} CapabilityMap;

/**
 * @brief Defines the mapping between supported capabilities and bitmaps.
 *
 */
static const CapabilityMap g_capabilityMap[] = {
    {HICALL_CAPABILITY_BITMAP, (char *)"hicall"},
    {PROFILE_CAPABILITY_BITMAP, (char *)"profile"},
    {HOMEVISIONPIC_CAPABILITY_BITMAP, (char *)"homevisionPic"},
    {CASTPLUS_CAPABILITY_BITMAP, (char *)"castPlus"},
    {AA_CAPABILITY_BITMAP, (char *)"aaCapability"},
    {DVKIT_CAPABILITY_BITMAP, (char *)"dvKit"},
    {DDMP_CAPABILITY_BITMAP, (char *)"ddmpCapability"},
    {OSD_CAPABILITY_BITMAP, (char *)"osdCapability"},
};

/**
 * @brief Defines service publishing information.
 *
 */
typedef struct {
    /** Service ID */
    int publishId;
    /** Discovery mode for service publishing. For details, see {@link Discovermode}. */
    DiscoverMode mode;
    /** Service publishing medium. For details, see {@link ExchanageMedium}. */
    ExchanageMedium medium;
    /** Service publishing frequency. For details, see {@link ExchangeFre}. */
    ExchangeFreq freq;
    /** Service publishing capabilities. For details, see {@link g_capabilityMap}. */
    const char *capability;
    /** Capability data for service publishing */
    unsigned char *capabilityData;
    /** Maximum length of the capability data for service publishing (512 bytes) */
    unsigned int dataLen;
} PublishInfo;

/**
 * @brief Defines service subscription information.
 *
 */
typedef struct {
    /** Service ID */
    int subscribeId;
    /** Discovery mode for service subscription. For details, see {@link Discovermode}. */
    DiscoverMode mode;
    /** Service subscription medium. For details, see {@link ExchanageMedium}. */
    ExchanageMedium medium;
    /** Service subscription frequency. For details, see {@link ExchangeFre}. */
    ExchangeFreq freq;
    /** only find the device with the same account */
    bool isSameAccount;
    /** find the sleeping devices */
    bool isWakeRemote;
    /** Service subscription capability. For details, see {@link g_capabilityMap}. */
    const char *capability;
    /** Capability data for service subscription */
    unsigned char *capabilityData;
    /** Maximum length of the capability data for service subscription (512 bytes) */
    unsigned int dataLen;
} SubscribeInfo;

/**
 * @brief Enumerates device types.
 *
 */
typedef enum {
    /* Smart speaker */
    SMART_SPEAKER = 0x00,
    /* PC */
    DESKTOP_PC,
    /* Laptop */
    LAPTOP,
    /* Mobile phone */
    SMART_PHONE,
    /* Tablet */
    SMART_PAD,
    /* Smart watch */
    SMART_WATCH,
    /* Smart car */
    SMART_CAR,
    /* Kids' watch */
    CHILDREN_WATCH,
    /* Smart TV */
    SMART_TV,
} DeviceType;

/**
 * @brief Defines the device information returned by <b>IDiscoveryCallback</b>.
 *
 */
typedef struct {
    /** Device ID. Its maximum length is specified by {@link DISC_MAX_DEVICE_ID_LEN}. */
    char devId[DISC_MAX_DEVICE_ID_LEN];
    /** Account hash code. Its maximum length is specified by {@link MAX_ACCOUNT_HASH_LEN}. */
    char accountHash[MAX_ACCOUNT_HASH_LEN];
    /** Device type. For details, see {@link DeviceType}. */
    DeviceType devType;
    /** Device name. Its maximum length is specified by {@link DISC_MAX_DEVICE_NAME_LEN}. */
    char devName[DISC_MAX_DEVICE_NAME_LEN];
    /** Number of available connections */
    unsigned int addrNum;
    /** Connection information. For details, see {@link ConnectAddr}. */
    ConnectionAddr addr[CONNECTION_ADDR_MAX];
    /** Number of capabilities */
    unsigned int capabilityBitmapNum;
    /** Device capability bitmap.
     * The maximum number of capabilities in the bitmap is specified by {@link DISC_MAX_CAPABILITY_NUM}.
     */
    unsigned int capabilityBitmap[DISC_MAX_CAPABILITY_NUM];
    /** Custom data. Its length is specified by {@link DISC_MAX_CUST_DATA_LEN}. */
    char custData[DISC_MAX_CUST_DATA_LEN];
} DeviceInfo;
#ifdef __cplusplus
}
#endif
#endif
/** @} */
