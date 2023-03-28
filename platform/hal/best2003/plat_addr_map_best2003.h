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
#ifndef __PLAT_ADDR_MAP_BEST2003_H__
#define __PLAT_ADDR_MAP_BEST2003_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_BASE                                0x00020000
#define ROMD_BASE                               0x24020000

#ifndef ROM_SIZE
#ifdef FPGA
#define ROM_SIZE                                0x00020000
#else
#define ROM_SIZE                                0x00010000
#endif
#endif

#ifndef SHARED_SRAM_SIZE
#define SHARED_SRAM_SIZE                        0
#endif

#define RAM0_BASE                               0x20000000
#define RAMX0_BASE                              0x00200000
#define RAM1_BASE                               0x20040000
#define RAMX1_BASE                              0x00240000
#define RAM2_BASE                               0x20080000
#define RAMX2_BASE                              0x00280000
#define RAM3_BASE                               0x20100000
#define RAMX3_BASE                              0x00300000
#define RAM4_BASE                               0x20180000
#define RAMX4_BASE                              0x00380000
#define RAM5_BASE                               0x201C0000
#define RAMX5_BASE                              0x003C0000
#define RAM_BASE                                (RAM0_BASE + SHARED_SRAM_SIZE)
#define RAMX_BASE                               (RAMX0_BASE + SHARED_SRAM_SIZE)

#define RAM5_SIZE                               0x00040000

/* total ramv size is 0x30000 */
#define RAMV_BASE                               0x20200000

#define CODEC_VAD_RAM_BASE                      RAMV_BASE
#define CODEC_VAD_MAX_BUF_SIZE                  0x00030000

#ifdef CODEC_VAD_CFG_BUF_SIZE
#if (CODEC_VAD_CFG_BUF_SIZE & (0x8000 - 1)) || (CODEC_VAD_CFG_BUF_SIZE > CODEC_VAD_MAX_BUF_SIZE)
#error "Bad CODEC_VAD_CFG_BUF_SIZE"
#endif
#ifndef RAMV_SIZE
#define RAMV_SIZE                           (CODEC_VAD_MAX_BUF_SIZE - CODEC_VAD_CFG_BUF_SIZE)
#endif
#define RAMV_END                            (RAMV_BASE + CODEC_VAD_MAX_BUF_SIZE - CODEC_VAD_CFG_BUF_SIZE)
#else
#ifndef RAMV_SIZE
#define RAMV_SIZE                           CODEC_VAD_MAX_BUF_SIZE
#endif
#define RAMV_END                            (RAMV_BASE + CODEC_VAD_MAX_BUF_SIZE)
#endif

#ifdef __BT_RAMRUN__
#define BT_RAMRUN_BASE                           RAM3_BASE
#define BT_RAMRUNX_BASE                          RAMX3_BASE
#endif

#ifndef CP_FLASH_SIZE
#define CP_FLASH_SIZE 0
#endif

#ifdef CHIP_HAS_CP
#if (CP_FLASH_SIZE > 0)
#define CP_FLASH_BASE                           (FLASH_BASE + FLASH_SIZE - CP_FLASH_SIZE)
#define CP_FLASHX_BASE                          (FLASHX_BASE + FLASH_SIZE - CP_FLASH_SIZE)
#endif

#ifdef LARGE_RAM
/*MCU use RAM0/1/2/3, CPX use RAM4, and CP use RAM5*/
#define RAMCP_TOP                               (RAM5_BASE + RAM5_SIZE)

#ifndef RAMCP_SIZE
#define RAMCP_SIZE                              RAM5_SIZE
#endif

#ifndef RAMCP_BASE
#define RAMCP_BASE                              (RAMCP_TOP - RAMCP_SIZE)
#endif

#ifndef RAMCPX_SIZE
#define RAMCPX_SIZE                             (RAM5_BASE - RAM4_BASE)
#endif

#ifndef RAMCPX_BASE
#define RAMCPX_BASE                             (RAM_TO_RAMX(RAMCP_BASE) - RAMCPX_SIZE)
#endif

#else /*LARGE_RAM*/
/*MCU use RAM0, CP and CPX use RAM1*/
#define RAMCP_TOP                               RAM2_BASE

#ifndef RAMCP_SIZE
#define RAMCP_SIZE                              0x20000
#endif

