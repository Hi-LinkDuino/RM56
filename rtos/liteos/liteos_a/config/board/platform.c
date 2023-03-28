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

#include "los_typedef.h"
#include "los_hwi.h"
#include "los_task_pri.h"
#include "los_spinlock.h"
#include "los_init.h"
#include "los_smp.h"
#include "los_cppsupport.h"
#include "hal_sysfreq.h"
#include "hal_uart.h"

UINT32 g_saveSRContext;
UINT32 g_saveAR;
static struct SmpOps arch_smp_ops;
uint32_t g_secondary_allow = 0;

extern void MMU_CreateTranslationTable_new(void);
extern void k_mmu_enable_new(void);
extern void hal_sys_timer_open(void);
extern void hwtimer_init(void);
extern uint32_t __sram_text_data_start_load__[];
extern uint32_t __sram_text_data_end_load__[];
extern uint32_t __sram_text_data_start__[];
extern uint32_t __sync_flags_start[];
extern uint32_t __sync_flags_end[];
extern uint32_t __psramuhs_text_data_start_load__[];
extern uint32_t __psramuhs_text_data_end_load__[];
extern uint32_t __psramuhs_text_start[];
extern uint32_t __init_array_start[];
extern uint32_t __init_array_end[];

uint32_t int_lock(void)
{
    return LOS_IntLock();
}

void int_unlock(uint32_t cpsr)
{
    return LOS_IntRestore(cpsr);
}

LITE_OS_SEC_BSS  SPIN_LOCK_INIT(g_ipc_lock);
uint32_t plat_ipc_lock(void)
{
    uint32_t state;
    LOS_SpinLockSave(&g_ipc_lock, &state);
    return state;
}

void plat_ipc_unlock(uint32_t state)
{
    LOS_SpinUnlockRestore(&g_ipc_lock, state);
}

typedef void (*LOS_ISR)(void);
unsigned int hal_irq_create(uint32_t hwiNum,
                                           uint16_t hwiPrio,
                                           uint16_t hwiMode,
                                           LOS_ISR hwiHandler,
                                           void *irqParam)
{
    return LOS_HwiCreate(hwiNum, hwiPrio, hwiMode, (HWI_PROC_FUNC)hwiHandler, irqParam);
}

void hal_ca_mmu_setup(uint32_t cpuid)
{
    if (cpuid == 0)
        MMU_CreateTranslationTable_new();
    k_mmu_enable_new();

#if (LOSCFG_KERNEL_SMP == YES)
    /* enable smp */
    asm volatile("mrc p15, 0, r1, c1, c0, 1");
    asm volatile("mov r0, #(1<<6)");
    asm volatile("orr r1, r0");
    asm volatile("mcr p15, 0, r1, c1, c0, 1");
#endif

    L1C_EnableCaches();
}

int32_t hal_secondary_cpu_on(uint32_t cpuNum, uint32_t * startEntry)
{
    /* secondary cpu has been started simutanlously with main cpu */
    g_secondary_allow = 1;
    DCacheFlushRange((UINTPTR)&g_secondary_allow, (UINTPTR)&g_secondary_allow + sizeof(g_secondary_allow));
    asm volatile ("dsb sy":::"memory");
    asm volatile ("sev");
    return 0;
}

uint32_t hal_earliest_init(void)
{
    uint32_t *dst, *src;

    if (__sram_text_data_start__ != __sram_text_data_start_load__) {
        for (dst = __sram_text_data_start__, src = __sram_text_data_start_load__;
                src < __sram_text_data_end_load__;
                dst++, src++) {
            *dst = *src;
        }
    }

    /*psramhus_test load region covers sram_bss, and it needs to be copyed first*/
#if defined(CHIP_HAS_PSRAMUHS) && defined(PSRAMUHS_ENABLE) && defined(EXEC_IN_RAM)
    for (dst = __psramuhs_text_start, src = __psramuhs_text_data_start_load__;
            src < __psramuhs_text_data_end_load__;
            dst++, src++) {
        *dst = *src;
    }
#endif

    for (dst = __sync_flags_start; dst < __sync_flags_end; dst++) {
        *dst = 0;
    }

    hal_sys_timer_open();
    hwtimer_init();
    //hal_audma_open();
    //hal_gpdma_open();
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_780M);

    arch_smp_ops.SmpCpuOn = hal_secondary_cpu_on;
    LOS_SmpOpsSet(&arch_smp_ops);
    return 0;
}
LOS_MODULE_INIT(hal_earliest_init, LOS_INIT_LEVEL_EARLIEST);

void hal_platform_early_init(void)
{
    /* cpp init */
    LOS_CppSystemInit((UINTPTR)__init_array_start, (UINTPTR)__init_array_end, 0);

    /* enable systick timer */
    uint32_t volatile *timer1_reset = (uint32_t volatile *)0x40000048;
    *timer1_reset = 0x10000000; //SYS_ORST_TIMER1_AP
    volatile uint32_t *ts_gen = (volatile uint32_t *)DSP_TIMESTAMP_GEN_BASE;
    *ts_gen = 0x3;
}
LOS_MODULE_INIT(hal_platform_early_init, LOS_INIT_LEVEL_PLATFORM_EARLY);

void *_malloc_r(void *ptr, size_t size)
{
    void *mem;
    (void)ptr;
    mem = LOS_MemAlloc(m_aucSysMem0, size);
    return mem;
}

void *_realloc_r(void *ptr, void *old, size_t newlen)
{
    void *mem;
    (void)ptr;
    mem = LOS_MemRealloc(m_aucSysMem0, old, newlen);
    return mem;
}

void *_calloc_r(void *ptr, size_t size, size_t len)
{
    void *mem;
    (void)ptr;
    mem = LOS_MemAlloc(m_aucSysMem0, size * len);
    if (mem) {
        memset(mem, 0, size * len);
    }
    return mem;
}

void _free_r(void *ptr, void *addr)
{
    (void)ptr;
    LOS_MemFree(m_aucSysMem0, addr);
}
