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
#ifndef __REG_DSPPLL_BEST2003_H__
#define __REG_DSPPLL_BEST2003_H__

#include "plat_types.h"

enum DSPPLL_REG_T {
    DSPPLL_REG_CB   = 0xCB,
    DSPPLL_REG_CC,
    DSPPLL_REG_CD,
    DSPPLL_REG_CE,
    DSPPLL_REG_CF,
    DSPPLL_REG_D0,
    DSPPLL_REG_D1,
    DSPPLL_REG_D2,
    DSPPLL_REG_D3,
    DSPPLL_REG_D4,
    DSPPLL_REG_D5,
    DSPPLL_REG_D6,
    DSPPLL_REG_D7,
    DSPPLL_REG_D8,
    DSPPLL_REG_D9,
    DSPPLL_REG_DA,
};

// REG_0CB
#define DIG_A7PLL_LOCK                      (1 << 15)
#define SAR_RAW_DATA_1_0_SHIFT              0
#define SAR_RAW_DATA_1_0_MASK               (0x3 << SAR_RAW_DATA_1_0_SHIFT)
#define SAR_RAW_DATA_1_0(n)                 BITFIELD_VAL(SAR_RAW_DATA_1_0, n)


// REG_0CC
#define REG_A7PLL_LOCKDET_BYPASS            (1 << 14)
#define REG_A7PLL_ICP_SHIFT                 11
#define REG_A7PLL_ICP_MASK                  (0x7 << REG_A7PLL_ICP_SHIFT)
#define REG_A7PLL_ICP(n)                    BITFIELD_VAL(REG_A7PLL_ICP, n)
#define REG_A7PLL_BW_SEL_SHIFT              7
#define REG_A7PLL_BW_SEL_MASK               (0xF << REG_A7PLL_BW_SEL_SHIFT)
#define REG_A7PLL_BW_SEL(n)                 BITFIELD_VAL(REG_A7PLL_BW_SEL, n)
#define REG_A7PLL_IOFST_SHIFT               4
#define REG_A7PLL_IOFST_MASK                (0x7 << REG_A7PLL_IOFST_SHIFT)
#define REG_A7PLL_IOFST(n)                  BITFIELD_VAL(REG_A7PLL_IOFST, n)
#define REG_A7PLL_REFCLK_SEL                (1 << 3)
#define REG_A7PLL_VCO_ICTRL_SHIFT           0
#define REG_A7PLL_VCO_ICTRL_MASK            (0x7 << REG_A7PLL_VCO_ICTRL_SHIFT)
#define REG_A7PLL_VCO_ICTRL(n)              BITFIELD_VAL(REG_A7PLL_VCO_ICTRL, n)

// REG_0CD
#define REG_A7PLL_CAL_EN                    (1 << 14)
#define REG_A7PLL_CAL_EN_DR                 (1 << 13)
#define REG_A7PLL_CAL_CNT_EN                (1 << 12)
#define REG_A7PLL_CAL_CNT_EN_DR             (1 << 11)
#define REG_A7PLL_CAL_MANUAL                (1 << 10)
#define REG_A7PLL_WAIT_TIME_SHIFT           7
#define REG_A7PLL_WAIT_TIME_MASK            (0x7 << REG_A7PLL_WAIT_TIME_SHIFT)
#define REG_A7PLL_WAIT_TIME(n)              BITFIELD_VAL(REG_A7PLL_WAIT_TIME, n)
#define REG_A7PLL_CNT_TIME_SHIFT            4
#define REG_A7PLL_CNT_TIME_MASK             (0x7 << REG_A7PLL_CNT_TIME_SHIFT)
#define REG_A7PLL_CNT_TIME(n)               BITFIELD_VAL(REG_A7PLL_CNT_TIME, n)
#define REG_A7PLL_INIT_DELAY_SHIFT          1
#define REG_A7PLL_INIT_DELAY_MASK           (0x7 << REG_A7PLL_INIT_DELAY_SHIFT)
#define REG_A7PLL_INIT_DELAY(n)             BITFIELD_VAL(REG_A7PLL_INIT_DELAY, n)
#define REG_A7PLL_CAL_OPT                   (1 << 0)

