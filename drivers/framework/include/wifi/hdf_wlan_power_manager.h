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
 * @file hdf_wlan_power_manager.h
 *
 * @brief Declares data structures and functions of the WLAN power manager.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDF_WLAN_POWER_MANAGE_H
#define HDF_WLAN_POWER_MANAGE_H

/**
 * @brief Provides functions for powering on and off the device, releasing power, and creating a power manager.
 *
 * @since 1.0
 * @version 1.0
 */
struct PowerManager {
    /**
     * @brief Powers on the device using a specified power manager.
     *
     * @param powerMgr Indicates the pointer to the power manager.
     * @return Returns <b>0</b> if the device is powered on; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*On)(struct PowerManager *powerMgr);

    /**
     * @brief Powers off the device using a specified power manager.
     *
     * @param powerMgr Indicates the pointer to the power manager.
     * @return Returns <b>0</b> if the device is powered off; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Off)(struct PowerManager *powerMgr);

    /**
     * @brief Releases power using a specified power manager.
     *
     * @param powerMgr Indicates the pointer to the power manager.
     * @return Returns <b>0</b> if power is released; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Release)(struct PowerManager *powerMgr);
};

/**
 * @brief Creates a WLAN power manager.
 *
 * @param configPowers Indicates the pointer to the power configuration.
 * @return Returns a pointer to the created power manager.
 *
 * @since 1.0
 * @version 1.0
 */
struct PowerManager *HdfWlanCreatePowerManager(const struct HdfConfWlanPowers *configPowers);

#endif
/** @} */
