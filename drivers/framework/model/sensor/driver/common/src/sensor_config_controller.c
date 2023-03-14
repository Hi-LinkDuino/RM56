/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_config_controller.h"
#include <securec.h>
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_commom

static int32_t SensorOpsNop(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    (void)busCfg;
    (void)cfgItem;
    return HDF_SUCCESS;
}

static int32_t SensorOpsRead(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    uint16_t value = 0;
    int32_t ret;

    ret = ReadSensor(busCfg, cfgItem->regAddr, (uint8_t *)&value, sizeof(value));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read i2c reg");

    return value;
}

static uint32_t GetSensorRegRealValueMask(struct SensorRegCfg *cfgItem, uint32_t *value, uint32_t busMask)
{
    uint32_t mask = cfgItem->mask & busMask;
    *value = cfgItem->value & busMask;

    if (cfgItem->shiftNum != 0) {
        if (cfgItem->calType == SENSOR_CFG_CALC_TYPE_RIGHT_SHIFT) {
            *value = *value >> cfgItem->shiftNum;
            mask = mask >> cfgItem->shiftNum;
        } else {
            *value = *value << cfgItem->shiftNum;
            mask = mask << cfgItem->shiftNum;
        }
    }

    return mask;
}

static int32_t SensorOpsWrite(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    uint8_t value[SENSOR_VALUE_BUTT];
    int32_t ret;
    uint32_t originValue;
    uint32_t busMask;
    uint32_t mask;

    busMask = (busCfg->i2cCfg.regWidth == SENSOR_ADDR_WIDTH_1_BYTE) ? 0x00ff : 0xffff;
    mask = GetSensorRegRealValueMask(cfgItem, &originValue, busMask);

    value[SENSOR_ADDR_INDEX] = cfgItem->regAddr;
    value[SENSOR_VALUE_INDEX] = originValue & mask;

    ret = WriteSensor(busCfg, value, sizeof(value));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "write i2c reg");

    return ret;
}

static int32_t SensorOpsReadCheck(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    uint32_t value = 0;
    uint32_t originValue;
    uint32_t mask;
    uint32_t busMask = 0xffff;
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        ret = ReadSensor(busCfg, cfgItem->regAddr, (uint8_t *)&value, sizeof(value));
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read i2c reg");
        busMask = (busCfg->i2cCfg.regWidth == SENSOR_ADDR_WIDTH_1_BYTE) ? 0x00ff : 0xffff;
    }

    mask = GetSensorRegRealValueMask(cfgItem, &originValue, busMask);
    if ((value & mask) != (originValue & mask)) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SensorBitwiseCalculate(struct SensorRegCfg *cfgItem, uint32_t *value, uint32_t valueMask)
{
    uint32_t originValue;
    uint32_t mask;
    uint32_t tmp;

    mask = GetSensorRegRealValueMask(cfgItem, &originValue, valueMask);
    switch ((enum SensorCalculateType)cfgItem->calType) {
        case SENSOR_CFG_CALC_TYPE_NONE:
            break;
        case SENSOR_CFG_CALC_TYPE_SET:
            *value &= ~mask;
            *value |= (originValue & mask);
            break;
        case SENSOR_CFG_CALC_TYPE_REVERT:
            tmp = *value & (~mask);
            *value = ~(*value & mask);
            *value = tmp | (*value & mask);
            break;
        case SENSOR_CFG_CALC_TYPE_XOR:
            tmp = *value & (~mask);
            originValue = originValue & mask;
            *value = *value & mask;
            *value ^= originValue;
            *value = tmp | (*value);
            break;
        default:
            HDF_LOGE("%s: unsupported cal type", __func__);
            break;
    }

    return 0;
}

static int32_t SensorOpsUpdateBitwise(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    uint32_t value = 0;
    uint32_t busMask = 0x000000ff;
    int32_t ret;
    uint8_t valueArray[SENSOR_VALUE_BUTT];

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        ret = ReadSensor(busCfg, cfgItem->regAddr, (uint8_t *)&value, busCfg->i2cCfg.regWidth);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read i2c reg");
        busMask = (busCfg->i2cCfg.regWidth == SENSOR_ADDR_WIDTH_1_BYTE) ? 0x000000ff : 0x0000ffff;
    }

    ret = SensorBitwiseCalculate(cfgItem, &value, busMask);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "update bitwise failed");

    valueArray[SENSOR_ADDR_INDEX] = cfgItem->regAddr;
    valueArray[SENSOR_VALUE_INDEX] = (uint8_t)value;

    ret = WriteSensor(busCfg, valueArray, sizeof(valueArray));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "update bitewise write failed");

    return HDF_SUCCESS;
}

static int32_t SensorOpsExtBuffRead(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(cfgItem, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(cfgItem->buff, HDF_FAILURE);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        ret = ReadSensor(busCfg, cfgItem->regAddr, cfgItem->buff, cfgItem->len);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read i2c reg");
    }

    return HDF_SUCCESS;
}

