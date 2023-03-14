/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef VIBRATOR_DRIVER_H
#define VIBRATOR_DRIVER_H

#include "osal_mutex.h"
#include "hdf_device_desc.h"
#include "hdf_workqueue.h"
#include "vibrator_driver_type.h"

struct VibratorOps {
    int32_t (*Start)(void);
    int32_t (*StartEffect)(uint32_t effectType);
    int32_t (*Stop)(void);
};

typedef int32_t (*VibratorCmdHandle)(struct HdfSBuf *reqData, struct HdfSBuf *reply);

struct VibratorCmdHandleList {
    int32_t cmd;
    VibratorCmdHandle func;
};

struct VibratorDriverData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue workQueue;
    HdfWork work;
    struct OsalMutex mutex;
    enum VibratorConfigMode mode;
    enum VibratorState state;
    struct VibratorOps ops;
};

void StartTimeVibrator(void);
void StopVibrator(void);
void SetEffectVibrator(uint32_t type);
int32_t RegisterVibrator(struct VibratorOps *ops);

#endif /* VIBRATOR_DRIVER_H */
