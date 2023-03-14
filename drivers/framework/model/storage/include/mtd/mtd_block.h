/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MTD_BLOCK_H
#define MTD_BLOCK_H

#include "mtd_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Block device init for the mtd device.
 *
 * This function behaves differently in different OS
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 * @return Returns 0 if init successfully; returns a negative value otherwise.
 */
int32_t MtdBlockInit(struct MtdDevice *mtdDevice);

/**
 * @brief Block device uninit for the mtd device.
 *
 * This function behaves differently in different OS
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 */
void MtdBlockUninit(struct MtdDevice *mtdDevice);

/**
 * @brief Block device init for the mtd device in specific os.
 *
 * These function gona be implemented by specific os
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 * @return Returns 0 if init successfully; returns a negative value otherwise.
 */
int32_t MtdBlockOsInit(struct MtdDevice *mtd);

/**
 * @brief Block device uninit for the mtd device in specific os.
 *
 * These function gona be implemented by specific os
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 */
void MtdBlockOsUninit(struct MtdDevice *mtd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MTD_BLOCK_H */
