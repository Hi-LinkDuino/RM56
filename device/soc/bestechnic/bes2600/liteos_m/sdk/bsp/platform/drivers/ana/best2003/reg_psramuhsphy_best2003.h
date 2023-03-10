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
#ifndef __REG_PSRAMUHSPHY_BEST2003_H__
#define __REG_PSRAMUHSPHY_BEST2003_H__

#include "plat_types.h"

enum PSRAMPHY_REG_T {
    PSRAMPHY_REG_00   = 0x00,
    PSRAMPHY_REG_01,
    PSRAMPHY_REG_02,
    PSRAMPHY_REG_03,
    PSRAMPHY_REG_04,
    PSRAMPHY_REG_05,
    PSRAMPHY_REG_06,
    PSRAMPHY_REG_07,
    PSRAMPHY_REG_08,
    PSRAMPHY_REG_09,
    PSRAMPHY_REG_0A,
    PSRAMPHY_REG_0B,
    PSRAMPHY_REG_0C,
    PSRAMPHY_REG_0D,
    PSRAMPHY_REG_0E,
    PSRAMPHY_REG_0F,
    PSRAMPHY_REG_10,
    PSRAMPHY_REG_11,
    PSRAMPHY_REG_12,
    PSRAMPHY_REG_13,
    PSRAMPHY_REG_14,
    PSRAMPHY_REG_15,
    PSRAMPHY_REG_16,
    PSRAMPHY_REG_17,
    PSRAMPHY_REG_60 = 0x60,
};
// REG_000
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_001
#define ANA0_REG_LDO_DVDD_PRECHARGE         (1 << 0)
#define ANA0_REG_VH_PRECHARGE               (1 << 1)
#define ANA0_REG_LDO_AVDDQ_PRECHARGE        (1 << 2)
#define ANA0_REG_VL_PU                      (1 << 3)
#define ANA0_REG_VH_PU                      (1 << 4)
#define ANA0_REG_AVDDQ_LDO_PU               (1 << 5)
#define ANA0_REG_AVDDQ_PU                   (1 << 6)
#define ANA0_REG_PSRAM_DIE_PU1P8            (1 << 7)
#define ANA0_REG_PSRAM_DIE_PU1P3            (1 << 8)
#define ANA0_REG_DVDD_PU                    (1 << 9)
#define ANA0_REG_VL_LDO_PU                  (1 << 10)
#define ANA0_REG_VH_LDO_PU                  (1 << 11)
#define ANA0_REG_LDO_DVDD_PU                (1 << 12)
#define ANA0_REG_PSRAM_PUOK                 (1 << 13)
#define ANA0_REG_DLL_CLK_RDY                (1 << 14)
#define ANA0_REG_DLL_RESETB                 (1 << 15)

// REG_002
#define ANA0_REG_LDO_AVDDQ_RES_SHIFT        0
#define ANA0_REG_LDO_AVDDQ_RES_MASK         (0xF << ANA0_REG_LDO_AVDDQ_RES_SHIFT)
#define ANA0_REG_LDO_AVDDQ_RES(n)           BITFIELD_VAL(ANA0_REG_LDO_AVDDQ_RES, n)
#define ANA0_REG_LDO_DVDD_RES_SHIFT         4
#define ANA0_REG_LDO_DVDD_RES_MASK          (0xF << ANA0_REG_LDO_DVDD_RES_SHIFT)
#define ANA0_REG_LDO_DVDD_RES(n)            BITFIELD_VAL(ANA0_REG_LDO_DVDD_RES, n)
#define ANA0_REG_VL_RES_SHIFT               8
#define ANA0_REG_VL_RES_MASK                (0x3 << ANA0_REG_VL_RES_SHIFT)
#define ANA0_REG_VL_RES(n)                  BITFIELD_VAL(ANA0_REG_VL_RES, n)
#define ANA0_REG_VH_RE_SHIFT                10
#define ANA0_REG_VH_RE_MASK                 (0x3 << ANA0_REG_VH_RE_SHIFT)
#define ANA0_REG_VH_RE(n)                   BITFIELD_VAL(ANA0_REG_VH_RE, n)
#define ANA0_REG_VREF_VTUNE_SHIFT           12
#define ANA0_REG_VREF_VTUNE_MASK            (0x7 << ANA0_REG_VREF_VTUNE_SHIFT)
#define ANA0_REG_VREF_VTUNE(n)              BITFIELD_VAL(ANA0_REG_VREF_VTUNE, n)

