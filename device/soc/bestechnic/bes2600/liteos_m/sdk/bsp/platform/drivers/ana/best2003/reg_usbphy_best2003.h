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
#ifndef __REG_USBPHY_BEST2003_H__
#define __REG_USBPHY_BEST2003_H__

#include "plat_types.h"

// REG_00
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_01
#define CFG_ANAPHY_RESETN                   (1 << 0)
#define CFG_CKCDR_EN                        (1 << 1)
#define CFG_CKPLL_EN                        (1 << 2)
#define CFG_EN_CLKMAC                       (1 << 3)
#define CFG_POL_CDRCLK                      (1 << 4)
#define CFG_RESETN_MAC                      (1 << 5)
#define CFG_RESETNCDR                       (1 << 6)
#define CFG_RESETNPLL                       (1 << 7)
#define CFG_BYPASS_CDR_ENDRESETN            (1 << 8)
#define CFG_INTR_EN_ALL                     (1 << 9)
#define CFG_LOW_SPEED_MODE                  (1 << 10)
#define CFG_POL_CLKPLL                      (1 << 11)
#define CFG_BYPASS_RDY                      (1 << 12)
#define CFG_MODE_HS_LINES_TX                (1 << 13)
#define CFG_MODE_FS_LINES_TX                (1 << 14)
#define CFG_ENUM_MODE                       (1 << 15)

// REG_02
#define CFG_TARGET_SYNC_TOUT_SHIFT          0
#define CFG_TARGET_SYNC_TOUT_MASK           (0xF << CFG_TARGET_SYNC_TOUT_SHIFT)
#define CFG_TARGET_SYNC_TOUT(n)             BITFIELD_VAL(CFG_TARGET_SYNC_TOUT, n)
#define CFG_BYPASS_SQL_VALID                (1 << 6)
#define CFG_EN_SYNCTOUT                     (1 << 7)
#define CFG_FORCERX                         (1 << 8)
#define CFG_RXRESET                         (1 << 9)
#define CFG_EBUF_THRD_SHIFT                 10
#define CFG_EBUF_THRD_MASK                  (0x1F << CFG_EBUF_THRD_SHIFT)
#define CFG_EBUF_THRD(n)                    BITFIELD_VAL(CFG_EBUF_THRD, n)
#define CFG_EOP_MODE_RX                     (1 << 15)

// REG_03
#define CFG_TARGET_TAIL_SHIFT               0
#define CFG_TARGET_TAIL_MASK                (0xFF << CFG_TARGET_TAIL_SHIFT)
#define CFG_TARGET_TAIL(n)                  BITFIELD_VAL(CFG_TARGET_TAIL, n)
#define CFG_TXPATTERN_SHIFT                 8
#define CFG_TXPATTERN_MASK                  (0xFF << CFG_TXPATTERN_SHIFT)
#define CFG_TXPATTERN(n)                    BITFIELD_VAL(CFG_TXPATTERN, n)

// REG_04
#define CFG_EN_HSTSOF                       (1 << 0)
#define CFG_FORCETX                         (1 << 1)
#define CFG_MODE_BITEN                      (1 << 2)
#define CFG_TXSTATE_RESET                   (1 << 3)
#define CFG_EMPTY_DLY_SEL_SHIFT             4
#define CFG_EMPTY_DLY_SEL_MASK              (0x7 << CFG_EMPTY_DLY_SEL_SHIFT)
#define CFG_EMPTY_DLY_SEL(n)                BITFIELD_VAL(CFG_EMPTY_DLY_SEL, n)
#define RESERVED_0                          (1 << 7)
#define CFG_TP_SEL_SHIFT                    8
#define CFG_TP_SEL_MASK                     (0xFF << CFG_TP_SEL_SHIFT)
#define CFG_TP_SEL(n)                       BITFIELD_VAL(CFG_TP_SEL, n)

// REG_05
#define CFG_ADP_PRB                         (1 << 0)
#define CFG_ADP_SNS                         (1 << 1)
#define CFG_RID_A                           (1 << 2)
#define CFG_RID_B                           (1 << 3)
#define CFG_RID_C                           (1 << 4)
#define CFG_RID_FLOAT                       (1 << 5)
#define CFG_RID_GND                         (1 << 6)
#define CFG_UTMIOTG_AVALID                  (1 << 7)
#define CFG_UTMISRP_BVALID                  (1 << 8)
#define CFG_UTMIOTG_VBUSVALID               (1 << 9)
#define CFG_UTMISRP_SESSEND                 (1 << 10)
#define CFG_SEL_SYNCPATTERN_SHIFT           11
#define CFG_SEL_SYNCPATTERN_MASK            (0x7 << CFG_SEL_SYNCPATTERN_SHIFT)
#define CFG_SEL_SYNCPATTERN(n)              BITFIELD_VAL(CFG_SEL_SYNCPATTERN, n)

