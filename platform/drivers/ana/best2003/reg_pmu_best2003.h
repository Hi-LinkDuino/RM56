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
#ifndef __REG_PMU_BEST2003_H__
#define __REG_PMU_BEST2003_H__

#include "plat_types.h"

// REG_000
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_001
#define LPO_OFF_CNT_SHIFT                   12
#define LPO_OFF_CNT_MASK                    (0xF << LPO_OFF_CNT_SHIFT)
#define LPO_OFF_CNT(n)                      BITFIELD_VAL(LPO_OFF_CNT, n)
#define POWER_ON_DB_CNT_SHIFT               8
#define POWER_ON_DB_CNT_MASK                (0xF << POWER_ON_DB_CNT_SHIFT)
#define POWER_ON_DB_CNT(n)                  BITFIELD_VAL(POWER_ON_DB_CNT, n)
#define POWER_OFF_CNT_SHIFT                 4
#define POWER_OFF_CNT_MASK                  (0xF << POWER_OFF_CNT_SHIFT)
#define POWER_OFF_CNT(n)                    BITFIELD_VAL(POWER_OFF_CNT, n)
#define CLK_STABLE_CNT_SHIFT                0
#define CLK_STABLE_CNT_MASK                 (0xF << CLK_STABLE_CNT_SHIFT)
#define CLK_STABLE_CNT(n)                   BITFIELD_VAL(CLK_STABLE_CNT, n)

// REG_002
#define REG_PU_VBAT_DIV                     (1 << 15)
#define PU_LPO_DR                           (1 << 14)
#define PU_LPO_REG                          (1 << 13)
#define POWERKEY_WAKEUP_OSC_EN              (1 << 12)
#define RTC_POWER_ON_EN                     (1 << 11)
#define PU_ALL_REG                          (1 << 10)
#define CLK_32K_SEL_SHIFT                   8
#define CLK_32K_SEL_MASK                    (0x3 << CLK_32K_SEL_SHIFT)
#define CLK_32K_SEL(n)                      BITFIELD_VAL(CLK_32K_SEL, n)
#define DEEPSLEEP_MODE_DIGI_DR              (1 << 7)
#define DEEPSLEEP_MODE_DIGI_REG             (1 << 6)
#define RESERVED_ANA_18_17_SHIFT            4
#define RESERVED_ANA_18_17_MASK             (0x3 << RESERVED_ANA_18_17_SHIFT)
#define RESERVED_ANA_18_17(n)               BITFIELD_VAL(RESERVED_ANA_18_17, n)
#define RESETN_ANA_DR                       (1 << 3)
#define RESETN_ANA_REG                      (1 << 2)
#define RESETN_DIG_DR                       (1 << 1)
#define RESETN_DIG_REG                      (1 << 0)

// REG_003
#define RESERVED_ANA_19                     (1 << 15)
#define PU_LP_BIAS_LDO_DSLEEP               (1 << 14)
#define PU_LP_BIAS_LDO_DR                   (1 << 13)
#define PU_LP_BIAS_LDO_REG                  (1 << 12)
#define PU_BIAS_LDO_DR                      (1 << 11)
#define PU_BIAS_LDO_REG                     (1 << 10)
#define BG_VBG_SEL_DR                       (1 << 9)
#define BG_VBG_SEL_REG                      (1 << 8)
#define BG_CONSTANT_GM_BIAS_DR              (1 << 7)
#define BG_CONSTANT_GM_BIAS_REG             (1 << 6)
#define BG_CORE_EN_DR                       (1 << 5)
#define BG_CORE_EN_REG                      (1 << 4)
#define BG_VTOI_EN_DR                       (1 << 3)
#define BG_VTOI_EN_REG                      (1 << 2)
#define BG_NOTCH_EN_DR                      (1 << 1)
#define BG_NOTCH_EN_REG                     (1 << 0)

// REG_004
#define BG_NOTCH_LPF_LOW_BW                 (1 << 15)
#define BG_OPX2                             (1 << 14)
#define BG_PTATX2                           (1 << 13)
#define BG_VBG_RES_SHIFT                    9
#define BG_VBG_RES_MASK                     (0xF << BG_VBG_RES_SHIFT)
#define BG_VBG_RES(n)                       BITFIELD_VAL(BG_VBG_RES, n)
#define BG_CONSTANT_GM_BIAS_BIT_SHIFT       7
#define BG_CONSTANT_GM_BIAS_BIT_MASK        (0x3 << BG_CONSTANT_GM_BIAS_BIT_SHIFT)
#define BG_CONSTANT_GM_BIAS_BIT(n)          BITFIELD_VAL(BG_CONSTANT_GM_BIAS_BIT, n)
#define BG_VTOI_IABS_BIT_SHIFT              2
#define BG_VTOI_IABS_BIT_MASK               (0x1F << BG_VTOI_IABS_BIT_SHIFT)
#define BG_VTOI_IABS_BIT(n)                 BITFIELD_VAL(BG_VTOI_IABS_BIT, n)
#define BG_VTOI_VCAS_BIT_SHIFT              0
#define BG_VTOI_VCAS_BIT_MASK               (0x3 << BG_VTOI_VCAS_BIT_SHIFT)
#define BG_VTOI_VCAS_BIT(n)                 BITFIELD_VAL(BG_VTOI_VCAS_BIT, n)

// REG_005
#define REG_PU_LDO_VRTC_RF_DSLEEP           (1 << 15)
#define REG_PU_LDO_VRTC_RF_DR               (1 << 14)
#define REG_PU_LDO_VRTC_RF_REG              (1 << 13)
#define REG_CHARGE_OUT_INTR_MSK             (1 << 12)
#define REG_CHARGE_IN_INTR_MSK              (1 << 11)
#define REG_AC_ON_OUT_EN                    (1 << 10)
#define REG_AC_ON_IN_EN                     (1 << 9)
#define REG_CHARGE_INTR_EN                  (1 << 8)
#define REG_AC_ON_DB_VALUE_SHIFT            0
#define REG_AC_ON_DB_VALUE_MASK             (0xFF << REG_AC_ON_DB_VALUE_SHIFT)
#define REG_AC_ON_DB_VALUE(n)               BITFIELD_VAL(REG_AC_ON_DB_VALUE, n)

// REG_006
#define REG_BYPASS_VBG_RF_BUFFER_DR         (1 << 15)
#define REG_BYPASS_VBG_RF_BUFFER_REG        (1 << 14)
#define PU_VBG_RF_BUFFER_DR                 (1 << 13)
#define PU_VBG_RF_BUFFER_REG                (1 << 12)
#define RESERVED_ANA_21_20_SHIFT            10
#define RESERVED_ANA_21_20_MASK             (0x3 << RESERVED_ANA_21_20_SHIFT)
#define RESERVED_ANA_21_20(n)               BITFIELD_VAL(RESERVED_ANA_21_20, n)
#define REG_LPO_KTRIM_SHIFT                 4
#define REG_LPO_KTRIM_MASK                  (0x3F << REG_LPO_KTRIM_SHIFT)
#define REG_LPO_KTRIM(n)                    BITFIELD_VAL(REG_LPO_KTRIM, n)
#define REG_LPO_ITRIM_SHIFT                 0
#define REG_LPO_ITRIM_MASK                  (0xF << REG_LPO_ITRIM_SHIFT)
#define REG_LPO_ITRIM(n)                    BITFIELD_VAL(REG_LPO_ITRIM, n)

// REG_007
#define REG_LIGHT_LOAD_VDCDC_LDO            (1 << 15)
#define REG_PULLDOWN_VANA_LDO               (1 << 14)
#define LP_EN_VANA_LDO_DSLEEP               (1 << 13)
#define LP_EN_VANA_LDO_DR                   (1 << 12)
#define LP_EN_VANA_LDO_REG                  (1 << 11)
#define REG_PU_LDO_VANA_DSLEEP              (1 << 10)
#define REG_PU_LDO_VANA_DR                  (1 << 9)
#define REG_PU_LDO_VANA                     (1 << 8)
#define LDO_VANA_VBIT_DSLEEP_SHIFT          4
#define LDO_VANA_VBIT_DSLEEP_MASK           (0xF << LDO_VANA_VBIT_DSLEEP_SHIFT)
#define LDO_VANA_VBIT_DSLEEP(n)             BITFIELD_VAL(LDO_VANA_VBIT_DSLEEP, n)
#define LDO_VANA_VBIT_NORMAL_SHIFT          0
#define LDO_VANA_VBIT_NORMAL_MASK           (0xF << LDO_VANA_VBIT_NORMAL_SHIFT)
#define LDO_VANA_VBIT_NORMAL(n)             BITFIELD_VAL(LDO_VANA_VBIT_NORMAL, n)

// REG_008
#define LP_EN_VDIG_LDO_DSLEEP               (1 << 15)
#define LP_EN_VDIG_LDO_DR                   (1 << 14)
#define LP_EN_VDIG_LDO_REG                  (1 << 13)
#define REG_PU_LDO_VDIG_DSLEEP              (1 << 12)
#define REG_PU_LDO_VDIG_DR                  (1 << 11)
#define REG_PU_LDO_VDIG                     (1 << 10)
#define LDO_VDIG_VBIT_DSLEEP_SHIFT          5
#define LDO_VDIG_VBIT_DSLEEP_MASK           (0x1F << LDO_VDIG_VBIT_DSLEEP_SHIFT)
#define LDO_VDIG_VBIT_DSLEEP(n)             BITFIELD_VAL(LDO_VDIG_VBIT_DSLEEP, n)
#define LDO_VDIG_VBIT_NORMAL_SHIFT          0
#define LDO_VDIG_VBIT_NORMAL_MASK           (0x1F << LDO_VDIG_VBIT_NORMAL_SHIFT)
#define LDO_VDIG_VBIT_NORMAL(n)             BITFIELD_VAL(LDO_VDIG_VBIT_NORMAL, n)

// REG_009
#define LP_EN_VIO_LDO_DSLEEP                (1 << 15)
#define LP_EN_VIO_LDO_DR                    (1 << 14)
#define LP_EN_VIO_LDO_REG                   (1 << 13)
#define REG_PU_LDO_VIO_DSLEEP               (1 << 12)
#define REG_PU_LDO_VIO_DR                   (1 << 11)
#define REG_PU_LDO_VIO                      (1 << 10)
#define LDO_VIO_VBIT_NORMAL_SHIFT           5
#define LDO_VIO_VBIT_NORMAL_MASK            (0x1F << LDO_VIO_VBIT_NORMAL_SHIFT)
#define LDO_VIO_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_VIO_VBIT_NORMAL, n)
#define LDO_VIO_VBIT_DSLEEP_SHIFT           0
#define LDO_VIO_VBIT_DSLEEP_MASK            (0x1F << LDO_VIO_VBIT_DSLEEP_SHIFT)
#define LDO_VIO_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_VIO_VBIT_DSLEEP, n)

// REG_00A
#define POWER_UP_MOD2_CNT_SHIFT             8
#define POWER_UP_MOD2_CNT_MASK              (0xFF << POWER_UP_MOD2_CNT_SHIFT)
#define POWER_UP_MOD2_CNT(n)                BITFIELD_VAL(POWER_UP_MOD2_CNT, n)
#define POWER_UP_MOD1_CNT_SHIFT             0
#define POWER_UP_MOD1_CNT_MASK              (0xFF << POWER_UP_MOD1_CNT_SHIFT)
#define POWER_UP_MOD1_CNT(n)                BITFIELD_VAL(POWER_UP_MOD1_CNT, n)

// REG_00B
#define POWER_UP_MOD4_CNT_SHIFT             8
#define POWER_UP_MOD4_CNT_MASK              (0xFF << POWER_UP_MOD4_CNT_SHIFT)
#define POWER_UP_MOD4_CNT(n)                BITFIELD_VAL(POWER_UP_MOD4_CNT, n)
#define POWER_UP_MOD3_CNT_SHIFT             0
#define POWER_UP_MOD3_CNT_MASK              (0xFF << POWER_UP_MOD3_CNT_SHIFT)
#define POWER_UP_MOD3_CNT(n)                BITFIELD_VAL(POWER_UP_MOD3_CNT, n)

// REG_00C
#define POWER_UP_MOD6_CNT_SHIFT             8
#define POWER_UP_MOD6_CNT_MASK              (0xFF << POWER_UP_MOD6_CNT_SHIFT)
#define POWER_UP_MOD6_CNT(n)                BITFIELD_VAL(POWER_UP_MOD6_CNT, n)
#define POWER_UP_MOD5_CNT_SHIFT             0
#define POWER_UP_MOD5_CNT_MASK              (0xFF << POWER_UP_MOD5_CNT_SHIFT)
#define POWER_UP_MOD5_CNT(n)                BITFIELD_VAL(POWER_UP_MOD5_CNT, n)

// REG_00D
#define REG_XO_32K_ISEL_VALUE_SHIFT         11
#define REG_XO_32K_ISEL_VALUE_MASK          (0x1F << REG_XO_32K_ISEL_VALUE_SHIFT)
#define REG_XO_32K_ISEL_VALUE(n)            BITFIELD_VAL(REG_XO_32K_ISEL_VALUE, n)
#define REG_XO_32K_ISEL_DR                  (1 << 10)
#define REG_VCORE_SSTIME_MODE_SHIFT         8
#define REG_VCORE_SSTIME_MODE_MASK          (0x3 << REG_VCORE_SSTIME_MODE_SHIFT)
#define REG_VCORE_SSTIME_MODE(n)            BITFIELD_VAL(REG_VCORE_SSTIME_MODE, n)
#define REG_LP_BIAS_SEL_LDO_SHIFT           6
#define REG_LP_BIAS_SEL_LDO_MASK            (0x3 << REG_LP_BIAS_SEL_LDO_SHIFT)
#define REG_LP_BIAS_SEL_LDO(n)              BITFIELD_VAL(REG_LP_BIAS_SEL_LDO, n)
#define REG_LOOP_CTL_VCORE_LDO              (1 << 5)
#define REG_PULLDOWN_VIO                    (1 << 4)
#define REG_PULLDOWN_VCORE                  (1 << 3)
#define REG_LOW_VIO                         (1 << 2)
#define REG_PU_AVDD25_ANA                   (1 << 1)
#define REG_BYPASS_VCORE                    (1 << 0)

// REG_00E
#define REG_PU_LDO_VCODEC_DSLEEP            (1 << 15)
#define REG_PU_LDO_VCODEC_DR                (1 << 14)
#define REG_PU_LDO_VCODEC                   (1 << 13)
#define LP_EN_VCODEC_LDO_DSLEEP             (1 << 12)
#define LP_EN_VCODEC_LDO_DR                 (1 << 11)
#define LP_EN_VCODEC_LDO                    (1 << 10)
#define LDO_VCODEC_VBIT_NORMAL_SHIFT        5
#define LDO_VCODEC_VBIT_NORMAL_MASK         (0x1F << LDO_VCODEC_VBIT_NORMAL_SHIFT)
#define LDO_VCODEC_VBIT_NORMAL(n)           BITFIELD_VAL(LDO_VCODEC_VBIT_NORMAL, n)
#define LDO_VCODEC_VBIT_DSLEEP_SHIFT        0
#define LDO_VCODEC_VBIT_DSLEEP_MASK         (0x1F << LDO_VCODEC_VBIT_DSLEEP_SHIFT)
#define LDO_VCODEC_VBIT_DSLEEP(n)           BITFIELD_VAL(LDO_VCODEC_VBIT_DSLEEP, n)

