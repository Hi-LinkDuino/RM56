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
#ifndef __REG_PSC_BEST2003_H__
#define __REG_PSC_BEST2003_H__

#include "plat_types.h"

struct AONPSC_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    __IO uint32_t REG_008;
    __IO uint32_t REG_00C;
    __IO uint32_t REG_010;
    __IO uint32_t REG_014;
    __IO uint32_t REG_018;
    __IO uint32_t REG_01C;
    __IO uint32_t REG_020;
    __IO uint32_t REG_024;
    __IO uint32_t REG_028;
    __IO uint32_t REG_02C;
    __IO uint32_t REG_030;
    __IO uint32_t REG_034;
    __IO uint32_t REG_038;
    __IO uint32_t REG_03C;
    __IO uint32_t REG_040;
    __IO uint32_t REG_044;
    __IO uint32_t REG_048;
    __IO uint32_t REG_04C;
    __IO uint32_t REG_050;
    __IO uint32_t REG_054;
    __IO uint32_t REG_058;
    __IO uint32_t REG_05C;
    __IO uint32_t REG_060;
    __IO uint32_t REG_064;
    __IO uint32_t REG_068;
    __IO uint32_t REG_06C;
    __IO uint32_t REG_070;
    __IO uint32_t REG_074;
    __IO uint32_t REG_078;
    __IO uint32_t REG_07C;
    __IO uint32_t REG_080;
    __IO uint32_t REG_084;
    __IO uint32_t REG_088;
    __IO uint32_t REG_08C;
    __IO uint32_t REG_090;
    __IO uint32_t REG_094;
    __IO uint32_t REG_098;
    __IO uint32_t REG_09C;
    __IO uint32_t REG_0A0;
    __IO uint32_t REG_0A4;
    __IO uint32_t REG_0A8;
    __IO uint32_t REG_0AC;
    __IO uint32_t REG_0B0;
    __IO uint32_t REG_0B4;
    __IO uint32_t REG_0B8;
    __IO uint32_t REG_0BC;
    __IO uint32_t REG_0C0;
    __IO uint32_t REG_0C4;
    __IO uint32_t REG_0C8;
    __IO uint32_t REG_0CC;
    __IO uint32_t REG_0D0;
    __IO uint32_t REG_0D4;
    __IO uint32_t REG_0D8;
    __IO uint32_t REG_0DC;
    __IO uint32_t REG_0E0;
    __IO uint32_t REG_0E4;
    __IO uint32_t REG_0E8;
    __IO uint32_t REG_0EC;
    __IO uint32_t REG_0F0;
    __IO uint32_t REG_0F4;
    __IO uint32_t REG_0F8;
    __IO uint32_t REG_0FC;
    __IO uint32_t REG_100;
    __IO uint32_t REG_104;
    __IO uint32_t REG_108;
    __IO uint32_t REG_10C;
    __IO uint32_t REG_110;
    __IO uint32_t REG_114;
    __IO uint32_t REG_118;
    __IO uint32_t REG_11C;
    __IO uint32_t REG_120;
    __IO uint32_t REG_124;
    __IO uint32_t REG_128;
    __IO uint32_t REG_12C;
    __IO uint32_t REG_130;
    __IO uint32_t REG_134;
    __IO uint32_t REG_138;
    __IO uint32_t REG_13C;
    __IO uint32_t REG_140;
    __IO uint32_t REG_144;
};

// reg_000
#define PSC_AON_MCU_PG_AUTO_EN                              (1 << 0)
#define PSC_AON_MCU_PG_HW_EN                                (1 << 1)

// reg_004
#define PSC_AON_MCU_PSW_ACK_VALID                           (1 << 0)
#define PSC_AON_MCU_RESERVED(n)                             (((n) & 0x7F) << 1)
#define PSC_AON_MCU_RESERVED_MASK                           (0x7F << 1)
#define PSC_AON_MCU_RESERVED_SHIFT                          (1)
#define PSC_AON_MCU_MAIN_STATE(n)                           (((n) & 0x3) << 8)
#define PSC_AON_MCU_MAIN_STATE_MASK                         (0x3 << 8)
#define PSC_AON_MCU_MAIN_STATE_SHIFT                        (8)
#define PSC_AON_MCU_POWERDN_STATE(n)                        (((n) & 0x7) << 10)
#define PSC_AON_MCU_POWERDN_STATE_MASK                      (0x7 << 10)
#define PSC_AON_MCU_POWERDN_STATE_SHIFT                     (10)
#define PSC_AON_MCU_POWERUP_STATE(n)                        (((n) & 0x7) << 13)
#define PSC_AON_MCU_POWERUP_STATE_MASK                      (0x7 << 13)
#define PSC_AON_MCU_POWERUP_STATE_SHIFT                     (13)

// reg_008
#define PSC_AON_MCU_POWERDN_TIMER1(n)                       (((n) & 0x3F) << 0)
#define PSC_AON_MCU_POWERDN_TIMER1_MASK                     (0x3F << 0)
#define PSC_AON_MCU_POWERDN_TIMER1_SHIFT                    (0)
#define PSC_AON_MCU_POWERDN_TIMER2(n)                       (((n) & 0x3F) << 6)
#define PSC_AON_MCU_POWERDN_TIMER2_MASK                     (0x3F << 6)
#define PSC_AON_MCU_POWERDN_TIMER2_SHIFT                    (6)
#define PSC_AON_MCU_POWERDN_TIMER3(n)                       (((n) & 0x3F) << 12)
#define PSC_AON_MCU_POWERDN_TIMER3_MASK                     (0x3F << 12)
#define PSC_AON_MCU_POWERDN_TIMER3_SHIFT                    (12)
#define PSC_AON_MCU_POWERDN_TIMER4(n)                       (((n) & 0x3F) << 18)
#define PSC_AON_MCU_POWERDN_TIMER4_MASK                     (0x3F << 18)
#define PSC_AON_MCU_POWERDN_TIMER4_SHIFT                    (18)
#define PSC_AON_MCU_POWERDN_TIMER5(n)                       (((n) & 0xFF) << 24)
#define PSC_AON_MCU_POWERDN_TIMER5_MASK                     (0xFF << 24)
#define PSC_AON_MCU_POWERDN_TIMER5_SHIFT                    (24)

