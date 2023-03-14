/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_PROXIMITY_DRIVER_H
#define SENSOR_PROXIMITY_DRIVER_H

#include "hdf_workqueue.h"
#include "osal_timer.h"
#include "sensor_config_parser.h"
#include "sensor_platform_if.h"

struct ProximityData {
    uint8_t stateFlag;
};

struct ProximityOpsCall {
    int32_t (*Init)(struct SensorCfgData *data);
    int32_t (*ReadData)(struct SensorCfgData *data);
};

struct ProximityDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue proximityWorkQueue;
    HdfWork proximityWork;
    OsalTimer proximityTimer;
    bool detectFlag;
    bool enable;
    int64_t interval;
    struct SensorCfgData *proximityCfg;
    struct ProximityOpsCall ops;
};

struct SensorCfgData *ProximityCreateCfgData(const struct DeviceResourceNode *node);
int32_t ProximityRegisterChipOps(const struct ProximityOpsCall *ops);
void ProximityReleaseCfgData(struct SensorCfgData *proximityCfg);

#endif /* SENSOR_PROXIMITY_DRIVER_H */