// REG_003
#define ANA0_REG_DVDD_SWRC_SHIFT            0
#define ANA0_REG_DVDD_SWRC_MASK             (0x3 << ANA0_REG_DVDD_SWRC_SHIFT)
#define ANA0_REG_DVDD_SWRC(n)               BITFIELD_VAL(ANA0_REG_DVDD_SWRC, n)
#define ANA0_REG_ADQ_DELAY_SEL              (1 << 2)
#define ANA0_REG_ODT_STRENGTH_SEL           (1 << 3)
#define ANA0_REG_RANGE_CK_SHIFT             4
#define ANA0_REG_RANGE_CK_MASK              (0x3 << ANA0_REG_RANGE_CK_SHIFT)
#define ANA0_REG_RANGE_CK(n)                BITFIELD_VAL(ANA0_REG_RANGE_CK, n)
#define ANA0_DLL_LOCK                       (1 << 6)
#define ANA0_DLL_ALL_ONE                    (1 << 7)
#define ANA0_DLL_ALL_ZERO                   (1 << 8)
#define ANA0_DLL_DLY_IN_SHIFT               9
#define ANA0_DLL_DLY_IN_MASK                (0x3F << ANA0_DLL_DLY_IN_SHIFT)
#define ANA0_DLL_DLY_IN(n)                  BITFIELD_VAL(ANA0_DLL_DLY_IN, n)

// REG_004
#define ANA0_REG_DLL_DLY_INI_SHIFT          0
#define ANA0_REG_DLL_DLY_INI_MASK           (0xFF << ANA0_REG_DLL_DLY_INI_SHIFT)
#define ANA0_REG_DLL_DLY_INI(n)             BITFIELD_VAL(ANA0_REG_DLL_DLY_INI, n)
#define ANA0_REG_DLL_SHIFT                  8
#define ANA0_REG_DLL_MASK                   (0xFF << ANA0_REG_DLL_SHIFT)
#define ANA0_REG_DLL(n)                     BITFIELD_VAL(ANA0_REG_DLL, n)

// REG_005
#define ANA0_REG_PSRAM_ODT_SHIFT            0
#define ANA0_REG_PSRAM_ODT_MASK             (0x7 << ANA0_REG_PSRAM_ODT_SHIFT)
#define ANA0_REG_PSRAM_ODT(n)               BITFIELD_VAL(ANA0_REG_PSRAM_ODT, n)
#define ANA0_REG_PSRAM_RX_BIAS_SHIFT        3
#define ANA0_REG_PSRAM_RX_BIAS_MASK         (0x3 << ANA0_REG_PSRAM_RX_BIAS_SHIFT)
#define ANA0_REG_PSRAM_RX_BIAS(n)           BITFIELD_VAL(ANA0_REG_PSRAM_RX_BIAS, n)
#define ANA0_REG_PSRAM_PULLUP               (1 << 5)
#define ANA0_REG_PSRAM_PULLDN               (1 << 6)
#define ANA0_REG_PSRAM_PULLUP_DQS_SHIFT     7
#define ANA0_REG_PSRAM_PULLUP_DQS_MASK      (0x3 << ANA0_REG_PSRAM_PULLUP_DQS_SHIFT)
#define ANA0_REG_PSRAM_PULLUP_DQS(n)        BITFIELD_VAL(ANA0_REG_PSRAM_PULLUP_DQS, n)
#define ANA0_REG_PSRAM_PULLDN_DQS_SHIFT     9
#define ANA0_REG_PSRAM_PULLDN_DQS_MASK      (0x3 << ANA0_REG_PSRAM_PULLDN_DQS_SHIFT)
#define ANA0_REG_PSRAM_PULLDN_DQS(n)        BITFIELD_VAL(ANA0_REG_PSRAM_PULLDN_DQS, n)
#define ANA0_REG_PSRAM_TX_WR_CK_SEL         (1 << 11)
#define ANA0_REG_PSRAM_TX_RD_CK_SEL         (1 << 12)
#define ANA0_REG_PSRAM_TXDRV_CKE_SHIFT      13
#define ANA0_REG_PSRAM_TXDRV_CKE_MASK       (0x7 << ANA0_REG_PSRAM_TXDRV_CKE_SHIFT)
#define ANA0_REG_PSRAM_TXDRV_CKE(n)         BITFIELD_VAL(ANA0_REG_PSRAM_TXDRV_CKE, n)

