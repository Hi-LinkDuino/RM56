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
#ifndef __REG_DDRPLL_BEST2003_H__
#define __REG_DDRPLL_BEST2003_H__

#include "plat_types.h"

enum DDRPLL_REG_T {
    DDRPLL_REG_D5   = 0xD5,
    DDRPLL_REG_D6,
    DDRPLL_REG_D7,
    DDRPLL_REG_D8,
    DDRPLL_REG_D9,
    DDRPLL_REG_DA,
    DDRPLL_REG_DB,
    DDRPLL_REG_DC,
    DDRPLL_REG_DD,
    DDRPLL_REG_DE,
    DDRPLL_REG_DF,
    DDRPLL_REG_E0,
    DDRPLL_REG_E1,
    DDRPLL_REG_E2,
    DDRPLL_REG_E3,
    DDRPLL_REG_E4,
    DDRPLL_REG_E5,
    DDRPLL_REG_E6,
    DDRPLL_REG_E7,
    DDRPLL_REG_E8,
    DDRPLL_REG_E9,
};

// REG_0D5
#define REG_DDRPLL_SEL_CAL_VT_SHIFT          0
#define REG_DDRPLL_SEL_CAL_VT_MASK           (0x3 << REG_DDRPLL_SEL_CAL_VT_SHIFT)
#define REG_DDRPLL_SEL_CAL_VT(n)             BITFIELD_VAL(REG_DDRPLL_SEL_CAL_VT, n)
#define DIG_DDRPLL_LOCK                      (1 << 3)

// REG_0D6
#define REG_DDRPLL_CAL_CLK_SEL               (1 << 0)
#define REG_DDRPLL_CPMODE                    (1 << 1)
#define REG_DDRPLL_PATH_EN_SHIFT             2
#define REG_DDRPLL_PATH_EN_MASK              (0x3 << REG_DDRPLL_PATH_EN_SHIFT)
#define REG_DDRPLL_PATH_EN(n)                BITFIELD_VAL(REG_DDRPLL_PATH_EN, n)
#define REG_DDRPLL_PDIV_PSRAM_SHIFT          4
#define REG_DDRPLL_PDIV_PSRAM_MASK           (0x1F << REG_DDRPLL_PDIV_PSRAM_SHIFT)
#define REG_DDRPLL_PDIV_PSRAM(n)             BITFIELD_VAL(REG_DDRPLL_PDIV_PSRAM, n)
#define REG_DDRPLL_PDIV_PSRAM2_SHIFT         9
#define REG_DDRPLL_PDIV_PSRAM2_MASK          (0x1F << REG_DDRPLL_PDIV_PSRAM2_SHIFT)
#define REG_DDRPLL_PDIV_PSRAM2(n)            BITFIELD_VAL(REG_DDRPLL_PDIV_PSRAM2, n)

// REG_0D7
#define REG_DDRPLL_BUF_VRES_SHIFT            0
#define REG_DDRPLL_BUF_VRES_MASK             (0x7 << REG_DDRPLL_BUF_VRES_SHIFT)
#define REG_DDRPLL_BUF_VRES(n)               BITFIELD_VAL(REG_DDRPLL_BUF_VRES, n)
#define REG_DDRPLL_BW_SEL_SHIFT              3
#define REG_DDRPLL_BW_SEL_MASK               (0xF << REG_DDRPLL_BW_SEL_SHIFT)
#define REG_DDRPLL_BW_SEL(n)                 BITFIELD_VAL(REG_DDRPLL_BW_SEL, n)
#define REG_DDRPLL_DIV_DLYCAP_SHIFT          7
#define REG_DDRPLL_DIV_DLYCAP_MASK           (0xF << REG_DDRPLL_DIV_DLYCAP_SHIFT)
#define REG_DDRPLL_DIV_DLYCAP(n)             BITFIELD_VAL(REG_DDRPLL_DIV_DLYCAP, n)

