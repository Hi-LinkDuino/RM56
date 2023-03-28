/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TARGET_CONFIG_H__
#define __TARGET_CONFIG_H__

#include "cmsis.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define IRQ_SPI_BASE                    32

#define LOSCFG_BASE_CORE_TICK_HW_TIME   1
#define OS_SYS_CLOCK           6000000
#define SYS_CTRL_REG_BASE      IO_DEVICE_ADDR(0x12020000)
#define REG_SC_CTRL            0

/* memory */
#define CACHE_ALIGNED_SIZE        64

/* physical memory base and size */
#define DDR_MEM_ADDR            0x3c000000
#define DDR_MEM_SIZE            0x00800000

/* Peripheral register address base and size */
#define PERIPH_PMM_BASE         0x00000000
#define PERIPH_PMM_SIZE         0x0F000000

#define SYS_MEM_SIZE_DEFAULT    DDR_MEM_SIZE


/* hwi */
/**
 * Maximum number of supported hardware devices that generate hardware interrupts.
 * The maximum number of hardware devices that generate hardware interrupts is 128.
 */
#define OS_HWI_MAX_NUM                  96

/**
 * Maximum interrupt number.
 */
#define OS_HWI_MAX                      ((OS_HWI_MAX_NUM) - 1)

/**
 * Minimum interrupt number.
 */
#define OS_HWI_MIN                      0
/**
 * Maximum usable interrupt number.
 */
#define OS_USER_HWI_MAX                 OS_HWI_MAX
/**
 * Minimum usable interrupt number.
 */
#define OS_USER_HWI_MIN                 OS_HWI_MIN

#define NUM_HAL_INTERRUPT_CNTPSIRQ      (IRQ_PPI_BASE + 13)
#define NUM_HAL_INTERRUPT_CNTPNSIRQ     (IRQ_PPI_BASE + 14)
#define OS_TICK_INT_NUM                 NUM_HAL_INTERRUPT_CNTPSIRQ // use secure physical timer for now
#define NUM_HAL_INTERRUPT_TIMER0        37
#define NUM_HAL_INTERRUPT_TIMER3        38
#define NUM_HAL_INTERRUPT_UART0         (IRQ_SPI_BASE + 1)

/* gic config */
#define IRQ_PPI_BASE                    16
#define GIC_BASE_ADDR             GIC_DISTRIBUTOR_BASE

#define GICD_OFFSET               0x00000     /* interrupt distributor offset */
#define GICC_OFFSET               0x1000     /* CPU interface register offset */

#if 0
/* timer config */
#define BIT(n)  (1U << (n))
#define TIMER0_ENABLE   BIT(16)
#define TIMER1_ENABLE   BIT(17)
#define TIMER2_ENABLE   BIT(18)
#define TIMER3_ENABLE   BIT(19)

#define TIMER0_REG_BASE           IO_DEVICE_ADDR(0x12000000)
#define TIMER1_REG_BASE           IO_DEVICE_ADDR(0x12000020)
#define TIMER2_REG_BASE           IO_DEVICE_ADDR(0x12001000)
#define TIMER3_REG_BASE           IO_DEVICE_ADDR(0x12001020)

#define TIMER_TICK_REG_BASE       TIMER0_REG_BASE   /* timer for tick */
#define TIMER_TICK_ENABLE         TIMER0_ENABLE
#define TIMER_TIME_REG_BASE       TIMER1_REG_BASE   /* timer for time */
#define TIMER_TIME_ENABLE         TIMER1_ENABLE
#define HRTIMER_TIMER_REG_BASE    TIMER3_REG_BASE /* timer for hrtimer */
#define HRTIMER_TIMER_ENABLE      TIMER3_ENABLE

#define NUM_HAL_INTERRUPT_TIMER         NUM_HAL_INTERRUPT_TIMER0
#define NUM_HAL_INTERRUPT_HRTIMER       NUM_HAL_INTERRUPT_TIMER3

#define TIMER_LOAD              0x0
#define TIMER_VALUE             0x4
#define TIMER_CONTROL           0x8
#define TIMER_INT_CLR           0xc
#define TIMER_RIS               0x10
#define TIMER_MIS               0x14
#define TIMER_BGLOAD            0x18

/* uart config */
#define UART0_REG_BASE            IO_DEVICE_ADDR(0x09000000)
#define TTY_DEVICE                "/dev/uartdev-0"
#define UART_REG_BASE             UART0_REG_BASE
#endif
#define NUM_HAL_INTERRUPT_UART    NUM_HAL_INTERRUPT_UART0

/**
 * @ingroup los_config
 * Configuration of memory pool record memory consumption waterline
 */
#ifndef LOSCFG_MEM_WATERLINE
#define LOSCFG_MEM_WATERLINE                                1
#endif
#ifndef __ASSEMBLER__
extern void uart_init(void);
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
