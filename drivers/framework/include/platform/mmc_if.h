/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup MMC
 * @{
 *
 * @brief Declares standard APIs of basic embedded multi media card (MMC) capabilities.
 *
 * You can use this module to access the MMC and enable the driver to operate an MMC-compliant device.
 * @since 1.0
 */

 /**
  * @file mmc_if.h
  *
  * @brief Declares the standard MMC APIs.
  *
  * @since 1.0
  */

#ifndef MMC_IF_H
#define MMC_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates MMC I/O commands.
 *
 * @since 1.0
 */
enum MmcIoCmd {
    MMC_CMD_DEV_PRESENT,
    MMC_CMD_MAX,
    EMMC_CMD_GET_CID,
    EMMC_CMD_MAX,
    SDIO_CMD_MAX,
};

/**
 * @brief Opens an MMC controller with a specified number.
 *
 * Before using the MMC interface, you can obtain the device handle of the MMC controller
 * by calling {@link MmcOpen}. This function is used in pair with {@link MmcClose}.
 *
 * @param number Indicates the mmc bus number.
 *
 * @return Returns the device handle {@link DevHandle} of the MMC controller if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
DevHandle MmcOpen(int16_t number);

/**
 * @brief Closes an MMC controller.
 *
 * After the MMC interface is used, you can close the MMC controller by calling {@link MmcClose}.
 * This function is used in pair with {@link MmcOpen}.
 *
 * @param handle Indicates device handle of the MMC controller.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
void MmcClose(DevHandle handle);

/**
 * @brief Check if a controller has a device present.
 *
 * This function is used to check if a controller has a device plugged.
 *
 * @param handle Indicates the device handle of the MMC controller.
 *
 * @return Returns <b>true</b> if there is a device present; otherwise <b>false</b>.
 *
 * @since 1.0
 */
bool MmcDevPresent(DevHandle handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MMC_IF_H */