// REG_00F
#define RESERVED_ANA_22                     (1 << 15)
#define REG_PULLDOWN_VCODEC                 (1 << 14)
#define REG_UVLO_SEL_SHIFT                  12
#define REG_UVLO_SEL_MASK                   (0x3 << REG_UVLO_SEL_SHIFT)
#define REG_UVLO_SEL(n)                     BITFIELD_VAL(REG_UVLO_SEL, n)
#define POWER_UP_SOFT_CNT_SHIFT             8
#define POWER_UP_SOFT_CNT_MASK              (0xF << POWER_UP_SOFT_CNT_SHIFT)
#define POWER_UP_SOFT_CNT(n)                BITFIELD_VAL(POWER_UP_SOFT_CNT, n)
#define POWER_UP_BIAS_CNT_SHIFT             4
#define POWER_UP_BIAS_CNT_MASK              (0xF << POWER_UP_BIAS_CNT_SHIFT)
#define POWER_UP_BIAS_CNT(n)                BITFIELD_VAL(POWER_UP_BIAS_CNT, n)
#define RESETN_MOD2_CNT_SHIFT               0
#define RESETN_MOD2_CNT_MASK                (0xF << RESETN_MOD2_CNT_SHIFT)
#define RESETN_MOD2_CNT(n)                  BITFIELD_VAL(RESETN_MOD2_CNT, n)

// REG_010
#define RESERVED_ANA_23                     (1 << 14)
#define REG_XO_32K_ISEL_RAMP_T_STEP_SHIFT   9
#define REG_XO_32K_ISEL_RAMP_T_STEP_MASK    (0x1F << REG_XO_32K_ISEL_RAMP_T_STEP_SHIFT)
#define REG_XO_32K_ISEL_RAMP_T_STEP(n)      BITFIELD_VAL(REG_XO_32K_ISEL_RAMP_T_STEP, n)
#define REG_XO_32K_ISEL_RAMP_START_DLY_SHIFT 0
#define REG_XO_32K_ISEL_RAMP_START_DLY_MASK (0x1FF << REG_XO_32K_ISEL_RAMP_START_DLY_SHIFT)
#define REG_XO_32K_ISEL_RAMP_START_DLY(n)   BITFIELD_VAL(REG_XO_32K_ISEL_RAMP_START_DLY, n)

// REG_011
#define REG_XO_32K_CAPBIT_SHIFT             7
#define REG_XO_32K_CAPBIT_MASK              (0x1FF << REG_XO_32K_CAPBIT_SHIFT)
#define REG_XO_32K_CAPBIT(n)                BITFIELD_VAL(REG_XO_32K_CAPBIT, n)
#define REG_XO_32K_LDO_PRECHARGE            (1 << 6)
#define REG_XO_32K_LDO_PRECHARGE_DR         (1 << 5)
#define REG_XO_32K_LDO_PRECHARGE_VALUE_SHIFT 2
#define REG_XO_32K_LDO_PRECHARGE_VALUE_MASK (0x7 << REG_XO_32K_LDO_PRECHARGE_VALUE_SHIFT)
#define REG_XO_32K_LDO_PRECHARGE_VALUE(n)   BITFIELD_VAL(REG_XO_32K_LDO_PRECHARGE_VALUE, n)
#define REG_XO_32K_PU_LDO                   (1 << 1)
#define REG_XO_32K_PU_LDO_DR                (1 << 0)

// REG_012
#define REG_PMU_LOBUF1_LDO_VRES_SHIFT       9
#define REG_PMU_LOBUF1_LDO_VRES_MASK        (0x7 << REG_PMU_LOBUF1_LDO_VRES_SHIFT)
#define REG_PMU_LOBUF1_LDO_VRES(n)          BITFIELD_VAL(REG_PMU_LOBUF1_LDO_VRES, n)
#define REG_PMU_LOBUF1_SWRC_SHIFT           7
#define REG_PMU_LOBUF1_SWRC_MASK            (0x3 << REG_PMU_LOBUF1_SWRC_SHIFT)
#define REG_PMU_LOBUF1_SWRC(n)              BITFIELD_VAL(REG_PMU_LOBUF1_SWRC, n)
#define REG_PMU_LOBUF1_LDO_PRECHARGE        (1 << 6)
#define REG_PMU_LOBUF1_LDO_PRECHARGE_DR     (1 << 5)
#define REG_PMU_LOBUF1_LDO_PRECHARGE_VALUE_SHIFT 2
#define REG_PMU_LOBUF1_LDO_PRECHARGE_VALUE_MASK (0x7 << REG_PMU_LOBUF1_LDO_PRECHARGE_VALUE_SHIFT)
#define REG_PMU_LOBUF1_LDO_PRECHARGE_VALUE(n) BITFIELD_VAL(REG_PMU_LOBUF1_LDO_PRECHARGE_VALUE, n)
#define REG_PMU_LOBUF1_PU_LDO               (1 << 1)
#define REG_PMU_LOBUF1_PU_LDO_DR            (1 << 0)


// REG_013
#define REG_PMU_LOBUF2_LDO_VRES_SHIFT       9
#define REG_PMU_LOBUF2_LDO_VRES_MASK        (0x7 << REG_PMU_LOBUF2_LDO_VRES_SHIFT)
#define REG_PMU_LOBUF2_LDO_VRES(n)          BITFIELD_VAL(REG_PMU_LOBUF2_LDO_VRES, n)
#define REG_PMU_LOBUF2_SWRC_SHIFT           7
#define REG_PMU_LOBUF2_SWRC_MASK            (0x3 << REG_PMU_LOBUF2_SWRC_SHIFT)
#define REG_PMU_LOBUF2_SWRC(n)              BITFIELD_VAL(REG_PMU_LOBUF2_SWRC, n)
#define REG_PMU_LOBUF2_LDO_PRECHARGE        (1 << 6)
#define REG_PMU_LOBUF2_LDO_PRECHARGE_DR     (1 << 5)
#define REG_PMU_LOBUF2_LDO_PRECHARGE_VALUE_SHIFT 2
#define REG_PMU_LOBUF2_LDO_PRECHARGE_VALUE_MASK (0x7 << REG_PMU_LOBUF2_LDO_PRECHARGE_VALUE_SHIFT)
#define REG_PMU_LOBUF2_LDO_PRECHARGE_VALUE(n) BITFIELD_VAL(REG_PMU_LOBUF2_LDO_PRECHARGE_VALUE, n)
#define REG_PMU_LOBUF2_PU_LDO               (1 << 1)
#define REG_PMU_LOBUF2_PU_LDO_DR            (1 << 0)

// REG_014
#define TEST_MODE_SHIFT                     13
#define TEST_MODE_MASK                      (0x7 << TEST_MODE_SHIFT)
#define TEST_MODE(n)                        BITFIELD_VAL(TEST_MODE, n)
#define REG_BUCK_ANA_IS_GAIN_DSLEEP_SHIFT   10
#define REG_BUCK_ANA_IS_GAIN_DSLEEP_MASK    (0x7 << REG_BUCK_ANA_IS_GAIN_DSLEEP_SHIFT)
#define REG_BUCK_ANA_IS_GAIN_DSLEEP(n)      BITFIELD_VAL(REG_BUCK_ANA_IS_GAIN_DSLEEP, n)
#define REG_BUCK_ANA_IS_GAIN_NORMAL_SHIFT   7
#define REG_BUCK_ANA_IS_GAIN_NORMAL_MASK    (0x7 << REG_BUCK_ANA_IS_GAIN_NORMAL_SHIFT)
#define REG_BUCK_ANA_IS_GAIN_NORMAL(n)      BITFIELD_VAL(REG_BUCK_ANA_IS_GAIN_NORMAL, n)
#define REG_BUCK_ANA_ULP_MODE_DSLEEP        (1 << 6)
#define REG_BUCK_ANA_ULP_MODE_NORMAL        (1 << 5)
#define REG_BUCK_ANA_BURST_MODE_NORMAL      (1 << 4)
#define REG_BUCK_ANA_BURST_MODE_DSLEEP      (1 << 3)
#define REG_PU_DCDC_ANA_DSLEEP              (1 << 2)
#define REG_PU_DCDC_ANA_REG                 (1 << 1)
#define REG_PU_DCDC_ANA_DR                  (1 << 0)

// REG_015
#define REG_BUCK_ANA_VBIT_DSLEEP_SHIFT      8
#define REG_BUCK_ANA_VBIT_DSLEEP_MASK       (0xFF << REG_BUCK_ANA_VBIT_DSLEEP_SHIFT)
#define REG_BUCK_ANA_VBIT_DSLEEP(n)         BITFIELD_VAL(REG_BUCK_ANA_VBIT_DSLEEP, n)
#define REG_BUCK_ANA_VBIT_NORMAL_SHIFT      0
#define REG_BUCK_ANA_VBIT_NORMAL_MASK       (0xFF << REG_BUCK_ANA_VBIT_NORMAL_SHIFT)
#define REG_BUCK_ANA_VBIT_NORMAL(n)         BITFIELD_VAL(REG_BUCK_ANA_VBIT_NORMAL, n)
#define MAX_DCDC_ANA_VBIT_VAL               (REG_BUCK_ANA_VBIT_NORMAL_MASK >> REG_BUCK_ANA_VBIT_NORMAL_SHIFT)

// REG_017
#define REG_SS_VCORE_EN                     (1 << 15)
#define REG_BUCK_HV_ANA_NORMAL              (1 << 13)
#define REG_BUCK_HV_ANA_DSLEEP              (1 << 12)
#define REG_GPADC_RESETN_DR                 (1 << 11)
#define REG_GPADC_RESETN                    (1 << 10)
#define SAR_PWR_BIT_SHIFT                   8
#define SAR_PWR_BIT_MASK                    (0x3 << SAR_PWR_BIT_SHIFT)
#define SAR_PWR_BIT(n)                      BITFIELD_VAL(SAR_PWR_BIT, n)
#define SAR_THERM_GAIN_SHIFT                3
#define SAR_THERM_GAIN_MASK                 (0x3 << SAR_THERM_GAIN_SHIFT)
#define SAR_THERM_GAIN(n)                   BITFIELD_VAL(SAR_THERM_GAIN, n)
#define SAR_VREF_BIT_SHIFT                  0
#define SAR_VREF_BIT_MASK                   (0x7 << SAR_VREF_BIT_SHIFT)
#define SAR_VREF_BIT(n)                     BITFIELD_VAL(SAR_VREF_BIT, n)

// REG_018
#define KEY_DB_DSB                          (1 << 14)
#define KEY_INTERVAL_MODE                   (1 << 13)
#define GPADC_INTERVAL_MODE                 (1 << 12)
#define STATE_RESET                         (1 << 11)
#define SAR_ADC_MODE                        (1 << 10)
#define DELAY_BEFORE_SAMP_SHIFT             8
#define DELAY_BEFORE_SAMP_MASK              (0x3 << DELAY_BEFORE_SAMP_SHIFT)
#define DELAY_BEFORE_SAMP(n)                BITFIELD_VAL(DELAY_BEFORE_SAMP, n)
#define CONV_CLK_INV                        (1 << 7)
#define SAMP_CLK_INV                        (1 << 6)
#define TIME_SAMP_NEG                       (1 << 5)
#define TIME_SAMP_POS                       (1 << 4)
#define SAR_OUT_POLARITY                    (1 << 3)
#define TIMER_SAR_STABLE_SEL_SHIFT          0
#define TIMER_SAR_STABLE_SEL_MASK           (0x7 << TIMER_SAR_STABLE_SEL_SHIFT)
#define TIMER_SAR_STABLE_SEL(n)             BITFIELD_VAL(TIMER_SAR_STABLE_SEL, n)

// REG_019
#define REG_CLK_GPADC_DIV_VALUE_SHIFT       11
#define REG_CLK_GPADC_DIV_VALUE_MASK        (0x1F << REG_CLK_GPADC_DIV_VALUE_SHIFT)
#define REG_CLK_GPADC_DIV_VALUE(n)          BITFIELD_VAL(REG_CLK_GPADC_DIV_VALUE, n)
#define REG_KEY_IN_DR                       (1 << 10)
#define REG_KEY_IN_REG                      (1 << 9)
#define REG_GPADC_EN_DR                     (1 << 8)
#define REG_GPADC_EN_REG_SHIFT              0
#define REG_GPADC_EN_REG_MASK               (0xFF << REG_GPADC_EN_REG_SHIFT)
#define REG_GPADC_EN_REG(n)                 BITFIELD_VAL(REG_GPADC_EN_REG, n)

// REG_01A
#define REG_PMU_VSEL1_SHIFT                 13
#define REG_PMU_VSEL1_MASK                  (0x7 << REG_PMU_VSEL1_SHIFT)
#define REG_PMU_VSEL1(n)                    BITFIELD_VAL(REG_PMU_VSEL1, n)
#define REG_POWER_SEL_CNT_SHIFT             8
#define REG_POWER_SEL_CNT_MASK              (0x1F << REG_POWER_SEL_CNT_SHIFT)
#define REG_POWER_SEL_CNT(n)                BITFIELD_VAL(REG_POWER_SEL_CNT, n)
#define REG_PWR_SEL                         (1 << 6)
#define CLK_BG_EN                           (1 << 5)
#define CLK_BG_DIV_VALUE_SHIFT              0
#define CLK_BG_DIV_VALUE_MASK               (0x1F << CLK_BG_DIV_VALUE_SHIFT)
#define CLK_BG_DIV_VALUE(n)                 BITFIELD_VAL(CLK_BG_DIV_VALUE, n)

// REG_01B
#define KEY_INTERVAL_SHIFT                  0
#define KEY_INTERVAL_MASK                   (0xFFFF << KEY_INTERVAL_SHIFT)
#define KEY_INTERVAL(n)                     BITFIELD_VAL(KEY_INTERVAL, n)

// REG_01C
#define GPADC_INTERVAL_SHIFT                0
#define GPADC_INTERVAL_MASK                 (0xFFFF << GPADC_INTERVAL_SHIFT)
#define GPADC_INTERVAL(n)                   BITFIELD_VAL(GPADC_INTERVAL, n)

// REG_01D
#define CHAN_EN_REG_SHIFT                   0
#define CHAN_EN_REG_MASK                    (0xFF << CHAN_EN_REG_SHIFT)
#define CHAN_EN_REG(n)                      BITFIELD_VAL(CHAN_EN_REG, n)
#define CHIP_ADDR_I2C_SHIFT                 8
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)
#define SLEEP_ALLOW                         (1 << 15)

// REG_01E
#define RESERVED_ANA_24                     (1 << 15)
#define REG_SAR_REF_MODE_SEL                (1 << 14)
#define REG_CLK_GPADC_EN                    (1 << 13)
#define REG_DR_PU_SAR                       (1 << 12)
#define REG_PU_SAR                          (1 << 11)
#define REG_DR_TSC_SAR_BIT                  (1 << 10)
#define REG_TSC_SAR_BIT_SHIFT               0
#define REG_TSC_SAR_BIT_MASK                (0x3FF << REG_TSC_SAR_BIT_SHIFT)
#define REG_TSC_SAR_BIT(n)                  BITFIELD_VAL(REG_TSC_SAR_BIT, n)

// REG_01F
#define USB_INSERT_INTR_MSK                 (1 << 15)
#define RTC_INT1_MSK                        (1 << 14)
#define RTC_INT0_MSK                        (1 << 13)
#define KEY_ERR1_INTR_MSK                   (1 << 12)
#define KEY_ERR0_INTR_MSK                   (1 << 11)
#define KEY_PRESS_INTR_MSK                  (1 << 10)
#define KEY_RELEASE_INTR_MSK                (1 << 9)
#define SAMPLE_DONE_INTR_MSK                (1 << 8)
#define CHAN_DATA_INTR_MSK_SHIFT            0
#define CHAN_DATA_INTR_MSK_MASK             (0xFF << CHAN_DATA_INTR_MSK_SHIFT)
#define CHAN_DATA_INTR_MSK(n)               BITFIELD_VAL(CHAN_DATA_INTR_MSK, n)

