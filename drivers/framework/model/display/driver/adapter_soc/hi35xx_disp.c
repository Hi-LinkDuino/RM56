/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi35xx_disp.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_disp.h"
#include "hdf_log.h"
#include "osal_io.h"
#include "pwm_if.h"

#define TRANSFORM_KILO 1000
#define TRANSFORM_MILL 1000000

static void MipiMuxCfg(unsigned long ioCfgBase)
{
    /* config dsi data lane0 */
    OSAL_WRITEL(0x670, ioCfgBase + 0x0088);
    OSAL_WRITEL(0x670, ioCfgBase + 0x0084);
    /* config dsi data lane1 */
    OSAL_WRITEL(0x470, ioCfgBase + 0x007C);
    OSAL_WRITEL(0x470, ioCfgBase + 0x0080);
    /* config dsi data lane2 */
    OSAL_WRITEL(0x470, ioCfgBase + 0x006C);
    OSAL_WRITEL(0x470, ioCfgBase + 0x0070);
    /* config dsi data lane3 */
    OSAL_WRITEL(0x670, ioCfgBase + 0x0068);
    OSAL_WRITEL(0x670, ioCfgBase + 0x0064);
    /* config dsi clock lane */
    OSAL_WRITEL(0x460, ioCfgBase + 0x0074);
    OSAL_WRITEL(0x460, ioCfgBase + 0x0078);
}

static void Lcd6BitMuxCfg(unsigned long ioCfgBase)
{
    OSAL_WRITEL(0x4f4, ioCfgBase + 0x0068);
    OSAL_WRITEL(0x454, ioCfgBase + 0x0084);
    OSAL_WRITEL(0x474, ioCfgBase + 0x007c);
    OSAL_WRITEL(0x674, ioCfgBase + 0x0088);
    OSAL_WRITEL(0x474, ioCfgBase + 0x0080);
    OSAL_WRITEL(0x474, ioCfgBase + 0x0074);
    OSAL_WRITEL(0x474, ioCfgBase + 0x0078);
    OSAL_WRITEL(0x474, ioCfgBase + 0x006C);
    OSAL_WRITEL(0x474, ioCfgBase + 0x0070);
    OSAL_WRITEL(0x674, ioCfgBase + 0x0064);
}

static void Lcd8BitMuxCfg(unsigned long ioCfgBase)
{
    OSAL_WRITEL(0x422, ioCfgBase + 0x0034);
    OSAL_WRITEL(0x462, ioCfgBase + 0x0058);
    OSAL_WRITEL(0x462, ioCfgBase + 0x004c);
    OSAL_WRITEL(0x462, ioCfgBase + 0x0054);
    OSAL_WRITEL(0x422, ioCfgBase + 0x0048);
    OSAL_WRITEL(0x622, ioCfgBase + 0x0040);
    OSAL_WRITEL(0x622, ioCfgBase + 0x0044);
    OSAL_WRITEL(0x622, ioCfgBase + 0x005C);
    OSAL_WRITEL(0x622, ioCfgBase + 0x003c);
    OSAL_WRITEL(0x422, ioCfgBase + 0x0038);
    OSAL_WRITEL(0x622, ioCfgBase + 0x0050);
    OSAL_WRITEL(0x462, ioCfgBase + 0x0060);
}