// reg_00c
#define PSC_AON_MCU_POWERUP_TIMER1(n)                       (((n) & 0x3F) << 0)
#define PSC_AON_MCU_POWERUP_TIMER1_MASK                     (0x3F << 0)
#define PSC_AON_MCU_POWERUP_TIMER1_SHIFT                    (0)
#define PSC_AON_MCU_POWERUP_TIMER2(n)                       (((n) & 0xFF) << 6)
#define PSC_AON_MCU_POWERUP_TIMER2_MASK                     (0xFF << 6)
#define PSC_AON_MCU_POWERUP_TIMER2_SHIFT                    (6)
#define PSC_AON_MCU_POWERUP_TIMER3(n)                       (((n) & 0x3F) << 14)
#define PSC_AON_MCU_POWERUP_TIMER3_MASK                     (0x3F << 14)
#define PSC_AON_MCU_POWERUP_TIMER3_SHIFT                    (14)
#define PSC_AON_MCU_POWERUP_TIMER4(n)                       (((n) & 0x3F) << 20)
#define PSC_AON_MCU_POWERUP_TIMER4_MASK                     (0x3F << 20)
#define PSC_AON_MCU_POWERUP_TIMER4_SHIFT                    (20)
#define PSC_AON_MCU_POWERUP_TIMER5(n)                       (((n) & 0x3F) << 26)
#define PSC_AON_MCU_POWERUP_TIMER5_MASK                     (0x3F << 26)
#define PSC_AON_MCU_POWERUP_TIMER5_SHIFT                    (26)

// reg_010
#define PSC_AON_MCU_POWERDN_START                           (1 << 0)

// reg_014
#define PSC_AON_MCU_POWERUP_START                           (1 << 0)

// reg_018
#define PSC_AON_MCU_CLK_STOP_REG                            (1 << 0)
#define PSC_AON_MCU_ISO_EN_REG                              (1 << 1)
#define PSC_AON_MCU_RESETN_ASSERT_REG                       (1 << 2)
#define PSC_AON_MCU_PSW_EN_REG                              (1 << 3)
#define PSC_AON_MCU_CLK_STOP_DR                             (1 << 4)
#define PSC_AON_MCU_ISO_EN_DR                               (1 << 5)
#define PSC_AON_MCU_RESETN_ASSERT_DR                        (1 << 6)
#define PSC_AON_MCU_PSW_EN_DR                               (1 << 7)

#if 0
// reg_01c
#define PSC_AON_MCU_MAIN_STATE(n)                           (((n) & 0x3) << 0)
#define PSC_AON_MCU_MAIN_STATE_MASK                         (0x3 << 0)
#define PSC_AON_MCU_MAIN_STATE_SHIFT                        (0)
#define PSC_AON_MCU_POWERDN_STATE(n)                        (((n) & 0x7) << 2)
#define PSC_AON_MCU_POWERDN_STATE_MASK                      (0x7 << 2)
#define PSC_AON_MCU_POWERDN_STATE_SHIFT                     (2)
#define PSC_AON_MCU_POWERUP_STATE(n)                        (((n) & 0x7) << 5)
#define PSC_AON_MCU_POWERUP_STATE_MASK                      (0x7 << 5)
#define PSC_AON_MCU_POWERUP_STATE_SHIFT                     (5)
#define PSC_AON_BT_MAIN_STATE(n)                            (((n) & 0x3) << 8)
#define PSC_AON_BT_MAIN_STATE_MASK                          (0x3 << 8)
#define PSC_AON_BT_MAIN_STATE_SHIFT                         (8)
#define PSC_AON_BT_POWERDN_STATE(n)                         (((n) & 0x7) << 10)
#define PSC_AON_BT_POWERDN_STATE_MASK                       (0x7 << 10)
#define PSC_AON_BT_POWERDN_STATE_SHIFT                      (10)
#define PSC_AON_BT_POWERUP_STATE(n)                         (((n) & 0x7) << 13)
#define PSC_AON_BT_POWERUP_STATE_MASK                       (0x7 << 13)
#define PSC_AON_BT_POWERUP_STATE_SHIFT                      (13)
#define PSC_AON_WLAN_MAIN_STATE(n)                          (((n) & 0x3) << 16)
#define PSC_AON_WLAN_MAIN_STATE_MASK                        (0x3 << 16)
#define PSC_AON_WLAN_MAIN_STATE_SHIFT                       (16)
#define PSC_AON_WLAN_POWERDN_STATE(n)                       (((n) & 0x7) << 18)
#define PSC_AON_WLAN_POWERDN_STATE_MASK                     (0x7 << 18)
#define PSC_AON_WLAN_POWERDN_STATE_SHIFT                    (18)
#define PSC_AON_WLAN_POWERUP_STATE(n)                       (((n) & 0x7) << 21)
#define PSC_AON_WLAN_POWERUP_STATE_MASK                     (0x7 << 21)
#define PSC_AON_WLAN_POWERUP_STATE_SHIFT                    (21)
#define PSC_AON_CODEC_MAIN_STATE(n)                         (((n) & 0x3) << 24)
#define PSC_AON_CODEC_MAIN_STATE_MASK                       (0x3 << 24)
#define PSC_AON_CODEC_MAIN_STATE_SHIFT                      (24)
#define PSC_AON_CODEC_POWERDN_STATE(n)                      (((n) & 0x7) << 26)
#define PSC_AON_CODEC_POWERDN_STATE_MASK                    (0x7 << 26)
#define PSC_AON_CODEC_POWERDN_STATE_SHIFT                   (26)
#define PSC_AON_CODEC_POWERUP_STATE(n)                      (((n) & 0x7) << 29)
#define PSC_AON_CODEC_POWERUP_STATE_MASK                    (0x7 << 29)
#define PSC_AON_CODEC_POWERUP_STATE_SHIFT                   (29)
#endif

// reg_020
#define PSC_AON_BT_PG_AUTO_EN                               (1 << 0)
#define PSC_AON_BT_PG_HW_EN                                 (1 << 1)

// reg_024
#define PSC_AON_BT_PSW_ACK_VALID                            (1 << 0)
#define PSC_AON_BT_RESERVED(n)                              (((n) & 0x7F) << 1)
#define PSC_AON_BT_RESERVED_MASK                            (0x7F << 1)
#define PSC_AON_BT_RESERVED_SHIFT                           (1)
#define PSC_AON_BT_MAIN_STATE(n)                            (((n) & 0x3) << 8)
#define PSC_AON_BT_MAIN_STATE_MASK                          (0x3 << 8)
#define PSC_AON_BT_MAIN_STATE_SHIFT                         (8)
#define PSC_AON_BT_POWERDN_STATE(n)                         (((n) & 0x7) << 10)
#define PSC_AON_BT_POWERDN_STATE_MASK                       (0x7 << 10)
#define PSC_AON_BT_POWERDN_STATE_SHIFT                      (10)
#define PSC_AON_BT_POWERUP_STATE(n)                         (((n) & 0x7) << 13)
#define PSC_AON_BT_POWERUP_STATE_MASK                       (0x7 << 13)
#define PSC_AON_BT_POWERUP_STATE_SHIFT                      (13)

#define PSC_AON_BT_SLEEP_NO_WFI                             (1 << 2)

