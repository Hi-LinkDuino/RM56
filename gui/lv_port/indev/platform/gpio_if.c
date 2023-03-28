#include "cmsis.h"
#include "gpio_if.h"
#include "hal_gpio.h"
#include "hal_iomux.h"

struct GpioDevice {
    uint16_t gpio;
    uint16_t dir;
    GpioIrqFunc func;
    void *arg;
    struct HAL_GPIO_IRQ_CFG_T irq_cfg;
};

static struct GpioDevice devs[HAL_GPIO_PIN_NUM] = {0};

int32_t GpioRead(uint16_t gpio, uint16_t *val)
{
    RETURN_VAL_IF_FAIL(gpio < HAL_GPIO_PIN_NUM, -1);
    uint8_t tmp = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)gpio);
    if (val) {
        *val = (uint16_t)tmp;
    }
    return 0;
}

int32_t GpioWrite(uint16_t gpio, uint16_t val)
{
    RETURN_VAL_IF_FAIL(gpio < HAL_GPIO_PIN_NUM, -1);
    if (val) {
        hal_gpio_pin_set((enum HAL_GPIO_PIN_T)gpio);
    } else {
        hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)gpio);
    }
    return 0;
}

int32_t GpioSetDir(uint16_t gpio, uint16_t dir)
{
    RETURN_VAL_IF_FAIL(gpio < HAL_GPIO_PIN_NUM, -1);
    struct HAL_IOMUX_PIN_FUNCTION_MAP mux = {(enum HAL_GPIO_PIN_T)gpio, HAL_IOMUX_FUNC_AS_GPIO,
                                             HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    if (dir == GPIO_DIR_IN_PULLUP || dir == GPIO_DIR_OUT_PULLUP) {
        mux.pull_sel = HAL_IOMUX_PIN_PULLUP_ENABLE;
    } else if (dir == GPIO_DIR_IN_PULLDOWN || dir == GPIO_DIR_OUT_PULLDOWN) {
        mux.pull_sel = HAL_IOMUX_PIN_PULLDOWN_ENABLE;
    }
    struct GpioDevice *dev = &devs[gpio];
    dev->dir = dir;
    hal_iomux_init(&mux, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)gpio, (enum HAL_GPIO_DIR_T)(dir & 0xf), 0);
    return 0;
}

int32_t GpioGetDir(uint16_t gpio, uint16_t *dir)
{
    RETURN_VAL_IF_FAIL(gpio < HAL_GPIO_PIN_NUM, -1);
    struct GpioDevice *dev = &devs[gpio];
    *dir = (uint16_t)dev->dir;
    return 0;
}

static void GpioIrqHandler(enum HAL_GPIO_PIN_T gpio)
{
    if (gpio < HAL_GPIO_PIN_NUM) {
        struct GpioDevice *dev = &devs[gpio];
        if ((dev->gpio == gpio) && !!(dev->func)) {
            dev->func((uint16_t)dev->gpio, dev->arg);
        }
    }
}

int32_t GpioSetIrq(uint16_t gpio, uint16_t mode, GpioIrqFunc func, void *arg)
{
    RETURN_VAL_IF_FAIL(gpio < HAL_GPIO_PIN_NUM, -1);
    RETURN_VAL_IF_FAIL((GPIO_IRQ_TRIGGER_RISING <= mode) && (mode <= GPIO_IRQ_TRIGGER_LOW), -1);
    RETURN_VAL_IF_FAIL(func != NULL, -1);
    struct GpioDevice *dev = &devs[gpio];
    dev->gpio = gpio;
    dev->func = func;
    dev->arg = arg;
    dev->irq_cfg.irq_debounce = 1;
    dev->irq_cfg.irq_enable = 1;
    dev->irq_cfg.irq_handler = GpioIrqHandler;
    switch (mode) {
    case GPIO_IRQ_TRIGGER_RISING:
        dev->irq_cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
        dev->irq_cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
        break;
    case GPIO_IRQ_TRIGGER_FALLING:
        dev->irq_cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
        dev->irq_cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
        break;
    case GPIO_IRQ_TRIGGER_HIGH:
        dev->irq_cfg.irq_type = HAL_GPIO_IRQ_TYPE_LEVEL_SENSITIVE;
        dev->irq_cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
        break;
    case GPIO_IRQ_TRIGGER_LOW:
        dev->irq_cfg.irq_type = HAL_GPIO_IRQ_TYPE_LEVEL_SENSITIVE;
        dev->irq_cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
        break;
    default:
        break;
    }
    return 0;
}

int32_t GpioUnSetIrq(uint16_t gpio)
{
    RETURN_VAL_IF_FAIL(gpio < HAL_GPIO_PIN_NUM, -1);
    struct GpioDevice *dev = &devs[gpio];
    memset(&dev->irq_cfg, 0, sizeof(struct HAL_GPIO_IRQ_CFG_T));
    return 0;
}

int32_t GpioEnableIrq(uint16_t gpio)
{
    RETURN_VAL_IF_FAIL(gpio < HAL_GPIO_PIN_NUM, -1);
    struct GpioDevice *dev = &devs[gpio];
    RETURN_VAL_IF_FAIL(dev->gpio == gpio && dev->func != NULL, -1);
    uint32_t flags = int_lock();
    dev->irq_cfg.irq_enable = 1;
    dev->irq_cfg.irq_debounce = 1;
    dev->irq_cfg.irq_handler = GpioIrqHandler;
    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)gpio, &dev->irq_cfg);
    int_unlock(flags);
    return 0;
}

int32_t GpioDisableIrq(uint16_t gpio)
{
    RETURN_VAL_IF_FAIL(gpio < HAL_GPIO_PIN_NUM, -1);
    struct GpioDevice *dev = &devs[gpio];
    uint32_t flags = int_lock();
    dev->irq_cfg.irq_enable = 0;
    dev->irq_cfg.irq_debounce = 0;
    dev->irq_cfg.irq_handler = NULL;
    int_unlock(flags);
    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)gpio, &dev->irq_cfg);
    return 0;
}