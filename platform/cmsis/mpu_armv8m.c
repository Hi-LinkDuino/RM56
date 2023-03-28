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
#ifndef __ARM_ARCH_ISA_ARM

#include "cmsis.h"
#include "mpu.h"
#include "hal_location.h"
#include "hal_trace.h"

// Non-Transient, Write-Back, Read-Allocation, Write-Allocation
#define NORM_MEM_WT_RA_ATTR             ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0)
#define NORM_MEM_WB_WA_ATTR             ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1)
#define NORM_MEM_NC_ATTR                ARM_MPU_ATTR_NON_CACHEABLE
#define DEV_MEM_ATTR_INNER              ARM_MPU_ATTR_DEVICE_nGnRnE
#define DEV_MEM_ATTR_OUTER              ARM_MPU_ATTR_DEVICE

static void init_mair_attr(void)
{
    MPU->MAIR0 = 0;
    MPU->MAIR1 = 0;

    ARM_MPU_SetMemAttr(MAIR_ATTR_FLASH,     ARM_MPU_ATTR(NORM_MEM_WT_RA_ATTR,   NORM_MEM_WT_RA_ATTR));
    ARM_MPU_SetMemAttr(MAIR_ATTR_INT_SRAM,  ARM_MPU_ATTR(NORM_MEM_WB_WA_ATTR,   NORM_MEM_WB_WA_ATTR));
    ARM_MPU_SetMemAttr(MAIR_ATTR_EXT_RAM,   ARM_MPU_ATTR(NORM_MEM_WB_WA_ATTR,   NORM_MEM_WB_WA_ATTR));
    ARM_MPU_SetMemAttr(MAIR_ATTR_DEVICE,    ARM_MPU_ATTR(DEV_MEM_ATTR_OUTER,    DEV_MEM_ATTR_INNER));
    ARM_MPU_SetMemAttr(MAIR_ATTR_NC_MEM,    ARM_MPU_ATTR(NORM_MEM_NC_ATTR,      NORM_MEM_NC_ATTR));
}

static int mpu_enable(void)
{
    int flags = 0;

    if (get_cpu_id()) {
        /*
         * if cpu is CP, we will use different mpu maps, that is, every map needed by CP will
         * be mapped, and the memory is not mapped will issue abort when access
         */
        flags = MPU_CTRL_HFNMIENA_Msk;
    } else {
        /*
         * mpu maps use default map designed by arm, that is, if the memory is not mapped,
         * the mpu attibutes will use the default setting by ARM.
         */
        flags = MPU_CTRL_PRIVDEFENA_Msk;
    }
    ARM_MPU_Enable(flags);
    return 0;
}

static int mpu_disable(void)
{
    ARM_MPU_Disable();
    return 0;
}

int mpu_open(void)
{
    int i;

    if ((MPU->TYPE & MPU_TYPE_DREGION_Msk) == 0) {
        return -1;
    }

    for (i = 0; i < MPU_ID_QTY; i++) {
        ARM_MPU_ClrRegion(i);
    }

    init_mair_attr();

    return 0;
}

int mpu_close(void)
{
    ARM_MPU_Disable();

    return 0;
}

static int mpu_set_armv8(uint8_t id, uint32_t addr, uint32_t len,
                        enum MPU_ATTR_T ap_attr, enum MAIR_ATTR_TYPE_T mem_attr)
{
    uint32_t rbar;
    uint32_t rlar;
    uint8_t xn;
    uint8_t ro;
    uint32_t lock;

    if (id >= MPU_ID_QTY) {
        return -1;
    }
    if (len < 32) {
        return -2;
    }
    if (addr & 0x1F) {
        return -3;
    }
    if (ap_attr >= MPU_ATTR_QTY) {
        return -4;
    }

    if (ap_attr == MPU_ATTR_READ_WRITE_EXEC || ap_attr == MPU_ATTR_READ_EXEC ||
            ap_attr == MPU_ATTR_EXEC) {
        xn = 0;
    } else {
        xn = 1;
    }

    if (ap_attr == MPU_ATTR_READ_WRITE_EXEC || ap_attr == MPU_ATTR_READ_WRITE) {
        ro = 0;
    } else if (ap_attr == MPU_ATTR_READ_EXEC || ap_attr == MPU_ATTR_READ || ap_attr == MPU_ATTR_EXEC) {
        ro = 1;
    } else {
        // Cannot support no access
        return -5;
    }