// REG_006
#define ANA0_REG_PSRAM_TX_DM_DLY_SHIFT      0
#define ANA0_REG_PSRAM_TX_DM_DLY_MASK       (0x1F << ANA0_REG_PSRAM_TX_DM_DLY_SHIFT)
#define ANA0_REG_PSRAM_TX_DM_DLY(n)         BITFIELD_VAL(ANA0_REG_PSRAM_TX_DM_DLY, n)
#define ANA0_REG_PSRAM_TX_DQ_DLY_4_0_SHIFT  5
#define ANA0_REG_PSRAM_TX_DQ_DLY_4_0_MASK   (0x1F << ANA0_REG_PSRAM_TX_DQ_DLY_4_0_SHIFT)
#define ANA0_REG_PSRAM_TX_DQ_DLY_4_0(n)     BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQ_DLY_4_0, n)
#define ANA0_REG_PSRAM_TX_DQ_DLY_9_5_SHIFT  10
#define ANA0_REG_PSRAM_TX_DQ_DLY_9_5_MASK   (0x1F << ANA0_REG_PSRAM_TX_DQ_DLY_9_5_SHIFT)
#define ANA0_REG_PSRAM_TX_DQ_DLY_9_5(n)     BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQ_DLY_9_5, n)

// REG_007
#define ANA0_REG_PSRAM_TX_DQ_DLY_14_10_SHIFT 0
#define ANA0_REG_PSRAM_TX_DQ_DLY_14_10_MASK (0x1F << ANA0_REG_PSRAM_TX_DQ_DLY_14_10_SHIFT)
#define ANA0_REG_PSRAM_TX_DQ_DLY_14_10(n)   BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQ_DLY_14_10, n)
#define ANA0_REG_PSRAM_TX_DQ_DLY_19_15_SHIFT 5
#define ANA0_REG_PSRAM_TX_DQ_DLY_19_15_MASK (0x1F << ANA0_REG_PSRAM_TX_DQ_DLY_19_15_SHIFT)
#define ANA0_REG_PSRAM_TX_DQ_DLY_19_15(n)   BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQ_DLY_19_15, n)
#define ANA0_REG_PSRAM_TX_DQ_DLY_24_20_SHIFT 10
#define ANA0_REG_PSRAM_TX_DQ_DLY_24_20_MASK (0x1F << ANA0_REG_PSRAM_TX_DQ_DLY_24_20_SHIFT)
#define ANA0_REG_PSRAM_TX_DQ_DLY_24_20(n)   BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQ_DLY_24_20, n)

