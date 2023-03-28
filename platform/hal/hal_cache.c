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
#include "plat_types.h"
#include "plat_addr_map.h"

#if defined(ICACHE_CTRL_BASE) || defined(DCACHE_CTRL_BASE)

#include "cmsis.h"
#include "hal_cache.h"
#include "hal_location.h"
#if (CHIP_CACHE_VER <= 1)
#include "hal_norflash.h"
#endif
#include "hal_timer.h"
#include "hal_trace.h"

#define HAL_CACHE_YES                       1
#define HAL_CACHE_NO                        0

/* cache controller */
#if 0
#elif defined(CHIP_BEST1000)
#define CACHE_SIZE                          0x1000
#elif defined(CHIP_BEST1305) || defined(CHIP_BEST1400) || \
    defined(CHIP_BEST2000) || defined(CHIP_BEST2300) || defined(CHIP_BEST2300A) || defined(CHIP_BEST2300P) || \
    defined(CHIP_BEST3001) || defined(CHIP_BEST3003)
#define CACHE_SIZE                          0x2000
#elif defined(CHIP_BEST2003)
#define CACHE_SIZE                          0x8000
#else
#define CACHE_SIZE                          0x4000
#endif

#if 0
#elif defined(CHIP_BEST1000) || defined(CHIP_BEST2000) || defined(CHIP_BEST3001)
#define CACHE_LINE_SIZE                     0x10
#else
#define CACHE_LINE_SIZE                     0x20
#endif

#define CACHE_ASSOCIATIVITY_WAY_NUM         4

/* reg value */
#define CACHE_ENABLE_REG_OFFSET             0x00
#define CACHE_INI_CMD_REG_OFFSET            0x04
#define WRITEBUFFER_ENABLE_REG_OFFSET       0x08
#define WRITEBUFFER_FLUSH_REG_OFFSET        0x0C
#define LOCK_UNCACHEABLE_REG_OFFSET         0x10
#define INVALIDATE_ADDRESS_REG_OFFSET       0x14
#define INVALIDATE_SET_CMD_REG_OFFSET       0x18
// Since best2300
#define MON_ENABLE_REG_OFFSET               0x1C
// 64 bits when >= V4 and 40 bits when <= V3
#define MON_CNT0_READ_HIT_L_REG_OFFSET      0x20
#define MON_CNT0_READ_HIT_H_REG_OFFSET      0x24
#define MON_CNT0_READ_MISS_L_REG_OFFSET     0x28
#define MON_CNT0_READ_MISS_H_REG_OFFSET     0x2C
// Since best2300p
#define STATUS_REG_OFFSET                   0x30
// Since best2300p
#define SYNC_CMD_REG_OFFSET                 0x34
#if (CHIP_CACHE_VER >= 3)
#define INVALIDATE_ALL_CMD_OFFSET           0x38
#define SYNC_SET_CMD_REG_OFFSET             0x40
#endif
#define MON_CNT0_WRITE_HIT_L_REG_OFFSET     0x48
#define MON_CNT0_WRITE_HIT_H_REG_OFFSET     0x4C
#define MON_CNT0_WRITE_MISS_L_REG_OFFSET    0x50
#define MON_CNT0_WRITE_MISS_H_REG_OFFSET    0x54
#if (CHIP_CACHE_VER >= 4)
#define MON_CNT1_READ_HIT_L_REG_OFFSET      0x58
#define MON_CNT1_READ_HIT_H_REG_OFFSET      0x5C
#define MON_CNT1_READ_MISS_L_REG_OFFSET     0x60
#define MON_CNT1_READ_MISS_H_REG_OFFSET     0x64
#define MON_CNT1_WRITE_HIT_L_REG_OFFSET     0x68
#define MON_CNT1_WRITE_HIT_H_REG_OFFSET     0x6C
#define MON_CNT1_WRITE_MISS_L_REG_OFFSET    0x70
#define MON_CNT1_WRITE_MISS_H_REG_OFFSET    0x74
#endif

#define CACHE_EN                            (1 << 0)
// Since best2300
#define WRAP_EN                             (1 << 1)

#define WRITEBUFFER_EN                      (1 << 0)
// Since best2300
#define WRITE_BACK_EN                       (1 << 1)

#define LOCK_UNCACHEABLE                    (1 << 0)

