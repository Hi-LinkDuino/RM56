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
 * @file osal_mem.h
 *
 * @brief Declares the driver memory request and release interfaces.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OSAL_MEM_H
#define OSAL_MEM_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Allocates memory of a specified size.
 *
 * @param size Indicates the size of memory to allocate.
 *
 * @return Returns the pointer to the allocated memory if the operation is successful; returns <b>NULL</b> otherwise.
 * @since 1.0
 * @version 1.0
 */
void *OsalMemAlloc(size_t size);

/**
 * @brief Allocates memory of a specified size, and clears the allocated memory.
 *
 * @param size Indicates the size of memory to allocate.
 *
 * @return Returns the pointer to the allocated memory if the operation is successful; returns <b>NULL</b> otherwise.
 * @since 1.0
 * @version 1.0
 */
void *OsalMemCalloc(size_t size);

/**
 * @brief Allocates memory of a specified size, and aligns the memory address on a given boundary.
 *
 * @param alignment Indicates the memory boundary alignment. The value must be a power of 2.
 * @param size Indicates the size of memory to allocate.
 *
 * @return Returns the pointer to the allocated memory if the operation is successful; returns <b>NULL</b> otherwise.
 * @since 1.0
 * @version 1.0
 */
void *OsalMemAllocAlign(size_t alignment, size_t size);

/**
 * @brief Releases memory.
 *
 * @param mem Indicates the pointer to the memory to release.
 * @since 1.0
 * @version 1.0
 */
void OsalMemFree(void *mem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_MEM_H */
/** @} */
