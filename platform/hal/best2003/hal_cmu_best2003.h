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
#ifndef __HAL_CMU_BEST2003_H__
#define __HAL_CMU_BEST2003_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FPGA
#define HAL_CMU_DEFAULT_CRYSTAL_FREQ        26000000
#define HAL_CMU_VALID_CRYSTAL_FREQ          { HAL_CMU_DEFAULT_CRYSTAL_FREQ, }
#else
#define HAL_CMU_DEFAULT_CRYSTAL_FREQ        24000000
#define HAL_CMU_VALID_CRYSTAL_FREQ          { 24000000, 40000000, }
#endif

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_MCU,          // 0
    HAL_CMU_MOD_H_CACHE0,       // 1
    HAL_CMU_MOD_H_CP,           // 2
    HAL_CMU_MOD_H_CACHE1,       // 3
    HAL_CMU_MOD_H_ADMA,         // 4
    HAL_CMU_MOD_H_GDMA,         // 5
    HAL_CMU_MOD_H_SEC_ENG,      // 6
    HAL_CMU_MOD_H_USBC,         // 7
    HAL_CMU_MOD_H_USBH,         // 8
    HAL_CMU_MOD_H_I2C_SLAVE,    // 9
    HAL_CMU_MOD_H_AX2H_A7,      // 10
    HAL_CMU_MOD_H_AH2H_WF,      // 11
    HAL_CMU_MOD_H_AH2H_BT,      // 12
    HAL_CMU_MOD_H_CODEC,        // 13
    HAL_CMU_MOD_H_AHB1,         // 14
    HAL_CMU_MOD_H_AHB0,         // 15
    HAL_CMU_MOD_H_PSRAM1G,      // 16
    HAL_CMU_MOD_H_PSRAM200,     // 17
    HAL_CMU_MOD_H_FLASH,        // 18
    HAL_CMU_MOD_H_RAM5,         // 19
    HAL_CMU_MOD_H_RAM4,         // 20
    HAL_CMU_MOD_H_RAM3,         // 21
    HAL_CMU_MOD_H_RAM2,         // 22
    HAL_CMU_MOD_H_RAM1,         // 23
    HAL_CMU_MOD_H_RAM0,         // 24
    HAL_CMU_MOD_H_ROM0,         // 25
    HAL_CMU_MOD_H_BT_DUMP,      // 26
    HAL_CMU_MOD_H_WF_DUMP,      // 27
    HAL_CMU_MOD_H_SDMMC,        // 28
    HAL_CMU_MOD_H_CHECKSUM,     // 29
    HAL_CMU_MOD_H_CRC,          // 30
    HAL_CMU_MOD_H_FLASH1,       // 31
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 0
    HAL_CMU_MOD_P_WDT,          // 1
    HAL_CMU_MOD_P_TIMER0,       // 2
    HAL_CMU_MOD_P_TIMER1,       // 3
    HAL_CMU_MOD_P_TIMER2,       // 4
    HAL_CMU_MOD_P_I2C0,         // 5
    HAL_CMU_MOD_P_I2C1,         // 6
    HAL_CMU_MOD_P_SPI,          // 7
    HAL_CMU_MOD_P_SLCD,         // 8
    HAL_CMU_MOD_P_SPI_ITN,      // 9
    HAL_CMU_MOD_P_SPI_PHY,      // 10
    HAL_CMU_MOD_P_UART0,        // 11
    HAL_CMU_MOD_P_UART1,        // 12
    HAL_CMU_MOD_P_UART2,        // 13
    HAL_CMU_MOD_P_PCM,          // 14
    HAL_CMU_MOD_P_I2S0,         // 15
    HAL_CMU_MOD_P_SPDIF0,       // 16
    HAL_CMU_MOD_P_TRANSQ0,      // 17
    HAL_CMU_MOD_P_TRANSQ1,      // 18
    HAL_CMU_MOD_P_TRNG,         // 19
    HAL_CMU_MOD_P_SEC_ENG,      // 20
    HAL_CMU_MOD_P_TZC,          // 21
    HAL_CMU_MOD_P_IR,           // 22
    HAL_CMU_MOD_P_I2C2,         // 23
    HAL_CMU_MOD_P_UART3,        // 24
    HAL_CMU_MOD_P_I2S1,         // 25
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 0
    HAL_CMU_MOD_O_USB,          // 1
    HAL_CMU_MOD_O_USB32K,       // 2
    HAL_CMU_MOD_O_PSRAM1G,      // 3
    HAL_CMU_MOD_O_PSRAM200,     // 4
    HAL_CMU_MOD_O_FLASH,        // 5
    HAL_CMU_MOD_O_SDMMC,        // 6
    HAL_CMU_MOD_O_WDT,          // 7
    HAL_CMU_MOD_O_TIMER0,       // 8
    HAL_CMU_MOD_O_TIMER1,       // 9
    HAL_CMU_MOD_O_TIMER2,       // 10
    HAL_CMU_MOD_O_I2C0,         // 11
    HAL_CMU_MOD_O_I2C1,         // 12
    HAL_CMU_MOD_O_SPI,          // 13
    HAL_CMU_MOD_O_SLCD,         // 14
    HAL_CMU_MOD_O_SPI_ITN,      // 15
    HAL_CMU_MOD_O_SPI_PHY,      // 16
    HAL_CMU_MOD_O_UART0,        // 17
    HAL_CMU_MOD_O_UART1,        // 18
    HAL_CMU_MOD_O_UART2,        // 19
    HAL_CMU_MOD_O_PCM,          // 20
    HAL_CMU_MOD_O_I2S0,         // 21
    HAL_CMU_MOD_O_SPDIF0,       // 22
    HAL_CMU_MOD_O_I2S1,         // 23
    HAL_CMU_MOD_O_A7,           // 24
    HAL_CMU_MOD_O_TSF,          // 25
    HAL_CMU_MOD_O_WDT_AP,       // 26
    HAL_CMU_MOD_O_TIMER0_AP,    // 27
    HAL_CMU_MOD_O_TIMER1_AP,    // 28
    HAL_CMU_MOD_O_FLASH1,       // 29
    HAL_CMU_MOD_O_I2C2,         // 30
    HAL_CMU_MOD_O_UART3,        // 31
    // QCLK/QRST
    HAL_CMU_MOD_Q_NULL,         // 0
    HAL_CMU_MOD_Q_DSI_32K,      // 1
    HAL_CMU_MOD_Q_DSI_PN,       // 2
    HAL_CMU_MOD_Q_DSI_TV,       // 3
    HAL_CMU_MOD_Q_DSI_PIX,      // 4
    HAL_CMU_MOD_Q_DSI_DSI,      // 5
    HAL_CMU_MOD_Q_CSI_LANE,     // 6
    HAL_CMU_MOD_Q_CSI_PIX,      // 7
    HAL_CMU_MOD_Q_CSI_LANG,     // 8
    HAL_CMU_MOD_Q_IR,           // 9

    // AON ACLK/ARST
    HAL_CMU_AON_A_CMU,          // 0
    HAL_CMU_AON_A_GPIO,         // 1
    HAL_CMU_AON_A_GPIO_INT,     // 2
    HAL_CMU_AON_A_WDT,          // 3
    HAL_CMU_AON_A_PWM,          // 4
    HAL_CMU_AON_A_TIMER,        // 5
    HAL_CMU_AON_A_IOMUX,        // 6
    HAL_CMU_AON_A_SPIDPD,       // 7
    HAL_CMU_AON_A_APBC,         // 8
    HAL_CMU_AON_A_H2H_MCU,      // 9
    HAL_CMU_AON_A_PSC,          // 10
    HAL_CMU_AON_A_PWM1,         // 11
    HAL_CMU_AON_A_WLAN_SLEEP,   // 12
    // AON OCLK/ORST
    HAL_CMU_AON_O_WDT,          // 13
    HAL_CMU_AON_O_TIMER,        // 14
    HAL_CMU_AON_O_GPIO,         // 15
    HAL_CMU_AON_O_PWM0,         // 16
    HAL_CMU_AON_O_PWM1,         // 17
    HAL_CMU_AON_O_PWM2,         // 18
    HAL_CMU_AON_O_PWM3,         // 19
    HAL_CMU_AON_O_IOMUX,        // 20
    HAL_CMU_AON_O_SLP32K,       // 21
    HAL_CMU_AON_O_SLP26M,       // 22
    HAL_CMU_AON_O_SPIDPD,       // 23
    HAL_CMU_AON_O_WLAN32K,      // 24
    HAL_CMU_AON_O_WLAN26M,      // 25
    HAL_CMU_AON_O_BTAON,        // 26
    HAL_CMU_AON_O_PWM4,         // 27
    HAL_CMU_AON_O_PWM5,         // 28
    HAL_CMU_AON_O_PWM6,         // 29
    HAL_CMU_AON_O_PWM7,         // 30

    // AON SUBSYS
    HAL_CMU_AON_A7,             // 0
    HAL_CMU_AON_A7CPU,          // 1
    HAL_CMU_AON_MCU,            // 2
    HAL_CMU_AON_CODEC,          // 3
    HAL_CMU_AON_WF,             // 4
    HAL_CMU_AON_BT,             // 5
    HAL_CMU_AON_MCUCPU,         // 6
    HAL_CMU_AON_WFCPU,          // 7
    HAL_CMU_AON_BTCPU,          // 8
    HAL_CMU_AON_GLOBAL,         // 9

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_AON_GLOBAL,
    HAL_CMU_MOD_BT = HAL_CMU_AON_BT,
    HAL_CMU_MOD_BTCPU = HAL_CMU_AON_BTCPU,
    HAL_CMU_MOD_WF = HAL_CMU_AON_WF,
    HAL_CMU_MOD_WFCPU = HAL_CMU_AON_WFCPU,

    HAL_CMU_MOD_P_PWM = HAL_CMU_AON_A_PWM,
    HAL_CMU_MOD_P_PWM1 = HAL_CMU_AON_A_PWM1,
    HAL_CMU_MOD_O_PWM0 = HAL_CMU_AON_O_PWM0,
    HAL_CMU_MOD_O_PWM1 = HAL_CMU_AON_O_PWM1,
    HAL_CMU_MOD_O_PWM2 = HAL_CMU_AON_O_PWM2,
    HAL_CMU_MOD_O_PWM3 = HAL_CMU_AON_O_PWM3,
    HAL_CMU_MOD_O_PWM4 = HAL_CMU_AON_O_PWM4,
    HAL_CMU_MOD_O_PWM5 = HAL_CMU_AON_O_PWM5,
    HAL_CMU_MOD_O_PWM6 = HAL_CMU_AON_O_PWM6,
    HAL_CMU_MOD_O_PWM7 = HAL_CMU_AON_O_PWM7,

    HAL_CMU_H_DCACHE = HAL_CMU_MOD_H_CACHE0,
    HAL_CMU_H_ICACHE = HAL_CMU_MOD_H_CACHE1,

    HAL_CMU_MOD_P_SPI_DPD = HAL_CMU_AON_A_SPIDPD,
    HAL_CMU_MOD_O_SPI_DPD = HAL_CMU_AON_O_SPIDPD,
    HAL_CMU_MOD_H_PSRAM = HAL_CMU_MOD_H_PSRAM200,
    HAL_CMU_MOD_O_PSRAM = HAL_CMU_MOD_O_PSRAM200,
    HAL_CMU_MOD_H_PSRAMUHS = HAL_CMU_MOD_H_PSRAM1G,
    HAL_CMU_MOD_O_PSRAMUHS = HAL_CMU_MOD_O_PSRAM1G,
    HAL_CMU_MOD_H_LCDC = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_X_DISPB = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_X_DISP = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_H_DISPPRE = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_H_DISP = HAL_CMU_MOD_QTY,

};