// REG_0D8
#define REG_DDRPLL_CP_VRES_SHIFT             0
#define REG_DDRPLL_CP_VRES_MASK              (0x7 << REG_DDRPLL_CP_VRES_SHIFT)
#define REG_DDRPLL_CP_VRES(n)                BITFIELD_VAL(REG_DDRPLL_CP_VRES, n)
#define REG_DDRPLL_DIG_VRES_SHIFT            3
#define REG_DDRPLL_DIG_VRES_MASK             (0x7 << REG_DDRPLL_DIG_VRES_SHIFT)
#define REG_DDRPLL_DIG_VRES(n)               BITFIELD_VAL(REG_DDRPLL_DIG_VRES, n)
#define REG_DDRPLL_DIV_FBC_SEL_SHIFT         6
#define REG_DDRPLL_DIV_FBC_SEL_MASK          (0x7 << REG_DDRPLL_DIV_FBC_SEL_SHIFT)
#define REG_DDRPLL_DIV_FBC_SEL(n)            BITFIELD_VAL(REG_DDRPLL_DIV_FBC_SEL, n)
#define REG_DDRPLL_FBRES_SHIFT               9
#define REG_DDRPLL_FBRES_MASK                (0x7 << REG_DDRPLL_FBRES_SHIFT)
#define REG_DDRPLL_FBRES(n)                  BITFIELD_VAL(REG_DDRPLL_FBRES, n)
#define REG_DDRPLL_ICP_SHIFT                 12
#define REG_DDRPLL_ICP_MASK                  (0x7 << REG_DDRPLL_ICP_SHIFT)
#define REG_DDRPLL_ICP(n)                    BITFIELD_VAL(REG_DDRPLL_ICP, n)

// REG_0D9
#define REG_DDRPLL_IOFST_SHIFT               0
#define REG_DDRPLL_IOFST_MASK                (0x7 << REG_DDRPLL_IOFST_SHIFT)
#define REG_DDRPLL_IOFST(n)                  BITFIELD_VAL(REG_DDRPLL_IOFST, n)
#define REG_DDRPLL_LOCKDET_BYPASS            (1 << 4)
#define REG_DDRPLL_LOCKDET_WIN_SHIFT         5
#define REG_DDRPLL_LOCKDET_WIN_MASK          (0x7 << REG_DDRPLL_LOCKDET_WIN_SHIFT)
#define REG_DDRPLL_LOCKDET_WIN(n)            BITFIELD_VAL(REG_DDRPLL_LOCKDET_WIN, n)
#define REG_DDRPLL_VAR_BIAS_SHIFT            8
#define REG_DDRPLL_VAR_BIAS_MASK             (0x7 << REG_DDRPLL_VAR_BIAS_SHIFT)
#define REG_DDRPLL_VAR_BIAS(n)               BITFIELD_VAL(REG_DDRPLL_VAR_BIAS, n)
#define REG_DDRPLL_TST_VCTL_SHIFT            11
#define REG_DDRPLL_TST_VCTL_MASK             (0xF << REG_DDRPLL_TST_VCTL_SHIFT)
#define REG_DDRPLL_TST_VCTL(n)               BITFIELD_VAL(REG_DDRPLL_TST_VCTL, n)

// REG_0DA
#define REG_DDRPLL_VCO_ICTRL_SHIFT           0
#define REG_DDRPLL_VCO_ICTRL_MASK            (0x7 << REG_DDRPLL_VCO_ICTRL_SHIFT)
#define REG_DDRPLL_VCO_ICTRL(n)              BITFIELD_VAL(REG_DDRPLL_VCO_ICTRL, n)
#define REG_DDRPLL_VCO_VRES_SHIFT            4
#define REG_DDRPLL_VCO_VRES_MASK             (0x7 << REG_DDRPLL_VCO_VRES_SHIFT)
#define REG_DDRPLL_VCO_VRES(n)               BITFIELD_VAL(REG_DDRPLL_VCO_VRES, n)
#define REG_DDRPLL_REFCLK_SEL                (1 << 7)

