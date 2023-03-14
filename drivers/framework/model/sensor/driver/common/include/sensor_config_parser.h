/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_CONFIG_PARSER_H
#define SENSOR_CONFIG_PARSER_H

#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "i2c_if.h"
#include "sensor_device_type.h"
#include "sensor_platform_if.h"
#include "spi_if.h"

#define SENSOR_CONFIG_MAX_ITEM 100
#define MAX_SENSOR_INDEX_NUM   48
#define MAX_SENSOR_AXIS_NUM    3

enum SensorRegOpsType {
    SENSOR_INIT_GROUP = 0,
    SENSOR_ENABLE_GROUP,
    SENSOR_DISABLE_GROUP,
    SENSOR_GROUP_MAX,
};

struct SensorAttr {
    const char *chipName;
    uint16_t chipIdReg;
    uint16_t chipIdValue;
};

enum SensorRegCfgIndex {
    SENSOR_REG_CFG_ADDR_INDEX = 0,
    SENSOR_REG_CFG_VALUE_INDEX,
    SENSOR_REG_CFG_MASK_INDEX,
    SENSOR_REG_CFG_LEN_INDEX,
    SENSOR_REG_CFG_DELAY_INDEX,
    SENSOR_REG_CFG_OPS_INDEX,
    SENSOR_REG_CFG_CAL_INDEX,
    SENSOR_REG_CFG_SHIFT_INDEX,
    SENSOR_REG_CFG_DEBUG_INDEX,
    SENSOR_REG_CFG_SAVE_INDEX,
    SENSOR_REG_CFG_INDEX_MAX,
};

struct SensorRegCfg {
    uint16_t regAddr;
    union {
        uint16_t value;
        uint8_t *buff;
    };
    uint16_t mask;
    uint16_t len;
    uint32_t delay : 12;
    uint32_t opsType : 5;
    uint32_t calType : 3;
    uint32_t shiftNum : 4;
    uint32_t debug : 1;
    uint32_t save : 2;
};

struct SensorRegCfgGroupNode {
    uint32_t itemNum;
    struct SensorRegCfg *regCfgItem;
};

struct SensorDirection {
    uint32_t sign[MAX_SENSOR_AXIS_NUM];
    uint32_t map[MAX_SENSOR_AXIS_NUM];
};

enum SensorDirectionIndex {
    SIGN_X_INDEX = 0,
    SIGN_Y_INDEX,
    SIGN_Z_INDEX,
    AXIS_X_INDEX,
    AXIS_Y_INDEX,
    AXIS_Z_INDEX,
    AXIS_INDEX_MAX,
};

enum AxisNum {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2,
};

struct SensorCfgData {
    struct SensorBusCfg busCfg;
    struct SensorBasicInfo sensorInfo;
    struct SensorAttr sensorAttr;
    struct SensorRegCfgGroupNode **regCfgGroup;
    struct SensorRegCfgGroupNode **extendedRegCfgGroup;
    const struct DeviceResourceNode *root;
    struct SensorDirection *direction;
};

int32_t GetSensorBaseConfigData(const struct DeviceResourceNode *node, struct SensorCfgData *config);
int32_t ParseSensorRegConfig(struct SensorCfgData *config);
void ReleaseSensorAllRegConfig(struct SensorCfgData *config);
int32_t GetSensorBusHandle(struct SensorBusCfg *busCfg);
int32_t ReleaseSensorBusHandle(struct SensorBusCfg *busCfg);
int32_t DetectSensorDevice(struct SensorCfgData *config);
int32_t SensorRawDataToRemapData(struct SensorDirection *direction, int32_t *remapData, uint32_t num);
void ReleaseSensorDirectionConfig(struct SensorCfgData *config);
int32_t ParseSensorDirection(struct SensorCfgData *config);
int32_t ParseSensorRegGroup(struct DeviceResourceIface *parser, const struct DeviceResourceNode *regCfgNode,
    const char *groupName, struct SensorRegCfgGroupNode **groupNode);

#endif /* SENSOR_CONFIG_PARSER_H */
