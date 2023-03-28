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
#include CHIP_SPECIFIC_HDR(reg_cmu)
#include CHIP_SPECIFIC_HDR(reg_aoncmu)
#include CHIP_SPECIFIC_HDR(reg_btcmu)
#include CHIP_SPECIFIC_HDR(reg_wfcmu)
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_bootmode.h"
#include "hal_chipid.h"
#include "hal_codec.h"
#include "hal_location.h"
#include "hal_psc.h"
#include "hal_sec.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "cmsis_nvic.h"
#include "pmu.h"
#include "analog.h"
#include "system_cp.h"
#include "hal_psram.h"
#include "hal_psramuhs.h"
#include "hal_norflash.h"
#include "hal_wdt.h"
#include "tgt_hardware.h"
#include "hal_dma.h"
#include CHIP_SPECIFIC_HDR(hal_dmacfg)

#ifdef FLASH_SUSPEND
#include "norflash_api.h"
#endif

#define HAL_CMU_USB_PLL_CLOCK           (192 * 1000 * 1000)
#define HAL_CMU_AUD_PLL_CLOCK           (CODEC_FREQ_48K_SERIES * CODEC_CMU_DIV)

#define HAL_CMU_USB_CLOCK_60M           (60 * 1000 * 1000)
#define HAL_CMU_USB_CLOCK_48M           (48 * 1000 * 1000)

#define HAL_CMU_PWM_SLOW_CLOCK          (32 * 1000)

#define HAL_CMU_PLL_LOCKED_TIMEOUT      US_TO_TICKS(200)
#define HAL_CMU_26M_READY_TIMEOUT       MS_TO_TICKS(3)
#define HAL_CMU_LPU_EXTRA_TIMEOUT       MS_TO_TICKS(1)

enum CMU_USB_CLK_SRC_T {
    CMU_USB_CLK_SRC_PLL_48M         = 0,
    CMU_USB_CLK_SRC_PLL_60M         = 1,
    CMU_USB_CLK_SRC_OSC_24M_X2      = 2,
};

enum CMU_AUD_26M_X4_USER_T {
    CMU_AUD_26M_X4_USER_IIR,
    CMU_AUD_26M_X4_USER_RS,

    CMU_AUD_26M_X4_USER_QTY,
};

enum CMU_DEBUG_REG_SEL_T {
    CMU_DEBUG_REG_SEL_MCU_PC        = 0,
    CMU_DEBUG_REG_SEL_MCU_LR        = 1,
    CMU_DEBUG_REG_SEL_MCU_SP        = 2,
    CMU_DEBUG_REG_SEL_CP_PC         = 3,
    CMU_DEBUG_REG_SEL_CP_LR         = 4,
    CMU_DEBUG_REG_SEL_CP_SP         = 5,
    CMU_DEBUG_REG_SEL_DEBUG         = 7,
};

enum CMU_DMA_REQ_T {
    CMU_DMA_REQ_CODEC_RX            = 0,
    CMU_DMA_REQ_CODEC_TX            = 1,
    CMU_DMA_REQ_DSD_RX              = 2,
    CMU_DMA_REQ_DSD_TX              = 3,
    CMU_DMA_REQ_IR_RX               = 6,
    CMU_DMA_REQ_IR_TX               = 7,
    CMU_DMA_REQ_FLS1                = 8,
    CMU_DMA_REQ_FLS0                = 10,
    CMU_DMA_REQ_BTDUMP              = 12,
    CMU_DMA_REQ_SDEMMC              = 13,
    CMU_DMA_REQ_I2C0_RX             = 14,
    CMU_DMA_REQ_I2C0_TX             = 15,
    CMU_DMA_REQ_I2C1_RX             = 16,
    CMU_DMA_REQ_I2C1_TX             = 17,
    CMU_DMA_REQ_SPILCD0_RX          = 18,
    CMU_DMA_REQ_SPILCD0_TX          = 19,
    CMU_DMA_REQ_SPILCD1_RX          = 20,
    CMU_DMA_REQ_SPILCD1_TX          = 21,
    CMU_DMA_REQ_SPI_ITN_RX          = 22,
    CMU_DMA_REQ_SPI_ITN_TX          = 23,
    CMU_DMA_REQ_UART0_RX            = 24,
    CMU_DMA_REQ_UART0_TX            = 25,
    CMU_DMA_REQ_UART1_RX            = 26,
    CMU_DMA_REQ_UART1_TX            = 27,
    CMU_DMA_REQ_UART2_RX            = 28,
    CMU_DMA_REQ_UART2_TX            = 29,
    CMU_DMA_REQ_PCM_RX              = 30,
    CMU_DMA_REQ_PCM_TX              = 31,
    CMU_DMA_REQ_I2S0_RX             = 32,
    CMU_DMA_REQ_I2S0_TX             = 33,
    CMU_DMA_REQ_SPDIF0_RX           = 34,
    CMU_DMA_REQ_SPDIF0_TX           = 35,
    CMU_DMA_REQ_I2C2_RX             = 36,
    CMU_DMA_REQ_I2C2_TX             = 37,
    CMU_DMA_REQ_UART3_RX            = 38,
    CMU_DMA_REQ_UART3_TX            = 39,
    CMU_DMA_REQ_I2S1_RX             = 40,
    CMU_DMA_REQ_I2S1_TX             = 41,

    CMU_DMA_REQ_NULL                = 5,
};

struct CP_STARTUP_CFG_T {
    __IO uint32_t stack;
    __IO uint32_t reset_hdlr;
};

static struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;

static struct AONCMU_T * const aoncmu = (struct AONCMU_T *)AON_CMU_BASE;

static struct BTCMU_T * const POSSIBLY_UNUSED btcmu = (struct BTCMU_T *)BT_CMU_BASE;

static struct WLANCMU_T * const POSSIBLY_UNUSED wlancmu = (struct WLANCMU_T *)WIFI_CMU_BASE;

static uint32_t cp_entry;

static uint8_t BOOT_BSS_LOC pll_user_map[HAL_CMU_PLL_QTY];
STATIC_ASSERT(HAL_CMU_PLL_USER_QTY <= sizeof(pll_user_map[0]) * 8, "Too many PLL users");

#ifdef __AUDIO_RESAMPLE__
static bool aud_resample_en = true;
#ifdef ANA_26M_X4_ENABLE
static uint8_t aud_26m_x4_map;
STATIC_ASSERT(CMU_AUD_26M_X4_USER_QTY <= sizeof(aud_26m_x4_map) * 8, "Too many aud_26m_x4 users");
#endif
#endif

#ifdef ANA_26M_X4_ENABLE
#error "CHIP_BEST2003 don't support X4"
#endif

#ifdef LOW_SYS_FREQ
static enum HAL_CMU_FREQ_T BOOT_BSS_LOC cmu_sys_freq;
#endif

void hal_cmu_audio_resample_enable(void)
{
#ifdef __AUDIO_RESAMPLE__
    aud_resample_en = true;
#endif
}

void hal_cmu_audio_resample_disable(void)
{
#ifdef __AUDIO_RESAMPLE__
    aud_resample_en = false;
#endif
}

int hal_cmu_get_audio_resample_status(void)
{
#ifdef __AUDIO_RESAMPLE__
    return aud_resample_en;
#else
    return false;
#endif
}

static inline void aocmu_reg_update_wait(void)
{
    // Make sure AOCMU (26M clock domain) write opertions finish before return
    aoncmu->CHIP_ID;
}

int hal_cmu_clock_enable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_AON_A7) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_ENABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_ENABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        cmu->OCLK_ENABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->QCLK_ENABLE = (1 << (id - HAL_CMU_MOD_Q_NULL));
    } else {
        aoncmu->MOD_CLK_ENABLE = (1 << (id - HAL_CMU_AON_A_CMU));
        aocmu_reg_update_wait();
    }

    return 0;
}

int hal_cmu_clock_disable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_AON_A7) {
        return 1;
    }

    if (id == HAL_CMU_MOD_P_TZC)
        return 0;

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_DISABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_DISABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        cmu->OCLK_DISABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->QCLK_DISABLE = (1 << (id - HAL_CMU_MOD_Q_NULL));
    } else {
        aoncmu->MOD_CLK_DISABLE = (1 << (id - HAL_CMU_AON_A_CMU));
    }

    return 0;
}

enum HAL_CMU_CLK_STATUS_T hal_cmu_clock_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_AON_A7) {
        return HAL_CMU_CLK_DISABLED;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HCLK_ENABLE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PCLK_ENABLE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        status = cmu->OCLK_ENABLE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = cmu->QCLK_ENABLE & (1 << (id - HAL_CMU_MOD_Q_NULL));
    } else {
        status = aoncmu->MOD_CLK_ENABLE & (1 << (id - HAL_CMU_AON_A_CMU));
    }

    return status ? HAL_CMU_CLK_ENABLED : HAL_CMU_CLK_DISABLED;
}

int hal_cmu_clock_set_mode(enum HAL_CMU_MOD_ID_T id, enum HAL_CMU_CLK_MODE_T mode)
{
    __IO uint32_t *reg;
    uint32_t val;
    uint32_t lock;

    if (id >= HAL_CMU_AON_A7) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        reg = &cmu->HCLK_MODE;
        val = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        reg = &cmu->PCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        reg = &cmu->OCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_A_CMU) {
        reg = &cmu->QCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_Q_NULL));
    } else {
        reg = &aoncmu->MOD_CLK_MODE;
        val = (1 << (id - HAL_CMU_AON_A_CMU));
    }

    lock = int_lock();
    if (mode == HAL_CMU_CLK_MANUAL) {
        *reg |= val;
    } else {
        *reg &= ~val;
    }
    int_unlock(lock);

    return 0;
}

enum HAL_CMU_CLK_MODE_T hal_cmu_clock_get_mode(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t mode;

    if (id >= HAL_CMU_AON_A7) {
        return HAL_CMU_CLK_AUTO;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        mode = cmu->HCLK_MODE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        mode = cmu->PCLK_MODE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        mode = cmu->OCLK_MODE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_A_CMU) {
        mode = cmu->QCLK_MODE & (1 << (id - HAL_CMU_MOD_Q_NULL));
    } else {
        mode = aoncmu->MOD_CLK_MODE & (1 << (id - HAL_CMU_AON_A_CMU));
    }

    return mode ? HAL_CMU_CLK_MANUAL : HAL_CMU_CLK_AUTO;
}

int hal_cmu_reset_set(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id == HAL_CMU_MOD_P_TZC)
        return 0;

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_SET = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_SET = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        cmu->ORESET_SET = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->QRESET_SET = (1 << (id - HAL_CMU_MOD_Q_NULL));
    } else if (id < HAL_CMU_AON_A7) {
        aoncmu->RESET_SET = (1 << (id - HAL_CMU_AON_A_CMU));
    } else {
        aoncmu->SOFT_RSTN_SET = (1 << (id - HAL_CMU_AON_A7));
    }

    return 0;
}

int hal_cmu_reset_clear(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_CLR = (1 << id);
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop;");
        asm volatile("nop; nop; nop; nop;");
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_CLR = (1 << (id - HAL_CMU_MOD_P_CMU));
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop;");
        asm volatile("nop; nop; nop; nop;");
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        cmu->ORESET_CLR = (1 << (id - HAL_CMU_MOD_O_SLEEP));
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop;");
        asm volatile("nop; nop; nop; nop;");
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->QRESET_CLR = (1 << (id - HAL_CMU_MOD_Q_NULL));
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop;");
        asm volatile("nop; nop; nop; nop;");
    } else if (id < HAL_CMU_AON_A7) {
        aoncmu->RESET_CLR = (1 << (id - HAL_CMU_AON_A_CMU));
        aocmu_reg_update_wait();
    } else {
        aoncmu->SOFT_RSTN_CLR = (1 << (id - HAL_CMU_AON_A7));
        aocmu_reg_update_wait();
    }

    return 0;
}

enum HAL_CMU_RST_STATUS_T hal_cmu_reset_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_RST_SET;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HRESET_SET & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PRESET_SET & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        status = cmu->ORESET_SET & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = cmu->QRESET_SET & (1 << (id - HAL_CMU_MOD_Q_NULL));
    } else if (id < HAL_CMU_AON_A7) {
        status = aoncmu->RESET_SET & (1 << (id - HAL_CMU_AON_A_CMU));
    } else {
        status = aoncmu->SOFT_RSTN_SET & (1 << (id - HAL_CMU_AON_A7));
    }

    return status ? HAL_CMU_RST_CLR : HAL_CMU_RST_SET;
}

int hal_cmu_reset_pulse(enum HAL_CMU_MOD_ID_T id)
{
    volatile int i;

    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (hal_cmu_reset_get_status(id) == HAL_CMU_RST_SET) {
        return hal_cmu_reset_clear(id);
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_PULSE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_PULSE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_MOD_Q_NULL) {
        cmu->ORESET_PULSE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->QRESET_PULSE = (1 << (id - HAL_CMU_MOD_Q_NULL));
    } else if (id < HAL_CMU_AON_A7) {
        aoncmu->RESET_PULSE = (1 << (id - HAL_CMU_AON_A_CMU));
        // Total 3 CLK-26M cycles needed
        // AOCMU runs in 26M clock domain and its read operations consume at least 1 26M-clock cycle.
        // (Whereas its write operations will finish at 1 HCLK cycle -- finish once in async bridge fifo)
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
    } else {
        aoncmu->SOFT_RSTN_PULSE = (1 << (id - HAL_CMU_AON_A7));
        // Total 3 CLK-26M cycles needed
        // AOCMU runs in 26M clock domain and its read operations consume at least 1 26M-clock cycle.
        // (Whereas its write operations will finish at 1 HCLK cycle -- finish once in async bridge fifo)
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
    }
    // Delay 5+ PCLK cycles (10+ HCLK cycles)
    for (i = 0; i < 3; i++);

    return 0;
}

int hal_cmu_timer_set_div(enum HAL_CMU_TIMER_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (div < 1) {
        return 1;
    }

    div -= 1;
    if ((div & (CMU_CFG_DIV_TIMER00_MASK >> CMU_CFG_DIV_TIMER00_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    if (id == HAL_CMU_TIMER_ID_00) {
        cmu->TIMER0_CLK = SET_BITFIELD(cmu->TIMER0_CLK, CMU_CFG_DIV_TIMER00, div);
    } else if (id == HAL_CMU_TIMER_ID_01) {
        cmu->TIMER0_CLK = SET_BITFIELD(cmu->TIMER0_CLK, CMU_CFG_DIV_TIMER01, div);
    } else if (id == HAL_CMU_TIMER_ID_10) {
        cmu->TIMER1_CLK = SET_BITFIELD(cmu->TIMER1_CLK, CMU_CFG_DIV_TIMER10, div);
    } else if (id == HAL_CMU_TIMER_ID_11) {
        cmu->TIMER1_CLK = SET_BITFIELD(cmu->TIMER1_CLK, CMU_CFG_DIV_TIMER11, div);
    } else if (id == HAL_CMU_TIMER_ID_20) {
        cmu->TIMER2_CLK = SET_BITFIELD(cmu->TIMER2_CLK, CMU_CFG_DIV_TIMER20, div);
    } else if (id == HAL_CMU_TIMER_ID_21) {
        cmu->TIMER2_CLK = SET_BITFIELD(cmu->TIMER2_CLK, CMU_CFG_DIV_TIMER21, div);
    }
    int_unlock(lock);

    return 0;
}

void hal_cmu_timer0_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->PERIPH_CLK |= (1 << CMU_SEL_TIMER_FAST_SHIFT);
    // AON Timer
    aoncmu->TIMER_CLK |= AON_CMU_SEL_TIMER_FAST;
    int_unlock(lock);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_timer0_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->PERIPH_CLK &= ~(1 << CMU_SEL_TIMER_FAST_SHIFT);
    // AON Timer
    aoncmu->TIMER_CLK &= ~AON_CMU_SEL_TIMER_FAST;
    int_unlock(lock);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_timer1_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->PERIPH_CLK |= (1 << (CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_timer1_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->PERIPH_CLK &= ~(1 << (CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

void hal_cmu_timer2_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->PERIPH_CLK |= (1 << (CMU_SEL_TIMER_FAST_SHIFT + 2));
    int_unlock(lock);
}

void hal_cmu_timer2_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->PERIPH_CLK &= ~(1 << (CMU_SEL_TIMER_FAST_SHIFT + 2));
    int_unlock(lock);
}

void hal_cmu_dsp_timer0_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->DSP_DIV |= (1 << CMU_SEL_TIMER_FAST_AP_SHIFT);
    int_unlock(lock);
}

void hal_cmu_dsp_timer0_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->DSP_DIV &= ~(1 << CMU_SEL_TIMER_FAST_AP_SHIFT);
    int_unlock(lock);
}

void hal_cmu_dsp_timer1_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->DSP_DIV |= (1 << (CMU_SEL_TIMER_FAST_AP_SHIFT + 1));
    int_unlock(lock);
}

void hal_cmu_dsp_timer1_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->DSP_DIV &= ~(1 << (CMU_SEL_TIMER_FAST_AP_SHIFT + 1));
    int_unlock(lock);
}

int hal_cmu_sdmmc_set_pll_div(uint32_t div)
{
    uint32_t lock;
    if (div < 2) {
        return 1;
    }
    div -= 2;
    if ((div & (CMU_CFG_DIV_SDMMC_MASK >> CMU_CFG_DIV_SDMMC_SHIFT)) != div) {
        return 1;
    }
    lock = int_lock();
    cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_CFG_DIV_SDMMC, div) |
        CMU_SEL_OSCX2_SDMMC | CMU_SEL_PLL_SDMMC | CMU_EN_PLL_SDMMC;
    int_unlock(lock);
    return 0;
}

