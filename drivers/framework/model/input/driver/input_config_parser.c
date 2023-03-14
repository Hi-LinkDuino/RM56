/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "osal_mem.h"
#include "hdf_log.h"
#include "input_config.h"

#define CHECK_PARSER_RET(ret,  str) do { \
    if ((ret) != HDF_SUCCESS) { \
        HDF_LOGE("%s: %s failed, ret = %d", __func__, str, ret); \
        return HDF_FAILURE; \
    } \
} while (0)

#define DEFAULT_I2C_SPEED 400
#define DEFAULT_SPI_SPEED 10000
#define I2C 0
#define SPI 1

int32_t ParseKeyConfig(const struct DeviceResourceNode *node, KeyChipCfg *config)
{
    int32_t ret;
    struct DeviceResourceIface *parser = NULL;
    if (node == NULL || config == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (parser == NULL) {
        HDF_LOGE("%s: instance parser failed", __func__);
        return HDF_FAILURE;
    }

    const struct DeviceResourceNode *keyNode = node;
    ret = parser->GetString(keyNode, "keyName", &config->keyName, NULL);
    CHECK_PARSER_RET(ret, "GetString");
    ret = parser->GetUint8(keyNode, "inputType", &config->devType, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetUint16(keyNode, "gpioNum", &config->gpioNum, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint16(keyNode, "irqFlag", &config->irqFlag, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetUint32(keyNode, "debounceTime", &config->debounceTime, 0);
    CHECK_PARSER_RET(ret, "GetUint32");

    return HDF_SUCCESS;
}

int32_t ParseEncoderConfig(const struct DeviceResourceNode *node, EncoderCfg *config)
{
    if (node == NULL || config == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

    struct DeviceResourceIface *parser = NULL;
    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (parser == NULL) {
        HDF_LOGE("%s: instance parser failed", __func__);
        return HDF_FAILURE;
    }

    const struct DeviceResourceNode *encoderNode = node;
    int32_t ret = parser->GetUint8(encoderNode, "inputType", &config->devType, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetUint16(encoderNode, "gpioClk", &config->gpioClk, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint16(encoderNode, "gpioDT", &config->gpioData, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint16(encoderNode, "gpioSW", &config->gpioSW, 0);
    CHECK_PARSER_RET(ret, "GetUint16");

    return HDF_SUCCESS;
}

static int32_t ParseAttr(struct DeviceResourceIface *parser, const struct DeviceResourceNode *attrNode,
    BoardAttrCfg *attr)
{
    int32_t ret;
    ret = parser->GetUint8(attrNode, "inputType", &attr->devType, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetString(attrNode, "devName", &attr->devName, NULL);
    CHECK_PARSER_RET(ret, "GetString");
    ret = parser->GetUint32(attrNode, "solutionX", &attr->resolutionX, 0);
    CHECK_PARSER_RET(ret, "GetUint32");
    ret = parser->GetUint32(attrNode, "solutionY", &attr->resolutionY, 0);
    CHECK_PARSER_RET(ret, "GetUint32");
    return HDF_SUCCESS;
}

static int32_t ParseBus(struct DeviceResourceIface *parser, const struct DeviceResourceNode *busNode, BoardBusCfg *bus)
{
    int32_t ret;
    ret = parser->GetUint8(busNode, "busType", &bus->busType, 0);
    CHECK_PARSER_RET(ret, "GetUint8");

    if (bus->busType == I2C) {
        ret = parser->GetUint8(busNode, "busNum", &bus->i2c.busNum, 0);
        CHECK_PARSER_RET(ret, "GetUint8");
        ret = parser->GetUint16(busNode, "clkGpio", &bus->i2c.clkGpio, 0);
        CHECK_PARSER_RET(ret, "GetUint16");
        ret = parser->GetUint16(busNode, "dataGpio", &bus->i2c.dataGpio, 0);
        CHECK_PARSER_RET(ret, "GetUint16");
        ret = parser->GetUint32Array(busNode, "i2cClkIomux", bus->i2c.i2cClkReg, REG_CONFIG_LEN, 0);
        CHECK_PARSER_RET(ret, "GetUint32Array");
        ret = parser->GetUint32Array(busNode, "i2cDataIomux", bus->i2c.i2cDataReg, REG_CONFIG_LEN, 0);
        CHECK_PARSER_RET(ret, "GetUint32Array");
    } else if (bus->busType == SPI) {
        ret = parser->GetUint8(busNode, "busNum", &bus->spi.busNum, 0);
        CHECK_PARSER_RET(ret, "GetUint8");
        ret = parser->GetUint16(busNode, "clkGpio", &bus->spi.clkGpio, 0);
        CHECK_PARSER_RET(ret, "GetUint16");
        ret = parser->GetUint16(busNode, "csGpio", &bus->spi.csGpio, 0);
        CHECK_PARSER_RET(ret, "GetUint16");
        ret = parser->GetUint16(busNode, "misoGpio", &bus->spi.misoGpio, 0);
        CHECK_PARSER_RET(ret, "GetUint16");
        ret = parser->GetUint16(busNode, "mosiGpio", &bus->spi.mosiGpio, 0);
        CHECK_PARSER_RET(ret, "GetUint16");
    } else {
        HDF_LOGE("%s: unknown bus type", __func__);
    }
    return HDF_SUCCESS;
}

static int32_t ParsePins(struct DeviceResourceIface *parser, const struct DeviceResourceNode *pinsNode,
    BoardPinCfg *pins)
{
    int32_t ret;
    ret = parser->GetUint16(pinsNode, "rstGpio", &pins->rstGpio, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint16(pinsNode, "intGpio", &pins->intGpio, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint32Array(pinsNode, "rstRegCfg", pins->rstPinReg, REG_CONFIG_LEN, 0);
    CHECK_PARSER_RET(ret, "GetUint32Array");
    ret = parser->GetUint32Array(pinsNode, "intRegCfg", pins->intPinReg, REG_CONFIG_LEN, 0);
    CHECK_PARSER_RET(ret, "GetUint32Array");
    return HDF_SUCCESS;
}

static int32_t ParsePower(struct DeviceResourceIface *parser, const struct DeviceResourceNode *powerNode,
    BoardPwrCfg *power)
{
    int32_t ret;
    ret = parser->GetUint16(powerNode, "vccType", &power->vcc.pwrType, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint16(powerNode, "vccNum", &power->vcc.pwrNum, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint32(powerNode, "vccValue", &power->vcc.pwrValue, 0);
    CHECK_PARSER_RET(ret, "GetUint32");
    ret = parser->GetUint16(powerNode, "vciType", &power->vci.pwrType, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint16(powerNode, "vciNum", &power->vci.pwrNum, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint32(powerNode, "vciValue", &power->vci.pwrValue, 0);
    CHECK_PARSER_RET(ret, "GetUint32");
    return HDF_SUCCESS;
}

static int32_t ParseFeature(struct DeviceResourceIface *parser, const struct DeviceResourceNode *featureNode,
    BoardFeatureCfg *feature)
{
    int32_t ret;
    ret = parser->GetUint8(featureNode, "capacitanceTest", &feature->capacitanceTest, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetUint8(featureNode, "gestureMode", &feature->gestureMode, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetUint8(featureNode, "gloverMOde", &feature->gloverMOde, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetUint8(featureNode, "coverMode", &feature->coverMode, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetUint8(featureNode, "chargerMode", &feature->chargeMode, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    ret = parser->GetUint8(featureNode, "knuckleMode", &feature->knuckleMode, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    return HDF_SUCCESS;
}

int32_t ParseTouchBoardConfig(const struct DeviceResourceNode *node, TouchBoardCfg *config)
{
    int32_t ret;
    struct DeviceResourceIface *parser = NULL;

    if (node == NULL || config == NULL) {
        HDF_LOGE("%s: input param is null", __func__);
        return HDF_FAILURE;
    }
    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (parser == NULL) {
        HDF_LOGE("%s: invalid parser", __func__);
        return HDF_FAILURE;
    }
    config->boardNode = node;
    const struct DeviceResourceNode *attrNode = parser->GetChildNode(node, "inputAttr");
    const struct DeviceResourceNode *busNode = parser->GetChildNode(node, "busConfig");
    const struct DeviceResourceNode *pinsNode = parser->GetChildNode(node, "pinConfig");
    const struct DeviceResourceNode *powerNode = parser->GetChildNode(node, "powerConfig");
    const struct DeviceResourceNode *featureNode = parser->GetChildNode(node, "featureConfig");
    if (attrNode == NULL || busNode == NULL || pinsNode == NULL || powerNode == NULL || featureNode == NULL) {
        HDF_LOGE("%s: get child node fail!", __func__);
        return HDF_FAILURE;
    }

    ret = ParseAttr(parser, attrNode, &config->attr);
    CHECK_PARSER_RET(ret, "ParseAttr");
    ret = ParseBus(parser, busNode, &config->bus);
    CHECK_PARSER_RET(ret, "ParseBus");
    ret = ParsePins(parser, pinsNode, &config->pins);
    CHECK_PARSER_RET(ret, "ParsePins");
    ret = ParsePower(parser, powerNode, &config->power);
    CHECK_PARSER_RET(ret, "ParsePower");
    ret = ParseFeature(parser, featureNode, &config->feature);
    CHECK_PARSER_RET(ret, "ParseFeature");
    return HDF_SUCCESS;
}

static int32_t ParsePowerSequence(struct DeviceResourceIface *parser, const struct DeviceResourceNode *seqNode,
    ChipPwrSeq *pwrSeq)
{
    int32_t ret;
    uint32_t num = (uint32_t)parser->GetElemNum(seqNode, "powerOnSeq");
    pwrSeq->pwrOn.count = num;
    pwrSeq->pwrOn.buf = (uint32_t *)OsalMemAlloc(sizeof(uint32_t) * num);
    if (pwrSeq->pwrOn.buf == NULL) {
        return HDF_FAILURE;
    }
    (void)memset_s(pwrSeq->pwrOn.buf, sizeof(uint32_t) * num, 0, sizeof(uint32_t) * num);
    ret = parser->GetUint32Array(seqNode, "powerOnSeq", pwrSeq->pwrOn.buf, num, 0);
    CHECK_PARSER_RET(ret, "GetUint32Array");

    pwrSeq->suspend.count = parser->GetElemNum(seqNode, "suspendSeq");
    num = pwrSeq->suspend.count;
    pwrSeq->suspend.buf = (uint32_t *)OsalMemAlloc(sizeof(uint32_t) * num);
    if (pwrSeq->suspend.buf == NULL) {
        OsalMemFree(pwrSeq->pwrOn.buf);
        pwrSeq->pwrOn.buf = NULL;
        return HDF_FAILURE;
    }
    (void)memset_s(pwrSeq->suspend.buf, sizeof(uint32_t) * num, 0, sizeof(uint32_t) * num);
    ret = parser->GetUint32Array(seqNode, "suspendSeq", pwrSeq->suspend.buf, num, 0);
    CHECK_PARSER_RET(ret, "GetUint32Array");

    pwrSeq->resume.count = parser->GetElemNum(seqNode, "resumeSeq");
    num = pwrSeq->resume.count;
    pwrSeq->resume.buf = (uint32_t *)OsalMemAlloc(sizeof(uint32_t) * num);
    if (pwrSeq->resume.buf == NULL) {
        OsalMemFree(pwrSeq->pwrOn.buf);
        pwrSeq->pwrOn.buf = NULL;
        OsalMemFree(pwrSeq->suspend.buf);
        pwrSeq->suspend.buf = NULL;
        return HDF_FAILURE;
    }
    (void)memset_s(pwrSeq->resume.buf, sizeof(uint32_t) * num, 0, sizeof(uint32_t) * num);
    ret = parser->GetUint32Array(seqNode, "resumeSeq", pwrSeq->resume.buf, num, 0);
    CHECK_PARSER_RET(ret, "GetUint32Array");

    pwrSeq->pwrOff.count = parser->GetElemNum(seqNode, "powerOffSeq");
    num = pwrSeq->pwrOff.count;
    pwrSeq->pwrOff.buf = (uint32_t *)OsalMemAlloc(sizeof(uint32_t) * num);
    if (pwrSeq->pwrOff.buf == NULL) {
        OsalMemFree(pwrSeq->pwrOn.buf);
        pwrSeq->pwrOn.buf = NULL;
        OsalMemFree(pwrSeq->suspend.buf);
        pwrSeq->suspend.buf = NULL;
        OsalMemFree(pwrSeq->resume.buf);
        pwrSeq->resume.buf = NULL;
        return HDF_FAILURE;
    }
    (void)memset_s(pwrSeq->pwrOff.buf, sizeof(uint32_t) * num, 0, sizeof(uint32_t) * num);
    ret = parser->GetUint32Array(seqNode, "powerOffSeq", pwrSeq->pwrOff.buf, num, 0);
    CHECK_PARSER_RET(ret, "GetUint32Array");

    return HDF_SUCCESS;
}

int32_t ParseTouchChipConfig(const struct DeviceResourceNode *node, TouchChipCfg *config)
{
    int32_t ret;
    struct DeviceResourceIface *parser = NULL;
    if (node == NULL || config == NULL) {
        HDF_LOGE("%s: point is null", __func__);
        return HDF_FAILURE;
    }
    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (parser == NULL) {
        HDF_LOGE("%s: instance parser failed", __func__);
        return HDF_FAILURE;
    }

    config->chipNode = node;
    ret = parser->GetString(node, "chipName", &config->chipName, NULL);
    CHECK_PARSER_RET(ret, "GetString");
    ret = parser->GetString(node, "vendorName", &config->vendorName, NULL);
    CHECK_PARSER_RET(ret, "GetString");
    ret = parser->GetString(node, "chipInfo", &config->chipInfo, NULL);
    CHECK_PARSER_RET(ret, "GetString");
    ret = parser->GetUint16(node, "chipVersion", &config->chipVersion, 0);
    CHECK_PARSER_RET(ret, "GetUint16");
    ret = parser->GetUint8(node, "busType", &config->bus.busType, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    if (config->bus.busType == I2C) {
        ret = parser->GetUint16(node, "irqFlag", &config->bus.chipI2c.irqFlag, 0);
        CHECK_PARSER_RET(ret, "GetUint16");
        ret = parser->GetUint32(node, "deviceAddr", &config->bus.chipI2c.commAddr, 0);
        CHECK_PARSER_RET(ret, "GetUint32");
        ret = parser->GetUint32(node, "maxSpeed", &config->bus.chipI2c.maxSpeed, DEFAULT_I2C_SPEED);
        CHECK_PARSER_RET(ret, "GetUint32");
    } else {
        ret = parser->GetUint16(node, "irqFlag", &config->bus.chipSpi.irqFlag, 0);
        CHECK_PARSER_RET(ret, "GetUint16");
        ret = parser->GetUint8(node, "wordMode", &config->bus.chipSpi.wordMode, 0);
        CHECK_PARSER_RET(ret, "GetUint8");
        ret = parser->GetUint8(node, "commMode", &config->bus.chipSpi.commMode, 0);
        CHECK_PARSER_RET(ret, "GetUint8");
        ret = parser->GetUint32(node, "maxSpeed", &config->bus.chipSpi.maxSpeed, DEFAULT_SPI_SPEED);
        CHECK_PARSER_RET(ret, "GetUint32");
    }
    const struct DeviceResourceNode *pwrSeqNode = parser->GetChildNode(node, "powerSequence");
    if (pwrSeqNode == NULL) {
        HDF_LOGE("%s: get powerSequence child node failed", __func__);
        return HDF_FAILURE;
    }
    if (ParsePowerSequence(parser, pwrSeqNode, &config->pwrSeq) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}