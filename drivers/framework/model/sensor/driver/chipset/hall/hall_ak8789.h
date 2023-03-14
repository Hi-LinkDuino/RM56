/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HALL_AK8789_H
#define HALL_AK8789_H

#include "sensor_config_parser.h"
#include "sensor_hall_driver.h"

int32_t DetectHallAk8789Chip(struct SensorCfgData *data);
int32_t ReadAk8789Data(struct SensorCfgData *data);

struct Ak8789DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /* HALL_AK8789_H */