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
#ifndef __REG_WFCMU_BEST2003_H__
#define __REG_WFCMU_BEST2003_H__

#include "plat_types.h"

struct WLANCMU_T {
    __IO uint32_t HCLK_ENABLE;          // 0x00
    __IO uint32_t HCLK_DISABLE;         // 0x04
    __IO uint32_t PCLK_ENABLE;          // 0x08
    __IO uint32_t PCLK_DISABLE;         // 0x0C
    __IO uint32_t OCLK_ENABLE;          // 0x10
    __IO uint32_t OCLK_DISABLE;         // 0x14
         uint32_t RESERVED_018[(0xD8 - 0x18) / 4]; // 0x18
    __IO uint32_t CLK_OUT;              // 0xD8
};

// reg_000
#define WLAN_CMU_MANUAL_HCLK_ENABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_HCLK_ENABLE_MASK                    (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_HCLK_ENABLE_SHIFT                   (0)

// reg_004
#define WLAN_CMU_MANUAL_HCLK_DISABLE(n)                     (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_HCLK_DISABLE_MASK                   (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_HCLK_DISABLE_SHIFT                  (0)

// reg_008
#define WLAN_CMU_MANUAL_PCLK_ENABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_PCLK_ENABLE_MASK                    (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_PCLK_ENABLE_SHIFT                   (0)

// reg_00c
#define WLAN_CMU_MANUAL_PCLK_DISABLE(n)                     (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_PCLK_DISABLE_MASK                   (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_PCLK_DISABLE_SHIFT                  (0)

// reg_010
#define WLAN_CMU_MANUAL_OCLK_ENABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_OCLK_ENABLE_MASK                    (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_OCLK_ENABLE_SHIFT                   (0)

// reg_014
#define WLAN_CMU_MANUAL_OCLK_DISABLE(n)                     (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_OCLK_DISABLE_MASK                   (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_OCLK_DISABLE_SHIFT                  (0)

// reg_018
#define WLAN_CMU_MANUAL_MCLK_ENABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_MCLK_ENABLE_MASK                    (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_MCLK_ENABLE_SHIFT                   (0)

// reg_01c
#define WLAN_CMU_MANUAL_MCLK_DISABLE(n)                     (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_MCLK_DISABLE_MASK                   (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_MCLK_DISABLE_SHIFT                  (0)

// reg_020
#define WLAN_CMU_MANUAL_YCLK_ENABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_YCLK_ENABLE_MASK                    (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_YCLK_ENABLE_SHIFT                   (0)

// reg_024
#define WLAN_CMU_MANUAL_YCLK_DISABLE(n)                     (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_YCLK_DISABLE_MASK                   (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_YCLK_DISABLE_SHIFT                  (0)

// reg_028
#define WLAN_CMU_MANUAL_BCLK_ENABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_BCLK_ENABLE_MASK                    (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_BCLK_ENABLE_SHIFT                   (0)

// reg_02c
#define WLAN_CMU_MANUAL_BCLK_DISABLE(n)                     (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_BCLK_DISABLE_MASK                   (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_BCLK_DISABLE_SHIFT                  (0)

// reg_030
#define WLAN_CMU_MANUAL_RCLK_ENABLE(n)                      (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_RCLK_ENABLE_MASK                    (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_RCLK_ENABLE_SHIFT                   (0)

// reg_034
#define WLAN_CMU_MANUAL_RCLK_DISABLE(n)                     (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_RCLK_DISABLE_MASK                   (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_RCLK_DISABLE_SHIFT                  (0)

// reg_038
#define WLAN_CMU_MODE_HCLK(n)                               (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MODE_HCLK_MASK                             (0xFFFFFFFF << 0)
#define WLAN_CMU_MODE_HCLK_SHIFT                            (0)

// reg_03c
#define WLAN_CMU_MODE_PCLK(n)                               (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MODE_PCLK_MASK                             (0xFFFFFFFF << 0)
#define WLAN_CMU_MODE_PCLK_SHIFT                            (0)

// reg_040
#define WLAN_CMU_MODE_OCLK(n)                               (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MODE_OCLK_MASK                             (0xFFFFFFFF << 0)
#define WLAN_CMU_MODE_OCLK_SHIFT                            (0)

// reg_044
#define WLAN_CMU_MODE_MCLK(n)                               (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MODE_MCLK_MASK                             (0xFFFFFFFF << 0)
#define WLAN_CMU_MODE_MCLK_SHIFT                            (0)

