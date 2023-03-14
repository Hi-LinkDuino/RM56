/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "cmsis_os2.h"
#include "hdf_log.h"
#include "lcd_abs_if.h"
#include "hdf_device_desc.h"
#include "device_resource_if.h"
#include "hal_gpio.h"
#include "hal_iomux.h"

#ifdef CONFIG_DISPLAY_ICNA3311
#define WIDTH 280
#define HEIGHT 456

static uint8_t g_payLoad0[] = {0xfe, 0x00};
static uint8_t g_payLoad1[] = {0x35, 0x00};
static uint8_t g_payLoad2[] = {0x36, 0x08};
static uint8_t g_payLoad3[] = {0x53, 0x20};
static uint8_t g_payLoad4[] = {0x51, 0xff};
static uint8_t g_payLoad5[] = {0x63, 0xff};
static uint8_t g_payLoad6[] = {0x2a, 0x00, 0x14, 0x01, 0x2B};
static uint8_t g_payLoad7[] = {0x2b, 0x00, 0x00, 0x01, 0xC7};
static uint8_t g_payLoad8[] = {0x11};
static uint8_t g_payLoad9[] = {0x29};
static uint8_t g_payLoad10[] = {0x28};
static uint8_t g_payLoad11[] = {0x10};

static struct DsiCmdDesc g_OnCmd[] = {
    {0x15, 0, sizeof(g_payLoad0), g_payLoad0},
    {0x15, 0, sizeof(g_payLoad1), g_payLoad1},
    {0x15, 0, sizeof(g_payLoad2), g_payLoad2},
    {0x15, 0, sizeof(g_payLoad3), g_payLoad3},
    {0x15, 0, sizeof(g_payLoad4), g_payLoad4},
    {0x15, 0, sizeof(g_payLoad5), g_payLoad5},
    {0x39, 0, sizeof(g_payLoad6), g_payLoad6},
    {0x39, 0, sizeof(g_payLoad7), g_payLoad7},
    {0x05, 120, sizeof(g_payLoad8), g_payLoad8},
    {0x05, 120, sizeof(g_payLoad9), g_payLoad9},
};

static struct DsiCmdDesc g_offCmd[] = {
    {0x15, 0, sizeof(g_payLoad0), g_payLoad0},
    {0x05, 20, sizeof(g_payLoad10), g_payLoad10},
    {0x05, 120, sizeof(g_payLoad11), g_payLoad11},
};

struct PanelDevice {
    struct PanelData panelData;
    struct PanelInfo panelInfo;
    DevHandle mipiHandle;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_te;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_vci;
};

static struct PanelDevice priv = {
    .panelInfo = {
        .width = WIDTH,
        .height = HEIGHT,
        .hbp = 8,
        .hfp = 8,
        .hsw = 2,
        .vbp = 8,
        .vfp = 8,
        .vsw = 2,
        .frameRate = 60,
        .intfType = MIPI_DSI,
        .intfSync = OUTPUT_USER,
        .mipi = {DSI_1_LANES, DSI_CMD_MODE, VIDEO_BURST_MODE, FORMAT_RGB_24_BIT},
    },
    .pin_rst = {
        HAL_GPIO_PIN_P0_3,
        HAL_IOMUX_FUNC_AS_GPIO,
        HAL_IOMUX_PIN_VOLTAGE_VIO,
        HAL_IOMUX_PIN_NOPULL,
    },
    .pin_te = {
        HAL_IOMUX_PIN_P2_1,
        HAL_IOMUX_FUNC_DISPLAY_TE,
        HAL_IOMUX_PIN_VOLTAGE_VIO,
        HAL_IOMUX_PIN_NOPULL,
    },
    .pin_vci = {
        HAL_IOMUX_PIN_P0_2,
        HAL_IOMUX_FUNC_AS_GPIO,
        HAL_IOMUX_PIN_VOLTAGE_VIO,
        HAL_IOMUX_PIN_NOPULL,
    },
};