static int32_t SensorOpsExtBuffWrite(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    int32_t ret;
    uint8_t *value = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(cfgItem, HDF_FAILURE);

    if (cfgItem->len > LENGTH_NUMBER) {
        return HDF_FAILURE;
    }
    value = (uint8_t *)OsalMemCalloc(sizeof(uint8_t) * (cfgItem->len + 1));
    CHECK_NULL_PTR_RETURN_VALUE(value, HDF_FAILURE);

    value[SENSOR_ADDR_INDEX] = cfgItem->regAddr;
    if (memcpy_s(&value[SENSOR_VALUE_INDEX], cfgItem->len, cfgItem->buff, cfgItem->len) != EOK) {
        HDF_LOGE("%s: Cpy value failed", __func__);
        OsalMemFree(value);
        return HDF_FAILURE;
    }

    if (busCfg->busType == SENSOR_BUS_I2C) {
        ret = WriteSensor(busCfg, value, cfgItem->len);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: write ext register failed", __func__);
            OsalMemFree(value);
            return HDF_FAILURE;
        }
    }
    OsalMemFree(value);
    return HDF_SUCCESS;
}

static struct SensorOpsCall g_doOpsCall[] = {
    { SENSOR_OPS_TYPE_NOP,                         SensorOpsNop },
    { SENSOR_OPS_TYPE_READ,                        SensorOpsRead },
    { SENSOR_OPS_TYPE_WRITE,                       SensorOpsWrite },
    { SENSOR_OPS_TYPE_READ_CHECK,                  SensorOpsReadCheck },
    { SENSOR_OPS_TYPE_UPDATE_BITWISE,              SensorOpsUpdateBitwise },
    { SENSOR_OPS_TYPE_EXTBUFF_READ,                SensorOpsExtBuffRead },
    { SENSOR_OPS_TYPE_EXTBUFF_WRITE,               SensorOpsExtBuffWrite },
};

int32_t SetSensorRegCfgArray(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group)
{
    int32_t num = 0;
    uint32_t count;
    struct SensorRegCfg *cfgItem = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);

    if (group == NULL) {
        HDF_LOGI("%s: Pointer group is null", __func__);
        return HDF_SUCCESS;
    }

    CHECK_NULL_PTR_RETURN_VALUE(group->regCfgItem, HDF_FAILURE);

    count = sizeof(g_doOpsCall) / sizeof(g_doOpsCall[0]);

    while (num < group->itemNum) {
        cfgItem = (group->regCfgItem + num);
        if (cfgItem->opsType >= count) {
            HDF_LOGE("%s: cfg item para invalid", __func__);
            break;
        }
        if ((g_doOpsCall[cfgItem->opsType].ops != NULL) && (cfgItem->opsType <= SENSOR_OPS_TYPE_UPDATE_BITWISE)) {
            if (g_doOpsCall[cfgItem->opsType].ops(busCfg, cfgItem) != HDF_SUCCESS) {
                HDF_LOGE("%s: malloc sensor reg config item data failed", __func__);
                return HDF_FAILURE;
            }
        }
        if (cfgItem->delay != 0) {
            OsalMDelay(cfgItem->delay);
        }
        num++;
    }

    return HDF_SUCCESS;
}

int32_t SetSensorRegCfgArrayByBuff(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group,
    uint8_t *buff, int16_t len)
{
    int32_t num = 0;
    uint32_t count;
    uint8_t buffOffset = 0;
    struct SensorRegCfg *cfgItem = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(group, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(group->regCfgItem, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(buff, HDF_FAILURE);

    count = sizeof(g_doOpsCall) / sizeof(g_doOpsCall[0]);

    while (num < group->itemNum) {
        cfgItem = (group->regCfgItem + num);
        if (cfgItem->opsType >= count) {
            HDF_LOGE("%s: cfg item para invalid", __func__);
            return HDF_FAILURE;
        }

        if ((g_doOpsCall[cfgItem->opsType].ops != NULL) && (cfgItem->opsType >= SENSOR_OPS_TYPE_EXTBUFF_READ)) {
            cfgItem->buff = buff + buffOffset;
            len -= (int16_t)cfgItem->len;
            if (len < 0) {
                HDF_LOGE("%s: cfg item para invalid", __func__);
                return HDF_FAILURE;
            }
            if (g_doOpsCall[cfgItem->opsType].ops(busCfg, cfgItem) != HDF_SUCCESS) {
                HDF_LOGE("%s: extbuff is read and write failed", __func__);
                return HDF_FAILURE;
            }
            buffOffset += cfgItem->len;
            if (cfgItem->delay != 0) {
                OsalMDelay(cfgItem->delay);
            }
        }

        num++;
    }

    return HDF_SUCCESS;
}

int32_t ReadSensorRegCfgArray(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group,
    int32_t index, uint8_t *buf, int32_t len)
{
    int32_t ret;
    struct SensorRegCfg *cfgItem = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(group, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(group->regCfgItem, HDF_FAILURE);

    if ((index >= group->itemNum) || index < 0) {
        HDF_LOGE("%s: Index is invalid parameter", __func__);
        return HDF_FAILURE;
    }

    cfgItem = group->regCfgItem + index;
    len = (cfgItem->len > len) ? len : cfgItem->len;

    ret = ReadSensor(busCfg, cfgItem->regAddr, buf, len);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read i2c reg");

    return HDF_SUCCESS;
}

int32_t WriteSensorRegCfgArray(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group,
    int32_t index, int32_t len)
{
    struct SensorRegCfg *cfgItem = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(group, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(group->regCfgItem, HDF_FAILURE);

    if ((index >= group->itemNum) || index < 0) {
        HDF_LOGE("%s: Index is invalid parameter", __func__);
        return HDF_FAILURE;
    }

    cfgItem = group->regCfgItem + index;

    int32_t ret = SensorOpsUpdateBitwise(busCfg, cfgItem);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "Write i2c reg");

    return HDF_SUCCESS;
}