// REG_0DB
#define REG_DDRPLL_CAL_OPT                   (1 << 0)
#define REG_DDRPLL_INIT_DELAY_SHIFT          1
#define REG_DDRPLL_INIT_DELAY_MASK           (0x7 << REG_DDRPLL_INIT_DELAY_SHIFT)
#define REG_DDRPLL_INIT_DELAY(n)             BITFIELD_VAL(REG_DDRPLL_INIT_DELAY, n)
#define REG_DDRPLL_CNT_TIME_SHIFT            4
#define REG_DDRPLL_CNT_TIME_MASK             (0x7 << REG_DDRPLL_CNT_TIME_SHIFT)
#define REG_DDRPLL_CNT_TIME(n)               BITFIELD_VAL(REG_DDRPLL_CNT_TIME, n)
#define REG_DDRPLL_WAIT_TIME_SHIFT           7
#define REG_DDRPLL_WAIT_TIME_MASK            (0x7 << REG_DDRPLL_WAIT_TIME_SHIFT)
#define REG_DDRPLL_WAIT_TIME(n)              BITFIELD_VAL(REG_DDRPLL_WAIT_TIME, n)
#define REG_DDRPLL_CAL_MANUAL                (1 << 10)
#define REG_DDRPLL_CAL_CNT_EN_DR             (1 << 11)
#define REG_DDRPLL_CAL_CNT_EN                (1 << 12)
#define REG_DDRPLL_CAL_EN_DR                 (1 << 13)
#define REG_DDRPLL_CAL_EN                    (1 << 14)

// REG_0DC
#define REG_DDRPLL_VCO_CAPBANK_DR            (1 << 0)
#define REG_DDRPLL_VCO_CAPBANK_SHIFT         1
#define REG_DDRPLL_VCO_CAPBANK_MASK          (0x1FF << REG_DDRPLL_VCO_CAPBANK_SHIFT)
#define REG_DDRPLL_VCO_CAPBANK(n)            BITFIELD_VAL(REG_DDRPLL_VCO_CAPBANK, n)
#define REG_DDRPLL_MULT_SHIFT                10
#define REG_DDRPLL_MULT_MASK                 (0x7 << REG_DDRPLL_MULT_SHIFT)
#define REG_DDRPLL_MULT(n)                   BITFIELD_VAL(REG_DDRPLL_MULT, n)
#define REG_DDRPLL_CAL_CLK_FREQ_SHIFT        13
#define REG_DDRPLL_CAL_CLK_FREQ_MASK         (0x3 << REG_DDRPLL_CAL_CLK_FREQ_SHIFT)
#define REG_DDRPLL_CAL_CLK_FREQ(n)           BITFIELD_VAL(REG_DDRPLL_CAL_CLK_FREQ, n)

// REG_0DD
#define REG_DDRPLL_SDM_FREQWORD_15_0_SHIFT   0
#define REG_DDRPLL_SDM_FREQWORD_15_0_MASK    (0xFFFF << REG_DDRPLL_SDM_FREQWORD_15_0_SHIFT)
#define REG_DDRPLL_SDM_FREQWORD_15_0(n)      BITFIELD_VAL(REG_DDRPLL_SDM_FREQWORD_15_0, n)

// REG_0DE
#define REG_DDRPLL_SDM_FREQWORD_31_16_SHIFT  0
#define REG_DDRPLL_SDM_FREQWORD_31_16_MASK   (0xFFFF << REG_DDRPLL_SDM_FREQWORD_31_16_SHIFT)
#define REG_DDRPLL_SDM_FREQWORD_31_16(n)     BITFIELD_VAL(REG_DDRPLL_SDM_FREQWORD_31_16, n)

// REG_0DF
#define REG_DDRPLL_PU_DR                     (1 << 0)
#define REG_DDRPLL_PU                        (1 << 1)
#define REG_DDRPLL_LDO_PU_DR                 (1 << 2)
#define REG_DDRPLL_LDO_PU                    (1 << 3)
#define REG_DDRPLL_PRECHARGE_DR              (1 << 4)
#define REG_DDRPLL_PRECHARGE                 (1 << 5)
#define REG_DDRPLL_LPF_PRECHARGE_DR          (1 << 6)
#define REG_DDRPLL_LPF_PRECHARGE             (1 << 7)
#define REG_DDRPLL_LOOP_RESETB_DR            (1 << 8)
#define REG_DDRPLL_LOOP_RESETB               (1 << 9)
#define REG_DDRPLL_CAPBANK_CAL_BYPASS        (1 << 10)
#define REG_DDRPLL_CAL_RESETN_DR             (1 << 11)
#define REG_DDRPLL_CAL_RESETN                (1 << 12)
#define REG_DDRPLL_SDM_RESETN_DR             (1 << 13)
#define REG_DDRPLL_SDM_RESETN                (1 << 14)