// reg_028
#define PSC_AON_BT_POWERDN_TIMER1(n)                        (((n) & 0x3F) << 0)
#define PSC_AON_BT_POWERDN_TIMER1_MASK                      (0x3F << 0)
#define PSC_AON_BT_POWERDN_TIMER1_SHIFT                     (0)
#define PSC_AON_BT_POWERDN_TIMER2(n)                        (((n) & 0x3F) << 6)
#define PSC_AON_BT_POWERDN_TIMER2_MASK                      (0x3F << 6)
#define PSC_AON_BT_POWERDN_TIMER2_SHIFT                     (6)
#define PSC_AON_BT_POWERDN_TIMER3(n)                        (((n) & 0x3F) << 12)
#define PSC_AON_BT_POWERDN_TIMER3_MASK                      (0x3F << 12)
#define PSC_AON_BT_POWERDN_TIMER3_SHIFT                     (12)
#define PSC_AON_BT_POWERDN_TIMER4(n)                        (((n) & 0x3F) << 18)
#define PSC_AON_BT_POWERDN_TIMER4_MASK                      (0x3F << 18)
#define PSC_AON_BT_POWERDN_TIMER4_SHIFT                     (18)
#define PSC_AON_BT_POWERDN_TIMER5(n)                        (((n) & 0xFF) << 24)
#define PSC_AON_BT_POWERDN_TIMER5_MASK                      (0xFF << 24)
#define PSC_AON_BT_POWERDN_TIMER5_SHIFT                     (24)

// reg_02c
#define PSC_AON_BT_POWERUP_TIMER1(n)                        (((n) & 0x3F) << 0)
#define PSC_AON_BT_POWERUP_TIMER1_MASK                      (0x3F << 0)
#define PSC_AON_BT_POWERUP_TIMER1_SHIFT                     (0)
#define PSC_AON_BT_POWERUP_TIMER2(n)                        (((n) & 0xFF) << 6)
#define PSC_AON_BT_POWERUP_TIMER2_MASK                      (0xFF << 6)
#define PSC_AON_BT_POWERUP_TIMER2_SHIFT                     (6)
#define PSC_AON_BT_POWERUP_TIMER3(n)                        (((n) & 0x3F) << 14)
#define PSC_AON_BT_POWERUP_TIMER3_MASK                      (0x3F << 14)
#define PSC_AON_BT_POWERUP_TIMER3_SHIFT                     (14)
#define PSC_AON_BT_POWERUP_TIMER4(n)                        (((n) & 0x3F) << 20)
#define PSC_AON_BT_POWERUP_TIMER4_MASK                      (0x3F << 20)
#define PSC_AON_BT_POWERUP_TIMER4_SHIFT                     (20)
#define PSC_AON_BT_POWERUP_TIMER5(n)                        (((n) & 0x3F) << 26)
#define PSC_AON_BT_POWERUP_TIMER5_MASK                      (0x3F << 26)
#define PSC_AON_BT_POWERUP_TIMER5_SHIFT                     (26)

// reg_030
#define PSC_AON_BT_POWERDN_START                            (1 << 0)

// reg_034
#define PSC_AON_BT_POWERUP_START                            (1 << 0)

// reg_038
#define PSC_AON_BT_CLK_STOP_REG                             (1 << 0)
#define PSC_AON_BT_ISO_EN_REG                               (1 << 1)
#define PSC_AON_BT_RESETN_ASSERT_REG                        (1 << 2)
#define PSC_AON_BT_PSW_EN_REG                               (1 << 3)
#define PSC_AON_BT_CLK_STOP_DR                              (1 << 4)
#define PSC_AON_BT_ISO_EN_DR                                (1 << 5)
#define PSC_AON_BT_RESETN_ASSERT_DR                         (1 << 6)
#define PSC_AON_BT_PSW_EN_DR                                (1 << 7)

// reg_040
#define PSC_AON_WLAN_PG_AUTO_EN                             (1 << 0)
#define PSC_AON_WLAN_PG_HW_EN                               (1 << 1)

// reg_044
#define PSC_AON_WLAN_PSW_ACK_VALID                          (1 << 0)
#define PSC_AON_WLAN_RESERVED(n)                            (((n) & 0x7F) << 1)
#define PSC_AON_WLAN_RESERVED_MASK                          (0x7F << 1)
#define PSC_AON_WLAN_RESERVED_SHIFT                         (1)
#define PSC_AON_WLAN_MAIN_STATE(n)                          (((n) & 0x3) << 8)
#define PSC_AON_WLAN_MAIN_STATE_MASK                        (0x3 << 8)
#define PSC_AON_WLAN_MAIN_STATE_SHIFT                       (8)
#define PSC_AON_WLAN_POWERDN_STATE(n)                       (((n) & 0x7) << 10)
#define PSC_AON_WLAN_POWERDN_STATE_MASK                     (0x7 << 10)
#define PSC_AON_WLAN_POWERDN_STATE_SHIFT                    (10)
#define PSC_AON_WLAN_POWERUP_STATE(n)                       (((n) & 0x7) << 13)
#define PSC_AON_WLAN_POWERUP_STATE_MASK                     (0x7 << 13)
#define PSC_AON_WLAN_POWERUP_STATE_SHIFT                    (13)

// reg_048
#define PSC_AON_WLAN_POWERDN_TIMER1(n)                      (((n) & 0x3F) << 0)
#define PSC_AON_WLAN_POWERDN_TIMER1_MASK                    (0x3F << 0)
#define PSC_AON_WLAN_POWERDN_TIMER1_SHIFT                   (0)
#define PSC_AON_WLAN_POWERDN_TIMER2(n)                      (((n) & 0x3F) << 6)
#define PSC_AON_WLAN_POWERDN_TIMER2_MASK                    (0x3F << 6)
#define PSC_AON_WLAN_POWERDN_TIMER2_SHIFT                   (6)
#define PSC_AON_WLAN_POWERDN_TIMER3(n)                      (((n) & 0x3F) << 12)
#define PSC_AON_WLAN_POWERDN_TIMER3_MASK                    (0x3F << 12)
#define PSC_AON_WLAN_POWERDN_TIMER3_SHIFT                   (12)
#define PSC_AON_WLAN_POWERDN_TIMER4(n)                      (((n) & 0x3F) << 18)
#define PSC_AON_WLAN_POWERDN_TIMER4_MASK                    (0x3F << 18)
#define PSC_AON_WLAN_POWERDN_TIMER4_SHIFT                   (18)
#define PSC_AON_WLAN_POWERDN_TIMER5(n)                      (((n) & 0xFF) << 24)
#define PSC_AON_WLAN_POWERDN_TIMER5_MASK                    (0xFF << 24)
#define PSC_AON_WLAN_POWERDN_TIMER5_SHIFT                   (24)