// REG_008
#define ANA0_REG_PSRAM_TX_DQ_DLY_29_25_SHIFT 0
#define ANA0_REG_PSRAM_TX_DQ_DLY_29_25_MASK (0x1F << ANA0_REG_PSRAM_TX_DQ_DLY_29_25_SHIFT)
#define ANA0_REG_PSRAM_TX_DQ_DLY_29_25(n)   BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQ_DLY_29_25, n)
#define ANA0_REG_PSRAM_TX_DQ_DLY_34_30_SHIFT 5
#define ANA0_REG_PSRAM_TX_DQ_DLY_34_30_MASK (0x1F << ANA0_REG_PSRAM_TX_DQ_DLY_34_30_SHIFT)
#define ANA0_REG_PSRAM_TX_DQ_DLY_34_30(n)   BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQ_DLY_34_30, n)
#define ANA0_REG_PSRAM_TX_DQ_DLY_39_35_SHIFT 10
#define ANA0_REG_PSRAM_TX_DQ_DLY_39_35_MASK (0x1F << ANA0_REG_PSRAM_TX_DQ_DLY_39_35_SHIFT)
#define ANA0_REG_PSRAM_TX_DQ_DLY_39_35(n)   BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQ_DLY_39_35, n)

// REG_009
#define ANA0_REG_PSRAM_TX_CMD_DLY_4_0_SHIFT 0
#define ANA0_REG_PSRAM_TX_CMD_DLY_4_0_MASK  (0x1F << ANA0_REG_PSRAM_TX_CMD_DLY_4_0_SHIFT)
#define ANA0_REG_PSRAM_TX_CMD_DLY_4_0(n)    BITFIELD_VAL(ANA0_REG_PSRAM_TX_CMD_DLY_4_0, n)
#define ANA0_REG_PSRAM_TX_CMD_DLY_9_5_SHIFT 5
#define ANA0_REG_PSRAM_TX_CMD_DLY_9_5_MASK  (0x1F << ANA0_REG_PSRAM_TX_CMD_DLY_9_5_SHIFT)
#define ANA0_REG_PSRAM_TX_CMD_DLY_9_5(n)    BITFIELD_VAL(ANA0_REG_PSRAM_TX_CMD_DLY_9_5, n)
#define ANA0_REG_PSRAM_TX_CMD_DLY_14_10_SHIFT 10
#define ANA0_REG_PSRAM_TX_CMD_DLY_14_10_MASK (0x1F << ANA0_REG_PSRAM_TX_CMD_DLY_14_10_SHIFT)
#define ANA0_REG_PSRAM_TX_CMD_DLY_14_10(n)  BITFIELD_VAL(ANA0_REG_PSRAM_TX_CMD_DLY_14_10, n)

// REG_00A
#define ANA0_REG_PSRAM_TX_CMD_DLY_19_15_SHIFT 0
#define ANA0_REG_PSRAM_TX_CMD_DLY_19_15_MASK (0x1F << ANA0_REG_PSRAM_TX_CMD_DLY_19_15_SHIFT)
#define ANA0_REG_PSRAM_TX_CMD_DLY_19_15(n)  BITFIELD_VAL(ANA0_REG_PSRAM_TX_CMD_DLY_19_15, n)
#define ANA0_REG_PSRAM_TX_CMD_DLY_24_20_SHIFT 5
#define ANA0_REG_PSRAM_TX_CMD_DLY_24_20_MASK (0x1F << ANA0_REG_PSRAM_TX_CMD_DLY_24_20_SHIFT)
#define ANA0_REG_PSRAM_TX_CMD_DLY_24_20(n)  BITFIELD_VAL(ANA0_REG_PSRAM_TX_CMD_DLY_24_20, n)
#define ANA0_REG_PSRAM_TX_CMD_DLY_29_25_SHIFT 10
#define ANA0_REG_PSRAM_TX_CMD_DLY_29_25_MASK (0x1F << ANA0_REG_PSRAM_TX_CMD_DLY_29_25_SHIFT)
#define ANA0_REG_PSRAM_TX_CMD_DLY_29_25(n)  BITFIELD_VAL(ANA0_REG_PSRAM_TX_CMD_DLY_29_25, n)

