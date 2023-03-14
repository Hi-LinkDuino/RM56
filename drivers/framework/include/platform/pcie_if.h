/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup PCIE
 * @{
 *
 * @brief Declares standard APIs of basic Peripheral Component Interconnect Express (PCIE) capabilities.
 *
 * You can use this module to access the PCIE and enable the driver to operate an PCIE device.
 * These capabilities include read and write the PCIE configuration Space.
 *
 * @since 1.0
 */

/**
 * @file pcie_if.h
 *
 * @brief Declares the standard PCIE interface functions.
 *
 * @since 1.0
 */

#ifndef PCIE_IF_H
#define PCIE_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Opens an PCIE controller with a specified bus number.
 *
 * Before using the PCIE interface, you can obtain the device handle of the PCIE controller
 * by calling {@link PcieOpen}. This function is used in pair with {@link PcieClose}.
 *
 * @param busNum Indicates the bus number.
 *
 * @return Returns the device handle {@link DevHandle} of the PCIE controller if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
DevHandle PcieOpen(uint16_t busNum);

/**
 * @brief Reads a given length of data from the PCIE configuration Space.
 *
 * @param handle Indicates the pointer to the device handle of the PCIE controller obtained by {@link PcieOpen}.
 * @param pos Indicates the start address of the data to read.
 * @param data Indicates the pointer to the data to read.
 * @param len Indicates the length of the data to read.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t PcieRead(DevHandle handle, uint32_t pos, uint8_t *data, uint32_t len);

/**
 * @brief Writes a given length of data into the PCIE configuration Space.
 *
 * @param handle Indicates the pointer to the device handle of the PCIE controller obtained by {@link PcieOpen}.
 * @param pos Indicates the start address of the data to write.
 * @param data Indicates the pointer to the data to write.
 * @param len Indicates the length of the data to write.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value if the operation fails.
 *
 * @since 1.0
 */
int32_t PcieWrite(DevHandle handle, uint32_t pos, uint8_t *data, uint32_t len);

/**
 * @brief Closes an PCIE controller.
 *
 * After the PCIE interface is used, you can close the PCIE controller by calling {@link PcieClose}.
 * This function is used in pair with {@link PcieOpen}.
 *
 * @param handle Indicates the pointer to the device handle of the PCIE controller.
 *
 * @since 1.0
 */
void PcieClose(DevHandle handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PCIE_IF_H */