// Since best2300
#define MONITOR_EN                          (1 << 0)

#define CNT_READ_HIT_31_0_SHIFT             (0)
#define CNT_READ_HIT_31_0_MASK              (0xFFFFFFFF << CNT_READ_HIT_31_0_SHIFT)
#define CNT_READ_HIT_31_0(n)                BITFIELD_VAL(CNT_READ_HIT_31_0, n)

#define CNT_READ_HIT_39_32_SHIFT            (0)
#define CNT_READ_HIT_39_32_MASK             (0xFF << CNT_READ_HIT_39_32_SHIFT)
#define CNT_READ_HIT_39_32(n)               BITFIELD_VAL(CNT_READ_HIT_39_32, n)

// Since best2300p
#define STATUS_FETCHING                     (1 << 0)

/* read write */
#define cacheip_write32(v,b,a) \
    (*((volatile uint32_t *)(b+a)) = v)
#define cacheip_read32(b,a) \
    (*((volatile uint32_t *)(b+a)))

struct SAVED_CACHE_REGS_T {
    uint32_t REG_CACHE_EN;
    uint32_t REG_WRITEBUF_EN;
    uint32_t REG_MONITOR_EN;
};

#ifdef CORE_SLEEP_POWER_DOWN
static SRAM_BSS_LOC struct SAVED_CACHE_REGS_T icache_regs;
#ifdef DCACHE_CTRL_BASE
static SRAM_BSS_LOC struct SAVED_CACHE_REGS_T dcache_regs;
#endif
#endif

