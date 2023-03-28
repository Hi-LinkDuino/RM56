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
#include "hal_cmu.h"
#ifdef CHIP_SUBSYS_SENS
#include CHIP_SPECIFIC_HDR(reg_senscmu)
#else
#include CHIP_SPECIFIC_HDR(reg_cmu)
#endif
#ifdef AON_CMU_BASE
#include CHIP_SPECIFIC_HDR(reg_aoncmu)
#endif
#include "cmsis.h"
#include "hal_analogif.h"
#include "hal_bootmode.h"
#include "hal_cache.h"
#include "hal_chipid.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_norflash.h"
#include "hal_sleep.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"

#if defined(CHIP_HAS_USB) && (defined(MCU_HIGH_PERFORMANCE_MODE) && !(defined(ULTRA_LOW_POWER) || defined(OSC_26M_X4_AUD2BB)))
#define USB_PLL_INIT_ON
#endif
#if (!defined(ULTRA_LOW_POWER) && !defined(OSC_26M_X4_AUD2BB)) || \
        (!defined(FLASH_LOW_SPEED) && !defined(OSC_26M_X4_AUD2BB)) || \
        (defined(PSRAM_ENABLE) && !defined(PSRAM_LOW_SPEED))
#define AUD_PLL_INIT_ON
#endif

// SIMU_RES
#define CMU_SIMU_RES_PASSED                 (0x9A55)
#define CMU_SIMU_RES_FAILED                 (0xFA11)

typedef void (*HAL_POWER_DOWN_WAKEUP_HANDLER)(void);

#ifdef CHIP_SUBSYS_SENS
static struct SENSCMU_T * const cmu = (struct SENSCMU_T *)SENS_CMU_BASE;
#else
static struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;
#endif
#ifdef AON_CMU_BASE
static struct AONCMU_T * const POSSIBLY_UNUSED aoncmu = (struct AONCMU_T *)AON_CMU_BASE;
#endif

#ifdef HAL_CMU_VALID_CRYSTAL_FREQ
static const uint32_t valid_crystal_freq_list[] = HAL_CMU_VALID_CRYSTAL_FREQ;
#define CRYSTAL_FREQ_ATTR                   BOOT_DATA_LOC
#else
#define CRYSTAL_FREQ_ATTR                   const
#endif

static uint32_t CRYSTAL_FREQ_ATTR crystal_freq = HAL_CMU_DEFAULT_CRYSTAL_FREQ;

void BOOT_TEXT_FLASH_LOC hal_cmu_set_crystal_freq_index(uint32_t index)
{
#ifdef HAL_CMU_VALID_CRYSTAL_FREQ
    if (index >= ARRAY_SIZE(valid_crystal_freq_list)) {
        index %= ARRAY_SIZE(valid_crystal_freq_list);
    }
    crystal_freq = valid_crystal_freq_list[index];
#endif
}

uint32_t BOOT_TEXT_SRAM_LOC hal_cmu_get_crystal_freq(void)
{
    return crystal_freq;
}

uint32_t BOOT_TEXT_FLASH_LOC hal_cmu_get_default_crystal_freq(void)
{
    return HAL_CMU_DEFAULT_CRYSTAL_FREQ;
}

#ifndef CMU_FAST_TIMER_FREQ
uint32_t BOOT_TEXT_SRAM_LOC hal_cmu_get_fast_timer_freq(void)
{
    return crystal_freq / 4;
}
#endif

void hal_cmu_write_lock(void)
{
    cmu->WRITE_UNLOCK = 0xCAFE0000;
}

void hal_cmu_write_unlock(void)
{
    cmu->WRITE_UNLOCK = 0xCAFE0001;
}

#ifndef HAL_CMU_SYS_REBOOT
void hal_cmu_sys_reboot(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
}
#endif

void hal_cmu_simu_init(void)
{
#if defined(CHIP_BEST1501SIMU)
    cmu->MISC_0F8 = 0;
#else
    cmu->SIMU_RES = 0;
#endif
}