enum HAL_CMU_CLOCK_OUT_ID_T {
    HAL_CMU_CLOCK_OUT_AON_32K           = 0x00,
    HAL_CMU_CLOCK_OUT_AON_OSC           = 0x01,
    HAL_CMU_CLOCK_OUT_AON_OSCX2         = 0x02,
    HAL_CMU_CLOCK_OUT_AON_DIG_OSCX2     = 0x03,
    HAL_CMU_CLOCK_OUT_AON_DIG_OSCX4     = 0x04,
    HAL_CMU_CLOCK_OUT_AON_PER           = 0x05,
    HAL_CMU_CLOCK_OUT_AON_USB           = 0x06,
    HAL_CMU_CLOCK_OUT_AON_DCDC0         = 0x07,
    HAL_CMU_CLOCK_OUT_AON_CHCLK         = 0x08,
    HAL_CMU_CLOCK_OUT_AON_SPDIF0        = 0x09,
    HAL_CMU_CLOCK_OUT_AON_MCU           = 0x0A,
    HAL_CMU_CLOCK_OUT_AON_FLASH         = 0x0B,
    HAL_CMU_CLOCK_OUT_AON_PSRAM         = 0x0C,
    HAL_CMU_CLOCK_OUT_AON_DDR           = 0x0D,
    HAL_CMU_CLOCK_OUT_AON_A7            = 0x0E,
    HAL_CMU_CLOCK_OUT_AON_DCDC1         = 0x0F,
    HAL_CMU_CLOCK_OUT_AON_OSCX4         = 0x10,
    HAL_CMU_CLOCK_OUT_AON_DSI           = 0x11,
    HAL_CMU_CLOCK_OUT_AON_CSI           = 0x12,
    HAL_CMU_CLOCK_OUT_AON_PIX_DSI       = 0x13,
    HAL_CMU_CLOCK_OUT_AON_PIX_CSI       = 0x14,
    HAL_CMU_CLOCK_OUT_AON_PSRAMX2       = 0x15,
    HAL_CMU_CLOCK_OUT_AON_DCDC2         = 0x16,
    HAL_CMU_CLOCK_OUT_AON_SYS           = 0x17,

