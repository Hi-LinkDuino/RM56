/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gpio_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "spi_if.h"
#include "ssp_st7789.h"

#define WIDTH         240
#define HEIGHT        320
#define FRAME_RATE    50

static struct LcdCmd g_onCmd[] = {
    { LCD_CMD,  0x11, 120 },
    { LCD_CMD,  0x36, 0 },
    { LCD_DATA, 0x00, 0 },
    { LCD_CMD,  0xb2, 0 },
    { LCD_DATA, 0x00, 0 },
    { LCD_DATA, 0x00, 0 },
    { LCD_DATA, 0x00, 0 },
    { LCD_DATA, 0x33, 0 },
    { LCD_DATA, 0x33, 0 },
    { LCD_CMD,  0xb7, 0 },
    { LCD_DATA, 0x35, 0 },
    { LCD_CMD,  0xb8, 0 },
    { LCD_DATA, 0x2f, 0 },
    { LCD_DATA, 0x2b, 0 },
    { LCD_DATA, 0x2f, 0 },
    { LCD_CMD,  0xbb, 0 },
    { LCD_DATA, 0x24, 0 },
    { LCD_CMD,  0xc0, 0 },
    { LCD_DATA, 0x2c, 0 },
    { LCD_CMD,  0xc3, 0 },
    { LCD_DATA, 0x20, 0 },
    { LCD_CMD,  0xc4, 0 },
    { LCD_DATA, 0x03, 0 },
    { LCD_CMD,  0xc6, 0 },
    { LCD_DATA, 0x11, 0 },
    { LCD_CMD,  0xd0, 0 },
    { LCD_DATA, 0xa4, 0 },
    { LCD_DATA, 0xa1, 0 },
    { LCD_CMD,  0xe8, 0 },
    { LCD_DATA, 0x03, 0 },
    { LCD_CMD,  0xe9, 0 },
    { LCD_DATA, 0x0d, 0 },
    { LCD_DATA, 0x12, 0 },
    { LCD_DATA, 0x00, 0 },
    { LCD_CMD,  0x21, 0 },
    { LCD_CMD,  0xb0, 0 },
    { LCD_DATA, 0x11, 0 },
    { LCD_DATA, 0x04, 0 },
    { LCD_DATA, 0x00, 0 },
    { LCD_CMD,  0xb1, 0 },
    { LCD_DATA, 0xc0, 0 },
    { LCD_DATA, 0x02, 0 },
    { LCD_DATA, 0x14, 0 },
    { LCD_CMD,  0x3a, 0 },
    { LCD_DATA, 0x66, 0 },
    { LCD_CMD,  0x11, 0 },
    { LCD_CMD,  0x29, 0 },
    { LCD_CMD,  0x2c, 0 },
};

static struct LcdCmd g_offCmd[] = {
    { LCD_CMD,  0x28, 20 },
    { LCD_CMD,  0x10, 120 },
};

static DevHandle g_spiHdl = NULL;

