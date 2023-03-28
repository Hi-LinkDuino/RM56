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
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_iomux)
#include "hal_iomux.h"
#include "hal_chipid.h"
#include "hal_gpio.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_uart.h"
#include "pmu.h"
#include "hal_chipid.h"

#define UART_HALF_DUPLEX

#ifdef I2S0_VOLTAGE_VMEM
#define I2S0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2S0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef I2S1_VOLTAGE_VMEM
#define I2S1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2S1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPDIF0_VOLTAGE_VMEM
#define SPDIF0_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPDIF0_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef DIGMIC_VOLTAGE_VMEM
#define DIGMIC_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define DIGMIC_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPI_VOLTAGE_VMEM
#define SPI_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPI_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPILCD_VOLTAGE_VMEM
#define SPILCD_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPILCD_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef I2C0_VOLTAGE_VMEM
#define I2C0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2C0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef I2C1_VOLTAGE_VMEM
#define I2C1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2C1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef I2C2_VOLTAGE_VMEM
#define I2C2_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2C2_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef CLKOUT_VOLTAGE_VMEM
#define CLKOUT_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define CLKOUT_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef PWM0_VOLTAGE_VMEM
#define PWM0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define PWM0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef PWM1_VOLTAGE_VMEM
#define PWM1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define PWM1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef PWM2_VOLTAGE_VMEM
#define PWM2_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define PWM2_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef PWM3_VOLTAGE_VMEM
#define PWM3_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define PWM3_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef PWM4_VOLTAGE_VMEM
#define PWM4_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define PWM4_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef PWM5_VOLTAGE_VMEM
#define PWM5_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define PWM5_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef PWM6_VOLTAGE_VMEM
#define PWM6_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define PWM6_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef PWM7_VOLTAGE_VMEM
#define PWM7_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define PWM7_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef IR_VOLTAGE_VMEM
#define IR_VOLTAGE_SEL                      HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define IR_VOLTAGE_SEL                      HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SDMMC_VOLTAGE_VMEM
#define SDMMC_VOLTAGE_SEL                   HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SDMMC_VOLTAGE_SEL                   HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifndef I2S0_I_IOMUX_INDEX
//00, 13, 37
#define I2S0_I_IOMUX_INDEX                  00
#endif
#ifndef I2S0_I1_IOMUX_INDEX
//12, 36
//#define I2S0_I1_IOMUX_INDEX                 12
#endif
#ifndef I2S0_I2_IOMUX_INDEX
//11, 35
//#define I2S0_I2_IOMUX_INDEX                 11
#endif
#ifndef I2S0_I3_IOMUX_INDEX
//10, 35
//#define I2S0_I3_IOMUX_INDEX                 10
#endif

#ifndef I2S0_O_IOMUX_INDEX
//01, 07
#define I2S0_O_IOMUX_INDEX                  01
#endif
#ifndef I2S0_O1_IOMUX_INDEX
//06
#define I2S0_O1_IOMUX_INDEX                 06
#endif
#ifndef I2S0_O2_IOMUX_INDEX
//05
#define I2S0_O2_IOMUX_INDEX                 05
#endif
#ifndef I2S0_O3_IOMUX_INDEX
//04
#define I2S0_O3_IOMUX_INDEX                 04
#endif

#ifndef I2S1_I_IOMUX_INDEX
//20, 33
#define I2S1_I_IOMUX_INDEX                  20
#endif
#ifndef I2S1_I1_IOMUX_INDEX
//32
//#define I2S1_I1_IOMUX_INDEX                 32
#endif
#ifndef I2S1_I2_IOMUX_INDEX
//31
//#define I2S1_I2_IOMUX_INDEX                 31
#endif
#ifndef I2S1_I3_IOMUX_INDEX
//30
//#define I2S1_I3_IOMUX_INDEX                 30
#endif

#ifndef I2S1_O_IOMUX_INDEX
//21, 27
#define I2S1_O_IOMUX_INDEX                  21
#endif
#ifndef I2S1_O1_IOMUX_INDEX
//26
#define I2S1_O1_IOMUX_INDEX                 26
#endif
#ifndef I2S1_O2_IOMUX_INDEX
//25
#define I2S1_O2_IOMUX_INDEX                 25
#endif
#ifndef I2S1_O3_IOMUX_INDEX
//24
#define I2S1_O3_IOMUX_INDEX                 24
#endif

#ifndef I2S_MCLK_IOMUX_INDEX
//04, 13, 15, 20, 22, 27, 34
#define I2S_MCLK_IOMUX_INDEX                04
#endif

#ifndef SPDIF0_I_IOMUX_INDEX
//02, 10, 20, 26, 37, 24
#define SPDIF0_I_IOMUX_INDEX                02
#endif

#ifndef SPDIF0_O_IOMUX_INDEX
//03, 11, 21, 27, 37, 07
#define SPDIF0_O_IOMUX_INDEX                03
#endif

#ifndef DIG_MIC_CK_IOMUX_PIN
//HAL_IOMUX_PIN_P0_0, HAL_IOMUX_PIN_P0_4, HAL_IOMUX_PIN_P3_3, HAL_IOMUX_PIN_P3_4
#define DIG_MIC_CK_IOMUX_PIN                0
#endif

#ifndef DIG_MIC_D0_IOMUX_PIN
//HAL_IOMUX_PIN_P0_1, HAL_IOMUX_PIN_P0_5, HAL_IOMUX_PIN_P3_0, HAL_IOMUX_PIN_P3_5
#define DIG_MIC_D0_IOMUX_PIN                1
#endif

#ifndef DIG_MIC_D1_IOMUX_PIN
//HAL_IOMUX_PIN_P0_2, HAL_IOMUX_PIN_P0_6, HAL_IOMUX_PIN_P3_1, HAL_IOMUX_PIN_P3_6
#define DIG_MIC_D1_IOMUX_PIN                2
#endif

#ifndef DIG_MIC_D2_IOMUX_PIN
//HAL_IOMUX_PIN_P0_3, HAL_IOMUX_PIN_P0_7, HAL_IOMUX_PIN_P3_2, HAL_IOMUX_PIN_P3_7
#define DIG_MIC_D2_IOMUX_PIN                3
#endif

#ifndef SPI_IOMUX_INDEX
//04(05,06,07), 24(25,26,27), 30(31,32,33)
#define SPI_IOMUX_INDEX                    04
#endif

#ifndef SPI_IOMUX_CS1_INDEX
//10, 35
//#define SPI_IOMUX_CS1_INDEX                10
#endif
#ifndef SPI_IOMUX_CS2_INDEX
//11, 37
//#define SPI_IOMUX_CS2_INDEX                11
#endif
#ifndef SPI_IOMUX_CS3_INDEX
//12, 25
//#define SPI_IOMUX_CS3_INDEX                12
#endif

#ifndef SPI_IOMUX_DI1_INDEX
//13, 34
//#define SPI_IOMUX_DI1_INDEX                13
#endif
#ifndef SPI_IOMUX_DI2_INDEX
//14, 36
//#define SPI_IOMUX_DI2_INDEX                14
#endif
#ifndef SPI_IOMUX_DI3_INDEX
//15, 24
//#define SPI_IOMUX_DI3_INDEX                15
#endif

#ifndef SPILCD_IOMUX_INDEX
//00(01,02,03), 10(11,14,15), 20(21,22,23), 34(35,36,37)
#define SPILCD_IOMUX_INDEX                    0
#endif

#ifndef SPILCD_IOMUX_CS1_INDEX
//05, 12, 27
//#define SPILCD_IOMUX_CS1_INDEX                05
#endif
#ifndef SPILCD_IOMUX_CS2_INDEX
//06, 31
//#define SPILCD_IOMUX_CS2_INDEX                06
#endif
#ifndef SPILCD_IOMUX_CS3_INDEX
//07, 32
//#define SPILCD_IOMUX_CS3_INDEX                07
#endif

#ifndef SPILCD_IOMUX_DI1_INDEX
//02, 26
//#define SPILCD_IOMUX_DI1_INDEX                02
#endif
#ifndef SPILCD_IOMUX_DI2_INDEX
//03, 30
//#define SPILCD_IOMUX_DI2_INDEX                03
#endif
#ifndef SPILCD_IOMUX_DI3_INDEX
//04, 33
//#define SPILCD_IOMUX_DI3_INDEX                04
#endif

#ifndef I2C0_IOMUX_INDEX
//00(01), 04(05), 16(17), 20(21), 26(27), 34(35)
#define I2C0_IOMUX_INDEX                    4
#endif

#ifndef I2C1_IOMUX_INDEX
//02(03), 06(07), 14(15), 22(23), 30(31)
#define I2C1_IOMUX_INDEX                    22
#endif

#ifndef I2C2_IOMUX_INDEX
//10(11), 12(13), 24(25), 32(33), 36(37)
#define I2C2_IOMUX_INDEX                    10
#endif

#ifndef CLKOUT_IOMUX_INDEX
//04, 13, 15, 20, 21, 22, 23, 27, 34
#define CLKOUT_IOMUX_INDEX                  20
#endif

#ifndef WIFI_UART_IOMUX_INDEX
//00(nothing), 20, 21, 30
#define WIFI_UART_IOMUX_INDEX               00
#endif

#ifndef UART1_IOMUX_INDEX
//02, 10, 20, 30, 32
#define UART1_IOMUX_INDEX                   20
#endif

#ifndef UART2_IOMUX_INDEX
//00, 12, 14, 22, 36
#define UART2_IOMUX_INDEX                   22
#endif

#ifndef UART3_IOMUX_INDEX
//04, 06, 24, 26, 34
#define UART3_IOMUX_INDEX                   04
#endif

#ifndef PWM0_IOMUX_INDEX
//00, 10, 20, 30
#define PWM0_IOMUX_INDEX                    00
#endif

#ifndef PWM1_IOMUX_INDEX
//01, 11, 21, 31
#define PWM1_IOMUX_INDEX                    01
#endif

#ifndef PWM2_IOMUX_INDEX
//02, 12, 22, 32
#define PWM2_IOMUX_INDEX                    02
#endif

#ifndef PWM3_IOMUX_INDEX
//03, 13, 23, 33
#define PWM3_IOMUX_INDEX                    03
#endif

#ifndef PWM4_IOMUX_INDEX
//04, 14, 24, 34
#define PWM4_IOMUX_INDEX                    04
#endif

#ifndef PWM5_IOMUX_INDEX
//05, 15, 25, 35
#define PWM5_IOMUX_INDEX                    05
#endif

#ifndef PWM6_IOMUX_INDEX
//06, 16, 26, 36
#define PWM6_IOMUX_INDEX                    06
#endif

