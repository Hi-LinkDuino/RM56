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
 * @file osal_firmware.h
 *
 * @brief Declares firmware structures and interfaces.
 *
 * This file provides operations such as requesting and reading a firmware file, setting the offset for reading
 * a firmware file, and releasing a firmware file. The firmware file can be read in split mode.
 * The size of the firmware block to read each time is defined by the macro {@link HDF_FW_BLOCK_SIZE}.
 * The firmware file is requested by calling {@link OsalRequestFirmware}, the firmware block is read from
 * the firmware file by calling {@link OsalReadFirmware}, and the firmware block can also be randomly read
 * at a specified position by calling {@link OsalSeekFirmware}.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OSAL_FIRMWARE_H
#define OSAL_FIRMWARE_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Defines the data structure for operating a firmware file.
 */
struct OsalFirmware {
    uint32_t fwSize; /**< Firmware file size, which is returned by calling {@link OsalRequestFirmware}. */
    void *para; /**< Pointer to a firmware file, which is returned by calling {@link OsalRequestFirmware}.
                 * You do not need to allocate space.
                 */
};

/**
 * @brief Defines the data structure for reading a firmware file.
 *
 * This structure declares the firmware block to read each time. The firmware file can be read in split mode.
 */
struct OsalFwBlock {
    uint8_t *data; /**< Firmware content read this time. You do not need to allocate space. */
    bool endFlag; /**< Whether the firmware file is read completely */
    uint32_t dataSize; /**< Firmware block size read this time */
    int32_t curOffset; /**< Offset in the firmware file */
};

/**
 * @brief Requests a firmware file based on its name and device information.
 *
 * @param fw Indicates the pointer to the firmware file {@link OsalFirmware}, which cannot be empty.
 * @param fwName Indicates the pointer to the firmware file name, which cannot be empty.
 * @param device Indicates the pointer to the information about the device that requests the firmware file.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 * HDF_ERR_MALLOC_FAIL | Memory allocation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalRequestFirmware(struct OsalFirmware *fw, const char *fwName, void *device);

/**
 * @brief Sets the offset for reading a firmware file to implement random access to the firmware file.
 *
 * @param fw Indicates the pointer to the firmware file {@link OsalFirmware}.
 * @param offset Indicates the offset where the firmware content is to read.
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
int32_t OsalSeekFirmware(struct OsalFirmware *fw, uint32_t offset);

/**
 * @brief Reads a firmware file.
 *
 * @param fw Indicates the pointer to the firmware file {@link OsalFirmware}.
 * @param block Indicates the pointer to the firmware block to read. For details, see {@link OsalFwBlock}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalReadFirmware(struct OsalFirmware *fw, struct OsalFwBlock *block);

/**
 * @brief Releases a firmware file.
 *
 * After the firmware file is read, this function is called to release the firmware file.
 *
 * @param fw Indicates the pointer to the firmware file {@link OsalFirmware}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalReleaseFirmware(struct OsalFirmware *fw);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_FIRMWARE_H */
/** @} */
