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
#ifdef CHIP_HAS_CP

#include "plat_addr_map.h"
#include "cmsis_nvic.h"
#include "hal_location.h"
#include "hal_mcu2cp.h"
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_cache.h"

#include CHIP_SPECIFIC_HDR(reg_cmu)

#define MAX_SEND_RECORD_COUNT               32

#define HAL_SYS_WAKE_LOCK_USER_MCU2CP       HAL_SYS_WAKE_LOCK_USER_4

#ifdef CP_API
#define API_POSTFIX                         _cp
#define MCU2CP_TEXT_LOC                     CP_TEXT_SRAM_LOC
#define MCU2CP_RODATA_LOC                   CP_DATA_LOC
#define MCU2CP_BSS_LOC                      CP_BSS_LOC
#define MCU2CP_BSS_DEF                      CP_BSS_DEF
#else
#define API_POSTFIX                         _mcu
#define MCU2CP_TEXT_LOC
#define MCU2CP_RODATA_LOC
#define MCU2CP_BSS_LOC
#define MCU2CP_BSS_DEF                      CP_BSS_DEF
#endif

#define MCU2CP_API_A(n, p)                  MCU2CP_TEXT_LOC n ## p
#define MCU2CP_API_B(n, p)                  MCU2CP_API_A(n, p)
#define MCU2CP_API(n)                       MCU2CP_API_B(n, API_POSTFIX)

enum HAL_MCU2CP_IRQ_TYPE_T {
    HAL_MCU2CP_IRQ_SEND_IND,
    HAL_MCU2CP_IRQ_RECV_DONE,

    HAL_MCU2CP_IRQ_TYPE_QTY
};

struct HAL_MCU2CP_MSG_T {
    struct HAL_MCU2CP_MSG_T *next;          // pointer to next element in the list
    unsigned int len;                       // message data length in bytes
    const unsigned char *data;              // pointer to the message data
};

struct HAL_MCU2CP_SEND_RECORD_T {
    struct HAL_MCU2CP_MSG_T msg;
    bool in_use;
};

static const IRQn_Type MCU2CP_RODATA_LOC rx_irq_id[HAL_MCU2CP_ID_QTY] = {
    CP2MCU_DATA_IRQn,
    CP2MCU_DATA1_IRQn,
};

static const IRQn_Type MCU2CP_RODATA_LOC tx_irq_id[HAL_MCU2CP_ID_QTY] = {
    MCU2CP_DONE_IRQn,
    MCU2CP_DONE1_IRQn,
};

static const struct HAL_MCU2CP_MSG_T ** MCU2CP_BSS_LOC recv_msg_list_p;

static struct HAL_MCU2CP_MSG_T * MCU2CP_BSS_LOC send_msg_list_p[HAL_MCU2CP_ID_QTY];
static struct HAL_MCU2CP_MSG_T * MCU2CP_BSS_LOC send_pending_list_p[HAL_MCU2CP_ID_QTY];

static struct HAL_MCU2CP_MSG_T MCU2CP_BSS_LOC recv_pending_head[HAL_MCU2CP_ID_QTY];

static struct HAL_MCU2CP_SEND_RECORD_T MCU2CP_BSS_LOC send_msgs[HAL_MCU2CP_ID_QTY][MAX_SEND_RECORD_COUNT];

static HAL_MCU2CP_RX_IRQ_HANDLER MCU2CP_BSS_LOC rx_irq_handler[HAL_MCU2CP_ID_QTY];
static HAL_MCU2CP_TX_IRQ_HANDLER MCU2CP_BSS_LOC tx_irq_handler[HAL_MCU2CP_ID_QTY];

static bool MCU2CP_BSS_LOC chan_opened[HAL_MCU2CP_ID_QTY];

static bool MCU2CP_BSS_LOC need_flow_ctrl[HAL_MCU2CP_ID_QTY] = { false, false, };

#if !defined(CONFIG_SMP)
#ifndef CP_API
static bool MCU2CP_BSS_LOC chan_busy[HAL_MCU2CP_ID_QTY] = { false, false, };

static bool MCU2CP_BSS_LOC busy_now = false;
#endif
#endif

static struct CMU_T * const MCU2CP_RODATA_LOC cmu = (struct CMU_T *)CMU_BASE;
uint32_t int_lock_smp_com();
void int_unlock_smp_com(uint32_t flags);

