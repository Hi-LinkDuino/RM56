/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "gpio_if.h"
#include "osal_mem.h"
#include "hdf_log.h"
#include "wifi_module.h"
#include "hdf_wlan_chipdriver_manager.h"
#include "securec.h"

#define HDF_LOG_TAG HDF_WIFI_CORE

/* power manager begin */
enum PowerType {
    POWER_NOT_MANAGED = 0,
    POWER_MANAGED_BY_GPIO = 1
};

enum GPIOActiveLevel {
    ACTIVE_LOW = 0,
    ACTIVE_HIGH
};

/* powerCount=1 single power source */
struct DevicePowerData {
    struct HdfConfigWlanPower power0;
    struct HdfConfigWlanPower power1;
};

struct PowerManagerImpl {
    struct PowerManager base;
    struct DevicePowerData powerDatas;
};

/* power on method */
static int32_t HdfWlanSinglePowerActive(struct HdfConfigWlanPower* powerDate)
{
    int32_t ret;
    if (powerDate == NULL) {
        HDF_LOGE("%s:powerDate is NULL", __func__);
        return HDF_FAILURE;
    }
    if (powerDate->powerType == POWER_NOT_MANAGED) {
        HDF_LOGI("%s:power type is always on", __func__);
        return HDF_SUCCESS;
    }
    ret = GpioSetDir(powerDate->gpioId, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:set dir fail! ret=%d\n", __func__, ret);
        return HDF_FAILURE;
    }
    OsalMSleep(powerDate->powerSeqDelay);
    ret = GpioWrite(powerDate->gpioId, powerDate->activeLevel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:set power on fail! ret=%d\n", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
static int32_t HdfWlanSingleDeActive(struct HdfConfigWlanPower* powerDate)
{
    int32_t ret;
    uint8_t deActive;

    if (powerDate == NULL) {
        HDF_LOGE("%s: powerDate is NULL", __func__);
        return HDF_FAILURE;
    }
    deActive = !powerDate->activeLevel;
    if (powerDate->powerType == POWER_NOT_MANAGED) {
        HDF_LOGE("%s:power type is not supported in current version", __func__);
        return HDF_FAILURE;
    }
    ret = GpioSetDir(powerDate->gpioId, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:set dir fail! ret:%d\n", __func__, ret);
        return ret;
    }
    ret = GpioWrite(powerDate->gpioId, deActive);
    return HDF_SUCCESS;
}

static int32_t HdfWlanChipPowerOn(struct PowerManager* powerMgr)
{
    int32_t ret;
    struct PowerManagerImpl *powerMgrimpl = NULL;
    if (powerMgr == NULL) {
        HDF_LOGE("%s: powerMgr is NULL", __func__);
        return HDF_FAILURE;
    }
    powerMgrimpl = (struct PowerManagerImpl*)powerMgr;
    ret = HdfWlanSinglePowerActive(&powerMgrimpl->powerDatas.power0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfWlanMainPowerOn fail! ret:%d\n", __func__, ret);
        (void)HdfWlanSingleDeActive(&powerMgrimpl->powerDatas.power0);
        return HDF_FAILURE;
    }
    ret = HdfWlanSinglePowerActive((struct HdfConfigWlanPower*)&powerMgrimpl->powerDatas.power1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfWlanPowerActive fail! ret:%d\n", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t HdfWlanChipPowerOff(struct PowerManager* powerMgr)
{
    int32_t ret;
    struct PowerManagerImpl *powerMgrimpl = NULL;
    if (powerMgr == NULL) {
        HDF_LOGE("%s: powerMgr is NULL", __func__);
        return HDF_FAILURE;
    }
    powerMgrimpl = (struct PowerManagerImpl*)powerMgr;
    ret = HdfWlanSingleDeActive(&powerMgrimpl->powerDatas.power0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: the main power off fail", __func__);
        return HDF_FAILURE;
    }
    ret = HdfWlanSingleDeActive((struct HdfConfigWlanPower*)&powerMgrimpl->powerDatas.power1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: the standby power off fail", __func__);
        return ret;
    }
    return HDF_SUCCESS;
}

int32_t HdfWlanPowerMgrRelease(struct PowerManager* powerMgr)
{
    if (powerMgr == NULL) {
        HDF_LOGW("%s: powerMgr already deinit or input para error ", __func__);
        return HDF_SUCCESS;
    }
    OsalMemFree(powerMgr);
    return HDF_SUCCESS;
}
/**
 * @brief create powers manager according to the powers config
 */
struct PowerManager* HdfWlanCreatePowerManager(const struct HdfConfWlanPowers *configPowers)
{
    struct PowerManagerImpl *powerMgrimpl = NULL;
    if (configPowers == NULL) {
        HDF_LOGE("%s: configPowers is NULL", __func__);
        return NULL;
    }
    powerMgrimpl = (struct PowerManagerImpl *)OsalMemCalloc(sizeof(struct PowerManagerImpl));
    if (powerMgrimpl == NULL) {
        HDF_LOGE("%s: OsalMemCalloc fail! ", __func__);
        return NULL;
    }
    powerMgrimpl->base.On = HdfWlanChipPowerOn;
    powerMgrimpl->base.Off = HdfWlanChipPowerOff;
    powerMgrimpl->base.Release = HdfWlanPowerMgrRelease;
    powerMgrimpl->powerDatas.power0 = configPowers->power0;
    powerMgrimpl->powerDatas.power1 = configPowers->power1;
    return (struct PowerManager *)powerMgrimpl;
}