// REG_00B
#define ANA0_REG_PSRAM_TX_CMD_DLY_34_30_SHIFT 0
#define ANA0_REG_PSRAM_TX_CMD_DLY_34_30_MASK (0x1F << ANA0_REG_PSRAM_TX_CMD_DLY_34_30_SHIFT)
#define ANA0_REG_PSRAM_TX_CMD_DLY_34_30(n)  BITFIELD_VAL(ANA0_REG_PSRAM_TX_CMD_DLY_34_30, n)
#define ANA0_REG_PSRAM_TX_CMD_DLY_39_35_SHIFT 5
#define ANA0_REG_PSRAM_TX_CMD_DLY_39_35_MASK (0x1F << ANA0_REG_PSRAM_TX_CMD_DLY_39_35_SHIFT)
#define ANA0_REG_PSRAM_TX_CMD_DLY_39_35(n)  BITFIELD_VAL(ANA0_REG_PSRAM_TX_CMD_DLY_39_35, n)

// REG_00C
#define ANA0_REG_PSRAM_TX_DQS_DLY_SHIFT     0
#define ANA0_REG_PSRAM_TX_DQS_DLY_MASK      (0x1F << ANA0_REG_PSRAM_TX_DQS_DLY_SHIFT)
#define ANA0_REG_PSRAM_TX_DQS_DLY(n)        BITFIELD_VAL(ANA0_REG_PSRAM_TX_DQS_DLY, n)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_4_0_SHIFT 5
#define ANA0_REG_PSRAM_RX_ADQ_DLY_4_0_MASK  (0x1F << ANA0_REG_PSRAM_RX_ADQ_DLY_4_0_SHIFT)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_4_0(n)    BITFIELD_VAL(ANA0_REG_PSRAM_RX_ADQ_DLY_4_0, n)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_9_5_SHIFT 10
#define ANA0_REG_PSRAM_RX_ADQ_DLY_9_5_MASK  (0x1F << ANA0_REG_PSRAM_RX_ADQ_DLY_9_5_SHIFT)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_9_5(n)    BITFIELD_VAL(ANA0_REG_PSRAM_RX_ADQ_DLY_9_5, n)

// REG_00D
#define ANA0_REG_PSRAM_RX_ADQ_DLY_14_10_SHIFT 0
#define ANA0_REG_PSRAM_RX_ADQ_DLY_14_10_MASK (0x1F << ANA0_REG_PSRAM_RX_ADQ_DLY_14_10_SHIFT)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_14_10(n)  BITFIELD_VAL(ANA0_REG_PSRAM_RX_ADQ_DLY_14_10, n)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_19_15_SHIFT 5
#define ANA0_REG_PSRAM_RX_ADQ_DLY_19_15_MASK (0x1F << ANA0_REG_PSRAM_RX_ADQ_DLY_19_15_SHIFT)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_19_15(n)  BITFIELD_VAL(ANA0_REG_PSRAM_RX_ADQ_DLY_19_15, n)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_24_20_SHIFT 10
#define ANA0_REG_PSRAM_RX_ADQ_DLY_24_20_MASK (0x1F << ANA0_REG_PSRAM_RX_ADQ_DLY_24_20_SHIFT)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_24_20(n)  BITFIELD_VAL(ANA0_REG_PSRAM_RX_ADQ_DLY_24_20, n)

// REG_00E
#define ANA0_REG_PSRAM_RX_ADQ_DLY_29_25_SHIFT 0
#define ANA0_REG_PSRAM_RX_ADQ_DLY_29_25_MASK (0x1F << ANA0_REG_PSRAM_RX_ADQ_DLY_29_25_SHIFT)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_29_25(n)  BITFIELD_VAL(ANA0_REG_PSRAM_RX_ADQ_DLY_29_25, n)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_34_30_SHIFT 5
#define ANA0_REG_PSRAM_RX_ADQ_DLY_34_30_MASK (0x1F << ANA0_REG_PSRAM_RX_ADQ_DLY_34_30_SHIFT)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_34_30(n)  BITFIELD_VAL(ANA0_REG_PSRAM_RX_ADQ_DLY_34_30, n)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_39_35_SHIFT 10
#define ANA0_REG_PSRAM_RX_ADQ_DLY_39_35_MASK (0x1F << ANA0_REG_PSRAM_RX_ADQ_DLY_39_35_SHIFT)
#define ANA0_REG_PSRAM_RX_ADQ_DLY_39_35(n)  BITFIELD_VAL(ANA0_REG_PSRAM_RX_ADQ_DLY_39_35, n)