// reg_048
#define WLAN_CMU_MODE_YCLK(n)                               (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MODE_YCLK_MASK                             (0xFFFFFFFF << 0)
#define WLAN_CMU_MODE_YCLK_SHIFT                            (0)

// reg_04c
#define WLAN_CMU_MODE_BCLK(n)                               (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MODE_BCLK_MASK                             (0xFFFFFFFF << 0)
#define WLAN_CMU_MODE_BCLK_SHIFT                            (0)

// reg_050
#define WLAN_CMU_MODE_RCLK(n)                               (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MODE_RCLK_MASK                             (0xFFFFFFFF << 0)
#define WLAN_CMU_MODE_RCLK_SHIFT                            (0)

// reg_054
#define WLAN_CMU_HRESETN_PULSE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_HRESETN_PULSE_MASK                         (0xFFFFFFFF << 0)
#define WLAN_CMU_HRESETN_PULSE_SHIFT                        (0)

// reg_058
#define WLAN_CMU_PRESETN_PULSE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_PRESETN_PULSE_MASK                         (0xFFFFFFFF << 0)
#define WLAN_CMU_PRESETN_PULSE_SHIFT                        (0)
#define WLAN_CMU_GLOBAL_RESETN_PULSE                        (1 << (WF_PRST_NUM+1-1))

// reg_05c
#define WLAN_CMU_ORESETN_PULSE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_ORESETN_PULSE_MASK                         (0xFFFFFFFF << 0)
#define WLAN_CMU_ORESETN_PULSE_SHIFT                        (0)

// reg_060
#define WLAN_CMU_HRESETN_SET(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_HRESETN_SET_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_HRESETN_SET_SHIFT                          (0)

// reg_064
#define WLAN_CMU_HRESETN_CLR(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_HRESETN_CLR_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_HRESETN_CLR_SHIFT                          (0)

// reg_068
#define WLAN_CMU_PRESETN_SET(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_PRESETN_SET_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_PRESETN_SET_SHIFT                          (0)
#define WLAN_CMU_GLOBAL_RESETN_SET                          (1 << (WF_PRST_NUM+1-1))

// reg_06c
#define WLAN_CMU_PRESETN_CLR(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_PRESETN_CLR_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_PRESETN_CLR_SHIFT                          (0)
#define WLAN_CMU_GLOBAL_RESETN_CLR                          (1 << (WF_PRST_NUM+1-1))

// reg_070
#define WLAN_CMU_ORESETN_SET(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_ORESETN_SET_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_ORESETN_SET_SHIFT                          (0)

// reg_074
#define WLAN_CMU_ORESETN_CLR(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_ORESETN_CLR_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_ORESETN_CLR_SHIFT                          (0)

// reg_078
#define WLAN_CMU_MRESETN_PULSE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MRESETN_PULSE_MASK                         (0xFFFFFFFF << 0)
#define WLAN_CMU_MRESETN_PULSE_SHIFT                        (0)

// reg_07c
#define WLAN_CMU_MRESETN_SET(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MRESETN_SET_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_MRESETN_SET_SHIFT                          (0)

// reg_080
#define WLAN_CMU_MRESETN_CLR(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MRESETN_CLR_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_MRESETN_CLR_SHIFT                          (0)

// reg_084
#define WLAN_CMU_YRESETN_PULSE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_YRESETN_PULSE_MASK                         (0xFFFFFFFF << 0)
#define WLAN_CMU_YRESETN_PULSE_SHIFT                        (0)

// reg_088
#define WLAN_CMU_YRESETN_SET(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_YRESETN_SET_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_YRESETN_SET_SHIFT                          (0)

// reg_08c
#define WLAN_CMU_YRESETN_CLR(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_YRESETN_CLR_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_YRESETN_CLR_SHIFT                          (0)

// reg_090
#define WLAN_CMU_BRESETN_PULSE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_BRESETN_PULSE_MASK                         (0xFFFFFFFF << 0)
#define WLAN_CMU_BRESETN_PULSE_SHIFT                        (0)

// reg_094
#define WLAN_CMU_BRESETN_SET(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_BRESETN_SET_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_BRESETN_SET_SHIFT                          (0)

// reg_098
#define WLAN_CMU_BRESETN_CLR(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_BRESETN_CLR_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_BRESETN_CLR_SHIFT                          (0)

