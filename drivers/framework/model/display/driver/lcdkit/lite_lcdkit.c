/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "gpio_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "mipi_dsi_if.h"
#include "osal.h"
#include "pwm_if.h"
#include "lite_lcdkit.h"

static struct PanelConfig *GetPanelCfg(void)
{
    static struct PanelConfig panelCfg;
    return &panelCfg;
}

static int32_t PowerInit(void)
{
    int32_t ret;
    int32_t count;
    int32_t i;
    struct PanelConfig *panelCfg = GetPanelCfg();

    count = panelCfg->setting.count;
    for (i = 0; i < count; i++) {
        if (panelCfg->setting.power[i].type == GPIO_POWER) {
            ret = GpioSetDir(panelCfg->setting.power[i].num, GPIO_DIR_OUT);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s:GpioSetDir failed", __func__);
                return HDF_FAILURE;
            }
        }
    }
    return HDF_SUCCESS;
}

static int32_t LcdkitInit(void)
{
    int32_t ret;
    struct PanelConfig *panelCfg = GetPanelCfg();

    if (panelCfg->info.intfType != MIPI_DSI) {
        HDF_LOGE("%s:not support intf: %u", __func__, panelCfg->info.intfType);
        return HDF_FAILURE;
    }
    ret = PowerInit();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:PowerInit failed", __func__);
        return HDF_FAILURE;
    }

    panelCfg->dsiHandle = MipiDsiOpen(panelCfg->dsiDev);
    if (panelCfg->dsiHandle == NULL) {
        HDF_LOGE("%s:MipiDsiOpen failed", __func__);
        return HDF_FAILURE;
    }

    if (panelCfg->info.blk.type == BLK_PWM) {
        panelCfg->pwmHandle = PwmOpen(panelCfg->info.pwm.dev);
        if (panelCfg->pwmHandle == NULL) {
            MipiDsiClose(panelCfg->dsiHandle);
            panelCfg->dsiHandle = NULL;
            HDF_LOGE("%s: PwmOpen failed", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static int32_t MipiDsiOn(void)
{
    int32_t ret;
    struct PanelConfig *panelCfg = GetPanelCfg();
    int32_t i;

    if (panelCfg->dsiHandle == NULL) {
        HDF_LOGE("%s:dsiHandle is null", __func__);
        return HDF_FAILURE;
    }
    /* send mipi init code */
    int32_t count = panelCfg->onCmd.count;
    for (i = 0; i < count; i++) {
        ret = MipiDsiTx(panelCfg->dsiHandle, &(panelCfg->onCmd.dsiCmd[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:MipiDsiTx failed", __func__);
            return ret;
        }
    }
    /* set mipi to hs mode */
    MipiDsiSetHsMode(panelCfg->dsiHandle);
    return HDF_SUCCESS;
}

static int32_t MipiDsiOff(void)
{
    int32_t ret;
    struct PanelConfig *panelCfg = GetPanelCfg();
    int32_t i;

    if (panelCfg->dsiHandle == NULL) {
        HDF_LOGE("%s:dsiHandle is null", __func__);
        return HDF_FAILURE;
    }
    /* send mipi panel off code */
    int32_t count = panelCfg->offCmd.count;
    for (i = 0; i < count; i++) {
        ret = MipiDsiTx(panelCfg->dsiHandle, &(panelCfg->offCmd.dsiCmd[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:MipiDsiTx failed", __func__);
            return ret;
        }
    }
    /* set mipi to lp mode */
    MipiDsiSetLpMode(panelCfg->dsiHandle);
    return HDF_SUCCESS;
}

static int32_t PowerOn(void)
{
    int32_t ret;
    struct PanelConfig *panelCfg = GetPanelCfg();
    int32_t i;

    for (i = 0; i < panelCfg->onSeq.count; i++) {
        if (panelCfg->onSeq.pwCtrl[i].type == GPIO_POWER) {
            ret = GpioWrite(panelCfg->onSeq.pwCtrl[i].num, panelCfg->onSeq.pwCtrl[i].opt);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s:GpioWrite failed", __func__);
                return HDF_FAILURE;
            }
            OsalMSleep(panelCfg->onSeq.pwCtrl[i].delay);
        }
    }
    return HDF_SUCCESS;
}

static int32_t PowerOff(void)
{
    int32_t ret;
    struct PanelConfig *panelCfg = GetPanelCfg();
    int32_t i;

    for (i = 0; i < panelCfg->offSeq.count; i++) {
        if (panelCfg->offSeq.pwCtrl[i].type == GPIO_POWER) {
            ret = GpioWrite(panelCfg->offSeq.pwCtrl[i].num, panelCfg->offSeq.pwCtrl[i].opt);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s:GpioWrite failed", __func__);
                return HDF_FAILURE;
            }
            OsalMSleep(panelCfg->offSeq.pwCtrl[i].delay);
        }
    }
    return HDF_SUCCESS;
}

static int32_t LcdkitOn(void)
{
    int32_t ret;

    ret = PowerOn();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:MipiPowerOn failed", __func__);
        return HDF_FAILURE;
    }
    ret = MipiDsiOn();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:MipiDsiOn failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t LcdkitOff(void)
{
    int32_t ret;

    ret = MipiDsiOff();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:MipiDsiOff failed", __func__);
        return HDF_FAILURE;
    }
    ret = PowerOff();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:PanelPowerOff failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SetBacklightByPwm(uint32_t level)
{
    struct PanelConfig *panelCfg = GetPanelCfg();
    int32_t ret;
    uint32_t duty;

    if (panelCfg->pwmHandle == NULL) {
        HDF_LOGE("%s:pwmHandle is null", __func__);
        return HDF_FAILURE;
    }
    duty = (level * panelCfg->info.pwm.period) / panelCfg->info.blk.maxLevel;
    ret = PwmSetDuty(panelCfg->pwmHandle, duty);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmSetDuty failed", __func__);
        return HDF_FAILURE;
    }
    static uint32_t lastLevel = 0;
    if (level != 0 && lastLevel == 0) {
        ret = PwmEnable(panelCfg->pwmHandle);
    } else if (level == 0 && lastLevel != 0) {
        ret = PwmDisable(panelCfg->pwmHandle);
    }
    lastLevel = level;
    return ret;
}

static int32_t SetBacklightByMipi(uint32_t level)
{
    int32_t ret;
    struct PanelConfig *panelCfg = GetPanelCfg();
    uint8_t payLoad[] = { 0x51, 0x00 }; // panel backlight cmd
    struct DsiCmdDesc bklCmd = { 0x15, 0, sizeof(payLoad), payLoad }; // dsi backlight cmd

    if (panelCfg->dsiHandle == NULL) {
        HDF_LOGE("%s: dsiHandle is null", __func__);
        return HDF_FAILURE;
    }
    payLoad[1] = level;
    ret = MipiDsiTx(panelCfg->dsiHandle, &bklCmd);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: MipiDsiTx failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t LcdkitSetBkl(uint32_t level)
{
    struct PanelConfig *panelCfg = GetPanelCfg();
    int32_t ret = HDF_SUCCESS;
    if (panelCfg->info.blk.type == BLK_PWM) {
        ret = SetBacklightByPwm(level);
    } else if (panelCfg->info.blk.type == BLK_MIPI) {
        ret = SetBacklightByMipi(level);
    } else {
        HDF_LOGE("%s: not support backlight type:%d", __func__, panelCfg->info.blk.type);
        return HDF_FAILURE;
    }
    return ret;
}

static struct PanelData g_panelData = {
    .init = LcdkitInit,
    .on = LcdkitOn,
    .off = LcdkitOff,
    .setBacklight = LcdkitSetBkl,
};

static int32_t LcdkitEntryInit(struct HdfDeviceObject *object)
{
    struct PanelConfig *panelCfg = GetPanelCfg();

    if (object == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }
    if (ParseLcdConfig(object->property, panelCfg)) {
        HDF_LOGE("%s: ParseLcdConfig failed", __func__);
        return HDF_FAILURE;
    }
    g_panelData.info = &panelCfg->info;
    if (PanelDataRegister(&g_panelData) != HDF_SUCCESS) {
        HDF_LOGE("%s: PanelDataRegister failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: exit succ", __func__);
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_lcdkitDevEntry = {
    .moduleVersion = 1,
    .moduleName = "LITE_LCDKIT",
    .Init = LcdkitEntryInit,
};

HDF_INIT(g_lcdkitDevEntry);
