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
 * @file hdf_wlan_chipdriver_manager.h
 *
 * @brief Declares data structures and functions of the WLAN chip driver manager.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDF_WLAN_CHIPDRIVER_MANGER_H
#define HDF_WLAN_CHIPDRIVER_MANGER_H
#include "hdf_wifi_product.h"
#include "hdf_wlan_power_manager.h"
#include "hdf_wlan_reset_manager.h"
#include "wifi_module.h"

/**
 * @brief Indicates the maximum number of chip drivers.
 *
 * @since 1.0
 * @version 1.0
 */
#define MAX_CHIPDRIVER_COUNT 16

/**
 * @brief Indicates the maximum number of WLAN chip driver managers.
 *
 * @since 1.0
 * @version 1.0
 */
#define HDF_WLAN_MAX_MANAGER_COUNT 8

/**
 * @brief Describes WLAN device data.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfWifiNetDeviceData {
    uint8_t netInterfaceId;            /**< Network interface ID */
    struct HdfChipDriver *chipDriver;  /**< Chip driver */
    struct HdfWlanDevice *device;      /**< WLAN device */
};

/**
 * @brief Describes the <b>HdfChipDriverFactory</b> instance and provides functions such as initializing and
 * deinitializing a chip, and obtaining the maximum number of interfaces.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfChipDriverFactory {
    const char *driverName;  /**< Driver name */

    /**
     * @brief Initializes a specified chip.
     *
     * @param device Indicates the pointer to the WLAN device where the chip resides.
     * @return Returns <b>0</b> if the chip is initialized; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*InitChip)(struct HdfWlanDevice *device);

    /**
     * @brief Deinitializes a specified chip.
     *
     * @param device Indicates the pointer to the WLAN device where the chip resides.
     * @return Returns <b>0</b> if the chip is deinitialized; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DeinitChip)(struct HdfWlanDevice *device);

    /**
     * @brief Releases a specified <b>HdfChipDriverFactory</b> instance.
     *
     * @param factory Indicates the pointer to the <b>HdfChipDriverFactory</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    void (*ReleaseFactory)(struct HdfChipDriverFactory *factory);

    /**
     * @brief Creates a <b>HdfChipDriver</b> instance with the initialization and deinitialization functions.
     *
     * @param device Indicates the pointer to the WLAN device.
     * @param ifIndex Indicates the interface index.
     * @return Returns the pointer to the <b>HdfChipDriver</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    struct HdfChipDriver *(*Build)(struct HdfWlanDevice *device, uint8_t ifIndex);

    /**
     * @brief Releases a specified <b>HdfChipDriver</b> instance.
     *
     * @param chipDriver Indicates the pointer to the <b>HdfChipDriver</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    void (*Release)(struct HdfChipDriver *chipDriver);

    /**
     * @brief Obtains the maximum number of interfaces of a specified <b>HdfChipDriverFactory</b> instance.
     *
     * @param factory Indicates the pointer to the <b>HdfChipDriverFactory</b> instance.
     * @return Returns the maximum number of interfaces.
     *
     * @since 1.0
     * @version 1.0
     */
    uint8_t (*GetMaxIFCount)(struct HdfChipDriverFactory *factory);
};

/**
 * @brief Manages chip drivers.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfChipDriverManager {
    struct HdfChipDriverFactory **chipFactoryInsts;  /**< <b>HdfChipDriverFactory</b> instances */

    /**
     * @brief Registers a chip driver.
     *
     * @param factoryInst Indicates the pointer to the <b>HdfChipDriverFactory</b> instance.
     * @return Returns <b>0</b> if the chip driver is registered; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*RegChipDriver)(struct HdfChipDriverFactory *factoryInst);

    /**
     * @brief Obtains the chip driver based on the network device name.
     *
     * @param name Indicates the pointer to the network device name.
     * @return Returns the pointer to the <b>HdfChipDriverFactory</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    struct HdfChipDriverFactory *(*GetChipDriverByName)(const char *name);
};

/**
 * @brief Obtains an <b>HdfChipDriverManager</b> instance.
 *
 * @return Returns the pointer to the <b>HdfChipDriverManager</b> instance.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfChipDriverManager *HdfWlanGetChipDriverMgr(void);

/**
 * @brief Deinitializes an <b>HdfChipDriverManager</b> instance.
 *
 * @return Returns <b>0</b> if the <b>HdfChipDriverManager</b> instance is deinitialized; returns a
 * negative value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t ChipDriverMgrDeInit(void);

#endif
/** @} */