    // Sharebility: Non-shareable
    // Non-privilege Access: Enabled
    rbar = ARM_MPU_RBAR(addr, 0, ro, 1, xn);
    /* rlar = ARM_MPU_RLAR((addr + len - 1), MAIR_ATTR_INT_SRAM); */
    rlar = ARM_MPU_RLAR((addr + len - 1), mem_attr);

    lock = int_lock();
    ARM_MPU_SetRegion(id, rbar, rlar);
    int_unlock(lock);

    return 0;
}

static inline int mpu_fram_protect_armv8(uint32_t fr_start, uint32_t fr_end)
{
    uint32_t len = fr_end - fr_start;
    int ret;
    uint8_t id;

    TRACE(0, "fram start %x size %x", fr_start, len);
    if (!len)
        return 0;

    if ((fr_start % 32 != 0) || (len % 32 != 0))
        ASSERT(0, "fr_start %x and len %d must be aligned to 32", fr_start, len);

    id = mpu_alloc_region();
    if (id == MPU_INVALID_ID)
        return -1;

    mpu_disable();
    ret = mpu_set_armv8(id, fr_start, len, MPU_ATTR_EXEC,  MAIR_ATTR_INT_SRAM);
    mpu_enable();

    return ret;
}

extern uint32_t __sram_text_start__[];
extern uint32_t  __sram_text_end__[];

static int mpu_sram_text_protect(void)
{
    int ret;
    uint32_t start  = RAMX_TO_RAM((uint32_t)__sram_text_start__);
    uint32_t end = RAMX_TO_RAM((uint32_t)__sram_text_end__);
    uint32_t len = end - start;
    uint8_t id;

    TRACE(0, "sram_text start %x size %x", start, len);
    if (!len)
        return 0;

    if ((start % 32 != 0) || (len % 32 != 0))
        ASSERT(0, "sram start %x and len %d must be aligned to 32", start, len);

    id = mpu_alloc_region();
    if (id == MPU_INVALID_ID)
        return -1;

    ret = mpu_set(id, start, len, MPU_ATTR_READ_EXEC);
    return ret;
}


int mpu_set(uint8_t id, uint32_t addr, uint32_t len, enum MPU_ATTR_T attr)
{
    int ret = -1;
    uint32_t lock;

    lock = int_lock();

    mpu_disable();
    ret = mpu_set_armv8(id, addr, len, attr, MAIR_ATTR_INT_SRAM);
    mpu_enable();

    int_unlock(lock);

    return ret;
}

int mpu_clear(uint8_t id)
{
    uint32_t lock;

    if (id >= MPU_ID_QTY) {
        return -2;
    }

    lock = int_lock();

    mpu_disable();
    ARM_MPU_ClrRegion(id);
    __DSB();
    __ISB();
    mpu_enable();

    int_unlock(lock);

    return 0;
}

extern uint32_t __fast_sram_text_exec_start__[];
extern uint32_t __fast_sram_text_exec_end__[];

static int mpu_fast_ram_protect(void)
{
    uint32_t ramx_start  = (uint32_t)__fast_sram_text_exec_start__;
    uint32_t ramx_end = (uint32_t)__fast_sram_text_exec_end__;
    uint32_t ram_start = RAMX_TO_RAM(ramx_start);
    uint32_t ram_end = RAMX_TO_RAM(ramx_end);
    int ret = -1;

    ret = mpu_fram_protect_armv8(ram_start, ram_end);
    return ret;
}

static uint32_t mpu_region_map;
STATIC_ASSERT(sizeof(mpu_region_map) * 8 >= MPU_ID_QTY, "mpu_regsion_map size too small");

static int mpu_init_table(const mpu_regions_t *mpu_table, uint32_t region_num, int cp)
{
    int ret;
    int i;
    uint32_t lock;
    const mpu_regions_t *region;

    if (region_num > MPU_ID_QTY) {
        return -1;
    }

    lock = int_lock();

    mpu_disable();

    for (i = 0; i < region_num; i++) {
        region = &mpu_table[i];
        ret = mpu_set_armv8(i, region->addr, region->len, region->ap_attr, region->mem_attr);
        if (ret)
            goto out;

        if (!cp)
            mpu_region_map |= 1 << i;
    }

    mpu_enable();

out:
    int_unlock(lock);

    return ret;
}