__STATIC_FORCEINLINE void cacheip_enable_cache(uint32_t reg_base, uint32_t v)
{
    uint32_t val;
    if (v) {
        val = CACHE_EN;
    } else {
        val = 0;
    }
    cacheip_write32(val, reg_base, CACHE_ENABLE_REG_OFFSET);
}
__STATIC_FORCEINLINE POSSIBLY_UNUSED int cacheip_cached_enabled(uint32_t reg_base)
{
    uint32_t val;
    val = cacheip_read32(reg_base, CACHE_ENABLE_REG_OFFSET);
    return !!(val & CACHE_EN);
}
__STATIC_FORCEINLINE void cacheip_enable_wrap(uint32_t reg_base, uint32_t v)
{
    uint32_t val;
    val = cacheip_read32(reg_base, CACHE_ENABLE_REG_OFFSET);
    if (v) {
        val |= WRAP_EN;
    } else {
        val &= ~WRAP_EN;
    }
    cacheip_write32(val, reg_base, CACHE_ENABLE_REG_OFFSET);
}
__STATIC_FORCEINLINE POSSIBLY_UNUSED int cacheip_wrap_enabled(uint32_t reg_base)
{
    uint32_t val;
    val = cacheip_read32(reg_base, CACHE_ENABLE_REG_OFFSET);
    return !!(val & WRAP_EN);
}
__STATIC_FORCEINLINE void cacheip_init_cache(uint32_t reg_base)
{
    cacheip_write32(1, reg_base, CACHE_INI_CMD_REG_OFFSET);
}
__STATIC_FORCEINLINE void cacheip_enable_writebuffer(uint32_t reg_base, uint32_t v)
{
	//caches remove write buffer since 2003
#if (CHIP_CACHE_VER <= 2)
    // PSRAM controller V2 has an embedded write buffer and the cache write buffer can be ignored
#if defined(CHIP_HAS_PSRAM) && defined(PSRAM_ENABLE) && defined(CHIP_PSRAM_CTRL_VER) && (CHIP_PSRAM_CTRL_VER == 1)
    uint32_t val;

    val = cacheip_read32(reg_base, WRITEBUFFER_ENABLE_REG_OFFSET);
    if (v) {
        val |= WRITEBUFFER_EN;
    } else {
        val &= ~WRITEBUFFER_EN;
    }
    cacheip_write32(val, reg_base, WRITEBUFFER_ENABLE_REG_OFFSET);
#endif
#endif
}
__STATIC_FORCEINLINE void cacheip_enable_writeback(uint32_t reg_base, uint32_t v)
{
    // Cache implements write back feature since PSRAM controller V2
#if (defined(CHIP_HAS_PSRAM) && defined(PSRAM_ENABLE) && defined(CHIP_PSRAM_CTRL_VER) && (CHIP_PSRAM_CTRL_VER >= 2)) || (defined(CHIP_HAS_PSRAMUHS) && defined(PSRAMUHS_ENABLE))
    uint32_t val;

    val = cacheip_read32(reg_base, WRITEBUFFER_ENABLE_REG_OFFSET);
    if (v) {
        val |= WRITE_BACK_EN;
    } else {
        val &= ~WRITE_BACK_EN;
    }
    cacheip_write32(val, reg_base, WRITEBUFFER_ENABLE_REG_OFFSET);
#endif
}
__STATIC_FORCEINLINE void cacheip_flush_writebuffer(uint32_t reg_base)
{
    cacheip_write32(1, reg_base, WRITEBUFFER_FLUSH_REG_OFFSET);
}
__STATIC_FORCEINLINE void cacheip_set_invalidate_address(uint32_t reg_base, uint32_t v)
{
    cacheip_write32(v, reg_base, INVALIDATE_ADDRESS_REG_OFFSET);
}
__STATIC_FORCEINLINE void cacheip_trigger_invalidate(uint32_t reg_base)
{
    cacheip_write32(1, reg_base, INVALIDATE_SET_CMD_REG_OFFSET);
}
#if (CHIP_CACHE_VER >= 3)
__STATIC_FORCEINLINE void cacheip_trigger_sync(uint32_t reg_base)
{
    cacheip_write32(1, reg_base, SYNC_SET_CMD_REG_OFFSET);
}
#endif
__STATIC_FORCEINLINE void cacheip_trigger_sync_all(uint32_t reg_base)
{
    cacheip_write32(1, reg_base, SYNC_CMD_REG_OFFSET);
}
__STATIC_FORCEINLINE void cacheip_trigger_invalidate_all(uint32_t reg_base)
{
#if (CHIP_CACHE_VER >= 3)
    cacheip_write32(1, reg_base, INVALIDATE_ALL_CMD_OFFSET);
#else // CHIP_CACHE_VER >= 3
    cacheip_init_cache(reg_base);
#ifdef CHIP_BEST2001
    // For writeback data cache only (psram data)
    cacheip_init_cache(reg_base);
#endif
#endif // CHIP_CACHE_VER >= 3
}
__STATIC_FORCEINLINE void cacheip_enable_monitor(uint32_t reg_base, uint32_t v)
{
    cacheip_write32(v, reg_base, MON_ENABLE_REG_OFFSET);
}
__STATIC_FORCEINLINE uint64_t cacheip_read64(uint32_t reg_base, uint32_t reg_offset_l, uint32_t reg_offset_h)
{
    uint32_t data[2];
    uint32_t tmp;

    data[1] = cacheip_read32(reg_base, reg_offset_h);
    data[0] = cacheip_read32(reg_base, reg_offset_l);
    tmp = cacheip_read32(reg_base, reg_offset_h);
    if (data[1] != tmp) {
        data[1] = tmp;
        data[0] = cacheip_read32(reg_base, reg_offset_l);
    }

    return (((uint64_t)data[1] << 32) | data[0]);
}
__STATIC_FORCEINLINE void cacheip_get_monitor_data(uint32_t reg_base, struct HAL_CACHE_MON_DATA_T *md)
{
#if (CHIP_CACHE_VER >= 3)
    uint32_t lock;

    lock = int_lock();
    md->r_hit   = cacheip_read64(reg_base, MON_CNT0_READ_HIT_L_REG_OFFSET,   MON_CNT0_READ_HIT_H_REG_OFFSET);
    md->r_miss  = cacheip_read64(reg_base, MON_CNT0_READ_MISS_L_REG_OFFSET,  MON_CNT0_READ_MISS_H_REG_OFFSET);
    md->w_hit   = cacheip_read64(reg_base, MON_CNT0_WRITE_HIT_L_REG_OFFSET,  MON_CNT0_WRITE_HIT_H_REG_OFFSET);
    md->w_miss  = cacheip_read64(reg_base, MON_CNT0_WRITE_MISS_L_REG_OFFSET, MON_CNT0_WRITE_MISS_H_REG_OFFSET);
#if (CHIP_CACHE_VER >= 4)
    md->r_hit  += cacheip_read64(reg_base, MON_CNT1_READ_HIT_L_REG_OFFSET,   MON_CNT1_READ_HIT_H_REG_OFFSET);
    md->r_miss += cacheip_read64(reg_base, MON_CNT1_READ_MISS_L_REG_OFFSET,  MON_CNT1_READ_MISS_H_REG_OFFSET);
    md->w_hit  += cacheip_read64(reg_base, MON_CNT1_WRITE_HIT_L_REG_OFFSET,  MON_CNT1_WRITE_HIT_H_REG_OFFSET);
    md->w_miss += cacheip_read64(reg_base, MON_CNT1_WRITE_MISS_L_REG_OFFSET, MON_CNT1_WRITE_MISS_H_REG_OFFSET);
#endif
    int_unlock(lock);
#endif
}
/* cache controller end */

