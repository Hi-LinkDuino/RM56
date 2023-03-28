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
#include "plat_addr_map.h"

#define BUILD_INFO_LOCATION             __attribute__((section(".build_info")))

#define TO_STR_A(s)                     # s
#define TO_STR(s)                       TO_STR_A(s)

#ifdef TRACE_CRLF
#define NEW_LINE_STR                    "\r\n"
#else
#define NEW_LINE_STR                    "\n"
#endif

const char BUILD_INFO_LOCATION sys_build_info[] =
#if defined(ROM_BUILD) || defined(PROGRAMMER)
    __DATE__   " "   __TIME__ " " TO_STR(REVISION_INFO);
#else
    NEW_LINE_STR "CHIP=" TO_STR(CHIP)
#ifdef CHIP_SUBTYPE
    NEW_LINE_STR "CHIP_SUBTYPE=" TO_STR(CHIP_SUBTYPE)
#endif
    NEW_LINE_STR "KERNEL=" TO_STR(KERNEL)
#ifdef SOFTWARE_VERSION
    NEW_LINE_STR "SW_VER=" TO_STR(SOFTWARE_VERSION)
#endif
#ifdef OTA_BOOT_SIZE
    NEW_LINE_STR "SW_TYPE=OTA"
    NEW_LINE_STR "OTA_BOOT_OFFSET=" TO_STR(OTA_BOOT_OFFSET)
    NEW_LINE_STR "OTA_BOOT_SIZE=" TO_STR(OTA_BOOT_SIZE)
#else
    NEW_LINE_STR "CRASH_DUMP_SIZE=" TO_STR(CRASH_DUMP_SECTION_SIZE)
    NEW_LINE_STR "AUD_SEC_SIZE=" TO_STR(AUD_SECTION_SIZE)
    NEW_LINE_STR "USER_SEC_SIZE=" TO_STR(USERDATA_SECTION_SIZE)
    NEW_LINE_STR "FACT_SEC_SIZE=" TO_STR(FACTORY_SECTION_SIZE)
    NEW_LINE_STR "NV_REC_DEV_VER=" TO_STR(NV_REC_DEV_VER)
#endif
    NEW_LINE_STR "FLASH_BASE=" TO_STR(FLASH_NC_BASE)
    NEW_LINE_STR "FLASH_SIZE=" TO_STR(FLASH_SIZE)
#ifdef OTA_CODE_OFFSET
    NEW_LINE_STR "OTA_CODE_OFFSET=" TO_STR(OTA_CODE_OFFSET)
#endif
#ifdef OTA_REMAP_OFFSET
    NEW_LINE_STR "OTA_REMAP_OFFSET=" TO_STR(OTA_REMAP_OFFSET)
#endif
    NEW_LINE_STR "CRC32_OF_IMAGE=0x00000000"
#ifdef HW_VERSION_STRING
    NEW_LINE_STR "HW_VERSION=" TO_STR(HW_VERSION_STRING)
#endif
    //--------------------
    // Add new items above
    //--------------------
    NEW_LINE_STR "BUILD_DATE=" __DATE__   " "   __TIME__
    NEW_LINE_STR "REV_INFO=" TO_STR(REVISION_INFO)
    NEW_LINE_STR;
#endif

