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
#include "cmsis.h"

#if (defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)) || \
        (defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U))

#include "cachel1_armv7.h"
#include "hal_cache.h"
#include "hal_location.h"

#define REG(a)                              (*(volatile uint32_t *)(a))

#define MSCR                                0xE001E000

uint8_t BOOT_TEXT_FLASH_LOC hal_cache_enable(enum HAL_CACHE_ID_T id)
{
#if (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
    if (id == HAL_CACHE_ID_I_CACHE) {
        REG(MSCR) |= (1 << 13);
    } else {
        REG(MSCR) |= (1 << 12);
    }
#endif

    if (id == HAL_CACHE_ID_I_CACHE) {
        SCB_EnableICache();
    } else {
        SCB_EnableDCache();
    }
    return 0;
}

uint8_t SRAM_TEXT_LOC hal_cache_disable(enum HAL_CACHE_ID_T id)
{
    if (id == HAL_CACHE_ID_I_CACHE) {
        SCB_DisableICache();
    } else {
        SCB_DisableDCache();
    }

#if (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
    if (id == HAL_CACHE_ID_I_CACHE) {
        REG(MSCR) &= ~(1 << 13);
    } else {
        REG(MSCR) &= ~(1 << 12);
    }
#endif
    return 0;
}

uint8_t BOOT_TEXT_FLASH_LOC hal_cache_writebuffer_enable(enum HAL_CACHE_ID_T id)
{
    return 0;
}

uint8_t hal_cache_writebuffer_disable(enum HAL_CACHE_ID_T id)
{
    return 0;
}

uint8_t hal_cache_writebuffer_flush(enum HAL_CACHE_ID_T id)
{
    // Drain the store buffer
    __DSB();
    return 0;
}

uint8_t BOOT_TEXT_FLASH_LOC hal_cache_writeback_enable(enum HAL_CACHE_ID_T id)
{
    return 0;
}

uint8_t hal_cache_writeback_disable(enum HAL_CACHE_ID_T id)
{
    return 0;
}

// Wrap is enabled during flash init
uint8_t BOOT_TEXT_SRAM_LOC hal_cache_wrap_enable(enum HAL_CACHE_ID_T id)
{
    return 0;
}

uint8_t hal_cache_wrap_disable(enum HAL_CACHE_ID_T id)
{
    return 0;
}

// Flash timing calibration might need to invalidate cache
uint8_t BOOT_TEXT_SRAM_LOC hal_cache_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len)
{
    if (id == HAL_CACHE_ID_I_CACHE) {
        SCB_InvalidateICache_by_Addr((void *)start_address, len);
    } else {
        SCB_InvalidateDCache_by_Addr((void *)start_address, len);
    }

    return 0;
}

uint8_t hal_cache_invalidate_all(enum HAL_CACHE_ID_T id)
{
    if (id == HAL_CACHE_ID_I_CACHE) {
        SCB_InvalidateICache();
    } else {
        SCB_InvalidateDCache();
    }

    return 0;
}

uint8_t hal_cache_sync_all(enum HAL_CACHE_ID_T id)
{
    if (id == HAL_CACHE_ID_D_CACHE) {
        SCB_CleanDCache();
    }

    return 0;
}

uint8_t BOOT_TEXT_SRAM_LOC hal_cache_sync(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len)
{
    if (id == HAL_CACHE_ID_D_CACHE) {
        SCB_CleanDCache_by_Addr((void *)start_address, len);
    }

    return 0;
}

#ifdef CORE_SLEEP_POWER_DOWN
void hal_cache_sleep(void)
{
}
void SRAM_TEXT_LOC hal_cache_wakeup(void)
{
}
#endif

#endif
