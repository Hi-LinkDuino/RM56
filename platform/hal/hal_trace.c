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
#if !(defined(DEBUG) || defined(REL_TRACE_ENABLE))
// Implement a local copy of dummy trace functions for library linking (which might be built with DEBUG enabled)
#define TRACE_FUNC_SPEC
#endif
#include "hal_trace.h"
#include "cmsis_nvic.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_bootmode.h"
#include "hal_cmu.h"
#include "hal_chipid.h"
#include "hal_codec.h"
#include "hal_dma.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_memsc.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "pmu.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#ifdef CORE_DUMP
#include "CrashCatcherApi.h"
#endif

#if (defined(CHIP_BEST2001) || defined(CHIP_BEST2003)) && !defined(PROGRAMMER)
#include "transq_msg.h"
#ifdef __ARM_ARCH_ISA_ARM
#include "mcu_audio.h"
#endif
#endif

#if defined(NUTTX_BUILD)
#undef CP_BUILD
#undef CP_BOOT
#endif

#if defined(CP_BOOT) || defined(CP_BUILD)
#include "cp_ipc.h"
#endif

extern const char sys_build_info[];

#ifdef FAULT_DUMP
void hal_trace_fault_dump(const uint32_t *regs, const uint32_t *extra, uint32_t extra_len);
#ifndef __ARM_ARCH_ISA_ARM
static void hal_trace_fault_handler(void);
#endif
#endif

#if (!(defined(ROM_BUILD) || defined(PROGRAMMER))) || defined(ROM_IN_FLASH) || defined(PROGRAMMER_INFLASH)
#define ASSERT_VERBOSE_DUMP
#endif

#if !(defined(ROM_BUILD) || defined(PROGRAMMER))
#define ASSERT_MUTE_CODEC
#define CRASH_DUMP_ENABLE
#if !(defined(NO_TRACE_TIME_STAMP) || defined(AUDIO_DEBUG))
#define TRACE_TIME_STAMP
#endif
#if (defined(DUMP_LOG_ENABLE) || defined(DUMP_CRASH_LOG) || defined(RESUME_MUSIC_AFTER_CRASH_REBOOT))
#define TRACE_TO_APP
#endif
#if defined(CHIP_HAS_CP) && !defined(CP_BOOT) && !defined(CP_BUILD) && !defined(CONFIG_SMP) && !defined(CONFIG_BES_DUALCORE_AMP) && !defined(I2C_PA_CHANNEL_SUPPORT)
#define CP_TRACE_ENABLE
#define CP_MEMSC_TIMEOUT_CHECK
#endif
#else
#ifdef __ARM_ARCH_ISA_ARM
#define TRACE_TIME_STAMP
#endif
#endif

#define TRACE_IDLE_OUTPUT               0

#ifndef TRACE_BAUD_RATE
#define TRACE_BAUD_RATE                 (921600)
#endif

#ifndef TRACE_BUF_SIZE
#define TRACE_BUF_SIZE                  (4 * 1024)
#endif

#ifdef AUDIO_DEBUG
// Fix baudrate and buffer size
#if TRACE_BAUD_RATE < 2000000
#undef TRACE_BAUD_RATE
#define TRACE_BAUD_RATE                 2000000
#endif
#if TRACE_BUF_SIZE < (16 * 1024)
#undef TRACE_BUF_SIZE
#define TRACE_BUF_SIZE                  (16 * 1024)
#endif
#endif


#ifndef TRACE_PRINTF_LEN
#define TRACE_PRINTF_LEN                (120)
#endif

#ifndef TRACE_DUMP_LEN
#define TRACE_DUMP_LEN                  (250)
#endif

#define CRASH_BUF_SIZE                  100
#define CRASH_BUF_ATTR                  ALIGNED(4) USED

#ifndef TRACE_STACK_DUMP_PREV_WORD
#define TRACE_STACK_DUMP_PREV_WORD      16
#endif
#ifndef TRACE_STACK_DUMP_WORD
#define TRACE_STACK_DUMP_WORD           32
#endif
#ifndef TRACE_BACKTRACE_NUM
#define TRACE_BACKTRACE_NUM             20
#endif
#ifndef TRACE_BACKTRACE_SEARCH_WORD
#define TRACE_BACKTRACE_SEARCH_WORD     1024
#endif

#define STACK_DUMP_CNT_PER_LEN          4
#define STACK_DUMP_CNT_PREV             ((TRACE_STACK_DUMP_PREV_WORD + STACK_DUMP_CNT_PER_LEN - 1) / STACK_DUMP_CNT_PER_LEN * STACK_DUMP_CNT_PER_LEN)
#define STACK_DUMP_CNT                  ((TRACE_STACK_DUMP_WORD + STACK_DUMP_CNT_PER_LEN - 1) / STACK_DUMP_CNT_PER_LEN * STACK_DUMP_CNT_PER_LEN)

#define ASSERT_STACK_ARG_WORD           8
#define ASSERT_STACK_RESERVED           (STACK_DUMP_CNT_PREV + ASSERT_STACK_ARG_WORD)

#define TRACE_FLUSH_TIMEOUT             MS_TO_TICKS(2000)

#define TRACE_NEAR_FULL_THRESH          200

#ifdef TRACE_CRLF
#define NEW_LINE_STR                    "\r\n"
#else
#define NEW_LINE_STR                    "\n"
#endif

#define HAL_TRACE_ASSERT_ID             0xBE57AAAA
#define HAL_TRACE_EXCEPTION_ID          0xBE57EEEE

#define TRACE_BUF_LOC                   SYNC_FLAGS_LOC

struct ASSERT_INFO_T {
    uint32_t ID;
    uint32_t CPU_ID;
    const char *FILE;
    const char *FUNC;
    uint32_t LINE;
    const char *FMT;
    uint32_t R[15];
#ifndef __ARM_ARCH_ISA_ARM
    uint32_t MSP;
    uint32_t PSP;
    uint32_t CONTROL;
#ifdef __ARM_ARCH_8M_MAIN__
    uint32_t MSPLIM;
    uint32_t PSPLIM;
#endif
#endif
};

struct EXCEPTION_INFO_T {
    uint32_t ID;
    uint32_t CPU_ID;
    const uint32_t *REGS;
#ifdef __ARM_ARCH_ISA_ARM
    const uint32_t *extra;
    uint32_t extra_len;
#else
    uint32_t MSP;
    uint32_t PSP;
    uint8_t PRIMASK;
    uint8_t FAULTMASK;
    uint8_t BASEPRI;
    uint8_t CONTROL;
    uint32_t ICSR;
    uint32_t AIRCR;
    uint32_t SCR;
    uint32_t CCR;
    uint32_t SHCSR;
    uint32_t CFSR;
    uint32_t HFSR;
    uint32_t AFSR;
    uint32_t DFSR;
    uint32_t MMFAR;
    uint32_t BFAR;
#ifdef __ARM_ARCH_8M_MAIN__
    uint32_t MSPLIM;
    uint32_t PSPLIM;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    uint32_t SFSR;
    uint32_t SFAR;
    uint32_t MSP_NS;
    uint32_t PSP_NS;
    uint32_t MSPLIM_NS;
    uint32_t PSPLIM_NS;
    uint8_t PRIMASK_NS;
    uint8_t FAULTMASK_NS;
    uint8_t BASEPRI_NS;
    uint8_t CONTROL_NS;
#endif
#endif
#endif
};

bool carsh_occurs[2]={0};
static CRASH_BUF_ATTR char crash_buf[CRASH_BUF_SIZE];

STATIC_ASSERT(sizeof(crash_buf) >= sizeof(((struct ASSERT_INFO_T *)0)->R), "crash_buf too small to hold assert registers");

static uint32_t g_trace_onoff = 0;

int hal_trace_set_onoff(uint32_t onoff)
{
    if (onoff < 0) {
        return 1;
    }
    g_trace_onoff = onoff;
    return 0;
}

static int get_hal_trace_onoff()
{
    return g_trace_onoff;
}

#if (defined(DEBUG) || defined(REL_TRACE_ENABLE))

#if (TRACE_BUF_SIZE > 0)
struct HAL_TRACE_BUF_T {
    unsigned char buf[TRACE_BUF_SIZE];
    unsigned short wptr;
    unsigned short rptr;
    unsigned short discards;
    bool sending;
    bool in_trace;
    bool wrapped;
    uint8_t pause_cnt;
#if (TRACE_IDLE_OUTPUT == 0)
    unsigned short sends[2];
#endif
#if defined(CRASH_DUMP_ENABLE) && defined(TRACE_TO_APP) && defined(CP_TRACE_ENABLE)
    unsigned short app_rptr;
#endif
};

STATIC_ASSERT(TRACE_BUF_SIZE < (1 << (8 * sizeof(((struct HAL_TRACE_BUF_T *)0)->wptr))), "TRACE_BUF_SIZE is too large to fit in wptr/rptr variable");

#if (TRACE_IDLE_OUTPUT == 0)
static const enum HAL_DMA_PERIPH_T uart_periph[] = {
    HAL_GPDMA_UART0_TX,
#if (CHIP_HAS_UART >= 2)
    HAL_GPDMA_UART1_TX,
#endif
#if (CHIP_HAS_UART >= 3)
    HAL_GPDMA_UART2_TX,
#endif
};

static struct HAL_DMA_CH_CFG_T dma_cfg;
//TRACE_BUF_LOC static struct HAL_DMA_DESC_T dma_desc[2];
#endif

#ifdef __ARM_ARCH_ISA_ARM
#ifdef A7_ALLOC_TRANSQ_BUF
TRACE_BUF_LOC static struct HAL_TRACE_BUF_T trace;
static struct HAL_TRACE_BUF_T *p_trace = &trace;
#else
static struct HAL_TRACE_BUF_T *p_trace = NULL;
#endif
#else
#if !defined(PROGRAMMER)
TRACE_BUF_LOC
#endif
static struct HAL_TRACE_BUF_T trace;
static struct HAL_TRACE_BUF_T *p_trace = &trace;
#endif

static const char discards_prefix[] = NEW_LINE_STR "LOST ";
static const uint32_t max_discards = 99999;
// 5 digits + "\r\n" = 7 chars
static char discards_buf[sizeof(discards_prefix) - 1 + 7];
static const unsigned char discards_digit_start = sizeof(discards_prefix) - 1;
#endif // TRACE_BUF_SIZE > 0

static const struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE,//HAL_UART_FLOW_CONTROL_RTSCTS,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_2,
    .baud = TRACE_BAUD_RATE,
#ifndef CP_BUILD
    .dma_rx = true,
#if (TRACE_IDLE_OUTPUT == 0)
    .dma_tx = true,
#else
    .dma_tx = false,
#endif
    .dma_rx_stop_on_err = false,
#endif
};

#if (TRACE_BUF_SIZE > 0) && (TRACE_IDLE_OUTPUT == 0)
#ifdef __ARM_ARCH_ISA_ARM
#ifdef A7_ALLOC_TRANSQ_BUF
TRACE_BUF_LOC static struct HAL_DMA_DESC_T dma_desc[2];
static struct HAL_DMA_DESC_T *p_dma_desc[2] = {&dma_desc[0], &dma_desc[1]};
#ifdef __A7_DSP_ENABLE__
TRACE_BUF_LOC static TRANSQ_MSG_T trace_transq_msg;
static TRANSQ_MSG_T *p_trace_transq_msg = &trace_transq_msg;
#endif
#else
static struct HAL_DMA_DESC_T *p_dma_desc[2] = {NULL, NULL};
static TRANSQ_MSG_T *p_trace_transq_msg = NULL;
#endif
#else
TRACE_BUF_LOC static struct HAL_DMA_DESC_T dma_desc[2];
static struct HAL_DMA_DESC_T *p_dma_desc[2] = {&dma_desc[0], &dma_desc[1]};
#if defined(__A7_DSP_ENABLE__)
TRACE_BUF_LOC static TRANSQ_MSG_T trace_transq_msg;
static TRANSQ_MSG_T *p_trace_transq_msg = &trace_transq_msg;
#endif
#endif

#ifdef CP_BUILD
static CP_IPC_MSG_HDR cpipc_trace_msg;
#endif
#endif
static enum HAL_TRACE_TRANSPORT_T trace_transport = HAL_TRACE_TRANSPORT_QTY;
static enum HAL_UART_ID_T trace_uart;

POSSIBLY_UNUSED
static const char newline[] = NEW_LINE_STR;

#ifdef CRASH_DUMP_ENABLE
static HAL_TRACE_CRASH_DUMP_CB_T crash_dump_cb_list[HAL_TRACE_CRASH_DUMP_MODULE_END];
static bool in_crash_dump = false;
#ifdef TRACE_TO_APP
static HAL_TRACE_APP_NOTIFY_T app_notify_cb[HAL_TRACE_APP_REG_ID_QTY];
static HAL_TRACE_APP_OUTPUT_T app_output_cb[HAL_TRACE_APP_REG_ID_QTY];
static HAL_TRACE_APP_OUTPUT_T app_crash_custom_cb = NULL;
static bool app_output_cb_valid = false;
static bool app_output_enabled =
#ifdef DUMP_LOG_ENABLE
    true;
#else
    false;
#endif

static void hal_trace_app_output_callback(const unsigned char *buf, unsigned int buf_len);
#endif // TRACE_TO_APP
#endif // CRASH_DUMP_ENABLE

#ifdef CP_TRACE_ENABLE
static uint8_t memsc_lock_cnt[2];
#ifdef CP_MEMSC_TIMEOUT_CHECK
static uint8_t memsc_timeout[2];
#endif
static HAL_TRACE_BUF_CTRL_T cp_buffer_cb = NULL;
static bool cp_buffer_cb_running;
#ifdef CRASH_DUMP_ENABLE
static HAL_TRACE_APP_NOTIFY_T cp_notify_cb = NULL;
#endif
#endif

#ifdef TRACE_GLOBAL_TAG
static HAL_TRACE_GLOBAL_TAG_CB_T gbl_tag_cb = NULL;
#endif

#ifdef AUDIO_DEBUG
static const char trace_head_buf[] = "[trace]";
#endif

static enum TR_LEVEL_T trace_max_level;

static uint32_t trace_mod_map[(TR_MODULE_QTY + 31) / 32];

#if (defined(CHIP_BEST2001) || defined(CHIP_BEST2003)) && defined(__A7_DSP_ENABLE__) && !defined(PROGRAMMER)
static bool hal_trace_is_transq_transport(enum HAL_TRACE_TRANSPORT_T transport)
{
    if (transport == HAL_TRACE_TRANSPORT_TRANSQ1) {
        return true;
    }
    return false;
}

