/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MTD_NAND_H
#define MTD_NAND_H

#include "hdf_base.h"
#include "osal_mutex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates the page size type of a nand flash memory.
 *
 */
enum MtdNandPageSize {
    MTD_NAND_PAGE_SIZE_512 = 512,
    MTD_NAND_PAGE_SIZE_2K = 2048,
    MTD_NAND_PAGE_SIZE_4K = 4096,
    MTD_NAND_PAGE_SIZE_8K = 8192,
    MTD_NAND_PAGE_SIZE_16K = 16384,
};

/**
 * @brief Enumerates the ECC type of a nand flash memory.
 *
 */
enum MtdNandEccTYpe {
    MTD_NAND_ECC_0BIT = 0,
    MTD_NAND_ECC_8BIT_1K = 1,
    MTD_NAND_ECC_16BIT_1K = 2,
    MTD_NAND_ECC_24BIT_1K = 3,
    MTD_NAND_ECC_28BIT_1K = 4,
    MTD_NAND_ECC_40BIT_1K = 5,
    MTD_NAND_ECC_64BIT_1K = 6,
};

/**
 * @brief Defines the bad block area size of a nand flash memory.
 *
 */
#define MTD_NAND_BB_SIZE 2

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MTD_NAND_H */