// REG_0E0
#define REG_DDRPLL_CLK_FBC_EDGE              (1 << 0)
#define REG_DDRPLL_INT_DEC_SEL_SHIFT         1
#define REG_DDRPLL_INT_DEC_SEL_MASK          (0x7 << REG_DDRPLL_INT_DEC_SEL_SHIFT)
#define REG_DDRPLL_INT_DEC_SEL(n)            BITFIELD_VAL(REG_DDRPLL_INT_DEC_SEL, n)
#define REG_DDRPLL_PRESCALER_DEL_SHIFT       4
#define REG_DDRPLL_PRESCALER_DEL_MASK        (0xF << REG_DDRPLL_PRESCALER_DEL_SHIFT)
#define REG_DDRPLL_PRESCALER_DEL(n)          BITFIELD_VAL(REG_DDRPLL_PRESCALER_DEL, n)
#define REG_DDRPLL_DITHER_BYPASS             (1 << 8)
#define REG_DDRPLL_SDM_CLK_SEL_DR            (1 << 9)
#define REG_DDRPLL_SDM_CLK_SEL               (1 << 10)

// REG_0E1
#define REG_DDRPLL_DIV_DR                    (1 << 0)
#define REG_DDRPLL_DIV_INT_SHIFT             1
#define REG_DDRPLL_DIV_INT_MASK              (0x7F << REG_DDRPLL_DIV_INT_SHIFT)
#define REG_DDRPLL_DIV_INT(n)                BITFIELD_VAL(REG_DDRPLL_DIV_INT, n)
#define REG_DDRPLL_DIV_FRAC_SHIFT            8
#define REG_DDRPLL_DIV_FRAC_MASK             (0x3 << REG_DDRPLL_DIV_FRAC_SHIFT)
#define REG_DDRPLL_DIV_FRAC(n)               BITFIELD_VAL(REG_DDRPLL_DIV_FRAC, n)
#define REG_DDRPLL_CAL_CLK_EN_DR             (1 << 10)
#define REG_DDRPLL_CAL_CLK_EN                (1 << 11)
#define REG_DDRPLL_SEL_SYNCLK_SHIFT          12
#define REG_DDRPLL_SEL_SYNCLK_MASK           (0x7 << REG_DDRPLL_SEL_SYNCLK_SHIFT)
#define REG_DDRPLL_SEL_SYNCLK(n)             BITFIELD_VAL(REG_DDRPLL_SEL_SYNCLK, n)


// REG_0E2
#define REG_DDRPLL_WAIT_T1_VALUE_SHIFT       0
#define REG_DDRPLL_WAIT_T1_VALUE_MASK        (0xFF << REG_DDRPLL_WAIT_T1_VALUE_SHIFT)
#define REG_DDRPLL_WAIT_T1_VALUE(n)          BITFIELD_VAL(REG_DDRPLL_WAIT_T1_VALUE, n)
#define REG_DDRPLL_WAIT_T2_VALUE_SHIFT       8
#define REG_DDRPLL_WAIT_T2_VALUE_MASK        (0xFF << REG_DDRPLL_WAIT_T2_VALUE_SHIFT)
#define REG_DDRPLL_WAIT_T2_VALUE(n)          BITFIELD_VAL(REG_DDRPLL_WAIT_T2_VALUE, n)

