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
#ifndef __REG_MIPIPHY_BEST2003_H__
#define __REG_MIPIPHY_BEST2003_H__

#include "plat_types.h"

enum MIPIPHY_REG_T {
    MIPIPHY_REG_00   = 0x00,
    MIPIPHY_REG_01,
    MIPIPHY_REG_02,
    MIPIPHY_REG_03,
    MIPIPHY_REG_04,
    MIPIPHY_REG_05,
    MIPIPHY_REG_06,
    MIPIPHY_REG_07,
    MIPIPHY_REG_08,
    MIPIPHY_REG_09,
    MIPIPHY_REG_0A,
    MIPIPHY_REG_0B,
    MIPIPHY_REG_0C,
};
// REG_00
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_01
#define REG_PU_LDO_CSI                      (1 << 0)
#define REG_PRECHARGE_CSI                   (1 << 1)
#define REG_RES_LDO_CSI_SHIFT               2
#define REG_RES_LDO_CSI_MASK                (0xF << REG_RES_LDO_CSI_SHIFT)
#define REG_RES_LDO_CSI(n)                  BITFIELD_VAL(REG_RES_LDO_CSI, n)
#define REG_PU_CH_CSI                       (1 << 6)
#define REG_LPRX_CTL_CSI                    (1 << 7)
#define REG_VH_LPRX_CSI_SHIFT               8
#define REG_VH_LPRX_CSI_MASK                (0x7 << REG_VH_LPRX_CSI_SHIFT)
#define REG_VH_LPRX_CSI(n)                  BITFIELD_VAL(REG_VH_LPRX_CSI, n)
#define REG_VL_LPRX_CSI_SHIFT               11
#define REG_VL_LPRX_CSI_MASK                (0x7 << REG_VL_LPRX_CSI_SHIFT)
#define REG_VL_LPRX_CSI(n)                  BITFIELD_VAL(REG_VL_LPRX_CSI, n)
#define REG_RANGE_CSI_SHIFT                 14
#define REG_RANGE_CSI_MASK                  (0x3 << REG_RANGE_CSI_SHIFT)
#define REG_RANGE_CSI(n)                    BITFIELD_VAL(REG_RANGE_CSI, n)

// REG_02
#define REG_CHCK_EN_CH_CSI                  (1 << 0)
#define REG_CHCK_HSRX_RTERM_CSI_SHIFT       1
#define REG_CHCK_HSRX_RTERM_CSI_MASK        (0x7 << REG_CHCK_HSRX_RTERM_CSI_SHIFT)
#define REG_CHCK_HSRX_RTERM_CSI(n)          BITFIELD_VAL(REG_CHCK_HSRX_RTERM_CSI, n)
#define REG_CHCK_HSRX_ISEL_CSI_SHIFT        4
#define REG_CHCK_HSRX_ISEL_CSI_MASK         (0x7 << REG_CHCK_HSRX_ISEL_CSI_SHIFT)
#define REG_CHCK_HSRX_ISEL_CSI(n)           BITFIELD_VAL(REG_CHCK_HSRX_ISEL_CSI, n)
#define REG_CHCK_HSRX_RES_CSI_SHIFT         7
#define REG_CHCK_HSRX_RES_CSI_MASK          (0xF << REG_CHCK_HSRX_RES_CSI_SHIFT)
#define REG_CHCK_HSRX_RES_CSI(n)            BITFIELD_VAL(REG_CHCK_HSRX_RES_CSI, n)
#define REG_CHCK_HSRX_DLY_CSI_SHIFT         11
#define REG_CHCK_HSRX_DLY_CSI_MASK          (0x1F << REG_CHCK_HSRX_DLY_CSI_SHIFT)
#define REG_CHCK_HSRX_DLY_CSI(n)            BITFIELD_VAL(REG_CHCK_HSRX_DLY_CSI, n)

// REG_03
#define REG_CHCK_PSEL_CSI                   (1 << 0)
#define RESERVED_CSI_14_0_SHIFT             1
#define RESERVED_CSI_14_0_MASK              (0x7FFF << RESERVED_CSI_14_0_SHIFT)
#define RESERVED_CSI_14_0(n)                BITFIELD_VAL(RESERVED_CSI_14_0, n)

