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
#include "plat_addr_map.h"
#include "cmsis.h"
#include "hal_gpio.h"
#include "hal_location.h"
#include "hal_psc.h"
#include "hal_timer.h"
#include CHIP_SPECIFIC_HDR(reg_psc)

#define PSC_WRITE_ENABLE                    0xCAFE0000

enum PSC_INTR_MASK2_T {
    PSC_INTR_MASK2_GPADC        = (1 << 8),
    PSC_INTR_MASK2_TIMER        = (1 << 9),
    PSC_INTR_MASK2_WDT          = (1 << 10),
    PSC_INTR_MASK2_PMU          = (1 << 11),
    PSC_INTR_MASK2_CHARGE       = (1 << 12),
    PSC_INTR_MASK2_CODEC        = (1 << 13),
    PSC_INTR_MASK2_WAKEUP_BT    = (1 << 14),
};

static struct AONPSC_T * const psc = (struct AONPSC_T *)AON_PSC_BASE;
BOOT_BSS_LOC static unsigned char g_psc_enable_flags = 0;

void BOOT_TEXT_FLASH_LOC hal_psc_init(void)
{
    // Setup MCU wakeup mask
    psc->REG_080 = 0;
    psc->REG_084 = 0;

    // Setup BT wakeup mask
    psc->REG_090 = 0;
    psc->REG_094 = PSC_INTR_MASK2_WAKEUP_BT;

    // Setup WLAN wakeup mask
    psc->REG_0A0 = 0;
    psc->REG_0A4 = 0;

    psc->REG_124 |= PSC_AON_PWR_MEM_SEL_AON_MCU | PSC_AON_PWR_MEM_SEL_AON_BT |
        PSC_AON_PWR_MEM_SEL_AON_WF | PSC_AON_PWR_MEM_SEL_AON_A7;

    // Power down WLAN
    hal_psc_wlan_disable();
    // Power down BT
    hal_psc_bt_disable();
    // Power down A7
    hal_psc_a7_disable();
    // Power down Codec
    hal_psc_codec_disable();
}

void BOOT_TEXT_FLASH_LOC hal_psc_codec_enable(void)
{
    if(!g_psc_enable_flags)
    {
        g_psc_enable_flags = 1;
        psc->REG_078 = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
            PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
        hal_sys_timer_delay(MS_TO_TICKS(1));
        psc->REG_078 = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR |
            PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
        psc->REG_078 = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR |
            PSC_AON_CODEC_ISO_EN_DR |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
        psc->REG_078 = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR |
            PSC_AON_CODEC_ISO_EN_DR |
            PSC_AON_CODEC_CLK_STOP_DR;
    }

}

void BOOT_TEXT_FLASH_LOC hal_psc_codec_disable(void)
{
    if(g_psc_enable_flags)
    {
        g_psc_enable_flags = 0;
        psc->REG_078 = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR |
            PSC_AON_CODEC_ISO_EN_DR |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
        psc->REG_078 = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR |
            PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
        psc->REG_078 = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_PSW_EN_DR |
            PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
            PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
        psc->REG_078 = PSC_WRITE_ENABLE |
            PSC_AON_CODEC_PSW_EN_DR | PSC_AON_CODEC_PSW_EN_REG |
            PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
            PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
            PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    }
}

void BOOT_TEXT_FLASH_LOC hal_psc_bt_enable(void)
{
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR | PSC_AON_BT_RESETN_ASSERT_REG |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR |
        PSC_AON_BT_CLK_STOP_DR;

#ifdef CORE_SLEEP_POWER_DOWN
    psc->REG_020 = PSC_WRITE_ENABLE | PSC_AON_BT_PG_AUTO_EN | PSC_AON_BT_PG_HW_EN;
    psc->REG_038 = PSC_WRITE_ENABLE | 0;
#endif
}

void BOOT_TEXT_FLASH_LOC hal_psc_bt_disable(void)
{
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR | PSC_AON_BT_RESETN_ASSERT_REG |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR | PSC_AON_BT_PSW_EN_REG |
        PSC_AON_BT_RESETN_ASSERT_DR | PSC_AON_BT_RESETN_ASSERT_REG |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
}

