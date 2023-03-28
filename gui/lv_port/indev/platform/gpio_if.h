/**
 * @addtogroup GPIO
 * @{
 *
 * @brief Provides standard general-purpose input/output (GPIO) interfaces for driver development.
 *
 * You can use this module to perform operations on a GPIO pin, including setting the input/output direction,
 * reading/writing the level value, and setting the interrupt service routine (ISR) function.
 *
 * @since 1.0
 */

/**
 * @file gpio_if.h
 *
 * @brief Declares the standard GPIO interface functions.
 *
 * @since 1.0
 */

#ifndef GPIO_IF_H
#define GPIO_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates GPIO level values.
 *
 * @since 1.0
 */
enum GpioValue {
    GPIO_VAL_LOW = 0,  /**< Low GPIO level */
    GPIO_VAL_HIGH = 1, /**< High GPIO level */
    GPIO_VAL_ERR,      /**< Invalid GPIO level */
};

/**
 * @brief Enumerates GPIO directions.
 *
 * @since 1.0
 */
enum GpioDirType {
    GPIO_DIR_IN = 0,  /**< Input direction */
    GPIO_DIR_OUT = 1, /**< Output direction */
    GPIO_DIR_IN_PULLUP = (1 << 4) | 0,
    GPIO_DIR_IN_PULLDOWN = (2 << 4) | 0,
    GPIO_DIR_OUT_PULLUP = (1 << 4) | 1,
    GPIO_DIR_OUT_PULLDOWN = (2 << 4) | 1,
    GPIO_DIR_ERR, /**< Invalid direction */
};

/**
 * @brief Enumerates GPIO irq types.
 *
 * @since 1.0
 */
enum GpioIrqType {
    /** Trigger is not set */
    GPIO_IRQ_TRIGGER_NONE = 0,
    /** Rising edge triggered */
    GPIO_IRQ_TRIGGER_RISING = 1,
    /** Falling edge triggered */
    GPIO_IRQ_TRIGGER_FALLING = 2,
    /** High-level triggered */
    GPIO_IRQ_TRIGGER_HIGH = 4,
    /** Low-level triggered */
    GPIO_IRQ_TRIGGER_LOW = 8,
    /** execute interrupt service routine in thread context */
    GPIO_IRQ_USING_THREAD = (0x1 << 8),
};

/**
 * @brief Defines the function type of a GPIO interrupt service routine (ISR).
 *
 * This function is used when you call {@link GpioSetIrq} to register the ISR for a GPIO pin.
 *
 * @param gpio Indicates the GPIO number of the ISR.
 * @param data Indicates the pointer to the private data passed to this ISR (The data is specified when the ISR
 * is registered).
 *
 * @return Returns <b>0</b> if the ISR function type is successfully defined; returns a negative value otherwise.
 * @see GpioSetIrq
 * @since 1.0
 */
typedef void (*GpioIrqFunc)(uint16_t gpio, void *data);

/**
 * @brief Reads the level value of a GPIO pin.
 *
 * Before using this function, you need to call {@link GpioSetDir} to set the GPIO pin direction to input.
 *
 * @param gpio Indicates the GPIO pin number.
 * @param val Indicates the pointer to the read level value. For details, see {@link GpioValue}.
 *
 * @return Returns <b>0</b> if the GPIO pin level value is successfully read; returns a negative value otherwise.
 * @since 1.0
 */
int32_t GpioRead(uint16_t gpio, uint16_t *val);

/**
 * @brief Writes the level value for a GPIO pin.
 *
 * Before using this function, you need to call {@link GpioSetDir} to set the GPIO pin direction to output.
 *
 * @param gpio Indicates the GPIO pin number.
 * @param val Indicates the level value to be written. For details, see {@link GpioValue}.
 *
 * @return Returns <b>0</b> if the GPIO pin level value is successfully written; returns a negative value otherwise.
 * @since 1.0
 */
int32_t GpioWrite(uint16_t gpio, uint16_t val);

/**
 * @brief Sets the input/output direction for a GPIO pin.
 *
 * Generally, you can set the direction to input when external level signals need to be read, and set the
 * direction to output when the level signals need to be sent externally.
 *
 * @param gpio Indicates the GPIO pin number.
 * @param dir Indicates the direction to set. For details, see {@link GpioDirType}.
 *
 * @return Returns <b>0</b> if the GPIO pin direction is successfully set; returns a negative value otherwise.
 * @since 1.0
 */
int32_t GpioSetDir(uint16_t gpio, uint16_t dir);

/**
 * @brief Obtains the input/output direction of a GPIO pin.
 *
 * @param gpio Indicates the GPIO pin number.
 * @param dir Indicates the pointer to the obtained input/output direction. For details, see {@link GpioDirType}.
 *
 * @return Returns <b>0</b> if the GPIO pin direction is successfully obtained; returns a negative value otherwise.
 * @since 1.0
 */
int32_t GpioGetDir(uint16_t gpio, uint16_t *dir);

/**
 * @brief Sets the ISR function for a GPIO pin.
 *
 * Before using a GPIO pin as an interrupt, you must call this function to set an ISR function for this GPIO pin,
 * including the ISR parameters and the interrupt trigger mode.
 * After the setting is successful, you also need to call {@link GpioEnableIrq} to enable the interrupt, so that
 * the ISR function can respond correctly.
 *
 * @param gpio Indicates the GPIO pin number.
 * @param mode Indicates the interrupt trigger mode. For details, see {@link OSAL_IRQF_TRIGGER_RISING}.
 * @param func Indicates the ISR function to set, which is specified by {@link GpioIrqFunc}.
 * @param arg Indicates the pointer to the parameters passed to the ISR function.
 *
 * @return Returns <b>0</b> if the ISR function of the GPIO pin is successfully set; returns a negative value otherwise.
 * @since 1.0
 */
int32_t GpioSetIrq(uint16_t gpio, uint16_t mode, GpioIrqFunc func, void *arg);

/**
 * @brief Cancels the setting of the ISR function for a GPIO pin.
 *
 * If you do not need the GPIO pin as an interrupt, call this function to cancel the ISR function set via
 * {@link GpioSetIrq}. Since this ISR function is no longer valid, you are advised to use {@link GpioDisableIrq} to
 * disable the GPIO pin interrupt.
 *
 * @param gpio Indicates the GPIO pin number.
 *
 * @return Returns <b>0</b> if the ISR function of the GPIO pin is successfully cancelled; returns a negative value
 * otherwise.
 * @since 1.0
 */
int32_t GpioUnSetIrq(uint16_t gpio);

/**
 * @brief Enables a GPIO pin interrupt.
 *
 * Before enabling the interrupt, you must call {@link GpioSetIrq} to set the ISR function for the GPIO pin.
 *
 * @param gpio Indicates the GPIO pin number.
 *
 * @return Returns <b>0</b> if the GPIO pin interrupt is successfully enabled; returns a negative value otherwise.
 * @since 1.0
 */
int32_t GpioEnableIrq(uint16_t gpio);

/**
 * @brief Disables a GPIO pin interrupt.
 *
 * You can call this function when you need to temporarily mask a GPIO pin interrupt or cancel an ISR function.
 *
 * @param gpio Indicates the GPIO pin number.
 *
 * @return Returns <b>0</b> if the GPIO pin interrupt is successfully disabled; returns a negative value otherwise.
 * @since 1.0
 */
int32_t GpioDisableIrq(uint16_t gpio);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* GPIO_IF_H */
/** @} */