// REG_04
#define REG_CH0_EN_CH_CSI                   (1 << 0)
#define REG_CH0_HSRX_RTERM_CSI_SHIFT        1
#define REG_CH0_HSRX_RTERM_CSI_MASK         (0x7 << REG_CH0_HSRX_RTERM_CSI_SHIFT)
#define REG_CH0_HSRX_RTERM_CSI(n)           BITFIELD_VAL(REG_CH0_HSRX_RTERM_CSI, n)
#define REG_CH0_HSRX_ISEL_CSI_SHIFT         4
#define REG_CH0_HSRX_ISEL_CSI_MASK          (0x7 << REG_CH0_HSRX_ISEL_CSI_SHIFT)
#define REG_CH0_HSRX_ISEL_CSI(n)            BITFIELD_VAL(REG_CH0_HSRX_ISEL_CSI, n)
#define REG_CH0_HSRX_RES_CSI_SHIFT          7
#define REG_CH0_HSRX_RES_CSI_MASK           (0xF << REG_CH0_HSRX_RES_CSI_SHIFT)
#define REG_CH0_HSRX_RES_CSI(n)             BITFIELD_VAL(REG_CH0_HSRX_RES_CSI, n)
#define REG_CH0_HSRX_DLY_CSI_SHIFT          11
#define REG_CH0_HSRX_DLY_CSI_MASK           (0x1F << REG_CH0_HSRX_DLY_CSI_SHIFT)
#define REG_CH0_HSRX_DLY_CSI(n)             BITFIELD_VAL(REG_CH0_HSRX_DLY_CSI, n)

// REG_05
#define REG_CH1_EN_CH_CSI                   (1 << 0)
#define REG_CH1_HSRX_RTERM_CSI_SHIFT        1
#define REG_CH1_HSRX_RTERM_CSI_MASK         (0x7 << REG_CH1_HSRX_RTERM_CSI_SHIFT)
#define REG_CH1_HSRX_RTERM_CSI(n)           BITFIELD_VAL(REG_CH1_HSRX_RTERM_CSI, n)
#define REG_CH1_HSRX_ISEL_CSI_SHIFT         4
#define REG_CH1_HSRX_ISEL_CSI_MASK          (0x7 << REG_CH1_HSRX_ISEL_CSI_SHIFT)
#define REG_CH1_HSRX_ISEL_CSI(n)            BITFIELD_VAL(REG_CH1_HSRX_ISEL_CSI, n)
#define REG_CH1_HSRX_RES_CSI_SHIFT          7
#define REG_CH1_HSRX_RES_CSI_MASK           (0xF << REG_CH1_HSRX_RES_CSI_SHIFT)
#define REG_CH1_HSRX_RES_CSI(n)             BITFIELD_VAL(REG_CH1_HSRX_RES_CSI, n)
#define REG_CH1_HSRX_DLY_CSI_SHIFT          11
#define REG_CH1_HSRX_DLY_CSI_MASK           (0x1F << REG_CH1_HSRX_DLY_CSI_SHIFT)
#define REG_CH1_HSRX_DLY_CSI(n)             BITFIELD_VAL(REG_CH1_HSRX_DLY_CSI, n)

// REG_06
#define REG_EN_LPRX0_DSI                    (1 << 0)
#define REG_CLK_SWRC_DSI_SHIFT              1
#define REG_CLK_SWRC_DSI_MASK               (0x3 << REG_CLK_SWRC_DSI_SHIFT)
#define REG_CLK_SWRC_DSI(n)                 BITFIELD_VAL(REG_CLK_SWRC_DSI, n)
#define REG_CLKGEN_PU_DSI                   (1 << 3)
#define REG_RX_SWAP_PN_DSI                  (1 << 4)
#define REG_SWAP_CKX1_DSI                   (1 << 5)
#define REG_SWAP_CKX8_DSI                   (1 << 6)
#define REG_PRECHARGE_LDO_DSI               (1 << 7)
#define REG_PU_CH_DSI                       (1 << 8)
#define REG_PU_LDO_DSI                      (1 << 9)
#define REG_DSI_PHY_RSTN                    (1 << 10)
#define RESERVED_DSI_4_0_SHIFT              11
#define RESERVED_DSI_4_0_MASK               (0x1F << RESERVED_DSI_4_0_SHIFT)
#define RESERVED_DSI_4_0(n)                 BITFIELD_VAL(RESERVED_DSI_4_0, n)

