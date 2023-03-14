/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pwm_bes.h"
#include <stdlib.h>
#include <stdio.h>
#include "hdf_device_desc.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "pwm_core.h"
#include "hdf_log.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif

#if defined (CHIP_BEST1600)
#define PWM_MAX_FUNCTION 4
#elif defined (CHIP_BEST2003)
#include "hal_iomux.h"
#define PWM_MAX_FUNCTION 8
#endif

#define UNTIL_NAN0SECONDS 1000000000
#define PERCENT 100
#define DEC_TEN 10
#define PIN_GROUP_NUM 8

static uint32_t g_pwmFunction[PWM_MAX_FUNCTION] = {
    HAL_IOMUX_FUNC_PWM0,
    HAL_IOMUX_FUNC_PWM1,
    HAL_IOMUX_FUNC_PWM2,
    HAL_IOMUX_FUNC_PWM3,
#if defined (CHIP_BEST2003)
    HAL_IOMUX_FUNC_PWM4,
    HAL_IOMUX_FUNC_PWM5,
    HAL_IOMUX_FUNC_PWM6,
    HAL_IOMUX_FUNC_PWM7,
#endif
};

static int32_t PwmDevSetConfig(struct PwmDev *pwm, struct PwmConfig *config);
static int32_t PwmDevOpen(struct PwmDev *pwm);
static int32_t PwmDevClose(struct PwmDev *pwm);

struct PwmMethod g_pwmmethod = {
    .setConfig = PwmDevSetConfig,
    .open = PwmDevOpen,
    .close = PwmDevClose,
};

