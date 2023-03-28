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
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "cmsis_nvic.h"
#include "pmu.h"
#include "analog.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_chipid.h"
#include "hal_location.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_bootmode.h"
#include "tgt_hardware.h"
#include "usbphy.h"
#include "psramuhsphy.h"
#include "wifi_rf_test_def.h"
#include "mipiphy.h"
#include CHIP_SPECIFIC_HDR(reg_pmu)

#include "hal_gpadc.h"

#define PMU_LED_VIA_PWM
#define DCDC_ULP_LP_ON

#ifdef PMU_IRQ_UNIFIED
#define PMU_IRQ_HDLR_PARAM              uint16_t irq_status
#else
#define PMU_IRQ_HDLR_PARAM              void
#endif

static unsigned short flash_16m_page0_bit[] = {0x00, 0x10, 0x30, 0xc0, 0xf0, 0x100, 0x110, 0x130, 0x1c0, 0x1f0}; //write page0:0x900b
static unsigned short flash_32m_page0_bit[] = {0x00, 0x10, 0x30, 0x70, 0xf0, 0x100, 0x110, 0x130, 0x170, 0x1f0}; //write page0:0x808b

// LDO soft start interval is about 1000 us
#define PMU_LDO_PU_STABLE_TIME_US       1800
#define PMU_DCDC_PU_STABLE_TIME_US      200 //100
#define PMU_VANA_STABLE_TIME_US         100 //10
#define PMU_VCORE_STABLE_TIME_US        100 //10

#ifdef __PMU_VIO_DYNAMIC_CTRL_MODE__
#define IO_VOLT_ACTIVE_NORMAL           PMU_IO_3_3V
#else
#define IO_VOLT_ACTIVE_NORMAL           PMU_IO_3_3V
#endif

#ifdef DIGMIC_HIGH_VOLT
#define IO_VOLT_ACTIVE_RISE             PMU_IO_3_3V
#else
#define IO_VOLT_ACTIVE_RISE             PMU_IO_3_3V
#endif
#define IO_VOLT_SLEEP                   PMU_IO_3_3V

#define PMU_DCDC_ANA_2_1V               0xC8
#define PMU_DCDC_ANA_2_0V               0xBB
#define PMU_DCDC_ANA_1_9V               0xAD
#define PMU_DCDC_ANA_1_8V               0xA0
#define PMU_DCDC_ANA_1_7V               0x93
#define PMU_DCDC_ANA_1_6V               0x85
#define PMU_DCDC_ANA_1_5V               0x78
#define PMU_DCDC_ANA_1_4V               0x6C
#define PMU_DCDC_ANA_1_35V              0x64
#define PMU_DCDC_ANA_1_3V               0x5D
#define PMU_DCDC_ANA_1_2V               0x50
#define PMU_DCDC_ANA_1_1V               0x43
#define PMU_DCDC_ANA_1_0V               0x35

#define PMU_DCDC_DIG_1_2V               0x50
#define PMU_DCDC_DIG_1_15V              0x49
#define PMU_DCDC_DIG_1_1V               0x43
#define PMU_DCDC_DIG_1_05V              0x3C
#define PMU_DCDC_DIG_1_0V               0x35
#define PMU_DCDC_DIG_0_95V              0x2F
#define PMU_DCDC_DIG_0_9V               0x28
#define PMU_DCDC_DIG_0_85V              0x21
#define PMU_DCDC_DIG_0_8V               0x1B
#define PMU_DCDC_DIG_0_75V              0x14
#define PMU_DCDC_DIG_0_7V               0x0D
#define PMU_DCDC_DIG_0_65V              0x07
#define PMU_DCDC_DIG_0_6V               0x00

#define PMU_DCDC_HPPA_2_1V              0xC8
#define PMU_DCDC_HPPA_2_0V              0xBB
#define PMU_DCDC_HPPA_1_95V             0xB4
#define PMU_DCDC_HPPA_1_9V              0xAD
#define PMU_DCDC_HPPA_1_8V              0xA0
#define PMU_DCDC_HPPA_1_7V              0x93
#define PMU_DCDC_HPPA_1_6V              0x85
#define PMU_DCDC_HPPA_1_5V              0x78
#define PMU_DCDC_HPPA_1_4V              0x6B
#define PMU_DCDC_HPPA_1_3V              0x5D
#define PMU_DCDC_HPPA_1_2V              0x50

#define PMU_VDIG_1_175V                 0x1F
#define PMU_VDIG_1_15V                  0x1E
#define PMU_VDIG_1_1V                   0x1C
#define PMU_VDIG_1_05V                  0x1A
#define PMU_VDIG_1_0V                   0x17
#define PMU_VDIG_0_95V                  0x16
#define PMU_VDIG_0_9V                   0x14
#define PMU_VDIG_0_85V                  0x12
#define PMU_VDIG_0_8V                   0x10
#define PMU_VDIG_0_75V                  0x0E
#define PMU_VDIG_0_7V                   0x0C
#define PMU_VDIG_0_65V                  0x0A
#define PMU_VDIG_0_6V                   0x08
#define PMU_VDIG_0_55V                  0x06
#define PMU_VDIG_0_5V                   0x04
#define PMU_VDIG_0_45V                  0x02
#define PMU_VDIG_0_4V                   0x00
#define PMU_VDIG_MAX                    PMU_VDIG_1_175V

#define PMU_VANA_1_4V                   0x02

#define PMU_IO_3_3V                     0x15
#define PMU_IO_3_2V                     0x14
#define PMU_IO_3_1V                     0x13
#define PMU_IO_3_0V                     0x12
#define PMU_IO_2_9V                     0x11
#define PMU_IO_2_8V                     0x10
#define PMU_IO_2_7V                     0xF
#define PMU_IO_2_6V                     0xE
#define PMU_IO_2_5V                     0xD
#define PMU_IO_2_4V                     0xC
#define PMU_IO_2_3V                     0xB
#define PMU_IO_2_2V                     0xA
#define PMU_IO_2_1V                     0x9
#define PMU_IO_2_0V                     0x8
#define PMU_IO_1_9V                     0x7
#define PMU_IO_1_8V                     0x6
#define PMU_IO_1_7V                     0x5

#define PMU_CODEC_2_7V                  0xF
#define PMU_CODEC_2_6V                  0xE
#define PMU_CODEC_2_5V                  0xD
#define PMU_CODEC_2_4V                  0xC
#define PMU_CODEC_2_3V                  0xB
#define PMU_CODEC_2_2V                  0xA
#define PMU_CODEC_2_1V                  0x9
#define PMU_CODEC_2_0V                  0x8
#define PMU_CODEC_1_9V                  0x7
#define PMU_CODEC_1_8V                  0x6
#define PMU_CODEC_1_7V                  0x5
#define PMU_CODEC_1_6V                  0x4
#define PMU_CODEC_1_5V                  0x3

#define PMU_VMIC_3_0V                   0x37
#define PMU_VMIC_2_9V                   0x34
#define PMU_VMIC_2_8V                   0x32
#define PMU_VMIC_2_7V                   0x2F
#define PMU_VMIC_2_6V                   0x2D
#define PMU_VMIC_2_5V                   0x2A
#define PMU_VMIC_2_4V                   0x28
#define PMU_VMIC_2_3V                   0x25
#define PMU_VMIC_2_2V                   0x23
#define PMU_VMIC_2_1V                   0x20
#define PMU_VMIC_2_0V                   0x1E
#define PMU_VMIC_1_9V                   0x1B
#define PMU_VMIC_1_8V                   0x19
#define PMU_VMIC_1_7V                   0x16
#define PMU_VMIC_1_6V                   0x14
#define PMU_VMIC_1_5V                   0x11
#define PMU_VMIC_1_4V                   0xF
#define PMU_VMIC_1_3V                   0xC

#define PMU_VMIC_RES_3_3V               0xF
#define PMU_VMIC_RES_2_8V               0xA
#define PMU_VMIC_RES_2_5V               0x7
#define PMU_VMIC_RES_2_2V               0x4

#define PMU_VRF_1_5V                    0xa
#define PMU_VRF_1_45V                   0x9
#define PMU_VRF_1_4V                    0x8
#define PMU_VRF_1_35V                   0x7
#define PMU_VRF_1_3V                    0x6
#define PMU_VRF_1_25V                   0x5
#define PMU_VRF_1_2V                    0x4
#define PMU_VRF_1_15V                   0x3
#define PMU_VRF_1_1V                    0x2
#define PMU_VRF_1_05V                   0x1
#define PMU_VRF_1_0V                    0x0

enum PMU_REG_T {
    PMU_REG_METAL_ID            = 0x00,
    PMU_REG_POWER_KEY_CFG       = 0x02,
    PMU_REG_BIAS_CFG            = 0x03,
    PMU_REG_CHARGER_CFG         = 0x05,
    PMU_REG_ANA_CFG             = 0x07,
    PMU_REG_DIG_CFG             = 0x08,
    PMU_REG_IO_CFG              = 0x09,
    PMU_REG_PWRUP_MOD           = 0x0A,
    PMU_REG_CODEC_CFG           = 0x0E,
    PMU_REG_DCDC_ANA_EN         = 0x14,
    PMU_REG_DCDC_ANA_VOLT       = 0x15,
    PMU_REG_PWR_SEL             = 0x1A,
    PMU_REG_SLEEP_CFG           = 0x1D,
    PMU_REG_INT_MASK            = 0x1F,
    PMU_REG_INT_EN              = 0x20,

    PMU_REG_RTC_DIV_1HZ         = 0x22,
    PMU_REG_RTC_LOAD_LOW        = 0x23,
    PMU_REG_RTC_LOAD_HIGH       = 0x24,
    PMU_REG_RTC_MATCH1_LOW      = 0x27,
    PMU_REG_RTC_MATCH1_HIGH     = 0x28,
    PMU_REG_MIC_BIAS_A          = 0x29,
    PMU_REG_MIC_BIAS_B          = 0x2A,
    PMU_REG_LED_CFG_IO1         = 0x2B,
    PMU_REG_PWM2_TOGGLE         = 0x2D,
    PMU_REG_PWM2_ST1            = 0x2E,
    PMU_REG_PWM2_EN             = 0x2F,
    PMU_REG_PWM2_BR_EN          = 0x30,
    PMU_REG_WDT_RESET_TIMER     = 0x36,
    PMU_REG_WDT_CFG             = 0x37,
    PMU_REG_PWMB_TOGGLE         = 0x3A,
    PMU_REG_PWMB_ST1            = 0x3B,
    PMU_REG_PWMB_EN             = 0x3C,
    PMU_REG_PWMB_BR_EN          = 0x3D,
    PMU_REG_LED_CFG_IO2         = 0x3E,
    PMU_REG_INT_MERGE_MASK1     = 0x40,
    PMU_REG_INT_MERGE_MASK2     = 0x41,
    PMU_REG_WDT_INT_CFG         = 0x43,
    PMU_REG_BUCK_CFG1           = 0x44,
    PMU_REG_LED_INTR_SEL        = 0x45,
    PMU_REG_UART1_CFG           = 0x46,
    PMU_REG_UART2_CFG           = 0x47,
    PMU_REG_LED_IO_IN           = 0x48,
    PMU_REG_DCDC_ANA_CFG        = 0x49,
    //PMW_REG_DIG_BUCK_VOLT       = 0x4A,
    PMU_REG_WDT_IRQ_TIMER       = 0x4D,
    PMU_REG_POWER_OFF           = 0x4F,
    PMU_REG_INT_STATUS          = 0x50,
    PMU_REG_INT_MSKED_STATUS    = 0x51,
    PMU_REG_INT_CLR             = 0x51,
    PMU_REG_POWER_ON            = 0x52,
    PMU_REG_RTC_VAL_LOW         = 0x54,
    PMU_REG_RTC_VAL_HIGH        = 0x55,
    PMU_REG_CHARGER_STATUS      = 0x5E,
    PMU_REG_USB_PIN_STATUS      = 0x5F,
    PMU_REG_DCDC_DIG_EN         = 0x65,
    PMU_REG_DCDC_DIG_VOLT       = 0x66,
    PMU_REG_RF_CFG              = 0x68,
    PMU_REG_LDO_VPA1_VOLT       = 0x69,
    PMU_REG_LDO_VPA2_VOLT       = 0x6A,

    PMU_REG_EFUSE_DATA_HIGH     = 0xBD,
    PMU_REG_EFUSE_DATA_LOW      = 0xBE,
    PMU_REG_EFUSE_CTRL          = 0xB7,

    PMU_REG_DCDC_HPPA_EN        = 0xBF,

    PMU_REG_DCDC_HPPA_VOLT      = 0xC0,
    PMU_REG_DCDC_HPPA_CFG       = 0xC1,
    PMU_REG_DCDC_HPPA_CFG1      = 0xC4,
    PMU_REG_DCDC_DIG_CFG        = 0xC5,

    PMU_REG_MODULE_START        = PMU_REG_ANA_CFG,
};

enum ANA_REG_T {
    ANA_REG_00                  = 0x00,
};

enum RF_REG_T {
    RF_REG_00                   = 0x00,
    RF_REG_9A                   = 0x9A,
};

enum WFRF_REG_T {
    WFRF_REG_00                 = 0x00,
    WFRF_REG_1F                 = 0x1F,
    WFRF_REG_18                 = 0x18,
    WFRF_REG_21                 = 0x21,
    WFRF_REG_4F                 = 0x4F,
    WFRF_REG_51                 = 0x51,
    WFRF_REG_52                 = 0x52,
    WFRF_REG_56                 = 0x56,
    WFRF_REG_92                 = 0x92,
    WFRF_REG_BD                 = 0xBD,
    WFRF_REG_BE                 = 0xBE,
    WFRF_REG_BF                 = 0xBF,
};
// WIFI REG_18
#define REG_WIFI_2G4_BG_SEL_TEMP_SHIFT      0
#define REG_WIFI_2G4_BG_SEL_TEMP_MASK       (0x7 << REG_WIFI_2G4_BG_SEL_TEMP_SHIFT)
#define REG_WIFI_2G4_BG_SEL_TEMP(n)         BITFIELD_VAL(REG_WIFI_2G4_BG_SEL_TEMP, n)
#define REG_WIFI_2G4_BG_SEL_IBIAS_SHIFT     3
#define REG_WIFI_2G4_BG_SEL_IBIAS_MASK      (0x7 << REG_WIFI_2G4_BG_SEL_IBIAS_SHIFT)
#define REG_WIFI_2G4_BG_SEL_IBIAS(n)        BITFIELD_VAL(REG_WIFI_2G4_BG_SEL_IBIAS, n)
#define REG_WIFI_2G4_BG_SEL_VOUT_SHIFT      6
#define REG_WIFI_2G4_BG_SEL_VOUT_MASK       (0x3 << REG_WIFI_2G4_BG_SEL_VOUT_SHIFT)
#define REG_WIFI_2G4_BG_SEL_VOUT(n)         BITFIELD_VAL(REG_WIFI_2G4_BG_SEL_VOUT, n)
#define REG_WIFI_2G4_BG_BYPASS              (1 << 8)
#define REG_WIFI_IQCAL_2G4_I2V_RIN_SEL_SHIFT 9
#define REG_WIFI_IQCAL_2G4_I2V_RIN_SEL_MASK (0x3 << REG_WIFI_IQCAL_2G4_I2V_RIN_SEL_SHIFT)
#define REG_WIFI_IQCAL_2G4_I2V_RIN_SEL(n)   BITFIELD_VAL(REG_WIFI_IQCAL_2G4_I2V_RIN_SEL, n)
#define REG_WIFI_IQCAL_2G4_S2_VCM_SEL_SHIFT 11
#define REG_WIFI_IQCAL_2G4_S2_VCM_SEL_MASK  (0x3 << REG_WIFI_IQCAL_2G4_S2_VCM_SEL_SHIFT)
#define REG_WIFI_IQCAL_2G4_S2_VCM_SEL(n)    BITFIELD_VAL(REG_WIFI_IQCAL_2G4_S2_VCM_SEL, n)

// WIFI REG_4F
#define REG_WIFI_BBPLL_LDO_VRES_SHIFT       0
#define REG_WIFI_BBPLL_LDO_VRES_MASK        (0x7 << REG_WIFI_BBPLL_LDO_VRES_SHIFT)
#define REG_WIFI_BBPLL_LDO_VRES(n)          BITFIELD_VAL(REG_WIFI_BBPLL_LDO_VRES, n)
#define REG_WIFI_BBPLL_ICP_SHIFT            3
#define REG_WIFI_BBPLL_ICP_MASK             (0x7 << REG_WIFI_BBPLL_ICP_SHIFT)
#define REG_WIFI_BBPLL_ICP(n)               BITFIELD_VAL(REG_WIFI_BBPLL_ICP, n)
#define REG_WIFI_BBPLL_VCO_CAL_VT_SHIFT     6
#define REG_WIFI_BBPLL_VCO_CAL_VT_MASK      (0x3 << REG_WIFI_BBPLL_VCO_CAL_VT_SHIFT)
#define REG_WIFI_BBPLL_VCO_CAL_VT(n)        BITFIELD_VAL(REG_WIFI_BBPLL_VCO_CAL_VT, n)
#define REG_WIFI_BBPLL_VCO_ICTL_SHIFT       8
#define REG_WIFI_BBPLL_VCO_ICTL_MASK        (0x7 << REG_WIFI_BBPLL_VCO_ICTL_SHIFT)
#define REG_WIFI_BBPLL_VCO_ICTL(n)          BITFIELD_VAL(REG_WIFI_BBPLL_VCO_ICTL, n)
#define REG_WIFI_BBPLL_VRES_VCO_SHIFT       11
#define REG_WIFI_BBPLL_VRES_VCO_MASK        (0x3 << REG_WIFI_BBPLL_VRES_VCO_SHIFT)
#define REG_WIFI_BBPLL_VRES_VCO(n)          BITFIELD_VAL(REG_WIFI_BBPLL_VRES_VCO, n)
#define REG_WIFI_BBPLL_VRES_POSTDIV_SHIFT   13
#define REG_WIFI_BBPLL_VRES_POSTDIV_MASK    (0x3 << REG_WIFI_BBPLL_VRES_POSTDIV_SHIFT)
#define REG_WIFI_BBPLL_VRES_POSTDIV(n)      BITFIELD_VAL(REG_WIFI_BBPLL_VRES_POSTDIV, n)

// WIFI REG_52
#define REG_WIFI_BBPLL_POSTDIV_MCU_SHIFT    0
#define REG_WIFI_BBPLL_POSTDIV_MCU_MASK     (0x7 << REG_WIFI_BBPLL_POSTDIV_MCU_SHIFT)
#define REG_WIFI_BBPLL_POSTDIV_MCU(n)       BITFIELD_VAL(REG_WIFI_BBPLL_POSTDIV_MCU, n)
#define REG_WIFI_BBPLL_DIVN_MAC_SHIFT       3
#define REG_WIFI_BBPLL_DIVN_MAC_MASK        (0x3 << REG_WIFI_BBPLL_DIVN_MAC_SHIFT)
#define REG_WIFI_BBPLL_DIVN_MAC(n)          BITFIELD_VAL(REG_WIFI_BBPLL_DIVN_MAC, n)
#define REG_WIFI_BBPLL_POSTDIV_PSRAM_SHIFT  5
#define REG_WIFI_BBPLL_POSTDIV_PSRAM_MASK   (0x1F << REG_WIFI_BBPLL_POSTDIV_PSRAM_SHIFT)
#define REG_WIFI_BBPLL_POSTDIV_PSRAM(n)     BITFIELD_VAL(REG_WIFI_BBPLL_POSTDIV_PSRAM, n)
#define REG_WIFI_BBPLL_DIVN_AD_SHIFT        10
#define REG_WIFI_BBPLL_DIVN_AD_MASK         (0x7 << REG_WIFI_BBPLL_DIVN_AD_SHIFT)
#define REG_WIFI_BBPLL_DIVN_AD(n)           BITFIELD_VAL(REG_WIFI_BBPLL_DIVN_AD, n)
#define REG_WIFI_BBPLL_DIVN_DA_SHIFT        13
#define REG_WIFI_BBPLL_DIVN_DA_MASK         (0x7 << REG_WIFI_BBPLL_DIVN_DA_SHIFT)
#define REG_WIFI_BBPLL_DIVN_DA(n)           BITFIELD_VAL(REG_WIFI_BBPLL_DIVN_DA, n)