// reg_09c
#define WLAN_CMU_RRESETN_PULSE(n)                           (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_RRESETN_PULSE_MASK                         (0xFFFFFFFF << 0)
#define WLAN_CMU_RRESETN_PULSE_SHIFT                        (0)

// reg_0a0
#define WLAN_CMU_RRESETN_SET(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_RRESETN_SET_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_RRESETN_SET_SHIFT                          (0)

// reg_0a4
#define WLAN_CMU_RRESETN_CLR(n)                             (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_RRESETN_CLR_MASK                           (0xFFFFFFFF << 0)
#define WLAN_CMU_RRESETN_CLR_SHIFT                          (0)

// reg_0a8
#define WLAN_CMU_WATCHDOG_RESET                             (1 << 0)
#define WLAN_CMU_SOFT_GLOBLE_RESET                          (1 << 1)
#define WLAN_CMU_RTC_INTR_H                                 (1 << 2)
#define WLAN_CMU_CHG_INTR_H                                 (1 << 3)
#define WLAN_CMU_SOFT_BOOT_MODE(n)                          (((n) & 0xFFFFFFF) << 4)
#define WLAN_CMU_SOFT_BOOT_MODE_MASK                        (0xFFFFFFF << 4)
#define WLAN_CMU_SOFT_BOOT_MODE_SHIFT                       (4)

// reg_0ac
#define WLAN_CMU_EN_CLK_PLL_ENABLE                          (1 << 0)
#define WLAN_CMU_SEL_160_HCLK_ENABLE                        (1 << 1)
#define WLAN_CMU_SEL_40M_HCLK_ENABLE                        (1 << 2)
#define WLAN_CMU_SEL_160_PCLK_ENABLE                        (1 << 3)
#define WLAN_CMU_SEL_40M_PCLK_ENABLE                        (1 << 4)
#define WLAN_CMU_SEL_160_MAC_ENABLE                         (1 << 5)
#define WLAN_CMU_SEL_40M_MAC_ENABLE                         (1 << 6)
#define WLAN_CMU_SEL_40M_PHY_ENABLE                         (1 << 7)
#define WLAN_CMU_SEL_OSCX2_UART_ENABLE                      (1 << 8)
#define WLAN_CMU_SEL_OSCX2_SPI_ENABLE                       (1 << 9)
#define WLAN_CMU_POL_CLK_ADC_ENABLE                         (1 << 10)
#define WLAN_CMU_POL_CLK_ADCAUX_ENABLE                      (1 << 11)
#define WLAN_CMU_POL_CLK_SDIO_ENABLE                        (1 << 12)
#define WLAN_CMU_POL_CLK_TXD_ENABLE                         (1 << 13)
#define WLAN_CMU_EN_CLK_DPD_ENABLE                          (1 << 14)
#define WLAN_CMU_SEL_120_DPD_ENABLE                         (1 << 15)
#define WLAN_CMU_SEL_HIGH_DPD_ENABLE                        (1 << 16)
#define WLAN_CMU_SEL_DUMPRX_MAC_ENABLE                      (1 << 17)
#define WLAN_CMU_SEL_DUMPTX_MAC_ENABLE                      (1 << 18)
#define WLAN_CMU_SEL_DUMPTX_80M_ENABLE                      (1 << 19)
#define WLAN_CMU_WAKEUP_RF_ENABLE                           (1 << 20)

