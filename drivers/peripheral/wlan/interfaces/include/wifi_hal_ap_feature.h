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
 * @file wifi_hal_ap_feature.h
 *
 * @brief Declares WLAN access point (AP) features.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef WIFI_HAL_AP_FEATURE_H
#define WIFI_HAL_AP_FEATURE_H

#include "wifi_hal_base_feature.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Describes an STA connected to an AP.
 *
 * @since 1.0
 * @version 1.0
 */
struct StaInfo {
    unsigned char mac[WIFI_MAC_ADDR_LENGTH];  /**< MAC address of the STA */
};

/**
 * @brief Inherits the basic features of {@link IWiFiBaseFeature} and additionally provides the features of
 * obtaining information about all the STAs connected to an AP and setting the country/region code.
 *
 * @since 1.0
 * @version 1.0
 */
struct IWiFiAp {
    struct IWiFiBaseFeature baseFeature;  /**< Basic features of {@link IWiFiBaseFeature} */

    /**
     * @brief Obtains information (MAC addresses only in the current version) about all the connected STAs.
     *
     * @param apFeature Indicates the pointer to the AP feature, as described in {@link IWiFiAp}.
     * @param staInfo Indicates the pointer to the information about the STAs connected to the AP.
     * @param count Indicates the number of elements in the staInfo structure array.
     * @param num Indicates the pointer to the number of connected STAs.
     *
     * @return Returns <b>0</b> if the information is obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getAsscociatedStas)(const struct IWiFiAp *apFeature, struct StaInfo *staInfo,
        uint32_t count, uint32_t *num);

    /**
     * @brief Sets the country/region code.
     *
     * @param apFeature Indicates the pointer to the AP feature, as described in {@link IWiFiAp}.
     * @param code Indicates the pointer to the country/region code to set.
     * @param len Indicates the length of the country/region code.
     *
     * @return Returns <b>0</b> if the country/region code is set; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*setCountryCode)(const struct IWiFiAp *apFeature, const char *code, uint32_t len);
};

/**
 * @brief Initializes a specified AP feature. This function is called during AP {@link FeatureType} creation.
 *
 * @param fe Indicates the double pointer to the AP feature.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value representing {@link HDF_STATUS}
 * if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t InitApFeature(struct IWiFiAp **fe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
/** @} */
