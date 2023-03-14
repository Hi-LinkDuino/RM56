/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_parser_test.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "hdf_main_test.h"
#include "osal_test_entry.h"

#if defined(LOSCFG_DRIVERS_HDF_PLATFORM) || defined(CONFIG_DRIVERS_HDF_PLATFORM)
#include "hdf_platform_core_entry_test.h"
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_GPIO) || defined(CONFIG_DRIVERS_HDF_PLATFORM_GPIO)
#include "hdf_gpio_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PIN) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PIN)
#include "hdf_pin_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I2C) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I2C)
#include "hdf_i2c_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I2S) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I2S)
#include "hdf_i2s_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_REGULATOR) || defined(CONFIG_DRIVERS_HDF_PLATFORM_REGULATOR)
#include "hdf_regulator_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_ADC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_ADC)
#include "hdf_adc_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_DAC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_DAC)
#include "hdf_dac_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I3C) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I3C)
#include "hdf_i3c_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SPI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SPI)
#include "hdf_spi_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SDIO) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SDIO)
#include "hdf_sdio_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PCIE) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PCIE)
#include "hdf_pcie_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_UART) || defined(CONFIG_DRIVERS_HDF_PLATFORM_UART)
#include "hdf_uart_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_WATCHDOG) || defined(CONFIG_DRIVERS_HDF_PLATFORM_WATCHDOG)
#include "hdf_watchdog_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_RTC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_RTC)
#include "hdf_rtc_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_DSI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MIPI_DSI)
#include "hdf_mipi_dsi_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_CSI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MIPI_CSI)
#include "hdf_mipi_csi_entry_test.h"
#endif
#if (defined(LOSCFG_STORAGE_EMMC) && defined(LOSCFG_DRIVERS_HDF_PLATFORM_EMMC)) || \
     defined(CONFIG_DRIVERS_HDF_PLATFORM_EMMC)
#include "hdf_emmc_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PWM) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PWM)
#include "hdf_pwm_entry_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_TIMER) || defined(CONFIG_DRIVERS_HDF_PLATFORM_TIMER)
#include "hdf_timer_entry_test.h"
#endif
#endif
#if defined(LOSCFG_DRIVERS_HDF_WIFI) || defined(CONFIG_DRIVERS_HDF_WIFI)
#include "hdf_wifi_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_AUDIO_TEST) || defined(CONFIG_DRIVERS_HDF_AUDIO_TEST)
#include "hdf_audio_test.h"
#include "hdf_audio_driver_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_USB_DDK_DEVICE) || defined(CONFIG_DRIVERS_HDF_USB_DDK_DEVICE)
#include "hdf_usb_device_test.h"
#endif
#if defined(LOSCFG_DRIVERS_HDF_USB_DDK_HOST) || defined(CONFIG_DRIVERS_HDF_USB_DDK_HOST)
#include "hdf_usb_entry_test.h"
#endif

#define HDF_LOG_TAG hdf_test

// add submodule entry
HdfTestFuncList g_hdfTestFuncList[] = {
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM) || defined(CONFIG_DRIVERS_HDF_PLATFORM)
    { TEST_PAL_EVENT_TYPE, HdfPlatformEventTestEntry },
    { TEST_PAL_QUEUE_TYPE, HdfPlatformQueueTestEntry },
    { TEST_PAL_DEVICE_TYPE, HdfPlatformDeviceTestEntry },
    { TEST_PAL_MANAGER_TYPE, HdfPlatformManagerTestEntry },
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_GPIO) || defined(CONFIG_DRIVERS_HDF_PLATFORM_GPIO)
    { TEST_PAL_GPIO_TYPE, HdfGpioTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PIN) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PIN)
    { TEST_PAL_PIN_TYPE, HdfPinTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I2C) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I2C)
    { TEST_PAL_I2C_TYPE, HdfI2cTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I2S) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I2S)
    { TEST_PAL_I2S_TYPE, HdfI2sUnitTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_REGULATOR) || defined(CONFIG_DRIVERS_HDF_PLATFORM_REGULATOR)
    { TEST_PAL_REGULATOR_TYPE, HdfRegulatorUnitTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_ADC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_ADC)
    { TEST_PAL_ADC_TYPE, HdfAdcTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_DAC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_DAC)
    { TEST_PAL_DAC_TYPE, HdfDacTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I3C) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I3C)
    { TEST_PAL_I3C_TYPE, HdfI3cTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SPI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SPI)
    { TEST_PAL_SPI_TYPE, HdfSpiUnitTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_UART) || defined(CONFIG_DRIVERS_HDF_PLATFORM_UART)
    { TEST_PAL_UART_TYPE, HdfUartUnitTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_WATCHDOG) || defined(CONFIG_DRIVERS_HDF_PLATFORM_WATCHDOG)
    { TEST_PAL_WDT_TYPE, HdfWatchdogTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_RTC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_RTC)
    { TEST_PAL_RTC_TYPE, HdfRtcTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_DSI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MIPI_DSI)
    { TEST_PAL_MIPI_DSI_TYPE, HdfMipiDsiEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_CSI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MIPI_CSI)
    { TEST_PAL_MIPI_CSI_TYPE, HdfMipiCsiEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SDIO) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SDIO)
    { TEST_PAL_SDIO_TYPE, HdfSdioUnitTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PCIE) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PCIE)
    { TEST_PAL_PCIE_TYPE, HdfPcieUnitTestEntry },