int BOOT_TEXT_SRAM_LOC hal_cmu_flash_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t set;
    uint32_t clr;
    uint32_t lock;
    bool clk_en;

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }
    if (freq == HAL_CMU_FREQ_32K) {
        return 2;
    }

    switch (freq) {
    case HAL_CMU_FREQ_26M:
        set = 0;
        clr = AON_CMU_RSTN_DIV_FLS | AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_OSCX2 | AON_CMU_SEL_FLS_OSCX4 | AON_CMU_SEL_FLS_PLL;
        break;
    case HAL_CMU_FREQ_52M:
        set = AON_CMU_SEL_FLS_OSCX2;
        clr = AON_CMU_RSTN_DIV_FLS | AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_OSCX4 | AON_CMU_SEL_FLS_PLL;
        break;
    case HAL_CMU_FREQ_78M:
    case HAL_CMU_FREQ_104M:
#ifdef OSC_26M_X4_AUD2BB
        set = AON_CMU_SEL_FLS_OSCX4;
        clr = AON_CMU_RSTN_DIV_FLS | AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_OSCX2 | AON_CMU_SEL_FLS_PLL;
        break;
#endif
    case HAL_CMU_FREQ_156M:
    case HAL_CMU_FREQ_208M:
    case HAL_CMU_FREQ_260M:
        set = AON_CMU_RSTN_DIV_FLS | AON_CMU_SEL_FLS_PLL;
        clr = AON_CMU_CFG_DIV_FLS_MASK | AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_OSCX2 | AON_CMU_SEL_FLS_OSCX4;
        if (0) {
#ifndef OSC_26M_X4_AUD2BB
        } else if (freq <= HAL_CMU_FREQ_78M) {
            set |= AON_CMU_CFG_DIV_FLS(3);
        } else if (freq <= HAL_CMU_FREQ_104M) {
            set |= AON_CMU_CFG_DIV_FLS(2);
#endif
        } else if (freq <= HAL_CMU_FREQ_156M) {
            set |= AON_CMU_CFG_DIV_FLS(1);
        } else { // 208M or 260M
            set |= AON_CMU_CFG_DIV_FLS(0);
        }
        break;
    case HAL_CMU_FREQ_390M:
    case HAL_CMU_FREQ_780M:
    default:
        set = AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_PLL;
        clr = AON_CMU_RSTN_DIV_FLS | AON_CMU_SEL_FLS_OSCX2 | AON_CMU_SEL_FLS_OSCX4;
        break;
    };

    lock = int_lock();
    clk_en = !!(cmu->OCLK_DISABLE & SYS_OCLK_FLASH);
    if (clk_en) {
        cmu->OCLK_DISABLE = SYS_OCLK_FLASH;
        cmu->HCLK_DISABLE = SYS_HCLK_FLASH;
        // Wait at least 2 cycles of flash controller. The min freq is 26M, the same as AON.
        aocmu_reg_update_wait();
        aocmu_reg_update_wait();
    }
    aoncmu->FLS_PSR_CLK = (aoncmu->FLS_PSR_CLK & ~clr) | set;
    if (clk_en) {
        cmu->HCLK_ENABLE = SYS_HCLK_FLASH;
        cmu->OCLK_ENABLE = SYS_OCLK_FLASH;
    }
    int_unlock(lock);

    return 0;
}

int BOOT_TEXT_SRAM_LOC hal_cmu_flash1_set_freq(enum HAL_CMU_FREQ_T freq)
{
    aoncmu->FLASH_IOCFG |= AON_CMU_PU_FLASH1_IO;
    //TODO: 2003 flash0 and flash1 use same clock source, so don't change clk freq in case of flash0 error
#ifdef ROM_BUILD //2003 rom only use one flash
    return hal_cmu_flash_set_freq(freq);
#else
    return 0;
#endif
}

int hal_cmu_mem_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t set;
    uint32_t clr;
    uint32_t lock;
    bool clk_en;

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }
    if (freq == HAL_CMU_FREQ_32K) {
        return 2;
    }

    // 2003 psram ctrl use 400M clk from bbpsram
    switch (freq) {
    case HAL_CMU_FREQ_26M:
        set = 0;
        clr = AON_CMU_RSTN_DIV_PSR | AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_OSCX2 | AON_CMU_SEL_PSR_OSCX4 | AON_CMU_SEL_PSR_PLL;
        break;
    case HAL_CMU_FREQ_52M:
        set = AON_CMU_SEL_PSR_OSCX2;
        clr = AON_CMU_RSTN_DIV_PSR | AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_OSCX4 | AON_CMU_SEL_PSR_PLL;
        break;
    case HAL_CMU_FREQ_78M:
    case HAL_CMU_FREQ_104M:
#ifdef OSC_26M_X4_AUD2BB
        set = AON_CMU_SEL_PSR_OSCX4;
        clr = AON_CMU_RSTN_DIV_PSR | AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_OSCX2 | AON_CMU_SEL_PSR_PLL;
        break;
#endif
    case HAL_CMU_FREQ_156M:
    case HAL_CMU_FREQ_208M:
    case HAL_CMU_FREQ_260M:
        set = AON_CMU_RSTN_DIV_PSR | AON_CMU_SEL_PSR_PLL | AON_CMU_SEL_PSR_INT;
        clr = AON_CMU_CFG_DIV_PSR_MASK | AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_OSCX2 | AON_CMU_SEL_PSR_OSCX4;
        if (0) {
#ifndef OSC_26M_X4_AUD2BB
        } else if (freq <= HAL_CMU_FREQ_78M) {
            set |= AON_CMU_CFG_DIV_PSR(3);
        } else if (freq <= HAL_CMU_FREQ_104M) {
            set |= AON_CMU_CFG_DIV_PSR(2);
#endif
        } else if (freq <= HAL_CMU_FREQ_156M) {
            set |= AON_CMU_CFG_DIV_PSR(1);
        } else { // 208M or 260M
            set |= AON_CMU_CFG_DIV_PSR(0);
        }
        break;
    case HAL_CMU_FREQ_390M:
    case HAL_CMU_FREQ_780M:
    default:
        set = AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_PLL | AON_CMU_SEL_PSR_INT;
        clr = AON_CMU_RSTN_DIV_PSR | AON_CMU_SEL_PSR_OSCX2 | AON_CMU_SEL_PSR_OSCX4;
        break;
    };

    lock = int_lock();
    clk_en = !!(cmu->OCLK_DISABLE & SYS_OCLK_PSRAM200);
    if (clk_en) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PHY_PS_DISABLE;
        cmu->OCLK_DISABLE = SYS_OCLK_PSRAM200;
        cmu->HCLK_DISABLE = SYS_HCLK_PSRAM200;
        // Wait at least 2 cycles of psram controller. The min freq is 26M, the same as AON.
        aocmu_reg_update_wait();
        aocmu_reg_update_wait();
    }
    aoncmu->FLS_PSR_CLK = (aoncmu->FLS_PSR_CLK & ~clr) | set;
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PHY_PS_ENABLE;
    if (clk_en) {
        cmu->HCLK_ENABLE = SYS_HCLK_PSRAM200;
        cmu->OCLK_ENABLE = SYS_OCLK_PSRAM200;
    }
    int_unlock(lock);

    return 0;
}

int hal_cmu_ddr_clock_enable()
{
    uint32_t set;
    uint32_t clr;
    uint32_t lock;
    bool clk_en;

    hal_cmu_pll_enable(HAL_CMU_PLL_DDR, HAL_CMU_PLL_USER_PSRAM);
#if 1
    //pxclk use xclk, works in synchronous mode
    //a7 can change sys freq in this mode
    cmu->PERIPH_CLK &= ~CMU_SEL_PSRAMX2;
    *(volatile uint32_t *)(GPV_PSRAM1G_BASE+0x42020) = 0;
#else
    //pxclk use psram2, works in asynchronous mode
    *(volatile uint32_t *)(GPV_PSRAM1G_BASE+0x42020) = 4;
    cmu->PERIPH_CLK |= CMU_SEL_PSRAMX2;
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_PSRAMX2_ENABLE;
#endif

    // PSRAMUHS can't use ddr pll divider
    set = AON_CMU_BYPASS_DIV_DDR | AON_CMU_SEL_DDR_PLL | AON_CMU_CFG_DIV_PSRAMX2(2);
    clr = AON_CMU_RSTN_DIV_DDR | AON_CMU_SEL_DDR_OSCX2 | AON_CMU_SEL_DDR_OSCX4 | AON_CMU_CFG_DIV_PSRAMX2_MASK;

    lock = int_lock();
    clk_en = !!(cmu->OCLK_DISABLE & SYS_OCLK_PSRAM1G);
    if (clk_en) {
        cmu->OCLK_DISABLE = SYS_OCLK_PSRAM1G;
        cmu->HCLK_DISABLE = SYS_HCLK_PSRAM1G;
        // Wait at least 2 cycles of psram controller. The min freq is 26M, the same as AON.
        aocmu_reg_update_wait();
        aocmu_reg_update_wait();
    }
    aoncmu->DDR_CLK = (aoncmu->DDR_CLK & ~clr) | set;

    cmu->XCLK_ENABLE = SYS_XCLK_PSRAM1G | SYS_XCLK_PSRAM1GMX | SYS_XCLK_GPV_PSRAM1G;
    cmu->HCLK_ENABLE = SYS_HCLK_PSRAM1G;
    cmu->OCLK_ENABLE = SYS_OCLK_PSRAM1G;

    int_unlock(lock);

    return 0;
}

void hal_cmu_ddr_clock_disable()
{
    cmu->XCLK_DISABLE = SYS_XCLK_PSRAM1G | SYS_XCLK_PSRAM1GMX | SYS_XCLK_GPV_PSRAM1G;
    cmu->OCLK_DISABLE = SYS_OCLK_PSRAM1G;
    cmu->HCLK_DISABLE = SYS_HCLK_PSRAM1G;
    hal_cmu_pll_disable(HAL_CMU_PLL_DDR, HAL_CMU_PLL_USER_PSRAM);
}

void hal_cmu_ddr_reset_set()
{
    cmu->XRESET_SET = SYS_XRST_PSRAM1G | SYS_XRST_PSRAM1GMX | SYS_XRST_GPV_PSRAM1G;
    cmu->ORESET_SET = SYS_ORST_PSRAM1G;
    cmu->HRESET_SET = SYS_HRST_PSRAM1G;
}

void hal_cmu_ddr_reset_clear()
{
    cmu->XRESET_CLR = SYS_XRST_PSRAM1G | SYS_XRST_PSRAM1GMX | SYS_XRST_GPV_PSRAM1G;
    cmu->ORESET_CLR = SYS_ORST_PSRAM1G;
    cmu->HRESET_CLR = SYS_HRST_PSRAM1G;
}

int hal_cmu_dsp_set_freq(enum HAL_CMU_FREQ_T freq);

void hal_cmu_dsi_phy_reset_set(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DSI_32K);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DSI_PN);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DSI_TV);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DSI_PIX);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DSI_DSI);
}

void hal_cmu_dsi_phy_reset_clear(void)
{
    // TODO: Move clock enable APIs out
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_32K);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_PN);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_TV);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_PIX);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_DSI);

    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_32K);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_PN);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_TV);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_PIX);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_DSI);
}

void hal_cmu_dsi_clock_enable(void)
{
#if !defined(DSP_ENABLE) && !defined(CHIP_BEST2003_DSP)
    hal_cmu_dsp_clock_enable();
    hal_cmu_dsp_reset_clear();
    hal_sys_timer_delay(US_TO_TICKS(10));
#endif
    hal_cmu_dsp_set_freq(HAL_CMU_FREQ_780M);
    cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_APCLK, 0x0);
    hal_cmu_pll_enable(HAL_CMU_PLL_DSI, HAL_CMU_PLL_USER_DSI);
    cmu->APCLK_ENABLE = SYS_APCLK_DISPLAY;
    aoncmu->MIPI_CLK = AON_CMU_EN_CLK_PIX_DSI | AON_CMU_POL_CLK_DSI_IN |
        SET_BITFIELD(aoncmu->MIPI_CLK, AON_CMU_CFG_DIV_PIX_DSI, 0x4);
    cmu->QCLK_ENABLE = SYS_QCLK_DSI_DSI | SYS_QCLK_DSI_PIX;
    hal_sys_timer_delay_us(10);
    hal_cmu_dsi_reset_set();//QR DSI/PIX CLK bit 4/5; APR APB CLK bit 8
    hal_sys_timer_delay(US_TO_TICKS(10));
    hal_cmu_dsi_reset_clear();
}

void hal_cmu_dsi_clock_enable_v2(uint8_t pixel_div)
{
    hal_cmu_dsp_set_freq(HAL_CMU_FREQ_780M);
    cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_APCLK, 0x0);
    hal_cmu_pll_enable(HAL_CMU_PLL_DSI, HAL_CMU_PLL_USER_DSI);
    cmu->APCLK_ENABLE = SYS_APCLK_DISPLAY;
    aoncmu->MIPI_CLK = AON_CMU_EN_CLK_PIX_DSI | AON_CMU_POL_CLK_DSI_IN |
        SET_BITFIELD(aoncmu->MIPI_CLK, AON_CMU_CFG_DIV_PIX_DSI, pixel_div);
    cmu->QCLK_ENABLE = SYS_QCLK_DSI_DSI | SYS_QCLK_DSI_PIX;
    hal_sys_timer_delay_us(10);
}

void hal_cmu_dsi_clock_disable(void)
{
    aoncmu->MIPI_CLK &= ~(AON_CMU_EN_CLK_PIX_DSI | AON_CMU_POL_CLK_DSI_IN);
    cmu->QCLK_DISABLE = SYS_QCLK_DSI_DSI | SYS_QCLK_DSI_PIX;
    cmu->APCLK_DISABLE = SYS_APCLK_DISPLAY;
    hal_cmu_pll_disable(HAL_CMU_PLL_DSI, HAL_CMU_PLL_USER_DSI);
}

void hal_cmu_dsi_reset_set(void)
{
    cmu->QRESET_SET = SYS_QRST_DSI_DSI | SYS_QRST_DSI_PIX;
    cmu->APRESET_SET = SYS_APRST_DISPLAY;
}

void hal_cmu_dsi_reset_clear(void)
{
    cmu->APRESET_CLR = SYS_APRST_DISPLAY;
    cmu->QRESET_CLR = SYS_QRST_DSI_DSI | SYS_QRST_DSI_PIX;
    hal_sys_timer_delay_us(10);
}

void hal_cmu_dsi_sleep(void)
{
    hal_cmu_dsi_clock_disable();
}

void hal_cmu_dsi_wakeup(void)
{
    hal_cmu_dsi_clock_enable();
}

void hal_cmu_lcdc_clock_enable(void)
{
    hal_cmu_lcdc_reset_clear();
    cmu->XCLK_ENABLE = SYS_XCLK_DSI | SYS_XCLK_DISPLAYX | SYS_XCLK_DISPLAYH;
    cmu->QCLK_ENABLE = SYS_QCLK_DSI_32K | SYS_QCLK_DSI_PN | SYS_QCLK_DSI_TV;

    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_32K);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_PN);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_TV);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_PIX);
    hal_cmu_clock_enable(HAL_CMU_MOD_Q_DSI_DSI);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_32K);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_PN);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_TV);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_PIX);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_DSI);
}

void hal_cmu_lcdc_clock_disable(void)
{
    cmu->QCLK_DISABLE = SYS_QCLK_DSI_32K | SYS_QCLK_DSI_PN | SYS_QCLK_DSI_TV;
    cmu->XCLK_DISABLE = SYS_XCLK_DSI | SYS_XCLK_DISPLAYX | SYS_XCLK_DISPLAYH;
}

void hal_cmu_lcdc_reset_set(void)
{
    cmu->QRESET_SET = SYS_QRST_DSI_32K | SYS_QRST_DSI_PN | SYS_QRST_DSI_TV;
    cmu->XRESET_SET = SYS_XRST_DSI | SYS_XRST_DISPLAYX | SYS_XRST_DISPLAYH;
}

void hal_cmu_lcdc_reset_clear(void)
{
    cmu->XRESET_CLR = SYS_XRST_DSI | SYS_XRST_DISPLAYX | SYS_XRST_DISPLAYH;
    cmu->QRESET_CLR = SYS_QRST_DSI_32K | SYS_QRST_DSI_PN | SYS_QRST_DSI_TV;
    hal_sys_timer_delay_us(10);
}

void hal_cmu_lcdc_sleep(void)
{
    hal_cmu_lcdc_clock_disable();
}

void hal_cmu_lcdc_wakeup(void)
{
    hal_cmu_lcdc_clock_enable();
}
void hal_cmu_csi_clock_enable(void)
{
    cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_APCLK, 0x0);
    aoncmu->MIPI_CLK = AON_CMU_EN_CLK_PIX_CSI | AON_CMU_POL_CLK_CSI_IN |
        SET_BITFIELD(aoncmu->MIPI_CLK, AON_CMU_CFG_DIV_PIX_CSI, 0x7); //0xa:240M 1lane
    cmu->APCLK_ENABLE = SYS_APCLK_CSI;
    cmu->XCLK_ENABLE = SYS_XCLK_CSI;
    cmu->QCLK_ENABLE = SYS_QCLK_CSI_LANE | SYS_QCLK_CSI_PIX | SYS_QCLK_CSI_LANG;

    hal_cmu_clock_set_mode(HAL_CMU_MOD_Q_CSI_LANE, HAL_CMU_CLK_AUTO);
    hal_cmu_clock_set_mode(HAL_CMU_MOD_Q_CSI_PIX, HAL_CMU_CLK_AUTO);
    hal_cmu_clock_set_mode(HAL_CMU_MOD_Q_CSI_LANG, HAL_CMU_CLK_AUTO);
}

void hal_cmu_csi_clock_disable(void)
{
    cmu->XCLK_DISABLE = SYS_XCLK_CSI;
    cmu->QCLK_ENABLE = SYS_QCLK_CSI_LANE | SYS_QCLK_CSI_PIX | SYS_QCLK_CSI_LANG;
    cmu->APCLK_DISABLE = SYS_APCLK_CSI;
    aoncmu->MIPI_CLK &= ~(AON_CMU_EN_CLK_PIX_CSI | AON_CMU_POL_CLK_CSI_IN);
}