#ifndef RAMCP_BASE
#define RAMCP_BASE                              (RAMCP_TOP - RAMCP_SIZE)
#endif

#ifndef RAMCPX_SIZE
#define RAMCPX_SIZE                             (RAMX2_BASE - RAMX1_BASE - RAMCP_SIZE)
#endif

#ifndef RAMCPX_BASE
#define RAMCPX_BASE                             (RAM_TO_RAMX(RAMCP_BASE) - RAMCPX_SIZE)
#endif

#endif /*LARGE_RAM*/
#endif

#define RAM_TOTAL_SIZE                          (RAM5_BASE + RAM5_SIZE - RAM0_BASE) // 0x00200000

#if defined(ARM_CMSE) || defined(ARM_CMNS)
#undef RAM_BASE
#undef RAMX_BASE

/*MPC: SRAM block size: 0x8000, FLASH block size 0x40000*/
#ifndef RAM_S_SIZE
#define RAM_S_SIZE                              0x00020000
#endif
#define RAM_NSC_SIZE                            0
#ifndef FLASH_S_SIZE
#define FLASH_S_SIZE                            0x00040000
#endif

#define RAM_NS_BASE                             (RAM0_BASE + RAM_S_SIZE)
#define RAMX_NS_BASE                            (RAMX0_BASE + RAM_S_SIZE)

#if defined(ARM_CMNS)
#define RAM_BASE                                (RAM_NS_BASE + SHARED_SRAM_SIZE)
#define RAMX_BASE                               (RAMX_NS_BASE + SHARED_SRAM_SIZE)
#else
#if ((RAM_S_SIZE) & (0x8000-1))
#error "RAM_S_SIZE should be 0x8000 aligned"
#endif
#if (FLASH_S_SIZE & (0x40000-1))
#error "FLASH_S_SIZE should be 0x40000 aligned"
#endif
#define RAM_BASE                                RAM0_BASE
#define RAMX_BASE                               RAMX0_BASE
#define RAM_SIZE                                RAM_S_SIZE
#ifndef NS_APP_START_OFFSET
#define NS_APP_START_OFFSET                     (FLASH_S_SIZE)
#endif
#ifndef FLASH_REGION_SIZE
#define FLASH_REGION_SIZE                       FLASH_S_SIZE
#endif
#endif
#endif /* defined(ARM_CMSE) || defined(ARM_CMNS) */

#ifndef RAM_MCU_SIZE
/* secure and non-secure ram size */
#ifdef LARGE_RAM
#ifdef CHIP_HAS_CP
#define RAM_MCU_SIZE                            (RAMCPX_BASE - RAMX_BASE)
#else
#define RAM_MCU_SIZE                            (RAM5_BASE + RAM5_SIZE - RAM_BASE)
#endif
#else
#ifdef CHIP_HAS_CP
#define RAM_MCU_SIZE                            (RAMCPX_BASE - RAMX_BASE)
#else
#define RAM_MCU_SIZE                            (RAM2_BASE - RAM_BASE) // 0x00080000
#endif
#endif
#endif /*RAM_MCU_SIZE*/

#if defined(ARM_CMSE)
#ifndef RAM_NS_SIZE
#define RAM_NS_SIZE                             (RAM_BASE + RAM_MCU_SIZE - RAM_NS_BASE)
#define RAMX_NS_SIZE                            (RAM_NS_SIZE)
#endif /*RAM_NS_SIZE*/
#endif

#ifndef RAM_SIZE
#define RAM_SIZE                                RAM_MCU_SIZE
#endif

#define FLASH_BASE                              0x2C000000
#define FLASH_NC_BASE                           0x28000000
#define FLASHX_BASE                             0x0C000000
#define FLASHX_NC_BASE                          0x08000000
#ifndef FLASH1_BASE
#define FLASH1_BASE                             0x2E000000
#define FLASH1_NC_BASE                          0x2A000000
#define FLASH1X_BASE                            0x0E000000
#define FLASH1X_NC_BASE                         0x0A000000
#endif

#ifndef PSRAM_BASE
#define PSRAM_BASE                              0x34000000
#endif
#define PSRAM_NC_BASE                           0x30000000
#define PSRAMX_BASE                             0x14000000
#define PSRAMX_NC_BASE                          0x10000000