#ifndef PWM7_IOMUX_INDEX
//07, 17, 27, 37
#define PWM7_IOMUX_INDEX                    07
#endif

#ifndef IR_RX_IOMUX_INDEX
//00, 02, 06, 10, 12, 14, 22, 26, 31, 33, 36
#define IR_RX_IOMUX_INDEX                   00
#endif

#ifndef IR_TX_IOMUX_INDEX
//01, 03, 07, 11, 13, 15, 21, 27, 32, 34, 37
#define IR_TX_IOMUX_INDEX                   01
#endif

#define IOMUX_FUNC_VAL_GPIO                 15

#define IOMUX_ALT_FUNC_NUM                  11

// Other func values: 2 -> uart rtx/ctx, 12 -> btdm, 13 -> wf_fem, 14 -> tport, 15 -> gpio
static const uint8_t index_to_func_val[IOMUX_ALT_FUNC_NUM] = {
    0,  1,  3,  4,
    5,  6,  7,  8,
    9,  10, 11
};

static const enum HAL_IOMUX_FUNCTION_T pin_func_map[HAL_IOMUX_PIN_NUM][IOMUX_ALT_FUNC_NUM] = {
    // P0_0
    { HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_I2C_M0_SCL, HAL_IOMUX_FUNC_SPILCD_DI0,
      HAL_IOMUX_FUNC_SPILCD_DCN, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_IR_RX, HAL_IOMUX_FUNC_PDM0_CK, HAL_IOMUX_FUNC_I2S0_SDI0, },
    // P0_1
    { HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_I2C_M0_SDA, HAL_IOMUX_FUNC_SPILCD_DIO,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_IR_TX, HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_I2S0_SDO0, },
    // P0_2
    { HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_FUNC_SPILCD_CS0,
      HAL_IOMUX_FUNC_SPILCD_DI1, HAL_IOMUX_FUNC_SPDIF0_DI, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_DISPLAY_BL_EN,
      HAL_IOMUX_FUNC_IR_RX, HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_I2S0_WS, },
    // P0_3
    { HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_FUNC_SPILCD_CLK,
      HAL_IOMUX_FUNC_SPILCD_DI2, HAL_IOMUX_FUNC_SPDIF0_DO, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_DISPLAY_BL_PWM,
      HAL_IOMUX_FUNC_IR_TX, HAL_IOMUX_FUNC_PDM2_D, HAL_IOMUX_FUNC_I2S0_SCK, },
    // P0_4
    { HAL_IOMUX_FUNC_PWM4, HAL_IOMUX_FUNC_UART3_RX, HAL_IOMUX_FUNC_I2C_M0_SCL, HAL_IOMUX_FUNC_SPI_DI0,
      HAL_IOMUX_FUNC_SPILCD_DI3, HAL_IOMUX_FUNC_I2S_MCLK, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_SPI_DCN,
      HAL_IOMUX_FUNC_SDMMC_DATA7, HAL_IOMUX_FUNC_PDM1_CK, HAL_IOMUX_FUNC_I2S0_SDO3, },
    // P0_5
    { HAL_IOMUX_FUNC_PWM5, HAL_IOMUX_FUNC_UART3_TX, HAL_IOMUX_FUNC_I2C_M0_SDA, HAL_IOMUX_FUNC_SPI_CLK,
      HAL_IOMUX_FUNC_SPILCD_CS1, HAL_IOMUX_FUNC_DISPLAY_SPI_CLK, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_DISPLAY_TE,
      HAL_IOMUX_FUNC_SDMMC_DATA6, HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_I2S0_SDO2, },
    // P0_6
    { HAL_IOMUX_FUNC_PWM6, HAL_IOMUX_FUNC_UART3_RX, HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_FUNC_SPI_CS0,
      HAL_IOMUX_FUNC_SPILCD_CS2, HAL_IOMUX_FUNC_DISPLAY_SPI_CS, HAL_IOMUX_FUNC_WF_WAKE_HOST, HAL_IOMUX_FUNC_IR_RX,
      HAL_IOMUX_FUNC_SDMMC_DATA5, HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_I2S0_SDO1, },
    // P0_7
    { HAL_IOMUX_FUNC_PWM7, HAL_IOMUX_FUNC_UART3_TX, HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_FUNC_SPI_DIO,
      HAL_IOMUX_FUNC_SPILCD_CS3, HAL_IOMUX_FUNC_DISPLAY_SPI_DIO, HAL_IOMUX_FUNC_SPDIF0_DO, HAL_IOMUX_FUNC_IR_TX,
      HAL_IOMUX_FUNC_SDMMC_DATA4, HAL_IOMUX_FUNC_PDM2_D, HAL_IOMUX_FUNC_I2S0_SDO0, },
    // P1_0
    { HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_FUNC_I2C_M2_SCL, HAL_IOMUX_FUNC_SPILCD_CLK,
      HAL_IOMUX_FUNC_SPI_CS1, HAL_IOMUX_FUNC_DISPLAY_SPI_DO1_DCN, HAL_IOMUX_FUNC_IR_RX, HAL_IOMUX_FUNC_WF_SDIO_CLK,
      HAL_IOMUX_FUNC_SDMMC_DATA2, HAL_IOMUX_FUNC_SPDIF0_DI, HAL_IOMUX_FUNC_I2S0_SDI3, },
    // P1_1
    { HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_FUNC_I2C_M2_SDA, HAL_IOMUX_FUNC_SPILCD_CS0,
      HAL_IOMUX_FUNC_SPI_CS2, HAL_IOMUX_FUNC_DISPLAY_SPI_DO2, HAL_IOMUX_FUNC_IR_TX, HAL_IOMUX_FUNC_WF_SDIO_CMD,
      HAL_IOMUX_FUNC_SDMMC_DATA3, HAL_IOMUX_FUNC_SPDIF0_DO, HAL_IOMUX_FUNC_I2S0_SDI2, },
    // P1_2
    { HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_I2C_M2_SCL, HAL_IOMUX_FUNC_SPILCD_CS1,
      HAL_IOMUX_FUNC_SPI_CS3, HAL_IOMUX_FUNC_DISPLAY_SPI_DO3, HAL_IOMUX_FUNC_CLK_32K_IN, HAL_IOMUX_FUNC_WF_SDIO_DATA0,
      HAL_IOMUX_FUNC_SDMMC_CMD, HAL_IOMUX_FUNC_IR_RX, HAL_IOMUX_FUNC_I2S0_SDI1, },
    // P1_3
    { HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_I2C_M2_SDA, HAL_IOMUX_FUNC_SPILCD_DCN,
      HAL_IOMUX_FUNC_SPI_DI1, HAL_IOMUX_FUNC_I2S_MCLK, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_WF_SDIO_DATA1,
      HAL_IOMUX_FUNC_SDMMC_CLK, HAL_IOMUX_FUNC_IR_TX, HAL_IOMUX_FUNC_I2S0_SDI0, },
    // P1_4
    { HAL_IOMUX_FUNC_PWM4, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_FUNC_SPILCD_DI0,
      HAL_IOMUX_FUNC_SPI_DI2, HAL_IOMUX_FUNC_DISPLAY_SPI_DI, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_WF_SDIO_DATA2,
      HAL_IOMUX_FUNC_SDMMC_DATA0, HAL_IOMUX_FUNC_IR_RX, HAL_IOMUX_FUNC_DISPLAY_TE, },
    // P1_5
    { HAL_IOMUX_FUNC_PWM5, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_FUNC_SPILCD_DIO,
      HAL_IOMUX_FUNC_SPI_DI3, HAL_IOMUX_FUNC_I2S_MCLK, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_WF_SDIO_DATA3,
      HAL_IOMUX_FUNC_SDMMC_DATA1, HAL_IOMUX_FUNC_IR_TX, HAL_IOMUX_FUNC_DISPLAY_TE, },
    // P1_6
    { HAL_IOMUX_FUNC_PWM6, HAL_IOMUX_FUNC_UART0_RX, HAL_IOMUX_FUNC_I2C_M0_SCL, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_WF_WAKE_HOST,
      HAL_IOMUX_FUNC_BT_UART_RX, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P1_7
    { HAL_IOMUX_FUNC_PWM7, HAL_IOMUX_FUNC_UART0_TX, HAL_IOMUX_FUNC_I2C_M0_SDA, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_BT_UART_TX, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P2_0
    { HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_FUNC_I2C_M0_SCL, HAL_IOMUX_FUNC_SPILCD_DI0,
      HAL_IOMUX_FUNC_SPILCD_DCN, HAL_IOMUX_FUNC_I2S_MCLK, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_WF_UART_RX,
      HAL_IOMUX_FUNC_BT_UART_RX, HAL_IOMUX_FUNC_SPDIF0_DI, HAL_IOMUX_FUNC_I2S1_SDI0, },
    // P2_1
    { HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_FUNC_I2C_M0_SDA, HAL_IOMUX_FUNC_SPILCD_DIO,
      HAL_IOMUX_FUNC_IR_TX, HAL_IOMUX_FUNC_DISPLAY_TE, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_WF_UART_TX,
      HAL_IOMUX_FUNC_BT_UART_TX, HAL_IOMUX_FUNC_SPDIF0_DO, HAL_IOMUX_FUNC_I2S1_SDO0, },
    // P2_2
    { HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_FUNC_SPILCD_CS0,
      HAL_IOMUX_FUNC_DISPLAY_BL_EN, HAL_IOMUX_FUNC_I2S_MCLK, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_WF_UART_CTS,
      HAL_IOMUX_FUNC_BT_UART_CTS, HAL_IOMUX_FUNC_IR_RX, HAL_IOMUX_FUNC_I2S1_WS, },
    // P2_3
    { HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_FUNC_SPILCD_CLK,
      HAL_IOMUX_FUNC_SPI_DCN, HAL_IOMUX_FUNC_DISPLAY_BL_PWM, HAL_IOMUX_FUNC_PCM_DI, HAL_IOMUX_FUNC_WF_UART_RTS,
      HAL_IOMUX_FUNC_BT_UART_RTS, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_I2S1_SCK, },
    // P2_4
    { HAL_IOMUX_FUNC_PWM4, HAL_IOMUX_FUNC_UART3_RX, HAL_IOMUX_FUNC_I2C_M2_SCL, HAL_IOMUX_FUNC_SPI_DI0,
      HAL_IOMUX_FUNC_SPI_DI3, HAL_IOMUX_FUNC_SPI_DCN, HAL_IOMUX_FUNC_PCM_DO, HAL_IOMUX_FUNC_SPDIF0_DI,
      HAL_IOMUX_FUNC_WF_SDIO_CLK, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_I2S1_SDO3, },
    // P2_5
    { HAL_IOMUX_FUNC_PWM5, HAL_IOMUX_FUNC_UART3_TX, HAL_IOMUX_FUNC_I2C_M2_SDA, HAL_IOMUX_FUNC_SPI_DIO,
      HAL_IOMUX_FUNC_SPI_CS3, HAL_IOMUX_FUNC_DISPLAY_TE, HAL_IOMUX_FUNC_PCM_FSYNC, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_WF_SDIO_CMD, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_I2S1_SDO2, },
    // P2_6
    { HAL_IOMUX_FUNC_PWM6, HAL_IOMUX_FUNC_UART3_RX, HAL_IOMUX_FUNC_I2C_M0_SCL, HAL_IOMUX_FUNC_SPI_CS0,
      HAL_IOMUX_FUNC_SPILCD_DI1, HAL_IOMUX_FUNC_CLK_32K_IN, HAL_IOMUX_FUNC_PCM_CLK, HAL_IOMUX_FUNC_IR_RX,
      HAL_IOMUX_FUNC_WF_SDIO_DATA0, HAL_IOMUX_FUNC_SPDIF0_DI, HAL_IOMUX_FUNC_I2S1_SDO1, },
    // P2_7
    { HAL_IOMUX_FUNC_PWM7, HAL_IOMUX_FUNC_UART3_TX, HAL_IOMUX_FUNC_I2C_M0_SDA, HAL_IOMUX_FUNC_SPI_CLK,
      HAL_IOMUX_FUNC_SPILCD_CS1, HAL_IOMUX_FUNC_I2S_MCLK, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_IR_TX,
      HAL_IOMUX_FUNC_WF_SDIO_DATA1, HAL_IOMUX_FUNC_SPDIF0_DO, HAL_IOMUX_FUNC_I2S1_SDO0, },
    // P3_0
    { HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_FUNC_SPI_DI0,
      HAL_IOMUX_FUNC_SPILCD_DI2, HAL_IOMUX_FUNC_SPI_DCN, HAL_IOMUX_FUNC_WF_UART_RX, HAL_IOMUX_FUNC_SPILCD_CS1,
      HAL_IOMUX_FUNC_WF_SDIO_DATA2, HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_I2S1_SDI3, },
    // P3_1
    { HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_FUNC_SPI_DIO,
      HAL_IOMUX_FUNC_WF_SDIO_DATA0, HAL_IOMUX_FUNC_DISPLAY_SPI_DI, HAL_IOMUX_FUNC_WF_UART_TX, HAL_IOMUX_FUNC_IR_RX,
      HAL_IOMUX_FUNC_WF_SDIO_DATA3, HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_I2S1_SDI2, },
    // P3_2
    { HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_FUNC_I2C_M2_SCL, HAL_IOMUX_FUNC_SPI_CS0,
      HAL_IOMUX_FUNC_SPILCD_CS3, HAL_IOMUX_FUNC_DISPLAY_SPI_DO3, HAL_IOMUX_FUNC_WF_UART_CTS, HAL_IOMUX_FUNC_IR_TX,
      HAL_IOMUX_FUNC_WF_WAKE_HOST, HAL_IOMUX_FUNC_PDM2_D, HAL_IOMUX_FUNC_I2S1_SDI1, },
    // P3_3
    { HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_FUNC_I2C_M2_SDA, HAL_IOMUX_FUNC_SPI_CLK,
      HAL_IOMUX_FUNC_SPILCD_DI3, HAL_IOMUX_FUNC_DISPLAY_SPI_DO2, HAL_IOMUX_FUNC_WF_UART_RTS, HAL_IOMUX_FUNC_SPILCD_DCN,
      HAL_IOMUX_FUNC_IR_RX, HAL_IOMUX_FUNC_PDM2_CK, HAL_IOMUX_FUNC_I2S1_SDI0, },
    // P3_4
    { HAL_IOMUX_FUNC_PWM4, HAL_IOMUX_FUNC_UART3_RX, HAL_IOMUX_FUNC_I2C_M0_SCL, HAL_IOMUX_FUNC_SPILCD_DI0,
      HAL_IOMUX_FUNC_SPI_DI1, HAL_IOMUX_FUNC_DISPLAY_SPI_DO1_DCN, HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_SPILCD_DCN,
      HAL_IOMUX_FUNC_IR_TX, HAL_IOMUX_FUNC_PDM0_CK, HAL_IOMUX_FUNC_I2S0_SDI3, },
    // P3_5
    { HAL_IOMUX_FUNC_PWM5, HAL_IOMUX_FUNC_UART3_TX, HAL_IOMUX_FUNC_I2C_M0_SDA, HAL_IOMUX_FUNC_SPILCD_DIO,
      HAL_IOMUX_FUNC_SPI_CS1, HAL_IOMUX_FUNC_DISPLAY_SPI_DIO, HAL_IOMUX_FUNC_CLK_32K_IN, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_DISPLAY_TE, HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_I2S0_SDI2, },
    // P3_6
    { HAL_IOMUX_FUNC_PWM6, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_I2C_M2_SCL, HAL_IOMUX_FUNC_SPILCD_CS0,
      HAL_IOMUX_FUNC_SPI_DI2, HAL_IOMUX_FUNC_DISPLAY_SPI_CS, HAL_IOMUX_FUNC_CLK_REQ_OUT, HAL_IOMUX_FUNC_SPDIF0_DI,
      HAL_IOMUX_FUNC_IR_RX, HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_I2S0_SDI1, },
    // P3_7
    { HAL_IOMUX_FUNC_PWM7, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_I2C_M2_SDA, HAL_IOMUX_FUNC_SPILCD_CLK,
      HAL_IOMUX_FUNC_SPI_CS2, HAL_IOMUX_FUNC_DISPLAY_SPI_CLK, HAL_IOMUX_FUNC_CLK_REQ_IN, HAL_IOMUX_FUNC_SPDIF0_DO,
      HAL_IOMUX_FUNC_IR_TX, HAL_IOMUX_FUNC_PDM2_D, HAL_IOMUX_FUNC_I2S0_SDI0, },
};

static struct IOMUX_T * const iomux = (struct IOMUX_T *)IOMUX_BASE;

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_ck_pin = DIG_MIC_CK_IOMUX_PIN;

static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_d0_pin = DIG_MIC_D0_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_d1_pin = DIG_MIC_D1_IOMUX_PIN;
static OPT_TYPE enum HAL_IOMUX_PIN_T digmic_d2_pin = DIG_MIC_D2_IOMUX_PIN;

void hal_iomux_set_default_config(void)
{
    uint32_t i;
    // Set all unused GPIOs to pull-down by default
    for (i = 0; i < 8; i++) {
        if (((iomux->REG_004 & (0xF << (i * 4))) >> (i * 4)) == 0xF) {
            iomux->REG_02C &= ~(1 << i);
            iomux->REG_030 |= (1 << i);
        }
    }
    for (i = 0; i < 6; i++) {
        if (((iomux->REG_008 & (0xF << (i * 4))) >> (i * 4)) == 0xF) {
            iomux->REG_02C &= ~(1 << (i + 8));
            iomux->REG_030 |= (1 << (i + 8));
        }
    }
    for (i = 0; i < 8; i++) {
        if (((iomux->REG_00C & (0xF << (i * 4))) >> (i * 4)) == 0xF) {
            iomux->REG_02C &= ~(1 << (i + 16));
            iomux->REG_030 |= (1 << (i + 16));
        }
    }
    for (i = 0; i < 8; i++) {
        if (((iomux->REG_010 & (0xF << (i * 4))) >> (i * 4)) == 0xF) {
            iomux->REG_02C &= ~(1 << (i + 24));
            iomux->REG_030 |= (1 << (i + 24));
        }
    }
}

uint32_t hal_iomux_check(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; ++i) {
    }
    return 0;
}

uint32_t hal_iomux_init(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count)
{
    uint32_t i;
    uint32_t ret;

    if (map == NULL)
        return 1;

    for (i = 0; i < count; ++i) {
        ret = hal_iomux_set_function(map[i].pin, map[i].function, HAL_IOMUX_OP_CLEAN_OTHER_FUNC_BIT);
        if (ret) {
            return (i << 8) + 1;
        }
		ret = hal_iomux_set_io_voltage_domains(map[i].pin, map[i].volt);
        if (ret) {
            return (i << 8) + 2;
        }
		ret = hal_iomux_set_io_pull_select(map[i].pin, map[i].pull_sel);
        if (ret) {
            return (i << 8) + 3;
        }
    }

    return 0;
}

uint32_t hal_iomux_set_function(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_FUNCTION_T func, enum HAL_IOMUX_OP_TYPE_T type)
{
    int i;
    uint8_t val;
    __IO uint32_t *reg;
    uint32_t shift;

    if (pin >= HAL_IOMUX_PIN_LED_NUM) {
        return 1;
    }
    if (func >= HAL_IOMUX_FUNC_END) {
        return 2;
    }

    if (pin == HAL_IOMUX_PIN_P1_6 || pin == HAL_IOMUX_PIN_P1_7) {
        if (func ==  HAL_IOMUX_FUNC_I2C_M0_SCL || func == HAL_IOMUX_FUNC_I2C_M0_SDA) {
            // Enable analog I2C slave
#ifndef FPGA
            iomux->REG_050 &= ~IOMUX_GPIO_I2C_MODE;
#endif
            // Set mcu GPIO func
            iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P16_SEL_MASK | IOMUX_GPIO_P17_SEL_MASK)) |
                IOMUX_GPIO_P16_SEL(IOMUX_FUNC_VAL_GPIO) | IOMUX_GPIO_P17_SEL(IOMUX_FUNC_VAL_GPIO);
            return 0;
        } else {
#ifndef FPGA
            iomux->REG_050 |= IOMUX_GPIO_I2C_MODE;
#endif
            // Continue to set the alt func
        }
    } else if (pin == HAL_IOMUX_PIN_P0_2) {
        if (func ==  HAL_IOMUX_FUNC_SPDIF0_DI) {
            iomux->REG_004 = SET_BITFIELD(iomux->REG_004, IOMUX_GPIO_P02_SEL, 6);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_P0_3) {
        if (func ==  HAL_IOMUX_FUNC_SPDIF0_DO) {
            iomux->REG_004 = SET_BITFIELD(iomux->REG_004, IOMUX_GPIO_P03_SEL, 6);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_P2_0) {
        if (func ==  HAL_IOMUX_FUNC_CLK_REQ_OUT) {
            iomux->REG_00C = SET_BITFIELD(iomux->REG_00C, IOMUX_GPIO_P20_SEL, 9);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_P2_1) {
        if (func == HAL_IOMUX_FUNC_CLK_REQ_IN) {
            iomux->REG_00C = SET_BITFIELD(iomux->REG_00C, IOMUX_GPIO_P21_SEL, 9);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        ASSERT(func == HAL_IOMUX_FUNC_GPIO, "Bad func=%d for IOMUX pin=%d", func, pin);
        return 0;
    }

    if (func == HAL_IOMUX_FUNC_GPIO) {
        val = IOMUX_FUNC_VAL_GPIO;
    } else {
        for (i = 0; i < IOMUX_ALT_FUNC_NUM; i++) {
            if (pin_func_map[pin][i] == func) {
                break;
            }
        }

        if (i == IOMUX_ALT_FUNC_NUM) {
            ASSERT(0, "[Func %d io %d] is unsupported by pin_func_map. Check it or use reg assignment like iomux_set_uart0.", pin, func);
            return 3;
        }
        val = index_to_func_val[i];
    }

    reg = &iomux->REG_004 + pin / 8;
    shift = (pin % 8) * 4;

    *reg = (*reg & ~(0xF << shift)) | (val << shift);

    return 0;
}

enum HAL_IOMUX_FUNCTION_T hal_iomux_get_function(enum HAL_IOMUX_PIN_T pin)
{
    return HAL_IOMUX_FUNC_NONE;
}

uint32_t hal_iomux_set_io_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    if (pin >= HAL_IOMUX_PIN_LED_NUM) {
        return 1;
    }
#if !defined(CHIP_A7_DSP)
    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        pmu_led_set_voltage_domains(pin, volt);
    }
#endif
    return 0;
}

uint32_t hal_iomux_set_io_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    if (pin >= HAL_IOMUX_PIN_LED_NUM) {
        return 1;
    }

    if (pin < HAL_IOMUX_PIN_NUM) {
        iomux->REG_02C &= ~(1 << pin);
        iomux->REG_030 &= ~(1 << pin);
        if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
            iomux->REG_02C |= (1 << pin);
        } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
            iomux->REG_030 |= (1 << pin);
        }
