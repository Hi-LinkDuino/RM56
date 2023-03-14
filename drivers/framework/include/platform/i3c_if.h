/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup I3C
 * @{
 *
 * @brief Provides Improved Inter-Integrated Circuit (I3C) interfaces.
 *
 * This module allows a driver to perform operations on an I3C controller for accessing devices on the I3C bus,
 * including creating and destroying I3C controller handles as well as reading and writing data.
 *
 * @since 1.0
 */

/**
 * @file i3c_if.h
 *
 * @brief Declares the standard I3C interface functions.
 *
 * @since 1.0
 */

#ifndef I3C_IF_H
#define I3C_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** CCC(Common Command Code) structure */
struct I3cCccCmd;

/** Broatcast commands */
#define I3C_CCC_ENEC_B        0x00
#define I3C_CCC_DISEC_B       0x01
#define I3C_CCC_ENTAS0_B      0x02
#define I3C_CCC_ENTAS1_B      0x03
#define I3C_CCC_ENTAS2_B      0x04
#define I3C_CCC_ENTAS3_B      0x05
#define I3C_CCC_RSTDAA_B      0x06
#define I3C_CCC_ENTDAA        0x07
#define I3C_CCC_DEFSLVS       0x08
#define I3C_CCC_SETMWL_B      0x09
#define I3C_CCC_SETMRL_B      0x0a
#define I3C_CCC_ENTTM         0x0b
#define I3C_CCC_ENDXFER       0X12
#define I3C_CCC_ENTHDR0       0x20
#define I3C_CCC_ENTHDR1       0x21
#define I3C_CCC_ENTHDR2       0x22
#define I3C_CCC_ENTHDR3       0x23
#define I3C_CCC_ENTHDR4       0x24
#define I3C_CCC_ENTHDR5       0x25
#define I3C_CCC_ENTHDR6       0x26
#define I3C_CCC_ENTHDR7       0x27
#define I3C_CCC_SETXTIME_B    0x28
#define I3C_CCC_SETAASA       0X29
#define I3C_CCC_VENDOR_B      0x61
/** Driect commands */
#define I3C_CCC_ENEC_D        0x80
#define I3C_CCC_DISEC_D       0x81
#define I3C_CCC_ENTAS0_D      0x82
#define I3C_CCC_ENTAS1_D      0x83
#define I3C_CCC_ENTAS2_D      0x84
#define I3C_CCC_ENTAS3_D      0x85
#define I3C_CCC_RSTDAA_D      0x86
#define I3C_CCC_SETDASA       0x87
#define I3C_CCC_SETNEWDA      0x88
#define I3C_CCC_SETMWL_D      0x89
#define I3C_CCC_SETMRL_D      0x8a
#define I3C_CCC_GETMWL        0x8b
#define I3C_CCC_GETMRL        0x8c
#define I3C_CCC_GETPID        0x8d
#define I3C_CCC_GETBCR        0x8e
#define I3C_CCC_GETDCR        0x8f
#define I3C_CCC_GETSTATUS     0x90
#define I3C_CCC_GETACCMST     0x91
#define I3C_CCC_SETBRGTGT     0x93
#define I3C_CCC_GETMXDS       0x94
#define I3C_CCC_GETHDRCAP     0x95
#define I3C_CCC_SETXTIME_D    0x98
#define I3C_CCC_GETXTIME      0x99
#define I3C_CCC_VENDOR_D      0xe0

struct I3cCccCmd {
    /** CCC(Common Command Code) */
    uint8_t cmd;
    /** The destination address of the command, valid only if sending a direct CCC. */
    uint16_t dest;
    /** When sending a read command, <b>dataLength</b> represents the length of the read data */
    uint32_t dataLength;
    /** When sending a read command, <b>buf</b> is address of the buffer for storing transferred data */
    uint8_t *buf;
};

enum TransMode {
    /** I2C transfer mode */
    I2C_MODE = 0,
    /** I3C transfer mode */
    I3C_MODE,
    /** CCC(Common Command Code) mode */
    CCC_CMD_MODE,
};

enum I3cBusMode {
    /** Single Data Rate mode */
    I3C_BUS_SDR_MODE = 0,
    /** High Data Rate mode */
    I3C_BUS_HDR_MODE,
};

/**
 * @brief Defines the configurations of I3C controller.
 *
 * @since 1.0
 */
struct I3cConfig {
    /** I3C bus mode */
    enum I3cBusMode busMode;
    /** Current master object, its <b>NULL</b> when the controller is the master  */
    struct I3cDevice *curMaster;
};

/**
 * @brief Defines the I3C transfer message used during I3C transfer, I2C transfer or sending CCC(Common Command Code).
 *
 * @attention This structure does not limit the data transfer length specified by <b>len</b>.
 * The specific I3C controller determines the maximum length allowed. \n
 * The device address <b>addr</b> indicates the original device address and does not need to
 * contain the read/write flag bit.
 *
 * @since 1.0
 */
struct I3cMsg {
    /** Address of the target device */
    uint16_t addr;
    /** Address of the buffer for storing transferred data */
    uint8_t *buf;
    /** Length of the transferred data */
    uint16_t len;
    /**
     * Transfer Mode Flag | Description
     * ------------| -----------------------
     * I2C_FLAG_READ | Read flag
     * I2C_FLAG_READ_NO_ACK | No-ACK read flag
     * I2C_FLAG_IGNORE_NO_ACK | Ignoring no-ACK flag
     * I2C_FLAG_NO_START | No START condition flag
     * I2C_FLAG_STOP | STOP condition flag
     */
    uint16_t flags;
    /** Transfer mode select, using I2C_MODE defalut */
    enum TransMode mode;
    /** CCC(Common Command Code) structure, which is used in CCC_CMD_MODE transfer mode */
    struct I3cCccCmd *ccc;
    /** I3C error code, update by driver */
    uint16_t err;
};

/**
 * @brief Defines the data of I3C IBI(In-Band Interrupt).
 *
 * @attention The data of IBI can be obtained by payload and buf in the IBI function when generate an IBI.
 *
 * @since 1.0
 */
struct I3cIbiData {
    /** payload data length. Indicates the length of IBI payload data. It is automatically generated when an
     *  IBI is generated, do not modify while reading.
     */
    uint32_t payload;
    /** data buffer. The pointer of payload data. */
    uint8_t *buf;
};

enum I3cFlag {
    /** Read flag. The value <b>1</b> indicates the read operation, and <b>0</b> indicates the write operation. */
    I3C_FLAG_READ           = (0x1 << 0),
    /** Non-ACK read flag. The value <b>1</b> indicates that no ACK signal is sent during the read process. */
    I3C_FLAG_READ_NO_ACK    = (0x1 << 11),
    /** Ignoring no-ACK flag. The value <b>1</b> indicates that the non-ACK signal is ignored. */
    I3C_FLAG_IGNORE_NO_ACK  = (0x1 << 12),
    /**
     * No START condition flag. The value <b>1</b> indicates that there is no START condition for the message
     * transfer.
     */
    I3C_FLAG_NO_START       = (0x1 << 14),
    /** STOP condition flag. The value <b>1</b> indicates that the current transfer ends with a STOP condition. */
    I3C_FLAG_STOP           = (0x1 << 15),
};

/**
 * @brief IBI(In-Band Interrupt) callback function.
 *
 * Use {@link I3cRequestIbi} to attach it to an I3C device.
 *
 * @param handle Indicates the pointer to the device handle of the I3C controller obtained via {@link I3cOpen}.
 * @param addr Indicates the address of device to requeset IBI(In-Band Interrupt).
 * @param data Indicates the data structure of IBI.
 *
 * @return Returns <b>0</b> if the operation is successful; Returns a negative value otherwise.
 * @since 1.0
 */
typedef int32_t (*I3cIbiFunc)(DevHandle handle, uint16_t addr, struct I3cIbiData data);

/**
 * @brief Obtains the handle of an I3C controller.
 *
 * You must call this function before accessing the I3C bus.
 *
 * @param number Indicates the I3C controller ID.
 *
 * @return Returns the pointer to the {@link DevHandle} of the I3C controller if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
DevHandle I3cOpen(int16_t number);

 /**
 * @brief Releases the handle of an I3C controller.
 *
 * If you no longer need to access the I3C controller, you should call this function to close its handle so as
 * to release unused memory resources.
 *
 * @param handle Indicates the pointer to the device handle of the I3C controller obtained via {@link I3cOpen}.
 *
 * @since 1.0
 */
void I3cClose(DevHandle handle);

 /**
 * @brief Launches an transfer to an I3C device or a compatible I2C device,
 *         or send a CCC (Common Command Code) to an I3C device which is supported.
 *
 * @param handle Indicates the pointer to the device handle of the I3C controller obtained via {@link I3cOpen}.
 * @param msg Indicates the pointer to the I3C transfer message structure array.
 * @param count Indicates the length of the message structure array.
 * @param mode Indicates the transmission mode.
 *
 * @return Returns the number of transferred message structures if the operation is successful;
 * returns a negative value otherwise.
 * @see I3cMsg
 * @attention This function does not limit the number of message structures specified by <b>count</b> or the data
 * length of each message structure. The specific I3C controller determines the maximum number and data length allowed.
 *
 * @since 1.0
 */
int32_t I3cTransfer(DevHandle handle, struct I3cMsg *msg, int16_t count, enum TransMode mode);

 /**
 * @brief Requeset an IBI(in-bind interrupt) for an I3C device which is supported.
 *
 * @param handle Indicates the pointer to the device handle of the I3C controller obtained via {@link I3cOpen}.
 * @param addr Indicates the address of device to requeset IBI(In-Band Interrupt).
 * @param func Indicates the call back function of the IBI.
 * @param payload Indicates the length of payload data, in bytes.
 *
 * @return Returns <b>0</b> if the operation is successful; Returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t I3cRequestIbi(DevHandle handle, uint16_t addr, I3cIbiFunc func, uint32_t payload);

 /**
 * @brief Free the IBI(in-bind interrupt) which is Requeseted by {@link I3cRequestIbi}.
 *
 * @param handle Indicates the pointer to the device handle of the I3C controller obtained via {@link I3cOpen}.
 * @param addr Indicates the address of device to free IBI.
 *
 * @return Returns <b>0</b> if the operation is successful; Returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t I3cFreeIbi(DevHandle handle, uint16_t addr);

 /**
 * @brief Set the config of a I3C controller.
 *
 * @param handle Indicates the pointer to the device handle of the I3C controller obtained via {@link I3cOpen}.
 * @param config Indicates the pointer of a configuration structure to set.
 *
 * @return Returns <b>0</b> if the operation is successful; Returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t I3cSetConfig(DevHandle handle, struct I3cConfig *config);

 /**
 * @brief Get the config of a I3C controller.
 *
 * @param handle Indicates the pointer to the device handle of the I3C controller obtained via {@link I3cOpen}.
 * @param config Indicates the structure used to store the configuration.
 *
 * @return Returns <b>0</b> if the operation is successful; Returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t I3cGetConfig(DevHandle handle, struct I3cConfig *config);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* I3C_IF_H */
/** @} */
