/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_GYRO_DRIVER_H
#define SENSOR_GYRO_DRIVER_H

#include "hdf_workqueue.h"
#include "osal_mutex.h"
#include "osal_timer.h"
#include "sensor_config_parser.h"
#include "sensor_platform_if.h"

enum GyroAxisNum {
    GYRO_X_AXIS   = 0,
    GYRO_Y_AXIS   = 1,
    GYRO_Z_AXIS   = 2,
    GYRO_AXIS_NUM = 3,
};

enum GyroAxisPart {
    GYRO_X_AXIS_LSB = 0,
    GYRO_X_AXIS_MSB = 1,
    GYRO_Y_AXIS_LSB = 2,
    GYRO_Y_AXIS_MSB = 3,
    GYRO_Z_AXIS_LSB = 4,
    GYRO_Z_AXIS_MSB = 5,
    GYRO_AXIS_BUTT,
};

struct GyroData {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct GyroOpsCall {
    int32_t (*Init)(struct SensorCfgData *data);
    int32_t (*ReadData)(struct SensorCfgData *data);
};

struct GyroDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue gyroWorkQueue;
    HdfWork gyroWork;
    OsalTimer gyroTimer;
    bool detectFlag;
    bool enable;
    int64_t interval;
    struct SensorCfgData *gyroCfg;
    struct GyroOpsCall ops;
};

int32_t GyroRegisterChipOps(const struct GyroOpsCall *ops);
struct SensorCfgData *GyroCreateCfgData(const struct DeviceResourceNode *node);
void GyroReleaseCfgData(struct SensorCfgData *gyroCfg);

#endif /* SENSOR_GYRO_DRIVER_H */
