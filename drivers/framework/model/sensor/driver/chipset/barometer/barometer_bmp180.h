/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef BAROMETER_BMP180_H
#define BAROMETER_BMP180_H

#include "sensor_barometer_driver.h"
#include "sensor_config_parser.h"

#define BMP180_REG_CHIP_ID         0xD0

// i2c slave address

#define BMP180_ADDR                0x77

// Define calibration register address

#define BMP180_AC1_MSB_ADDR        0xAA
#define BMP180_AC1_LSB_ADDR        0xAB
#define BMP180_AC2_MSB_ADDR        0xAC
#define BMP180_AC2_LSB_ADDR        0xAD
#define BMP180_AC3_MSB_ADDR        0xAE
#define BMP180_AC3_LSB_ADDR        0xAF
#define BMP180_AC4_MSB_ADDR        0xB0
#define BMP180_AC4_LSB_ADDR        0xB1
#define BMP180_AC5_MSB_ADDR        0xB2
#define BMP180_AC5_LSB_ADDR        0xB3
#define BMP180_AC6_MSB_ADDR        0xB4
#define BMP180_AC6_LSB_ADDR        0xB5
#define BMP180_B1_MSB_ADDR         0xB6
#define BMP180_B1_LSB_ADDR         0xB7
#define BMP180_B2_MSB_ADDR         0xB8
#define BMP180_B2_LSB_ADDR         0xB9
#define BMP180_MB_MSB_ADDR         0xBA
#define BMP180_MB_LSB_ADDR         0xBB
#define BMP180_MC_MSB_ADDR         0xBC
#define BMP180_MC_LSB_ADDR         0xBD
#define BMP180_MD_MSB_ADDR         0xBE
#define BMP180_MD_LSB_ADDR         0xBf

// Control register

#define BMP180_CONTROL_REG_ADDR    0xF4
#define BMP180_COVERT_TEMP         0x2E
#define BMP180_COVERT_PRES_0       0x34
#define BMP180_COVERT_PRES_1       0x74
#define BMP180_COVERT_PRES_2       0xB4
#define BMP180_COVERT_PRES_3       0xF4

#define BMP180_OUT_MSB_ADDR        0xF6
#define BMP180_OUT_LSB_ADDR        0xF7
#define BMP180_OUT_XLSB_ADDR       0xF8

#define BMP180_STATUS_ADDR         0X20
#define BMP180_STATUS_JUDGE        0X00

#define SENSOR_DATA_WIDTH_16_BIT   16

#define OSSETTING                  1
#define DELAY_0                    5
#define DELAY_1                    8
#define OSS_TIME_MS	               26

#define BMP180_CONSTANT_0          (-7357)
#define BMP180_CONSTANT_1          1
#define BMP180_CONSTANT_2          2
#define BMP180_CONSTANT_3          4
#define BMP180_CONSTANT_4          8
#define BMP180_CONSTANT_5          11
#define BMP180_CONSTANT_6          12
#define BMP180_CONSTANT_7          13
#define BMP180_CONSTANT_8          15
#define BMP180_CONSTANT_9          16
#define BMP180_CONSTANT_10         3038
#define BMP180_CONSTANT_11         3791
#define BMP180_CONSTANT_12         4000
#define BMP180_CONSTANT_13         32768
#define BMP180_CONSTANT_14         50000
#define BMP180_CONSTANT_15         0x80000000


int32_t DetectBarometerBmp180Chip(struct SensorCfgData *data);
int32_t ReadBmp180Data(struct SensorCfgData *data);

struct Bmp180DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /*  BAROMETER_BMP180_H */