#if (TRACE_IDLE_OUTPUT == 0)
static void hal_trace_transq_msg_send(uint32_t addr, uint32_t len)
{
    memset(p_trace_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_trace_transq_msg->type = TRANSQ_MSG_TYPE_TRACE;
    p_trace_transq_msg->pri = HAL_TRANSQ_PRI_HIGH;
    p_trace_transq_msg->msg.trace.addr = addr;
    p_trace_transq_msg->msg.trace.len = len;
    transq_msg_tx(p_trace_transq_msg);
}

static void hal_trace_transq_send(void)
{
    uint32_t sends[2];
    uint32_t int_state;

    int_state = int_lock();

    // There is a race condition if we do not check s/w flag, but only check the h/w status.
    // [e.g., hal_gpdma_chan_busy(dma_cfg.ch)]
    // When the DMA is done, but DMA IRQ handler is still pending due to interrupt lock
    // or higher priority IRQ, it will have a chance to send the same content twice.
    if (p_trace->wptr != p_trace->rptr && !p_trace->sending) {
        p_trace->sending = true;

        if (p_trace->wptr > p_trace->rptr) {
            sends[0] = p_trace->wptr - p_trace->rptr;
        } else {
            sends[0] = TRACE_BUF_SIZE - p_trace->rptr;
        }
        p_trace->sends[0] = sends[0];
        hal_trace_transq_msg_send((uint32_t)&p_trace->buf[p_trace->rptr], sends[0]);
    }

    int_unlock(int_state);
}

static void hal_trace_transq_xfer_done(void *param)
{
    p_trace->rptr += p_trace->sends[0];
    if (p_trace->rptr >= TRACE_BUF_SIZE) {
        p_trace->rptr -= TRACE_BUF_SIZE;
    }
    p_trace->sends[0] = 0;
    p_trace->sends[1] = 0;
    p_trace->sending = false;

    if (p_trace->wptr != p_trace->rptr) {
        hal_trace_transq_send();
    }
}
#endif
#endif

#ifdef CP_BUILD
static bool hal_trace_is_cpipc_transport(enum HAL_TRACE_TRANSPORT_T transport)
{
    if (transport == HAL_TRACE_TRANSPORT_CPIPC) {
        return true;
    }
    return false;
}

#if (TRACE_IDLE_OUTPUT == 0)
#include "reg_uart.h"
static void hal_trace_cp_block_send(uint32_t addr, uint32_t len)
{
    int i;
    uint8_t *ptr;
    uint32_t int_state;
    int_state = int_lock();
    struct UART_T *base = (struct UART_T *)UART0_BASE;
    ptr = (uint8_t *)addr;
    for (i = 0; i < len; i++) {
        while ((base->UARTFR & (1 << 5)) != 0);
        base->UARTDR = ptr[i];
    }
    p_trace->rptr += p_trace->sends[0];
    if (p_trace->rptr >= TRACE_BUF_SIZE) {
        p_trace->rptr -= TRACE_BUF_SIZE;
    }
    p_trace->sends[0] = 0;
    p_trace->sends[1] = 0;
    p_trace->sending = false;
    int_unlock(int_state);
}
static void hal_trace_cpipc_msg_send(uint32_t addr, uint32_t len)
{
    cpipc_trace_msg.id = CP_IPC_MSG_TRACE;
    cpipc_trace_msg.len = len;
    cpipc_trace_msg.data = (void *)addr;
    cp_ipc_send(&cpipc_trace_msg);
}
static void hal_trace_cpipc_send(void)
{
    uint32_t sends[2];
    uint32_t int_state;
    int_state = int_lock();

    // There is a race condition if we do not check s/w flag, but only check the h/w status.
    // [e.g., hal_gpdma_chan_busy(dma_cfg.ch)]
    // When the DMA is done, but DMA IRQ handler is still pending due to interrupt lock
    // or higher priority IRQ, it will have a chance to send the same content twice.
    if (p_trace->wptr != p_trace->rptr && !p_trace->sending) {
        p_trace->sending = true;

        if (p_trace->wptr > p_trace->rptr) {
            sends[0] = p_trace->wptr - p_trace->rptr;
        } else {
            sends[0] = TRACE_BUF_SIZE - p_trace->rptr;
        }
        p_trace->sends[0] = sends[0];
        if (carsh_occurs[1])
            hal_trace_cp_block_send((uint32_t)&p_trace->buf[p_trace->rptr], sends[0]);
        else
            hal_trace_cpipc_msg_send((uint32_t)&p_trace->buf[p_trace->rptr], sends[0]);
    }

    int_unlock(int_state);
}

static void hal_trace_cpipc_xfer_done(void *param)
{
    p_trace->rptr += p_trace->sends[0];
    if (p_trace->rptr >= TRACE_BUF_SIZE) {
        p_trace->rptr -= TRACE_BUF_SIZE;
    }
    p_trace->sends[0] = 0;
    p_trace->sends[1] = 0;
    p_trace->sending = false;

    if (p_trace->wptr != p_trace->rptr)
        hal_trace_cpipc_send();
}
#endif
#endif

#ifdef CP_BOOT
#define CP_TRACE_TAG "CP:"
static HAL_TRACE_OUTPUT_HOOK_T _hal_trace_printf_cp_hook = NULL;
void hal_trace_register_cp_hook(HAL_TRACE_OUTPUT_HOOK_T hook)
{
    _hal_trace_printf_cp_hook = hook;
}

void hal_trace_unregister_cp_hook(HAL_TRACE_OUTPUT_HOOK_T hook)
{
    if (_hal_trace_printf_cp_hook == hook) {
        _hal_trace_printf_cp_hook = NULL;
    }
}

int hal_trace_printf_cp_hook(const char *tag, const char *fmt, uint32_t len)
{
    int ret = 0;
    if (_hal_trace_printf_cp_hook) {
        ret = _hal_trace_printf_cp_hook(tag, fmt, len);
    }
    return ret;
}

void hal_trace_cpipc_recv(void *param)
{
    CP_IPC_MSG_HDR *msg = param;

    if (msg && _hal_trace_printf_cp_hook &&
        _hal_trace_printf_cp_hook(NULL, (const char *)msg->data, msg->len))
        return;

    if (msg && (get_hal_trace_onoff() == 0) ) {
        unsigned char tag_cp[32];
        unsigned int timestamp = (unsigned)TICKS_TO_MS(hal_sys_timer_get());
        char len = sprintf((char *)tag_cp, "%s[%u]", CP_TRACE_TAG, timestamp);
        REL_LOG_RAW_OUTPUT(tag_cp, len);
        REL_LOG_RAW_OUTPUT(msg->data, msg->len);
    }
}
#endif

static bool hal_trace_is_uart_transport(enum HAL_TRACE_TRANSPORT_T transport)
{
    if (transport == HAL_TRACE_TRANSPORT_UART0
#if (CHIP_HAS_UART >= 2)
            || transport == HAL_TRACE_TRANSPORT_UART1
#endif
#if (CHIP_HAS_UART >= 3)
            || transport == HAL_TRACE_TRANSPORT_UART2
#endif
            ) {
        return true;
    }
    return false;
}

#if (TRACE_BUF_SIZE > 0)
#if (TRACE_IDLE_OUTPUT)

static void hal_trace_uart_idle_send(void)
{
    int i;
    uint32_t lock;
    unsigned short wptr, rptr;

    lock = int_lock();
    wptr = p_trace->wptr;
    rptr = p_trace->rptr;
    int_unlock(lock);

    if (wptr == rptr) {
        return;
    }

    if (wptr < rptr) {
        for (i = rptr; i < TRACE_BUF_SIZE; i++) {
            hal_uart_blocked_putc(trace_uart, p_trace->buf[i]);
        }
        rptr = 0;
    }

    for (i = rptr; i < wptr; i++) {
        hal_uart_blocked_putc(trace_uart, p_trace->buf[i]);
    }

    p_trace->rptr = wptr;
    if (p_trace->rptr >= TRACE_BUF_SIZE) {
        p_trace->rptr -= TRACE_BUF_SIZE;
    }
}

#else // TRACE_IDLE_OUTPUT

static void hal_trace_uart_send(void)
{
    uint32_t wptr, rptr;
    uint32_t sends[2];
    uint32_t lock;

    lock = int_lock();

    wptr = p_trace->wptr;
    rptr = p_trace->rptr;

    // There is a race condition if we do not check s/w flag, but only check the h/w status.
    // [e.g., hal_dma_chan_busy(dma_cfg.ch)]
    // When the DMA is done, but DMA IRQ handler is still pending due to interrupt lock
    // or higher priority IRQ, it will have a chance to send the same content twice.
    if (!p_trace->sending && wptr != rptr) {
        p_trace->sending = true;

        sends[1] = 0;
        if (wptr > rptr) {
            sends[0] = wptr - rptr;
        } else {
            sends[0] = TRACE_BUF_SIZE - rptr;
            if (sends[0] <= HAL_DMA_MAX_DESC_XFER_SIZE) {
                sends[1] = wptr;
            }
        }
        if (sends[0] > HAL_DMA_MAX_DESC_XFER_SIZE) {
            sends[1] = sends[0] - HAL_DMA_MAX_DESC_XFER_SIZE;
            sends[0] = HAL_DMA_MAX_DESC_XFER_SIZE;
        }
        if (sends[1] > HAL_DMA_MAX_DESC_XFER_SIZE) {
            sends[1] = HAL_DMA_MAX_DESC_XFER_SIZE;
        }

        dma_cfg.src = (uint32_t)&p_trace->buf[rptr];
        if (sends[1] == 0) {
            dma_cfg.src_tsize = sends[0];
            hal_dma_init_desc(p_dma_desc[0], &dma_cfg, NULL, 1);
        } else {
            dma_cfg.src_tsize = sends[0];
            hal_dma_init_desc(p_dma_desc[0], &dma_cfg, p_dma_desc[1], 0);

            if (rptr + sends[0] < TRACE_BUF_SIZE) {
                dma_cfg.src = (uint32_t)&p_trace->buf[rptr + sends[0]];
            } else {
                dma_cfg.src = (uint32_t)&p_trace->buf[0];
            }
            dma_cfg.src_tsize = sends[1];
            hal_dma_init_desc(p_dma_desc[1], &dma_cfg, NULL, 1);
        }
        p_trace->sends[0] = sends[0];
        p_trace->sends[1] = sends[1];

        hal_dma_sg_start(p_dma_desc[0], &dma_cfg);
    }

    int_unlock(lock);
}

static void hal_trace_uart_xfer_done(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    uint32_t sends[2];
    uint32_t lock;

    lock = int_lock();

    sends[0] = p_trace->sends[0];
    sends[1] = p_trace->sends[1];

    if (error) {
        if (lli || sends[1] == 0) {
            if (sends[0] > remain_tsize) {
                sends[0] -= remain_tsize;
            } else {
                sends[0] = 0;
            }
            sends[1] = 0;
        } else {
            if (sends[1] > remain_tsize) {
                sends[1] -= remain_tsize;
            } else {
                sends[1] = 0;
            }
        }
    }

    p_trace->rptr += sends[0] + sends[1];
    if (p_trace->rptr >= TRACE_BUF_SIZE) {
        p_trace->rptr -= TRACE_BUF_SIZE;
    }
    p_trace->sends[0] = 0;
    p_trace->sends[1] = 0;
    p_trace->sending = false;

    if (p_trace->pause_cnt == 0) {
        hal_trace_uart_send();
    }

    int_unlock(lock);
}

static void hal_trace_uart_stop_dma_send(void)
{
    uint32_t lock;
    uint32_t remain;

    lock = int_lock();

    if (hal_dma_chan_busy(dma_cfg.ch)) {
        hal_dma_cancel(dma_cfg.ch);

        remain = hal_dma_get_sg_remain_size(dma_cfg.ch);

        p_trace->rptr += remain;
        if (p_trace->rptr >= TRACE_BUF_SIZE) {
            p_trace->rptr -= TRACE_BUF_SIZE;
        }
        p_trace->sends[0] = 0;
        p_trace->sends[1] = 0;
        p_trace->sending = false;
    }

    int_unlock(lock);
}

#endif // TRACE_IDLE_OUTPUT

void hal_trace_send(void)
{
#ifdef CP_TRACE_ENABLE
    if (get_cpu_id()) {
        return;
    }
#endif

    if (p_trace->pause_cnt) {
        return;
    }

#if (defined(CHIP_BEST2001_DSP) || defined(CHIP_BEST2003_DSP)) && defined(__A7_DSP_ENABLE__) && !defined(PROGRAMMER)
    if (hal_trace_is_transq_transport(trace_transport)) {
        hal_trace_transq_send();
    }
#endif

#ifdef CP_BUILD
    if (hal_trace_is_cpipc_transport(trace_transport)) {
        hal_trace_cpipc_send();
    }
#endif

    if (hal_trace_is_uart_transport(trace_transport)) {
#if (TRACE_IDLE_OUTPUT)
        hal_trace_uart_idle_send();
#else
        hal_trace_uart_send();
#endif
    }
}

#endif // TRACE_BUF_SIZE > 0

static void hal_trace_output_linefeed(void)
{
    hal_trace_output((const unsigned char *)newline, sizeof(newline) - 1);
}

int hal_trace_open(enum HAL_TRACE_TRANSPORT_T transport)
{
#if (CHIP_HAS_UART >= 2)
#ifdef FORCE_TRACE_UART1
    transport = HAL_TRACE_TRANSPORT_UART1;
#endif
#endif

#if (CHIP_HAS_UART >= 3)
#ifdef FORCE_TRACE_UART2
    transport = HAL_TRACE_TRANSPORT_UART2;
#endif
#endif

    if (transport >= HAL_TRACE_TRANSPORT_QTY) {
        return 1;
    }
#ifdef CHIP_HAS_USB
    if (transport == HAL_TRACE_TRANSPORT_USB) {
        return 1;
    }
#endif
#if !defined(CONFIG_CONSOLE_USE_CHAR)
    if (trace_transport != HAL_TRACE_TRANSPORT_QTY) {
        return hal_trace_switch(transport);
    }

#if (TRACE_BUF_SIZE > 0)
    memcpy(discards_buf, discards_prefix, discards_digit_start);

    p_trace->wptr = 0;
    p_trace->rptr = 0;
    p_trace->discards = 0;
    p_trace->sending = false;
    p_trace->in_trace = false;
    p_trace->wrapped = false;
    p_trace->pause_cnt = 0;

#ifdef __ARM_ARCH_ISA_ARM
#ifdef __A7_DSP_ENABLE__
#if !defined(NUTTX_BUILD)
    if (p_dma_desc[0] == NULL) {
        p_dma_desc[0] = (struct HAL_DMA_DESC_T *)a7_dsp_heap_alloc(sizeof(struct HAL_DMA_DESC_T));
        p_dma_desc[1] = (struct HAL_DMA_DESC_T *)a7_dsp_heap_alloc(sizeof(struct HAL_DMA_DESC_T));
    }
    if (p_trace_transq_msg == NULL) {
        p_trace_transq_msg = (TRANSQ_MSG_T *)a7_dsp_heap_alloc(sizeof(TRANSQ_MSG_T));
    }
#endif
#endif
#endif

    if (hal_trace_is_uart_transport(transport)) {
        int ret;

        trace_uart = HAL_UART_ID_0 + (transport - HAL_TRACE_TRANSPORT_UART0);
        ret = hal_uart_open(trace_uart, &uart_cfg);
        if (ret) {
            return ret;
        }

#if (TRACE_IDLE_OUTPUT == 0)
        if (uart_cfg.dma_tx) {
            p_trace->sends[0] = 0;
            p_trace->sends[1] = 0;
            memset(&dma_cfg, 0, sizeof(dma_cfg));
            dma_cfg.dst = 0; // useless
            dma_cfg.dst_bsize = HAL_DMA_BSIZE_8;
            dma_cfg.dst_periph = uart_periph[trace_uart - HAL_UART_ID_0];
            dma_cfg.dst_width = HAL_DMA_WIDTH_BYTE;
            dma_cfg.handler = hal_trace_uart_xfer_done;
            dma_cfg.src_bsize = HAL_DMA_BSIZE_32;
            dma_cfg.src_periph = 0; // useless
            dma_cfg.src_width = HAL_DMA_WIDTH_BYTE;
            dma_cfg.type = HAL_DMA_FLOW_M2P_DMA;
            dma_cfg.try_burst = 0;
            dma_cfg.ch = hal_dma_get_chan(dma_cfg.dst_periph, HAL_DMA_HIGH_PRIO);

            ASSERT(dma_cfg.ch != HAL_DMA_CHAN_NONE, "Failed to get DMA channel");
        }
#endif
    }
#endif // TRACE_BUF_SIZE > 0
#endif

#if (defined(CHIP_BEST2001) || defined(CHIP_BEST2003)) && defined(__A7_DSP_ENABLE__) && !defined(PROGRAMMER) && !defined(NUTTX_BUILD)
    if (hal_trace_is_transq_transport(transport)) {
        transq_msg_init();
        transq_msg_register(TRANSQ_MSG_TYPE_TRACE, hal_trace_transq_xfer_done, true);
    }
#endif

#ifdef CP_BUILD
    if (hal_trace_is_cpipc_transport(transport)) {
        cp_ipc_cb_set(CP_IPC_MSG_TRACE, hal_trace_cpipc_xfer_done, true);
    }
#endif

#ifdef CP_BOOT
    cp_ipc_cb_set(CP_IPC_MSG_TRACE, hal_trace_cpipc_recv, 0);
#endif

#ifdef FAULT_DUMP
#ifdef __ARM_ARCH_ISA_ARM
    GIC_SetFaultDumpHandler(hal_trace_fault_dump);
#elif defined(CP_BUILD)
    NVIC_SetDefaultFaultHandler_cp(hal_trace_fault_handler);
#else
    NVIC_SetDefaultFaultHandler(hal_trace_fault_handler);
#endif
#endif

#ifdef CRASH_DUMP_ENABLE
    in_crash_dump = false;
#endif

    trace_max_level = TR_LEVEL_INFO;
    for (int i = 0; i < ARRAY_SIZE(trace_mod_map); i++) {
        trace_mod_map[i] = ~0;
    }
#if !defined(CONFIG_CONSOLE_USE_CHAR)
    trace_transport = transport;
#else
    trace_transport = HAL_TRACE_TRANSPORT_QTY;
#endif

    // Show build info
    hal_trace_output_linefeed();
    hal_trace_output_linefeed();
    hal_trace_output((unsigned char *)sys_build_info, strlen(sys_build_info));

#if !(defined(CHIP_SUBSYS_DSP) || \
        defined(CHIP_SUBSYS_SENS) || \
        (defined(CHIP_SUBSYS_BTH) && defined(BTH_AS_MAIN_MCU)))
    char buf[50];
    int len;
    len = snprintf(buf, sizeof(buf),
        NEW_LINE_STR NEW_LINE_STR "------" NEW_LINE_STR "METAL_ID: %d" NEW_LINE_STR "------" NEW_LINE_STR NEW_LINE_STR,
        hal_get_chip_metal_id());
    hal_trace_output((unsigned char *)buf, len);
#endif

    return 0;
}