#if !defined(CHIP_A7_DSP)
    } else if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        pmu_led_set_pull_select(pin, pull_sel);
#endif
    }

    return 0;
}

uint32_t hal_iomux_set_io_drv(enum HAL_IOMUX_PIN_T pin, uint32_t val)
{
    if (pin >= HAL_IOMUX_PIN_NUM) {
        return 1;
    }
    if (val > 3) {
        return 2;
    }
    if (pin < HAL_IOMUX_PIN_P2_0)
        iomux->REG_074 = (iomux->REG_074 & ~(IOMUX_GPIO_P0_DRV0_SEL_MASK << 2*(pin-HAL_IOMUX_PIN_P0_0))) |
            (IOMUX_GPIO_P0_DRV0_SEL(val) << 2*(pin-HAL_IOMUX_PIN_P0_0));
    else
        iomux->REG_078 = (iomux->REG_078 & ~(IOMUX_GPIO_P0_DRV0_SEL_MASK << 2*(pin-HAL_IOMUX_PIN_P2_0))) |
            (IOMUX_GPIO_P0_DRV0_SEL(val) << 2*(pin-HAL_IOMUX_PIN_P2_0));
    return 0;
}

void hal_iomux_set_sdmmc_dt_n_out_group(int enable)
{
}

void hal_iomux_set_uart0_voltage(enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
}

