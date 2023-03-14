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
#ifndef __REG_WIFITSF_H__
#define __REG_WIFITSF_H__

#include "plat_types.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

struct WIFITSF_T {
    __IO uint32_t CTRL;             // 0x00
    __IO uint32_t TIME_ADJ;         // 0x04
    __IO uint32_t TIME_LOW;         // 0x08
    __IO uint32_t TIME_HIGH;        // 0x0C
    __IO uint32_t EVENT0;           // 0x10
    __IO uint32_t EVENT1;           // 0x14
    __IO uint32_t DMA_TIME_LOW;     // 0x18
    __IO uint32_t DMA_TIME_HIGH;    // 0x1C
    __IO uint32_t USB_TIME_LOW;     // 0x20
    __IO uint32_t USB_TIME_HIGH;    // 0x24
    __IO uint32_t REG_028;          // 0x28
    __IO uint32_t REG_02C;          // 0x2C
    __IO uint32_t INT_STATUS;       // 0x30
    __IO uint32_t INT_MASK;         // 0x34
    __IO uint32_t USB_CTRL;         // 0x38
    __IO uint32_t RESERVED_3C[11];  // 0x3C
    __IO uint32_t EVENT0_LOW;       // 0x68
    __IO uint32_t EVENT0_HIGH;      // 0x6C
};

// reg_00
#define TIMER_CONTROL_TSF_ENABLE                 (1 << 0)
#define DMA_TRIG_TSf_ENABLE                      (1 << 1)
#define DMA_TRIG_EDGE_SEL                        (1 << 2)
#define USB_TRIG_TSf_ENABLE                      (1 << 3)
#define WIFITSF_RESERVED_BIT4                    (1 << 4)
#define TSF_EVENT_SEL                            (1 << 5)
#define TSF_SW_STORE_EN                          (1 << 6)
#define TSF_SW_STORE_TRIG                        (1 << 7)
#define TSF_PRESCALE(n)                          (((n) & 0xFF) << 8)
#define TSF_PRESCALE_MASK                        (0xFF << 8)
#define TSF_PRESCALE_SHIFT                       (8)

// reg_04
#define TSF_TIME_ADJUST_ENABLE                   (1 << 0)
#define TSF_TIME_ADJUST_VALUE(n)                 (((n) & 0xFF) << 4)
#define TSF_TIME_ADJUST_VALUE_MASK               (0xFF << 4)
#define TSF_TIME_ADJUST_VALUE_SHIFT              (4)

// reg_10
#define TSF_EVENT0(n)                            (((n) & 0x7FFFFFFF) << 0)
#define TSF_EVENT0_MASK                          (0x7FFFFFFF << 0)
#define TSF_EVENT0_SHIFT                         (0)
#define TSF_EVENT0_ENABLE                        (1 << 31)

// reg_14
#define TSF_EVENT1(n)                            (((n) & 0x7FFFFFFF) << 0)
#define TSF_EVENT1_MASK                          (0x7FFFFFFF << 0)
#define TSF_EVENT1_SHIFT                         (0)
#define TSF_EVENT1_ENABLE                        (1 << 31)

// reg_30
#define TSF_EVENT0_STATUS                        (1 << 0)
#define TSF_EVENT1_STATUS                        (1 << 1)

// reg_34
//#define TSF_EVENT0_MASK                          (1 << 0)
//#define TSF_EVENT1_MASK                          (1 << 1)

// reg_38
#define USB_TRIG_THRESHOLD(n)                    (((n) & 0xFFFF) << 0)
#define USB_TRIG_THRESHOLD_MASK                  (0xFFFF << 0)
#define USB_TRIG_THRESHOLD_SHIFT                 (0)

#ifdef __cplusplus
}
#endif

#endif

