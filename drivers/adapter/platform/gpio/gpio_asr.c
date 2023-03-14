/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include "gpio_core.h"
#include "hdf_log.h"
#include "osal_irq.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif
#include "duet_gpio.h"

#define ASR_GPIO_TOTAL_NUM    DUET_GPIO_TOTAL_NUM
#define asr_gpio_config_t duet_gpio_config_t
#define ASR_ANALOG_MODE DUET_ANALOG_MODE
#define ASR_IRQ_MODE DUET_IRQ_MODE
#define ASR_INPUT_PULL_UP DUET_INPUT_PULL_UP
#define ASR_INPUT_PULL_DOWN DUET_INPUT_PULL_DOWN
#define ASR_INPUT_HIGH_IMPEDANCE DUET_INPUT_HIGH_IMPEDANCE
#define ASR_OUTPUT_PUSH_PULL DUET_OUTPUT_PUSH_PULL
#define ASR_OUTPUT_OPEN_DRAIN_NO_PULL DUET_OUTPUT_OPEN_DRAIN_NO_PULL
#define ASR_OUTPUT_OPEN_DRAIN_PULL_UP DUET_OUTPUT_OPEN_DRAIN_PULL_UP

#define asr_gpio_dev_t duet_gpio_dev_t
#define asr_gpio_irq_trigger_t duet_gpio_irq_trigger_t
#define asr_gpio_irq_handler_t duet_gpio_irq_handler_t
#define asr_gpio_cb_t duet_gpio_cb_t
#define asr_gpio_init duet_gpio_init
#define asr_gpio_output_high duet_gpio_output_high
#define asr_gpio_output_low duet_gpio_output_low
#define asr_gpio_output_toggle duet_gpio_output_toggle
#define asr_gpio_input_get duet_gpio_input_get
#define asr_gpio_enable_irq duet_gpio_enable_irq
#define asr_gpio_disable_irq duet_gpio_disable_irq
#define asr_gpio_clear_irq duet_gpio_clear_irq
#define asr_gpio_finalize duet_gpio_finalize

#define HDF_LOG_TAG GPIO_ASR

struct GpioResource {
    uint32_t pin;
    uint32_t realPin;
    uint32_t config;
    uint32_t pinNum;
    uint32_t type; /**< Type of the input event EV_KEY */
    uint32_t code; /**< Specific code item of the input event KEY_POWER*/
    unsigned long physBase;
};

enum GpioDeviceState {
    GPIO_DEVICE_UNINITIALIZED = 0x0u,
    GPIO_DEVICE_INITIALIZED = 0x1u,
};

struct GpioDevice {
    asr_gpio_dev_t dev;
    struct GpioResource resource;
};

static struct GpioCntlr g_gpioCntlr;
struct OemGpioIrqHandler {
    uint8_t port;
    GpioIrqFunc func;
    void *arg;
};

static asr_gpio_dev_t g_gpioPinMap[ASR_GPIO_TOTAL_NUM] = {0};
static asr_gpio_irq_trigger_t g_gpioIrqCfg[ASR_GPIO_TOTAL_NUM] = {0};

static void OemGpioIrqHdl(uint16_t *arg)
{
    asr_gpio_dev_t *dev = NULL;
    uint16_t gpio = (uint16_t)arg;

    dev = &g_gpioPinMap[gpio];
    if ((uint16_t)dev->port >= ASR_GPIO_TOTAL_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, (uint16_t)dev->port);
        return;
    }
    GpioCntlrIrqCallback(&g_gpioCntlr, gpio);
}

