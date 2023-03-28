#include "gpio_peripheral_test.h"
#include "hal_iomux.h"
#include "hal_iomux_best2003.h"
#include "hal_trace.h"
struct bes_peripheral_gpio_irq_handler {
    uint8_t                 port;
    bes_peripheral_gpio_irq_handler_t  func;
    void                    *arg;
};

static struct bes_peripheral_gpio_irq_handler bes_peripheral_gpio_irq_handler[HAL_GPIO_PIN_LED_NUM] = {0};

#ifndef KEY_ON_NUM
    #define KEY_ON_NUM            HAL_IOMUX_PIN_P3_0
#endif

#ifndef KEY_OFF_NUM
    #define KEY_OFF_NUM            HAL_IOMUX_PIN_P3_1
#endif

void bes_peripheral_gpio_irq_hdl(enum HAL_GPIO_PIN_T pin)
{
    struct bes_peripheral_gpio_irq_handler *hdl_pointer = NULL;
    if (pin >= HAL_GPIO_PIN_LED_NUM) {
        printf("%s %d, error pin:%d", __func__, __LINE__, pin);
        return;
    }

    //rt_kprintf("%s %d, pin:%d", __func__, __LINE__, pin);
    hdl_pointer = &bes_peripheral_gpio_irq_handler[pin];
    if (hdl_pointer->port == pin) {
        if (hdl_pointer->func) {
            hdl_pointer->func(hdl_pointer->arg);
        } else {
            printf("%s %d, error hdl_pointer->func == NULL", __func__, __LINE__);
        }
    } else {
        printf("%s %d, error pin:%d, hdl_pointer->port:%d", __func__, __LINE__, pin, hdl_pointer->port);
    }
}

void key_low(void *args)
{
    printf("turn on beep!\n");
    hal_gpio_pin_set_dir(BEEP_PIN_NUM,HAL_GPIO_DIR_OUT,HAL_GPIO_PIN_LOW);
}

void key_high(void *args)
{
    printf("turn off beep!\n");

    hal_gpio_pin_set_dir(BEEP_PIN_NUM, HAL_GPIO_DIR_OUT,HAL_GPIO_PIN_HIGH);
}

static void gpio_config(struct GPIO_DEV_T *gpio)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_gpio;
    cfg_hw_gpio.pin = gpio->port;
    cfg_hw_gpio.function = HAL_IOMUX_FUNC_AS_GPIO;
    cfg_hw_gpio.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
	cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_NOPULL;

    if ((gpio->config == OUTPUT_PUSH_PULL) || (gpio->config == OUTPUT_OPEN_DRAIN_PULL_UP)
        || (gpio->config == INPUT_PULL_UP) || (gpio->config == IRQ_MODE)) {
        cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_PULLUP_ENABLE;
    } else if ((gpio->config == INPUT_PULL_DOWN)) {
        cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_PULLDOWN_ENABLE;
    }

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_hw_gpio, 1);
}

static void gpio_irq_config(struct GPIO_DEV_T *gpio)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

    hal_gpio_pin_set_dir(gpio->port, HAL_GPIO_DIR_IN, 0);
    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    gpiocfg.irq_handler = bes_peripheral_gpio_irq_hdl;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    hal_gpio_setup_irq(gpio->port, &gpiocfg);
}

static void gpio_irq_callback(enum HAL_GPIO_PIN_T pin, void (*irq_cb)(void *), void *irq_arg)
{
    bes_peripheral_gpio_irq_handler[pin].port  = pin;
    bes_peripheral_gpio_irq_handler[pin].func  = irq_cb;
    bes_peripheral_gpio_irq_handler[pin].arg   = irq_arg;
}

static void on_key_init(void)
{

    struct GPIO_DEV_T gpio;
    gpio.port = KEY_ON_NUM;
    gpio.config = INPUT_PULL_UP;

    if (gpio.port >= HAL_GPIO_PIN_LED_NUM) {
        return ;
    }
    gpio_config(&gpio);
    hal_iomux_set_function(gpio.port,HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_OP_KEEP_OTHER_FUNC_BIT);
    gpio_irq_config(&gpio);
    gpio_irq_callback(gpio.port,key_low,NULL);
}

static void off_key_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_gpio;
    struct GPIO_DEV_T gpio;
    gpio.port = KEY_OFF_NUM;
    gpio.config = INPUT_PULL_UP;

    if (gpio.port >= HAL_GPIO_PIN_LED_NUM) {
        return ;
    }
    gpio_config(&gpio);
    hal_iomux_set_function(gpio.port,HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_OP_KEEP_OTHER_FUNC_BIT);
    gpio_irq_config(&gpio);
    gpio_irq_callback(gpio.port,key_high,NULL);
}

void irq_key_test(void)
{
    printf("enter %s %d ",__func__,__LINE__);
    on_key_init();
    off_key_init();
}