void hal_cmu_csi_reset_set(void)
{
    cmu->XRESET_SET = SYS_XRST_CSI;
    cmu->QRESET_SET = SYS_QRST_CSI_LANE | SYS_QRST_CSI_PIX | SYS_QRST_CSI_LANG;
    cmu->APRESET_SET = SYS_APRST_CSI;
}

void hal_cmu_csi_reset_clear(void)
{
    cmu->APRESET_CLR = SYS_APRST_CSI;
    cmu->XRESET_CLR = SYS_XRST_CSI;
    cmu->QRESET_CLR = SYS_QRST_CSI_LANE | SYS_QRST_CSI_PIX | SYS_QRST_CSI_LANG;
    hal_sys_timer_delay_us(10);
}

#ifdef LOW_SYS_FREQ
#ifdef LOW_SYS_FREQ_6P5M
int hal_cmu_fast_timer_offline(void)
{
    return (cmu_sys_freq == HAL_CMU_FREQ_6P5M);
}
#endif
#endif

#ifdef CHIP_BEST2003_DSP

int hal_cmu_sys_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_dsp_set_freq")));

enum HAL_CMU_FREQ_T hal_cmu_sys_get_freq(void) __attribute__((alias("hal_cmu_dsp_get_freq")));

#else // !CHIP_BEST2003_DSP

int hal_cmu_sys_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t enable;
    uint32_t disable;
    int div;
    uint32_t lock;

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }

#ifdef LOW_SYS_FREQ
    cmu_sys_freq = freq;
#endif

    div = -1;

    switch (freq) {
    case HAL_CMU_FREQ_32K:
        enable = 0;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_SLOW_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_6P5M:
#if defined(LOW_SYS_FREQ) && defined(LOW_SYS_FREQ_6P5M)
        enable = CMU_SEL_MCU_OSC_4_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_SLOW_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_13M:
#ifdef LOW_SYS_FREQ
        enable = CMU_SEL_MCU_OSC_2_ENABLE | CMU_SEL_MCU_SLOW_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_26M:
        enable = CMU_SEL_MCU_SLOW_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_2_ENABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_52M:
        enable = CMU_SEL_MCU_FAST_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_78M:
    case HAL_CMU_FREQ_104M:
#ifdef OSC_26M_X4_AUD2BB
        enable = CMU_SEL_MCU_OSCX4_ENABLE | CMU_SEL_MCU_FAST_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_156M:
    case HAL_CMU_FREQ_208M:
    case HAL_CMU_FREQ_260M:
        enable = CMU_RSTN_DIV_MCU_ENABLE | CMU_SEL_MCU_PLL_ENABLE;
        disable = CMU_BYPASS_DIV_MCU_DISABLE;
        if (0) {
#ifndef OSC_26M_X4_AUD2BB
        } else if (freq <= HAL_CMU_FREQ_78M) {
            div = 3;
        } else if (freq <= HAL_CMU_FREQ_104M) {
            div = 2;
#endif
        } else if (freq <= HAL_CMU_FREQ_156M) {
            div = 1;
        } else { // 208M or 260M
            div = 0;
        }
        break;
    case HAL_CMU_FREQ_390M:
    case HAL_CMU_FREQ_780M:
    default:
        enable = CMU_BYPASS_DIV_MCU_ENABLE | CMU_SEL_MCU_PLL_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE;
        break;
    };

    if (div >= 0) {
        lock = int_lock();
        cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_MCU, div);
        int_unlock(lock);
    }

    if (enable & CMU_SEL_MCU_PLL_ENABLE) {
        cmu->SYS_CLK_ENABLE = CMU_RSTN_DIV_MCU_ENABLE;
        if (enable & CMU_BYPASS_DIV_MCU_ENABLE) {
            cmu->SYS_CLK_ENABLE = CMU_BYPASS_DIV_MCU_ENABLE;
        } else {
            cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_MCU_DISABLE;
        }
    }
    cmu->SYS_CLK_ENABLE = enable;
    if (enable & CMU_SEL_MCU_PLL_ENABLE) {
        cmu->SYS_CLK_DISABLE = disable;
    } else {
        cmu->SYS_CLK_DISABLE = disable & ~(CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE);
        cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_MCU_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_MCU_DISABLE;
    }

    return 0;
}

enum HAL_CMU_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_sys_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = cmu->SYS_CLK_ENABLE;

    if (sys_clk & CMU_SEL_MCU_PLL_ENABLE) {
        if (sys_clk & CMU_BYPASS_DIV_MCU_ENABLE) {
            // 384M
            return HAL_CMU_FREQ_390M;
        } else {
            div = GET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_MCU);
            if (div == 0) {
                // 192M
                return HAL_CMU_FREQ_208M;
            } else if (div == 1) {
                // 128M
                return HAL_CMU_FREQ_156M;
            } else if (div == 2) {
                // 96M
                return HAL_CMU_FREQ_104M;
            } else { // div == 3
                // 76.8M
                return HAL_CMU_FREQ_78M;
            }
        }
    } else if (sys_clk & CMU_SEL_MCU_FAST_ENABLE) {
        if (sys_clk & CMU_SEL_MCU_OSCX4_ENABLE) {
            return HAL_CMU_FREQ_104M;
        } else {
            return HAL_CMU_FREQ_52M;
        }
    } else if (sys_clk & CMU_SEL_MCU_SLOW_ENABLE) {
        return HAL_CMU_FREQ_26M;
    } else {
        return HAL_CMU_FREQ_32K;
    }
}

#endif // !CHIP_BEST2003_DSP

int hal_cmu_dsp_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t enable;
    uint32_t disable;
    int div;
    uint32_t lock;

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }

#ifdef CHIP_BEST2003_DSP
#ifdef LOW_SYS_FREQ
    cmu_sys_freq = freq;
#endif
#endif

    div = -1;

    switch (freq) {
    case HAL_CMU_FREQ_32K:
        enable = 0;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_4_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_SLOW_DISABLE | CMU_SEL_A7_FAST_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_6P5M:
#if defined(LOW_SYS_FREQ) && defined(LOW_SYS_FREQ_6P5M)
        enable = CMU_SEL_A7_OSC_4_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_SLOW_DISABLE | CMU_SEL_A7_FAST_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_13M:
#ifdef LOW_SYS_FREQ
        enable = CMU_SEL_A7_OSC_2_ENABLE | CMU_SEL_A7_SLOW_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_FAST_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_26M:
        enable = CMU_SEL_A7_SLOW_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_FAST_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_52M:
        enable = CMU_SEL_A7_FAST_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_78M:
    case HAL_CMU_FREQ_104M:
#ifdef OSC_26M_X4_AUD2BB
        enable = CMU_SEL_A7_OSCX4_ENABLE | CMU_SEL_A7_FAST_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_156M:
    case HAL_CMU_FREQ_208M:
    case HAL_CMU_FREQ_260M:
    case HAL_CMU_FREQ_390M:
        enable = CMU_RSTN_DIV_A7_ENABLE | CMU_SEL_A7_PLL_ENABLE;
        disable = CMU_BYPASS_DIV_A7_DISABLE;
        if (0) {
        } else if (freq <= HAL_CMU_FREQ_156M) {
            div = 3;
        } else if (freq <= HAL_CMU_FREQ_208M) {
            div = 2;
        } else if (freq <= HAL_CMU_FREQ_260M) {
            div = 1;
        } else { // 390M
            div = 0;
        }
        break;
    case HAL_CMU_FREQ_780M:
    default:
        enable = CMU_BYPASS_DIV_A7_ENABLE | CMU_SEL_A7_PLL_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE;
        break;
    };

    if (div >= 0) {
        lock = int_lock();
        cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_A7, div);
        int_unlock(lock);
    }

    if (enable & CMU_SEL_A7_PLL_ENABLE) {
        cmu->SYS_CLK_ENABLE = CMU_RSTN_DIV_A7_ENABLE;
        if (enable & CMU_BYPASS_DIV_A7_ENABLE) {
            cmu->SYS_CLK_ENABLE = CMU_BYPASS_DIV_A7_ENABLE;
        } else {
            cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_A7_DISABLE;
        }
    }
    cmu->SYS_CLK_ENABLE = enable;
    if (enable & CMU_SEL_A7_PLL_ENABLE) {
        cmu->SYS_CLK_DISABLE = disable;
    } else {
        cmu->SYS_CLK_DISABLE = disable & ~(CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE);
        cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_A7_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_A7_DISABLE;
    }

    return 0;
}

enum HAL_CMU_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_dsp_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = cmu->SYS_CLK_ENABLE;

    if (sys_clk & CMU_SEL_A7_PLL_ENABLE) {
        if (sys_clk & CMU_BYPASS_DIV_A7_ENABLE) {
            return HAL_CMU_FREQ_780M;
        } else {
            div = GET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_A7);
            if (div == 0) {
                return HAL_CMU_FREQ_390M;
            } else if (div == 1) {
                return HAL_CMU_FREQ_260M;
            } else if (div == 2) {
                return HAL_CMU_FREQ_208M;
            } else { // div == 3
                return HAL_CMU_FREQ_156M;
            }
        }
    } else if (sys_clk & CMU_SEL_A7_FAST_ENABLE) {
        if (sys_clk & CMU_SEL_A7_OSCX4_ENABLE) {
            return HAL_CMU_FREQ_104M;
        } else {
            return HAL_CMU_FREQ_52M;
        }
    } else if (sys_clk & CMU_SEL_A7_SLOW_ENABLE) {
        return HAL_CMU_FREQ_26M;
    } else {
        return HAL_CMU_FREQ_32K;
    }
}

int BOOT_TEXT_SRAM_LOC hal_cmu_flash_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int BOOT_TEXT_SRAM_LOC hal_cmu_flash1_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int hal_cmu_mem_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int hal_cmu_dsp_select_pll(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint32_t set;
    uint32_t clr;

    if (pll == HAL_CMU_PLL_USB) {
        set = AON_CMU_SEL_A7_PLLUSB | AON_CMU_SEL_A7_PLLBB;
        clr = 0;
    } else if (pll == HAL_CMU_PLL_DSP) {
        set = 0;
        clr = AON_CMU_SEL_A7_PLLUSB | AON_CMU_SEL_A7_PLLBB;
    } else if (pll == HAL_CMU_PLL_BB) {
        set = AON_CMU_SEL_A7_PLLBB;
        clr = AON_CMU_SEL_A7_PLLUSB;
    } else {
        // == HAL_CMU_PLL_DDR or == HAL_CMU_PLL_BB_PSRAM or >= HAL_CMU_PLL_QTY
        return 1;
    }

    lock = int_lock();
    aoncmu->DSP_PLL_SELECT = (aoncmu->DSP_PLL_SELECT & ~clr) | set;
    int_unlock(lock);

    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_sys_select_pll(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint32_t set;
    uint32_t clr;

    if (pll == HAL_CMU_PLL_USB) {
        set = AON_CMU_SEL_MCU_PLLUSB | AON_CMU_SEL_MCU_PLLA7USB;
        clr = AON_CMU_SEL_MCU_PLLBB_PS;
    } else if (pll == HAL_CMU_PLL_DSP) {
        set = AON_CMU_SEL_MCU_PLLA7USB;
        clr = AON_CMU_SEL_MCU_PLLBB_PS | AON_CMU_SEL_MCU_PLLUSB;
    } else if (pll == HAL_CMU_PLL_BB) {
        set = 0;
        clr = AON_CMU_SEL_MCU_PLLBB_PS | AON_CMU_SEL_MCU_PLLUSB | AON_CMU_SEL_MCU_PLLA7USB;
    } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
        set = AON_CMU_SEL_MCU_PLLBB_PS;
        clr = AON_CMU_SEL_MCU_PLLUSB | AON_CMU_SEL_MCU_PLLA7USB;
    } else {
        // == HAL_CMU_PLL_DDR or >= HAL_CMU_PLL_QTY
        return 1;
    }

    lock = int_lock();
    aoncmu->FLS_PSR_CLK = (aoncmu->FLS_PSR_CLK & ~clr) | set;
    int_unlock(lock);

    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_audio_select_pll(enum HAL_CMU_PLL_T pll)
{
    if (pll == HAL_CMU_PLL_BB) {
        aoncmu->PCM_I2S_CLK &= ~AON_CMU_SEL_AUD_PLLUSB;
    } else {
        aoncmu->PCM_I2S_CLK |= AON_CMU_SEL_AUD_PLLUSB;
    }

    return 0;
}

int hal_cmu_get_pll_status(enum HAL_CMU_PLL_T pll)
{
    bool en;

    if (pll == HAL_CMU_PLL_USB) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE);
    } else if (pll == HAL_CMU_PLL_DDR) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLDDR_ENABLE);
    } else if (pll == HAL_CMU_PLL_DSP) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLA7_ENABLE);
    } else if (pll == HAL_CMU_PLL_BB) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLBB_ENABLE);
    } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLBB_PS_ENABLE);
    } else {
        en = false;
    }

    return en;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t pu_val;
    uint32_t en_val;
    uint32_t lock;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    if (pll == HAL_CMU_PLL_USB) {
        pu_val = AON_CMU_PU_PLLDSI_ENABLE | AON_CMU_PU_PLLDSI_DIV_PS_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
    } else if (pll == HAL_CMU_PLL_DDR) {
        pu_val = AON_CMU_PU_PLLDDR_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLDDR_ENABLE;
    } else if (pll == HAL_CMU_PLL_DSP) {
        pu_val = AON_CMU_PU_PLLA7_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLA7_ENABLE;
    } else if (pll == HAL_CMU_PLL_BB) {
#ifdef BBPLL_USE_DSI_MCUPLL
        pu_val = AON_CMU_PU_PLLDSI_ENABLE | AON_CMU_PU_PLLDSI_DIV_MCU_ENABLE;
#else
        pu_val = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLBB_DIV_MCU_ENABLE;
#endif
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_ENABLE;
    } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
#ifdef BB_PSRAMPLL_USE_DSI_PSRAMPLL
        pu_val = AON_CMU_PU_PLLDSI_ENABLE | AON_CMU_PU_PLLDSI_DIV_PS_ENABLE;
#else
        pu_val = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLBB_DIV_PS_ENABLE;
#endif
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_PS_ENABLE;
    } else if (pll == HAL_CMU_PLL_DSI) {
        pu_val = AON_CMU_PU_PLLDSI_ENABLE;
        en_val = 0;
    } else {
        pu_val = 0;
        en_val = 0;
    }

    lock = int_lock();

    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
#ifndef ROM_BUILD
        pmu_pll_div_reset_set(pll);
#endif
        aoncmu->PLL_ENABLE = pu_val;
#ifndef ROM_BUILD
        hal_sys_timer_delay_us(20);
        pmu_pll_div_reset_clear(pll);
        // Wait at least 10us for clock ready
        hal_sys_timer_delay_us(10);
#endif
        aoncmu->TOP_CLK_ENABLE = en_val;
    }
    if (user < HAL_CMU_PLL_USER_QTY) {
        pll_user_map[pll] |= (1 << user);
    } else if (user == HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] |= ((1 << user) - 1);
    }
    int_unlock(lock);

    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t pu_val;
    uint32_t en_val;
    uint32_t lock;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    if (pll == HAL_CMU_PLL_USB) {
        pu_val = 0;
        en_val = AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE;
    } else if (pll == HAL_CMU_PLL_DDR) {
        pu_val = AON_CMU_PU_PLLDDR_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLDDR_DISABLE;
    } else if (pll == HAL_CMU_PLL_DSP) {
        pu_val = AON_CMU_PU_PLLA7_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLA7_DISABLE;
    } else if (pll == HAL_CMU_PLL_BB) {
#ifdef BBPLL_USE_DSI_MCUPLL
        pu_val = AON_CMU_PU_PLLDSI_DIV_MCU_DISABLE;
#else
        pu_val = AON_CMU_PU_PLLBB_DIV_MCU_DISABLE;
#endif
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_DISABLE;
    } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
#ifdef BB_PSRAMPLL_USE_DSI_PSRAMPLL
        pu_val = AON_CMU_PU_PLLDSI_DIV_PS_DISABLE;
#else
        pu_val = AON_CMU_PU_PLLBB_DIV_PS_DISABLE;
#endif
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_PS_DISABLE;
    } else if (pll == HAL_CMU_PLL_DSI) {
        pu_val = 0;
        en_val = 0;
    } else {
        pu_val = 0;
        en_val = 0;
    }

    lock = int_lock();
    if (user < HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] &= ~(1 << user);
    } else if (user == HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] = 0;
    }
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
#if defined(BBPLL_USE_DSI_MCUPLL) && defined(BB_PSRAMPLL_USE_DSI_PSRAMPLL)
        if (pll_user_map[HAL_CMU_PLL_BB] == 0 && pll_user_map[HAL_CMU_PLL_BB_PSRAM] == 0 &&
            pll_user_map[HAL_CMU_PLL_USB] == 0 && pll_user_map[HAL_CMU_PLL_DSI] == 0)
            pu_val |= AON_CMU_PU_PLLBB_DISABLE;
#elif defined(BBPLL_USE_DSI_MCUPLL) && !defined(BB_PSRAMPLL_USE_DSI_PSRAMPLL)
        if (pll == HAL_CMU_PLL_BB || pll == HAL_CMU_PLL_USB || pll == HAL_CMU_PLL_DSI) {
            if (pll_user_map[HAL_CMU_PLL_BB] == 0 && pll_user_map[HAL_CMU_PLL_USB] == 0 &&
                    pll_user_map[HAL_CMU_PLL_DSI] == 0)
                pu_val |= AON_CMU_PU_PLLDSI_DISABLE;
        } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
            pu_val |= AON_CMU_PU_PLLBB_DISABLE;
        }
