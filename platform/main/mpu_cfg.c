/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#include "plat_addr_map.h"
#include "hal_trace.h"
#include "mpu.h"
#include "mpu_cfg.h"

#ifndef ROMX_BASE
#define ROMX_BASE                               ROM_BASE
#endif

#if (ROMX_BASE >= 0x800)
#define ROM_DATA_MAP_BASE                       ROMX_BASE
#else
#ifdef CHIP_BEST1000
#define ROM_DATA_MAP_BASE                       0x400
#else
#define ROM_DATA_MAP_BASE                       0x800
#endif
#endif

#ifndef RAM_TOTAL_SIZE
#define RAM_TOTAL_SIZE RAM_SIZE
#endif

#if defined(__ARM_ARCH_8M_MAIN__)
/* Never overlap the memory region for armv8 MPU, or there will be data abort
 * When access overlaped memory
 */
static const mpu_regions_t mpu_table[] = {
    {0,             ROM_DATA_MAP_BASE,  MPU_ATTR_EXEC,          MAIR_ATTR_INT_SRAM },
    {RAMX_BASE,     RAM_TOTAL_SIZE,     MPU_ATTR_READ_EXEC,     MAIR_ATTR_INT_SRAM },
#ifdef MPU_INIT_TABLE
    MPU_INIT_TABLE
#endif
};
#else
/* Can overlap the memory region for armv7 MPU, the attribute of high number region
 * will override the low one
 */
static const mpu_regions_t mpu_table[] = {
    {0,             ROM_DATA_MAP_BASE,    MPU_ATTR_NO_ACCESS },
    {RAMX_BASE,     RAM_TOTAL_SIZE,       MPU_ATTR_READ_EXEC },
    {RAM_BASE,      RAM_TOTAL_SIZE,       MPU_ATTR_READ_WRITE },
};
#endif

void mpu_cfg(void)
{
    int ret;

    ret = mpu_setup(mpu_table, ARRAY_SIZE(mpu_table));
    if (ret) {
        TR_INFO(TR_MOD(MAIN), "Warning, MPU is not setup correctly: %d", ret);
    }
}