// WIFI REG_51
#define REG_WIFI_BBPLL_TEMP_ITUNE_SHIFT     0
#define REG_WIFI_BBPLL_TEMP_ITUNE_MASK      (0x3 << REG_WIFI_BBPLL_TEMP_ITUNE_SHIFT)
#define REG_WIFI_BBPLL_TEMP_ITUNE(n)        BITFIELD_VAL(REG_WIFI_BBPLL_TEMP_ITUNE, n)
#define REG_WIFI_BBPLL_CP_BYPASS_R          (1 << 2)
#define REG_WIFI_BBPLL_CP_MODE              (1 << 3)
#define REG_WIFI_BBPLL_PFD_RSTB_SEL         (1 << 4)
#define REG_WIFI_BBPLL_REFCLK_EDGE_SW       (1 << 5)
#define REG_WIFI_BBPLL_REFCLK_SEL           (1 << 6)
#define REG_WIFI_BBPLL_EN_TEMP              (1 << 7)
#define REG_WIFI_BBPLL_PFD_RSTB             (1 << 8)
#define REG_WIFI_BBPLL_LPF_PRECHARGE_VT_SHIFT 9
#define REG_WIFI_BBPLL_LPF_PRECHARGE_VT_MASK (0x3 << REG_WIFI_BBPLL_LPF_PRECHARGE_VT_SHIFT)
#define REG_WIFI_BBPLL_LPF_PRECHARGE_VT(n)  BITFIELD_VAL(REG_WIFI_BBPLL_LPF_PRECHARGE_VT, n)
#define REG_WIFI_BBPLL_VRES_DIV_SHIFT       11
#define REG_WIFI_BBPLL_VRES_DIV_MASK        (0x3 << REG_WIFI_BBPLL_VRES_DIV_SHIFT)
#define REG_WIFI_BBPLL_VRES_DIV(n)          BITFIELD_VAL(REG_WIFI_BBPLL_VRES_DIV, n)
#define REG_WIFI_BBPLL_USB_EN               (1 << 13)

// RF REG_09A
#define REG_RFPLL_SDM_DIVN_OUT_48M_24M_SEL  (1 << 0)
#define REG_BBPLL_DIVN_DDRCLK_SHIFT         1
#define REG_BBPLL_DIVN_DDRCLK_MASK          (0x1F << REG_BBPLL_DIVN_DDRCLK_SHIFT)
#define REG_BBPLL_DIVN_DDRCLK(n)            BITFIELD_VAL(REG_BBPLL_DIVN_DDRCLK, n)
#define REG_BT_RXVCO_DIVN_ADCLK_SHIFT       6
#define REG_BT_RXVCO_DIVN_ADCLK_MASK        (0x1F << REG_BT_RXVCO_DIVN_ADCLK_SHIFT)
#define REG_BT_RXVCO_DIVN_ADCLK(n)          BITFIELD_VAL(REG_BT_RXVCO_DIVN_ADCLK, n)
#define REG_BT_RXVCO_PDT_EN_DR              (1 << 11)
#define REG_BT_RXVCO_PDT_EN                 (1 << 12)


// MIPI REG_019
#define REG_MIPI_BBPLL_EN_DSICLK            (1 << 0)
#define REG_MIPI_BBPLL_EN_PSRAMCLK          (1 << 1)
#define REG_MIPI_BBPLL_EN_MCUCLK            (1 << 2)
#define REG_MIPI_BBPLL_DIVN_DSI_SHIFT       3
#define REG_MIPI_BBPLL_DIVN_DSI_MASK        (0x1F << REG_MIPI_BBPLL_DIVN_DSI_SHIFT)
#define REG_MIPI_BBPLL_DIVN_DSI(n)          BITFIELD_VAL(REG_MIPI_BBPLL_DIVN_DSI, n)
#define REG_MIPI_BBPLL_VRES_TST_SHIFT       8
#define REG_MIPI_BBPLL_VRES_TST_MASK        (0xF << REG_MIPI_BBPLL_VRES_TST_SHIFT)
#define REG_MIPI_BBPLL_VRES_TST(n)          BITFIELD_VAL(REG_MIPI_BBPLL_VRES_TST, n)

// MIPI REG_01A
#define REG_MIPI_BBPLL_DIVN_PSRAM_SHIFT     0
#define REG_MIPI_BBPLL_DIVN_PSRAM_MASK      (0x1F << REG_MIPI_BBPLL_DIVN_PSRAM_SHIFT)
#define REG_MIPI_BBPLL_DIVN_PSRAM(n)        BITFIELD_VAL(REG_MIPI_BBPLL_DIVN_PSRAM, n)
#define REG_MIPI_BBPLL_DIVN_MCU_SHIFT       5
#define REG_MIPI_BBPLL_DIVN_MCU_MASK        (0x1F << REG_MIPI_BBPLL_DIVN_MCU_SHIFT)
#define REG_MIPI_BBPLL_DIVN_MCU(n)          BITFIELD_VAL(REG_MIPI_BBPLL_DIVN_MCU, n)
#define REG_MIPI_BBPLL_EN_PSRAMCLK_DR       (1 << 10)
#define REG_MIPI_BBPLL_EN_MCUCLK_DR         (1 << 11)
#define REG_MIPI_BBPLL_PU_DR                (1 << 12)
enum PMU_VCORE_REQ_T {
    PMU_VCORE_FLASH_WRITE_ENABLED   = (1 << 0),
    PMU_VCORE_FLASH_FREQ_HIGH       = (1 << 1),
    PMU_VCORE_FLASH_FREQ_MEDIUM     = (1 << 2),
    PMU_VCORE_USB_HS_ENABLED        = (1 << 3),
    PMU_VCORE_IIR_FREQ_MEDIUM       = (1 << 4),
    PMU_VCORE_IIR_FREQ_HIGH         = (1 << 5),
    PMU_VCORE_SYS_FREQ_MEDIUM       = (1 << 6),
    PMU_VCORE_SYS_FREQ_HIGH         = (1 << 7),
    PMU_VCORE_SYS_FREQ_ULTRAHIGH    = (1 << 8),
};

enum PMU_MODUAL_T {
    PMU_ANA,
    PMU_DIG,
    PMU_IO,
    PMU_CODEC,
    PMU_RF,
};

struct PMU_MODULE_CFG_T {
    unsigned short manual_bit;
    unsigned short ldo_en;
    unsigned short lp_mode;
    unsigned short dsleep_mode;
    unsigned short dsleep_v;
    unsigned short dsleep_v_shift;
    unsigned short normal_v;
    unsigned short normal_v_shift;
};

#define PMU_MOD_CFG_VAL(m)              { \
    REG_PU_LDO_V##m##_DR, REG_PU_LDO_V##m, \
    LP_EN_V##m##_LDO_DSLEEP, REG_PU_LDO_V##m##_DSLEEP, \
    LDO_V##m##_VBIT_DSLEEP_MASK, LDO_V##m##_VBIT_DSLEEP_SHIFT,\
    LDO_V##m##_VBIT_NORMAL_MASK, LDO_V##m##_VBIT_NORMAL_SHIFT }

BOOT_RODATA_SRAM_LOC  static const struct PMU_MODULE_CFG_T pmu_module_cfg[] = {
    PMU_MOD_CFG_VAL(ANA),
    PMU_MOD_CFG_VAL(DIG),
    PMU_MOD_CFG_VAL(IO),
    PMU_MOD_CFG_VAL(CODEC),
    PMU_MOD_CFG_VAL(RF),
};

#define OPT_TYPE                        const

BOOT_RODATA_SRAM_LOC static OPT_TYPE bool vcodec_off =
#ifdef VCODEC_OFF
    true;
#else
    false;
#endif
BOOT_RODATA_SRAM_LOC static OPT_TYPE uint8_t ana_act_dcdc =
#ifdef VANA_1P2V
    PMU_DCDC_ANA_1_2V;
#elif defined(VANA_1P35V)
    PMU_DCDC_ANA_1_35V;
#elif defined(VANA_1P4V)
    PMU_DCDC_ANA_1_4V;
#elif defined(VANA_1P5V)
    PMU_DCDC_ANA_1_5V;
#elif defined(VANA_1P6V)
    PMU_DCDC_ANA_1_6V;
#elif defined(VANA_1P7V)
    PMU_DCDC_ANA_1_7;
#elif defined(VANA_1P8V)
    PMU_DCDC_ANA_1_8V;
#else
    PMU_DCDC_ANA_1_3V;
#endif
static OPT_TYPE POSSIBLY_UNUSED uint16_t vcodec_mv = (uint16_t)(VCODEC_VOLT * 1000);
static OPT_TYPE POSSIBLY_UNUSED uint16_t vhppa_mv = (uint16_t)(VHPPA_VOLT * 1000);

static enum PMU_POWER_MODE_T BOOT_DATA_LOC pmu_power_mode = PMU_POWER_MODE_NONE;
static enum PMU_POWER_MODE_T POSSIBLY_UNUSED pmu_active_power_mode = PMU_POWER_MODE_NONE;

static enum PMU_VCORE_REQ_T BOOT_BSS_LOC pmu_vcore_req;

#define DCDC_RAMP_MAP_VCORE         (1<<0)
#define DCDC_RAMP_MAP_VANA          (1<<1)
#define DCDC_RAMP_MAP_VHPPA         (1<<2)
static uint8_t BOOT_BSS_LOC dcdc_ramp_map;

static PMU_CHARGER_IRQ_HANDLER_T charger_irq_handler;

#ifdef PMU_IRQ_UNIFIED
static bool gpio_irq_en[2];
static HAL_GPIO_PIN_IRQ_HANDLER gpio_irq_handler[2];

static PMU_WDT_IRQ_HANDLER_T BOOT_BSS_LOC wdt_irq_handler;

static PMU_IRQ_UNIFIED_HANDLER_T BOOT_BSS_LOC pmu_irq_hdlrs[PMU_IRQ_TYPE_QTY];
#endif

static uint8_t SRAM_BSS_DEF(vio_risereq_map);
STATIC_ASSERT(sizeof(vio_risereq_map) * 8 >= PMU_VIORISE_REQ_USER_QTY, "vio_risereq_map size too small");

static uint8_t SRAM_DATA_DEF(vio_act_normal) = IO_VOLT_ACTIVE_NORMAL;
static uint8_t SRAM_DATA_DEF(vio_act_rise) = IO_VOLT_ACTIVE_RISE;
static uint8_t SRAM_DATA_DEF(vio_lp) = IO_VOLT_SLEEP;

#ifdef PSRAMUHS_ENABLE
static const uint8_t ana_lp_dcdc = PMU_DCDC_ANA_1_3V;
#else
static const uint8_t ana_lp_dcdc = PMU_DCDC_ANA_1_3V;
#endif

// Move all the data/bss invovled in pmu_open() to .sram_data/.sram_bss,
// so that pmu_open() can be called at the end of BootInit(),
// for data/bss is initialized after BootInit().
static const uint8_t dig_lp_ldo = PMU_VDIG_0_6V;
#ifdef PSRAMUHS_ENABLE
static const uint8_t dig_lp_dcdc = PMU_DCDC_DIG_0_6V;
#else
static const uint8_t dig_lp_dcdc = PMU_DCDC_DIG_0_6V;
#endif

static uint8_t BOOT_DATA_LOC bbpll_dig_div = 5;
static uint8_t BOOT_DATA_LOC bbpll_psram_div = 0;
static uint8_t BOOT_DATA_LOC dsipll_dig_div = 8;
static uint8_t BOOT_DATA_LOC dsipll_psram_div = 8;
static uint8_t BOOT_DATA_LOC dsipll_dsi_div = 8;

static uint32_t BOOT_BSS_LOC pmu_metal_id;

static uint16_t BOOT_BSS_LOC wdt_irq_timer;
static uint16_t BOOT_BSS_LOC wdt_reset_timer;

static uint8_t BOOT_BSS_LOC vcore_high_volt;
#if defined(_AUTO_TEST_)
static bool at_skip_shutdown = false;

void pmu_at_skip_shutdown(bool enable)
{
    at_skip_shutdown = enable;
}
#endif

#ifdef RTC_ENABLE
struct PMU_RTC_CTX_T {
    bool enabled;
    bool alarm_set;
    uint32_t alarm_val;
};

static struct PMU_RTC_CTX_T BOOT_BSS_LOC rtc_ctx;

static PMU_RTC_IRQ_HANDLER_T rtc_irq_handler;

static void BOOT_TEXT_SRAM_LOC pmu_rtc_save_context(void)
{
    if (pmu_rtc_enabled()) {
        rtc_ctx.enabled = true;
        if (pmu_rtc_alarm_status_set()) {
            rtc_ctx.alarm_set = true;
            rtc_ctx.alarm_val = pmu_rtc_get_alarm();
        }
    } else {
        rtc_ctx.enabled = false;
    }
}

static void pmu_rtc_restore_context(void)
{
    uint32_t rtc_val;

    if (rtc_ctx.enabled) {
        pmu_rtc_enable();
        if (rtc_ctx.alarm_set) {
            rtc_val = pmu_rtc_get();
            if (rtc_val - rtc_ctx.alarm_val <= 1 || rtc_ctx.alarm_val - rtc_val < 5) {
                rtc_ctx.alarm_val = rtc_val + 5;
            }
            pmu_rtc_set_alarm(rtc_ctx.alarm_val);
        }
    }
}
#endif

static unsigned int NOINLINE BOOT_TEXT_SRAM_LOC pmu_count_zeros(unsigned int val, unsigned int bits)
{
    int cnt = 0;
    int i;

    for (i = 0; i < bits; i++) {
        if ((val & (1 << i)) == 0) {
            cnt++;
        }
    }

    return cnt;
}

#ifdef PMU_DCDC_CALIB
union VOLT_COMP_T {
    struct VOLT_COMP_FIELD_T {
        uint16_t dcdc1_v: 5; //bit[4:0]: 0 ~ 31
        uint16_t dcdc1_f: 1; //bit[5]  : 1: negative, 0: positive;
        uint16_t dcdc2_v: 4; //bit[9:6]: 0 ~ 15
        uint16_t dcdc2_f: 1; //bit[10] :
        uint16_t dcdc3_v: 4; //bit[14:11]: 0 ~ 15
        uint16_t dcdc3_f: 1; //bit[15]
    } f;
    uint16_t v;
};

static int8_t pmu_dcdc_dig_comp = 0;
static int8_t pmu_dcdc_ana_comp = 0;
static int8_t pmu_dcdc_hppa_comp = 0;

static void POSSIBLY_UNUSED pmu_get_dcdc_calib_value(void)
{
    union VOLT_COMP_T cv;

    pmu_get_efuse(PMU_EFUSE_PAGE_DCDC_CALI, &cv.v);
    if (cv.f.dcdc1_f) { //digital
        pmu_dcdc_dig_comp = -(int8_t)(cv.f.dcdc1_v);
    } else {
        pmu_dcdc_dig_comp = (int8_t)(cv.f.dcdc1_v);
    }
    if (cv.f.dcdc2_f) { //ana
        pmu_dcdc_ana_comp = -(int8_t)(cv.f.dcdc2_v);
    } else {
        pmu_dcdc_ana_comp = (int8_t)(cv.f.dcdc2_v);
    }
    if (cv.f.dcdc3_f) { //hppa
        pmu_dcdc_hppa_comp = -(int8_t)(cv.f.dcdc3_v);
    } else {
        pmu_dcdc_hppa_comp = (int8_t)(cv.f.dcdc3_v);
    }
}


static POSSIBLY_UNUSED unsigned short pmu_reg_val_add(unsigned short val, int delta, unsigned short max)
{
    int result = val + delta;

    if (result > max) {
        result = max;
    } else if (result < 0) {
        result = 0;
    }

    return (unsigned short)result;
}

#endif

uint32_t BOOT_TEXT_FLASH_LOC read_hw_metal_id(void)
{
    int ret = 0;
    uint16_t val;
    uint32_t metal_id;
    union BOOT_SETTINGS_T boot;

#ifdef RTC_ENABLE
    // RTC will be restored in pmu_open()
    pmu_rtc_save_context();
#endif

#if !defined(FPGA) && !defined(PROGRAMMER)
    pmu_charger_save_context();
#endif

    // Reset PMU (to recover from a possible insane state, e.g., ESD reset)
    if ((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_WARM_BOOT) == 0) {
        pmu_write(PMU_REG_METAL_ID, 0xCAFE);
        pmu_write(PMU_REG_METAL_ID, 0x5FEE);
        hal_sys_timer_delay(US_TO_TICKS(500));

#if !defined(SIMU) && (defined(PMU_FULL_INIT) && (!defined(FPGA) && !defined(PROGRAMMER)))
        // Reset WFRF
        wfrf_write(WFRF_REG_00, 0xCAFE);
        wfrf_write(WFRF_REG_00, 0x5FEE);
        hal_sys_timer_delay(US_TO_TICKS(500));

        // Reset RF
        rf_write(RF_REG_00, 0xCAFE);
        rf_write(RF_REG_00, 0x5FEE);
        hal_sys_timer_delay(US_TO_TICKS(500));

        // Reset ANA
        ana_write(ANA_REG_00, 0xCAFE);
        ana_write(ANA_REG_00, 0x5FEE);
        hal_sys_timer_delay(US_TO_TICKS(500));
#endif
	//dcdc config
        pmu_write(PMU_REG_DCDC_DIG_EN, 0x13c0);
        pmu_write(0xc6, 0xcf10);
        pmu_write(0xc7, 0x6ad2);
        pmu_write(0xc8, 0x1f6);

        pmu_write(PMU_REG_DCDC_ANA_EN, 0x1380);
        pmu_write(0x4a, 0xcf10);
        pmu_write(0x64, 0x1f6);

        pmu_write(PMU_REG_DCDC_HPPA_EN, 0x1380);
        pmu_write(0xc2, 0xcf10);
        pmu_write(0xc4, 0x1f6);
    }

    pmu_read(0x1d, &val);
    val &= ~(0xFF);
    pmu_write(0x1d, val);
    pmu_read(0x4f, &val);
    val &= ~(3 << 4);
    pmu_write(0x4f, val);

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &val);
    if (ret) {
        val = 0;
    }
    if ((val & (1 << 11)) == 0) {
        // Default to set largest address space for flash0
        // The actual address space can be changed later
#if defined(CHIP_HAS_EXT_NORFLASH)
#if FLASH_TYPE == 0
        hal_cmu_set_flash0_size(HAL_FLASH0_SIZE_16M);
#else
        hal_cmu_set_flash0_size(HAL_FLASH0_SIZE_32M);
#endif
#else
        hal_cmu_set_flash0_size(HAL_FLASH0_SIZE_64M);
#endif
    }

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_BOOT, &boot.reg);
    if (ret) {
        boot.reg = 0;
    }

    if ((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_WARM_BOOT) == 0) {
        //non-sec app don't cali bbpll
        // init pll config
        if (boot.crystal_freq == 0x0) {         // 24M
            wfrf_write(WFRF_REG_21, 0x4088);
            wfrf_write(WFRF_REG_BD, 0x0400);
            wfrf_write(WFRF_REG_51, 0x1D19);
        } else if (boot.crystal_freq == 0x1) {  // 40M
            wfrf_write(WFRF_REG_21, 0x4088);
            wfrf_write(WFRF_REG_BD, 0x0400);
            wfrf_write(WFRF_REG_51, 0x1D19);
        } else if (boot.crystal_freq == 0x2) {  // 48M
            wfrf_write(WFRF_REG_21, 0xC088);
            wfrf_write(WFRF_REG_BD, 0x0400);
            wfrf_write(WFRF_REG_51, 0x1D59);
        } else {                                // 80M
            wfrf_write(WFRF_REG_21, 0xC088);
            wfrf_write(WFRF_REG_BD, 0x0400);
            wfrf_write(WFRF_REG_51, 0x1D59);
        }
    }

#ifndef SIMU
    hal_sys_timer_delay(MS_TO_TICKS(10));