#elif !defined(BBPLL_USE_DSI_MCUPLL) && defined(BB_PSRAMPLL_USE_DSI_PSRAMPLL)
        if (pll == HAL_CMU_PLL_BB_PSRAM || pll == HAL_CMU_PLL_USB || pll == HAL_CMU_PLL_DSI) {
            if (pll_user_map[HAL_CMU_PLL_BB_PSRAM] == 0 && pll_user_map[HAL_CMU_PLL_USB] == 0 &&
                    pll_user_map[HAL_CMU_PLL_DSI] == 0)
                pu_val |= AON_CMU_PU_PLLDSI_DISABLE;
        } else if (pll == HAL_CMU_PLL_BB) {
            pu_val |= AON_CMU_PU_PLLBB_DISABLE;
        }
#else
        if (pll == HAL_CMU_PLL_USB || pll == HAL_CMU_PLL_DSI) {
            if (pll_user_map[HAL_CMU_PLL_USB] == 0 && pll_user_map[HAL_CMU_PLL_DSI] == 0)
                pu_val |= AON_CMU_PU_PLLDSI_DISABLE;
        } else if (pll == HAL_CMU_PLL_BB || pll == HAL_CMU_PLL_BB_PSRAM) {
            if (pll_user_map[HAL_CMU_PLL_BB] == 0 && pll_user_map[HAL_CMU_PLL_BB_PSRAM] == 0)
                pu_val |= AON_CMU_PU_PLLBB_DISABLE;
        }
#endif
        aoncmu->TOP_CLK_DISABLE = en_val;
        aoncmu->PLL_DISABLE = pu_val;
    }
    int_unlock(lock);

    return 0;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_low_freq_mode_init(void)
{
}

void hal_cmu_low_freq_mode_enable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_FREQ_T switch_freq;

    // TODO: Select the PLL used by sys
    pll = HAL_CMU_PLL_BB;

#ifdef OSC_26M_X4_AUD2BB
    switch_freq = HAL_CMU_FREQ_104M;
#else
    switch_freq = HAL_CMU_FREQ_52M;
#endif

    if (old_freq > switch_freq && new_freq <= switch_freq) {
        hal_cmu_pll_disable(pll, HAL_CMU_PLL_USER_SYS);
    }
}

void hal_cmu_low_freq_mode_disable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_FREQ_T switch_freq;

    // TODO: Select the PLL used by sys
    pll = HAL_CMU_PLL_BB;

#ifdef OSC_26M_X4_AUD2BB
    switch_freq = HAL_CMU_FREQ_104M;
#else
    switch_freq = HAL_CMU_FREQ_52M;
#endif

    if (old_freq <= switch_freq && new_freq > switch_freq) {
        hal_cmu_pll_enable(pll, HAL_CMU_PLL_USER_SYS);
    }
}

void hal_cmu_rom_enable_pll(void)
{
    hal_cmu_sys_select_pll(HAL_CMU_PLL_BB);
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_SYS);

#if defined(PSRAM_ENABLE) && !defined(PSRAM_LOW_SPEED)
    hal_cmu_pll_enable(HAL_CMU_PLL_BB_PSRAM, HAL_CMU_PLL_USER_PSRAM);
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_sram_init()
{
    uint32_t mcu_ram_size;

#if defined(ARM_CMSE)
    mcu_ram_size = RAM5_BASE + RAM5_SIZE - RAM0_BASE;
#elif defined(ARM_CMNS)
    mcu_ram_size = RAM_SIZE + RAM_S_SIZE;
#else
    mcu_ram_size = RAM_SIZE;
#endif
#if defined(CHIP_HAS_CP) && ((RAMCP_SIZE > 0) || (RAMCPX_SIZE > 0))
    mcu_ram_size += (RAMCP_SIZE + RAMCPX_SIZE);
#endif
#ifdef __BT_RAMRUN__
    mcu_ram_size += BT_RAMRUN_SIZE;
#endif

    switch (mcu_ram_size) {
    case RAM1_BASE - RAM0_BASE: //RAM0
        cmu->WAKEUP_CLK_CFG = SET_BITFIELD(cmu->WAKEUP_CLK_CFG, CMU_CFG_SRAM_IN_M33, 0x0);
        break;
    case RAM2_BASE - RAM0_BASE: //RAM0,1
        cmu->WAKEUP_CLK_CFG = SET_BITFIELD(cmu->WAKEUP_CLK_CFG, CMU_CFG_SRAM_IN_M33, 0x1);
        break;
    case RAM3_BASE - RAM0_BASE: //RAM0,1,2
        cmu->WAKEUP_CLK_CFG = SET_BITFIELD(cmu->WAKEUP_CLK_CFG, CMU_CFG_SRAM_IN_M33, 0x3);
        break;
    case RAM4_BASE - RAM0_BASE: //RAM0,1,2,3
        cmu->WAKEUP_CLK_CFG = SET_BITFIELD(cmu->WAKEUP_CLK_CFG, CMU_CFG_SRAM_IN_M33, 0x7);
        break;
    case RAM5_BASE - RAM0_BASE: //RAM0,1,2,3,4
        cmu->WAKEUP_CLK_CFG = SET_BITFIELD(cmu->WAKEUP_CLK_CFG, CMU_CFG_SRAM_IN_M33, 0xF);
        break;
    case (RAM5_BASE + RAM5_SIZE - RAM0_BASE): //RAM0,1,2,3,4,5
    default:
        cmu->WAKEUP_CLK_CFG = SET_BITFIELD(cmu->WAKEUP_CLK_CFG, CMU_CFG_SRAM_IN_M33, 0x1F);
        break;
    }
}

void hal_cmu_programmer_enable_pll(void)
{
    hal_cmu_sram_init();
    hal_cmu_dma_req_init();

    hal_cmu_flash_select_pll(HAL_CMU_PLL_BB);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_BB);
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_SYS);

#if defined(PSRAM_ENABLE) && !defined(PSRAM_LOW_SPEED)
    hal_cmu_pll_enable(HAL_CMU_PLL_BB_PSRAM, HAL_CMU_PLL_USER_PSRAM);
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_init_pll_selection(void)
{
    enum HAL_CMU_PLL_T sys;

#if !defined(ARM_CMNS)
    // Disable the PLL which might be enabled in ROM
    hal_cmu_pll_disable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_ALL);
#endif

#ifdef BBPLL_USE_DSI_MCUPLL
    aoncmu->FLS_PSR_CLK |= AON_CMU_SEL_MCU_PLLDSI;
#endif

#ifdef BB_PSRAMPLL_USE_DSI_PSRAMPLL
    aoncmu->FLS_PSR_CLK |= AON_CMU_SEL_PSR_PLLDSI;
#endif

#ifdef SYS_USE_USBPLL
    sys = HAL_CMU_PLL_USB;
#elif defined(SYS_USE_BB_PSRAMPLL)
    sys = HAL_CMU_PLL_BB_PSRAM;
#elif defined(SYS_USE_DSPPLL)
    sys = HAL_CMU_PLL_DSP;
#else
    sys = HAL_CMU_PLL_BB;
#endif
    hal_cmu_sys_select_pll(sys);

#ifdef AUDIO_USE_BBPLL
    hal_cmu_audio_select_pll(HAL_CMU_PLL_BB);
#else
    hal_cmu_audio_select_pll(HAL_CMU_PLL_USB);
#endif

#ifndef ULTRA_LOW_POWER
    hal_cmu_pll_enable(sys, HAL_CMU_PLL_USER_SYS);
#endif
#if !(defined(FLASH_LOW_SPEED) || defined(OSC_26M_X4_AUD2BB))
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_FLASH);
#endif
#if defined(PSRAM_ENABLE) && !defined(PSRAM_LOW_SPEED)
    hal_cmu_pll_enable(HAL_CMU_PLL_BB_PSRAM, HAL_CMU_PLL_USER_PSRAM);
#endif

    hal_cmu_dsp_timer0_select_slow();
#ifdef TIMER1_BASE
    hal_cmu_dsp_timer1_select_fast();
#endif
}

#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
void hal_cmu_audio_26m_x4_enable(enum CMU_AUD_26M_X4_USER_T user)
{
    uint32_t lock;

    if (user >= CMU_AUD_26M_X4_USER_QTY) {
        return;
    }

    lock = int_lock();

    if (aud_26m_x4_map == 0) {
        aoncmu->PCM_I2S_CLK |= AON_CMU_SEL_AUD_X4;
    }
    aud_26m_x4_map |= (1 << user);

    int_unlock(lock);
}

void hal_cmu_audio_26m_x4_disable(enum CMU_AUD_26M_X4_USER_T user)
{
    uint32_t lock;

    if (user >= CMU_AUD_26M_X4_USER_QTY) {
        return;
    }

    lock = int_lock();

    if (aud_26m_x4_map & (1 << user)) {
        aud_26m_x4_map &= ~(1 << user);
        if (aud_26m_x4_map == 0) {
            aoncmu->PCM_I2S_CLK &= ~AON_CMU_SEL_AUD_X4;
        }
    }

    int_unlock(lock);
}
#endif

void hal_cmu_codec_iir_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;
    uint32_t cfg_speed = 0;

    mask = AON_CMU_SEL_CODECIIR_OSC | AON_CMU_SEL_CODECIIR_OSCX2 | AON_CMU_BYPASS_DIV_CODECIIR;
    val = 0;

    if (speed <= 26000000) {
        val |= AON_CMU_SEL_CODECIIR_OSC | AON_CMU_SEL_CODECIIR_OSCX2;
        cfg_speed = 26000000;
    } else if (speed <= 52000000) {
        val |= AON_CMU_SEL_CODECIIR_OSCX2;
        cfg_speed = 52000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (hal_cmu_get_audio_resample_status()) {
            hal_cmu_audio_26m_x4_enable(CMU_AUD_26M_X4_USER_IIR);
            val |= AON_CMU_BYPASS_DIV_CODECIIR;
            cfg_speed = 104000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = HAL_CMU_AUD_PLL_CLOCK / speed;
            if (div >= 2) {
                hal_cmu_codec_iir_set_div(div);
                cfg_speed = HAL_CMU_AUD_PLL_CLOCK / div;
            } else {
                val |= AON_CMU_BYPASS_DIV_CODECIIR;
                cfg_speed = HAL_CMU_AUD_PLL_CLOCK;
            }
            analog_aud_freq_pll_config(CODEC_FREQ_48K_SERIES, CODEC_PLL_DIV);
            analog_aud_pll_open(ANA_AUD_PLL_USER_IIR);
        }

       //pmu_iir_freq_config(cfg_speed);
    }

    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    lock = int_lock();
    aoncmu->CODEC_IIR = (aoncmu->CODEC_IIR & ~mask) | val;
    int_unlock(lock);

    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_CODECIIR_ENABLE;

    aocmu_reg_update_wait();
}

void hal_cmu_codec_iir_disable(void)
{
    uint32_t lock;
    uint32_t val;
    bool high_speed;

    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODECIIR_DISABLE;

#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
    hal_cmu_audio_26m_x4_disable(CMU_AUD_26M_X4_USER_IIR);
#endif

    high_speed = !(aoncmu->CODEC_IIR & AON_CMU_SEL_CODECIIR_OSC);

    val = AON_CMU_SEL_CODECIIR_OSC | AON_CMU_SEL_CODECIIR_OSCX2;

    lock = int_lock();
    aoncmu->CODEC_IIR |= val;
    int_unlock(lock);

    if (high_speed) {
        //pmu_iir_freq_config(0);
    }
    analog_aud_pll_close(ANA_AUD_PLL_USER_IIR);
}

int hal_cmu_codec_iir_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    aoncmu->CODEC_IIR = SET_BITFIELD(aoncmu->CODEC_IIR, AON_CMU_CFG_DIV_CODECIIR, div);
    int_unlock(lock);

    return 0;
}

void hal_cmu_codec_rs_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;
    uint32_t cfg_speed = 0;

    mask = AON_CMU_SEL_CODECRS0_OSC | AON_CMU_SEL_CODECRS0_OSCX2 | AON_CMU_BYPASS_DIV_CODECRS0;
    mask |= AON_CMU_SEL_CODECRS1_OSC | AON_CMU_SEL_CODECRS1_OSCX2 | AON_CMU_BYPASS_DIV_CODECRS1;
    val = 0;

    if (speed <= 26000000) {
        val |= AON_CMU_SEL_CODECRS0_OSC | AON_CMU_SEL_CODECRS0_OSCX2;
        val |= AON_CMU_SEL_CODECRS1_OSC | AON_CMU_SEL_CODECRS1_OSCX2;
        cfg_speed = 26000000;
    } else if (speed <= 52000000) {
        val |= AON_CMU_SEL_CODECRS0_OSCX2;
        val |= AON_CMU_SEL_CODECRS1_OSCX2;
        cfg_speed = 52000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (hal_cmu_get_audio_resample_status()) {
            hal_cmu_audio_26m_x4_enable(CMU_AUD_26M_X4_USER_RS);
            val |= AON_CMU_BYPASS_DIV_CODECRS0;
            val |= AON_CMU_BYPASS_DIV_CODECRS1;
            cfg_speed = 104000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = HAL_CMU_AUD_PLL_CLOCK / speed;
            if (div >= 2) {
                hal_cmu_codec_rs_set_div(div);
                cfg_speed = HAL_CMU_AUD_PLL_CLOCK / div;
            } else {
                val |= AON_CMU_BYPASS_DIV_CODECRS0;
                val |= AON_CMU_BYPASS_DIV_CODECRS1;
                cfg_speed = HAL_CMU_AUD_PLL_CLOCK;
            }
            analog_aud_freq_pll_config(CODEC_FREQ_48K_SERIES, CODEC_PLL_DIV);
            analog_aud_pll_open(ANA_AUD_PLL_USER_RS);
        }
    }

    ASSERT(speed <= cfg_speed, "%s: speed %u should <= cfg_speed %u", __func__, speed, cfg_speed);

    lock = int_lock();
    aoncmu->CODEC_IIR = (aoncmu->CODEC_IIR & ~mask) | val;
    int_unlock(lock);

    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_CODECRS0_ENABLE | AON_CMU_EN_CLK_CODECRS1_ENABLE;

    aocmu_reg_update_wait();
}

void hal_cmu_codec_rs_disable(void)
{
    uint32_t lock;
    //bool high_speed;

    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODECRS0_DISABLE | AON_CMU_EN_CLK_CODECRS1_DISABLE;

#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
    hal_cmu_audio_26m_x4_disable(CMU_AUD_26M_X4_USER_RS);
#endif

    lock = int_lock();
    aoncmu->CODEC_IIR |= AON_CMU_SEL_CODECRS0_OSC | AON_CMU_SEL_CODECRS0_OSCX2 | AON_CMU_SEL_CODECRS1_OSC | AON_CMU_SEL_CODECRS1_OSCX2;
    int_unlock(lock);

    analog_aud_pll_close(ANA_AUD_PLL_USER_RS);
}

int hal_cmu_codec_rs_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    aoncmu->CODEC_IIR = (aoncmu->CODEC_IIR & ~(AON_CMU_CFG_DIV_CODECRS0_MASK | AON_CMU_CFG_DIV_CODECRS1_MASK)) |
        AON_CMU_CFG_DIV_CODECRS0(div) | AON_CMU_CFG_DIV_CODECRS1(div);
    int_unlock(lock);

    return 0;
}

void hal_cmu_codec_clock_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CODEC_DIV = (aoncmu->CODEC_DIV & ~(AON_CMU_SEL_CODEC_OSC | AON_CMU_SEL_CODECHCLK_OSCX2 | AON_CMU_SEL_CODEC_ANA_2 | AON_CMU_SEL_CODECHCLK_PLL)) |
       AON_CMU_SEL_CODEC_OSC_2;
    int_unlock(lock);

    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_CODECHCLK_ENABLE | AON_CMU_EN_CLK_CODEC_ENABLE;
    hal_cmu_clock_enable(HAL_CMU_MOD_H_CODEC);
}

void hal_cmu_codec_clock_disable(void)
{
    hal_cmu_clock_disable(HAL_CMU_MOD_H_CODEC);
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODECHCLK_DISABLE | AON_CMU_EN_CLK_CODEC_DISABLE;
}

void hal_cmu_codec_vad_clock_enable(int type)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CODEC_DIV |= (AON_CMU_EN_VAD_IIR | AON_CMU_EN_VAD_RS);
    int_unlock(lock);

    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSC_ENABLE | AON_CMU_EN_CLK_VAD32K_ENABLE;
    if (type == AUD_VAD_TYPE_MIX) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODEC_DISABLE;
    }
}

void hal_cmu_codec_vad_clock_disable(int type)
{
    uint32_t lock;

    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_OSC_DISABLE | AON_CMU_EN_CLK_VAD32K_DISABLE;
    if (type == AUD_VAD_TYPE_MIX) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_CODEC_ENABLE;
    }

    lock = int_lock();
    aoncmu->CODEC_DIV &= ~(AON_CMU_EN_VAD_IIR | AON_CMU_EN_VAD_RS);
    int_unlock(lock);
}

void hal_cmu_codec_reset_set(void)
{
    aoncmu->SOFT_RSTN_SET = AON_CMU_SOFT_RSTN_CODEC_SET;
}

void hal_cmu_codec_reset_clear(void)
{
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CODEC);
    aoncmu->SOFT_RSTN_CLR = AON_CMU_SOFT_RSTN_CODEC_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_codec_set_fault_mask(uint32_t msk)
{
    uint32_t lock;

    lock = int_lock();
    // If bit set 1, DAC will be muted when some faults occur
    cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_MASK_OBS, msk);
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_EN_CLK_I2S0_OUT;
    } else {
        val = CMU_EN_CLK_I2S1_OUT;
    }

    lock = int_lock();
    cmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_EN_CLK_I2S0_OUT;
    } else {
        val = CMU_EN_CLK_I2S1_OUT;
    }

    lock = int_lock();
    cmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_set_slave_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_CLKIN;
    } else {
        val = CMU_SEL_I2S1_CLKIN;
    }

    lock = int_lock();
    cmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_set_master_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_CLKIN;
    } else {
        val = CMU_SEL_I2S1_CLKIN;
    }

    lock = int_lock();
    cmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;
    volatile uint32_t *reg;

    if (id == HAL_I2S_ID_0) {
        val = AON_CMU_EN_CLK_PLL_I2S0;
        reg = &aoncmu->PCM_I2S_CLK;
    } else {
        val = AON_CMU_EN_CLK_PLL_I2S1;
        reg = &aoncmu->SPDIF_CLK;
    }

    lock = int_lock();
    *reg |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;
    volatile uint32_t *reg;

    if (id == HAL_I2S_ID_0) {
        val = AON_CMU_EN_CLK_PLL_I2S0;
        reg = &aoncmu->PCM_I2S_CLK;
    } else {
        val = AON_CMU_EN_CLK_PLL_I2S1;
        reg = &aoncmu->SPDIF_CLK;
    }

    lock = int_lock();
    *reg &= ~val;
    int_unlock(lock);
}