/* hal api */
__STATIC_FORCEINLINE uint32_t _cache_get_reg_base(enum HAL_CACHE_ID_T id)
{
    uint32_t base;

    if (id == HAL_CACHE_ID_I_CACHE) {
        base = ICACHE_CTRL_BASE;
    } else if (id == HAL_CACHE_ID_D_CACHE) {
#ifdef DCACHE_CTRL_BASE
        base = DCACHE_CTRL_BASE;
#else
        base = 0;
#endif
    } else {
        base = 0;
    }

    return base;
}
uint8_t BOOT_TEXT_FLASH_LOC hal_cache_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

#if (CHIP_CACHE_VER >= 3)
    if (cacheip_cached_enabled(reg_base)) {
        cacheip_trigger_invalidate_all(reg_base);
    } else
#endif
    {
        cacheip_init_cache(reg_base);
        cacheip_enable_cache(reg_base, HAL_CACHE_YES);
    }

    return 0;
}
uint8_t SRAM_TEXT_LOC hal_cache_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }
#if (CHIP_CACHE_VER >= 3)
    if (cacheip_cached_enabled(reg_base) == 0)
        return 0;
#endif

#if !(defined(ROM_BUILD) || defined(PROGRAMMER))
#if (CHIP_CACHE_VER <= 1)
    uint32_t time;

    time = hal_sys_timer_get();
    while (hal_norflash_busy() && (hal_sys_timer_get() - time) < MS_TO_TICKS(2));
    // Delay for at least 8 cycles till the cache becomes idle
    for (int delay = 0; delay < 8; delay++) {
        asm volatile("nop");
    }
#else // CHIP_CACHE_VER >= 2
    uint32_t val;

    do {
        val = cacheip_read32(reg_base, STATUS_REG_OFFSET);
    } while (val & STATUS_FETCHING);