void hal_cmu_simu_pass(void)
{
#if defined(CHIP_BEST1501SIMU)
    cmu->MISC_0F8 = CMU_SIMU_RES_PASSED;
#else
    cmu->SIMU_RES = CMU_SIMU_RES_PASSED;
#endif
}

void hal_cmu_simu_fail(void)
{
#if defined(CHIP_BEST1501SIMU)
    cmu->MISC_0F8 = CMU_SIMU_RES_FAILED;
#else
    cmu->SIMU_RES = CMU_SIMU_RES_FAILED;
#endif
}

void hal_cmu_simu_tag(uint8_t shift)
{
#if defined(CHIP_BEST1501SIMU)
    cmu->MISC_0F8 |= (1 << shift);
#else
    cmu->SIMU_RES |= (1 << shift);
#endif
}

void hal_cmu_simu_set_val(uint32_t val)
{
#if defined(CHIP_BEST1501SIMU)
    cmu->MISC_0F8 = val;
#else
    cmu->SIMU_RES = val;
#endif
}

uint32_t hal_cmu_simu_get_val(void)
{
#if defined(CHIP_BEST1501SIMU)
    return cmu->MISC_0F8;
#else
    return cmu->SIMU_RES;
#endif
}

#if defined(CP_BOOT) || defined(CP_BUILD)
void hal_cmu_dbg_set_val(uint8_t id, uint32_t val)
{
    aoncmu->DEBUG_RES[id] = val;
}

uint32_t hal_cmu_dbg_get_val(uint8_t id)
{
    return aoncmu->DEBUG_RES[id];
}
#endif

int BOOT_TEXT_FLASH_LOC hal_cmu_flash_all_select_pll(enum HAL_CMU_PLL_T pll)
{
    hal_cmu_flash_select_pll(pll);
#ifdef FLASH1_CTRL_BASE
    hal_cmu_flash1_select_pll(pll);
#endif
    return 0;
}

#if !(defined(CHIP_SUBSYS_SENS) || (defined(CHIP_SUBSYS_BTH) && !defined(BTH_AS_MAIN_MCU)))

#if defined(CHIP_BEST1501) || defined(CHIP_BEST2000) || \
        defined(CHIP_BEST2300) || defined(CHIP_BEST2300A) || defined(CHIP_BEST2300P)
void hal_cmu_set_wakeup_pc(uint32_t pc)
{
    uint32_t *wake_pc =
#ifdef CHIP_BEST2000
        (uint32_t *)REGRET_BASE;
#else
        (uint32_t *)&aoncmu->WAKEUP_PC;

    STATIC_ASSERT(sizeof(HAL_POWER_DOWN_WAKEUP_HANDLER) <= sizeof(uint32_t), "Invalid func ptr size");
#endif

    *wake_pc = pc;
}

void hal_cmu_rom_wakeup_check(void)
{
    union HAL_HW_BOOTMODE_T hw;
    uint32_t sw;
    HAL_POWER_DOWN_WAKEUP_HANDLER *wake_fn =
#ifdef CHIP_BEST2000
        (HAL_POWER_DOWN_WAKEUP_HANDLER *)REGRET_BASE;
#else
        (HAL_POWER_DOWN_WAKEUP_HANDLER *)&aoncmu->WAKEUP_PC;
#endif

    hw = hal_rom_hw_bootmode_get();
    if (hw.watchdog == 0 && hw.global == 0) {
        sw = hal_sw_bootmode_get();
        if ((sw & HAL_SW_BOOTMODE_POWER_DOWN_WAKEUP) && *wake_fn) {
            (*wake_fn)();
        }
    }

    *wake_fn = NULL;
}
#endif

#ifndef HAL_CMU_USB_ROM_SELECT_CLOCK_SOURCE
enum HAL_CMU_USB_CLOCK_SEL_T hal_cmu_usb_rom_select_clock_source(int pll_en, unsigned int crystal)
{
    enum HAL_CMU_USB_CLOCK_SEL_T sel;