// reg_04c
#define PSC_AON_WLAN_POWERUP_TIMER1(n)                      (((n) & 0x3F) << 0)
#define PSC_AON_WLAN_POWERUP_TIMER1_MASK                    (0x3F << 0)
#define PSC_AON_WLAN_POWERUP_TIMER1_SHIFT                   (0)
#define PSC_AON_WLAN_POWERUP_TIMER2(n)                      (((n) & 0xFF) << 6)
#define PSC_AON_WLAN_POWERUP_TIMER2_MASK                    (0xFF << 6)
#define PSC_AON_WLAN_POWERUP_TIMER2_SHIFT                   (6)
#define PSC_AON_WLAN_POWERUP_TIMER3(n)                      (((n) & 0x3F) << 14)
#define PSC_AON_WLAN_POWERUP_TIMER3_MASK                    (0x3F << 14)
#define PSC_AON_WLAN_POWERUP_TIMER3_SHIFT                   (14)
#define PSC_AON_WLAN_POWERUP_TIMER4(n)                      (((n) & 0x3F) << 20)
#define PSC_AON_WLAN_POWERUP_TIMER4_MASK                    (0x3F << 20)
#define PSC_AON_WLAN_POWERUP_TIMER4_SHIFT                   (20)
#define PSC_AON_WLAN_POWERUP_TIMER5(n)                      (((n) & 0x3F) << 26)
#define PSC_AON_WLAN_POWERUP_TIMER5_MASK                    (0x3F << 26)
#define PSC_AON_WLAN_POWERUP_TIMER5_SHIFT                   (26)

// reg_050
#define PSC_AON_WLAN_POWERDN_START                          (1 << 0)

// reg_054
#define PSC_AON_WLAN_POWERUP_START                          (1 << 0)

// reg_058
#define PSC_AON_WLAN_CLK_STOP_REG                           (1 << 0)
#define PSC_AON_WLAN_ISO_EN_REG                             (1 << 1)
#define PSC_AON_WLAN_RESETN_ASSERT_REG                      (1 << 2)
#define PSC_AON_WLAN_PSW_EN_REG                             (1 << 3)
#define PSC_AON_WLAN_CLK_STOP_DR                            (1 << 4)
#define PSC_AON_WLAN_ISO_EN_DR                              (1 << 5)
#define PSC_AON_WLAN_RESETN_ASSERT_DR                       (1 << 6)
#define PSC_AON_WLAN_PSW_EN_DR                              (1 << 7)

// reg_060
#define PSC_AON_CODEC_PG_AUTO_EN                            (1 << 0)

// reg_064
#define PSC_AON_CODEC_PSW_ACK_VALID                         (1 << 0)
#define PSC_AON_CODEC_RESERVED(n)                           (((n) & 0x7F) << 1)
#define PSC_AON_CODEC_RESERVED_MASK                         (0x7F << 1)
#define PSC_AON_CODEC_RESERVED_SHIFT                        (1)
#define PSC_AON_CODEC_MAIN_STATE(n)                         (((n) & 0x3) << 8)
#define PSC_AON_CODEC_MAIN_STATE_MASK                       (0x3 << 8)
#define PSC_AON_CODEC_MAIN_STATE_SHIFT                      (8)
#define PSC_AON_CODEC_POWERDN_STATE(n)                      (((n) & 0x7) << 10)
#define PSC_AON_CODEC_POWERDN_STATE_MASK                    (0x7 << 10)
#define PSC_AON_CODEC_POWERDN_STATE_SHIFT                   (10)
#define PSC_AON_CODEC_POWERUP_STATE(n)                      (((n) & 0x7) << 13)
#define PSC_AON_CODEC_POWERUP_STATE_MASK                    (0x7 << 13)
#define PSC_AON_CODEC_POWERUP_STATE_SHIFT                   (13)

// reg_068
#define PSC_AON_CODEC_POWERDN_TIMER1(n)                     (((n) & 0x3F) << 0)
#define PSC_AON_CODEC_POWERDN_TIMER1_MASK                   (0x3F << 0)
#define PSC_AON_CODEC_POWERDN_TIMER1_SHIFT                  (0)
#define PSC_AON_CODEC_POWERDN_TIMER2(n)                     (((n) & 0x3F) << 6)
#define PSC_AON_CODEC_POWERDN_TIMER2_MASK                   (0x3F << 6)
#define PSC_AON_CODEC_POWERDN_TIMER2_SHIFT                  (6)
#define PSC_AON_CODEC_POWERDN_TIMER3(n)                     (((n) & 0x3F) << 12)
#define PSC_AON_CODEC_POWERDN_TIMER3_MASK                   (0x3F << 12)
#define PSC_AON_CODEC_POWERDN_TIMER3_SHIFT                  (12)
#define PSC_AON_CODEC_POWERDN_TIMER4(n)                     (((n) & 0x3F) << 18)
#define PSC_AON_CODEC_POWERDN_TIMER4_MASK                   (0x3F << 18)
#define PSC_AON_CODEC_POWERDN_TIMER4_SHIFT                  (18)
#define PSC_AON_CODEC_POWERDN_TIMER5(n)                     (((n) & 0xFF) << 24)
#define PSC_AON_CODEC_POWERDN_TIMER5_MASK                   (0xFF << 24)
#define PSC_AON_CODEC_POWERDN_TIMER5_SHIFT                  (24)

// reg_06c
#define PSC_AON_CODEC_POWERUP_TIMER1(n)                     (((n) & 0x3F) << 0)
#define PSC_AON_CODEC_POWERUP_TIMER1_MASK                   (0x3F << 0)
#define PSC_AON_CODEC_POWERUP_TIMER1_SHIFT                  (0)
#define PSC_AON_CODEC_POWERUP_TIMER2(n)                     (((n) & 0xFF) << 6)
#define PSC_AON_CODEC_POWERUP_TIMER2_MASK                   (0xFF << 6)
#define PSC_AON_CODEC_POWERUP_TIMER2_SHIFT                  (6)
#define PSC_AON_CODEC_POWERUP_TIMER3(n)                     (((n) & 0x3F) << 14)
#define PSC_AON_CODEC_POWERUP_TIMER3_MASK                   (0x3F << 14)
#define PSC_AON_CODEC_POWERUP_TIMER3_SHIFT                  (14)
#define PSC_AON_CODEC_POWERUP_TIMER4(n)                     (((n) & 0x3F) << 20)
#define PSC_AON_CODEC_POWERUP_TIMER4_MASK                   (0x3F << 20)
#define PSC_AON_CODEC_POWERUP_TIMER4_SHIFT                  (20)
#define PSC_AON_CODEC_POWERUP_TIMER5(n)                     (((n) & 0x3F) << 26)
#define PSC_AON_CODEC_POWERUP_TIMER5_MASK                   (0x3F << 26)
#define PSC_AON_CODEC_POWERUP_TIMER5_SHIFT                  (26)