// REG_07
#define REG_LDO0P4_RES_DSI_SHIFT            0
#define REG_LDO0P4_RES_DSI_MASK             (0xF << REG_LDO0P4_RES_DSI_SHIFT)
#define REG_LDO0P4_RES_DSI(n)               BITFIELD_VAL(REG_LDO0P4_RES_DSI, n)
#define REG_LDO1P0_RES_DSI_SHIFT            4
#define REG_LDO1P0_RES_DSI_MASK             (0xF << REG_LDO1P0_RES_DSI_SHIFT)
#define REG_LDO1P0_RES_DSI(n)               BITFIELD_VAL(REG_LDO1P0_RES_DSI, n)
#define REG_LDO1P2_RES_DSI_SHIFT            8
#define REG_LDO1P2_RES_DSI_MASK             (0xF << REG_LDO1P2_RES_DSI_SHIFT)
#define REG_LDO1P2_RES_DSI(n)               BITFIELD_VAL(REG_LDO1P2_RES_DSI, n)
#define RESERVED_DSI_8_5_SHIFT              12
#define RESERVED_DSI_8_5_MASK               (0xF << RESERVED_DSI_8_5_SHIFT)
#define RESERVED_DSI_8_5(n)                 BITFIELD_VAL(RESERVED_DSI_8_5, n)

// REG_08
#define REG_CH0_EN_DSI                      (1 << 0)
#define REG_CH0_DLY_TUNE_DSI_SHIFT          1
#define REG_CH0_DLY_TUNE_DSI_MASK           (0x7 << REG_CH0_DLY_TUNE_DSI_SHIFT)
#define REG_CH0_DLY_TUNE_DSI(n)             BITFIELD_VAL(REG_CH0_DLY_TUNE_DSI, n)
#define REG_CH0_HSTX_DRV_DSI_SHIFT          4
#define REG_CH0_HSTX_DRV_DSI_MASK           (0xF << REG_CH0_HSTX_DRV_DSI_SHIFT)
#define REG_CH0_HSTX_DRV_DSI(n)             BITFIELD_VAL(REG_CH0_HSTX_DRV_DSI, n)
#define REG_CH0_LPTX_DRV_DSI_SHIFT          8
#define REG_CH0_LPTX_DRV_DSI_MASK           (0x7 << REG_CH0_LPTX_DRV_DSI_SHIFT)
#define REG_CH0_LPTX_DRV_DSI(n)             BITFIELD_VAL(REG_CH0_LPTX_DRV_DSI, n)
#define REG_CH0_PULL_DN_DSI                 (1 << 11)
#define REG_CH0_SWAP_PN_DSI                 (1 << 12)
#define RESERVED_DSI_11_9_SHIFT             13
#define RESERVED_DSI_11_9_MASK              (0x7 << RESERVED_DSI_11_9_SHIFT)
#define RESERVED_DSI_11_9(n)                BITFIELD_VAL(RESERVED_DSI_11_9, n)

// REG_09
#define REG_CH1_EN_DSI                      (1 << 0)
#define REG_CH1_DLY_TUNE_DSI_SHIFT          1
#define REG_CH1_DLY_TUNE_DSI_MASK           (0x7 << REG_CH1_DLY_TUNE_DSI_SHIFT)
#define REG_CH1_DLY_TUNE_DSI(n)             BITFIELD_VAL(REG_CH1_DLY_TUNE_DSI, n)
#define REG_CH1_HSTX_DRV_DSI_SHIFT          4
#define REG_CH1_HSTX_DRV_DSI_MASK           (0xF << REG_CH1_HSTX_DRV_DSI_SHIFT)
#define REG_CH1_HSTX_DRV_DSI(n)             BITFIELD_VAL(REG_CH1_HSTX_DRV_DSI, n)
#define REG_CH1_LPTX_DRV_DSI_SHIFT          8
#define REG_CH1_LPTX_DRV_DSI_MASK           (0x7 << REG_CH1_LPTX_DRV_DSI_SHIFT)
#define REG_CH1_LPTX_DRV_DSI(n)             BITFIELD_VAL(REG_CH1_LPTX_DRV_DSI, n)
#define REG_CH1_PULL_DN_DSI                 (1 << 11)
#define REG_CH1_SWAP_PN_DSI                 (1 << 12)
#define RESERVED_DSI_14_12_SHIFT            13
#define RESERVED_DSI_14_12_MASK             (0x7 << RESERVED_DSI_14_12_SHIFT)
#define RESERVED_DSI_14_12(n)               BITFIELD_VAL(RESERVED_DSI_14_12, n)