#endif
#if (defined(LOSCFG_STORAGE_EMMC) && defined(LOSCFG_DRIVERS_HDF_PLATFORM_EMMC)) || \
     defined(CONFIG_DRIVERS_HDF_PLATFORM_EMMC)
    { TEST_PAL_EMMC_TYPE, HdfEmmcUnitTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PWM) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PWM)
    { TEST_PAL_PWM_TYPE, HdfPwmUnitTestEntry },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_TIMER) || defined(CONFIG_DRIVERS_HDF_PLATFORM_TIMER)
        { TEST_PAL_TIMER_TYPE, HdfTimerUnitTestEntry },
#endif
#endif
    { TEST_CONFIG_TYPE, HdfConfigEntry },
    { TEST_OSAL_ITEM, HdfOsalEntry },
#if defined(LOSCFG_DRIVERS_HDF_WIFI) || defined(CONFIG_DRIVERS_HDF_WIFI)
    {TEST_WIFI_TYPE, HdfWifiEntry},
#endif
#if defined(LOSCFG_DRIVERS_HDF_AUDIO_TEST) || defined(CONFIG_DRIVERS_HDF_AUDIO_TEST)
    {TEST_AUDIO_TYPE, HdfAudioEntry},
    {TEST_AUDIO_DRIVER_TYPE, HdfAudioDriverEntry},
#endif
#if defined(LOSCFG_DRIVERS_HDF_USB_DDK_DEVICE) || defined(CONFIG_DRIVERS_HDF_USB_DDK_DEVICE)
    {TEST_USB_DEVICE_TYPE, HdfUsbDeviceEntry},
#endif
#if defined(LOSCFG_DRIVERS_HDF_USB_DDK_HOST) || defined(CONFIG_DRIVERS_HDF_USB_DDK_HOST)
#if defined(LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY) || defined(CONFIG_DRIVERS_HDF_USB_PNP_NOTIFY)
    {TEST_USB_HOST_TYPE, HdfUsbUnitTestEntry},
    {TEST_USB_HOST_RAW_TYPE, HdfUsbUnitTestEntry},
#endif
#endif
};

static int32_t HdfTestCaseProcess(struct HdfDeviceIoClient *client,
    int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HdfTestMsg *msg = NULL;
    HdfTestMsg replyMsg;
    uint32_t len;
    int i;
    struct TestService *testService = NULL;

    if (client == NULL || client->device == NULL) {
        HDF_LOGE("%s::  client or client->device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s:: data or reply is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    testService = (struct TestService *)client->device->service;
    if (testService == NULL) {
        HDF_LOGE("%s:: service is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (!HdfSbufReadBuffer(data, (const void **)&msg, &len)) {
        HDF_LOGE("%s::  msg sbuf read fail", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    replyMsg.cmd = msg->cmd;
    replyMsg.subCmd = msg->subCmd;

    HDF_LOGE("%s::Hdf test receive cmd: command[%u], subCommand[%u]", __func__, msg->cmd, msg->subCmd);

    replyMsg.result = HDF_FAILURE;
    for (i = 0; i < sizeof(g_hdfTestFuncList) / sizeof(g_hdfTestFuncList[0]); ++i) {
        if ((msg->cmd == g_hdfTestFuncList[i].cmd) && (g_hdfTestFuncList[i].testFunc != NULL)) {
            g_hdfTestFuncList[i].testFunc(&replyMsg);
            break;
        }
    }

    if (!HdfSbufWriteBuffer(reply, &replyMsg, sizeof(replyMsg))) {
        HDF_LOGE("%s:: hdf test sbuf write fail", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

void HdfTestDriverRelease(struct HdfDeviceObject *device)
{
    return;
}

void HdfTestDriverDispatch(struct HdfDeviceObject *device)
{
    return;
}

int HdfTestDriverBind(struct HdfDeviceObject *device)
{
    static struct TestService testService;

    if (device == NULL) {
        return -1;
    }

    testService.testRemoteService.Dispatch = HdfTestCaseProcess;

    device->service = &testService.testRemoteService;
    return 0;
}

static struct HdfDeviceObject *g_devObject;
struct HdfDeviceObject *GetDeviceObject(void)
{
    return g_devObject;
}

int HdfTestDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s::ptr is null!\n", __func__);
        return -1;
    }
    g_devObject = device;
    return 0;
}

struct HdfDriverEntry g_hdfTestDevice = {
    .moduleVersion = 1,
    .moduleName = "HDF_TEST",
    .Bind = HdfTestDriverBind,
    .Init = HdfTestDriverInit,
    .Release = HdfTestDriverRelease,
};

HDF_INIT(g_hdfTestDevice);