/* dispatch */
int32_t GpioDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (client == NULL || client->device == NULL || data == NULL || reply == NULL) {
        HDF_LOGE("%s: client or client->device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

/* dev api */
static int32_t GpioDevWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    (void)cntlr;
    uint32_t ret = 0;
    asr_gpio_dev_t *dev = &g_gpioPinMap[gpio];

    if (val) {
        ret = asr_gpio_output_high(dev);
    } else {
        ret = asr_gpio_output_low(dev);
    }
    if (EIO == ret) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, dev->port);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    (void)cntlr;
    uint32_t ret = 0;
    asr_gpio_dev_t *dev = &g_gpioPinMap[gpio];

    if ((cntlr == NULL) || (val == NULL)) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = asr_gpio_input_get(dev, val);
    if (EIO == ret) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, dev->port);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    (void)cntlr;
    uint32_t ret = 0;
    asr_gpio_dev_t *dev = &g_gpioPinMap[gpio];

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    dev->config = (asr_gpio_config_t)dir;
    ret = asr_gpio_init(dev);
    if (EIO == ret) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, dev->port);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
    (void)cntlr;
    uint32_t ret = 0;
    asr_gpio_dev_t *dev = &g_gpioPinMap[gpio];

    if ((cntlr == NULL) || (dir == NULL)) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (dev->port >= ASR_GPIO_TOTAL_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, dev->port);
        return HDF_ERR_NOT_SUPPORT;
    }
    *dir = (uint16_t)dev->config;

    return HDF_SUCCESS;
}

static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode)
{
    (void)cntlr;
    asr_gpio_dev_t *dev = &g_gpioPinMap[gpio];

    HDF_LOGE("%s %d, pin:%d", __func__, __LINE__, (uint16_t)dev->port);
    if (dev->port >= ASR_GPIO_TOTAL_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, dev->port);
        return HDF_ERR_NOT_SUPPORT;
    }

    if ((mode == OSAL_IRQF_TRIGGER_RISING) || (mode == OSAL_IRQF_TRIGGER_FALLING)
        || (mode == (OSAL_IRQF_TRIGGER_RISING | OSAL_IRQF_TRIGGER_FALLING))) {
        g_gpioIrqCfg[gpio] = (asr_gpio_config_t)mode;
    } else {
        HDF_LOGE("%s %d, error mode:%d", __func__, __LINE__, mode);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevUnSetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    asr_gpio_dev_t *dev = &g_gpioPinMap[gpio];

    if (dev->port >= ASR_GPIO_TOTAL_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, dev->port);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    asr_gpio_dev_t *dev = &g_gpioPinMap[gpio];

    if ((uint16_t)dev->port >= ASR_GPIO_TOTAL_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, (uint16_t)dev->port);
        return HDF_ERR_NOT_SUPPORT;
    }
    
    dev->config = ASR_IRQ_MODE;
    asr_gpio_init(dev);
    asr_gpio_enable_irq(dev, g_gpioIrqCfg[gpio], OemGpioIrqHdl, (void *)gpio);
    HDF_LOGE("%s %d, pin:%d", __func__, __LINE__, (uint16_t)dev->port);

    return HDF_SUCCESS;
}

static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    asr_gpio_dev_t *dev = &g_gpioPinMap[gpio];

    if ((uint16_t)dev->port >= ASR_GPIO_TOTAL_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, (uint16_t)dev->port);
        return HDF_ERR_NOT_SUPPORT;
    }
    asr_gpio_disable_irq(dev);

    return HDF_SUCCESS;
}

struct GpioMethod g_GpioCntlrMethod = {
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

static int InitGpioDevice(struct GpioDevice *device)
{
    uint32_t ret = 0;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGE("%s %d, pin:%d, mode:%d", __func__, __LINE__, device->dev.port, device->dev.config);
    ret = asr_gpio_init(&device->dev);
    if (EIO == ret) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, device->dev.port);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define PLATFORM_GPIO_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), gpio_config)