// REG_0E3
#define REG_DDRPLL_WAIT_T3_VALUE_SHIFT       0
#define REG_DDRPLL_WAIT_T3_VALUE_MASK        (0xFF << REG_DDRPLL_WAIT_T3_VALUE_SHIFT)
#define REG_DDRPLL_WAIT_T3_VALUE(n)          BITFIELD_VAL(REG_DDRPLL_WAIT_T3_VALUE, n)
#define REG_DDRPLL_WAIT_T4_VALUE_SHIFT       8
#define REG_DDRPLL_WAIT_T4_VALUE_MASK        (0xFF << REG_DDRPLL_WAIT_T4_VALUE_SHIFT)
#define REG_DDRPLL_WAIT_T4_VALUE(n)          BITFIELD_VAL(REG_DDRPLL_WAIT_T4_VALUE, n)

// REG_0E4
#define REG_DDRPLL_POSTDIV_PU_DR             (1 << 0)
#define REG_DDRPLL_POSTDIV_PU                (1 << 1)
#define REG_DDRPLL_DIV_PU_DR                 (1 << 2)
#define REG_DDRPLL_DIV_PU                    (1 << 3)
#define REG_DDRPLL_CP_PRECHARGE_DR           (1 << 4)
#define REG_DDRPLL_CP_PRECHARGE              (1 << 5)
#define REG_DDRPLL_CP_PU_DR                  (1 << 6)
#define REG_DDRPLL_CP_PU                     (1 << 7)
#define REG_DDRPLL_VCO_PU_DR                 (1 << 8)
#define REG_DDRPLL_VCO_PU                    (1 << 9)
#define REG_DDRPLL_LDO_PRECHARGE_DR          (1 << 10)
#define REG_DDRPLL_LDO_PRECHARGE             (1 << 11)

// REG_0E5
#define SAR_OFFSET_BT_VREF_SHIFT            0
#define SAR_OFFSET_BT_VREF_MASK             (0x3F << SAR_OFFSET_BT_VREF_SHIFT)
#define SAR_OFFSET_BT_VREF(n)               BITFIELD_VAL(SAR_OFFSET_BT_VREF, n)
#define SAR_OFFSET_VREF_CALIB               (1 << 6)
#define REG_DDRPLL_1M_PULSE_ALWAYS_ON        (1 << 7)
#define REG_DDRPLL_1M_PULSE_MODE             (1 << 8)

// REG_0E6
#define REG_DDRPLL_LPF_PFD_RSTB              (1 << 0)
#define REG_DDRPLL_LPF_PFD_RSTB_SEL          (1 << 1)
#define REG_DDRPLL_VRES_LDO_SHIFT            3
#define REG_DDRPLL_VRES_LDO_MASK             (0x7 << REG_DDRPLL_VRES_LDO_SHIFT)
#define REG_DDRPLL_VRES_LDO(n)               BITFIELD_VAL(REG_DDRPLL_VRES_LDO, n)
#define REG_DDRPLL_LPF_PRECHAR_VT_SHIFT      7
#define REG_DDRPLL_LPF_PRECHAR_VT_MASK       (0x3 << REG_DDRPLL_LPF_PRECHAR_VT_SHIFT)
#define REG_DDRPLL_LPF_PRECHAR_VT(n)         BITFIELD_VAL(REG_DDRPLL_LPF_PRECHAR_VT, n)
#define REG_DDRPLL_EN_TEMC                   (1 << 9)
#define REG_DDRPLL_EN_TEMC_ITUNE_SHIFT       10
#define REG_DDRPLL_EN_TEMC_ITUNE_MASK        (0x3 << REG_DDRPLL_EN_TEMC_ITUNE_SHIFT)
#define REG_DDRPLL_EN_TEMC_ITUNE(n)          BITFIELD_VAL(REG_DDRPLL_EN_TEMC_ITUNE, n)
#define REG_DDRPLL_VRES_VCO_SHIFT            12
#define REG_DDRPLL_VRES_VCO_MASK             (0x3 << REG_DDRPLL_VRES_VCO_SHIFT)
#define REG_DDRPLL_VRES_VCO(n)               BITFIELD_VAL(REG_DDRPLL_VRES_VCO, n)
#define REG_DDRPLL_VRES_CP_SHIFT             14
#define REG_DDRPLL_VRES_CP_MASK              (0x3 << REG_DDRPLL_VRES_CP_SHIFT)
#define REG_DDRPLL_VRES_CP(n)                BITFIELD_VAL(REG_DDRPLL_VRES_CP, n)

