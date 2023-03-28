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
#include "cmsis.h"
#include "pmu.h"
#include "hal_cache.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_psram.h"
#include "hal_psramuhs.h"
#include "hal_psc.h"
#include "hal_bootmode.h"

#if !defined(NOSTD) && defined(ACCURATE_DB_TO_FLOAT)
#include <math.h>
#endif
#ifdef __ARMCC_VERSION
#include "link_sym_armclang.h"
#endif

extern uint32_t __got_info_start[];

struct irq_masked_address irq_masked_addr;

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
// CAUTION:
// O2 option in GCC 10 will optimize the copy loop to an invokation of memcpy,
// whereas memcpy might be located in RAM and not initialized yet.
#define BOOT_CODE_OPT_ATTR                  __attribute__((optimize("no-tree-loop-distribute-patterns")))
#else
#define BOOT_CODE_OPT_ATTR
#endif

void BOOT_TEXT_FLASH_LOC GotBaseInit(void)
{
#ifndef __ARMCC_VERSION
    asm volatile("ldr r9, =__got_info_start");
#endif
}

#ifndef __ARM_ARCH_ISA_ARM
// -----------------------------------------------------------
// Boot initialization
// CAUTION: This function and all the functions it called must
//          NOT access global data/bss, for the global data/bss
//          is not available at that time.
// -----------------------------------------------------------
extern uint32_t __boot_sram_start_flash__[];
extern uint32_t __boot_text_sram_end_flash__[];
extern uint32_t __boot_data_sram_start_flash__[];
extern uint32_t __boot_sram_end_flash__[];
extern uint32_t __boot_sram_start__[];
extern uint32_t __boot_text_sram_start__[];
extern uint32_t __boot_data_sram_start__[];
extern uint32_t __boot_bss_sram_start__[];
extern uint32_t __boot_bss_sram_end__[];

extern uint32_t __sram_text_data_start_flash__[];
extern uint32_t __sram_text_end_flash__[];
extern uint32_t __sram_data_start_flash__[];
extern uint32_t __sram_text_data_end_flash__[];
extern uint32_t __sram_text_data_start__[];
extern uint32_t __sram_text_start__[];
extern uint32_t __sram_data_start__[];
extern uint32_t __sram_bss_start__[];
extern uint32_t __sram_bss_end__[];
extern uint32_t __fast_sram_text_data_start__[];
extern uint32_t __fast_sram_text_data_end__[];
extern uint32_t __fast_sram_text_data_start_flash__[];
extern uint32_t __fast_sram_text_data_end_flash__[];
#ifdef ARM_CMSE
extern uint32_t __sram_nsc_text_start__[];
extern uint32_t __sram_nsc_text_start_flash__[];
extern uint32_t __sram_nsc_text_end_flash__[];
#endif

extern uint32_t __psram_text_data_start__[];
extern uint32_t __psram_text_data_start_flash__[];
extern uint32_t __psram_text_data_end_flash__[];
extern uint32_t __psram_bss_start__[];
extern uint32_t __psram_bss_end__[];
extern uint32_t __psramuhs_text_data_start__[];
extern uint32_t __psramuhs_text_data_start_flash__[];
extern uint32_t __psramuhs_text_data_end_flash__[];
extern uint32_t __psramuhs_bss_start__[];
extern uint32_t __psramuhs_bss_end__[];
extern uint32_t __got_start_flash__[];
extern uint32_t __got_end_flash__[];
extern uint32_t __got_info_start[];
extern uint32_t __got_info_end[];