static void MCU2CP_TEXT_LOC hal_mcu2cp_busy(enum HAL_MCU2CP_ID_T id, bool busy)
{
#if !defined(CONFIG_SMP)
#ifndef CP_API
    int i;
    bool new_state;

    if (chan_busy[id] == busy) {
        return;
    }

    chan_busy[id] = busy;

    if (busy_now == busy) {
        return;
    }

    if (busy) {
        hal_sys_wake_lock(HAL_SYS_WAKE_LOCK_USER_MCU2CP);
        busy_now = true;
    } else {
        new_state = false;
        for (i = 0; i < HAL_MCU2CP_ID_QTY; i++) {
            if (chan_busy[i]) {
                new_state = true;
                break;
            }
        }
        if (!new_state) {
            hal_sys_wake_unlock(HAL_SYS_WAKE_LOCK_USER_MCU2CP);
            busy_now = false;
        }
    }
#endif
#endif
}

static int MCU2CP_TEXT_LOC hal_mcu2cp_peer_irq_set(enum HAL_MCU2CP_ID_T id, enum HAL_MCU2CP_IRQ_TYPE_T type)
{
    uint32_t value;
#if defined(CONFIG_BES_DUALCORE_AMP)
#if (defined(CHIP_HAS_PSRAM) && defined(PSRAM_ENABLE)) || (defined(CHIP_HAS_PSRAMUHS) && defined(PSRAMUHS_ENABLE))
    hal_cache_sync(HAL_CACHE_ID_D_CACHE);
#endif
#endif

#ifdef CP_API
    if (id == HAL_MCU2CP_ID_0) {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_CP2MCU_DATA_IND_SET;
        } else {
            value = CMU_MCU2CP_DATA_DONE_SET;
        }
    } else {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_CP2MCU_DATA1_IND_SET;
        } else {
            value = CMU_MCU2CP_DATA1_DONE_SET;
        }
    }

    cmu->CP2MCU_IRQ_SET = value;
#else
    if (id == HAL_MCU2CP_ID_0) {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_MCU2CP_DATA_IND_SET;
        } else {
            value = CMU_CP2MCU_DATA_DONE_SET;
        }
    } else {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_MCU2CP_DATA1_IND_SET;
        } else {
            value = CMU_CP2MCU_DATA1_DONE_SET;
        }
    }

    cmu->MCU2CP_IRQ_SET = value;
#endif
    return 0;
}

static int MCU2CP_TEXT_LOC hal_mcu2cp_local_irq_clear(enum HAL_MCU2CP_ID_T id, enum HAL_MCU2CP_IRQ_TYPE_T type)
{
    uint32_t value;

#ifdef CP_API
    if (id == HAL_MCU2CP_ID_0) {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_MCU2CP_DATA_IND_CLR;
        } else {
            value = CMU_CP2MCU_DATA_DONE_CLR;
        }
    } else {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_MCU2CP_DATA1_IND_CLR;
        } else {
            value = CMU_CP2MCU_DATA1_DONE_CLR;
        }
    }

    cmu->MCU2CP_IRQ_CLR = value;
#else
    if (id == HAL_MCU2CP_ID_0) {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_CP2MCU_DATA_IND_CLR;
        } else {
            value = CMU_MCU2CP_DATA_DONE_CLR;
        }
    } else {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_CP2MCU_DATA1_IND_CLR;
        } else {
            value = CMU_MCU2CP_DATA1_DONE_CLR;
        }
    }

    cmu->CP2MCU_IRQ_CLR = value;
#endif

    return 0;
}

static int MCU2CP_TEXT_LOC hal_mcu2cp_local_irq_set(enum HAL_MCU2CP_ID_T id, enum HAL_MCU2CP_IRQ_TYPE_T type)
{
    uint32_t value;

#ifdef CP_API
    if (id == HAL_MCU2CP_ID_0) {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_MCU2CP_DATA_IND_SET;
        } else {
            value = CMU_CP2MCU_DATA_DONE_SET;
        }
    } else {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_MCU2CP_DATA1_IND_SET;
        } else {
            value = CMU_CP2MCU_DATA1_DONE_SET;
        }
    }

    cmu->MCU2CP_IRQ_SET = value;
