/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_BAROMETER_DRIVER_H
#define SENSOR_BAROMETER_DRIVER_H

#include "hdf_workqueue.h"
#include "osal_timer.h"
#include "sensor_config_parser.h"
#include "sensor_platform_if.h"

#define BAR_DEFAULT_SAMPLING_200_MS    200000000
#define BAROMETER_CHIP_NAME_BMP180    "bmp180"

enum BarometerEeprom {
    BAROMETER_AC1_MSB = 0,
    BAROMETER_AC1_LSB = 1,
    BAROMETER_AC2_MSB = 2,
    BAROMETER_AC2_LSB = 3,
    BAROMETER_AC3_MSB = 4,
    BAROMETER_AC3_LSB = 5,
    BAROMETER_AC4_MSB = 6,
    BAROMETER_AC4_LSB = 7,
    BAROMETER_AC5_MSB = 8,
    BAROMETER_AC5_LSB = 9,
    BAROMETER_AC6_MSB = 10,
    BAROMETER_AC6_LSB = 11,
    BAROMETER_B1_MSB  = 12,
    BAROMETER_B1_LSB  = 13,
    BAROMETER_B2_MSB  = 14,
    BAROMETER_B2_LSB  = 15,
    BAROMETER_MB_MSB  = 16,
    BAROMETER_MB_LSB  = 17,
    BAROMETER_MC_MSB  = 18,
    BAROMETER_MC_LSB  = 19,
    BAROMETER_MD_MSB  = 20,
    BAROMETER_MD_LSB  = 21,
    BAROMETER_EEPROM_SUM,
};

struct BarometerEepromData {
    int32_t ac1;
    int32_t ac2;
    int32_t ac3;
    int32_t b1;
    int32_t b2;
    int32_t mb;
    int32_t mc;
    int32_t md;
    uint32_t ac4;
    uint32_t ac5;
    uint32_t ac6;
};

struct Coefficient {
    int32_t b3;
    int32_t b5;
    int32_t b6;
    int32_t x1;
    int32_t x2;
    int32_t x3;
    int32_t p;
    uint32_t b4;
    uint32_t b7;
};

enum Temperature {
    BAROMETER_TEM_MSB = 0,
    BAROMETER_TEM_LSB = 1,
    BAROMETER_TEM_SUM,
};

enum Barometer {
    BAROMETER_BAR_MSB  = 0,
    BAROMETER_BAR_LSB  = 1,
    BAROMETER_BAR_XLSB = 2,
    BAROMETER_BAR_SUM,
};

struct  BarometerRawData {
    int32_t unpensatePre;
    int32_t unpensateTemp;
};

enum BarometerData {
    BAROMETER_BAROMETER   = 0,
    BAROMETER_TEMPERATURE = 1,
    BAROMETER_ALTITUDE    = 2,
    BAROMETER_SUM,
};

struct BarometerOpsCall {
    int32_t (*Init)(struct SensorCfgData *data);
    int32_t (*ReadData)(struct SensorCfgData *data);
};

struct BarometerDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue barometerWorkQueue;
    HdfWork barometerWork;
    OsalTimer barometerTimer;
    bool detectFlag;
    bool enable;
    int64_t interval;
    struct SensorCfgData *barometerCfg;
    struct BarometerOpsCall ops;
};

int32_t BarometerRegisterChipOps(const struct BarometerOpsCall *ops);
struct SensorCfgData *BarometerCreateCfgData(const struct DeviceResourceNode *node);
void BarometerReleaseCfgData(struct SensorCfgData *barometerCfg);

#endif /* SENSOR_BAROMETER_DRIVER_H */