// REG_0CE
#define REG_A7PLL_CAL_CLK_FREQ_SHIFT        13
#define REG_A7PLL_CAL_CLK_FREQ_MASK         (0x3 << REG_A7PLL_CAL_CLK_FREQ_SHIFT)
#define REG_A7PLL_CAL_CLK_FREQ(n)           BITFIELD_VAL(REG_A7PLL_CAL_CLK_FREQ, n)
#define REG_A7PLL_MULT_SHIFT                10
#define REG_A7PLL_MULT_MASK                 (0x7 << REG_A7PLL_MULT_SHIFT)
#define REG_A7PLL_MULT(n)                   BITFIELD_VAL(REG_A7PLL_MULT, n)
#define REG_A7PLL_VCO_CAPBANK_SHIFT         1
#define REG_A7PLL_VCO_CAPBANK_MASK          (0x1FF << REG_A7PLL_VCO_CAPBANK_SHIFT)
#define REG_A7PLL_VCO_CAPBANK(n)            BITFIELD_VAL(REG_A7PLL_VCO_CAPBANK, n)
#define REG_A7PLL_VCO_CAPBANK_DR            (1 << 0)

// REG_0CF
#define REG_A7PLL_SDM_FREQWORD_15_0_SHIFT   0
#define REG_A7PLL_SDM_FREQWORD_15_0_MASK    (0xFFFF << REG_A7PLL_SDM_FREQWORD_15_0_SHIFT)
#define REG_A7PLL_SDM_FREQWORD_15_0(n)      BITFIELD_VAL(REG_A7PLL_SDM_FREQWORD_15_0, n)

// REG_0D0
#define REG_A7PLL_SDM_FREQWORD_31_16_SHIFT  0
#define REG_A7PLL_SDM_FREQWORD_31_16_MASK   (0xFFFF << REG_A7PLL_SDM_FREQWORD_31_16_SHIFT)
#define REG_A7PLL_SDM_FREQWORD_31_16(n)     BITFIELD_VAL(REG_A7PLL_SDM_FREQWORD_31_16, n)

// REG_0D1
#define REG_A7PLL_SDM_RESETN                (1 << 14)
#define REG_A7PLL_SDM_RESETN_DR             (1 << 13)
#define REG_A7PLL_CAL_RESETN                (1 << 12)
#define REG_A7PLL_CAL_RESETN_DR             (1 << 11)
#define REG_A7PLL_CAPBANK_CAL_BYPASS        (1 << 10)
#define REG_A7PLL_LOOP_RESETB               (1 << 9)
#define REG_A7PLL_LOOP_RESETB_DR            (1 << 8)
#define REG_A7PLL_LPF_PRECHARGE             (1 << 7)
#define REG_A7PLL_LPF_PRECHARGE_DR          (1 << 6)
#define REG_A7PLL_PRECHARGE                 (1 << 5)
#define REG_A7PLL_PRECHARGE_DR              (1 << 4)
#define REG_A7PLL_LDO_PU                    (1 << 3)
#define REG_A7PLL_LDO_PU_DR                 (1 << 2)
#define REG_A7PLL_PU                        (1 << 1)
#define REG_A7PLL_PU_DR                     (1 << 0)

// REG_0D2
#define REG_A7PLL_1M_PULSE_MODE             (1 << 15)
#define REG_A7PLL_SEL_CAL_VT_SHIFT          13
#define REG_A7PLL_SEL_CAL_VT_MASK           (0x3 << REG_A7PLL_SEL_CAL_VT_SHIFT)
#define REG_A7PLL_SEL_CAL_VT(n)             BITFIELD_VAL(REG_A7PLL_SEL_CAL_VT, n)
#define REG_A7PLL_CPMODE                    (1 << 12)
#define REG_A7PLL_CAL_CLK_SEL               (1 << 11)
#define REG_A7PLL_SDM_CLK_SEL               (1 << 10)
#define REG_A7PLL_SDM_CLK_SEL_DR            (1 << 9)
#define REG_A7PLL_DITHER_BYPASS             (1 << 8)
#define REG_A7PLL_PRESCALER_DEL_SHIFT       4
#define REG_A7PLL_PRESCALER_DEL_MASK        (0xF << REG_A7PLL_PRESCALER_DEL_SHIFT)
#define REG_A7PLL_PRESCALER_DEL(n)          BITFIELD_VAL(REG_A7PLL_PRESCALER_DEL, n)
#define REG_A7PLL_INT_DEC_SEL_SHIFT         1
#define REG_A7PLL_INT_DEC_SEL_MASK          (0x7 << REG_A7PLL_INT_DEC_SEL_SHIFT)
#define REG_A7PLL_INT_DEC_SEL(n)            BITFIELD_VAL(REG_A7PLL_INT_DEC_SEL, n)
#define REG_A7PLL_CLK_FBC_EDGE              (1 << 0)