int hal_trace_switch(enum HAL_TRACE_TRANSPORT_T transport)
{
    int ret = 0;

#if (CHIP_HAS_UART >= 2)
#ifdef FORCE_TRACE_UART1
    transport = HAL_TRACE_TRANSPORT_UART1;
#endif
#endif

#if (CHIP_HAS_UART >= 3)
#ifdef FORCE_TRACE_UART2
    transport = HAL_TRACE_TRANSPORT_UART2;
#endif
#endif

#ifdef CHIP_HAS_USB
    if (transport == HAL_TRACE_TRANSPORT_USB) {
        return 1;
    }
#endif
    if (transport >= HAL_TRACE_TRANSPORT_QTY) {
        return 1;
    }
    if (trace_transport >= HAL_TRACE_TRANSPORT_QTY) {
        return 1;
    }
    if (trace_transport == transport) {
        return 0;
    }

#if (CHIP_HAS_UART >= 2)
    uint32_t lock;

    lock = int_lock();

#if (TRACE_BUF_SIZE > 0)
    if (hal_trace_is_uart_transport(trace_transport)) {
#if (TRACE_IDLE_OUTPUT == 0)
        if (dma_cfg.ch != HAL_DMA_CHAN_NONE) {
            hal_dma_cancel(dma_cfg.ch);
        }
#endif
        hal_uart_close(trace_uart);
    }

    if (hal_trace_is_uart_transport(transport)) {
        trace_uart = HAL_UART_ID_0 + (transport - HAL_TRACE_TRANSPORT_UART0);
#if (TRACE_IDLE_OUTPUT == 0)
        if (dma_cfg.ch != HAL_DMA_CHAN_NONE)
            hal_dma_free_chan(dma_cfg.ch);
        if (uart_cfg.dma_tx) {
            dma_cfg.dst_periph = uart_periph[trace_uart - HAL_UART_ID_0];
            dma_cfg.ch = hal_dma_get_chan(dma_cfg.dst_periph, HAL_DMA_HIGH_PRIO);
        }
        p_trace->sends[0] = 0;
        p_trace->sends[1] = 0;
#endif
        ret = hal_uart_open(trace_uart, &uart_cfg);
        if (ret) {
#if (TRACE_IDLE_OUTPUT == 0)
            hal_dma_free_chan(dma_cfg.ch);
            dma_cfg.ch = HAL_DMA_CHAN_NONE;
#endif
            trace_transport = HAL_TRACE_TRANSPORT_QTY;
            goto _exit;
        }
    }

    p_trace->sending = false;
#endif // TRACE_BUF_SIZE > 0

    trace_transport = transport;

_exit: POSSIBLY_UNUSED;
    int_unlock(lock);
#endif // (CHIP_HAS_UART >= 2)

    return ret;
}

int hal_trace_close(void)
{
#if (TRACE_BUF_SIZE > 0)
    if (trace_transport >= HAL_TRACE_TRANSPORT_QTY) {
        goto _exit;
    }
#ifdef CHIP_HAS_USB
    if (trace_transport == HAL_TRACE_TRANSPORT_USB) {
        goto _exit;
    }
#endif

    if (hal_trace_is_uart_transport(trace_transport)) {
#if (TRACE_IDLE_OUTPUT == 0)
        if (dma_cfg.ch != HAL_DMA_CHAN_NONE) {
            hal_dma_cancel(dma_cfg.ch);
            hal_dma_free_chan(dma_cfg.ch);
            dma_cfg.ch = HAL_DMA_CHAN_NONE;
        }
#endif
        hal_uart_close(trace_uart);
    }

_exit:
    trace_transport = HAL_TRACE_TRANSPORT_QTY;
#endif // TRACE_BUF_SIZE > 0

    return 0;
}

enum HAL_TRACE_TRANSPORT_T hal_trace_get_transport(void)
{
#if (TRACE_BUF_SIZE > 0)
    return trace_transport;
#else
    return HAL_TRACE_TRANSPORT_QTY;
#endif
}

int hal_trace_enable_log_module(enum TR_MODULE_T module)
{
    if (module >= TR_MODULE_QTY) {
        return 1;
    }

    trace_mod_map[module >> 5] |= (1 << (module & 0x1F));
    return 0;
}

int hal_trace_disable_log_module(enum TR_MODULE_T module)
{
    if (module >= TR_MODULE_QTY) {
        return 1;
    }

    trace_mod_map[module >> 5] &= ~(1 << (module & 0x1F));
    return 0;
}

int hal_trace_set_log_module(const uint32_t *map, uint32_t word_cnt)
{
    if (map == NULL || word_cnt == 0) {
        return 1;
    }

    if (word_cnt > ARRAY_SIZE(trace_mod_map)) {
        word_cnt = ARRAY_SIZE(trace_mod_map);
    }
    for (int i = 0; i < word_cnt; i++) {
        trace_mod_map[i] = map[i];
    }
    return 0;
}

int hal_trace_set_log_level(enum TR_LEVEL_T level)
{
    if (level >= TR_LEVEL_QTY) {
        return 1;
    }

    trace_max_level = level;
    return 0;
}

void hal_trace_get_history_buffer(const uint8_t **buf1, uint32_t *len1, const uint8_t **buf2, uint32_t *len2)
{
    uint8_t *b1, *b2;
    uint32_t l1, l2;

    b1 = b2 = NULL;
    l1 = l2 = 0;

#if (TRACE_BUF_SIZE > 0)
    uint32_t lock;

    lock = int_lock();

    if (TRACE_BUF_SIZE > p_trace->wptr) {
        if (p_trace->wrapped) {
            b1 = &p_trace->buf[p_trace->wptr];
            l1 = TRACE_BUF_SIZE - p_trace->wptr;
            b2 = &p_trace->buf[0];
            l2 = p_trace->wptr;
        } else {
            b1 = &p_trace->buf[0];
            l1 = p_trace->wptr;
            b2 = NULL;
            l2 = 0;
        }
    }

    int_unlock(lock);
#endif

    if (buf1) {
        *buf1 = b1;
    }
    if (len1) {
        *len1 = l1;
    }
    if (buf2) {
        *buf2 = b2;
    }
    if (len2) {
        *len2 = l2;
    }
}

static uint32_t hal_trace_print_unsigned(char *buf, uint32_t len, uint32_t val)
{
    const uint8_t base = 10;
    char digit[10];
    char *d;
    uint32_t cnt = 0;

    if (len == 0) {
        return 0;
    }

    d = &digit[0];
    do {
        *d++ = (val % base) + '0';
    } while (val /= base);

    do {
        *buf++ = *--d;
        if (++cnt >= len) {
            break;
        }
    } while (d > &digit[0]);

    return cnt;
}

#if (TRACE_BUF_SIZE > 0)
static void hal_trace_print_discards(uint32_t discards)
{
    char *out;
    uint16_t len;
    uint16_t size;

    if (discards > max_discards) {
        discards = max_discards;
    }

    len = hal_trace_print_unsigned(&discards_buf[discards_digit_start], 5, discards);
    out = &discards_buf[discards_digit_start + len];
#ifdef TRACE_CRLF
    *out++ = '\r';
#endif
    *out++ = '\n';
    len = out - &discards_buf[0];

    size = TRACE_BUF_SIZE - p_trace->wptr;
    if (size >= len) {
        size = len;
    }
    memcpy(&p_trace->buf[p_trace->wptr], &discards_buf[0], size);
    if (size < len) {
        memcpy(&p_trace->buf[0], &discards_buf[size], len - size);
    }
    p_trace->wptr += len;
    if (p_trace->wptr >= TRACE_BUF_SIZE) {
        p_trace->wptr -= TRACE_BUF_SIZE;
    }
}
#endif

#ifdef AUDIO_DEBUG
static void hal_trace_print_head(void)
{
    uint16_t len;
    uint16_t size;

    len = sizeof(trace_head_buf) - 1;

    size = TRACE_BUF_SIZE - p_trace->wptr;
    if (size >= len) {
        size = len;
    }
    memcpy(&p_trace->buf[p_trace->wptr], &trace_head_buf[0], size);
    if (size < len) {
        memcpy(&p_trace->buf[0], &trace_head_buf[size], len - size);
    }
    p_trace->wptr += len;
    if (p_trace->wptr >= TRACE_BUF_SIZE) {
        p_trace->wptr -= TRACE_BUF_SIZE;
    }
}
#endif

#ifdef CP_TRACE_ENABLE
static void hal_trace_cp_lock(uint8_t cpu_id)
{
    // Avoid CPU hangup when the function is re-entered due to hal_trace_app_output_callback()
#ifdef CP_MEMSC_TIMEOUT_CHECK
    uint32_t start_time;
    const uint32_t timeout = MS_TO_TICKS(500);

    if (memsc_lock_cnt[cpu_id] == 0 && memsc_timeout[cpu_id] == 0) {
        start_time = hal_sys_timer_get();
        while (hal_memsc_lock(HAL_MEMSC_ID_TRACE) == 0) {
            if (hal_sys_timer_get() - start_time >= timeout) {
                memsc_timeout[cpu_id] = 1;
                break;
            }
        }
    }
#else
    if (memsc_lock_cnt[cpu_id] == 0) {
        while (hal_memsc_lock(HAL_MEMSC_ID_TRACE) == 0);
    }
#endif
    memsc_lock_cnt[cpu_id]++;
}

static void hal_trace_cp_unlock(uint8_t cpu_id)
{
    memsc_lock_cnt[cpu_id]--;
    if (memsc_lock_cnt[cpu_id] == 0) {
        hal_memsc_unlock(HAL_MEMSC_ID_TRACE);
    }
}

static void hal_trace_cp_force_unlock(void)
{
    uint8_t cpu_id = get_cpu_id() ? 1 : 0;

    if (memsc_lock_cnt[cpu_id]) {
        memsc_lock_cnt[cpu_id] = 0;
        hal_memsc_unlock(HAL_MEMSC_ID_TRACE);
    }
}
#endif

static HAL_TRACE_OUTPUT_HOOK_T _hal_trace_output_hook = NULL;
void hal_trace_register_hook(HAL_TRACE_OUTPUT_HOOK_T hook)
{
    _hal_trace_output_hook = hook;
}

void hal_trace_unregister_hook(HAL_TRACE_OUTPUT_HOOK_T hook)
{
    if (_hal_trace_output_hook == hook) {
        _hal_trace_output_hook = NULL;
    }
}

int hal_trace_output_hook(const char *tag, const char *fmt, uint32_t len)
{
    int ret = 0;
    if (_hal_trace_output_hook) {
        ret = _hal_trace_output_hook(tag, fmt, len);
    }
    return ret;
}

bool crash_occurs[2];
int hal_trace_output(const unsigned char *buf, unsigned int buf_len)
{
    int ret;
    uint32_t lock;
    POSSIBLY_UNUSED uint32_t app_wptr = 0;
#ifdef CP_TRACE_ENABLE
    uint8_t cpu_id = get_cpu_id() ? 1 : 0;
#endif
#if defined(NUTTX_BUILD)
#ifdef CP_TRACE_ENABLE
    if (cpu_id == 0 && !hal_trace_is_uart_transport(trace_transport))
#else
    if (!hal_trace_is_uart_transport(trace_transport))
#endif
    {
#ifdef __ARM_ARCH_ISA_ARM
#if defined(CONFIG_BES_LPUART)
        hal_uart_output(buf,buf_len);
#else
        syslog(LOG_ERR,"%s\n",buf);
#endif
#else
        hal_uart_output(buf,buf_len);
#endif
        return buf_len;
    }
#endif

#ifdef CP_BUILD
    if (hal_trace_is_uart_transport(trace_transport)) {
        hal_uart_output(buf,buf_len);
        return buf_len;
    } else if (crash_occurs[1]) {
        /* exec trace redirect to uart0 */
        for (int i = 0; i < buf_len; i++) {
            hal_uart_blocked_putc(HAL_UART_ID_0, buf[i]);
        }
        return buf_len;
    }
#endif

    /* output trace by hook */
    if (hal_trace_output_hook(NULL, (const char *)buf, buf_len)) {
        return buf_len;
    }

    ret = 0;

    lock = int_lock();

#ifdef CP_TRACE_ENABLE
    hal_trace_cp_lock(cpu_id);
#endif

#if (TRACE_BUF_SIZE > 0)
    // Avoid troubles when NMI occurs during trace
    if (!p_trace->in_trace) {
        uint32_t avail;
        uint32_t out_len;
        uint16_t size;

        p_trace->in_trace = true;

        if (p_trace->wptr >= p_trace->rptr) {
            avail = TRACE_BUF_SIZE - (p_trace->wptr - p_trace->rptr) - 1;
        } else {
            avail = (p_trace->rptr - p_trace->wptr) - 1;
        }

        out_len = buf_len;
#ifdef AUDIO_DEBUG
        out_len += sizeof(trace_head_buf) - 1;
#endif
        if (p_trace->discards) {
            out_len += sizeof(discards_buf);
        }

        if (avail < out_len) {
            ret = 1;
            if (p_trace->discards < (1 << (sizeof(p_trace->discards) * 8)) - 1) {
                p_trace->discards++;
            }
#ifdef CP_TRACE_ENABLE
#if (TRACE_IDLE_OUTPUT == 0)
            // CP might have filled up the whole buffer
            hal_trace_send();
#endif
#endif
        } else {
#ifdef AUDIO_DEBUG
            hal_trace_print_head();
#endif

            if (p_trace->discards) {
                hal_trace_print_discards(p_trace->discards);
                p_trace->discards = 0;
            }

            size = TRACE_BUF_SIZE - p_trace->wptr;
            if (size >= buf_len) {
                size = buf_len;
            }
            memcpy(&p_trace->buf[p_trace->wptr], &buf[0], size);
            if (size < buf_len) {
                memcpy(&p_trace->buf[0], &buf[size], buf_len - size);
            }
            p_trace->wptr += buf_len;
            if (p_trace->wptr >= TRACE_BUF_SIZE) {
                p_trace->wptr -= TRACE_BUF_SIZE;
                p_trace->wrapped = true;
            }
#if (TRACE_IDLE_OUTPUT == 0)
            hal_trace_send();
#endif
        }

        p_trace->in_trace = false;

#ifdef CP_TRACE_ENABLE
        if (cpu_id) {
            if (cp_buffer_cb && !cp_buffer_cb_running) {
                cp_buffer_cb_running = true;
                if (avail < out_len) {
                    cp_buffer_cb(HAL_TRACE_BUF_STATE_FULL);
                } else if (avail - out_len < TRACE_NEAR_FULL_THRESH) {
                    cp_buffer_cb(HAL_TRACE_BUF_STATE_NEAR_FULL);
                }
                cp_buffer_cb_running = false;
            }
        } else {
            app_wptr = p_trace->wptr;
        }
#endif
    }
#endif // TRACE_BUF_SIZE > 0

#ifdef CP_TRACE_ENABLE
    hal_trace_cp_unlock(cpu_id);
#endif

#ifdef CRASH_DUMP_ENABLE
#ifdef TRACE_TO_APP
    bool app_output;

    app_output = app_output_cb_valid && app_output_enabled;
#ifdef CP_TRACE_ENABLE
    if (cpu_id) {
        app_output = false;
    }
#endif

    if (app_output) {
        app_output_enabled = false;
#if defined(CP_TRACE_ENABLE) && (TRACE_BUF_SIZE > 0)
        if (app_wptr < p_trace->app_rptr) {
            hal_trace_app_output_callback(&p_trace->buf[p_trace->app_rptr], TRACE_BUF_SIZE - p_trace->app_rptr);
            p_trace->app_rptr = 0;
        }
        if (app_wptr > p_trace->app_rptr) {
            hal_trace_app_output_callback(&p_trace->buf[p_trace->app_rptr], app_wptr - p_trace->app_rptr);
            p_trace->app_rptr = app_wptr;
        }
#else
        hal_trace_app_output_callback(buf, buf_len);
#endif
        app_output_enabled = true;
    }
#endif
#endif

#if defined(CP_TRACE_ENABLE) && defined(CP_MEMSC_TIMEOUT_CHECK)
    if (memsc_timeout[cpu_id] == 1) {
        memsc_timeout[cpu_id] = 2;
        ASSERT(false, "TRACE-%u: Wait memsc timeout", cpu_id);
    }
#endif

    int_unlock(lock);

    return ret ? 0 : buf_len;
}

int hal_raw_trace_output(const unsigned char *buf, unsigned int buf_len)
{
    if (get_hal_trace_onoff() == 1)
    {
        return 0;
    }
    return hal_trace_output(buf, buf_len);
}

