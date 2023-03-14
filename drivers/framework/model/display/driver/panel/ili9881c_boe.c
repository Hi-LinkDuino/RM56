/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ili9881c_boe.h"
#include "gpio_if.h"
#include "hdf_bl.h"
#include "hdf_disp.h"
#include "osal.h"

static int Ili9881cBoeSendCmds(struct mipi_dsi_device *dsi,
	const struct DsiCmdDesc *cmds, int size)
{
    int32_t i;

    if (dsi == NULL) {
        return -EINVAL;
    }

    for (i = 0; i < size; i++) {
        mipi_dsi_generic_write(dsi, cmds[i].payload, cmds[i].dataLen);
        if (cmds[i].delay) {
            OsalMSleep(cmds[i].delay);
        }
    }
    return HDF_SUCCESS;
}

static struct Ili9881cBoeDev *ToIli9881cBoeDev(const struct PanelData *panel)
{
    return (struct Ili9881cBoeDev *)panel->object->priv;
}

static SetGpioState(uint16_t gpio, uint16_t dir, uint16_t level, uint32_t delay)
{
    int32_t ret;

    ret = GpioSetDir(gpio, dir);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s GpioSetDir failed, ret:%d", __func__, ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(gpio, level);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s GpioWrite failed, ret:%d", __func__, ret);
        return HDF_FAILURE;
    }
    OsalMSleep(delay);
    return HDF_SUCCESS;
}

static int32_t Ili9881cBoePrepare(const struct Ili9881cBoeDev *ili9881cBoeDev)
{
    int32_t i;
    int32_t ret;
    int32_t items;
    struct GpioTiming *timing = NULL;

    HDF_LOGI("%s()", __func__);
    ret = regulator_enable(ili9881cBoeDev->supply);
    if (ret < 0) {
        HDF_LOGE("regulator_enable failed");
    }
    ret = SetGpioState(ili9881cBoeDev->avddGpio, GPIO_DIR_OUT, GPIO_VAL_HIGH, 5); /* delay 5ms */
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s SetGpioState failed, gpio:%d", __func__, ili9881cBoeDev->avddGpio);
        return HDF_FAILURE;
    }
    ret = SetGpioState(ili9881cBoeDev->aveeGpio, GPIO_DIR_OUT, GPIO_VAL_HIGH, 5);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s SetGpioState failed, gpio:%d", __func__, ili9881cBoeDev->aveeGpio);
        return HDF_FAILURE;
    }
    ret = SetGpioState(ili9881cBoeDev->vghlGpio, GPIO_DIR_OUT, GPIO_VAL_HIGH, 5);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s SetGpioState failed, gpio:%d", __func__, ili9881cBoeDev->vghlGpio);
        return HDF_FAILURE;
    }
    ret = SetGpioState(ili9881cBoeDev->tsrstGpio, GPIO_DIR_OUT, GPIO_VAL_HIGH, 5);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s SetGpioState failed, gpio:%d", __func__, ili9881cBoeDev->tsrstGpio);
        return HDF_FAILURE;
    }
    ret = GpioSetDir(ili9881cBoeDev->resetGpio, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioSetDir failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    items = ili9881cBoeDev->rstOnSeq.items;
    timing = ili9881cBoeDev->rstOnSeq.timing;
    for (i = 0; i < items; i++) {
        GpioWrite(ili9881cBoeDev->resetGpio, timing[i].level);
        OsalMSleep(timing[i].delay);
    }
    return HDF_SUCCESS;
}