void Lcd24BitMuxCfg(unsigned long ioCfgBase)
{
    OSAL_WRITEL(0x462, ioCfgBase + 0x0034);
    OSAL_WRITEL(0x432, ioCfgBase + 0x0058);
    OSAL_WRITEL(0x462, ioCfgBase + 0x004C);
    OSAL_WRITEL(0x432, ioCfgBase + 0x0054);
    OSAL_WRITEL(0x432, ioCfgBase + 0x0048);
    OSAL_WRITEL(0x632, ioCfgBase + 0x0040);
    OSAL_WRITEL(0x632, ioCfgBase + 0x0044);
    OSAL_WRITEL(0x632, ioCfgBase + 0x005C);
    OSAL_WRITEL(0x632, ioCfgBase + 0x003C);
    OSAL_WRITEL(0x432, ioCfgBase + 0x0038);
    OSAL_WRITEL(0x632, ioCfgBase + 0x0050);
    OSAL_WRITEL(0x462, ioCfgBase + 0x0060);
    OSAL_WRITEL(0x672, ioCfgBase + 0x0084);
    OSAL_WRITEL(0x672, ioCfgBase + 0x0088);
    OSAL_WRITEL(0x472, ioCfgBase + 0x007C);
    OSAL_WRITEL(0x472, ioCfgBase + 0x0080);
    OSAL_WRITEL(0x472, ioCfgBase + 0x0074);
    OSAL_WRITEL(0x472, ioCfgBase + 0x0078);
    OSAL_WRITEL(0x472, ioCfgBase + 0x006C);
    OSAL_WRITEL(0x462, ioCfgBase + 0x0070);
    OSAL_WRITEL(0x672, ioCfgBase + 0x0064);
    OSAL_WRITEL(0x672, ioCfgBase + 0x0068);
    OSAL_WRITEL(0x532, ioCfgBase + 0x0094);
    OSAL_WRITEL(0x532, ioCfgBase + 0x0090);
    OSAL_WRITEL(0x532, ioCfgBase + 0x008C);
    OSAL_WRITEL(0x632, ioCfgBase + 0x0098);
    OSAL_WRITEL(0x632, ioCfgBase + 0x009C);
    OSAL_WRITEL(0x532, ioCfgBase + 0x0030);
}

static void LcdPinMuxCfg(uint32_t intf)
{
    unsigned long ioCfgBase;

    ioCfgBase = (unsigned long)OsalIoRemap(IO_CFG2_BASE, IO_CFG_SIZE);
    if (intf == MIPI_DSI) {
        MipiMuxCfg(ioCfgBase);
    } else if (intf == LCD_6BIT) {
        Lcd6BitMuxCfg(ioCfgBase);
    } else if (intf == LCD_8BIT) {
        Lcd8BitMuxCfg(ioCfgBase);
    } else if (intf == LCD_24BIT) {
        Lcd24BitMuxCfg(ioCfgBase);
    } else {
        HDF_LOGE("%s: not support intf: %d", __func__, intf);
    }
}

static void PwmPinMuxCfg(uint32_t dev)
{
    /* pwm pin config */
    unsigned long ioCfgBase;
    ioCfgBase = (unsigned long)OsalIoRemap(IO_CFG1_BASE, IO_CFG_SIZE);
    switch (dev) {
        case PWM_DEV0:
            OSAL_WRITEL(0x601, ioCfgBase + 0x0024);
            break;
        case PWM_DEV1:
            OSAL_WRITEL(0x601, ioCfgBase + 0x0028);
            break;
        default:
            HDF_LOGE("%s: not support pwm dev: %d", __func__, dev);
            break;
    }
}

static int32_t GetBitsPerPixel(enum DsiOutFormat format)
{
    int32_t bpp;

    switch (format) {
        case FORMAT_RGB_16_BIT:
            bpp = 16; // 16 bits per pixel
            break;
        case FORMAT_RGB_18_BIT:
            bpp = 18; // 18 bits per pixel
            break;
        case FORMAT_RGB_24_BIT:
            bpp = 24; // 24 bits per pixel
            break;
        default:
            bpp = 24; // 24 bits per pixel
            break;
    }
    return bpp;
}

static uint32_t CalcPixelClk(struct PanelInfo *info)
{
    uint16_t hpixel;
    uint16_t vline;

    hpixel = info->width + info->hbp + info->hfp + info->hsw;
    vline = info->height + info->vbp + info->vfp + info->vsw;
    uint32_t pixNum = hpixel * vline * info->frameRate;
    if ((pixNum % TRANSFORM_KILO) == 0) {
        return pixNum / TRANSFORM_KILO;
    }
    return (pixNum / TRANSFORM_KILO + 1);
}

static uint32_t CalcDataRate(struct PanelInfo *info)
{
    uint16_t hpixel;
    uint16_t vline;
    uint32_t bitClk;

    hpixel = info->width + info->hbp + info->hfp + info->hsw;
    vline = info->height + info->vbp + info->vfp + info->vsw;
    int32_t bpp = GetBitsPerPixel(info->mipi.format);
    uint32_t bitNum = hpixel * vline * info->frameRate * bpp;
    if ((bitNum % TRANSFORM_MILL) == 0) {
        bitClk = bitNum / TRANSFORM_MILL;
    } else {
        bitClk = bitNum / TRANSFORM_MILL + 1;
    }
    if (!info->mipi.lane) {
        return HDF_FAILURE;
    }
    if ((bitClk % info->mipi.lane) == 0) {
        return bitClk / info->mipi.lane;
    }
    return (bitClk / info->mipi.lane) + 1;
}