void BOOT_TEXT_FLASH_LOC BOOT_CODE_OPT_ATTR boot_init_boot_sections(void)
{
    uint32_t *dst, *src;

#ifdef CHIP_HAS_TCM
    for (dst = __boot_text_sram_start__, src = __boot_sram_start_flash__;
            src < __boot_text_sram_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
    for (dst = __boot_data_sram_start__, src = __boot_data_sram_start_flash__;
            src < __boot_sram_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
#else
    for (dst = __boot_sram_start__, src = __boot_sram_start_flash__;
            src < __boot_sram_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
#endif

    for (dst = __boot_bss_sram_start__; dst < __boot_bss_sram_end__; dst++) {
        *dst = 0;
    }
}

void BOOT_TEXT_FLASH_LOC BOOT_CODE_OPT_ATTR boot_init_sram_sections(void)
{
    uint32_t *dst, *src;

#ifdef CHIP_HAS_TCM
    for (dst = __sram_text_start__, src = __sram_text_data_start_flash__;
            src < __sram_text_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
    for (dst = __sram_data_start__, src = __sram_data_start_flash__;
            src < __sram_text_data_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
#else
    for (dst = __sram_text_data_start__, src = __sram_text_data_start_flash__;
            src < __sram_text_data_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
#endif

    for (dst = __sram_bss_start__; dst < __sram_bss_end__; dst++) {
        *dst = 0;
    }

    for (dst = __fast_sram_text_data_start__, src = __fast_sram_text_data_start_flash__;
            src < __fast_sram_text_data_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
}

void BOOT_TEXT_FLASH_LOC BOOT_CODE_OPT_ATTR boot_init_psram_sections(void)
{
#if (PSRAM_SIZE > 0)
    uint32_t *dst, *src;
    for (dst = __psram_text_data_start__, src = __psram_text_data_start_flash__;
        src < __psram_text_data_end_flash__;
        dst++, src++) {
        *dst = *src;
    }

    for (dst = __psram_bss_start__; dst < __psram_bss_end__; dst++) {
        *dst = 0;
    }
#endif
}

void BOOT_TEXT_FLASH_LOC BOOT_CODE_OPT_ATTR boot_init_psramuhs_sections(void)
{
#if (MCU_PSRAMUHS_SIZE > 0)
    uint32_t *dst, *src;
    for (dst = __psramuhs_text_data_start__, src = __psramuhs_text_data_start_flash__;
        src < __psramuhs_text_data_end_flash__;
        dst++, src++) {
        *dst = *src;
    }

    for (dst = __psramuhs_bss_start__; dst < __psramuhs_bss_end__; dst++) {
        *dst = 0;
    }
#endif
}

#if defined(ARM_CMSE) && (RAM_NSC_SIZE != 0)
void BOOT_TEXT_FLASH_LOC BOOT_CODE_OPT_ATTR boot_init_security_sections(void)
{
    uint32_t *dst, *src;

    for (dst = __sram_nsc_text_start__, src = __sram_nsc_text_start_flash__;
            src < __sram_nsc_text_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
}
#endif

#if (defined(PSRAM_ENABLE) || defined(PSRAMUHS_ENABLE)) && defined(PSRAM_INIT_CHECK)
#include "hal_wdt.h"
#include "hal_bootmode.h"
static void dummy_irq_handler_event(enum HAL_WDT_ID_T id, enum HAL_WDT_EVENT_T event) {}
static void dummy_irq_handler(void) {}
void set_psram_wdt()
{
    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI) {
        hal_wdt_set_irq_callback(HAL_WDT_ID_0, dummy_irq_handler_event);
        hal_wdt_set_timeout(HAL_WDT_ID_0, 2);
        hal_wdt_start(HAL_WDT_ID_0);
    } else {
        pmu_wdt_config(1000, 1000);
        pmu_wdt_set_irq_handler(dummy_irq_handler);
        pmu_wdt_start();
    }
}
#endif

volatile uint32_t BOOT_DATA_LOC init_boot_mode;
void BOOT_TEXT_FLASH_LOC BOOT_CODE_OPT_ATTR BootInit(void)
{
    // Enable icache
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    // Enable dcache
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);
    // Enable write buffer
    hal_cache_writebuffer_enable(HAL_CACHE_ID_D_CACHE);
    // Enable write back
    hal_cache_writeback_enable(HAL_CACHE_ID_D_CACHE);

#if !defined(FPGA)
    // Init GOT base register
    GotBaseInit();
#endif

#if !defined( NANDFLASH_BUILD) && !defined(ROM_BUILD)
    boot_init_boot_sections();
#endif

    init_boot_mode = hal_sw_bootmode_get();

#ifdef FPGA
    hal_cmu_fpga_setup();
#else
    hal_cmu_setup();
#endif

#if !defined( NANDFLASH_BUILD) && !defined(ROM_BUILD)
    boot_init_sram_sections();
#endif

#if defined(ARM_CMSE) && (RAM_NSC_SIZE != 0)
    boot_init_security_sections();
#endif

#if (defined(PSRAM_ENABLE) || defined(PSRAMUHS_ENABLE)) && defined(PSRAM_INIT_CHECK)
    set_psram_wdt();
#endif

#if defined(CHIP_HAS_PSRAM) && defined(PSRAM_ENABLE)
    // Init psram
    pmu_open();
    hal_psram_init();
    boot_init_psram_sections();
#endif

#if defined(CHIP_HAS_PSRAMUHS) && defined(PSRAMUHS_ENABLE)
    // Init psramuhs
    pmu_open();
    hal_cmu_dsp_clock_enable();
    hal_cmu_dsp_reset_clear();
    hal_psramuhs_init();
    boot_init_psramuhs_sections();
#endif

#if defined(RAMV_BASE) && (RAMV_SIZE > 0)
    hal_psc_codec_enable();
    hal_cmu_codec_clock_enable();
    hal_cmu_codec_reset_clear();
#endif
}
#endif

#ifdef ROM_IN_FLASH
void BOOT_TEXT_FLASH_LOC boot_init_rom_in_flash(void)
{
#ifdef __ARMCC_VERSION
    extern uint32_t __code_start_addr[];
    asm volatile("" : : "r"(__code_start_addr));
#endif

#ifndef INTSRAM_RUN
    // Enable icache
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    // Enable dcache
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);
#endif

    // Init ram code and data
    boot_init_boot_sections();
    boot_init_sram_sections();
}
#endif

// -----------------------------------------------------------
// Mutex flag
// -----------------------------------------------------------

int BOOT_TEXT_SRAM_LOC set_bool_flag(bool *flag)
{
    bool busy;

    do {
        busy = (bool)__LDREXB((unsigned char *)flag);
        if (busy) {
            __CLREX();
            return -1;
        }
    } while (__STREXB(true, (unsigned char *)flag));
    __DMB();

    return 0;
}

void BOOT_TEXT_SRAM_LOC clear_bool_flag(bool *flag)
{
    *flag = false;
    __DMB();
}

// -----------------------------------------------------------
// Misc
// -----------------------------------------------------------

float db_to_float(float db)
{
    float coef;

#if !defined(NOSTD) && defined(ACCURATE_DB_TO_FLOAT)
    // The math lib will consume 4K+ bytes of space
    coef = powf(10, db / 20);
#else
    static const float factor_m9db      = 0.354813389234;
    static const float factor_m3db      = 0.707945784384;
    static const float factor_m1db      = 0.891250938134;
    static const float factor_m0p5db    = 0.944060876286;
    static const float factor_m0p1db    = 0.988553094657;

    coef = 1.0;

    if (db < 0) {
        while (1) {
            if (db <= -9.0) {
                db += 9.0;
                coef *= factor_m9db;
            } else if (db <= -3.0) {
                db += 3.0;
                coef *= factor_m3db;
            } else if (db <= -1.0) {
                db += 1.0;
                coef *= factor_m1db;
            } else if (db <= -0.5) {
                db += 0.5;
                coef *= factor_m0p5db;
            } else if (db <= -0.1 / 2) {
                db += 0.1;
                coef *= factor_m0p1db;
            } else {
                break;
            }
        }
    } else if (db > 0) {
        while (1) {
            if (db >= 9.0) {
                db -= 9.0;
                coef /= factor_m9db;
            } else if (db >= 3.0) {
                db -= 3.0;
                coef /= factor_m3db;
            } else if (db >= 1.0) {
                db -= 1.0;
                coef /= factor_m1db;
            } else if (db >= 0.5) {
                db -= 0.5;
                coef /= factor_m0p5db;
            } else if (db >= 0.1 / 2) {
                db -= 0.1;
                coef /= factor_m0p1db;
            } else {
                break;
            }
        }
    }
#endif

    return coef;
}

uint32_t get_msb_pos(uint32_t val)
{
    uint32_t lead_zero;

    lead_zero = __CLZ(val);
    return (lead_zero >= 32) ? 32 : 31 - lead_zero;
}

uint32_t get_lsb_pos(uint32_t val)
{
    return __CLZ(__RBIT(val));
}

uint32_t integer_sqrt(uint32_t val)
{
    unsigned int msb;
    unsigned int x;
    unsigned int y;

    if (val == 0) {
        return 0;
    }

    for (msb = 31; msb > 0; msb--) {
        if (val & (1 << msb)) {
            break;
        }
    }

    x = ((1 << msb) + 1) / 2;

    while (1) {
        y = (x + val / x) / 2;
        if (y >= x) {
            break;
        }
        x = y;
    }

    return x;
}

uint32_t integer_sqrt_nearest(uint32_t val)
{
    unsigned int s;

    s = integer_sqrt(val * 4);
    return (s + 1) / 2;
}