    if (pll_en) {
        sel = HAL_CMU_USB_CLOCK_SEL_PLL;
    } else {
        if (crystal == 24000000) {
            sel = HAL_CMU_USB_CLOCK_SEL_24M_X2;
        } else if (crystal == 48000000) {
            sel = HAL_CMU_USB_CLOCK_SEL_48M;
        } else {
            sel = HAL_CMU_USB_CLOCK_SEL_26M_X2;
        }
    }

    hal_cmu_usb_rom_set_clock_source(sel);

    return sel;
}
#endif

__STATIC_FORCEINLINE int hal_cmu_flash_all_set_freq(enum HAL_CMU_FREQ_T freq)
{
    hal_cmu_flash_set_freq(freq);
#ifdef FLASH1_CTRL_BASE
    hal_cmu_flash1_set_freq(freq);
#endif
    return 0;
}

#ifdef ROM_IN_FLASH
SRAM_TEXT_LOC
#endif
void hal_cmu_flash_all_reset_clear(int reset)
{
    if (reset) {
        // Reset flash controller (for JTAG reset and run)
        hal_cmu_reset_set(HAL_CMU_MOD_O_FLASH);
        hal_cmu_reset_set(HAL_CMU_MOD_H_FLASH);
    }
    // Enable flash controller (reset by default in BEST1400)
    hal_cmu_reset_clear(HAL_CMU_MOD_H_FLASH);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_FLASH);
#ifdef FLASH1_CTRL_BASE
    if (reset) {
        hal_cmu_reset_set(HAL_CMU_MOD_O_FLASH1);
        hal_cmu_reset_set(HAL_CMU_MOD_H_FLASH1);
    }
    hal_cmu_reset_clear(HAL_CMU_MOD_H_FLASH1);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_FLASH1);
#endif
}

#ifndef HAL_CMU_PLL_T
void hal_cmu_rom_enable_pll(void)
{
#ifdef CHIP_HAS_USB
    hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_USB);
    hal_cmu_flash_all_select_pll(HAL_CMU_PLL_USB);
#else
    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_AUD);
    hal_cmu_flash_all_select_pll(HAL_CMU_PLL_AUD);
#endif
}

void hal_cmu_programmer_enable_pll(void)
{
    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    hal_cmu_flash_all_select_pll(HAL_CMU_PLL_AUD);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_AUD);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_init_pll_selection(void)
{
    // !!!!!!
    // CAUTION:
    // hal_cmu_pll_enable()/hal_cmu_pll_disable() must be called after hal_chipid_init(),
    // for the init div values are extracted in hal_chipid_init().
    // !!!!!!

#if defined(CHIP_BEST1000) || defined(CHIP_BEST2000)
#ifdef CHIP_HAS_USB
    // Enable USB PLL before switching (clock mux requirement)
    // -- USB PLL might not be started in ROM
    hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);
#endif
    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
#else // !(best1000 || best2000)
    // Disable the PLL which might be enabled in ROM
#ifdef CHIP_HAS_USB
    hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_ALL);
#else
    hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_ALL);
#endif
#endif // !(best1000 || best2000)

#ifdef FLASH_LOW_SPEED
#ifdef CHIP_HAS_USB
    // Switch flash clock to USB PLL, and then shutdown USB PLL,
    // to save power consumed in clock divider
    hal_cmu_flash_all_select_pll(HAL_CMU_PLL_USB);
#endif
#else
    // Switch flash clock to audio PLL
    hal_cmu_flash_all_select_pll(HAL_CMU_PLL_AUD);
#endif

#ifdef CHIP_HAS_PSRAM
#ifdef PSRAM_LOW_SPEED
#ifdef CHIP_HAS_USB
    // Switch psram clock to USB PLL, and then shutdown USB PLL,
    // to save power consumed in clock divider
    hal_cmu_mem_select_pll(HAL_CMU_PLL_USB);
