/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef HDF_POWER_MANAGEMENT_H
#define HDF_POWER_MANAGEMENT_H

#include "hdf_device_desc.h"
#include "osal_sysevent.h"

enum PowerManagementMode {
    HDF_POWER_SYS_CTRL,
    HDF_POWER_DYNAMIC_CTRL,
    HDF_POWER_MODE_MAX,
};

/**
 * @brief Defines the power management functions provided by the HDF for the driver.
 *
 * To use the power management mechanism provided by the HDF, implement operations of <b>IPowerEventListener</b> and
 * invoke {@linkHdfDeviceRegisterPowerListener} to register the operations with the HDF.
 *
 * @since 1.0
 */
struct IPowerEventListener {
    int (*DozeResume)(struct HdfDeviceObject *deviceObject);
    int (*DozeSuspend)(struct HdfDeviceObject *deviceObject);
    /** Wakes up the driver device. The driver developer implements the operation. */
    int (*Resume)(struct HdfDeviceObject *deviceObject);
    /** Hibernates the driver device. The driver developer implements the operation. */
    int (*Suspend)(struct HdfDeviceObject *deviceObject);
};

int HdfPmRegisterPowerListener(struct HdfDeviceObject *deviceObject, const struct IPowerEventListener *listener);
void HdfPmUnregisterPowerListener(struct HdfDeviceObject *deviceObject, const struct IPowerEventListener *listener);
void HdfPmAcquireDevice(struct HdfDeviceObject *deviceObject);
void HdfPmReleaseDevice(struct HdfDeviceObject *deviceObject);
void HdfPmAcquireDeviceAsync(struct HdfDeviceObject *deviceObject);
void HdfPmReleaseDeviceAsync(struct HdfDeviceObject *deviceObject);
void HdfPmSetMode(struct HdfDeviceObject *deviceObject, uint32_t mode);
#endif /* HDF_POWER_MANAGEMENT_H */
