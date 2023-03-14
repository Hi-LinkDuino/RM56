/*
 * Copyright (c) 2021-2022 GOODIX.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "app_io.h"
#include "app_gpiote.h"
#include "gpio/gpio_core.h"
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "stdio.h"

#define HDF_LOG_TAG        gpio_gr5xx
#define IO_NUM_GPIO        32
#define IO_NUM_AON         8
#define IO_NUM_MSIO        5
#define IO_NUM_MAX         (IO_NUM_GPIO + IO_NUM_AON + IO_NUM_MSIO)

#define IO_BASE_GPIO       0
#define IO_BASE_AON        IO_NUM_GPIO
#define IO_BASE_MSIO       (IO_NUM_GPIO + IO_NUM_AON)

#define IO_PULL_DEFAULT    APP_IO_PULLUP
#define IO_MODE_DEFAULT    APP_IO_MODE_OUT_PUT

struct Gr5xxGpioPortConfig {
    app_io_mode_t mode;
    app_io_pull_t pull;
    app_handle_mode_t handleMode;
};

static struct GpioCntlr g_GpioCntlr;
struct Gr5xxGpioPortConfig g_portCfg[IO_NUM_MAX];

static inline app_io_type_t GpioDevPinMap(uint32_t id, uint32_t *pin)
{
    if (id < IO_BASE_AON) {
        *pin  = 1 << (id - IO_BASE_GPIO);
        return APP_IO_TYPE_NORMAL;
    } else if (id < IO_BASE_MSIO) {
        *pin  = 1 << (id - IO_BASE_AON);
        return APP_IO_TYPE_AON;
    } else {
        *pin  = 1 << (id - IO_BASE_MSIO);
        return APP_IO_TYPE_MSIO;
    }
}

static void PortConfigInit(void)
{
    uint16_t cnt;

    for (cnt = 0; cnt < IO_NUM_MAX; cnt++) {
        g_portCfg[cnt].pull       = IO_PULL_DEFAULT;
        g_portCfg[cnt].mode       = IO_MODE_DEFAULT;
        g_portCfg[cnt].handleMode = APP_IO_NONE_WAKEUP;
    }
}

static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    (void)cntlr;
    uint32_t pin = 0;
    app_io_init_t ioInit;
    app_io_type_t iotype;

    if (gpio >= IO_NUM_MAX) {
        HDF_LOGE("%s, gpio index is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    iotype = GpioDevPinMap(gpio, &pin);
    ioInit.pin  = pin;
    ioInit.pull = g_portCfg[gpio].pull;
    ioInit.mux  = APP_IO_MUX_7;
    ioInit.mode = IO_MODE_DEFAULT;

    if (dir == GPIO_DIR_IN) {
        ioInit.mode = APP_IO_MODE_INPUT;
    } else if (dir == GPIO_DIR_OUT) {
        ioInit.mode = APP_IO_MODE_OUT_PUT;
    }

    g_portCfg[gpio].mode = ioInit.mode;
    if (APP_DRV_SUCCESS == app_io_init(iotype, &ioInit)) {
        return HDF_SUCCESS;
    } else {
        return HDF_FAILURE;
    }
}

static int32_t GpioDevGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
    (void)cntlr;
    if (gpio >= IO_NUM_MAX) {
        HDF_LOGE("%s, gpio index is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    *dir = g_portCfg[gpio].mode;

    return HDF_SUCCESS;
}

static int32_t GpioDevWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    (void)cntlr;
    uint32_t pin = 0;
    app_io_type_t ioType;

    if (gpio >= IO_NUM_MAX) {
        HDF_LOGE("%s, gpio index is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ioType = GpioDevPinMap(gpio, &pin);
    if (val == GPIO_VAL_LOW) {
        app_io_write_pin(ioType, pin, APP_IO_PIN_RESET);
    } else if (val == GPIO_VAL_HIGH) {
        app_io_write_pin(ioType, pin, APP_IO_PIN_SET);
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    (void)cntlr;
    uint32_t pin = 0;
    app_io_type_t ioType;
    app_io_pin_state_t ioState;

    if (val == NULL) {
        HDF_LOGE("%s, value buffer is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (gpio >= IO_NUM_MAX) {
        HDF_LOGE("%s, gpio index is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ioType = GpioDevPinMap(gpio, &pin);
    ioState = app_io_read_pin(ioType, pin);
    if (APP_IO_PIN_RESET == ioState) {
        *val = GPIO_VAL_LOW;
    } else {
        *val = GPIO_VAL_HIGH;
    }

    return HDF_SUCCESS;
}

static int32_t GetPinIndex(uint32_t pin)
{
    int32_t index = 0;
    while ((pin & 1) != 1) {
        index++;
        pin = pin >> 1;
    }
    return index;
}

static void GR5xxGpioIrqHdl(app_gpiote_evt_t *event)
{
    uint32_t gpio = 0;

    if (event == NULL) {
        HDF_LOGE("%s, event is NULL", __func__);
        return;
    }

    if (event->type == APP_IO_TYPE_NORMAL) {
        gpio = GetPinIndex(event->pin);
    } else if (event->type == APP_IO_TYPE_AON) {
        gpio = GetPinIndex(event->pin) + IO_BASE_AON;
    } else {
        HDF_LOGE("%s, event type error", __func__);
    }

    GpioCntlrIrqCallback(&g_GpioCntlr, gpio);
}

static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    uint32_t pin = 0;
    app_io_type_t ioType;
    app_gpiote_param_t gpioteParam;

    if (gpio >= IO_NUM_MAX) {
        HDF_LOGE("%s, gpio index is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ioType = GpioDevPinMap(gpio, &pin);
    gpioteParam.type = ioType;
    gpioteParam.pin  = pin;
    gpioteParam.pull = g_portCfg[gpio].pull;
    gpioteParam.mode = g_portCfg[gpio].mode;
    gpioteParam.handle_mode = g_portCfg[gpio].handleMode;
    gpioteParam.io_evt_cb = GR5xxGpioIrqHdl;
    app_gpiote_init(&gpioteParam, 1);

    return HDF_SUCCESS;
}

static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    uint32_t pin = 0;
    app_io_type_t ioType;
    app_gpiote_param_t gpioteParam;

    if (gpio >= IO_NUM_MAX) {
        HDF_LOGE("%s, gpio index is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ioType = GpioDevPinMap(gpio, &pin);
    gpioteParam.type = ioType;
    gpioteParam.pin  = pin;
    gpioteParam.pull = g_portCfg[gpio].pull;
    gpioteParam.mode = g_portCfg[gpio].mode;
    gpioteParam.handle_mode = APP_IO_NONE_WAKEUP;
    gpioteParam.io_evt_cb = NULL;
    app_gpiote_init(&gpioteParam, 1);

    return HDF_SUCCESS;
}

static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode)
{
    (void)cntlr;
    uint32_t pin = 0;
    app_io_type_t ioType;

    if (gpio >= IO_NUM_MAX) {
        HDF_LOGE("%s, gpio index is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ioType = GpioDevPinMap(gpio, &pin);
    if (APP_IO_TYPE_MSIO == ioType) {
        HDF_LOGE("%s, IO type not support", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (mode) {
        case GPIO_IRQ_TRIGGER_RISING:
            g_portCfg[gpio].mode = APP_IO_MODE_IT_RISING;
            break;
        case GPIO_IRQ_TRIGGER_FALLING:
            g_portCfg[gpio].mode = APP_IO_MODE_IT_FALLING;
            break;
        case GPIO_IRQ_TRIGGER_HIGH:
            g_portCfg[gpio].mode = APP_IO_MODE_IT_HIGH;
            break;
        case GPIO_IRQ_TRIGGER_LOW:
            g_portCfg[gpio].mode = APP_IO_MODE_IT_LOW;
            break;
        default:
            HDF_LOGE("%s, IRQ mode not support", __func__);
            return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevUnSetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    uint32_t pin = 0;
    app_io_type_t ioType;
    app_gpiote_param_t gpioteParam;

    if (gpio >= IO_NUM_MAX) {
        HDF_LOGE("%s, gpio index is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ioType = GpioDevPinMap(gpio, &pin);
    gpioteParam.type = ioType;
    gpioteParam.pin  = pin;
    gpioteParam.mode = g_portCfg[gpio].mode;
    gpioteParam.pull = g_portCfg[gpio].pull;
    gpioteParam.handle_mode = APP_IO_NONE_WAKEUP;
    gpioteParam.io_evt_cb = NULL;

    app_gpiote_init(&gpioteParam, 1);

    return HDF_SUCCESS;
}

static struct GpioMethod g_GpioCntlrMethod = {
    .request = NULL,
    .release = NULL,
    .write = GpioDevWrite,
    .read = GpioDevRead,
    .setDir = GpioDevSetDir,
    .getDir = GpioDevGetDir,
    .toIrq = NULL,
    .setIrq = GpioDevSetIrq,
    .unsetIrq = GpioDevUnSetIrq,
    .enableIrq = GpioDevEnableIrq,
    .disableIrq = GpioDevDisableIrq,
};

static uint16_t GetGpioResourceData(const struct DeviceResourceNode *node, const char *resName, uint32_t index)
{
    int32_t ret;
    uint16_t resData;
    struct DeviceResourceIface *drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    ret = drsOps->GetUint16ArrayElem(node, resName, index, &resData, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read %s fail!", __func__, resName);
        return 0;
    }

    return resData;
}

static int32_t GetGpioDeviceResource(const struct DeviceResourceNode *node)
{
    int32_t ret;
    uint16_t configNum = 0;
    uint16_t gpioIndex;
    uint16_t pullConfig;
    uint16_t handleModeConfig;

    if (node == NULL) {
        HDF_LOGE("%s, device resource node is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct DeviceResourceIface *drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint16 == NULL || drsOps->GetUint16ArrayElem == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint16(node, "configNum", &configNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read configNum fail!", __func__);
        return ret;
    }
    if (configNum >= IO_NUM_MAX) {
        HDF_LOGE("%s: configNum(%d) invalid!", __func__, configNum);
        return HDF_ERR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < configNum; i++) {
        gpioIndex = GetGpioResourceData(node, "gpioIndex", i);
        pullConfig = GetGpioResourceData(node, "pull", i);
        handleModeConfig = GetGpioResourceData(node, "handleMode", i);
        if (gpioIndex >= IO_NUM_MAX) {
            HDF_LOGE("%s: gpioIndex(%d) invalid!", __func__, gpioIndex);
            return HDF_ERR_INVALID_PARAM;
        }
        if (pullConfig >= APP_IO_PULL_MAX) {
            HDF_LOGE("%s: pullConfig(%d) invalid!", __func__, pullConfig);
            return HDF_ERR_INVALID_PARAM;
        }
        if (handleModeConfig > APP_IO_ENABLE_WAKEUP) {
            HDF_LOGE("%s: handleModeConfig(%d) invalid!", __func__, handleModeConfig);
            return HDF_ERR_INVALID_PARAM;
        }
        g_portCfg[gpioIndex].pull = pullConfig;
        g_portCfg[gpioIndex].handleMode = handleModeConfig;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDriverBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static int32_t GpioDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct GpioCntlr *gpioCntlr = &g_GpioCntlr;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    PortConfigInit();
    ret = GetGpioDeviceResource(device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs fail:%d", __func__, ret);
        return ret;
    }

    gpioCntlr->count = IO_NUM_MAX;
    gpioCntlr->priv = (void *)device->property;
    gpioCntlr->ops = &g_GpioCntlrMethod;
    (void)PlatformDeviceBind(&gpioCntlr->device, device);
    ret = GpioCntlrAdd(gpioCntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: err add controller:%d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static void GpioDriverRelease(struct HdfDeviceObject *device)
{
    struct GpioCntlr *gpioCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null!", __func__);
        return;
    }

    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("%s: no service binded!", __func__);
        return;
    }

    app_gpiote_deinit();
    GpioCntlrRemove(gpioCntlr);
}

struct HdfDriverEntry g_gpioDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PLATFORM_GPIO",
    .Bind = GpioDriverBind,
    .Init = GpioDriverInit,
    .Release = GpioDriverRelease,
};

HDF_INIT(g_gpioDriverEntry);
