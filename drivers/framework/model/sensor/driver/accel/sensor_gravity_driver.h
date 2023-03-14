/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_GRAVITY_DRIVER_H
#define SENSOR_GRAVITY_DRIVER_H

#include "hdf_workqueue.h"
#include "osal_timer.h"
#include "sensor_config_parser.h"
#include "sensor_platform_if.h"

#define GRAVITY_TIMER_MIN_TIME           10
#define GRAVITY_TIMER_MAX_TIME           20
#define GRAVITY_UNITS                    100
#define GRAVITY_FILTER_UNITS             85
enum GravityAxisNum {
    GRAVITY_X_AXIS   = 0,
    GRAVITY_Y_AXIS   = 1,
    GRAVITY_Z_AXIS   = 2,
    GRAVITY_AXIS_NUM = 3,
};

struct GravityData {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct GravityDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue gravityWorkQueue;
    HdfWork gravityWork;
    OsalTimer gravityTimer;
    struct SensorCfgData *gravityCfg;
    int64_t interval;
    bool enable;
};

#endif /* SENSOR_GRAVITY_DRIVER_H */