#endif // CHIP_CACHE_VER >= 2
#endif

    cacheip_trigger_invalidate_all(reg_base);
    cacheip_enable_cache(reg_base, HAL_CACHE_NO);

    return 0;
}
uint8_t BOOT_TEXT_FLASH_LOC hal_cache_writebuffer_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writebuffer(reg_base, HAL_CACHE_YES);

    return 0;
}
uint8_t hal_cache_writebuffer_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writebuffer(reg_base, HAL_CACHE_NO);

    return 0;
}
uint8_t hal_cache_writebuffer_flush(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_flush_writebuffer(reg_base);

    return 0;
}
uint8_t BOOT_TEXT_FLASH_LOC hal_cache_writeback_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writeback(reg_base, HAL_CACHE_YES);

    return 0;
}
uint8_t hal_cache_writeback_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writeback(reg_base, HAL_CACHE_NO);

    return 0;
}
// Wrap is enabled during flash init
uint8_t BOOT_TEXT_SRAM_LOC hal_cache_wrap_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_wrap(reg_base, HAL_CACHE_YES);

    return 0;
}
uint8_t BOOT_TEXT_SRAM_LOC hal_cache_wrap_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_wrap(reg_base, HAL_CACHE_NO);

    return 0;
}
// Flash timing calibration might need to invalidate cache
uint8_t BOOT_TEXT_SRAM_LOC hal_cache_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len)
{
    uint32_t reg_base;
    uint32_t end_address;
    uint32_t lock;

#ifndef DCACHE_CTRL_BASE
    if (id == HAL_CACHE_ID_D_CACHE) {
        id = HAL_CACHE_ID_I_CACHE;
    }
#endif

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    lock = int_lock_global();

#if defined(CHIP_BEST2300) || defined(CHIP_BEST1400)
    uint32_t time;

    time = hal_sys_timer_get();
    while (hal_norflash_busy() && (hal_sys_timer_get() - time) < MS_TO_TICKS(2));
    // Delay for at least 8 cycles till the cache becomes idle
    for (int delay = 0; delay < 8; delay++) {
        asm volatile("nop");
    }
#endif

    if (len >= CACHE_SIZE / 2) {
        cacheip_trigger_invalidate_all(reg_base);
    } else {
        end_address = start_address + len;
        start_address &= (~(CACHE_LINE_SIZE-1));
        while (start_address < end_address) {
            cacheip_set_invalidate_address(reg_base, start_address);
            cacheip_trigger_invalidate(reg_base);
#ifdef CHIP_BEST2001
            // For writeback data cache only (psram data)
            cacheip_trigger_invalidate(reg_base);
#endif
            start_address += CACHE_LINE_SIZE;
        }
    }

    int_unlock_global(lock);

    return 0;
}
uint8_t hal_cache_invalidate_all(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;
    uint32_t lock;

#ifndef DCACHE_CTRL_BASE
    if (id == HAL_CACHE_ID_D_CACHE) {
        id = HAL_CACHE_ID_I_CACHE;
    }
#endif

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    lock = int_lock_global();

    cacheip_trigger_invalidate_all(reg_base);

    int_unlock_global(lock);

    return 0;
}
uint8_t hal_cache_sync_all(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;
    uint32_t lock;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    lock = int_lock_global();

    cacheip_trigger_sync_all(reg_base);

    int_unlock_global(lock);

    return 0;
}
#if (CHIP_CACHE_VER >= 3)
uint8_t BOOT_TEXT_SRAM_LOC hal_cache_sync(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len)
{
    uint32_t reg_base;
    uint32_t end_address;
    uint32_t lock;

#ifndef DCACHE_CTRL_BASE
    if (id == HAL_CACHE_ID_D_CACHE) {
        id = HAL_CACHE_ID_I_CACHE;
    }
#endif

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    lock = int_lock_global();

    if (len >= CACHE_SIZE / 2) {
        cacheip_trigger_sync_all(reg_base);
    } else {
        end_address = start_address + len;
        start_address &= (~(CACHE_LINE_SIZE-1));
        while (start_address < end_address) {
            cacheip_set_invalidate_address(reg_base, start_address);
            cacheip_trigger_sync(reg_base);
            start_address += CACHE_LINE_SIZE;
        }
    }

    int_unlock_global(lock);

    return 0;
}
#endif
uint8_t hal_cache_monitor_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_monitor(reg_base, 1);
    return 0;
}
uint8_t hal_cache_monitor_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_monitor(reg_base, 0);
    return 0;
}
uint8_t hal_cache_get_monitor_data(enum HAL_CACHE_ID_T id, struct HAL_CACHE_MON_DATA_T *md)
{
    uint32_t reg_base = 0;

    if (!md) {
        return 1;
    }
    md->r_hit = 0;
    md->r_miss = 0;
    md->w_hit = 0;
    md->w_miss = 0;

    reg_base = _cache_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_get_monitor_data(reg_base, md);
    return 0;
}

void hal_cache_print_stats(void)
{
    struct HAL_CACHE_MON_DATA_T data;
    uint64_t total;
    uint32_t r_ppm;
    uint32_t w_ppm;

    TRACE(0, "CACHE STATS:");
    for (int i = HAL_CACHE_ID_I_CACHE; i < HAL_CACHE_ID_NUM; i++) {
        hal_cache_get_monitor_data(i, &data);
        hal_cache_monitor_disable(i);
        hal_cache_monitor_enable(i);
        total = data.r_hit + data.r_miss;
        if (total) {
            r_ppm = data.r_miss * 1000000 / total;
        } else {
            r_ppm = 0;
        }
        total = data.w_hit + data.w_miss;
        if (total) {
            w_ppm = data.r_miss * 1000000 / total;
        } else {
            w_ppm = 0;
        }
        TRACE(TR_ATTR_NO_TS, "\t[%2u] r_miss=%3u w_miss=%3u (ppm)", i, r_ppm, w_ppm);
    }
}