int hal_cmu_i2s_set_div(enum HAL_I2S_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (AON_CMU_CFG_DIV_I2S0_MASK >> AON_CMU_CFG_DIV_I2S0_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    if (id == HAL_I2S_ID_0) {
        aoncmu->PCM_I2S_CLK = SET_BITFIELD(aoncmu->PCM_I2S_CLK, AON_CMU_CFG_DIV_I2S0, div);
    } else {
        aoncmu->SPDIF_CLK = SET_BITFIELD(aoncmu->SPDIF_CLK, AON_CMU_CFG_DIV_I2S1, div);
    }
    int_unlock(lock);

    return 0;
}

void hal_cmu_pcm_clock_out_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK |= CMU_EN_CLK_PCM_OUT;
    int_unlock(lock);
}

void hal_cmu_pcm_clock_out_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK &= ~CMU_EN_CLK_PCM_OUT;
    int_unlock(lock);
}

void hal_cmu_pcm_set_slave_mode(int clk_pol)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t cfg;

    mask = CMU_SEL_PCM_CLKIN | CMU_POL_CLK_PCM_IN;

    if (clk_pol) {
        cfg = CMU_SEL_PCM_CLKIN | CMU_POL_CLK_PCM_IN;
    } else {
        cfg = CMU_SEL_PCM_CLKIN;
    }

    lock = int_lock();
    cmu->I2C_CLK = (cmu->I2C_CLK & ~mask) | cfg;
    int_unlock(lock);
}

void hal_cmu_pcm_set_master_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK &= ~CMU_SEL_PCM_CLKIN;
    int_unlock(lock);
}

void hal_cmu_pcm_clock_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->PCM_I2S_CLK |= AON_CMU_EN_CLK_PLL_PCM;
    int_unlock(lock);
}

void hal_cmu_pcm_clock_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->PCM_I2S_CLK &= ~AON_CMU_EN_CLK_PLL_PCM;
    int_unlock(lock);
}

int hal_cmu_pcm_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (AON_CMU_CFG_DIV_PCM_MASK >> AON_CMU_CFG_DIV_PCM_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    aoncmu->PCM_I2S_CLK = SET_BITFIELD(aoncmu->PCM_I2S_CLK, AON_CMU_CFG_DIV_PCM, div);
    int_unlock(lock);
    return 0;
}

int hal_cmu_spdif_clock_enable(enum HAL_SPDIF_ID_T id)
{
    uint32_t lock;
    uint32_t mask;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    mask = AON_CMU_EN_CLK_PLL_SPDIF0;

    lock = int_lock();
    aoncmu->SPDIF_CLK |= mask;
    int_unlock(lock);
    return 0;
}

int hal_cmu_spdif_clock_disable(enum HAL_SPDIF_ID_T id)
{
    uint32_t lock;
    uint32_t mask;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    mask = AON_CMU_EN_CLK_PLL_SPDIF0;

    lock = int_lock();
    aoncmu->SPDIF_CLK &= ~mask;
    int_unlock(lock);

    return 0;
}

int hal_cmu_spdif_set_div(enum HAL_SPDIF_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    if (div < 2) {
        return 2;
    }

    div -= 2;
    if ((div & (AON_CMU_CFG_DIV_SPDIF0_MASK >> AON_CMU_CFG_DIV_SPDIF0_SHIFT)) != div) {
        return 2;
    }

    lock = int_lock();
    aoncmu->SPDIF_CLK = SET_BITFIELD(aoncmu->SPDIF_CLK, AON_CMU_CFG_DIV_SPDIF0, div);
    int_unlock(lock);
    return 0;
}

#ifdef CHIP_HAS_USB
void hal_cmu_usb_set_device_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->SYS_DIV |= CMU_USB_ID;
    int_unlock(lock);
}

void hal_cmu_usb_set_host_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->SYS_DIV &= ~CMU_USB_ID;
    int_unlock(lock);
}

#ifdef ROM_BUILD
enum HAL_CMU_USB_CLOCK_SEL_T hal_cmu_usb_rom_select_clock_source(int pll_en, unsigned int crystal)
{
    enum HAL_CMU_USB_CLOCK_SEL_T sel;

    if (crystal == 24000000) {
        sel = HAL_CMU_USB_CLOCK_SEL_24M_X2;
    } else {
        sel = HAL_CMU_USB_CLOCK_SEL_PLL;
    }

    hal_cmu_usb_rom_set_clock_source(sel);

    return sel;
}
void hal_cmu_usb_rom_set_clock_source(enum HAL_CMU_USB_CLOCK_SEL_T sel)
{
}
#endif

static uint32_t hal_cmu_usb_get_clock_source(void)
{
    uint32_t src;

#ifdef USB_HIGH_SPEED
    src = CMU_USB_CLK_SRC_PLL_60M;
#else
#ifndef USB_USE_USBPLL
    if (hal_cmu_get_crystal_freq() == 24000000)
        src = CMU_USB_CLK_SRC_OSC_24M_X2;
    else
#endif
        src = CMU_USB_CLK_SRC_PLL_48M;
#endif

    return src;
}

void hal_cmu_usb_clock_enable(void)
{
    enum HAL_CMU_PLL_T pll;
    uint32_t lock;
    uint32_t src;
    POSSIBLY_UNUSED uint32_t div;

#if !defined(USB_HIGH_SPEED) && defined(USB_USE_USBPLL)
    pll = HAL_CMU_PLL_USB;
#else
    pll = HAL_CMU_PLL_BB;
#endif
    src = hal_cmu_usb_get_clock_source();
    if (src != CMU_USB_CLK_SRC_OSC_24M_X2)
        hal_cmu_pll_enable(pll, HAL_CMU_PLL_USER_USB);

    lock = int_lock();
#ifdef USB_HIGH_SPEED
    cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_SEL_USB_SRC, 1);
#else
    cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_SEL_USB_SRC, 0);
    if (src == CMU_USB_CLK_SRC_OSC_24M_X2) {
        aoncmu->CLK_SELECT |= AON_CMU_SEL_USB_OSCX2;
    } else {
#ifdef USB_USE_USBPLL
        aoncmu->CLK_SELECT |= AON_CMU_SEL_USB_PLLUSB;
        // DSI PSRAMPLL to dig: ?
        div = 8;///TODO:
#else // USB usb BBPLL
        aoncmu->CLK_SELECT &= ~AON_CMU_SEL_USB_PLLUSB;
        // BBPLL to dig: 384M
        div = 8;
#endif
        aoncmu->CLK_SELECT = SET_BITFIELD(aoncmu->CLK_SELECT, AON_CMU_CFG_DIV_USB, div-2);
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_USB_PLL_ENABLE;
    }
#endif /*USB_HIGH_SPEED*/
    int_unlock(lock);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_USBC);
#ifdef USB_HIGH_SPEED
    hal_cmu_clock_enable(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_clock_enable(HAL_CMU_MOD_O_USB32K);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBC);
    hal_sys_timer_delay(US_TO_TICKS(60));
    hal_cmu_reset_clear(HAL_CMU_MOD_H_USBC);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_clear(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_clear(HAL_CMU_MOD_O_USB32K);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_USB);
}

void hal_cmu_usb_clock_disable(void)
{
    enum HAL_CMU_PLL_T pll;

#if !defined(USB_HIGH_SPEED) && defined(USB_USE_USBPLL)
    pll = HAL_CMU_PLL_USB;
#else
    pll = HAL_CMU_PLL_BB;
#endif

    hal_cmu_reset_set(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBC);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_USB);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_clock_disable(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_clock_disable(HAL_CMU_MOD_H_USBC);
#ifndef USB_HIGH_SPEED
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_USB_PLL_DISABLE;
#endif

    if (hal_cmu_usb_get_clock_source() != CMU_USB_CLK_SRC_OSC_24M_X2)
        hal_cmu_pll_disable(pll, HAL_CMU_PLL_USER_USB);
}
#endif

void BOOT_TEXT_FLASH_LOC hal_cmu_apb_init_div(void)
{
    // Divider defaults to 2 (div = reg_val + 2)
    //cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_PCLK, 0);
}

int hal_cmu_periph_set_div(uint32_t div)
{
    uint32_t lock;
    int ret = 0;

    if (div == 0 || div > ((AON_CMU_CFG_DIV_PER_MASK >> AON_CMU_CFG_DIV_PER_SHIFT) + 2)) {
        ret = 1;
    } else {
        lock = int_lock();
        if (div == 1) {
            aoncmu->CLK_SELECT |= AON_CMU_BYPASS_DIV_PER;
        } else {
            div -= 2;
            aoncmu->CLK_SELECT = (aoncmu->CLK_SELECT & ~(AON_CMU_CFG_DIV_PER_MASK | AON_CMU_BYPASS_DIV_PER)) |
                AON_CMU_CFG_DIV_PER(div);
        }
        int_unlock(lock);
    }

    return ret;
}

#define PERPH_SET_DIV_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_div(uint32_t div) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (div < 2 || div > ((CMU_CFG_DIV_ ##F## _MASK >> CMU_CFG_DIV_ ##F## _SHIFT) + 2)) { \
        cmu->r &= ~(CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F); \
        ret = 1; \
    } else { \
        div -= 2; \
        cmu->r = (cmu->r & ~(CMU_CFG_DIV_ ##F## _MASK)) | CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | \
            CMU_CFG_DIV_ ##F(div); \
        cmu->r |= CMU_EN_PLL_ ##F; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_DIV_FUNC(uart0, UART0, UART_CLK);
PERPH_SET_DIV_FUNC(uart1, UART1, UART_CLK);
PERPH_SET_DIV_FUNC(uart2, UART2, UART_CLK);
PERPH_SET_DIV_FUNC(uart3, UART3, UART_CLK);
PERPH_SET_DIV_FUNC(spi, SPI0, SYS_DIV);
PERPH_SET_DIV_FUNC(slcd, SPI1, SYS_DIV);
PERPH_SET_DIV_FUNC(sdmmc, SDMMC, PERIPH_CLK);
PERPH_SET_DIV_FUNC(i2c, I2C, I2C_CLK);

#define PERPH_SET_FREQ_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (freq == HAL_CMU_PERIPH_FREQ_26M) { \
        cmu->r &= ~(CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F); \
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) { \
        cmu->r = (cmu->r & ~(CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F)) | CMU_SEL_OSCX2_ ##F; \
    } else { \
        ret = 1; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_FREQ_FUNC(uart0, UART0, UART_CLK);
PERPH_SET_FREQ_FUNC(uart1, UART1, UART_CLK);
PERPH_SET_FREQ_FUNC(uart2, UART2, UART_CLK);
PERPH_SET_FREQ_FUNC(uart3, UART3, UART_CLK);
PERPH_SET_FREQ_FUNC(spi, SPI0, SYS_DIV);
PERPH_SET_FREQ_FUNC(slcd, SPI1, SYS_DIV);
PERPH_SET_FREQ_FUNC(sdmmc, SDMMC, PERIPH_CLK);
PERPH_SET_FREQ_FUNC(i2c, I2C, I2C_CLK);

int hal_cmu_ispi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq)
{
    uint32_t lock;
    int ret = 0;

    lock = int_lock();
    if (freq == HAL_CMU_PERIPH_FREQ_26M) {
        cmu->SYS_DIV &= ~CMU_SEL_OSCX2_SPI2;
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) {
        cmu->SYS_DIV |= CMU_SEL_OSCX2_SPI2;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

void hal_cmu_sec_eng_clock_enable(void)
{
    hal_cmu_clock_enable(HAL_CMU_MOD_H_SEC_ENG);
    hal_cmu_clock_enable(HAL_CMU_MOD_P_SEC_ENG);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_SEC_ENG);
    hal_cmu_reset_clear(HAL_CMU_MOD_P_SEC_ENG);
}

void hal_cmu_sec_eng_clock_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_SEC_ENG);
    hal_cmu_reset_set(HAL_CMU_MOD_P_SEC_ENG);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_SEC_ENG);
    hal_cmu_clock_disable(HAL_CMU_MOD_P_SEC_ENG);
}

int hal_cmu_clock_out_enable(enum HAL_CMU_CLOCK_OUT_ID_T id)
{
    uint32_t lock;
    uint32_t sel;
    uint32_t cfg;

    enum CMU_CLK_OUT_SEL_T {
        CMU_CLK_OUT_SEL_AON     = 0,
        CMU_CLK_OUT_SEL_CODEC   = 1,
        CMU_CLK_OUT_SEL_BT      = 2,
        CMU_CLK_OUT_SEL_MCU     = 3,
        CMU_CLK_OUT_SEL_WF      = 4,

        CMU_CLK_OUT_SEL_QTY
    };

    sel = CMU_CLK_OUT_SEL_QTY;
    cfg = 0;

    if (id <= HAL_CMU_CLOCK_OUT_AON_SYS) {
        sel = CMU_CLK_OUT_SEL_AON;
        cfg = id - HAL_CMU_CLOCK_OUT_AON_32K;
    } else if (HAL_CMU_CLOCK_OUT_MCU_32K <= id && id <= HAL_CMU_CLOCK_OUT_MCU_I2S1) {
        sel = CMU_CLK_OUT_SEL_MCU;
        lock = int_lock();
        cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_MCU_32K);
        int_unlock(lock);
    } else if (HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA <= id && id <= HAL_CMU_CLOCK_OUT_CODEC_HCLK) {
        sel = CMU_CLK_OUT_SEL_CODEC;
        hal_codec_select_clock_out(id - HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA);
    } else if (HAL_CMU_CLOCK_OUT_BT_NONE <= id && id <= HAL_CMU_CLOCK_OUT_BT_DACD8) {
        sel = CMU_CLK_OUT_SEL_BT;
        btcmu->CLK_OUT = SET_BITFIELD(btcmu->CLK_OUT, BT_CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_BT_NONE);
    } else if (HAL_CMU_CLOCK_OUT_WF_32K <= id && id <= HAL_CMU_CLOCK_OUT_WF_BBDIGFIFO) {
        sel = CMU_CLK_OUT_SEL_WF;
        wlancmu->CLK_OUT = SET_BITFIELD(wlancmu->CLK_OUT, WLAN_CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_WF_32K);
    }

    if (sel < CMU_CLK_OUT_SEL_QTY) {
        lock = int_lock();
        aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~(AON_CMU_SEL_CLK_OUT_MASK | AON_CMU_CFG_CLK_OUT_MASK)) |
            AON_CMU_SEL_CLK_OUT(sel) | AON_CMU_CFG_CLK_OUT(cfg) | AON_CMU_EN_CLK_OUT;
        int_unlock(lock);

        return 0;
    }

    return 1;
}

void hal_cmu_clock_out_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CLK_OUT &= ~AON_CMU_EN_CLK_OUT;
    int_unlock(lock);
}

int hal_cmu_i2s_mclk_enable(enum HAL_CMU_I2S_MCLK_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CODEC_DIV = SET_BITFIELD(aoncmu->CODEC_DIV, AON_CMU_SEL_I2S_MCLK, id) | AON_CMU_EN_I2S_MCLK;
    int_unlock(lock);

    return 0;
}

void hal_cmu_i2s_mclk_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CODEC_DIV &= ~AON_CMU_EN_I2S_MCLK;
    int_unlock(lock);
}