static int32_t Ili9881cBoeUnprepare(const struct Ili9881cBoeDev *ili9881cBoeDev)
{
    int32_t i;
    int32_t ret;
    int32_t items;
    struct GpioTiming *timing = NULL;

    HDF_LOGI("%s()", __func__);
    ret = SetGpioState(ili9881cBoeDev->avddGpio, GPIO_DIR_OUT, GPIO_VAL_LOW, 5); /* delay 5ms */
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s SetGpioState failed, gpio:%d", __func__, ili9881cBoeDev->avddGpio);
        return HDF_FAILURE;
    }
    ret = SetGpioState(ili9881cBoeDev->aveeGpio, GPIO_DIR_OUT, GPIO_VAL_LOW, 5);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s SetGpioState failed, gpio:%d", __func__, ili9881cBoeDev->aveeGpio);
        return HDF_FAILURE;
    }
    ret = SetGpioState(ili9881cBoeDev->vghlGpio, GPIO_DIR_OUT, GPIO_VAL_LOW, 5);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s SetGpioState failed, gpio:%d", __func__, ili9881cBoeDev->vghlGpio);
        return HDF_FAILURE;
    }
    ret = SetGpioState(ili9881cBoeDev->tsrstGpio, GPIO_DIR_OUT, GPIO_VAL_LOW, 5);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s SetGpioState failed, gpio:%d", __func__, ili9881cBoeDev->tsrstGpio);
        return HDF_FAILURE;
    }
    ret = GpioSetDir(ili9881cBoeDev->resetGpio, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioSetDir failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    items = ili9881cBoeDev->rstOffSeq.items;
    timing = ili9881cBoeDev->rstOffSeq.timing;
    for (i = 0; i < items; i++) {
        GpioWrite(ili9881cBoeDev->resetGpio, timing[i].level);
        OsalMSleep(timing[i].delay);
    }
    regulator_disable(ili9881cBoeDev->supply);
    return HDF_SUCCESS;
}