void hal_iomux_set_uart1_voltage(enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
}

bool hal_iomux_uart0_connected(void)
{
    uint32_t reg_050, reg_008, reg_02c, reg_030;
    uint32_t mask;
    int val;

    // Save current iomux settings
    reg_050 = iomux->REG_050;
    reg_008 = iomux->REG_008;
    reg_02c = iomux->REG_02C;
    reg_030 = iomux->REG_030;

    // Disable analog I2C slave & master
#ifndef FPGA
    iomux->REG_050 |= IOMUX_GPIO_I2C_MODE | IOMUX_I2C0_M_SEL_GPIO;
#endif
    // Set uart0-rx as gpio
    iomux->REG_008 = SET_BITFIELD(iomux->REG_008, IOMUX_GPIO_P16_SEL, IOMUX_FUNC_VAL_GPIO);

    mask = (1 << HAL_IOMUX_PIN_P1_6);
    // Clear pullup
    iomux->REG_02C &= ~mask;
    // Setup pulldown
    iomux->REG_030 |= mask;

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P1_6, HAL_GPIO_DIR_IN, 0);

    hal_sys_timer_delay(MS_TO_TICKS(2));

    val = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P1_6);

    // Restore iomux settings
    iomux->REG_030 = reg_030;
    iomux->REG_02C = reg_02c;
    iomux->REG_008 = reg_008;
    iomux->REG_050 = reg_050;

    hal_sys_timer_delay(MS_TO_TICKS(2));

    return !!val;
}

bool hal_iomux_uart1_connected(void)
{
    uint32_t reg_00c, reg_02c, reg_030;
    uint32_t mask;
    int val;

    // Save current iomux settings
    reg_00c = iomux->REG_00C;
    reg_02c = iomux->REG_02C;
    reg_030 = iomux->REG_030;

    // Set uart1-rx as gpio
    iomux->REG_00C = SET_BITFIELD(iomux->REG_00C, IOMUX_GPIO_P20_SEL, IOMUX_FUNC_VAL_GPIO);

    mask = (1 << HAL_IOMUX_PIN_P2_0);
    // Clear pullup
    iomux->REG_02C &= ~mask;
    // Setup pulldown
    iomux->REG_030 |= mask;

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P2_0, HAL_GPIO_DIR_IN, 0);

    hal_sys_timer_delay(MS_TO_TICKS(2));

    val = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)HAL_IOMUX_PIN_P2_0);

    // Restore iomux settings
    iomux->REG_030 = reg_030;
    iomux->REG_02C = reg_02c;
    iomux->REG_00C = reg_00c;

    hal_sys_timer_delay(MS_TO_TICKS(2));

    return !!val;
}

void hal_iomux_set_uart0(void)
{
    uint32_t mask;

    // Disable analog I2C slave & master
#ifndef FPGA
    iomux->REG_050 |= IOMUX_GPIO_I2C_MODE | IOMUX_I2C0_M_SEL_GPIO;
#endif
    // Set uart0 func
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P16_SEL_MASK | IOMUX_GPIO_P17_SEL_MASK)) |
        IOMUX_GPIO_P16_SEL(1) | IOMUX_GPIO_P17_SEL(1);

    mask = (1 << HAL_IOMUX_PIN_P1_6) | (1 << HAL_IOMUX_PIN_P1_7);
    // Setup pullup
    iomux->REG_02C |= (1 << HAL_IOMUX_PIN_P1_6);
    iomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P1_7);
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

void hal_iomux_set_uart1(void)
{
    uint32_t mask_pd_c, mask_pu, mask_pu_c;

    // Set uart1 func
#if (UART1_IOMUX_INDEX == 02)
    iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P02_SEL_MASK | IOMUX_GPIO_P03_SEL_MASK)) |
        IOMUX_GPIO_P02_SEL(1) | IOMUX_GPIO_P03_SEL(1);
    mask_pd_c = (1 << HAL_IOMUX_PIN_P0_2) | (1 << HAL_IOMUX_PIN_P0_3);
    mask_pu = (1 << HAL_IOMUX_PIN_P0_2);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P0_3);
#elif (UART1_IOMUX_INDEX == 03) // 32: UART1 RX; 03 UART1 TX
    iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_GPIO_P32_SEL_MASK)) | IOMUX_GPIO_P32_SEL(1);
    iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P03_SEL_MASK)) | IOMUX_GPIO_P03_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P0_3) | (1 << HAL_IOMUX_PIN_P3_2);
    mask_pu = (1 << HAL_IOMUX_PIN_P3_2);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P0_3);
#elif (UART1_IOMUX_INDEX == 10)
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P10_SEL_MASK | IOMUX_GPIO_P11_SEL_MASK)) |
        IOMUX_GPIO_P10_SEL(1) | IOMUX_GPIO_P11_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P1_0) | (1 << HAL_IOMUX_PIN_P1_1);
    mask_pu = (1 << HAL_IOMUX_PIN_P1_0);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P1_1);
#elif (UART1_IOMUX_INDEX == 20)
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK | IOMUX_GPIO_P21_SEL_MASK)) |
        IOMUX_GPIO_P20_SEL(1) | IOMUX_GPIO_P21_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P2_0) | (1 << HAL_IOMUX_PIN_P2_1);
    mask_pu = (1 << HAL_IOMUX_PIN_P2_0);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P2_1);
#elif (UART1_IOMUX_INDEX == 30)
    iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_GPIO_P30_SEL_MASK | IOMUX_GPIO_P31_SEL_MASK)) |
        IOMUX_GPIO_P30_SEL(1) | IOMUX_GPIO_P31_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P3_0) | (1 << HAL_IOMUX_PIN_P3_1);
    mask_pu = (1 << HAL_IOMUX_PIN_P3_0);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P3_1);
#elif (UART1_IOMUX_INDEX == 32)
    iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_GPIO_P32_SEL_MASK | IOMUX_GPIO_P33_SEL_MASK)) |
        IOMUX_GPIO_P32_SEL(1) | IOMUX_GPIO_P33_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P3_2) | (1 << HAL_IOMUX_PIN_P3_3);
    mask_pu = (1 << HAL_IOMUX_PIN_P3_2);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P3_3);
#else
#error "Unsupported UART1_IOMUX_INDEX"
#endif
    // Setup pullup
    iomux->REG_02C |= mask_pu;
    iomux->REG_02C &= ~(mask_pu_c);
    // Clear pulldown
    iomux->REG_030 &= ~mask_pd_c;
}

void hal_iomux_set_uart2(void)
{
    uint32_t mask_pd_c, mask_pu, mask_pu_c;

    // Set uart1 func
#if (UART2_IOMUX_INDEX == 00)
    iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P00_SEL_MASK | IOMUX_GPIO_P01_SEL_MASK)) |
        IOMUX_GPIO_P00_SEL(1) | IOMUX_GPIO_P01_SEL(1);
    mask_pd_c = (1 << HAL_IOMUX_PIN_P0_0) | (1 << HAL_IOMUX_PIN_P0_1);
    mask_pu = (1 << HAL_IOMUX_PIN_P0_0);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P0_1);
#elif (UART2_IOMUX_INDEX == 12)
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P12_SEL_MASK | IOMUX_GPIO_P13_SEL_MASK)) |
        IOMUX_GPIO_P12_SEL(1) | IOMUX_GPIO_P13_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P1_2) | (1 << HAL_IOMUX_PIN_P1_3);
    mask_pu = (1 << HAL_IOMUX_PIN_P1_2);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P1_3);