static int32_t SpiWrite9Bits(DevHandle spiHandle, uint8_t cmd, uint8_t data)
{
    int32_t ret;
    uint16_t wbuff[1] = {0};

    wbuff[0] = ((cmd << BITS_PER_BYTE) | data);
    ret = SpiWrite(spiHandle, (uint8_t *)wbuff, SPI_MSG_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SpiWrite failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

DevHandle GetSpiHandle(void)
{
    int32_t ret;
    DevHandle spiHandle = NULL;
    struct SpiDevInfo spiDevinfo;
    struct SpiCfg cfg;

    spiDevinfo.busNum = 1;
    spiDevinfo.csNum = 0;
    spiHandle = SpiOpen(&spiDevinfo);
    if (spiHandle == NULL) {
        HDF_LOGE("SpiOpen failed");
        return NULL;
    }
    cfg.mode = SPI_CLK_PHASE | SPI_CLK_POLARITY;
    cfg.bitsPerWord = BITS_PER_WORD;
    cfg.maxSpeedHz = SPI_MAX_SPEED;
    ret = SpiSetCfg(spiHandle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SpiSetCfg failed, ret:%d", ret);
        SpiClose(spiHandle);
        return NULL;
    }
    HDF_LOGI("Spi init success!");
    return spiHandle;
}

static int32_t LcdResetOn(void)
{
    int32_t ret;

    ret = GpioSetDir(RESET_GPIO, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("set lcd reset dir failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(RESET_GPIO, GPIO_VAL_HIGH);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("set lcd reset hi failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(RESET_GPIO, GPIO_VAL_LOW);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("set lcd reset how failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    /* delay 10ms */
    OsalMSleep(10);
    ret = GpioWrite(RESET_GPIO, GPIO_VAL_HIGH);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("set lcd reset hi after delay failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    /* delay 120ms */
    OsalMSleep(120);
    return HDF_SUCCESS;
}

static int32_t LcdResetOff(void)
{
    int32_t ret;

    ret = GpioSetDir(RESET_GPIO, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioSetDir failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(RESET_GPIO, GPIO_VAL_LOW);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioWrite failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    /* delay 20ms */
    OsalMSleep(20);
    return HDF_SUCCESS;
}

static int32_t St7789Init(void)
{
    g_spiHdl = GetSpiHandle();
    if (g_spiHdl == NULL) {
        HDF_LOGE("GetSpiHandle failed");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t St7789On(void)
{
    int32_t ret;
    int i;

    /* lcd reset power on */
    ret = LcdResetOn();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LcdResetOn failed", __func__);
        return HDF_FAILURE;
    }
    if (g_spiHdl == NULL) {
        HDF_LOGE("%s: g_spiHdl is null", __func__);
        return HDF_FAILURE;
    }
    int32_t count = sizeof(g_onCmd) / sizeof(g_onCmd[0]);
    for (i = 0; i < count; i++) {
        ret = SpiWrite9Bits(g_spiHdl, g_onCmd[i].cmd, g_onCmd[i].data);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SpiWrite failed");
            return HDF_FAILURE;
        }
        if (g_onCmd[i].delay > 0) {
            OsalMSleep(g_onCmd[i].delay);
        }
    }
    return HDF_SUCCESS;
}

static int32_t St7789Off(void)
{
    int32_t ret;
    int i;

    if (g_spiHdl == NULL) {
        HDF_LOGE("%s: g_spiHdl is null", __func__);
        return HDF_FAILURE;
    }
    /* send mipi init code */
    int32_t count = sizeof(g_offCmd) / sizeof(g_offCmd[0]);

    for (i = 0; i < count; i++) {
        ret = SpiWrite9Bits(g_spiHdl, g_offCmd[i].cmd, g_offCmd[i].data);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SpiWrite9Bits failed");
            return HDF_FAILURE;
        }
        if (g_offCmd[i].delay > 0) {
            OsalMSleep(g_offCmd[i].delay);
        }
    }
    /* lcd reset power off */
    ret = LcdResetOff();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LcdResetOff failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t St7789SetBacklight(uint32_t level)
{
    (void)level;
    return HDF_SUCCESS;
}

static struct PanelInfo g_panelInfo = {
    .width = WIDTH,                   /* width */
    .height = HEIGHT,                 /* height */
    .frameRate = FRAME_RATE,          /* frame rate */
    .intfType = LCD_6BIT,             /* panel interface type */
    .intfSync = 42,    /* Hi3559AV100: For ili9341 at 50 Hz (6bit) */
};

static struct PanelData g_panelData = {
    .info = &g_panelInfo,
    .init = St7789Init,
    .on = St7789On,
    .off = St7789Off,
    .setBacklight = St7789SetBacklight,
};

int32_t SspSt7789EntryInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }
    if (PanelDataRegister(&g_panelData) != HDF_SUCCESS) {
        HDF_LOGE("%s: PanelDataRegister failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: exit succ", __func__);
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_st7789DevEntry = {
    .moduleVersion = 1,
    .moduleName = "LCD_ST7789",
    .Init = SspSt7789EntryInit,
};

HDF_INIT(g_st7789DevEntry);