// REG_020
#define USB_INSERT_INTR_EN                  (1 << 15)
#define RTC_INT_EN_1                        (1 << 14)
#define RTC_INT_EN_0                        (1 << 13)
#define KEY_ERR1_INTR_EN                    (1 << 12)
#define KEY_ERR0_INTR_EN                    (1 << 11)
#define KEY_PRESS_INTR_EN                   (1 << 10)
#define KEY_RELEASE_INTR_EN                 (1 << 9)
#define SAMPLE_DONE_INTR_EN                 (1 << 8)
#define CHAN_DATA_INTR_EN_SHIFT             0
#define CHAN_DATA_INTR_EN_MASK              (0xFF << CHAN_DATA_INTR_EN_SHIFT)
#define CHAN_DATA_INTR_EN(n)                BITFIELD_VAL(CHAN_DATA_INTR_EN, n)

// REG_021
#define RESERVED_ANA_26_25_SHIFT            14
#define RESERVED_ANA_26_25_MASK             (0x3 << RESERVED_ANA_26_25_SHIFT)
#define RESERVED_ANA_26_25(n)               BITFIELD_VAL(RESERVED_ANA_26_25, n)
#define REG_VCORE_RAMP_EN                   (1 << 8)
#define REG_VCORE_BIT_START_SHIFT           4
#define REG_VCORE_BIT_START_MASK            (0xF << REG_VCORE_BIT_START_SHIFT)
#define REG_VCORE_BIT_START(n)              BITFIELD_VAL(REG_VCORE_BIT_START, n)
#define REG_VCORE_BIT_STOP_SHIFT            0
#define REG_VCORE_BIT_STOP_MASK             (0xF << REG_VCORE_BIT_STOP_SHIFT)
#define REG_VCORE_BIT_STOP(n)               BITFIELD_VAL(REG_VCORE_BIT_STOP, n)

// REG_022
#define CFG_DIV_RTC_1HZ_SHIFT               0
#define CFG_DIV_RTC_1HZ_MASK                (0xFFFF << CFG_DIV_RTC_1HZ_SHIFT)
#define CFG_DIV_RTC_1HZ(n)                  BITFIELD_VAL(CFG_DIV_RTC_1HZ, n)

// REG_023
#define RTC_LOAD_VALUE_15_0_SHIFT           0
#define RTC_LOAD_VALUE_15_0_MASK            (0xFFFF << RTC_LOAD_VALUE_15_0_SHIFT)
#define RTC_LOAD_VALUE_15_0(n)              BITFIELD_VAL(RTC_LOAD_VALUE_15_0, n)

// REG_024
#define RTC_LOAD_VALUE_31_16_SHIFT          0
#define RTC_LOAD_VALUE_31_16_MASK           (0xFFFF << RTC_LOAD_VALUE_31_16_SHIFT)
#define RTC_LOAD_VALUE_31_16(n)             BITFIELD_VAL(RTC_LOAD_VALUE_31_16, n)

// REG_025
#define RTC_MATCH_VALUE_0_15_0_SHIFT        0
#define RTC_MATCH_VALUE_0_15_0_MASK         (0xFFFF << RTC_MATCH_VALUE_0_15_0_SHIFT)
#define RTC_MATCH_VALUE_0_15_0(n)           BITFIELD_VAL(RTC_MATCH_VALUE_0_15_0, n)

// REG_026
#define RTC_MATCH_VALUE_0_31_16_SHIFT       0
#define RTC_MATCH_VALUE_0_31_16_MASK        (0xFFFF << RTC_MATCH_VALUE_0_31_16_SHIFT)
#define RTC_MATCH_VALUE_0_31_16(n)          BITFIELD_VAL(RTC_MATCH_VALUE_0_31_16, n)

// REG_027
#define RTC_MATCH_VALUE_1_15_0_SHIFT        0
#define RTC_MATCH_VALUE_1_15_0_MASK         (0xFFFF << RTC_MATCH_VALUE_1_15_0_SHIFT)
#define RTC_MATCH_VALUE_1_15_0(n)           BITFIELD_VAL(RTC_MATCH_VALUE_1_15_0, n)

// REG_028
#define RTC_MATCH_VALUE_1_31_16_SHIFT       0
#define RTC_MATCH_VALUE_1_31_16_MASK        (0xFFFF << RTC_MATCH_VALUE_1_31_16_SHIFT)
#define RTC_MATCH_VALUE_1_31_16(n)          BITFIELD_VAL(RTC_MATCH_VALUE_1_31_16, n)

// REG_029
#define REG_MIC_BIASA_CHANSEL_SHIFT         14
#define REG_MIC_BIASA_CHANSEL_MASK          (0x3 << REG_MIC_BIASA_CHANSEL_SHIFT)
#define REG_MIC_BIASA_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASA_CHANSEL, n)
#define REG_MIC_BIASA_EN                    (1 << 13)
#define REG_MIC_BIASA_ENLPF                 (1 << 12)
#define REG_MIC_BIASA_VSEL_SHIFT            5
#define REG_MIC_BIASA_VSEL_MASK             (0x3F << REG_MIC_BIASA_VSEL_SHIFT)
#define REG_MIC_BIASA_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASA_VSEL, n)
#define REG_MIC_LDO_RES_SHIFT               1
#define REG_MIC_LDO_RES_MASK                (0xF << REG_MIC_LDO_RES_SHIFT)
#define REG_MIC_LDO_RES(n)                  BITFIELD_VAL(REG_MIC_LDO_RES, n)
#define REG_MIC_LDO_LOOPCTRL                (1 << 0)

// REG_02A
#define REG_MIC_BIASB_CHANSEL_SHIFT         14
#define REG_MIC_BIASB_CHANSEL_MASK          (0x3 << REG_MIC_BIASB_CHANSEL_SHIFT)
#define REG_MIC_BIASB_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASB_CHANSEL, n)
#define REG_MIC_BIASB_EN                    (1 << 13)
#define REG_MIC_BIASB_ENLPF                 (1 << 12)
#define REG_MIC_BIASB_VSEL_SHIFT            5
#define REG_MIC_BIASB_VSEL_MASK             (0x3F << REG_MIC_BIASB_VSEL_SHIFT)
#define REG_MIC_BIASB_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASB_VSEL, n)
#define REG_MIC_LDO_EN                      (1 << 4)
#define REG_MIC_LDO_PULLDOWN                (1 << 3)
#define VBATDET_LP_MODE                     (1 << 2)
#define PU_VBATDET                          (1 << 1)
#define RESERVED_ANA_28                     (1 << 0)

// REG_02B
#define REG_LED_IO1_IBIT_SHIFT              14
#define REG_LED_IO1_IBIT_MASK               (0x3 << REG_LED_IO1_IBIT_SHIFT)
#define REG_LED_IO1_IBIT(n)                 BITFIELD_VAL(REG_LED_IO1_IBIT, n)
#define REG_LED_IO1_OENB_PRE                (1 << 13)
#define REG_LED_IO1_PDEN                    (1 << 12)
#define REG_LED_IO1_PU                      (1 << 11)
#define REG_LED_IO1_PUEN                    (1 << 10)
#define REG_LED_IO1_SEL_SHIFT               8
#define REG_LED_IO1_SEL_MASK                (0x3 << REG_LED_IO1_SEL_SHIFT)
#define REG_LED_IO1_SEL(n)                  BITFIELD_VAL(REG_LED_IO1_SEL, n)
#define REG_LED_IO1_RX_EN                   (1 << 7)
#define REG_LED_IO1_AIO_EN                  (1 << 6)
#define RESERVED_ANA_27                     (1 << 5)
#define VBATDET_RES_SEL_SHIFT               0
#define VBATDET_RES_SEL_MASK                (0x1F << VBATDET_RES_SEL_SHIFT)
#define VBATDET_RES_SEL(n)                  BITFIELD_VAL(VBATDET_RES_SEL, n)

// REG_02C
#define DIG_PU_DCDC_HPPA                    (1 << 2)
#define DIG_PU_DCDC_VCORE                   (1 << 1)
#define DIG_PU_DCDC_ANA                     (1 << 0)

// REG_02D
#define R_PWM2_TOGGLE_SHIFT                 0
#define R_PWM2_TOGGLE_MASK                  (0xFFFF << R_PWM2_TOGGLE_SHIFT)
#define R_PWM2_TOGGLE(n)                    BITFIELD_VAL(R_PWM2_TOGGLE, n)

// REG_02E
#define REG_PWM2_ST1_SHIFT                  0
#define REG_PWM2_ST1_MASK                   (0xFFFF << REG_PWM2_ST1_SHIFT)
#define REG_PWM2_ST1(n)                     BITFIELD_VAL(REG_PWM2_ST1, n)

// REG_02F
#define SUBCNT_DATA2_SHIFT                  8
#define SUBCNT_DATA2_MASK                   (0xFF << SUBCNT_DATA2_SHIFT)
#define SUBCNT_DATA2(n)                     BITFIELD_VAL(SUBCNT_DATA2, n)
#define TG_SUBCNT_D2_ST_SHIFT               1
#define TG_SUBCNT_D2_ST_MASK                (0x7F << TG_SUBCNT_D2_ST_SHIFT)
#define TG_SUBCNT_D2_ST(n)                  BITFIELD_VAL(TG_SUBCNT_D2_ST, n)
#define REG_LED0_OUT                        (1 << 0)

// REG_030
#define RESERVED_ANA_29                     (1 << 15)
#define RESETN_DB_NUMBER_SHIFT              11
#define RESETN_DB_NUMBER_MASK               (0xF << RESETN_DB_NUMBER_SHIFT)
#define RESETN_DB_NUMBER(n)                 BITFIELD_VAL(RESETN_DB_NUMBER, n)
#define RESETN_DB_EN                        (1 << 10)
#define REG_PWM_CLK_EN                      (1 << 9)
#define REG_CLK_PWM_DIV_SHIFT               4
#define REG_CLK_PWM_DIV_MASK                (0x1F << REG_CLK_PWM_DIV_SHIFT)
#define REG_CLK_PWM_DIV(n)                  BITFIELD_VAL(REG_CLK_PWM_DIV, n)
#define REG_PWM2_BR_EN                      (1 << 3)
#define PWM_SELECT_EN                       (1 << 2)
#define PWM_SELECT_INV                      (1 << 1)
#define LED_GPIO_SELECT                     (1 << 0)

// REG_031
#define REG_HW_WDT_COM_RST_TIME_SHIFT       0
#define REG_HW_WDT_COM_RST_TIME_MASK        (0xFFF << REG_HW_WDT_COM_RST_TIME_SHIFT)
#define REG_HW_WDT_COM_RST_TIME(n)          BITFIELD_VAL(REG_HW_WDT_COM_RST_TIME, n)

// REG_032
#define REG_COMMAND_RST_BIT_WIDTH_TOLERANCE_SHIFT 0
#define REG_COMMAND_RST_BIT_WIDTH_TOLERANCE_MASK (0xFF << REG_COMMAND_RST_BIT_WIDTH_TOLERANCE_SHIFT)
#define REG_COMMAND_RST_BIT_WIDTH_TOLERANCE(n) BITFIELD_VAL(REG_COMMAND_RST_BIT_WIDTH_TOLERANCE, n)

// REG_034
#define RESERVED_ANA_30                     (1 << 15)
#define CLK_32K_COUNT_NUM_SHIFT             11
#define CLK_32K_COUNT_NUM_MASK              (0xF << CLK_32K_COUNT_NUM_SHIFT)
#define CLK_32K_COUNT_NUM(n)                BITFIELD_VAL(CLK_32K_COUNT_NUM, n)
#define REG_IPTAT_CORE_I_SEL_SHIFT          5
#define REG_IPTAT_CORE_I_SEL_MASK           (0x3F << REG_IPTAT_CORE_I_SEL_SHIFT)
#define REG_IPTAT_CORE_I_SEL(n)             BITFIELD_VAL(REG_IPTAT_CORE_I_SEL, n)
#define DIG_IPTAT_CORE_EN                   (1 << 4)

// REG_035
#define REG_DIV_HW_RESET_SHIFT              0
#define REG_DIV_HW_RESET_MASK               (0xFFFF << REG_DIV_HW_RESET_SHIFT)
#define REG_DIV_HW_RESET(n)                 BITFIELD_VAL(REG_DIV_HW_RESET, n)

// REG_036
#define REG_WDT_TIMER_SHIFT                 0
#define REG_WDT_TIMER_MASK                  (0xFFFF << REG_WDT_TIMER_SHIFT)
#define REG_WDT_TIMER(n)                    BITFIELD_VAL(REG_WDT_TIMER, n)

// REG_037
#define RESERVED_ANA_33_31_SHIFT            13
#define RESERVED_ANA_33_31_MASK             (0x7 << RESERVED_ANA_33_31_SHIFT)
#define RESERVED_ANA_33_31(n)               BITFIELD_VAL(RESERVED_ANA_33_31, n)
#define CLK_32K_COUNT_EN                    (1 << 12)
#define CLK_32K_COUNT_CLOCK_EN              (1 << 11)
#define POWERON_DETECT_EN                   (1 << 10)
#define MERGE_INTR                          (1 << 9)
#define REG_WDT_EN                          (1 << 8)
#define REG_WDT_RESET_EN                    (1 << 7)
#define REG_HW_RESET_TIME_SHIFT             1
#define REG_HW_RESET_TIME_MASK              (0x3F << REG_HW_RESET_TIME_SHIFT)
#define REG_HW_RESET_TIME(n)                BITFIELD_VAL(REG_HW_RESET_TIME, n)
#define REG_HW_RESET_EN                     (1 << 0)

// REG_038
#define RESERVED_ANA_15_0_SHIFT             0
#define RESERVED_ANA_15_0_MASK              (0xFFFF << RESERVED_ANA_15_0_SHIFT)
#define RESERVED_ANA_15_0(n)                BITFIELD_VAL(RESERVED_ANA_15_0, n)

// REG_039
#define RESERVED_DIG_15_0_SHIFT             0
#define RESERVED_DIG_15_0_MASK              (0xFFFF << RESERVED_DIG_15_0_SHIFT)
#define RESERVED_DIG_15_0(n)                BITFIELD_VAL(RESERVED_DIG_15_0, n)

// REG_03A
#define R_PWMB_TOGGLE_SHIFT                 0
#define R_PWMB_TOGGLE_MASK                  (0xFFFF << R_PWMB_TOGGLE_SHIFT)
#define R_PWMB_TOGGLE(n)                    BITFIELD_VAL(R_PWMB_TOGGLE, n)

// REG_03B
#define REG_PWMB_ST1_SHIFT                  0
#define REG_PWMB_ST1_MASK                   (0xFFFF << REG_PWMB_ST1_SHIFT)
#define REG_PWMB_ST1(n)                     BITFIELD_VAL(REG_PWMB_ST1, n)

// REG_03C
#define SUBCNT_DATAB_SHIFT                  8
#define SUBCNT_DATAB_MASK                   (0xFF << SUBCNT_DATAB_SHIFT)
#define SUBCNT_DATAB(n)                     BITFIELD_VAL(SUBCNT_DATAB, n)
#define TG_SUBCNT_DB_ST_SHIFT               1
#define TG_SUBCNT_DB_ST_MASK                (0x7F << TG_SUBCNT_DB_ST_SHIFT)
#define TG_SUBCNT_DB_ST(n)                  BITFIELD_VAL(TG_SUBCNT_DB_ST, n)
#define REG_LEDB_OUT                        (1 << 0)

// REG_03D
#define RESERVED_PWMB_SHIFT                 10
#define RESERVED_PWMB_MASK                  (0x3F << RESERVED_PWMB_SHIFT)
#define RESERVED_PWMB(n)                    BITFIELD_VAL(RESERVED_PWMB, n)
#define REG_PWMB_CLK_EN                     (1 << 9)
#define REG_CLK_PWMB_DIV_SHIFT              4
#define REG_CLK_PWMB_DIV_MASK               (0x1F << REG_CLK_PWMB_DIV_SHIFT)
#define REG_CLK_PWMB_DIV(n)                 BITFIELD_VAL(REG_CLK_PWMB_DIV, n)
#define REG_PWMB_BR_EN                      (1 << 3)
#define PWMB_SELECT_EN                      (1 << 2)
#define PWMB_SELECT_INV                     (1 << 1)
#define LEDB_GPIO_SELECT                    (1 << 0)