// REG_06
#define CFG_HS_SRC_SEL                      (1 << 0)
#define CFG_HS_TX_EN                        (1 << 1)
#define CFG_CHIRP_EN                        (1 << 2)
#define CFG_HS_MODE                         (1 << 3)
#define CFG_EN_RHS                          (1 << 4)
#define CFG_HS_DRV_SEL_SHIFT                5
#define CFG_HS_DRV_SEL_MASK                 (0xF << CFG_HS_DRV_SEL_SHIFT)
#define CFG_HS_DRV_SEL(n)                   BITFIELD_VAL(CFG_HS_DRV_SEL, n)
#define CFG_RHS_TRIM_SHIFT                  9
#define CFG_RHS_TRIM_MASK                   (0xF << CFG_RHS_TRIM_SHIFT)
#define CFG_RHS_TRIM(n)                     BITFIELD_VAL(CFG_RHS_TRIM, n)
#define CFG_HS_RCV_PD                       (1 << 13)
#define CFG_EN_HOLD_LAST                    (1 << 14)
#define CFG_EN_HS_S2P                       (1 << 15)

// REG_07
#define CFG_RST_INTP                        (1 << 0)
#define CFG_EN_ZPS                          (1 << 1)
#define CFG_RES_GN_SHIFT                    2
#define CFG_RES_GN_MASK                     (0xF << CFG_RES_GN_SHIFT)
#define CFG_RES_GN(n)                       BITFIELD_VAL(CFG_RES_GN, n)
#define CFG_ISEL_RCV_SHIFT                  6
#define CFG_ISEL_RCV_MASK                   (0x7 << CFG_ISEL_RCV_SHIFT)
#define CFG_ISEL_RCV(n)                     BITFIELD_VAL(CFG_ISEL_RCV, n)
#define CFG_ISEL_SQ_SHIFT                   9
#define CFG_ISEL_SQ_MASK                    (0x7 << CFG_ISEL_SQ_SHIFT)
#define CFG_ISEL_SQ(n)                      BITFIELD_VAL(CFG_ISEL_SQ, n)
#define CFG_INT_FLT_SHIFT                   12
#define CFG_INT_FLT_MASK                    (0x3 << CFG_INT_FLT_SHIFT)
#define CFG_INT_FLT(n)                      BITFIELD_VAL(CFG_INT_FLT, n)
#define CFG_PI_GN_SHIFT                     14
#define CFG_PI_GN_MASK                      (0x3 << CFG_PI_GN_SHIFT)
#define CFG_PI_GN(n)                        BITFIELD_VAL(CFG_PI_GN, n)

// REG_08
#define CFG_CDR_GN_SHIFT                    0
#define CFG_CDR_GN_MASK                     (0x3 << CFG_CDR_GN_SHIFT)
#define CFG_CDR_GN(n)                       BITFIELD_VAL(CFG_CDR_GN, n)
#define CFG_SEL_HS_GAIN                     (1 << 2)
#define CFG_SEL_TERR                        (1 << 3)
#define CFG_SEL_HS_LSTATE                   (1 << 4)
#define CFG_LOOP_BACK                       (1 << 5)
#define CFG_XCVR_SELECT                     (1 << 6)
#define CFG_XTERM_SELECT                    (1 << 7)
#define CFG_USPENDM                         (1 << 8)
#define CFG_DISCON_DET_EN                   (1 << 9)
#define CFG_DISCON_VTHSET_SHIFT             10
#define CFG_DISCON_VTHSET_MASK              (0x7 << CFG_DISCON_VTHSET_SHIFT)
#define CFG_DISCON_VTHSET(n)                BITFIELD_VAL(CFG_DISCON_VTHSET, n)
#define CFG_FS_TX_ON                        (1 << 13)
#define CFG_FS_EN_RFS                       (1 << 14)
#define CFG_FS_EN_R15K                      (1 << 15)

