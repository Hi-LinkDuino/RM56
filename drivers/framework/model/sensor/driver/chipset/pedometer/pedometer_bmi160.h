/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PEDOMETER_BMI160_H
#define PEDOMETER_BMI160_H

#include "sensor_pedometer_driver.h"
#include "sensor_config_parser.h"

#define PEDOMETER_TEST_NUM                   1000

/* PEDOMETER DATA REGISTERS ADDR */
#define BMI160_PEDOMETER_LSB_ADDR            0x78
#define BMI160_PEDOMETER_MSB_ADDR            0x79
#define BMI160_STATUS_ADDR                   0x1B

/* PEDOMETER DATA READY */
#define BMI160_PEDOMETER_DATA_READY_MASK     0x80

int32_t DetectPedometerBim160Chip(struct SensorCfgData *data);
int32_t ReadBmi160PedometerData(struct SensorCfgData *data);
struct Bmi160DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /* PEDOMETER_BMI160_H */
