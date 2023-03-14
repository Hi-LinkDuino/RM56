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
 * @file wifi_module_config.h
 *
 * @brief Declares the WLAN module configuration.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDFLITE_WIFI_MODULE_CONFIG_H
#define HDFLITE_WIFI_MODULE_CONFIG_H

#include "osal.h"
#include "hdf_wlan_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the WLAN module configuration.
 *
 * @since 1.0
 * @version 1.0
 */
struct WifiModuleConfig {
    /**< Configuration of each feature of the WLAN module */
    const struct HdfConfigWlanModuleConfig *hslConfig;
};
#ifdef __cplusplus
}
#endif

#endif // HDFLITE_WIFI_MODULE_CONFIG_H
/** @} */