static int32_t PanelInit(struct PanelData *panel)
{
    if (panel == NULL || panel->info == NULL) {
        return HDF_FAILURE;
    }
    priv.mipiHandle = MipiDsiOpen(0);
    if (priv.mipiHandle == NULL) {
        HDF_LOGE("%s: MipiDsiOpen failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: width %d, height %d", __func__, WIDTH, HEIGHT);

    struct PanelInfo *panelInfo = panel->info;
    struct MipiCfg cfg = {0};
    cfg.lane = panelInfo->mipi.lane;
    cfg.mode = panelInfo->mipi.mode;
    cfg.format = panelInfo->mipi.format;
    cfg.burstMode = panelInfo->mipi.burstMode;
    cfg.timing.xPixels = panelInfo->width;
    cfg.timing.hsaPixels = panelInfo->hsw;
    cfg.timing.hbpPixels = panelInfo->hbp;
    cfg.timing.hlinePixels = panelInfo->width + panelInfo->hbp + panelInfo->hfp + panelInfo->hsw;
    cfg.timing.vsaLines = panelInfo->vsw;
    cfg.timing.vbpLines = panelInfo->vbp;
    cfg.timing.vfpLines = panelInfo->vfp;
    cfg.timing.ylines = panelInfo->height;

    cfg.pixelClk = CalcPixelClk(panelInfo);
    cfg.phyDataRate = CalcDataRate(panelInfo);
    /* config mipi device */
    if (MipiDsiSetCfg(priv.mipiHandle, &cfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: MipiDsiSetCfg failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void PanelPowerControl(bool on)
{
    if (on) {
        hal_iomux_init(&priv.pin_vci, 1);
        hal_iomux_init(&priv.pin_rst, 1);
        hal_iomux_init(&priv.pin_te, 1);
        hal_gpio_pin_set_dir(priv.pin_rst.pin, HAL_GPIO_DIR_OUT, 0);
        hal_gpio_pin_set_dir(priv.pin_vci.pin, HAL_GPIO_DIR_OUT, 0);
        hal_gpio_pin_set(priv.pin_vci.pin);
        osDelay(20);
        hal_gpio_pin_set(priv.pin_rst.pin);
        osDelay(20);
    } else {
        osDelay(120);
        hal_gpio_pin_clr(priv.pin_rst.pin);
        osDelay(20);
        hal_gpio_pin_clr(priv.pin_vci.pin);
    }
}

static int32_t PanelOn(struct PanelData *panel)
{
    (void)panel;
    PanelPowerControl(true);
    /* send mipi power on code */
    int32_t count = sizeof(g_OnCmd) / sizeof(g_OnCmd[0]);
    int32_t i;
    for (i = 0; i < count; i++) {
        int32_t ret = MipiDsiTx(priv.mipiHandle, &(g_OnCmd[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: MipiDsiTx failed", __func__);
            return HDF_FAILURE;
        }
    }
    /* set mipi to hs mode */
    MipiDsiSetHsMode(priv.mipiHandle);
    return HDF_SUCCESS;
}

static int32_t PanelOff(struct PanelData *panel)
{
    (void)panel;
    /* send mipi power off code */
    int32_t count = sizeof(g_offCmd) / sizeof(g_offCmd[0]);
    int32_t i;
    for (i = 0; i < count; i++) {
        int32_t ret = MipiDsiTx(priv.mipiHandle, &(g_offCmd[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: MipiDsiTx failed", __func__);
            return HDF_FAILURE;
        }
    }
    /* set mipi to lp mode */
    MipiDsiSetLpMode(priv.mipiHandle);
    PanelPowerControl(false);
    return HDF_SUCCESS;
}

static int32_t PanelSetBacklight(struct PanelData *panel, uint32_t level)
{
    (void)panel;
    uint8_t payload[] = {0x51, level & 0xff};
    struct DsiCmdDesc cmd = {0x15, 0, sizeof(payload), payload};
    if (MipiDsiTx(priv.mipiHandle, &cmd) != HDF_SUCCESS) {
        HDF_LOGE("%s: MipiDsiTx failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

#define REAL_PIN(n) (n / 10 * 8 + n % 10)
static uint32_t PanelGetResource(struct PanelDevice *priv, const struct DeviceResourceNode *resourceNode)
{
    struct DeviceResourceIface *res = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (res == NULL || res->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid");
        return HDF_FAILURE;
    }
    uint32_t temp = 0;
    if (res->GetUint32(resourceNode, "rst_pin", &temp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get rst_pin", __func__);
        return HDF_FAILURE;
    }
    priv->pin_rst.pin = REAL_PIN(temp);
    if (res->GetUint32(resourceNode, "te_pin", &temp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get te_pin", __func__);
        return HDF_FAILURE;
    }
    priv->pin_te.pin = REAL_PIN(temp);
    if (res->GetUint32(resourceNode, "vci_pin", &temp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get vci_pin", __func__);
        return HDF_FAILURE;
    }
    priv->pin_vci.pin = REAL_PIN(temp);
    HDF_LOGD("%s: rst_pin=%d, te_pin=%d, vci_pin=%d", __func__, priv->pin_rst.pin, priv->pin_te.pin, priv->pin_vci.pin);
    return HDF_SUCCESS;
}

static int32_t PanelDriverInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        return HDF_FAILURE;
    }
    if (object->property) {
        if (PanelGetResource(&priv, object->property) != HDF_SUCCESS) {
            HDF_LOGE("%s: PanelGetResource failed", __func__);
            return HDF_FAILURE;
        }
    }
    priv.panelData.info = &priv.panelInfo;
    priv.panelData.init = PanelInit;
    priv.panelData.on = PanelOn;
    priv.panelData.off = PanelOff;
    priv.panelData.setBacklight = PanelSetBacklight;
    priv.panelData.object = object;
    if (RegisterPanel(&priv.panelData) != HDF_SUCCESS) {
        HDF_LOGE("%s: RegisterPanel failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t PanelDriverBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void PanelDriverRelease(struct HdfDeviceObject *device)
{
    (void)device;
}

static struct HdfDriverEntry g_ICNA3311DriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PANEL_ICNA3311",
    .Bind = PanelDriverBind,
    .Init = PanelDriverInit,
    .Release = PanelDriverRelease,
};

HDF_INIT(g_ICNA3311DriverEntry);
#endif