// REG_03E
#define REG_LED_IO2_IBIT_SHIFT              14
#define REG_LED_IO2_IBIT_MASK               (0x3 << REG_LED_IO2_IBIT_SHIFT)
#define REG_LED_IO2_IBIT(n)                 BITFIELD_VAL(REG_LED_IO2_IBIT, n)
#define REG_LED_IO2_OENB_PRE                (1 << 13)
#define REG_LED_IO2_PDEN                    (1 << 12)
#define REG_LED_IO2_PU                      (1 << 11)
#define REG_LED_IO2_PUEN                    (1 << 10)
#define REG_LED_IO2_SEL_SHIFT               8
#define REG_LED_IO2_SEL_MASK                (0x3 << REG_LED_IO2_SEL_SHIFT)
#define REG_LED_IO2_SEL(n)                  BITFIELD_VAL(REG_LED_IO2_SEL, n)
#define REG_LED_IO2_RX_EN                   (1 << 7)
#define REG_LED_IO2_AIO_EN                  (1 << 6)
#define RESERVED_3E_SHIFT                   0
#define RESERVED_3E_MASK                    (0x3F << RESERVED_3E_SHIFT)
#define RESERVED_3E(n)                      BITFIELD_VAL(RESERVED_3E, n)

// REG_03F
#define AC_OUT_LDO_ON_EN                    (1 << 15)

// REG_040
#define RESERVED_40                         (1 << 15)
#define RTC_INTR_TMP_MERGED_MSK             (1 << 14)
#define GPADC_INTR_MERGED_MSK               (1 << 13)
#define CHARGE_INTR_MERGED_MSK              (1 << 12)

// REG_041
#define USB_INTR_MERGED_MSK                 (1 << 15)
#define POWER_ON_INTR_MERGED_MSK            (1 << 14)
#define PMU_GPIO_INTR_MSKED1_MERGED_MSK     (1 << 13)
#define PMU_GPIO_INTR_MSKED2_MERGED_MSK     (1 << 12)
#define WDT_INTR_MSKED_MERGED_MSK           (1 << 11)


// REG_043
#define REG_LOW_LEVEL_INTR_SEL1             (1 << 15)
#define REG_WDT_INTR_EN                     (1 << 14)
#define REG_WDT_INTR_MSK                    (1 << 13)
#define LDO_DCDC2ANA_VBIT_DSLEEP_SHIFT      8
#define LDO_DCDC2ANA_VBIT_DSLEEP_MASK       (0x1F << LDO_DCDC2ANA_VBIT_DSLEEP_SHIFT)
#define LDO_DCDC2ANA_VBIT_DSLEEP(n)         BITFIELD_VAL(LDO_DCDC2ANA_VBIT_DSLEEP, n)

// REG_044
#define REG_32KFRM26M_DIV_SHIFT             6
#define REG_32KFRM26M_DIV_MASK              (0xF << REG_32KFRM26M_DIV_SHIFT)
#define REG_32KFRM26M_DIV(n)                BITFIELD_VAL(REG_32KFRM26M_DIV, n)
#define RESETN_RF_DR                        (1 << 5)
#define RESETN_RF_REG                       (1 << 4)
#define RESETN_MOD1_CNT_SHIFT               0
#define RESETN_MOD1_CNT_MASK                (0xF << RESETN_MOD1_CNT_SHIFT)
#define RESETN_MOD1_CNT(n)                  BITFIELD_VAL(RESETN_MOD1_CNT, n)

// REG_045
#define RESERVED_ANA_16                     (1 << 15)
#define REG_EDGE_INTR_SEL2                  (1 << 14)
#define REG_POS_INTR_SEL2                   (1 << 13)
#define REG_EDGE_INTR_SEL1                  (1 << 12)
#define REG_POS_INTR_SEL1                   (1 << 11)

// REG_046
#define REG_GPIO_I_SEL                      (1 << 15)
#define PMU_DB_BYPASS1                      (1 << 14)
#define PMU_DB_TARGET1_SHIFT                6
#define PMU_DB_TARGET1_MASK                 (0xFF << PMU_DB_TARGET1_SHIFT)
#define PMU_DB_TARGET1(n)                   BITFIELD_VAL(PMU_DB_TARGET1, n)
#define REG_PMU_UART_DR1                    (1 << 5)
#define REG_PMU_UART_TX1                    (1 << 4)
#define REG_PMU_UART_OENB1                  (1 << 3)
#define REG_UART_LEDA_SEL                   (1 << 2)
#define REG_PMU_GPIO_INTR_MSK1              (1 << 1)
#define REG_PMU_GPIO_INTR_EN1               (1 << 0)

// REG_047
#define REG_LOW_LEVEL_INTR_SEL2             (1 << 15)
#define PMU_DB_BYPASS2                      (1 << 14)
#define PMU_DB_TARGET2_SHIFT                6
#define PMU_DB_TARGET2_MASK                 (0xFF << PMU_DB_TARGET2_SHIFT)
#define PMU_DB_TARGET2(n)                   BITFIELD_VAL(PMU_DB_TARGET2, n)
#define REG_PMU_UART_DR2                    (1 << 5)
#define REG_PMU_UART_TX2                    (1 << 4)
#define REG_PMU_UART_OENB2                  (1 << 3)
#define REG_UART_LEDB_SEL                   (1 << 2)
#define REG_PMU_GPIO_INTR_MSK2              (1 << 1)
#define REG_PMU_GPIO_INTR_EN2               (1 << 0)

// REG_048
#define PMU_GPIO_INTR_MSKED1                (1 << 15)
#define PMU_GPIO_INTR_MSKED2                (1 << 14)
#define WDT_INTR_MSKED                      (1 << 11)


#define REG_PMU_GPIO_INTR_CLR1              (1 << 15)
#define REG_PMU_GPIO_INTR_CLR2              (1 << 14)
#define REG_WDT_INTR_CLR                    (1 << 11)

// REG_049
#define REG_BUCK_ANA_SYNC_DISABLE           (1 << 15)
#define REG_BUCK_ANA_SOFT_START_EN          (1 << 14)
#define REG_BUCK_ANA_SLOPE_DOUBLE           (1 << 13)
#define REG_BUCK_ANA_DT_BIT                 (1 << 12)
#define REG_BUCK_ANA_CAL_DELTAV_SHIFT       6
#define REG_BUCK_ANA_CAL_DELTAV_MASK        (0x3F << REG_BUCK_ANA_CAL_DELTAV_SHIFT)
#define REG_BUCK_ANA_CAL_DELTAV(n)          BITFIELD_VAL(REG_BUCK_ANA_CAL_DELTAV, n)
#define REG_BUCK_ANA_HYSTERESIS_BIT_SHIFT   3
#define REG_BUCK_ANA_HYSTERESIS_BIT_MASK    (0x7 << REG_BUCK_ANA_HYSTERESIS_BIT_SHIFT)
#define REG_BUCK_ANA_HYSTERESIS_BIT(n)      BITFIELD_VAL(REG_BUCK_ANA_HYSTERESIS_BIT, n)
#define REG_BUCK_ANA_COUNTER_SEL_SHIFT      1
#define REG_BUCK_ANA_COUNTER_SEL_MASK       (0x3 << REG_BUCK_ANA_COUNTER_SEL_SHIFT)
#define REG_BUCK_ANA_COUNTER_SEL(n)         BITFIELD_VAL(REG_BUCK_ANA_COUNTER_SEL, n)
#define REG_BUCK_ANA_RAMP_EN                (1 << 0)

// REG_04A
#define REG_BUCK_ANA_ZCD_CAP_BIT_SHIFT      12
#define REG_BUCK_ANA_ZCD_CAP_BIT_MASK       (0xF << REG_BUCK_ANA_ZCD_CAP_BIT_SHIFT)
#define REG_BUCK_ANA_ZCD_CAP_BIT(n)         BITFIELD_VAL(REG_BUCK_ANA_ZCD_CAP_BIT, n)
#define REG_BUCK_ANA_CAP_BIT_SHIFT          8
#define REG_BUCK_ANA_CAP_BIT_MASK           (0xF << REG_BUCK_ANA_CAP_BIT_SHIFT)
#define REG_BUCK_ANA_CAP_BIT(n)             BITFIELD_VAL(REG_BUCK_ANA_CAP_BIT, n)
#define REG_BUCK_ANA_PULLDOWN_EN            (1 << 6)
#define REG_BUCK_ANA_ANTI_RES_DISABLE       (1 << 5)
#define REG_BUCK_ANA_OFFSET_BIT_SHIFT       0
#define REG_BUCK_ANA_OFFSET_BIT_MASK        (0x1F << REG_BUCK_ANA_OFFSET_BIT_SHIFT)
#define REG_BUCK_ANA_OFFSET_BIT(n)          BITFIELD_VAL(REG_BUCK_ANA_OFFSET_BIT, n)

// REG_04B
#define REG_BUCK_RAMP_STEP_SEL_SHIFT        6
#define REG_BUCK_RAMP_STEP_SEL_MASK         (0x3 << REG_BUCK_RAMP_STEP_SEL_SHIFT)
#define REG_BUCK_RAMP_STEP_SEL(n)           BITFIELD_VAL(REG_BUCK_RAMP_STEP_SEL, n)
#define RESETN_A7_REG                       (1 << 5)
#define RESETN_A7_DR                        (1 << 4)
#define RESETN_USB_REG                      (1 << 3)
#define RESETN_USB_DR                       (1 << 2)
#define RESETN_MIPI_REG                     (1 << 1)
#define RESETN_MIPI_DR                      (1 << 0)

// REG_04C
#define REG_VCORE_ON_DELAY_DR               (1 << 15)
#define REG_VCORE_ON_DELAY                  (1 << 14)
#define LDO_DCDC2ANA_VBIT_NORMAL_SHIFT      9
#define LDO_DCDC2ANA_VBIT_NORMAL_MASK       (0x1F << LDO_DCDC2ANA_VBIT_NORMAL_SHIFT)
#define LDO_DCDC2ANA_VBIT_NORMAL(n)         BITFIELD_VAL(LDO_DCDC2ANA_VBIT_NORMAL, n)
#define REG_PU_LDO_VDCDC2VANA_DSLEEP        (1 << 8)
#define REG_PU_LDO_VDCDC2VANA_DR            (1 << 7)
#define REG_PU_LDO_VDCDC2VANA               (1 << 6)
#define LP_EN_VDCDC2VANA_LDO_DSLEEP         (1 << 5)
#define LP_EN_VDCDC2VANA_LDO_DR             (1 << 4)
#define LP_EN_VDCDC2VANA_LDO_REG            (1 << 3)
#define REG_PULLDOWN_VDCDC2VANA             (1 << 2)
#define REG_BYPASS_VDCDC2VANA               (1 << 1)
#define REG_LIGHT_LOAD_VDCDC2VANA           (1 << 0)

// REG_04D
#define REG_WDT_TIMER_INTR_SHIFT            0
#define REG_WDT_TIMER_INTR_MASK             (0xFFFF << REG_WDT_TIMER_INTR_SHIFT)
#define REG_WDT_TIMER_INTR(n)               BITFIELD_VAL(REG_WDT_TIMER_INTR, n)

// REG_04E
#define CLK_32K_COUNTER_26M_READY           (1 << 15)
#define CLK_32K_COUNTER_26M_SHIFT           0
#define CLK_32K_COUNTER_26M_MASK            (0x7FFF << CLK_32K_COUNTER_26M_SHIFT)
#define CLK_32K_COUNTER_26M(n)              BITFIELD_VAL(CLK_32K_COUNTER_26M, n)

// REG_04F
#define GPADC_START                         (1 << 5)
#define KEY_START                           (1 << 4)
#define AC_ON_EN                            (1 << 2)
#define HARDWARE_POWER_OFF_EN               (1 << 1)
#define SOFT_POWER_OFF                      (1 << 0)

// REG_050
#define RTC_INT_1                           (1 << 14)
#define RTC_INT_0                           (1 << 13)
#define KEY_ERR1_INTR                       (1 << 12)
#define KEY_ERR0_INTR                       (1 << 11)
#define KEY_PRESS_INTR                      (1 << 10)
#define KEY_RELEASE_INTR                    (1 << 9)
#define SAMPLE_PERIOD_DONE_INTR             (1 << 8)
#define CHAN_DATA_VALID_INTR_SHIFT          0
#define CHAN_DATA_VALID_INTR_MASK           (0xFF << CHAN_DATA_VALID_INTR_SHIFT)
#define CHAN_DATA_VALID_INTR(n)             BITFIELD_VAL(CHAN_DATA_VALID_INTR, n)

// REG_051
#define RTC_INT1_MSKED                      (1 << 14)
#define RTC_INT0_MSKED                      (1 << 13)
#define KEY_ERR1_INTR_MSKED                 (1 << 12)
#define KEY_ERR0_INTR_MSKED                 (1 << 11)
#define KEY_PRESS_INTR_MSKED                (1 << 10)
#define KEY_RELEASE_INTR_MSKED              (1 << 9)
#define SAMPLE_DONE_INTR_MSKED              (1 << 8)
#define CHAN_DATA_INTR_MSKED_SHIFT          0
#define CHAN_DATA_INTR_MSKED_MASK           (0xFF << CHAN_DATA_INTR_MSKED_SHIFT)
#define CHAN_DATA_INTR_MSKED(n)             BITFIELD_VAL(CHAN_DATA_INTR_MSKED, n)

#define RTC_INT_CLR_1                       (1 << 14)
#define RTC_INT_CLR_0                       (1 << 13)
#define KEY_ERR1_INTR_CLR                   (1 << 12)
#define KEY_ERR0_INTR_CLR                   (1 << 11)
#define KEY_PRESS_INTR_CLR                  (1 << 10)
#define KEY_RELEASE_INTR_CLR                (1 << 9)
#define SAMPLE_DONE_INTR_CLR                (1 << 8)
#define CHAN_DATA_INTR_CLR_SHIFT            0
#define CHAN_DATA_INTR_CLR_MASK             (0xFF << CHAN_DATA_INTR_CLR_SHIFT)
#define CHAN_DATA_INTR_CLR(n)               BITFIELD_VAL(CHAN_DATA_INTR_CLR, n)

// REG_052
#define PMU_LDO_ON_SOURCE_SHIFT             11
#define PMU_LDO_ON_SOURCE_MASK              (0x7 << PMU_LDO_ON_SOURCE_SHIFT)
#define PMU_LDO_ON_SOURCE(n)                BITFIELD_VAL(PMU_LDO_ON_SOURCE, n)
#define LED_IO1_IN_DB                       (1 << 10)
#define LED_IO2_IN_DB                       (1 << 9)
#define PMU_GPIO_INTR2                      (1 << 8)
#define PMU_GPIO_INTR1                      (1 << 7)
#define R_WDT_INTR                          (1 << 6)
#define VCORE_ON_DELAY                      (1 << 4)
#define RTC_LOAD                            (1 << 3)
#define WDT_LOAD                            (1 << 2)
#define CORE_GPIO_OUT1                      (1 << 1)
#define POWER_ON                            (1 << 0)

// REG_053
#define DATA_CHAN_MSB_SHIFT                 0
#define DATA_CHAN_MSB_MASK                  (0xFF << DATA_CHAN_MSB_SHIFT)
#define DATA_CHAN_MSB(n)                    BITFIELD_VAL(DATA_CHAN_MSB, n)

// REG_054
#define RTC_VALUE_15_0_SHIFT                0
#define RTC_VALUE_15_0_MASK                 (0xFFFF << RTC_VALUE_15_0_SHIFT)
#define RTC_VALUE_15_0(n)                   BITFIELD_VAL(RTC_VALUE_15_0, n)

// REG_055
#define RTC_VALUE_31_16_SHIFT               0
#define RTC_VALUE_31_16_MASK                (0xFFFF << RTC_VALUE_31_16_SHIFT)
#define RTC_VALUE_31_16(n)                  BITFIELD_VAL(RTC_VALUE_31_16, n)

