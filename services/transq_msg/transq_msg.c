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
#include "string.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cache.h"
#include "transq_msg.h"

#ifdef __ARM_ARCH_ISA_ARM
#ifdef RTOS
#define TRANSQ_MSG_RX_THREAD_MODE
#endif
#endif
#if !defined(CHIP_BEST2001_DSP) && !defined(CHIP_BEST2003_DSP)
#define TRANSQ_MSG_RX_THREAD_MODE
#endif

static TRANSQ_MSG_HANDLE_CB_T msg_rx_handler[TRANSQ_MSG_TYPE_NUM] = {NULL};

static TRANSQ_MSG_HANDLE_CB_T msg_tx_handler[TRANSQ_MSG_TYPE_NUM] = {NULL};

static enum HAL_TRANSQ_ID_T transq_msg_transq_id = HAL_TRANSQ_ID_1;

static int _onoff = 0;
void transq_msg_onoff(int onoff)
{
    _onoff = onoff;
}

void _transq_msg_flush(TRANSQ_MSG_T *msg, bool tx);
int transq_msg_tx(TRANSQ_MSG_T *msg)
{
    if (msg == NULL){
        return -1;
    }
    int _ret = 0;
    if (_onoff) {
        enum HAL_TRANSQ_RET_T ret;
        #if defined(NUTTX_BUILD)
        msg->pri = HAL_TRANSQ_PRI_HIGH;
        #endif
        //TRACE(0, "%s:msg:%p, pri=%d type=%u", __func__, msg, msg->pri, msg->type);
        _transq_msg_flush(msg, true);
        ret = hal_transq_tx(transq_msg_transq_id, msg->pri, (const uint8_t *)msg, sizeof(*msg));
        //ASSERT(ret == HAL_TRANSQ_RET_OK, "hal_transq_tx failed: pri=%d ret=%d", msg->pri, ret);
        if (ret == HAL_TRANSQ_RET_OK) {
            _ret = 1;
        } else
            TRACE(0, "hal_transq_tx failed: pri=%d ret=%d", msg->pri, ret);

    }
    return _ret;
}

static void transq_msg_tx_handler(enum HAL_TRANSQ_PRI_T pri, const uint8_t *data, uint32_t len)
{
    if (data == NULL){
        return;
    }
    //TRACE(0, "%s: pri=%d data=%p len=%u", __func__, pri, data, len);
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)data;
    //TRACE(0, "%s: pri=%d type=%u, handler:%p", __func__, msg->pri, msg->type, msg_tx_handler[msg->type]);
    if (msg_tx_handler[msg->type])
        msg_tx_handler[msg->type](msg);
}

void transq_msg_rx_process(TRANSQ_MSG_T *msg)
{
    if (msg == NULL || !_onoff){
        return;
    }
    _transq_msg_flush(msg, false);
    //TRACE(0, "%s: rx msg type:%d", __func__, msg->type);
    if (msg->type == TRANSQ_MSG_TYPE_TRACE) {
#ifndef __ARM_ARCH_ISA_ARM
        hal_trace_print_a7((const unsigned char *)msg->msg.trace.addr, msg->msg.trace.len);
#endif
    } else {
        if (msg_rx_handler[msg->type])
            msg_rx_handler[msg->type]((void *)msg);
    }
}

static void transq_msg_get_rx_data(enum HAL_TRANSQ_PRI_T pri)
{
    enum HAL_TRANSQ_RET_T ret;
    const uint8_t *msg;
    uint32_t len;

    ret = hal_transq_rx_first(transq_msg_transq_id, pri, &msg, &len);
    //ASSERT(ret == HAL_TRANSQ_RET_OK, "hal_transq_rx_first failed: pri=%d ret=%d", pri, ret);
    if (ret == HAL_TRANSQ_RET_RX_EMPTY) {
        TRACE(0, "%s %d  id:%d  rx empty: pri=%d", __func__, __LINE__, transq_msg_transq_id, pri);
        return;
    }

    while (ret == HAL_TRANSQ_RET_OK && msg) {
        //TRACE(0, "    rx: pri=%d data=%p len=%u", pri, msg, len);
        transq_msg_rx_process((TRANSQ_MSG_T *)msg);
        ret = hal_transq_rx_next(transq_msg_transq_id, pri, &msg, &len);
    }

    if (ret == HAL_TRANSQ_RET_RX_EMPTY) {
        //TRACE(0, "    rx empty: pri=%d", pri);
    } else {
        ASSERT(false, "    rx failed: pri=%d ret=%d", pri, ret);
    }
}
#ifdef TRANSQ_MSG_RX_THREAD_MODE
#ifdef RTOS
#define TRANSQ_MSG_THREAD_STACK_SIZE 0x4000
static osThreadId transq_msg_thread_tid;

static void transq_msg_thread(void const *argument);
osThreadDef(transq_msg_thread, osPriorityAboveNormal, 1, TRANSQ_MSG_THREAD_STACK_SIZE, "transq_msg");