static int32_t Ili9881cBoeOn(struct PanelData *panel)
{
    int32_t ret;
    struct Ili9881cBoeDev *ili9881cBoeDev = NULL;

    HDF_LOGI("%s()", __func__);
    ili9881cBoeDev = ToIli9881cBoeDev(panel);
    ret = Ili9881cBoePrepare(ili9881cBoeDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s Ili9881cBoePrepare failed", __func__);
        return HDF_FAILURE;
    }
    ret = Ili9881cBoeSendCmds(ili9881cBoeDev->dsiDev, g_panelOnCode,
                              sizeof(g_panelOnCode) / sizeof(g_panelOnCode[0]));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s Ili9881cBoeSendCmds failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t Ili9881cBoeOff(struct PanelData *panel)
{
    int32_t ret;
    struct Ili9881cBoeDev *ili9881cBoeDev = NULL;

    ili9881cBoeDev = ToIli9881cBoeDev(panel);
    HDF_LOGI(" %s line = %d", __func__, __LINE__);
    ret = Ili9881cBoeSendCmds(ili9881cBoeDev->dsiDev, g_panelOffCode,
                              sizeof(g_panelOffCode) / sizeof(g_panelOffCode[0]));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s Ili9881cBoeSendCmds failed", __func__);
        return HDF_FAILURE;
    }
    ret = Ili9881cBoeUnprepare(ili9881cBoeDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s Ili9881cBoeUnprepare failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t Ili9881cBoeInit(struct PanelData *panel)
{
    return 0;
}

#define BLK_PWM_INDEX             2
#define PWM_MAX_PERIOD            40000
/* backlight setting */
#define MIN_LEVEL                 0
#define MAX_LEVEL                 255
#define DEFAULT_LEVEL             127

static struct PanelInfo g_panelInfo = {
    .width = 800,           /* width */
    .height = 1280,         /* height */
    .hbp = 80,              /* horizontal back porch */
    .hfp = 80,              /* horizontal front porch */
    .hsw = 20,              /* horizontal sync width */
    .vbp = 12,              /* vertical back porch */
    .vfp = 20,              /* vertical front porch */
    .vsw = 4,               /* vertical sync width */
    .clockFreq = 76800000,  /* clock */
    .pWidth = 150,          /* physical width */
    .pHeight = 240,         /* physical height */
    .blk = { BLK_PWM, MIN_LEVEL, MAX_LEVEL, DEFAULT_LEVEL },
};

static struct GpioTiming g_rstOnSeq[] = {
    {1, 5},   /* high, delay 5ms */
    {0, 20},  /* low, delay 20ms */
    {1, 30},  /* high, delay 30ms */
};

static struct GpioTiming g_rstOffSeq = {0, 10}; /* low, delay 10ms */

static void Ili9881cBoeResInit(struct Ili9881cBoeDev *ili9881cBoeDev)
{
    ili9881cBoeDev->avddGpio = AVDD_GPIO;
    ili9881cBoeDev->aveeGpio = AVEE_GPIO;
    ili9881cBoeDev->vghlGpio = VGHL_GPIO;
    ili9881cBoeDev->tsrstGpio = TSRST_GPIO;
    ili9881cBoeDev->resetGpio = RESET_GPIO;
    ili9881cBoeDev->rstOnSeq.items = sizeof(g_rstOnSeq) / sizeof(struct GpioTiming);
    ili9881cBoeDev->rstOnSeq.timing = g_rstOnSeq;
    ili9881cBoeDev->rstOffSeq.items = 1; /* number of reset off sequence */
    ili9881cBoeDev->rstOffSeq.timing = &g_rstOffSeq;
    ili9881cBoeDev->dsiDev->lanes = 4; /* number of active data lanes */
    ili9881cBoeDev->dsiDev->format = 0; /* pixel format for video mode */
    ili9881cBoeDev->dsiDev->mode_flags = 3; /* DSI operation mode related flags */
    ili9881cBoeDev->panel.info = &g_panelInfo;
    ili9881cBoeDev->panel.init = Ili9881cBoeInit;
    ili9881cBoeDev->panel.on = Ili9881cBoeOn;
    ili9881cBoeDev->panel.off = Ili9881cBoeOff;
    ili9881cBoeDev->panel.priv = ili9881cBoeDev->dsiDev;
}

int32_t Ili9881cBoeEntryInit(struct HdfDeviceObject *object)
{
    struct device_node *panelNode = NULL;
    struct Ili9881cBoeDev *ili9881cBoeDev = NULL;

    ili9881cBoeDev = (struct Ili9881cBoeDev *)OsalMemCalloc(sizeof(struct Ili9881cBoeDev));
    if (ili9881cBoeDev == NULL) {
        HDF_LOGE("%s ili9881cBoeDev malloc fail", __func__);
        return HDF_FAILURE;
    }
    panelNode = of_find_compatible_node(NULL, NULL, "sprd,generic-mipi-panel");
    if (panelNode == NULL) {
        HDF_LOGE("%s of_find_compatible_node fail", __func__);
        goto FAIL;
    }
    ili9881cBoeDev->dsiDev = of_find_mipi_dsi_device_by_node(panelNode);
    if (ili9881cBoeDev->dsiDev == NULL) {
        HDF_LOGE("%s of_find_mipi_dsi_device_by_node fail", __func__);
        goto FAIL;
    }
    ili9881cBoeDev->supply = devm_regulator_get(&ili9881cBoeDev->dsiDev->dev, "power");
    if (ili9881cBoeDev->supply == NULL) {
        HDF_LOGE("Get regulator fail");
        goto FAIL;
    }
    Ili9881cBoeResInit(ili9881cBoeDev);
    ili9881cBoeDev->panel.blDev = GetBacklightDev("hdf_pwm");
    if (ili9881cBoeDev->panel.blDev == NULL) {
        HDF_LOGE("%s GetBacklightDev fail", __func__);
        goto FAIL;
    }
    ili9881cBoeDev->panel.object = object;
    object->priv = ili9881cBoeDev;
    if (RegisterPanel(&ili9881cBoeDev->panel) == HDF_SUCCESS) {
        HDF_LOGI("%s success", __func__);
        return HDF_SUCCESS;
    }

FAIL:
    OsalMemFree(ili9881cBoeDev);
    return HDF_FAILURE;
}

struct HdfDriverEntry g_ili9881cBoeDevEntry = {
    .moduleVersion = 1,
    .moduleName = "LCD_ILI9881CBOE",
    .Init = Ili9881cBoeEntryInit,
};

HDF_INIT(g_ili9881cBoeDevEntry);