#define PSRAMUHS_BASE                           0x3C000000
#define PSRAMUHS_NC_BASE                        0x38000000
#define PSRAMUHSX_BASE                          0x1C000000
#define PSRAMUHSX_NC_BASE                       0x18000000

#define DCACHE_CTRL_BASE                        0x27FFA000
#define ICACHE_CTRL_BASE                        0x27FFC000

#define CMU_BASE                                0x40000000
#define MCU_WDT_BASE                            0x40001000
#define MCU_TIMER0_BASE                         0x40002000
#define MCU_TIMER1_BASE                         0x40003000
#define MCU_TIMER2_BASE                         0x40004000
#define I2C0_BASE                               0x40005000
#define I2C1_BASE                               0x40006000
#define SPI_BASE                                0x40007000
#define SPILCD_BASE                             0x40008000
#define ISPI_BASE                               0x40009000
#define SPIPHY_BASE                             0x4000A000
#define UART0_BASE                              0x4000B000
#define UART1_BASE                              0x4000C000
#define UART2_BASE                              0x4000D000
#define BTPCM_BASE                              0x4000E000
#define I2S0_BASE                               0x4000F000
#define SPDIF0_BASE                             0x40010000
#define TRANSQW_BASE                            0x40011000
#define TRANSQD_BASE                            0x40012000
#define TRNG_BASE                               0x40013000
#define IRDA_BASE                               0x40014000
#define I2C2_BASE                               0x40015000
#define UART3_BASE                              0x40016000
#define I2S1_BASE                               0x40017000
#define SEC_ENG_BASE                            0x40020000
#define TRUST_ZONE_BASE                         0X40030000
#define MPC_PSRAMUHS_BASE                       0x40040000
#define SPY_PSRAMUHS_BASE                       0x40041000
#define MPC_IMEM_LO_A7_BASE                     0x40042000
#define SPY_IMEM_LO_A7_BASE                     0x40043000
#define MPC_IMEM_HI_A7_BASE                     0x40044000
#define SPY_IMEM_HI_A7_BASE                     0x40045000
#define MPC_PSRAM0_BASE                         0x40046000
#define SPY_PSRAM0_BASE                         0x40047000
#define MPC_PSRAM1_BASE                         0x40048000
#define SPY_PSRAM1_BASE                         0x40049000
#define MPC_PSRAM_A7_BASE                       0x4004A000
#define SPY_PSRAM_A7_BASE                       0x4004B000
#define MPC_FLASH0_BASE                         0x4004C000
#define SPY_FLASH0_BASE                         0x4004D000
#define MPC_FLASH1_BASE                         0x4004E000
#define SPY_FLASH1_BASE                         0x4004F000
#define MPC_ROM0_BASE                           0x40050000
#define SPY_ROM0_BASE                           0x40051000
#define MPC_SRAM0_BASE                          0x40052000
#define SPY_SRAM0_BASE                          0x40053000
#define MPC_SRAM1_BASE                          0x40054000
#define SPY_SRAM1_BASE                          0x40055000
#define MPC_SRAM2_BASE                          0x40056000
#define SPY_SRAM2_BASE                          0x40057000
#define MPC_SRAM3_BASE                          0x40058000
#define SPY_SRAM3_BASE                          0x40059000
#define MPC_SRAM4_BASE                          0x4005A000
#define SPY_SRAM4_BASE                          0x4005B000
#define MPC_SRAM5_BASE                          0x4005C000
#define SPY_SRAM5_BASE                          0x4005D000

#define AON_CMU_BASE                            0x40080000
#define AON_GPIO_BASE                           0x40081000
#define AON_WDT_BASE                            0x40082000
#define AON_PWM0_BASE                           0x40083000
#define AON_TIMER_BASE                          0x40084000
#define AON_IOMUX_BASE                          0x40086000
#define AON_PSC_BASE                            0x40088000
#define AON_PWM1_BASE                           0x40089000

#define CHECKSUM_BASE                           0x40100000
#define CRC_BASE                                0x40108000
#define SDMMC_BASE                              0x40110000
#define BES2003_AUDMA_BASE                      0x40120000
#define BES2003_GPDMA_BASE                      0x40130000
#define FLASH_CTRL_BASE                         0x40140000
#define FLASH1_CTRL_BASE                        0x40148000
#define PSRAM_CTRL_BASE                         0x40150000
#define I2C_SLAVE_BASE                          0x40160000
#define WFDUMP_BASE                             0x40170000
#define USB_BASE                                0x40180000
#define SEDMA_BASE                              0x401D0000
#define BTDUMP_BASE                             0x401E0000
#define PSRAMUHS_CTRL_BASE                      0x401F0000

