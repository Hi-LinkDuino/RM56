/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_config_parser.h"
#include <securec.h>
#include "device_resource_if.h"
#include "osal_mem.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_commom

static char *g_sensorRegGroupName[SENSOR_GROUP_MAX] = {
    "initSeqConfig",
    "enableSeqConfig",
    "disableSeqConfig",
};

static uint32_t GetSensorRegGroupNameIndex(const char *name)
{
    uint32_t index;

    if (name == NULL) {
        return SENSOR_GROUP_MAX;
    }

    for (index = 0; index < SENSOR_GROUP_MAX; ++index) {
        if ((g_sensorRegGroupName[index] != NULL) && (strcmp(name, g_sensorRegGroupName[index]) == 0)) {
            break;
        }
    }

    return index;
}

void ReleaseSensorAllRegConfig(struct SensorCfgData *config)
{
    int32_t index;

    if (config == NULL || config->regCfgGroup == NULL) {
        return;
    }

    for (index = 0; index < SENSOR_GROUP_MAX; ++index) {
        if (config->regCfgGroup[index] != NULL) {
            if (config->regCfgGroup[index]->regCfgItem != NULL) {
                OsalMemFree(config->regCfgGroup[index]->regCfgItem);
                config->regCfgGroup[index]->regCfgItem = NULL;
            }
            OsalMemFree(config->regCfgGroup[index]);
            config->regCfgGroup[index] = NULL;
        }
    }
}