#ifdef CORE_SLEEP_POWER_DOWN
void hal_cache_sleep(void)
{
    icache_regs.REG_CACHE_EN = cacheip_read32(ICACHE_CTRL_BASE, CACHE_ENABLE_REG_OFFSET);
    icache_regs.REG_WRITEBUF_EN = cacheip_read32(ICACHE_CTRL_BASE, WRITEBUFFER_ENABLE_REG_OFFSET);
    icache_regs.REG_MONITOR_EN = cacheip_read32(ICACHE_CTRL_BASE, MON_ENABLE_REG_OFFSET);
#ifdef DCACHE_CTRL_BASE
    dcache_regs.REG_CACHE_EN = cacheip_read32(DCACHE_CTRL_BASE, CACHE_ENABLE_REG_OFFSET);
    dcache_regs.REG_WRITEBUF_EN = cacheip_read32(DCACHE_CTRL_BASE, WRITEBUFFER_ENABLE_REG_OFFSET);
    dcache_regs.REG_MONITOR_EN = cacheip_read32(DCACHE_CTRL_BASE, MON_ENABLE_REG_OFFSET);
#endif
}
void SRAM_TEXT_LOC hal_cache_wakeup(void)
{
    if (icache_regs.REG_CACHE_EN & CACHE_EN) {
        cacheip_init_cache(ICACHE_CTRL_BASE);
    }
    cacheip_write32(icache_regs.REG_CACHE_EN, ICACHE_CTRL_BASE, CACHE_ENABLE_REG_OFFSET);
    cacheip_write32(icache_regs.REG_WRITEBUF_EN, ICACHE_CTRL_BASE, WRITEBUFFER_ENABLE_REG_OFFSET);
    cacheip_write32(icache_regs.REG_MONITOR_EN, ICACHE_CTRL_BASE, MON_ENABLE_REG_OFFSET);
#ifdef DCACHE_CTRL_BASE
    if (dcache_regs.REG_CACHE_EN & CACHE_EN) {
        cacheip_init_cache(DCACHE_CTRL_BASE);
    }
    cacheip_write32(dcache_regs.REG_CACHE_EN, DCACHE_CTRL_BASE, CACHE_ENABLE_REG_OFFSET);
    cacheip_write32(dcache_regs.REG_WRITEBUF_EN, DCACHE_CTRL_BASE, WRITEBUFFER_ENABLE_REG_OFFSET);
    cacheip_write32(dcache_regs.REG_MONITOR_EN, DCACHE_CTRL_BASE, MON_ENABLE_REG_OFFSET);
#endif
}
#endif

#if defined(CHIP_HAS_CP) && defined(ICACHECP_CTRL_BASE)
__STATIC_FORCEINLINE uint32_t _cachecp_get_reg_base(enum HAL_CACHE_ID_T id)
{
    uint32_t base;

#if (ICACHECP_CTRL_BASE == ICACHE_CTRL_BASE)
    if (get_cpu_id() == 0) {
        ASSERT(0, "MCU can't access to CP's cache, because their cache base is equal.");
        return 0;
    }
#endif

    if (id == HAL_CACHE_ID_I_CACHE) {
        base = ICACHECP_CTRL_BASE;
    } else if (id == HAL_CACHE_ID_D_CACHE) {
#ifdef DCACHECP_CTRL_BASE
        base = DCACHECP_CTRL_BASE;
#else
        base = 0;
#endif
    } else {
        base = 0;
    }

    return base;
}
uint8_t hal_cachecp_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;
    uint32_t main_cache_reg_base;
    enum HAL_CMU_MOD_ID_T mod;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    if (id == HAL_CACHE_ID_D_CACHE) {
        mod = HAL_CMU_H_DCACHECP;
    } else {
        mod = HAL_CMU_H_ICACHECP;
    }
    hal_cmu_clock_enable(mod);
    hal_cmu_reset_clear(mod);

    cacheip_init_cache(reg_base);
    cacheip_enable_cache(reg_base, HAL_CACHE_YES);
    // Init wrap option
    main_cache_reg_base = _cache_get_reg_base(id);
    if (main_cache_reg_base == 0) {
        return 0;
    }
    cacheip_enable_wrap(reg_base, cacheip_wrap_enabled(main_cache_reg_base));

    return 0;
}
uint8_t CP_TEXT_SRAM_LOC hal_cachecp_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;
    enum HAL_CMU_MOD_ID_T mod;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