#define CODEC_BASE                              0x40380000

#define PATCH_ENTRY_NUM                         8
#define PATCH_CTRL_BASE                         0x000FFE00
#define PATCH_DATA_BASE                         0x000FFF00
#define WIFI_PATCH_CTRL_BASE                    0x60040000
#define WIFI_PATCH_DATA_BASE                    0x60040200

#define BT_SUBSYS_BASE                          0xA0000000
#define BT_RAM_BASE                             0xC0000000
#define BT_RAM_SIZE                             0x00008000
#define BT_UART_BASE                            0xD0300000
#define BT_CMU_BASE                             0xD0330000

#define WIFI_SUBSYS_BASE                        0x60000000
#define WIFI_RAM_BASE                           0x80000000
#define WIFI_PAS_BASE                           0x82000000
#define WIFI_TRANSQM_BASE                       0x9000A000
#define WIFI_CMU_BASE                           0x90000000

#define DSP_SUBSYS_BASE                         0x50000000

#define DSP_BOOT_BASE                           0x00000000
#define DSP_BOOT_SIZE                           (24 * 4)

#define DSP_RAM0_BASE                           0x50000000
#define DSP_RAM1_BASE                           0x50040000
#define DSP_RAM2_BASE                           0x50080000
#define DSP_RAM3_BASE                           0x50100000
#define DSP_RAM4_BASE                           0x50180000
#define DSP_RAM5_BASE                           0x501C0000
#define DSP_RAM_BASE                            DSP_RAM0_BASE

#define DSP_RAM5_SIZE                           0x00040000

#define MAX_DSP_RAM_SIZE                        (DSP_RAM5_BASE + DSP_RAM5_SIZE - DSP_RAM1_BASE)

#ifndef DSP_RAM_SIZE
#ifdef LARGE_DSP_RAM
#define DSP_RAM_SIZE                            MAX_DSP_RAM_SIZE
#else /*LARGE_DSP_RAM*/
#ifdef FPGA
#define DSP_RAM_SIZE                            (DSP_RAM1_BASE - DSP_RAM0_BASE)
#else /*FPGA*/
#define DSP_RAM_SIZE                            (DSP_RAM3_BASE - DSP_RAM0_BASE)
#endif /*FPGA*/
#endif /*LARGE_DSP_RAM*/
#endif

#define GPV_MAIN_BASE                           0x50300000
#define GPV_PSRAM1G_BASE                        0x50400000

#define DSP_BOOT_REG                            0x58000000
#define DSP_WDT_BASE                            0x58001000
#define DSP_TIMER0_BASE                         0x58002000
#define DSP_TIMER1_BASE                         0x58003000
#define DSP_TRANSQM_BASE                        0x58004000
#define DSP_TIMESTAMP_GEN_BASE                  0x58020000

#define DSI_BASE                                0x58005000
#define CSI_BASE                                0x58006000
#define LCDC_BASE                               0x58100000

#define DSP_DEBUGSYS_APB_BASE                   0x58040000

#define DSP_XDMA_BASE                           0x58200000

#define GIC_DISTRIBUTOR_BASE                    0x58301000
#define GIC_INTERFACE_BASE                      0x58302000

#define IOMUX_BASE                              AON_IOMUX_BASE
#define GPIO_BASE                               AON_GPIO_BASE
#define PWM_BASE                                AON_PWM0_BASE
#define PWM1_BASE                               AON_PWM1_BASE

#ifdef CHIP_BEST2003_DSP
#define TIMER0_BASE                             DSP_TIMER0_BASE
#define TIMER1_BASE                             DSP_TIMER1_BASE
#define TRANSQ0_BASE                            TRANSQW_BASE
#define TRANSQ0_PEER_BASE                       WIFI_TRANSQM_BASE
#define TRANSQ1_BASE                            DSP_TRANSQM_BASE
#define TRANSQ1_PEER_BASE                       TRANSQD_BASE
#define WDT_BASE                                DSP_WDT_BASE
#else
#define TIMER0_BASE                             MCU_TIMER0_BASE
#define TIMER1_BASE                             MCU_TIMER1_BASE
#define TIMER2_BASE                             MCU_TIMER2_BASE
#define TRANSQ0_BASE                            TRANSQW_BASE
#define TRANSQ0_PEER_BASE                       WIFI_TRANSQM_BASE
#define TRANSQ1_BASE                            TRANSQD_BASE
#define TRANSQ1_PEER_BASE                       DSP_TRANSQM_BASE
#define WDT_BASE                                AON_WDT_BASE
#endif