// REG_056
#define DATA_CHAN0_SHIFT                    0
#define DATA_CHAN0_MASK                     (0xFFFF << DATA_CHAN0_SHIFT)
#define DATA_CHAN0(n)                       BITFIELD_VAL(DATA_CHAN0, n)

// REG_057
#define DATA_CHAN1_SHIFT                    0
#define DATA_CHAN1_MASK                     (0xFFFF << DATA_CHAN1_SHIFT)
#define DATA_CHAN1(n)                       BITFIELD_VAL(DATA_CHAN1, n)

// REG_058
#define DATA_CHAN2_SHIFT                    0
#define DATA_CHAN2_MASK                     (0xFFFF << DATA_CHAN2_SHIFT)
#define DATA_CHAN2(n)                       BITFIELD_VAL(DATA_CHAN2, n)

// REG_059
#define DATA_CHAN3_SHIFT                    0
#define DATA_CHAN3_MASK                     (0xFFFF << DATA_CHAN3_SHIFT)
#define DATA_CHAN3(n)                       BITFIELD_VAL(DATA_CHAN3, n)

// REG_05A
#define DATA_CHAN4_SHIFT                    0
#define DATA_CHAN4_MASK                     (0xFFFF << DATA_CHAN4_SHIFT)
#define DATA_CHAN4(n)                       BITFIELD_VAL(DATA_CHAN4, n)

// REG_05B
#define DATA_CHAN5_SHIFT                    0
#define DATA_CHAN5_MASK                     (0xFFFF << DATA_CHAN5_SHIFT)
#define DATA_CHAN5(n)                       BITFIELD_VAL(DATA_CHAN5, n)

// REG_05C
#define DATA_CHAN6_SHIFT                    0
#define DATA_CHAN6_MASK                     (0xFFFF << DATA_CHAN6_SHIFT)
#define DATA_CHAN6(n)                       BITFIELD_VAL(DATA_CHAN6, n)

// REG_05D
#define DATA_CHAN7_SHIFT                    0
#define DATA_CHAN7_MASK                     (0xFFFF << DATA_CHAN7_SHIFT)
#define DATA_CHAN7(n)                       BITFIELD_VAL(DATA_CHAN7, n)

// REG_05E
#define POWER_ON_RELEASE                    (1 << 15)
#define POWER_ON_PRESS                      (1 << 14)
#define DIG_PU_SAR                          (1 << 13)
#define DIG_PU_LPO                          (1 << 12)
#define DIG_LP_EN_VPA2                      (1 << 11)
#define DIG_PU_VPA2                         (1 << 10)
#define DIG_LP_EN_VPA1                      (1 << 9)
#define DIG_PU_VPA1                         (1 << 8)
#define DIG_PU_BIAS_LDO                     (1 << 7)
#define DIG_PU_LP_BIAS_LDO                  (1 << 6)
#define INTR_MSKED_CHARGE_SHIFT             3
#define INTR_MSKED_CHARGE_MASK              (0x3 << INTR_MSKED_CHARGE_SHIFT)
#define INTR_MSKED_CHARGE(n)                BITFIELD_VAL(INTR_MSKED_CHARGE, n)
#define AC_ON                               (1 << 2)
#define AC_ON_DET_OUT                       (1 << 1)
#define AC_ON_DET_IN                        (1 << 0)

// REG_05F
#define DEEPSLEEP_MODE                      (1 << 15)
#define BDOPTION_SPI_VSEL_RTC               (1 << 14)
#define UVLO_LV                             (1 << 13)
#define PMU_LDO_ON_STATUS                   (1 << 12)
#define DIG_LP_EN_VCODEC                    (1 << 11)
#define DIG_PU_VCODEC                       (1 << 10)
#define DIG_PU_VRTC_RF                      (1 << 9)
#define DIG_LP_EN_VRF                       (1 << 8)
#define DIG_PU_VRF                          (1 << 7)
#define DIG_LP_EN_VIO                       (1 << 6)
#define DIG_PU_VIO                          (1 << 5)
#define DIG_LP_EN_VDCDC                     (1 << 4)
#define DIG_PU_VDCDC                        (1 << 3)
#define DIG_LP_EN_VCORE_LDO                 (1 << 1)
#define DIG_PU_VCORE_LDO                    (1 << 0)

// REG_060
#define REG_WD_RESETN_INFOR_SHIFT           0
#define REG_WD_RESETN_INFOR_MASK            (0xFFFF << REG_WD_RESETN_INFOR_SHIFT)
#define REG_WD_RESETN_INFOR(n)              BITFIELD_VAL(REG_WD_RESETN_INFOR, n)

// REG_061
#define REG_NOT_RESET_SHIFT                 0
#define REG_NOT_RESET_MASK                  (0x7FFF << REG_NOT_RESET_SHIFT)
#define REG_NOT_RESET(n)                    BITFIELD_VAL(REG_NOT_RESET, n)
#define REG_COMMAND_RST_DIS                 (1 << 15)


// REG_062
#define RESETN_A7PLL_REG                    (1 << 11)
#define RESETN_A7PLL_DR                     (1 << 10)
#define RESETN_PSRAM1_REG                   (1 << 9)
#define RESETN_PSRAM1_DR                    (1 << 8)
#define RESETN_PSRAM0_REG                   (1 << 7)
#define RESETN_PSRAM0_DR                    (1 << 6)
#define RESETN_PSRAMPLL_REG                 (1 << 5)
#define RESETN_PSRAMPLL_DR                  (1 << 4)
#define RESETN_BT_REG                       (1 << 3)
#define RESETN_BT_DR                        (1 << 2)
#define RESETN_WIFI_REG                     (1 << 1)
#define RESETN_WIFI_DR                      (1 << 0)

// REG_063
#define REG_BUCK_ANA_INTERNAL_FREQUENCY_SHIFT 13
#define REG_BUCK_ANA_INTERNAL_FREQUENCY_MASK (0x7 << REG_BUCK_ANA_INTERNAL_FREQUENCY_SHIFT)
#define REG_BUCK_ANA_INTERNAL_FREQUENCY(n)  BITFIELD_VAL(REG_BUCK_ANA_INTERNAL_FREQUENCY, n)
#define REG_BUCK_ANA_VREF_SEL_SHIFT         11
#define REG_BUCK_ANA_VREF_SEL_MASK          (0x3 << REG_BUCK_ANA_VREF_SEL_SHIFT)
#define REG_BUCK_ANA_VREF_SEL(n)            BITFIELD_VAL(REG_BUCK_ANA_VREF_SEL, n)
#define REG_BUCK_ANA_TEST_POWERMOS          (1 << 10)
#define REG_BUCK_ANA_EN_ZCD_CAL             (1 << 9)
#define REG_BUCK_ANA_TEST_PMOS              (1 << 8)
#define REG_BUCK_ANA_HALF_BIAS              (1 << 7)
#define REG_BUCK_ANA_SLOPE_EN_BURST         (1 << 6)
#define REG_BUCK_ANA_OFFSET_CURRENT_EN      (1 << 5)
#define REG_BUCK_ANA_I_DELTAV_X2            (1 << 4)
#define REG_BUCK_ANA_LOGIC_IR_CLK_EN        (1 << 3)
#define REG_BUCK_ANA_ENB_PU_DELAY           (1 << 2)
#define REG_BUCK_ANA_IX2_ERR                (1 << 1)
#define REG_BUCK_ANA_LP_VCOMP               (1 << 0)

// REG_064
#define REG_BUCK_ANA_VBAT_3V_EXTRA          (1 << 10)
#define REG_BUCK_ANA_TEST_MODE_EN           (1 << 9)
#define REG_BUCK_ANA_BURST_THRESHOLD_SHIFT  4
#define REG_BUCK_ANA_BURST_THRESHOLD_MASK   (0x1F << REG_BUCK_ANA_BURST_THRESHOLD_SHIFT)
#define REG_BUCK_ANA_BURST_THRESHOLD(n)     BITFIELD_VAL(REG_BUCK_ANA_BURST_THRESHOLD, n)
#define REG_BUCK_ANA_EDGE_CON_SHIFT         0
#define REG_BUCK_ANA_EDGE_CON_MASK          (0xF << REG_BUCK_ANA_EDGE_CON_SHIFT)
#define REG_BUCK_ANA_EDGE_CON(n)            BITFIELD_VAL(REG_BUCK_ANA_EDGE_CON, n)

// REG_065
#define REG_BUCK_VCORE_IS_GAIN_DSLEEP_SHIFT 10
#define REG_BUCK_VCORE_IS_GAIN_DSLEEP_MASK  (0x7 << REG_BUCK_VCORE_IS_GAIN_DSLEEP_SHIFT)
#define REG_BUCK_VCORE_IS_GAIN_DSLEEP(n)    BITFIELD_VAL(REG_BUCK_VCORE_IS_GAIN_DSLEEP, n)
#define REG_BUCK_VCORE_IS_GAIN_NORMAL_SHIFT 7
#define REG_BUCK_VCORE_IS_GAIN_NORMAL_MASK  (0x7 << REG_BUCK_VCORE_IS_GAIN_NORMAL_SHIFT)
#define REG_BUCK_VCORE_IS_GAIN_NORMAL(n)    BITFIELD_VAL(REG_BUCK_VCORE_IS_GAIN_NORMAL, n)
#define REG_BUCK_VCORE_ULP_MODE_DSLEEP      (1 << 6)
#define REG_BUCK_VCORE_ULP_MODE_NORMAL      (1 << 5)
#define REG_BUCK_VCORE_BURST_MODE_NORMAL    (1 << 4)
#define REG_BUCK_VCORE_BURST_MODE_DSLEEP    (1 << 3)
#define REG_PU_DCDC_VCORE_DSLEEP            (1 << 2)
#define REG_PU_DCDC_VCORE_REG               (1 << 1)
#define REG_PU_DCDC_VCORE_DR                (1 << 0)

// REG_066
#define REG_BUCK_VCORE_VBIT_DSLEEP_SHIFT    8
#define REG_BUCK_VCORE_VBIT_DSLEEP_MASK     (0xFF << REG_BUCK_VCORE_VBIT_DSLEEP_SHIFT)
#define REG_BUCK_VCORE_VBIT_DSLEEP(n)       BITFIELD_VAL(REG_BUCK_VCORE_VBIT_DSLEEP, n)
#define REG_BUCK_VCORE_VBIT_NORMAL_SHIFT    0
#define REG_BUCK_VCORE_VBIT_NORMAL_MASK     (0xFF << REG_BUCK_VCORE_VBIT_NORMAL_SHIFT)
#define REG_BUCK_VCORE_VBIT_NORMAL(n)       BITFIELD_VAL(REG_BUCK_VCORE_VBIT_NORMAL, n)
#define MAX_DCDC_VCORE_VBIT_VAL             (REG_BUCK_VCORE_VBIT_NORMAL_MASK >> REG_BUCK_VCORE_VBIT_NORMAL_SHIFT)

// REG_067
#define REG_BYPASS_VRF                      (1 << 10)
#define REG_PULLDOWN_VRF                    (1 << 9)
#define REG_MIC_BIASB_LPFSEL_SHIFT          6
#define REG_MIC_BIASB_LPFSEL_MASK           (0x7 << REG_MIC_BIASB_LPFSEL_SHIFT)
#define REG_MIC_BIASB_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASB_LPFSEL, n)
#define REG_MIC_BIASA_LPFSEL_SHIFT          3
#define REG_MIC_BIASA_LPFSEL_MASK           (0x7 << REG_MIC_BIASA_LPFSEL_SHIFT)
#define REG_MIC_BIASA_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASA_LPFSEL, n)
#define REG_MIC_BIASB_PULLDOWN              (1 << 2)
#define REG_MIC_BIASA_PULLDOWN              (1 << 1)
#define REG_MIC_LP_ENABLE                   (1 << 0)

// REG_068
#define LP_EN_VRF_LDO_DSLEEP                (1 << 15)
#define LP_EN_VRF_LDO_DR                    (1 << 14)
#define LP_EN_VRF_LDO_REG                   (1 << 13)
#define REG_PU_LDO_VRF_DSLEEP               (1 << 12)
#define REG_PU_LDO_VRF_DR                   (1 << 11)
#define REG_PU_LDO_VRF                      (1 << 10)
#define LDO_VRF_VBIT_NORMAL_SHIFT           4
#define LDO_VRF_VBIT_NORMAL_MASK            (0xF << LDO_VRF_VBIT_NORMAL_SHIFT)
#define LDO_VRF_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_VRF_VBIT_NORMAL, n)
#define LDO_VRF_VBIT_DSLEEP_SHIFT           0
#define LDO_VRF_VBIT_DSLEEP_MASK            (0xF << LDO_VRF_VBIT_DSLEEP_SHIFT)
#define LDO_VRF_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_VRF_VBIT_DSLEEP, n)

// REG_069
#define LP_EN_VPA1_LDO_DSLEEP               (1 << 15)
#define LP_EN_VPA1_LDO_DR                   (1 << 14)
#define LP_EN_VPA1_LDO_REG                  (1 << 13)
#define REG_PU_LDO_VPA1_DSLEEP              (1 << 12)
#define REG_PU_LDO_VPA1_DR                  (1 << 11)
#define REG_PU_LDO_VPA1_REG                 (1 << 10)
#define LDO_VPA1_VBIT_NORMAL_SHIFT          6
#define LDO_VPA1_VBIT_NORMAL_MASK           (0xF << LDO_VPA1_VBIT_NORMAL_SHIFT)
#define LDO_VPA1_VBIT_NORMAL(n)             BITFIELD_VAL(LDO_VPA1_VBIT_NORMAL, n)
#define LDO_VPA1_VBIT_DSLEEP_SHIFT          2
#define LDO_VPA1_VBIT_DSLEEP_MASK           (0xF << LDO_VPA1_VBIT_DSLEEP_SHIFT)
#define LDO_VPA1_VBIT_DSLEEP(n)             BITFIELD_VAL(LDO_VPA1_VBIT_DSLEEP, n)
#define REG_OCP_VPA1                        (1 << 1)
#define REG_PULLDOWN_VPA1                   (1 << 0)

// REG_06A
#define LP_EN_VPA2_LDO_DSLEEP               (1 << 15)
#define LP_EN_VPA2_LDO_DR                   (1 << 14)
#define LP_EN_VPA2_LDO_REG                  (1 << 13)
#define REG_PU_LDO_VPA2_DSLEEP              (1 << 12)
#define REG_PU_LDO_VPA2_DR                  (1 << 11)
#define REG_PU_LDO_VPA2_REG                 (1 << 10)
#define LDO_VPA2_VBIT_NORMAL_SHIFT          6
#define LDO_VPA2_VBIT_NORMAL_MASK           (0xF << LDO_VPA2_VBIT_NORMAL_SHIFT)
#define LDO_VPA2_VBIT_NORMAL(n)             BITFIELD_VAL(LDO_VPA2_VBIT_NORMAL, n)
#define LDO_VPA2_VBIT_DSLEEP_SHIFT          2
#define LDO_VPA2_VBIT_DSLEEP_MASK           (0xF << LDO_VPA2_VBIT_DSLEEP_SHIFT)
#define LDO_VPA2_VBIT_DSLEEP(n)             BITFIELD_VAL(LDO_VPA2_VBIT_DSLEEP, n)
#define REG_BYPASS_VPA2                     (1 << 1)
#define REG_PULLDOWN_VPA2                   (1 << 0)

// REG_06B
#define POWER_UP_MOD7_CNT_SHIFT             0
#define POWER_UP_MOD7_CNT_MASK              (0xFF << POWER_UP_MOD7_CNT_SHIFT)
#define POWER_UP_MOD7_CNT(n)                BITFIELD_VAL(POWER_UP_MOD7_CNT, n)
#define POWER_UP_MOD8_CNT_SHIFT             8
#define POWER_UP_MOD8_CNT_MASK              (0xFF << POWER_UP_MOD8_CNT_SHIFT)
#define POWER_UP_MOD8_CNT(n)                BITFIELD_VAL(POWER_UP_MOD8_CNT, n)

