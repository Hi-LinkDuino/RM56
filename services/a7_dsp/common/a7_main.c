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
#include <string.h>
#include "plat_addr_map.h"
#include "hal_timer.h"
#ifndef NUTTX_BUILD
#ifndef NOSTD
#include "sys_time.h"
#endif
#endif
#include "hal_uart.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_iomux.h"
#include "hal_sysfreq.h"
#include "hwtimer_list.h"
#ifdef SPI_ROM_ONLY
#include "pmu_rom.h"
#else
#include "hal_analogif.h"
#include "hal_chipid.h"
#endif
#include "main_entry.h"
#include "transq_msg.h"
#ifdef RTOS
#include "cmsis_os.h"
#include "bes_kv/bes_kv.h"
#endif
#include "mcu_audio.h"
#include "a7_dump.h"
#include "hal_location.h"
#include "net_memory.h"
#include "a7_cmd.h"
#include "dsp_codec.h"

extern void a7_audio_task_init(void);
extern void a7_audio_task_main(void);
extern int mbw_test(int arraysize_KB, int blocksize_KB);
extern void OS_Tick_Enable (void);
extern int ali_record_init(void);

int a7_dsp_main(void);

// The following macros can be defined in command line by TC=...
// E.g., TC=I2S_TEST,SPI_TEST

void clock_init(void)
{
#ifdef NO_CLOCK_INIT
    hal_sys_timer_open();
#else
    hal_cmu_rom_setup();
#endif

    // Enable OSC X2/X4
#ifdef SPI_ROM_ONLY
#ifndef NO_PMU
    pmu_rom_open();
    pmu_rom_update_pll_config();
    pmu_rom_enable_osc_x2();
#endif
#else
#ifndef NO_SPI
    hal_analogif_open();
#endif
#ifndef NO_PMU
    hal_chipid_init();
#endif
#endif

#ifndef NO_CLOCK_INIT
    hal_cmu_module_init_state();

    hal_cmu_osc_x2_enable();
    hal_cmu_osc_x4_enable();

#ifdef NO_LPU_26M
    while (hal_cmu_lpu_init(HAL_CMU_LPU_CLK_NONE) == -1);
#else
    while (hal_cmu_lpu_init(HAL_CMU_LPU_CLK_26M) == -1);
#endif
    hal_cmu_rom_enable_pll();
#endif
}

void a7_uart_switch(enum HAL_TRACE_TRANSPORT_T transport)
{
    static enum HAL_TRACE_TRANSPORT_T last_transport = HAL_TRACE_TRANSPORT_QTY;

    if (last_transport != transport) {
        if(last_transport != HAL_TRACE_TRANSPORT_QTY)
            hal_trace_close();

        switch (transport) {
            case HAL_TRACE_TRANSPORT_UART0:
                hal_iomux_set_uart0();
                hal_trace_open(transport);
                break;
            case HAL_TRACE_TRANSPORT_UART1:
                hal_iomux_set_uart1();
                hal_trace_open(transport);
                break;
            case HAL_TRACE_TRANSPORT_UART2:
                hal_iomux_set_uart2();
                hal_trace_open(transport);
                break;
            case HAL_TRACE_TRANSPORT_TRANSQ1:
                hal_trace_open(transport);
                break;
            default:
                break;
        }
        last_transport = transport;
    }
}

#ifndef RTOS
int MAIN_ENTRY(void)
#else
int bes_hal_init(void)
#endif
{
    clock_init();

    hal_audma_open();
    hal_gpdma_open();

#ifdef A7_ALLOC_TRANSQ_BUF
#if (DEBUG_PORT == 1)
    a7_uart_switch(HAL_TRACE_TRANSPORT_UART0);
#elif (DEBUG_PORT == 2)
    a7_uart_switch(HAL_TRACE_TRANSPORT_UART1);
#elif (DEBUG_PORT == 3)
    a7_uart_switch(HAL_TRACE_TRANSPORT_UART2);
#endif
#endif
    hwtimer_init();

    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_780M);

#ifdef RTOS
    OS_Tick_Enable();
#endif

#ifdef A7_ALLOC_TRANSQ_BUF
#if (DEBUG_PORT != 4)
    TRACE(0, "a7 main start");
#endif
#endif

#ifndef RTOS
    a7_dsp_main();
#endif

    return 0;
}

