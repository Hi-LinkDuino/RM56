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
 * @file wifi_module.h
 *
 * @brief Provides features of the WLAN module and functions to create and delete modules.
 *
 * The {@link WifiModule} object is a core abstraction of the WLAN driver. It contains a module that communicates
 * with user-level processes and also contains features.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDFLITE_WIFI_MODULE_H
#define HDFLITE_WIFI_MODULE_H

#include "wifi_inc.h"
#include "wifi_module_config.h"
#include "net_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines a WLAN feature.
 *
 * @since 1.0
 * @version 1.0
 */
struct WifiFeature {
    char name[MAX_WIFI_COMPONENT_NAME_LEN]; /**< Feature name, which contains a maximum of 10 characters */
    /**
     * @brief Initializes a feature.
     *
     * @param feature Indicates the pointer to the feature.
     *
     * @return Returns <b>0</b> if the feature is initialized; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*init)(struct WifiFeature *feature);

    /**
     * @brief Deinitializes a feature.
     *
     * @param feature Indicates the pointer to the feature.
     *
     * @return Returns <b>0</b> if the feature is deinitialized; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*deInit)(struct WifiFeature *feature);
};

/**
 * @brief Defines the WLAN feature list.
 *
 * @since 1.0
 * @version 1.0
 */
struct WifiFeatureList {
    struct WifiFeature *fe[HDF_WIFI_FEATURE_NUM]; /**< An array of WLAN features */
};

/**
 * @brief Defines WLAN module APIs.
 *
 * The APIs can be used to obtain, initialize, update, and perform other operations on a WLAN module.
 *
 * @since 1.0
 * @version 1.0
 */
struct WifiModuleIface {
    /**
     * @brief Deinitializes a WLAN module.
     *
     * @param module Indicates the pointer to the WLAN module.
     *
     * @return Returns <b>0</b> if the WLAN module is deinitialized; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*deInit)(struct WifiModule *module);

    /**
     * @brief Adds a feature.
     *
     * @param module Indicates the pointer to the WLAN module.
     * @param featureType Indicates the feature type.
     * @param feature Indicates the pointer to the feature.
     *
     * @return Returns <b>0</b> if the feature is added; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*addFeature)(struct WifiModule *module, uint16_t featureType, struct WifiFeature *feature);

    /**
     * @brief Deletes a feature.
     *
     * @param module Indicates the pointer to the WLAN module.
     * @param featureType Indicates the feature type.
     *
     * @return Returns <b>0</b> if the feature is deleted; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*delFeature)(struct WifiModule *module, uint16_t featureType);
};

/**
 * @brief Defines the WLAN module.
 *
 * The structure contains private data, APIs, module configurations, a module that communicates with user-level
 * processes, and WLAN features.
 *
 * @since 1.0
 * @version 1.0
 */
struct WifiModule {
    void *modulePrivate;                  /**< Private data */
    struct WifiModuleIface iface;         /**< APIs */
    struct WifiModuleConfig moduleConfig; /**< Module configurations */
    struct WifiFeatureList feList;        /**< WLAN features */
};

/**
 * @brief Defines a WLAN chip driver.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfChipDriver {
    uint16_t type;                          /**< Chip type */
    char name[MAX_WIFI_COMPONENT_NAME_LEN]; /**< Chip name */
    struct HdfMac80211BaseOps *ops;         /**< MAC address for the basic feature */
    struct HdfMac80211STAOps *staOps;       /**< MAC address for the STA feature */
    struct HdfMac80211APOps *apOps;         /**< MAC address for the AP feature */
    struct HdfMac80211P2POps *p2pOps;         /**< MAC address for the P2Pfeature */
    void *priv;                             /**< Private data of the chip driver */
    /**
     * @brief Initializes a chip driver.
     *
     * @param chipDriver Indicates the pointer to the chip driver.
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     *
     * @return Returns <b>0</b> if the chip is initialized; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*init)(struct HdfChipDriver *chipDriver, NetDevice *netDev);

    /**
     * @brief Deinitializes a chip driver.
     *
     * @param chipDriver Indicates the pointer to the chip driver.
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     *
     * @return Returns <b>0</b> if the chip is deinitialized; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*deinit)(struct HdfChipDriver *chipDriver, NetDevice *netDev);
};

/**
 * @brief Initializes a <b>WifiModule</b> object.
 *
 * @param module Indicates the pointer to the <b>WifiModule</b> object to initialize.
 * @param config Indicates the pointer to the configuration generated by the HDF Configuration Source (HCS).
 *
 * @return Returns <b>0</b> if the <b>WifiModule</b> object is initialized; returns a negative value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int16_t InitWifiModule(struct WifiModule *module, const struct HdfConfigWlanModuleConfig *config);

#define RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipOps, opsName)                                     \
    do {                                                                                      \
        if ((chipOps) == NULL || (chipOps)->opsName == NULL) { \
            HDF_LOGE("macOps" #opsName "not implement");                                      \
            return HDF_ERR_INVALID_OBJECT;                                                    \
        }                                                                                     \
    } while (0)


#ifdef __cplusplus
}
#endif

#endif // HDFLITE_WIFI_MODULE_H
/** @} */