void hal_psc_bt_enable_auto_power_down(void)
{
    psc->REG_020 = PSC_WRITE_ENABLE | PSC_AON_BT_PG_AUTO_EN | PSC_AON_BT_PG_HW_EN;
    psc->REG_038 = PSC_WRITE_ENABLE | 0;

    psc->REG_024 |= PSC_AON_BT_SLEEP_NO_WFI;
}

void BOOT_TEXT_FLASH_LOC hal_psc_wlan_enable(void)
{
    hal_cmu_wlan_set_sleep_allow(1);
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR | PSC_AON_WLAN_RESETN_ASSERT_REG |
        PSC_AON_WLAN_ISO_EN_DR | PSC_AON_WLAN_ISO_EN_REG |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR |
        PSC_AON_WLAN_ISO_EN_DR | PSC_AON_WLAN_ISO_EN_REG |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR |
        PSC_AON_WLAN_ISO_EN_DR |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR |
        PSC_AON_WLAN_ISO_EN_DR |
        PSC_AON_WLAN_CLK_STOP_DR;
#ifdef CORE_SLEEP_POWER_DOWN
    psc->REG_040 = PSC_WRITE_ENABLE | PSC_AON_WLAN_PG_AUTO_EN | PSC_AON_WLAN_PG_HW_EN;
    psc->REG_058 = PSC_WRITE_ENABLE | 0;
#endif
}

void BOOT_TEXT_FLASH_LOC hal_psc_wlan_disable(void)
{
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR |
        PSC_AON_WLAN_ISO_EN_DR |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR |
        PSC_AON_WLAN_ISO_EN_DR | PSC_AON_WLAN_ISO_EN_REG |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR | PSC_AON_WLAN_RESETN_ASSERT_REG |
        PSC_AON_WLAN_ISO_EN_DR | PSC_AON_WLAN_ISO_EN_REG |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR | PSC_AON_WLAN_PSW_EN_REG |
        PSC_AON_WLAN_RESETN_ASSERT_DR | PSC_AON_WLAN_RESETN_ASSERT_REG |
        PSC_AON_WLAN_ISO_EN_DR | PSC_AON_WLAN_ISO_EN_REG |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    hal_cmu_wlan_set_sleep_allow(0);
}

void BOOT_TEXT_FLASH_LOC hal_psc_a7_enable(void)
{
    psc->REG_0D8 = PSC_WRITE_ENABLE |
        PSC_AON_A7_PSW_EN_DR |
        PSC_AON_A7_RESETN_ASSERT_DR | PSC_AON_A7_RESETN_ASSERT_REG |
        PSC_AON_A7_ISO_EN_DR | PSC_AON_A7_ISO_EN_REG |
        PSC_AON_A7_CLK_STOP_DR | PSC_AON_A7_CLK_STOP_REG;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    psc->REG_0D8 = PSC_WRITE_ENABLE |
        PSC_AON_A7_PSW_EN_DR |
        PSC_AON_A7_RESETN_ASSERT_DR |
        PSC_AON_A7_ISO_EN_DR | PSC_AON_A7_ISO_EN_REG |
        PSC_AON_A7_CLK_STOP_DR | PSC_AON_A7_CLK_STOP_REG;
    psc->REG_0D8 = PSC_WRITE_ENABLE |
        PSC_AON_A7_PSW_EN_DR |
        PSC_AON_A7_RESETN_ASSERT_DR |
        PSC_AON_A7_ISO_EN_DR |
        PSC_AON_A7_CLK_STOP_DR | PSC_AON_A7_CLK_STOP_REG;
    psc->REG_0D8 = PSC_WRITE_ENABLE |
        PSC_AON_A7_PSW_EN_DR |
        PSC_AON_A7_RESETN_ASSERT_DR |
        PSC_AON_A7_ISO_EN_DR |
        PSC_AON_A7_CLK_STOP_DR;
}