#endif

    boot.crystal_freq &= 0x1;
    hal_cmu_set_crystal_freq_index(boot.crystal_freq);
    // Update ISPI cfg
    ret = hal_analogif_open();
    if (ret) {
        SAFE_PROGRAM_STOP();
    }

    metal_id = hal_cmu_get_aon_revision_id();
    pmu_metal_id = metal_id;

    if ((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_WARM_BOOT) == 0) {
        //BBPLL run at 1920MHz in 2003 WIFI chip
        wfrf_read(WFRF_REG_18, &val);
        val = SET_BITFIELD(val, REG_WIFI_2G4_BG_SEL_IBIAS, 6);
        wfrf_write(WFRF_REG_18, val);
        wfrf_read(WFRF_REG_4F, &val);
        if (metal_id == HAL_CHIP_METAL_ID_7) {
            val = SET_BITFIELD(val, REG_WIFI_BBPLL_VCO_ICTL, 7);
            val = SET_BITFIELD(val, REG_WIFI_BBPLL_VRES_VCO, 3);
        } else {
            val = SET_BITFIELD(val, REG_WIFI_BBPLL_VCO_ICTL, 6);
        }
        val = SET_BITFIELD(val, REG_WIFI_BBPLL_VRES_POSTDIV, 0x2);
        wfrf_write(WFRF_REG_4F, val);
        if (boot.crystal_freq == 0) {//24M
            wfrf_write(WFRF_REG_BF, 0xA000);
            wfrf_write(WFRF_REG_BE, 0x0000);
        } else {//40M
            wfrf_write(WFRF_REG_BF, 0x6000);
            wfrf_write(WFRF_REG_BE, 0x0000);
        }
        //xtal_cmom: 0x80
        wfrf_write(WFRF_REG_1F, 0x201A);

    }
    pmu_temperature_sensor_init();

#if !defined(ARM_CMSE) && !defined(BUILD_BOOT1) && !defined(BUILD_BOOT2B) && !defined(BUILD_BOOT2A)
    //don't cali bbpll in boot1/2
#if WIFI_BBPL_TEMPTR_CALI_V2
    //bbpl tempreture comp. 20210517 HanCC
    bbpl_temprt_comp_init_2003();
    bbpl_compensate_by_temprt_2003();
#endif
#endif

#if defined(MCU_HIGH_PERFORMANCE_MODE_320)
    pmu_pll_div_set(HAL_CMU_PLL_BB, PMU_PLL_DIV_DIG, 6);
#endif

#if (PSRAM_SPEED == 240) // 240
    pmu_pll_div_set(HAL_CMU_PLL_BB, PMU_PLL_DIV_PSRAM, 4);
#elif (PSRAM_SPEED == 200) // 192
    pmu_pll_div_set(HAL_CMU_PLL_BB, PMU_PLL_DIV_PSRAM, 5);
#elif (PSRAM_SPEED == 166) // 160
    pmu_pll_div_set(HAL_CMU_PLL_BB, PMU_PLL_DIV_PSRAM, 6);
#elif (PSRAM_SPEED == 133) // 137
    pmu_pll_div_set(HAL_CMU_PLL_BB, PMU_PLL_DIV_PSRAM, 7);
#elif (PSRAM_SPEED == 104) // 96
    pmu_pll_div_set(HAL_CMU_PLL_BB, PMU_PLL_DIV_PSRAM, 4);
#elif (PSRAM_SPEED == 52) // 48
    pmu_pll_div_set(HAL_CMU_PLL_BB, PMU_PLL_DIV_PSRAM, 4);
#else
#error "invalid PSRAM_SPEED"
#endif

    //DDR PLL
    pmu_write(0x12, 0x0f87);//reg_pmu_lobuf1_pu_ldo_dr=1
    pmu_write(0x13, 0x0f87);//reg_pmu_lobuf2_pu_ldo_dr=1
    rf_write(0xe6, 0x7523);
    rf_write(0xe7, 0xe0ca);
    rf_write(0xa5, 0x76);//reg_bt_bg_sel_ibias=0x6

#if (PSRAMUHS_SPEED == 200)
    //4800M, div6, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xa600);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7200);
    }
    rf_write(0x9a, 0x454);
#elif (PSRAMUHS_SPEED == 380)
    //4560M, div6, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xbe00);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7200);
    }
    rf_write(0x9a, 0x44c);//divn_ddrclk = 0x6, div=6
#elif (PSRAMUHS_SPEED == 400)
    //4800M, div6, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xc800);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7800);
    }
    rf_write(0x9a, 0x44c);//divn_ddrclk = 0x6, div=6
#elif (PSRAMUHS_SPEED == 533)
    //4264M, div4, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0xaaaa);
        rf_write(0xde, 0xb1aa);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x6a99);
    }
    rf_write(0x9a, 0x448);//divn_ddrclk = 0x4, div=4
#elif (PSRAMUHS_SPEED == 500)
    //4000M, div4, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0xaaaa);
        rf_write(0xde, 0xa6aa);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x6400);
    }
    rf_write(0x9a, 0x448);//divn_ddrclk = 0x4, div=4
#elif (PSRAMUHS_SPEED == 580)
    //4800M, div4, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x5555);
        rf_write(0xde, 0xC155);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7400);
    }
    rf_write(0x9a, 0x448);//divn_ddrclk = 0x4, div=4
#elif (PSRAMUHS_SPEED == 600)
    //4800M, div4, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xc800);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7800);
    }
    rf_write(0x9a, 0x448);//divn_ddrclk = 0x4, div=4
#elif (PSRAMUHS_SPEED == 650)
    //3900M, div4, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xa280);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x6180);
    }
    rf_write(0x9a, 0x448);//divn_ddrclk = 0x4, div=4
#elif (PSRAMUHS_SPEED == 700)
    //4200M, div4, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xaf00);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x6900);
    }
    rf_write(0x9a, 0x448);//divn_ddrclk = 0x4, div=4
#elif (PSRAMUHS_SPEED == 750)
    //4500M, div3, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xbb80);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7080);
    }
    rf_write(0x9a, 0x462);//divn_ddrclk = 0x11, div=3
#elif (PSRAMUHS_SPEED == 800)
    //4800M, div3, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xc800);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7800);
    }
    rf_write(0x9a, 0x462);//divn_ddrclk = 0x11, div=3
#elif (PSRAMUHS_SPEED == 900)
    //3600M, div2, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x9600);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x5a00);
    }
    rf_write(0x9a, 0x442);//divn_ddrclk = 0x2, div=2
#elif (PSRAMUHS_SPEED == 933)
    if (boot.crystal_freq == 0) {
        //3744M, div2, div2 = 936
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x9c00);
    } else {
        //3732M, div2, div2
        rf_write(0xdd, 0xcccc);
        rf_write(0xde, 0x5d4c);
    }
    rf_write(0x9a, 0x442);//divn_ddrclk = 0x2, div=2
#elif (PSRAMUHS_SPEED == 1000)
    //3984MHz, div2, div2 = 996MHz
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xa600);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x6400);
    }
    rf_write(0x9a, 0x442);//divn_ddrclk = 0x2, div=2
#elif (PSRAMUHS_SPEED == 1066)
    //4264M, div2, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0xaaaa);
        rf_write(0xde, 0xb1aa);
    } else {
        rf_write(0xdd, 0x9999);
        rf_write(0xde, 0x6a99);
    }
    rf_write(0x9a, 0x442);//divn_ddrclk = 0x2, div=2
#elif (PSRAMUHS_SPEED == 1100)
    //4400M, div2, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x5555);
        rf_write(0xde, 0xb755);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x6e00);
    }
    rf_write(0x9a, 0x442);//divn_ddrclk = 0x2, div=2
#elif (PSRAMUHS_SPEED == 1150)
    //4600M, div2, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0xaaaa);
        rf_write(0xde, 0xBFAA);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7300);
    }
    rf_write(0x9a, 0x442);//divn_ddrclk = 0x2, div=2
#elif (PSRAMUHS_SPEED == 1200)
    //4800M, div2, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0xc800);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7800);
    }
    rf_write(0x9a, 0x442);//divn_ddrclk = 0x2, div=2
#elif (PSRAMUHS_SPEED == 1250)
    //5000M, div2, div2
    if (boot.crystal_freq == 0) {
        rf_write(0xdd, 0x5555);
        rf_write(0xde, 0xd055);
    } else {
        rf_write(0xdd, 0x0);
        rf_write(0xde, 0x7d00);
    }
    rf_write(0x9a, 0x442);//divn_ddrclk = 0x2, div=2
#else
#error "invalid PSRAMUHS_SPEED"
#endif

    //DSP PLL
    pmu_write(0xcc, 0x1c35);
    pmu_write(0xd7, 0x7529);
    pmu_write(0xd8, 0x8e); //reg_a7pll_vres_div = 0x3
#if (A7_DSP_SPEED == 600)
    //4800MHz/8 = 600M
    if (boot.crystal_freq == 0) {
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0xc800);
    } else {
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0x7800);
    }
    pmu_write(0xd9, 0x1010);//divn = 0x8, div = 8
#elif (A7_DSP_SPEED == 780)
    //4680MHz/6 = 780M
    if (boot.crystal_freq == 0) {
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0xc300);
    } else {
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0x7500);
    }
    pmu_write(0xd9, 0xc10);//divn = 0x6, div = 6
#elif (A7_DSP_SPEED == 900)
    //3600MHz/4 = 900M
    if (boot.crystal_freq == 0) {
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0x9600);
    } else {
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0x5a00);
    }
    pmu_write(0xd9, 0x810);//divn = 0x4, div = 4
#elif (A7_DSP_SPEED == 966)
    //3864MHz/4 = 966M
    if (boot.crystal_freq == 0) {
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0xa100);
    } else {
        pmu_write(0xcf, 0x9999);
        pmu_write(0xd0, 0x6099);
    }
    pmu_write(0xd9, 0x810);//divn = 0x4, div = 4
#elif (A7_DSP_SPEED == 1000)
    if (boot.crystal_freq == 0) {
        //3984MHz/4 = 996M
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0xa600);
    } else {
        //4000MHz/4 = 1000M
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0x6400);
    }
    pmu_write(0xd9, 0x810);//divn = 0x4, div = 4
#elif (A7_DSP_SPEED == 1100)
    if (boot.crystal_freq == 0) {
        //4360MHz/4 = 1092M
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0xb600);
    } else {
        //4400MHz/4 = 1100M
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0x6e00);
    }
    pmu_write(0xd9, 0x810);//divn = 0x4, div = 4
#elif (A7_DSP_SPEED == 1200)
     if (boot.crystal_freq == 0) {
        //4800MHz/4 = 1200M
        pmu_write(0xcf, 0x0);
        pmu_write(0xd0, 0xc800);
     }
     pmu_write(0xd9, 0x810);//divn = 0x4, div = 4
#else
#error "invalid A7_DSP_SPEED"
#endif

    ret = hal_phyif_open(SPIPHY_MIPI_CS);
    if (ret) {
        SAFE_PROGRAM_STOP();
    }
    mipiphy_write(0x0d, 0xa020);
    mipiphy_write(0x16, 0x229b);
    mipiphy_write(0x17, 0x36dd);
    mipiphy_write(0x18, 0x1a7d);
    mipiphy_write(0x19, 0x0041);

#if 0 //200MHz, 400Mbps
    // 4800M
    if (hal_cmu_get_crystal_freq() == 24000000) {
        mipiphy_write(0x0F, 0x0);
        mipiphy_write(0x10, 0xc800);
    } else {
        mipiphy_write(0x0F, 0x0);
        mipiphy_write(0x10, 0x7800);
    }
    pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_DIG, 0x18); // div = 12
    pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_PSRAM, 0x4); // div = 4
    pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_DSI, 0x18); // div = 12
#else //250MHz, 500Mbps
    if (hal_cmu_get_crystal_freq() == 24000000) {
        // 4500M
        mipiphy_write(0x0F, 0x0);
        mipiphy_write(0x10, 0xbb80);
    } else {
        // 4500M
        mipiphy_write(0x0F, 0x0);
        mipiphy_write(0x10, 0x7080);
    }
    pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_DIG, 0x18); // div = 12
    pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_PSRAM, 0x4); // div = 4
    pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_DSI, 0x16); // div = 9
#endif

#ifndef SIMU
    hal_sys_timer_delay(MS_TO_TICKS(50));
#endif

    return metal_id;
}

void pmu_set_dsi_clk(uint32_t dsi_clk)
{
    // TODO: add more dsi_clk configurations
    if (dsi_clk == 200) {
        if (hal_cmu_get_crystal_freq() == 24000000) {
            mipiphy_write(0x0F, 0x0);
            mipiphy_write(0x10, 0xc800);
        } else {
            mipiphy_write(0x0F, 0x0);
            mipiphy_write(0x10, 0x7800);
        }
        pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_DSI, 0x18); // div = 12
    } else if (dsi_clk == 250) {
        if (hal_cmu_get_crystal_freq() == 24000000) {
            mipiphy_write(0x0F, 0x0);
            mipiphy_write(0x10, 0xbb80);
        } else {
            mipiphy_write(0x0F, 0x0);
            mipiphy_write(0x10, 0x7080);
        }
        pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_DSI, 0x16); // div = 9
    } else {
        TRACE(0, "invalid dsi_clk %u", dsi_clk);
        return;
    }
#ifndef SIMU
    hal_sys_timer_delay(MS_TO_TICKS(50));
#endif
}

void BOOT_TEXT_SRAM_LOC pmu_pll_div_reset_set(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_QTY) {
        // In fact usbhspll
    }
    int_unlock(lock);
}

void BOOT_TEXT_SRAM_LOC pmu_pll_div_reset_clear(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_QTY) {
        // In fact usbhspll
    }
    int_unlock(lock);
}

void BOOT_TEXT_SRAM_LOC pmu_pll_div_set(enum HAL_CMU_PLL_T pll, enum PMU_PLL_DIV_TYPE_T type, uint32_t div)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_BB) {
        if (type == PMU_PLL_DIV_DIG) {
            if (div != bbpll_dig_div) {
                bbpll_dig_div = div;
                wfrf_read(WFRF_REG_52, &val);
                val = SET_BITFIELD(val, REG_WIFI_BBPLL_POSTDIV_MCU, div);
                wfrf_write(WFRF_REG_52, val);
            }
        } else if (type == PMU_PLL_DIV_PSRAM) {
            if (div != bbpll_psram_div) {
                bbpll_psram_div = div;
                if (pmu_metal_id >= HAL_CHIP_METAL_ID_4) {
                    wfrf_read(0x207, &val);
                    val |= 0x2;
                    wfrf_write(0x207, val); //switch POSTDIV_PSRAM and DIV_DA
                    wfrf_read(WFRF_REG_52, &val);
                    val = SET_BITFIELD(val, REG_WIFI_BBPLL_DIVN_DA, div);
                    wfrf_write(WFRF_REG_52, val);
                } else {
                    wfrf_read(WFRF_REG_52, &val);
                    val = SET_BITFIELD(val, REG_WIFI_BBPLL_POSTDIV_PSRAM, div);
                    wfrf_write(WFRF_REG_52, val);
                }
            }
        }
    } else if (pll == HAL_CMU_PLL_DSI) {
        if (type == PMU_PLL_DIV_DIG) {
            if (div != dsipll_dig_div) {
                dsipll_dig_div = div;
                mipiphy_read(0x1A, &val);
                val = SET_BITFIELD(val, REG_MIPI_BBPLL_DIVN_MCU, div);
                mipiphy_write(0x1A, val);

            }
        } else if (type == PMU_PLL_DIV_PSRAM) {
            if (div != dsipll_psram_div) {
                dsipll_psram_div = div;
                mipiphy_read(0x1A, &val);
                val = SET_BITFIELD(val, REG_MIPI_BBPLL_DIVN_PSRAM, div);
                mipiphy_write(0x1A, val);

            }
        } else if (type == PMU_PLL_DIV_DSI) {
            if (div != dsipll_dsi_div) {
                dsipll_dsi_div = div;
                mipiphy_read(0x19, &val);
                val = SET_BITFIELD(val, REG_MIPI_BBPLL_DIVN_DSI, div);
                mipiphy_write(0x19, val);

            }
        }
    }
    int_unlock(lock);
}

int BOOT_TEXT_SRAM_LOC pmu_get_efuse(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
    int ret;
    unsigned short val;
    unsigned short tmp[2];

    // Enable CLK_EN
    val = REG_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Enable TURN_ON
    val |= REG_EFUSE_TURN_ON;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Write Address
#ifdef PMU_EFUSE_NO_REDUNDANCY
    val |= REG_EFUSE_ADDRESS(page / 2);
#else
    val |= REG_EFUSE_ADDRESS(page); //redundancy
#endif
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Set Strobe Trigger = 1
    val |= REG_EFUSE_STROBE_TRIGGER;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // set Strobe Trigger = 0
    val &= ~REG_EFUSE_STROBE_TRIGGER;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Read Efuse Low 16 bits
    ret = pmu_read(PMU_REG_EFUSE_DATA_LOW, &tmp[0]);
    if (ret) {
        goto _exit;
    }

    // Read Efuse High 16 bits
    ret = pmu_read(PMU_REG_EFUSE_DATA_HIGH, &tmp[1]);
    if (ret) {
        goto _exit;
    }
#ifdef PMU_EFUSE_NO_REDUNDANCY
    *efuse = tmp[page % 2];
#else
    *efuse = (tmp[0] | tmp[1]); //redundancy
#endif

_exit:
    // Disable TURN_ON
    val &= ~(REG_EFUSE_TURN_ON | REG_EFUSE_ADDRESS_MASK);
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    // Disable CLK_EN
    val &= ~REG_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    return ret;
}


int pmu_get_security_value(union SECURITY_VALUE_T *val)
{
    int ret;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &val->reg);
    if (ret) {
        // Error
        goto err;
    }

    if (!val->root.security_en) {
        // OK
        return 0;
    }

    if (pmu_count_zeros(val->reg, 12) != val->root.chksum) {
        // Error
        goto err;
    }

    // OK
    return 0;

err:
    val->reg = 0;
    return 1;
}

int pmu_set_security_value(enum FLASH_SIZE_TYPE_T flash_size)
{
    int ret = 0;
    unsigned short val = 0, val_tmp = 0;
    int i = 0, total = 0;

    switch (flash_size) {
        case FLASH_16M:
            total = sizeof(flash_16m_page0_bit) / sizeof(flash_16m_page0_bit[0]);
            break;

        case FLASH_32M:
            total = sizeof(flash_32m_page0_bit) / sizeof(flash_32m_page0_bit[0]);
            break;

        default:
            return -1;
    }

    // Enable CLK_EN
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Enable TURN_ON
    val |= REG_EFUSE_TURN_ON;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }
    val_tmp = val;

    // write programming_bit
    for (i = 0; i < total; i++) {
        val = val_tmp;

        // Write Address
        switch (flash_size) {
            case FLASH_16M:
                val |= REG_EFUSE_ADDRESS(flash_16m_page0_bit[i]);
                ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
                if (ret) {
                    goto _exit;
                }
                break;

            case FLASH_32M:
                val |= REG_EFUSE_ADDRESS(flash_32m_page0_bit[i]);
                ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
                if (ret) {
                    goto _exit;
                }
                break;

            default:
                return -1;
        }

        // Set Strobe Trigger = 1
        val |= REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        // set Strobe Trigger = 0
        val &= ~REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        hal_sys_timer_delay(MS_TO_TICKS(10));
    }

_exit:
    // Disable TURN_ON
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    // Disable CLK_EN
    val = 0;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    return ret;
}

int pmu_set_security_value_to_efuse(void)
{
    TRACE(0,"%s stub!!!", __FUNCTION__);
    return 0;
}

int pmu_set_sec_boot_struct(void)
{
    int ret = 0;
    unsigned short val = 0, val_tmp = 0;
    unsigned short programming_bit[] = {0x01, 0x11, 0xd1, 0x101, 0x111, 0x1d1}; //write page1:0x2003
    int i = 0;

    // Enable CLK_EN
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Enable TURN_ON
    val |= REG_EFUSE_TURN_ON;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }
    val_tmp = val;

    // write programming_bit
    for (i = 0; i < sizeof(programming_bit) / sizeof(programming_bit[0]); i++) {
        val = val_tmp;

        // Write Address
        val |= REG_EFUSE_ADDRESS(programming_bit[i]);
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        // Set Strobe Trigger = 1
        val |= REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        // set Strobe Trigger = 0
        val &= ~REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        hal_sys_timer_delay(MS_TO_TICKS(10));
    }