int hal_cmu_pwm_set_freq(enum HAL_PWM_ID_T id, uint32_t freq)
{
    uint32_t lock;
    int clk_32k;
    uint32_t div;

    if (id >= HAL_PWM_ID_QTY) {
        return 1;
    }

    if (freq == 0) {
        clk_32k = 1;
        div = 0;
    } else {
        clk_32k = 0;
        div = hal_cmu_get_crystal_freq() / freq;
        if (div < 2) {
            return -1;
        }

        div -= 2;
        if ((div & (AON_CMU_CFG_DIV_PWM0_MASK >> AON_CMU_CFG_DIV_PWM0_SHIFT)) != div) {
            return -2;
        }
    }

    lock = int_lock();
    if (id == HAL_PWM_ID_0) {
        aoncmu->PWM01_CLK = (aoncmu->PWM01_CLK & ~(AON_CMU_CFG_DIV_PWM0_MASK | AON_CMU_SEL_PWM0_OSC | AON_CMU_EN_CLK_PWM0_OSC)) |
            AON_CMU_CFG_DIV_PWM0(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM0_OSC | AON_CMU_EN_CLK_PWM0_OSC));
    } else if (id == HAL_PWM_ID_1) {
        aoncmu->PWM01_CLK = (aoncmu->PWM01_CLK & ~(AON_CMU_CFG_DIV_PWM1_MASK | AON_CMU_SEL_PWM1_OSC | AON_CMU_EN_CLK_PWM1_OSC)) |
            AON_CMU_CFG_DIV_PWM1(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM1_OSC | AON_CMU_EN_CLK_PWM1_OSC));
    } else if (id == HAL_PWM_ID_2) {
        aoncmu->PWM23_CLK = (aoncmu->PWM23_CLK & ~(AON_CMU_CFG_DIV_PWM2_MASK | AON_CMU_SEL_PWM2_OSC | AON_CMU_EN_CLK_PWM2_OSC)) |
            AON_CMU_CFG_DIV_PWM2(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM2_OSC | AON_CMU_EN_CLK_PWM2_OSC));
    } else if (id == HAL_PWM_ID_3) {
        aoncmu->PWM23_CLK = (aoncmu->PWM23_CLK & ~(AON_CMU_CFG_DIV_PWM3_MASK | AON_CMU_SEL_PWM3_OSC | AON_CMU_EN_CLK_PWM3_OSC)) |
            AON_CMU_CFG_DIV_PWM3(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM3_OSC | AON_CMU_EN_CLK_PWM3_OSC));
    } else if (id == HAL_PWM1_ID_0) {
        aoncmu->PWM45_CLK = (aoncmu->PWM45_CLK & ~(AON_CMU_CFG_DIV_PWM4_MASK | AON_CMU_SEL_PWM4_OSC | AON_CMU_EN_CLK_PWM4_OSC)) |
            AON_CMU_CFG_DIV_PWM4(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM4_OSC | AON_CMU_EN_CLK_PWM4_OSC));
    } else if (id == HAL_PWM1_ID_1) {
        aoncmu->PWM45_CLK = (aoncmu->PWM45_CLK & ~(AON_CMU_CFG_DIV_PWM5_MASK | AON_CMU_SEL_PWM5_OSC | AON_CMU_EN_CLK_PWM5_OSC)) |
            AON_CMU_CFG_DIV_PWM5(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM5_OSC | AON_CMU_EN_CLK_PWM5_OSC));
    } else if (id == HAL_PWM1_ID_2) {
        aoncmu->PWM67_CLK = (aoncmu->PWM67_CLK & ~(AON_CMU_CFG_DIV_PWM6_MASK | AON_CMU_SEL_PWM6_OSC | AON_CMU_EN_CLK_PWM6_OSC)) |
            AON_CMU_CFG_DIV_PWM6(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM6_OSC | AON_CMU_EN_CLK_PWM6_OSC));
    } else if (id == HAL_PWM1_ID_3) {
        aoncmu->PWM67_CLK = (aoncmu->PWM67_CLK & ~(AON_CMU_CFG_DIV_PWM7_MASK | AON_CMU_SEL_PWM7_OSC | AON_CMU_EN_CLK_PWM7_OSC)) |
            AON_CMU_CFG_DIV_PWM7(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM7_OSC | AON_CMU_EN_CLK_PWM7_OSC));
    } else {
        ASSERT(0,"PWM id error!");
    }
    int_unlock(lock);
    return 0;
}

void hal_cmu_jtag_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->MCU_TIMER &= ~(CMU_SECURE_BOOT_JTAG | CMU_SECURE_BOOT_I2C);
    int_unlock(lock);
}

void hal_cmu_jtag_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->MCU_TIMER |= (CMU_SECURE_BOOT_JTAG | CMU_SECURE_BOOT_I2C);
    int_unlock(lock);
}

void hal_cmu_jtag_clock_enable(void)
{
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_JTAG_ENABLE;
}

void hal_cmu_jtag_clock_disable(void)
{
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_JTAG_DISABLE;
}

void hal_cmu_jtag_set_cp(void)
{
    cmu->PERIPH_CLK |= CMU_JTAG_SEL_CP;
}

void hal_cmu_jtag_set_a7(void)
{
    cmu->PERIPH_CLK |= CMU_JTAG_SEL_A7;
}

void hal_cmu_rom_clock_init(void)
{
    aoncmu->CODEC_DIV = (aoncmu->CODEC_DIV & ~AON_CMU_SEL_AON_OSCX2) | AON_CMU_SEL_AON_OSC;
    // Enable PMU fast clock
    //aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~AON_CMU_SEL_DCDC_PLL) | AON_CMU_SEL_DCDC_OSC | AON_CMU_BYPASS_DIV_DCDC | AON_CMU_EN_CLK_DCDC0;

    hal_cmu_dma_req_init();

    // Debug Select CMU REG F4
    cmu->MCU_TIMER = SET_BITFIELD(cmu->MCU_TIMER, CMU_DEBUG_REG_SEL, CMU_DEBUG_REG_SEL_DEBUG);
}

void hal_cmu_init_chip_feature(uint16_t feature)
{
    aoncmu->CHIP_FEATURE = feature | AON_CMU_EFUSE_LOCK;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x2_enable(void)
{
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_8)
        aoncmu->CLK_OUT = AON_CMU_EN_CLK_DCDC0 | AON_CMU_CFG_DIV_DCDC(3);

    // Debug Select CMU REG F4
    cmu->MCU_TIMER = SET_BITFIELD(cmu->MCU_TIMER, CMU_DEBUG_REG_SEL, CMU_DEBUG_REG_SEL_DEBUG);
    // Power on OSCX2
    aoncmu->PLL_ENABLE = AON_CMU_PU_OSCX2_ENABLE;
    // Disable DIG OSCX2
    aoncmu->CLK_SELECT &= ~AON_CMU_SEL_OSCX2_DIG;
    // Enable OSCX2
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSCX2_ENABLE | AON_CMU_EN_CLK_MCU_OSCX2_ENABLE;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x4_enable(void)
{
#ifdef ANA_26M_X4_ENABLE
    // Power on OSCX4
    aoncmu->PLL_ENABLE = AON_CMU_PU_OSCX4_ENABLE;
    // Disable DIG OSCX4
    aoncmu->CLK_SELECT &= ~AON_CMU_SEL_OSCX4_DIG;
    // Enable OSCX4
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSCX4_ENABLE;
#endif
}

struct CMU_DMA_PER_2_REQ_T {
    enum HAL_DMA_PERIPH_T periph;
    enum CMU_DMA_REQ_T req;
};
const static BOOT_RODATA_FLASH_LOC struct CMU_DMA_PER_2_REQ_T periph_map[] = {
    {HAL_DMA_PERIPH_NULL,   CMU_DMA_REQ_NULL},
    {HAL_AUDMA_CODEC_RX,    CMU_DMA_REQ_CODEC_RX},
    {HAL_AUDMA_CODEC_TX,    CMU_DMA_REQ_CODEC_TX},
    {HAL_AUDMA_DSD_RX,      CMU_DMA_REQ_DSD_RX},
    {HAL_AUDMA_DSD_TX,      CMU_DMA_REQ_DSD_TX},
    {HAL_GPDMA_IR_RX,       CMU_DMA_REQ_IR_RX},
    {HAL_GPDMA_IR_TX,       CMU_DMA_REQ_IR_TX},
    {HAL_GPDMA_FLASH1,      CMU_DMA_REQ_FLS1},
    {HAL_GPDMA_FLASH0,      CMU_DMA_REQ_FLS0},
    {HAL_AUDMA_BTDUMP,      CMU_DMA_REQ_BTDUMP},
    {HAL_GPDMA_SDMMC,       CMU_DMA_REQ_SDEMMC},
    {HAL_GPDMA_I2C0_RX,     CMU_DMA_REQ_I2C0_RX},
    {HAL_GPDMA_I2C0_TX,     CMU_DMA_REQ_I2C0_TX},
    {HAL_GPDMA_I2C1_RX,     CMU_DMA_REQ_I2C1_RX},
    {HAL_GPDMA_I2C1_TX,     CMU_DMA_REQ_I2C1_TX},
    {HAL_GPDMA_I2C2_RX,     CMU_DMA_REQ_I2C2_RX},
    {HAL_GPDMA_I2C2_TX,     CMU_DMA_REQ_I2C2_TX},
    {HAL_GPDMA_SPI_RX,      CMU_DMA_REQ_SPILCD0_RX},
    {HAL_GPDMA_SPI_TX,      CMU_DMA_REQ_SPILCD0_TX},
    {HAL_GPDMA_SPILCD_RX,   CMU_DMA_REQ_SPILCD1_RX},
    {HAL_GPDMA_SPILCD_TX,   CMU_DMA_REQ_SPILCD1_TX},
    {HAL_GPDMA_ISPI_RX,     CMU_DMA_REQ_SPI_ITN_RX},
    {HAL_GPDMA_ISPI_TX,     CMU_DMA_REQ_SPI_ITN_TX},
    {HAL_GPDMA_UART0_RX,    CMU_DMA_REQ_UART0_RX},
    {HAL_GPDMA_UART0_TX,    CMU_DMA_REQ_UART0_TX},
    {HAL_GPDMA_UART1_RX,    CMU_DMA_REQ_UART1_RX},
    {HAL_GPDMA_UART1_TX,    CMU_DMA_REQ_UART1_TX},
    {HAL_GPDMA_UART2_RX,    CMU_DMA_REQ_UART2_RX},
    {HAL_GPDMA_UART2_TX,    CMU_DMA_REQ_UART2_TX},
    {HAL_GPDMA_UART3_RX,    CMU_DMA_REQ_UART3_RX},
    {HAL_GPDMA_UART3_TX,    CMU_DMA_REQ_UART3_TX},
    {HAL_AUDMA_BTPCM_RX,    CMU_DMA_REQ_PCM_RX},
    {HAL_AUDMA_BTPCM_TX,    CMU_DMA_REQ_PCM_TX},
    {HAL_AUDMA_I2S0_RX,     CMU_DMA_REQ_I2S0_RX},
    {HAL_AUDMA_I2S0_TX,     CMU_DMA_REQ_I2S0_TX},
    {HAL_AUDMA_I2S1_RX,     CMU_DMA_REQ_I2S1_RX},
    {HAL_AUDMA_I2S1_TX,     CMU_DMA_REQ_I2S1_TX},
    {HAL_AUDMA_SPDIF0_RX,   CMU_DMA_REQ_SPDIF0_RX},
    {HAL_AUDMA_SPDIF0_TX,   CMU_DMA_REQ_SPDIF0_TX},
};

enum CMU_DMA_REQ_T BOOT_TEXT_FLASH_LOC hal_dma_periph_2_idx(enum HAL_DMA_PERIPH_T periph)
{
    uint32_t i;
    for (i=0; i<ARRAY_SIZE(periph_map); ++i)
        if (periph_map[i].periph == periph)
            return periph_map[i].req;
    return CMU_DMA_REQ_NULL;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_dma_req_init(void)
{
    // DMA channel config
    cmu->ADMA_CH0_4_REQ =
        CMU_ADMA_CH0_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[0])) |
        CMU_ADMA_CH1_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[1])) |
        CMU_ADMA_CH2_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[2])) |
        CMU_ADMA_CH3_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[3])) |
        CMU_ADMA_CH4_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[4]));
    cmu->ADMA_CH5_9_REQ =
        CMU_ADMA_CH5_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[5])) |
        CMU_ADMA_CH6_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[6])) |
        CMU_ADMA_CH7_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[7])) |
        CMU_ADMA_CH8_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[8])) |
        CMU_ADMA_CH9_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[9]));
    cmu->ADMA_CH10_14_REQ =
        CMU_ADMA_CH10_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[10])) |
        CMU_ADMA_CH11_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[11])) |
        CMU_ADMA_CH12_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[12])) |
        CMU_ADMA_CH13_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[13])) |
        CMU_ADMA_CH14_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[14]));
    cmu->ADMA_CH15_REQ =
        CMU_ADMA_CH15_REQ_IDX(hal_dma_periph_2_idx(bes2003_audma_fifo_periph[15]));

    cmu->GDMA_CH0_4_REQ =
        CMU_GDMA_CH0_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[0])) |
        CMU_GDMA_CH1_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[1])) |
        CMU_GDMA_CH2_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[2])) |
        CMU_GDMA_CH3_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[3])) |
        CMU_GDMA_CH4_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[4]));
    cmu->GDMA_CH5_9_REQ =
        CMU_GDMA_CH5_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[5])) |
        CMU_GDMA_CH6_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[6])) |
        CMU_GDMA_CH7_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[7])) |
        CMU_GDMA_CH8_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[8])) |
        CMU_GDMA_CH9_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[9]));
    cmu->GDMA_CH10_14_REQ =
        CMU_GDMA_CH10_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[10])) |
        CMU_GDMA_CH11_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[11])) |
        CMU_GDMA_CH12_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[12])) |
        CMU_GDMA_CH13_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[13])) |
        CMU_GDMA_CH14_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[14]));
    cmu->GDMA_CH15_REQ =
        CMU_GDMA_CH15_REQ_IDX(hal_dma_periph_2_idx(bes2003_gpdma_fifo_periph[15]));
}

void BOOT_TEXT_FLASH_LOC hal_cmu_module_init_state(void)
{
    aoncmu->CLK_OUT = AON_CMU_EN_CLK_DCDC0 | AON_CMU_CFG_DIV_DCDC(1);
    aoncmu->CODEC_DIV = (aoncmu->CODEC_DIV & ~AON_CMU_SEL_AON_OSCX2) | AON_CMU_SEL_AON_OSC;
    // Slow down PMU fast clock
    //aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~(AON_CMU_BYPASS_DIV_DCDC | AON_CMU_CFG_DIV_DCDC_MASK)) | AON_CMU_CFG_DIV_DCDC(2);

    hal_cmu_sram_init();

    hal_cmu_dma_req_init();

#ifndef SIMU
    cmu->ORESET_SET = SYS_ORST_USB | SYS_ORST_USB32K | SYS_ORST_PSRAM1G | SYS_ORST_PSRAM200 | SYS_ORST_SDMMC |
        SYS_ORST_WDT | SYS_ORST_I2C0 | SYS_ORST_I2C1 | SYS_ORST_I2C2 | SYS_ORST_SPI | SYS_ORST_SLCD | SYS_ORST_SPI_PHY |
#ifndef ARM_CMNS
        SYS_ORST_UART0 |
        SYS_ORST_UART2 |
        SYS_ORST_TIMER2 |
#endif
        SYS_ORST_UART1 | SYS_ORST_UART3 | SYS_ORST_PCM | SYS_ORST_I2S0 | SYS_ORST_I2S1 |
        SYS_ORST_SPDIF0 | SYS_ORST_A7 | SYS_ORST_TSF | SYS_ORST_WDT_AP | SYS_ORST_TIMER0_AP | SYS_ORST_TIMER1_AP | SYS_ORST_FLASH1;
    cmu->PRESET_SET = SYS_PRST_WDT | SYS_PRST_I2C0 | SYS_PRST_I2C1 | SYS_PRST_I2C2 | SYS_PRST_IR |
        SYS_PRST_SPI | SYS_PRST_SLCD | SYS_PRST_SPI_PHY |
#ifndef ARM_CMNS
        SYS_PRST_UART0 |
        SYS_PRST_UART2 |
        SYS_PRST_TIMER2 |
#endif
        SYS_PRST_UART1 | SYS_PRST_UART3 |
        SYS_PRST_PCM | SYS_PRST_I2S0 | SYS_PRST_I2S1 | SYS_PRST_SPDIF0 | SYS_PRST_TQWF | SYS_PRST_TQA7;
    cmu->HRESET_SET = SYS_HRST_CORE1 | SYS_HRST_BCM | SYS_HRST_USBC | SYS_HRST_USBH | SYS_HRST_CODEC |
        SYS_HRST_AX2H_A7 | SYS_HRST_PSRAM1G | SYS_HRST_PSRAM200 | SYS_HRST_BT_DUMP | SYS_HRST_WF_DUMP | SYS_HRST_SDMMC |
        SYS_HRST_CHECKSUM | SYS_HRST_CRC | SYS_HRST_FLASH1;
    cmu->XRESET_SET = SYS_XRST_DMA | SYS_XRST_NIC | SYS_XRST_IMEMLO | SYS_XRST_IMEMHI | SYS_XRST_PSRAM1G | SYS_XRST_PER |
        SYS_XRST_PDBG | SYS_XRST_CORE0 | SYS_XRST_CORE1 | SYS_XRST_CORE2 | SYS_XRST_CORE3 | SYS_XRST_DBG | SYS_XRST_SCU |
        SYS_XRST_DISPLAYX | SYS_XRST_DISPLAYH | SYS_XRST_CSI | SYS_XRST_DSI | SYS_XRST_PSRAM1GMX | SYS_XRST_GPV_MAIN | SYS_XRST_GPV_PSRAM1G;
    cmu->APRESET_SET = SYS_APRST_BOOTREG | SYS_APRST_WDT| SYS_APRST_TIMER0 | SYS_APRST_TIMER1 | SYS_APRST_TQ | SYS_APRST_DAP |
        SYS_APRST_DISPLAY | SYS_APRST_CSI;
    cmu->QRESET_SET = SYS_QRST_DSI_32K | SYS_QRST_DSI_PN | SYS_QRST_DSI_TV | SYS_QRST_DSI_PIX |
        SYS_QRST_DSI_DSI | SYS_QRST_CSI_LANE | SYS_QRST_CSI_PIX | SYS_QRST_CSI_LANG | SYS_QRST_IR;

    cmu->OCLK_DISABLE = SYS_OCLK_USB | SYS_OCLK_USB32K | SYS_OCLK_PSRAM1G | SYS_OCLK_PSRAM200 | SYS_OCLK_SDMMC |
        SYS_OCLK_WDT | SYS_OCLK_I2C0 | SYS_OCLK_I2C1 | SYS_OCLK_I2C2 | SYS_OCLK_SPI | SYS_OCLK_SLCD | SYS_OCLK_SPI_PHY |
#ifndef ARM_CMNS
        SYS_OCLK_UART0 |
        SYS_OCLK_UART2 |
        SYS_OCLK_TIMER2 |
#endif
        SYS_OCLK_UART1 | SYS_OCLK_UART3 | SYS_OCLK_PCM | SYS_OCLK_I2S0 | SYS_OCLK_I2S1 |
        SYS_OCLK_SPDIF0 | SYS_OCLK_A7 | SYS_OCLK_TSF | SYS_OCLK_WDT_AP | SYS_OCLK_TIMER0_AP | SYS_OCLK_TIMER1_AP | SYS_OCLK_FLASH1;
    cmu->PCLK_DISABLE = SYS_PCLK_WDT | SYS_PCLK_I2C0 | SYS_PCLK_I2C1 | SYS_PCLK_I2C2 | SYS_PCLK_IR |
        SYS_PCLK_SPI | SYS_PCLK_SLCD | SYS_PCLK_SPI_PHY |
#ifndef ARM_CMNS
        SYS_PCLK_UART0 |
        SYS_PCLK_UART2 |
        SYS_PCLK_TIMER2 |
#endif
        SYS_PCLK_UART1 | SYS_PCLK_UART3 |
        SYS_PCLK_PCM | SYS_PCLK_I2S0 | SYS_PCLK_I2S1 | SYS_PCLK_SPDIF0 | SYS_PCLK_TQWF | SYS_PCLK_TQA7;
    cmu->HCLK_DISABLE = SYS_HCLK_CORE1 | SYS_HCLK_BCM | SYS_HCLK_USBC | SYS_HCLK_USBH | SYS_HCLK_CODEC |
        SYS_HCLK_AX2H_A7 | SYS_HCLK_PSRAM1G | SYS_HCLK_PSRAM200 | SYS_HCLK_BT_DUMP | SYS_HCLK_WF_DUMP | SYS_HCLK_SDMMC |
        SYS_HCLK_CHECKSUM | SYS_HCLK_CRC | SYS_HCLK_FLASH1;
    cmu->XCLK_DISABLE = SYS_XCLK_DMA | SYS_XCLK_NIC | SYS_XCLK_IMEMLO | SYS_XCLK_IMEMHI | SYS_XCLK_PSRAM1G | SYS_XCLK_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU |
        SYS_XCLK_DISPLAYX | SYS_XCLK_DISPLAYH | SYS_XCLK_CSI | SYS_XCLK_DSI | SYS_XCLK_PSRAM1GMX | SYS_XCLK_GPV_MAIN | SYS_XCLK_GPV_PSRAM1G;;
    cmu->APCLK_DISABLE = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP |
        SYS_APCLK_DISPLAY | SYS_APCLK_CSI;
    cmu->QCLK_DISABLE = SYS_QCLK_DSI_32K | SYS_QCLK_DSI_PN | SYS_QCLK_DSI_TV | SYS_QCLK_DSI_PIX |
        SYS_QCLK_DSI_DSI | SYS_QCLK_CSI_LANE | SYS_QCLK_CSI_PIX | SYS_QCLK_CSI_LANG | SYS_QCLK_IR;

    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODEC_DISABLE | AON_CMU_EN_CLK_CODECIIR_DISABLE | AON_CMU_EN_CLK_CODECRS0_DISABLE |
        AON_CMU_EN_CLK_CODECRS1_DISABLE | AON_CMU_EN_CLK_CODECHCLK_DISABLE | AON_CMU_EN_CLK_VAD32K_DISABLE |
        AON_CMU_EN_CLK_BT_DISABLE | AON_CMU_EN_CLK_WF_DISABLE;

    aoncmu->RESET_SET = AON_CMU_ARESETN_SET(AON_ARST_PWM | AON_ARST_PWM1) |
        AON_CMU_ORESETN_SET(AON_ORST_PWM0 | AON_ORST_PWM1 | AON_ORST_PWM2 | AON_ORST_PWM3 |
            AON_ORST_PWM4 | AON_ORST_PWM5 | AON_ORST_PWM6 | AON_ORST_PWM7 | AON_ORST_BTAON);

    aoncmu->SOFT_RSTN_SET = AON_CMU_SOFT_RSTN_CODEC_SET |
        AON_CMU_SOFT_RSTN_A7_SET | AON_CMU_SOFT_RSTN_A7CPU_SET |
        AON_CMU_SOFT_RSTN_WF_SET | AON_CMU_SOFT_RSTN_WFCPU_SET |
        AON_CMU_SOFT_RSTN_BT_SET | AON_CMU_SOFT_RSTN_BTCPU_SET;

    aoncmu->MOD_CLK_DISABLE = AON_CMU_MANUAL_ACLK_DISABLE(AON_ACLK_PWM) |
        AON_CMU_MANUAL_OCLK_DISABLE(AON_OCLK_PWM0 | AON_OCLK_PWM1 | AON_OCLK_PWM2 | AON_OCLK_PWM3 | AON_OCLK_BTAON);

    aoncmu->MOD_CLK_MODE &= ~AON_CMU_MODE_ACLK(AON_ACLK_CMU | AON_ACLK_GPIO_INT | AON_ACLK_WDT | AON_ACLK_PWM |
        AON_ACLK_TIMER | AON_ACLK_IOMUX);
    cmu->PCLK_MODE &= ~(SYS_PCLK_CMU | SYS_PCLK_WDT | SYS_PCLK_TIMER0 | SYS_PCLK_TIMER1 | SYS_PCLK_TIMER2);

    aoncmu->MIPI_CLK &= ~(AON_CMU_EN_CLK_PIX_DSI | AON_CMU_EN_CLK_PIX_CSI | AON_CMU_POL_CLK_DSI_IN |
        AON_CMU_POL_CLK_CSI_IN);

    //cmu->HCLK_MODE = 0;
    //cmu->PCLK_MODE = SYS_PCLK_UART0 | SYS_PCLK_UART1 | SYS_PCLK_UART2;
    //cmu->OCLK_MODE = 0;
#endif
    hal_psc_init();
}

