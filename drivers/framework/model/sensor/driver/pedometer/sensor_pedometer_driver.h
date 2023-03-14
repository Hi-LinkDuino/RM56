/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_PEDOMETER_DRIVER_H
#define SENSOR_PEDOMETER_DRIVER_H

#include "hdf_workqueue.h"
#include "osal_timer.h"
#include "sensor_config_parser.h"
#include "sensor_platform_if.h"

enum PedometerDataPart {
    PEDOMETER_NU_LSB = 0,
    PEDOMETER_NU_MSB = 1,
    PEDOMETER_BUTT,
};

struct PedometerData {
    int32_t pedometer;
};

struct PedometerOpsCall {
    int32_t (*Init)(struct SensorCfgData *data);
    int32_t (*ReadData)(struct SensorCfgData *data);
};

struct PedometerDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue pedometerWorkQueue;
    HdfWork pedometerWork;
    OsalTimer pedometerTimer;
    bool detectFlag;
    bool enable;
    int64_t interval;
    struct SensorCfgData *pedometerCfg;
    struct PedometerOpsCall ops;
};

int32_t PedometerRegisterChipOps(const struct PedometerOpsCall *ops);
struct SensorCfgData *PedometerCreateCfgData(const struct DeviceResourceNode *node);
void PedometerReleaseCfgData(struct SensorCfgData *pedometerCfg);

#endif /* SENSOR_PEDOMETER_DRIVER_H */