// REG_09
#define CFG_LS_EN_RLS                       (1 << 0)
#define CFG_LS_MODE                         (1 << 1)
#define CFG_ATEST_EN_REG                    (1 << 2)
#define CFG_ATEST_SELX_REG_SHIFT            3
#define CFG_ATEST_SELX_REG_MASK             (0x3 << CFG_ATEST_SELX_REG_SHIFT)
#define CFG_ATEST_SELX_REG(n)               BITFIELD_VAL(CFG_ATEST_SELX_REG, n)
#define CFG_DTESTEN1_REG                    (1 << 5)
#define CFG_DTESTEN2_REG                    (1 << 6)
#define CFG_DTEST_SEL_REG_SHIFT             7
#define CFG_DTEST_SEL_REG_MASK              (0x3 << CFG_DTEST_SEL_REG_SHIFT)
#define CFG_DTEST_SEL_REG(n)                BITFIELD_VAL(CFG_DTEST_SEL_REG, n)
#define CFG_ATEST_EN_DISCON_REG             (1 << 9)
#define CFG_ATEST_SELX_DISCON_REG           (1 << 10)
#define CFG_MODE_LINESTATE                  (1 << 11)
#define RESERVED_8_6_SHIFT                  12
#define RESERVED_8_6_MASK                   (0x7 << RESERVED_8_6_SHIFT)
#define RESERVED_8_6(n)                     BITFIELD_VAL(RESERVED_8_6, n)
#define CFG_BISTEN                          (1 << 15)

// REG_0A
#define CFG_DR_HOSTMODE                     (1 << 0)
#define CFG_DR_FSLS_SEL                     (1 << 1)
#define CFG_DR_SDATA                        (1 << 2)
#define CFG_DR_TERMSEL                      (1 << 3)
#define CFG_DR_XCVRSEL                      (1 << 4)
#define CFG_DR_SON                          (1 << 5)
#define CFG_DR_OPMODE                       (1 << 6)
#define RESERVED_9                          (1 << 7)
#define CFG_DR_HS_TXEN                      (1 << 8)
#define CFG_DR_FSTXEN                       (1 << 9)
#define CFG_DR_FSTXD                        (1 << 10)
#define CFG_DR_FSTXDB                       (1 << 11)
#define CFG_FS_LS_SEL                       (1 << 12)
#define CFG_CLK480M_EDGE_SEL                (1 << 13)
#define CFG_FS_DM_DP_BYPASS                 (1 << 14)

// REG_0B
#define CFG_REG_HOSTMODE                    (1 << 0)
#define CFG_REG_FSLS_SEL                    (1 << 1)
#define CFG_REG_SDATA                       (1 << 2)
#define CFG_REG_TERM                        (1 << 3)
#define CFG_REG_XCVRSEL                     (1 << 4)
#define CFG_REG_SON                         (1 << 5)
#define CFG_REG_OPMODE_SHIFT                6
#define CFG_REG_OPMODE_MASK                 (0x3 << CFG_REG_OPMODE_SHIFT)
#define CFG_REG_OPMODE(n)                   BITFIELD_VAL(CFG_REG_OPMODE, n)
#define CFG_REG_HS_TXEN                     (1 << 8)
#define CFG_REG_FSTXEN                      (1 << 9)
#define CFG_REG_FSTXD                       (1 << 10)
#define CFG_REG_FSTXDB                      (1 << 11)
#define RESERVED_17_14_SHIFT                12
#define RESERVED_17_14_MASK                 (0xF << RESERVED_17_14_SHIFT)
#define RESERVED_17_14(n)                   BITFIELD_VAL(RESERVED_17_14, n)

// REG_0C
#define CFG_RXINTR_MSK_SHIFT                0
#define CFG_RXINTR_MSK_MASK                 (0xFFFF << CFG_RXINTR_MSK_SHIFT)
#define CFG_RXINTR_MSK(n)                   BITFIELD_VAL(CFG_RXINTR_MSK, n)

// REG_0D
#define CFG_HSHOST_DISC_MSK                 (1 << 0)
#define CFG_POL_CKSIE60                     (1 << 1)
#define CFG_RESETN_HSTXP                    (1 << 2)
#define CFG_CKSIE60_EN                      (1 << 3)
#define CFG_TXP_MODE                        (1 << 4)
#define CFG_CKOSC_EN                        (1 << 5)
#define CFG_POL_OSC                         (1 << 6)
#define CFG_CKPRE_EN                        (1 << 7)
#define CFG_RX_P_SEL                        (1 << 8)
#define CFG_RERESTN_HSRXP                   (1 << 9)
#define CFG_POL_RXP                         (1 << 10)
#define CFG_CKRXP_EN                        (1 << 11)
#define CFG_PEBUF_THRD_SHIFT                12
#define CFG_PEBUF_THRD_MASK                 (0xF << CFG_PEBUF_THRD_SHIFT)
#define CFG_PEBUF_THRD(n)                   BITFIELD_VAL(CFG_PEBUF_THRD, n)