void BOOT_TEXT_FLASH_LOC hal_cmu_ema_init(void)
{
    // Never change EMA
}

void hal_cmu_lpu_wait_26m_ready(void)
{
    while ((cmu->WAKEUP_CLK_CFG & (CMU_LPU_AUTO_SWITCH26 | CMU_LPU_STATUS_26M)) ==
            CMU_LPU_AUTO_SWITCH26);
}

int hal_cmu_lpu_busy(void)
{
    if ((cmu->WAKEUP_CLK_CFG & (CMU_LPU_AUTO_SWITCH26 | CMU_LPU_STATUS_26M)) ==
            CMU_LPU_AUTO_SWITCH26) {
        return 1;
    }
    if ((cmu->WAKEUP_CLK_CFG & (CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_STATUS_PLL)) ==
            CMU_LPU_AUTO_SWITCHPLL) {
        return 1;
    }
    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_lpu_init(enum HAL_CMU_LPU_CLK_CFG_T cfg)
{
    uint32_t lpu_clk;
    uint32_t timer_26m;
    uint32_t timer_pll;

    timer_26m = LPU_TIMER_US(TICKS_TO_US(HAL_CMU_26M_READY_TIMEOUT));
    timer_pll = LPU_TIMER_US(TICKS_TO_US(HAL_CMU_PLL_LOCKED_TIMEOUT));

    if (cfg >= HAL_CMU_LPU_CLK_QTY) {
        return 1;
    }
    if ((timer_26m & (CMU_TIMER_WT26_MASK >> CMU_TIMER_WT26_SHIFT)) != timer_26m) {
        return 2;
    }
    if ((timer_pll & (CMU_TIMER_WTPLL_MASK >> CMU_TIMER_WTPLL_SHIFT)) != timer_pll) {
        return 3;
    }
    if (hal_cmu_lpu_busy()) {
        return -1;
    }
    lpu_clk = cmu->WAKEUP_CLK_CFG;
    lpu_clk &= ~(CMU_LPU_EN_MCU | CMU_LPU_EN_A7 | CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26);

    if (cfg == HAL_CMU_LPU_CLK_26M) {
        lpu_clk |= CMU_LPU_EN_MCU | CMU_LPU_EN_A7 | CMU_LPU_AUTO_SWITCH26;
    } else if (cfg == HAL_CMU_LPU_CLK_PLL) {
        lpu_clk |= CMU_LPU_EN_MCU | CMU_LPU_EN_A7 | CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26;
    } else {
    }

    if (lpu_clk & CMU_LPU_AUTO_SWITCH26) {
        // Disable RAM wakeup early
        cmu->MCU_TIMER &= ~CMU_RAM_RETN_UP_EARLY;
        // MCU/ROM/RAM auto clock gating (which depends on RAM gating signal)
        cmu->HCLK_MODE &= ~(SYS_HCLK_CORE0 | SYS_HCLK_ROM0 | SYS_HCLK_RAM0 | SYS_HCLK_RAM1 |
            SYS_HCLK_RAM2 | SYS_HCLK_RAM3 | SYS_HCLK_RAM4 | SYS_HCLK_RAM5);
        // AON_CMU enable auto switch 26M (AON_CMU must have selected 26M and disabled 52M/32K already)
        aoncmu->CODEC_DIV |= AON_CMU_LPU_AUTO_SWITCH26;
    } else {
        // AON_CMU disable auto switch 26M
        aoncmu->CODEC_DIV &= ~AON_CMU_LPU_AUTO_SWITCH26;
    }

    lpu_clk |= CMU_TIMER_WT26(timer_26m);
    if (timer_pll) {
        lpu_clk |= CMU_TIMER_WTPLL(timer_pll);
        hal_sys_timer_delay(US_TO_TICKS(60));
        cmu->WAKEUP_CLK_CFG = lpu_clk;
    } else {
        lpu_clk |= CMU_TIMER_WTPLL(0);
        cmu->WAKEUP_CLK_CFG = lpu_clk;
    }

#ifndef ARM_CMNS
    hal_sec_init(); //need metal_id
#endif

    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4) {
        cmu->CMU_REMAP = CMU_REMAP(0xF); //psram cache and flash cache
    } else {
        cmu->CMU_REMAP = CMU_REMAP(0xC); //psram cache
    }

    return 0;
}

__STATIC_FORCEINLINE void cpu_sleep(uint32_t wakeup_cfg)
{
    __DSB();

    if (wakeup_cfg & (CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26)) {
        // 1) Avoid race condition between LPU state machine entry and IRQ wakeup:
        //    Wait 4 (at least 2) cycles of 32K clock, or 3248 cycles of 26M clock.
        // 2) Avoid race condition between CPU clock gating and RAM access when waiting:
        //    No consecutive RAM access is allowed.
        //    This is related to instruction width/alignment, pipeline state, branch prediction state, etc.
        //    Unfortunately, it is too hard to control all of these, especially on M33.
        __WFI();
    } else {
        __WFI();
    }
}

int SRAM_TEXT_LOC hal_cmu_lpu_sleep(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
    uint32_t start;
    uint32_t timeout;
    uint32_t saved_pll_cfg;
    uint32_t saved_clk_cfg;
    uint32_t saved_codec_div;
    uint32_t saved_top_clk;
    uint32_t wakeup_cfg;
    uint32_t saved_hclk;
    uint32_t saved_oclk;

    saved_codec_div = aoncmu->CODEC_DIV;
    saved_top_clk = aoncmu->TOP_CLK_ENABLE;
    saved_pll_cfg = aoncmu->PLL_ENABLE;
    saved_clk_cfg = cmu->SYS_CLK_ENABLE;
    saved_hclk = cmu->HCLK_ENABLE;
    saved_oclk = cmu->OCLK_ENABLE;

    // Switch VAD clock to AON and disable codec HCLK
    aoncmu->CODEC_DIV |= AON_CMU_SEL_CODECHCLK_MCU;
    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODECHCLK_DISABLE;
    } else {
        // Avoid auto-gating OSC and OSCX2
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSCX2_ENABLE | AON_CMU_EN_CLK_TOP_OSC_ENABLE;
    }

    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        wakeup_cfg = cmu->WAKEUP_CLK_CFG;
    } else {
        wakeup_cfg = 0;
    }

    // Disable memory/flash freq
    cmu->OCLK_DISABLE = SYS_OCLK_PSRAM1G | SYS_OCLK_PSRAM200 | SYS_OCLK_FLASH;
    cmu->HCLK_DISABLE = SYS_HCLK_PSRAM1G | SYS_HCLK_PSRAM200 | SYS_HCLK_FLASH;

#ifndef ROM_BUILD
    // Reset pll div if pll is enabled
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_USB);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_DDR);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_DSP);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_BB);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_BB_PSRAM);
    }
#endif

    // Setup wakeup mask
#ifdef __ARM_ARCH_ISA_ARM
    cmu->WAKEUP_MASK0 = GICDistributor->ISENABLER[0];
    cmu->WAKEUP_MASK1 = GICDistributor->ISENABLER[1];
    cmu->WAKEUP_MASK2 = GICDistributor->ISENABLER[2];
#else
    cmu->WAKEUP_MASK0 = NVIC->ISER[0];
    cmu->WAKEUP_MASK1 = NVIC->ISER[1];
    cmu->WAKEUP_MASK2 = NVIC->ISER[2];
#endif

    // Switch system freq to 26M
    cmu->SYS_CLK_ENABLE = CMU_SEL_MCU_SLOW_ENABLE;
    cmu->SYS_CLK_DISABLE = CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
        CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
    cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_MCU_DISABLE;
    // Shutdown PLLs
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLUSB_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLDDR_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLDDR_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLA7_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLA7_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLBB_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLBB_DIV_MCU_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLBB_PS_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLBB_DIV_PS_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_ENABLE) {
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLBB_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_OSCX4_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_OSCX4_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_OSCX4_DISABLE;
    }

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        // Do nothing
        // Hardware will switch system freq to 32K and shutdown PLLs automatically
    } else {
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            // Do nothing
            // Hardware will switch system freq to 32K automatically
        } else {
            // Manually switch AON_CMU clock to 32K
            aoncmu->CODEC_DIV &= ~(AON_CMU_SEL_AON_OSC | AON_CMU_SEL_AON_OSCX2);
            // Switch system freq to 32K
            cmu->SYS_CLK_DISABLE = CMU_SEL_MCU_SLOW_DISABLE;
        }
    }

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
        // Enable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_MANUAL_RAM_RETN) |
            CMU_DEEPSLEEP_EN | CMU_DEEPSLEEP_ROMRAM_EN | CMU_DEEPSLEEP_START;
    } else {
        // Disable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_DEEPSLEEP_ROMRAM_EN) |
            CMU_DEEPSLEEP_EN | CMU_MANUAL_RAM_RETN | CMU_DEEPSLEEP_START;
    }

#ifndef __ARM_ARCH_ISA_ARM
    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
    } else {
        SCB->SCR = 0;
    }
#endif

    cpu_sleep(wakeup_cfg);

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        start = hal_sys_timer_get();
        timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_PLL_LOCKED_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
        while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_PLL) == 0 &&
            (hal_sys_timer_get() - start) < timeout);
        // Hardware will switch system to PLL divider and enable PLLs automatically
    } else {
        // Wait for 26M ready
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            start = hal_sys_timer_get();
            timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
            while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_26M) == 0 &&
                (hal_sys_timer_get() - start) < timeout);
            // Hardware will switch system freq to 26M automatically
        } else {
            if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
                timeout = HAL_CMU_26M_READY_TIMEOUT;
                hal_sys_timer_delay(timeout);
            }
            // Switch system freq to 26M
            cmu->SYS_CLK_ENABLE = CMU_SEL_MCU_SLOW_ENABLE;
            // Restore AON_CMU clock
            aoncmu->CODEC_DIV = saved_codec_div;
        }
    }

    // System freq is 26M now and will be restored later
    // Restore PLLs
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLUSB_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLDDR_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLA7_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLBB_DIV_MCU_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLBB_DIV_PS_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_OSCX4_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_OSCX4_ENABLE;
    }
#ifndef ROM_BUILD
    hal_sys_timer_delay_us(20);
    // Clear pll div reset if pll is enabled
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_USB);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_DDR);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_DSP);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_BB);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_BB_PSRAM);
    }
    hal_sys_timer_delay_us(10);
#endif
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLDDR_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLA7_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLBB_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLBB_PS_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_OSCX4_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSCX4_ENABLE;
    }
    // Restore system freq
    cmu->SYS_CLK_ENABLE = saved_clk_cfg & CMU_RSTN_DIV_MCU_ENABLE;
    cmu->SYS_CLK_ENABLE = saved_clk_cfg;
    // The original system freq are at least 26M
    //cmu->SYS_CLK_DISABLE = ~saved_clk_cfg;

    // Switch VAD clock to MCU and enable codec HCLK if it is on before entering sleep
    aoncmu->CODEC_DIV &= ~AON_CMU_SEL_CODECHCLK_MCU;
    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        aoncmu->TOP_CLK_ENABLE = saved_top_clk & AON_CMU_EN_CLK_CODECHCLK_ENABLE;
    } else {
        // Restore the original top clock settings
        aoncmu->TOP_CLK_DISABLE = ~saved_top_clk;
    }

    if (saved_oclk & (SYS_OCLK_PSRAM1G | SYS_OCLK_PSRAM200 | SYS_OCLK_FLASH)) {
        // Enable memory/flash clock
        cmu->HCLK_ENABLE = saved_hclk;
        cmu->OCLK_ENABLE = saved_oclk;
        // Wait until memory/flash clock ready
        hal_sys_timer_delay_us(2);
    }

    return 0;
}

volatile uint32_t *hal_cmu_get_bootmode_addr(void)
{
    return &aoncmu->BOOTMODE;
}

void hal_cmu_bt_clock_enable(void)
{
    aoncmu->MOD_CLK_ENABLE = AON_CMU_MANUAL_OCLK_ENABLE(AON_OCLK_BTAON);
    aocmu_reg_update_wait();
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_BT_ENABLE;
    aocmu_reg_update_wait();
}

void hal_cmu_bt_clock_disable(void)
{
    aoncmu->MOD_CLK_DISABLE = AON_CMU_MANUAL_OCLK_DISABLE(AON_OCLK_BTAON);
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_BT_DISABLE;
}

void hal_cmu_bt_reset_set(void)
{
    aoncmu->RESET_SET = AON_CMU_ORESETN_SET(AON_ORST_BTAON);
    aoncmu->SOFT_RSTN_SET = AON_CMU_SOFT_RSTN_BT_SET | AON_CMU_SOFT_RSTN_BTCPU_SET;
}

