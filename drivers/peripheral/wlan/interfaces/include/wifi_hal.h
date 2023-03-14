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
 * @file wifi_hal.h
 *
 * @brief Declares APIs for basic WLAN features.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef WIFI_HAL_H
#define WIFI_HAL_H

#include "wifi_hal_ap_feature.h"
#include "wifi_hal_sta_feature.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Defines a callback to listen for <b>IWiFi</b> asynchronous events.
 *
 * @param event Indicates the event type passed to the callback.
 * @param data Indicates the pointer to the data passed to the callback.
 * @param ifName The interface name.
 *
 * @return Returns <b>0</b> if the <b>IWiFi</b> callback is defined; returns a negative value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
typedef int32_t (*CallbackFunc)(uint32_t event, void *data, const char *ifName);

/**
 * @brief Defines the basic WLAN features provided by the hardware abstraction layer (HAL).
 *
 * The basic features include creating and stopping a channel between the HAL and the WLAN driver,
 * and creating, obtaining, and destroying WLAN features.
 *
 * @since 1.0
 * @version 1.0
 */
struct IWiFi {
    /**
     * @brief Creates a channel between the HAL and the WLAN driver and obtains the driver NIC information.
     *
     * @param iwifi Indicates the pointer to the {@link IWiFi} object.
     *
     * @return Returns <b>0</b> if the channel is created and the driver NIC information is obtained;
     * returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*start)(struct IWiFi *iwifi);

    /**
     * @brief Stops the channel between the HAL and the WLAN driver.
     *
     * @param iwifi Indicates the pointer to the {@link IWiFi} object.
     *
     * @return Returns <b>0</b> if the channel is stopped; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*stop)(struct IWiFi *iwifi);

    /**
     * @brief Obtains the WLAN features available for the device no matter whether it works as an AP,
     * STA, or P2P server/client.
     *
     * @param supType Indicates the pointer to the WLAN features available for the device.
     * @param size Indicates the length of the <b>supType</b> array.
     *
     * @return Returns <b>0</b> if the WLAN features are obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getSupportFeature)(uint8_t *supType, uint32_t size);

    /**
     * @brief Obtains the WLAN features available for the device that plays different roles simultaneously
     * (any combination of AP, STA, and P2P server/client).
     *
     * @param combo Indicates the pointer to WLAN features available for the device.
     * @param size Indicates the length of the <b>combo</b> array.
     *
     * @return Returns <b>0</b> if the WLAN features are obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getSupportCombo)(uint64_t *combo, uint32_t size);

    /**
     * @brief Creates an {@link IWiFiBaseFeature} object of a specified type.
     *
     * @param type Indicates the feature type.
     * @param ifeature Indicates the double pointer to the {@link IWiFiBaseFeature} object.
     *
     * @return Returns <b>0</b> if the {@link IWiFiBaseFeature} object is created; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*createFeature)(int32_t type, struct IWiFiBaseFeature **ifeature);

    /**
     * @brief Obtains an {@link IWiFiBaseFeature} object based on a specified network interface name.
     *
     * @param ifName Indicates the pointer to the network interface name.
     * @param ifeature Indicates the double pointer to the {@link IWiFiBaseFeature} object.
     *
     * @return Returns <b>0</b> if the {@link IWiFiBaseFeature} object is obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getFeatureByIfName)(const char *ifName, struct IWiFiBaseFeature **ifeature);

    /**
     * @brief Registers a callback to listen for <b>IWiFi</b> asynchronous events.
     *
     * @param cbFunc Indicates the callback to register.
     * @param ifName Indicates the pointer to the network interface name.
     *
     * @return Returns <b>0</b> if the callback is registered; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*registerEventCallback)(CallbackFunc cbFunc, const char *ifName);

    /**
     * @brief Deregisters an <b>IWiFi</b> callback.
    
     * @param cbFunc Indicates the callback to register.
     * @param ifName Indicates the pointer to the network interface name.
     *
     * @return Returns <b>0</b> if the <b>IWiFi</b> callback is deregistered; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*unregisterEventCallback)(CallbackFunc cbFunc, const char *ifName);

    /**
     * @brief Destroys a specified {@link IWiFiBaseFeature} object.
     *
     * @param ifeature Indicates the pointer to the {@link IWiFiBaseFeature} object to destroy.
     *
     * @return Returns <b>0</b> if the {@link IWiFiBaseFeature} object is destroyed; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*destroyFeature)(struct IWiFiBaseFeature *ifeature);

    /**
     * @brief Resets the WLAN driver with a specified chip ID.
     *
     * @param chipId Indicates the chip ID.
     *
     * @return Returns <b>0</b> if the WLAN driver is reset; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*resetDriver)(const uint8_t chipId, const char *ifName);

    /**
     * @brief get net device infos.
     *
     * @param netDeviceInfoResult get net device infos.
     *
     * @return Returns <b>0</b> if get infos successful; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getNetDevInfo)(struct NetDeviceInfoResult *netDeviceInfoResult);
};

/**
 * @brief Creates an {@link IWiFi} structure.
 *
 * @param wifiInstance Indicates the double pointer to the {@link IWiFi} structure.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t WifiConstruct(struct IWiFi **wifiInstance);

/**
 * @brief Destroys a specified {@link IWiFi} structure.
 *
 * @param wifiInstance Indicates the double pointer to the {@link IWiFi} structure.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t WifiDestruct(struct IWiFi **wifiInstance);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
/** @} */
