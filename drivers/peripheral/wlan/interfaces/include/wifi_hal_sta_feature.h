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
 * @file wifi_hal_sta_feature.h
 *
 * @brief Declares WLAN station (STA) features.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef WIFI_HAL_STA_FEATURE_H
#define WIFI_HAL_STA_FEATURE_H

#include "wifi_hal_base_feature.h"
#include "wifi_driver_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Inherits the basic features of {@link IWiFiBaseFeature} and additionally provides the feature of setting
 * scanning for a single MAC address.
 *
 * @since 1.0
 * @version 1.0
 */
struct IWiFiSta {
    struct IWiFiBaseFeature baseFeature;  /**< Basic features of {@link IWiFiBaseFeature} */

    /**
     * @brief Sets scanning for a single MAC address.
     *
     * @param staFeature Indicates the pointer to the STA feature, as described in {@link IWiFiSta}.
     * @param scanMac Indicates the pointer to the MAC address to scan for.
     * @param len Indicates the length of the MAC address.
     *
     * @return Returns <b>0</b> if scanning for the MAC address is successfully set; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*setScanningMacAddress)(const struct IWiFiSta *staFeature, unsigned char *scanMac, uint8_t len);

    /**
     * @brief wlan hal start scan.
     *
     * @param ifName The interface name.
     * @param scan start scan with param.
     *
     * @return Returns <b>0</b> if start scan is successfully; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*startScan)(const char *ifName, WifiScan *scan);
};

/**
 * @brief Initializes a specified STA feature. This function is called during STA {@link FeatureType} creation.
 *
 * @param fe Indicates the double pointer to the STA feature.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value representing {@link HDF_STATUS}
 * if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t InitStaFeature(struct IWiFiSta **fe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
/** @} */