// REG_0E
#define RB_RXERRSTATUS_RAW_SHIFT            0
#define RB_RXERRSTATUS_RAW_MASK             (0xFFFF << RB_RXERRSTATUS_RAW_SHIFT)
#define RB_RXERRSTATUS_RAW(n)               BITFIELD_VAL(RB_RXERRSTATUS_RAW, n)

// REG_0F
#define RB_RXERRSTATUS_MSKD_SHIFT           0
#define RB_RXERRSTATUS_MSKD_MASK            (0xFFFF << RB_RXERRSTATUS_MSKD_SHIFT)
#define RB_RXERRSTATUS_MSKD(n)              BITFIELD_VAL(RB_RXERRSTATUS_MSKD, n)

// REG_10
#define RB_HSHOST_DISC_RAW                  (1 << 0)

// REG_11
#define CFG_USB_3P3_PU                      (1 << 0)
#define CFG_USB_1P3_PU                      (1 << 1)
#define CFG_USB_LDO_PU                      (1 << 2)
#define CFG_USB_3P3_PU_DR                   (1 << 3)
#define CFG_USB_1P3_PU_DR                   (1 << 4)
#define CFG_USB_LDO_PU_DR                   (1 << 5)
#define CFG_USB_LDO_PRECHARGE_DR            (1 << 6)
#define CFG_USB_3P3_PU_REG                  (1 << 7)
#define CFG_USB_1P3_PU_REG                  (1 << 8)
#define CFG_USB_LDO_PU_REG                  (1 << 9)
#define CFG_USB_LDO_PRECHARGE_REG           (1 << 10)
#define POWER_UP_SOFT_CNT_SHIFT             12
#define POWER_UP_SOFT_CNT_MASK              (0xF << POWER_UP_SOFT_CNT_SHIFT)
#define POWER_UP_SOFT_CNT(n)                BITFIELD_VAL(POWER_UP_SOFT_CNT, n)

// REG_12
#define CFG_EN_SETERR_RX_SHIFT              0
#define CFG_EN_SETERR_RX_MASK               (0xFFFF << CFG_EN_SETERR_RX_SHIFT)
#define CFG_EN_SETERR_RX(n)                 BITFIELD_VAL(CFG_EN_SETERR_RX, n)

// REG_13
#define CFG_TXSTART_DLY_SEL_SHIFT           0
#define CFG_TXSTART_DLY_SEL_MASK            (0x7 << CFG_TXSTART_DLY_SEL_SHIFT)
#define CFG_TXSTART_DLY_SEL(n)              BITFIELD_VAL(CFG_TXSTART_DLY_SEL, n)

// REG_14
#define CFG_USB_RES_SHIFT                   0
#define CFG_USB_RES_MASK                    (0xF << CFG_USB_RES_SHIFT)
#define CFG_USB_RES(n)                      BITFIELD_VAL(CFG_USB_RES, n)
#define CFG_USB_IBIAS_SEL_SHIFT             4
#define CFG_USB_IBIAS_SEL_MASK              (0x7 << CFG_USB_IBIAS_SEL_SHIFT)
#define CFG_USB_IBIAS_SEL(n)                BITFIELD_VAL(CFG_USB_IBIAS_SEL, n)

// REG_16
#define USB_EMPHASIS_EN                     (1 << 0)
#define USB_EM_SEL_SHIFT                    1
#define USB_EM_SEL_MASK                     (0x7 << USB_EM_SEL_SHIFT)
#define USB_EM_SEL(n)                       BITFIELD_VAL(USB_EM_SEL, n)
#define USB_DCC_BPS                         (1 << 4)
#define USB_DCC_SEL_SHIFT                   5
#define USB_DCC_SEL_MASK                    (0xF << USB_DCC_SEL_SHIFT)
#define USB_DCC_SEL(n)                      BITFIELD_VAL(USB_DCC_SEL, n)
#define USB_CK60M_EN                        (1 << 9)
#define USB_CK480M_EN                       (1 << 10)
#define USB_CKIN4_EN                        (1 << 11)
#define USB_FS_RCV_PD                       (1 << 12)
#define USB_HS_TX_PD                        (1 << 13)