    HAL_CMU_CLOCK_OUT_WF_32K            = 0x20,
    HAL_CMU_CLOCK_OUT_WF_HCLK           = 0x21,
    HAL_CMU_CLOCK_OUT_WF_PCLK           = 0x22,
    HAL_CMU_CLOCK_OUT_WF_MAC            = 0x23,
    HAL_CMU_CLOCK_OUT_WF_PHY            = 0x24,
    HAL_CMU_CLOCK_OUT_WF_OSC            = 0x25,
    HAL_CMU_CLOCK_OUT_WF_40M            = 0x26,
    HAL_CMU_CLOCK_OUT_WF_80M            = 0x27,
    HAL_CMU_CLOCK_OUT_WF_160M           = 0x28,
    HAL_CMU_CLOCK_OUT_WF_UART           = 0x29,
    HAL_CMU_CLOCK_OUT_WF_SPI            = 0x2A,
    HAL_CMU_CLOCK_OUT_WF_SDIO           = 0x2B,
    HAL_CMU_CLOCK_OUT_WF_ADC            = 0x2C,
    HAL_CMU_CLOCK_OUT_WF_TX_DAC         = 0x2D,
    HAL_CMU_CLOCK_OUT_WF_BBDIGFIFO      = 0x2E,

    HAL_CMU_CLOCK_OUT_BT_NONE           = 0x40,
    HAL_CMU_CLOCK_OUT_BT_32K            = 0x41,
    HAL_CMU_CLOCK_OUT_BT_SYS            = 0x42,
    HAL_CMU_CLOCK_OUT_BT_OSCX2          = 0x43,
    HAL_CMU_CLOCK_OUT_BT_OSC_2          = 0x44,
    HAL_CMU_CLOCK_OUT_BT_ADC            = 0x45,
    HAL_CMU_CLOCK_OUT_BT_ADCD3          = 0x46,
    HAL_CMU_CLOCK_OUT_BT_DAC            = 0x47,
    HAL_CMU_CLOCK_OUT_BT_DACD2          = 0x48,
    HAL_CMU_CLOCK_OUT_BT_DACD4          = 0x49,
    HAL_CMU_CLOCK_OUT_BT_DACD8          = 0x4A,