_exit:
    // Disable TURN_ON
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    // Disable CLK_EN
    val = 0;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    return ret;
}

int pmu_get_boot_setting(union BOOT_SETTINGS_T *val)
{
    int ret = -1;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_BOOT, &val->reg);
    if (ret) {
        // Error
        goto err;
    }

    // OK
    return 0;

err:
    val->reg = 0;
    return 1;
}

int pmu_set_boot_flash_cfg(void)
{
    int ret = 0;
    unsigned short val = 0, val_tmp = 0;
    unsigned short programming_bit[] = {0xE2, 0xF2, 0x1E2, 0x1F2}; //write page2:0xc000
    int i = 0;

    // Enable CLK_EN
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Enable TURN_ON
    val |= REG_EFUSE_TURN_ON;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }
    val_tmp = val;

    // write programming_bit
    for (i = 0; i < sizeof(programming_bit) / sizeof(programming_bit[0]); i++) {
        val = val_tmp;

        // Write Address
        val |= REG_EFUSE_ADDRESS(programming_bit[i]);
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        // Set Strobe Trigger = 1
        val |= REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        // set Strobe Trigger = 0
        val &= ~REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        hal_sys_timer_delay(MS_TO_TICKS(10));
    }

_exit:
    // Disable TURN_ON
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    // Disable CLK_EN
    val = 0;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    return ret;
}

int pmu_get_boot_flash_cfg(union BOOT_FLASH_CFG_T *val)
{
    int ret;

    val->reg = 0;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_FLASH_CFG, &val->reg);
    if (ret) {
        // Error
        return ret;
    }

    // OK
    return 0;
}

int pmu_set_code_ver(unsigned short data)
{
    int ret = 0;
    unsigned short val = 0, val_tmp = 0;
    int i = 0, j = 0, bit1_num = 0;
    unsigned short programming_bit[16][2] = {
        {0x06, 0x106}, //write 0x1
        {0x16, 0x116}, //write 0x3
        {0x26, 0x126}, //write 0x7
        {0x36, 0x136}, //write 0xf
        {0x46, 0x146}, //write 0x1f
        {0x56, 0x156}, //write 0x3f
        {0x66, 0x166}, //write 0x7f
        {0x76, 0x176}, //write 0xff
        {0x86, 0x186}, //write 0x1ff
        {0x96, 0x196}, //write 0x3ff
        {0xa6, 0x1a6}, //write 0x7ff
        {0xb6, 0x1b6}, //write 0xfff
        {0xc6, 0x1c6}, //write 0x1fff
        {0xd6, 0x1d6}, //write 0x3fff
        {0xe6, 0x1e6}, //write 0x7fff
        {0xf6, 0x1f6}, //write 0xffff
    };

    while (data & (0x1 << i)) {
        i ++;
        bit1_num ++;
    }
    if (bit1_num > 16) {
        bit1_num = 16;
    }
    //PMU_INFO_TRACE_IMM(0, "%s %d, bit1_num:%d", __func__, __LINE__, bit1_num);

    // Enable CLK_EN
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Enable TURN_ON
    val |= REG_EFUSE_TURN_ON;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }
    val_tmp = val;

    // write programming_bit
    for (j = 0; j < 2; j ++) {
        for (i = 0; i < bit1_num; i++) {
            val = val_tmp;

            // Write Address
            val |= REG_EFUSE_ADDRESS(programming_bit[i][j]);
            ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
            if (ret) {
                goto _exit;
            }

            // Set Strobe Trigger = 1
            val |= REG_EFUSE_STROBE_TRIGGER;
            ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
            if (ret) {
                goto _exit;
            }

            // set Strobe Trigger = 0
            val &= ~REG_EFUSE_STROBE_TRIGGER;
            ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
            if (ret) {
                goto _exit;
            }

            hal_sys_timer_delay(MS_TO_TICKS(10));
        }
    }

_exit:
    // Disable TURN_ON
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    // Disable CLK_EN
    val = 0;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    return ret;
}

int pmu_get_code_ver(unsigned short *val)
{
    int ret = -1;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_CODE_VER, val);
    if (ret) {
        // Error
        goto err;
    }

    // OK
    return 0;

err:
    *val = 0;
    return 1;
}

enum HAL_FLASH_ID_T BOOT_TEXT_SRAM_LOC pmu_get_boot_flash_id(void)
{
    union SECURITY_VALUE_T security;

    pmu_get_security_value(&security);

    return (security.root.flash_id) ? HAL_FLASH_ID_1 : HAL_FLASH_ID_0;
}

static void pmu_sys_ctrl(bool shutdown)
{
    uint16_t val;
    uint32_t lock = int_lock();

    PMU_INFO_TRACE_IMM(0, "Start pmu %s", shutdown ? "shutdown" : "reboot");

    if (shutdown) {
#if !defined(SIMU) && (defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER)))
        pmu_mode_change(PMU_POWER_MODE_LDO);
        hal_sys_timer_delay(MS_TO_TICKS(1));
#endif

#ifdef RTC_ENABLE
        pmu_rtc_save_context();
#endif

        // Reset PMU
        pmu_write(PMU_REG_METAL_ID, 0xCAFE);
        pmu_write(PMU_REG_METAL_ID, 0x5FEE);
        hal_sys_timer_delay(MS_TO_TICKS(200));

#ifdef RTC_ENABLE
        pmu_rtc_restore_context();
#endif

#if defined(_AUTO_TEST_)
        if (at_skip_shutdown) {
            hal_cmu_sys_reboot();
            return;
        }
#endif

        pmu_read(PMU_REG_POWER_OFF,&val);
        val |= SOFT_POWER_OFF;
        for (int i = 0; i < 100; i++) {
            pmu_write(PMU_REG_POWER_OFF,val);
            hal_sys_timer_delay(MS_TO_TICKS(200));
        }

        hal_sys_timer_delay(MS_TO_TICKS(50));

        //can't reach here
        PMU_INFO_TRACE_IMM(0, "\nError: pmu shutdown failed!\n");
        hal_sys_timer_delay(MS_TO_TICKS(5));
    } else {
#if defined(PMU_FULL_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
        // CAUTION:
        // 1) Never reset RF because system or flash might be using X2/X4, which are off by default
        // 2) Never reset WFRF/RF/ANA because system or flash might be using PLL, and the reset might cause clock glitch
        // TODO:
        // Restore BBPLL settings in WFRF
#endif
    }

    hal_cmu_sys_reboot();

    int_unlock(lock);
}

void pmu_shutdown(void)
{
    hal_cmu_shutdown_hook();
    pmu_sys_ctrl(true);
}

void pmu_reboot(void)
{
    //hal_cmu_shutdown_hook();
    pmu_sys_ctrl(false);
}

static inline uint16_t pmu_get_module_addr(enum PMU_MODUAL_T module)
{
    if (module == PMU_CODEC) {
        return PMU_REG_CODEC_CFG;
    } else if (module == PMU_RF) {
            return PMU_REG_RF_CFG;
    } else {
        return module + PMU_REG_MODULE_START;
    }
}

BOOT_TEXT_FLASH_LOC void pmu_module_config(enum PMU_MODUAL_T module,unsigned short is_manual,unsigned short ldo_on,unsigned short lp_mode,unsigned short dpmode)
{
    int ret;
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = pmu_get_module_addr(module);

    ret = pmu_read(module_address, &val);
    if(ret == 0)
    {
        if(is_manual)
        {
            val |= module_cfg_p->manual_bit;
        }
        else
        {
            val &= ~module_cfg_p->manual_bit;
        }
        if(ldo_on)
        {
            val |= module_cfg_p->ldo_en;
        }
        else
        {
            val &= ~module_cfg_p->ldo_en;
        }
        if(lp_mode)
        {
            val |= module_cfg_p->lp_mode;
        }
        else
        {
            val &= ~module_cfg_p->lp_mode;
        }
        if(dpmode)
        {
            val |= module_cfg_p->dsleep_mode;
        }
        else
        {
            val &= ~module_cfg_p->dsleep_mode;
        }
        pmu_write(module_address, val);
    }
}

BOOT_TEXT_FLASH_LOC void pmu_module_set_volt(unsigned char module, unsigned short sleep_v,unsigned short normal_v)
{
    int ret;
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = pmu_get_module_addr(module);

    ret = pmu_read(module_address, &val);
    if(ret == 0)
    {
        val &= ~module_cfg_p->normal_v;
        val |= (normal_v << module_cfg_p->normal_v_shift) & module_cfg_p->normal_v;
        val &= ~module_cfg_p->dsleep_v;
        val |= (sleep_v << module_cfg_p->dsleep_v_shift) & module_cfg_p->dsleep_v;
        pmu_write(module_address, val);
    }
}

BOOT_TEXT_FLASH_LOC int pmu_module_get_volt(unsigned char module, unsigned short *sleep_vp,unsigned short *normal_vp)
{
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = pmu_get_module_addr(module);

    pmu_read(module_address, &val);
    if (normal_vp) {
        *normal_vp = (val & module_cfg_p->normal_v) >> module_cfg_p->normal_v_shift;
    }
    if (sleep_vp) {
        *sleep_vp = (val & module_cfg_p->dsleep_v) >> module_cfg_p->dsleep_v_shift;
    }

    return 0;
}

static BOOT_TEXT_FLASH_LOC void pmu_dcdc_ana_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_ANA_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_BUCK_ANA_VBIT_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_BUCK_ANA_VBIT_DSLEEP);
    }
#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_ana_comp, MAX_DCDC_ANA_VBIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_dcdc_ana_comp, MAX_DCDC_ANA_VBIT_VAL);
    }
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_dcdc_ana_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    int ret;
    unsigned short val;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_ana_comp, MAX_DCDC_ANA_VBIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_dcdc_ana_comp, MAX_DCDC_ANA_VBIT_VAL);
#endif

    ret = pmu_read(PMU_REG_DCDC_ANA_VOLT, &val);
    if(ret == 0)
    {
        val &= ~REG_BUCK_ANA_VBIT_DSLEEP_MASK;
        val &= ~REG_BUCK_ANA_VBIT_NORMAL_MASK;
        val |= REG_BUCK_ANA_VBIT_DSLEEP(dsleep_v);
        val |= REG_BUCK_ANA_VBIT_NORMAL(normal_v);
        pmu_write(PMU_REG_DCDC_ANA_VOLT, val);
    }

    if (normal_v > dsleep_v) {
        dcdc_ramp_map |= DCDC_RAMP_MAP_VANA;
    } else {
        dcdc_ramp_map &= ~DCDC_RAMP_MAP_VANA;
    }
}

static BOOT_TEXT_FLASH_LOC void pmu_ana_set_volt(int mode_change, enum PMU_POWER_MODE_T mode)
{
    uint16_t old_act_dcdc;
    uint16_t old_lp_dcdc;

    if (mode == PMU_POWER_MODE_ANA_DCDC || mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_ana_get_volt(&old_act_dcdc, &old_lp_dcdc);
        if (old_act_dcdc < ana_act_dcdc) {
            while (old_act_dcdc++ < ana_act_dcdc) {
                pmu_dcdc_ana_set_volt(old_act_dcdc, ana_lp_dcdc);
            }
            hal_sys_timer_delay_us(PMU_VANA_STABLE_TIME_US);
        } else if (old_act_dcdc != ana_act_dcdc || old_lp_dcdc != ana_lp_dcdc) {
            pmu_dcdc_ana_set_volt(ana_act_dcdc, ana_lp_dcdc);
        }
    }
    if (pmu_metal_id >= HAL_CHIP_METAL_ID_8)
        pmu_module_set_volt(PMU_ANA, PMU_VANA_1_4V, PMU_VANA_1_4V);
}

BOOT_TEXT_FLASH_LOC static void pmu_dcdc_dig_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_DIG_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_BUCK_VCORE_VBIT_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_BUCK_VCORE_VBIT_DSLEEP);
    }
#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_dig_comp, MAX_DCDC_VCORE_VBIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_dcdc_dig_comp, MAX_DCDC_VCORE_VBIT_VAL);
    }
#endif
}

BOOT_TEXT_FLASH_LOC static void pmu_dcdc_dig_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    int ret;
    unsigned short val;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_dig_comp, MAX_DCDC_VCORE_VBIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_dcdc_dig_comp, MAX_DCDC_VCORE_VBIT_VAL);
#endif

    ret = pmu_read(PMU_REG_DCDC_DIG_VOLT, &val);
    if(ret == 0)
    {
        val &= ~REG_BUCK_VCORE_VBIT_NORMAL_MASK;
        val &= ~REG_BUCK_VCORE_VBIT_DSLEEP_MASK;
        val |= REG_BUCK_VCORE_VBIT_NORMAL(normal_v);
        val |= REG_BUCK_VCORE_VBIT_DSLEEP(dsleep_v);
        pmu_write(PMU_REG_DCDC_DIG_VOLT, val);
    }
    if (normal_v > dsleep_v) {
        dcdc_ramp_map |= DCDC_RAMP_MAP_VCORE;
    } else {
        dcdc_ramp_map &= ~DCDC_RAMP_MAP_VCORE;
    }
}

static void BOOT_TEXT_FLASH_LOC pmu_dig_get_target_volt(uint16_t *ldo, uint16_t *dcdc)
{
    uint16_t ldo_volt;
    uint16_t dcdc_volt;

    if (0) {
    } else if (pmu_vcore_req & (PMU_VCORE_SYS_FREQ_ULTRAHIGH)) {
#if defined(MCU_HIGH_PERFORMANCE_MODE_320)
        ldo_volt = PMU_VDIG_1_0V;
        dcdc_volt = PMU_DCDC_DIG_1_0V;
#else //384MHz
        ldo_volt = PMU_VDIG_1_0V;
        dcdc_volt = PMU_DCDC_DIG_1_0V;
#endif
    } else if (pmu_vcore_req & (PMU_VCORE_IIR_FREQ_MEDIUM | PMU_VCORE_SYS_FREQ_HIGH)) {
        ldo_volt = PMU_VDIG_0_9V;
        dcdc_volt = PMU_DCDC_DIG_0_9V;
    } else if (pmu_vcore_req & (PMU_VCORE_USB_HS_ENABLED | PMU_VCORE_SYS_FREQ_MEDIUM |
        PMU_VCORE_FLASH_FREQ_MEDIUM | PMU_VCORE_FLASH_WRITE_ENABLED | PMU_VCORE_FLASH_FREQ_HIGH)) {
        ldo_volt = PMU_VDIG_0_8V;
        dcdc_volt = PMU_DCDC_DIG_0_8V;
    } else {
        ldo_volt = PMU_VDIG_0_7V;
        dcdc_volt = PMU_DCDC_DIG_0_7V;
    }

#if defined(PROGRAMMER) && !defined(PMU_FULL_INIT)
    // Try to keep the same vcore voltage as ROM (hardware default)
    if (ldo_volt < PMU_VDIG_1_1V) {
        //ldo_volt = PMU_VDIG_1_0V;
    }
    if (dcdc_volt < PMU_DCDC_DIG_1_1V) {
        //dcdc_volt = PMU_DCDC_DIG_1_0V;
    }
#endif

#if defined(PSRAM_ENABLE)
    if (ldo_volt < PMU_VDIG_1_0V) {
        //ldo_volt = PMU_VDIG_1_0V;
    }
    if (dcdc_volt < PMU_DCDC_DIG_1_0V) {
        //dcdc_volt = PMU_DCDC_DIG_1_0V;
    }
#endif

#if defined(PSRAMUHS_ENABLE)
    if (ldo_volt < PMU_VDIG_0_95V) {
        ldo_volt = PMU_VDIG_0_95V;
    }
    if (dcdc_volt < PMU_DCDC_DIG_0_95V) {
        dcdc_volt = PMU_DCDC_DIG_0_95V;
    }
#endif

#if defined(DSP_ENABLE)
#if (A7_DSP_SPEED >= 1000)
    if (ldo_volt < PMU_VDIG_0_95V) {
        ldo_volt = PMU_VDIG_0_95V;
    }
    if (dcdc_volt < PMU_DCDC_DIG_0_95V) {
        dcdc_volt = PMU_DCDC_DIG_0_95V;
    }
#else
    if (ldo_volt < PMU_VDIG_0_9V) {
        ldo_volt = PMU_VDIG_0_9V;
    }
    if (dcdc_volt < PMU_DCDC_DIG_0_9V) {
        dcdc_volt = PMU_DCDC_DIG_0_9V;
    }
#endif
#endif

    if (ldo) {
        *ldo = ldo_volt;
    }
    if (dcdc) {
        *dcdc = dcdc_volt;
    }
}

BOOT_TEXT_FLASH_LOC static void pmu_dig_set_volt(int mode_change, enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;
    uint16_t dcdc_volt;
    uint16_t ldo_volt;

    lock = int_lock();

    if (mode == PMU_POWER_MODE_NONE) {
        mode = pmu_power_mode;
    }

    uint16_t old_act_dcdc, old_lp_dcdc;
    uint16_t old_act_ldo, old_lp_ldo;
    bool volt_inc = false;

    pmu_dig_get_target_volt(&ldo_volt, &dcdc_volt);

    pmu_module_get_volt(PMU_DIG, &old_lp_ldo, &old_act_ldo);
    pmu_dcdc_dig_get_volt(&old_act_dcdc, &old_lp_dcdc);

    if (mode_change || mode == PMU_POWER_MODE_DIG_DCDC) {
        if (old_act_dcdc < dcdc_volt) {
            volt_inc = true;
            while (old_act_dcdc++ < dcdc_volt) {
                pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
            }
        } else if (!vcore_high_volt && old_act_dcdc != dcdc_volt) {
            while (old_act_dcdc-- > dcdc_volt) {
                pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
                hal_sys_timer_delay_us(100);
            }
        } else if (old_lp_dcdc != dig_lp_dcdc) {
            pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
        }
    }

    if (mode_change || mode != PMU_POWER_MODE_DIG_DCDC) {
        if (old_act_ldo < ldo_volt) {
            volt_inc = true;
            while (old_act_ldo++ < ldo_volt) {
                pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_act_ldo);
            }
        } else if (!vcore_high_volt && old_act_ldo != ldo_volt) {
            while (old_act_ldo-- > ldo_volt) {
                pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_act_ldo);
                hal_sys_timer_delay_us(100);
            }
        } else if (old_lp_ldo != dig_lp_ldo) {
            pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_act_ldo);
        }
    }

    if (mode_change == 0) {
        // Update the voltage of the other mode
        if (mode == PMU_POWER_MODE_DIG_DCDC) {
            if (!vcore_high_volt || old_act_ldo < ldo_volt)
                pmu_module_set_volt(PMU_DIG, dig_lp_ldo, ldo_volt);
        } else {
            if (!vcore_high_volt || old_act_dcdc < dcdc_volt)
                pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        }
    }

    if (volt_inc) {
        hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
    }

    int_unlock(lock);
}