// REG_00F
#define ANA0_REG_PSRAM_RX_DQS_DLY_SHIFT     0
#define ANA0_REG_PSRAM_RX_DQS_DLY_MASK      (0x1F << ANA0_REG_PSRAM_RX_DQS_DLY_SHIFT)
#define ANA0_REG_PSRAM_RX_DQS_DLY(n)        BITFIELD_VAL(ANA0_REG_PSRAM_RX_DQS_DLY, n)
#define ANA0_REG_PSRAM_TX_CLK_DLY_SHIFT     5
#define ANA0_REG_PSRAM_TX_CLK_DLY_MASK      (0x1F << ANA0_REG_PSRAM_TX_CLK_DLY_SHIFT)
#define ANA0_REG_PSRAM_TX_CLK_DLY(n)        BITFIELD_VAL(ANA0_REG_PSRAM_TX_CLK_DLY, n)
#define ANA0_REG_PSRAM_TX_CEB_DLY_SHIFT     10
#define ANA0_REG_PSRAM_TX_CEB_DLY_MASK      (0x1F << ANA0_REG_PSRAM_TX_CEB_DLY_SHIFT)
#define ANA0_REG_PSRAM_TX_CEB_DLY(n)        BITFIELD_VAL(ANA0_REG_PSRAM_TX_CEB_DLY, n)

// REG_010
#define ANA0_REG_PSRAM_OCD_CLKP_SHIFT       0
#define ANA0_REG_PSRAM_OCD_CLKP_MASK        (0x1F << ANA0_REG_PSRAM_OCD_CLKP_SHIFT)
#define ANA0_REG_PSRAM_OCD_CLKP(n)          BITFIELD_VAL(ANA0_REG_PSRAM_OCD_CLKP, n)
#define ANA0_REG_PSRAM_OCD_CLKN_SHIFT       5
#define ANA0_REG_PSRAM_OCD_CLKN_MASK        (0x1F << ANA0_REG_PSRAM_OCD_CLKN_SHIFT)
#define ANA0_REG_PSRAM_OCD_CLKN(n)          BITFIELD_VAL(ANA0_REG_PSRAM_OCD_CLKN, n)

// REG_011
#define ANA0_REG_PSRAM_OCD_DQSP_SHIFT       0
#define ANA0_REG_PSRAM_OCD_DQSP_MASK        (0x1F << ANA0_REG_PSRAM_OCD_DQSP_SHIFT)
#define ANA0_REG_PSRAM_OCD_DQSP(n)          BITFIELD_VAL(ANA0_REG_PSRAM_OCD_DQSP, n)
#define ANA0_REG_PSRAM_OCD_DQSN_SHIFT       5
#define ANA0_REG_PSRAM_OCD_DQSN_MASK        (0x1F << ANA0_REG_PSRAM_OCD_DQSN_SHIFT)
#define ANA0_REG_PSRAM_OCD_DQSN(n)          BITFIELD_VAL(ANA0_REG_PSRAM_OCD_DQSN, n)

// REG_012
#define ANA0_REG_PSRAM_TXDRVP_SHIFT         0
#define ANA0_REG_PSRAM_TXDRVP_MASK          (0x1F << ANA0_REG_PSRAM_TXDRVP_SHIFT)
#define ANA0_REG_PSRAM_TXDRVP(n)            BITFIELD_VAL(ANA0_REG_PSRAM_TXDRVP, n)
#define ANA0_REG_PSRAM_TXDRVN_SHIFT         5
#define ANA0_REG_PSRAM_TXDRVN_MASK          (0x1F << ANA0_REG_PSRAM_TXDRVN_SHIFT)
#define ANA0_REG_PSRAM_TXDRVN(n)            BITFIELD_VAL(ANA0_REG_PSRAM_TXDRVN, n)