// reg_0b0
#define WLAN_CMU_EN_CLK_PLL_DISABLE                         (1 << 0)
#define WLAN_CMU_SEL_160_HCLK_DISABLE                       (1 << 1)
#define WLAN_CMU_SEL_40M_HCLK_DISABLE                       (1 << 2)
#define WLAN_CMU_SEL_160_PCLK_DISABLE                       (1 << 3)
#define WLAN_CMU_SEL_40M_PCLK_DISABLE                       (1 << 4)
#define WLAN_CMU_SEL_160_MAC_DISABLE                        (1 << 5)
#define WLAN_CMU_SEL_40M_MAC_DISABLE                        (1 << 6)
#define WLAN_CMU_SEL_40M_PHY_DISABLE                        (1 << 7)
#define WLAN_CMU_SEL_OSCX2_UART_DISABLE                     (1 << 8)
#define WLAN_CMU_SEL_OSCX2_SPI_DISABLE                      (1 << 9)
#define WLAN_CMU_POL_CLK_ADC_DISABLE                        (1 << 10)
#define WLAN_CMU_POL_CLK_ADCAUX_DISABLE                     (1 << 11)
#define WLAN_CMU_POL_CLK_SDIO_DISABLE                       (1 << 12)
#define WLAN_CMU_POL_CLK_TXD_DISABLE                        (1 << 13)
#define WLAN_CMU_EN_CLK_DPD_DISABLE                         (1 << 14)
#define WLAN_CMU_SEL_120_DPD_DISABLE                        (1 << 15)
#define WLAN_CMU_SEL_HIGH_DPD_DISABLE                       (1 << 16)
#define WLAN_CMU_SEL_DUMPRX_MAC_DISABLE                     (1 << 17)
#define WLAN_CMU_SEL_DUMPTX_MAC_DISABLE                     (1 << 18)
#define WLAN_CMU_SEL_DUMPTX_80M_DISABLE                     (1 << 19)
#define WLAN_CMU_WAKEUP_RF_DISABLE                          (1 << 20)

// reg_0b4
#define WLAN_CMU_WRITE_UNLOCK_H                             (1 << 0)
#define WLAN_CMU_WRITE_UNLOCK_STATUS                        (1 << 1)

// reg_0b8
#define WLAN_CMU_WAKEUP_IRQ_MASK(n)                         (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_WAKEUP_IRQ_MASK_MASK                       (0xFFFFFFFF << 0)
#define WLAN_CMU_WAKEUP_IRQ_MASK_SHIFT                      (0)

// reg_0bc
#define WLAN_CMU_INTR2MCU_MASK_SET(n)                       (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_INTR2MCU_MASK_SET_MASK                     (0xFFFFFFFF << 0)
#define WLAN_CMU_INTR2MCU_MASK_SET_SHIFT                    (0)

// reg_0c0
#define WLAN_CMU_INTR2MCU_MASK_CLR(n)                       (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_INTR2MCU_MASK_CLR_MASK                     (0xFFFFFFFF << 0)
#define WLAN_CMU_INTR2MCU_MASK_CLR_SHIFT                    (0)

// reg_0c4
#define WLAN_CMU_MEMSC0                                     (1 << 0)

// reg_0c8
#define WLAN_CMU_MEMSC1                                     (1 << 0)

// reg_0cc
#define WLAN_CMU_MEMSC2                                     (1 << 0)

// reg_0d0
#define WLAN_CMU_MEMSC3                                     (1 << 0)

// reg_0d4
#define WLAN_CMU_MEMSC_STATUS0                              (1 << 0)
#define WLAN_CMU_MEMSC_STATUS1                              (1 << 1)
#define WLAN_CMU_MEMSC_STATUS2                              (1 << 2)
#define WLAN_CMU_MEMSC_STATUS3                              (1 << 3)

// reg_0d8
#define WLAN_CMU_CFG_CLK_OUT(n)                             (((n) & 0xF) << 0)
#define WLAN_CMU_CFG_CLK_OUT_MASK                           (0xF << 0)
#define WLAN_CMU_CFG_CLK_OUT_SHIFT                          (0)
#define WLAN_CMU_FORCE_CLK_OSC_ON                           (1 << 4)
#define WLAN_CMU_FORCE_CLK_OSCX2_ON                         (1 << 5)
#define WLAN_CMU_FORCE_CLK_PLL_ON                           (1 << 6)
#define WLAN_CMU_FORCE_WAKEUP                               (1 << 7)
#define WLAN_CMU_CFG_ENABLE_WAKE_REV1                       (1 << 8)
#define WLAN_CMU_CFG_ENABLE_WAKE_REV2                       (1 << 9)
#define WLAN_CMU_CFG_ENABLE_CPU_WAKE                        (1 << 10)
#define WLAN_CMU_CFG_ENABLE_SDIO_WAKE                       (1 << 11)
#define WLAN_CMU_CFG_POL_WAKE_REV1                          (1 << 12)
#define WLAN_CMU_CFG_POL_WAKE_REV2                          (1 << 13)
#define WLAN_CMU_CFG_POL_SDIO_WAKE                          (1 << 14)
#define WLAN_CMU_CFG_SLEEP_MODE                             (1 << 15)
#define WLAN_CMU_CFG_OSC_STABLE_TIME(n)                     (((n) & 0xFF) << 16)
#define WLAN_CMU_CFG_OSC_STABLE_TIME_MASK                   (0xFF << 16)
#define WLAN_CMU_CFG_OSC_STABLE_TIME_SHIFT                  (16)
#define WLAN_CMU_CFG_PLL_STABLE_TIME(n)                     (((n) & 0xFF) << 24)
#define WLAN_CMU_CFG_PLL_STABLE_TIME_MASK                   (0xFF << 24)
#define WLAN_CMU_CFG_PLL_STABLE_TIME_SHIFT                  (24)

