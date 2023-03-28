#ifndef  __GPIO_PER_TEST__
#define  __GPIO_PER_TEST__
#include <stdio.h>
#include "hal_iomux_best2003.h"
#include "hal_gpio.h"
#include "cmsis_os.h"
#include "cmsis_os2.h"
enum GPIO_HIGH_LOW{
    HAL_GPIO_PIN_LOW = 0,
    HAL_GPIO_PIN_HIGH,
};

enum GPIO_COMFIG_T{
    ANALOG_MODE,               /**< Used as a function pin, input and output analog */
    IRQ_MODE,                  /**< Used to trigger interrupt */
    INPUT_PULL_UP,             /**< Input with an internal pull-up resistor - use with devices
                                  that actively drive the signal low - e.g. button connected to ground */
    INPUT_PULL_DOWN,           /**< Input with an internal pull-down resistor - use with devices
                                  that actively drive the signal high - e.g. button connected to a power rail */
    INPUT_HIGH_IMPEDANCE,      /**< Input - must always be driven, either actively or by an external pullup resistor */
    OUTPUT_PUSH_PULL,          /**< Output actively driven high and actively driven low -
                                  must not be connected to other active outputs - e.g. LED output */
    OUTPUT_OPEN_DRAIN_NO_PULL, /**< Output actively driven low but is high-impedance when set high -
                                  can be connected to other open-drain/open-collector outputs.
                                  Needs an external pull-up resistor */
    OUTPUT_OPEN_DRAIN_PULL_UP, /**< Output actively driven low and is pulled high
                                  with an internal resistor when set high -
                                  can be connected to other open-drain/open-collector outputs. */
    OUTPUT_OPEN_DRAIN_AF,      /**< Alternate Function Open Drain Mode. */
    OUTPUT_PUSH_PULL_AF,       /**< Alternate Function Push Pull Mode. */
};

struct GPIO_DEV_T{
    uint8_t             port;   /**< gpio port */
    enum GPIO_COMFIG_T  config; /**< gpio config */
    void                *priv;   /**< priv data */
};
#ifndef BEEP_PIN_NUM
    #define BEEP_PIN_NUM            HAL_IOMUX_PIN_P0_1
#endif

typedef void (*bes_peripheral_gpio_irq_handler_t)(void *arg);
#endif/*__GPIO_PER_TEST__*/