/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MMC_BLOCK_H
#define MMC_BLOCK_H

#include "mmc_corex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @Defines the default mmc sector size
 *
 */
#define MMC_SEC_SIZE  512

/**
 * @Defines the default mmc sector size shift
 *
 */
#define MMC_SEC_SHIFT 9

/**
 * @Defines the help macro for getting sector number
 *
 */
#define MMC_MAX_SEC_NR (MMC_MAX_BYTES >> MMC_SEC_SHIFT)

#define MMC_SEC_PARAM_INVALID(s, n) \
    (s >= MMC_MAX_SEC_NR || \
    n >= MMC_MAX_SEC_NR || \
    (MMC_MAX_SEC_NR - n) <= s)

/**
 * @Defines the max length of mmc block device name
 *
 */
#define MMC_BLOCK_NAME_LEN 32

/**
 * @Defines the structure used to identify a general block device.
 *
 */
struct MmcBlock {
    char name[MMC_BLOCK_NAME_LEN]; /* name of the block device */
    int32_t index;
    bool removable;
    size_t capacity;               /* sized by sector */
    size_t secSize;                /* sized by bytes */
    uint32_t errCnt;               /* err count on io transfer */
    void *bops;                    /* block operations of specific os */
    void *osData;                  /* os specific data */
    struct MmcDevice *mmc;
};

/**
 * @brief Block device init for the mmc  device.
 *
 * This function behaves differently in different OS
 *
 * @param mmcDevice Indicates the pointer to the mmc device.
 *
 * @return Returns 0 if init successfully; returns a negative value otherwise.
 */
int32_t MmcBlockInit(struct MmcDevice *mmcDevice);

/**
 * @brief Block device uninit for the mmc device.
 *
 * This function behaves differently in different OS
 *
 * @param mmcDevice Indicates the pointer to the mmc device.
 *
 */
void MmcBlockUninit(struct MmcDevice *mmcDevice);

/**
 * @brief Block device init for the mmc device in specific os.
 *
 * These function gona be implemented by specific os
 *
 * @param mmcDevice Indicates the pointer to the mmc device.
 *
 * @return Returns 0 if init successfully; returns a negative value otherwise.
 */
int32_t MmcBlockOsInit(struct MmcDevice *mmcDevice);

/**
 * @brief Block device uninit for the mmc device in specific os.
 *
 * These function gona be implemented by specific os
 *
 * @param mmcDevice Indicates the pointer to the mmc device.
 *
 */
void MmcBlockOsUninit(struct MmcDevice *mmcDevice);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MMC_BLOCK_H */
