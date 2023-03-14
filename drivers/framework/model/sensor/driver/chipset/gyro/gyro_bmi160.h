/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef GYRO_BMI160_H
#define GYRO_BMI160_H

#include "sensor_gyro_driver.h"
#include "sensor_config_parser.h"

/* GYRO DATA REGISTERS ADDR */
#define BMI160_GYRO_X_LSB_ADDR              0X0C
#define BMI160_GYRO_X_MSB_ADDR              0X0D
#define BMI160_GYRO_Y_LSB_ADDR              0X0E
#define BMI160_GYRO_Y_MSB_ADDR              0X0F
#define BMI160_GYRO_Z_LSB_ADDR              0X10
#define BMI160_GYRO_Z_MSB_ADDR              0X11
#define BMI160_STATUS_ADDR                  0X1B

/* GYRO ODR */
#define BMI160_GYRO_ODR_RESERVED            0x00
#define BMI160_GYRO_ODR_25HZ                0x06
#define BMI160_GYRO_ODR_50HZ                0x07
#define BMI160_GYRO_ODR_100HZ               0x08
#define BMI160_GYRO_ODR_200HZ               0x09
#define BMI160_GYRO_ODR_400HZ               0x0A
#define BMI160_GYRO_ODR_800HZ               0x0B
#define BMI160_GYRO_ODR_1600HZ              0x0C
#define BMI160_GYRO_ODR_3200HZ              0x0D

/* default HZ */
#define BMI160_GYRO_DEFAULT_ODR_100HZ       100
#define BMI160_GYRO_DEFAULT_ODR_25HZ        25

/* GYRO RANGE */
#define BMI160_GYRO_RANGE_2000DPS           0X00
#define BMI160_GYRO_RANGE_1000DPS           0X01
#define BMI160_GYRO_RANGE_500DPS            0X02
#define BMI160_GYRO_RANGE_250DPS            0X03
#define BMI160_GYRO_RANGE_125DPS            0X04

/* GYRO sensitivity */
#define BMI160_GYRO_SENSITIVITY_2000DPS       61

/* GYRO DATA READY */
#define BMI160_GYRO_DATA_READY_MASK         0x40

int32_t DetectGyroBim160Chip(struct SensorCfgData *data);
int32_t ReadBmi160Data(struct SensorCfgData *data);

struct Bmi160DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /* GYRO_BMI160_H */
