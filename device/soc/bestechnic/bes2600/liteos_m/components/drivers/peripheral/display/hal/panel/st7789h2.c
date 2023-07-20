/******************************************************************************
 ** File Name:     cstn_pcf8833.c                                             *
 ** Description:															  *
 **    This file contains driver for color LCD.(CSTN)						  *
 ** Author:         Jim zhang                                                 *
 ** DATE:           03/01/2004                                                *
 ** Copyright:      2004 Spreatrum, Incoporated. All Rights Reserved.         *
 ** Description:                                                              *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 02/04/2004     louis.wei	    Create.
 ******************************************************************************/
#include "cmsis_os2.h"
#include "hdf_log.h"
#include "lcd_abs_if.h"
#include "hdf_device_desc.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif
#include "hal_spi.h"
#include "spi_if.h"
#include "st7789h2.h"

#ifdef CONFIG_DISPLAY_ST7789H2

#define _ENABLE       1
#define _DISABLE      0
uint32_t XTC_ST7789H2_Id[4] = {0};

struct PanelDevice {
    struct PanelData panelData;
    struct PanelInfo panelInfo;
    DevHandle spiHandle;
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
        .intfType = LCD_16BIT,
        .intfSync = OUTPUT_USER,
    },
    .pin_rst = {
        HAL_GPIO_PIN_P2_1,
        HAL_IOMUX_FUNC_AS_GPIO,
        HAL_IOMUX_PIN_VOLTAGE_VIO,
        HAL_IOMUX_PIN_NOPULL,
    },
    .pin_te = {
        HAL_IOMUX_PIN_P0_3,
        HAL_IOMUX_FUNC_AS_GPIO,
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

void WriteData(uint8_t data)
{
    uint16_t spi_tx[1];
    spi_tx[0] = 0x100 | (uint16_t)data; // bit1 is high(data)
#ifdef SPI_USE_DMA
    hal_spilcd_dma_send(&spi_tx, 2, NULL);
#else
    hal_spilcd_send(&spi_tx, 2);
#endif
}

void WriteCmd(uint8_t cmd)
{
    uint16_t spi_tx[1];
    spi_tx[0] = 0x0000 | (uint16_t)cmd; // bit1 is low(cmd)
#ifdef SPI_USE_DMA
    hal_spilcd_dma_send(&spi_tx,2,NULL);
#else
    hal_spilcd_send(&spi_tx, 2);
#endif
}

void lcd_set_startaddr(uint16_t x_start, uint16_t x_end, uint16_t y_start,  uint16_t y_end)
{
    WriteCmd(0x2a);
    WriteData(x_start >> 8);
    WriteData(x_start);
    WriteData(x_end >> 8);
    WriteData(x_end);

    WriteCmd(0x2b);
    WriteData(y_start >> 8);
    WriteData(y_start);
    WriteData(y_end >> 8);
    WriteData(y_end);

    WriteCmd(0x2c);
}

/*
static void ST7789H2_Panel2LineEn(DevHandle dev, bool enable, uint16_t lcd_id)
{
    if(enable) {
        WriteCmd(0xE7);
        WriteData(0x10);
        WriteCmd(0x2c);
        //LCM_2LineEn(_ENABLE, 0);
    }
    else {
        //LCM_2LineEn(_DISABLE, 0);
        WriteCmd(0xe7);
        WriteData(0x00);
    }
}
*/

static void ST7789H2_driver(DevHandle dev)
{
    // uint16_t cmd_temp;
    // Do HW Reset
    // PanelPowerControl(true);
    WriteCmd(0x11);
    osDelay(120);
    //--------------------------------Display and color format setting-------------------
    WriteCmd(0x36);
    WriteData(0x00);

    WriteCmd(0x3a);
    WriteData(0x05);

    WriteCmd(0xb0);
    WriteData(0x00);
    WriteData(0xF8);

    //--------------------------------ST7789V Frame rate setting----------------------------------//
    WriteCmd(0xb2);
    WriteData(0x0c);
    WriteData(0x0c);
    WriteData(0x00);
    WriteData(0x33);
    WriteData(0x33);

    WriteCmd(0xb7);
    WriteData(0x35);

    //---------------------------------ST7789V Power setting--------------------------------------//
    WriteCmd(0xbb);
    WriteData(0x35);

    WriteCmd(0xc0);
    WriteData(0x2c);

    WriteCmd(0xc2);
    WriteData(0x01);

    WriteCmd(0xc3);
    WriteData(0x11);

    WriteCmd(0xc4);
    WriteData(0x20);

    WriteCmd(0xc6);
    WriteData(0x0f);

    WriteCmd(0xd0);
    WriteData(0xa4);
    WriteData(0xa1);
    //--------------------------------ST7789V gamma setting---------------------------------------//
    WriteCmd(0xe0);
    WriteData(0xd0);
    WriteData(0x00);
    WriteData(0x05);
    WriteData(0x0e);
    WriteData(0x15);
    WriteData(0x0d);
    WriteData(0x37);
    WriteData(0x43);
    WriteData(0x47);
    WriteData(0x09);
    WriteData(0x15);
    WriteData(0x12);
    WriteData(0x16);
    WriteData(0x19);

    WriteCmd(0xe1);
    WriteData(0xd0);
    WriteData(0x00);
    WriteData(0x05);
    WriteData(0x0d);
    WriteData(0x0c);
    WriteData(0x06);
    WriteData(0x2d);
    WriteData(0x44);
    WriteData(0x40);
    WriteData(0x0e);
    WriteData(0x1c);
    WriteData(0x18);
    WriteData(0x16);
    WriteData(0x19);

    WriteCmd(0x21);

    WriteCmd(0x2a); //Frame rate control
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0xef);

    WriteCmd(0x2b); //Display function control
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0x00);    // 01
    WriteData(0xEF);    // 3F
    WriteCmd(0x35);     //TE On
    WriteData(0x00);

    WriteCmd(0x29); //display on
    WriteCmd(0x2c);

    // TEST DISPLAY
