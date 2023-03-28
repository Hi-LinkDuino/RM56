/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef NORFLASH_EN25S80B_H
#define NORFLASH_EN25S80B_H

#include "plat_types.h"

/* bytes */
#define EN25S80B_PAGE_SIZE (256)
#define EN25S80B_SECTOR_SIZE (4096)
#define EN25S80B_BLOCK_SIZE (32*1024)
#define EN25S80B_TOTAL_SIZE (1*1024*1024)

/* device cmd : FIXME fixed for EN25S80B */
#define EN25S80B_CMD_ID 0x9F
#define EN25S80B_CMD_WRITE_ENABLE 0x06
#define EN25S80B_CMD_PAGE_PROGRAM 0x02
#define EN25S80B_CMD_QUAD_PAGE_PROGRAM 0x32
#define EN25S80B_CMD_BLOCK_ERASE_32K 0x52
#define EN25S80B_CMD_BLOCK_ERASE_64K 0xD8
#define EN25S80B_CMD_BLOCK_ERASE EN25S80B_CMD_BLOCK_ERASE_32K
#define EN25S80B_CMD_SECTOR_ERASE 0x20
#define EN25S80B_CMD_CHIP_ERASE 0x60
#define EN25S80B_CMD_READ_STATUS_S0_S7 0x05
#define EN25S80B_CMD_READ_STATUS_S8_S15 0x35
#define EN25S80B_CMD_WRITE_STATUS 0x01
#define EN25S80B_CMD_WRITE_STATUS3 0xC0
#define EN25S80B_CMD_READ_STATUS3 0x95
#define EN25S80B_CMD_WRITE_STATUS_S0_S7 0x01
#define EN25S80B_CMD_WRITE_STATUS_S8_S15 0x31
#define EN25S80B_CMD_FAST_QUAD_READ 0xEB
#define EN25S80B_CMD_FAST_DUAL_READ 0xBB
#define EN25S80B_CMD_STANDARD_READ 0x03
#define EN25S80B_CMD_STANDARD_FAST_READ 0x0B
#define EN25S80B_CMD_DEEP_POWER_DOWN 0xB9
#define EN25S80B_CMD_RELEASE_FROM_DP 0xAB
#define EN25S80B_CMD_HIGH_PERFORMANCE 0xA3

#define EN25S80B_CMD_QUAD_RESET_ENABLE 0x66
#define EN25S80B_CMD_QUAD_RESET 0x99
#define EN25S80B_CMD_SPI_RESET_ENABLE 0x66
#define EN25S80B_CMD_SPI_RESET 0x99

#define EN25S80B_CMD_EQPI_SET 0x38
#define EN25S80B_CMD_EQPI_RELEASE 0xff

#define EN25S80B_CMD_ENTER_OTP 0x3A
#define EN25S80B_CMD_EXIT_OTP 0x04

/* device register */
/* status register */
#define EN25S80B_WIP_BIT_SHIFT 0
#define EN25S80B_WIP_BIT_MASK ((0x1)<<EN25S80B_WIP_BIT_SHIFT)
#define EN25S80B_WEL_BIT_SHIFT 1
#define EN25S80B_WEL_BIT_MASK ((0x1)<<EN25S80B_WEL_BIT_SHIFT)
#define EN25S80B_WHDIS_BIT_SHIFT 6
#define EN25S80B_WHDIS_BIT_MASK ((0x1)<<EN25S80B_WHDIS_BIT_SHIFT)


#endif /* NORFLASH_EN25S80B_H */