#else
    if (id == HAL_MCU2CP_ID_0) {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_CP2MCU_DATA_IND_SET;
        } else {
            value = CMU_MCU2CP_DATA_DONE_SET;
        }
    } else {
        if (type == HAL_MCU2CP_IRQ_SEND_IND) {
            value = CMU_CP2MCU_DATA1_IND_SET;
        } else {
            value = CMU_MCU2CP_DATA1_DONE_SET;
        }
    }

    cmu->CP2MCU_IRQ_SET = value;
#endif

    return 0;
}

static void MCU2CP_TEXT_LOC hal_mcu2cp_rx_irq(void)
{
    int id;
    const struct HAL_MCU2CP_MSG_T *msg_ptr;
    unsigned int processed;
    uint32_t lock = int_lock_smp_com();

    for (id = HAL_MCU2CP_ID_0; id < HAL_MCU2CP_ID_QTY; id++) {
        if (NVIC_GetActive(rx_irq_id[id])) {
            hal_mcu2cp_local_irq_clear(id, HAL_MCU2CP_IRQ_SEND_IND);

            if (recv_pending_head[id].data) {
                // Previous unprocessed message
                msg_ptr = &recv_pending_head[id];
            } else {
                // New message
                msg_ptr = recv_msg_list_p[id];
            }
            while (msg_ptr) {
                if (rx_irq_handler[id]) {
                    int_unlock_smp_com(lock);
                    processed = rx_irq_handler[id](msg_ptr->data, msg_ptr->len);
                    lock = int_lock_smp_com();
                    // Check if flow control needed
                    if (processed < msg_ptr->len) {
                        recv_pending_head[id].next = msg_ptr->next;
                        recv_pending_head[id].len = msg_ptr->len - processed;
                        recv_pending_head[id].data = msg_ptr->data + processed;
                        break;
                    }
                } else {
                    // Error
                    ASSERT(false, "MCU2CP-RX: Handler missing");
                    break;
                }
                msg_ptr = msg_ptr->next;
            }

            if (msg_ptr == NULL) {
                if (!need_flow_ctrl[id]){
                    hal_mcu2cp_peer_irq_set(id, HAL_MCU2CP_IRQ_RECV_DONE);
                }
                recv_pending_head[id].data = NULL;
            }
        }
    }
    int_unlock_smp_com(lock);
}

#if !defined(CONFIG_SMP)
static void MCU2CP_TEXT_LOC hal_mcu2cp_tx_irq(void)
{
    int id;
    struct HAL_MCU2CP_MSG_T *msg_ptr;
    uint32_t lock = int_lock_smp_com();

    for (id = HAL_MCU2CP_ID_0; id < HAL_MCU2CP_ID_QTY; id++) {
        if (NVIC_GetActive(tx_irq_id[id])) {
            hal_mcu2cp_local_irq_clear(id, HAL_MCU2CP_IRQ_RECV_DONE);

            msg_ptr = send_msg_list_p[id];
            while (msg_ptr) {
                if (tx_irq_handler[id]) {
                    int_unlock_smp_com(lock);
                    tx_irq_handler[id](msg_ptr->data, msg_ptr->len);
                    lock = int_lock_smp_com();
                };
                CONTAINER_OF(msg_ptr, struct HAL_MCU2CP_SEND_RECORD_T, msg)->in_use = false;
                msg_ptr = msg_ptr->next;
            }

            if (send_pending_list_p[id]) {
                send_msg_list_p[id] = send_pending_list_p[id];
                send_pending_list_p[id] = NULL;
                hal_mcu2cp_peer_irq_set(id, HAL_MCU2CP_IRQ_SEND_IND);
            } else {
                send_msg_list_p[id] = NULL;
                // Allow sleep
                hal_mcu2cp_busy(id, false);
            }
        }
    }
    int_unlock_smp_com(lock);
}
#endif
const struct HAL_MCU2CP_MSG_T ** hal_mcu2cp_get_send_msg_list_mcu(void);
const struct HAL_MCU2CP_MSG_T ** hal_mcu2cp_get_send_msg_list_cp (void);

