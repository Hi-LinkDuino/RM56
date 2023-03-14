/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
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
 * @file wifi_inc.h
 *
 * @brief Describes the data structure of WLAN features and bus.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDFLITE_WIFI_MODULE_INC_H
#define HDFLITE_WIFI_MODULE_INC_H

#ifdef __cplusplus
extern "C" {
#endif

struct WifiModule;
struct WifiFeature;
struct WifiFeatureList;
struct HdfChipDriver;
struct BusDev;

/**
 * @brief Indicates the length of the name of a WLAN module or feature.
 *
 * The name of a WLAN module or feature cannot contain more than 10 characters; otherwise, the name will be truncated
 * and cannot be identified.
 *
 * @since 1.0
 * @version 1.0
 */
#define MAX_WIFI_COMPONENT_NAME_LEN 10

/**
 * @brief Enumerates feature types of a WLAN module.
 *
 * You can query and register a feature or bind a feature to a WLAN module based on the feature type.
 *
 * @since 1.0
 * @version 1.0
 */
enum WifiMainFeatureType {
    HDF_WIFI_FEATURE_AP,       /**< Access point (AP) */
    HDF_WIFI_FEATURE_STA,      /**< Station */
    HDF_WIFI_FEATURE_P2P,      /**< Peer-to-peer (P2P) */
    HDF_WIFI_FEATURE_NAN,      /**< Neighbor Awareness Networking (NAN) */
    HDF_WIFI_FEATURE_RTT,      /**< Round Trip Time (RTT) */
    HDF_WIFI_FEATURE_NUM = 10  /**< Maximum number of features */
};

/**
 * @brief Enumerates bus types of a WLAN module.
 *
 * @since 1.0
 * @version 1.0
 */
enum WifiBusType {
    BUS_SDIO,  /**< Secure Digital Input and Output (SDIO) */
    BUS_USB,   /**< Universal Serial Bus (USB) */
};

#ifdef __cplusplus
}
#endif

#endif // HDFLITE_WIFI_MODULE_INC_H
/** @} */
