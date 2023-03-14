/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_MAIN_TEST_H
#define HDF_MAIN_TEST_H

#include "hdf_object.h"
#include "hdf_device_desc.h"

typedef struct {
    uint16_t cmd;   // submodule command
    uint8_t subCmd; // test case command
    int8_t result;
} HdfTestMsg;

typedef int32_t (*TestFunc)(HdfTestMsg*);
typedef int32_t (*TestCaseFunc)(void);
typedef struct {
    uint16_t cmd;      // submodule command
    TestFunc testFunc; // function
} HdfTestFuncList;
typedef struct {
    uint8_t subCmd;        // test case command
    TestCaseFunc testFunc; // test case function
} HdfTestCaseList;

struct TestService {
    struct IDeviceIoService testRemoteService;
};

// two hundred values allocated per submodule
typedef enum {
    TEST_PAL_I2C_TYPE       = 0,
    TEST_PAL_SPI_TYPE       = 1,
    TEST_PAL_GPIO_TYPE      = 2,
    TEST_PAL_PIN_TYPE       = 3,
    TEST_PAL_CLOCK_TYPE     = 4,
    TEST_PAL_REGULATOR_TYPE = 5,
    TEST_PAL_MIPI_DSI_TYPE  = 6,
    TEST_PAL_UART_TYPE      = 7,
    TEST_PAL_SDIO_TYPE      = 8,
    TEST_PAL_MDIO_TYPE      = 9,
    TEST_PAL_APB_TYPE       = 10,
    TEST_PAL_PCIE_TYPE      = 11,
    TEST_PAL_PCM_TYPE       = 12,
    TEST_PAL_I2S_TYPE       = 13,
    TEST_PAL_PWM_TYPE       = 14,
    TEST_PAL_DMA_TYPE       = 15,
    TEST_PAL_EFUSE_TYPE     = 16,
    TEST_PAL_FLASH_TYPE     = 17,
    TEST_PAL_EMMC_TYPE      = 18,
    TEST_PAL_RTC_TYPE       = 19,
    TEST_PAL_ADC_TYPE       = 20,
    TEST_PAL_WDT_TYPE       = 21,
    TEST_PAL_I3C_TYPE       = 22,
    TEST_PAL_MIPI_CSI_TYPE  = 23,
    TEST_PAL_DAC_TYPE       = 24,
    TEST_PAL_TIMER_TYPE     = 25,
    TEST_PAL_MANAGER_TYPE   = 196,
    TEST_PAL_DEVICE_TYPE    = 197,
    TEST_PAL_QUEUE_TYPE     = 198,
    TEST_PAL_EVENT_TYPE     = 199,
    TEST_PAL_END            = 200,
    TEST_OSAL_BEGIN         = TEST_PAL_END,
#define HDF_OSAL_TEST_ITEM(v) (TEST_OSAL_BEGIN + (v))
    TEST_OSAL_ITEM = HDF_OSAL_TEST_ITEM(1),
    TEST_OSAL_END           = 400,
    TEST_WIFI_BEGIN         = TEST_OSAL_END,
    TEST_WIFI_TYPE          = TEST_WIFI_BEGIN + 1,
    TEST_WIFI_END           = 600,
    TEST_CONFIG_TYPE        = 601,
    TEST_AUDIO_TYPE         = 701,
    TEST_AUDIO_DRIVER_TYPE  = TEST_AUDIO_TYPE + 1,
    TEST_HDF_FRAME_END      = 800,
    TEST_USB_DEVICE_TYPE    = 900,
    TEST_USB_HOST_TYPE      = 1000,
    TEST_USB_HOST_RAW_TYPE  = 1100,
} HdfTestSubModuleCmd;

struct HdfDeviceObject *GetDeviceObject(void);

#endif // HDF_MAIN_TEST_H
