/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MAGNETIC_LSM303_H
#define MAGNETIC_LSM303_H

#include "sensor_config_parser.h"
#include "sensor_magnetic_driver.h"

#define LSM303_MAGNETIC_GIN                     1000

/* MAGNETIC SET RATE AND MODE ADDR */
#define LSM303_CRA_REG_ADDR                     0X00
#define LSM303_CRB_REG_ADDR                     0X01
#define LSM303_MR_REG_ADDR                      0X02

/* MAGNETIC DATA REGISTERS ADDR */
#define LSM303_MAGNETIC_X_MSB_ADDR              0X03
#define LSM303_MAGNETIC_X_LSB_ADDR              0X04
#define LSM303_MAGNETIC_Y_MSB_ADDR              0X05
#define LSM303_MAGNETIC_Y_LSB_ADDR              0X06
#define LSM303_MAGNETIC_Z_MSB_ADDR              0X07
#define LSM303_MAGNETIC_Z_LSB_ADDR              0X08
#define LSM303_STATUS_ADDR                      0X09

/* MAGNETIC DATA RATE CONFIG HZ */
#define LSM303_DATA_RATE_0                      0X00
#define LSM303_DATA_RATE_1                      0X04
#define LSM303_DATA_RATE_2                      0X08
#define LSM303_DATA_RATE_3                      0X0C
#define LSM303_DATA_RATE_4                      0X10
#define LSM303_DATA_RATE_5                      0X14
#define LSM303_DATA_RATE_6                      0X18
#define LSM303_DATA_RATE_7                      0X1C

/* MAGNETIC GAIN CONFIG GAUSS */
#define LSM303_GAIN_RATE_0                      0X20
#define LSM303_GAIN_RATE_1                      0X40
#define LSM303_GAIN_RATE_2                      0X60
#define LSM303_GAIN_RATE_3                      0X80
#define LSM303_GAIN_RATE_4                      0XA0
#define LSM303_GAIN_RATE_5                      0XC0
#define LSM303_GAIN_RATE_6                      0XE0

/* MAGNETIC GAIN SENSITIVITY RANGE */
#define LSM303DLHC_SENSITIVITY_XY13GA           1100
#define LSM303DLHC_SENSITIVITY_XY19GA           855
#define LSM303DLHC_SENSITIVITY_XY25GA           670
#define LSM303DLHC_SENSITIVITY_XY40GA           450
#define LSM303DLHC_SENSITIVITY_XY47GA           400
#define LSM303DLHC_SENSITIVITY_XY56GA           330
#define LSM303DLHC_SENSITIVITY_XY81GA           230
#define LSM303DLHC_SENSITIVITY_Z13GA            980
#define LSM303DLHC_SENSITIVITY_Z19GA            760
#define LSM303DLHC_SENSITIVITY_Z25GA            600
#define LSM303DLHC_SENSITIVITY_Z40GA            400
#define LSM303DLHC_SENSITIVITY_Z47GA            355
#define LSM303DLHC_SENSITIVITY_Z56GA            295
#define LSM303DLHC_SENSITIVITY_Z81GA            205

/* MAGNETIC MODE CONFIG */
#define LSM303_OPERATING_MODE_1                 0X00
#define LSM303_OPERATING_MODE_2                 0X01
#define LSM303_OPERATING_MODE_3                 0X02
#define LSM303_OPERATING_MODE_4                 0X03

/* MAGNETIC DATA READY */
#define LSM303_DATA_READY_MASK                  0x01

int32_t DetectMagneticLsm303Chip(struct SensorCfgData *data);
int32_t ReadLsm303Data(struct SensorCfgData *data);

struct Lsm303DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /* MAGNETIC_LSM303_H */