// REG_013
#define ANA0_REG_PSRAM_TX_RD_POINTER_PSEL   (1 << 0)
#define ANA0_REG_PSRAM_DQS_CK_DLY_SHIFT     1
#define ANA0_REG_PSRAM_DQS_CK_DLY_MASK      (0x7 << ANA0_REG_PSRAM_DQS_CK_DLY_SHIFT)
#define ANA0_REG_PSRAM_DQS_CK_DLY(n)        BITFIELD_VAL(ANA0_REG_PSRAM_DQS_CK_DLY, n)
#define ANA0_REG_PSRAM_PAD_LOOPBACK         (1 << 4)
#define ANA0_REG_PSRAM_DLL_RANGE_SHIFT      5
#define ANA0_REG_PSRAM_DLL_RANGE_MASK       (0x3 << ANA0_REG_PSRAM_DLL_RANGE_SHIFT)
#define ANA0_REG_PSRAM_DLL_RANGE(n)         BITFIELD_VAL(ANA0_REG_PSRAM_DLL_RANGE, n)
#define ANA0_REG_PSRAM_TX_FIFO_TEST_EN      (1 << 7)
#define ANA0_REG_PSRAM_TX_FIFO_BYPASS       (1 << 8)
#define ANA0_REG_PSRAM_RX_FIFO_BYPASS       (1 << 9)
#define ANA0_REG_PSRAM_TX_RD_CK_DLY_SHIFT   10
#define ANA0_REG_PSRAM_TX_RD_CK_DLY_MASK    (0x7 << ANA0_REG_PSRAM_TX_RD_CK_DLY_SHIFT)
#define ANA0_REG_PSRAM_TX_RD_CK_DLY(n)      BITFIELD_VAL(ANA0_REG_PSRAM_TX_RD_CK_DLY, n)
#define ANA0_REG_PSRAM_TX_RD_POINTER_DLY_SHIFT 13
#define ANA0_REG_PSRAM_TX_RD_POINTER_DLY_MASK (0x7 << ANA0_REG_PSRAM_TX_RD_POINTER_DLY_SHIFT)
#define ANA0_REG_PSRAM_TX_RD_POINTER_DLY(n) BITFIELD_VAL(ANA0_REG_PSRAM_TX_RD_POINTER_DLY, n)

// REG_014
#define ANA0_REG_PSRAM_RESERVE0_SHIFT       0
#define ANA0_REG_PSRAM_RESERVE0_MASK        (0xFFFF << ANA0_REG_PSRAM_RESERVE0_SHIFT)
#define ANA0_REG_PSRAM_RESERVE0(n)          BITFIELD_VAL(ANA0_REG_PSRAM_RESERVE0, n)

// REG_015
#define ANA0_REG_PSRAM_RESERVE1_SHIFT       0
#define ANA0_REG_PSRAM_RESERVE1_MASK        (0xFFFF << ANA0_REG_PSRAM_RESERVE1_SHIFT)
#define ANA0_REG_PSRAM_RESERVE1(n)          BITFIELD_VAL(ANA0_REG_PSRAM_RESERVE1, n)

// REG_016
#define ANA0_REG_PSRAM_RESERVE2_SHIFT       0
#define ANA0_REG_PSRAM_RESERVE2_MASK        (0xFFFF << ANA0_REG_PSRAM_RESERVE2_SHIFT)
#define ANA0_REG_PSRAM_RESERVE2(n)          BITFIELD_VAL(ANA0_REG_PSRAM_RESERVE2, n)

// REG_017
#define ANA0_REG_PSRAM_RESERVE3_SHIFT       0
#define ANA0_REG_PSRAM_RESERVE3_MASK        (0xFFFF << ANA0_REG_PSRAM_RESERVE3_SHIFT)
#define ANA0_REG_PSRAM_RESERVE3(n)          BITFIELD_VAL(ANA0_REG_PSRAM_RESERVE3, n)

// REG_060
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)

#endif