// REG_06C
#define POWER_UP_MOD9_CNT_SHIFT             0
#define POWER_UP_MOD9_CNT_MASK              (0xFF << POWER_UP_MOD9_CNT_SHIFT)
#define POWER_UP_MOD9_CNT(n)                BITFIELD_VAL(POWER_UP_MOD9_CNT, n)

// REG_06D
#define REG_SAR_RESULT_SEL                  (1 << 15)
#define REG_SAR_ADC_ON                      (1 << 14)
#define SAR_BIT00_WEIGHT_SHIFT              0
#define SAR_BIT00_WEIGHT_MASK               (0x3FFF << SAR_BIT00_WEIGHT_SHIFT)
#define SAR_BIT00_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT00_WEIGHT, n)

// REG_06E
#define SAR_BIT01_WEIGHT_SHIFT              0
#define SAR_BIT01_WEIGHT_MASK               (0x3FFF << SAR_BIT01_WEIGHT_SHIFT)
#define SAR_BIT01_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT01_WEIGHT, n)

// REG_06F
#define SAR_BIT02_WEIGHT_SHIFT              0
#define SAR_BIT02_WEIGHT_MASK               (0x3FFF << SAR_BIT02_WEIGHT_SHIFT)
#define SAR_BIT02_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT02_WEIGHT, n)

// REG_070
#define SAR_BIT03_WEIGHT_SHIFT              0
#define SAR_BIT03_WEIGHT_MASK               (0x3FFF << SAR_BIT03_WEIGHT_SHIFT)
#define SAR_BIT03_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT03_WEIGHT, n)

// REG_071
#define SAR_BIT04_WEIGHT_SHIFT              0
#define SAR_BIT04_WEIGHT_MASK               (0x3FFF << SAR_BIT04_WEIGHT_SHIFT)
#define SAR_BIT04_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT04_WEIGHT, n)

// REG_072
#define SAR_P_BIT05_WEIGHT_SHIFT            0
#define SAR_P_BIT05_WEIGHT_MASK             (0x3FFF << SAR_P_BIT05_WEIGHT_SHIFT)
#define SAR_P_BIT05_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT05_WEIGHT, n)

// REG_073
#define SAR_P_BIT06_WEIGHT_SHIFT            0
#define SAR_P_BIT06_WEIGHT_MASK             (0x3FFF << SAR_P_BIT06_WEIGHT_SHIFT)
#define SAR_P_BIT06_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT06_WEIGHT, n)

// REG_074
#define SAR_P_BIT07_WEIGHT_SHIFT            0
#define SAR_P_BIT07_WEIGHT_MASK             (0x3FFF << SAR_P_BIT07_WEIGHT_SHIFT)
#define SAR_P_BIT07_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT07_WEIGHT, n)

// REG_075
#define SAR_P_BIT08_WEIGHT_SHIFT            0
#define SAR_P_BIT08_WEIGHT_MASK             (0x3FFF << SAR_P_BIT08_WEIGHT_SHIFT)
#define SAR_P_BIT08_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT08_WEIGHT, n)

// REG_076
#define SAR_P_BIT09_WEIGHT_SHIFT            0
#define SAR_P_BIT09_WEIGHT_MASK             (0x3FFF << SAR_P_BIT09_WEIGHT_SHIFT)
#define SAR_P_BIT09_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT09_WEIGHT, n)

// REG_077
#define SAR_P_BIT10_WEIGHT_SHIFT            0
#define SAR_P_BIT10_WEIGHT_MASK             (0x3FFF << SAR_P_BIT10_WEIGHT_SHIFT)
#define SAR_P_BIT10_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT10_WEIGHT, n)

// REG_078
#define SAR_P_BIT11_WEIGHT_SHIFT            0
#define SAR_P_BIT11_WEIGHT_MASK             (0x3FFF << SAR_P_BIT11_WEIGHT_SHIFT)
#define SAR_P_BIT11_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT11_WEIGHT, n)

// REG_079
#define SAR_P_BIT12_WEIGHT_SHIFT            0
#define SAR_P_BIT12_WEIGHT_MASK             (0x3FFF << SAR_P_BIT12_WEIGHT_SHIFT)
#define SAR_P_BIT12_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT12_WEIGHT, n)

// REG_07A
#define SAR_P_BIT13_WEIGHT_SHIFT            0
#define SAR_P_BIT13_WEIGHT_MASK             (0x3FFF << SAR_P_BIT13_WEIGHT_SHIFT)
#define SAR_P_BIT13_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT13_WEIGHT, n)

// REG_07B
#define SAR_P_BIT14_WEIGHT_SHIFT            0
#define SAR_P_BIT14_WEIGHT_MASK             (0x3FFF << SAR_P_BIT14_WEIGHT_SHIFT)
#define SAR_P_BIT14_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT14_WEIGHT, n)

// REG_07C
#define SAR_P_BIT15_WEIGHT_SHIFT            0
#define SAR_P_BIT15_WEIGHT_MASK             (0x3FFF << SAR_P_BIT15_WEIGHT_SHIFT)
#define SAR_P_BIT15_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT15_WEIGHT, n)

// REG_07D
#define SAR_P_BIT16_WEIGHT_SHIFT            0
#define SAR_P_BIT16_WEIGHT_MASK             (0x3FFF << SAR_P_BIT16_WEIGHT_SHIFT)
#define SAR_P_BIT16_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT16_WEIGHT, n)

// REG_07E
#define SAR_P_BIT17_WEIGHT_SHIFT            0
#define SAR_P_BIT17_WEIGHT_MASK             (0x3FFF << SAR_P_BIT17_WEIGHT_SHIFT)
#define SAR_P_BIT17_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT17_WEIGHT, n)

// REG_07F
#define SAR_N_BIT05_WEIGHT_SHIFT            0
#define SAR_N_BIT05_WEIGHT_MASK             (0x3FFF << SAR_N_BIT05_WEIGHT_SHIFT)
#define SAR_N_BIT05_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT05_WEIGHT, n)

// REG_080
#define SAR_N_BIT06_WEIGHT_SHIFT            0
#define SAR_N_BIT06_WEIGHT_MASK             (0x3FFF << SAR_N_BIT06_WEIGHT_SHIFT)
#define SAR_N_BIT06_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT06_WEIGHT, n)

// REG_081
#define SAR_N_BIT07_WEIGHT_SHIFT            0
#define SAR_N_BIT07_WEIGHT_MASK             (0x3FFF << SAR_N_BIT07_WEIGHT_SHIFT)
#define SAR_N_BIT07_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT07_WEIGHT, n)

// REG_082
#define SAR_N_BIT08_WEIGHT_SHIFT            0
#define SAR_N_BIT08_WEIGHT_MASK             (0x3FFF << SAR_N_BIT08_WEIGHT_SHIFT)
#define SAR_N_BIT08_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT08_WEIGHT, n)

// REG_083
#define SAR_N_BIT09_WEIGHT_SHIFT            0
#define SAR_N_BIT09_WEIGHT_MASK             (0x3FFF << SAR_N_BIT09_WEIGHT_SHIFT)
#define SAR_N_BIT09_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT09_WEIGHT, n)

// REG_084
#define SAR_N_BIT10_WEIGHT_SHIFT            0
#define SAR_N_BIT10_WEIGHT_MASK             (0x3FFF << SAR_N_BIT10_WEIGHT_SHIFT)
#define SAR_N_BIT10_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT10_WEIGHT, n)

// REG_085
#define SAR_N_BIT11_WEIGHT_SHIFT            0
#define SAR_N_BIT11_WEIGHT_MASK             (0x3FFF << SAR_N_BIT11_WEIGHT_SHIFT)
#define SAR_N_BIT11_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT11_WEIGHT, n)

// REG_086
#define SAR_N_BIT12_WEIGHT_SHIFT            0
#define SAR_N_BIT12_WEIGHT_MASK             (0x3FFF << SAR_N_BIT12_WEIGHT_SHIFT)
#define SAR_N_BIT12_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT12_WEIGHT, n)

// REG_087
#define SAR_N_BIT13_WEIGHT_SHIFT            0
#define SAR_N_BIT13_WEIGHT_MASK             (0x3FFF << SAR_N_BIT13_WEIGHT_SHIFT)
#define SAR_N_BIT13_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT13_WEIGHT, n)

// REG_088
#define SAR_N_BIT14_WEIGHT_SHIFT            0
#define SAR_N_BIT14_WEIGHT_MASK             (0x3FFF << SAR_N_BIT14_WEIGHT_SHIFT)
#define SAR_N_BIT14_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT14_WEIGHT, n)

// REG_089
#define SAR_N_BIT15_WEIGHT_SHIFT            0
#define SAR_N_BIT15_WEIGHT_MASK             (0x3FFF << SAR_N_BIT15_WEIGHT_SHIFT)
#define SAR_N_BIT15_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT15_WEIGHT, n)

// REG_08A
#define SAR_N_BIT16_WEIGHT_SHIFT            0
#define SAR_N_BIT16_WEIGHT_MASK             (0x3FFF << SAR_N_BIT16_WEIGHT_SHIFT)
#define SAR_N_BIT16_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT16_WEIGHT, n)

// REG_08B
#define SAR_N_BIT17_WEIGHT_SHIFT            0
#define SAR_N_BIT17_WEIGHT_MASK             (0x3FFF << SAR_N_BIT17_WEIGHT_SHIFT)
#define SAR_N_BIT17_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT17_WEIGHT, n)

// REG_08C
#define CLK_SARADC_CODEC_INV                (1 << 14)
#define REG_SAR_ADC_OFFSET_DR               (1 << 13)
#define REG_SAR_OFFSET_DR                   (1 << 12)
#define REG_SAR_WEIGHT_DR                   (1 << 11)
#define REG_SAR_CALI_LSB_SCRN               (1 << 10)
#define REG_SAR_CALI_CNT_SHIFT              6
#define REG_SAR_CALI_CNT_MASK               (0xF << REG_SAR_CALI_CNT_SHIFT)
#define REG_SAR_CALI_CNT(n)                 BITFIELD_VAL(REG_SAR_CALI_CNT, n)
#define REG_SAR_CALI                        (1 << 5)
#define REG_SAR_INIT_CALI_BIT_SHIFT         0
#define REG_SAR_INIT_CALI_BIT_MASK          (0x1F << REG_SAR_INIT_CALI_BIT_SHIFT)
#define REG_SAR_INIT_CALI_BIT(n)            BITFIELD_VAL(REG_SAR_INIT_CALI_BIT, n)

// REG_08D
#define REG_SAR_SAMPLE_IBIT_SHIFT           10
#define REG_SAR_SAMPLE_IBIT_MASK            (0x7 << REG_SAR_SAMPLE_IBIT_SHIFT)
#define REG_SAR_SAMPLE_IBIT(n)              BITFIELD_VAL(REG_SAR_SAMPLE_IBIT, n)
#define REG_SAR_OFFSET_N_SHIFT              5
#define REG_SAR_OFFSET_N_MASK               (0x1F << REG_SAR_OFFSET_N_SHIFT)
#define REG_SAR_OFFSET_N(n)                 BITFIELD_VAL(REG_SAR_OFFSET_N, n)
#define REG_SAR_OFFSET_P_SHIFT              0
#define REG_SAR_OFFSET_P_MASK               (0x1F << REG_SAR_OFFSET_P_SHIFT)
#define REG_SAR_OFFSET_P(n)                 BITFIELD_VAL(REG_SAR_OFFSET_P, n)

// REG_08E
#define REG_SAR_ADC_OFFSET_SHIFT            0
#define REG_SAR_ADC_OFFSET_MASK             (0xFFFF << REG_SAR_ADC_OFFSET_SHIFT)
#define REG_SAR_ADC_OFFSET(n)               BITFIELD_VAL(REG_SAR_ADC_OFFSET, n)

// REG_08F
#define REG_SAR_RESET                       (1 << 14)
#define REG_SAR_VCM_CORE_SEL_SHIFT          11
#define REG_SAR_VCM_CORE_SEL_MASK           (0x7 << REG_SAR_VCM_CORE_SEL_SHIFT)
#define REG_SAR_VCM_CORE_SEL(n)             BITFIELD_VAL(REG_SAR_VCM_CORE_SEL, n)
#define REG_SAR_DIFF_EN_IN_REG              (1 << 10)
#define REG_SAR_DIFF_EN_IN_DR               (1 << 9)
#define REG_SAR_SE2DIFF_EN                  (1 << 8)
#define REG_SAR_BUF_EN                      (1 << 7)
#define SAR_CONV_DONE_INV                   (1 << 6)
#define SAR_VOUT_CALIB_INV                  (1 << 5)
#define REG_SAR_CLK_OUT_SEL                 (1 << 4)
#define REG_SAR_OFFSET_CALI_CNT_SHIFT       1
#define REG_SAR_OFFSET_CALI_CNT_MASK        (0x7 << REG_SAR_OFFSET_CALI_CNT_SHIFT)
#define REG_SAR_OFFSET_CALI_CNT(n)          BITFIELD_VAL(REG_SAR_OFFSET_CALI_CNT, n)
#define REG_SAR_OFFSET_CALI                 (1 << 0)

// REG_090
#define REG_SAR_DELAY_CMP                   (1 << 15)
#define REG_SAR_INPUT_BUF_EN                (1 << 14)
#define REG_SAR_EN_PREAMP                   (1 << 13)
#define REG_SAR_DELAY_BIT_SHIFT             11
#define REG_SAR_DELAY_BIT_MASK              (0x3 << REG_SAR_DELAY_BIT_SHIFT)
#define REG_SAR_DELAY_BIT(n)                BITFIELD_VAL(REG_SAR_DELAY_BIT, n)
#define REG_SAR_CLK_MODE                    (1 << 10)
#define REG_SAR_CLK_TRIM_SHIFT              8
#define REG_SAR_CLK_TRIM_MASK               (0x3 << REG_SAR_CLK_TRIM_SHIFT)
#define REG_SAR_CLK_TRIM(n)                 BITFIELD_VAL(REG_SAR_CLK_TRIM, n)

// REG_091
#define REG_SAR_HIGH_PW_CMP                 (1 << 11)
#define REG_SAR_HIGH_DVDD_EN                (1 << 10)
#define REG_SAR_VREG_OUTCAP_MODE            (1 << 9)
#define REG_SAR_VREG_IBIT_SHIFT             7
#define REG_SAR_VREG_IBIT_MASK              (0x3 << REG_SAR_VREG_IBIT_SHIFT)
#define REG_SAR_VREG_IBIT(n)                BITFIELD_VAL(REG_SAR_VREG_IBIT, n)
#define REG_SAR_SE_MODE                     (1 << 4)
#define REG_SAR_PU_NIN_BIAS                 (1 << 3)
#define REG_SAR_PU_NIN_BIAS_DR              (1 << 2)
#define REG_SAR_PU_PIN_BIAS                 (1 << 1)
#define REG_SAR_PU_PIN_BIAS_DR              (1 << 0)

// REG_092
#define REG_SAR_VREG_SEL_SHIFT              9
#define REG_SAR_VREG_SEL_MASK               (0x7F << REG_SAR_VREG_SEL_SHIFT)
#define REG_SAR_VREG_SEL(n)                 BITFIELD_VAL(REG_SAR_VREG_SEL, n)
#define REG_SAR_ADC_RESET                   (1 << 8)
#define REG_SAR_CLK_OUT_DIV_SHIFT           0
#define REG_SAR_CLK_OUT_DIV_MASK            (0xFF << REG_SAR_CLK_OUT_DIV_SHIFT)
#define REG_SAR_CLK_OUT_DIV(n)              BITFIELD_VAL(REG_SAR_CLK_OUT_DIV, n)