#ifdef USE_TRACE_ID
//#define USE_CRC_CHECK
//#define LITE_VERSION

#define TRACE_ID_MAX_ARG_NUM                15

typedef struct {
    uint32_t crc:6;
    uint32_t count:4;
    uint32_t tskid:5;
    uint32_t rfu:17;    //!< reserved for future use
    uint32_t addr;      //!< enough for trace string address
}trace_info_t;

typedef struct {
    uint32_t crc:8;
    uint32_t timestamp:24; // 4 hours support
}trace_head_t;

typedef struct {
#ifndef LITE_VERSION
    trace_head_t trace_head;
#endif
    trace_info_t trace_info;
}__attribute__((packed)) LOG_DATA_T;

struct PACKED LOG_BODY_T {
    LOG_DATA_T hdr;
    uint32_t arg[TRACE_ID_MAX_ARG_NUM];
};

extern uint32_t __trc_str_start__[];
extern uint32_t __trc_str_end__[];

uint8_t crc8(uint8_t *data, uint32_t length)
{
    uint8_t i;
    uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

uint8_t crc6(uint8_t *data, uint32_t length)
{
    uint8_t i;
    uint8_t crc = 0;         // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x30;// 0x30 = (reverse 0x03)>>(8-6)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}


static int hal_trace_format_id(uint32_t attr, struct LOG_BODY_T *log, const char *fmt, va_list ap)
{
    uint8_t num;

    num = GET_BITFIELD(attr, TR_ATTR_ARG_NUM);
    if (num > TRACE_ID_MAX_ARG_NUM) {
        num = TRACE_ID_MAX_ARG_NUM;
    }
    for (int i = 0; i < num; i++) {
        log->arg[i] = va_arg(ap, unsigned long);
    }

    log->hdr.trace_info.count = num;
    log->hdr.trace_info.addr = (uint32_t)fmt-(uint32_t)0xFFFC0000; //(uint32_t)fmt-(uint32_t)__trc_str_start__;
    log->hdr.trace_info.tskid =
#ifdef RTOS
        osGetThreadIntId();
#else
        0;
#endif
    log->hdr.trace_info.crc = 0x2A;
#ifndef LITE_VERSION
    log->hdr.trace_head.timestamp = TICKS_TO_MS(hal_sys_timer_get());
#ifdef USE_CRC_CHECK
    log->hdr.trace_head.crc = crc8(((uint8_t *)&log->hdr) + 1, 7);
#else
    log->hdr.trace_head.crc = 0xBE;
#endif
#else
    log->hdr.trace_info.crc = crc6(((uint8_t *)&log->hdr) + 1, 3);
#endif

    return sizeof(log->hdr) + sizeof(log->arg[0]) * num;
}
#endif // USE_TRACE_ID

static int hal_trace_print_time(enum TR_LEVEL_T level, enum TR_MODULE_T module, char *buf, unsigned int size)
{
#ifdef TRACE_TIME_STAMP
#ifdef CONFIG_SMP
#define PRINT_CPU_ID
#else
#define PRINT_MODE_LEVEL
#endif
#ifdef PRINT_MODE_LEVEL
    static const char level_ch[] = { 'C', 'E', 'W', 'N', 'I', 'D', 'V', '-', };
    const char *mod_name;
    int i;
#endif
    char ctx[10];
    int len;

#ifdef CRASH_DUMP_ENABLE
    if (in_crash_dump) {
        return 0;
    }
#endif

    len = 0;
    len += snprintf(&buf[len], size - len, "%9u/", (unsigned)__SLIM_TICKS_TO_MS(hal_sys_timer_get()));
#ifdef TRACE_GLOBAL_TAG
    if (gbl_tag_cb && size > len) {
#if defined(CP_TRACE_ENABLE) && !defined(CONFIG_SMP)
        if (get_cpu_id() == 0)
#endif
        {
            len += gbl_tag_cb(&buf[len], size - len);
        }
    }
#endif
#if defined(ARM_CMNS) || defined(ARM_CMSE)
    if (size > len) {
#if defined(ARM_CMNS)
        len += snprintf(&buf[len], size - len, "NS/");
#elif defined(ARM_CMSE)
        len += snprintf(&buf[len], size - len, "SE/");
#endif
    }
#endif
#ifdef PRINT_MODE_LEVEL
    if (size > len + 2) {
        buf[len++] = level_ch[level];
        buf[len++] = '/';
    }
    if (size > len + 7) {
        mod_name = hal_trace_get_log_module_desc(module);
        if (mod_name) {
            for (i = 0; i < 6; i++) {
                if (mod_name[i] == '\0') {
                    break;
                }
                buf[len++] = mod_name[i];
            }
        } else {
            buf[len++] = 'M';
            i = hal_trace_print_unsigned(&buf[len], 5, module);
            len += i;
            i++;
        }
        for (; i < 6; i++) {
            buf[len++] = ' ';
        }
        buf[len++] = '/';
    }
#endif
#ifdef PRINT_CPU_ID
    if (size > len) {
        len += snprintf(&buf[len], size - len, "cpu%d/", get_cpu_id());
    }
#endif
    if (size > len) {
        if (0) {
#if defined(CP_TRACE_ENABLE) && !defined(CONFIG_SMP)
        } else if (get_cpu_id()) {
            ctx[0] = ' ';
            ctx[1] = 'C';
            ctx[2] = 'P';
            ctx[3] = '\0';
#endif
        } else if (in_isr()) {
            uint32_t ctx_len;

            ctx_len = snprintf(ctx, sizeof(ctx), "%2d", (int8_t)NVIC_GetCurrentActiveIRQ());
            if (ctx_len + 1 < ARRAY_SIZE(ctx)) {
                ctx[ctx_len] = 'E';
                ctx[ctx_len + 1] = '\0';
            } else {
                ctx[ARRAY_SIZE(ctx) - 2] = '.';
                ctx[ARRAY_SIZE(ctx) - 1] = '\0';
            }
        } else {
#ifdef RTOS
#if defined(KERNEL_RHINO) || defined(KERNEL_RTX5)
            const char *thread_name = osGetThreadName();
            snprintf(ctx, sizeof(ctx), "%.9s", thread_name ? (char *)thread_name : "NULL");
#else
            snprintf(ctx, sizeof(ctx), "%3d", osGetThreadIntId());
#endif

#else
            ctx[0] = ' ';
            ctx[1] = ' ';
            ctx[2] = '0';
            ctx[3] = '\0';
#endif
        }
        ctx[ARRAY_SIZE(ctx) - 1] = '\0';
        len += snprintf(&buf[len], size - len, "%s | ", ctx);
    }
    return len;
#else // !TRACE_TIME_STAMP
    return 0;
#endif // !TRACE_TIME_STAMP
}

static inline int hal_trace_format_va(uint32_t attr, char *buf, unsigned int size, const char *fmt, va_list ap)
{
    int len;

    len = vsnprintf(&buf[0], size, fmt, ap);
    if ((attr & TR_ATTR_NO_LF) == 0) {
#ifdef TRACE_CRLF
        if (len + 2 < size) {
            buf[len++] = '\r';
        }
#endif
        if (len + 1 < size) {
            buf[len++] = '\n';
        }
    }
    //if (len < size) buf[len] = 0;

    return len;
}

static HAL_TRACE_PRINTF_HOOK_T _hal_trace_printf_hook = NULL;
void hal_trace_printf_register_hook(HAL_TRACE_PRINTF_HOOK_T hook)
{
    _hal_trace_printf_hook = hook;
}

void hal_trace_printf_unregister_hook(HAL_TRACE_PRINTF_HOOK_T hook)
{
    if (_hal_trace_printf_hook == hook) {
        _hal_trace_printf_hook = NULL;
    }
}

int hal_trace_printf_hook(const char *tag, const char *fmt, va_list ap)
{
    int ret = 0;
    if (_hal_trace_printf_hook) {
        ret = _hal_trace_printf_hook(tag, fmt, ap);
    }
    return ret;
}

int hal_trace_printf_internal(uint32_t attr, const char *fmt, va_list ap)
{
#ifdef USE_TRACE_ID
    struct PACKED LOG_CONTAINER_T {
        char prefix[4];
        struct LOG_BODY_T body;
    };
    union LOG_BUF_T {
        char buf[60];
        struct LOG_CONTAINER_T container;
        uint32_t align;
    };

    union LOG_BUF_T log_buf;
    char *buf = (char *)&log_buf;
#else
    char buf[TRACE_PRINTF_LEN];
#endif
    if (get_hal_trace_onoff()){
        return 0;
    }

#ifndef __ARM_ARCH_ISA_ARM
    if (!in_isr()) {
        if (hal_trace_printf_hook(NULL, fmt, ap)) {
            return 1;
        }
    }
#endif

    int len = 0;
    enum TR_LEVEL_T level;
    enum TR_MODULE_T module;

    level = GET_BITFIELD(attr, TR_ATTR_LEVEL);
    module = GET_BITFIELD(attr, TR_ATTR_MOD);
#if defined(NUTTX_BUILD) && !defined(CONFIG_ARCH_CHIP_DEBUG_H)
    bool is_nolf = attr & TR_ATTR_NO_LF;
#ifndef __ARM_ARCH_ISA_ARM
    if (get_cpu_id())
        return;
#endif
    //if (!is_nolf)
    {
        len = snprintf(buf, TRACE_PRINTF_LEN,"%s\n", fmt);
        fmt = buf;
    }
    vsyslog(level+1, fmt , ap);
    return len;
#else

#ifdef CRASH_DUMP_ENABLE
    if (!in_crash_dump)
#endif
    {
        if (level > trace_max_level) {
            return 0;
        }
        if (level > TR_LEVEL_CRITICAL && (trace_mod_map[module >> 5] & (1 << (module & 0x1F))) == 0) {
            return 0;
        }
    }

#ifdef USE_TRACE_ID
    if ((attr & TR_ATTR_NO_ID) == 0 && (len = hal_trace_format_id(attr, &log_buf.container.body, fmt, ap)) > 0) {
        buf = &log_buf.container.prefix[3];
        buf[0] = '\0';
        len += 1;
    }
    else
#endif
    {
        len = 0;
        if ((attr & TR_ATTR_NO_TS) == 0) {
            len += hal_trace_print_time(level, module, &buf[len], sizeof(buf) - len);
        }
        len += hal_trace_format_va(attr, &buf[len], sizeof(buf) - len, fmt, ap);
    }

    return hal_trace_output((unsigned char *)buf, len);
#endif
}

int hal_trace_printf(uint32_t attr, const char *fmt, ...)
{
    int ret;
    va_list ap;

    if (attr & TR_ATTR_IMM) {
        hal_trace_flush_buffer();
    }

    va_start(ap, fmt);
    ret = hal_trace_printf_internal(attr, fmt, ap);
    va_end(ap);

    if (attr & TR_ATTR_IMM) {
        hal_trace_flush_buffer();
    }

    return ret;
}

int hal_trace_printf_without_crlf_ts(const char *fmt, ...)
{
    int ret;
    uint32_t attr = TR_ATTR_NO_LF|TR_ATTR_NO_TS|TR_ATTR_NO_ID;
    va_list ap;

    va_start(ap, fmt);
    ret = hal_trace_printf_internal(attr, fmt, ap);
    va_end(ap);

    return ret;
}

int hal_trace_printf_without_crlf(const char *fmt, ...)
{
    int ret;
    uint32_t attr = TR_ATTR_NO_LF|TR_ATTR_NO_ID;
    va_list ap;

    va_start(ap, fmt);
    ret = hal_trace_printf_internal(attr, fmt, ap);
    va_end(ap);

    return ret;
}

int hal_trace_dump(const char *fmt, unsigned int size,  unsigned int count, const void *buffer)
{
    char buf[TRACE_DUMP_LEN];
    int len=0, n=0, i=0;

    switch( size )
    {
        case sizeof(uint32_t):
            while(i<count && len<sizeof(buf))
            {
                len += snprintf(&buf[len], sizeof(buf) - len, fmt, *(uint32_t *)((uint32_t *)buffer+i));
                i++;
            }
            break;
        case sizeof(uint16_t):
                while(i<count && len<sizeof(buf))
                {
                    len += snprintf(&buf[len], sizeof(buf) - len, fmt, *(uint16_t *)((uint16_t *)buffer+i));
                    i++;
                }
                break;
        case sizeof(uint8_t):
        default:
            while(i<count && len<sizeof(buf))
            {
                len += snprintf(&buf[len], sizeof(buf) - len, fmt, *(uint8_t *)((uint8_t *)buffer+i));
                i++;
            }
            break;
    }

#ifdef TRACE_CRLF
    if (len + 2 < sizeof(buf)) {
        buf[len++] = '\r';
    }
#endif
    if (len + 1 < sizeof(buf)) {
        buf[len++] = '\n';
    }

    if (get_hal_trace_onoff() == 1){
        return len;
    }

    n = hal_trace_output((unsigned char *)buf, len);

    return n;
}

int hal_trace_busy(void)
{
#if (TRACE_BUF_SIZE > 0)
    union HAL_UART_FLAG_T flag;

    if (hal_trace_is_uart_transport(trace_transport)) {
        if (hal_uart_opened(trace_uart)) {
            flag = hal_uart_get_flag(trace_uart);
            return flag.BUSY;
        }
    }
#endif
    return 0;
}

int hal_trace_pause(void)
{
#if (TRACE_BUF_SIZE > 0)
    int ret = 0;
    uint32_t lock;

    lock = int_lock();
    if (p_trace->pause_cnt == 0) {
        if (hal_trace_is_uart_transport(trace_transport)) {
#if (TRACE_IDLE_OUTPUT == 0)
            hal_trace_uart_stop_dma_send();
#endif
            hal_uart_pause(trace_uart, HAL_UART_XFER_TYPE_TX);
        }
    }
    p_trace->pause_cnt++;
    if (p_trace->pause_cnt == 0) {
        p_trace->pause_cnt--;
        ret = 1;
    }
    int_unlock(lock);

    return ret;
#else
    return 0;
#endif
}

int hal_trace_continue(void)
{
#if (TRACE_BUF_SIZE > 0)
    int ret = 0;
    uint32_t lock;

    lock = int_lock();
    if (p_trace->pause_cnt == 0) {
        ret = 1;
    } else {
        p_trace->pause_cnt--;
        if (p_trace->pause_cnt == 0) {
            if (hal_trace_is_uart_transport(trace_transport)) {
                hal_uart_continue(trace_uart, HAL_UART_XFER_TYPE_TX);
            }
#if (TRACE_IDLE_OUTPUT == 0)
            hal_trace_send();
#endif
        }
    }
    int_unlock(lock);

    return ret;
#else
    return 0;
#endif
}

static void hal_trace_force_continue(void)
{
#if (TRACE_BUF_SIZE > 0)
    if (p_trace->pause_cnt) {
        // Allow to flush buffer
        p_trace->pause_cnt = 0;
        if (hal_trace_is_uart_transport(trace_transport)) {
            hal_uart_continue(trace_uart, HAL_UART_XFER_TYPE_TX);
        }
#if (TRACE_IDLE_OUTPUT == 0)
        hal_trace_send();
#endif
    }
#endif
}

int hal_trace_flush_buffer(void)
{
#if (TRACE_BUF_SIZE > 0)
    int ret = 0;
    uint32_t lock;
    uint32_t time;
#if (TRACE_IDLE_OUTPUT == 0)
    enum HAL_DMA_RET_T dma_ret;
#endif

    if (trace_transport >= HAL_TRACE_TRANSPORT_QTY)  {
        return -1;
    }

#ifdef CP_TRACE_ENABLE
    if (get_cpu_id()) {
        if (cp_buffer_cb) {
            cp_buffer_cb(HAL_TRACE_BUF_STATE_FLUSH);
        }
        return 0;
    }
#endif

    lock = int_lock();

    if (p_trace->pause_cnt == 0) {
        time = hal_sys_timer_get();
        while (p_trace->wptr != p_trace->rptr &&
                (hal_sys_timer_get() - time) < TRACE_FLUSH_TIMEOUT) {
#if (TRACE_IDLE_OUTPUT == 0)
#if (defined(CHIP_BEST2001_DSP) || defined(CHIP_BEST2003_DSP)) && !defined(PROGRAMMER) && defined(RTOS)
            if (trace_transport == HAL_TRACE_TRANSPORT_TRANSQ1) {
                if (p_trace->sending) {
                    while (hal_transq_tx_busy(HAL_TRANSQ_ID_1));
                    hal_transq_local_irq_handler_body(HAL_TRANSQ_ID_1);
                }
                hal_trace_send();
            } else
#endif
            {
                while (hal_dma_chan_busy(dma_cfg.ch));
                dma_ret = hal_dma_irq_run_chan(dma_cfg.ch);
                if (dma_ret != HAL_DMA_OK) {
                    hal_trace_send();
                }
            }
#else
            hal_trace_send();
#endif
        }
        ret = (p_trace->wptr == p_trace->rptr) ? 0 : 1;
    }

    int_unlock(lock);

    return ret;
#else
    return 0;
#endif
}

int hal_trace_flush_output(void)
{
#if (TRACE_BUF_SIZE > 0)
    uint32_t lock;
    uint32_t time;
    int ret;
    int busy;

    lock = int_lock();

    ret = hal_trace_flush_buffer();

    time = hal_sys_timer_get();
    while ((busy = hal_trace_busy()) && (hal_sys_timer_get() - time) < TRACE_FLUSH_TIMEOUT);

    int_unlock(lock);

    return (ret || busy);
#else
    return 0;
#endif
}

uint32_t hal_trace_get_backtrace_addr(uint32_t addr)
{
    if (!hal_trace_address_executable(addr)) {
        return 0;
    }

#ifndef __ARM_ARCH_ISA_ARM
#if defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_8M_MAIN__)
    // BL Instruction
    // OFFSET: 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    // VALUE :  1  1  1  1  0  -  -  -  -  -  -  -  -  -  -  -  1  1  -  1  -  -  -  -  -  -  -  -  -  -  -  -

    // BLX Instruction
    // OFFSET: 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    // VALUE :  0  1  0  0  0  1  1  1  1  -  -  -  -  -  -  -

    uint16_t val;
    uint32_t new_addr;

    new_addr = (addr & ~1) - 2;

    val = *(uint16_t *)new_addr;
    if ((val & 0xFF80) == 0x4780) {
        // BLX
        return new_addr;
    } else if ((val & 0xD000) == 0xD000) {
        new_addr -= 2;
        val = *(uint16_t *)new_addr;
        if ((val & 0xF800) == 0xF000) {
            // BL
            return new_addr;
        }
    }
#else
#if defined(NUTTX_BUILD)
    extern uint32_t _mmu_text_start[];
    extern uint32_t _mmu_text_end[];
    if (addr >= _mmu_text_start && addr <= _mmu_text_end)
        return addr;
#else
#error "Only ARMv7-M/ARMv8-M function can be checked for BL/BLX instructions"
#endif
#endif
#endif

    return 0;
}

#ifndef __ARM_ARCH_ISA_ARM
void hal_trace_print_special_stack_registers(uint32_t msp, uint32_t psp)
{
    int len;

    hal_trace_output_linefeed();

    len = snprintf(crash_buf, sizeof(crash_buf), "MSP   =%08X, PSP   =%08X" NEW_LINE_STR, (unsigned)msp, (unsigned)psp);
    hal_trace_output((unsigned char *)crash_buf, len);

#ifdef __ARM_ARCH_8M_MAIN__
    len = snprintf(crash_buf, sizeof(crash_buf), "MSPLIM=%08X, PSPLIM=%08X" NEW_LINE_STR, (unsigned)__get_MSPLIM(), (unsigned)__get_PSPLIM());
    hal_trace_output((unsigned char *)crash_buf, len);
#endif
}

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
void hal_trace_print_special_stack_registers_ns(uint32_t msp_ns, uint32_t psp_ns)
{
    int len;

    hal_trace_output_linefeed();

    len = snprintf(crash_buf, sizeof(crash_buf), "MSP_NS   =%08X, PSP_NS   =%08X" NEW_LINE_STR, (unsigned)msp_ns, (unsigned)psp_ns);
    hal_trace_output((unsigned char *)crash_buf, len);

    len = snprintf(crash_buf, sizeof(crash_buf), "MSPLIM_NS=%08X, PSPLIM_NS=%08X" NEW_LINE_STR, (unsigned)__TZ_get_MSPLIM_NS(), (unsigned)__TZ_get_PSPLIM_NS());
    hal_trace_output((unsigned char *)crash_buf, len);
}
#endif
#endif

void hal_trace_print_common_registers(const uint32_t *regs)
{
    int len;
    int i;
    int index;

    hal_trace_output_linefeed();

    for (i = 0; i < 3; i++) {
        index = i * 4;
        len = snprintf(crash_buf, sizeof(crash_buf), "R%-2d=%08X, R%-2d=%08X, R%-2d=%08X, R%-2d=%08X" NEW_LINE_STR,
            index, (unsigned)regs[index], index + 1, (unsigned)regs[index + 1],
            index + 2, (unsigned)regs[index + 2], index + 3, (unsigned)regs[index + 3]);
        hal_trace_output((unsigned char *)crash_buf, len);
    }
    len = snprintf(crash_buf, sizeof(crash_buf), "R12=%08X, SP =%08X, LR =%08X" NEW_LINE_STR,
        (unsigned)regs[12], (unsigned)regs[13], (unsigned)regs[14]);
    hal_trace_output((unsigned char *)crash_buf, len);
}

void hal_trace_print_stack(uint32_t addr)
{
    static const char stack_title[] = "Stack:" NEW_LINE_STR;
    int len = 0;
    int i;
    int pos;
    uint32_t *stack;

    addr &= ~3;
    if (!hal_trace_address_writable(addr)) {
        return;
    }

#ifndef DUMP_SECURE_STACK
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    // don't dump secure stack
    if (RAM_BASE < addr && addr < RAM_BASE + RAM_SIZE) {
        return;
    }
#endif
#endif

    hal_trace_output_linefeed();
    hal_trace_output((const unsigned char *)stack_title, sizeof(stack_title) - 1);

    stack = (uint32_t *)addr - STACK_DUMP_CNT_PREV;
    for (i = 0; i < (STACK_DUMP_CNT_PREV + STACK_DUMP_CNT); i++) {
        if (!hal_trace_address_writable((uint32_t)&stack[i])) {
            break;
        }
        pos = (i % STACK_DUMP_CNT_PER_LEN);
        if (pos == 0) {
            len = snprintf(crash_buf, sizeof(crash_buf), "%c %08X: %08X",
                (i == STACK_DUMP_CNT_PREV) ? '*' : ' ', (unsigned)&stack[i], (unsigned)stack[i]);
        } else {
            len += snprintf(crash_buf + len, sizeof(crash_buf) - len, " %08X", (unsigned)stack[i]);
            if (pos == (STACK_DUMP_CNT_PER_LEN - 1)) {
#ifdef TRACE_CRLF
                if (len + 2 < sizeof(crash_buf)) {
                    crash_buf[len++] = '\r';
                }
#endif
                if (len + 1 < sizeof(crash_buf)) {
                    crash_buf[len++] = '\n';
                }
                hal_trace_output((unsigned char *)crash_buf, len);
                hal_trace_flush_buffer();
            }
        }
    }
}

void hal_trace_print_backtrace(uint32_t addr, uint32_t search_cnt, uint32_t print_cnt)
{
    static const char bt_title[] = "Possible Backtrace:" NEW_LINE_STR;
    int i, j;
    int len;
    uint32_t *stack;
    uint32_t call_addr;

    addr &= ~3;
    if (!hal_trace_address_writable(addr)) {
        return;
    }

    hal_trace_output_linefeed();
    hal_trace_output((const unsigned char *)bt_title, sizeof(bt_title) - 1);

    stack = (uint32_t *)addr;
    for (i = 0, j = 0; i < search_cnt && j < print_cnt; i++) {
        if (!hal_trace_address_writable((uint32_t)&stack[i])) {
            break;
        }
        call_addr = hal_trace_get_backtrace_addr(stack[i]);
        if (call_addr) {
 #if defined(NUTTX_BUILD)
            len = snprintf(crash_buf, sizeof(crash_buf), "%8X ", (unsigned)call_addr);
 #else
            len = snprintf(crash_buf, sizeof(crash_buf), "%8X" NEW_LINE_STR, (unsigned)call_addr);
 #endif
            hal_trace_output((unsigned char *)crash_buf, len);
            j++;
        }
    }
}

uint32_t hal_trace_get_id(void)
{
    return trace_uart;
}

uint32_t hal_trace_get_baudrate(void)
{
#if (TRACE_BUF_SIZE > 0)
    return uart_cfg.baud;
#else
    return 0;
#endif
}

bool hal_trace_in_crash_dump(void)
{
#ifdef CRASH_DUMP_ENABLE
    return in_crash_dump;
#else
    return false;
#endif
}

int hal_trace_crash_dump_register(enum HAL_TRACE_CRASH_DUMP_MODULE_T module, HAL_TRACE_CRASH_DUMP_CB_T cb)
{
#ifdef CRASH_DUMP_ENABLE
    ASSERT(module < HAL_TRACE_CRASH_DUMP_MODULE_END, "%s module %d", __func__, module);
    crash_dump_cb_list[module] = cb;
#endif
    return 0;
}

void hal_trace_crash_dump_callback(void)
{
#ifdef CRASH_DUMP_ENABLE
    int i;

    in_crash_dump = true;

    for (i = 0; i < ARRAY_SIZE(crash_dump_cb_list); i++) {
        if (crash_dump_cb_list[i]) {
            crash_dump_cb_list[i]();
        }
    }
#endif
}

#ifdef CRASH_DUMP_ENABLE
#ifdef TRACE_TO_APP
void hal_trace_app_notify_callback(enum HAL_TRACE_STATE_T state)
{
    if (state == HAL_TRACE_STATE_CRASH_ASSERT_START) {
        app_output_enabled = true;
    } else if (state == HAL_TRACE_STATE_CRASH_FAULT_START) {
        if (app_crash_custom_cb == NULL) {
            app_output_enabled = true;
        }
    }
    for (int i = 0; i < HAL_TRACE_APP_REG_ID_QTY; i++) {
        if (app_notify_cb[i]) {
            app_notify_cb[i](state);
        }
    }
}

static void hal_trace_app_output_callback(const unsigned char *buf, unsigned int buf_len)
{
    for (int i = 0; i < HAL_TRACE_APP_REG_ID_QTY; i++) {
        if (app_output_cb[i]) {
            app_output_cb[i](buf, buf_len);
        }
    }
}
#endif
#endif

int hal_trace_app_register(enum HAL_TRACE_APP_REG_ID_T id, HAL_TRACE_APP_NOTIFY_T notify_cb, HAL_TRACE_APP_OUTPUT_T output_cb)
{
    if (id >= HAL_TRACE_APP_REG_ID_QTY) {
        return 1;
    }
#ifdef TRACE_TO_APP
    bool output_valid = false;
    uint32_t lock;

    lock = int_lock();

    app_notify_cb[id] = notify_cb;
    app_output_cb[id] = output_cb;

    for (int i = 0; i < HAL_TRACE_APP_REG_ID_QTY; i++) {
        if (app_output_cb[i]) {
            output_valid = true;
            break;
        }
    }
    app_output_cb_valid = output_valid;

    int_unlock(lock);
#endif
    return 0;
}

void hal_trace_app_custom_register(HAL_TRACE_APP_NOTIFY_T notify_cb, HAL_TRACE_APP_OUTPUT_T output_cb, HAL_TRACE_APP_OUTPUT_T crash_custom_cb)
{
#ifdef TRACE_TO_APP
    hal_trace_app_register(HAL_TRACE_APP_REG_ID_0, notify_cb, output_cb);
    app_crash_custom_cb = crash_custom_cb;
#endif
}

void hal_trace_global_tag_register(HAL_TRACE_GLOBAL_TAG_CB_T tag_cb)
{
#ifdef TRACE_GLOBAL_TAG
    gbl_tag_cb = tag_cb;
#endif
}

int hal_trace_open_cp(HAL_TRACE_BUF_CTRL_T buf_cb, HAL_TRACE_APP_NOTIFY_T notify_cb)
{
#ifdef CP_TRACE_ENABLE
    cp_buffer_cb = buf_cb;
#ifdef CRASH_DUMP_ENABLE
    cp_notify_cb = notify_cb;
#endif
#ifdef FAULT_DUMP
    NVIC_SetDefaultFaultHandler_cp(hal_trace_fault_handler);
#endif
#endif

    return 0;
}

int hal_trace_close_cp(void)
{
#ifdef CP_TRACE_ENABLE
    cp_buffer_cb = NULL;
#ifdef CRASH_DUMP_ENABLE
    cp_notify_cb = NULL;
#endif
    // Force to unlock CP trace memsc
    hal_memsc_unlock(HAL_MEMSC_ID_TRACE);
#endif

    return 0;
}

#else // !(DEBUG || REL_TRACE_ENABLE)

int hal_trace_open(enum HAL_TRACE_TRANSPORT_T transport)
{
#ifdef FAULT_DUMP
#ifdef __ARM_ARCH_ISA_ARM
    GIC_SetFaultDumpHandler(hal_trace_fault_dump);
#else
    NVIC_SetDefaultFaultHandler(hal_trace_fault_handler);
#endif
#endif

    return 0;
}

#endif // !(DEBUG || REL_TRACE_ENABLE)

int hal_trace_address_writable(uint32_t addr)
{
    if (RAM_BASE < addr && addr < RAM_BASE + RAM_SIZE) {
        return 1;
    }
#if defined(PSRAM_BASE) && (PSRAM_SIZE > 0)
    if (PSRAM_BASE < addr && addr < PSRAM_BASE + PSRAM_SIZE) {
        return 1;
    }
#endif
#if defined(PSRAM_NC_BASE) && (PSRAM_SIZE > 0)
    if (PSRAM_NC_BASE < addr && addr < PSRAM_NC_BASE + PSRAM_SIZE) {
        return 1;
    }
#endif
#if defined(PSRAMUHS_BASE) && (PSRAMUHS_SIZE > 0)
    if (PSRAMUHS_BASE < addr && addr < PSRAMUHS_BASE + PSRAMUHS_SIZE) {
        return 1;
    }
#endif
#if defined(PSRAMUHS_NC_BASE) && (PSRAMUHS_SIZE > 0)
    if (PSRAMUHS_NC_BASE < addr && addr < PSRAMUHS_NC_BASE + PSRAMUHS_SIZE) {
        return 1;
    }
#endif
#if defined(RAMRET_BASE) && (RAMRET_SIZE > 0)
    if (RAMRET_BASE < addr && addr < RAMRET_BASE + RAMRET_SIZE) {
        return 1;
    }
#endif
#if defined(RAMCP_BASE) && (RAMCP_SIZE > 0)
    if (RAMCP_BASE < addr && addr < RAMCP_BASE + RAMCP_SIZE) {
        return 1;
    }
#endif
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    if (RAM_NS_BASE < addr && addr < RAM_NS_BASE + RAM_NS_SIZE) {
        return 1;
    }
#endif

#if defined(CONFIG_BES_DUALCORE_AMP)
#ifdef PSRAMCP_BASE
    if (PSRAMCP_BASE < addr && addr < PSRAMCP_BASE + PSRAMCP_SIZE) {
        return 1;
    }
#endif
#endif
    return 0;
}

int hal_trace_address_executable(uint32_t addr)
{
    // Check thumb code
    if ((addr & 1) == 0) {
        return 0;
    }

// Avoid reading out of valid memory region when parsing instruction content
#define X_ADDR_OFFSET                   0x10

    // Check location
    if ((RAMX_BASE + X_ADDR_OFFSET) < addr && addr < (RAMX_BASE + RAM_SIZE)) {
        return 1;
    }
#if defined(PSRAMX_BASE) && (PSRAM_SIZE > 0)
    if ((PSRAMX_BASE + X_ADDR_OFFSET) < addr && addr < (PSRAMX_BASE + PSRAM_SIZE)) {
        return 1;
    }
#endif
#if defined(PSRAMUHSX_BASE) && (PSRAMUHS_SIZE > 0)
    if ((PSRAMUHSX_BASE + X_ADDR_OFFSET) < addr && addr < (PSRAMUHSX_BASE + PSRAMUHS_SIZE)) {
        return 1;
    }
#endif
#if defined(RAMXRET_BASE) && (RAMRET_SIZE > 0)
    if ((RAMXRET_BASE + X_ADDR_OFFSET) < addr && addr < (RAMXRET_BASE + RAMRET_SIZE)) {
        return 1;
    }
#endif

    // Check flash location
#ifndef NO_FLASH_BASE_ACCESS
#ifdef OTA_CODE_OFFSET
#define FLASH_EXEC_START                    (FLASHX_BASE + OTA_CODE_OFFSET)
#define FLASH_EXEC_SIZE                     (FLASH_SIZE - OTA_CODE_OFFSET)
#else
#define FLASH_EXEC_START                    (FLASHX_BASE)
#define FLASH_EXEC_SIZE                     (FLASH_SIZE)
#endif
#ifdef FLASH_REGION_SIZE
#undef FLASH_EXEC_SIZE
#define FLASH_EXEC_SIZE                     (FLASH_REGION_SIZE)
#endif
    if ((FLASH_EXEC_START + X_ADDR_OFFSET) < addr && addr < (FLASH_EXEC_START + FLASH_EXEC_SIZE)) {
        return 1;
    }
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    if ((RAMX_NS_BASE + X_ADDR_OFFSET) < addr && addr < (RAMX_NS_BASE + RAM_NS_SIZE)) {
        return 1;
    }
    if ((FLASHX_BASE + NS_APP_START_OFFSET + X_ADDR_OFFSET) < addr && addr < (FLASHX_BASE + FLASH_SIZE)) {
        return 1;
    }
#endif
#if defined(CONFIG_BES_DUALCORE_AMP)
#ifdef PSRAMCPX_BASE
    if (PSRAMCPX_BASE + X_ADDR_OFFSET < addr && addr < PSRAMCPX_BASE + PSRAMCP_SIZE) {
        return 1;
    }
#endif
#endif
//#define CHECK_ROM_CODE
#ifdef CHECK_ROM_CODE
#ifdef ROM_EXT_SIZE
#define ROM_TOTAL_SIZE                      (ROM_SIZE + ROM_EXT_SIZE)
#else
#define ROM_TOTAL_SIZE                      (ROM_SIZE)
#endif
    if ((ROMX_BASE + (NVIC_USER_IRQ_OFFSET * 4)) < addr && addr < (ROMX_BASE + ROM_TOTAL_SIZE)) {
        return 1;
    }
#endif

#if 0
    if (FLASHX_NC_BASE < addr && addr < FLASHX_NC_BASE + FLASH_SIZE) {
        return 1;
    }
#ifdef PSRAMX_NC_BASE
    if (PSRAMX_NC_BASE < addr && addr < PSRAMX_NC_BASE + PSRAM_SIZE) {
        return 1;
    }
#endif
#ifdef PSRAMUHSX_NC_BASE
    if (PSRAMUHSX_NC_BASE < addr && addr < PSRAMUHSX_NC_BASE + PSRAMUHS_SIZE) {
        return 1;
    }
#endif
#endif
    return 0;
}

int hal_trace_address_readable(uint32_t addr)
{
    if (hal_trace_address_writable(addr)) {
        return 1;
    }
    if (hal_trace_address_executable(addr)) {
        return 1;
    }
    if (FLASH_BASE < addr && addr < FLASH_BASE + FLASH_SIZE) {
        return 1;
    }
    if (FLASH_NC_BASE < addr && addr < FLASH_NC_BASE + FLASH_SIZE) {
        return 1;
    }
#if defined(PSRAM_NC_BASE) && (PSRAM_SIZE > 0)
    if (PSRAM_NC_BASE < addr && addr < PSRAM_NC_BASE + PSRAM_SIZE) {
        return 1;
    }
#endif
#if defined(PSRAMUHS_NC_BASE) && (PSRAMUHS_SIZE > 0)
    if (PSRAMUHS_NC_BASE < addr && addr < PSRAMUHS_NC_BASE + PSRAMUHS_SIZE) {
        return 1;
    }
#endif
    return 0;
}

static void NORETURN hal_trace_crash_end(void)
{
#if (defined(DEBUG) || defined(REL_TRACE_ENABLE))
    hal_trace_output_linefeed();
    hal_trace_flush_buffer();
    hal_sys_timer_delay(MS_TO_TICKS(5));
#endif

    // Tag failure for simulation environment
    hal_cmu_simu_fail();

#ifndef IN_SUBSYS
#ifdef CRASH_REBOOT
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_REBOOT|HAL_SW_BOOTMODE_REBOOT_FROM_CRASH);
    pmu_reboot();
#else
    hal_iomux_set_analog_i2c();
    hal_iomux_set_jtag();
    hal_cmu_jtag_clock_enable();
#endif
#endif

    SAFE_PROGRAM_STOP();
}

__STATIC_FORCEINLINE uint32_t get_cpu_id_tag(void)
{
    uint32_t cpu_id;
#ifdef CP_TRACE_ENABLE
    cpu_id = get_cpu_id();
#elif defined(__ARM_ARCH_ISA_ARM)
    cpu_id = 0xAAAA0000 | (get_cpu_id() & 0xFFFF);
#else
    cpu_id = 0;
#endif
    return cpu_id;
}

static void NORETURN USED hal_trace_assert_dump_internal(ASSERT_DUMP_ARGS)
{
    const uint32_t *p_regs;
    struct ASSERT_INFO_T info;
    int i;

    crash_occurs[get_cpu_id()] = true;

    int_lock_global();

    p_regs = (const uint32_t *)crash_buf;
    for (i = 0; i < ARRAY_SIZE(info.R); i++) {
        info.R[i] = p_regs[i];
    }
#ifndef __ARM_ARCH_ISA_ARM
    info.MSP = p_regs[ARRAY_SIZE(info.R)];
    info.PSP = p_regs[ARRAY_SIZE(info.R) + 1];
#endif

    info.ID = HAL_TRACE_ASSERT_ID;
    info.CPU_ID = get_cpu_id_tag();
#if (defined(DEBUG) || defined(REL_TRACE_ENABLE)) && defined(ASSERT_SHOW_FILE_FUNC)
    info.FILE = file;
#elif (defined(DEBUG) || defined(REL_TRACE_ENABLE)) && defined(ASSERT_SHOW_FILE)
    info.FILE = scope;
#else
    info.FILE = NULL;
#endif
#if (defined(DEBUG) || defined(REL_TRACE_ENABLE)) && defined(ASSERT_SHOW_FILE_FUNC)
    info.FUNC = func;
#elif (defined(DEBUG) || defined(REL_TRACE_ENABLE)) && defined(ASSERT_SHOW_FUNC)
    info.FUNC = scope;
#else
    info.FUNC = NULL;
#endif
#if (defined(DEBUG) || defined(REL_TRACE_ENABLE)) && (defined(ASSERT_SHOW_FILE_FUNC) || defined(ASSERT_SHOW_FILE) || defined(ASSERT_SHOW_FUNC))
    info.LINE = line;
#else
    info.LINE = 0;
#endif
    info.FMT = fmt;
#ifndef __ARM_ARCH_ISA_ARM
    info.CONTROL = __get_CONTROL();
#ifdef __ARM_ARCH_8M_MAIN__
    info.MSPLIM = __get_MSPLIM();
    info.PSPLIM = __get_PSPLIM();
#endif
#endif

    *(volatile uint32_t *)RAM_BASE = (uint32_t)&info;

#ifdef ASSERT_MUTE_CODEC
    bool crash_mute = true;
#ifdef CP_TRACE_ENABLE
    if (get_cpu_id()) {
        // Forbid CP to access CODEC
        crash_mute = false;
    }
#endif
    if (crash_mute) {
        hal_codec_crash_mute();
    }
#endif

#if (defined(DEBUG) || defined(REL_TRACE_ENABLE))

    static const char POSSIBLY_UNUSED desc_file[] = "FILE    : ";
    static const char POSSIBLY_UNUSED desc_func[] = "FUNCTION: ";
    static const char POSSIBLY_UNUSED desc_line[] = "LINE    : ";
    int len;
    va_list ap;

#ifdef CP_TRACE_ENABLE
    // Release all the possible trace locks
    hal_trace_cp_force_unlock();
#endif
    // Continue the trace
    hal_trace_force_continue();

#ifdef CRASH_DUMP_ENABLE
    bool full_dump = true;

#ifdef CP_TRACE_ENABLE
    if (get_cpu_id()) {
        full_dump = false;
        if (cp_notify_cb) {
            cp_notify_cb(HAL_TRACE_STATE_CRASH_ASSERT_START);
        }
    }
#endif

    if (full_dump) {
#ifdef TRACE_TO_APP
        hal_trace_app_notify_callback(HAL_TRACE_STATE_CRASH_ASSERT_START);
#endif
    }
#endif

    hal_trace_flush_buffer();

    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_52M);

    len = hal_trace_print_time(TR_LEVEL_CRITICAL, TR_MODULE_NONE, &crash_buf[0], sizeof(crash_buf));
    if (len > 0) {
        hal_trace_output_linefeed();
        hal_trace_output((unsigned char *)crash_buf, len);
    }
    len = snprintf(&crash_buf[0], sizeof(crash_buf), NEW_LINE_STR "### ASSERT @ 0x%08X ###" NEW_LINE_STR, (unsigned)info.R[14]);
    hal_trace_output((unsigned char *)crash_buf, len);

#if defined(ASSERT_SHOW_FILE_FUNC) || defined(ASSERT_SHOW_FILE) || defined(ASSERT_SHOW_FUNC)
    const char separate_line[] = "----------------------------------------" NEW_LINE_STR;
#ifdef ASSERT_SHOW_FILE
    const char *file = scope;
#elif defined(ASSERT_SHOW_FUNC)
    const char *func = scope;
#endif

#if defined(ASSERT_SHOW_FILE_FUNC) || defined(ASSERT_SHOW_FILE)
    hal_trace_output((const unsigned char *)desc_file, sizeof(desc_file) - 1);
    hal_trace_output((const unsigned char *)file, strlen(file));
    hal_trace_output_linefeed();
#endif

#if defined(ASSERT_SHOW_FILE_FUNC) || defined(ASSERT_SHOW_FUNC)
    hal_trace_output((const unsigned char *)desc_func, sizeof(desc_func) - 1);
    hal_trace_output((const unsigned char *)func, strlen(func));
    hal_trace_output_linefeed();
#endif

    hal_trace_output((const unsigned char *)desc_line, sizeof(desc_func) - 1);
    len = snprintf(crash_buf, sizeof(crash_buf), "%u", line);
    hal_trace_output((const unsigned char *)crash_buf, len);
    hal_trace_output_linefeed();

    hal_trace_output((unsigned char *)separate_line, sizeof(separate_line) - 1);

    hal_trace_flush_buffer();
#endif

    va_start(ap, fmt);
    len = hal_trace_format_va(0, crash_buf, sizeof(crash_buf), fmt, ap);
    va_end(ap);

    hal_trace_output((unsigned char *)crash_buf, len);

    hal_trace_flush_buffer();

#ifdef ASSERT_VERBOSE_DUMP
    hal_trace_print_common_registers(info.R);
    hal_trace_print_special_stack_registers(info.MSP, info.PSP);

    hal_trace_print_stack(info.R[13]);

    hal_trace_print_backtrace(info.R[13], TRACE_BACKTRACE_SEARCH_WORD, TRACE_BACKTRACE_NUM);

    hal_trace_output_linefeed();

    hal_trace_flush_buffer();
#endif

#ifdef CRASH_DUMP_ENABLE
    if (full_dump) {
        hal_trace_crash_dump_callback();
        hal_trace_flush_buffer();
        hal_sys_timer_delay(MS_TO_TICKS(5));

#ifdef CORE_DUMP
        {
            static CrashCatcherAssertRegisters regs;

            regs.msp = info.MSP;
            regs.psp = info.PSP;
            regs.assertPSR = __get_xPSR();
            regs.R.r0 = info.R[0];
            regs.R.r1 = info.R[1];
            regs.R.r2 = info.R[2];
            regs.R.r3 = info.R[3];
            regs.R.r4 = info.R[4];
            regs.R.r5 = info.R[5];
            regs.R.r6 = info.R[6];
            regs.R.r7 = info.R[7];
            regs.R.r8 = info.R[8];
            regs.R.r9 = info.R[9];
            regs.R.r10 = info.R[10];
            regs.R.r11 = info.R[11];
            regs.R.r12 = info.R[12];
            regs.R.sp = info.R[13];
            regs.R.lr = info.R[14];
            /*
             * ASSERT's pc is not important, but who calling it is more important,
             * and just setting it to lr as normal assert dump.
             */
            regs.R.pc = info.R[14];
            regs.R.psr = regs.assertPSR;

            AssertCatcher_Entry(&regs);
        }
        hal_sys_timer_delay(MS_TO_TICKS(5));
#endif

#ifdef TRACE_TO_APP
        hal_trace_app_notify_callback(HAL_TRACE_STATE_CRASH_END);
#endif
    }

#ifdef CP_TRACE_ENABLE
    if (get_cpu_id()) {
        if (cp_notify_cb) {
            cp_notify_cb(HAL_TRACE_STATE_CRASH_END);
        }
        SAFE_PROGRAM_STOP();
    }
#endif
#endif // CRASH_DUMP_ENABLE

#endif // DEBUG || REL_TRACE_ENABLE

    hal_trace_crash_end();
}