// REG_17
#define USB_FS_VREF_H_SHIFT                 0
#define USB_FS_VREF_H_MASK                  (0x7 << USB_FS_VREF_H_SHIFT)
#define USB_FS_VREF_H(n)                    BITFIELD_VAL(USB_FS_VREF_H, n)
#define USB_FS_VREF_L_SHIFT                 3
#define USB_FS_VREF_L_MASK                  (0x7 << USB_FS_VREF_L_SHIFT)
#define USB_FS_VREF_L(n)                    BITFIELD_VAL(USB_FS_VREF_L, n)

// REG_18
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)

// REG_19
#define USB_SOF_INTR                        (1 << 0)

// REG_1A
#define HS_TEST                             (1 << 0)
#define FS_TEST                             (1 << 1)
#define USBINSERT_DET_EN                    (1 << 2)
#define USBINSERT_INTR_EN                   (1 << 3)
#define INTR_CLR                            (1 << 4)
#define POL_USB_RX_DP                       (1 << 5)
#define POL_USB_RX_DM                       (1 << 6)
#define DEBOUNCE_EN                         (1 << 7)
#define NOLS_MODE                           (1 << 8)
#define USB_INSERT_INTR_MSK                 (1 << 9)
#define PHY_TEST_GOON                       (1 << 10)

// REG_1B
#define CFG_HS_PDATA_SHIFT                  0
#define CFG_HS_PDATA_MASK                   (0xFF << CFG_HS_PDATA_SHIFT)
#define CFG_HS_PDATA(n)                     BITFIELD_VAL(CFG_HS_PDATA, n)
#define CFG_HS_P_ON_SHIFT                   8
#define CFG_HS_P_ON_MASK                    (0xFF << CFG_HS_P_ON_SHIFT)
#define CFG_HS_P_ON(n)                      BITFIELD_VAL(CFG_HS_P_ON, n)

// REG_1C
#define DEBUG_DATA_CLR                      (1 << 0)
#define DEBUG_ERR_CLR                       (1 << 1)
#define USB_HS_PS_CLR                       (1 << 2)

// REG_20
#define RB_BISTERR_CAUSE_SHIFT              0
#define RB_BISTERR_CAUSE_MASK               (0x7 << RB_BISTERR_CAUSE_SHIFT)
#define RB_BISTERR_CAUSE(n)                 BITFIELD_VAL(RB_BISTERR_CAUSE, n)

// REG_21
#define RB_BIST_FAIL                        (1 << 0)
#define RB_BIST_DONE                        (1 << 1)
#define RB_CHIRP_ON                         (1 << 2)
#define RB_ADP_SNS_EN                       (1 << 3)
#define RB_ADP_PRB_EN                       (1 << 4)
#define RB_ADP_DISCHRG                      (1 << 5)
#define RB_ADP_CHRG                         (1 << 6)
#define RB_UTMISRP_DISCHRGVBUS              (1 << 7)
#define RB_UTMISRP_CHRGVBUS                 (1 << 8)
#define RB_UTMIOTG_IDPULLUP                 (1 << 9)
#define RB_UTMIOTG_DPPULLDOWN               (1 << 10)
#define RB_UTMIOTG_DMPULLDOWN               (1 << 11)
#define RB_UTMIOTG_DRVVBUS                  (1 << 12)

// REG_22
#define CFG_RESETNTX                        (1 << 0)
#define CFG_RESETNRX                        (1 << 1)
#define CFG_CKTX_EN                         (1 << 2)
#define CFG_CKRX_EN                         (1 << 3)
#define CFG_FORCE_TXCK                      (1 << 4)
#define CFG_FORCE_RXCK                      (1 << 5)
#define CFG_FORCE_CDRCK                     (1 << 6)
#define CFG_MODE_S_ON                       (1 << 7)
#define CFG_RESETNFRE                       (1 << 8)
#define CFG_RESETNOSC                       (1 << 9)
#define CFG_DIG_LOOP                        (1 << 10)
#define CFG_FORCE_CDRPCK                    (1 << 11)
#define CFG_DIG_SLOOP                       (1 << 12)

// REG_25
#define USB_STATUS_RX_DP                    (1 << 0)
#define USB_STATUS_RX_DM                    (1 << 1)

// REG_30
#define REV_REG30_SHIFT                     0
#define REV_REG30_MASK                      (0xFFFF << REV_REG30_SHIFT)
#define REV_REG30(n)                        BITFIELD_VAL(REV_REG30, n)

// REG_31
#define REV_REG31_SHIFT                     0
#define REV_REG31_MASK                      (0xFFFF << REV_REG31_SHIFT)
#define REV_REG31(n)                        BITFIELD_VAL(REV_REG31, n)

