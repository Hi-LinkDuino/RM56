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
 * @addtogroup WLAN
 * @{
 *
 * @brief Provides cross-OS migration, component adaptation, and modular assembly and compilation.
 *
 * Based on the unified APIs provided by the WLAN module, developers of the Hardware Driver Interface
 * (HDI) are capable of creating, disabling, scanning for, and connecting to WLAN hotspots, managing WLAN chips,
 * network devices, and power, and applying for, releasing, and moving network data buffers.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file wifi_hal_base_feature.h
 *
 * @brief Declares basic WLAN features.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef WIFI_HAL_BASE_FEATURE_H
#define WIFI_HAL_BASE_FEATURE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Indicates the maximum length of the network interface name.
 *
 * @since 1.0
 * @version 1.0
 */
#define IFNAME_MAX_LEN 16
/**
 * @brief Indicates the length of the MAC address.
 *
 * @since 1.0
 * @version 1.0
 */
#define WIFI_MAC_ADDR_LENGTH 6
/**
 * @brief Defines the access failure error.
 *
 * @since 1.0
 * @version 1.0
 */
#define ERR_UNAUTH_ACCESS (-6)

/**
 * @brief Enumerates feature types.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    PROTOCOL_80211_IFTYPE_UNSPECIFIED,                         /**< Unspecified type */
    PROTOCOL_80211_IFTYPE_ADHOC,                               /**< Ad hoc network */
    PROTOCOL_80211_IFTYPE_STATION,                             /**< Station */
    PROTOCOL_80211_IFTYPE_AP,                                  /**< Access point (AP) */
    PROTOCOL_80211_IFTYPE_AP_VLAN,                             /**< Virtual AP */
    PROTOCOL_80211_IFTYPE_WDS,                                 /**< Wireless distributed system */
    PROTOCOL_80211_IFTYPE_MONITOR,                             /**< Listening */
    PROTOCOL_80211_IFTYPE_MESH_POINT,                          /**< Mesh network */
    PROTOCOL_80211_IFTYPE_P2P_CLIENT,                          /**< P2P client */
    PROTOCOL_80211_IFTYPE_P2P_GO,                              /**< P2P group owner */
    PROTOCOL_80211_IFTYPE_P2P_DEVICE,                          /**< P2P device */
    PROTOCOL_80211_IFTYPE_NUM,                                 /**< Number of network ports */
} FeatureType;

/**
 * @brief Defines basic WLAN features, such as obtaining the network interface name, setting the MAC address,
 * and setting the transmit power.
 *
 * @since 1.0
 * @version 1.0
 */
struct IWiFiBaseFeature {
    char ifName[IFNAME_MAX_LEN];  /**< Network interface name */
    int32_t type;                 /**< Feature type, as enumerated in {@link FeatureType} */

    /**
     * @brief Obtains the name of a network interface.
     *
     * @param baseFeature Indicates the pointer to the basic feature, as described in {@link IWiFiBaseFeature}.
     *
     * @return Returns <b>0</b> if the network interface name is obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    const char *(*getNetworkIfaceName)(const struct IWiFiBaseFeature *baseFeature);

    /**
     * @brief Obtains the type of a basic feature.
     *
     * @param baseFeature Indicates the pointer to the basic feature, as described in {@link IWiFiBaseFeature}.
     *
     * @return Returns <b>0</b> if the feature type is obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getFeatureType)(const struct IWiFiBaseFeature *baseFeature);

    /**
     * @brief Sets the MAC address.
     *
     * @param baseFeature Indicates the pointer to the basic feature, as described in {@link IWiFiBaseFeature}.
     * @param mac Indicates the pointer to the MAC address.
     * @param len Indicates the length of the MAC address.
     *
     * @return Returns <b>0</b> if the MAC address is set; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*setMacAddress)(const struct IWiFiBaseFeature *baseFeature, unsigned char *mac, uint8_t len);

    /**
     * @brief Obtains the device MAC address.
     *
     * @param baseFeature Indicates the pointer to the basic feature, as described in {@link IWiFiBaseFeature}.
     * @param mac Indicates the pointer to the MAC address.
     * @param len Indicates the length of the MAC address.
     *
     * @return Returns <b>0</b> if the MAC address is obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getDeviceMacAddress)(const struct IWiFiBaseFeature *baseFeature, unsigned char *mac, uint8_t len);

    /**
     * @brief Obtains the frequencies supported by the 2.4 GHz or 5 GHz band.
     *
     * @param baseFeature Indicates the pointer to the basic feature, as described in {@link IWiFiBaseFeature}.
     * @param band Indicates the frequency band, either 2.4 GHz or 5 GHz.
     * @param freqs Indicates the pointer to the supported frequencies.
     * @param count Indicates the number of elements in the frequency array.
     * @param num Indicates the number of supported frequencies.
     *
     * @return Returns <b>0</b> if the supported frequencies are obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getValidFreqsWithBand)(const struct IWiFiBaseFeature *baseFeature, int32_t band, int32_t *freqs,
        uint32_t count, uint32_t *num);

    /**
     * @brief Sets the transmit power.
     *
     * @param baseFeature Indicates the pointer to the basic feature, as described in {@link IWiFiBaseFeature}.
     * @param power Indicates the transmit power to set.
     *
     * @return Returns <b>0</b> if the transmit power is set; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*setTxPower)(const struct IWiFiBaseFeature *baseFeature, int32_t power);

    /**
     * @brief Obtains the chip ID of the current driver.
     *
     * @param baseFeature Indicates the pointer to the {@link IWiFiBaseFeature}.
     * @param chipId Indicates the pointer to the chip ID.
     *
     * @return Returns <b>0</b> if the chip ID is obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getChipId)(const struct IWiFiBaseFeature *baseFeature, uint8_t *chipId);

    /**
     * @brief Obtains names of all the NICs of the current chip based on the chip ID.
     *
     * @param chipId Indicates the chip ID.
     * @param ifNames Indicates the pointer to the NIC names.
     * @param num Indicates the pointer to the number of NICs.
     *
     * @return Returns <b>0</b> if the NIC names are obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getIfNamesByChipId)(const uint8_t chipId, char **ifNames, uint32_t *num);
};

/**
 * @brief Initializes a specified basic feature. This function is called during {@link FeatureType} creation.
 *
 * @param fe Indicates the double pointer to the basic feature.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value representing {@link HDF_STATUS}
 * if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t InitBaseFeature(struct IWiFiBaseFeature **fe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
/** @} */