static int InitPwmDevice(struct PwmDev *host)
{
    struct PwmDevice *pwmDevice = NULL;
    struct PwmResource *resource = NULL;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invaild parameter\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    pwmDevice = (struct PwmDevice *)host->priv;
    if (pwmDevice == NULL) {
        HDF_LOGE("pwmDevice is NULL\r\n");
        return HDF_DEV_ERR_NO_DEVICE;
    }

    resource = &pwmDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is NULL\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

    struct HAL_IOMUX_PIN_FUNCTION_MAP pinMuxPwm[] = {
        {0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };

    pinMuxPwm->pin = resource->pwmPin;
    pinMuxPwm->function = g_pwmFunction[resource->pwmId];

    hal_iomux_init(pinMuxPwm, ARRAY_SIZE(pinMuxPwm));

    return HDF_SUCCESS;
}

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define PWM_FIND_CONFIG(node, name, resource) \
    do { \
        if (strcmp(HCS_PROP(node, match_attr), name) == 0) { \
            tempPin = HCS_PROP(node, pwmPin); \
            resource->pwmPin = ((tempPin / DEC_TEN) * PIN_GROUP_NUM) + (tempPin % DEC_TEN); \
            resource->pwmId = HCS_PROP(node, pwmId); \
            result = HDF_SUCCESS; \
            break; \
        } \
    } while (0)
#define PLATFORM_CONFIG HCS_NODE(HCS_ROOT, platform)
#define PLATFORM_PWM_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), pwm_config)
static uint32_t GetPwmDeviceResource(struct PwmDevice *device, const char *deviceMatchAttr)
{
    uint32_t tempPin;
    int32_t result = HDF_FAILURE;
    struct PwmResource *resource = NULL;
    if (device == NULL || deviceMatchAttr == NULL) {
        HDF_LOGE("%s: device or deviceMatchAttr is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
#if HCS_NODE_HAS_PROP(PLATFORM_CONFIG, pwm_config)
    HCS_FOREACH_CHILD_VARGS(PLATFORM_PWM_CONFIG, PWM_FIND_CONFIG, deviceMatchAttr, resource);
#endif
    if (result != HDF_SUCCESS) {
        HDF_LOGE("resourceNode %s is NULL\r\n", deviceMatchAttr);
    }
    return result;
}
#else
static uint32_t GetPwmDeviceResource(
    struct PwmDevice *device, const struct DeviceResourceNode *resourceNode)
{
    uint32_t tempPin = 0;
    struct DeviceResourceIface *dri = NULL;
    struct PwmResource *resource = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("resource or device is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is NULL\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    if (dri->GetUint32(resourceNode, "pwmPin", &tempPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("read pwmPin fail\r\n");
        return HDF_FAILURE;
    }
    resource->pwmPin = ((tempPin / DEC_TEN) * PIN_GROUP_NUM) + (tempPin % DEC_TEN);

    if (dri->GetUint32(resourceNode, "pwmId", &resource->pwmId, 0) != HDF_SUCCESS) {
        HDF_LOGE("read pwmId fail\r\n");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}
#endif
static int32_t AttachPwmDevice(struct PwmDev *host, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct PwmDevice *pwmDevice = NULL;
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (device == NULL || host == NULL) {
#else
    if (device == NULL || device->property == NULL || host == NULL) {
#endif
        HDF_LOGE("%s: param is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    pwmDevice = (struct PwmDevice *)OsalMemAlloc(sizeof(struct PwmDevice));
    if (pwmDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc pwmDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    ret = GetPwmDeviceResource(pwmDevice, device->deviceMatchAttr);
#else
    ret = GetPwmDeviceResource(pwmDevice, device->property);
#endif
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(pwmDevice);
        return HDF_FAILURE;
    }

    host->priv = pwmDevice;
    host->num = pwmDevice->resource.pwmId;

    return InitPwmDevice(host);
}
/* HdfDriverEntry method definitions */
static int32_t PwmDriverBind(struct HdfDeviceObject *device);
static int32_t PwmDriverInit(struct HdfDeviceObject *device);
static void PwmDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_pwmDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BES_PWM_MODULE_HDF",
    .Bind = PwmDriverBind,
    .Init = PwmDriverInit,
    .Release = PwmDriverRelease,
};

// Initialize HdfDriverEntry
HDF_INIT(g_pwmDriverEntry);

static int32_t PwmDriverBind(struct HdfDeviceObject *device)
{
    static struct PwmDev devService;

    if (device == NULL) {
        HDF_LOGE("hdfDevice object is null!\r\n");
        return HDF_FAILURE;
    }

    device->service = &devService.service;
    devService.device = device;
    HDF_LOGI("Enter %s\r\n", __func__);

    return HDF_SUCCESS;
}

static int32_t PwmDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct PwmDev *host = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HDF_LOGI("Enter %s:\r\n", __func__);

    host = (struct PwmDev *)device->service;
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = AttachPwmDevice(host, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:attach error\r\n", __func__);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    host->method = &g_pwmmethod;
    ret = PwmDeviceAdd(device, host);
    if (ret != HDF_SUCCESS) {
        PwmDeviceRemove(device, host);
        OsalMemFree(host->device);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    HDF_LOGI("PwmDriverInit success!\r\n");
    return HDF_SUCCESS;
}

static void PwmDriverRelease(struct HdfDeviceObject *device)
{
    struct PwmDev *host = NULL;

    if (device == NULL || device->service == NULL) {
        HDF_LOGE("device is null\r\n");
        return;
    }

    host = (struct PwmDev *)device->service;
    if (host != NULL && host->device != NULL) {
        host->method = NULL;
        OsalMemFree(host->device);
        host->device = NULL;
        host = NULL;
    }

    device->service = NULL;
    HDF_LOGI("PwmDriverRelease finish!!\r\n");
    return;
}

static int32_t PwmDevSetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    struct PwmDevice *prvPwm = NULL;
    struct HAL_PWM_CFG_T *pwmCfg = NULL;
    enum HAL_PWM_ID_T pwmId;

    if (pwm == NULL || config == NULL || (config->period > UNTIL_NAN0SECONDS)) {
        HDF_LOGE("%s\r\n", __FUNCTION__);
        return HDF_FAILURE;
    }

    prvPwm = (struct PwmDevice *)PwmGetPriv(pwm);
    if (prvPwm == NULL) {
        return HDF_FAILURE;
    }
    pwmCfg = &prvPwm->pwmCfg;
    pwmCfg->freq = UNTIL_NAN0SECONDS / config->period;
    pwmCfg->ratio = config->duty * PERCENT / config->period;
    pwmCfg->inv = false;
    pwmCfg->sleep_on = false;

    pwmId = prvPwm->resource.pwmId;

    if (config->status == PWM_ENABLE_STATUS) {
        hal_pwm_enable(pwmId, pwmCfg);
    } else {
        hal_pwm_disable(pwmId);
    }

    return HDF_SUCCESS;
}

static int32_t PwmDevOpen(struct PwmDev *pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s\r\n", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

static int32_t PwmDevClose(struct PwmDev *pwm)
{
    struct PwmDevice *prvPwm = NULL;
    enum HAL_PWM_ID_T pwmId = 0;

    if (pwm == NULL) {
        HDF_LOGE("%s\r\n", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }

    prvPwm = (struct PwmDevice *)PwmGetPriv(pwm);
    if (prvPwm == NULL) {
        HDF_LOGE("%s\r\n", __FUNCTION__);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    pwmId = prvPwm->resource.pwmId;

    hal_pwm_disable(pwmId);

    return HDF_SUCCESS;
}