#endif
#else
    // Switch psram clock to audio PLL
    hal_cmu_mem_select_pll(HAL_CMU_PLL_AUD);
#endif
#endif

    // Select system PLL after selecting flash/psram PLLs
#ifdef ULTRA_LOW_POWER
    hal_cmu_low_freq_mode_init();
#else
#if defined(MCU_HIGH_PERFORMANCE_MODE) && defined(CHIP_HAS_USB)
    // Switch system clocks to USB PLL
    hal_cmu_sys_select_pll(HAL_CMU_PLL_USB);
#else
    // Switch system clocks to audio PLL
    hal_cmu_sys_select_pll(HAL_CMU_PLL_AUD);
#endif
#endif

#if defined(CHIP_BEST1000) || defined(CHIP_BEST2000)
#ifndef USB_PLL_INIT_ON
    // Disable USB PLL after switching (clock mux requirement)
    hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);
#endif
#ifndef AUD_PLL_INIT_ON
    hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
#endif
#else // !(best1000 || best2000)
#ifdef USB_PLL_INIT_ON
    hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);
#endif
#ifdef AUD_PLL_INIT_ON
    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
#endif
#endif // !(best1000 || best2000)

#if defined(MCU_HIGH_PERFORMANCE_MODE) && !defined(ULTRA_LOW_POWER) && defined(OSC_26M_X4_AUD2BB)
#error "Error configuration: MCU_HIGH_PERFORMANCE_MODE has no effect"
#endif
}
#endif // !HAL_CMU_PLL_T

static void BOOT_TEXT_FLASH_LOC hal_cmu_init_periph_clock(void)
{
#ifdef PERIPH_PLL_FREQ
    hal_cmu_periph_set_div(1);
#endif

    // TODO: Move the following SDIO freq setting to hal_sdio.c
#ifdef CHIP_HAS_SDIO
    hal_cmu_sdio_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
}

#ifdef ROM_IN_FLASH
SRAM_TEXT_LOC
#endif
void hal_cmu_rom_setup(void)
{
    int reset_flash;

    hal_cmu_lpu_wait_26m_ready();
    hal_cmu_simu_init();
    hal_cmu_rom_clock_init();
    hal_sys_timer_open();

    // Init sys clock
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);

#ifdef ROM_IN_FLASH
    // Wait until norflash becomes idle
    hal_sys_timer_delay(MS_TO_TICKS(3));
#endif
    // Init flash clock (this should be done before load_boot_settings, for security register read)
#ifndef CHIP_BEST1501SIMU
    hal_cmu_flash_all_set_freq(HAL_CMU_FREQ_26M);
#endif
#ifdef ROM_IN_FLASH
    reset_flash = false;
#else
    reset_flash = true;
#endif
    hal_cmu_flash_all_reset_clear(reset_flash);

    // TODO: Check why system crashes when ROM_IN_FLASH=1 and INSRAM_RUN=0
    // Disable cache (for JTAG reset and run)
    hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    hal_cache_disable(HAL_CACHE_ID_D_CACHE);

    // Init APB clock
    hal_cmu_apb_init_div();
}

void hal_cmu_programmer_setup(void)
{
#ifdef JTAG_ENABLE
    hal_iomux_set_jtag();
    hal_cmu_jtag_clock_enable();
#endif

    hal_cmu_ema_init();
    hal_sys_timer_open();

    // Init system/flash/memory clocks before initializing clock setting
    // and before switching PLL
    hal_cmu_flash_all_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);

#ifndef FPGA
    int ret;
    // Open analogif (ISPI)
    ret = hal_analogif_open();
    if (ret) {
        hal_cmu_simu_tag(31);
        do { volatile int i = 0; i++; } while (1);
    }
    // Init chip id
    // 1) Read id from ana/rf/pmu
    // 2) Init clock settings in ana/rf/pmu if the default h/w register values are bad
    hal_chipid_init();

    // Enable OSC X2/X4 in cmu after enabling their source in hal_chipid_init()
    hal_cmu_osc_x2_enable();
    hal_cmu_osc_x4_enable();

    // Enable PLL for flash (and system)
    hal_cmu_programmer_enable_pll();
