/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#ifndef __PLAT_ADDR_MAP_H__
#define __PLAT_ADDR_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#elif defined(CHIP_FPGA1000)
#define CHIP_ID_LITERAL                         fpga1000
#elif defined(CHIP_BEST1000)
#define CHIP_ID_LITERAL                         best1000
#elif defined(CHIP_BEST1305)
#define CHIP_ID_LITERAL                         best1305
#elif defined(CHIP_BEST1400) || defined(CHIP_BEST1402)
#define CHIP_ID_LITERAL                         best1400
#elif defined(CHIP_BEST1501)
#define CHIP_ID_LITERAL                         best1501
#elif defined(CHIP_BEST1600)
#define CHIP_ID_LITERAL                         best1600
#elif defined(CHIP_BEST2000)
#define CHIP_ID_LITERAL                         best2000
#elif defined(CHIP_BEST2001)
#define CHIP_ID_LITERAL                         best2001
#elif defined(CHIP_BEST2002)
#define CHIP_ID_LITERAL                         best2002
#elif defined(CHIP_BEST2003)
#define CHIP_ID_LITERAL                         best2003
#elif defined(CHIP_BEST2300)
#define CHIP_ID_LITERAL                         best2300
#elif defined(CHIP_BEST2300A)
#define CHIP_ID_LITERAL                         best2300a
#elif defined(CHIP_BEST2300P)
#define CHIP_ID_LITERAL                         best2300p
#elif defined(CHIP_BEST3001) || defined(CHIP_BEST3005)
#define CHIP_ID_LITERAL                         best3001
#elif defined(CHIP_BEST3003)
#define CHIP_ID_LITERAL                         best3003
#elif defined(CHIP_BEST1501SIMU)
#define CHIP_ID_LITERAL                         best1501simu
#elif defined(CHIP_BEST1600SIMU)
#define CHIP_ID_LITERAL                         best1600simu
#else
#error "Unknown chip ID"
#endif

#define _TO_STRING_A(s)                         # s
#define _TO_STRING(s)                           _TO_STRING_A(s)

#define CONCAT_NAME_A(a, b)                     a ## b
#define CONCAT_NAME(a, b)                       CONCAT_NAME_A(a, b)
#define CONCAT_SUFFIX(a, b)                     a.b

#define CONSTRUCT_HDR_NAME_A(d, n)              d/n ## _ ## d
#define CONSTRUCT_HDR_NAME(d, n)                CONSTRUCT_HDR_NAME_A(d, n)
#define CHIP_SPECIFIC_HDR(n)                    _TO_STRING(CONSTRUCT_HDR_NAME(CHIP_ID_LITERAL, n).h)

#include CHIP_SPECIFIC_HDR(plat_addr_map)

#ifndef ROMX_BASE
#define ROMX_BASE                               ROM_BASE
#endif

#ifdef CHIP_HAS_TCM

#define RAM_TO_RAMX(d)                          (d)
#define RAMX_TO_RAM(d)                          (d)

#define FLASH_TO_FLASHX(d)                      (d)
#define FLASHX_TO_FLASH(d)                      (d)

#define PSRAM_TO_PSRAMX(d)                      (d)
#define PSRAMX_TO_PSRAM(d)                      (d)

#else /* !CHIP_HAS_TCM */

#define RAM_TO_RAMX(d)                          ((d) - RAM_BASE + RAMX_BASE)
#define RAMX_TO_RAM(d)                          ((d) - RAMX_BASE + RAM_BASE)

#define FLASH_TO_FLASHX(d)                      ((d) - FLASH_BASE + FLASHX_BASE)
#define FLASHX_TO_FLASH(d)                      ((d) - FLASHX_BASE + FLASH_BASE)

#define PSRAM_TO_PSRAMX(d)                      ((d) - PSRAM_BASE + PSRAMX_BASE)
#define PSRAMX_TO_PSRAM(d)                      ((d) - PSRAMX_BASE + PSRAM_BASE)

#endif /* !CHIP_HAS_TCM */

#define FLASH_C_TO_NC(d)                        ((d) - FLASH_BASE + FLASH_NC_BASE)
#define FLASH_NC_TO_C(d)                        ((d) - FLASH_NC_BASE + FLASH_BASE)

#define PSRAM_C_TO_NC(d)                        ((d) - PSRAM_BASE + PSRAM_NC_BASE)
#define PSRAM_NC_TO_C(d)                        ((d) - PSRAM_NC_BASE + PSRAM_BASE)

#define BUILD_INFO_MAGIC                        0xBE57341D

#if (RAM_BASE & (0x400 - 1))
#error "RAM_BASE should be aligned on 0x400 boundary"
#endif
#if defined(RAMCP_BASE) && (RAMCP_BASE & (0x400 - 1))
#error "RAMCP_BASE should be aligned on 0x400 boundary"
#endif

#ifdef __cplusplus
}
#endif

#endif