static int32_t MipiDsiInit(struct PanelInfo *info)
{
    int32_t ret;
    DevHandle mipiHandle = NULL;
    struct MipiCfg cfg;

    mipiHandle = MipiDsiOpen(0);
    if (mipiHandle == NULL) {
        HDF_LOGE("%s: MipiDsiOpen failed", __func__);
        return HDF_FAILURE;
    }
    cfg.lane = info->mipi.lane;
    cfg.mode = info->mipi.mode;
    cfg.format = info->mipi.format;
    cfg.burstMode = info->mipi.burstMode;
    cfg.timing.xPixels = info->width;
    cfg.timing.hsaPixels = info->hsw;
    cfg.timing.hbpPixels = info->hbp;
    cfg.timing.hlinePixels = info->width + info->hbp + info->hfp + info->hsw;
    cfg.timing.vsaLines = info->vsw;
    cfg.timing.vbpLines = info->vbp;
    cfg.timing.vfpLines = info->vfp;
    cfg.timing.ylines = info->height;
    /* 0 : no care */
    cfg.timing.edpiCmdSize = 0;
    cfg.pixelClk = CalcPixelClk(info);
    cfg.phyDataRate = CalcDataRate(info);
    /* config mipi device */
    ret = MipiDsiSetCfg(mipiHandle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:MipiDsiSetCfg failed", __func__);
    }
    MipiDsiClose(mipiHandle);
    HDF_LOGI("%s:pixelClk = %d, phyDataRate = %u", __func__, cfg.pixelClk, cfg.phyDataRate);
    return ret;
}

static int32_t GetLcdIntfType(enum LcdIntfType type, uint32_t *out)
{
    int32_t ret = HDF_SUCCESS;

    switch (type) {
        case MIPI_DSI:
            *out = INTF_MIPI;
            break;
        case LCD_6BIT:
            *out = INTF_LCD_6BIT;
            break;
        case LCD_8BIT:
            *out = INTF_LCD_8BIT;
            break;
        case LCD_16BIT:
            *out = INTF_LCD_16BIT;
            break;
        case LCD_18BIT:
            *out = INTF_LCD_18BIT;
            break;
        case LCD_24BIT:
            *out = INTF_LCD_24BIT;
            break;
        default:
            HDF_LOGE("%s: not support intf: %d", __func__, type);
            ret = HDF_FAILURE;
            break;
    }
    return ret;
}

static int32_t Hi35xxHardWareInit(void)
{
    int32_t i;
    int32_t ret;
    struct PanelManager *panelManager = NULL;
    struct PanelData *panel = NULL;
    struct PanelInfo *info = NULL;

    panelManager = GetPanelManager();
    if (panelManager == NULL) {
        HDF_LOGE("%s: panelManager is null", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < panelManager->panelNum; i++) {
        panel = panelManager->panel[i];
        info = panel->info;
        if (info == NULL) {
            HDF_LOGE("%s:GetPanelInfo failed", __func__);
            return HDF_FAILURE;
        }
        if (info->blk.type == BLK_PWM) {
            PwmPinMuxCfg(info->pwm.dev);
        }
        /* lcd pin mux config */
        LcdPinMuxCfg(info->intfType);
        if (info->intfType == MIPI_DSI) {
            /* mipi dsi init */
            ret = MipiDsiInit(info);
            if (ret) {
                HDF_LOGE("%s:MipiDsiInit failed", __func__);
                return HDF_FAILURE;
            }
        }
        if (GetLcdIntfType(info->intfType, &info->intfType) != HDF_SUCCESS) {
            HDF_LOGE("%s:GetLcdIntfType failed", __func__);
            return HDF_FAILURE;
        }
        if (panel->init != NULL) {
            if ((panel->init(panel)) != HDF_SUCCESS) {
                HDF_LOGE("%s:panel[%d] init failed", __func__, i);
                return HDF_FAILURE;
            }
        }
    }
    return HDF_SUCCESS;
}

static int32_t Hi35xxEntryInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        HDF_LOGE("%s: object is null", __func__);
        return HDF_FAILURE;
    }
    return Hi35xxHardWareInit();
}

struct HdfDriverEntry g_hi35xxDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HI351XX_DISP",
    .Init = Hi35xxEntryInit,
};

HDF_INIT(g_hi35xxDevEntry);
