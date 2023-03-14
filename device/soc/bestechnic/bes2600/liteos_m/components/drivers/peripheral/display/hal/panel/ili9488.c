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

#ifdef CONFIG_DISPLAY_ILI9488
#define WIDTH 320
#define HEIGHT 480

static void PanelPowerControl(bool on);
static int32_t PanelCheckStatus(struct PanelData *panel);
static int32_t PanelReadId();

static uint8_t cmd0[] = {0xE0, 0x00, 0x13, 0x18, 0x04, 0x0F, 0x06, 0x3A, 0x56, 0x4D, 0x03, 0x0A, 0x06, 0x30, 0x3E, 0x0F};
static uint8_t cmd1[] = {0xE1, 0x00, 0x13, 0x18, 0x01, 0x11, 0x06, 0x38, 0x34, 0x4D, 0x06, 0x0D, 0x0B, 0x31, 0x37, 0x0F};
static uint8_t cmd2[] = {0x35, 0x00};
static uint8_t cmd3[] = {0XC0, 0x18, 0x17};       // Power Control 1
static uint8_t cmd4[] = {0xC1, 0x41};             // Power Control 2
static uint8_t cmd5[] = {0xC5, 0x00, 0x1A, 0x80}; // Power Control 3
static uint8_t cmd6[] = {0x36, 0x48};
static uint8_t cmd7[] = {0x3A, 0x66}; // Interface Pixel Format //18bit
static uint8_t cmd8[] = {0XB0, 0x00};
static uint8_t cmd9[] = {0xB1, 0xA0};                    // Frame rate 60HZ
static uint8_t cmd10[] = {0xB4, 0x02};                   // Display Inversion Control //2-dot
static uint8_t cmd11[] = {0xB6, 0x02, 0x02};             // RGB/MCU Interface Control //MCU //Source,Gate scan dieection
static uint8_t cmd12[] = {0xE9, 0x00};                   // Set Image Function,0x00 disable 24 bit data input
static uint8_t cmd13[] = {0xF7, 0xA9, 0x51, 0x2C, 0x82}; // Adjust Control
static uint8_t cmd14[] = {0x21};                         // Normal Black
static uint8_t cmd15[] = {0x11};                         // Sleep out
static uint8_t cmd16[] = {0x29};                         // Display on
static uint8_t cmd17[] = {0x28};
static uint8_t cmd18[] = {0x10};

static struct DsiCmdDesc g_OnCmd[] = {
    {0x39, 0, sizeof(cmd0), cmd0},
    {0x39, 0, sizeof(cmd1), cmd1},
    {0x15, 0, sizeof(cmd2), cmd2},
    {0x39, 0, sizeof(cmd3), cmd3},
    {0x15, 0, sizeof(cmd4), cmd4},
    {0x39, 0, sizeof(cmd5), cmd5},
    {0x15, 0, sizeof(cmd6), cmd6},
    {0x15, 0, sizeof(cmd7), cmd7},
    {0x15, 0, sizeof(cmd8), cmd8},
    {0x15, 0, sizeof(cmd9), cmd9},
    {0x15, 0, sizeof(cmd10), cmd10},
    {0x39, 0, sizeof(cmd11), cmd11},
    {0x15, 0, sizeof(cmd12), cmd12},
    {0x39, 0, sizeof(cmd13), cmd13},
    {0x05, 0, sizeof(cmd14), cmd14},
    {0x05, 150, sizeof(cmd15), cmd15},
    {0x05, 50, sizeof(cmd16), cmd16},
};

struct DsiCmdDesc g_offCmd[] = {
    {0x05, 150, sizeof(cmd17), cmd17},
    {0x05, 50, sizeof(cmd18), cmd18},
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
        .hbp = 15,
        .hfp = 15,
        .hsw = 5,
        .vbp = 8,
        .vfp = 8,
        .vsw = 4,
        .frameRate = 60,
        .intfType = MIPI_DSI,
        .intfSync = OUTPUT_USER,
        .mipi = {DSI_1_LANES, DSI_VIDEO_MODE, VIDEO_BURST_MODE, FORMAT_RGB_24_BIT},
    },
    .panelEsd = {
        .support = true,
        .interval = 30 * 1000,
        .recoveryNum = 5,
        .checkFunc = PanelCheckStatus,
    },
    .pin_rst = {
        HAL_GPIO_PIN_P2_0,
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
        HAL_IOMUX_PIN_P0_3,
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
        hal_iomux_init(&priv.pin_te, 1);
        hal_gpio_pin_set_dir(priv.pin_rst.pin, HAL_GPIO_DIR_OUT, 1);
        osDelay(10);
        hal_gpio_pin_clr(priv.pin_rst.pin);
        osDelay(50);
        hal_gpio_pin_set(priv.pin_rst.pin);
        osDelay(150);
    } else {
        osDelay(50);
        hal_gpio_pin_clr(priv.pin_rst.pin);
        osDelay(150);
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
    osDelay(100);
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

static struct HdfDriverEntry g_ili9488DriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PANEL_ILI9488",
    .Bind = PanelDriverBind,
    .Init = PanelDriverInit,
    .Release = PanelDriverRelease,
};

HDF_INIT(g_ili9488DriverEntry);
#endif