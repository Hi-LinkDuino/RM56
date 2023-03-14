/*
 * Copyright (c) 2021 xu
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PROXIMITY_APDS9960_H
#define PROXIMITY_APDS9960_H

#include "sensor_config_parser.h"
#include "sensor_proximity_driver.h"

#define APDS9960_PROXIMITY_DATA_ADDR              0X9C    // Proximity Data

#define APDS9960_PROXIMITY_THRESHOLD               7    // threshold

int32_t DetectProximityApds9960Chip(struct SensorCfgData *data);
int32_t ReadApds9960Data(struct SensorCfgData *data);

struct Apds9960DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /* PROXIMITY_APDS9960_H */