BOOT_TEXT_FLASH_LOC static void pmu_ldo_mode_en(void)
{
    unsigned short val;

    // Enable vana ldo
    pmu_module_config(PMU_ANA,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    // Enable vcore ldo
    pmu_module_config(PMU_DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

    pmu_ana_set_volt(1, PMU_POWER_MODE_LDO);
    pmu_dig_set_volt(1, PMU_POWER_MODE_LDO);

    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);

    // Disable vana dcdc
    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
    val = (val & ~REG_PU_DCDC_ANA_REG) | REG_PU_DCDC_ANA_DR;
    pmu_write(PMU_REG_DCDC_ANA_EN, val);

    // Disable vcore dcdc
    pmu_read(PMU_REG_DCDC_DIG_EN, &val);
    val = (val & ~REG_PU_DCDC_VCORE_REG) | REG_PU_DCDC_VCORE_DR;
    pmu_write(PMU_REG_DCDC_DIG_EN, val);
}

BOOT_TEXT_FLASH_LOC static void pmu_dcdc_ana_mode_en(void)
{
    unsigned short val;

    if (pmu_power_mode == PMU_POWER_MODE_DIG_DCDC) {
        // Enable vcore ldo
        pmu_module_config(PMU_DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

        pmu_ana_set_volt(1, PMU_POWER_MODE_ANA_DCDC);
        pmu_dig_set_volt(1, PMU_POWER_MODE_ANA_DCDC);

        hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);

        // Disable vcore dcdc
        pmu_read(PMU_REG_DCDC_DIG_EN, &val);
        val = (val & ~REG_PU_DCDC_VCORE_REG) | REG_PU_DCDC_VCORE_DR;
        pmu_write(PMU_REG_DCDC_DIG_EN, val);
    } else {
        //u_read(PMU_REG_DCDC_ANA_CFG_19, &val);
        //l |= DCDC2_REG_BYPASS;
        //u_write(PMU_REG_DCDC_ANA_CFG_19, val);

        // Enable vana dcdc
        pmu_read(PMU_REG_DCDC_ANA_EN, &val);
#ifdef PMU_DCDC_ULP_MODE
        val |= REG_BUCK_ANA_ULP_MODE_NORMAL | REG_BUCK_ANA_ULP_MODE_DSLEEP | REG_PU_DCDC_ANA_DR | REG_PU_DCDC_ANA_REG;
#elif defined(DCDC_ULP_LP_ON)
        val = (val & ~REG_BUCK_ANA_ULP_MODE_NORMAL) | REG_BUCK_ANA_ULP_MODE_DSLEEP | REG_PU_DCDC_ANA_DR | REG_PU_DCDC_ANA_REG;
#else
        val = (val & ~(REG_BUCK_ANA_ULP_MODE_NORMAL | REG_BUCK_ANA_ULP_MODE_DSLEEP)) | REG_PU_DCDC_ANA_DR | REG_PU_DCDC_ANA_REG;
#endif
        pmu_write(PMU_REG_DCDC_ANA_EN, val);

        pmu_ana_set_volt(1, PMU_POWER_MODE_ANA_DCDC);
        pmu_dig_set_volt(1, PMU_POWER_MODE_ANA_DCDC);

        hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

        // Disable vana ldo
        pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);

        //pmu_read(PMU_REG_DCDC_ANA_CFG_19, &val);
        //val &= ~DCDC2_REG_BYPASS;
        //pmu_write(PMU_REG_DCDC_ANA_CFG_19, val);
    }
}


BOOT_TEXT_FLASH_LOC static void pmu_dcdc_dual_mode_en(void)
{
    unsigned short val;

    // Enable vana dcdc
    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
#ifdef PMU_DCDC_ULP_MODE
    val |= REG_BUCK_ANA_ULP_MODE_NORMAL | REG_BUCK_ANA_ULP_MODE_DSLEEP | REG_PU_DCDC_ANA_DR | REG_PU_DCDC_ANA_REG;
#elif defined(DCDC_ULP_LP_ON)
    val = (val & ~REG_BUCK_ANA_ULP_MODE_NORMAL) | REG_BUCK_ANA_ULP_MODE_DSLEEP | REG_PU_DCDC_ANA_DR | REG_PU_DCDC_ANA_REG;
#else
    val = (val & ~(REG_BUCK_ANA_ULP_MODE_NORMAL | REG_BUCK_ANA_ULP_MODE_DSLEEP)) | REG_PU_DCDC_ANA_DR | REG_PU_DCDC_ANA_REG;
#endif
    pmu_write(PMU_REG_DCDC_ANA_EN, val);
    // Enable vcore dcdc
    pmu_read(PMU_REG_DCDC_DIG_EN, &val);
#ifdef PMU_DCDC_ULP_MODE
    val |= REG_BUCK_VCORE_ULP_MODE_NORMAL | REG_BUCK_VCORE_ULP_MODE_DSLEEP | REG_PU_DCDC_VCORE_DR | REG_PU_DCDC_VCORE_REG;
#elif defined(DCDC_ULP_LP_ON)
    val = (val & ~REG_BUCK_VCORE_ULP_MODE_NORMAL) | REG_BUCK_VCORE_ULP_MODE_DSLEEP | REG_PU_DCDC_VCORE_DR | REG_PU_DCDC_VCORE_REG;
#else
    val = (val & ~(REG_BUCK_VCORE_ULP_MODE_NORMAL | REG_BUCK_VCORE_ULP_MODE_DSLEEP)) | REG_PU_DCDC_VCORE_DR | REG_PU_DCDC_VCORE_REG;
#endif
    pmu_write(PMU_REG_DCDC_DIG_EN, val);

    pmu_ana_set_volt(1, PMU_POWER_MODE_DIG_DCDC);
    pmu_dig_set_volt(1, PMU_POWER_MODE_DIG_DCDC);

    hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

    if (pmu_metal_id < HAL_CHIP_METAL_ID_8) { // 2003
        // Disable vana ldo
        pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        // Disable vcore ldo
        pmu_module_config(PMU_DIG,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    }
}

BOOT_TEXT_FLASH_LOC void pmu_mode_change(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;
    bool update_mode;

    if (pmu_power_mode == mode)
        return;

    update_mode = true;

    lock = int_lock();

    if (mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_dual_mode_en();
    } else if (mode == PMU_POWER_MODE_ANA_DCDC) {
        pmu_dcdc_ana_mode_en();
    } else if (mode == PMU_POWER_MODE_LDO) {
        pmu_ldo_mode_en();
    } else {
        update_mode = false;
    }

    if (update_mode) {
        pmu_power_mode = mode;
    }

    int_unlock(lock);
}

BOOT_TEXT_FLASH_LOC void pmu_low_power_config(void)
{
    u16 val_u16;

    pmu_read(PMU_REG_IO_CFG, &val_u16);
    val_u16 &= ~REG_PU_LDO_VIO;
    val_u16 |= REG_PU_LDO_VIO_DR; //close vio
    pmu_write(PMU_REG_IO_CFG, val_u16);

    pmu_read(PMU_REG_ANA_CFG, &val_u16);
    val_u16 &= ~REG_PU_LDO_VANA;
    val_u16 |= REG_PU_LDO_VANA_DR;
    pmu_write(PMU_REG_ANA_CFG, val_u16);

    pmu_read(PMU_REG_LDO_VPA1_VOLT, &val_u16);
    val_u16 &= ~REG_PU_LDO_VPA1_REG;
    val_u16 |= REG_PU_LDO_VPA1_DR;
    pmu_write(PMU_REG_LDO_VPA1_VOLT, 0x4ad2);

    pmu_read(PMU_REG_LDO_VPA2_VOLT, &val_u16);
    val_u16 &= ~REG_PU_LDO_VPA2_REG;
    val_u16 |= REG_PU_LDO_VPA2_DR;
    pmu_write(PMU_REG_LDO_VPA2_VOLT, 0xaa64);

    pmu_read(PMU_REG_DCDC_ANA_EN, &val_u16);
    val_u16 |= REG_BUCK_ANA_BURST_MODE_DSLEEP;
    val_u16 |= REG_BUCK_ANA_BURST_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_ANA_EN, val_u16);

    pmu_read(PMU_REG_DCDC_DIG_EN, &val_u16);
    val_u16 |= REG_BUCK_VCORE_BURST_MODE_DSLEEP;
    val_u16 |= REG_BUCK_VCORE_BURST_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_DIG_EN, val_u16);

    pmu_read(PMU_REG_DCDC_HPPA_EN, &val_u16);
    val_u16 |= REG_BUCK_HPPA_BURST_MODE_DSLEEP;
    val_u16 |= REG_BUCK_HPPA_BURST_MODE_NORMAL;
    pmu_write(PMU_REG_DCDC_HPPA_EN, val_u16);
}

void pmu_sleep_en(unsigned char sleep_en)
{
    int ret;
    unsigned short val;

    ret = pmu_read(PMU_REG_SLEEP_CFG, &val);
    if(ret == 0)
    {
        if(sleep_en)
        {
            val |= SLEEP_ALLOW;
        }
        else
        {
            val &= ~SLEEP_ALLOW;
        }
        pmu_write(PMU_REG_SLEEP_CFG, val);
    }
}

#if !defined(SIMU) && (defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER)))
static uint32_t pmu_vcodec_mv_to_val(uint16_t mv)
{
    switch(mv) {
    case 1500:
        return PMU_CODEC_1_5V;
    case 1600:
        return PMU_CODEC_1_6V;
    case 1700:
        return PMU_CODEC_1_7V;
    case 1800:
        return PMU_CODEC_1_8V;
    case 1900:
        return PMU_CODEC_1_9V;
    case 2000:
        return PMU_CODEC_2_0V;
    case 2100:
        return PMU_CODEC_2_1V;
    case 2200:
        return PMU_CODEC_2_2V;
    case 2300:
        return PMU_CODEC_2_3V;
    case 2400:
        return PMU_CODEC_2_4V;
    case 2500:
        return PMU_CODEC_2_5V;
    case 2600:
        return PMU_CODEC_2_6V;
    case 2700:
        return PMU_CODEC_2_7V;
    default:
        return PMU_CODEC_1_8V;
    }
}

static uint32_t pmu_dcdc_hppa_mv_to_val(uint16_t mv)
{
    uint32_t val;

    if (mv == 1600) {
        val = PMU_DCDC_HPPA_1_6V;
    } else if (mv == 1700) {
        val = PMU_DCDC_HPPA_1_7V;
    } else if (mv == 1900) {
        val = PMU_DCDC_HPPA_1_9V;
    } else if (mv >= 1950) {
        val = PMU_DCDC_HPPA_1_95V;
    } else {
        val = PMU_DCDC_HPPA_1_8V;
    }

    return val;
}

static void pmu_dcdc_hppa_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_HPPA_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_BUCK_HPPA_VBIT_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_BUCK_HPPA_VBIT_DSLEEP);
    }

#ifdef PMU_DCDC_CALIB
    if (normal_vp) {
        *normal_vp = pmu_reg_val_add(*normal_vp, -pmu_dcdc_hppa_comp, MAX_DCDC_HPPA_VBIT_VAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = pmu_reg_val_add(*dsleep_vp, -pmu_dcdc_hppa_comp, MAX_DCDC_HPPA_VBIT_VAL);
    }
#endif
}

static void pmu_dcdc_hppa_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

#ifdef PMU_DCDC_CALIB
    normal_v = pmu_reg_val_add(normal_v, pmu_dcdc_hppa_comp, MAX_DCDC_HPPA_VBIT_VAL);
    dsleep_v = pmu_reg_val_add(dsleep_v, pmu_dcdc_hppa_comp, MAX_DCDC_HPPA_VBIT_VAL);
#endif

    pmu_read(PMU_REG_DCDC_HPPA_VOLT, &val);
    val &= ~REG_BUCK_HPPA_VBIT_DSLEEP_MASK;
    val &= ~REG_BUCK_HPPA_VBIT_NORMAL_MASK;
    val |= REG_BUCK_HPPA_VBIT_DSLEEP(dsleep_v);
    val |= REG_BUCK_HPPA_VBIT_NORMAL(normal_v);
    pmu_write(PMU_REG_DCDC_HPPA_VOLT, val);

    if (normal_v > dsleep_v) {
        dcdc_ramp_map |= DCDC_RAMP_MAP_VHPPA;
    } else {
        dcdc_ramp_map &= ~DCDC_RAMP_MAP_VHPPA;
    }
}

static void pmu_dcdc_hppa_en(int enable)
{
    uint16_t val;

    if (enable) {
        //pmu_read(PMU_REG_DCDC_HPPA_CFG_1D, &val);
        //val |= DCDC3_REG_BYPASS;
        //pmu_write(PMU_REG_DCDC_HPPA_CFG_1D, val);
    }

    pmu_read(PMU_REG_DCDC_HPPA_EN, &val);
    if (enable) {
#ifdef PMU_DCDC_ULP_MODE
    val |= REG_BUCK_HPPA_ULP_MODE_NORMAL | REG_BUCK_HPPA_ULP_MODE_DSLEEP | REG_PU_DCDC_HPPA_DR | REG_PU_DCDC_HPPA_REG;
#elif defined(DCDC_ULP_LP_ON)
        val = (val & ~REG_BUCK_HPPA_ULP_MODE_NORMAL) | REG_BUCK_HPPA_ULP_MODE_DSLEEP | REG_PU_DCDC_HPPA_DR | REG_PU_DCDC_HPPA_REG;
#else
        val = (val & ~(REG_BUCK_HPPA_ULP_MODE_NORMAL | REG_BUCK_HPPA_ULP_MODE_DSLEEP)) | REG_PU_DCDC_HPPA_DR | REG_PU_DCDC_HPPA_REG;
#endif
    } else {
        val = (val & ~REG_PU_DCDC_HPPA_REG) | REG_PU_DCDC_HPPA_DR;
    }
    pmu_write(PMU_REG_DCDC_HPPA_EN, val);

    if (enable) {
        hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);
        //pmu_read(PMU_REG_DCDC_HPPA_CFG_1D, &val);
        //val &= ~DCDC3_REG_BYPASS;
        //pmu_write(PMU_REG_DCDC_HPPA_CFG_1D, val);
    }
}

static uint32_t pmu_vhppa_mv_to_val(uint16_t mv)
{
    return pmu_dcdc_hppa_mv_to_val(mv);
}

static void POSSIBLY_UNUSED pmu_hppa_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    pmu_dcdc_hppa_get_volt(normal_vp, dsleep_vp);
}

static void pmu_hppa_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    pmu_dcdc_hppa_set_volt(normal_v, dsleep_v);
}

static void pmu_hppa_en(int enable)
{
    pmu_dcdc_hppa_en(enable);
}

static void BOOT_TEXT_FLASH_LOC pmu_dig_init_volt(void)
{
#if !defined(SIMU) && (defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER)))
    uint16_t ldo_volt;
    uint16_t val;

    pmu_dig_get_target_volt(&ldo_volt, NULL);

    pmu_read(PMU_REG_DIG_CFG, &val);
    if (GET_BITFIELD(val, LDO_VDIG_VBIT_NORMAL) < ldo_volt) {
        val = SET_BITFIELD(val, LDO_VDIG_VBIT_NORMAL, ldo_volt);
        pmu_write(PMU_REG_DIG_CFG, val);
    }
#endif
}
#endif

POSSIBLY_UNUSED static BOOT_BSS_LOC bool pmu_opened = false;
int BOOT_TEXT_FLASH_LOC pmu_open(void)
{
#if !defined(SIMU) && (defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER)))
    int ret;
    uint16_t val;
    enum PMU_POWER_MODE_T mode;

    if (pmu_opened)
        return 0;

    pmu_opened = true;

    ASSERT(!vcodec_off || vcodec_mv == vhppa_mv,
        "Invalid vcodec/vhppa cfg: vcodec_off=%d vcodec_mv=%u vhppa_mv=%u", vcodec_off, vcodec_mv, vhppa_mv);
    ASSERT(vcodec_mv == 1600 || vcodec_mv == 1700 || vcodec_mv == 1800 || vcodec_mv == 1900 || vcodec_mv == 1950,
        "Invalid vcodec cfg: vcodec_mv=%u", vcodec_mv);
    ASSERT(vhppa_mv == 1600 || vhppa_mv == 1700 || vhppa_mv == 1800 || vhppa_mv == 1900 || vhppa_mv == 1950,
        "Invalid vhppa cfg: vhppa_mv=%u", vhppa_mv);

    // Disable and clear all PMU irqs by default
    pmu_write(PMU_REG_INT_MASK, 0);
    pmu_write(PMU_REG_INT_EN, 0);
    // PMU irqs cannot be cleared by PMU soft reset
    pmu_read(PMU_REG_CHARGER_STATUS, &val);
    pmu_write(PMU_REG_CHARGER_STATUS, val);
    pmu_read(PMU_REG_INT_STATUS, &val);
    pmu_write(PMU_REG_INT_CLR, val);

    pmu_read(0x91, &val);
    val = SET_BITFIELD(val, REG_SAR_VREG_IBIT, 0x2);
    pmu_write(0x91, val);

    // Allow PMU to sleep when power key is pressed
    ret = pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    if (ret == 0) {
        val &= ~POWERKEY_WAKEUP_OSC_EN;
        pmu_write(PMU_REG_POWER_KEY_CFG, val);
    }

#ifdef PMU_DCDC_CALIB
    pmu_get_dcdc_calib_value();
#endif

    if ((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_WARM_BOOT) == 0) {
#ifndef PMU_FORCE_LP_MODE
#ifdef FORCE_BIG_BANDGAP
        // Force big bandgap
        pmu_read(PMU_REG_BIAS_CFG, &val);
        val |= BG_CONSTANT_GM_BIAS_DR | BG_CONSTANT_GM_BIAS_REG;
        val |= BG_CORE_EN_DR | BG_CORE_EN_REG;
        pmu_write(PMU_REG_BIAS_CFG, val);
        hal_sys_timer_delay_us(20);
        val |= BG_VBG_SEL_DR | BG_VBG_SEL_REG;
        pmu_write(PMU_REG_BIAS_CFG, val);
#else
        // Allow low power bandgap
        pmu_read(PMU_REG_BIAS_CFG, &val);
        val &= ~BG_VBG_SEL_DR;
        pmu_write(PMU_REG_BIAS_CFG, val);
#endif
#endif

#ifdef DIG_DCDC_MODE
        mode = PMU_POWER_MODE_DIG_DCDC;
#elif defined(ANA_DCDC_MODE)
        mode = PMU_POWER_MODE_ANA_DCDC;
#else // LDO_MODE
        mode = PMU_POWER_MODE_LDO;
#endif
        pmu_mode_change(mode);

#ifdef RTC_ENABLE
        pmu_rtc_restore_context();
#endif
    }

#ifndef NO_SLEEP
    pmu_sleep_en(true);
#endif

    //3v3power
    pmu_wf_tx_config(0);

#ifdef PMU_IRQ_UNIFIED
    pmu_read(PMU_REG_WDT_CFG, &val);
    val = (val & ~POWERON_DETECT_EN) | MERGE_INTR;
    pmu_write(PMU_REG_WDT_CFG, val);
#endif

#ifdef __BEST_FLASH_VIA_ANA__
    pmu_module_config(PMU_IO,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VIO in sleep
#else
    pmu_module_config(PMU_IO,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON); //enable VIO in sleep
#endif
    pmu_module_set_volt(PMU_IO, vio_lp, vio_act_normal);

    // 2003 doesn't have vmem, memory use vcodec or vhppa. -- enable vhppa dcdc and disable vcodec ldo
    val = pmu_vhppa_mv_to_val(vhppa_mv);
    pmu_hppa_set_volt(val, val);
    val = pmu_vcodec_mv_to_val(vcodec_mv);
    pmu_module_set_volt(PMU_CODEC, val, val);
    if (vcodec_off) {
        pmu_hppa_en(true);
        pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VCODEC
    } else {
        pmu_module_config(PMU_CODEC,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON); //disable VCODEC
    }

#ifdef PSRAMUHS_ENABLE
    if (pmu_metal_id >= HAL_CHIP_METAL_ID_8) {
        //pmu_module_set_volt(PMU_RF, PMU_VRF_1_0V, PMU_VRF_1_3V);
        pmu_module_set_volt(PMU_RF, PMU_VRF_1_2V, PMU_VRF_1_4V);
    } else {
        pmu_module_set_volt(PMU_RF, PMU_VRF_1_2V, PMU_VRF_1_4V);
    }
    pmu_module_config(PMU_RF,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON); //enable VRF for psramuhs
#else
    pmu_module_config(PMU_RF,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_OFF,PMU_DSLEEP_MODE_OFF); //disable VRF
#endif

#ifdef DIG_DCDC_MODE
    pmu_power_mode = PMU_POWER_MODE_DIG_DCDC;
#elif defined(ANA_DCDC_MODE)
    pmu_power_mode = PMU_POWER_MODE_ANA_DCDC;
#else
    pmu_power_mode = PMU_POWER_MODE_LDO;
#endif

#ifndef PROGRAMMER
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_WARM_BOOT);
    pmu_vcore_set_high_volt();
#endif

#ifndef SIMU
    hal_sys_timer_delay(MS_TO_TICKS(10));
#endif
#endif // PMU_INIT || (!FPGA && !PROGRAMMER)

    return 0;
}