    HAL_CMU_CLOCK_OUT_MCU_32K           = 0x60,
    HAL_CMU_CLOCK_OUT_MCU_SYS           = 0x61,
    HAL_CMU_CLOCK_OUT_MCU_FLASH         = 0x62,
    HAL_CMU_CLOCK_OUT_MCU_USB           = 0x63,
    HAL_CMU_CLOCK_OUT_MCU_PCLK          = 0x64,
    HAL_CMU_CLOCK_OUT_MCU_I2S0          = 0x65,
    HAL_CMU_CLOCK_OUT_MCU_PCM           = 0x66,
    HAL_CMU_CLOCK_OUT_MCU_SPDIF0        = 0x67,
    HAL_CMU_CLOCK_OUT_MCU_SDMMC         = 0x68,
    HAL_CMU_CLOCK_OUT_MCU_SPI2          = 0x69,
    HAL_CMU_CLOCK_OUT_MCU_SPI0          = 0x6A,
    HAL_CMU_CLOCK_OUT_MCU_SPI1          = 0x6B,
    HAL_CMU_CLOCK_OUT_MCU_XCLK          = 0x6C,
    HAL_CMU_CLOCK_OUT_MCU_APCLK         = 0x6D,
    HAL_CMU_CLOCK_OUT_MCU_I2S1          = 0x6E,

    HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA     = 0x80,
    HAL_CMU_CLOCK_OUT_CODEC_CODEC       = 0x81,
    HAL_CMU_CLOCK_OUT_CODEC_IIR         = 0x82,
    HAL_CMU_CLOCK_OUT_CODEC_RS_DAC      = 0x83,
    HAL_CMU_CLOCK_OUT_CODEC_RS_ADC      = 0x84,
    HAL_CMU_CLOCK_OUT_CODEC_HCLK        = 0x85,
};

enum HAL_CMU_I2S_MCLK_ID_T {
    HAL_CMU_I2S_MCLK_PLLCODEC           = 0x00,
    HAL_CMU_I2S_MCLK_CODEC              = 0x01,
    HAL_CMU_I2S_MCLK_PLLIIR             = 0x02,
    HAL_CMU_I2S_MCLK_PLLRS              = 0x03,
    HAL_CMU_I2S_MCLK_PLLSPDIF0          = 0x04,
    HAL_CMU_I2S_MCLK_PLLPCM             = 0x05,
    HAL_CMU_I2S_MCLK_PER                = 0x06,
    HAL_CMU_I2S_MCLK_CLK_OUT            = 0x07,
};

enum HAL_PWM_ID_T {
    HAL_PWM_ID_0,
    HAL_PWM_ID_1,
    HAL_PWM_ID_2,
    HAL_PWM_ID_3,
    HAL_PWM1_ID_0,
    HAL_PWM1_ID_1,
    HAL_PWM1_ID_2,
    HAL_PWM1_ID_3,

    HAL_PWM_ID_QTY
};
#define HAL_PWM_ID_T                        HAL_PWM_ID_T

enum HAL_I2S_ID_T {
    HAL_I2S_ID_0 = 0,
    HAL_I2S_ID_1,

    HAL_I2S_ID_QTY,
};
#define HAL_I2S_ID_T                        HAL_I2S_ID_T

enum HAL_CMU_FREQ_T {
    HAL_CMU_FREQ_32K,
    HAL_CMU_FREQ_6P5M,
    HAL_CMU_FREQ_13M,
    HAL_CMU_FREQ_26M,
    HAL_CMU_FREQ_52M,
    HAL_CMU_FREQ_78M,
    HAL_CMU_FREQ_104M,
    HAL_CMU_FREQ_156M,
    HAL_CMU_FREQ_208M,
    HAL_CMU_FREQ_260M,
    HAL_CMU_FREQ_390M,
    HAL_CMU_FREQ_780M,

    HAL_CMU_FREQ_QTY
};
#define HAL_CMU_FREQ_T                      HAL_CMU_FREQ_T
#define HAL_CMU_FREQ_390M                   HAL_CMU_FREQ_390M

enum HAL_CMU_PLL_T {
    HAL_CMU_PLL_USB,
    HAL_CMU_PLL_AUD = HAL_CMU_PLL_USB,
    HAL_CMU_PLL_DDR,
    HAL_CMU_PLL_DSP,
    HAL_CMU_PLL_BB,
    HAL_CMU_PLL_BB_PSRAM,
    HAL_CMU_PLL_DSI,

    HAL_CMU_PLL_QTY
};
#define HAL_CMU_PLL_T                       HAL_CMU_PLL_T

enum HAL_CMU_PLL_USER_T {
    HAL_CMU_PLL_USER_SYS,
    HAL_CMU_PLL_USER_AUD,
    HAL_CMU_PLL_USER_USB,
    HAL_CMU_PLL_USER_FLASH,
    HAL_CMU_PLL_USER_PSRAM,
    HAL_CMU_PLL_USER_DSP,
    HAL_CMU_PLL_USER_DSI,