static int32_t ParseSensorRegItem(struct DeviceResourceIface *parser, const struct DeviceResourceNode *regNode,
    const char *groupName, struct SensorRegCfgGroupNode *group)
{
    int32_t ret;
    int32_t step;
    uint32_t index;
    int32_t num;
    uint32_t itemNum = group->itemNum;
    uint16_t *buf = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(group->regCfgItem, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(groupName, HDF_ERR_INVALID_PARAM);

    num = parser->GetElemNum(regNode, groupName);
    if (num <= 0 || num > SENSOR_CONFIG_MAX_ITEM) {
        HDF_LOGE("%s: parser %s element num failed", __func__, groupName);
        return HDF_SUCCESS;
    }

    buf = (uint16_t *)OsalMemCalloc(sizeof(uint16_t) * num);
    CHECK_NULL_PTR_RETURN_VALUE(buf, HDF_ERR_MALLOC_FAIL);

    ret = parser->GetUint16Array(regNode, groupName, buf, num, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: parser %s reg array failed", __func__, groupName);
        OsalMemFree(buf);
        return HDF_SUCCESS;
    }

    for (index = 0; index < itemNum; ++index) {
        step = SENSOR_REG_CFG_INDEX_MAX * index;
        if (step + SENSOR_REG_CFG_SAVE_INDEX >= num) {
            break;
        }
        group->regCfgItem[index].regAddr = buf[step + SENSOR_REG_CFG_ADDR_INDEX];
        group->regCfgItem[index].value = buf[step + SENSOR_REG_CFG_VALUE_INDEX];
        group->regCfgItem[index].mask = buf[step + SENSOR_REG_CFG_MASK_INDEX];
        group->regCfgItem[index].len = buf[step + SENSOR_REG_CFG_LEN_INDEX];
        group->regCfgItem[index].delay = buf[step + SENSOR_REG_CFG_DELAY_INDEX];
        group->regCfgItem[index].opsType = buf[step + SENSOR_REG_CFG_OPS_INDEX];
        group->regCfgItem[index].calType = buf[step + SENSOR_REG_CFG_CAL_INDEX];
        group->regCfgItem[index].shiftNum = buf[step + SENSOR_REG_CFG_SHIFT_INDEX];
        group->regCfgItem[index].debug = buf[step + SENSOR_REG_CFG_DEBUG_INDEX];
        group->regCfgItem[index].save = buf[step + SENSOR_REG_CFG_SAVE_INDEX];
    }
    OsalMemFree(buf);

    return HDF_SUCCESS;
}

int32_t ParseSensorRegGroup(struct DeviceResourceIface *parser, const struct DeviceResourceNode *regCfgNode,
    const char *groupName, struct SensorRegCfgGroupNode **groupNode)
{
    int32_t num;
    struct SensorRegCfgGroupNode *group = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(parser, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(regCfgNode, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(groupName, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(groupNode, HDF_ERR_INVALID_PARAM);

    num = parser->GetElemNum(regCfgNode, groupName);
    group = *groupNode;

    if (num > 0) {
        if (group != NULL) {
            if (group->regCfgItem != NULL) {
                OsalMemFree(group->regCfgItem);
            }
            OsalMemFree(group);
        }

        group = (struct SensorRegCfgGroupNode*)OsalMemCalloc(sizeof(*group));
        if (group == NULL) {
            HDF_LOGE("%s: malloc sensor reg config group failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        *groupNode = group;
        group->itemNum = (uint32_t)(num / SENSOR_REG_CFG_INDEX_MAX);
        group->itemNum = ((SENSOR_REG_CFG_INDEX_MAX * group->itemNum) < (uint32_t)num) ?
            (group->itemNum + 1) : group->itemNum;

        group->regCfgItem = (struct SensorRegCfg*)OsalMemCalloc(group->itemNum * sizeof(*(group->regCfgItem)));
        if (group->regCfgItem == NULL) {
            HDF_LOGE("%s: malloc sensor reg config item failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        if (ParseSensorRegItem(parser, regCfgNode, groupName, group) != HDF_SUCCESS) {
            HDF_LOGE("%s: malloc sensor reg config item data failed", __func__);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

int32_t ParseSensorRegConfig(struct SensorCfgData *config)
{
    uint32_t index;
    const struct DeviceResourceNode *regCfgNode = NULL;
    struct DeviceResourceIface *parser = NULL;
    const struct DeviceResourceAttr *regAttr = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(config->root, HDF_ERR_INVALID_PARAM);
    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    CHECK_NULL_PTR_RETURN_VALUE(parser, HDF_ERR_INVALID_PARAM);

    regCfgNode = parser->GetChildNode(config->root, "sensorRegConfig");
    CHECK_NULL_PTR_RETURN_VALUE(regCfgNode, HDF_ERR_INVALID_PARAM);

    DEV_RES_NODE_FOR_EACH_ATTR(regCfgNode, regAttr) {
        if (regAttr == NULL || regAttr->name == NULL) {
            HDF_LOGE("%s:sensor reg node attr is null", __func__);
            break;
        }

        index = GetSensorRegGroupNameIndex(regAttr->name);
        if (index >= SENSOR_GROUP_MAX) {
            HDF_LOGE("%s: get sensor register group index failed", __func__);
            goto error;
        }

        if (ParseSensorRegGroup(parser, regCfgNode, regAttr->name, &config->regCfgGroup[index]) != HDF_SUCCESS) {
            HDF_LOGE("%s: parse sensor register group failed", __func__);
            goto error;
        }
    }
    return HDF_SUCCESS;

error:
    ReleaseSensorAllRegConfig(config);
    HDF_LOGE("%s: parse sensor reg config failed", __func__);
    return HDF_FAILURE;
}

int32_t GetSensorBusHandle(struct SensorBusCfg *busCfg)
{
    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_ERR_INVALID_PARAM);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        uint16_t busNum = busCfg->i2cCfg.busNum;
        busCfg->i2cCfg.handle = I2cOpen(busNum);
        if (busCfg->i2cCfg.handle == NULL) {
            HDF_LOGE("%s: sensor i2c Handle invalid", __func__);
            return HDF_FAILURE;
        }

#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SPI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SPI)
    } else if (busCfg->busType == SENSOR_BUS_SPI) {
        struct SpiDevInfo spiDevinfo;
        struct SpiCfg cfg;
        int32_t ret;

        spiDevinfo.busNum = busCfg->spiCfg.busNum;
        spiDevinfo.csNum = busCfg->spiCfg.csNum;
        busCfg->i2cCfg.handle = SpiOpen(&spiDevinfo);

        cfg.mode = SPI_CLK_PHASE | SPI_CLK_POLARITY;
        cfg.bitsPerWord = SENSOR_DATA_WIDTH_8_BIT;
        cfg.maxSpeedHz = SENSOR_SPI_MAX_SPEED;
        ret = SpiSetCfg(busCfg->i2cCfg.handle, &cfg);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: SpiSetCfg failed", __func__);
            SpiClose(busCfg->i2cCfg.handle);
            return ret;
        }
#endif
    }

    return HDF_SUCCESS;
}

int32_t ReleaseSensorBusHandle(struct SensorBusCfg *busCfg)
{
    if (busCfg == NULL) {
        return HDF_SUCCESS;
    }

    if (busCfg->busType == SENSOR_BUS_I2C && busCfg->i2cCfg.handle != NULL) {
        I2cClose(busCfg->i2cCfg.handle);
        busCfg->i2cCfg.handle = NULL;

#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SPI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SPI)
    } else if (busCfg->busType == SENSOR_BUS_SPI) {
        SpiClose(busCfg->spiCfg.handle);
        busCfg->spiCfg.handle = NULL;
#endif
    }

    return HDF_SUCCESS;
}

int32_t DetectSensorDevice(struct SensorCfgData *config)
{
    uint8_t value = 0;
    uint16_t chipIdReg;
    uint16_t chipIdValue;
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    chipIdReg = config->sensorAttr.chipIdReg;
    chipIdValue = config->sensorAttr.chipIdValue;

    ret = GetSensorBusHandle(&config->busCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get sensor bus handle failed", __func__);
        (void)ReleaseSensorBusHandle(&config->busCfg);
        return HDF_FAILURE;
    }

    ret = ReadSensor(&config->busCfg, chipIdReg, &value, sizeof(value));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: i2c read chip id failed", __func__);
        (void)ReleaseSensorBusHandle(&config->busCfg);
        return HDF_FAILURE;
    }

    if (value != chipIdValue) {
        HDF_LOGE("%s: sensor chip[0x%x] id [0x%x] detect value[%hhu]", __func__, chipIdReg, chipIdValue, value);
        (void)ReleaseSensorBusHandle(&config->busCfg);
        return HDF_FAILURE;
    }

    HDF_LOGD("%s: sensor [%s] detect chip success", __func__, config->sensorInfo.sensorName);
    return HDF_SUCCESS;
}

static int32_t ParseSensorInfo(struct DeviceResourceIface *parser, const struct DeviceResourceNode *infoNode,
    struct SensorCfgData *config)
{
    int32_t ret;
    uint16_t id;
    int32_t value;
    const char *name = NULL;

    ret = parser->GetString(infoNode, "sensorName", &name, NULL);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "sensorName");
    if (strcpy_s(config->sensorInfo.sensorName, SENSOR_INFO_NAME_MAX_LEN, name) != EOK) {
        HDF_LOGE("%s:copy sensorName failed!", __func__);
        return HDF_FAILURE;
    }

    ret = parser->GetString(infoNode, "vendorName", &name, NULL);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "vendorName");
    if (strcpy_s(config->sensorInfo.vendorName, SENSOR_INFO_NAME_MAX_LEN, name) != EOK) {
        HDF_LOGE("%s:copy vendorName failed!", __func__);
        return HDF_FAILURE;
    }

    ret = parser->GetString(infoNode, "firmwareVersion", &name, NULL);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "firmwareVersion");
    if (strcpy_s(config->sensorInfo.firmwareVersion, SENSOR_INFO_VERSION_MAX_LEN, name) != EOK) {
        HDF_LOGE("%s:copy firmwareVersion failed!", __func__);
        return HDF_FAILURE;
    }

    ret = parser->GetString(infoNode, "hardwareVersion", &name, NULL);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "hardwareVersion");
    if (strcpy_s(config->sensorInfo.hardwareVersion, SENSOR_INFO_VERSION_MAX_LEN, name) != EOK) {
        HDF_LOGE("%s:copy hardwareVersion failed!", __func__);
        return HDF_FAILURE;
    }

    ret = parser->GetUint16(infoNode, "sensorTypeId", &id, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "sensorTypeId");
    config->sensorInfo.sensorTypeId = id;
    ret = parser->GetUint16(infoNode, "sensorId", &id, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "sensorId");
    config->sensorInfo.sensorId = id;

    ret = parser->GetUint32(infoNode, "maxRange", (uint32_t *)&value, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "maxRange");
    config->sensorInfo.maxRange = value;
    ret = parser->GetUint32(infoNode, "accuracy", (uint32_t *)&value, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "accuracy");
    config->sensorInfo.accuracy = value;
    ret = parser->GetUint32(infoNode, "power", (uint32_t *)&value, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "power");
    config->sensorInfo.power = value;

    return ret;
}

static int32_t ParseSensorBus(struct DeviceResourceIface *parser, const struct DeviceResourceNode *busNode,
    struct SensorCfgData *config)
{
    int32_t ret;

    ret = parser->GetUint8(busNode, "busType", &config->busCfg.busType, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "busType");
    ret = parser->GetUint8(busNode, "regBigEndian", &config->busCfg.regBigEndian, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "regBigEndian");

    if (config->busCfg.busType == SENSOR_BUS_I2C) {
        ret = parser->GetUint16(busNode, "busNum", &config->busCfg.i2cCfg.busNum, 0);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "busNum");
        ret = parser->GetUint16(busNode, "busAddr", &config->busCfg.i2cCfg.devAddr, 0);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "busAddr");
        ret = parser->GetUint16(busNode, "regWidth", &config->busCfg.i2cCfg.regWidth, 0);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "regWidth");
    } else if (config->busCfg.busType == SENSOR_BUS_SPI) {
        ret = parser->GetUint32(busNode, "busNum", &config->busCfg.spiCfg.busNum, 0);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "busNum");
        ret = parser->GetUint32(busNode, "busAddr", &config->busCfg.spiCfg.csNum, 0);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "busAddr");
    } else if (config->busCfg.busType == SENSOR_BUS_GPIO) {
        ret = parser->GetUint32(busNode, "gpioIrq1", &config->busCfg.GpioNum[SENSOR_GPIO_NUM1], 0);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "gpioIrq1");
        ret = parser->GetUint32(busNode, "gpioIrq2", &config->busCfg.GpioNum[SENSOR_GPIO_NUM2], 0);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "gpioIrq2");
    }

    return HDF_SUCCESS;
}