void pmu_sleep(void)
{
    uint16_t val;

    if (dcdc_ramp_map & DCDC_RAMP_MAP_VCORE) {
        pmu_read(PMU_REG_DCDC_DIG_CFG, &val);
        val |= REG_BUCK_VCORE_RAMP_EN;
        pmu_write(PMU_REG_DCDC_DIG_CFG, val);
    }
    if (dcdc_ramp_map & DCDC_RAMP_MAP_VANA) {
        pmu_read(PMU_REG_DCDC_ANA_CFG, &val);
        val |= REG_BUCK_ANA_RAMP_EN;
        pmu_write(PMU_REG_DCDC_ANA_CFG, val);
    }
    if (dcdc_ramp_map & DCDC_RAMP_MAP_VHPPA) {
        pmu_read(PMU_REG_DCDC_HPPA_CFG, &val);
        val |= REG_BUCK_HPPA_RAMP_EN;
        pmu_write(PMU_REG_DCDC_HPPA_CFG, val);
    }

    return;
}

void pmu_wakeup(void)
{
    uint16_t val;

    if (dcdc_ramp_map & DCDC_RAMP_MAP_VCORE) {
        pmu_read(PMU_REG_DCDC_DIG_CFG, &val);
        val &= ~REG_BUCK_VCORE_RAMP_EN;
        pmu_write(PMU_REG_DCDC_DIG_CFG, val);
    }
    if (dcdc_ramp_map & DCDC_RAMP_MAP_VANA) {
        pmu_read(PMU_REG_DCDC_ANA_CFG, &val);
        val &= ~REG_BUCK_ANA_RAMP_EN;
        pmu_write(PMU_REG_DCDC_ANA_CFG, val);
    }
    if (dcdc_ramp_map & DCDC_RAMP_MAP_VHPPA) {
        pmu_read(PMU_REG_DCDC_HPPA_CFG, &val);
        val &= ~REG_BUCK_HPPA_RAMP_EN;
        pmu_write(PMU_REG_DCDC_HPPA_CFG, val);
    }

    return;
}

void pmu_codec_config(int enable)
{
    if (vcodec_off) {
#if !defined(SIMU) && (defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER)))
#ifdef VMEM_ON
        pmu_hppa_en(enable);
#endif
#endif
    } else {
        if (enable) {
            pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        } else {
            pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        }
    }
}

void pmu_codec_hppa_enable(int enable)
{
    if (!vcodec_off) {
#ifdef VMEM_ON
        pmu_hppa_en(enable);
#endif
    }
}

void pmu_codec_mic_bias_enable(uint32_t map)
{
    uint16_t val_a, val_b;

    // Increase LDO_RES if higher vmic is required
    val_a = REG_MIC_LDO_RES(PMU_VMIC_RES_3_3V);

    val_b = 0;

    if (map & AUD_VMIC_MAP_VMIC1) {
        val_a |= REG_MIC_BIASA_EN | REG_MIC_BIASA_ENLPF |
#ifdef DIGMIC_HIGH_VOLT
        REG_MIC_BIASA_VSEL(PMU_VMIC_3_0V)
#else
        REG_MIC_BIASA_VSEL(PMU_VMIC_2_0V)
#endif
        ;
        val_b |= REG_MIC_LDO_EN;
    }
    if (map & AUD_VMIC_MAP_VMIC2) {
        val_b |= REG_MIC_BIASB_EN | REG_MIC_BIASB_ENLPF |
#ifdef DIGMIC_HIGH_VOLT
        REG_MIC_BIASB_VSEL(PMU_VMIC_3_0V) |
#else
        REG_MIC_BIASB_VSEL(PMU_VMIC_2_0V) |
#endif
        REG_MIC_LDO_EN;
    }
    pmu_write(PMU_REG_MIC_BIAS_A, val_a);
    pmu_write(PMU_REG_MIC_BIAS_B, val_b);

}

void pmu_codec_mic_bias_lowpower_mode(uint32_t map)
{
}

void pmu_codec_adc_pre_start(void)
{
}

void pmu_codec_adc_post_start(void (*delay_ms)(uint32_t))
{
}

void pmu_codec_dac_pre_start(void)
{
}

void pmu_codec_dac_post_start(void (*delay_ms)(uint32_t))
{
}

void pmu_flash_write_config(void)
{
#ifdef FLASH_WRITE_AT_HIGH_VCORE
    uint32_t lock;

    if (pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req |= PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void pmu_flash_read_config(void)
{
#ifdef FLASH_WRITE_AT_HIGH_VCORE
    uint32_t lock;

    if ((pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) == 0) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req &= ~PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_flash_freq_config(uint32_t freq)
{
#if !defined(SIMU) && (defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER)))
    uint32_t lock;

    lock = int_lock();
    pmu_vcore_req &= ~(PMU_VCORE_FLASH_FREQ_HIGH | PMU_VCORE_FLASH_FREQ_MEDIUM);
    if (freq > 52000000) {
        // The real max freq is 120M
        pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_HIGH;
    } else {
        // The real max freq is about 90~100M. It seems to work at VCORE 0.9V.
        pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_MEDIUM;
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void pmu_anc_config(int enable)
{
}

void pmu_fir_high_speed_config(int enable)
{
}

void pmu_iir_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    pmu_vcore_req &= ~(PMU_VCORE_IIR_FREQ_HIGH | PMU_VCORE_IIR_FREQ_MEDIUM);
    if (freq >= 52000000) {
        pmu_vcore_req |= PMU_VCORE_IIR_FREQ_HIGH;
    } else if (freq > 26000000) {
        pmu_vcore_req |= PMU_VCORE_IIR_FREQ_MEDIUM;
    }
    int_unlock(lock);

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

void BOOT_TEXT_SRAM_LOC pmu_sys_freq_config(enum HAL_CMU_FREQ_T freq)
{
#if !defined(SIMU) && (defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER)))
    uint32_t lock;
    enum PMU_VCORE_REQ_T old_req;
    bool update = false;

    lock = int_lock();
    old_req = pmu_vcore_req;
    pmu_vcore_req &= ~(PMU_VCORE_SYS_FREQ_MEDIUM | PMU_VCORE_SYS_FREQ_HIGH | PMU_VCORE_SYS_FREQ_ULTRAHIGH);
    if (freq > HAL_CMU_FREQ_208M) {
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_ULTRAHIGH;
    } else if (freq > HAL_CMU_FREQ_104M) {
        // Real freq 192M
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_HIGH;
    } else if (freq == HAL_CMU_FREQ_104M) {
        // Real freq 104M
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
    } else {
        // 78M/52M/26M/32K
    }
    if (old_req != pmu_vcore_req) {
        update = true;
    }
    int_unlock(lock);

    if (!update) {
        // Nothing changes
        return;
    }

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void pmu_usb_config(enum PMU_USB_CONFIG_TYPE_T type)
{
    pmu_module_set_volt(PMU_IO, PMU_IO_3_3V, PMU_IO_3_3V);
#ifdef USB_HIGH_SPEED
    uint16_t val;
    wfrf_read(WFRF_REG_51, &val);
    val |= REG_WIFI_BBPLL_USB_EN;
    wfrf_write(WFRF_REG_51, val);

    if (pmu_metal_id <= HAL_CHIP_METAL_ID_5) {
        wfrf_read(WFRF_REG_56, &val);
        val |= 0x3FF;
        wfrf_write(WFRF_REG_56, val);
    }

    if (type == PMU_USB_CONFIG_TYPE_NONE) {
        pmu_vcore_req &= ~PMU_VCORE_USB_HS_ENABLED;
    } else {
        pmu_vcore_req |= PMU_VCORE_USB_HS_ENABLED;
    }
#endif
}
#if !defined(FPGA) && !defined(PROGRAMMER)
struct PMU_CHG_CTX_T {
    uint16_t pmu_chg_status;
};

struct PMU_CHG_CTX_T BOOT_BSS_LOC pmu_chg_ctx;

void BOOT_TEXT_SRAM_LOC pmu_charger_save_context(void)
{
    pmu_read(PMU_REG_CHARGER_STATUS, &pmu_chg_ctx.pmu_chg_status);
}

enum PMU_POWER_ON_CAUSE_T pmu_charger_poweron_status(void)
{
    enum PMU_POWER_ON_CAUSE_T pmu_power_on_cause = PMU_POWER_ON_CAUSE_NONE;

    if (pmu_chg_ctx.pmu_chg_status & AC_ON_DET_OUT){
        pmu_power_on_cause = PMU_POWER_ON_CAUSE_CHARGER_ACOFF;
    }else if (pmu_chg_ctx.pmu_chg_status & AC_ON){
        pmu_power_on_cause = PMU_POWER_ON_CAUSE_CHARGER_ACON;
    }
    return pmu_power_on_cause;
}

enum PMU_POWER_ON_CAUSE_T pmu_get_power_on_cause(void)
{
    enum PMU_POWER_ON_CAUSE_T pmu_power_on_cause = PMU_POWER_ON_CAUSE_NONE;

    pmu_power_on_cause = pmu_charger_poweron_status();
    if (pmu_power_on_cause == PMU_POWER_ON_CAUSE_NONE){
        pmu_power_on_cause = PMU_POWER_ON_CAUSE_POWER_KEY;
    }

    return pmu_power_on_cause;
}
#endif

void pmu_charger_init(void)
{
    unsigned short readval_cfg;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg &= ~(REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK |
        REG_AC_ON_OUT_EN | REG_AC_ON_IN_EN | REG_CHARGE_INTR_EN);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg |= REG_AC_ON_OUT_EN | REG_AC_ON_IN_EN | REG_CHARGE_INTR_EN;
    readval_cfg = SET_BITFIELD(readval_cfg, REG_AC_ON_DB_VALUE, 8);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);
}

static void pmu_charger_irq_handler(PMU_IRQ_HDLR_PARAM)
{
    enum PMU_CHARGER_STATUS_T status = PMU_CHARGER_UNKNOWN;
    unsigned short readval;

#ifdef PMU_IRQ_UNIFIED
    readval = irq_status;
#else
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    pmu_write(PMU_REG_CHARGER_STATUS, readval);
    int_unlock(lock);
#endif
    PMU_DEBUG_TRACE(3,"%s REG_%02X=0x%04X", __func__, PMU_REG_CHARGER_STATUS, readval);

    if ((readval & INTR_MSKED_CHARGE_MASK) == 0){
        PMU_DEBUG_TRACE(1,"%s SKIP", __func__);
        return;
    } else if ((readval & INTR_MSKED_CHARGE_MASK) ==
            INTR_MSKED_CHARGE_MASK) {
        PMU_DEBUG_TRACE(1,"%s DITHERING", __func__);
        hal_sys_timer_delay(2);
    } else {
        PMU_DEBUG_TRACE(1,"%s NORMAL", __func__);
    }

    status = pmu_charger_get_status();

    if (charger_irq_handler) {
        charger_irq_handler(status);
    }
}

void pmu_charger_set_irq_handler(PMU_CHARGER_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    charger_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &val);
    if (handler) {
        val |= REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK;
    } else {
        val &= ~(REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK);
    }
    pmu_write(PMU_REG_CHARGER_CFG, val);

#ifdef PMU_IRQ_UNIFIED
    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_CHARGER, handler ? pmu_charger_irq_handler : NULL);
#else
    if (handler) {
        NVIC_SetVector(CHARGER_IRQn, (uint32_t)pmu_charger_irq_handler);
        NVIC_SetPriority(CHARGER_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(CHARGER_IRQn);
        NVIC_EnableIRQ(CHARGER_IRQn);
    } else {
        NVIC_DisableIRQ(CHARGER_IRQn);
    }
#endif
    int_unlock(lock);
}

void pmu_charger_plugin_config(void)
{
    if (IO_VOLT_ACTIVE_RISE < PMU_IO_3_1V) {
        vio_act_rise = PMU_IO_3_1V;
    }
    if (IO_VOLT_SLEEP < PMU_IO_3_2V) {
        vio_lp = PMU_IO_3_2V;
    }
    pmu_viorise_req(PMU_VIORISE_REQ_USER_CHARGER, true);
}

void pmu_charger_plugout_config(void)
{
    vio_act_rise = IO_VOLT_ACTIVE_RISE;
    vio_lp = IO_VOLT_SLEEP;
    pmu_viorise_req(PMU_VIORISE_REQ_USER_CHARGER, false);
}

enum PMU_CHARGER_STATUS_T pmu_charger_get_status(void)
{
    unsigned short readval;
    enum PMU_CHARGER_STATUS_T status;

    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    if (readval & AC_ON)
        status = PMU_CHARGER_PLUGIN;
    else
        status = PMU_CHARGER_PLUGOUT;

    return status;
}

#ifdef RTC_ENABLE
void pmu_rtc_enable(void)
{
    uint16_t readval;
    uint32_t lock;

#ifdef SIMU
    // Set RTC counter to 1KHz
    pmu_write(PMU_REG_RTC_DIV_1HZ, 32 - 2);
#else
    // Set RTC counter to 1Hz
    pmu_write(PMU_REG_RTC_DIV_1HZ, CONFIG_SYSTICK_HZ * 2 - 2);
#endif

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval |= RTC_POWER_ON_EN | PU_LPO_DR | PU_LPO_REG;
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

void pmu_rtc_disable(void)
{
    uint16_t readval;
    uint32_t lock;

    pmu_rtc_clear_alarm();

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval &= ~(RTC_POWER_ON_EN | PU_LPO_DR);
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

int BOOT_TEXT_SRAM_LOC pmu_rtc_enabled(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);

    return !!(readval & RTC_POWER_ON_EN);
}

void pmu_rtc_set(uint32_t seconds)
{
    uint16_t high, low;

    // Need 3 seconds to load a new value
    seconds += 3;

    high = seconds >> 16;
    low = seconds & 0xFFFF;

    pmu_write(PMU_REG_RTC_LOAD_LOW, low);
    pmu_write(PMU_REG_RTC_LOAD_HIGH, high);
}

uint32_t pmu_rtc_get(void)
{
    uint16_t high, low, high2;

    pmu_read(PMU_REG_RTC_VAL_HIGH, &high);
    pmu_read(PMU_REG_RTC_VAL_LOW, &low);
    // Handle counter wrap
    pmu_read(PMU_REG_RTC_VAL_HIGH, &high2);
    if (high != high2) {
        high = high2;
        pmu_read(PMU_REG_RTC_VAL_LOW, &low);
    }

    return (high << 16) | low;
}

void pmu_rtc_set_alarm(uint32_t seconds)
{
    uint16_t readval;
    uint16_t high, low;
    uint32_t lock;

    // Need 1 second to raise the interrupt
    if (seconds > 0) {
        seconds -= 1;
    }

    high = seconds >> 16;
    low = seconds & 0xFFFF;

    pmu_write(PMU_REG_INT_CLR, RTC_INT1_MSKED);

    pmu_write(PMU_REG_RTC_MATCH1_LOW, low);
    pmu_write(PMU_REG_RTC_MATCH1_HIGH, high);

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval |= RTC_INT_EN_1;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);
}

uint32_t BOOT_TEXT_SRAM_LOC pmu_rtc_get_alarm(void)
{
    uint16_t high, low;

    pmu_read(PMU_REG_RTC_MATCH1_LOW, &low);
    pmu_read(PMU_REG_RTC_MATCH1_HIGH, &high);

    // Compensate the alarm offset
    return (uint32_t)((high << 16) | low) + 1;
}

void pmu_rtc_clear_alarm(void)
{
    uint16_t readval;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval &= ~RTC_INT_EN_1;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);

    pmu_write(PMU_REG_INT_CLR, RTC_INT1_MSKED);
}

int BOOT_TEXT_SRAM_LOC pmu_rtc_alarm_status_set(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_EN, &readval);

    return !!(readval & RTC_INT_EN_1);
}

int pmu_rtc_alarm_alerted()
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_STATUS, &readval);

    return !!(readval & RTC_INT_1);
}

static void pmu_rtc_irq_handler(PMU_IRQ_HDLR_PARAM)
{
    uint32_t seconds;
    bool alerted;

#ifdef PMU_IRQ_UNIFIED
    alerted = !!(irq_status & RTC_INT1_MSKED);
#else
    alerted = pmu_rtc_alarm_alerted();
#endif

    if (alerted) {
        pmu_rtc_clear_alarm();

        if (rtc_irq_handler) {
            seconds = pmu_rtc_get();
            rtc_irq_handler(seconds);
        }
    }
}

void pmu_rtc_set_irq_handler(PMU_RTC_IRQ_HANDLER_T handler)
{
    uint16_t readval;
    uint32_t lock;

    rtc_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_INT_MASK, &readval);
    if (handler) {
        readval |= RTC_INT1_MSK;
    } else {
        readval &= ~RTC_INT1_MSK;
    }
    pmu_write(PMU_REG_INT_MASK, readval);

#ifdef PMU_IRQ_UNIFIED
    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_RTC, handler ? pmu_rtc_irq_handler : NULL);
#else
    if (handler) {
        NVIC_SetVector(RTC_IRQn, (uint32_t)pmu_rtc_irq_handler);
        NVIC_SetPriority(RTC_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(RTC_IRQn);
        NVIC_EnableIRQ(RTC_IRQn);
    } else {
        NVIC_DisableIRQ(RTC_IRQn);
    }
#endif
    int_unlock(lock);
}
#endif

#ifdef PMU_IRQ_UNIFIED
static void pmu_general_irq_handler(void)
{
    uint32_t lock;
    uint16_t val;
    bool pwrkey, charger, gpadc, rtc, gpio, wdt;

    pwrkey = false;
    charger = false;
    gpadc = false;
    rtc = false;
    usbpin = false;
    gpio = false;
    wdt = false;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_STATUS, &val);
    if (val & (POWER_ON_RELEASE | POWER_ON_PRESS)) {
        pwrkey = true;
    }
    if (val & INTR_MSKED_CHARGE_MASK) {
        charger = true;
    }
    if (pwrkey || charger) {
        pmu_write(PMU_REG_CHARGER_STATUS, val);
    }
    int_unlock(lock);

    if (charger) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_CHARGER]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_CHARGER](val);
        }
    }

    lock = int_lock();
    pmu_read(PMU_REG_INT_MSKED_STATUS, &val);
    if (hal_gpadc_masked_irq_valid(val)) {
        val = hal_gpadc_filter_out_unmasked_irq(val);
        gpadc = true;
    }
    if (val & (RTC_INT_CLR_1 | RTC_INT_CLR_0)) {
        rtc = true;
    }

    if (gpadc || rtc) {
        pmu_write(PMU_REG_INT_CLR, val);
    }
    int_unlock(lock);

    if (gpadc) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_GPADC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_GPADC](val);
        }
    }
    if (rtc) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC](val);
        }
    }

    lock = int_lock();
    pmu_read(PMU_REG_LED_IO_IN, &val);
    if (val & (PMU_GPIO_INTR_MSKED1 | PMU_GPIO_INTR_MSKED2)) {
        gpio = true;
    }
    if (val & WDT_INTR_MSKED) {
        wdt = true;
    }
    if (gpio || wdt) {
        uint16_t clr;

        clr = val;
        if (wdt) {
            clr |= REG_WDT_INTR_CLR;
        }
        pmu_write(PMU_REG_LED_IO_IN, clr);
    }
    int_unlock(lock);

    if (gpio) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_GPIO]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_GPIO](val);
        }
    }
    if (wdt) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_WDT]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_WDT](val);
        }
    }
}