void hal_cmu_bt_reset_clear(void)
{
    aoncmu->RESET_CLR = AON_CMU_ORESETN_CLR(AON_ORST_BTAON);
    aocmu_reg_update_wait();
    aoncmu->SOFT_RSTN_CLR = AON_CMU_SOFT_RSTN_BT_CLR | AON_CMU_SOFT_RSTN_BTCPU_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_bt_module_init(void)
{
    //btcmu->CLK_MODE = 0;
}

void hal_cmu_wifi_clock_enable(void)
{
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_WF_ENABLE;
    aocmu_reg_update_wait();
}

void hal_cmu_wifi_clock_disable(void)
{
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_WF_DISABLE;
}

void hal_cmu_wifi_reset_set(void)
{
    aoncmu->SOFT_RSTN_SET = AON_CMU_SOFT_RSTN_WF_SET | AON_CMU_SOFT_RSTN_WFCPU_SET;
}

void hal_cmu_wifi_reset_clear(void)
{
    aoncmu->SOFT_RSTN_CLR = AON_CMU_SOFT_RSTN_WF_CLR | AON_CMU_SOFT_RSTN_WFCPU_CLR;
    aocmu_reg_update_wait();
}

void BOOT_TEXT_FLASH_LOC hal_cmu_wlan_set_sleep_allow(uint32_t auto_mode)
{
    if (auto_mode) { //controlled by wifi sleep signal
        aoncmu->RESERVED_0E8 = aoncmu->RESERVED_0E8 & (~((1<<4)|(1<<6)));
    } else { //force allow cmu sleep
        aoncmu->RESERVED_0E8 = (aoncmu->RESERVED_0E8 & (~((1<<5)|(1<<7)))) | ((1<<4)|(1<<6));
    }
}

#ifdef DSP_ENABLE
#ifndef ARM_CMNS
static void GPV_init(void)
{
#ifdef GPV_MAIN_BASE
    volatile uint32_t *gpv_ctrl_psram = (volatile uint32_t *)(GPV_MAIN_BASE+0x6000);
    volatile uint32_t *gpv_ctrl_x2h = (volatile uint32_t *)(GPV_MAIN_BASE+0x7000);
    gpv_ctrl_psram[17] |= 0x2; //0x50306044
    gpv_ctrl_x2h[17] |= 0x2; //0x50307044
#endif
}
#endif
#endif

void hal_cmu_dsp_clock_enable(void)
{
    enum HAL_CMU_PLL_T dsp;
#ifdef DSP_ENABLE
    hal_psc_a7_enable();
#endif
    // DSP AXI clock divider defaults to 4 (div = reg_val + 2)
    cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_XCLK, 2);
    cmu->XCLK_ENABLE = SYS_XCLK_DMA | SYS_XCLK_NIC | SYS_XCLK_IMEMLO | SYS_XCLK_IMEMHI | SYS_XCLK_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU |
        SYS_XCLK_GPV_MAIN;
    cmu->APCLK_ENABLE = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;
    cmu->HCLK_ENABLE = SYS_HCLK_AX2H_A7;

    cmu->DSP_CFG0 |= CMU_CA7_DBGEN_MASK | CMU_CA7_SPIDEN_MASK | CMU_CA7_NIDEN_MASK | CMU_CA7_SPNIDEN_MASK;

    cmu->PERIPH_CLK |= CMU_A7_ALLIRQ_MASK;
    cmu->DSP_CFG0 |= CMU_A7TOM33_IRQS_MASK(8);

#ifdef DSP_USE_BBPLL
    dsp = HAL_CMU_PLL_BB;
#elif defined(DSP_USE_USBPLL)
    dsp = HAL_CMU_PLL_USB;
#else
    dsp = HAL_CMU_PLL_DSP;
#endif
    hal_cmu_dsp_select_pll(dsp);
    hal_cmu_pll_enable(dsp, HAL_CMU_PLL_USER_DSP);

    hal_cmu_dsp_set_freq(HAL_CMU_FREQ_780M);

#ifdef DSP_ENABLE
    hal_cmu_clock_enable(HAL_CMU_MOD_O_A7);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_WDT_AP);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_TIMER0_AP);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_TIMER1_AP);
#ifndef ARM_CMNS
    ///TODO: add cmse interface if trustzone is enabled
    GPV_init();
#endif
#endif
}

void hal_cmu_dsp_clock_disable(void)
{
    enum HAL_CMU_PLL_T dsp;

    hal_cmu_clock_disable(HAL_CMU_MOD_O_A7);

#ifdef DSP_USE_BBPLL
    dsp = HAL_CMU_PLL_BB;
#elif defined(DSP_USE_USBPLL)
    dsp = HAL_CMU_PLL_USB;
#else
    dsp = HAL_CMU_PLL_DSP;
#endif
    hal_cmu_pll_disable(dsp, HAL_CMU_PLL_USER_DSP);

    cmu->XCLK_DISABLE = SYS_XCLK_DMA | SYS_XCLK_NIC | SYS_XCLK_IMEMLO | SYS_XCLK_IMEMHI | SYS_XCLK_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU |
        SYS_XCLK_GPV_MAIN;
    cmu->APCLK_DISABLE = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;
    cmu->OCLK_DISABLE = SYS_OCLK_WDT_AP | SYS_OCLK_TIMER0_AP | SYS_OCLK_TIMER1_AP;
    cmu->HCLK_DISABLE = SYS_HCLK_AX2H_A7;

    hal_psc_a7_disable();
}


void hal_cmu_dsp_clock_continue(void)
{
    enum HAL_CMU_PLL_T dsp;
    // DSP AXI clock divider defaults to 4 (div = reg_val + 2)
    cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_XCLK, 2);
    cmu->XCLK_ENABLE = SYS_XCLK_DMA | SYS_XCLK_NIC | SYS_XCLK_IMEMLO | SYS_XCLK_IMEMHI | SYS_XCLK_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU |
        SYS_XCLK_GPV_MAIN;
    cmu->APCLK_ENABLE = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;
    cmu->HCLK_ENABLE = SYS_HCLK_AX2H_A7;

    cmu->DSP_CFG0 |= CMU_CA7_DBGEN_MASK | CMU_CA7_SPIDEN_MASK | CMU_CA7_NIDEN_MASK | CMU_CA7_SPNIDEN_MASK;

    cmu->PERIPH_CLK |= CMU_A7_ALLIRQ_MASK;
    cmu->DSP_CFG0 |= CMU_A7TOM33_IRQS_MASK(8);

#ifdef DSP_USE_BBPLL
    dsp = HAL_CMU_PLL_BB;
#elif defined(DSP_USE_USBPLL)
    dsp = HAL_CMU_PLL_USB;
#else
    dsp = HAL_CMU_PLL_DSP;
#endif
    hal_cmu_dsp_select_pll(dsp);
    hal_cmu_pll_enable(dsp, HAL_CMU_PLL_USER_DSP);

    hal_cmu_dsp_set_freq(HAL_CMU_FREQ_780M);

#ifdef DSP_ENABLE
    hal_cmu_clock_enable(HAL_CMU_MOD_O_A7);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_WDT_AP);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_TIMER0_AP);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_TIMER1_AP);
#ifndef ARM_CMNS
    ///TODO: add cmse interface if trustzone is enabled
    GPV_init();
#endif
#endif
}


void hal_cmu_dsp_clock_hold(void)
{
    enum HAL_CMU_PLL_T dsp;

    hal_cmu_clock_disable(HAL_CMU_MOD_O_A7);

#ifdef DSP_USE_BBPLL
    dsp = HAL_CMU_PLL_BB;
#elif defined(DSP_USE_USBPLL)
    dsp = HAL_CMU_PLL_USB;
#else
    dsp = HAL_CMU_PLL_DSP;
#endif
    hal_cmu_pll_disable(dsp, HAL_CMU_PLL_USER_DSP);

    cmu->XCLK_DISABLE = SYS_XCLK_DMA | SYS_XCLK_NIC | SYS_XCLK_IMEMLO | SYS_XCLK_IMEMHI | SYS_XCLK_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU |
        SYS_XCLK_GPV_MAIN;
    cmu->APCLK_DISABLE = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;
    cmu->OCLK_DISABLE = SYS_OCLK_WDT_AP | SYS_OCLK_TIMER0_AP | SYS_OCLK_TIMER1_AP;
    cmu->HCLK_DISABLE = SYS_HCLK_AX2H_A7;
}

void hal_cmu_dsp_reset_set(void)
{
    aoncmu->SOFT_RSTN_SET = AON_CMU_SOFT_RSTN_A7_SET | AON_CMU_SOFT_RSTN_A7CPU_SET;
    cmu->ORESET_SET = SYS_ORST_WDT_AP | SYS_ORST_TIMER0_AP | SYS_ORST_TIMER1_AP;
    cmu->XRESET_SET = SYS_XRST_DMA | SYS_XRST_NIC | SYS_XRST_IMEMLO | SYS_XRST_IMEMHI | SYS_XRST_PER |
        SYS_XRST_PDBG | SYS_XRST_CORE0 | SYS_XRST_CORE1 | SYS_XRST_CORE2 | SYS_XRST_CORE3 | SYS_XRST_DBG | SYS_XRST_SCU |
        SYS_XRST_GPV_MAIN;
    cmu->APRESET_SET = SYS_APRST_BOOTREG | SYS_APRST_WDT | SYS_APRST_TIMER0 | SYS_APRST_TIMER1 | SYS_APRST_TQ | SYS_APRST_DAP;
    cmu->HRESET_SET = SYS_HRST_AX2H_A7;
}

void hal_cmu_dsp_reset_clear(void)
{
    cmu->APRESET_CLR = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;
    cmu->XRESET_CLR = SYS_XRST_DMA | SYS_XRST_NIC | SYS_XRST_IMEMLO | SYS_XRST_IMEMHI | SYS_XRST_PER |
        SYS_XRST_PDBG | SYS_XRST_CORE0 | SYS_XRST_CORE1 | SYS_XRST_CORE2 | SYS_XRST_CORE3 | SYS_XRST_DBG | SYS_XRST_SCU |
        SYS_XRST_GPV_MAIN;
    cmu->ORESET_CLR = SYS_ORST_WDT_AP | SYS_ORST_TIMER0_AP | SYS_ORST_TIMER1_AP;
    cmu->HRESET_CLR = SYS_HRST_AX2H_A7;
    aoncmu->SOFT_RSTN_CLR = AON_CMU_SOFT_RSTN_A7_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_dsp_reset_hold(void)
{
    aoncmu->SOFT_RSTN_SET = AON_CMU_SOFT_RSTN_A7_SET | AON_CMU_SOFT_RSTN_A7CPU_SET;
    cmu->HRESET_SET = SYS_HRST_AX2H_A7 | SYS_HRST_PSRAM1G;
    cmu->ORESET_SET = SYS_ORST_PSRAM1G;
    cmu->ORESET_SET = SYS_ORST_WDT_AP | SYS_ORST_TIMER1_AP;
}

void hal_cmu_dsp_reset_release(void)
{
    cmu->ORESET_CLR = SYS_ORST_WDT_AP | SYS_ORST_TIMER1_AP;
    cmu->ORESET_CLR = SYS_ORST_PSRAM1G;
    cmu->HRESET_CLR = SYS_HRST_AX2H_A7 | SYS_HRST_PSRAM1G;
    aoncmu->SOFT_RSTN_CLR = AON_CMU_SOFT_RSTN_A7_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_dsp_init_boot_reg(uint32_t entry)
{
    int i;
    volatile uint32_t *boot = (volatile uint32_t *)DSP_BOOT_REG;

    // Unlock
    boot[32] = 0xCAFE0001;
    __DMB();

    for (i = 0; i < 8; i++) {
        // ldr pc, [pc, #24]
        boot[0 + i] = 0xE59FF018;
        // Init_Handler
        boot[8 + i] = 0x00000040;
    }
    // b   40
    boot[16] = 0xEAFFFFFE;

    // Update reset handler
    boot[8] = entry;

    // Lock
    __DMB();
    boot[32] = 0xCAFE0000;
    __DMB();
}

void hal_cmu_dsp_start_cpu(void)
{
    hal_cmu_dsp_set_freq(HAL_CMU_FREQ_156M);//div 5
    aoncmu->SOFT_RSTN_CLR = AON_CMU_SOFT_RSTN_A7CPU_CLR;
}

void hal_cmu_dsp_stop_cpu(void)
{
    uint32_t lock;
    lock = int_lock();
#ifdef PSRAMUHS_ENABLE
    hal_psramuhs_hold();
#endif
    hal_cmu_dsp_reset_hold();
    //hal_cmu_dsp_clock_disable();

#ifdef PSRAMUHS_ENABLE
    //hal_cmu_dsp_clock_enable();
    hal_cmu_dsp_reset_release();
    hal_psramuhs_release();
#endif
    int_unlock(lock);
}

uint32_t hal_cmu_get_aon_chip_id(void)
{
    return aoncmu->CHIP_ID;
}

uint32_t hal_cmu_get_aon_revision_id(void)
{
    return GET_BITFIELD(aoncmu->CHIP_ID, AON_CMU_REVISION_ID);
}

void hal_cmu_cp_enable(uint32_t sp, uint32_t entry)
{
    struct CP_STARTUP_CFG_T * cp_cfg;
    uint32_t cfg_addr;

    // Use (sp - 128) as the default vector. The Address must aligned to 128-byte boundary.
    cfg_addr = (sp - (1 << 7)) & ~((1 << 7) - 1);

    cmu->CP_VTOR = cfg_addr;
    cp_cfg = (struct CP_STARTUP_CFG_T *)cfg_addr;

    cp_cfg->stack = sp;
    cp_cfg->reset_hdlr = (uint32_t)system_cp_reset_handler;
    cp_entry = entry;

    hal_cmu_clock_enable(HAL_CMU_MOD_H_CP);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CP);
}

void hal_cmu_cp_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_CP);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_CP);
}

uint32_t hal_cmu_cp_get_entry_addr(void)
{
    return cp_entry;
}

void hal_cmu_cp_boot(uint32_t entry)
{
    cmu->CP_VTOR = entry;

    hal_cmu_clock_enable(HAL_CMU_MOD_H_CACHE1);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CACHE1);

    hal_cmu_clock_enable(HAL_CMU_MOD_H_CP);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CP);
}

void hal_cmu_jtag_sel(enum HAL_CMU_JTAG_SEL_T sel)
{
    uint32_t lock;

    lock = int_lock();
    cmu->PERIPH_CLK &= ~CMU_JTAG_SEL_MASK;
    switch (sel) {
        case HAL_CMU_JTAG_SEL_CP:
            cmu->PERIPH_CLK |= CMU_JTAG_SEL_CP;
            break;
        case HAL_CMU_JTAG_SEL_A7:
            cmu->PERIPH_CLK |= CMU_JTAG_SEL_A7;
            break;
        default:
            break;
    }
    int_unlock(lock);
}

void hal_cmu_dsp_setup(void)
{
    hal_sys_timer_open();

    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_390M);

}

void hal_cmu_flash0_dual_die()
{
    cmu->MCU_TIMER &= ~CMU_FLS1_IO_SEL;
}

void hal_cmu_flash1_enable()
{
    
    hal_cmu_clock_enable(HAL_CMU_MOD_O_FLASH1);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_FLASH1);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_FLASH1);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_FLASH1);
    
    //aoncmu->FLASH_IOCFG |= AON_CMU_PU_FLASH1_IO;
    aoncmu->FLASH_IOCFG = SET_BITFIELD(aoncmu->FLASH_IOCFG, AON_CMU_FLASH1_IODRV, 0x7) | AON_CMU_PU_FLASH1_IO;

}

void hal_cmu_set_flash0_x8_mode(uint32_t en)
{
    if (en)
        cmu->MCU_TIMER |= CMU_FLS0_X8_SEL;
    else
        cmu->MCU_TIMER &= ~CMU_FLS0_X8_SEL;
}

void hal_cmu_set_flash0_size(enum HAL_FLASH0_SIZE_CFG cfg)
{
    *(volatile uint32_t *)(ICACHE_CTRL_BASE + 0x44) = cfg;
}

uint32_t hal_cmu_get_osc_ready_cycle_cnt(void)
{
    uint32_t cnt=10;



    return cnt;
}

uint32_t hal_cmu_get_osc_switch_overhead(void)
{
        return 6;
}

void hal_cmu_bt_sys_set_freq(enum HAL_CMU_FREQ_T freq)
{

}

void hal_cmu_bt_sys_clock_force_on(void)
{
    uint32_t lock;

    lock = int_lock();
    hal_psc_dslp_force_on_bt_enable();
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_BT_CLK_SYS_ENABLE;
    int_unlock(lock);
    aocmu_reg_update_wait();
}

void hal_cmu_bt_sys_clock_auto(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_BT_CLK_SYS_DISABLE;
    hal_psc_dslp_force_on_bt_disable();
    int_unlock(lock);
}

#ifdef REUSE_WIFI_CALI_RESULT
extern void save_wifi_cali_result();
#endif

void hal_cmu_shutdown_hook(void)
{
    int_lock();
    hal_cmu_codec_clock_disable();
    hal_cmu_codec_rs_disable();

    hal_cmu_dsp_reset_set();
    hal_cmu_dsp_clock_disable();

#ifdef REUSE_WIFI_CALI_RESULT
    save_wifi_cali_result();
#endif

    hal_cmu_wifi_reset_set();
    hal_cmu_wifi_clock_disable();
    hal_cmu_cp_disable();
    hal_cmu_bt_reset_set();
    hal_cmu_bt_clock_disable();

    // hal_norflash_deinit();

    hal_cmu_mem_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);

    // psramuhsphy_sleep();
    // hal_psram_phy_sleep();
    // hal_cmu_pll_disable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_ALL);
    // hal_cmu_pll_disable(HAL_CMU_PLL_BB_PSRAM, HAL_CMU_PLL_USER_ALL);
    // hal_cmu_pll_disable(HAL_CMU_PLL_DDR, HAL_CMU_PLL_USER_ALL);
    // hal_cmu_pll_disable(HAL_CMU_PLL_DSP, HAL_CMU_PLL_USER_ALL);
    // hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_ALL);
}

void hal_cmu_sys_reboot(void)
{
#if defined(FLASH_SUSPEND) && defined(FLASH_API_GUARD_THREAD)
    norflash_api_flush_all();
#endif
    hal_cmu_shutdown_hook();
    hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
}