static int32_t ParseSensorAttr(struct DeviceResourceIface *parser, const struct DeviceResourceNode *attrNode,
    struct SensorCfgData *config)
{
    int32_t ret;
    ret = parser->GetString(attrNode, "chipName", &config->sensorAttr.chipName, NULL);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "chipName");
    ret = parser->GetUint16(attrNode, "chipIdRegister", &config->sensorAttr.chipIdReg, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "chipIdRegister");
    ret = parser->GetUint16(attrNode, "chipIdValue", &config->sensorAttr.chipIdValue, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "chipIdValue");

    return ret;
}

void ReleaseSensorDirectionConfig(struct SensorCfgData *config)
{
    CHECK_NULL_PTR_RETURN(config);

    if (config->direction != NULL) {
        OsalMemFree(config->direction);
        config->direction = NULL;
    }
}

int32_t ParseSensorDirection(struct SensorCfgData *config)
{
    int32_t num;
    int32_t ret;
    uint32_t index;
    uint32_t *buf = NULL;
    const struct DeviceResourceNode *directionNode = NULL;
    struct DeviceResourceIface *parser = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(config->root, HDF_ERR_INVALID_PARAM);
    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    CHECK_NULL_PTR_RETURN_VALUE(parser, HDF_ERR_INVALID_PARAM);

    directionNode = parser->GetChildNode(config->root, "sensorDirection");
    CHECK_NULL_PTR_RETURN_VALUE(directionNode, HDF_ERR_INVALID_PARAM);

    num = parser->GetElemNum(directionNode, "convert");
    ret = parser->GetUint32(directionNode, "direction", &index, 0);
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "direction");
    if ((num <= 0 || num > MAX_SENSOR_INDEX_NUM) || (index > num / AXIS_INDEX_MAX)) {
        HDF_LOGE("%s: parser %d element num failed", __func__, num);
        return HDF_FAILURE;
    }

    buf = (uint32_t *)OsalMemCalloc(sizeof(uint32_t) * num);
    CHECK_NULL_PTR_RETURN_VALUE(buf, HDF_ERR_MALLOC_FAIL);

    ret = parser->GetUint32Array(directionNode, "convert", buf, num, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: parser %s convert failed", __func__, "convert");
        OsalMemFree(buf);
        return HDF_FAILURE;
    }

    config->direction = (struct SensorDirection*)OsalMemCalloc(sizeof(struct SensorDirection));
    if (config->direction == NULL) {
        HDF_LOGE("%s: malloc sensor direction config item failed", __func__);
        OsalMemFree(buf);
        return HDF_ERR_MALLOC_FAIL;
    }

    index = index * AXIS_INDEX_MAX;
    config->direction->sign[AXIS_X] = buf[index + SIGN_X_INDEX];
    config->direction->sign[AXIS_Y] = buf[index + SIGN_Y_INDEX];
    config->direction->sign[AXIS_Z] = buf[index + SIGN_Z_INDEX];
    config->direction->map[AXIS_X] = buf[index + AXIS_X_INDEX];
    config->direction->map[AXIS_Y] = buf[index + AXIS_Y_INDEX];
    config->direction->map[AXIS_Z] = buf[index + AXIS_Z_INDEX];

    OsalMemFree(buf);
    return HDF_SUCCESS;
}