int pmu_set_irq_unified_handler(enum PMU_IRQ_TYPE_T type, PMU_IRQ_UNIFIED_HANDLER_T hdlr)
{
    bool update;
    uint32_t lock;
    int i;

    if (type >= PMU_IRQ_TYPE_QTY) {
        return 1;
    }

    enum PMU_REG_T reg;
    uint16_t val;
    uint16_t mask;

    if (type == PMU_IRQ_TYPE_GPADC) {
        reg = PMU_REG_INT_MERGE_MASK1;
        mask = GPADC_INTR_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_RTC) {
        reg = PMU_REG_INT_MERGE_MASK1;
        mask = RTC_INTR_TMP_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_CHARGER) {
        reg = PMU_REG_INT_MERGE_MASK1;
        mask = CHARGE_INTR_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_GPIO) {
        reg = PMU_REG_INT_MERGE_MASK2;
        mask = PMU_GPIO_INTR_MSKED1_MERGED_MSK | PMU_GPIO_INTR_MSKED2_MERGED_MSK;
    } else if (type == PMU_IRQ_TYPE_WDT) {
        reg = PMU_REG_INT_MERGE_MASK2;
        mask = WDT_INTR_MSKED_MERGED_MSK;
    } else {
        return 2;
    }

    update = false;

    lock = int_lock();

    for (i = 0; i < PMU_IRQ_TYPE_QTY; i++) {
        if (pmu_irq_hdlrs[i]) {
            break;
        }
    }

    pmu_irq_hdlrs[type] = hdlr;

    pmu_read(reg, &val);
    if (hdlr) {
        val |= mask;
    } else {
        val &= ~mask;
    }
    pmu_write(reg, val);

    if (hdlr) {
        update = (i >= PMU_IRQ_TYPE_QTY);
    } else {
        if (i == type) {
            for (; i < PMU_IRQ_TYPE_QTY; i++) {
                if (pmu_irq_hdlrs[i]) {
                    break;
                }
            }
            update = (i >= PMU_IRQ_TYPE_QTY);
        }
    }

    if (update) {
        if (hdlr) {
            NVIC_SetVector(RTC_IRQn, (uint32_t)pmu_general_irq_handler);
            NVIC_SetPriority(RTC_IRQn, IRQ_PRIORITY_NORMAL);
            NVIC_ClearPendingIRQ(RTC_IRQn);
            NVIC_EnableIRQ(RTC_IRQn);
        } else {
            NVIC_DisableIRQ(RTC_IRQn);
        }
    }

    int_unlock(lock);

    return 0;
}
#endif

void pmu_viorise_req(enum PMU_VIORISE_REQ_USER_T user, bool rise)
{
    uint32_t lock;

    lock = int_lock();
    if (rise) {
        if (vio_risereq_map == 0) {
            pmu_module_set_volt(PMU_IO,vio_lp,vio_act_rise);
        }
        vio_risereq_map |= (1 << user);
    } else {
        vio_risereq_map &= ~(1 << user);
        if (vio_risereq_map == 0) {
            pmu_module_set_volt(PMU_IO,vio_lp,vio_act_normal);
        }
    }
    int_unlock(lock);
}

int pmu_debug_config_ana(uint16_t volt)
{
    return 0;
}

int pmu_debug_config_codec(uint16_t volt)
{
    return 0;
}

int pmu_debug_config_vcrystal(bool on)
{
    return 0;
}

int pmu_debug_config_audio_output(bool diff)
{
    return 0;
}

void pmu_debug_reliability_test(int stage)
{
    uint16_t volt;

    if (stage == 0) {
        volt = PMU_DCDC_ANA_1_8V;
    } else {
        volt = PMU_DCDC_ANA_1_9V;
    }
    pmu_dcdc_ana_set_volt(volt, ana_lp_dcdc);
}

void pmu_led_set_direction(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_DIR_T dir)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_GPIO_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            val |= REG_LED_IO1_PU;
            if (dir == HAL_GPIO_DIR_IN) {
                val |= REG_LED_IO1_OENB_PRE | REG_LED_IO1_RX_EN;
            } else {
                val &= ~(REG_LED_IO1_OENB_PRE | REG_LED_IO1_RX_EN);
            }
            pmu_write(PMU_REG_LED_CFG_IO1, val);
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            val |= REG_LED_IO2_PU;
            if (dir == HAL_GPIO_DIR_IN) {
                val |= REG_LED_IO2_OENB_PRE | REG_LED_IO2_RX_EN;
            } else {
                val &= ~(REG_LED_IO2_OENB_PRE | REG_LED_IO2_RX_EN);
            }
            pmu_write(PMU_REG_LED_CFG_IO2, val);
        }
        int_unlock(lock);
    }
}

enum HAL_GPIO_DIR_T pmu_led_get_direction(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        if (pin == HAL_GPIO_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            return (val & REG_LED_IO1_OENB_PRE) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            return (val & REG_LED_IO2_OENB_PRE) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
        }
    } else {
        return HAL_GPIO_DIR_IN;
    }
}

void pmu_led_set_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    enum PMU_LED_VOLT_T {
        PMU_LED_VOLT_VBAT,
        PMU_LED_VOLT_VMEM,
        PMU_LED_VOLT_VIO,
    };
    enum PMU_LED_VOLT_T sel;
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        if (volt == HAL_IOMUX_PIN_VOLTAGE_VIO) {
            sel = PMU_LED_VOLT_VIO;
        } else if (volt == HAL_IOMUX_PIN_VOLTAGE_MEM) {
            sel = PMU_LED_VOLT_VMEM;
        } else {
            sel = PMU_LED_VOLT_VBAT;
        }

        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            val = SET_BITFIELD(val, REG_LED_IO1_SEL, sel);
            pmu_write(PMU_REG_LED_CFG_IO1, val);
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            val = SET_BITFIELD(val, REG_LED_IO2_SEL, sel);
            pmu_write(PMU_REG_LED_CFG_IO2, val);
        }
        int_unlock(lock);
    }
}

void pmu_led_set_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_read(PMU_REG_LED_CFG_IO1, &val);
            val &= ~(REG_LED_IO1_PDEN | REG_LED_IO1_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                val |= REG_LED_IO1_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                val |= REG_LED_IO1_PDEN;
            }
            pmu_write(PMU_REG_LED_CFG_IO1, val);
        } else {
            pmu_read(PMU_REG_LED_CFG_IO2, &val);
            val &= ~(REG_LED_IO2_PDEN | REG_LED_IO2_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                val |= REG_LED_IO2_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                val |= REG_LED_IO2_PDEN;
            }
            pmu_write(PMU_REG_LED_CFG_IO2, val);
        }
        int_unlock(lock);
    }
}

void pmu_led_set_value(enum HAL_GPIO_PIN_T pin, int data)
{
    uint32_t lock;
    uint16_t val;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_GPIO_PIN_LED1) {
#ifdef PMU_LED_VIA_PWM
            pmu_write(PMU_REG_PWM2_TOGGLE, 0xFFFF);
            pmu_read(PMU_REG_PWM2_BR_EN, &val);
            val &= ~REG_PWM2_BR_EN;
            val |= PWM_SELECT_EN;
            if (data) {
                val |= PWM_SELECT_INV;
            } else {
                val &= ~PWM_SELECT_INV;
            }
            pmu_write(PMU_REG_PWM2_BR_EN, val);
#else
            pmu_read(PMU_REG_PWM2_EN, &val);
            if (data) {
                val |= REG_LED0_OUT;
            } else {
                val &= ~REG_LED0_OUT;
            }
            pmu_write(PMU_REG_PWM2_EN, val);
#endif
        }else {
#ifdef PMU_LED_VIA_PWM
            pmu_write(PMU_REG_PWMB_TOGGLE, 0xFFFF);
            pmu_read(PMU_REG_PWMB_BR_EN, &val);
            val &= ~REG_PWMB_BR_EN;
            val |= PWMB_SELECT_EN;
            if (data) {
                val |= PWMB_SELECT_INV;
            } else {
                val &= ~PWMB_SELECT_INV;
            }
            pmu_write(PMU_REG_PWMB_BR_EN, val);
#else
            pmu_read(PMU_REG_PWMB_EN, &val);
            if (data) {
                val |= REG_LED1_OUT;
            } else {
                val &= ~REG_LED1_OUT;
            }
            pmu_write(PMU_REG_PWMB_EN, val);
#endif
        }
        int_unlock(lock);
    }
}

int pmu_led_get_value(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;
    int data = 0;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        pmu_read(PMU_REG_LED_IO_IN, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
            data = LED_IO1_IN_DB;
        } else {
            data = LED_IO2_IN_DB;
        }
        data &= val;
    }

    return !!data;
}

void pmu_led_uart_enable(enum HAL_IOMUX_PIN_T pin)
{
    uint16_t val;
    if (pin == HAL_IOMUX_PIN_LED1) {
        pmu_read(PMU_REG_UART1_CFG, &val);
        val &= ~(REG_UART_LEDA_SEL | REG_PMU_UART_DR1);
        val |=  REG_GPIO_I_SEL;
        pmu_write(PMU_REG_UART1_CFG, val);

        pmu_read(PMU_REG_UART2_CFG, &val);
        val |= REG_PMU_UART_DR2;
        pmu_write(PMU_REG_UART2_CFG, val);

        pmu_read(PMU_REG_LED_CFG_IO1, &val);
        val |= REG_LED_IO1_PUEN;
        pmu_write(PMU_REG_LED_CFG_IO1, val);
    }else{
        pmu_read(PMU_REG_UART1_CFG, &val);
        val &= ~ REG_GPIO_I_SEL;
        val |= REG_PMU_UART_DR1;
        pmu_write(PMU_REG_UART1_CFG, val);

        pmu_read(PMU_REG_UART2_CFG, &val);
        val &= ~(REG_UART_LEDB_SEL | REG_PMU_UART_DR2);
        pmu_write(PMU_REG_UART2_CFG, val);

        pmu_read(PMU_REG_LED_CFG_IO2, &val);
        val |= REG_LED_IO2_PUEN;
        pmu_write(PMU_REG_LED_CFG_IO2, val);
    }
}

void pmu_led_uart_disable(enum HAL_IOMUX_PIN_T pin)
{
    uint16_t val;
    if (pin == HAL_IOMUX_PIN_LED1) {
        pmu_read(PMU_REG_UART1_CFG, &val);
        val |= (REG_UART_LEDA_SEL | REG_PMU_UART_DR1);
        val &=  ~REG_GPIO_I_SEL;
        pmu_write(PMU_REG_UART1_CFG, val);

        pmu_read(PMU_REG_UART2_CFG, &val);
        val &= ~REG_PMU_UART_DR2;
        pmu_write(PMU_REG_UART2_CFG, val);

        pmu_read(PMU_REG_LED_CFG_IO1, &val);
        val &= ~REG_LED_IO1_PUEN;
        pmu_write(PMU_REG_LED_CFG_IO1, val);
    }else{
        pmu_read(PMU_REG_UART1_CFG, &val);
        val |= REG_GPIO_I_SEL;
        val &= ~REG_PMU_UART_DR1;
        pmu_write(PMU_REG_UART1_CFG, val);

        pmu_read(PMU_REG_UART2_CFG, &val);
        val |= (REG_UART_LEDB_SEL | REG_PMU_UART_DR2);
        pmu_write(PMU_REG_UART2_CFG, val);

        pmu_read(PMU_REG_LED_CFG_IO2, &val);
        val &= ~REG_LED_IO2_PUEN;
        pmu_write(PMU_REG_LED_CFG_IO2, val);
    }
}

void pmu_led_breathing_enable(enum HAL_IOMUX_PIN_T pin, const struct PMU_LED_BR_CFG_T *cfg)
{
    uint32_t st1;
    uint32_t st2;
    uint32_t subcnt_data;
    uint8_t tg;
    uint16_t val;
    uint32_t lock;
    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        st1 = MS_TO_TICKS(cfg->off_time_ms);
        if (st1 > 0xFFFF) {
            st1 = 0xFFFF;
        }
        st2 = MS_TO_TICKS(cfg->on_time_ms);
        if (st2 > 0xFFFF) {
            st2 = 0xFFFF;
        }
        subcnt_data = MS_TO_TICKS(cfg->fade_time_ms);
        subcnt_data = integer_sqrt_nearest(subcnt_data);
        if (subcnt_data > (SUBCNT_DATA2_MASK >> SUBCNT_DATA2_SHIFT)) {
            subcnt_data = (SUBCNT_DATA2_MASK >> SUBCNT_DATA2_SHIFT);
        }
        // TODO: Keep compatible with digital PWM module (can be removed after 2500)
        if (subcnt_data > 0xFE) {
            subcnt_data = 0xFE;
        }
        tg = 1;

        pmu_led_set_direction((enum HAL_GPIO_PIN_T)pin, HAL_GPIO_DIR_OUT);
        pmu_led_set_pull_select(pin, HAL_IOMUX_PIN_NOPULL);
        pmu_led_set_voltage_domains(pin, HAL_IOMUX_PIN_VOLTAGE_VBAT);

        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_write(PMU_REG_PWM2_TOGGLE, st2);
            pmu_write(PMU_REG_PWM2_ST1, st1);
            val = SUBCNT_DATA2(subcnt_data) | TG_SUBCNT_D2_ST(tg);
            pmu_write(PMU_REG_PWM2_EN, val);
            pmu_read(PMU_REG_PWM2_BR_EN, &val);
            val = (val & ~REG_CLK_PWM_DIV_MASK) | REG_CLK_PWM_DIV(0) |
                REG_PWM_CLK_EN | REG_PWM2_BR_EN | PWM_SELECT_EN;
            pmu_write(PMU_REG_PWM2_BR_EN, val);
        } else {
            pmu_write(PMU_REG_PWMB_TOGGLE, st2);
            pmu_write(PMU_REG_PWMB_ST1, st1);
            val = SUBCNT_DATAB(subcnt_data) | TG_SUBCNT_DB_ST(tg);
            pmu_write(PMU_REG_PWMB_EN, val);
            pmu_read(PMU_REG_PWMB_BR_EN, &val);
            val = (val & ~REG_CLK_PWMB_DIV_MASK) | REG_CLK_PWMB_DIV(0) |
                REG_PWMB_CLK_EN | REG_PWMB_BR_EN | PWMB_SELECT_EN;
            pmu_write(PMU_REG_PWMB_BR_EN, val);
        }
        int_unlock(lock);
    }
}

void pmu_led_breathing_disable(enum HAL_IOMUX_PIN_T pin)
{
    uint16_t val;
    uint32_t lock;
    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        lock = int_lock();
        if (pin == HAL_IOMUX_PIN_LED1) {
            pmu_read(PMU_REG_PWM2_BR_EN, &val);
            val &= ~(REG_PWM_CLK_EN | REG_PWM2_BR_EN | PWM_SELECT_EN);
            pmu_write(PMU_REG_PWM2_BR_EN, val);
        } else {
            pmu_read(PMU_REG_PWMB_BR_EN, &val);
            val &= ~(REG_PWMB_CLK_EN | REG_PWMB_BR_EN | PWMB_SELECT_EN);
            pmu_write(PMU_REG_PWMB_BR_EN, val);
        }
        int_unlock(lock);
        pmu_led_set_direction((enum HAL_GPIO_PIN_T)pin, HAL_GPIO_DIR_IN);
        pmu_led_set_pull_select(pin, HAL_IOMUX_PIN_PULLUP_ENABLE);
    }
}

#ifdef PMU_IRQ_UNIFIED
static void pmu_gpio_irq_handler(uint16_t irq_status)
{
    if (irq_status & PMU_GPIO_INTR_MSKED1) {
        if (gpio_irq_handler[0]) {
            gpio_irq_handler[0](HAL_GPIO_PIN_LED1);
        }
    }
    if (irq_status & PMU_GPIO_INTR_MSKED2) {
        if (gpio_irq_handler[1]) {
            gpio_irq_handler[1](HAL_GPIO_PIN_LED2);
        }
    }
}

uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg)
{
    uint32_t lock;
    uint16_t val;
    bool old_en;

    if (pin != HAL_GPIO_PIN_LED1 && pin != HAL_GPIO_PIN_LED2) {
        return 1;
    }

    lock = int_lock();

    old_en = (gpio_irq_en[0] || gpio_irq_en[1]);

    if (pin == HAL_GPIO_PIN_LED1) {
        gpio_irq_en[0] = cfg->irq_enable;
        gpio_irq_handler[0] = cfg->irq_handler;

        if (cfg->irq_enable) {
        }

        pmu_read(PMU_REG_UART1_CFG, &val);
        if (cfg->irq_enable) {
            val |= REG_PMU_GPIO_INTR_MSK1 | REG_PMU_GPIO_INTR_EN1;
            if (cfg->irq_debounce) {
                val &= ~PMU_DB_BYPASS1;
            } else {
                val |= PMU_DB_BYPASS1;
            }
        } else {
            val &= ~(REG_PMU_GPIO_INTR_MSK1 | REG_PMU_GPIO_INTR_EN1);
        }
        pmu_write(PMU_REG_UART1_CFG, val);
    } else {
        gpio_irq_en[1] = cfg->irq_enable;
        gpio_irq_handler[1] = cfg->irq_handler;

        pmu_read(PMU_REG_UART2_CFG, &val);
        if (cfg->irq_enable) {
            val |= REG_PMU_GPIO_INTR_MSK2 | REG_PMU_GPIO_INTR_EN2;
            if (cfg->irq_debounce) {
                val &= ~PMU_DB_BYPASS2;
            } else {
                val |= PMU_DB_BYPASS2;
            }
        } else {
            val &= ~(REG_PMU_GPIO_INTR_MSK1 | REG_PMU_GPIO_INTR_EN1);
        }
        pmu_write(PMU_REG_UART2_CFG, val);
    }

    if (cfg->irq_enable) {
        uint16_t type;
        uint16_t pol;

        type = (pin == HAL_GPIO_PIN_LED1) ? REG_EDGE_INTR_SEL1 : REG_EDGE_INTR_SEL2;
        pol = (pin == HAL_GPIO_PIN_LED1) ? REG_POS_INTR_SEL1 : REG_POS_INTR_SEL2;
        pmu_read(PMU_REG_LED_INTR_SEL, &val);
        if (cfg->irq_type == HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE) {
            val |= type;
            if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_LOW_FALLING) {
                val &= ~pol;
            } else {
                val |= pol;
            }
        } else {
            val &= ~type;
        }
        pmu_write(PMU_REG_LED_INTR_SEL, val);

        if (cfg->irq_type != HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE) {
            if (pin == HAL_GPIO_PIN_LED1) {
                pmu_read(PMU_REG_WDT_INT_CFG, &val);
                if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_LOW_FALLING) {
                    val |= REG_LOW_LEVEL_INTR_SEL1;
                } else {
                    val &= ~REG_LOW_LEVEL_INTR_SEL1;
                }
                pmu_write(PMU_REG_WDT_INT_CFG, val);
            } else {
                pmu_read(PMU_REG_UART2_CFG, &val);
                if (cfg->irq_polarity == HAL_GPIO_IRQ_POLARITY_LOW_FALLING) {
                    val |= REG_LOW_LEVEL_INTR_SEL2;
                } else {
                    val &= ~REG_LOW_LEVEL_INTR_SEL2;
                }
                pmu_write(PMU_REG_UART2_CFG, val);
            }
        }
    }

    if (old_en != cfg->irq_enable) {
        pmu_set_irq_unified_handler(PMU_IRQ_TYPE_GPIO, cfg->irq_enable ? pmu_gpio_irq_handler : NULL);
    }

    int_unlock(lock);

    return 0;
}

static void pmu_wdt_irq_handler(uint16_t irq_status)
{
    if (wdt_irq_handler) {
        wdt_irq_handler();
    }
}

void pmu_wdt_set_irq_handler(PMU_WDT_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();

    wdt_irq_handler = handler;

    pmu_read(PMU_REG_WDT_INT_CFG, &val);
    if (handler) {
        val |= REG_WDT_INTR_EN | REG_WDT_INTR_MSK;
    } else {
        val &= ~(REG_WDT_INTR_EN | REG_WDT_INTR_MSK);
    }
    pmu_write(PMU_REG_WDT_INT_CFG, val);

    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_WDT, handler ? pmu_wdt_irq_handler : NULL);

    int_unlock(lock);
}
#else
uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg)
{
    ASSERT(false, "PMU_IRQ_UNIFIED must defined to use PMU GPIO IRQ");
    return 1;
}
void pmu_wdt_set_irq_handler(PMU_WDT_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();

    pmu_read(PMU_REG_WDT_INT_CFG, &val);
    if (handler) {
        val |= REG_WDT_INTR_EN | REG_WDT_INTR_MSK;
    } else {
        val &= ~(REG_WDT_INTR_EN | REG_WDT_INTR_MSK);
    }
    pmu_write(PMU_REG_WDT_INT_CFG, val);

    int_unlock(lock);
}
#endif

int pmu_wdt_config(uint32_t irq_ms, uint32_t reset_ms)
{
    if (irq_ms > 0xFFFF) {
        return 1;
    }
    if (reset_ms > 0xFFFF) {
        return 1;
    }
    wdt_irq_timer = irq_ms;
    wdt_reset_timer = reset_ms;

    pmu_write(PMU_REG_WDT_IRQ_TIMER, wdt_irq_timer);
    pmu_write(PMU_REG_WDT_RESET_TIMER, wdt_reset_timer);

    return 0;
}

