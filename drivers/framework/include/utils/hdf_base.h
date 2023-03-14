/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup DriverUtils
 * @{
 *
 * @brief Defines common macros and interfaces of the driver module.
 *
 * This module provides interfaces such as log printing, doubly linked list operations, and work queues.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file hdf_base.h
 *
 * @brief Declares driver common types, including the enumerated values returned by the function
 * and the macro for obtaining the array size.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef HDF_BASE_TYPE_H
#define HDF_BASE_TYPE_H

#include "hdf_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Enumerates HDF return value types.
 */
typedef enum {
    HDF_SUCCESS  = 0, /**< The operation is successful. */
    HDF_FAILURE = -1, /**< Failed to invoke the OS underlying function. */
    HDF_ERR_NOT_SUPPORT = -2, /**< Not supported. */
    HDF_ERR_INVALID_PARAM = -3, /**< Invalid parameter. */
    HDF_ERR_INVALID_OBJECT = -4, /**< Invalid object. */
    HDF_ERR_MALLOC_FAIL    = -6, /**< Memory allocation fails. */
    HDF_ERR_TIMEOUT        = -7, /**< Timeout occurs. */
    HDF_ERR_THREAD_CREATE_FAIL = -10, /**< Failed to create a thread. */
    HDF_ERR_QUEUE_FULL  = -15, /**< The queue is full. */
    HDF_ERR_DEVICE_BUSY = -16, /**< The device is busy. */
    HDF_ERR_IO          = -17, /**< I/O error. */
    HDF_ERR_BAD_FD      = -18, /**< Incorrect file descriptor. */

#define HDF_BSP_ERR_START (-100) /**< Defines the start of the Board Support Package (BSP) module error codes. */
#define HDF_BSP_ERR_NUM(v) (HDF_BSP_ERR_START + (v)) /**< Defines the BSP module error codes. */
    HDF_BSP_ERR_OP = HDF_BSP_ERR_NUM(-1), /**< Failed to operate a BSP module. */
    HDF_ERR_BSP_PLT_API_ERR = HDF_BSP_ERR_NUM(-2), /**< The platform API of the BSP module is incorrect. */
    HDF_PAL_ERR_DEV_CREATE = HDF_BSP_ERR_NUM(-3), /**< Failed to create a BSP module device. */
    HDF_PAL_ERR_INNER = HDF_BSP_ERR_NUM(-4), /**< Internal error codes of the BSP module. */

#define HDF_DEV_ERR_START (-200) /**< Defines the start of the device module error codes. */
#define HDF_DEV_ERR_NUM(v) (HDF_DEV_ERR_START + (v)) /**< Defines the device module error codes. */
    HDF_DEV_ERR_NO_MEMORY               = HDF_DEV_ERR_NUM(-1), /**< Failed to allocate memory to the device module. */
    HDF_DEV_ERR_NO_DEVICE               = HDF_DEV_ERR_NUM(-2), /**< The device module has no device. */
    HDF_DEV_ERR_NO_DEVICE_SERVICE       = HDF_DEV_ERR_NUM(-3), /**< The device module has no device service. */
    HDF_DEV_ERR_DEV_INIT_FAIL           = HDF_DEV_ERR_NUM(-4), /**< Failed to initialize a device module. */
    HDF_DEV_ERR_PUBLISH_FAIL            = HDF_DEV_ERR_NUM(-5), /**< The device module failed to release a service. */
    HDF_DEV_ERR_ATTACHDEV_FAIL          = HDF_DEV_ERR_NUM(-6), /**< Failed to attach a device to a device module. */
    HDF_DEV_ERR_NODATA                  = HDF_DEV_ERR_NUM(-7), /**< Failed to read data from a device module. */
    HDF_DEV_ERR_NORANGE                 = HDF_DEV_ERR_NUM(-8), /**< The device module data is out of range. */
    HDF_DEV_ERR_OP                      = HDF_DEV_ERR_NUM(-10), /**< Failed to operate a device module. */
} HDF_STATUS;

/**
 * @brief Indicates that the function keeps waiting to obtain a semaphore or mutex.
 */
#define HDF_WAIT_FOREVER 0xFFFFFFFF

/**
 * @brief Defines the array size.
 */
#define HDF_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/**
 * @brief Defines a time conversion unit, for example, the unit for converting from second to millisecond.
 */
#define HDF_KILO_UNIT 1000

#ifdef __LITEOS__
#define HDF_LIBRARY_FULL_PATH(x) "/usr/lib/" x ".so"
#define HDF_LIBRARY_DIR "/usr/lib"
#define HDF_ETC_DIR "/etc"
#define HDF_CONFIG_DIR "/etc"
#else
#define HDF_LIBRARY_FULL_PATH(x) "/vendor/lib/" x ".z.so"
#define HDF_LIBRARY_DIR "/vendor/lib"
#define HDF_ETC_DIR "/vendor/etc"
#define HDF_CONFIG_DIR "/vendor/etc/hdfconfig"
#define HDF_MODULE_DIR "/vendor/modules/"
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_BASE_TYPE_H */
/** @} */