// REG_0A
#define REG_CHCK_EN_DSI                     (1 << 0)
#define REG_CHCK_DLY_TUNE_DSI_SHIFT         1
#define REG_CHCK_DLY_TUNE_DSI_MASK          (0x7 << REG_CHCK_DLY_TUNE_DSI_SHIFT)
#define REG_CHCK_DLY_TUNE_DSI(n)            BITFIELD_VAL(REG_CHCK_DLY_TUNE_DSI, n)
#define REG_CHCK_HSTX_DRV_DSI_SHIFT         4
#define REG_CHCK_HSTX_DRV_DSI_MASK          (0xF << REG_CHCK_HSTX_DRV_DSI_SHIFT)
#define REG_CHCK_HSTX_DRV_DSI(n)            BITFIELD_VAL(REG_CHCK_HSTX_DRV_DSI, n)
#define REG_CHCK_LPTX_DRV_DSI_SHIFT         8
#define REG_CHCK_LPTX_DRV_DSI_MASK          (0x7 << REG_CHCK_LPTX_DRV_DSI_SHIFT)
#define REG_CHCK_LPTX_DRV_DSI(n)            BITFIELD_VAL(REG_CHCK_LPTX_DRV_DSI, n)
#define REG_CHCK_PULL_DN_DSI                (1 << 11)
#define REG_CHCK_SWAP_PN_DSI                (1 << 12)
#define RESERVED_DSI_17_15_SHIFT            13
#define RESERVED_DSI_17_15_MASK             (0x7 << RESERVED_DSI_17_15_SHIFT)
#define RESERVED_DSI_17_15(n)               BITFIELD_VAL(RESERVED_DSI_17_15, n)

// REG_0B
#define REG_VB_LPCD_H_DSI_SHIFT             0
#define REG_VB_LPCD_H_DSI_MASK              (0x7 << REG_VB_LPCD_H_DSI_SHIFT)
#define REG_VB_LPCD_H_DSI(n)                BITFIELD_VAL(REG_VB_LPCD_H_DSI, n)
#define REG_VB_LPCD_L_DSI_SHIFT             3
#define REG_VB_LPCD_L_DSI_MASK              (0x7 << REG_VB_LPCD_L_DSI_SHIFT)
#define REG_VB_LPCD_L_DSI(n)                BITFIELD_VAL(REG_VB_LPCD_L_DSI, n)
#define REG_VB_LPRX_H_DSI_SHIFT             6
#define REG_VB_LPRX_H_DSI_MASK              (0x7 << REG_VB_LPRX_H_DSI_SHIFT)
#define REG_VB_LPRX_H_DSI(n)                BITFIELD_VAL(REG_VB_LPRX_H_DSI, n)
#define REG_VB_LPRX_L_DSI_SHIFT             9
#define REG_VB_LPRX_L_DSI_MASK              (0x7 << REG_VB_LPRX_L_DSI_SHIFT)
#define REG_VB_LPRX_L_DSI(n)                BITFIELD_VAL(REG_VB_LPRX_L_DSI, n)

// REG_0C
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)
#define RESERVED_CSI_23_15_SHIFT            7
#define RESERVED_CSI_23_15_MASK             (0x1FF << RESERVED_CSI_23_15_SHIFT)
#define RESERVED_CSI_23_15(n)               BITFIELD_VAL(RESERVED_CSI_23_15, n)

#endif
