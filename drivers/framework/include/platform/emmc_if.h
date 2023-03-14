/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup EMMC
 * @{
 *
 * @brief Declares standard APIs of basic embedded multi media card (EMMC) capabilities.
 *
 * You can use this module to access the EMMC and enable the driver to operate an EMMC-compliant device.
 * These capabilities include get CID of EMMC device.
 *
 * @since 1.0
 */

 /**
  * @file emmc_if.h
  *
  * @brief Declares the standard EMMC APIs.
  *
  * @since 1.0
  */

#ifndef EMMC_IF_H
#define EMMC_IF_H

#include "mmc_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Indicates that the CID len of EMMC device is 16 bytes.
 *
 * @since 1.0
 */
#define EMMC_CID_LEN 16

/**
 * @brief Opens an EMMC controller with a specified bus number.
 *
 * Before using the EMMC interface, you can obtain the device handle of the EMMC controller
 * by calling {@link EmmcOpen}. This function is used in pair with {@link EmmcClose}.
 *
 * @param mmcBusNum Indicates the bus number.
 *
 * @return Returns the device handle {@link DevHandle} of the EMMC controller if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
static inline DevHandle EmmcOpen(int16_t mmcBusNum)
{
    return MmcOpen(mmcBusNum);
}

/**
 * @brief Closes an EMMC controller.
 *
 * After the EMMC interface is used, you can close the EMMC controller by calling {@link EmmcClose}.
 * This function is used in pair with {@link EmmcOpen}.
 *
 * @param handle Indicates the device handle of the EMMC controller.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
static inline void EmmcClose(DevHandle handle)
{
    MmcClose(handle);
}

/**
 * @brief Get The CID of EMMC device.
 *
 * @param handle Indicates the pointer to the device handle of the EMMC controller.
 * @param cid Indicates the pointer to the CID to read.
 * @param size Indicates the length of the CID.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t EmmcGetCid(DevHandle handle, uint8_t *cid, uint32_t size);

/**
 * @brief Get The HUID(Hardware Unique ID, that is CID) of EMMC device.
 *
 * @param cid Indicates the pointer to the CID to read.
 * @param size Indicates the length of the CID.
 *
 * @since 1.0
 */
void EmmcGetHuid(uint8_t *cid, uint32_t size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* EMMC_IF_H */
/** @} */
