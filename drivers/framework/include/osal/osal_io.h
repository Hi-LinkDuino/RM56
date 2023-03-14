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
 * @file osal_io.h
 *
 * @brief Declares I/O interfaces.
 *
 * This file provides operations, such as reading data from and writing data into an I/O address space,
 * remapping an I/O address space to its virtual address space, and unmapping an I/O virtual address
 * associated with the physical address.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OSAL_IO_H
#define OSAL_IO_H

#include "osal_io_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Writes one byte of data into an I/O address space.
 *
 * @param value Indicates the data to write.
 * @param address Indicates the address to write.
 *
 * @since 1.0
 * @version 1.0
 */
#define OSAL_WRITEB(value, address) writeb(value, address)
/**
 * @brief Writes a short integer into an I/O address space.
 *
 * @param value Indicates the data to write.
 * @param address Indicates the address to write.
 *
 * @since 1.0
 * @version 1.0
 */
#define OSAL_WRITEW(value, address) writew(value, address)
/**
 * @brief Writes an integer into an I/O address space.
 *
 * @param value Indicates the data to write.
 * @param address Indicates the address to write.
 *
 * @since 1.0
 * @version 1.0
 */
#define OSAL_WRITEL(value, address) writel(value, address)

/**
 * @brief Reads one byte of data from an I/O address space.
 *
 * @param address Indicates the address to read.
 * @return Returns the byte.
 *
 * @since 1.0
 * @version 1.0
 */
#define OSAL_READB(address) readb(address)
/**
 * @brief Reads a short integer from an I/O address space.
 *
 * @param address Indicates the address to read.
 * @return Returns the short integer.
 *
 * @since 1.0
 * @version 1.0
 */
#define OSAL_READW(address) readw(address)
/**
 * @brief Reads an integer from an I/O address space.
 *
 * @param address Indicates the address to read.
 * @return Returns the integer.
 *
 * @since 1.0
 * @version 1.0
 */
#define OSAL_READL(address) readl(address)

/**
 * @brief Remaps an I/O physical address to its virtual address.
 *
 * @param phys_addr Indicates the I/O physical address.
 * @param size Indicates the size of the physical address to remap.
 * @return Returns the virtual address.
 *
 * @since 1.0
 * @version 1.0
 */
static inline void *OsalIoRemap(unsigned long phys_addr, unsigned long size)
{
    return ioremap(phys_addr, size);
}

/**
 * @brief Unmaps an I/O virtual address associated with the physical address.
 *
 * The virtual address is the one returned by calling {@link OsalIoRemap}.
 *
 * @param addr Indicates the pointer to the virtual address to unmap.
 *
 * @since 1.0
 * @version 1.0
 */
static inline void OsalIoUnmap(void *addr)
{
    iounmap(addr);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_IO_H */
/** @} */