#elif (UART2_IOMUX_INDEX == 14)
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P14_SEL_MASK | IOMUX_GPIO_P15_SEL_MASK)) |
        IOMUX_GPIO_P14_SEL(1) | IOMUX_GPIO_P15_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P1_4) | (1 << HAL_IOMUX_PIN_P1_5);
    mask_pu = (1 << HAL_IOMUX_PIN_P1_4);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P1_5);
#elif (UART2_IOMUX_INDEX == 22)
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P22_SEL_MASK | IOMUX_GPIO_P23_SEL_MASK)) |
        IOMUX_GPIO_P22_SEL(1) | IOMUX_GPIO_P23_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P2_2) | (1 << HAL_IOMUX_PIN_P2_3);
    mask_pu = (1 << HAL_IOMUX_PIN_P2_2);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P2_3);
#elif (UART2_IOMUX_INDEX == 36)
    iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_GPIO_P36_SEL_MASK | IOMUX_GPIO_P37_SEL_MASK)) |
        IOMUX_GPIO_P36_SEL(1) | IOMUX_GPIO_P37_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P3_6) | (1 << HAL_IOMUX_PIN_P3_7);
    mask_pu = (1 << HAL_IOMUX_PIN_P3_6);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P3_7);
#else
#error "Unsupported UART1_IOMUX_INDEX"
#endif
    // Setup pullup
    iomux->REG_02C |= mask_pu;
    iomux->REG_02C &= ~(mask_pu_c);
    // Clear pulldown
    iomux->REG_030 &= ~mask_pd_c;
}

void hal_iomux_set_uart3(void)
{
    uint32_t mask_pd_c, mask_pu, mask_pu_c;

    // Set uart1 func
#if (UART3_IOMUX_INDEX == 04)
    iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P04_SEL_MASK | IOMUX_GPIO_P05_SEL_MASK)) |
        IOMUX_GPIO_P04_SEL(1) | IOMUX_GPIO_P05_SEL(1);
    mask_pd_c = (1 << HAL_IOMUX_PIN_P0_4) | (1 << HAL_IOMUX_PIN_P0_5);
    mask_pu = (1 << HAL_IOMUX_PIN_P0_4);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P0_5);
#elif (UART3_IOMUX_INDEX == 06)
    iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P06_SEL_MASK | IOMUX_GPIO_P07_SEL_MASK)) |
        IOMUX_GPIO_P06_SEL(1) | IOMUX_GPIO_P07_SEL(1);
    mask_pd_c = (1 << HAL_IOMUX_PIN_P0_6) | (1 << HAL_IOMUX_PIN_P0_7);
    mask_pu = (1 << HAL_IOMUX_PIN_P0_6);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P0_7);
#elif (UART3_IOMUX_INDEX == 24)
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P24_SEL_MASK | IOMUX_GPIO_P25_SEL_MASK)) |
        IOMUX_GPIO_P24_SEL(1) | IOMUX_GPIO_P25_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P2_4) | (1 << HAL_IOMUX_PIN_P2_5);
    mask_pu = (1 << HAL_IOMUX_PIN_P2_4);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P2_5);
#elif (UART3_IOMUX_INDEX == 26)
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P26_SEL_MASK | IOMUX_GPIO_P27_SEL_MASK)) |
        IOMUX_GPIO_P26_SEL(1) | IOMUX_GPIO_P27_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P2_6) | (1 << HAL_IOMUX_PIN_P2_7);
    mask_pu = (1 << HAL_IOMUX_PIN_P2_6);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P2_7);
#elif (UART3_IOMUX_INDEX == 34)
    iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_GPIO_P34_SEL_MASK | IOMUX_GPIO_P35_SEL_MASK)) |
        IOMUX_GPIO_P34_SEL(1) | IOMUX_GPIO_P35_SEL(1);

    mask_pd_c = (1 << HAL_IOMUX_PIN_P3_4) | (1 << HAL_IOMUX_PIN_P3_5);
    mask_pu = (1 << HAL_IOMUX_PIN_P3_4);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P3_5);
#else
#error "Unsupported UART1_IOMUX_INDEX"
#endif
    // Setup pullup
    iomux->REG_02C |= mask_pu;
    iomux->REG_02C &= ~(mask_pu_c);
    // Clear pulldown
    iomux->REG_030 &= ~mask_pd_c;
}

void hal_iomux_set_analog_i2c(void)
{
    uint32_t mask;

    // Disable analog I2C master
    iomux->REG_050 |= IOMUX_I2C0_M_SEL_GPIO;
    // Set mcu GPIO func
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P16_SEL_MASK | IOMUX_GPIO_P17_SEL_MASK)) |
        IOMUX_GPIO_P16_SEL(0xF) | IOMUX_GPIO_P17_SEL(0xF);
    // Enable analog I2C slave
    iomux->REG_050 &= ~IOMUX_GPIO_I2C_MODE;

    mask = (1 << HAL_IOMUX_PIN_P1_6) | (1 << HAL_IOMUX_PIN_P1_7);
    // Setup pullup
    iomux->REG_02C |= mask;
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

void hal_iomux_set_jtag(void)
{
    uint32_t mask;
    uint32_t val;

    // SWCLK/TCK, SWDIO/TMS
    mask = IOMUX_GPIO_P01_SEL_MASK | IOMUX_GPIO_P00_SEL_MASK;
    val = IOMUX_GPIO_P01_SEL(7) | IOMUX_GPIO_P00_SEL(7);

    // TDI, TDO
#ifdef JTAG_TDI_TDO_PIN
    mask |= IOMUX_GPIO_P02_SEL_MASK | IOMUX_GPIO_P03_SEL_MASK;
    val |= IOMUX_GPIO_P02_SEL(7) | IOMUX_GPIO_P03_SEL(7);
#endif
    iomux->REG_004 = (iomux->REG_004 & ~mask) | val;

    // RESET
#if defined(JTAG_RESET_PIN) || defined(JTAG_TDI_TDO_PIN)
    iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P05_SEL_MASK)) | IOMUX_GPIO_P05_SEL(7);
#endif

    mask = (1 << HAL_IOMUX_PIN_P0_1) | (1 << HAL_IOMUX_PIN_P0_0);
#ifdef JTAG_TDI_TDO_PIN
    mask |= (1 << HAL_IOMUX_PIN_P0_2) | (1 << HAL_IOMUX_PIN_P0_3);
#endif
#if defined(JTAG_RESET_PIN) || defined(JTAG_TDI_TDO_PIN)
    mask |= (1 << HAL_IOMUX_PIN_P0_5);
#endif
    // Clear pullup
    iomux->REG_02C &= ~mask;
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

enum HAL_IOMUX_ISPI_ACCESS_T hal_iomux_ispi_access_enable(enum HAL_IOMUX_ISPI_ACCESS_T access)
{
    uint32_t v;

    v = iomux->REG_044;
    iomux->REG_044 |= access;

    return v;
}

enum HAL_IOMUX_ISPI_ACCESS_T hal_iomux_ispi_access_disable(enum HAL_IOMUX_ISPI_ACCESS_T access)
{
    uint32_t v;

    v = iomux->REG_044;
    iomux->REG_044 &= ~access;

    return v;
}

void hal_iomux_ispi_access_init(void)
{
    // Disable bt spi access ana/pmu interface
    hal_iomux_ispi_access_disable(HAL_IOMUX_ISPI_BT_ANA | HAL_IOMUX_ISPI_BT_PMU);
}

void hal_iomux_set_i2s0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_I2S0_WS,   I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_I2S0_SCK,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#if (I2S0_I_IOMUX_INDEX == 00)
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_I2S0_SDI0, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_I_IOMUX_INDEX == 13)
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_I2S0_SDI0, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_I_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_I2S0_SDI0, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported I2S0_I_IOMUX_INDEX"
#endif
#if (I2S0_I1_IOMUX_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_I2S0_SDI1, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_I1_IOMUX_INDEX == 36)
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_I2S0_SDI1, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (I2S0_I2_IOMUX_INDEX == 11)
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_I2S0_SDI2, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_I2_IOMUX_INDEX == 35)
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_I2S0_SDI2, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (I2S0_I3_IOMUX_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_I2S0_SDI3, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_I3_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_I2S0_SDI3, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif

#if (I2S0_O_IOMUX_INDEX == 01)
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_I2S0_SDO0,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S0_O_IOMUX_INDEX == 07)
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_I2S0_SDO0,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported I2S0_O_IOMUX_INDEX"
#endif
#if (I2S0_O1_IOMUX_INDEX == 06)
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_I2S0_SDO1,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (I2S0_O2_IOMUX_INDEX == 05)
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_I2S0_SDO2,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (I2S0_O3_IOMUX_INDEX == 04)
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_I2S0_SDO3,  I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
}

void hal_iomux_set_i2s1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_I2S1_WS,   I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_I2S1_SCK,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#if (I2S1_I_IOMUX_INDEX == 20)
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_I2S1_SDI0, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S1_I_IOMUX_INDEX == 33)
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_I2S1_SDI0, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported I2S1_I_IOMUX_INDEX"
#endif
#if (I2S1_I1_IOMUX_INDEX == 32)
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_I2S1_SDI1, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (I2S1_I2_IOMUX_INDEX == 31)
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_I2S1_SDI2, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (I2S1_I3_IOMUX_INDEX == 30)
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_I2S1_SDI3, I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif

#if (I2S1_O_IOMUX_INDEX == 21)
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_I2S1_SDO0,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S1_O_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_I2S1_SDO0,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported I2S1_O_IOMUX_INDEX"
#endif
#if (I2S1_O1_IOMUX_INDEX == 26)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_I2S1_SDO1,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (I2S1_O2_IOMUX_INDEX == 25)
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_I2S1_SDO2,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (I2S1_O3_IOMUX_INDEX == 24)
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_I2S1_SDO3,  I2S1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
}

void hal_iomux_set_i2s_mclk(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
#if (I2S_MCLK_IOMUX_INDEX == 04)
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S_MCLK_IOMUX_INDEX == 13)
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S_MCLK_IOMUX_INDEX == 15)
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S_MCLK_IOMUX_INDEX == 20)
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S_MCLK_IOMUX_INDEX == 22)
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S_MCLK_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S_MCLK_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_I2S_MCLK, I2S0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported I2S_MCLK_IOMUX_INDEX"
#endif
    };

    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
}