// reg_0dc
#define WLAN_CMU_MANUAL_YCLK_ENABLE2(n)                     (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_YCLK_ENABLE2_MASK                   (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_YCLK_ENABLE2_SHIFT                  (0)

// reg_0e0
#define WLAN_CMU_MANUAL_YCLK_DISABLE2(n)                    (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MANUAL_YCLK_DISABLE2_MASK                  (0xFFFFFFFF << 0)
#define WLAN_CMU_MANUAL_YCLK_DISABLE2_SHIFT                 (0)

// reg_0e4
#define WLAN_CMU_MODE_YCLK2(n)                              (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_MODE_YCLK2_MASK                            (0xFFFFFFFF << 0)
#define WLAN_CMU_MODE_YCLK2_SHIFT                           (0)

// reg_0e8
#define WLAN_CMU_YRESETN_PULSE2(n)                          (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_YRESETN_PULSE2_MASK                        (0xFFFFFFFF << 0)
#define WLAN_CMU_YRESETN_PULSE2_SHIFT                       (0)

// reg_0ec
#define WLAN_CMU_YRESETN_SET2(n)                            (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_YRESETN_SET2_MASK                          (0xFFFFFFFF << 0)
#define WLAN_CMU_YRESETN_SET2_SHIFT                         (0)

// reg_0f0
#define WLAN_CMU_YRESETN_CLR2(n)                            (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_YRESETN_CLR2_MASK                          (0xFFFFFFFF << 0)
#define WLAN_CMU_YRESETN_CLR2_SHIFT                         (0)

// reg_0f4
#define WLAN_CMU_INTR2BT_MASK_SET(n)                        (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_INTR2BT_MASK_SET_MASK                      (0xFFFFFFFF << 0)
#define WLAN_CMU_INTR2BT_MASK_SET_SHIFT                     (0)

// reg_0f8
#define WLAN_CMU_INTR2BT_MASK_CLR(n)                        (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_INTR2BT_MASK_CLR_MASK                      (0xFFFFFFFF << 0)
#define WLAN_CMU_INTR2BT_MASK_CLR_SHIFT                     (0)

// reg_0fc
#define WLAN_CMU_CFG_OSC_EXT_TIMER(n)                       (((n) & 0xFFFF) << 0)
#define WLAN_CMU_CFG_OSC_EXT_TIMER_MASK                     (0xFFFF << 0)
#define WLAN_CMU_CFG_OSC_EXT_TIMER_SHIFT                    (0)

// reg_100
#define WLAN_CMU_CFG_SLEEP_TARGET(n)                        (((n) & 0xFFFFFF) << 0)
#define WLAN_CMU_CFG_SLEEP_TARGET_MASK                      (0xFFFFFF << 0)
#define WLAN_CMU_CFG_SLEEP_TARGET_SHIFT                     (0)

// reg_104
#define WLAN_CMU_CFG_SLEEP_INTR_CLR(n)                      (((n) & 0x3) << 0)
#define WLAN_CMU_CFG_SLEEP_INTR_CLR_MASK                    (0x3 << 0)
#define WLAN_CMU_CFG_SLEEP_INTR_CLR_SHIFT                   (0)
#define WLAN_CMU_RB_INTR_PENDING(n)                         (((n) & 0x3) << 2)
#define WLAN_CMU_RB_INTR_PENDING_MASK                       (0x3 << 2)
#define WLAN_CMU_RB_INTR_PENDING_SHIFT                      (2)
#define WLAN_CMU_RB_COUNT_DONE                              (1 << 4)

