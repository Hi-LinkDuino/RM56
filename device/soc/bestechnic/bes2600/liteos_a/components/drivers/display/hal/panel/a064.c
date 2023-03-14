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
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_dsi.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif

#ifdef CONFIG_DISPLAY_A064
#define WIDTH 480
#define HEIGHT 480

static void PanelPowerControl(bool on);
static int32_t PanelCheckStatus(struct PanelData *panel);
static int32_t PanelReadId();

static uint8_t cmd0[] = {0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00};
static uint8_t cmd1[] = {0xF6, 0x5A, 0x87};
static uint8_t cmd2[] = {0x3A, 0x70};
static uint8_t cmd3[] = {0xC1, 0x3F};
static uint8_t cmd4[] = {0xC2, 0x0E};
static uint8_t cmd5[] = {0xC6, 0xF8};
static uint8_t cmd6[] = {0xC9, 0x10};
static uint8_t cmd7[] = {0xCD, 0x25};
static uint8_t cmd8[] = {0xF8, 0x8A};
static uint8_t cmd9[] = {0xAC, 0x65};
static uint8_t cmd10[] = {0xA0, 0xDD};
static uint8_t cmd11[] = {0xA7, 0x47};
static uint8_t cmd12[] = {0xFA, 0x00, 0x00, 0x00, 0x04};
static uint8_t cmd13[] = {0x86, 0x99, 0xa3, 0xa3, 0x51};
static uint8_t cmd14[] = {0xA3, 0xEE};
static uint8_t cmd15[] = {0xFD, 0x3c, 0x3c, 0x00};
static uint8_t cmd16[] = {0x71, 0x48};
static uint8_t cmd17[] = {0x72, 0x48};
static uint8_t cmd18[] = {0x73, 0x00, 0x44};
static uint8_t cmd19[] = {0x97, 0xEE};
static uint8_t cmd20[] = {0x83, 0x93};
static uint8_t cmd21[] = {0x9A, 0x79};
static uint8_t cmd22[] = {0x9B, 0x62};
static uint8_t cmd23[] = {0x82, 0x34, 0x34};
static uint8_t cmd24[] = {0xB1, 0x90}; // 10 REV-0-BGR-X-X-X-GS-SS   //mipi
static uint8_t cmd25[] = {0x6D, 0x00, 0x1F, 0x19, 0x1A, 0x10, 0x0e, 0x0c, 0x0a, 0x02, 0x07, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x08, 0x01, 0x09, 0x0b, 0x0D, 0x0F, 0x1a, 0x19, 0x1f, 0x00};
static uint8_t cmd26[] = {0x64, 0x38, 0x05, 0x01, 0xdb, 0x03, 0x03, 0x38, 0x04, 0x01, 0xdc, 0x03, 0x03, 0x7A, 0x7A, 0x7A, 0x7A};
static uint8_t cmd27[] = {0x65, 0x38, 0x03, 0x01, 0xdd, 0x03, 0x03, 0x38, 0x02, 0x01, 0xde, 0x03, 0x03, 0x7A, 0x7A, 0x7A, 0x7A};
static uint8_t cmd28[] = {0x66, 0x38, 0x01, 0x01, 0xdf, 0x03, 0x03, 0x38, 0x00, 0x01, 0xe0, 0x03, 0x03, 0x7A, 0x7A, 0x7A, 0x7A};
static uint8_t cmd29[] = {0x67, 0x30, 0x01, 0x01, 0xe1, 0x03, 0x03, 0x30, 0x02, 0x01, 0xe2, 0x03, 0x03, 0x7A, 0x7A, 0x7A, 0x7A};
static uint8_t cmd30[] = {0x68, 0x00, 0x08, 0x15, 0x08, 0x15, 0x7A, 0x7A, 0x08, 0x15, 0x08, 0x15, 0x7A, 0x7A};
static uint8_t cmd31[] = {0x60, 0x38, 0x07, 0x7A, 0x7A, 0x38, 0x08, 0x7A, 0x7A};
static uint8_t cmd32[] = {0x63, 0x31, 0xe4, 0x7A, 0x7A, 0x31, 0xe5, 0x7A, 0x7A};
static uint8_t cmd33[] = {0x69, 0x04, 0x22, 0x14, 0x22, 0x14, 0x22, 0x08};
static uint8_t cmd34[] = {0x6B, 0x07};
static uint8_t cmd35[] = {0x7A, 0x08, 0x13};
static uint8_t cmd36[] = {0x7B, 0x08, 0x13};
static uint8_t cmd37[] = {0xD1, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd38[] = {0xD2, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd39[] = {0xD3, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd40[] = {0xD4, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd41[] = {0xD5, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd42[] = {0xD6, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd43[] = {0x11};
static uint8_t cmd44[] = {0x29};
static uint8_t cmd45[] = {0x28};
static uint8_t cmd46[] = {0x10};

static struct DsiCmdDesc g_OnCmd[] = {
    {0x39, 0, sizeof(cmd0), cmd0},
    {0x39, 0, sizeof(cmd1), cmd1},
    {0x15, 0, sizeof(cmd2), cmd2},
    {0x15, 0, sizeof(cmd3), cmd3},
    {0x15, 0, sizeof(cmd4), cmd4},
    {0x15, 0, sizeof(cmd5), cmd5},
    {0x15, 0, sizeof(cmd6), cmd6},
    {0x15, 0, sizeof(cmd7), cmd7},
    {0x15, 0, sizeof(cmd8), cmd8},
    {0x15, 0, sizeof(cmd9), cmd9},
    {0x15, 0, sizeof(cmd10), cmd10},
    {0x15, 0, sizeof(cmd11), cmd11},
    {0x39, 0, sizeof(cmd12), cmd12},
    {0x39, 0, sizeof(cmd13), cmd13},
    {0x15, 0, sizeof(cmd14), cmd14},
    {0x39, 0, sizeof(cmd15), cmd15},
    {0x15, 0, sizeof(cmd16), cmd16},
    {0x15, 0, sizeof(cmd17), cmd17},
    {0x39, 0, sizeof(cmd18), cmd18},
    {0x15, 0, sizeof(cmd19), cmd19},
    {0x15, 0, sizeof(cmd20), cmd20},
    {0x15, 0, sizeof(cmd21), cmd21},
    {0x15, 0, sizeof(cmd22), cmd22},
    {0x39, 0, sizeof(cmd23), cmd23},
    {0x15, 0, sizeof(cmd24), cmd24},
    {0x39, 0, sizeof(cmd25), cmd25},
    {0x39, 0, sizeof(cmd26), cmd26},
    {0x39, 0, sizeof(cmd27), cmd27},
    {0x39, 0, sizeof(cmd28), cmd28},
    {0x39, 0, sizeof(cmd29), cmd29},
    {0x39, 0, sizeof(cmd30), cmd30},
    {0x39, 0, sizeof(cmd31), cmd31},
    {0x39, 0, sizeof(cmd32), cmd32},
    {0x39, 0, sizeof(cmd33), cmd33},
    {0x15, 0, sizeof(cmd34), cmd34},
    {0x39, 0, sizeof(cmd35), cmd35},
    {0x39, 0, sizeof(cmd36), cmd36},
    {0x39, 0, sizeof(cmd37), cmd37},
    {0x39, 0, sizeof(cmd38), cmd38},
    {0x39, 0, sizeof(cmd39), cmd39},
    {0x39, 0, sizeof(cmd40), cmd40},
    {0x39, 0, sizeof(cmd41), cmd41},
    {0x39, 0, sizeof(cmd42), cmd42},
    {0x05, 120, sizeof(cmd43), cmd43},
    {0x05, 20, sizeof(cmd44), cmd44},
};

struct DsiCmdDesc g_offCmd[] = {
    {0x05, 20, sizeof(cmd45), cmd45},
    {0x05, 120, sizeof(cmd46), cmd46},
};

struct PanelDevice {
    struct PanelData panelData;
    struct PanelInfo panelInfo;
    struct PanelEsd panelEsd;
    DevHandle mipiHandle;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_te;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_led;
};

static struct PanelDevice priv = {
    .panelInfo = {
        .width = WIDTH,
        .height = HEIGHT,
        .hbp = 10,
        .hfp = 10,
        .hsw = 8,
        .vbp = 20,
        .vfp = 20,
        .vsw = 8,
        .frameRate = 60,
        .intfType = MIPI_DSI,
        .intfSync = OUTPUT_USER,
        .mipi = {DSI_2_LANES, DSI_VIDEO_MODE, VIDEO_BURST_MODE, FORMAT_RGB_24_BIT},
    },
    .panelEsd = {
        .support = true,
        .interval = 30 * 1000,
        .recoveryNum = 5,
        .checkFunc = PanelCheckStatus,
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
    .pin_led = {
        HAL_IOMUX_PIN_P2_6,
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
    PanelPowerControl(true);

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
    PanelReadId();
    return HDF_SUCCESS;
}

static void PanelPowerControl(bool on)
{
    if (on) {
        hal_iomux_init(&priv.pin_led, 1);
        hal_iomux_init(&priv.pin_rst, 1);
        // hal_iomux_init(&priv.pin_te, 1);
        hal_gpio_pin_set_dir(priv.pin_rst.pin, HAL_GPIO_DIR_OUT, 1);
        osDelay(10);
        hal_gpio_pin_clr(priv.pin_rst.pin);
        osDelay(10);
        hal_gpio_pin_set(priv.pin_rst.pin);
        osDelay(10);
    } else {
        osDelay(20);
        hal_gpio_pin_clr(priv.pin_rst.pin);
        osDelay(120);
        hal_gpio_pin_clr(priv.pin_led.pin);
    }
}

static int32_t PanelReadId()
{
    uint8_t read_id[3] = {0};
    uint8_t payload[] = {0x04};
    struct DsiCmdDesc cmd = {0x06, 0, sizeof(payload), payload};
    int32_t ret = MipiDsiRx(priv.mipiHandle, &cmd, 3, read_id);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: MipiDsiRx failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: read id %02X-%02X-%02X", __func__, read_id[0], read_id[1], read_id[2]);
    return HDF_SUCCESS;
}
#define DSI_INIT_DELAY 100
static int32_t PanelCheckStatus(struct PanelData *panel)
{
    uint8_t powerMode = 0;
    uint8_t payload[] = {0x0A};
    struct DsiCmdDesc cmd = {0x06, 0, sizeof(payload), payload};
    int64_t cnt = panel->esd->recoveryNum;
    do {
        int32_t ret = MipiDsiRx(priv.mipiHandle, &cmd, 1, &powerMode);
        if (ret == HDF_SUCCESS) {
            HDF_LOGD("%s: powerMode 0x%x ok", __func__, powerMode);
            break;
        }
        HDF_LOGE("%s: reset", __func__);
        hal_dsi_init(WIDTH, priv.panelInfo.mipi.lane);
        osDelay(DSI_INIT_DELAY);
        for (int32_t i = 0; i < sizeof(g_OnCmd) / sizeof(g_OnCmd[0]); i++) {
            ret = MipiDsiTx(priv.mipiHandle, &(g_OnCmd[i]));
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s: MipiDsiTx failed", __func__);
                return HDF_FAILURE;
            }
        }
    } while (--cnt > 0);
    if (cnt < panel->esd->recoveryNum) {
        hal_dsi_start();
    }
    return HDF_SUCCESS;
}

static int32_t PanelOn(struct PanelData *panel)
{
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
    PanelCheckStatus(panel);
    hal_gpio_pin_set_dir(priv.pin_led.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(20);
    return HDF_SUCCESS;
}

static int32_t PanelOff(struct PanelData *panel)
{
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
    PanelCheckStatus(panel);
    return HDF_SUCCESS;
}
#define REAL_PIN(n) (n / 10 * 8 + n % 10)
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define DISPLAY_PANEL_CONFIG HCS_NODE(HCS_NODE(HCS_NODE(HCS_ROOT, display), panel_config), a064_config)
static uint32_t PanelGetResource(struct PanelDevice *priv)
{
    uint32_t temp;
    temp = HCS_PROP(DISPLAY_PANEL_CONFIG, rst_pin);
    priv->pin_rst.pin = REAL_PIN(temp);

    temp = HCS_PROP(DISPLAY_PANEL_CONFIG, te_pin);
    priv->pin_te.pin = REAL_PIN(temp);

    temp = HCS_PROP(DISPLAY_PANEL_CONFIG, led_pin);
    priv->pin_led.pin = REAL_PIN(temp);

    HDF_LOGD("%s: rst_pin=%d, te_pin=%d, led_pin=%d", __func__, priv->pin_rst.pin, priv->pin_te.pin, priv->pin_led.pin);
    return HDF_SUCCESS;
}
#else
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
    if (res->GetUint32(resourceNode, "led_pin", &temp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get led_pin", __func__);
        return HDF_FAILURE;
    }
    priv->pin_led.pin = REAL_PIN(temp);
    HDF_LOGD("%s: rst_pin=%d, te_pin=%d, led_pin=%d", __func__, priv->pin_rst.pin, priv->pin_te.pin, priv->pin_led.pin);
    return HDF_SUCCESS;
}
#endif
static int32_t PanelDriverInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        return HDF_FAILURE;
    }
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (PanelGetResource(&priv) != HDF_SUCCESS) {
        HDF_LOGE("%s: PanelGetResource failed", __func__);
        return HDF_FAILURE;
    }
#else
    if (object->property) {
        if (PanelGetResource(&priv, object->property) != HDF_SUCCESS) {
            HDF_LOGE("%s: PanelGetResource failed", __func__);
            return HDF_FAILURE;
        }
    }
#endif
    priv.panelData.info = &priv.panelInfo;
    priv.panelData.esd = &priv.panelEsd;
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

static struct HdfDriverEntry g_A064DriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PANEL_A064",
    .Bind = PanelDriverBind,
    .Init = PanelDriverInit,
    .Release = PanelDriverRelease,
};

HDF_INIT(g_A064DriverEntry);
#endif