void hal_iomux_set_spdif0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spdif[] = {
#if (SPDIF0_I_IOMUX_INDEX == 02)
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (SPDIF0_I_IOMUX_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (SPDIF0_I_IOMUX_INDEX == 20)
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (SPDIF0_I_IOMUX_INDEX == 26)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (SPDIF0_I_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (SPDIF0_I_IOMUX_INDEX == 24)
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
#error "Unsupported SPDIF0_I_IOMUX_INDEX"
#endif

#if (SPDIF0_O_IOMUX_INDEX == 03)
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPDIF0_O_IOMUX_INDEX == 11)
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPDIF0_O_IOMUX_INDEX == 21)
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPDIF0_O_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPDIF0_O_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPDIF0_O_IOMUX_INDEX == 07)
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported SPDIF0_O_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_spdif, ARRAY_SIZE(pinmux_spdif));
}


void hal_iomux_set_dig_mic(uint32_t map)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic_clk[] = {
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_PDM0_CK, DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic0[] = {
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_PDM0_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic1[] = {
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_PDM1_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic2[] = {
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_PDM2_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    if (digmic_ck_pin == HAL_IOMUX_PIN_P0_0) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P0_0;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM0_CK;
    } else if (digmic_ck_pin == HAL_IOMUX_PIN_P0_4) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P0_4;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM1_CK;
    } else if (digmic_ck_pin == HAL_IOMUX_PIN_P3_3) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P3_3;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM2_CK;
    } else if (digmic_ck_pin == HAL_IOMUX_PIN_P3_4) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P3_4;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM0_CK;
    }

    if (digmic_d0_pin == HAL_IOMUX_PIN_P0_1) {
        pinmux_digitalmic0[0].pin = HAL_IOMUX_PIN_P0_1;
    } else if (digmic_d0_pin == HAL_IOMUX_PIN_P0_5) {
        pinmux_digitalmic0[0].pin = HAL_IOMUX_PIN_P0_5;
    } else if (digmic_d0_pin == HAL_IOMUX_PIN_P3_0) {
        pinmux_digitalmic0[0].pin = HAL_IOMUX_PIN_P3_0;
    } else if (digmic_d0_pin == HAL_IOMUX_PIN_P3_5) {
        pinmux_digitalmic0[0].pin = HAL_IOMUX_PIN_P3_5;
    }

    if (digmic_d1_pin == HAL_IOMUX_PIN_P0_2) {
        pinmux_digitalmic1[0].pin = HAL_IOMUX_PIN_P0_2;
    } else if (digmic_d1_pin == HAL_IOMUX_PIN_P0_6) {
        pinmux_digitalmic1[0].pin = HAL_IOMUX_PIN_P0_6;
    } else if (digmic_d1_pin == HAL_IOMUX_PIN_P3_1) {
        pinmux_digitalmic1[0].pin = HAL_IOMUX_PIN_P3_1;
    } else if (digmic_d1_pin == HAL_IOMUX_PIN_P3_6) {
        pinmux_digitalmic1[0].pin = HAL_IOMUX_PIN_P3_6;
    }

    if (digmic_d2_pin == HAL_IOMUX_PIN_P0_3) {
        pinmux_digitalmic2[0].pin = HAL_IOMUX_PIN_P0_3;
    } else if (digmic_d2_pin == HAL_IOMUX_PIN_P0_7) {
        pinmux_digitalmic2[0].pin = HAL_IOMUX_PIN_P0_7;
    } else if (digmic_d2_pin == HAL_IOMUX_PIN_P3_2) {
        pinmux_digitalmic2[0].pin = HAL_IOMUX_PIN_P3_2;
    } else if (digmic_d2_pin == HAL_IOMUX_PIN_P3_7) {
        pinmux_digitalmic2[0].pin = HAL_IOMUX_PIN_P3_7;
    }

    if ((map & 0xF) == 0) {
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_GPIO;
    }
    hal_iomux_init(pinmux_digitalmic_clk, ARRAY_SIZE(pinmux_digitalmic_clk));
    if (map & (1 << 0)) {
        hal_iomux_init(pinmux_digitalmic0, ARRAY_SIZE(pinmux_digitalmic0));
    }
    if (map & (1 << 1)) {
        hal_iomux_init(pinmux_digitalmic1, ARRAY_SIZE(pinmux_digitalmic1));
    }
    if (map & (1 << 2)) {
        hal_iomux_init(pinmux_digitalmic2, ARRAY_SIZE(pinmux_digitalmic2));
    }
}

void hal_iomux_set_spi(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi[] = {
#if (SPI_IOMUX_INDEX == 04)
#ifdef SPI_IOMUX_4WIRE
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_SPI_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_SPI_CS0,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},

#elif (SPI_IOMUX_INDEX == 24)
#ifdef SPI_IOMUX_4WIRE
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_SPI_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_SPI_CS0,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},

#elif (SPI_IOMUX_INDEX == 30)
#ifdef SPI_IOMUX_4WIRE
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_SPI_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_SPI_CS0,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported SPI_IOMUX_INDEX"
#endif

#if (SPI_IOMUX_CS1_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SPI_CS1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPI_IOMUX_CS1_INDEX == 35)
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SPI_CS1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (SPI_IOMUX_CS2_INDEX == 11)
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPI_CS2,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPI_IOMUX_CS2_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPI_CS2,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (SPI_IOMUX_CS3_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SPI_CS3,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPI_IOMUX_CS3_INDEX == 25)
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_SPI_CS3,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif

#ifdef SPI_IOMUX_4WIRE
#if (SPI_IOMUX_DI1_INDEX == 13)
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SPI_DI1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPI_IOMUX_DI1_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SPI_DI1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (SPI_IOMUX_DI2_INDEX == 14)
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SPI_DI2,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPI_IOMUX_DI2_INDEX == 36)
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_SPI_DI2,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (SPI_IOMUX_DI3_INDEX == 15)
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SPI_DI3,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPI_IOMUX_DI3_INDEX == 24)
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_SPI_DI3,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#endif
    };

    hal_iomux_init(pinmux_spi, ARRAY_SIZE(pinmux_spi));
}

void hal_iomux_set_spilcd_slave(void)
{
    iomux->REG_050 |= IOMUX_SPILCD1_MASTER_N;
}

void hal_iomux_set_spilcd(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd[] = {
#if (SPILCD_IOMUX_INDEX == 00)
#ifdef SPILCD_IOMUX_4WIRE
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SPILCD_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_SPILCD_CS0,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},

#elif (SPILCD_IOMUX_INDEX == 10)
#ifdef SPILCD_IOMUX_4WIRE
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SPILCD_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPILCD_CS0,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},

#elif (SPILCD_IOMUX_INDEX == 20)
#ifdef SPILCD_IOMUX_4WIRE
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_SPILCD_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_SPILCD_CS0,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},

#elif (SPILCD_IOMUX_INDEX == 34)
#ifdef SPILCD_IOMUX_4WIRE
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SPILCD_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_SPILCD_CS0,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported SPILCD_IOMUX_INDEX"
#endif

#if (SPILCD_IOMUX_CS1_INDEX == 05)
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_SPILCD_CS1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_CS1_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SPILCD_CS1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_CS1_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_SPILCD_CS1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (SPILCD_IOMUX_CS2_INDEX == 06)
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_SPILCD_CS2,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_CS2_INDEX == 31)
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_SPILCD_CS2,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (SPILCD_IOMUX_CS3_INDEX == 07)
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SPILCD_CS3,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_CS3_INDEX == 32)
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_SPILCD_CS3,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif

#ifdef SPILCD_IOMUX_4WIRE
#if (SPILCD_IOMUX_DI1_INDEX == 02)
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_SPILCD_DI1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_DI1_INDEX == 26)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_SPILCD_DI1,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (SPILCD_IOMUX_DI2_INDEX == 03)
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SPILCD_DI2,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_DI2_INDEX == 30)
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_SPILCD_DI2,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#if (SPILCD_IOMUX_DI3_INDEX == 04)
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_SPILCD_DI3,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_DI3_INDEX == 33)
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_SPILCD_DI3,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
#endif
    };

    hal_iomux_init(pinmux_spilcd, ARRAY_SIZE(pinmux_spilcd));
}

void hal_iomux_set_i2c0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if   (I2C0_IOMUX_INDEX == 00)
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C0_IOMUX_INDEX == 04)
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C0_IOMUX_INDEX == 16)
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C0_IOMUX_INDEX == 20)
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C0_IOMUX_INDEX == 26)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C0_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
#error "Unsupported I2C0_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
    iomux->REG_050 |= IOMUX_I2C0_M_SEL_GPIO;
}

void hal_iomux_set_i2c1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if   (I2C1_IOMUX_INDEX == 02)
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C1_IOMUX_INDEX == 06)
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C1_IOMUX_INDEX == 14)
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C1_IOMUX_INDEX == 22)
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C1_IOMUX_INDEX == 30)
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
#error "Unsupported I2C1_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
    iomux->REG_050 |= IOMUX_I2C1_M_SEL_GPIO;
}

void hal_iomux_set_i2c2(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if   (I2C2_IOMUX_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_I2C_M2_SCL, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_I2C_M2_SDA, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C2_IOMUX_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_I2C_M2_SCL, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_I2C_M2_SDA, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C2_IOMUX_INDEX == 24)
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_I2C_M2_SCL, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_I2C_M2_SDA, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C2_IOMUX_INDEX == 32)
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_I2C_M2_SCL, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_I2C_M2_SDA, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#elif (I2C2_IOMUX_INDEX == 36)
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_I2C_M2_SCL, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_I2C_M2_SDA, I2C2_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENABLE},
#else
#error "Unsupported I2C2_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
    iomux->REG_050 |= IOMUX_I2C2_M_SEL_GPIO;
}

void hal_iomux_set_pwm0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
#if   (PWM0_IOMUX_INDEX == 00)
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_PWM0, PWM0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM0_IOMUX_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_PWM0, PWM0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM0_IOMUX_INDEX == 20)
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_PWM0, PWM0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM0_IOMUX_INDEX == 30)
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_PWM0, PWM0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported PWM0_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
}

void hal_iomux_set_pwm1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
#if   (PWM1_IOMUX_INDEX == 01)
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_PWM1, PWM1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM1_IOMUX_INDEX == 11)
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_PWM1, PWM1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM1_IOMUX_INDEX == 21)
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_PWM1, PWM1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM1_IOMUX_INDEX == 31)
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_PWM1, PWM1_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported PWM1_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
}

void hal_iomux_set_pwm2(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
#if   (PWM2_IOMUX_INDEX == 02)
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_PWM2, PWM2_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM2_IOMUX_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_PWM2, PWM2_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM2_IOMUX_INDEX == 22)
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_PWM2, PWM2_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM2_IOMUX_INDEX == 32)
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_PWM2, PWM2_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported PWM2_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
}

void hal_iomux_set_pwm3(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
#if   (PWM3_IOMUX_INDEX == 03)
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_PWM3, PWM3_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM3_IOMUX_INDEX == 13)
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_PWM3, PWM3_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM3_IOMUX_INDEX == 23)
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_PWM3, PWM3_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM3_IOMUX_INDEX == 33)
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_PWM3, PWM3_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported PWM3_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
}