osMutexId transq_msg_mutex_id = NULL;
osMutexDef(transq_msg_mutex);
#else // !RTOS

static volatile uint32_t transq_msg_flag_lock;

#endif // RTOS

void transq_msg_lock_thread(void)
{
    register uint32_t r14 asm("r14");
    uint32_t POSSIBLY_UNUSED lr = r14;
#ifdef RTOS
    osMutexWait(transq_msg_mutex_id, osWaitForever);
#else
    static uint32_t POSSIBLY_UNUSED locked_lr;
    ASSERT(transq_msg_flag_lock == 0, "transq_msg has been locked by %p. LR=%p", (void *)locked_lr, (void *)lr);
    transq_msg_flag_lock = 1;
    locked_lr = lr;
#endif
}

void transq_msg_unlock_thread(void)
{
    register uint32_t r14 asm("r14");
    uint32_t POSSIBLY_UNUSED lr = r14;
#ifdef RTOS
    osMutexRelease(transq_msg_mutex_id);
#else
    static uint32_t POSSIBLY_UNUSED unlocked_lr;
    ASSERT(transq_msg_flag_lock == 1, "transq_msg not locked before (lastly unlocked by %p). LR=%p", (void *)unlocked_lr, (void *)lr);
    transq_msg_flag_lock = 0;
    unlocked_lr = lr;
#endif
}

#define TRANS_MSG_RX_SIGNAL_PRI_HIGH   0x1
#define TRANS_MSG_RX_SIGNAL_PRI_NORMAL 0x2
#ifdef RTOS
static void transq_msg_thread(void const *argument)
{
    osEvent evt;
    uint32_t signals = 0;

    while(1)
    {
        //wait any signal
        evt = osSignalWait(0x0, osWaitForever);
        signals = evt.value.signals;
        //TRACE(0, "[%s] status = %x, signals = %d", __func__, evt.status, evt.value.signals);

        if(evt.status == osEventSignal)
        {
            if (signals & TRANS_MSG_RX_SIGNAL_PRI_HIGH)
                transq_msg_get_rx_data(HAL_TRANSQ_PRI_HIGH);
            if (signals & TRANS_MSG_RX_SIGNAL_PRI_NORMAL)
                transq_msg_get_rx_data(HAL_TRANSQ_PRI_NORMAL);
        }
        else
        {
            TRACE(0, "[%s] ERROR: evt.status = %d", __func__, evt.status);
            continue;
        }
    }
}

#else // !RTOS

#include "cmsis.h"
static volatile uint32_t transq_msg_flag_signal;

static void transq_msg_set_flag(volatile uint32_t *flag, uint32_t set)
{
    if (flag == NULL){
        return;
    }
    uint32_t lock;

    lock = int_lock();
    *flag |= set;
    int_unlock(lock);
}

static void transq_msg_clear_flag(volatile uint32_t *flag, uint32_t clear)
{
    if (flag == NULL){
        return;
    }
    uint32_t lock;

    lock = int_lock();
    *flag &= ~clear;
    int_unlock(lock);
}

static bool transq_msg_flag_active(volatile uint32_t *flag, uint32_t bits)
{
    if (flag == NULL){
        return false;
    }
    return !!(*flag & bits);
}

void transq_msg_thread(void const *argument)
{
    if (transq_msg_flag_active(&transq_msg_flag_signal, TRANS_MSG_RX_SIGNAL_PRI_HIGH)) {
        transq_msg_get_rx_data(HAL_TRANSQ_PRI_HIGH);
        transq_msg_clear_flag(&transq_msg_flag_signal, TRANS_MSG_RX_SIGNAL_PRI_HIGH);
    }
    if (transq_msg_flag_active(&transq_msg_flag_signal, TRANS_MSG_RX_SIGNAL_PRI_NORMAL)) {
        transq_msg_get_rx_data(HAL_TRANSQ_PRI_NORMAL);
        transq_msg_clear_flag(&transq_msg_flag_signal, TRANS_MSG_RX_SIGNAL_PRI_NORMAL);
    }
}
#endif // !RTOS
#endif /*TRANSQ_MSG_RX_THREAD_MODE*/

static void transq_msg_rx_handler(enum HAL_TRANSQ_PRI_T pri)
{
    //TRACE(0, "%s: pri=%d", __func__, pri);
#ifndef TRANSQ_MSG_RX_THREAD_MODE
    transq_msg_get_rx_data(pri);
#else /*TRANSQ_MSG_RX_THREAD_MODE*/
    int32_t signals;
    if (pri == HAL_TRANSQ_PRI_NORMAL)
        signals = TRANS_MSG_RX_SIGNAL_PRI_NORMAL;
    else
        signals = TRANS_MSG_RX_SIGNAL_PRI_HIGH;
#ifdef RTOS
    osSignalSet(transq_msg_thread_tid, signals);
#else
    transq_msg_set_flag(&transq_msg_flag_signal, signals);
#endif
#endif /*TRANSQ_MSG_RX_THREAD_MODE*/
}