void NORETURN NAKED WEAK hal_trace_assert_dump(ASSERT_DUMP_ARGS)
{
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) && !defined(DEBUG)
    // No way to read out the debug information. Just stop here
    SAFE_PROGRAM_STOP();
#endif

#if defined(CHIP_BEST1501SIMU) || defined(CHIP_BEST1600SIMU)
    hal_cmu_simu_set_val(0xffffffff);
#ifdef FPGA
    while(1);
#endif
#endif

    asm volatile (
        "sub sp, sp, #4*(2+" TO_STRING(ASSERT_STACK_RESERVED) ");"
        ".cfi_def_cfa_offset 4*(2+" TO_STRING(ASSERT_STACK_RESERVED) ");"
        "push {r0-r5};"
        ".cfi_adjust_cfa_offset 4*6;"
        ".cfi_offset r0, -(4*(6+2+" TO_STRING(ASSERT_STACK_RESERVED) "));"
        ".cfi_offset r1, -(4*(5+2+" TO_STRING(ASSERT_STACK_RESERVED) "));"
        ".cfi_offset r2, -(4*(4+2+" TO_STRING(ASSERT_STACK_RESERVED) "));"
        ".cfi_offset r3, -(4*(3+2+" TO_STRING(ASSERT_STACK_RESERVED) "));"
        ".cfi_offset r4, -(4*(2+2+" TO_STRING(ASSERT_STACK_RESERVED) "));"
        ".cfi_offset r5, -(4*(1+2+" TO_STRING(ASSERT_STACK_RESERVED) "));"
        "ldr r0, =crash_buf;"
        "ldr r1, [sp];"
        "str r1, [r0], #4;"
        "ldr r1, [sp, #4];"
        "str r1, [r0], #4;"
        "stmia r0!, {r2-r12};"
        "add r1, sp, #4*(6+2+" TO_STRING(ASSERT_STACK_RESERVED) ");"
        "movs r5, r1;"
        "str r1, [r0], #4;"
        "str lr, [r0], #4;"
#ifndef __ARM_ARCH_ISA_ARM
        // Check CONTROL.SPSEL (bit[1])
        "mrs r3, control;"
        "tst r3, #0x02;"
        "itte ne;"
        // r1 is still the original SP
        "movne r2, r1;"
        "mrsne r1, msp;"
        "mrseq r2, psp;"
#endif
        "str r1, [r0], #4;"
        "str r2, [r0], #4;"
#if (ASSERT_STACK_ARG_WORD != 8)
#error "Bad ASSERT_STACK_ARG_WORD: should be 8"
#endif
        // Save assert arguments in stack (ASSERT_STACK_ARG_WORD)
        "add r4, sp, #4*6;"
        "ldmia r5!, {r0-r3};"
        "stmia r4!, {r0-r3};"
        "ldmia r5!, {r0-r3};"
        "stmia r4!, {r0-r3};"
        "str lr, [r4];"
        ".cfi_offset lr, -(4*(2+" TO_STRING(STACK_DUMP_CNT_PREV) "));"
        "pop {r0-r5};"
        ".cfi_restore r5;"
        ".cfi_restore r4;"
        ".cfi_restore r3;"
        ".cfi_restore r2;"
        ".cfi_restore r1;"
        ".cfi_restore r0;"
        ".cfi_adjust_cfa_offset -4*6;"
        "bl hal_trace_assert_dump_internal;"
    );
}