#if defined(CP_BOOT) || defined(CP_BUILD)
#ifdef CP_API
// This is initialization code and should NOT be in CP text location
const struct HAL_MCU2CP_MSG_T ** hal_mcu2cp_get_send_msg_list_cp(void)
{
    return (const struct HAL_MCU2CP_MSG_T **)hal_cmu_dbg_get_val(1);
}
#else
const struct HAL_MCU2CP_MSG_T ** hal_mcu2cp_get_send_msg_list_mcu(void)
{
    return (const struct HAL_MCU2CP_MSG_T **)hal_cmu_dbg_get_val(0);
}
#endif
#else
#ifdef CP_API
// This is initialization code and should NOT be in CP text location
const struct HAL_MCU2CP_MSG_T ** hal_mcu2cp_get_send_msg_list_cp(void)
#else
const struct HAL_MCU2CP_MSG_T ** hal_mcu2cp_get_send_msg_list_mcu(void)
#endif
{
    return (const struct HAL_MCU2CP_MSG_T **)&send_msg_list_p[0];
}
#endif

int MCU2CP_API(hal_mcu2cp_open)(enum HAL_MCU2CP_ID_T id,
                                HAL_MCU2CP_RX_IRQ_HANDLER rxhandler, HAL_MCU2CP_TX_IRQ_HANDLER txhandler, bool rx_flowctrl)
{
    int i;

    if (id >= HAL_MCU2CP_ID_QTY) {
        return 1;
    }
    uint32_t lock = int_lock_smp_com();
    if (chan_opened[id]) {
        ASSERT(need_flow_ctrl[id] == rx_flowctrl, "MCU2CP-OPEN: rx_flowctrl=%d (should be %d)", rx_flowctrl, need_flow_ctrl[id]);
        int_unlock_smp_com(lock);
        return 3;
    } else {
        hal_mcu2cp_local_irq_clear(id, HAL_MCU2CP_IRQ_SEND_IND);
        hal_mcu2cp_local_irq_clear(id, HAL_MCU2CP_IRQ_RECV_DONE);
#ifdef CP_API
        recv_msg_list_p = hal_mcu2cp_get_send_msg_list_mcu();
#else
        recv_msg_list_p = hal_mcu2cp_get_send_msg_list_cp();
#endif

        NVIC_SetVector(rx_irq_id[id], (uint32_t)hal_mcu2cp_rx_irq);
        NVIC_SetPriority(rx_irq_id[id], IRQ_PRIORITY_NORMAL);
#if !defined(CONFIG_SMP)
        NVIC_SetVector(tx_irq_id[id], (uint32_t)hal_mcu2cp_tx_irq);
        NVIC_SetPriority(tx_irq_id[id], IRQ_PRIORITY_NORMAL);
#endif
        // Stop IRQs by default
        NVIC_DisableIRQ(rx_irq_id[id]);
        NVIC_DisableIRQ(tx_irq_id[id]);

        send_msg_list_p[id] = NULL;
        send_pending_list_p[id] = NULL;
        recv_pending_head[id].data = NULL;
        for (i = 0; i < MAX_SEND_RECORD_COUNT; i++) {
            send_msgs[id][i].in_use = false;
        }
        need_flow_ctrl[id] = rx_flowctrl;
    }
    chan_opened[id] = true;

    rx_irq_handler[id] = rxhandler;
    tx_irq_handler[id] = txhandler;
    int_unlock_smp_com(lock);
    return 0;
}

int MCU2CP_API(hal_mcu2cp_close)(enum HAL_MCU2CP_ID_T id)
{
    if (id >= HAL_MCU2CP_ID_QTY) {
        return 1;
    }
    uint32_t lock = int_lock_smp_com();

    chan_opened[id] = false;
    rx_irq_handler[id] = NULL;
    tx_irq_handler[id] = NULL;

    // Stop IRQs by default
    NVIC_DisableIRQ(rx_irq_id[id]);
    NVIC_DisableIRQ(tx_irq_id[id]);

    send_msg_list_p[id] = NULL;
    send_pending_list_p[id] = NULL;
    recv_pending_head[id].data = NULL;
    need_flow_ctrl[id] = false;

    int_unlock_smp_com(lock);

    return 0;
}

int MCU2CP_API(hal_mcu2cp_start_recv)(enum HAL_MCU2CP_ID_T id)
{
    if (id >= HAL_MCU2CP_ID_QTY) {
        return 1;
    }
    uint32_t lock = int_lock_smp_com();
    NVIC_EnableIRQ(rx_irq_id[id]);
    // Check if there is any previous unprocessed message
    if (recv_pending_head[id].data) {
        hal_mcu2cp_local_irq_set(id, HAL_MCU2CP_IRQ_SEND_IND);
    }
    int_unlock_smp_com(lock);
    return 0;
}

