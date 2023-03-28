#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_timer.h"
#include "hal_trace.h"

#define mdelay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))

static void gpio_output_test(void)
{
    printf("set gpio %d high\r\n", HAL_GPIO_PIN_P0_2);
    hal_gpio_pin_set(HAL_GPIO_PIN_P0_2); // set high
    mdelay(1000);
    printf("set gpio %d low\r\n", HAL_GPIO_PIN_P0_2);
    hal_gpio_pin_clr(HAL_GPIO_PIN_P0_2); // set low
}

static void gpio_input_test(void)
{
    uint8_t val = 0;
    while (1) {
        uint8_t tmp = hal_gpio_pin_get_val(HAL_GPIO_PIN_P0_4);
        if (tmp != val) {
            printf("gpio %d toggle from %d to %d\r\n", HAL_GPIO_PIN_P0_4, val, tmp);
            val = tmp;
        }
        mdelay(100);
    }
}

static void gpio_irq_handler(enum HAL_GPIO_PIN_T pin)
{
    uint8_t val = hal_gpio_pin_get_val(pin);
    printf("irq: gpio %d, val %d\r\n", pin, val);
}

void gpio_test(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
        {HAL_GPIO_PIN_P0_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_GPIO_PIN_P0_4, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_GPIO_PIN_P0_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };

    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
    hal_gpio_pin_set_dir(HAL_GPIO_PIN_P0_2, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(HAL_GPIO_PIN_P0_4, HAL_GPIO_DIR_IN, 0);
    hal_gpio_pin_set_dir(HAL_GPIO_PIN_P0_5, HAL_GPIO_DIR_IN, 0);

    struct HAL_GPIO_IRQ_CFG_T cfg;
    cfg.irq_debounce = 1;
    cfg.irq_enable = 1;
    cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE; // HAL_GPIO_IRQ_TYPE_LEVEL_SENSITIVE
    cfg.irq_handler = gpio_irq_handler;
    cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING; // HAL_GPIO_IRQ_POLARITY_LOW_FALLING if key_down is low level
    hal_gpio_setup_irq(HAL_GPIO_PIN_P0_5, &cfg);

    gpio_output_test();

    gpio_input_test();
}