// reg_070
#define PSC_AON_CODEC_POWERDN_START                         (1 << 0)

// reg_074
#define PSC_AON_CODEC_POWERUP_START                         (1 << 0)

// reg_078
#define PSC_AON_CODEC_CLK_STOP_REG                          (1 << 0)
#define PSC_AON_CODEC_ISO_EN_REG                            (1 << 1)
#define PSC_AON_CODEC_RESETN_ASSERT_REG                     (1 << 2)
#define PSC_AON_CODEC_PSW_EN_REG                            (1 << 3)
#define PSC_AON_CODEC_CLK_STOP_DR                           (1 << 4)
#define PSC_AON_CODEC_ISO_EN_DR                             (1 << 5)
#define PSC_AON_CODEC_RESETN_ASSERT_DR                      (1 << 6)
#define PSC_AON_CODEC_PSW_EN_DR                             (1 << 7)

// reg_080
#define PSC_AON_MCU_INTR_MASK(n)                            (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_MCU_INTR_MASK_MASK                          (0xFFFFFFFF << 0)
#define PSC_AON_MCU_INTR_MASK_SHIFT                         (0)

// reg_084
#define PSC_AON_MCU_INTR_MASK2(n)                           (((n) & 0xFFFF) << 0)
#define PSC_AON_MCU_INTR_MASK2_MASK                         (0xFFFF << 0)
#define PSC_AON_MCU_INTR_MASK2_SHIFT                        (0)

// reg_088
#define PSC_AON_MCU_INTR_MASK_STATUS(n)                     (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_MCU_INTR_MASK_STATUS_MASK                   (0xFFFFFFFF << 0)
#define PSC_AON_MCU_INTR_MASK_STATUS_SHIFT                  (0)

// reg_08c
#define PSC_AON_MCU_INTR_MASK_STATUS2(n)                    (((n) & 0xFFFF) << 0)
#define PSC_AON_MCU_INTR_MASK_STATUS2_MASK                  (0xFFFF << 0)
#define PSC_AON_MCU_INTR_MASK_STATUS2_SHIFT                 (0)

// reg_090
#define PSC_AON_BT_INTR_MASK(n)                             (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_BT_INTR_MASK_MASK                           (0xFFFFFFFF << 0)
#define PSC_AON_BT_INTR_MASK_SHIFT                          (0)

// reg_094
#define PSC_AON_BT_INTR_MASK2(n)                            (((n) & 0xFFFF) << 0)
#define PSC_AON_BT_INTR_MASK2_MASK                          (0xFFFF << 0)
#define PSC_AON_BT_INTR_MASK2_SHIFT                         (0)

// reg_098
#define PSC_AON_BT_INTR_MASK_STATUS(n)                      (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_BT_INTR_MASK_STATUS_MASK                    (0xFFFFFFFF << 0)
#define PSC_AON_BT_INTR_MASK_STATUS_SHIFT                   (0)

// reg_09c
#define PSC_AON_BT_INTR_MASK_STATUS2(n)                     (((n) & 0xFFFF) << 0)
#define PSC_AON_BT_INTR_MASK_STATUS2_MASK                   (0xFFFF << 0)
#define PSC_AON_BT_INTR_MASK_STATUS2_SHIFT                  (0)

// reg_0a0
#define PSC_AON_WLAN_INTR_MASK(n)                           (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_WLAN_INTR_MASK_MASK                         (0xFFFFFFFF << 0)
#define PSC_AON_WLAN_INTR_MASK_SHIFT                        (0)

// reg_0a4
#define PSC_AON_WLAN_INTR_MASK2(n)                          (((n) & 0xFFFF) << 0)
#define PSC_AON_WLAN_INTR_MASK2_MASK                        (0xFFFF << 0)
#define PSC_AON_WLAN_INTR_MASK2_SHIFT                       (0)

// reg_0a8
#define PSC_AON_WLAN_INTR_MASK_STATUS(n)                    (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_WLAN_INTR_MASK_STATUS_MASK                  (0xFFFFFFFF << 0)
#define PSC_AON_WLAN_INTR_MASK_STATUS_SHIFT                 (0)

// reg_0ac
#define PSC_AON_WLAN_INTR_MASK_STATUS2(n)                   (((n) & 0xFFFF) << 0)
#define PSC_AON_WLAN_INTR_MASK_STATUS2_MASK                 (0xFFFF << 0)
#define PSC_AON_WLAN_INTR_MASK_STATUS2_SHIFT                (0)

// reg_0b0
#define PSC_AON_INTR_RAW_STATUS(n)                          (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_INTR_RAW_STATUS_MASK                        (0xFFFFFFFF << 0)
#define PSC_AON_INTR_RAW_STATUS_SHIFT                       (0)

// reg_0b4
#define PSC_AON_INTR_RAW_STATUS2(n)                         (((n) & 0xFFFF) << 0)
#define PSC_AON_INTR_RAW_STATUS2_MASK                       (0xFFFF << 0)
#define PSC_AON_INTR_RAW_STATUS2_SHIFT                      (0)

// reg_0c0
#define PSC_AON_A7_PG_AUTO_EN                               (1 << 0)
#define PSC_AON_A7_PG_HW_EN                                 (1 << 1)

// reg_0c4
#define PSC_AON_A7_PSW_ACK_VALID                            (1 << 0)
#define PSC_AON_A7_RESERVED(n)                              (((n) & 0x7F) << 1)
#define PSC_AON_A7_RESERVED_MASK                            (0x7F << 1)
#define PSC_AON_A7_RESERVED_SHIFT                           (1)
#define PSC_AON_A7_MAIN_STATE(n)                            (((n) & 0x3) << 8)
#define PSC_AON_A7_MAIN_STATE_MASK                          (0x3 << 8)
#define PSC_AON_A7_MAIN_STATE_SHIFT                         (8)
#define PSC_AON_A7_POWERDN_STATE(n)                         (((n) & 0x7) << 10)
#define PSC_AON_A7_POWERDN_STATE_MASK                       (0x7 << 10)
#define PSC_AON_A7_POWERDN_STATE_SHIFT                      (10)
#define PSC_AON_A7_POWERUP_STATE(n)                         (((n) & 0x7) << 13)
#define PSC_AON_A7_POWERUP_STATE_MASK                       (0x7 << 13)
#define PSC_AON_A7_POWERUP_STATE_SHIFT                      (13)

