/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup COMMON
 * @{
 *
 * @brief Provides common APIs of the platform driver.
 *
 * This module also provides <b>DevHandle</b>, which represents the common data structure of the platform driver.
 *
 * @since 1.0
 */

/**
 * @file platform_if.h
 *
 * @brief Declares common APIs of the platform driver.
 *
 * @since 1.0
 */

#ifndef PLATFORM_IF_H
#define PLATFORM_IF_H

#include "hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines the common device handle of the platform driver.
 *
 * The handle is associated with a specific platform device and is used as the
 * first input parameter for all APIs of the platform driver.
 *
 * @since 1.0
 */
typedef void* DevHandle;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLATFORM_IF_H */
/** @} */