int32_t SensorRawDataToRemapData(struct SensorDirection *direction, int32_t *remapData, uint32_t num)
{
    uint32_t axis;
    int32_t directionSign[MAX_SENSOR_AXIS_NUM];
    int32_t newData[MAX_SENSOR_AXIS_NUM];

    CHECK_NULL_PTR_RETURN_VALUE(direction, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(remapData, HDF_ERR_INVALID_PARAM);
    if (num != MAX_SENSOR_AXIS_NUM) {
        HDF_LOGE("%s: afferent num failed", __func__);
        return HDF_FAILURE;
    }

    for (axis = 0; axis < num; axis++) {
        if (direction->sign[axis] == 0) {
            directionSign[axis] = 1;
        } else {
            directionSign[axis] = -1;
        }
    }

    newData[direction->map[AXIS_X]] = directionSign[AXIS_X] * remapData[AXIS_X];
    newData[direction->map[AXIS_Y]] = directionSign[AXIS_Y] * remapData[AXIS_Y];
    newData[direction->map[AXIS_Z]] = directionSign[AXIS_Z] * remapData[AXIS_Z];

    remapData[AXIS_X] = newData[direction->map[AXIS_X]];
    remapData[AXIS_Y] = newData[direction->map[AXIS_Y]];
    remapData[AXIS_Z] = newData[direction->map[AXIS_Z]];

    return HDF_SUCCESS;
}

int32_t GetSensorBaseConfigData(const struct DeviceResourceNode *node, struct SensorCfgData *config)
{
    int32_t ret;
    struct DeviceResourceIface *parser = NULL;
    const struct DeviceResourceNode *infoNode = NULL;
    const struct DeviceResourceNode *busNode = NULL;
    const struct DeviceResourceNode *attrNode = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    CHECK_NULL_PTR_RETURN_VALUE(parser, HDF_ERR_INVALID_PARAM);

    config->root = node;
    CHECK_NULL_PTR_RETURN_VALUE(parser->GetChildNode, HDF_ERR_INVALID_PARAM);

    infoNode = parser->GetChildNode(node, "sensorInfo");
    if (infoNode != NULL) {
        ret = ParseSensorInfo(parser, infoNode, config);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "sensorInfo");
    }

    busNode = parser->GetChildNode(node, "sensorBusConfig");
    if (busNode != NULL) {
        ret = ParseSensorBus(parser, busNode, config);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "sensorBusConfig");
    }

    attrNode = parser->GetChildNode(node, "sensorIdAttr");
    if (attrNode != NULL) {
        ret = ParseSensorAttr(parser, attrNode, config);
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "sensorIdAttr");
    }

    return HDF_SUCCESS;
}