// REG_32
#define REV_REG32_SHIFT                     0
#define REV_REG32_MASK                      (0xFFFF << REV_REG32_SHIFT)
#define REV_REG32(n)                        BITFIELD_VAL(REV_REG32, n)

// REG_33
#define CFG_ANA_REV_SHIFT                   0
#define CFG_ANA_REV_MASK                    (0xFFFF << CFG_ANA_REV_SHIFT)
#define CFG_ANA_REV(n)                      BITFIELD_VAL(CFG_ANA_REV, n)

// REG_40
#define USB_SOF__NUM_SHIFT                  0
#define USB_SOF__NUM_MASK                   (0xFFFF << USB_SOF__NUM_SHIFT)
#define USB_SOF__NUM(n)                     BITFIELD_VAL(USB_SOF__NUM, n)

// REG_41
#define USB_SOF_DETECT_EN                   (1 << 0)
#define USB_SOF_INTR_MASK                   (1 << 1)
#define USB_SOF_INTR_CLR                    (1 << 2)
#define USB_SOF_FRM_NUM_CLR                 (1 << 3)
#define USB_SOF_TOGGLE_EN                   (1 << 4)
#define USB_SOF_HALT                        (1 << 5)

// REG_42
#define USB_SOF_START_NUM_SHIFT             0
#define USB_SOF_START_NUM_MASK              (0x3FFF << USB_SOF_START_NUM_SHIFT)
#define USB_SOF_START_NUM(n)                BITFIELD_VAL(USB_SOF_START_NUM, n)

// REG_43
#define USB_SOF_END_NUM_SHIFT               0
#define USB_SOF_END_NUM_MASK                (0x3FFF << USB_SOF_END_NUM_SHIFT)
#define USB_SOF_END_NUM(n)                  BITFIELD_VAL(USB_SOF_END_NUM, n)

// REG_44
#define USB_SOF_CAL_COUNT_15_0_SHIFT        0
#define USB_SOF_CAL_COUNT_15_0_MASK         (0xFFFF << USB_SOF_CAL_COUNT_15_0_SHIFT)
#define USB_SOF_CAL_COUNT_15_0(n)           BITFIELD_VAL(USB_SOF_CAL_COUNT_15_0, n)

// REG_45
#define USB_SOF_CAL_COUNT_27_16_SHIFT       0
#define USB_SOF_CAL_COUNT_27_16_MASK        (0xFFF << USB_SOF_CAL_COUNT_27_16_SHIFT)
#define USB_SOF_CAL_COUNT_27_16(n)          BITFIELD_VAL(USB_SOF_CAL_COUNT_27_16, n)

enum USBPHY_REG_T {
    USBPHY_REG_00   = 0x00,
    USBPHY_REG_01,
    USBPHY_REG_02,
    USBPHY_REG_03,
    USBPHY_REG_04,
    USBPHY_REG_05,
    USBPHY_REG_06,
    USBPHY_REG_07,
    USBPHY_REG_08,
    USBPHY_REG_09,
    USBPHY_REG_0A,
    USBPHY_REG_0B,
    USBPHY_REG_0C,
    USBPHY_REG_0D,
    USBPHY_REG_0E,
    USBPHY_REG_0F,
    USBPHY_REG_10,
    USBPHY_REG_11,
    USBPHY_REG_12,
    USBPHY_REG_13,
    USBPHY_REG_14,
    USBPHY_REG_15,
    USBPHY_REG_16,
    USBPHY_REG_17,
    USBPHY_REG_18,
    USBPHY_REG_19,
    USBPHY_REG_1A,
    USBPHY_REG_1B,

    USBPHY_REG_20   = 0x20,
    USBPHY_REG_21,
    USBPHY_REG_22,
    USBPHY_REG_23,
    USBPHY_REG_24,
    USBPHY_REG_25,

    USBPHY_REG_30   = 0x30,
    USBPHY_REG_31,
    USBPHY_REG_32,
    USBPHY_REG_33,
    USBPHY_REG_34,
    USBPHY_REG_35,
    USBPHY_REG_36,
    USBPHY_REG_37,
    USBPHY_REG_38,
    USBPHY_REG_39,
    USBPHY_REG_3A,

    USBPHY_REG_41   = 0x41,
    USBPHY_REG_42,
    USBPHY_REG_43,
    USBPHY_REG_44,
    USBPHY_REG_45,
};

#endif
