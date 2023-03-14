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
 * @file hdf_log.h
 *
 * @brief Declares log printing functions of the driver module.
 * This module provides functions for printing logs at the verbose, debug, information, warning, and error levels.
 *
 * To use these functions, you must define <b>HDF_LOG_TAG</b>, for example, #define HDF_LOG_TAG evt.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDF_LOG_H
#define HDF_LOG_H

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif /* HDF_LOG_TAG */

/** Add quotation mark */
#define LOG_TAG_MARK_EXTEND(HDF_TAG) #HDF_TAG
#define LOG_TAG_MARK(HDF_TAG) LOG_TAG_MARK_EXTEND(HDF_TAG)

#ifndef LOG_TAG
#define LOG_TAG LOG_TAG_MARK(HDF_LOG_TAG)
#endif /* LOG_TAG */

#include "hdf_log_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Prints logs at the verbose level.
 *
 * To use this function, you must define <b>HDF_LOG_TAG</b>, for example, #define HDF_LOG_TAG evt.
 *
 * @since 1.0
 * @version 1.0
 */
#define HDF_LOGV(fmt, args...) HDF_LOGV_WRAPPER(fmt, ##args)
/**
 * @brief Prints logs at the debug level.
 *
 * To use this function, you must define <b>HDF_LOG_TAG</b>, for example, #define HDF_LOG_TAG evt.
 *
 * @since 1.0
 * @version 1.0
 */
#define HDF_LOGD(fmt, args...) HDF_LOGD_WRAPPER(fmt, ##args)
/**
 * @brief Prints logs at the information level.
 *
 * To use this function, you must define <b>HDF_LOG_TAG</b>, for example, #define HDF_LOG_TAG evt.
 *
 * @since 1.0
 * @version 1.0
 */
#define HDF_LOGI(fmt, args...) HDF_LOGI_WRAPPER(fmt, ##args)
/**
 * @brief Prints logs at the warning level.
 *
 * To use this function, you must define <b>HDF_LOG_TAG</b>, for example, #define HDF_LOG_TAG evt.
 *
 * @since 1.0
 * @version 1.0
 */
#define HDF_LOGW(fmt, args...) HDF_LOGW_WRAPPER(fmt, ##args)
/**
 * @brief Prints logs at the error level.
 *
 * To use this function, you must define <b>HDF_LOG_TAG</b>, for example, #define HDF_LOG_TAG evt.
 *
 * @since 1.0
 * @version 1.0
 */
#define HDF_LOGE(fmt, args...) HDF_LOGE_WRAPPER(fmt, ##args)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_LOG_H */
/** @} */
