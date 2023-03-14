/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef ALS_BH1745_H
#define ALS_BH1745_H

#include "sensor_als_driver.h"
#include "sensor_config_parser.h"

/* ALS DATA REGISTERS ADDR */
#define BH1745_ALS_R_LSB_ADDR                     0X50
#define BH1745_ALS_R_MSB_ADDR                     0X51
#define BH1745_ALS_G_LSB_ADDR                     0X52
#define BH1745_ALS_G_MSB_ADDR                     0X53
#define BH1745_ALS_B_LSB_ADDR                     0X54
#define BH1745_ALS_B_MSB_ADDR                     0X55
#define BH1745_ALS_C_LSB_ADDR                     0X56
#define BH1745_ALS_C_MSB_ADDR                     0X57
#define BH1745_MODECONTROL3_ADDR                  0X44

/* ALS DATA READY */
#define BH1745_ALS_DATA_READY_MASK                0X02

/* ALS MULTIPLE */
#define BH1745_MULTIPLE_100                       100

/* ALS COEFFICIENT  */
#define BH1745_COEFFICIENT_RED                    2
#define BH1745_COEFFICIENT_GREEN                  2
#define BH1745_COEFFICIENT_RED_LEVEL_0            77300          // 7.73 * 10000
#define BH1745_COEFFICIENT_RED_LEVEL_1            92715          // 9.2715 * 10000
#define BH1745_COEFFICIENT_GREEN_LEVEL_0          131920         // 1.3192 * 10000
#define BH1745_COEFFICIENT_GREEN_LEVEL_1          214770         // 2.1477 * 10000
#define BH1745_COEFFICIENT_JUDGE                  78             // 0.78*100

/* ALS TIME */
#define BH1745_TIME_160MSEC                      160
#define BH1745_TIME_320MSEC                      320
#define BH1745_TIME_640MSEC                      640
#define BH1745_TIME_1280MSEC                     1280
#define BH1745_TIME_2560MSEC                     2560
#define BH1745_TIME_5120MSEC                     5120
#define BH1745_TIME_MAX                          5570475         // 65535*0.85*100
#define BH1745_TIME_MIN                          1638375         // 65535*0.25*100

/* ALS GAIN */
#define BH1745_GAIN_1X                           1
#define BH1745_GAIN_2X                           2
#define BH1745_GAIN_16X                          16

/* ALS TIME REG VALUE */
#define EXTENDED_ALS_TIME_GROUP_ATTR_VALUE_0                 0x00
#define EXTENDED_ALS_TIME_GROUP_ATTR_VALUE_1                 0x01
#define EXTENDED_ALS_TIME_GROUP_ATTR_VALUE_2                 0x02
#define EXTENDED_ALS_TIME_GROUP_ATTR_VALUE_3                 0x03
#define EXTENDED_ALS_TIME_GROUP_ATTR_VALUE_4                 0x04
#define EXTENDED_ALS_TIME_GROUP_ATTR_VALUE_5                 0x05

/* ALS GAIN REG VALUE */
#define EXTENDED_ALS_GAIN_GROUP_ATTR_VALUE_0                 0x00
#define EXTENDED_ALS_GAIN_GROUP_ATTR_VALUE_1                 0x01
#define EXTENDED_ALS_GAIN_GROUP_ATTR_VALUE_2                 0x02

enum ExtendedAlsTimeRegGroupIndex {
    EXTENDED_ALS_TIME_GROUP_INDEX_0 = 0,
    EXTENDED_ALS_TIME_GROUP_INDEX_1,
    EXTENDED_ALS_TIME_GROUP_INDEX_2,
    EXTENDED_ALS_TIME_GROUP_INDEX_3,
    EXTENDED_ALS_TIME_GROUP_INDEX_4,
    EXTENDED_ALS_TIME_GROUP_INDEX_5,
    EXTENDED_ALS_TIME_GROUP_INDEX_MAX,
};

enum ExtendedAlsGainRegGroupIndex {
    EXTENDED_ALS_GAIN_GROUP_INDEX_0 = 0,
    EXTENDED_ALS_GAIN_GROUP_INDEX_1,
    EXTENDED_ALS_GAIN_GROUP_INDEX_2,
    EXTENDED_ALS_GAIN_GROUP_INDEX_MAX,
};

int32_t DetectAlsBim160Chip(struct SensorCfgData *data);
int32_t ReadBh1745Data(struct SensorCfgData *data);

struct Bh1745DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /* ALS_BH1745_H */
