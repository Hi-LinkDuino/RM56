/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_ERRNO_H
#define PLATFORM_ERRNO_H

#include "hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum PlatformErrno {
#define HDF_PLT_ERR_START HDF_BSP_ERR_START            /**< Error number start of platform. */
#define HDF_PLT_ERR_NUM(v) (HDF_PLT_ERR_START + (v))
    HDF_PLT_ERR_OS_API = HDF_ERR_BSP_PLT_API_ERR,
    HDF_PLT_ERR_OPEN_DEV = HDF_PAL_ERR_DEV_CREATE,     /**< Failed to open a device. */
    HDF_PLT_ERR_INNER = HDF_PAL_ERR_INNER,             /**< Internal error of platform framework. */
    HDF_PLT_ERR_NO_DEV = HDF_PLT_ERR_NUM(-5),          /**< There is no device present. */
    HDF_PLT_ERR_DEV_TYPE = HDF_PLT_ERR_NUM(-6),        /**< invalid device type */
    HDF_PLT_ERR_DEV_GET = HDF_PLT_ERR_NUM(-7),         /**< err on device get */
    HDF_PLT_ERR_DEV_ADD = HDF_PLT_ERR_NUM(-8),         /**< err on device add */
    HDF_PLT_ERR_DEV_FULL = HDF_PLT_ERR_NUM(-9),        /**< device of a manager is full */
    HDF_PLT_ERR_ID_REPEAT = HDF_PLT_ERR_NUM(-10),      /**< id number conflict */
    HDF_PLT_ERR_NAME_REPEAT = HDF_PLT_ERR_NUM(-11),    /**< name conflict */
    HDF_PLT_ERR_OBJ_REPEAT = HDF_PLT_ERR_NUM(-12),     /**< repeated object */
    HDF_PLT_ERR_NO_DATA = HDF_PLT_ERR_NUM(-13),        /**< no data available */
    HDF_PLT_RSC_NOT_AVL = HDF_PLT_ERR_NUM(-14),        /**< resource not available */
#define HDF_MMC_ERR_START (HDF_PLT_ERR_START - 50)     /**< Error number start for mmc. */
#define HDF_MMC_ERR_NUM(v) (HDF_MMC_ERR_START + (v))
    HDF_MMC_ERR_SWITCH_FAIL = HDF_MMC_ERR_NUM(-1),
    HDF_MMC_ERR_OTHER_CMD_IS_RUNNING = HDF_MMC_ERR_NUM(-2),
    HDF_MMC_ERR_ILLEGAL_SEQ = HDF_MMC_ERR_NUM(-3),
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLATFORM_ERRNO_H */
