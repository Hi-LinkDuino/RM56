/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup OSAL
 * @{
 *
 * @brief Defines the structures and interfaces for the Operating System Abstraction Layer (OSAL) module.
 *
 * The OSAL module OpenHarmony OS interface differences and provides unified OS interfaces externally,
 * including the memory management, thread, mutex, spinlock, semaphore, timer, file, interrupt, time,
 * atomic, firmware, and I/O operation modules.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file osal_irq.h
 *
 * @brief Declares interrupt request (IRQ) interfaces and common IRQ trigger modes.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OSAL_IRQ_H
#define OSAL_IRQ_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Enumerates interrupt trigger modes.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    OSAL_IRQF_TRIGGER_NONE = 0, /**< Edge-triggered is not set */
    OSAL_IRQF_TRIGGER_RISING = 1, /**< Rising edge triggered */
    OSAL_IRQF_TRIGGER_FALLING = 2, /**< Falling edge triggered */
    OSAL_IRQF_TRIGGER_HIGH = 4, /**< High-level triggered */
    OSAL_IRQF_TRIGGER_LOW = 8, /**< Low-level triggered */
} OSAL_IRQ_TRIGGER_MODE;

/**
 * @brief Defines an IRQ type.
 *
 * @since 1.0
 * @version 1.0
 */
typedef uint32_t (*OsalIRQHandle)(uint32_t irqId, void *dev);

/**
 * @brief Registers the function for processing the specified IRQ.
 *
 * @param irqId Indicates the IRQ ID.
 * @param config Indicates the interrupt trigger mode. For details, see {@link OSAL_IRQ_TRIGGER_MODE}.
 * @param handle Indicates the interrupt processing function.
 * @param name Indicates the pointer to the device name for registering an IRQ.
 * @param dev Indicates the pointer to the parameter passed to the interrupt processing function.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to register the IRQ.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalRegisterIrq(uint32_t irqId, uint32_t config, OsalIRQHandle handle, const char *name, void *dev);

/**
 * @brief Unregisters the interrupt processing function so that the system will no longer process the specified IRQ.
 *
 * @param irqId Indicates the IRQ ID.
 * @param dev Indicates the pointer to the parameter passed to the interrupt processing function
 *        in {@link OsalRegisterIrq}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to unregister the IRQ.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalUnregisterIrq(uint32_t irqId, void *dev);

/**
 * @brief Enables the processing of the specified IRQ.
 *
 * @param irqId Indicates the IRQ ID.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalEnableIrq(uint32_t irqId);

/**
 * @brief Disables the IRQ function of a device.
 *
 * @param irqId Indicates the IRQ ID.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalDisableIrq(uint32_t irqId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_IRQ_H */
/** @} */