#ifdef FAULT_DUMP
static void hal_trace_fill_exception_info(struct EXCEPTION_INFO_T *info, const uint32_t *regs, const uint32_t *extra, uint32_t extra_len)
{
    info->ID = HAL_TRACE_EXCEPTION_ID;
    info->CPU_ID = get_cpu_id_tag();
    info->REGS = regs;
#ifdef __ARM_ARCH_ISA_ARM
    info->extra = extra;
    info->extra_len = extra_len;
#else
    info->MSP = regs[18];
    info->PSP = __get_PSP();
    info->PRIMASK = (regs[17] & 0xFF);
    info->FAULTMASK = ((regs[17] >> 8) & 0xFF);
    info->BASEPRI = ((regs[17] >> 16) & 0xFF);
    info->CONTROL = ((regs[17] >> 24) & 0xFF);
    info->ICSR = SCB->ICSR;
    info->AIRCR = SCB->AIRCR;
    info->SCR = SCB->SCR;
    info->CCR = SCB->CCR;
    info->SHCSR = SCB->SHCSR;
    info->CFSR = SCB->CFSR;
    info->HFSR = SCB->HFSR;
    info->AFSR = SCB->AFSR;
    info->DFSR = SCB->DFSR;
    info->MMFAR = SCB->MMFAR;
    info->BFAR = SCB->BFAR;
#ifdef __ARM_ARCH_8M_MAIN__
    info->MSPLIM = __get_MSPLIM();
    info->PSPLIM = __get_PSPLIM();
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    info->SFSR = SAU->SFSR;
    info->SFAR = SAU->SFAR;
    info->MSP_NS = __TZ_get_MSP_NS();
    info->PSP_NS = __TZ_get_PSP_NS();
    info->MSPLIM_NS = __TZ_get_MSPLIM_NS();
    info->PSPLIM_NS = __TZ_get_PSPLIM_NS();
    info->PRIMASK_NS = __TZ_get_PRIMASK_NS();
    info->FAULTMASK_NS = __TZ_get_FAULTMASK_NS();
    info->BASEPRI_NS = __TZ_get_BASEPRI_NS();
    info->CONTROL_NS = __TZ_get_CONTROL_NS();
#endif
#endif
#endif
}