// reg_0c8
#define PSC_AON_A7_POWERDN_TIMER1(n)                        (((n) & 0x3F) << 0)
#define PSC_AON_A7_POWERDN_TIMER1_MASK                      (0x3F << 0)
#define PSC_AON_A7_POWERDN_TIMER1_SHIFT                     (0)
#define PSC_AON_A7_POWERDN_TIMER2(n)                        (((n) & 0x3F) << 6)
#define PSC_AON_A7_POWERDN_TIMER2_MASK                      (0x3F << 6)
#define PSC_AON_A7_POWERDN_TIMER2_SHIFT                     (6)
#define PSC_AON_A7_POWERDN_TIMER3(n)                        (((n) & 0x3F) << 12)
#define PSC_AON_A7_POWERDN_TIMER3_MASK                      (0x3F << 12)
#define PSC_AON_A7_POWERDN_TIMER3_SHIFT                     (12)
#define PSC_AON_A7_POWERDN_TIMER4(n)                        (((n) & 0x3F) << 18)
#define PSC_AON_A7_POWERDN_TIMER4_MASK                      (0x3F << 18)
#define PSC_AON_A7_POWERDN_TIMER4_SHIFT                     (18)
#define PSC_AON_A7_POWERDN_TIMER5(n)                        (((n) & 0xFF) << 24)
#define PSC_AON_A7_POWERDN_TIMER5_MASK                      (0xFF << 24)
#define PSC_AON_A7_POWERDN_TIMER5_SHIFT                     (24)

// reg_0cc
#define PSC_AON_A7_POWERUP_TIMER1(n)                        (((n) & 0x3F) << 0)
#define PSC_AON_A7_POWERUP_TIMER1_MASK                      (0x3F << 0)
#define PSC_AON_A7_POWERUP_TIMER1_SHIFT                     (0)
#define PSC_AON_A7_POWERUP_TIMER2(n)                        (((n) & 0xFF) << 6)
#define PSC_AON_A7_POWERUP_TIMER2_MASK                      (0xFF << 6)
#define PSC_AON_A7_POWERUP_TIMER2_SHIFT                     (6)
#define PSC_AON_A7_POWERUP_TIMER3(n)                        (((n) & 0x3F) << 14)
#define PSC_AON_A7_POWERUP_TIMER3_MASK                      (0x3F << 14)
#define PSC_AON_A7_POWERUP_TIMER3_SHIFT                     (14)
#define PSC_AON_A7_POWERUP_TIMER4(n)                        (((n) & 0x3F) << 20)
#define PSC_AON_A7_POWERUP_TIMER4_MASK                      (0x3F << 20)
#define PSC_AON_A7_POWERUP_TIMER4_SHIFT                     (20)
#define PSC_AON_A7_POWERUP_TIMER5(n)                        (((n) & 0x3F) << 26)
#define PSC_AON_A7_POWERUP_TIMER5_MASK                      (0x3F << 26)
#define PSC_AON_A7_POWERUP_TIMER5_SHIFT                     (26)

// reg_0d0
#define PSC_AON_A7_POWERDN_START                            (1 << 0)

// reg_0d4
#define PSC_AON_A7_POWERUP_START                            (1 << 0)

// reg_0d8
#define PSC_AON_A7_CLK_STOP_REG                             (1 << 0)
#define PSC_AON_A7_ISO_EN_REG                               (1 << 1)
#define PSC_AON_A7_RESETN_ASSERT_REG                        (1 << 2)
#define PSC_AON_A7_PSW_EN_REG                               (1 << 3)
#define PSC_AON_A7_CLK_STOP_DR                              (1 << 4)
#define PSC_AON_A7_ISO_EN_DR                                (1 << 5)
#define PSC_AON_A7_RESETN_ASSERT_DR                         (1 << 6)
#define PSC_AON_A7_PSW_EN_DR                                (1 << 7)

// reg_0e0
#define PSC_AON_A7_INTR_MASK(n)                             (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_A7_INTR_MASK_MASK                           (0xFFFFFFFF << 0)
#define PSC_AON_A7_INTR_MASK_SHIFT                          (0)

// reg_0e4
#define PSC_AON_A7_INTR_MASK2(n)                            (((n) & 0xFFFF) << 0)
#define PSC_AON_A7_INTR_MASK2_MASK                          (0xFFFF << 0)
#define PSC_AON_A7_INTR_MASK2_SHIFT                         (0)

// reg_0e8
#define PSC_AON_A7_INTR_MASK_STATUS(n)                      (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_A7_INTR_MASK_STATUS_MASK                    (0xFFFFFFFF << 0)
#define PSC_AON_A7_INTR_MASK_STATUS_SHIFT                   (0)

// reg_0ec
#define PSC_AON_A7_INTR_MASK_STATUS2(n)                     (((n) & 0xFFFF) << 0)
#define PSC_AON_A7_INTR_MASK_STATUS2_MASK                   (0xFFFF << 0)
#define PSC_AON_A7_INTR_MASK_STATUS2_SHIFT                  (0)

// reg_0f0
#define PSC_AON_A7SYS_PG_AUTO_EN                            (1 << 0)
#define PSC_AON_A7SYS_PG_HW_EN                              (1 << 1)

// reg_0f4
#define PSC_AON_A7SYS_PSW_ACK_VALID                         (1 << 0)
#define PSC_AON_A7SYS_RESERVED(n)                           (((n) & 0x7F) << 1)
#define PSC_AON_A7SYS_RESERVED_MASK                         (0x7F << 1)
#define PSC_AON_A7SYS_RESERVED_SHIFT                        (1)
#define PSC_AON_A7SYS_MAIN_STATE(n)                         (((n) & 0x3) << 8)
#define PSC_AON_A7SYS_MAIN_STATE_MASK                       (0x3 << 8)
#define PSC_AON_A7SYS_MAIN_STATE_SHIFT                      (8)
#define PSC_AON_A7SYS_POWERDN_STATE(n)                      (((n) & 0x7) << 10)
#define PSC_AON_A7SYS_POWERDN_STATE_MASK                    (0x7 << 10)
#define PSC_AON_A7SYS_POWERDN_STATE_SHIFT                   (10)
#define PSC_AON_A7SYS_POWERUP_STATE(n)                      (((n) & 0x7) << 13)
#define PSC_AON_A7SYS_POWERUP_STATE_MASK                    (0x7 << 13)
#define PSC_AON_A7SYS_POWERUP_STATE_SHIFT                   (13)