// REG_093
#define REG_SAR_PU_START_DLY_CNT_SHIFT      9
#define REG_SAR_PU_START_DLY_CNT_MASK       (0x7F << REG_SAR_PU_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_START_DLY_CNT(n)         BITFIELD_VAL(REG_SAR_PU_START_DLY_CNT, n)
#define REG_SAR_PU_RST_REG                  (1 << 8)
#define REG_SAR_PU_RST_DR                   (1 << 7)
#define REG_SAR_PU_PRECHARGE_REG            (1 << 6)
#define REG_SAR_PU_PRECHARGE_DR             (1 << 5)
#define REG_SAR_PU_REG                      (1 << 4)
#define REG_SAR_PU_DR                       (1 << 3)
#define REG_SAR_PU_VREF_IN                  (1 << 2)
#define REG_SAR_PU_VREF_REG                 (1 << 1)
#define REG_SAR_PU_VREF_DR                  (1 << 0)

// REG_094
#define REG_SAR_PU_RST_START_DLY_CNT_SHIFT  7
#define REG_SAR_PU_RST_START_DLY_CNT_MASK   (0x7F << REG_SAR_PU_RST_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_RST_START_DLY_CNT(n)     BITFIELD_VAL(REG_SAR_PU_RST_START_DLY_CNT, n)
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT_SHIFT 0
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT_MASK (0x7F << REG_SAR_PU_PRECHARGE_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT(n) BITFIELD_VAL(REG_SAR_PU_PRECHARGE_START_DLY_CNT, n)

// REG_095
#define REG_SAR_PU_PRECHARGE_LAST_CNT_SHIFT 7
#define REG_SAR_PU_PRECHARGE_LAST_CNT_MASK  (0x1FF << REG_SAR_PU_PRECHARGE_LAST_CNT_SHIFT)
#define REG_SAR_PU_PRECHARGE_LAST_CNT(n)    BITFIELD_VAL(REG_SAR_PU_PRECHARGE_LAST_CNT, n)
#define REG_SAR_PU_RST_LAST_CNT_SHIFT       0
#define REG_SAR_PU_RST_LAST_CNT_MASK        (0x7F << REG_SAR_PU_RST_LAST_CNT_SHIFT)
#define REG_SAR_PU_RST_LAST_CNT(n)          BITFIELD_VAL(REG_SAR_PU_RST_LAST_CNT, n)


// REG_096
#define SAR_P_BIT05_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT05_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT05_WEIGHT_IN_SHIFT)
#define SAR_P_BIT05_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT05_WEIGHT_IN, n)

// REG_097
#define SAR_P_BIT06_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT06_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT06_WEIGHT_IN_SHIFT)
#define SAR_P_BIT06_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT06_WEIGHT_IN, n)

// REG_098
#define SAR_P_BIT07_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT07_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT07_WEIGHT_IN_SHIFT)
#define SAR_P_BIT07_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT07_WEIGHT_IN, n)

// REG_099
#define SAR_P_BIT08_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT08_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT08_WEIGHT_IN_SHIFT)
#define SAR_P_BIT08_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT08_WEIGHT_IN, n)

// REG_09A
#define SAR_P_BIT09_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT09_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT09_WEIGHT_IN_SHIFT)
#define SAR_P_BIT09_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT09_WEIGHT_IN, n)

// REG_09B
#define SAR_P_BIT10_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT10_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT10_WEIGHT_IN_SHIFT)
#define SAR_P_BIT10_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT10_WEIGHT_IN, n)

// REG_09C
#define SAR_P_BIT11_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT11_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT11_WEIGHT_IN_SHIFT)
#define SAR_P_BIT11_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT11_WEIGHT_IN, n)

// REG_09D
#define SAR_P_BIT12_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT12_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT12_WEIGHT_IN_SHIFT)
#define SAR_P_BIT12_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT12_WEIGHT_IN, n)

// REG_09E
#define SAR_P_BIT13_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT13_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT13_WEIGHT_IN_SHIFT)
#define SAR_P_BIT13_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT13_WEIGHT_IN, n)

// REG_09F
#define SAR_P_BIT14_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT14_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT14_WEIGHT_IN_SHIFT)
#define SAR_P_BIT14_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT14_WEIGHT_IN, n)

// REG_0A0
#define SAR_P_BIT15_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT15_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT15_WEIGHT_IN_SHIFT)
#define SAR_P_BIT15_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT15_WEIGHT_IN, n)

// REG_0A1
#define SAR_P_BIT16_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT16_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT16_WEIGHT_IN_SHIFT)
#define SAR_P_BIT16_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT16_WEIGHT_IN, n)

// REG_0A2
#define SAR_P_BIT17_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT17_WEIGHT_IN_MASK          (0x3FFF << SAR_P_BIT17_WEIGHT_IN_SHIFT)
#define SAR_P_BIT17_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT17_WEIGHT_IN, n)

// REG_0A3
#define SAR_N_BIT05_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT05_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT05_WEIGHT_IN_SHIFT)
#define SAR_N_BIT05_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT05_WEIGHT_IN, n)

// REG_0A4
#define SAR_N_BIT06_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT06_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT06_WEIGHT_IN_SHIFT)
#define SAR_N_BIT06_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT06_WEIGHT_IN, n)

// REG_0A5
#define SAR_N_BIT07_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT07_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT07_WEIGHT_IN_SHIFT)
#define SAR_N_BIT07_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT07_WEIGHT_IN, n)

// REG_0A6
#define SAR_N_BIT08_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT08_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT08_WEIGHT_IN_SHIFT)
#define SAR_N_BIT08_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT08_WEIGHT_IN, n)

// REG_0A7
#define SAR_N_BIT09_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT09_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT09_WEIGHT_IN_SHIFT)
#define SAR_N_BIT09_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT09_WEIGHT_IN, n)

// REG_0A8
#define SAR_N_BIT10_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT10_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT10_WEIGHT_IN_SHIFT)
#define SAR_N_BIT10_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT10_WEIGHT_IN, n)

// REG_0A9
#define SAR_N_BIT11_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT11_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT11_WEIGHT_IN_SHIFT)
#define SAR_N_BIT11_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT11_WEIGHT_IN, n)

// REG_0AA
#define SAR_N_BIT12_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT12_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT12_WEIGHT_IN_SHIFT)
#define SAR_N_BIT12_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT12_WEIGHT_IN, n)

// REG_0AB
#define SAR_N_BIT13_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT13_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT13_WEIGHT_IN_SHIFT)
#define SAR_N_BIT13_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT13_WEIGHT_IN, n)

// REG_0AC
#define SAR_N_BIT14_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT14_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT14_WEIGHT_IN_SHIFT)
#define SAR_N_BIT14_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT14_WEIGHT_IN, n)

// REG_0AD
#define SAR_N_BIT15_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT15_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT15_WEIGHT_IN_SHIFT)
#define SAR_N_BIT15_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT15_WEIGHT_IN, n)

// REG_0AE
#define SAR_N_BIT16_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT16_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT16_WEIGHT_IN_SHIFT)
#define SAR_N_BIT16_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT16_WEIGHT_IN, n)

// REG_0AF
#define SAR_N_BIT17_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT17_WEIGHT_IN_MASK          (0x3FFF << SAR_N_BIT17_WEIGHT_IN_SHIFT)
#define SAR_N_BIT17_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT17_WEIGHT_IN, n)

// REG_0B0
#define REG_SAR_OFFSET_N_IN_SHIFT           5
#define REG_SAR_OFFSET_N_IN_MASK            (0x1F << REG_SAR_OFFSET_N_IN_SHIFT)
#define REG_SAR_OFFSET_N_IN(n)              BITFIELD_VAL(REG_SAR_OFFSET_N_IN, n)
#define REG_SAR_OFFSET_P_IN_SHIFT           0
#define REG_SAR_OFFSET_P_IN_MASK            (0x1F << REG_SAR_OFFSET_P_IN_SHIFT)
#define REG_SAR_OFFSET_P_IN(n)              BITFIELD_VAL(REG_SAR_OFFSET_P_IN, n)


// REG_0B1
#define REG_SAR_ADC_OFFSET_IN_SHIFT         0
#define REG_SAR_ADC_OFFSET_IN_MASK          (0xFFFF << REG_SAR_ADC_OFFSET_IN_SHIFT)
#define REG_SAR_ADC_OFFSET_IN(n)            BITFIELD_VAL(REG_SAR_ADC_OFFSET_IN, n)

// REG_0B2
#define REG_EFUSE_REDUNDANCY_DATA_OUT_SHIFT 2
#define REG_EFUSE_REDUNDANCY_DATA_OUT_MASK  (0xF << REG_EFUSE_REDUNDANCY_DATA_OUT_SHIFT)
#define REG_EFUSE_REDUNDANCY_DATA_OUT(n)    BITFIELD_VAL(REG_EFUSE_REDUNDANCY_DATA_OUT, n)
#define REG_EFUSE_REDUNDANCY_DATA_OUT_DR    (1 << 1)
#define REG_EFUSE_DATA_OUT_DR               (1 << 0)

// REG_0B3
#define REG_EFUSE_DATA_OUT_HI_SHIFT         0
#define REG_EFUSE_DATA_OUT_HI_MASK          (0xFFFF << REG_EFUSE_DATA_OUT_HI_SHIFT)
#define REG_EFUSE_DATA_OUT_HI(n)            BITFIELD_VAL(REG_EFUSE_DATA_OUT_HI, n)

// REG_0B4
#define REG_EFUSE_DATA_OUT_LO_SHIFT         0
#define REG_EFUSE_DATA_OUT_LO_MASK          (0xFFFF << REG_EFUSE_DATA_OUT_LO_SHIFT)
#define REG_EFUSE_DATA_OUT_LO(n)            BITFIELD_VAL(REG_EFUSE_DATA_OUT_LO, n)

// REG_0B5
#define EFUSE_REDUNDANCY_DATA_OUT_SHIFT     0
#define EFUSE_REDUNDANCY_DATA_OUT_MASK      (0xF << EFUSE_REDUNDANCY_DATA_OUT_SHIFT)
#define EFUSE_REDUNDANCY_DATA_OUT(n)        BITFIELD_VAL(EFUSE_REDUNDANCY_DATA_OUT, n)

// REG_0B6
#define EFUSE_CHIP_SEL_ENB_DR               (1 << 15)
#define EFUSE_CHIP_SEL_ENB_REG              (1 << 14)
#define EFUSE_STROBE_DR                     (1 << 13)
#define EFUSE_STROBE_REG                    (1 << 12)
#define EFUSE_LOAD_DR                       (1 << 11)
#define EFUSE_LOAD_REG                      (1 << 10)
#define EFUSE_PGM_ENB_DR                    (1 << 9)
#define EFUSE_PGM_ENB_REG                   (1 << 8)
#define EFUSE_PGM_SEL_DR                    (1 << 7)
#define EFUSE_PGM_SEL_REG                   (1 << 6)
#define EFUSE_READ_MODE_DR                  (1 << 5)
#define EFUSE_READ_MODE_REG                 (1 << 4)
#define EFUSE_PWR_DN_ENB_DR                 (1 << 3)
#define EFUSE_PWR_DN_ENB_REG                (1 << 2)
#define EFUSE_REDUNDANCY_EN_DR              (1 << 1)
#define EFUSE_REDUNDANCY_EN_REG             (1 << 0)

// REG_0B7
#define REG_EFUSE_ADDRESS_SHIFT             6
#define REG_EFUSE_ADDRESS_MASK              (0x1FF << REG_EFUSE_ADDRESS_SHIFT)
#define REG_EFUSE_ADDRESS(n)                BITFIELD_VAL(REG_EFUSE_ADDRESS, n)
#define REG_EFUSE_STROBE_TRIGGER            (1 << 5)
#define REG_EFUSE_TURN_ON                   (1 << 4)
#define REG_EFUSE_CLK_EN                    (1 << 3)
#define REG_EFUSE_READ_MODE                 (1 << 2)
#define REG_EFUSE_PGM_MODE                  (1 << 1)
#define REG_EFUSE_PGM_READ_SEL              (1 << 0)

// REG_0B8
#define REG_EFUSE_TIME_CSB_ADDR_VALUE_SHIFT 10
#define REG_EFUSE_TIME_CSB_ADDR_VALUE_MASK  (0xF << REG_EFUSE_TIME_CSB_ADDR_VALUE_SHIFT)
#define REG_EFUSE_TIME_CSB_ADDR_VALUE(n)    BITFIELD_VAL(REG_EFUSE_TIME_CSB_ADDR_VALUE, n)
#define REG_EFUSE_TIME_PS_CSB_VALUE_SHIFT   6
#define REG_EFUSE_TIME_PS_CSB_VALUE_MASK    (0xF << REG_EFUSE_TIME_PS_CSB_VALUE_SHIFT)
#define REG_EFUSE_TIME_PS_CSB_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_PS_CSB_VALUE, n)
#define REG_EFUSE_TIME_PD_PS_VALUE_SHIFT    0
#define REG_EFUSE_TIME_PD_PS_VALUE_MASK     (0x3F << REG_EFUSE_TIME_PD_PS_VALUE_SHIFT)
#define REG_EFUSE_TIME_PD_PS_VALUE(n)       BITFIELD_VAL(REG_EFUSE_TIME_PD_PS_VALUE, n)


// REG_0B9
#define REG_EFUSE_TIME_PGM_STROBING_VALUE_SHIFT 4
#define REG_EFUSE_TIME_PGM_STROBING_VALUE_MASK (0x1FF << REG_EFUSE_TIME_PGM_STROBING_VALUE_SHIFT)
#define REG_EFUSE_TIME_PGM_STROBING_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_PGM_STROBING_VALUE, n)
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE_SHIFT 0
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE_MASK (0xF << REG_EFUSE_TIME_ADDR_STROBE_VALUE_SHIFT)
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_ADDR_STROBE_VALUE, n)

// REG_0BA
#define REG_EFUSE_TIME_READ_STROBING_VALUE_SHIFT 0
#define REG_EFUSE_TIME_READ_STROBING_VALUE_MASK (0x1FF << REG_EFUSE_TIME_READ_STROBING_VALUE_SHIFT)
#define REG_EFUSE_TIME_READ_STROBING_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_READ_STROBING_VALUE, n)

// REG_0BB
#define REG_EFUSE_TIME_PS_PD_VALUE_SHIFT    10
#define REG_EFUSE_TIME_PS_PD_VALUE_MASK     (0x3F << REG_EFUSE_TIME_PS_PD_VALUE_SHIFT)
#define REG_EFUSE_TIME_PS_PD_VALUE(n)       BITFIELD_VAL(REG_EFUSE_TIME_PS_PD_VALUE, n)
#define REG_EFUSE_TIME_CSB_PS_VALUE_SHIFT   6
#define REG_EFUSE_TIME_CSB_PS_VALUE_MASK    (0xF << REG_EFUSE_TIME_CSB_PS_VALUE_SHIFT)
#define REG_EFUSE_TIME_CSB_PS_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_CSB_PS_VALUE, n)
#define REG_EFUSE_TIME_STROBE_CSB_VALUE_SHIFT 0
#define REG_EFUSE_TIME_STROBE_CSB_VALUE_MASK (0x3F << REG_EFUSE_TIME_STROBE_CSB_VALUE_SHIFT)
#define REG_EFUSE_TIME_STROBE_CSB_VALUE(n)  BITFIELD_VAL(REG_EFUSE_TIME_STROBE_CSB_VALUE, n)

// REG_0BC
#define EFUSE_REDUNDANCY_INFO_ROW_SEL_DR    (1 << 7)
#define EFUSE_REDUNDANCY_INFO_ROW_SEL_REG   (1 << 6)
#define REG_EFUSE_TIME_PD_OFF_VALUE_SHIFT   0
#define REG_EFUSE_TIME_PD_OFF_VALUE_MASK    (0x3F << REG_EFUSE_TIME_PD_OFF_VALUE_SHIFT)
#define REG_EFUSE_TIME_PD_OFF_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_PD_OFF_VALUE, n)

