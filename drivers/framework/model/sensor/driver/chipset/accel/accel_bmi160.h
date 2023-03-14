/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef ACCEL_BMI160_H
#define ACCEL_BMI160_H

#include "sensor_accel_driver.h"
#include "sensor_config_parser.h"

/* ACCEL DATA REGISTERS ADDR */
#define BMI160_ACCEL_X_LSB_ADDR              0X12
#define BMI160_ACCEL_X_MSB_ADDR              0X13
#define BMI160_ACCEL_Y_LSB_ADDR              0X14
#define BMI160_ACCEL_Y_MSB_ADDR              0X15
#define BMI160_ACCEL_Z_LSB_ADDR              0X16
#define BMI160_ACCEL_Z_MSB_ADDR              0X17
#define BMI160_STATUS_ADDR                   0X1B

/* ACCEL ODR */
#define BMI160_ACCEL_ODR_RESERVED            0x00
#define BMI160_ACCEL_ODR_0_78HZ              0x01
#define BMI160_ACCEL_ODR_1_56HZ              0x02
#define BMI160_ACCEL_ODR_3_12HZ              0x03
#define BMI160_ACCEL_ODR_6_25HZ              0x04
#define BMI160_ACCEL_ODR_12_5HZ              0x05
#define BMI160_ACCEL_ODR_25HZ                0x06
#define BMI160_ACCEL_ODR_50HZ                0x07
#define BMI160_ACCEL_ODR_100HZ               0x08
#define BMI160_ACCEL_ODR_200HZ               0x09
#define BMI160_ACCEL_ODR_400HZ               0x0A
#define BMI160_ACCEL_ODR_800HZ               0x0B
#define BMI160_ACCEL_ODR_1600HZ              0x0C
#define BMI160_ACCEL_ODR_RESERVED0           0x0D
#define BMI160_ACCEL_ODR_RESERVED1           0x0E
#define BMI160_ACCEL_ODR_RESERVED2           0x0F

/* default HZ */
#define BMI160_ACCEL_DEFAULT_ODR_100HZ       100
#define BMI160_ACCEL_DEFAULT_ODR_25HZ        25

/* ACCEL RANGE */
#define BMI160_ACCEL_RANGE_2G                0X03
#define BMI160_ACCEL_RANGE_4G                0X05
#define BMI160_ACCEL_RANGE_8G                0X08
#define BMI160_ACCEL_RANGE_16G               0X0C

/* ACC sensitivity */
#define BMI160_ACC_SENSITIVITY_2G            61
#define BMI160_ACC_SENSITIVITY_4G            122
#define BMI160_ACC_SENSITIVITY_8G            244
#define BMI160_ACC_SENSITIVITY_16G           488

/* ACCEL DATA READY */
#define BMI160_ACCEL_DATA_READY_MASK         0x80

int32_t DetectAccelBim160Chip(struct SensorCfgData *data);
int32_t ReadBmi160Data(struct SensorCfgData *cfg, struct SensorReportEvent *event);
struct Bmi160DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /* ACCEL_BMI160_H */
