/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_ALS_DRIVER_H
#define SENSOR_ALS_DRIVER_H

#include "hdf_workqueue.h"
#include "osal_timer.h"
#include "sensor_config_parser.h"
#include "sensor_platform_if.h"

#define ALS_DEFAULT_SAMPLING_200_MS    200000000
#define ALS_CHIP_NAME_BH1745           "bh1745"
#define INVALID_VALUE                  (-1)

enum ExtendedAlsRegGroupType {
    EXTENDED_ALS_TIME_GROUP = 0,
    EXTENDED_ALS_GAIN_GROUP,
    EXTENDED_ALS_GROUP_MAX,
};

enum AlsLightNum {
    ALS_R = 0,
    ALS_G = 1,
    ALS_B = 2,
    ALS_C = 3,
    ALS_LIGHT_NUM = 4,
};

enum AlsLightPart {
    ALS_R_LSB = 0,
    ALS_R_MSB = 1,
    ALS_G_LSB = 2,
    ALS_G_MSB = 3,
    ALS_B_LSB = 4,
    ALS_B_MSB = 5,
    ALS_C_LSB = 6,
    ALS_C_MSB = 7,
    ALS_LIGHT_BUTT = 8,
};

struct AlsReportData {
    int32_t als;
    int32_t cct;
    int32_t irData;
};

struct TimeRegAddrValueMap {
    uint8_t timeRegKey;
    uint32_t timeValue;
};

struct GainRegAddrValueMap {
    uint8_t gainRegKey;
    uint32_t gainValue;
};

struct AlsData {
    int32_t red;
    int32_t green;
    int32_t blue;
    int32_t clear;
};

struct AlsOpsCall {
    int32_t (*Init)(struct SensorCfgData *data);
    int32_t (*ReadData)(struct SensorCfgData *data);
};

struct AlsDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    HdfWorkQueue alsWorkQueue;
    HdfWork alsWork;
    OsalTimer alsTimer;
    bool detectFlag;
    bool enable;
    int64_t interval;
    struct SensorCfgData *alsCfg;
    struct AlsOpsCall ops;
};

int32_t AlsRegisterChipOps(const struct AlsOpsCall *ops);
struct SensorCfgData *AlsCreateCfgData(const struct DeviceResourceNode *node);
void AlsReleaseCfgData(struct SensorCfgData *alsCfg);
int32_t GetTimeByRegValue(uint8_t regValue, struct TimeRegAddrValueMap *map, int32_t itemNum);
int32_t GetRegGroupIndexByTime(uint32_t timeValue, struct TimeRegAddrValueMap *map, int32_t itemNum);
int32_t GetGainByRegValue(uint8_t regValue, struct GainRegAddrValueMap *map, int32_t itemNum);
#endif /* SENSOR_ALS_DRIVER_H */