#if 0
    uint16_t color = 0x00;
    while(1) {
        color = color>0xFFFF ? 0 : ++color;
        for(uint16_t i=0; i<(240*240); i++)
        {
            WriteData(color >> 8);
            WriteData(color & 0xFF);
        }
        osDelay(1000);
    }
#endif

}
/*
static uint32_t ST7789H2_ReadID(uint16_t lcd_cs, uint16_t lcd_cd, uint16_t lcd_id)
 {
     ST7789H2_Panel2LineEn(_DISABLE, 0);

     WriteCmd(0xfe);
     WriteCmd(0xef);

     //LCM_Read (0x04, &id, 4, lcd_id);

     ST7789H2_Panel2LineEn(_ENABLE, 0);

     memcpy(&XTC_ST7789H2_Id, &id, sizeof(id));

     HDF_LOGI("ReadID SPI: 0x%0x, 0x%x, 0x%0x\n", id[0], id[1], id[2]);
     HDF_LOGI("ModuleID: 0x%0x, 0x%x, 0x%0x, 0x%0x\n",XTC_ST7789H2_Id[0], XTC_ST7789H2_Id[1], XTC_ST7789H2_Id[2], XTC_ST7789H2_Id[3]);

     if (0x85 == id[1] && 0x52 == id[2]) {
         HDF_LOGI("Read id success\n");
         return 0x7789;
     }

     return 0;
 }
*/

static int32_t PanelInit(struct PanelData *panel)
{
    if (panel == NULL || panel->info == NULL) {
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: width %d, height %d", __func__, WIDTH, HEIGHT);

    // int ret;
    struct SpiDevInfo info = {1, 0};
    priv.spiHandle = SpiOpen(&info);
    if (priv.spiHandle == NULL) {
        HDF_LOGE("SpiOpen failed");
        return -1;
    }
    HDF_LOGE("===== spi open ok");

    struct PanelInfo *panelInfo = panel->info;
    struct SpiCfg cfg = {0};
    cfg.bitsPerWord  = 16;
    cfg.maxSpeedHz   = 24000000;
    cfg.mode         = SPI_MODE_3WIRE;
    cfg.transferMode = SPI_POLLING_TRANSFER;

    /* config spi device */
    if (SpiSetCfg(priv.spiHandle, &cfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: SpiSetCfg failed", __func__);
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

static int32_t PanelOn(struct PanelData *panel)
{
    /* send spi power on code */
    // ST7789H2_Panel2LineEn(priv.spiHandle, _DISABLE, 0);
    // Do HW Reset
    PanelPowerControl(true);
    ST7789H2_driver(priv.spiHandle);
    // ST7789H2_Panel2LineEn(priv.spiHandle, _ENABLE, 0);

    /* set spi mode */
    //MipiDsiSetHsMode(priv.mipiHandle);
    //PanelCheckStatus(panel);
    hal_gpio_pin_set_dir(priv.pin_led.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(20);
    return HDF_SUCCESS;
}

static int32_t PanelOff(struct PanelData *panel)
{
    (void)panel;
    return HDF_SUCCESS;
}

static int32_t PanelSetBacklight(struct PanelData *panel, uint32_t level)
{
    (void)panel;
    return HDF_SUCCESS;
}

#define REAL_PIN(n) (n / 10 * 8 + n % 10)
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define DISPLAY_PANEL_CONFIG HCS_NODE(HCS_NODE(HCS_NODE(HCS_ROOT, display), panel_config), st7789h2_config)
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


static struct HdfDriverEntry g_ST7789H2DriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PANEL_ST7789H2",
    .Bind = PanelDriverBind,
    .Init = PanelDriverInit,
    .Release = PanelDriverRelease,
};

HDF_INIT(g_ST7789H2DriverEntry);

#endif

