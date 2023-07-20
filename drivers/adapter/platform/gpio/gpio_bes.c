/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include "gpio_core.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hdf_log.h"
#include "osal_irq.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif

#define HDF_LOG_TAG GPIO_BES

#ifndef LOSCFG_SOC_SERIES_BES2600
#ifndef CONFIG_DISPLAY_SH8601Z
#define GPIO_SIM_BUTTON
#endif
#endif

#ifdef GPIO_SIM_BUTTON
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_disp.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
//map to key SW7
#define DEV_SIMULATE_GPIO  HAL_GPIO_PIN_P0_4
const struct HAL_IOMUX_PIN_FUNCTION_MAP iomux_cfg[] = {
    {DEV_SIMULATE_GPIO, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}
};

osSemaphoreDef(gpio_irq_sem);
osSemaphoreId gpio_irq_sem_id = NULL;

static void gpio_irq_job_thread(void const *argument);
osThreadDef(gpio_irq_job_thread, osPriorityHigh, 1, 1024*8, "touch_job_thread");
osThreadId gpio_irq_job_thread_id;

static void gpio_irq_job_thread(void const *arg)
{
    int ii = 0;
    osStatus_t ret = osOK;
	
    while(1)
    {
        ret = osSemaphoreAcquire(gpio_irq_sem_id, 0xffffffff);
        if(ret != osOK)
        {
            continue;
        }

        ii++;
        if(ii%2)
        {
            //HDF_LOGE("set lcd off ------------");
            DispOff(0);
        }
        else
        {
			ii = 0;
            //HDF_LOGE("set lcd on++++++++++++++++");
            DispOn(0);
        }
    }
}
static void gpio_irq_handler(enum HAL_GPIO_PIN_T pin)
{
	//HDF_LOGE("%s %ums---", __func__, __SLIM_TICKS_TO_MS(hal_sys_timer_get()));
	static uint32_t t1 = 0;
	uint32_t t2 = 0;
	uint32_t diff_t = 0;//ms


	if(pin != DEV_SIMULATE_GPIO)
	{
		return;
	}

	t2 = TICKS_TO_MS(hal_sys_timer_get());
	diff_t = t2 - t1;
	if(diff_t < 100)
	{
		return;
	}
	
	t1 = t2;
	osSemaphoreRelease(gpio_irq_sem_id);
}
static void bes_gpio_irq_enable(bool enable, uint32_t int_pin, uint32_t int_mode)
{
    uint32_t lock;
    struct HAL_GPIO_IRQ_CFG_T gpiocfg = {0};
    lock = int_lock_global();

    gpiocfg.irq_polarity = int_mode ;
    gpiocfg.irq_type     = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;

    if (enable)
    {
        gpiocfg.irq_enable   = true;
        gpiocfg.irq_debounce = true;
        gpiocfg.irq_handler  = gpio_irq_handler;
    }
    else
    {
        gpiocfg.irq_enable = false;
        gpiocfg.irq_debounce = false;
        gpiocfg.irq_handler = NULL;
    }

    hal_gpio_setup_irq(int_pin, &gpiocfg);

    int_unlock_global(lock);
}
void gpio_simulate_button(void)
{
	//irq process task.
	gpio_irq_sem_id = osSemaphoreCreate(osSemaphore(gpio_irq_sem), 0);
    //ASSERT(gpio_irq_sem_id, "create gpio_irq_sem_id fail!");

    gpio_irq_job_thread_id = osThreadCreate(osThread(gpio_irq_job_thread), NULL);
    //ASSERT(gpio_irq_job_thread_id, "Failed to create gpio_irq_job_thread_id");
	
    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_16, HAL_CMU_FREQ_104M);
    hal_iomux_init(iomux_cfg, sizeof(iomux_cfg)/sizeof(iomux_cfg[0]));
    hal_gpio_pin_set_dir(DEV_SIMULATE_GPIO,HAL_GPIO_DIR_IN, 0);

    bes_gpio_irq_enable(true, DEV_SIMULATE_GPIO, HAL_GPIO_IRQ_POLARITY_LOW_FALLING);
}
#endif

/*
 * Pin configuration
 */