// reg_108
#define WLAN_CMU_RB_STATE_32K(n)                            (((n) & 0xF) << 0)
#define WLAN_CMU_RB_STATE_32K_MASK                          (0xF << 0)
#define WLAN_CMU_RB_STATE_32K_SHIFT                         (0)
#define WLAN_CMU_RB_STATE_26M(n)                            (((n) & 0xF) << 4)
#define WLAN_CMU_RB_STATE_26M_MASK                          (0xF << 4)
#define WLAN_CMU_RB_STATE_26M_SHIFT                         (4)
#define WLAN_CMU_RB_M4_INTR_ALL                             (1 << 8)
#define WLAN_CMU_RB_WAKE_REV2_INNER                         (1 << 9)
#define WLAN_CMU_RB_WAKE_REV1_INNER                         (1 << 10)
#define WLAN_CMU_RB_INTR_SLEEP(n)                           (((n) & 0x3) << 11)
#define WLAN_CMU_RB_INTR_SLEEP_MASK                         (0x3 << 11)
#define WLAN_CMU_RB_INTR_SLEEP_SHIFT                        (11)
#define WLAN_CMU_RB_WAKE_COMBO                              (1 << 13)
#define WLAN_CMU_RB_WAKE_SDIO                               (1 << 14)

// reg_10c
#define WLAN_CMU_RB_LP_TIMER32K_VALUE(n)                    (((n) & 0xFFFFFF) << 0)
#define WLAN_CMU_RB_LP_TIMER32K_VALUE_MASK                  (0xFFFFFF << 0)
#define WLAN_CMU_RB_LP_TIMER32K_VALUE_SHIFT                 (0)

// reg_110
#define WLAN_CMU_RB_LP_TIMER26M_VALUE(n)                    (((n) & 0xFFFF) << 0)
#define WLAN_CMU_RB_LP_TIMER26M_VALUE_MASK                  (0xFFFF << 0)
#define WLAN_CMU_RB_LP_TIMER26M_VALUE_SHIFT                 (0)
#define WLAN_CMU_RB_CALIB_TIMER_VALUE(n)                    (((n) & 0xFFFF) << 16)
#define WLAN_CMU_RB_CALIB_TIMER_VALUE_MASK                  (0xFFFF << 16)
#define WLAN_CMU_RB_CALIB_TIMER_VALUE_SHIFT                 (16)

// reg_114
#define WLAN_CMU_CFG_CALIB_TARGET(n)                        (((n) & 0xFF) << 0)
#define WLAN_CMU_CFG_CALIB_TARGET_MASK                      (0xFF << 0)
#define WLAN_CMU_CFG_CALIB_TARGET_SHIFT                     (0)
#define WLAN_CMU_CFG_CALIB_START                            (1 << 8)
#define WLAN_CMU_CFG_CALIB_MODE                             (1 << 9)

// reg_118
#define WLAN_CMU_CFG_SLEEP_INTR_MASK(n)                     (((n) & 0x3) << 0)
#define WLAN_CMU_CFG_SLEEP_INTR_MASK_MASK                   (0x3 << 0)
#define WLAN_CMU_CFG_SLEEP_INTR_MASK_SHIFT                  (0)
#define WLAN_CMU_CFG_INTR_PULSE_WIDTH(n)                    (((n) & 0x7) << 2)
#define WLAN_CMU_CFG_INTR_PULSE_WIDTH_MASK                  (0x7 << 2)
#define WLAN_CMU_CFG_INTR_PULSE_WIDTH_SHIFT                 (2)

// reg_120
#define WLAN_CMU_CFG_DR_CPU_RAM_SMIC40(n)                   (((n) & 0xFF) << 0)
#define WLAN_CMU_CFG_DR_CPU_RAM_SMIC40_MASK                 (0xFF << 0)
#define WLAN_CMU_CFG_DR_CPU_RAM_SMIC40_SHIFT                (0)
#define WLAN_CMU_CFG_DR_PHY_RAM_SMIC40(n)                   (((n) & 0xFF) << 8)
#define WLAN_CMU_CFG_DR_PHY_RAM_SMIC40_MASK                 (0xFF << 8)
#define WLAN_CMU_CFG_DR_PHY_RAM_SMIC40_SHIFT                (8)
#define WLAN_CMU_CFG_DR_ROM_SMIC40(n)                       (((n) & 0xFF) << 16)
#define WLAN_CMU_CFG_DR_ROM_SMIC40_MASK                     (0xFF << 16)
#define WLAN_CMU_CFG_DR_ROM_SMIC40_SHIFT                    (16)