void pmu_wdt_start(void)
{
    uint16_t val;

    if (wdt_irq_timer == 0 && wdt_reset_timer == 0) {
        return;
    }

    pmu_read(PMU_REG_WDT_CFG, &val);
    val |= (REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val);
}

void pmu_wdt_stop(void)
{
    uint16_t val;

    pmu_read(PMU_REG_WDT_CFG, &val);
    val &= ~(REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val);

    pmu_read(PMU_REG_LED_IO_IN, &val);
    val |= REG_WDT_INTR_CLR;
    pmu_write(PMU_REG_LED_IO_IN, val);
}

void pmu_wdt_feed(void)
{
    if (wdt_irq_timer == 0 && wdt_reset_timer == 0) {
        return;
    }

    pmu_write(PMU_REG_WDT_IRQ_TIMER, wdt_irq_timer);
    pmu_write(PMU_REG_WDT_RESET_TIMER, wdt_reset_timer);
}

void pmu_ntc_capture_enable(void)
{
}

void pmu_ntc_capture_disable(void)
{
}

void pmu_wf_tx_config(uint8_t band)
{
    uint16_t val;
    uint16_t volt;

    //0x69 bit[9:6] 0xc, 20210621 FengChong, 2g and 5g VPA 3.3v
    pmu_read(0x69, &val);
    volt = (val >> 6) & 0xF;

    while (volt < 0xC) {
        ++ volt;
        val &= ~(0xF << 6);
        val |= (volt << 6);
        pmu_write(0x69, val);
        hal_sys_timer_delay_us(100);
    }
    while (volt > 0xC) {
        -- volt;
        val &= ~(0xF << 6);
        val |= (volt << 6);
        pmu_write(0x69, val);
        hal_sys_timer_delay_us(100);
    }

    pmu_read(0x6a, &val);
    volt = (val >> 6) & 0xF;
    while (volt < 0x4) {
        ++ volt;
        val &= ~(0xF << 6);
        val |= (volt << 6);
        pmu_write(0x6a, val);
        hal_sys_timer_delay_us(100);
    }
    while (volt > 0x4) {
        -- volt;
        val &= ~(0xF << 6);
        val |= (volt << 6);
        pmu_write(0x6a, val);
        hal_sys_timer_delay_us(100);
    }
}

void pmu_dcdc_config(void)
{
    ///WARNIG: DO NOT change vana/vcore/vhppa here
    // u16 val_u16;
#if WIFI_RF_XIERENZHONG
    // pmu_read(0x15, &val_u16);//Vana 1.40v; 20210527 xierenzhong
    // val_u16 &= ~(0xff << 0);
    // val_u16 |= (0x6b << 0);
    // pmu_write(0x15, val_u16);
#endif
}

#define BT_RCOSC_CAL

#ifdef BT_RCOSC_CAL

POSSIBLY_UNUSED static int16_t RF_39F_INTI = 0;
POSSIBLY_UNUSED static bool initial = false;

POSSIBLY_UNUSED int16_t pmu_get_rcosc_cal_cnt_initial(void)
{
    int16_t tmp = 0;
    tmp = RF_39F_INTI;
    return tmp;
}

POSSIBLY_UNUSED
static void pmu_bt_xtal_startup_config(int startup_en)
{
}

POSSIBLY_UNUSED
static void pmu_bt_rcosc_cal(void)
{
}
#endif


void pmu_reboot_hook()
{
#ifdef DSP_ENABLE
    pmu_vcore_set_high_volt();
#endif
    hal_sys_timer_delay(MS_TO_TICKS(300));
}

void pmu_vcore_set_high_volt()
{
    uint32_t lock;

    lock = int_lock();

    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI) {
#ifdef DIG_DCDC_MODE
        pmu_ana_set_volt(0, PMU_POWER_MODE_DIG_DCDC);
        pmu_dig_set_volt(0, PMU_POWER_MODE_DIG_DCDC);
#elif defined(ANA_DCDC_MODE)
        pmu_ana_set_volt(0, PMU_POWER_MODE_ANA_DCDC);
        pmu_dig_set_volt(0, PMU_POWER_MODE_ANA_DCDC);
#endif
        vcore_high_volt = 1;
        int_unlock(lock);
        return;
    }

    vcore_high_volt = 1;

#ifdef DIG_DCDC_MODE
    uint16_t old_act_dcdc;
    uint16_t dcdc_volt = PMU_DCDC_DIG_1_0V;

    pmu_dcdc_dig_get_volt(&old_act_dcdc, NULL);
    if (old_act_dcdc < dcdc_volt) {
        while (old_act_dcdc++ < dcdc_volt) {
            pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
            hal_sys_timer_delay_us(200);
        }
    } else {
        while (old_act_dcdc-- > dcdc_volt) {
            pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
            hal_sys_timer_delay_us(200);
        }
    }

    pmu_ana_set_volt(0, PMU_POWER_MODE_DIG_DCDC);
#else
    uint16_t old_act_ldo;
    uint16_t ldo_volt = PMU_VDIG_1_0V;
    pmu_module_get_volt(PMU_DIG, &old_act_ldo, NULL);
    if (old_act_ldo < ldo_volt) {
        while (old_act_ldo++ < ldo_volt) {
            pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_act_ldo);
            hal_sys_timer_delay_us(200);
        }
    }
    pmu_dig_set_volt(0, PMU_POWER_MODE_ANA_DCDC);
#endif
    hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
    int_unlock(lock);
}

void pmu_vcore_set_normal_volt()
{
    uint16_t dcdc_volt;
    uint16_t ldo_volt;
    uint16_t old_dig_dcdc;
    uint16_t old_act_ldo;

    vcore_high_volt = 0;

    pmu_dig_get_target_volt(&ldo_volt, &dcdc_volt);

    if (pmu_power_mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_dig_get_volt(&old_dig_dcdc, NULL);
        uint32_t count = 0;
        while (old_dig_dcdc>dcdc_volt) {
            if (((count++) % 2) == 0) {
                if (old_dig_dcdc > dcdc_volt)
                    pmu_dcdc_dig_set_volt(--old_dig_dcdc, dig_lp_dcdc);
            }
            osDelay(1500);
        }
    } else {
        pmu_module_get_volt(PMU_DIG, &old_act_ldo, NULL);
        while (old_act_ldo-- > ldo_volt) {
            pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_act_ldo);
            osDelay(1500);
        }
    }
    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

uint16_t BOOT_BSS_LOC before_temperature_reg_tx_pa_v1;
uint16_t BOOT_BSS_LOC before_temperature_reg_bbpl;
int32_t BOOT_BSS_LOC g_wifi_ini_temperature;

/*
1. Dig_iptat_core_en=1  0x34 bit4
2. Reg_pu_avdd25_ana=1  0x0d bit1
3. Therm_gain=10  0x17 bit3 bit4
4. select channel0=1  0x1D bit0
5. Gpadc start=1  0x4f bit5
6. read channel0 value 0x56
function : enable tempreture sensor and record first temperature_reg
*/
static uint16_t BOOT_TEXT_FLASH_LOC pmu_read_temperature_adc(void)
{
    /* Warning : DO NOT add TRACE here */
    uint32_t cnt = 0;
    uint16_t  val_u16 = 0;
    uint16_t  currt_tmprtr_u16 = 0;

    //clear irq
    val_u16 = (0x1<<0);
    pmu_write(0x51, val_u16);

    //enable irq
    pmu_read(0x20, &val_u16);
    val_u16 |= (0x1<<0);
    pmu_write(0x20, val_u16);

    //enable ch0
    pmu_read(0x1d,&val_u16);
    val_u16 |= (0x1<<0);
    pmu_write(0x1d,val_u16);

    //enable gpadc
    pmu_read(0x4f,&val_u16);
    val_u16 |= (0x1<<5);
    pmu_write(0x4f,val_u16);

    while (true) {
        pmu_read(0x50,&val_u16);
        if (val_u16 & 0x1) //ch0 valid
            break;
        if (++cnt > 10)
            break;
        hal_sys_timer_delay(MS_TO_TICKS(1));
    }

    if (val_u16 & 0x1) { //ch0 valid
        pmu_read(0x56, &currt_tmprtr_u16);
    } else {
        currt_tmprtr_u16 = 21708; // timeout, return 25 degrees Celsius
    }

    //disable irq
    pmu_read(0x20, &val_u16);
    val_u16 &= ~(0x1<<0);
    pmu_write(0x20, val_u16);

    //clear valid and irq
    val_u16 = (0x1<<0);
    pmu_write(0x51, val_u16);

    //disable ch0
    pmu_read(0x1d, &val_u16);
    val_u16 &= ~(0x1<<0);
    pmu_write(0x1d, val_u16);

    return currt_tmprtr_u16;
}

#define HAL_GPADC_MVOLT_A                   -93360
#define HAL_GPADC_MVOLT_B                   -21454
#define HAL_GPADC_CALIB_DEFAULT_A           0x3789
#define HAL_GPADC_CALIB_DEFAULT_B           0x4950
static uint16_t temperature_25 = 21708;

static float g_adcSlope = 0;
static float g_adcIntcpt = 0;
static bool g_adcCalibrated = false;
static int pmu_temperature_adc2temp_calib(void)
{
    float y1, y2, x1, x2;
    uint16_t efuse_a = 0;
    uint16_t efuse_b = 0;

    if (!g_adcCalibrated)
    {
        y1 = (float)HAL_GPADC_MVOLT_A;
        y2 = (float)HAL_GPADC_MVOLT_B;

        pmu_get_efuse(PMU_EFUSE_PAGE_BATTER_LV, &efuse_a);

        x1 = (float)(efuse_a > 0 ? efuse_a : HAL_GPADC_CALIB_DEFAULT_A);

        pmu_get_efuse(PMU_EFUSE_PAGE_BATTER_HV, &efuse_b);
        x2 = (float)(efuse_b > 0 ? efuse_b : HAL_GPADC_CALIB_DEFAULT_B);

        g_adcSlope = (y2 - y1) / (x2 - x1);
        g_adcIntcpt = ((y1 * x2) - (x1 * y2)) / ((x2 - x1));
        g_adcCalibrated = true;

        TRACE(7,"%s efuse:%d/%d LV=%d, HV=%d, 1000*Slope=%d 1000*Intcpt=%d", __func__,
            efuse_a, efuse_b, (int32_t)x1, (int32_t)x2, (int32_t)(g_adcSlope*1000.0), (int32_t)(g_adcIntcpt*1000.0));
    }

    return 0;
}

static int32_t pmu_temperature_adc2temp(uint16_t gpadcVal)
{
    int32_t voltage;

    pmu_temperature_adc2temp_calib();
    if (temperature_25)
        voltage = (int32_t)(g_adcSlope * (gpadcVal-temperature_25)) + 25*1000;
    else
        voltage = (int32_t)((g_adcSlope * gpadcVal) + (g_adcIntcpt));
    return voltage;
}

static int32_t temperature_val;
volatile bool temperature_read_done = false;
static void pmu_adc_temperature_irqhandler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
#if 0
    int32_t abs_centigrade, abs_centigrade1, abs_centigrade2;
    abs_centigrade = 15.8 * irq_val - 317990;

    abs_centigrade1 = pmu_temperature_adc2temp(irq_val);
    abs_centigrade2 = (int32_t)((g_adcSlope * irq_val) + (g_adcIntcpt));
    TRACE(0, "pmu_adc_temperature_irqhandler\t%u\t%u\t%d\t%d\t%d", irq_val, volt, abs_centigrade, abs_centigrade1, abs_centigrade2);
#endif
    temperature_val = pmu_temperature_adc2temp(irq_val)/1000;
    temperature_read_done = true;
}

int32_t pmu_read_temperature(void)
{
    int ret;
    uint32_t cnt = 0;
    temperature_read_done = false;

retry:
    ret = hal_gpadc_open(0, HAL_GPADC_ATP_ONESHOT, pmu_adc_temperature_irqhandler);
    if (ret) {
        osDelay(1);
        if (++cnt > 10) { // timeout, return 25 degrees Celsius
            TRACE(0, "error, read temperature timeout0");
            return 25;
        }
        goto retry;
    }

    cnt = 0;
    while(!temperature_read_done) {
        if (++cnt > 10) { // timeout, return 25 degrees Celsius
            TRACE(0, "error, read temperature timeout1");
            return 25;
        }
        osDelay(1);
    }

    return temperature_val;
}

void BOOT_TEXT_FLASH_LOC pmu_temperature_sensor_init(void)
{
    /* Warning : DO NOT add TRACE here */

    uint16_t val_u16;

    pmu_get_efuse(PMU_EFUSE_PAGE_TEMP_CALI, &temperature_25);

    pmu_read(0x34, &val_u16);
    val_u16 &= ~((0x1<<4));
    val_u16 |= (0x1<<4);
    pmu_write(0x34, val_u16);

    pmu_read(0x0d, &val_u16);
    val_u16 &= ~((0x1<<1));
    val_u16 |= (0x1<<1);
    pmu_write(0x0d, val_u16);

    pmu_read(0x17, &val_u16);
    val_u16 &= ~((0x3<<3));
    val_u16 |= (0x2<<3);
    pmu_write(0x17, val_u16);

    hal_sys_timer_delay(MS_TO_TICKS(200));
}

uint16_t wifi_rf_read_temperature_2003()
{
    return pmu_read_temperature_adc();
}

void BOOT_TEXT_FLASH_LOC wifi_temperature_init(void)
{
    g_wifi_ini_temperature = pmu_read_temperature();

    before_temperature_reg_tx_pa_v1 = wifi_rf_read_temperature_2003();//first temperature_reg tx pa

    before_temperature_reg_bbpl = before_temperature_reg_tx_pa_v1;//first temperature_reg bbpl
}

void BOOT_TEXT_FLASH_LOC bbpl_temprt_comp_init_2003(void)
{
    uint16_t val_u16;

    wfrf_write(0x50,0x890b);

    // wfrf_read(0x51,&val_u16);
    // val_u16 |= (0x1 << 7);
    // wfrf_write(0x51,val_u16);

    //0xbb bit[15:13] 101; 20211125 XuLC
    wfrf_read(0xbb, &val_u16);
    val_u16 &= ~(0x7 << 13);
    val_u16 |= (0x5 << 13);
    wfrf_write(0xbb, val_u16);

    if (pmu_metal_id >= HAL_CHIP_METAL_ID_8){//2006
        wfrf_write(0x4f, 0x7f9e);
        wfrf_write(0x51, 0x1519);
    }
}

void BOOT_TEXT_FLASH_LOC bbpl_compensate_by_temprt_2003(void)
{
    /* Warning : DO NOT add TRACE here */

    uint16_t current_temperature;
    int32_t abs_centigrade; // absolute temperature
    uint16_t val_51;
    uint16_t val_4f;

    current_temperature = pmu_read_temperature_adc();
    //adc val to centigrade
    abs_centigrade = 158 * current_temperature - 3179900;
    abs_centigrade /= 10000;

#if WIFI_BBPL_TEMPERT_V2_DEBUG
    // TRACE(1, "%s Line%d abs_centigrade:%d", __FUNCTION__, __LINE__, abs_centigrade);
#endif

    wfrf_read(0x51,&val_51);

    wfrf_read(0x4f,&val_4f);

    if (abs_centigrade < 0)
    {
        val_51 |= (0x3 << 9);//0x51 bit[10:9]=11

        val_4f |= (0x3 << 6);//0x4f bit[7:6]=11
    }
    else if (abs_centigrade < 30)
    {
        val_51 &= ~(0x3 << 9);//0x51 bit[10:9]=10
        val_51 |= (0x2 << 9);

        val_4f &= ~(0x3 << 6);//0x4f bit[7:6]=10
        val_4f |= (0x2 << 6);
    }
    else if (abs_centigrade < 60)
    {
        val_51 &= ~(0x3 << 9);//0x51 bit[10:9]=01
        val_51 |= (0x1 << 9);

        val_4f &= ~(0x3 << 6);//0x4f bit[7:6]=01
        val_4f |= (0x1 << 6);
    }
    else
    {
        val_51 &= ~(0x3 << 9);//0x51 bit[10:9]=00

        val_4f &= ~(0x3 << 6);//0x4f bit[7:6]=00
    }

    if (pmu_metal_id < HAL_CHIP_METAL_ID_8){//2003
        wfrf_write(0x51,val_51);
        wfrf_write(0x4f,val_4f);
    }
    //reset bbpl
    wfrf_write(0xc0,0x400);
    hal_sys_timer_delay(MS_TO_TICKS(1));
    wfrf_write(0xc0,0xc00);
}

int pmu_get_efuse_wifi_cali(uint8_t *pa_pad_cap_m, uint8_t *pa_driver_a)
{
    int ret;
    uint16_t val;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_WIFI_5G_CALI, &val);
    if (ret) {
        TRACE(0, "%s, read efuse fail:%d", __func__, ret);
        return ret;
    }

    if (val & (1<<15)) {
        *pa_pad_cap_m = (uint8_t)((val >> 0) & 0x1F); // bit[4:0]
        *pa_driver_a = (uint8_t)((val >> 8) & 0x1f); // bit[12:8]
        return 0;
    } else {
        return -1;
    }
}

int pmu_set_efuse_wifi_cali(uint8_t pa_pad_cap_m, uint8_t pa_driver_a)
{
    int ret = 0;
    uint16_t val = 0, val_tmp = 0;
    uint16_t programming_bit[32];
    int i = 0;
    uint32_t programming_bit_len = 0;
    uint32_t efuse_bit_offset;

    TRACE(0, "%s, pa_pad_cap_m:0x%x, pa_driver_a:0x%x", __func__, pa_pad_cap_m, pa_driver_a);

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_WIFI_5G_CALI, &val);
    if (ret) {
        TRACE(0, "%s, read efuse fail:%d", __func__, ret);
        return ret;
    }

    if (val & (1<<15)) {
        TRACE(0, "%s, efuse has beed written, skip", __func__);
        return 0;
    }

    // pa_pad_cap_m, efuse[4:0]
    efuse_bit_offset = 0;
    while (pa_pad_cap_m) {
        if (pa_pad_cap_m & 0x1) {
            programming_bit[programming_bit_len++] = (efuse_bit_offset << 4) | PMU_EFUSE_PAGE_WIFI_5G_CALI;
            programming_bit[programming_bit_len++] = (0x1 << 8) | (efuse_bit_offset << 4) | PMU_EFUSE_PAGE_WIFI_5G_CALI;
        }
        pa_pad_cap_m >>= 1;
        efuse_bit_offset++;
    }

    // pa_driver_a, efuse[12:8]
    efuse_bit_offset = 8;
    while (pa_driver_a) {
        if (pa_driver_a & 0x1) {
            programming_bit[programming_bit_len++] = (efuse_bit_offset << 4) | PMU_EFUSE_PAGE_WIFI_5G_CALI;
            programming_bit[programming_bit_len++] = (0x1 << 8) | (efuse_bit_offset << 4) | PMU_EFUSE_PAGE_WIFI_5G_CALI;
        }
        pa_driver_a >>= 1;
        efuse_bit_offset++;
    }
    // efuse[15] = 1
    programming_bit[programming_bit_len++] = (15 << 4) | PMU_EFUSE_PAGE_WIFI_5G_CALI;
    programming_bit[programming_bit_len++] = (0x1 << 8) | (15 << 4) | PMU_EFUSE_PAGE_WIFI_5G_CALI;

    // Enable CLK_EN
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Enable TURN_ON
    val |= REG_EFUSE_TURN_ON;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }
    val_tmp = val;

    // write programming_bit
    for (i = 0; i < programming_bit_len; i++) {
        val = val_tmp;

        // Write Address
        val |= REG_EFUSE_ADDRESS(programming_bit[i]);
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        // Set Strobe Trigger = 1
        val |= REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        // set Strobe Trigger = 0
        val &= ~REG_EFUSE_STROBE_TRIGGER;
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }
        hal_sys_timer_delay(MS_TO_TICKS(10));
    }

_exit:
    // Disable TURN_ON
    val = REG_EFUSE_CLK_EN | REG_EFUSE_PGM_READ_SEL;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    // Disable CLK_EN
    val = 0;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    return ret;
}