void bes_transq_msg_rx_handler(enum HAL_TRANSQ_PRI_T pri)
{
    transq_msg_get_rx_data(pri);
}
void bes_transq_msg_tx_handler(enum HAL_TRANSQ_PRI_T pri, const uint8_t *data, uint32_t len)
{
    transq_msg_tx_handler(pri,data,len);
}

void transq_msg_register(TRANSQ_MSG_TYPE_T type, TRANSQ_MSG_HANDLE_CB_T func, bool tx)
{
    ASSERT(type < TRANSQ_MSG_TYPE_NUM, "msg_type :%d error", type);
    if (tx)
        msg_tx_handler[type] = func;
    else
        msg_rx_handler[type] = func;
}

void transq_msg_flush_data(bool tx, void *addr, uint32_t len)
{
    uint32_t start = (uint32_t)addr;
    uint32_t end = start + len;

    if (!addr || len == 0)
        return;

    if (!((start >= DSP_RAM_BASE) && (end < DSP_RAM_BASE+MAX_DSP_RAM_SIZE)) &&
         !((start >= RAM_BASE) && (end < RAM_BASE+RAM_TOTAL_SIZE))) {
    #if defined(__ARM_ARCH_ISA_ARM)
        L1C_CleanInvalidateCacheRange(tx, start, end);
    #else
        if (tx)
            hal_cache_sync(HAL_CACHE_ID_D_CACHE, start, len);
        else
            hal_cache_invalidate(HAL_CACHE_ID_D_CACHE, start, len);
    #endif
    }
}

void _transq_msg_flush(TRANSQ_MSG_T *msg, bool tx)
{
    if (msg->user_data_len)
        transq_msg_flush_data(tx, msg->user_data, msg->user_data_len);
    switch (msg->type) {
        case TRANSQ_MSG_TYPE_TRACE:
            transq_msg_flush_data(tx, (void *)msg->msg.trace.addr, msg->msg.trace.len);
            break;
        case TRANSQ_MSG_TYPE_RECORD_DATA:
        case TRANSQ_MSG_TYPE_AF_CAPTURE_IRQ:
            transq_msg_flush_data(tx, msg->msg.stream_buf.buf, msg->msg.stream_buf.len);
            break;
        default:
            break;
    }
    transq_msg_flush_data(tx, msg, sizeof(TRANSQ_MSG_T));
}

int transq_msg_flush()
{
    hal_transq_flush(transq_msg_transq_id);
    osDelay(100); //wait for tx_done irq
    return 0;
}

int transq_msg_init()
{
    static int init_state = 0;
    enum HAL_TRANSQ_RET_T ret;
    struct HAL_TRANSQ_CFG_T cfg;

    if (init_state) {
        return 0;
    }

    init_state = 1;

#ifdef RTOS
    transq_msg_thread_tid = osThreadCreate(osThread(transq_msg_thread), NULL);
    osThreadSetPriority(transq_msg_thread_tid, osPriorityHigh);
    osSignalSet(transq_msg_thread_tid, 0x0);
#endif
#if !defined(CONFIG_RPTUN)
    memset(&cfg, 0, sizeof(cfg));
    cfg.slot.rx_num[HAL_TRANSQ_PRI_HIGH] = 16;
    cfg.slot.rx_num[HAL_TRANSQ_PRI_NORMAL] = 16;

    cfg.slot.tx_num[HAL_TRANSQ_PRI_HIGH] = 16;
    cfg.slot.tx_num[HAL_TRANSQ_PRI_NORMAL] = 16;

    cfg.rx_handler = transq_msg_rx_handler;
    cfg.tx_handler = transq_msg_tx_handler;
    ret = hal_transq_open(transq_msg_transq_id, &cfg);
    ASSERT(ret == HAL_TRANSQ_RET_OK, "hal_transq_open failed: %d", ret);
#endif
    return 0;
}

int transq_msg_reinit()
{
#if !defined(CONFIG_RPTUN)
    enum HAL_TRANSQ_RET_T ret;
    struct HAL_TRANSQ_CFG_T cfg;

    hal_transq_close(transq_msg_transq_id);

    memset(&cfg, 0, sizeof(cfg));
    cfg.slot.rx_num[HAL_TRANSQ_PRI_HIGH] = 16;
    cfg.slot.rx_num[HAL_TRANSQ_PRI_NORMAL] = 16;

    cfg.slot.tx_num[HAL_TRANSQ_PRI_HIGH] = 16;
    cfg.slot.tx_num[HAL_TRANSQ_PRI_NORMAL] = 16;

    cfg.rx_handler = transq_msg_rx_handler;
    cfg.tx_handler = transq_msg_tx_handler;
    ret = hal_transq_open(transq_msg_transq_id, &cfg);
    ASSERT(ret == HAL_TRANSQ_RET_OK, "hal_transq_open failed: %d", ret);
#endif
    return 0;
}