// reg_0f8
#define PSC_AON_A7SYS_POWERDN_TIMER1(n)                     (((n) & 0x3F) << 0)
#define PSC_AON_A7SYS_POWERDN_TIMER1_MASK                   (0x3F << 0)
#define PSC_AON_A7SYS_POWERDN_TIMER1_SHIFT                  (0)
#define PSC_AON_A7SYS_POWERDN_TIMER2(n)                     (((n) & 0x3F) << 6)
#define PSC_AON_A7SYS_POWERDN_TIMER2_MASK                   (0x3F << 6)
#define PSC_AON_A7SYS_POWERDN_TIMER2_SHIFT                  (6)
#define PSC_AON_A7SYS_POWERDN_TIMER3(n)                     (((n) & 0x3F) << 12)
#define PSC_AON_A7SYS_POWERDN_TIMER3_MASK                   (0x3F << 12)
#define PSC_AON_A7SYS_POWERDN_TIMER3_SHIFT                  (12)
#define PSC_AON_A7SYS_POWERDN_TIMER4(n)                     (((n) & 0x3F) << 18)
#define PSC_AON_A7SYS_POWERDN_TIMER4_MASK                   (0x3F << 18)
#define PSC_AON_A7SYS_POWERDN_TIMER4_SHIFT                  (18)
#define PSC_AON_A7SYS_POWERDN_TIMER5(n)                     (((n) & 0xFF) << 24)
#define PSC_AON_A7SYS_POWERDN_TIMER5_MASK                   (0xFF << 24)
#define PSC_AON_A7SYS_POWERDN_TIMER5_SHIFT                  (24)

// reg_0fc
#define PSC_AON_A7SYS_POWERUP_TIMER1(n)                     (((n) & 0x3F) << 0)
#define PSC_AON_A7SYS_POWERUP_TIMER1_MASK                   (0x3F << 0)
#define PSC_AON_A7SYS_POWERUP_TIMER1_SHIFT                  (0)
#define PSC_AON_A7SYS_POWERUP_TIMER2(n)                     (((n) & 0xFF) << 6)
#define PSC_AON_A7SYS_POWERUP_TIMER2_MASK                   (0xFF << 6)
#define PSC_AON_A7SYS_POWERUP_TIMER2_SHIFT                  (6)
#define PSC_AON_A7SYS_POWERUP_TIMER3(n)                     (((n) & 0x3F) << 14)
#define PSC_AON_A7SYS_POWERUP_TIMER3_MASK                   (0x3F << 14)
#define PSC_AON_A7SYS_POWERUP_TIMER3_SHIFT                  (14)
#define PSC_AON_A7SYS_POWERUP_TIMER4(n)                     (((n) & 0x3F) << 20)
#define PSC_AON_A7SYS_POWERUP_TIMER4_MASK                   (0x3F << 20)
#define PSC_AON_A7SYS_POWERUP_TIMER4_SHIFT                  (20)
#define PSC_AON_A7SYS_POWERUP_TIMER5(n)                     (((n) & 0x3F) << 26)
#define PSC_AON_A7SYS_POWERUP_TIMER5_MASK                   (0x3F << 26)
#define PSC_AON_A7SYS_POWERUP_TIMER5_SHIFT                  (26)

// reg_100
#define PSC_AON_A7SYS_POWERDN_START                         (1 << 0)

// reg_104
#define PSC_AON_A7SYS_POWERUP_START                         (1 << 0)

// reg_108
#define PSC_AON_A7SYS_CLK_STOP_REG                          (1 << 0)
#define PSC_AON_A7SYS_ISO_EN_REG                            (1 << 1)
#define PSC_AON_A7SYS_RESETN_ASSERT_REG                     (1 << 2)
#define PSC_AON_A7SYS_PSW_EN_REG                            (1 << 3)
#define PSC_AON_A7SYS_CLK_STOP_DR                           (1 << 4)
#define PSC_AON_A7SYS_ISO_EN_DR                             (1 << 5)
#define PSC_AON_A7SYS_RESETN_ASSERT_DR                      (1 << 6)
#define PSC_AON_A7SYS_PSW_EN_DR                             (1 << 7)

// reg_110
#define PSC_AON_A7SYS_INTR_MASK(n)                          (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_A7SYS_INTR_MASK_MASK                        (0xFFFFFFFF << 0)
#define PSC_AON_A7SYS_INTR_MASK_SHIFT                       (0)

// reg_114
#define PSC_AON_A7SYS_INTR_MASK2(n)                         (((n) & 0xFFFF) << 0)
#define PSC_AON_A7SYS_INTR_MASK2_MASK                       (0xFFFF << 0)
#define PSC_AON_A7SYS_INTR_MASK2_SHIFT                      (0)

// reg_118
#define PSC_AON_A7SYS_INTR_MASK_STATUS(n)                   (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_A7SYS_INTR_MASK_STATUS_MASK                 (0xFFFFFFFF << 0)
#define PSC_AON_A7SYS_INTR_MASK_STATUS_SHIFT                (0)

// reg_11c
#define PSC_AON_A7SYS_INTR_MASK_STATUS2(n)                  (((n) & 0xFFFF) << 0)
#define PSC_AON_A7SYS_INTR_MASK_STATUS2_MASK                (0xFFFF << 0)
#define PSC_AON_A7SYS_INTR_MASK_STATUS2_SHIFT               (0)

// reg_124
#define PSC_AON_TIMER1_MCU_REG(n)                           (((n) & 0xF) << 0)
#define PSC_AON_TIMER1_MCU_REG_MASK                         (0xF << 0)
#define PSC_AON_TIMER1_MCU_REG_SHIFT                        (0)
#define PSC_AON_TIMER2_MCU_REG(n)                           (((n) & 0xF) << 4)
#define PSC_AON_TIMER2_MCU_REG_MASK                         (0xF << 4)
#define PSC_AON_TIMER2_MCU_REG_SHIFT                        (4)
#define PSC_AON_TIMER3_MCU_REG(n)                           (((n) & 0xF) << 8)
#define PSC_AON_TIMER3_MCU_REG_MASK                         (0xF << 8)
#define PSC_AON_TIMER3_MCU_REG_SHIFT                        (8)
#define PSC_AON_PG_AUTO_EN_MCU_REG                          (1 << 12)
#define PSC_AON_PG_AUTO_EN_BT_REG                           (1 << 13)
#define PSC_AON_PG_AUTO_EN_WF_REG                           (1 << 14)
#define PSC_AON_PG_AUTO_EN_A7_REG                           (1 << 15)
#define PSC_AON_POWER_MODE_BT_DR                            (1 << 16)
#define PSC_AON_DSLP_FORCE_ON_BT_REG                        (1 << 17)
#define PSC_AON_PWR_MEM_SEL_AON_MCU                         (1 << 18)
#define PSC_AON_PWR_MEM_SEL_AON_BT                          (1 << 19)
#define PSC_AON_PWR_MEM_SEL_AON_WF                          (1 << 20)
#define PSC_AON_PWR_MEM_SEL_AON_A7                          (1 << 21)
#define PSC_AON_DEEPSLEEP_MODE_A7_REG                       (1 << 22)
#define PSC_AON_DEEPSLEEP_MODE_WF_REG                       (1 << 23)

// reg_128
#define PSC_AON_RAM_RET1N0_0(n)                             (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_RAM_RET1N0_0_MASK                           (0xFFFFFFFF << 0)
#define PSC_AON_RAM_RET1N0_0_SHIFT                          (0)

