#ifndef __REG_SEC_BEST2003_H__
#define __REG_SEC_BEST2003_H__

#include "plat_types.h"

struct HAL_SEC_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    __IO uint32_t REG_008;
    __IO uint32_t REG_00C;
    __IO uint32_t REG_010;
    __IO uint32_t REG_014;
    __IO uint32_t REG_018;
    __IO uint32_t REG_01C;
    __IO uint32_t REG_020;
    __IO uint32_t REG_024;
    __IO uint32_t REG_028;
    __IO uint32_t REG_02C;
    __IO uint32_t REG_030;
    __IO uint32_t REG_034;
    __IO uint32_t REG_038;
    __IO uint32_t REG_03C;
};

// reg_00
#define SEC_CFG_AP_PROT_CODEC                               (1 << 0)
#define SEC_CFG_NONSEC_PROT_CODEC                           (1 << 1)
#define SEC_CFG_SEC_RESP_PROT_CODEC                         (1 << 2)
#define SEC_CFG_NONSEC_BYPASS_PROT_CODEC                    (1 << 3)
#define SEC_CFG_AP_PROT_MCU2BT                              (1 << 4)
#define SEC_CFG_NONSEC_PROT_MCU2BT                          (1 << 5)
#define SEC_CFG_SEC_RESP_PROT_MCU2BT                        (1 << 6)
#define SEC_CFG_NONSEC_BYPASS_PROT_MCU2BT                   (1 << 7)
#define SEC_CFG_AP_PROT_MCU2WF                              (1 << 8)
#define SEC_CFG_NONSEC_PROT_MCU2WF                          (1 << 9)
#define SEC_CFG_SEC_RESP_PROT_MCU2WF                        (1 << 10)
#define SEC_CFG_NONSEC_BYPASS_PROT_MCU2WF                   (1 << 11)

// reg_08
#define SEC_IRQ_CLR_PPC_CODEC                               (1 << 0)
#define SEC_IRQ_EN_PPC_CODEC                                (1 << 1)
#define SEC_IRQ_CLR_PPC_MCU2BT                              (1 << 2)
#define SEC_IRQ_EN_PPC_MCU2BT                               (1 << 3)
#define SEC_IRQ_CLR_PPC_MCU2WF                              (1 << 4)
#define SEC_IRQ_EN_PPC_MCU2WF                               (1 << 5)

// reg_0c
#define SEC_CFG_NONSEC_A7_AUTO                              (1 << 0)
#define SEC_CFG_NONSEC_A7                                   (1 << 1)
#define SEC_CFG_NONSEC_CP_AUTO                              (1 << 2)
#define SEC_CFG_NONSEC_CP                                   (1 << 3)
#define SEC_CFG_NONSEC_ADMA1                                (1 << 4)
#define SEC_CFG_NONSEC_ADMA2                                (1 << 5)
#define SEC_CFG_NONSEC_GDMA1                                (1 << 6)
#define SEC_CFG_NONSEC_GDMA2                                (1 << 7)
#define SEC_CFG_NONSEC_BCM                                  (1 << 8)
#define SEC_CFG_NONSEC_CKSUM                                (1 << 9)
#define SEC_CFG_NONSEC_CRC                                  (1 << 10)
#define SEC_CFG_NONSEC_USB                                  (1 << 11)
#define SEC_CFG_NONSEC_I2C_SLV                              (1 << 12)
#define SEC_CFG_NONSEC_BT2MCU                               (1 << 13)
#define SEC_CFG_NONSEC_WF2MCU                               (1 << 14)
#define SEC_CFG_RD_SEC_DMA(n)                               (((n) & 0x3) << 15)
#define SEC_CFG_RD_SEC_DMA_MASK                             (0x3 << 15)
#define SEC_CFG_RD_SEC_DMA_SHIFT                            (15)
#define SEC_CFG_WR_SEC_DMA(n)                               (((n) & 0x3) << 17)
#define SEC_CFG_WR_SEC_DMA_MASK                             (0x3 << 17)
#define SEC_CFG_WR_SEC_DMA_SHIFT                            (17)

// reg_10
#define SEC_CFG_NONSEC_PROT_AHB1(n)                         (((n) & 0xFFFF) << 0)
#define SEC_CFG_NONSEC_PROT_AHB1_MASK                       (0xFFFF << 0)
#define SEC_CFG_NONSEC_PROT_AHB1_SHIFT                      (0)
#define SEC_CFG_NONSEC_BYPASS_PROT_AHB1(n)                  (((n) & 0xFFFF) << 16)
#define SEC_CFG_NONSEC_BYPASS_PROT_AHB1_MASK                (0xFFFF << 16)
#define SEC_CFG_NONSEC_BYPASS_PROT_AHB1_SHIFT               (16)

// reg_14
#define SEC_CFG_NONSEC_PROT_APB0(n)                         (((n) & 0x3FFFFFFF) << 0)
#define SEC_CFG_NONSEC_PROT_APB0_MASK                       (0x3FFFFFFF << 0)
#define SEC_CFG_NONSEC_PROT_APB0_SHIFT                      (0)

// reg_18
#define SEC_CFG_NONSEC_BYPASS_PROT_APB0(n)                  (((n) & 0x3FFFFFFF) << 0)
#define SEC_CFG_NONSEC_BYPASS_PROT_APB0_MASK                (0x3FFFFFFF << 0)
#define SEC_CFG_NONSEC_BYPASS_PROT_APB0_SHIFT               (0)