#if (defined(DEBUG) || defined(REL_TRACE_ENABLE))
#ifdef __ARM_ARCH_ISA_ARM
static void hal_trace_print_fault_info_ca(const struct EXCEPTION_INFO_T *info)
{
    const struct FAULT_REGS_T *fregs;
    const uint32_t *extra;
    //uint32_t extra_len;
    enum EXCEPTION_ID_T id;
    int len;
    uint32_t val;
    const char *desc;

    fregs = (const struct FAULT_REGS_T*)info->REGS;
    extra = info->extra;
    //extra_len = info->extra_len;

    id = (enum EXCEPTION_ID_T)extra[0];

    len = snprintf(crash_buf, sizeof(crash_buf), "PC =%08X", (unsigned)fregs->r[15]);
    len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", ExceptionNumber=%d" NEW_LINE_STR, id);
    hal_trace_output((unsigned char *)crash_buf, len);

    hal_trace_print_common_registers(fregs->r);

    len = snprintf(crash_buf, sizeof(crash_buf), "SPSR=%08X", (unsigned)fregs->spsr);
    len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", APSR=%c%c%c%c%c",
        (fregs->spsr & (1 << 31)) ? 'N' : 'n',
        (fregs->spsr & (1 << 30)) ? 'Z' : 'z',
        (fregs->spsr & (1 << 29)) ? 'C' : 'c',
        (fregs->spsr & (1 << 28)) ? 'V' : 'v',
        (fregs->spsr & (1 << 27)) ? 'Q' : 'q'
        );
    len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", XC=%c%c%c%c%c",
        (fregs->spsr & (1 << 9)) ? 'E' : 'e',
        (fregs->spsr & (1 << 8)) ? 'A' : 'a',
        (fregs->spsr & (1 << 7)) ? 'I' : 'i',
        (fregs->spsr & (1 << 6)) ? 'F' : 'f',
        (fregs->spsr & (1 << 5)) ? 'T' : 't'
        );
    val = fregs->spsr & 0x1F;
    if (val == 0x10) {
        desc = "USR";
    } else if (val == 0x11) {
        desc = "FIQ";
    } else if (val == 0x12) {
        desc = "IRQ";
    } else if (val == 0x13) {
        desc = "SVC";
    } else if (val == 0x16) {
        desc = "MON";
    } else if (val == 0x17) {
        desc = "ABT";
    } else if (val == 0x1A) {
        desc = "HYP";
    } else if (val == 0x1B) {
        desc = "UND";
    } else if (val == 0x1F) {
        desc = "SYS";
    } else {
        desc = "UNKNOWN";
    }
    len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", MODE=%02X (%s)", (unsigned)val, desc);
    hal_trace_output((unsigned char *)crash_buf, len);
    hal_trace_output_linefeed();
    hal_trace_output_linefeed();

    hal_trace_flush_buffer();

    if (id == EXCEPTION_UNDEF) {
    } else if (id == EXCEPTION_SVC) {
    } else if (id == EXCEPTION_PABT) {
    } else if (id == EXCEPTION_DABT) {
    } else {
    }
}
#else
static void hal_trace_print_fault_info_cm(const struct EXCEPTION_INFO_T *info)
{
    const uint32_t *regs;
    int len;
    uint32_t val;

    regs = info->REGS;

    len = snprintf(crash_buf, sizeof(crash_buf), "PC =%08X", (unsigned)regs[15]);
    val = __get_IPSR();
    if (val == 0) {
        len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", ThreadMode");
    } else {
        len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", ExceptionNumber=D'%d", (int)val - 16);
    }
    len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", EXC_RETURN=%08X" NEW_LINE_STR, regs[19]);
    hal_trace_output((unsigned char *)crash_buf, len);

    hal_trace_print_common_registers(regs);
    hal_trace_print_special_stack_registers(info->MSP, info->PSP);
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    hal_trace_print_special_stack_registers_ns(info->MSP_NS, info->PSP_NS);
#endif
    hal_trace_output_linefeed();

    len = snprintf(crash_buf, sizeof(crash_buf), "PRIMASK   =%02X, FAULTMASK   =%02X, BASEPRI   =%02X, CONTROL   =%02X" NEW_LINE_STR,
        (unsigned)info->PRIMASK, (unsigned)info->FAULTMASK, (unsigned)info->BASEPRI, (unsigned)info->CONTROL);
    hal_trace_output((unsigned char *)crash_buf, len);
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    len = snprintf(crash_buf, sizeof(crash_buf), "PRIMASK_NS=%02X, FAULTMASK_NS=%02X, BASEPRI_NS=%02X, CONTROL_NS=%02X" NEW_LINE_STR,
        (unsigned)info->PRIMASK_NS, (unsigned)info->FAULTMASK_NS, (unsigned)info->BASEPRI_NS, (unsigned)info->CONTROL_NS);
    hal_trace_output((unsigned char *)crash_buf, len);
#endif
    hal_trace_output_linefeed();

    len = snprintf(crash_buf, sizeof(crash_buf), "XPSR=%08X", (unsigned)regs[16]);
    len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", APSR=%c%c%c%c%c",
        (regs[16] & (1 << 31)) ? 'N' : 'n',
        (regs[16] & (1 << 30)) ? 'Z' : 'z',
        (regs[16] & (1 << 29)) ? 'C' : 'c',
        (regs[16] & (1 << 28)) ? 'V' : 'v',
        (regs[16] & (1 << 27)) ? 'Q' : 'q'
        );
    val = regs[16] & 0x1FF;
    len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, ", EPSR=%08X, IPSR=%03X", (unsigned)(regs[16] & 0x0700FC00), (unsigned)val);
    if (val == 0) {
        len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (NoException)");
    }
    hal_trace_output((unsigned char *)crash_buf, len);
    hal_trace_output_linefeed();
    hal_trace_output_linefeed();

    hal_trace_flush_buffer();

    len = snprintf(crash_buf, sizeof(crash_buf), "ICSR =%08X, AIRCR=%08X, SCR  =%08X, CCR  =%08X" NEW_LINE_STR,
        (unsigned)info->ICSR, (unsigned)info->AIRCR, (unsigned)info->SCR, (unsigned)info->CCR);
    hal_trace_output((unsigned char *)crash_buf, len);

    len = snprintf(crash_buf, sizeof(crash_buf), "SHCSR=%08X, CFSR =%08X, HFSR =%08X, AFSR =%08X" NEW_LINE_STR,
        (unsigned)info->SHCSR, (unsigned)info->CFSR, (unsigned)info->HFSR, (unsigned)info->AFSR);
    hal_trace_output((unsigned char *)crash_buf, len);

    len = snprintf(crash_buf, sizeof(crash_buf), "DFSR =%08X", (unsigned)info->DFSR);
    hal_trace_output((unsigned char *)crash_buf, len);
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    len = snprintf(crash_buf, sizeof(crash_buf), ", SFSR =%08X", (unsigned)info->SFSR);
    hal_trace_output((unsigned char *)crash_buf, len);
#endif
    hal_trace_output_linefeed();

    len = snprintf(crash_buf, sizeof(crash_buf), "MMFAR=%08X, BFAR =%08X",
        (unsigned)info->MMFAR, (unsigned)info->BFAR);
    hal_trace_output((unsigned char *)crash_buf, len);
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    len = snprintf(crash_buf, sizeof(crash_buf), ", SFAR =%08X", (unsigned)info->SFAR);
    hal_trace_output((unsigned char *)crash_buf, len);
#endif
    hal_trace_output_linefeed();
    hal_trace_output_linefeed();

    if (info->HFSR & (1 << 30)) {
        len = snprintf(crash_buf, sizeof(crash_buf), "(Escalation HardFault)" NEW_LINE_STR);
        hal_trace_output((unsigned char *)crash_buf, len);
    }

    len = snprintf(crash_buf, sizeof(crash_buf), "FaultInfo    :");
    if ((info->SHCSR & 0x13F) == 0) {
        len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (None)");
    } else {
        if (info->SHCSR & (1 << 0)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (MemFault)");
        }
        if (info->SHCSR & (1 << 1)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (BusFault)");
        }
#ifdef __ARM_ARCH_8M_MAIN__
        if (info->SHCSR & (1 << 2)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (HardFault)");
        }
#endif
        if (info->SHCSR & (1 << 3)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (UsageFault)");
        }
#ifdef __ARM_ARCH_8M_MAIN__
        if (info->SHCSR & (1 << 4)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (SecureFault)");
        }
        if (info->SHCSR & (1 << 5)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (NMI)");
        }
#endif
        if (info->SHCSR & (1 << 8)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Monitor)");
        }
    }
    hal_trace_output((unsigned char *)crash_buf, len);
    hal_trace_output_linefeed();

    len = snprintf(crash_buf, sizeof(crash_buf), "FaultCause   :");
    if (info->CFSR == 0) {
        len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (None)");
    } else {
        if (info->CFSR & (1 << 0)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Instruction access violation)");
        }
        if (info->CFSR & (1 << 1)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Data access violation)");
        }
        if (info->CFSR & (1 << 3)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (MemFault on unstacking for a return from exception)");
        }
        if (info->CFSR & (1 << 4)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (MemFault on stacking for exception entry)");
        }
        if (info->CFSR & (1 << 5)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (MemFault during floating-point lazy state preservation)");
        }
        if (info->CFSR & (1 << 7)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (MMFAR valid)");
        }
        if (len) {
            hal_trace_output((unsigned char *)crash_buf, len);
            hal_trace_flush_buffer();
            len = 0;
        }
        if (info->CFSR & (1 << 8)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Instruction bus error)");
        }
        if (info->CFSR & (1 << 9)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Precise data bus error)");
        }
#ifndef __ARM_ARCH_8M_MAIN__
        if (info->CFSR & (1 << 10)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Imprecise data bus error)");
        }
#endif
        if (info->CFSR & (1 << 11)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (BusFault on unstacking for a return from exception)");
        }
        if (info->CFSR & (1 << 12)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (BusFault on stacking for exception entry)");
        }
        if (info->CFSR & (1 << 13)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (BusFault during floating-point lazy state preservation)");
        }
        if (info->CFSR & (1 << 15)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (BFAR valid)");
        }
        if (len) {
            hal_trace_output((unsigned char *)crash_buf, len);
            hal_trace_flush_buffer();
            len = 0;
        }
        if (info->CFSR & (1 << 16)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Undefined instruction UsageFault)");
        }
        if (info->CFSR & (1 << 17)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Invalid state UsageFault)");
        }
        if (info->CFSR & (1 << 18)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Invalid PC load by EXC_RETURN UsageFault)");
        }
        if (info->CFSR & (1 << 19)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (No coprocessor UsageFault)");
        }
#ifdef __ARM_ARCH_8M_MAIN__
        if (info->CFSR & (1 << 20)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Stack overflow UsageFault)");
        }
#endif
        if (info->CFSR & (1 << 24)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Unaligned access UsageFault)");
        }
        if (info->CFSR & (1 << 25)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Divide by zero UsageFault)");
        }
    }
    hal_trace_output((unsigned char *)crash_buf, len);
    hal_trace_output_linefeed();

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    len = snprintf(crash_buf, sizeof(crash_buf), "SecFaultCause:");
    if (info->SFSR == 0) {
        len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (None)");
    } else {
        if (info->SFSR & (1 << 0)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Invalid entry point)");
        }
        if (info->SFSR & (1 << 1)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Invalid integrity signature)");
        }
        if (info->SFSR & (1 << 2)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Invalid excpetion return)");
        }
        if (info->SFSR & (1 << 3)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Attribution unit violation)");
        }
        if (info->SFSR & (1 << 4)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Invalid transition)");
        }
        if (info->SFSR & (1 << 5)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Lazy floating-point state preservation error)");
        }
        if (info->SFSR & (1 << 6)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (SFAR valid)");
        }
        if (info->SFSR & (1 << 7)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Lazy state error)");
        }
    }
    hal_trace_output((unsigned char *)crash_buf, len);
    hal_trace_output_linefeed();
#endif

    len = snprintf(crash_buf, sizeof(crash_buf), "DebugEvent   :");
    if (info->DFSR == 0) {
        len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (None)");
    } else {
        if (info->DFSR & (1 << 0)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Halted)");
        }
        if (info->DFSR & (1 << 2)) {
            len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Data Watchpoint Match)");
        }
        /*scan the all dwt functions*/
        volatile uint32_t *func = &DWT->FUNCTION0;
        uint32_t func_num = (DWT->CTRL >> 28) & 0xf;

        for (int i = 0; i < func_num; i++) {
            if (*func & DWT_FUNCTION_MATCHED_Msk) {
                len += snprintf(&crash_buf[len], sizeof(crash_buf) - len, " (Function %d matched)", i);
            }
            func += 4;
        }
    }

    hal_trace_output((unsigned char *)crash_buf, len);
    hal_trace_output_linefeed();
}
#endif

static void hal_trace_print_fault_info(const struct EXCEPTION_INFO_T *info)
{
#ifdef __ARM_ARCH_ISA_ARM
    hal_trace_print_fault_info_ca(info);
#else
    hal_trace_print_fault_info_cm(info);
#endif
}
#endif // DEBUG || REL_TRACE_ENABLE

void hal_trace_fault_dump(const uint32_t *regs, const uint32_t *extra, uint32_t extra_len)
{
    struct EXCEPTION_INFO_T info;

    crash_occurs[get_cpu_id()] = true;
#if defined(CHIP_BEST1501SIMU) || defined(CHIP_BEST1600SIMU)
    hal_cmu_simu_set_val(0xdddddddd);
#endif

    int_lock_global();

    hal_trace_fill_exception_info(&info, regs, extra, extra_len);

    *(volatile uint32_t *)RAM_BASE = (uint32_t)&info;

#if (defined(DEBUG) || defined(REL_TRACE_ENABLE))

    static const char title[] = NEW_LINE_STR "### EXCEPTION ###" NEW_LINE_STR;
    int len;

#ifdef CP_TRACE_ENABLE
    // Release all the possible trace locks
    hal_trace_cp_force_unlock();
#endif
    // Continue the trace
    hal_trace_force_continue();

#ifdef CRASH_DUMP_ENABLE
    bool full_dump = true;

#ifdef CP_TRACE_ENABLE
    if (get_cpu_id()) {
        full_dump = false;
        if (cp_notify_cb) {
            cp_notify_cb(HAL_TRACE_STATE_CRASH_FAULT_START);
        }
    }
#endif

    if (full_dump) {
#ifdef TRACE_TO_APP
        hal_trace_app_notify_callback(HAL_TRACE_STATE_CRASH_FAULT_START);
#endif
    }
#endif

    hal_trace_flush_buffer();

    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_52M);

    len = hal_trace_print_time(TR_LEVEL_CRITICAL, TR_MODULE_NONE, &crash_buf[0], sizeof(crash_buf));
    if (len > 0) {
        hal_trace_output_linefeed();
        hal_trace_output((unsigned char *)crash_buf, len);
    }
    hal_trace_output((unsigned char *)title, sizeof(title) - 1);

    hal_trace_print_fault_info(&info);

    hal_trace_flush_buffer();

    hal_trace_print_stack(regs[13]);

    hal_trace_print_backtrace(regs[13], TRACE_BACKTRACE_SEARCH_WORD, TRACE_BACKTRACE_NUM);

    hal_trace_output_linefeed();

    hal_trace_flush_buffer();