#if !(defined(ROM_BUILD) || defined(PROGRAMMER))
    uint32_t val;

    do {
        val = cacheip_read32(reg_base, STATUS_REG_OFFSET);
    } while (val & STATUS_FETCHING);
#endif

    cacheip_enable_cache(reg_base, HAL_CACHE_NO);

    if (id == HAL_CACHE_ID_D_CACHE) {
        mod = HAL_CMU_H_DCACHECP;
    } else {
        mod = HAL_CMU_H_ICACHECP;
    }
    hal_cmu_reset_set(mod);
    hal_cmu_clock_disable(mod);

    return 0;
}
uint8_t hal_cachecp_writebuffer_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writebuffer(reg_base, HAL_CACHE_YES);

    return 0;
}
uint8_t hal_cachecp_writebuffer_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writebuffer(reg_base, HAL_CACHE_NO);

    return 0;
}
uint8_t hal_cachecp_writebuffer_flush(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_flush_writebuffer(reg_base);

    return 0;
}
uint8_t hal_cachecp_writeback_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writeback(reg_base, HAL_CACHE_YES);

    return 0;
}
uint8_t hal_cachecp_writeback_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writeback(reg_base, HAL_CACHE_NO);

    return 0;
}
uint8_t CP_TEXT_SRAM_LOC hal_cachecp_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len)
{
    uint32_t reg_base;
    uint32_t end_address;

#ifndef DCACHECP_CTRL_BASE
    if (id == HAL_CACHE_ID_D_CACHE) {
        id = HAL_CACHE_ID_I_CACHE;
    }
#endif

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    if (len >= CACHE_SIZE / 2) {
        cacheip_trigger_invalidate_all(reg_base);
    } else {
        end_address = start_address + len;
        start_address &= (~(CACHE_LINE_SIZE-1));
        while (start_address < end_address) {
            cacheip_set_invalidate_address(reg_base, start_address);
            cacheip_trigger_invalidate(reg_base);
#ifdef CHIP_BEST2001
            // For writeback data cache only (psram data)
            cacheip_trigger_invalidate(reg_base);
#endif
            start_address += CACHE_LINE_SIZE;
        }
    }

    return 0;
}
uint8_t hal_cachecp_sync_all(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_trigger_sync_all(reg_base);

    return 0;
}
uint8_t hal_cachecp_monitor_enable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_monitor(reg_base, 1);
    return 0;
}
uint8_t hal_cachecp_monitor_disable(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_monitor(reg_base, 0);
    return 0;
}
uint8_t hal_cachecp_get_monitor_data(enum HAL_CACHE_ID_T id, struct HAL_CACHE_MON_DATA_T *md)
{
    uint32_t reg_base = 0;

    if (!md) {
        return 1;
    }
    md->r_hit = 0;
    md->r_miss = 0;
    md->w_hit = 0;
    md->w_miss = 0;

    reg_base = _cachecp_get_reg_base(id);
    if (reg_base == 0) {
        return 0;
    }

    cacheip_get_monitor_data(reg_base, md);
    return 0;
}

void hal_cachecp_print_stats(void)
{
    struct HAL_CACHE_MON_DATA_T data;
    uint64_t total;
    uint32_t r_ppm;
    uint32_t w_ppm;

    TRACE(0, "CACHE-CP STATS:");
    for (int i = HAL_CACHE_ID_I_CACHE; i < HAL_CACHE_ID_NUM; i++) {
        hal_cachecp_get_monitor_data(i, &data);
        hal_cachecp_monitor_disable(i);
        hal_cachecp_monitor_enable(i);
        total = data.r_hit + data.r_miss;
        if (total) {
            r_ppm = data.r_miss * 1000000 / total;
        } else {
            r_ppm = 0;
        }
        total = data.w_hit + data.w_miss;
        if (total) {
            w_ppm = data.r_miss * 1000000 / total;
        } else {
            w_ppm = 0;
        }
        TRACE(TR_ATTR_NO_TS, "\t[%2u] r_miss=%3u w_miss=%3u (ppm)", i, r_ppm, w_ppm);
    }
}
#endif

#endif