static unsigned char *a7_dsp_flags_start = NULL;
static unsigned char *a7_dsp_flags_end = NULL;
static unsigned char *a7_dsp_flags_cur = NULL;
#define A7_DSP_HEAP_ARRAY_NUM 20
#define A7_DSP_HEAP_ARRAY_FUNC_SIZE 64
static int a7_dsp_heap_array_idx = 0;
static struct {
    char func[A7_DSP_HEAP_ARRAY_FUNC_SIZE];
    int line;
    void *addr;
    int size;
} a7_dsp_heap_array[A7_DSP_HEAP_ARRAY_NUM];

void a7_dsp_heap_init(unsigned char *buf, int len)
{
    if (buf == NULL) {
        return;
    }
#ifndef A7_ALLOC_TRANSQ_BUF
    printf("%s buf=%p, len=%d\n", __FUNCTION__, buf, len);
#endif
    a7_dsp_flags_start = buf;
    a7_dsp_flags_end = a7_dsp_flags_start+len;
    a7_dsp_flags_cur = a7_dsp_flags_start;
}
unsigned char *_a7_dsp_heap_alloc(char *func, int line, int size)
{
    ASSERT((a7_dsp_flags_start), "a7_dsp_flags_start not ready!");
    unsigned char *addr = a7_dsp_flags_cur;
    if (func == NULL) {
        return NULL;
    }
    a7_dsp_flags_cur += (size+15)/16*16;
    ASSERT((a7_dsp_flags_cur<a7_dsp_flags_end), "a7_dsp_heap_alloc overflow!");
    if (strlen(func) < A7_DSP_HEAP_ARRAY_FUNC_SIZE) {
        strcpy(a7_dsp_heap_array[a7_dsp_heap_array_idx].func, func);
    } else {
        memcpy(a7_dsp_heap_array[a7_dsp_heap_array_idx].func, func, A7_DSP_HEAP_ARRAY_FUNC_SIZE-1);
        a7_dsp_heap_array[a7_dsp_heap_array_idx].func[A7_DSP_HEAP_ARRAY_FUNC_SIZE-1] = '\0';
    }
    a7_dsp_heap_array[a7_dsp_heap_array_idx].line = line;
    a7_dsp_heap_array[a7_dsp_heap_array_idx].size = size;
    a7_dsp_heap_array[a7_dsp_heap_array_idx].addr = addr;
    a7_dsp_heap_array_idx++;
    return addr;
}

#ifdef DSP_SRAM_SOLO
void dsp_solo_heap_init(void)
{
    uint32_t __ram1_start = 0x50000000;
    uint32_t __ram1_end = __ram1_start + 0x80000;
    nts_system_heap_init(__ram1_start, __ram1_end);
    printf("%s nts_system_heap_init start=0x%X, end=0x%X\n", __FUNCTION__, __ram1_start, __ram1_end);

    void *p = NULL;
    for (int size=512; size<300*1024; size*=2) {
        p = nts_malloc(size);
        printf("%s nts_malloc size=%d, p=%p\n", __FUNCTION__, size, p);
        nts_free(p);
        p = NULL;
    }
    printf("%s test nts_malloc done!\n", __FUNCTION__);
    for (int size=512; size<300*1024; size*=2) {
        p = nts_calloc(1, size);
        printf("%s nts_calloc size=%d, p=%p\n", __FUNCTION__, size, p);
        nts_free(p);
        p = NULL;
    }
    printf("%s test nts_calloc done!\n", __FUNCTION__);
}
#endif

#ifdef MBW_TEST
#ifndef MBW_ARRAY_SIZE_KB
#define MBW_ARRAY_SIZE_KB 256
#endif
#ifndef MBW_BLOCK_SIZE_KB
#define MBW_BLOCK_SIZE_KB 16
#endif
void a7_mbw_test(void)
{
    mbw_test(MBW_ARRAY_SIZE_KB, MBW_BLOCK_SIZE_KB);
}
#endif

void WEAK a7_dsp_init_hook(void)
{

}

/* following APIs for new alg A7_SRAM */
void WEAK *nts_malloc(size_t size)
{
  return NULL;
}

void WEAK *nts_calloc(size_t count, size_t size)
{
  return NULL;
}

void WEAK nts_free(void *rmem)
{
  return;
}

void panicHandler_pre_hook(void)
{
    A7_CMD_T cmd = {A7_CMD_TYPE_PANIC, 0, 0};
    a7_cmd_send2(&cmd);
#if (DEBUG_PORT == 3)
    hal_uart_printf_init(); // use non-dma mode for cpu1
#endif
}

static int boot_num = 0;
void a7_set_bootnum(int num)
{
    boot_num = num;
}
int a7_get_bootnum(void)
{
    TRACE(0, "%s boot_num=%d", __FUNCTION__, boot_num);
    return boot_num;
}