#endif
}

#ifdef FPGA

void BOOT_TEXT_FLASH_LOC hal_cmu_fpga_setup(void)
{
    hal_sys_timer_open();
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_52M);

    hal_cmu_apb_init_div();
    hal_cmu_ispi_set_freq(HAL_CMU_PERIPH_FREQ_26M);

    // Init peripheral clocks
    hal_cmu_init_periph_clock();
#if !defined(CHIP_BEST1501SIMU)
    hal_norflash_init();
 #endif
 #if defined(CHIP_BEST1501SIMU)
    hal_cmu_module_init_state();
#endif
}

#else // !FPGA

void BOOT_TEXT_FLASH_LOC hal_cmu_setup(void)
{
    POSSIBLY_UNUSED int ret;
    enum HAL_CMU_FREQ_T freq;

#ifndef ARM_CMNS
    hal_iomux_set_default_config();
#endif

#ifdef JTAG_ENABLE
    hal_iomux_set_jtag();
    hal_cmu_jtag_clock_enable();
#endif
#ifdef CLOCK_OUT_ID
    hal_iomux_set_clock_out();
    hal_cmu_clock_out_enable(CLOCK_OUT_ID);
#endif

    hal_cmu_ema_init();
    hal_cmu_module_init_state();
    hal_sys_timer_open();
    hal_hw_bootmode_init();
#ifndef __MCU_FW_2002__
#if !defined(ARM_CMNS)
    // Init system/flash/memory clocks before initializing clock setting
    // and before switching PLL
    hal_norflash_set_boot_freq(HAL_CMU_FREQ_26M);
#if defined(CHIP_HAS_EXT_NORFLASH)
        //hal_cmu_flash1_enable();
#endif
#endif
#endif
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);

    // Set ISPI module freq
    hal_cmu_ispi_set_freq(HAL_CMU_PERIPH_FREQ_26M);
    // Open analogif (ISPI)
    ret = hal_analogif_open();
    if (ret) {
        hal_cmu_simu_tag(31);
        do { volatile int i = 0; i++; } while (1);
    }
    // Init chip id
    // 1) Read id from ana/rf/pmu
    // 2) Init clock settings in ana/rf/pmu if the default h/w register values are bad
    hal_chipid_init();

#ifdef CALIB_SLOW_TIMER
    // Calib slow timer after determining the crystal freq
    hal_sys_timer_calib();
#endif

    // Enable OSC X2/X4 in cmu after enabling their source in hal_chipid_init()
    hal_cmu_osc_x2_enable();
    hal_cmu_osc_x4_enable();

    // Init PLL selection
    hal_cmu_init_pll_selection();

    // Init peripheral clocks
    hal_cmu_init_periph_clock();

    // Sleep setting
#ifdef NO_LPU_26M
    while (hal_cmu_lpu_init(HAL_CMU_LPU_CLK_NONE) == -1);
#else
    while (hal_cmu_lpu_init(HAL_CMU_LPU_CLK_26M) == -1);
#endif
    // Init sys freq after applying the sleep setting (which might change sys freq)
#ifdef NO_LPU_26M
   hal_sys_timer_delay(MS_TO_TICKS(20));
#endif

    // Init system clock
#ifdef ULTRA_LOW_POWER
    freq = HAL_CMU_FREQ_52M;
#else
    freq = HAL_CMU_FREQ_104M;
#endif
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, freq);
#ifndef __MCU_FW_2002__
    // Init flash
#if !defined(ARM_CMNS)
    hal_norflash_init();
#if defined(CHIP_HAS_EXT_NORFLASH)&&defined(FLASH1_CTRL_BASE)
    //hal_norflash2_init();
#endif

#endif
#endif
}

#endif // !FPGA

#endif // !CHIP_SUBSYS_SENS