enum GPIO_CONFIG {
    ANALOG_MODE,               /* Used as a function pin, input and output analog */
    IRQ_MODE,                  /* Used to trigger interrupt */
    INPUT_PULL_UP,             /* Input with an internal pull-up resistor - use with devices
                                  that actively drive the signal low - e.g. button connected to ground */
    INPUT_PULL_DOWN,           /* Input with an internal pull-down resistor - use with devices
                                  that actively drive the signal high - e.g. button connected to a power rail */
    INPUT_HIGH_IMPEDANCE,      /* Input - must always be driven, either actively or by an external pullup resistor */
    OUTPUT_PUSH_PULL,          /* Output actively driven high and actively driven low -
                                  must not be connected to other active outputs - e.g. LED output */
    OUTPUT_OPEN_DRAIN_NO_PULL, /* Output actively driven low but is high-impedance when set high -
                                  can be connected to other open-drain/open-collector outputs.
                                  Needs an external pull-up resistor */
    OUTPUT_OPEN_DRAIN_PULL_UP, /* Output actively driven low and is pulled high
                                  with an internal resistor when set high -
                                  can be connected to other open-drain/open-collector outputs. */
};

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
    uint8_t port; /* gpio port */
    struct GpioResource resource;
    enum GPIO_CONFIG config; /* gpio config */
};

typedef int32_t (*oem_gpio_irq_handler_t)(uint16_t gpio, void *data);

#define DECIMALNUM 10
#define OCTALNUM 8

static struct GpioCntlr g_gpioCntlr;

enum HAL_GPIO_PIN_T g_gpioPinReflectionMap[HAL_GPIO_PIN_LED_NUM] = {0};

static struct HAL_GPIO_IRQ_CFG_T g_gpioIrqCfg[HAL_GPIO_PIN_LED_NUM] = {0};

static void OemGpioIrqHdl(enum HAL_GPIO_PIN_T pin)
{
    if (pin >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, pin);
        return;
    }
    for (size_t i = 0; i < HAL_GPIO_PIN_LED_NUM; i++) {
        if (pin == (enum HAL_GPIO_PIN_T)g_gpioPinReflectionMap[i]) {
            GpioCntlrIrqCallback(&g_gpioCntlr, i);
            return;
        }
    }
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

/* HdfDriverEntry method definitions */
static int32_t GpioDriverInit(struct HdfDeviceObject *device);
static void GpioDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_GpioDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BES_GPIO_MODULE_HDF",
    .Init = GpioDriverInit,
    .Release = GpioDriverRelease,
};
HDF_INIT(g_GpioDriverEntry);

/* GpioMethod method definitions */
static int32_t GpioDevWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val);
static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val);
static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir);
static int32_t GpioDevGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir);
static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode);
static int32_t GpioDevUnSetIrq(struct GpioCntlr *cntlr, uint16_t gpio);
static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio);
static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio);
/* GpioMethod definitions */
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
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpioCfg;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    gpioCfg.pin = device->port;
    gpioCfg.function = HAL_IOMUX_FUNC_AS_GPIO;
    gpioCfg.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;

    if ((device->config == OUTPUT_PUSH_PULL) || (device->config == OUTPUT_OPEN_DRAIN_PULL_UP)
        || (device->config == INPUT_PULL_UP) || (device->config == IRQ_MODE)) {
        gpioCfg.pull_sel = HAL_IOMUX_PIN_PULLUP_ENABLE;
    } else if ((device->config == INPUT_PULL_DOWN)) {
        gpioCfg.pull_sel = HAL_IOMUX_PIN_PULLDOWN_ENABLE;
    } else {
        gpioCfg.pull_sel = HAL_IOMUX_PIN_NOPULL;
    }

    hal_iomux_init(&gpioCfg, 1);

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

        relPin = resource->realPin / DECIMALNUM * OCTALNUM + resource->realPin % DECIMALNUM;
        g_gpioPinReflectionMap[resource->pin] = relPin;
        device->config = resource->config;
        resource->pin = relPin;
        device->port = relPin;

        ret = InitGpioDevice(device);
		HDF_LOGE("%s: InitGpioDevice %d succeed", __func__, realPins[i]);
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

        relPin = resource->realPin / DECIMALNUM * OCTALNUM + resource->realPin % DECIMALNUM;
        g_gpioPinReflectionMap[resource->pin] = relPin;
        device->config = resource->config;
        resource->pin = relPin;
        device->port = relPin;

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
        OsalMemFree(gpioDevice);
        return HDF_FAILURE;
    }

    gpioCntlr->count = gpioDevice->resource.pinNum;
    gpioCntlr->priv = (void *)gpioDevice;
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
#ifdef GPIO_SIM_BUTTON
    gpio_simulate_button();
#endif
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

    gpioCntlr->ops = NULL;
    OsalMemFree(gpioCntlr->priv);
    gpioCntlr->count = 0;
}

/* dev api */
static int32_t GpioDevWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    (void)cntlr;
    uint16_t halGpio = g_gpioPinReflectionMap[gpio];
    if ((enum HAL_GPIO_PIN_T)halGpio >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, halGpio);
        return HDF_ERR_NOT_SUPPORT;
    }

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)halGpio, HAL_GPIO_DIR_OUT, val);

    return HDF_SUCCESS;
}