void BOOT_TEXT_FLASH_LOC hal_psc_a7_disable(void)
{
    psc->REG_0D8 = PSC_WRITE_ENABLE |
        PSC_AON_A7_PSW_EN_DR |
        PSC_AON_A7_RESETN_ASSERT_DR |
        PSC_AON_A7_ISO_EN_DR |
        PSC_AON_A7_CLK_STOP_DR | PSC_AON_A7_CLK_STOP_REG;
    psc->REG_0D8 = PSC_WRITE_ENABLE |
        PSC_AON_A7_PSW_EN_DR |
        PSC_AON_A7_RESETN_ASSERT_DR |
        PSC_AON_A7_ISO_EN_DR | PSC_AON_A7_ISO_EN_REG |
        PSC_AON_A7_CLK_STOP_DR | PSC_AON_A7_CLK_STOP_REG;
    psc->REG_0D8 = PSC_WRITE_ENABLE |
        PSC_AON_A7_PSW_EN_DR |
        PSC_AON_A7_RESETN_ASSERT_DR | PSC_AON_A7_RESETN_ASSERT_REG |
        PSC_AON_A7_ISO_EN_DR | PSC_AON_A7_ISO_EN_REG |
        PSC_AON_A7_CLK_STOP_DR | PSC_AON_A7_CLK_STOP_REG;
    psc->REG_0D8 = PSC_WRITE_ENABLE |
        PSC_AON_A7_PSW_EN_DR | PSC_AON_A7_PSW_EN_REG |
        PSC_AON_A7_RESETN_ASSERT_DR | PSC_AON_A7_RESETN_ASSERT_REG |
        PSC_AON_A7_ISO_EN_DR | PSC_AON_A7_ISO_EN_REG |
        PSC_AON_A7_CLK_STOP_DR | PSC_AON_A7_CLK_STOP_REG;
}

void SRAM_TEXT_LOC hal_psc_dslp_force_on_bt_enable(void)
{
    psc->REG_124 |= PSC_AON_DSLP_FORCE_ON_BT_REG;
}

void SRAM_TEXT_LOC hal_psc_dslp_force_on_bt_disable(void)
{
    psc->REG_124 &= ~(PSC_AON_DSLP_FORCE_ON_BT_REG);
}

void SRAM_TEXT_LOC hal_psc_bt_auto_power_down(void)
{
    psc->REG_038 = PSC_WRITE_ENABLE | 0;
    psc->REG_020 = PSC_WRITE_ENABLE | PSC_AON_BT_PG_AUTO_EN | PSC_AON_BT_PG_HW_EN;
    psc->REG_030 = PSC_WRITE_ENABLE | PSC_AON_BT_POWERDN_START;
}

void SRAM_TEXT_LOC hal_psc_wlan_auto_power_down(void)
{
    psc->REG_058 = PSC_WRITE_ENABLE | 0;
    psc->REG_040 = PSC_WRITE_ENABLE | PSC_AON_WLAN_PG_AUTO_EN | PSC_AON_WLAN_PG_HW_EN;
    psc->REG_050 = PSC_WRITE_ENABLE | PSC_AON_WLAN_POWERDN_START;
}

void SRAM_TEXT_LOC hal_psc_core_auto_power_down(void)
{
    // 2003 MCU doesn't support power_down
}

void hal_psc_mcu_gpio_irq_enable(enum HAL_GPIO_PIN_T pin)
{
    if (pin < HAL_GPIO_PIN_NUM) {
        psc->REG_080 |= (1 << pin);
    }
}

void hal_psc_mcu_gpio_irq_disable(enum HAL_GPIO_PIN_T pin)
{
    if (pin < HAL_GPIO_PIN_NUM) {
        psc->REG_080 &= ~(1 << pin);
    }
}

uint32_t hal_psc_mcu_gpio_irq_get_status(uint32_t *status, uint32_t cnt)
{
    volatile uint32_t * const irq_status[] = {
        &psc->REG_088,
    };
    uint32_t i;

    if (cnt > ARRAY_SIZE(irq_status)) {
        cnt = ARRAY_SIZE(irq_status);
    }
    if (status) {
        for (i = 0; i < cnt; i++) {
            status[i] = *irq_status[i];
        }
    }
    return cnt;
}

void hal_psc_gpio_irq_enable(enum HAL_GPIO_PIN_T pin)
{
    hal_psc_mcu_gpio_irq_enable(pin);
}

void hal_psc_gpio_irq_disable(enum HAL_GPIO_PIN_T pin)
{
    hal_psc_mcu_gpio_irq_disable(pin);
}

uint32_t hal_psc_gpio_irq_get_status(uint32_t *status, uint32_t cnt)
{
    return hal_psc_mcu_gpio_irq_get_status(status, cnt);
}

uint32_t hal_psc_get_power_loop_cycle_cnt(void)
{
    return 6 + 14 + 14; // 1.5 + 4.5 + 14 + 14
}

