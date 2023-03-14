/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef VIBRATOR_LINEAR_DRIVER_H
#define VIBRATOR_LINEAR_DRIVER_H

#include "hdf_device_desc.h"

struct VibratorLinearDriverData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    uint32_t busType;
    uint32_t gpioNum;
    uint32_t startReg;
    uint32_t stopReg;
    uint32_t mask;
};

#endif /* VIBRATOR_LINEAR_DRIVER_H */