int mpu_setup(const mpu_regions_t *mpu_table, uint32_t region_num)
{
    int ret;

    ret = mpu_open();
    if (ret)
        return ret;

    ret = mpu_init_table(mpu_table, region_num, 0);
    if (ret)
        return ret;

    ret = mpu_sram_text_protect();
    if (ret)
        return ret;

    ret = mpu_fast_ram_protect();
    return ret;
}

int mpu_setup_cp(const mpu_regions_t *mpu_table, uint32_t region_num)
{
    int ret;

    ret = mpu_open();
    if (ret) {
        return ret;
    }

    ret = mpu_init_table(mpu_table, region_num, 1);
    return ret;
}

uint8_t mpu_alloc_region(void)
{
    uint8_t i;
    uint32_t lock;

    lock = int_lock();
    for (i = 0; i < MPU_ID_QTY; i++)
        if ((mpu_region_map & (1 << i)) == 0)
            break;

    if (i >= MPU_ID_QTY) {
        int_unlock(lock);
        return MPU_INVALID_ID;
    }

    mpu_region_map |= 1 << i;
    int_unlock(lock);
    return i;
}

void mpu_free_region(uint8_t id)
{
    uint32_t lock;

    if (id >= MPU_ID_QTY)
        return;

    lock = int_lock();
    mpu_region_map &= ~(1 << id);
    int_unlock(lock);
}

#ifdef CORE_SLEEP_POWER_DOWN
struct MPU_SAVED_REGS_T {
    uint32_t ctrl;
    uint32_t mair[2];
    uint32_t rbar[MPU_ID_QTY];
    uint32_t rlar[MPU_ID_QTY];
};

static struct MPU_SAVED_REGS_T mpu_saved_regs;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
static struct MPU_SAVED_REGS_T mpu_ns_regs;
#endif

void SRAM_TEXT_LOC mpu_sleep(void)
{
    int i;

    mpu_saved_regs.ctrl = MPU->CTRL;
    mpu_saved_regs.mair[0] = MPU->MAIR[0];
    mpu_saved_regs.mair[1] = MPU->MAIR[1];
    for (i = 0; i < MPU_ID_QTY; i++) {
        MPU->RNR = i;
        mpu_saved_regs.rbar[i] = MPU->RBAR;
        mpu_saved_regs.rlar[i] = MPU->RLAR;
    }
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    mpu_ns_regs.ctrl = MPU_NS->CTRL;
    mpu_ns_regs.mair[0] = MPU_NS->MAIR[0];
    mpu_ns_regs.mair[1] = MPU_NS->MAIR[1];
    for (int i = 0; i < MPU_ID_QTY; i++) {
        MPU_NS->RNR = i;
        mpu_ns_regs.rbar[i] = MPU_NS->RBAR;
        mpu_ns_regs.rlar[i] = MPU_NS->RLAR;
    }
#endif
}

void SRAM_TEXT_LOC mpu_wakeup(void)
{
    int i;

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    for (i = 0; i < MPU_ID_QTY; i++) {
        MPU_NS->RNR = i;
        MPU_NS->RBAR = mpu_ns_regs.rbar[i];
        MPU_NS->RLAR = mpu_ns_regs.rlar[i];
    }
    MPU_NS->MAIR[0] = mpu_ns_regs.mair[0];
    MPU_NS->MAIR[1] = mpu_ns_regs.mair[1];
    MPU_NS->CTRL = mpu_ns_regs.ctrl;
#endif
    for (i = 0; i < MPU_ID_QTY; i++) {
        MPU->RNR = i;
        MPU->RBAR = mpu_saved_regs.rbar[i];
        MPU->RLAR = mpu_saved_regs.rlar[i];
    }
    MPU->MAIR[0] = mpu_saved_regs.mair[0];
    MPU->MAIR[1] = mpu_saved_regs.mair[1];
    MPU->CTRL = mpu_saved_regs.ctrl;
    __DSB();
    __ISB();
}
#endif

#endif