void hal_iomux_set_pwm4(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
#if   (PWM4_IOMUX_INDEX == 04)
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_PWM4, PWM4_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM4_IOMUX_INDEX == 14)
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_PWM4, PWM4_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM4_IOMUX_INDEX == 24)
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_PWM4, PWM4_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM4_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_PWM4, PWM4_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported PWM4_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
}

void hal_iomux_set_pwm5(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
#if   (PWM5_IOMUX_INDEX == 05)
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_PWM5, PWM5_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM5_IOMUX_INDEX == 15)
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_PWM5, PWM5_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM5_IOMUX_INDEX == 25)
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_PWM5, PWM5_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM5_IOMUX_INDEX == 35)
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_PWM5, PWM5_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported PWM5_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
}

void hal_iomux_set_pwm6(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
#if   (PWM6_IOMUX_INDEX == 06)
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_PWM6, PWM6_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM6_IOMUX_INDEX == 16)
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_PWM6, PWM6_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM6_IOMUX_INDEX == 26)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_PWM6, PWM6_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM6_IOMUX_INDEX == 36)
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_PWM6, PWM6_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported PWM6_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
}

void hal_iomux_set_pwm7(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
#if   (PWM7_IOMUX_INDEX == 07)
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_PWM7, PWM7_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM7_IOMUX_INDEX == 17)
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_PWM7, PWM7_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM7_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_PWM7, PWM7_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (PWM7_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_PWM7, PWM7_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported PWM7_IOMUX_INDEX"
#endif
    };
    hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
}

void hal_iomux_set_ir(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_ir[] = {
#if   (IR_RX_IOMUX_INDEX == 00)
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 02)
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 06)
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 14)
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 22)
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 26)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 31)
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 33)
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_RX_IOMUX_INDEX == 36)
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_IR_RX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported IR_RX_IOMUX_INDEX"
#endif

#if   (IR_TX_IOMUX_INDEX == 01)
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 03)
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 07)
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 11)
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 13)
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 15)
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 21)
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 32)
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (IR_TX_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_IR_TX, IR_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported IR_TX_IOMUX_INDEX"
#endif
    };

    hal_iomux_init(pinmux_ir, ARRAY_SIZE(pinmux_ir));
}

void hal_iomux_set_sdmmc(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_sdmmc[] = {
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SDMMC_CLK,   SDMMC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SDMMC_CMD,   SDMMC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SDMMC_DATA0, SDMMC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SDMMC_DATA1, SDMMC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SDMMC_DATA2, SDMMC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SDMMC_DATA3, SDMMC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    hal_iomux_init(pinmux_sdmmc, ARRAY_SIZE(pinmux_sdmmc));
}

void hal_iomux_set_clock_out(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clkout[] = {
#if (CLKOUT_IOMUX_INDEX == 04)
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 13)
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 15)
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 20)
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 21)
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 22)
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 23)
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
#error "Unsupported CLKOUT_IOMUX_INDEX"
#endif
    };

    hal_iomux_init(pinmux_clkout, ARRAY_SIZE(pinmux_clkout));
}

void hal_iomux_set_mcu_clock_out(void)
{
}

void hal_iomux_set_bt_clock_out(void)
{
}

void hal_iomux_set_bt_tport(void)
{
    ///TODO:
    return ;
    // P0_0 ~ P0_3,
    iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P00_SEL_MASK | IOMUX_GPIO_P01_SEL_MASK | IOMUX_GPIO_P02_SEL_MASK | IOMUX_GPIO_P03_SEL_MASK)) |
    IOMUX_GPIO_P00_SEL(0xA) | IOMUX_GPIO_P01_SEL(0xA) | IOMUX_GPIO_P02_SEL(0xA) |IOMUX_GPIO_P03_SEL(0xA);
    //P1_0 ~ P1_3,
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P10_SEL_MASK | IOMUX_GPIO_P11_SEL_MASK | IOMUX_GPIO_P12_SEL_MASK | IOMUX_GPIO_P13_SEL_MASK )) |
    IOMUX_GPIO_P10_SEL(0xA) | IOMUX_GPIO_P11_SEL(0xA) | IOMUX_GPIO_P12_SEL(0xA) | IOMUX_GPIO_P13_SEL(0xA);
    // ANA TEST DIR
    iomux->REG_014 = 0x0f0f;
    // ANA TEST SEL
    iomux->REG_018 = IOMUX_ANA_TEST_SEL(5);
}

void hal_iomux_set_bt_rf_sw(int rx_on, int tx_on)
{
    ///TODO:
    return ;
    uint32_t val;
    uint32_t dir;

    //iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P00_SEL_MASK | IOMUX_GPIO_P01_SEL_MASK)) |
    //    IOMUX_GPIO_P00_SEL(6) | IOMUX_GPIO_P01_SEL(6);

    val = iomux->REG_004;
    dir = 0;
    if (rx_on) {
        val = SET_BITFIELD(val, IOMUX_GPIO_P00_SEL, 0xA);
        dir = (1 << HAL_IOMUX_PIN_P0_0);
    }
    if (tx_on) {
        val = SET_BITFIELD(val, IOMUX_GPIO_P01_SEL, 0xA);
        dir = (1 << HAL_IOMUX_PIN_P0_1);
    }
    iomux->REG_004 = val;
    // ANA TEST DIR
    iomux->REG_014 |= dir;
    // ANA TEST SEL
    iomux->REG_018 = IOMUX_ANA_TEST_SEL(5);
}
void hal_iomux_set_wifi_uart(void)
{
#if (WIFI_UART_IOMUX_INDEX == 20)
    uint32_t mask_pd, mask_pu, mask_pu_c;
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK | IOMUX_GPIO_P21_SEL_MASK)) |
        IOMUX_GPIO_P20_SEL(8) | IOMUX_GPIO_P21_SEL(8);

    mask_pd = (1 << HAL_IOMUX_PIN_P2_0) | (1 << HAL_IOMUX_PIN_P2_1);
    mask_pu = (1 << HAL_IOMUX_PIN_P2_0);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P2_1);

    // Setup pullup
    iomux->REG_02C |= mask_pu;
    iomux->REG_02C &= ~(mask_pu_c);
    // Clear pulldown
    iomux->REG_030 &= ~mask_pd;

#elif (WIFI_UART_IOMUX_INDEX == 21)
    uint32_t mask_pd, mask_pu, mask_pu_c;
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P21_SEL_MASK)) | IOMUX_GPIO_P21_SEL(8);

    mask_pd = (1 << HAL_IOMUX_PIN_P2_1);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P2_1);

    // Setup pullup
    // iomux->REG_02C |= mask_pu;
    iomux->REG_02C &= ~(mask_pu_c);
    // Clear pulldown
    iomux->REG_030 &= ~mask_pd;

#elif (WIFI_UART_IOMUX_INDEX == 30)
    uint32_t mask_pd, mask_pu, mask_pu_c;
    iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_GPIO_P30_SEL_MASK | IOMUX_GPIO_P31_SEL_MASK)) |
        IOMUX_GPIO_P30_SEL(7) | IOMUX_GPIO_P31_SEL(7);

    mask_pd = (1 << HAL_IOMUX_PIN_P3_0) | (1 << HAL_IOMUX_PIN_P3_1);
    mask_pu = (1 << HAL_IOMUX_PIN_P3_0);
    mask_pu_c = (1 << HAL_IOMUX_PIN_P3_1);

    // Setup pullup
    iomux->REG_02C |= mask_pu;
    iomux->REG_02C &= ~(mask_pu_c);
    // Clear pulldown
    iomux->REG_030 &= ~mask_pd;

#else
    //default:nothing
#endif
}

int WEAK hal_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type)
{
    uint32_t v;

    if (type == HAL_PWRKEY_IRQ_NONE) {
        v = IOMUX_POWER_KEY_ON_INT_STATUS | IOMUX_POWER_KEY_OFF_INT_STATUS;
    } else if (type == HAL_PWRKEY_IRQ_FALLING_EDGE) {
        v = IOMUX_POWER_KEY_ON_INT_EN | IOMUX_POWER_KEY_ON_INT_MSK;
    } else if (type == HAL_PWRKEY_IRQ_RISING_EDGE) {
        v = IOMUX_POWER_KEY_OFF_INT_EN | IOMUX_POWER_KEY_OFF_INT_MSK;
    } else if (type == HAL_PWRKEY_IRQ_BOTH_EDGE) {
        v = IOMUX_POWER_KEY_ON_INT_EN | IOMUX_POWER_KEY_ON_INT_MSK |
            IOMUX_POWER_KEY_OFF_INT_EN | IOMUX_POWER_KEY_OFF_INT_MSK;
    } else {
        return 1;
    }

    iomux->REG_040 = v;

    return 0;
}


bool WEAK hal_pwrkey_pressed(void)
{
    uint32_t v = iomux->REG_040;
    return !!(v & IOMUX_POWER_ON_FEEDOUT);

}

bool hal_pwrkey_startup_pressed(void)
{
    return hal_pwrkey_pressed();
}

enum HAL_PWRKEY_IRQ_T WEAK hal_pwrkey_get_irq_state(void)
{
    enum HAL_PWRKEY_IRQ_T state = HAL_PWRKEY_IRQ_NONE;
    uint32_t v = iomux->REG_040;

    if (v & IOMUX_R_POWER_KEY_INTR_U) {
        state |= HAL_PWRKEY_IRQ_FALLING_EDGE;
    }

    if (v & IOMUX_R_POWER_KEY_INTR_D) {
        state |= HAL_PWRKEY_IRQ_RISING_EDGE;
    }

    return state;
}

void hal_iomux_set_codec_gpio_trigger(enum HAL_IOMUX_PIN_T pin, bool polarity)
{
    iomux->REG_064 = SET_BITFIELD(iomux->REG_064, IOMUX_CFG_CODEC_TRIG_SEL, pin);
    if (polarity) {
        iomux->REG_064 &= ~IOMUX_CFG_CODEC_TRIG_POL;
    } else {
        iomux->REG_064 |= IOMUX_CFG_CODEC_TRIG_POL;
    }
}