int MCU2CP_API(hal_mcu2cp_stop_recv)(enum HAL_MCU2CP_ID_T id)
{
    if (id >= HAL_MCU2CP_ID_QTY) {
        return 1;
    }
    uint32_t lock = int_lock_smp_com();
    NVIC_DisableIRQ(rx_irq_id[id]);
    int_unlock_smp_com(lock);
    return 0;
}

int MCU2CP_API(hal_mcu2cp_send)(enum HAL_MCU2CP_ID_T id,
                                const unsigned char *data, unsigned int len)
{
    int ret;
    struct HAL_MCU2CP_SEND_RECORD_T *record;
    struct HAL_MCU2CP_MSG_T *next;
    int i;
    if (id >= HAL_MCU2CP_ID_QTY) {
        return 1;
    }
    if (!chan_opened[id]) {
        return 3;
    }
    uint32_t lock = int_lock_smp_com();

    NVIC_EnableIRQ(tx_irq_id[id]);

    ret = -1;
    record = &send_msgs[id][0];

    for (i = 0; i < MAX_SEND_RECORD_COUNT; i++) {
#if !defined(CONFIG_SMP)
        if (record->in_use) {
            record++;
            continue;
        }
#endif
        record->in_use = true;
        record->msg.next = NULL;
        record->msg.len = len;
        record->msg.data = data;
        ret = 1;
#if !defined(CONFIG_SMP)
        if (send_msg_list_p[id] == NULL) {
#else
        if (1){
#endif
            send_msg_list_p[id] = &record->msg;
            hal_mcu2cp_peer_irq_set(id, HAL_MCU2CP_IRQ_SEND_IND);
            ret = 0;
        } else if (send_pending_list_p[id] == NULL) {
            send_pending_list_p[id] = &record->msg;
            // TRACE(1,"hal_mcu2cp_send add to pendinglist send_msg_list_p[id].data=0x%x",(uint32_t)send_msg_list_p[id]->data);
        } else {
            next = send_pending_list_p[id];
            while (next->next) {
                next = next->next;
            }
            next->next = &record->msg;
            // TRACE(1,"hal_mcu2cp_send add to pendinglist 2 send_msg_list_p[id].data=0x%x",(uint32_t)send_msg_list_p[id]->data);
        }
        // Prohibit sleep here
        hal_mcu2cp_busy(id, true);
        break;
    }
    int_unlock_smp_com(lock);

    return ret;
}

void MCU2CP_API(hal_mcu2cp_rx_done)(enum HAL_MCU2CP_ID_T id)
{
    hal_mcu2cp_peer_irq_set(id, HAL_MCU2CP_IRQ_RECV_DONE);
}

int MCU2CP_API(hal_mcu2cp_opened)(enum HAL_MCU2CP_ID_T id)
{
    return chan_opened[id];
}

int MCU2CP_API(hal_mcu2cp_local_irq_pending)(enum HAL_MCU2CP_ID_T id)
{
    uint32_t value;

#ifdef CP_API
    if (id == HAL_MCU2CP_ID_0) {
        value = CMU_MCU2CP_DATA_IND_SET | CMU_CP2MCU_DATA_DONE_SET;
    } else {
        value = CMU_MCU2CP_DATA1_IND_SET | CMU_CP2MCU_DATA1_DONE_SET;
    }

    return !!(cmu->MCU2CP_IRQ_SET & value);
#else
    if (id == HAL_MCU2CP_ID_0) {
        value = CMU_CP2MCU_DATA_IND_SET | CMU_MCU2CP_DATA_DONE_SET;
    } else {
        value = CMU_CP2MCU_DATA1_IND_SET | CMU_MCU2CP_DATA1_DONE_SET;
    }

    return !!(cmu->CP2MCU_IRQ_SET & value);
#endif
}

void MCU2CP_API(hal_mcu2cp_irq_ack)(enum HAL_MCU2CP_ID_T id)
{
    hal_mcu2cp_local_irq_clear(id, HAL_MCU2CP_IRQ_RECV_DONE);
}

#if defined(CP_BOOT) || defined(CP_BUILD)
void MCU2CP_API(hal_mcu2cp_set_send_msg_list)(void)
{
#ifdef CP_API
    hal_cmu_dbg_set_val(1, (uint32_t)&send_msg_list_p[0]);
#else
    hal_cmu_dbg_set_val(0, (uint32_t)&send_msg_list_p[0]);
#endif
}
#endif

#endif

