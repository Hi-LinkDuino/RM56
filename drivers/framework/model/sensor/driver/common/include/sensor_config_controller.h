/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_CONFIG_CONTROLLER_H
#define SENSOR_CONFIG_CONTROLLER_H

#include "sensor_config_parser.h"

#define LENGTH_NUMBER    32

enum SensorCalculateType {
    SENSOR_CFG_CALC_TYPE_NONE   = 0,
    SENSOR_CFG_CALC_TYPE_SET    = 1,      // To Support:1.Bitwise AND "&"  2.Bitwise OR "|"
    SENSOR_CFG_CALC_TYPE_REVERT = 2,      // To Support:1.Bitwise REVERT '~'
    SENSOR_CFG_CALC_TYPE_XOR    = 3,      // To Support:1.Bitwise XOR "^"
    SENSOR_CFG_CALC_TYPE_LEFT_SHIFT  = 4, // To Support:1.Left shift <<
    SENSOR_CFG_CALC_TYPE_RIGHT_SHIFT = 5, // To Support:1.Right shift >>
};

enum SensorOpsType {
    SENSOR_OPS_TYPE_NOP    = 0,
    SENSOR_OPS_TYPE_READ   = 1,
    SENSOR_OPS_TYPE_WRITE  = 2,
    SENSOR_OPS_TYPE_READ_CHECK     = 3,
    SENSOR_OPS_TYPE_UPDATE_BITWISE = 4,
    SENSOR_OPS_TYPE_EXTBUFF_READ   = 5,
    SENSOR_OPS_TYPE_EXTBUFF_WRITE  = 6,
};

struct SensorOpsCall {
    enum SensorOpsType type;
    int32_t (*ops)(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem);
};

int32_t SetSensorRegCfgArray(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group);
int32_t SetSensorRegCfgArrayByBuff(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group,
    uint8_t *buff, int16_t len);
int32_t ReadSensorRegCfgArray(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group,
    int32_t index, uint8_t *buf, int32_t len);
int32_t WriteSensorRegCfgArray(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group,
    int32_t index, int32_t len);
#endif /* SENSOR_CONFIG_CONTROLLER_H */