// reg_12c
#define PSC_AON_RAM_RET1N0_1(n)                             (((n) & 0x1F) << 0)
#define PSC_AON_RAM_RET1N0_1_MASK                           (0x1F << 0)
#define PSC_AON_RAM_RET1N0_1_SHIFT                          (0)

// reg_130
#define PSC_AON_RAM_RET2N0_0(n)                             (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_RAM_RET2N0_0_MASK                           (0xFFFFFFFF << 0)
#define PSC_AON_RAM_RET2N0_0_SHIFT                          (0)

// reg_134
#define PSC_AON_RAM_RET2N0_1(n)                             (((n) & 0x1F) << 0)
#define PSC_AON_RAM_RET2N0_1_MASK                           (0x1F << 0)
#define PSC_AON_RAM_RET2N0_1_SHIFT                          (0)

// reg_138
#define PSC_AON_RAM_PGEN0_0(n)                              (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_RAM_PGEN0_0_MASK                            (0xFFFFFFFF << 0)
#define PSC_AON_RAM_PGEN0_0_SHIFT                           (0)

// reg_13c
#define PSC_AON_RAM_PGEN0_1(n)                              (((n) & 0x1F) << 0)
#define PSC_AON_RAM_PGEN0_1_MASK                            (0x1F << 0)
#define PSC_AON_RAM_PGEN0_1_SHIFT                           (0)

// reg_140
#define PSC_AON_RAM_RET1N1_0(n)                             (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_RAM_RET1N1_0_MASK                           (0xFFFFFFFF << 0)
#define PSC_AON_RAM_RET1N1_0_SHIFT                          (0)

// reg_144
#define PSC_AON_RAM_RET1N1_1(n)                             (((n) & 0x1F) << 0)
#define PSC_AON_RAM_RET1N1_1_MASK                           (0x1F << 0)
#define PSC_AON_RAM_RET1N1_1_SHIFT                          (0)

// reg_148
#define PSC_AON_RAM_RET2N1_0(n)                             (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_RAM_RET2N1_0_MASK                           (0xFFFFFFFF << 0)
#define PSC_AON_RAM_RET2N1_0_SHIFT                          (0)

// reg_14c
#define PSC_AON_RAM_RET2N1_1(n)                             (((n) & 0x1F) << 0)
#define PSC_AON_RAM_RET2N1_1_MASK                           (0x1F << 0)
#define PSC_AON_RAM_RET2N1_1_SHIFT                          (0)

// reg_150
#define PSC_AON_RAM_PGEN1_0(n)                              (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_RAM_PGEN1_0_MASK                            (0xFFFFFFFF << 0)
#define PSC_AON_RAM_PGEN1_0_SHIFT                           (0)

// reg_154
#define PSC_AON_RAM_PGEN1_1(n)                              (((n) & 0x1F) << 0)
#define PSC_AON_RAM_PGEN1_1_MASK                            (0x1F << 0)
#define PSC_AON_RAM_PGEN1_1_SHIFT                           (0)

// reg_158
#define PSC_AON_SRAM_AUTO_EN_MODE_0(n)                      (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_SRAM_AUTO_EN_MODE_0_MASK                    (0xFFFFFFFF << 0)
#define PSC_AON_SRAM_AUTO_EN_MODE_0_SHIFT                   (0)

// reg_15c
#define PSC_AON_SRAM_AUTO_EN_MODE_1(n)                      (((n) & 0x1F) << 0)
#define PSC_AON_SRAM_AUTO_EN_MODE_1_MASK                    (0x1F << 0)
#define PSC_AON_SRAM_AUTO_EN_MODE_1_SHIFT                   (0)

// reg_160
#define PSC_AON_CEN_MSK_A7_DR(n)                            (((n) & 0x3) << 0)
#define PSC_AON_CEN_MSK_A7_DR_MASK                          (0x3 << 0)
#define PSC_AON_CEN_MSK_A7_DR_SHIFT                         (0)
#define PSC_AON_CEN_MSK_WF_DR(n)                            (((n) & 0x3) << 2)
#define PSC_AON_CEN_MSK_WF_DR_MASK                          (0x3 << 2)
#define PSC_AON_CEN_MSK_WF_DR_SHIFT                         (2)
#define PSC_AON_CEN_MSK_BT_DR(n)                            (((n) & 0x1FF) << 4)
#define PSC_AON_CEN_MSK_BT_DR_MASK                          (0x1FF << 4)
#define PSC_AON_CEN_MSK_BT_DR_SHIFT                         (4)

// reg_164
#define PSC_AON_CEN_MSK_MCU_DR(n)                           (((n) & 0x3FFFFF) << 0)
#define PSC_AON_CEN_MSK_MCU_DR_MASK                         (0x3FFFFF << 0)
#define PSC_AON_CEN_MSK_MCU_DR_SHIFT                        (0)

// reg_168
#define PSC_AON_CEN_MSK_A7_REG(n)                           (((n) & 0x3) << 0)
#define PSC_AON_CEN_MSK_A7_REG_MASK                         (0x3 << 0)
#define PSC_AON_CEN_MSK_A7_REG_SHIFT                        (0)
#define PSC_AON_CEN_MSK_WF_REG(n)                           (((n) & 0x3) << 2)
#define PSC_AON_CEN_MSK_WF_REG_MASK                         (0x3 << 2)
#define PSC_AON_CEN_MSK_WF_REG_SHIFT                        (2)
#define PSC_AON_CEN_MSK_BT_REG(n)                           (((n) & 0x1FF) << 4)
#define PSC_AON_CEN_MSK_BT_REG_MASK                         (0x1FF << 4)
#define PSC_AON_CEN_MSK_BT_REG_SHIFT                        (4)

// reg_16c
#define PSC_AON_CEN_MSK_MCU_REG(n)                          (((n) & 0x3FFFFF) << 0)
#define PSC_AON_CEN_MSK_MCU_REG_MASK                        (0x3FFFFF << 0)
#define PSC_AON_CEN_MSK_MCU_REG_SHIFT                       (0)

// reg_170
#define PSC_AON_POWER_MODE_MCU_0(n)                         (((n) & 0xFFFFFFFF) << 0)
#define PSC_AON_POWER_MODE_MCU_0_MASK                       (0xFFFFFFFF << 0)
#define PSC_AON_POWER_MODE_MCU_0_SHIFT                      (0)

// reg_174
#define PSC_AON_POWER_MODE_MCU_1(n)                         (((n) & 0x1F) << 0)
#define PSC_AON_POWER_MODE_MCU_1_MASK                       (0x1F << 0)
#define PSC_AON_POWER_MODE_MCU_1_SHIFT                      (0)

#endif
