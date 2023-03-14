/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_MAGNETIC_DRIVER_H
#define SENSOR_MAGNETIC_DRIVER_H

#include "hdf_workqueue.h"
#include "osal_timer.h"
#include "sensor_config_parser.h"
#include "sensor_platform_if.h"

#define MAGNETIC_DEFAULT_SAMPLING_200_MS    200000000
#define MAGNETIC_CHIP_NAME_LSM303    "lsm303"

enum MagneticAxisNum {
    MAGNETIC_X_AXIS   = 0,
    MAGNETIC_Y_AXIS   = 1,
    MAGNETIC_Z_AXIS   = 2,
    MAGNETIC_AXIS_NUM = 3,
};

enum MagneticAxisPart {
    MAGNETIC_X_AXIS_MSB = 0,
    MAGNETIC_X_AXIS_LSB = 1,
    MAGNETIC_Y_AXIS_MSB = 2,
    MAGNETIC_Y_AXIS_LSB = 3,
    MAGNETIC_Z_AXIS_MSB = 4,
    MAGNETIC_Z_AXIS_LSB = 5,
    MAGNETIC_AXIS_BUTT,
};

struct MagneticData {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct MagneticOpsCall {
    int32_t (*Init)(struct SensorCfgData *data);
    int32_t (*ReadData)(struct SensorCfgData *data);
};

struct MagneticDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue magneticWorkQueue;
    HdfWork magneticWork;
    OsalTimer magneticTimer;
    bool detectFlag;
    bool enable;
    int64_t interval;
    struct SensorCfgData *magneticCfg;
    struct MagneticOpsCall ops;
};

int32_t MagneticRegisterChipOps(const struct MagneticOpsCall *ops);
struct SensorCfgData *MagneticCreateCfgData(const struct DeviceResourceNode *node);
void MagneticReleaseCfgData(struct SensorCfgData *magneticCfg);

#endif /* SENSOR_MAGNETIC_DRIVER_H */