// REG_0BD
#define EFUSE_DATA_OUT_HI_SHIFT             0
#define EFUSE_DATA_OUT_HI_MASK              (0xFFFF << EFUSE_DATA_OUT_HI_SHIFT)
#define EFUSE_DATA_OUT_HI(n)                BITFIELD_VAL(EFUSE_DATA_OUT_HI, n)

// REG_0BE
#define EFUSE_DATA_OUT_LO_SHIFT             0
#define EFUSE_DATA_OUT_LO_MASK              (0xFFFF << EFUSE_DATA_OUT_LO_SHIFT)
#define EFUSE_DATA_OUT_LO(n)                BITFIELD_VAL(EFUSE_DATA_OUT_LO, n)

// REG_0BF
#define REG_BUCK_HPPA_IS_GAIN_DSLEEP_SHIFT  10
#define REG_BUCK_HPPA_IS_GAIN_DSLEEP_MASK   (0x7 << REG_BUCK_HPPA_IS_GAIN_DSLEEP_SHIFT)
#define REG_BUCK_HPPA_IS_GAIN_DSLEEP(n)     BITFIELD_VAL(REG_BUCK_HPPA_IS_GAIN_DSLEEP, n)
#define REG_BUCK_HPPA_IS_GAIN_NORMAL_SHIFT  7
#define REG_BUCK_HPPA_IS_GAIN_NORMAL_MASK   (0x7 << REG_BUCK_HPPA_IS_GAIN_NORMAL_SHIFT)
#define REG_BUCK_HPPA_IS_GAIN_NORMAL(n)     BITFIELD_VAL(REG_BUCK_HPPA_IS_GAIN_NORMAL, n)
#define REG_BUCK_HPPA_ULP_MODE_DSLEEP       (1 << 6)
#define REG_BUCK_HPPA_ULP_MODE_NORMAL       (1 << 5)
#define REG_BUCK_HPPA_BURST_MODE_NORMAL     (1 << 4)
#define REG_BUCK_HPPA_BURST_MODE_DSLEEP     (1 << 3)
#define REG_PU_DCDC_HPPA_DSLEEP             (1 << 2)
#define REG_PU_DCDC_HPPA_REG                (1 << 1)
#define REG_PU_DCDC_HPPA_DR                 (1 << 0)

// REG_0C0
#define REG_BUCK_HPPA_VBIT_DSLEEP_SHIFT     8
#define REG_BUCK_HPPA_VBIT_DSLEEP_MASK      (0xFF << REG_BUCK_HPPA_VBIT_DSLEEP_SHIFT)
#define REG_BUCK_HPPA_VBIT_DSLEEP(n)        BITFIELD_VAL(REG_BUCK_HPPA_VBIT_DSLEEP, n)
#define REG_BUCK_HPPA_VBIT_NORMAL_SHIFT     0
#define REG_BUCK_HPPA_VBIT_NORMAL_MASK      (0xFF << REG_BUCK_HPPA_VBIT_NORMAL_SHIFT)
#define REG_BUCK_HPPA_VBIT_NORMAL(n)        BITFIELD_VAL(REG_BUCK_HPPA_VBIT_NORMAL, n)
#define MAX_DCDC_HPPA_VBIT_VAL              (REG_BUCK_HPPA_VBIT_NORMAL_MASK >> REG_BUCK_HPPA_VBIT_NORMAL_SHIFT)

// REG_0C1
#define REG_BUCK_HPPA_SYNC_DISABLE          (1 << 15)
#define REG_BUCK_HPPA_SOFT_START_EN         (1 << 14)
#define REG_BUCK_HPPA_SLOPE_DOUBLE          (1 << 13)
#define REG_BUCK_HPPA_DT_BIT                (1 << 12)
#define REG_BUCK_HPPA_CAL_DELTAV_SHIFT      6
#define REG_BUCK_HPPA_CAL_DELTAV_MASK       (0x3F << REG_BUCK_HPPA_CAL_DELTAV_SHIFT)
#define REG_BUCK_HPPA_CAL_DELTAV(n)         BITFIELD_VAL(REG_BUCK_HPPA_CAL_DELTAV, n)
#define REG_BUCK_HPPA_HYSTERESIS_BIT_SHIFT  3
#define REG_BUCK_HPPA_HYSTERESIS_BIT_MASK   (0x7 << REG_BUCK_HPPA_HYSTERESIS_BIT_SHIFT)
#define REG_BUCK_HPPA_HYSTERESIS_BIT(n)     BITFIELD_VAL(REG_BUCK_HPPA_HYSTERESIS_BIT, n)
#define REG_BUCK_HPPA_COUNTER_SEL_SHIFT     1
#define REG_BUCK_HPPA_COUNTER_SEL_MASK      (0x3 << REG_BUCK_HPPA_COUNTER_SEL_SHIFT)
#define REG_BUCK_HPPA_COUNTER_SEL(n)        BITFIELD_VAL(REG_BUCK_HPPA_COUNTER_SEL, n)
#define REG_BUCK_HPPA_RAMP_EN               (1 << 0)

// REG_0C2
#define REG_BUCK_HPPA_ZCD_CAP_BIT_SHIFT     12
#define REG_BUCK_HPPA_ZCD_CAP_BIT_MASK      (0xF << REG_BUCK_HPPA_ZCD_CAP_BIT_SHIFT)
#define REG_BUCK_HPPA_ZCD_CAP_BIT(n)        BITFIELD_VAL(REG_BUCK_HPPA_ZCD_CAP_BIT, n)
#define REG_BUCK_HPPA_CAP_BIT_SHIFT         8
#define REG_BUCK_HPPA_CAP_BIT_MASK          (0xF << REG_BUCK_HPPA_CAP_BIT_SHIFT)
#define REG_BUCK_HPPA_CAP_BIT(n)            BITFIELD_VAL(REG_BUCK_HPPA_CAP_BIT, n)
#define REG_BUCK_HPPA_PULLDOWN_EN           (1 << 6)
#define REG_BUCK_HPPA_ANTI_RES_DISABLE      (1 << 5)
#define REG_BUCK_HPPA_OFFSET_BIT_SHIFT      0
#define REG_BUCK_HPPA_OFFSET_BIT_MASK       (0x1F << REG_BUCK_HPPA_OFFSET_BIT_SHIFT)
#define REG_BUCK_HPPA_OFFSET_BIT(n)         BITFIELD_VAL(REG_BUCK_HPPA_OFFSET_BIT, n)

// REG_0C3
#define REG_BUCK_HPPA_INTERNAL_FREQUENCY_SHIFT 13
#define REG_BUCK_HPPA_INTERNAL_FREQUENCY_MASK (0x7 << REG_BUCK_HPPA_INTERNAL_FREQUENCY_SHIFT)
#define REG_BUCK_HPPA_INTERNAL_FREQUENCY(n) BITFIELD_VAL(REG_BUCK_HPPA_INTERNAL_FREQUENCY, n)
#define REG_BUCK_HPPA_VREF_SEL_SHIFT        11
#define REG_BUCK_HPPA_VREF_SEL_MASK         (0x3 << REG_BUCK_HPPA_VREF_SEL_SHIFT)
#define REG_BUCK_HPPA_VREF_SEL(n)           BITFIELD_VAL(REG_BUCK_HPPA_VREF_SEL, n)
#define REG_BUCK_HPPA_TEST_POWERMOS         (1 << 10)
#define REG_BUCK_HPPA_EN_ZCD_CAL            (1 << 9)
#define REG_BUCK_HPPA_TEST_PMOS             (1 << 8)
#define REG_BUCK_HPPA_HALF_BIAS             (1 << 7)
#define REG_BUCK_HPPA_SLOPE_EN_BURST        (1 << 6)
#define REG_BUCK_HPPA_OFFSET_CURRENT_EN     (1 << 5)
#define REG_BUCK_HPPA_I_DELTAV_X2           (1 << 4)
#define REG_BUCK_HPPA_LOGIC_IR_CLK_EN       (1 << 3)
#define REG_BUCK_HPPA_ENB_PU_DELAY          (1 << 2)
#define REG_BUCK_HPPA_IX2_ERR               (1 << 1)
#define REG_BUCK_HPPA_LP_VCOMP              (1 << 0)

// REG_0C4
#define REG_BUCK_HPPA_VBAT_3V_EXTRA         (1 << 10)
#define REG_BUCK_HPPA_TEST_MODE_EN          (1 << 9)
#define REG_BUCK_HPPA_BURST_THRESHOLD_SHIFT 4
#define REG_BUCK_HPPA_BURST_THRESHOLD_MASK  (0x1F << REG_BUCK_HPPA_BURST_THRESHOLD_SHIFT)
#define REG_BUCK_HPPA_BURST_THRESHOLD(n)    BITFIELD_VAL(REG_BUCK_HPPA_BURST_THRESHOLD, n)
#define REG_BUCK_HPPA_EDGE_CON_SHIFT        0
#define REG_BUCK_HPPA_EDGE_CON_MASK         (0xF << REG_BUCK_HPPA_EDGE_CON_SHIFT)
#define REG_BUCK_HPPA_EDGE_CON(n)           BITFIELD_VAL(REG_BUCK_HPPA_EDGE_CON, n)

// REG_0C5
#define REG_BUCK_VCORE_SYNC_DISABLE         (1 << 15)
#define REG_BUCK_VCORE_SOFT_START_EN        (1 << 14)
#define REG_BUCK_VCORE_SLOPE_DOUBLE         (1 << 13)
#define REG_BUCK_VCORE_DT_BIT               (1 << 12)
#define REG_BUCK_VCORE_CAL_DELTAV_SHIFT     6
#define REG_BUCK_VCORE_CAL_DELTAV_MASK      (0x3F << REG_BUCK_VCORE_CAL_DELTAV_SHIFT)
#define REG_BUCK_VCORE_CAL_DELTAV(n)        BITFIELD_VAL(REG_BUCK_VCORE_CAL_DELTAV, n)
#define REG_BUCK_VCORE_HYSTERESIS_BIT_SHIFT 3
#define REG_BUCK_VCORE_HYSTERESIS_BIT_MASK  (0x7 << REG_BUCK_VCORE_HYSTERESIS_BIT_SHIFT)
#define REG_BUCK_VCORE_HYSTERESIS_BIT(n)    BITFIELD_VAL(REG_BUCK_VCORE_HYSTERESIS_BIT, n)
#define REG_BUCK_VCORE_COUNTER_SEL_SHIFT    1
#define REG_BUCK_VCORE_COUNTER_SEL_MASK     (0x3 << REG_BUCK_VCORE_COUNTER_SEL_SHIFT)
#define REG_BUCK_VCORE_COUNTER_SEL(n)       BITFIELD_VAL(REG_BUCK_VCORE_COUNTER_SEL, n)
#define REG_BUCK_VCORE_RAMP_EN              (1 << 0)

// REG_0C6
#define REG_BUCK_VCORE_ZCD_CAP_BIT_SHIFT    12
#define REG_BUCK_VCORE_ZCD_CAP_BIT_MASK     (0xF << REG_BUCK_VCORE_ZCD_CAP_BIT_SHIFT)
#define REG_BUCK_VCORE_ZCD_CAP_BIT(n)       BITFIELD_VAL(REG_BUCK_VCORE_ZCD_CAP_BIT, n)
#define REG_BUCK_VCORE_CAP_BIT_SHIFT        8
#define REG_BUCK_VCORE_CAP_BIT_MASK         (0xF << REG_BUCK_VCORE_CAP_BIT_SHIFT)
#define REG_BUCK_VCORE_CAP_BIT(n)           BITFIELD_VAL(REG_BUCK_VCORE_CAP_BIT, n)
#define REG_BUCK_VCORE_PULLDOWN_EN          (1 << 6)
#define REG_BUCK_VCORE_ANTI_RES_DISABLE     (1 << 5)
#define REG_BUCK_VCORE_OFFSET_BIT_SHIFT     0
#define REG_BUCK_VCORE_OFFSET_BIT_MASK      (0x1F << REG_BUCK_VCORE_OFFSET_BIT_SHIFT)
#define REG_BUCK_VCORE_OFFSET_BIT(n)        BITFIELD_VAL(REG_BUCK_VCORE_OFFSET_BIT, n)

// REG_0C7
#define REG_BUCK_VCORE_INTERNAL_FREQUENCY_SHIFT 13
#define REG_BUCK_VCORE_INTERNAL_FREQUENCY_MASK (0x7 << REG_BUCK_VCORE_INTERNAL_FREQUENCY_SHIFT)
#define REG_BUCK_VCORE_INTERNAL_FREQUENCY(n) BITFIELD_VAL(REG_BUCK_VCORE_INTERNAL_FREQUENCY, n)
#define REG_BUCK_VCORE_VREF_SEL_SHIFT       11
#define REG_BUCK_VCORE_VREF_SEL_MASK        (0x3 << REG_BUCK_VCORE_VREF_SEL_SHIFT)
#define REG_BUCK_VCORE_VREF_SEL(n)          BITFIELD_VAL(REG_BUCK_VCORE_VREF_SEL, n)
#define REG_BUCK_VCORE_TEST_POWERMOS        (1 << 10)
#define REG_BUCK_VCORE_EN_ZCD_CAL           (1 << 9)
#define REG_BUCK_VCORE_TEST_PMOS            (1 << 8)
#define REG_BUCK_VCORE_HALF_BIAS            (1 << 7)
#define REG_BUCK_VCORE_SLOPE_EN_BURST       (1 << 6)
#define REG_BUCK_VCORE_OFFSET_CURRENT_EN    (1 << 5)
#define REG_BUCK_VCORE_I_DELTAV_X2          (1 << 4)
#define REG_BUCK_VCORE_LOGIC_IR_CLK_EN      (1 << 3)
#define REG_BUCK_VCORE_ENB_PU_DELAY         (1 << 2)
#define REG_BUCK_VCORE_IX2_ERR              (1 << 1)
#define REG_BUCK_VCORE_LP_VCOMP             (1 << 0)

// REG_0C8
#define REG_BUCK_VCORE_VBAT_3V_EXTRA        (1 << 10)
#define REG_BUCK_VCORE_TEST_MODE_EN         (1 << 9)
#define REG_BUCK_VCORE_BURST_THRESHOLD_SHIFT 4
#define REG_BUCK_VCORE_BURST_THRESHOLD_MASK (0x1F << REG_BUCK_VCORE_BURST_THRESHOLD_SHIFT)
#define REG_BUCK_VCORE_BURST_THRESHOLD(n)   BITFIELD_VAL(REG_BUCK_VCORE_BURST_THRESHOLD, n)
#define REG_BUCK_VCORE_EDGE_CON_SHIFT       0
#define REG_BUCK_VCORE_EDGE_CON_MASK        (0xF << REG_BUCK_VCORE_EDGE_CON_SHIFT)
#define REG_BUCK_VCORE_EDGE_CON(n)          BITFIELD_VAL(REG_BUCK_VCORE_EDGE_CON, n)

// REG_0C9
#define SAR_EN_VREF_CALIB                   (1 << 6)
#define SAR_OFFSET_BIT_VREF_SHIFT           0
#define SAR_OFFSET_BIT_VREF_MASK            (0x3F << SAR_OFFSET_BIT_VREF_SHIFT)
#define SAR_OFFSET_BIT_VREF(n)              BITFIELD_VAL(SAR_OFFSET_BIT_VREF, n)

// REG_0CA
#define SAR_RAW_DATA_17_2_SHIFT             0
#define SAR_RAW_DATA_17_2_MASK              (0xFFFF << SAR_RAW_DATA_17_2_SHIFT)
#define SAR_RAW_DATA_17_2(n)                BITFIELD_VAL(SAR_RAW_DATA_17_2, n)

// REG_0CB
#define DIG_A7PLL_LOCK                      (1 << 15)
#define SAR_RAW_DATA_1_0_SHIFT              0
#define SAR_RAW_DATA_1_0_MASK               (0x3 << SAR_RAW_DATA_1_0_SHIFT)
#define SAR_RAW_DATA_1_0(n)                 BITFIELD_VAL(SAR_RAW_DATA_1_0, n)

#endif