    HAL_CMU_PLL_USER_QTY,
    HAL_CMU_PLL_USER_ALL = HAL_CMU_PLL_USER_QTY,
};
#define HAL_CMU_PLL_USER_T                  HAL_CMU_PLL_USER_T

#define HAL_CMU_USB_ROM_SELECT_CLOCK_SOURCE

#define HAL_CMU_SYS_REBOOT

enum HAL_FLASH0_SIZE_CFG {
    HAL_FLASH0_SIZE_0M  = 0x1,
    HAL_FLASH0_SIZE_8M  = 0x2,
    HAL_FLASH0_SIZE_16M = 0x0,
    HAL_FLASH0_SIZE_32M = 0x4,
    HAL_FLASH0_SIZE_64M = 0x8,
};

enum HAL_CMU_JTAG_SEL_T {
    HAL_CMU_JTAG_SEL_MAIN,
    HAL_CMU_JTAG_SEL_CP,
    HAL_CMU_JTAG_SEL_A7,

    HAL_CMU_JTAG_SEL_QTY
};

int hal_cmu_fast_timer_offline(void);

int hal_cmu_ddr_clock_enable();

void hal_cmu_ddr_clock_disable();

void hal_cmu_ddr_reset_set();

void hal_cmu_ddr_reset_clear();

uint32_t hal_cmu_get_aon_chip_id(void);

uint32_t hal_cmu_get_aon_revision_id(void);

void hal_cmu_cp_enable(uint32_t sp, uint32_t entry);

void hal_cmu_cp_disable(void);

uint32_t hal_cmu_cp_get_entry_addr(void);

void hal_cmu_wifi_clock_enable(void);

void hal_cmu_wifi_clock_disable(void);

void hal_cmu_wifi_reset_set(void);

void hal_cmu_wifi_reset_clear(void);

void hal_cmu_dsp_clock_enable(void);

void hal_cmu_dsp_clock_disable(void);

void hal_cmu_dsp_clock_continue(void);

void hal_cmu_dsp_clock_hold(void);

void hal_cmu_dsp_reset_set(void);

void hal_cmu_dsp_reset_clear(void);

void hal_cmu_dsp_init_boot_reg(uint32_t entry);

void hal_cmu_dsp_start_cpu(void);

void hal_cmu_dsp_setup(void);

void hal_cmu_dsp_stop_cpu(void);

void hal_cmu_jtag_set_cp(void);

void hal_cmu_jtag_set_a7(void);

void hal_cmu_cp_boot(uint32_t entry);

void hal_cmu_jtag_sel(enum HAL_CMU_JTAG_SEL_T sel);

void hal_cmu_dma_req_init(void);

void hal_cmu_flash0_dual_die();

void hal_cmu_set_flash0_x8_mode(uint32_t en);

void hal_cmu_set_flash0_size(enum HAL_FLASH0_SIZE_CFG cfg);

void hal_cmu_flash1_enable();

void hal_cmu_wlan_set_sleep_allow(uint32_t auto_mode);

uint32_t hal_cmu_get_osc_ready_cycle_cnt(void);

uint32_t hal_cmu_get_osc_switch_overhead(void);

void hal_cmu_dsi_phy_reset_set(void);

void hal_cmu_dsi_phy_reset_clear(void);

void hal_cmu_dsi_clock_enable(void);

void hal_cmu_dsi_clock_enable_v2(uint8_t pixel_div);

void hal_cmu_dsi_clock_disable(void);

void hal_cmu_dsi_reset_set(void);

void hal_cmu_dsi_reset_clear(void);

void hal_cmu_csi_clock_enable(void);

void hal_cmu_csi_clock_disable(void);

void hal_cmu_csi_reset_set(void);

void hal_cmu_csi_reset_clear(void);

void hal_cmu_lcdc_clock_enable(void);

void hal_cmu_lcdc_clock_disable(void);

void hal_cmu_lcdc_reset_set(void);

void hal_cmu_lcdc_reset_clear(void);

void hal_cmu_shutdown_hook(void);

void hal_cmu_dsi_sleep(void);

void hal_cmu_dsi_wakeup(void);

void hal_cmu_lcdc_sleep(void);

void hal_cmu_lcdc_wakeup(void);

int hal_cmu_dsp_set_freq(enum HAL_CMU_FREQ_T freq);

#ifdef __cplusplus
}
#endif

#endif

