/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef STORAGE_CHAR_H
#define STORAGE_CHAR_H

#include "mtd_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CHAR_NAME_LEN 32

/**
 * @brief Char device init for the mtd device.
 *
 * This function behaves differently in different OS
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 * @return Returns 0 if init successfully; returns a negative value otherwise.
 */
int32_t MtdCharInit(struct MtdDevice *mtdDevice);

/**
 * @brief Char device uninit for the mtd device.
 *
 * This function behaves differently in different OS
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 */
void MtdCharUninit(struct MtdDevice *mtdDevice);

/**
 * @brief Char device init for the mtd device in specific OS.
 *
 * These function gona be implemented by specific os
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 * @return Returns 0 if init successfully; returns a negative value otherwise.
 */
int32_t MtdCharOsInit(struct MtdDevice *mtdDevice);

/**
 * @brief Char device uninit for the mtd device in specific OS.
 *
 * These function gona be implemented by specific os
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 */
void MtdCharOsUninit(struct MtdDevice *mtdDevice);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* STORAGE_CHAR_H */
