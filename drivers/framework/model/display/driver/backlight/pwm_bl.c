/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_bl.h"
#include "hdf_disp.h"
#include "hdf_log.h"
#include "hdf_types.h"
#include "osal.h"
#include "pwm_if.h"

#define DEF_MIN_BRIGHTNESS     0
#define DEF_MAX_BRIGHTNESS     255

#define CHECK_PARSER_RET(ret,  str) do { \
    if ((ret) != HDF_SUCCESS) { \
        HDF_LOGE("%s: %s failed, ret = %d", __func__, str, ret); \
        return HDF_FAILURE; \
    } \
} while (0)

struct BlPwmDev {
    const char *name;
    uint8_t pwmDevNum;
    DevHandle pwmHandle;
    struct PwmConfig config;
    struct BacklightProperties props;
};

static int32_t BlPwmUpdateBrightness(struct BacklightDev *blDev, uint32_t brightness)
{
    int32_t ret;
    uint32_t duty;
    struct BlPwmDev *blPwmDev = NULL;

    blPwmDev = ToBlDevPriv(blDev);
    if (blPwmDev == NULL) {
        HDF_LOGE("%s blPwmDev is null", __func__);
        return HDF_FAILURE;
    }
    if (blPwmDev->props.maxBrightness == 0) {
        HDF_LOGE("%s maxBrightness is 0", __func__);
        return HDF_FAILURE;
    }
    if (brightness == 0) {
        return PwmDisable(blPwmDev->pwmHandle);
    }
    duty = (brightness * blPwmDev->config.period) / blPwmDev->props.maxBrightness;
    ret = PwmSetDuty(blPwmDev->pwmHandle, duty);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmSetDuty failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    return PwmEnable(blPwmDev->pwmHandle);
}

static struct BacklightOps g_blDevOps = {
    .updateBrightness = BlPwmUpdateBrightness,
};

static int32_t ParsePwmNum(const struct DeviceResourceNode *node, uint8_t *pwmNum)
{
    int32_t ret;
    struct DeviceResourceIface *parser = NULL;

    if (node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return HDF_FAILURE;
    }
    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (parser == NULL) {
        HDF_LOGE("%s: instance parser failed", __func__);
        return HDF_FAILURE;
    }
    ret = parser->GetUint8(node, "pwmDevNum", pwmNum, 0);
    CHECK_PARSER_RET(ret, "GetUint8");
    return HDF_SUCCESS;
}
static int32_t ParseBlPwmCfg(const struct DeviceResourceNode *node, struct BlPwmDev *blPwmDev)
{
    int32_t ret;
    uint32_t defBrightness;
    struct DeviceResourceIface *parser = NULL;

    if (node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return HDF_FAILURE;
    }
    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (parser == NULL) {
        HDF_LOGE("%s: instance parser failed", __func__);
        return HDF_FAILURE;
    }
    ret = parser->GetString(node, "backlightDevName", &blPwmDev->name, NULL);
    CHECK_PARSER_RET(ret, "GetString");
    ret = parser->GetUint32(node, "pwmMaxPeroid", &blPwmDev->config.period, 0);
    CHECK_PARSER_RET(ret, "GetUint32");
    ret = parser->GetUint32(node, "minBrightness", &blPwmDev->props.minBrightness, DEF_MIN_BRIGHTNESS);
    CHECK_PARSER_RET(ret, "GetUint32");
    ret = parser->GetUint32(node, "maxBrightness", &blPwmDev->props.maxBrightness, DEF_MAX_BRIGHTNESS);
    CHECK_PARSER_RET(ret, "GetUint32");
    // if not provided defBrightness, defBrightness = (min + max) / 2
    defBrightness = (blPwmDev->props.minBrightness + blPwmDev->props.maxBrightness) / 2;
    ret = parser->GetUint32(node, "defBrightness", &blPwmDev->props.defBrightness, defBrightness);
    CHECK_PARSER_RET(ret, "GetUint32");
    blPwmDev->config.duty = 1;
    blPwmDev->config.status = 1;
    return HDF_SUCCESS;
}

static int32_t BlPwmDevInit(struct HdfDeviceObject *object, struct BlPwmDev *blPwmDev)
{
    if (ParsePwmNum(object->property, &blPwmDev->pwmDevNum) != HDF_SUCCESS) {
        HDF_LOGE("%s ParsePwmNum fail", __func__);
        return HDF_FAILURE;
    }
    blPwmDev->pwmHandle = PwmOpen(blPwmDev->pwmDevNum);
    if (blPwmDev->pwmHandle == NULL) {
        HDF_LOGE("%s: PwmOpen failed", __func__);
        return HDF_FAILURE;
    }
    if (PwmGetConfig(blPwmDev->pwmHandle, &blPwmDev->config) != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmGetConfig fail", __func__);
        return HDF_FAILURE;
    }
    if (ParseBlPwmCfg(object->property, blPwmDev) != HDF_SUCCESS) {
        HDF_LOGE("%s: ParseBlPwmCfg fail", __func__);
        return HDF_FAILURE;
    }
    if (PwmSetConfig(blPwmDev->pwmHandle, &blPwmDev->config) != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmSetConfig fail", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t BlPwmEntryInit(struct HdfDeviceObject *object)
{
    int32_t ret;
    struct BacklightDev *blDev = NULL;
    struct BlPwmDev *blPwmDev = NULL;

    blPwmDev = (struct BlPwmDev *)OsalMemCalloc(sizeof(struct BlPwmDev));
    if (blPwmDev == NULL) {
        HDF_LOGE("%s blPwmDev malloc fail", __func__);
        return HDF_FAILURE;
    }
    ret = BlPwmDevInit(object, blPwmDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: BlPwmDevInit fail", __func__);
        goto FAIL;
    }
    blDev = RegisterBlDev(blPwmDev->name, &blPwmDev->props, &g_blDevOps, blPwmDev);
    if (blDev == NULL) {
        HDF_LOGE("%s: RegisterBlDev fail", __func__);
        goto FAIL;
    }
    if (UpdateBacklightState(blDev, FB_POWER_ON) != HDF_SUCCESS) {
        HDF_LOGE("%s: UpdateBacklightState fail", __func__);
    }
    if (UpdateBrightness(blDev, blPwmDev->props.defBrightness) != HDF_SUCCESS) {
        HDF_LOGE("%s: UpdateBrightness fail", __func__);
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;

FAIL:
    OsalMemFree(blPwmDev);
    return HDF_FAILURE;
}

struct HdfDriverEntry g_pwmBlDevEntry = {
    .moduleVersion = 1,
    .moduleName = "PWM_BL",
    .Init = BlPwmEntryInit,
};

HDF_INIT(g_pwmBlDevEntry);
