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
#ifndef NORFLASH_GD25LQ32C_H
#define NORFLASH_GD25LQ32C_H

#include "plat_types.h"

/* bytes */
#define GD25LQ32C_PAGE_SIZE (256)
#define GD25LQ32C_SECTOR_SIZE (4096)
#define GD25LQ32C_BLOCK_SIZE (32*1024)
#define GD25LQ32C_TOTAL_SIZE (4*1024*1024)

#define GD25LE255E_TOTAL_SIZE (32*1024*1024)

#define GD25LE128E_TOTAL_SIZE (16*1024*1024)

#define GD25LQ64C_TOTAL_SIZE (8*1024*1024)

#define GD25LQ16C_TOTAL_SIZE (2*1024*1024)

#define GD25LQ80C_TOTAL_SIZE (1*1024*1024)

#define GD25Q80C_TOTAL_SIZE (1*1024*1024)

#define GD25D40C_TOTAL_SIZE (512*1024)

#define GD25D20C_TOTAL_SIZE (256*1024)

#define P25Q16L_TOTAL_SIZE (2*1024*1024)

#define P25Q80H_TOTAL_SIZE (1*1024*1024)

#define P25Q21H_TOTAL_SIZE (256*1024)

#define P25Q40H_TOTAL_SIZE (512*1024)

#define XT25Q08B_TOTAL_SIZE (1*1024*1024)

#define W25Q128JV_TOTAL_SIZE (16*1024*1024)

/* device cmd */
#define GD25LQ32C_CMD_ID 0x9F
#define GD25LQ32C_CMD_WRITE_ENABLE 0x06
#define GD25LQ32C_CMD_PAGE_PROGRAM 0x02
#define GD25LQ32C_CMD_QUAD_PAGE_PROGRAM 0x32
#define GD25LQ32C_CMD_BLOCK_ERASE_32K 0x52
#define GD25LQ32C_CMD_BLOCK_ERASE_64K 0xD8
#define GD25LQ32C_CMD_BLOCK_ERASE GD25LQ32C_CMD_BLOCK_ERASE_32K
#define GD25LQ32C_CMD_SECTOR_ERASE 0x20
#define GD25LQ32C_CMD_CHIP_ERASE 0x60
#define GD25LQ32C_CMD_READ_STATUS_S0_S7 0x05
#define GD25LQ32C_CMD_READ_STATUS_S8_S15 0x35
#define GD25LQ32C_CMD_WRITE_STATUS 0x01

#define GD25LQ32C_CMD_FAST_QUAD_READ 0xEB
#define GD25LQ32C_CMD_FAST_DUAL_READ 0xBB
#define GD25LQ32C_CMD_STANDARD_READ 0x03
#define GD25LQ32C_CMD_STANDARD_FAST_READ 0x0B
#define GD25LQ32C_CMD_DEEP_POWER_DOWN 0xB9
#define GD25LQ32C_CMD_RELEASE_FROM_DP 0xAB

/* device register */
/* status register _S0_S7*/
#define GD25LQ32C_WIP_BIT_SHIFT 0
#define GD25LQ32C_WIP_BIT_MASK ((0x1)<<GD25LQ32C_WIP_BIT_SHIFT)
#define GD25LQ32C_WEL_BIT_SHIFT 1
#define GD25LQ32C_WEL_BIT_MASK ((0x1)<<GD25LQ32C_WEL_BIT_SHIFT)
#define GD25LQ32C_BP0_4_BIT_SHIFT 2
#define GD25LQ32C_BP0_4_BIT_MASK ((0x1F)<<GD25LQ32C_WEL_BIT_SHIFT)
#define GD25LQ32C_BP0_4_BIT(n) (((n) & 0x1F)<<GD25LQ32C_WEL_BIT_SHIFT)
/* status register _S8_S15*/
#define GD25LQ32C_QE_BIT_SHIFT 1
#define GD25LQ32C_QE_BIT_MASK ((0x1)<<GD25LQ32C_QE_BIT_SHIFT)
#define GD25LQ32C_CMP_BIT_SHIFT 6
#define GD25LQ32C_CMP_BIT_MASK ((0x1)<<GD25LQ32C_CMP_BIT_SHIFT)

#endif