// REG_0D3
#define REG_A7PLL_SEL_SYNCLK_SHIFT          12
#define REG_A7PLL_SEL_SYNCLK_MASK           (0x7 << REG_A7PLL_SEL_SYNCLK_SHIFT)
#define REG_A7PLL_SEL_SYNCLK(n)             BITFIELD_VAL(REG_A7PLL_SEL_SYNCLK, n)
#define REG_A7PLL_CAL_CLK_EN                (1 << 11)
#define REG_A7PLL_CAL_CLK_EN_DR             (1 << 10)
#define REG_A7PLL_DIV_FRAC_SHIFT            8
#define REG_A7PLL_DIV_FRAC_MASK             (0x3 << REG_A7PLL_DIV_FRAC_SHIFT)
#define REG_A7PLL_DIV_FRAC(n)               BITFIELD_VAL(REG_A7PLL_DIV_FRAC, n)
#define REG_A7PLL_DIV_INT_SHIFT             1
#define REG_A7PLL_DIV_INT_MASK              (0x7F << REG_A7PLL_DIV_INT_SHIFT)
#define REG_A7PLL_DIV_INT(n)                BITFIELD_VAL(REG_A7PLL_DIV_INT, n)
#define REG_A7PLL_DIV_DR                    (1 << 0)

// REG_0D4
#define REG_A7PLL_WAIT_T2_VALUE_SHIFT       8
#define REG_A7PLL_WAIT_T2_VALUE_MASK        (0xFF << REG_A7PLL_WAIT_T2_VALUE_SHIFT)
#define REG_A7PLL_WAIT_T2_VALUE(n)          BITFIELD_VAL(REG_A7PLL_WAIT_T2_VALUE, n)
#define REG_A7PLL_WAIT_T1_VALUE_SHIFT       0
#define REG_A7PLL_WAIT_T1_VALUE_MASK        (0xFF << REG_A7PLL_WAIT_T1_VALUE_SHIFT)
#define REG_A7PLL_WAIT_T1_VALUE(n)          BITFIELD_VAL(REG_A7PLL_WAIT_T1_VALUE, n)

// REG_0D5
#define REG_A7PLL_WAIT_T4_VALUE_SHIFT       8
#define REG_A7PLL_WAIT_T4_VALUE_MASK        (0xFF << REG_A7PLL_WAIT_T4_VALUE_SHIFT)
#define REG_A7PLL_WAIT_T4_VALUE(n)          BITFIELD_VAL(REG_A7PLL_WAIT_T4_VALUE, n)
#define REG_A7PLL_WAIT_T3_VALUE_SHIFT       0
#define REG_A7PLL_WAIT_T3_VALUE_MASK        (0xFF << REG_A7PLL_WAIT_T3_VALUE_SHIFT)
#define REG_A7PLL_WAIT_T3_VALUE(n)          BITFIELD_VAL(REG_A7PLL_WAIT_T3_VALUE, n)


// REG_0D6
#define REG_A7_1M_PULSE_MODE                (1 << 13)
#define REG_A7_1M_PULSE_ALWAYS_ON           (1 << 12)
#define REG_A7PLL_LDO_PRECHARGE             (1 << 11)
#define REG_A7PLL_LDO_PRECHARGE_DR          (1 << 10)
#define REG_A7PLL_VCO_PU                    (1 << 9)
#define REG_A7PLL_VCO_PU_DR                 (1 << 8)
#define REG_A7PLL_CP_PU                     (1 << 7)
#define REG_A7PLL_CP_PU_DR                  (1 << 6)
#define REG_A7PLL_CP_PRECHARGE              (1 << 5)
#define REG_A7PLL_CP_PRECHARGE_DR           (1 << 4)
#define REG_A7PLL_DIV_PU                    (1 << 3)
#define REG_A7PLL_DIV_PU_DR                 (1 << 2)
#define REG_A7PLL_POSTDIV_PU                (1 << 1)
#define REG_A7PLL_POSTDIV_PU_DR             (1 << 0)

