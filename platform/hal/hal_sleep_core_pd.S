/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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

/*
=========================================================================
    Core Sleep Power Down and Wakeup Power Up Helper Functions
=========================================================================
*/

    #include "cmsis.h"

    .syntax unified

#ifdef CORE_SLEEP_POWER_DOWN

#ifdef ROM_BUILD
    .section .bss.core_sleep_stack
#else
    .section .sram_bss.core_sleep_stack
#endif
    .align    2
core_sleep_stack:
    .space 4

#ifdef ROM_BUILD
    .section .text.hal_sleep_core_power_down, "ax", %progbits
#else
    .section .sram_text.hal_sleep_core_power_down, "ax", %progbits
#endif
    .thumb
    .thumb_func
    .align    2
    .globl    hal_sleep_core_power_down
    .type    hal_sleep_core_power_down, %function
hal_sleep_core_power_down:

    push {r4-r12,lr}
#ifdef __ARM_ARCH_8M_MAIN__
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    mrs r0, msplim_ns
    mrs r1, psplim_ns
    mrs r2, msp_ns
    mrs r3, psp_ns
    mrs r4, primask_ns
    mrs r5, faultmask_ns
    mrs r6, control_ns
    push {r0-r6}
#endif
    mrs r0, msplim
    mrs r1, psplim
    push {r0,r1}
#endif
    mrs r0, psp
    mrs r1, primask
    mrs r2, faultmask
    mrs r3, control
    push {r0-r3}
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    ldr     r1, =0xE000EF34
    ldr     r0, [r1]                /* Load FPCCR */
    bic     r0, #1                  /* Clear LSPACT (Lazy state) */
    str     r0, [r1]                /* Store FPCCR */
    /* TODO: Save FPU registers? */
#endif
    ldr r3, =core_sleep_stack
    str sp, [r3]
    dsb
    bl hal_psc_core_auto_power_down
.pd_loop:
    b .pd_loop

    .pool
    .size    hal_sleep_core_power_down, . - hal_sleep_core_power_down


#ifdef ROM_BUILD
    .section .text.hal_sleep_core_power_up, "ax", %progbits
#else
    .section .sram_text.hal_sleep_core_power_up, "ax", %progbits
#endif
    .thumb
    .thumb_func
    .align    2
    .globl    hal_sleep_core_power_up
    .type    hal_sleep_core_power_up, %function
hal_sleep_core_power_up:

    ldr r3, =core_sleep_stack
    ldr sp, [r3]
    dsb
    isb
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    /* TODO: Restore FPU registers? */
#if 0
/* Enable CP10 and CP11 Full Access. SCB->CPACR |= (3 << 10*2) | (3 << 11*2) */
    ldr.w r0, =0xE000ED88
    ldr r1, [r0]
    orr r1, r1, #(0xF << 20)
    str r1, [r0]
    dsb
    isb
#endif
#endif
    pop {r0-r3}
    msr psp, r0
    msr primask, r1
    msr faultmask, r2
    msr control, r3
#ifdef __ARM_ARCH_8M_MAIN__
    pop {r0,r1}
    msr msplim, r0
    msr psplim, r1
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    pop {r0-r6}
    msr msplim_ns, r0
    msr psplim_ns, r1
    msr msp_ns, r2
    msr psp_ns, r3
    msr primask_ns, r4
    msr faultmask_ns, r5
    msr control_ns, r6
#endif
#endif
    pop {r4-r12,lr}
    bx lr

    .pool
    .size    hal_sleep_core_power_up, . - hal_sleep_core_power_up

#endif

    .end
