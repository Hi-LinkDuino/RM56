/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup I2C
 * @{
 *
 * @brief Provides standard Inter-Integrated Circuit (I2C) interfaces.
 *
 * This module allows a driver to perform operations on an I2C controller for accessing devices on the I2C bus,
 * including creating and destroying I2C controller handles as well as reading and writing data.
 *
 * @since 1.0
 */

/**
 * @file i2c_if.h
 *
 * @brief Declares the standard I2C interface functions.
 *
 * @since 1.0
 */

#ifndef I2C_IF_H
#define I2C_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines the I2C transfer message used during custom transfers.
 *
 * @attention This structure does not limit the data transfer length specified by <b>len</b>.
 * The specific I2C controller determines the maximum length allowed. \n
 * The device address <b>addr</b> indicates the original device address and does not need to
 * contain the read/write flag bit.
 *
 * @since 1.0
 */
struct I2cMsg {
    /** Address of the I2C device */
    uint16_t addr;
    /** Address of the buffer for storing transferred data */
    uint8_t *buf;
    /** Length of the transferred data */
    uint16_t len;
    /**
     * Transfer Mode Flag | Description
     * ------------| -----------------------
     * I2C_FLAG_READ | Read flag
     * I2C_FLAG_ADDR_10BIT | 10-bit addressing flag
     * I2C_FLAG_READ_NO_ACK | No-ACK read flag
     * I2C_FLAG_IGNORE_NO_ACK | Ignoring no-ACK flag
     * I2C_FLAG_NO_START | No START condition flag
     * I2C_FLAG_STOP | STOP condition flag
     */
    uint16_t flags;
};

/**
 * @brief Enumerates flags used for transferring I2C messages.
 *
 * Multiple flags can be used to jointly control a single I2C message transfer.
 * If a bit is set to <b>1</b>, the corresponding feature is enabled. If a bit is set to <b>0</b>,
 * the corresponding feature is disabled.
 *
 * @since 1.0
 */
enum I2cFlag {
    /** Read flag. The value <b>1</b> indicates the read operation, and <b>0</b> indicates the write operation. */
    I2C_FLAG_READ           = (0x1 << 0),
    /** 10-bit addressing flag. The value <b>1</b> indicates that a 10-bit address is used. */
    I2C_FLAG_ADDR_10BIT     = (0x1 << 4),
    /** Non-ACK read flag. The value <b>1</b> indicates that no ACK signal is sent during the read process. */
    I2C_FLAG_READ_NO_ACK    = (0x1 << 11),
    /** Ignoring no-ACK flag. The value <b>1</b> indicates that the non-ACK signal is ignored. */
    I2C_FLAG_IGNORE_NO_ACK  = (0x1 << 12),
    /**
     * No START condition flag. The value <b>1</b> indicates that there is no START condition for the message
     * transfer.
     */
    I2C_FLAG_NO_START       = (0x1 << 14),
    /** STOP condition flag. The value <b>1</b> indicates that the current transfer ends with a STOP condition. */
    I2C_FLAG_STOP           = (0x1 << 15),
};

/**
 * @brief Obtains the handle of an I2C controller.
 *
 * You must call this function before accessing the I2C bus.
 *
 * @param number Indicates the I2C controller ID.
 *
 * @return Returns the pointer to the {@link DevHandle} of the I2C controller if the operation is successful;
 * returns <b>NULL</b> otherwise.
 * @since 1.0
 */
DevHandle I2cOpen(int16_t number);

 /**
 * @brief Releases the handle of an I2C controller.
 *
 * If you no longer need to access the I2C controller, you should call this function to close its handle so as
 * to release unused memory resources.
 *
 * @param handle Indicates the pointer to the device handle of the I2C controller.
 *
 * @since 1.0
 */
void I2cClose(DevHandle handle);

/**
 * @brief Launches a custom transfer to an I2C device.
 *
 * @param handle Indicates the pointer to the device handle of the I2C controller obtained via {@link I2cOpen}.
 * @param msgs Indicates the pointer to the I2C transfer message structure array.
 * @param count Indicates the length of the message structure array.
 *
 * @return Returns the number of transferred message structures if the operation is successful;
 * returns a negative value otherwise.
 * @see I2cMsg
 * @attention This function does not limit the number of message structures specified by <b>count</b> or the data
 * length of each message structure. The specific I2C controller determines the maximum number and data length allowed.
 *
 * @since 1.0
 */
int32_t I2cTransfer(DevHandle handle, struct I2cMsg *msgs, int16_t count);

/**
 * @brief Enumerates I2C I/O commands.
 *
 * @since 1.0
 */
enum I2cIoCmd {
    I2C_IO_TRANSFER = 0,      /**< Execute one or more I2C messages. */
    I2C_IO_OPEN = 1,          /**< Open the I2C device. */
    I2C_IO_CLOSE = 2,         /**< Close the I2C device. */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* I2C_IF_H */
/** @} */
