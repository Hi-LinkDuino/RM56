/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_ACCEL_DRIVER_H
#define SENSOR_ACCEL_DRIVER_H

#include "hdf_workqueue.h"
#include "osal_mutex.h"
#include "osal_timer.h"
#include "sensor_config_parser.h"
#include "sensor_platform_if.h"

enum AccelAxisNum {
    ACCEL_X_AXIS   = 0,
    ACCEL_Y_AXIS   = 1,
    ACCEL_Z_AXIS   = 2,
    ACCEL_AXIS_NUM = 3,
};

enum AccelAxisPart {
    ACCEL_X_AXIS_LSB = 0,
    ACCEL_X_AXIS_MSB = 1,
    ACCEL_Y_AXIS_LSB = 2,
    ACCEL_Y_AXIS_MSB = 3,
    ACCEL_Z_AXIS_LSB = 4,
    ACCEL_Z_AXIS_MSB = 5,
    ACCEL_AXIS_BUTT,
};

typedef int32_t (*GravitySubscribeAccelCallback)(int32_t *rawData, int32_t size);

struct AccelData {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct AccelOpsCall {
    int32_t (*Init)(struct SensorCfgData *data);
    int32_t (*ReadData)(struct SensorCfgData *data, struct SensorReportEvent *event);
};

struct AccelDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue accelWorkQueue;
    HdfWork accelWork;
    OsalTimer accelTimer;
    bool detectFlag;
    bool enable;
    int64_t interval;
    struct SensorCfgData *accelCfg;
    struct AccelOpsCall ops;
    GravitySubscribeAccelCallback cb;
};

int32_t AccelRegisterChipOps(const struct AccelOpsCall *ops);
struct SensorCfgData *AccelCreateCfgData(const struct DeviceResourceNode *node);
void AccelReleaseCfgData(struct SensorCfgData *accelCfg);
int32_t SubscribeAccelDataCallbackFunc(GravitySubscribeAccelCallback cb);
#endif /* SENSOR_ACCEL_DRIVER_H */