static int _debug_mm_interval = 0;
void a7_debug_mm_interval_set(int interval)
{
    _debug_mm_interval = interval;
}

int a7_dsp_main(void)
{
#ifdef A7_ALLOC_TRANSQ_BUF
#if (DEBUG_PORT == 3)
    printf("%s\n", __FUNCTION__);
#endif
#endif
    transq_msg_onoff(1);
    /* init transq channel for mcu-a7 communication */
    transq_msg_init();
    a7_cmd_init();
    // handshake
#ifdef __A7_DSP_AUDIO_TASK__
    while (!mic_get_handshake_done()) {
        printf("%s wait handshake\n", __FUNCTION__);
        osDelay(100);
    }
#endif

#ifndef A7_ALLOC_TRANSQ_BUF
    while (a7_dsp_flags_start == NULL) {
        printf("%s wait heapinit\n", __FUNCTION__);
        osDelay(100);
    }
#if (DEBUG_PORT == 3)
    a7_uart_switch(HAL_TRACE_TRANSPORT_UART2);
#else
    a7_uart_switch(HAL_TRACE_TRANSPORT_TRANSQ1);
#endif
#else
#if (DEBUG_PORT == 4)
    a7_uart_switch(HAL_TRACE_TRANSPORT_TRANSQ1);
#endif
#endif

    TRACE(0, "%s go, freq:%dMhz", __FUNCTION__, hal_sys_timer_calc_cpu_freq(5, 0)/1000000);

#ifdef DSP_SRAM_SOLO
    dsp_solo_heap_init();
#endif

#ifdef MBW_TEST
    a7_mbw_test();
#endif

    /* task init */
#ifdef __A7_DSP_AUDIO_TASK__
    a7_audio_task_init();
#endif

#ifdef __A7_DSP_CODEC__
    a7_codec_open();
#endif

#ifdef RTOS
    extern void heartbeat_init(void);
    heartbeat_init();
    bes_kv_init();
#endif

    a7_dsp_init_hook();

#ifdef MY_TG_AUDIO_INPUT_TEST
    extern int my_tg_audio_input_test(void);
    my_tg_audio_input_test();
#endif

#ifdef MY_TG_VOIP_TEST
    /* bes_a7_voip_test*/
    extern void bes_a7_voip_test();
    bes_a7_voip_test();
#endif

#ifdef MY_A7_CPU0_CMSIS_TEST
    extern void my_a7_cpu0_cmsis_test(void);
    my_a7_cpu0_cmsis_test();
#endif /* MY_A7_CPU0_CMSIS_TEST */
#ifdef MY_A7_CPU1_CMSIS_TEST
    extern void my_a7_cpu1_cmsis_test(void);
    my_a7_cpu1_cmsis_test();
#endif /* MY_A7_CPU1_CMSIS_TEST */
#ifdef MY_A7_CPUS_TX_RX_TEST
    extern void my_a7_cpus_tx_rx_test();
    my_a7_cpus_tx_rx_test();
#endif /* MY_A7_CPUS_TX_RX_TEST */

    /* main loop for task scheduling */
#ifdef RTOS
    aud_dump_init();
#endif
#ifndef A7_ALLOC_TRANSQ_BUF
    // dump heap array
    printf("%s heap [%p - %p, cur %p, item num %d]\n", __FUNCTION__, a7_dsp_flags_start, a7_dsp_flags_end, a7_dsp_flags_cur, a7_dsp_heap_array_idx);
    for (int i=0; i<a7_dsp_heap_array_idx; i++) {
        printf("%s item%d [%s:%d, size=%d addr=%p]\n", __FUNCTION__, i, a7_dsp_heap_array[i].func, a7_dsp_heap_array[i].line, a7_dsp_heap_array[i].size, a7_dsp_heap_array[i].addr);
    }
#endif
    uint32_t start_tick = TICKS_TO_MS(hal_sys_timer_get());
    while (1) {
#ifndef A7_RECORD_DIRECT_CB
#ifdef __A7_DSP_AUDIO_TASK__
        a7_audio_task_main();
#endif
#ifdef RTOS
        osDelay(1);
#endif
#else
#ifdef RTOS
        if (aud_dump_send()) {
            osDelay(5);
        } else {
            osDelay(100);
        }
#endif
#endif
        if ((_debug_mm_interval > 0) && (TICKS_TO_MS(hal_sys_timer_get()) >= start_tick+_debug_mm_interval)) {
            a7_debug_mm_print();
            start_tick = TICKS_TO_MS(hal_sys_timer_get());
        }
    }
}

