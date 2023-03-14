/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_CHIP_H
#define HDF_CHIP_H

#include "hdf_chip_config.h"
#include "osal/osal_time.h"

struct HdfReset;
struct HdfPower;

struct HdfPowerOps {
    /**
     * @brief Powers on the device using a specified power manage interface.
     *
     * @param powerMgr Indicates the pointer to the power manage interface.
     * @return Returns <b>0</b> if the device is powered on; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*On)(struct HdfPower *powerMgr);

    /**
     * @brief Powers off the device using a specified power manage interface.
     *
     * @param powerMgr Indicates the pointer to the power manage interface.
     * @return Returns <b>0</b> if the device is powered off; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Off)(struct HdfPower *powerMgr);

    /**
     * @brief Releases power using a specified power manage interface.
     *
     * @param powerMgr Indicates the pointer to the power manage interface.
     *
     * @since 1.0
     * @version 1.0
     */
    void (*Release)(struct HdfPower *powerMgr);
};
/**
 * @brief Provides functions for powering on and off the device, releasing power, and creating a power manage interface.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfPower {
    const struct HdfPowerOps *ops;
};

struct HdfResetOps {
    /**
     * @brief Resets the WLAN module using a specified reset manage interface.
     *
     * @param resetManager Indicates the pointer to the reset manage interface.
     * @return Returns <b>0</b> if the WLAN module is reset; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Reset)(struct HdfReset *resetManager);

    /**
     * @brief Releases a specified reset manage interface.
     *
     * @param resetMgr Indicates the pointer to the reset manage interface.
     *
     * @since 1.0
     * @version 1.0
     */
    void (*Release)(struct HdfReset *resetMgr);
};

/**
 * @brief Describes the reset manage interface, including its configuration and functions.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfReset {
    const struct HdfResetOps *ops;
};

struct HdfUartBus {
    const char *name;
};

struct HdfBus {
    uint8_t type;
    union {
        struct HdfUartBus uart;
    };
};

enum FunctionType { FUNC_TYPE_WLAN = 0, FUNC_TYPE_BT };

struct HdfVirtualDevice {
    const char *name;
    struct HdfCompositeDevice *parent;
    struct HdfPower *power;
    struct HdfReset *reset;
    struct HdfBus *bus;
    uint8_t bootUpTimeOut;
    uint8_t functionType;
};

struct HdfVirtualDevice *CreateVirtualDevice(struct HdfChipConfig *config);
void ReleaseVirtualDevice(struct HdfVirtualDevice *device);

inline static int32_t HdfPowerOnVirtualDevice(struct HdfVirtualDevice *device) {
    if (device == NULL) {
        return HDF_FAILURE;
    }
    if (device->power == NULL || device->power->ops == NULL || device->power->ops->On == NULL) {
        return HDF_FAILURE;
    }
    return device->power->ops->On(device->power);
}
inline static int32_t HdfPowerOffVirtualDevice(struct HdfVirtualDevice *device) {
    if (device == NULL) {
        return HDF_FAILURE;
    }
    if (device->power == NULL || device->power->ops == NULL || device->power->ops->Off == NULL) {
        return HDF_FAILURE;
    }
    return device->power->ops->Off(device->power);
}
inline static int32_t HdfResetVirtualDevice(struct HdfVirtualDevice *device) {
    int32_t ret;
    if (device == NULL) {
        return HDF_FAILURE;
    }
    if (device->reset == NULL || device->reset->ops == NULL || device->reset->ops->Reset == NULL) {
        return HDF_FAILURE;
    }
    ret = device->reset->ops->Reset(device->reset);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    OsalMSleep(device->bootUpTimeOut);
}

#endif