// reg_1c
#define SEC_CFG_NONSEC_PROT_AONAPB(n)                       (((n) & 0x3FFFFFFF) << 0)
#define SEC_CFG_NONSEC_PROT_AONAPB_MASK                     (0x3FFFFFFF << 0)
#define SEC_CFG_NONSEC_PROT_AONAPB_SHIFT                    (0)

// reg_20
#define SEC_CFG_NONSEC_BYPASS_PROT_AONAPB(n)                (((n) & 0x3FFFFFFF) << 0)
#define SEC_CFG_NONSEC_BYPASS_PROT_AONAPB_MASK              (0x3FFFFFFF << 0)
#define SEC_CFG_NONSEC_BYPASS_PROT_AONAPB_SHIFT             (0)

// reg_24
#define SEC_CFG_NONSEC_PROT_PERAPB(n)                       (((n) & 0x3FFFFFFF) << 0)
#define SEC_CFG_NONSEC_PROT_PERAPB_MASK                     (0x3FFFFFFF << 0)
#define SEC_CFG_NONSEC_PROT_PERAPB_SHIFT                    (0)

// reg_28
#define SEC_CFG_NONSEC_BYPASS_PROT_PERAPB(n)                (((n) & 0x3FFFFFFF) << 0)
#define SEC_CFG_NONSEC_BYPASS_PROT_PERAPB_MASK              (0x3FFFFFFF << 0)
#define SEC_CFG_NONSEC_BYPASS_PROT_PERAPB_SHIFT             (0)

// reg_2c
#define SEC_IRQ_PPC_CODEC                                   (1 << 0)
#define SEC_IRQ_PPC_MCU2BT                                  (1 << 1)
#define SEC_IRQ_PPC_MCU2WF                                  (1 << 2)
#define SEC_IRQ_MPC_SRAM5                                   (1 << 3)
#define SEC_IRQ_MPC_SRAM4                                   (1 << 4)
#define SEC_IRQ_MPC_SRAM3                                   (1 << 5)
#define SEC_IRQ_MPC_SRAM2                                   (1 << 6)
#define SEC_IRQ_MPC_SRAM1                                   (1 << 7)
#define SEC_IRQ_MPC_ROM0                                    (1 << 8)
#define SEC_IRQ_MPC_PSRAM1G                                 (1 << 9)
#define SEC_IRQ_MPC_PSRAM200                                (1 << 10)
#define SEC_IRQ_MPC_PSRAM0                                  (1 << 11)
#define SEC_IRQ_MPC_PSRAM1                                  (1 << 12)
#define SEC_IRQ_MPC_FLASH0                                  (1 << 13)
#define SEC_IRQ_MPC_FLASH1                                  (1 << 14)
#define SEC_IRQ_MPC_SRAM0                                   (1 << 15)
#define SEC_IRQ_MPC_IMEM_LO                                 (1 << 16)
#define SEC_IRQ_MPC_IMEM_HI                                 (1 << 17)

// reg_30
#define SEC_SPIDEN_CORE0                                    (1 << 0)
#define SEC_SPNIDEN_CORE0                                   (1 << 1)
#define SEC_SPIDEN_CORE1                                    (1 << 2)
#define SEC_SPNIDEN_CORE1                                   (1 << 3)
#define SEC_CFGSECEXT_CORE1                                 (1 << 4)

// reg_34
#define SEC_IDAUEN_CORE0                                    (1 << 0)
#define SEC_IDAUIDV_EN_CORE0                                (1 << 1)
#define SEC_IDAU_CALL_ADDR_CORE0(n)                         (((n) & 0x7FFFFFF) << 2)
#define SEC_IDAU_CALL_ADDR_CORE0_MASK                       (0x7FFFFFF << 2)
#define SEC_IDAU_CALL_ADDR_CORE0_SHIFT                      (2)

// reg_38
#define SEC_IDAUEN_CORE1                                    (1 << 0)
#define SEC_IDAUIDV_EN_CORE1                                (1 << 1)
#define SEC_IDAU_CALL_ADDR_CORE1(n)                         (((n) & 0x7FFFFFF) << 2)
#define SEC_IDAU_CALL_ADDR_CORE1_MASK                       (0x7FFFFFF << 2)
#define SEC_IDAU_CALL_ADDR_CORE1_SHIFT                      (2)

// reg_3c
#define SEC_IRQ_SPY_SRAM5                                   (1 << 0)
#define SEC_IRQ_SPY_SRAM4                                   (1 << 1)
#define SEC_IRQ_SPY_SRAM3                                   (1 << 2)
#define SEC_IRQ_SPY_SRAM2                                   (1 << 3)
#define SEC_IRQ_SPY_SRAM1                                   (1 << 4)
#define SEC_IRQ_SPY_ROM0                                    (1 << 5)
#define SEC_IRQ_SPY_PSRAM1G                                 (1 << 6)
#define SEC_IRQ_SPY_PSRAM200                                (1 << 7)
#define SEC_IRQ_SPY_PSRAM0                                  (1 << 8)
#define SEC_IRQ_SPY_PSRAM1                                  (1 << 9)
#define SEC_IRQ_SPY_FLASH0                                  (1 << 10)
#define SEC_IRQ_SPY_FLASH1                                  (1 << 11)
#define SEC_IRQ_SPY_SRAM0                                   (1 << 12)
#define SEC_IRQ_SPY_IMEM_LO                                 (1 << 13)
#define SEC_IRQ_SPY_IMEM_HI                                 (1 << 14)

#endif
