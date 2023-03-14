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
 * @file hdf_wlan_reset_manager.h
 *
 * @brief Declares data structures and functions of the WLAN reset manager.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDF_WLAN_RESET_MANAGE_H
#define HDF_WLAN_RESET_MANAGE_H

/**
 * @brief Describes the reset manager, including its configuration and functions.
 *
 * @since 1.0
 * @version 1.0
 */
struct ResetManager {
    /**
     * @brief Resets the WLAN module using a specified reset manager.
     *
     * @param resetManager Indicates the pointer to the reset manager.
     * @return Returns <b>0</b> if the WLAN module is reset; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Reset)(struct ResetManager *resetManager);

    /**
     * @brief Releases a specified reset manager.
     *
     * @param resetMgr Indicates the pointer to the reset manager.
     * @return Returns <b>0</b> if the reset manager is released; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Release)(struct ResetManager *resetMgr);
};

/**
 * @brief Creates a WLAN reset manager.
 *
 * @param resetConfig Indicates the pointer to the reset configuration.
 * @param holdTime Indicates the time for the reset configuration to take effect.
 * @return Returns a pointer to the created reset manager.
 *
 * @since 1.0
 * @version 1.0
 */
struct ResetManager *HdfWlanCreateResetManager(const struct HdfConfWlanRest *resetConfig, uint8_t holdTime);

#endif
/** @} */
