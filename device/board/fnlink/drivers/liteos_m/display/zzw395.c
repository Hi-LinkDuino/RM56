/*
 * Copyright (c) 2021 FnLink (hunan) Technologies CO., LIMITED.
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

#ifdef CONFIG_DISPLAY_ZZW395

#define WIDTH 480
#define HEIGHT 480

static uint8_t cmd0[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x10};
static uint8_t cmd1[] = {0xC0, 0x3B, 0x00};
static uint8_t cmd2[] = {0xC1, 0x0D, 0x02};
static uint8_t cmd3[] = {0xC2, 0x21, 0x08};
static uint8_t cmd4[] = {0xB0, 0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08, 0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18};
static uint8_t cmd5[] = {0xB1, 0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08, 0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18};
static uint8_t cmd6[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x11};
static uint8_t cmd7[] = {0xB0, 0x60};
static uint8_t cmd8[] = {0xB1, 0x30};
static uint8_t cmd9[] = {0xB2, 0x87};
static uint8_t cmd10[] = {0xB3, 0x80};
static uint8_t cmd11[] = {0xB5, 0x49};
static uint8_t cmd12[] = {0xB7, 0x85};
static uint8_t cmd13[] = {0xB8, 0x21};
static uint8_t cmd14[] = {0xC1, 0x78};
static uint8_t cmd15[] = {0xC2, 0x78};
static uint8_t cmd16[] = {0xE0, 0x00, 0x1B, 0x02};
static uint8_t cmd17[] = {0xE1, 0x08, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x44, 0x44};
static uint8_t cmd18[] = {0xE2, 0x11, 0x11, 0x44, 0x44, 0xED, 0xA0, 0x00, 0x00, 0xEC, 0xA0, 0x00, 0x00};
static uint8_t cmd19[] = {0xE3, 0x00, 0x00, 0x11, 0x11};
static uint8_t cmd20[] = {0xE4, 0x44, 0x44};
static uint8_t cmd21[] = {0xE5, 0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0, 0x0E, 0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0,};
static uint8_t cmd22[] = {0xE6, 0x00, 0x00, 0x11, 0x11};
static uint8_t cmd23[] = {0xE7, 0x44, 0x44};
static uint8_t cmd24[] = {0xE8, 0x09, 0xE8, 0xD8, 0xA0, 0x0B, 0xEA, 0xD8, 0xA0, 0x0D, 0xEC, 0xD8, 0xA0, 0x0F, 0xEE, 0xD8, 0xA0};
static uint8_t cmd25[] = {0xEB, 0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40};
static uint8_t cmd26[] = {0xEC, 0x3C, 0x00};
static uint8_t cmd27[] = {0xED, 0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA};
static uint8_t cmd28[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x00};
static uint8_t cmd29[] = {0x36, 0x00, 0x77, 0x01, 0x00, 0x00,0x12};
static uint8_t cmd30[] = {0xd1, 0x81};
static uint8_t cmd31[] = {0xd2, 0x08};
static uint8_t cmd32[] = {0x11};
static uint8_t cmd33[] = {0x29};
static uint8_t cmd34[] = {0x28};
static uint8_t cmd35[] = {0x10};

static struct DsiCmdDesc g_OnCmd[] = {
    {0x39, 0, sizeof(cmd0), cmd0},
    {0x39, 0, sizeof(cmd1), cmd1},
    {0x39, 0, sizeof(cmd2), cmd2},
    {0x39, 0, sizeof(cmd3), cmd3},
    {0x39, 0, sizeof(cmd4), cmd4},
    {0x39, 0, sizeof(cmd5), cmd5},
    {0x39, 0, sizeof(cmd6), cmd6},
    {0x15, 0, sizeof(cmd7), cmd7},
    {0x15, 0, sizeof(cmd8), cmd8},
    {0x15, 0, sizeof(cmd9), cmd9},
    {0x15, 0, sizeof(cmd10), cmd10},
    {0x15, 0, sizeof(cmd11), cmd11},
    {0x15, 0, sizeof(cmd12), cmd12},
    {0x15, 0, sizeof(cmd13), cmd13},
    {0x15, 0, sizeof(cmd14), cmd14},
    {0x15, 20, sizeof(cmd15), cmd15},
    {0x39, 0, sizeof(cmd16), cmd16},
    {0x39, 0, sizeof(cmd17), cmd17},
    {0x39, 0, sizeof(cmd18), cmd18},
    {0x39, 0, sizeof(cmd19), cmd19},
    {0x39, 0, sizeof(cmd20), cmd20},
    {0x39, 0, sizeof(cmd21), cmd21},
    {0x39, 0, sizeof(cmd22), cmd22},
    {0x39, 0, sizeof(cmd23), cmd23},
    {0x39, 0, sizeof(cmd24), cmd24},
    {0x39, 0, sizeof(cmd25), cmd25},
    {0x39, 0, sizeof(cmd26), cmd26},
    {0x39, 0, sizeof(cmd27), cmd27},
    {0x39, 0, sizeof(cmd28), cmd28},
    {0x39, 0, sizeof(cmd29), cmd29},
    {0x15, 0, sizeof(cmd30), cmd30},
    {0x15, 0, sizeof(cmd31), cmd31},
    {0x05, 120, sizeof(cmd32), cmd32},
    {0x05, 20, sizeof(cmd33), cmd33},
};

static struct DsiCmdDesc g_offCmd[] = {
    {0x05, 20, sizeof(cmd34), cmd34},
    {0x05, 120, sizeof(cmd35), cmd35},
};

struct PanelDevice {
    struct PanelData panelData;
    struct PanelInfo panelInfo;
    DevHandle mipiHandle;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_te;
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_led;
};

static struct PanelDevice priv = {
    .panelInfo = {
        .width = WIDTH,
        .height = HEIGHT,
        .hbp = 80,
        .hfp = 80,
        .hsw = 8,
        .vbp = 30,
        .vfp = 18,
        .vsw = 2,
        .frameRate = 60,
        .intfType = MIPI_DSI,
        .intfSync = OUTPUT_USER,
        .mipi = {DSI_2_LANES, DSI_VIDEO_MODE, VIDEO_BURST_MODE, FORMAT_RGB_24_BIT},
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
        hal_iomux_init(&priv.pin_led, 1);
        hal_iomux_init(&priv.pin_rst, 1);
        hal_iomux_init(&priv.pin_te, 1);
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
    struct DsiCmdDesc cmd = {0x06, 0, sizeof(payload), &payload};
    int32_t ret = MipiDsiRx(priv.mipiHandle, &cmd, 3, read_id);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: MipiDsiRx failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: read id %02X-%02X-%02X", __func__, read_id[0], read_id[1], read_id[2]);
    return HDF_SUCCESS;
}

static int32_t PanelOn(struct PanelData *panel)
{
    (void)panel;
    PanelPowerControl(true);
    if (PanelReadId() != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
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
    hal_gpio_pin_set_dir(priv.pin_led.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(20);
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
    if (res->GetUint32(resourceNode, "led_pin", &temp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get led_pin", __func__);
        return HDF_FAILURE;
    }
    priv->pin_led.pin = REAL_PIN(temp);
    HDF_LOGD("%s: rst_pin=%d, te_pin=%d, led_pin=%d", __func__, priv->pin_rst.pin, priv->pin_te.pin, priv->pin_led.pin);
    return HDF_SUCCESS;
}

static int32_t PanelDriverInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        return HDF_FAILURE;
    }
    HDF_LOGD("%s entry !!!", __func__);
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

static struct HdfDriverEntry g_ZZW395DriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PANEL_ZZW395",
    .Bind = PanelDriverBind,
    .Init = PanelDriverInit,
    .Release = PanelDriverRelease,
};

HDF_INIT(g_ZZW395DriverEntry);
#endif