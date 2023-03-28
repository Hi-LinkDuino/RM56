#include "gpio_peripheral_test.h"
#include "hal_iomux.h"
#include "hal_trace.h"

void gpio_beep_test(void)
{
    printf("enter %s %d ",__func__,__LINE__);
    struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_gpio;
    struct GPIO_DEV_T gpio;
    gpio.port = BEEP_PIN_NUM;
    gpio.config = OUTPUT_PUSH_PULL;

    if (gpio.port >= HAL_GPIO_PIN_LED_NUM) {
        return ;
    }

    cfg_hw_gpio.pin = gpio.port;
    cfg_hw_gpio.function = HAL_IOMUX_FUNC_AS_GPIO;
    cfg_hw_gpio.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
	cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_NOPULL;

    if ((gpio.config == OUTPUT_PUSH_PULL) || (gpio.config == OUTPUT_OPEN_DRAIN_PULL_UP)
        || (gpio.config == INPUT_PULL_UP) || (gpio.config == IRQ_MODE)) {
        cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_PULLUP_ENABLE;
    } else if ((gpio.config == INPUT_PULL_DOWN)) {
        cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_PULLDOWN_ENABLE;
    }

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_hw_gpio, 1);

    hal_gpio_pin_set_dir(BEEP_PIN_NUM,HAL_GPIO_DIR_OUT,HAL_GPIO_PIN_LOW);
    osDelay(1000);
    hal_gpio_pin_set_dir(BEEP_PIN_NUM,HAL_GPIO_DIR_OUT,HAL_GPIO_PIN_HIGH);
    osDelay(1000);
    return ;
}