// reg_124
#define WLAN_CMU_CFG_REG_CPU_RAM_SMIC40(n)                  (((n) & 0xFF) << 0)
#define WLAN_CMU_CFG_REG_CPU_RAM_SMIC40_MASK                (0xFF << 0)
#define WLAN_CMU_CFG_REG_CPU_RAM_SMIC40_SHIFT               (0)
#define WLAN_CMU_CFG_REG_PHY_RAM_SMIC40(n)                  (((n) & 0xFF) << 8)
#define WLAN_CMU_CFG_REG_PHY_RAM_SMIC40_MASK                (0xFF << 8)
#define WLAN_CMU_CFG_REG_PHY_RAM_SMIC40_SHIFT               (8)
#define WLAN_CMU_CFG_REG_ROM_SMIC40(n)                      (((n) & 0xFF) << 16)
#define WLAN_CMU_CFG_REG_ROM_SMIC40_MASK                    (0xFF << 16)
#define WLAN_CMU_CFG_REG_ROM_SMIC40_SHIFT                   (16)

// reg_128
#define WLAN_CMU_CFG_DR_CPU_RAM_TSMC40LP(n)                 (((n) & 0xFF) << 0)
#define WLAN_CMU_CFG_DR_CPU_RAM_TSMC40LP_MASK               (0xFF << 0)
#define WLAN_CMU_CFG_DR_CPU_RAM_TSMC40LP_SHIFT              (0)
#define WLAN_CMU_CFG_DR_PHY_RAM_TSMC40LP(n)                 (((n) & 0xFF) << 8)
#define WLAN_CMU_CFG_DR_PHY_RAM_TSMC40LP_MASK               (0xFF << 8)
#define WLAN_CMU_CFG_DR_PHY_RAM_TSMC40LP_SHIFT              (8)
#define WLAN_CMU_CFG_DR_ROM_TSMC40LP(n)                     (((n) & 0xFF) << 16)
#define WLAN_CMU_CFG_DR_ROM_TSMC40LP_MASK                   (0xFF << 16)
#define WLAN_CMU_CFG_DR_ROM_TSMC40LP_SHIFT                  (16)

// reg_12c
#define WLAN_CMU_CFG_REG_CPU_RAM_TSMC40LP(n)                (((n) & 0xFF) << 0)
#define WLAN_CMU_CFG_REG_CPU_RAM_TSMC40LP_MASK              (0xFF << 0)
#define WLAN_CMU_CFG_REG_CPU_RAM_TSMC40LP_SHIFT             (0)
#define WLAN_CMU_CFG_REG_PHY_RAM_TSMC40LP(n)                (((n) & 0xFF) << 8)
#define WLAN_CMU_CFG_REG_PHY_RAM_TSMC40LP_MASK              (0xFF << 8)
#define WLAN_CMU_CFG_REG_PHY_RAM_TSMC40LP_SHIFT             (8)
#define WLAN_CMU_CFG_REG_ROM_TSMC40LP(n)                    (((n) & 0xFF) << 16)
#define WLAN_CMU_CFG_REG_ROM_TSMC40LP_MASK                  (0xFF << 16)
#define WLAN_CMU_CFG_REG_ROM_TSMC40LP_SHIFT                 (16)

// reg_130
#define WLAN_CMU_EXTEND_TARGET_REG(n)                       (((n) & 0x3F) << 0)
#define WLAN_CMU_EXTEND_TARGET_REG_MASK                     (0x3F << 0)
#define WLAN_CMU_EXTEND_TARGET_REG_SHIFT                    (0)
#define WLAN_CMU_EXTEND_TARGET_SMC(n)                       (((n) & 0x3F) << 6)
#define WLAN_CMU_EXTEND_TARGET_SMC_MASK                     (0x3F << 6)
#define WLAN_CMU_EXTEND_TARGET_SMC_SHIFT                    (6)
#define WLAN_CMU_CFG_SDIO_TYPE_SEL(n)                       (((n) & 0x3) << 12)
#define WLAN_CMU_CFG_SDIO_TYPE_SEL_MASK                     (0x3 << 12)
#define WLAN_CMU_CFG_SDIO_TYPE_SEL_SHIFT                    (12)
#define WLAN_CMU_CFG_CRYP_MODE                              (1 << 14)
#define WLAN_CMU_CFG_POL_RFSPI_CLK_OUT                      (1 << 15)
#define WLAN_CMU_CFG_POL_IRQ2HOST                           (1 << 16)
#define WLAN_CMU_CFG_ENABLE_IRQ2HOST                        (1 << 17)
#define WLAN_CMU_EN_DPD_CALIB                               (1 << 18)
#define WLAN_CMU_CFG_WLAN_RDY2SDIO                          (1 << 19)