static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    (void)cntlr;
    uint16_t value;
    uint16_t halGpio = g_gpioPinReflectionMap[gpio];
    if ((enum HAL_GPIO_PIN_T)halGpio >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, halGpio);
        return HDF_ERR_NOT_SUPPORT;
    }

    value = (uint16_t)hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)halGpio);
    *val = value;

    return HDF_SUCCESS;
}

static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    (void)cntlr;
    uint16_t halGpio = g_gpioPinReflectionMap[gpio];
    if ((enum HAL_GPIO_PIN_T)halGpio >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, halGpio);
        return HDF_ERR_NOT_SUPPORT;
    }

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)halGpio, (enum HAL_GPIO_DIR_T)dir, 0);

    return HDF_SUCCESS;
}

static int32_t GpioDevGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
    (void)cntlr;
    uint16_t value;
    uint16_t halGpio = g_gpioPinReflectionMap[gpio];
    if ((enum HAL_GPIO_PIN_T)halGpio >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, gpio);
        return HDF_ERR_NOT_SUPPORT;
    }

    value = (uint16_t)hal_gpio_pin_get_dir((enum HAL_GPIO_PIN_T)halGpio);
    *dir = value;

    return HDF_SUCCESS;
}

static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode)
{
    (void)cntlr;
    enum HAL_GPIO_PIN_T pin = (enum HAL_GPIO_PIN_T)g_gpioPinReflectionMap[gpio];
    if (pin >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, pin);
        return HDF_ERR_NOT_SUPPORT;
    }

    if ((mode & OSAL_IRQF_TRIGGER_RISING) || (mode & OSAL_IRQF_TRIGGER_FALLING)) {
        g_gpioIrqCfg[pin].irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    } else if ((mode & OSAL_IRQF_TRIGGER_HIGH) || (mode & OSAL_IRQF_TRIGGER_LOW)) {
        g_gpioIrqCfg[pin].irq_type = HAL_GPIO_IRQ_TYPE_LEVEL_SENSITIVE;
    } else {
        HDF_LOGE("%s %d, error mode:%d", __func__, __LINE__, mode);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (mode & OSAL_IRQF_TRIGGER_RISING) {
        g_gpioIrqCfg[pin].irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    } else if (mode & OSAL_IRQF_TRIGGER_FALLING) {
        g_gpioIrqCfg[pin].irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    } else {
        g_gpioIrqCfg[pin].irq_polarity = mode;
    }
	if (mode & GPIO_IRQ_NO_DEBOUNCE) 
		g_gpioIrqCfg[pin].irq_debounce = false;
	else
		g_gpioIrqCfg[pin].irq_debounce = true;

    return HDF_SUCCESS;
}

static int32_t GpioDevUnSetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    enum HAL_GPIO_PIN_T pin = (enum HAL_GPIO_PIN_T)g_gpioPinReflectionMap[gpio];
    if (pin >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, pin);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    struct HAL_GPIO_IRQ_CFG_T gpioCfg;
    uint16_t halGpio = (enum HAL_GPIO_PIN_T)g_gpioPinReflectionMap[gpio];
    if ((enum HAL_GPIO_PIN_T)halGpio >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, (enum HAL_GPIO_PIN_T)halGpio);
        return HDF_ERR_NOT_SUPPORT;
    }

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)halGpio, HAL_GPIO_DIR_IN, 0);

    gpioCfg.irq_enable = true;
    gpioCfg.irq_debounce = g_gpioIrqCfg[(enum HAL_GPIO_PIN_T)halGpio].irq_debounce;;
    gpioCfg.irq_polarity = g_gpioIrqCfg[(enum HAL_GPIO_PIN_T)halGpio].irq_polarity;
    gpioCfg.irq_handler = OemGpioIrqHdl;
    gpioCfg.irq_type = g_gpioIrqCfg[(enum HAL_GPIO_PIN_T)halGpio].irq_type;
    g_gpioIrqCfg[halGpio] = gpioCfg;

	HDF_LOGE("%s %d,irq_debounce:%d", __func__, __LINE__, gpioCfg.irq_debounce);

    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)halGpio, &gpioCfg);

    return HDF_SUCCESS;
}

static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    uint16_t halGpio = (enum HAL_GPIO_PIN_T)g_gpioPinReflectionMap[gpio];
    if ((enum HAL_GPIO_PIN_T)halGpio >= HAL_GPIO_PIN_LED_NUM) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, halGpio);
        return HDF_ERR_NOT_SUPPORT;
    }

    const struct HAL_GPIO_IRQ_CFG_T gpioCfg = {
        .irq_enable = false,
        .irq_debounce = false,
        .irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING,
        .irq_handler = NULL,
        .irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE,
    };

    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)halGpio, &gpioCfg);

    return HDF_SUCCESS;
}