static uint32_t GetGpioDeviceResource(struct GpioDevice *device)
{
    uint32_t relPin;
    int32_t ret;
    struct GpioResource *resource = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    resource->pinNum = HCS_PROP(PLATFORM_GPIO_CONFIG, pinNum);
    uint32_t pins[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, pin));
    uint32_t realPins[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, realPin));
    uint32_t configs[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, config));
    for (size_t i = 0; i < resource->pinNum; i++) {
        resource->pin = pins[i];
        resource->realPin = realPins[i];
        resource->config = configs[i];

        relPin = resource->realPin;// / DECIMALNUM * OCTALNUM + resource->realPin % DECIMALNUM;
        g_gpioPinMap[resource->pin].port = (uint8_t)relPin;
        g_gpioPinMap[resource->pin].config = (asr_gpio_config_t)resource->config;
        device->dev.config = (asr_gpio_config_t)resource->config;
        resource->pin = relPin;
        device->dev.port = relPin;

        ret = InitGpioDevice(device);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("InitGpioDevice FAIL\r\n");
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}
#else
static uint32_t GetGpioDeviceResource(
    struct GpioDevice *device, const struct DeviceResourceNode *resourceNode)
{
    uint32_t relPin;
    int32_t ret;
    struct GpioResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint32(resourceNode, "pinNum", &resource->pinNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("gpio config read pinNum fail");
        return HDF_FAILURE;
    }

    for (size_t i = 0; i < resource->pinNum; i++) {
        if (dri->GetUint32ArrayElem(resourceNode, "pin", i, &resource->pin, 0) != HDF_SUCCESS) {
            HDF_LOGE("gpio config read pin fail");
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "realPin", i, &resource->realPin, 0) != HDF_SUCCESS) {
            HDF_LOGE("gpio config read realPin fail");
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "config", i, &resource->config, 0) != HDF_SUCCESS) {
            HDF_LOGE("gpio config read config fail");
            return HDF_FAILURE;
        }

        relPin = resource->realPin;// / DECIMALNUM * OCTALNUM + resource->realPin % DECIMALNUM;
        g_gpioPinMap[resource->pin].port = (uint8_t)relPin;
        g_gpioPinMap[resource->pin].config = (asr_gpio_config_t)resource->config;
        device->dev.config = (asr_gpio_config_t)resource->config;
        device->dev.port = relPin;
        resource->pin = relPin;

        ret = InitGpioDevice(device);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("InitGpioDevice FAIL\r\n");
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}
#endif


static int32_t AttachGpioDevice(struct GpioCntlr *gpioCntlr, struct HdfDeviceObject *device)
{
    int32_t ret;

    struct GpioDevice *gpioDevice = NULL;
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (device == NULL) {
#else
    if (device == NULL || device->property == NULL) {
#endif
        HDF_LOGE("%s: param is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    gpioDevice = (struct GpioDevice *)OsalMemAlloc(sizeof(struct GpioDevice));
    if (gpioDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc gpioDevice error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    ret = GetGpioDeviceResource(gpioDevice);
#else
    ret = GetGpioDeviceResource(gpioDevice, device->property);
#endif
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(gpioDevice);
        return HDF_FAILURE;
    }

    (void)OsalMemFree(gpioDevice);
    gpioCntlr->count = gpioDevice->resource.pinNum;

    return HDF_SUCCESS;
}

static int32_t GpioDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct GpioCntlr *gpioCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PlatformDeviceBind(&g_gpioCntlr.device, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: bind hdf device failed:%d", __func__, ret);
        return ret;
    }

    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("GpioCntlrFromHdfDev fail\r\n");
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    ret = AttachGpioDevice(gpioCntlr, device); // GpioCntlr add GpioDevice to priv
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AttachGpioDevice fail\r\n");
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    gpioCntlr->ops = &g_GpioCntlrMethod; // register callback
    ret = GpioCntlrAdd(gpioCntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioCntlrAdd fail %d\r\n", gpioCntlr->start);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void GpioDriverRelease(struct HdfDeviceObject *device)
{
    struct GpioCntlr *gpioCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }

    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("GpioCntlrFromHdfDev fail\r\n");
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    (void)OsalMemFree(gpioCntlr->priv);
    gpioCntlr->count = 0;
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_GpioDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "ASR_GPIO_MODULE_HDF",
    .Init = GpioDriverInit,
    .Release = GpioDriverRelease,
};
HDF_INIT(g_GpioDriverEntry);
