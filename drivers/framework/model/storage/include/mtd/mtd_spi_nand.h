/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MTD_SPI_NAND_H
#define MTD_SPI_NAND_H

#include "mtd_core.h"
#include "mtd_nand.h"
#include "mtd_spi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MTD_SPI_NAND_RDID_ADDR           0x00
#define MTD_SPI_NAND_PROTECT_ADDR        0xa0
#define SPI_NAND_PROTECT_BP0_MASK  (1 << 3)
#define SPI_NAND_PROTECT_BP1_MASK  (1 << 4)
#define SPI_NAND_PROTECT_BP2_MASK  (1 << 5)
#define SPI_NAND_PROTECT_BP3_MASK  (1 << 6)
#define SPI_NAND_ALL_BP_MASK       (SPI_NAND_PROTECT_BP0_MASK | \
                                    SPI_NAND_PROTECT_BP1_MASK | \
                                    SPI_NAND_PROTECT_BP2_MASK | \
                                    SPI_NAND_PROTECT_BP3_MASK)

#define SPI_NAND_ANY_BP_ENABLE(x)  ((SPI_NAND_PROTECT_BP0_MASK & (x)) || \
                                    (SPI_NAND_PROTECT_BP1_MASK & (x)) || \
                                    (SPI_NAND_PROTECT_BP2_MASK & (x)) || \
                                    (SPI_NAND_PROTECT_BP3_MASK & (x)))

#define MTD_SPI_NAND_FEATURE_ADDR        0xb0

#define MTD_SPI_NAND_STATUS_ADDR         0xc0
#define MTD_SPI_NAND_ERASE_FAIL     0x04
#define MTD_SPI_NAND_PROG_FAIL      0x08

/**
 * @Defines the structure used to describe a spi nand flash.
 *
 */
struct SpinandInfo {
    const char *name;
    uint8_t id[MTD_FLASH_ID_LEN_MAX];
    uint8_t idLen;
    uint32_t chipSize;
    uint32_t blockSize;
    uint32_t pageSize;
    uint32_t oobSize;
    struct MtdSpiConfig eraseCfg;
    struct MtdSpiConfig writeCfg;
    struct MtdSpiConfig readCfg;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MTD_SPI_NAND_H */
