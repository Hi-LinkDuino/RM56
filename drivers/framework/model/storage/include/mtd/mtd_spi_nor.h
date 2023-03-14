/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MTD_SPI_NOR_H
#define MTD_SPI_NOR_H

#include "mtd_core.h"
#include "mtd_spi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MTD_SPI_NOR_CS_MAX       2

#define MTD_SPI_ADDR_3BYTE       3
#define MTD_SPI_ADDR_4BYTE       4

/**
 * @Defines the structure used to describe a spi nor flash.
 *
 */
struct SpinorInfo {
    const char *name;
    uint8_t id[MTD_FLASH_ID_LEN_MAX];
    uint8_t idLen;
    uint32_t blockSize;
    uint32_t chipSize;
    uint32_t addrCycle;
    uint32_t qeEnable;
    uint32_t qeSupport;
    struct MtdSpiConfig eraseCfg;
    struct MtdSpiConfig writeCfg;
    struct MtdSpiConfig readCfg;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MTD_SPI_NOR_H */