#ifndef DSP_USE_GPDMA
#define AUDMA_BASE                              BES2003_AUDMA_BASE
#define GPDMA_BASE                              BES2003_GPDMA_BASE
#else /*DSP_USE_GPDMA*/
#ifdef CHIP_BEST2003_DSP
#define AUDMA_BASE                              BES2003_GPDMA_BASE //A7 use GPDMA
#else
#define AUDMA_BASE                              BES2003_AUDMA_BASE //MCU use AUDMA
#endif
#endif /*DSP_USE_GPDMA*/

/* For linker scripts */
#define VECTOR_SECTION_SIZE                     360
#define REBOOT_PARAM_SECTION_SIZE               64
#define ROM_BUILD_INFO_SECTION_SIZE             40
#define ROM_EXPORT_FN_SECTION_SIZE              128

#define PSRAMX_TO_PSRAM(d)                      ((d) - PSRAMX_BASE + PSRAM_BASE)
#define PSRAM_TO_PSRAMX(d)                      ((d) - PSRAM_BASE + PSRAMX_BASE)

#define PSRAMUHSX_TO_PSRAMUHS(d)                      ((d) - PSRAMUHSX_BASE + PSRAMUHS_BASE)
#define PSRAMUHS_TO_PSRAMUHSX(d)                      ((d) - PSRAMUHS_BASE + PSRAMUHSX_BASE)

#define RAMCP_TO_RAMCPX(d)                          ((d) - (RAMCP_BASE) + (RAMCPX_BASE))
#define RAMCPX_TO_RAMCP(d)                          ((d) - (RAMCPX_BASE) + (RAMCP_BASE))

#define BT_INTESYS_MEM_OFFSET                   0x00004000

/* For module features */
#define I2S_TX_MULTI_CHAN
#define I2S_RX_MULTI_CHAN
#define SEC_ENG_HAS_HASH

/* For boot struct version */
#ifndef SECURE_BOOT_VER
#define SECURE_BOOT_VER                         4
#endif

#ifndef DSP_PSRAMUHS_SIZE
#define DSP_PSRAMUHS_SIZE                       (0)
#endif

#ifndef MCU_PSRAMUHS_SIZE
#define MCU_PSRAMUHS_SIZE                       (0)
#endif

#define DSP_PSRAMUHS_BASE           (PSRAMUHS_BASE)
#define DSP_PSRAMUHS_NC_BASE        (PSRAMUHS_NC_BASE)
#define DSP_PSRAMUHSX_BASE          (PSRAMUHSX_BASE)

#define MCU_PSRAMUHS_BASE           (PSRAMUHS_BASE + DSP_PSRAMUHS_SIZE)
#define MCU_PSRAMUHS_NC_BASE        (PSRAMUHS_NC_BASE + DSP_PSRAMUHS_SIZE)
#define MCU_PSRAMUHSX_BASE          (PSRAMUHSX_BASE + DSP_PSRAMUHS_SIZE)

#ifndef MCU_PSRAM_SIZE
#define MCU_PSRAM_SIZE              PSRAM_SIZE
#endif

#define PSRAMCP_BASE                (PSRAM_BASE + MCU_PSRAM_SIZE)
#define PSRAMCPX_BASE               (PSRAMX_BASE + MCU_PSRAM_SIZE)

#ifndef PSRAMCP_SIZE
#define PSRAMCP_SIZE                0
#endif

#define PSRAMFS_BASE                (PSRAMCP_BASE + PSRAMCP_SIZE)

#ifndef PSRAMFS_SIZE
#define PSRAMFS_SIZE                0
#endif

#define PSRAMDSP_BASE               (PSRAMFS_BASE + PSRAMFS_SIZE)

/* For ROM export functions */
#define NO_MEMMOVE

#ifdef __cplusplus
}
#endif

#endif