// REG_0D7
#define REG_A7PLL_VRES_CP_SHIFT             14
#define REG_A7PLL_VRES_CP_MASK              (0x3 << REG_A7PLL_VRES_CP_SHIFT)
#define REG_A7PLL_VRES_CP(n)                BITFIELD_VAL(REG_A7PLL_VRES_CP, n)
#define REG_A7PLL_VRES_VCO_SHIFT            12
#define REG_A7PLL_VRES_VCO_MASK             (0x3 << REG_A7PLL_VRES_VCO_SHIFT)
#define REG_A7PLL_VRES_VCO(n)               BITFIELD_VAL(REG_A7PLL_VRES_VCO, n)
#define REG_A7PLL_EN_TEMC_ITUNE_SHIFT       10
#define REG_A7PLL_EN_TEMC_ITUNE_MASK        (0x3 << REG_A7PLL_EN_TEMC_ITUNE_SHIFT)
#define REG_A7PLL_EN_TEMC_ITUNE(n)          BITFIELD_VAL(REG_A7PLL_EN_TEMC_ITUNE, n)
#define REG_A7PLL_EN_TEMC                   (1 << 9)
#define REG_A7PLL_LPF_PRECHAR_VT_SHIFT      7
#define REG_A7PLL_LPF_PRECHAR_VT_MASK       (0x3 << REG_A7PLL_LPF_PRECHAR_VT_SHIFT)
#define REG_A7PLL_LPF_PRECHAR_VT(n)         BITFIELD_VAL(REG_A7PLL_LPF_PRECHAR_VT, n)
#define REG_A7PLL_VRES_LDO_SHIFT            3
#define REG_A7PLL_VRES_LDO_MASK             (0x7 << REG_A7PLL_VRES_LDO_SHIFT)
#define REG_A7PLL_VRES_LDO(n)               BITFIELD_VAL(REG_A7PLL_VRES_LDO, n)
#define REG_A7PLL_LPF_PFD_RSTB_SEL          (1 << 1)
#define REG_A7PLL_LPF_PFD_RSTB              (1 << 0)


// REG_0D8
#define REG_A7PLL_DIV_DLY_CAP_SHIFT         4
#define REG_A7PLL_DIV_DLY_CAP_MASK          (0xF << REG_A7PLL_DIV_DLY_CAP_SHIFT)
#define REG_A7PLL_DIV_DLY_CAP(n)            BITFIELD_VAL(REG_A7PLL_DIV_DLY_CAP, n)
#define REG_A7PLL_VRES_DIV_SHIFT            2
#define REG_A7PLL_VRES_DIV_MASK             (0x3 << REG_A7PLL_VRES_DIV_SHIFT)
#define REG_A7PLL_VRES_DIV(n)               BITFIELD_VAL(REG_A7PLL_VRES_DIV, n)
#define REG_A7PLL_CP_MODE                   (1 << 1)
#define REG_A7PLL_CP_BYPASS_R               (1 << 0)


// REG_0D9
#define REG_A7PLL_DIVN_SHIFT                9
#define REG_A7PLL_DIVN_MASK                 (0x1F << REG_A7PLL_DIVN_SHIFT)
#define REG_A7PLL_DIVN(n)                   BITFIELD_VAL(REG_A7PLL_DIVN, n)
#define DIG_A7PLL_EN_MCUCLK                 (1 << 8)
#define DIG_A7PLL_EN_PSRAMCLK               (1 << 7)
#define REG_A7PLL_EN_DSICLK                 (1 << 6)
#define REG_A7PLL_VRES_PDIV_SHIFT           4
#define REG_A7PLL_VRES_PDIV_MASK            (0x3 << REG_A7PLL_VRES_PDIV_SHIFT)
#define REG_A7PLL_VRES_PDIV(n)              BITFIELD_VAL(REG_A7PLL_VRES_PDIV, n)
#define REG_A7PLL_SEL_DYNCLK_SHIFT          1
#define REG_A7PLL_SEL_DYNCLK_MASK           (0x7 << REG_A7PLL_SEL_DYNCLK_SHIFT)
#define REG_A7PLL_SEL_DYNCLK(n)             BITFIELD_VAL(REG_A7PLL_SEL_DYNCLK, n)
#define REG_A7PLL_REFCLK_EDG_SW             (1 << 0)

// REG_0DA
#define REG_A7PLL_VRES_TST_SHIFT            10
#define REG_A7PLL_VRES_TST_MASK             (0xF << REG_A7PLL_VRES_TST_SHIFT)
#define REG_A7PLL_VRES_TST(n)               BITFIELD_VAL(REG_A7PLL_VRES_TST, n)
#define REG_A7PLL_DIVN_MCU_SHIFT            5
#define REG_A7PLL_DIVN_MCU_MASK             (0x1F << REG_A7PLL_DIVN_MCU_SHIFT)
#define REG_A7PLL_DIVN_MCU(n)               BITFIELD_VAL(REG_A7PLL_DIVN_MCU, n)
#define REG_A7PLL_DIVN_PSRAM_SHIFT          0
#define REG_A7PLL_DIVN_PSRAM_MASK           (0x1F << REG_A7PLL_DIVN_PSRAM_SHIFT)
#define REG_A7PLL_DIVN_PSRAM(n)             BITFIELD_VAL(REG_A7PLL_DIVN_PSRAM, n)

#endif