void hal_iomux_single_wire_uart_rx(uint32_t uart)
{
#ifdef UART_HALF_DUPLEX
#define SUART_TX_PIN_PULL_SEL_IN_RX         HAL_IOMUX_PIN_NOPULL
#else
#define SUART_TX_PIN_PULL_SEL_IN_RX         HAL_IOMUX_PIN_PULLUP_ENABLE
#endif

    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_uart[] =
    {
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_GPIO,     HAL_IOMUX_PIN_VOLTAGE_VIO, SUART_TX_PIN_PULL_SEL_IN_RX},
    };

    if (uart == HAL_UART_ID_2) {
        pinmux_uart[0].pin = HAL_IOMUX_PIN_P2_2;
        pinmux_uart[0].function = HAL_IOMUX_FUNC_UART2_RX;
        pinmux_uart[1].pin = HAL_IOMUX_PIN_P2_3;
#ifdef UART_HALF_DUPLEX
        iomux->REG_050 &= ~IOMUX_UART2_HALFN;
    } else {
        iomux->REG_050 &= ~IOMUX_UART1_HALFN;
#endif
    }

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_uart[0].pin, HAL_GPIO_DIR_IN, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_uart[1].pin, HAL_GPIO_DIR_IN, 1);

    hal_iomux_init(pinmux_uart, ARRAY_SIZE(pinmux_uart));

#ifndef UART_HALF_DUPLEX
    hal_uart_flush(uart, 0);
#endif
}

void hal_iomux_single_wire_uart_tx(uint32_t uart)
{
#ifndef UART_HALF_DUPLEX
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_uart[] =
    {
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_GPIO,     HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };

    if (uart == HAL_UART_ID_2) {
        pinmux_uart[0].pin = HAL_IOMUX_PIN_P2_2;
        pinmux_uart[1].pin = HAL_IOMUX_PIN_P2_3;
        pinmux_uart[1].function = HAL_IOMUX_FUNC_UART2_TX;
    }

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_uart[0].pin, HAL_GPIO_DIR_IN, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_uart[1].pin, HAL_GPIO_DIR_IN, 1);

    hal_iomux_init(pinmux_uart, ARRAY_SIZE(pinmux_uart));
#endif
}

void hal_iomux_set_dsi_te(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] =
    {
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_DISPLAY_TE, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };

    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
}

enum HAL_IOMUX_PIN_T hal_iomux_get_dsi_te_pin(void)
{
    return HAL_IOMUX_PIN_P2_1;
}

void hal_iomux_set_wf_fem(int rf_switch)
{
    uint32_t mask_pd, mask_pu;
#if (1 == WIFI_RF_SWITCH) //bes EVB v2

    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK)) | IOMUX_GPIO_P20_SEL(13);

    // mask_pd = (1 << HAL_IOMUX_PIN_P2_0) | (1 << HAL_IOMUX_PIN_P2_1);
    mask_pd = (1 << HAL_IOMUX_PIN_P2_0);
    mask_pu = (1 << HAL_IOMUX_PIN_P2_0);
    // mask_pu_c = (1 << HAL_IOMUX_PIN_P2_1);

    // Setup voltage as VIO
    iomux->REG_090 &= ~(IOMUX_GPIO_P20_SEL_VIO);

    // Setup pullup
    iomux->REG_02C |= mask_pu;
    // iomux->REG_02C &= ~(mask_pu_c);
    // Clear pulldown
    iomux->REG_030 &= ~mask_pd;

#else
    if( (rf_switch  == 20) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4 )
    {
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P24_SEL_MASK)) | IOMUX_GPIO_P24_SEL(13);

        mask_pd = (1 << HAL_IOMUX_PIN_P2_4);
        mask_pu = (1 << HAL_IOMUX_PIN_P2_4);

        // Setup voltage as VIO
        iomux->REG_094 &= ~(IOMUX_GPIO_P24_SEL_VIO);

        // Setup pullup
        iomux->REG_02C |= mask_pu;
        // Clear pulldown
        iomux->REG_030 &= ~mask_pd;
    }

    if( (rf_switch  == 12) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P14_SEL_MASK)) | IOMUX_GPIO_P14_SEL(13);

        mask_pd = (1 << HAL_IOMUX_PIN_P1_4);
        mask_pu = (1 << HAL_IOMUX_PIN_P1_4);

        // Setup voltage as VIO
        iomux->REG_090 &= ~(IOMUX_GPIO_P14_SEL_VIO);

        // Setup pullup
        iomux->REG_02C |= mask_pu;
        // Clear pulldown
        iomux->REG_030 &= ~mask_pd;
    }

    if( (rf_switch  == 16) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK)) | IOMUX_GPIO_P20_SEL(13);

        // mask_pd = (1 << HAL_IOMUX_PIN_P2_0) | (1 << HAL_IOMUX_PIN_P2_1);
        mask_pd = (1 << HAL_IOMUX_PIN_P2_0);
        mask_pu = (1 << HAL_IOMUX_PIN_P2_0);
        // mask_pu_c = (1 << HAL_IOMUX_PIN_P2_1);

        // Setup voltage as VIO
        iomux->REG_094 &= ~(IOMUX_GPIO_P20_SEL_VIO);

        // Setup pullup
        iomux->REG_02C |= mask_pu;
        // iomux->REG_02C &= ~(mask_pu_c);
        // Clear pulldown
        iomux->REG_030 &= ~mask_pd;
    }

    if( (rf_switch  == 2) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P02_SEL_MASK)) | IOMUX_GPIO_P02_SEL(13);

        mask_pd = (1 << HAL_IOMUX_PIN_P0_2);
        mask_pu = (1 << HAL_IOMUX_PIN_P0_2);

        // Setup voltage as VIO
        iomux->REG_090 &= ~(IOMUX_GPIO_P02_SEL_VIO);

        // Setup pullup
        iomux->REG_02C |= mask_pu;
        // Clear pulldown
        iomux->REG_030 &= ~mask_pd;
    }

    //gpio12(sw2)5g txon; gpio13(sw3)0:bt, 1:2g4;
    if( (rf_switch  == 10) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P12_SEL_MASK)) | IOMUX_GPIO_P12_SEL(13);
        iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P13_SEL_MASK)) | IOMUX_GPIO_P13_SEL(13);
    }

    //gpio4 no 5G 2.4g/bt --> 1: 2.4, 0:bt;
    if( (rf_switch  == 4) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P04_SEL_MASK)) | IOMUX_GPIO_P04_SEL(13);
    }

    //5G GPIO-02 -sw1,2.4G GPIO-04-sw4
    if( (rf_switch  == 6) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P02_SEL_MASK)) | IOMUX_GPIO_P02_SEL(13);

        mask_pd = (1 << HAL_IOMUX_PIN_P0_2);
        mask_pu = (1 << HAL_IOMUX_PIN_P0_2);

        // Setup voltage as VIO
        iomux->REG_090 &= ~(IOMUX_GPIO_P02_SEL_VIO);

        // Setup pullup
        iomux->REG_02C |= mask_pu;
        // Clear pulldown
        iomux->REG_030 &= ~mask_pd;

        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P04_SEL_MASK)) | IOMUX_GPIO_P04_SEL(13);

        mask_pd = (1 << HAL_IOMUX_PIN_P0_4);
        mask_pu = (1 << HAL_IOMUX_PIN_P0_4);

        // Setup voltage as VIO
        iomux->REG_090 &= ~(IOMUX_GPIO_P04_SEL_VIO);

        // Setup pullup
        iomux->REG_02C |= mask_pu;
        // Clear pulldown
        iomux->REG_030 &= ~mask_pd;

    }

    if( (rf_switch  == 25) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_GPIO_P31_SEL_MASK)) | IOMUX_GPIO_P31_SEL(13);

        mask_pd = (1 << HAL_IOMUX_PIN_P3_1);
        mask_pu = (1 << HAL_IOMUX_PIN_P3_1);

        // Setup voltage as VIO
        iomux->REG_094 &= ~(IOMUX_GPIO_P31_SEL_VIO);

        // Setup pullup
        iomux->REG_02C |= mask_pu;
        // Clear pulldown
        iomux->REG_030 &= ~mask_pd;
    }

    //gpio4:epta; gpio20: rxon
    if ((rf_switch  == 100) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P04_SEL_MASK)) | IOMUX_GPIO_P04_SEL(13);
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK)) | IOMUX_GPIO_P20_SEL(13);
    }

    //gpio02 rxon
    if ((rf_switch  == 102) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P02_SEL_MASK)) | IOMUX_GPIO_P02_SEL(13);
    }
    //gpio35 :epta;
    if ((rf_switch  == 105) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_GPIO_P35_SEL_MASK)) | IOMUX_GPIO_P35_SEL(13);
    }

    /*out fem */
    if( (rf_switch  == 37) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4 )
    {
        iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P13_SEL_MASK)) | IOMUX_GPIO_P13_SEL(13);
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P24_SEL_MASK)) | IOMUX_GPIO_P24_SEL(13);
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P27_SEL_MASK)) | IOMUX_GPIO_P27_SEL(13);
    }
    if( (rf_switch  == 101) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4 )
    {
        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P02_SEL_MASK)) | IOMUX_GPIO_P02_SEL(13);
        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P04_SEL_MASK)) | IOMUX_GPIO_P04_SEL(13);
    }
    //gpio13(sw2)5g txon; gpio12(sw3)0:bt, 1:2g4;
    if((rf_switch  == 103) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P12_SEL_MASK)) | IOMUX_GPIO_P12_SEL(13);
        iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P13_SEL_MASK)) | IOMUX_GPIO_P13_SEL(13);
    }
    if( (rf_switch  == 104) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4 )
    {
        iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P12_SEL_MASK)) | IOMUX_GPIO_P12_SEL(13);
        iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P13_SEL_MASK)) | IOMUX_GPIO_P13_SEL(13);
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P24_SEL_MASK)) | IOMUX_GPIO_P24_SEL(13);
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P27_SEL_MASK)) | IOMUX_GPIO_P27_SEL(13);
    }

    if ((rf_switch  == 106) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        iomux->REG_004 = (iomux->REG_004 & ~(IOMUX_GPIO_P04_SEL_MASK)) | IOMUX_GPIO_P04_SEL(13);
    }

    if( (rf_switch  == 107) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4 )
    {
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P22_SEL_MASK)) | IOMUX_GPIO_P22_SEL(13);
        mask_pd = (1 << HAL_IOMUX_PIN_P2_2);
        mask_pu = (1 << HAL_IOMUX_PIN_P2_2);
        // Setup voltage as VIO
        iomux->REG_094 &= ~(IOMUX_GPIO_P22_SEL_VIO);

        // Setup pullup
        iomux->REG_02C |= mask_pu;
        // Clear pulldown
        iomux->REG_030 &= ~mask_pd;
    }

    if( (rf_switch  == 108) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4 )
    {
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P22_SEL_MASK)) | IOMUX_GPIO_P22_SEL(13);
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK)) | IOMUX_GPIO_P20_SEL(13);
    }

    if( (rf_switch  == 109) && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4 )
    {
        iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK)) | IOMUX_GPIO_P20_SEL(13);
    }

#endif

}