#ifdef CRASH_DUMP_ENABLE
    if (full_dump) {
        hal_trace_crash_dump_callback();
        hal_trace_flush_buffer();

#ifndef __ARM_ARCH_ISA_ARM
#ifdef TRACE_TO_APP
        // Crash-Dump "Lite-Version"
        if (app_crash_custom_cb) {
            uint32_t *stack;
            app_crash_custom_cb((unsigned char *)regs,
                           CRASH_DUMP_REGISTERS_NUM_BYTES);
            stack =  (uint32_t *)(__get_MSP() & ~3);
            app_crash_custom_cb((unsigned char *)stack,
                           ((CRASH_DUMP_STACK_NUM_BYTES)/2));
            stack =  (uint32_t *)(__get_PSP() & ~3);
            app_crash_custom_cb((unsigned char *)stack,
                          ((CRASH_DUMP_STACK_NUM_BYTES)/2));
        }
#endif

#ifdef CORE_DUMP
        {
            static CrashCatcherExceptionRegisters eregs;

            eregs.msp = info.MSP;
            eregs.psp = info.PSP;
            eregs.exceptionPSR = regs[16] & 0x0700FE00;
            eregs.r4 = regs[4];
            eregs.r5 = regs[5];
            eregs.r6 = regs[6];
            eregs.r7 = regs[7];
            eregs.r8 = regs[8];
            eregs.r9 = regs[9];
            eregs.r10 = regs[10];
            eregs.r11 = regs[11];
            eregs.exceptionLR = regs[19];
            CrashCatcher_Entry( &eregs);
        }
#endif
#endif // !__ARM_ARCH_ISA_ARM

#ifdef TRACE_TO_APP
        hal_trace_app_notify_callback(HAL_TRACE_STATE_CRASH_END);
#endif
    }

#ifdef CP_TRACE_ENABLE
    if (get_cpu_id()) {
        if (cp_notify_cb) {
            cp_notify_cb(HAL_TRACE_STATE_CRASH_END);
        }
        SAFE_PROGRAM_STOP();
    }
#endif
#endif // CRASH_DUMP_ENABLE

#endif // DEBUG || REL_TRACE_ENABLE

    hal_trace_crash_end();
}

#ifndef __ARM_ARCH_ISA_ARM
static void NAKED hal_trace_fault_handler(void)
{
    // TODO: Save FP registers (and check lazy floating-point context preservation)

    // Registers saved in SP
    // R0-R15 PSR SPECIAL_REGS MSP EXC_RETURN

    asm volatile (
        // Get special_regs(primask/faultmask/basepri/control)
        "mrs r0, primask;"
        "mrs r1, faultmask;"
        "mrs r2, basepri;"
        "mrs r3, control;"
        "bfi r12, r0, #0, #8;"
        "bfi r12, r1, #8, #8;"
        "bfi r12, r2, #16, #8;"
        "bfi r12, r3, #24, #8;"
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
        // Check EXC_RETURN.S (bit[6])
        "tst lr, #0x40;"
        "bne _get_sec_sp;"
        // EXC_RETURN.ES must be 1 here
        // Check EXC_RETURN.MODE (bit[3]) and CONTROL.SPSEL (bit[1])
        "ubfx r3, lr, #3, #1;"
        "mrs r2, control_ns;"
        "ubfx r2, r2, #1, #1;"
        "ands r3, r2;"
        "ite ne;"
        // Using PSP_NS
        "mrsne r3, psp_ns;"
        // Using MSP_NS
        "mrseq r3, msp_ns;"
        "b _save_msp_lr;"
        "_get_sec_sp:;"
#endif
        // Check EXC_RETURN.MODE (bit[3]) and EXC_RETURN.SPSEL (bit[2])
        "and r3, lr, #0x0C;"
        "teq r3, #0x0C;"
        "ite eq;"
        // Using PSP
        "mrseq r3, psp;"
        // Using MSP
        "mrsne r3, msp;"
        "_save_msp_lr:;"
        // Save original MSP and current exception lr
        "mrs r0, msp;"
        "push {r0, lr};"
        ".cfi_def_cfa_offset 4*2;"
        ".cfi_rel_offset lr, 4*1;"
#ifdef ARM_CMNS
        // -- Check EXC_RETURN.S (bit[6])
        "tst lr, #0x40;"
        "beq _check_fp_cntx;"
        // Save special_regs(primask/faultmask/basepri/control)
        "str r12, [sp, #-4]!;"
        // Save 0 as psr
        "mov r1, 0;"
        "str r1, [sp, #-4]!;"
        // Save 0 as pc
        "str r1, [sp, #-4]!;"
        // Save 0 as lr
        "str r1, [sp, #-4]!;"
        // Save sp
        "str r0, [sp, #-4]!;"
        // Save 0 as r0-12
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "str r1, [sp, #-4]!;"
        "b _call_fault_handler;"
#elif defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
        // -- Check EXC_RETURN.DCRS (bit[5])
        "tst lr, #0x20;"
        "beq _get_callee_saved_regs;"
        // -- Check EXC_RETURN.ES (bit[0])
        "tst lr, #0x01;"
        "bne _check_fp_cntx;"
        "_get_callee_saved_regs:;"
        "add r3, #4*2;"
        "ldm r3!, {r4-r11};"
#endif
        "_check_fp_cntx:;"
        // Check EXC_RETURN.FType (bit[4])
        "tst lr, #0x10;"
        "ite eq;"
        // FPU context saved
        "moveq r1, #1;"
        // No FPU context
        "movne r1, #0;"
        // Make room for r0-r15,psr,special_regs(primask/faultmask/basepri/control)
        "sub sp, #4*18;"
        ".cfi_adjust_cfa_offset 4*18;"
        // Save r4-r11
        "add r0, sp, #4*4;"
        "stm r0, {r4-r11};"
        ".cfi_rel_offset r4, 4*4;"
        ".cfi_rel_offset r5, 4*5;"
        ".cfi_rel_offset r6, 4*6;"
        ".cfi_rel_offset r7, 4*7;"
        ".cfi_rel_offset r8, 4*8;"
        ".cfi_rel_offset r9, 4*9;"
        ".cfi_rel_offset r10, 4*10;"
        ".cfi_rel_offset r11, 4*11;"
        // Save r0-r3
        "ldm r3, {r4-r7};"
        "stm sp, {r4-r7};"
        ".cfi_rel_offset r0, 4*0;"
        ".cfi_rel_offset r1, 4*1;"
        ".cfi_rel_offset r2, 4*2;"
        ".cfi_rel_offset r3, 4*3;"
        // Save r12
        "ldr r0, [r3, #4*4];"
        "str r0, [sp, #4*12];"
        ".cfi_rel_offset r12, 4*12;"
        // Save sp
        "teq r1, 0;"
        "itt eq;"
        "addeq r0, r3, #4*8;"
        "beq _done_stack_frame;"
        "add r0, r3, #4*(8+18);"
        "_done_stack_frame:;"
        // -- Check RETPSR.SPREALIGN (bit[9])
        "ldr r4, [r3, #4*7];"
        "tst r4, #(1 << 9);"
        "it ne;"
        "addne r0, #4;"
        "str r0, [sp, #4*13];"
        // Save lr
        "ldr r0, [r3, #4*5];"
        "str r0, [sp, #4*14];"
        // Save pc
        "ldr r0, [r3, #4*6];"
        "str r0, [sp, #4*15];"
        // Save PSR
        "ldr r0, [r3, #4*7];"
        "str r0, [sp, #4*16];"
        // Save special_regs(primask/faultmask/basepri/control)
        "str r12, [sp, #4*17];"
        "_call_fault_handler:;"
        // Invoke the fault handler
        "mov r0, sp;"
        "mov r1, 0;"
        "mov r2, 0;"
        "ldr r3, =hal_trace_fault_dump;"
        "blx r3;"
        // Restore r4-r7
        "add r0, sp, #4*4;"
        "ldm r0, {r4-r7};"
        // Restore sp
        "add sp, #18*4;"
        ".cfi_adjust_cfa_offset -4*18;"
        "pop {r0, lr};"
        ".cfi_adjust_cfa_offset -4*2;"
        ".cfi_restore lr;"
        ".cfi_def_cfa_offset 0;"
        "bx lr;"
        ""
        );
}
#endif
#endif

//==============================================================================
// TRACE RX
//==============================================================================

#if (defined(DEBUG) || defined(REL_TRACE_ENABLE))

enum HAL_TRACE_RX_STATE_T {
    HAL_TRACE_RX_STATE_CLOSED = 0,
    HAL_TRACE_RX_STATE_OPENED,
    HAL_TRACE_RX_STATE_SLEEP,
};

static enum HAL_TRACE_RX_STATE_T trace_rx_state;
static uint8_t *trace_rx_buf;
static uint32_t trace_rx_len;
static HAL_TRACE_RX_CALLBACK_T trace_rx_cb;

static void hal_trace_rx_start(void)
{
    uint32_t desc_cnt = 1;
    union HAL_UART_IRQ_T mask;
    struct HAL_DMA_DESC_T dma_desc_rx;
    int ret;

    mask.reg = 0;
    mask.BE = 0;
    mask.FE = 0;
    mask.OE = 0;
    mask.PE = 0;
    mask.RT = 1;

    // TODO: Use stream mode
    ret = hal_uart_dma_recv_mask(trace_uart, trace_rx_buf, trace_rx_len, &dma_desc_rx, &desc_cnt, &mask);
    ASSERT(ret == 0, "%s: Failed to start dma rx: %d", __func__, ret);
}

static void hal_trace_rx_stop(void)
{
    union HAL_UART_IRQ_T mask;

    hal_uart_stop_dma_recv(trace_uart);

    mask.reg = 0;
    hal_uart_irq_set_mask(trace_uart, mask);
}

static void hal_trace_rx_irq_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    POSSIBLY_UNUSED int res = 0;

    if (trace_rx_cb == NULL) {
        return;
    }

    if (xfer_size) {
        res = trace_rx_cb(trace_rx_buf, xfer_size);
        //TRACE(0, "%s: trace_rx_cb (%p) prase data error: %d", __func__, trace_rx_cb, res);
    }
    if (xfer_size || status.RT) {
        if (trace_rx_state == HAL_TRACE_RX_STATE_OPENED) {
            hal_trace_rx_start();
        }
    }
}

int hal_trace_rx_open(unsigned char *buf, unsigned int len, HAL_TRACE_RX_CALLBACK_T rx_callback)
{
    if (buf == NULL || len == 0 || rx_callback == NULL) {
        return 1;
    }

    if (!hal_trace_is_uart_transport(trace_transport)) {
        return 2;
    }

#if (TRACE_BUF_SIZE <= 0)
    int ret;

    trace_uart = HAL_UART_ID_0 + (trace_transport - HAL_TRACE_TRANSPORT_UART0);
    ret = hal_uart_open(trace_uart, &uart_cfg);
    if (ret) {
        return ret;
    }
#endif

    trace_rx_buf = buf;
    trace_rx_len = len;
    trace_rx_cb = rx_callback;

    hal_uart_irq_set_dma_handler(trace_uart, hal_trace_rx_irq_handler, NULL);

    if (trace_rx_state != HAL_TRACE_RX_STATE_OPENED) {
        trace_rx_state = HAL_TRACE_RX_STATE_OPENED;
        hal_trace_rx_start();
    }

    return 0;
}

int hal_trace_rx_close(void)
{
    uint32_t lock;

    if (!hal_trace_is_uart_transport(trace_transport)) {
        return 2;
    }

    lock = int_lock();
    hal_trace_rx_stop();
    trace_rx_state = HAL_TRACE_RX_STATE_CLOSED;
    int_unlock(lock);

    trace_rx_buf = NULL;
    trace_rx_len = 0;
    trace_rx_cb = NULL;

#if (TRACE_BUF_SIZE <= 0)
    hal_uart_close(trace_uart);
#endif

    return 0;
}

int hal_trace_rx_sleep(void)
{
    uint32_t lock;

    if (trace_rx_state != HAL_TRACE_RX_STATE_OPENED) {
        return 1;
    }

    lock = int_lock();
    hal_trace_rx_stop();
    trace_rx_state = HAL_TRACE_RX_STATE_SLEEP;
    int_unlock(lock);

    return 0;
}

int hal_trace_rx_wakeup(void)
{
    if (trace_rx_state != HAL_TRACE_RX_STATE_SLEEP) {
        return 1;
    }

    if (trace_rx_buf == NULL || trace_rx_len == 0 || trace_rx_cb == NULL) {
        return 2;
    }

    trace_rx_state = HAL_TRACE_RX_STATE_OPENED;
    hal_trace_rx_start();

    return 0;
}

#endif

#ifdef TOTA_CRASH_DUMP_TOOL_ENABLE
int hal_trace_crash_dump_printf(uint32_t attr, const char *fmt, ...)
{
    int ret;
    va_list ap;
    char buf[120];
    int len = 0;
    enum TR_LEVEL_T level;
    enum TR_MODULE_T module;

    if (attr & TR_ATTR_IMM) {
        hal_trace_flush_buffer();
    }

    va_start(ap, fmt);

    level = GET_BITFIELD(attr, TR_ATTR_LEVEL);
    module = GET_BITFIELD(attr, TR_ATTR_MOD);

#ifdef CRASH_DUMP_ENABLE
    if (!in_crash_dump)
#endif
    {
        if (level > get_hal_trace_log_level()) {
            return 0;
        }
        if (level > TR_LEVEL_CRITICAL && (trace_mod_map[module >> 5] & (1 << (module & 0x1F))) == 0) {
            return 0;
        }
    }

    len = 0;
    if ((attr & TR_ATTR_NO_TS) == 0)
    {
        len += hal_trace_print_time(level, module, &buf[len], sizeof(buf) - len);
    }
    len += hal_trace_format_va(attr, &buf[len], sizeof(buf) - len, fmt, ap);
    ret = hal_trace_output((unsigned char *)buf, len);
    va_end(ap);

    if (attr & TR_ATTR_IMM) {
        hal_trace_flush_buffer();
    }

    return ret;
}
#endif

#if (TRACE_BUF_SIZE > 0)
#define UART_CRLF "\r\n"
#define UART_LFCR "\n\r"
#define UART_CR "\r"
#define UART_LF "\n"
#define UART_MAX_TEMP  128
int hal_trace_output_crlf(const unsigned char *buf, unsigned int len,bool block_flag)
{
    char p_str[UART_MAX_TEMP];
    char *p_data;
    uint32_t j;
    uint32_t str_len = len;
    if (buf == NULL)
    {
        return -1;
    }
    uint32_t lock;
    uint32_t avail = 0;
    if (block_flag)
    {
        lock = int_lock();
        if (p_trace->wptr >= p_trace->rptr) {
            avail = TRACE_BUF_SIZE - (p_trace->wptr - p_trace->rptr) - 1;
        } else {
            avail = (p_trace->rptr - p_trace->wptr) - 1;
        }
        int_unlock(lock);
        if (avail < (len+1024)) {
            hal_trace_flush_buffer();
        }
    }
    p_data = (char *)buf;
    j=0;
    for(int i =0;i<str_len;i++){
	    p_str[j++] = p_data[i];
	    if(p_data[i]=='\r' && p_data[i+1]!='\n'){
		    p_str[j]='\n';
            j++;
		    continue;
	    }
	    if(p_data[i]=='\n'){
		    p_str[j-1] = '\r';
		    p_str[j] = '\n';
            j++;
		    continue;
	    }
	    if(j<UART_MAX_TEMP)
	    {
		    continue;
	    }
        if(j>0){
            hal_trace_output((const unsigned char *)p_str,j);
            j=0;
        }
    }
    if(j>0){
        hal_trace_output((const unsigned char *)p_str,j);
        j = 0;
    }
    return len;
}
int hal_trace_output_block(const unsigned char *buf, unsigned int len)
{
    uint32_t lock;
    uint32_t avail = 0;
    lock = int_lock();
    if (p_trace->wptr >= p_trace->rptr) {
        avail = TRACE_BUF_SIZE - (p_trace->wptr - p_trace->rptr) - 1;
    } else {
        avail = (p_trace->rptr - p_trace->wptr) - 1;
    }
    int_unlock(lock);
    if (avail < (len+1024)) {
        hal_trace_flush_buffer();
    }

    return hal_trace_output((unsigned char *)buf, len);
}
#endif

#ifndef __ARM_ARCH_ISA_ARM
#define A7_TRACE_TAG "A7_TRACE:"
int _print_a7_flush = 0;
void hal_trace_print_a7_flush(int onoff)
{
    _print_a7_flush = onoff;
    TRACE(0, "%s onoff=%d", __FUNCTION__, onoff);
}

void hal_trace_print_a7(const unsigned char *buf, unsigned int buf_len)
{
    /* check platform trace on/off */
    if (get_hal_trace_onoff() == 1)
    {
        return;
    }

    /* output trace by local driver */
    if (buf && (buf_len > 0)) {
        if (_print_a7_flush == 0) // don't add A7_TRACE_TAG for A7 crashdump
            hal_trace_output((const unsigned char *)A7_TRACE_TAG, sizeof(A7_TRACE_TAG));
        hal_trace_output(buf, buf_len);
        if (_print_a7_flush) {
            hal_trace_flush_buffer();
        }
    }
}
#endif