// reg_134
#define WLAN_CMU_TPORTS_SEL_WLAN(n)                         (((n) & 0xFF) << 0)
#define WLAN_CMU_TPORTS_SEL_WLAN_MASK                       (0xFF << 0)
#define WLAN_CMU_TPORTS_SEL_WLAN_SHIFT                      (0)
#define WLAN_CMU_TPORTS_SEL_SDIO(n)                         (((n) & 0xFF) << 8)
#define WLAN_CMU_TPORTS_SEL_SDIO_MASK                       (0xFF << 8)
#define WLAN_CMU_TPORTS_SEL_SDIO_SHIFT                      (8)
#define WLAN_CMU_TPORTS_SEL_APB(n)                          (((n) & 0xFF) << 16)
#define WLAN_CMU_TPORTS_SEL_APB_MASK                        (0xFF << 16)
#define WLAN_CMU_TPORTS_SEL_APB_SHIFT                       (16)
#define WLAN_CMU_TPORT_SEL(n)                               (((n) & 0xFF) << 24)
#define WLAN_CMU_TPORT_SEL_MASK                             (0xFF << 24)
#define WLAN_CMU_TPORT_SEL_SHIFT                            (24)

// reg_138
#define WLAN_CMU_TPORTS_MUX0700(n)                          (((n) & 0x3) << 0)
#define WLAN_CMU_TPORTS_MUX0700_MASK                        (0x3 << 0)
#define WLAN_CMU_TPORTS_MUX0700_SHIFT                       (0)
#define WLAN_CMU_TPORTS_MUX1508(n)                          (((n) & 0x3) << 2)
#define WLAN_CMU_TPORTS_MUX1508_MASK                        (0x3 << 2)
#define WLAN_CMU_TPORTS_MUX1508_SHIFT                       (2)
#define WLAN_CMU_TPORTS_MUX2316(n)                          (((n) & 0x3) << 4)
#define WLAN_CMU_TPORTS_MUX2316_MASK                        (0x3 << 4)
#define WLAN_CMU_TPORTS_MUX2316_SHIFT                       (4)
#define WLAN_CMU_TPORTS_MUX3124(n)                          (((n) & 0x3) << 6)
#define WLAN_CMU_TPORTS_MUX3124_MASK                        (0x3 << 6)
#define WLAN_CMU_TPORTS_MUX3124_SHIFT                       (6)
#define WLAN_CMU_REV_AUX1(n)                                (((n) & 0xFF) << 8)
#define WLAN_CMU_REV_AUX1_MASK                              (0xFF << 8)
#define WLAN_CMU_REV_AUX1_SHIFT                             (8)
#define WLAN_CMU_REV_AUX2(n)                                (((n) & 0xFF) << 16)
#define WLAN_CMU_REV_AUX2_MASK                              (0xFF << 16)
#define WLAN_CMU_REV_AUX2_SHIFT                             (16)
#define WLAN_CMU_REV_AUX3(n)                                (((n) & 0xFF) << 24)
#define WLAN_CMU_REV_AUX3_MASK                              (0xFF << 24)
#define WLAN_CMU_REV_AUX3_SHIFT                             (24)

// reg_13c
#define WLAN_CMU_REV_00(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_REV_00_MASK                                (0xFFFFFFFF << 0)
#define WLAN_CMU_REV_00_SHIFT                               (0)

// reg_140
#define WLAN_CMU_REV_01(n)                                  (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_REV_01_MASK                                (0xFFFFFFFF << 0)
#define WLAN_CMU_REV_01_SHIFT                               (0)

// reg_1f0
#define WLAN_CMU_RESERVED(n)                                (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_RESERVED_MASK                              (0xFFFFFFFF << 0)
#define WLAN_CMU_RESERVED_SHIFT                             (0)

// reg_1f4
#define WLAN_CMU_DEBUG(n)                                   (((n) & 0xFFFFFFFF) << 0)
#define WLAN_CMU_DEBUG_MASK                                 (0xFFFFFFFF << 0)
#define WLAN_CMU_DEBUG_SHIFT                                (0)

#endif