// REG_0E7
#define REG_DDRPLL_CP_BYPASS_R               (1 << 0)
#define REG_DDRPLL_CP_MODE                   (1 << 1)
#define REG_DDRPLL_VRES_DIV_SHIFT            2
#define REG_DDRPLL_VRES_DIV_MASK             (0x3 << REG_DDRPLL_VRES_DIV_SHIFT)
#define REG_DDRPLL_VRES_DIV(n)               BITFIELD_VAL(REG_DDRPLL_VRES_DIV, n)
#define REG_DDRPLL_DIV_DLY_CAP_SHIFT         4
#define REG_DDRPLL_DIV_DLY_CAP_MASK          (0xF << REG_DDRPLL_DIV_DLY_CAP_SHIFT)
#define REG_DDRPLL_DIV_DLY_CAP(n)            BITFIELD_VAL(REG_DDRPLL_DIV_DLY_CAP, n)
#define REG_DDRPLL_VRES_VCO_ICTRL_SHIFT      13
#define REG_DDRPLL_VRES_VCO_ICTRL_MASK       (0x7 << REG_DDRPLL_VRES_VCO_ICTRL_SHIFT)
#define REG_DDRPLL_VRES_VCO_ICTRL(n)         BITFIELD_VAL(REG_DDRPLL_VRES_VCO_ICTRL, n)

// REG_0E8
#define REG_DDRPLL_REFCLK_EDG_SW             (1 << 0)
#define REG_DDRPLL_SEL_DYNCLK_SHIFT          1
#define REG_DDRPLL_SEL_DYNCLK_MASK           (0x7 << REG_DDRPLL_SEL_DYNCLK_SHIFT)
#define REG_DDRPLL_SEL_DYNCLK(n)             BITFIELD_VAL(REG_DDRPLL_SEL_DYNCLK, n)
#define REG_DDRPLL_VRES_PDIV_SHIFT           4
#define REG_DDRPLL_VRES_PDIV_MASK            (0x3 << REG_DDRPLL_VRES_PDIV_SHIFT)
#define REG_DDRPLL_VRES_PDIV(n)              BITFIELD_VAL(REG_DDRPLL_VRES_PDIV, n)
#define REG_DDRPLL_EN_DSICLK                 (1 << 6)
#define DIG_DDRPLL_EN_PSRAMCLK               (1 << 7)
#define DIG_DDRPLL_EN_MCUCLK                 (1 << 8)
#define REG_DDRPLL_DIVN_SHIFT                9
#define REG_DDRPLL_DIVN_MASK                 (0x1F << REG_DDRPLL_DIVN_SHIFT)
#define REG_DDRPLL_DIVN(n)                   BITFIELD_VAL(REG_DDRPLL_DIVN, n)
#define REG_DDRPLL_EN_DET                    (1 << 14)

// REG_0E9
#define REG_DDRPLL_DIVN_PSRAM_SHIFT          0
#define REG_DDRPLL_DIVN_PSRAM_MASK           (0x1F << REG_DDRPLL_DIVN_PSRAM_SHIFT)
#define REG_DDRPLL_DIVN_PSRAM(n)             BITFIELD_VAL(REG_DDRPLL_DIVN_PSRAM, n)
#define REG_DDRPLL_DIVN_MCU_SHIFT            5
#define REG_DDRPLL_DIVN_MCU_MASK             (0x1F << REG_DDRPLL_DIVN_MCU_SHIFT)
#define REG_DDRPLL_DIVN_MCU(n)               BITFIELD_VAL(REG_DDRPLL_DIVN_MCU, n)
#define REG_DDRPLL_VRES_TST_SHIFT            10
#define REG_DDRPLL_VRES_TST_MASK             (0xF << REG_DDRPLL_VRES_TST_SHIFT)
#define REG_DDRPLL_VRES_TST(n)               BITFIELD_VAL(REG_DDRPLL_VRES_TST, n)
#define REG_DDRPLL_EN_DDRCLK                 (1 << 14)

#endif
