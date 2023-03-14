/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup PWM
 * @{
 *
 * @brief Provides standard pulse width modulation (PWM) interfaces for driver development.
 *
 * The PWM module abstracts the PWM capabilities of different system platforms to provide stable APIs for driver
 * development. You can use this module to create/release PWM device handles, set the PWM period, polarity, and duty
 * cycle, enable/disable PWM devices, and set/obtain the PWM configuration.
 *
 * @since 1.0
 */

/**
 * @file pwm_if.h
 *
 * @brief Declares standard PWM interfaces for driver development.
 *
 * A driver needs to use the PWM interfaces to perform operations on a PWM device.
 *
 * @since 1.0
 */

#ifndef PWM_IF_H
#define PWM_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Indicates the normal polarity of a PWM device.
 *
 * @since 1.0
 */
#define PWM_NORMAL_POLARITY 0

/**
 * @brief Indicates the inverted polarity of a PWM device.
 *
 * @since 1.0
 */
#define PWM_INVERTED_POLARITY 1

/**
 * @brief Indicates that a PWM device is in the disabled state.
 *
 * @since 1.0
 */
#define PWM_DISABLE_STATUS 0

/**
 * @brief Indicates that a PWM device is in the enabled state.
 *
 * @since 1.0
 */
#define PWM_ENABLE_STATUS 1

/**
 * @brief Enumerates PWM I/O commands.
 *
 * @since 1.0
 */
enum PwmIoCmd {
    PWM_IO_GET = 0,         /**< Get the PWM device. */
    PWM_IO_PUT,             /**< Put the PWM device. */
    PWM_IO_SET_CONFIG,      /**< Set config. */
    PWM_IO_GET_CONFIG,      /**< Get config. */
};

/**
 * @brief Defines the PWM device configuration parameters.
 *
 * @attention The specific PWM device determines which variables in this structure are supported.
 *
 * @since 1.0
 */
struct PwmConfig {
    uint32_t duty;    /**< Duty cycle, in nanoseconds */
    uint32_t period;  /**< PWM period, in nanoseconds */
    uint32_t number;  /**< Number of square waves to generate. A positive value indicates that
                       * the specified number of square waves will be generated, and <b>0</b> indicates
                       * that square waves will be continuously generated.
                       */
    uint8_t polarity; /**< Polarity
                       * --------------------- | ---------------------
                       * PWM_NORMAL_POLARITY | Normal polarity
                       * PWM_INVERTED_POLARITY | Inverted polarity
                       */
    uint8_t status;   /**< Running status
                       * ------------------ | ------------------
                       * PWM_DISABLE_STATUS | Disabled
                       * PWM_ENABLE_STATUS  | Enabled
                       */
};

/**
 * @brief Obtains the PWM device handle.
 *
 * @param num Indicates the PWM device number.
 *
 * @return Returns the PWM device handle if the operation is successful; returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
DevHandle PwmOpen(uint32_t num);

/**
 * @brief Releases the PWM device handle.
 *
 * @param handle Indicates the PWM device handle obtained via {@link PwmOpen}.
 *
 * @since 1.0
 */
void PwmClose(DevHandle handle);

/**
 * @brief Sets the PWM period.
 *
 * @param handle Indicates the PWM device handle obtained via {@link PwmOpen}.
 * @param period Indicates the PWM device period to set, in nanoseconds.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PwmSetPeriod(DevHandle handle, uint32_t period);

/**
 * @brief Sets the PWM duty cycle.
 *
 * @param handle Indicates the PWM device handle obtained via {@link PwmOpen}.
 * @param duty Indicates the duty cycle to set, in nanoseconds.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PwmSetDuty(DevHandle handle, uint32_t duty);

/**
 * @brief Sets the PWM polarity.
 *
 * @param handle Indicates the PWM device handle obtained via {@link PwmOpen}.
 * @param polarity Indicates the polarity to set, which can be {@link PWM_NORMAL_POLARITY}
 * or {@link PWM_INVERTED_POLARITY}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PwmSetPolarity(DevHandle handle, uint8_t polarity);

/**
 * @brief Enables the PWM device.
 *
 * @param handle Indicates the PWM device handle obtained via {@link PwmOpen}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PwmEnable(DevHandle handle);

/**
 * @brief Disables the PWM device.
 *
 * @param handle Indicates the PWM device handle obtained via {@link PwmOpen}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PwmDisable(DevHandle handle);

/**
 * @brief Sets the PWM device configuration parameters.
 *
 * @param handle Indicates the PWM device handle obtained via {@link PwmOpen}.
 * @param config Indicates the pointer to the {@link PwmConfig} structure that
 * contains PWM device configuration parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PwmSetConfig(DevHandle handle, struct PwmConfig *config);

/**
 * @brief Obtains the PWM device configuration parameters.
 *
 * @param handle Indicates the PWM device handle obtained via {@link PwmOpen}.
 * @param config Indicates the pointer to the {@link PwmConfig} structure that contains
 * PWM device configuration parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PwmGetConfig(DevHandle handle, struct PwmConfig *config);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PWM_IF_H */
/** @} */
