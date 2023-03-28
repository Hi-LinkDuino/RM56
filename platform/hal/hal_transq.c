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
#ifdef CHIP_HAS_TRANSQ
#include "plat_addr_map.h"
#include "reg_transq.h"
#include "hal_transq.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_sleep.h"
#include "hal_cache.h"
#include "hal_chipid.h"
#include "stdbool.h"
#include "cmsis_nvic.h"

#ifndef TRANSQ0_SLOT_NUM
#define TRANSQ0_SLOT_NUM 32
#endif

#if (CHIP_HAS_TRANSQ > 1)
#ifndef TRANSQ1_SLOT_NUM
#define TRANSQ1_SLOT_NUM 32
#endif
#endif

#define READ_REG(b,a) \
     (*(volatile uint32_t *)(b+a))

#define WRITE_REG(v,b,a) \
     ((*(volatile uint32_t *)(b+a)) = v)

#define REG(a)          *(volatile uint32_t *)(a)

#define U64_STR "0x%08X%08X"
#define U64_2_STR(a) (uint32_t)(a >> 32), (uint32_t)a

#if defined(__ARM_ARCH_ISA_ARM)
void WEAK transq_lock_init(void)
{
    return;
}
uint32_t WEAK transq_lock(void)
{
    return int_lock();
}
void WEAK transq_unlock(uint32_t cpsr)
{
    int_unlock(cpsr);
}
#endif

// BITMAP:
// [High Priority Slots] ...... [Normal Priority Slots]
// 31 30 29 28 27 26 25  ...... 10 9 8 7 6 5 4 3 2 1 0

static struct TRANSQ_T * const transq[HAL_TRANSQ_ID_QTY] = {
    (struct TRANSQ_T *)TRANSQ0_BASE,
#if (CHIP_HAS_TRANSQ > 1)
    (struct TRANSQ_T *)TRANSQ1_BASE,
#endif
};

static struct TRANSQ_T * const peer_transq[HAL_TRANSQ_ID_QTY] = {
    (struct TRANSQ_T *)TRANSQ0_PEER_BASE,
#if (CHIP_HAS_TRANSQ > 1)
    (struct TRANSQ_T *)TRANSQ1_PEER_BASE,
#endif
};

static const enum HAL_CMU_MOD_ID_T transq_mod[HAL_TRANSQ_ID_QTY] = {
    HAL_CMU_MOD_P_TRANSQ0,
#if (CHIP_HAS_TRANSQ > 1)
    HAL_CMU_MOD_P_TRANSQ1,
#endif
};

static const IRQn_Type remote_irq_num[HAL_TRANSQ_ID_QTY] = {
    TRANSQ0_RMT_IRQn,
#if (CHIP_HAS_TRANSQ > 1)
    TRANSQ1_RMT_IRQn,
#endif
};

static const IRQn_Type local_irq_num[HAL_TRANSQ_ID_QTY] = {
    TRANSQ0_LCL_IRQn,
#if (CHIP_HAS_TRANSQ > 1)
    TRANSQ1_LCL_IRQn,
#endif
};

uint8_t transq_slot_num[HAL_TRANSQ_ID_QTY] = {
    TRANSQ0_SLOT_NUM,
#if (CHIP_HAS_TRANSQ > 1)
    TRANSQ1_SLOT_NUM,
#endif
};

static struct HAL_TRANSQ_CFG_T transq_cfg[HAL_TRANSQ_ID_QTY];

static uint8_t next_tx_slot[HAL_TRANSQ_ID_QTY][HAL_TRANSQ_PRI_QTY];

static uint8_t active_tx_slot[HAL_TRANSQ_ID_QTY][HAL_TRANSQ_PRI_QTY];

static bool tx_slot_full[HAL_TRANSQ_ID_QTY][HAL_TRANSQ_PRI_QTY];

static uint8_t next_rx_slot[HAL_TRANSQ_ID_QTY][HAL_TRANSQ_PRI_QTY];

static uint64_t rx_irq_mask[HAL_TRANSQ_ID_QTY][HAL_TRANSQ_PRI_QTY];
static uint64_t tx_irq_mask[HAL_TRANSQ_ID_QTY][HAL_TRANSQ_PRI_QTY];

static uint32_t TxOutstanding = 0;

static uint64_t construct_mask(uint32_t lsb, uint32_t width)
{
    uint32_t i;
    uint64_t result;

    if (lsb >= 32 || width == 0) {
        return 0;
    }

    result = 0;
    for (i = lsb; i < lsb + width; i++) {
        result |= ((uint64_t)1 << i);
    }

    return result;
}

static uint32_t get_next_rx_slot(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri, uint32_t slot)
{
    slot++;
    if (pri == HAL_TRANSQ_PRI_HIGH) {
        if (slot >= transq_slot_num[id]) {
            slot =  transq_slot_num[id] - transq_cfg[id].slot.rx_num[pri];
        }
    } else {
        if (slot >= transq_cfg[id].slot.rx_num[pri]) {
            slot = 0;
        }
    }

    return slot;
}

static uint32_t get_next_tx_slot(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri, uint32_t slot)
{
    slot++;
    if (pri == HAL_TRANSQ_PRI_HIGH) {
        if (slot >=  transq_slot_num[id]) {
            slot =  transq_slot_num[id] - transq_cfg[id].slot.tx_num[pri];
        }
    } else {
        if (slot >= transq_cfg[id].slot.tx_num[pri]) {
            slot = 0;
        }
    }

    return slot;
}

#ifdef __ARM_ARCH_ISA_ARM
#ifdef RTOS
static void hal_transq_remote_irq_handler(int irq_num, void * irq_data)
#else
static void hal_transq_remote_irq_handler(void)
#endif
#else
static void hal_transq_remote_irq_handler(void)
#endif
{
    enum HAL_TRANSQ_ID_T id;
    enum HAL_TRANSQ_PRI_T pri = HAL_TRANSQ_PRI_NORMAL;
    uint64_t status;
    uint32_t mask;
    uint32_t slot;
    uint32_t lock;

#if (CHIP_HAS_TRANSQ > 1)
#ifdef __ARM_ARCH_ISA_ARM
#ifdef RTOS
#ifdef KERNEL_LITEOS_A
    IRQn_Type irq = NVIC_GetCurrentActiveIRQ();
#else
    IRQn_Type irq = (IRQn_Type)irq_num;
#endif
#else
    IRQn_Type irq = NVIC_GetCurrentActiveIRQ();
#endif
#else
    IRQn_Type irq = NVIC_GetCurrentActiveIRQ();
#endif

    for (id = HAL_TRANSQ_ID_0; id < HAL_TRANSQ_ID_QTY; id++) {
        if (irq == remote_irq_num[id]) {
            break;
        }
    }
    if (id >= HAL_TRANSQ_ID_QTY) {
        return;
    }
#else
    id = HAL_TRANSQ_ID_0;
#endif

    while (1) {
        status = 0;
        if (transq_slot_num[id] > 32) {
            status = peer_transq[id]->RMT_MIS_E;
            status = status << 32;
        }
        status |= peer_transq[id]->RMT_MIS;

        if (status == 0)
            break;

#if defined(__ARM_ARCH_ISA_ARM)
        lock = transq_lock();
#else
        lock = int_lock();
#endif

        if (status & rx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]) {
            pri = HAL_TRANSQ_PRI_HIGH;
        } else if (status & rx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL]) {
            pri = HAL_TRANSQ_PRI_NORMAL;
        } else {
            ASSERT(false, "TRANSQ-%d: Corrupted rx mask: status=" U64_STR " rx_mask=" U64_STR " / " U64_STR "",
                id, U64_2_STR(status), U64_2_STR(rx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]), U64_2_STR(rx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL]));
        }
        slot = next_rx_slot[id][pri];

#if !(defined(__MCU_FW_2002__) && defined(CONFIG_SDIO_SUPPORT) && !(defined(_WIFI_RF_TEST_MODULE_) || defined(BEST2002_BOOT_FOR_DPD)))
        ASSERT(slot <  transq_slot_num[id] && (status & ((uint64_t)1 << slot)),
            "TRANSQ-: Rx IRQ  status=" U64_STR " rx_mask=" U64_STR "slot=%d, irq_count:%d",
            U64_2_STR(status) , U64_2_STR(rx_irq_mask[id][pri]), slot, transq_cfg[id].rx_irq_count);
#else
        (void)slot;
#endif

        // Mask IRQ from corresponding slots
        mask = (uint32_t)rx_irq_mask[id][pri];
        peer_transq[id]->RMT_INTMASK &= ~mask;
        // Force to flush the write FIFO in the bus path
        peer_transq[id]->RMT_INTMASK;
        if (transq_slot_num[id] > 32) {
            mask = rx_irq_mask[id][pri] >> 32;
            peer_transq[id]->RMT_INTMASK_E &= ~mask;
            peer_transq[id]->RMT_INTMASK_E;
        }

#if defined(__ARM_ARCH_ISA_ARM)
        transq_unlock(lock);
#else
        int_unlock(lock);
#endif
        transq_cfg[id].rx_irq_count ++;
        if (transq_cfg[id].rx_handler) {
            transq_cfg[id].rx_handler(pri);
        }
    }
}

static int hal_transq_active_tx_valid(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri)
{
    return ((active_tx_slot[id][pri] >=  transq_slot_num[id]) ||
            (active_tx_slot[id][pri] != next_tx_slot[id][pri]) ||
            tx_slot_full[id][pri]);
}

void hal_transq_local_irq_handler_body(enum HAL_TRANSQ_ID_T id)
{
    enum HAL_TRANSQ_PRI_T pri = HAL_TRANSQ_PRI_NORMAL;
    uint32_t status0 = 0;
    uint32_t status1 = 0;
    uint64_t status;
    uint32_t slot, next_slot;
    uint32_t lock;

    status0 = transq[id]->LERR_MIS;
    if (transq_slot_num[id] > 32)
        status1 = transq[id]->LERR_MIS_E;

    if (status0)
        transq[id]->LERR_ISC.LERR_INTCLR = status0;
    if (status1)
        transq[id]->LERR_ISC_E.LERR_INTCLR = status1;

    if (status0 || status1) {
        ASSERT(false, "TRANSQ-%d: Tx on active slot: 0x%08x 0x%08x", id, status0, status1);
    }

    while (1) {
        status = 0;
        if (transq_slot_num[id] > 32) {
            status = transq[id]->LDONE_MIS_E;
            status = status << 32;
        }
        status |= transq[id]->LDONE_MIS;

        if (status == 0)
            break;

        if (transq_cfg[id].tx_handler) {
#if defined(__ARM_ARCH_ISA_ARM)
            lock = transq_lock();
#else
            lock = int_lock();
#endif
            ASSERT(hal_transq_active_tx_valid(id, HAL_TRANSQ_PRI_HIGH), "TRANSQ-%d: Corrupted pri active tx: active=%d next=%d full=%d",
                id, active_tx_slot[id][HAL_TRANSQ_PRI_HIGH], next_tx_slot[id][HAL_TRANSQ_PRI_HIGH], tx_slot_full[id][HAL_TRANSQ_PRI_HIGH]);
            ASSERT(hal_transq_active_tx_valid(id, HAL_TRANSQ_PRI_NORMAL), "TRANSQ-%d: Corrupted active tx: active=%d next=%d full=%d",
                id, active_tx_slot[id][HAL_TRANSQ_PRI_NORMAL], next_tx_slot[id][HAL_TRANSQ_PRI_NORMAL], tx_slot_full[id][HAL_TRANSQ_PRI_NORMAL]);

            if (status & tx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]) {
                pri = HAL_TRANSQ_PRI_HIGH;
            } else if (status & tx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL]) {
                pri = HAL_TRANSQ_PRI_NORMAL;
            } else {
                ASSERT(false, "TRANSQ-%d: Corrupted tx mask: status=" U64_STR "tx_mask=" U64_STR " / " U64_STR "",
                    id, U64_2_STR(status), U64_2_STR(tx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]), U64_2_STR(tx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL]));
            }
            slot = active_tx_slot[id][pri];

            ASSERT(slot <  transq_slot_num[id] && (status & ((uint64_t)1 << slot)),
                "TRANSQ-%d: Tx done IRQ when slot empty or out of order: status==" U64_STR " next=%d pri_next=%d",
                id, U64_2_STR(status), active_tx_slot[id][HAL_TRANSQ_PRI_NORMAL], active_tx_slot[id][HAL_TRANSQ_PRI_HIGH]);

            // Clear the interrupt
            if(slot < 32)
                transq[id]->LDONE_ISC.LDONE_INTCLR = (1 << slot);
            else
                transq[id]->LDONE_ISC_E.LDONE_INTCLR = (1 << (slot - 32));

            next_slot = get_next_tx_slot(id, pri, slot);

            if (!tx_slot_full[id][pri] && (next_slot == next_tx_slot[id][pri])) {
                // No tx in progress
                active_tx_slot[id][pri] =  transq_slot_num[id];
            } else {
                tx_slot_full[id][pri] = false;
                if (transq_cfg[id].slot.tx_num[pri] == 1) {
                    // No tx in progress
                    active_tx_slot[id][pri] =  transq_slot_num[id];
                } else {
                    // Some tx in progress
                    active_tx_slot[id][pri] = next_slot;
                }
            }
            if (TxOutstanding) {
                TxOutstanding--;
                if (!TxOutstanding) {
                    hal_sys_wake_unlock(HAL_SYS_WAKE_LOCK_USER_TRANSQ);
                }
            }
#if defined(__ARM_ARCH_ISA_ARM)
            transq_unlock(lock);
#else
            int_unlock(lock);
#endif
            transq_cfg[id].tx_handler(pri,
                (const uint8_t *)transq[id]->WSLOT[slot].ADDR,
                transq[id]->WSLOT[slot].LEN);
        } else {
            transq[id]->LDONE_INTMASK = 0;
            if (transq_slot_num[id] > 32)
                transq[id]->LDONE_INTMASK_E = 0;
        }
    }
}

#ifdef __ARM_ARCH_ISA_ARM
#ifdef RTOS
void hal_transq_local_irq_handler(int irq_num, void *irq_data)
#else
static void hal_transq_local_irq_handler(void)
#endif
#else
static void hal_transq_local_irq_handler(void)
#endif
{
    enum HAL_TRANSQ_ID_T id;
#if (CHIP_HAS_TRANSQ > 1)
#ifdef __ARM_ARCH_ISA_ARM
#ifdef RTOS
#ifdef KERNEL_LITEOS_A
    IRQn_Type irq = NVIC_GetCurrentActiveIRQ();
#else
    IRQn_Type irq = (IRQn_Type)irq_num;
#endif
#else
    IRQn_Type irq = NVIC_GetCurrentActiveIRQ();
#endif
#else
    IRQn_Type irq = NVIC_GetCurrentActiveIRQ();
#endif

    for (id = HAL_TRANSQ_ID_0; id < HAL_TRANSQ_ID_QTY; id++) {
        if (irq == local_irq_num[id]) {
            break;
        }
    }
    if (id >= HAL_TRANSQ_ID_QTY) {
        return;
    }
#else
    id = HAL_TRANSQ_ID_0;
#endif

    hal_transq_local_irq_handler_body(id);
}

enum HAL_TRANSQ_RET_T hal_transq_get_rx_status(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri, bool *ready)
{
    uint32_t lock;
    uint32_t slot;

    if (id >= HAL_TRANSQ_ID_QTY) {
        return HAL_TRANSQ_RET_BAD_ID;
    }
    if (pri >= HAL_TRANSQ_PRI_QTY) {
        return HAL_TRANSQ_RET_BAD_PRI;
    }
    if (transq_cfg[id].slot.rx_num[pri] == 0) {
        return HAL_TRANSQ_RET_BAD_RX_NUM;
    }
    if (transq_cfg[id].rx_handler) {
        // Rx will be processed by IRQ handler
        return HAL_TRANSQ_RET_BAD_MODE;
    }

    lock = int_lock();

    slot = next_rx_slot[id][pri];

    if ((slot <  32) && (peer_transq[id]->RMT_ISC.RMT_RIS & (1 << slot))) {
        *ready = true;
    } else if (32 <= slot && slot < TRANSQ_SLOT_NUM_MAX && (peer_transq[id]->RMT_ISC_E.RMT_RIS & (1 << (slot - 32)))) {
        *ready = true;
    } else {
        *ready = false;
    }

    int_unlock(lock);

    return HAL_TRANSQ_RET_OK;
}

enum HAL_TRANSQ_RET_T hal_transq_get_tx_status(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri, bool *done)
{
    uint32_t lock;
    uint32_t slot, next_slot;

    if (id >= HAL_TRANSQ_ID_QTY) {
        return HAL_TRANSQ_RET_BAD_ID;
    }
    if (pri >= HAL_TRANSQ_PRI_QTY) {
        return HAL_TRANSQ_RET_BAD_PRI;
    }
    if (transq_cfg[id].slot.tx_num[pri] == 0) {
        return HAL_TRANSQ_RET_BAD_TX_NUM;
    }
    if (transq_cfg[id].tx_handler) {
        // Tx done will be processed by IRQ handler
        return HAL_TRANSQ_RET_BAD_MODE;
    }

    lock = int_lock();

    slot = active_tx_slot[id][pri];

    ASSERT(hal_transq_active_tx_valid(id, pri), "TRANSQ-%d: Corrupted active tx: pri=%d active=%d next=%d full=%d",
        id, pri, active_tx_slot[id][pri], next_tx_slot[id][pri], tx_slot_full[id][pri]);

    if (slot < 32 && (transq[id]->LDONE_ISC.LDONE_RIS & (1 << slot))) {
        *done = true;

        // Clear the interrupt
        transq[id]->LDONE_ISC.LDONE_INTCLR = (1 << slot);
    } else if (32 <= slot && slot < TRANSQ_SLOT_NUM_MAX && (transq[id]->LDONE_ISC_E.LDONE_RIS & (1 << (slot - 32)))) {
        *done = true;

        // Clear the interrupt
        transq[id]->LDONE_ISC_E.LDONE_INTCLR = (1 << slot);
    } else {
        *done = false;
    }

    if (*done == true) {
        next_slot = get_next_tx_slot(id, pri, slot);

        if (!tx_slot_full[id][pri] && (next_slot == next_tx_slot[id][pri])) {
            // No tx in progress
            active_tx_slot[id][pri] =  transq_slot_num[id];
        } else {
            tx_slot_full[id][pri] = false;
            if (transq_cfg[id].slot.tx_num[pri] == 1) {
                // No tx in progress
                active_tx_slot[id][pri] =  transq_slot_num[id];
            } else {
                // Some tx in progress
                active_tx_slot[id][pri] = next_slot;
            }
        }
    }
    int_unlock(lock);

    return HAL_TRANSQ_RET_OK;
}

bool hal_transq_tx_busy(enum HAL_TRANSQ_ID_T id)
{
    bool ret;
    uint32_t lock;
    lock = int_lock();

    if ((active_tx_slot[id][HAL_TRANSQ_PRI_NORMAL] !=  transq_slot_num[id] ||
                active_tx_slot[id][HAL_TRANSQ_PRI_HIGH] !=  transq_slot_num[id])
            && (transq[id]->LDONE_MIS == 0 && (transq_slot_num[id] < 32 || transq[id]->LDONE_MIS_E == 0))) {
        ret = true;
    } else {
        ret = false;
    }

    int_unlock(lock);
    return ret;
}

#if defined(__MCU_FW_2002__) && defined(CONFIG_SDIO_SUPPORT) && !(defined(_WIFI_RF_TEST_MODULE_) || defined(BEST2002_BOOT_FOR_DPD))

#ifdef TRANSQ_RX_SLOTS_DEBUG
#define TRANSQ_RX_SLOTS_TRACE TRACE
#else
#define TRANSQ_RX_SLOTS_TRACE TRACE_DUMMY
#endif

#define container_of(ptr, type, member) ({ \
        const typeof( ((type *)0)->member )*__mptr = (ptr); \
        (type *)( (char *)__mptr - (char *)(&(((type *)0)->member)) );})

struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

struct slot_wrapper_t {
    struct list_head node;
    int slot;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->prev = list;
    list->next = list;
}

static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_entry_is_head(pos, head, member) \
    (&pos->member == (head))

static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

#define list_for_each_entry_safe(pos, n, head, member) \
    for (pos = list_first_entry(head, typeof(*pos), member), \
        n = list_next_entry(pos, member); \
        !list_entry_is_head(pos, head, member); \
        pos = n, n = list_next_entry(n, member))

static struct local_copy_t {
    uint8_t slot_toggle;
    uint32_t previous_rx_slot_wr;
    uint32_t previous_rx_slot_rd;
    struct list_head previous_rx_slot[2];
    struct slot_wrapper_t previous_rx_slot_pool[2][SDIO_TX_SCATTERLISTS_NUM];
} local_copy[HAL_TRANSQ_ID_QTY][HAL_TRANSQ_PRI_QTY], *local_copy_ptr;
static struct slot_wrapper_t *slot_wrapper;

void hal_transq_previous_init(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri)
{
    local_copy_ptr = &local_copy[id][pri];
    INIT_LIST_HEAD(&local_copy_ptr->previous_rx_slot[0]);
    INIT_LIST_HEAD(&local_copy_ptr->previous_rx_slot[1]);
    local_copy_ptr->previous_rx_slot_wr = 0;
    local_copy_ptr->previous_rx_slot_rd = 0;
    for(int i = 0; i < SDIO_TX_SCATTERLISTS_NUM; i++) {
        INIT_LIST_HEAD(&local_copy_ptr->previous_rx_slot_pool[0][i].node);
        INIT_LIST_HEAD(&local_copy_ptr->previous_rx_slot_pool[1][i].node);
    }
    local_copy_ptr->slot_toggle = 0;
}

void hal_transq_previous_toggle(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri)
{
    local_copy[id][pri].slot_toggle ^= 1;
}

void hal_transq_int_enable(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri)
{
    uint32_t lock;
    uint32_t mask;

#if defined(__ARM_ARCH_ISA_ARM)
    lock = transq_lock();
#else
    lock = int_lock();
#endif

    transq_cfg[id].rx_irq_count = 0;
    mask = (uint32_t)rx_irq_mask[id][pri];
    peer_transq[id]->RMT_INTMASK |= mask;

    if (transq_slot_num[id] > 32) {
        mask = rx_irq_mask[id][pri] >> 32;
        peer_transq[id]->RMT_INTMASK_E |= mask;
    }

#if defined(__ARM_ARCH_ISA_ARM)
    transq_unlock(lock);
#else
    int_unlock(lock);
#endif
}

void hal_transq_clear_previous(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri, int data_toggle)
{
    int i = 0;
    uint32_t slot = -1;
    uint32_t lock;
    struct list_head *slot_gather;
    struct slot_wrapper_t *temp;
    uint32_t mask;

#if defined(__ARM_ARCH_ISA_ARM)
    lock = transq_lock();
#else
    lock = int_lock();
#endif

    local_copy_ptr = &local_copy[id][pri];
    slot_gather = &local_copy_ptr->previous_rx_slot[data_toggle];
    list_for_each_entry_safe(slot_wrapper, temp, slot_gather, node) {
        slot = slot_wrapper->slot;
        if (slot < transq_slot_num[id]) {
            uint32_t rmt_ris;
            rmt_ris = (slot < 32) ? peer_transq[id]->RMT_ISC.RMT_RIS & (1 << slot) :
                    peer_transq[id]->RMT_ISC_E.RMT_RIS & (1 << (slot -32));

            if (rmt_ris) {
                if (slot < 32)
                    peer_transq[id]->RMT_ISC.RMT_INTCLR = (1 << slot);
                else
                    peer_transq[id]->RMT_ISC_E.RMT_INTCLR = (1 << (slot - 32));
                ++local_copy_ptr->previous_rx_slot_rd;
                //TRANSQ_RX_SLOTS_TRACE(0, "%s,rd=%u slot=%d", __func__, local_copy_ptr->previous_rx_slot_rd, slot);
            } else {
                if (0 == i)
                    break;
                else
                    ASSERT(0, "%s,%d %d,%d,%d", __func__, __LINE__, slot, data_toggle, i);
            }
        }
        ++i;
        list_del(&slot_wrapper->node);
    }
    TRANSQ_RX_SLOTS_TRACE(0, "%s,wr=%u rd=%u slot=0x%x tg=%d %d", __func__, local_copy_ptr->previous_rx_slot_wr, local_copy_ptr->previous_rx_slot_rd,
            slot, data_toggle, transq_cfg[id].rx_irq_count);
    if (local_copy_ptr->previous_rx_slot_rd == local_copy_ptr->previous_rx_slot_wr) {
        ASSERT(list_empty(slot_gather),"%s,%d", __func__, __LINE__);
        transq_cfg[id].rx_irq_count =0;

        mask = (uint32_t)rx_irq_mask[id][pri];
        peer_transq[id]->RMT_INTMASK |= mask;

        if (transq_slot_num[id] > 32) {
            mask = rx_irq_mask[id][pri] >> 32;
            peer_transq[id]->RMT_INTMASK_E |= mask;
        }
    }

#if defined(__ARM_ARCH_ISA_ARM)
    transq_unlock(lock);
#else
    int_unlock(lock);
#endif
}
#endif

enum HAL_TRANSQ_RET_T hal_transq_rx_first(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri, const uint8_t **data, uint32_t *len)
{
    enum HAL_TRANSQ_RET_T ret;
    uint32_t slot;
    uint32_t lock;

    if (id >= HAL_TRANSQ_ID_QTY) {
        return HAL_TRANSQ_RET_BAD_ID;
    }
    if (pri >= HAL_TRANSQ_PRI_QTY) {
        return HAL_TRANSQ_RET_BAD_PRI;
    }
    if (transq_cfg[id].slot.rx_num[pri] == 0) {
        return HAL_TRANSQ_RET_BAD_RX_NUM;
    }

#if defined(__ARM_ARCH_ISA_ARM)
    lock = transq_lock();
#else
    lock = int_lock();
#endif

    slot = next_rx_slot[id][pri];

    do {
        if (slot <  transq_slot_num[id]) {
            uint32_t rmt_ris;
            rmt_ris = (slot < 32) ? peer_transq[id]->RMT_ISC.RMT_RIS & (1 << slot) :
                            peer_transq[id]->RMT_ISC_E.RMT_RIS & (1 << (slot - 32));
            if (rmt_ris) {
                // Msg available
                ret = HAL_TRANSQ_RET_OK;
                if (data)
                    *data = (const uint8_t *)peer_transq[id]->RSLOT[slot].ADDR;
                if (len)
                    *len = peer_transq[id]->RSLOT[slot].LEN;
                break;
            }
        }

        // No msg. Re-enable IRQ
        ret = HAL_TRANSQ_RET_RX_EMPTY;
       // wlan_func.transq_irq_count =0;
        if (data) {
            *data = NULL;
        }
        if (len) {
            *len = 0;
        }
#if !(defined(__MCU_FW_2002__) && defined(CONFIG_SDIO_SUPPORT) && !(defined(_WIFI_RF_TEST_MODULE_) || defined(BEST2002_BOOT_FOR_DPD)))
        transq_cfg[id].rx_irq_count =0;

        uint32_t mask;
        mask = (uint32_t)rx_irq_mask[id][pri];
        peer_transq[id]->RMT_INTMASK |= mask;
        if (transq_slot_num[id] > 32) {
            mask = rx_irq_mask[id][pri] >> 32;
            peer_transq[id]->RMT_INTMASK_E |= mask;
        }
#endif
    } while (0);

#if defined(__ARM_ARCH_ISA_ARM)
    transq_unlock(lock);
#else
    int_unlock(lock);
#endif

    return ret;
}

enum HAL_TRANSQ_RET_T hal_transq_rx_next(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri, const uint8_t **data, uint32_t *len)
{
    enum HAL_TRANSQ_RET_T ret;
    uint32_t slot;
    uint32_t lock;

    if (id >= HAL_TRANSQ_ID_QTY) {
        return HAL_TRANSQ_RET_BAD_ID;
    }
    if (pri >= HAL_TRANSQ_PRI_QTY) {
        return HAL_TRANSQ_RET_BAD_PRI;
    }
    if (transq_cfg[id].slot.rx_num[pri] == 0) {
        return HAL_TRANSQ_RET_BAD_RX_NUM;
    }

    ret = HAL_TRANSQ_RET_OK;

#if defined(__ARM_ARCH_ISA_ARM)
    lock = transq_lock();
#else
    lock = int_lock();
#endif

    slot = next_rx_slot[id][pri];

    do {
        if (slot <  transq_slot_num[id]) {
            uint32_t rmt_ris;
            rmt_ris = (slot < 32) ? peer_transq[id]->RMT_ISC.RMT_RIS & (1 << slot) :
                            peer_transq[id]->RMT_ISC_E.RMT_RIS & (1 << (slot -32));
            if (rmt_ris) {
                // Clear cur IRQ
#if defined(__MCU_FW_2002__) && defined(CONFIG_SDIO_SUPPORT) && !(defined(_WIFI_RF_TEST_MODULE_) || defined(BEST2002_BOOT_FOR_DPD))
                local_copy_ptr = &local_copy[id][pri];
                slot_wrapper = &local_copy_ptr->previous_rx_slot_pool[local_copy_ptr->slot_toggle][local_copy_ptr->previous_rx_slot_wr & (SDIO_TX_SCATTERLISTS_NUM - 1)];
                slot_wrapper->slot = slot;
                list_add_tail(&slot_wrapper->node, &local_copy_ptr->previous_rx_slot[local_copy_ptr->slot_toggle]);
                ++local_copy_ptr->previous_rx_slot_wr;
                TRANSQ_RX_SLOTS_TRACE(0, "%s,wr=%u rd=%u slot=%d tg=%d", __func__, local_copy_ptr->previous_rx_slot_wr, local_copy_ptr->previous_rx_slot_rd, slot, local_copy_ptr->slot_toggle);
#else
                if (slot < 32)
                    peer_transq[id]->RMT_ISC.RMT_INTCLR = (1 << slot);
                else
                    peer_transq[id]->RMT_ISC_E.RMT_INTCLR = (1 << (slot - 32));
#endif

                // Update next_rx_slot
                slot = get_next_rx_slot(id, pri, slot);

                next_rx_slot[id][pri] = slot;

                rmt_ris = (slot < 32) ? peer_transq[id]->RMT_ISC.RMT_RIS & (1 << slot) :
                                peer_transq[id]->RMT_ISC_E.RMT_RIS & (1 << (slot -32));
                if (rmt_ris) {
                    // Next msg available
                    if (data)
                        *data = (const uint8_t *)peer_transq[id]->RSLOT[slot].ADDR;
                    if (len)
                        *len = peer_transq[id]->RSLOT[slot].LEN;
                    break;
                }
            }
        }
        ret = HAL_TRANSQ_RET_RX_EMPTY;
    } while (0);

    if (ret == HAL_TRANSQ_RET_RX_EMPTY) {
        if (data) {
            *data = NULL;
        }
        if (len) {
            *len = 0;
        }
#if !(defined(__MCU_FW_2002__) && defined(CONFIG_SDIO_SUPPORT) && !(defined(_WIFI_RF_TEST_MODULE_) || defined(BEST2002_BOOT_FOR_DPD)))
        //wlan_func.transq_irq_count =0;
        transq_cfg[id].rx_irq_count =0;
        // Re-enable IRQ

        uint32_t mask;
        mask = (uint32_t)rx_irq_mask[id][pri];
        peer_transq[id]->RMT_INTMASK |= mask;
        if (transq_slot_num[id] > 32) {
            mask = rx_irq_mask[id][pri] >> 32;
            peer_transq[id]->RMT_INTMASK_E |= mask;
        }
#endif
    }

#if defined(__ARM_ARCH_ISA_ARM)
    transq_unlock(lock);
#else
    int_unlock(lock);
#endif

    return ret;
}

enum HAL_TRANSQ_RET_T hal_transq_tx(enum HAL_TRANSQ_ID_T id, enum HAL_TRANSQ_PRI_T pri, const uint8_t *data, uint32_t len)
{
    enum HAL_TRANSQ_RET_T ret;
    uint32_t lock;
    uint32_t slot;

    if (id >= HAL_TRANSQ_ID_QTY) {
        return HAL_TRANSQ_RET_BAD_ID;
    }
    if (pri >= HAL_TRANSQ_PRI_QTY) {
        return HAL_TRANSQ_RET_BAD_PRI;
    }
    if (transq_cfg[id].slot.tx_num[pri] == 0) {
        return HAL_TRANSQ_RET_BAD_TX_NUM;
    }

#if defined(__ARM_ARCH_ISA_ARM)
    lock = transq_lock();
#else
    lock = int_lock();
#endif

#ifndef __ARM_ARCH_ISA_ARM
#if (defined(PSRAM_ENABLE) || defined(PSRAMUHS_ENABLE)) && defined(CHIP_BEST2001)
    if (!((uint32_t)data > RAM_BASE && (uint32_t)data < (RAM_BASE + RAM_TOTAL_SIZE))) {
        hal_cache_sync_all(HAL_CACHE_ID_D_CACHE);
    }
#endif
#endif
    if (tx_slot_full[id][pri]) {
        ret = HAL_TRANSQ_RET_TX_FULL;
    } else {
        TxOutstanding++;
        if (TxOutstanding == 1) {
            hal_sys_wake_lock(HAL_SYS_WAKE_LOCK_USER_TRANSQ);
        }

        ret = HAL_TRANSQ_RET_OK;

        slot = next_tx_slot[id][pri];

        transq[id]->WSLOT[slot].ADDR = (uint32_t)data;
        transq[id]->WSLOT[slot].LEN = len;
        if(slot < 32) {
            transq[id]->RMT_INTSET = (1 << slot);
        } else {
            transq[id]->RMT_INTSET_E = (1 << (slot - 32));
        }

        // Update active_tx_slot if this is the only tx in progress
        if (active_tx_slot[id][pri] >=  transq_slot_num[id]) {
            active_tx_slot[id][pri] = slot;
        }

        // Update next_tx_slot
        next_tx_slot[id][pri] = get_next_tx_slot(id, pri, slot);

        if (next_tx_slot[id][pri] == active_tx_slot[id][pri]) {
            tx_slot_full[id][pri] = true;
        }
    }

#if defined(__ARM_ARCH_ISA_ARM)
    transq_unlock(lock);
#else
    int_unlock(lock);
#endif
    return ret;
}

enum HAL_TRANSQ_RET_T hal_transq_update_num(enum HAL_TRANSQ_ID_T id, const struct HAL_TRANSQ_SLOT_NUM_T *slot)
{
    uint32_t rx_mask;
    uint32_t tx_mask;

    if (id >= HAL_TRANSQ_ID_QTY) {
        return HAL_TRANSQ_RET_BAD_ID;
    }
    if (slot == NULL) {
        return HAL_TRANSQ_RET_BAD_SLOT;
    }
    if (slot->tx_num[HAL_TRANSQ_PRI_NORMAL] + slot->tx_num[HAL_TRANSQ_PRI_HIGH] >  transq_slot_num[id]) {
        return HAL_TRANSQ_RET_BAD_TX_NUM;
    }
    if (slot->rx_num[HAL_TRANSQ_PRI_NORMAL] + slot->rx_num[HAL_TRANSQ_PRI_HIGH] >  transq_slot_num[id]) {
        return HAL_TRANSQ_RET_BAD_RX_NUM;
    }

    transq_cfg[id].slot = *slot;

    rx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL] = construct_mask(0, slot->rx_num[HAL_TRANSQ_PRI_NORMAL]);
    rx_irq_mask[id][HAL_TRANSQ_PRI_HIGH] = construct_mask(
        transq_slot_num[id] - slot->rx_num[HAL_TRANSQ_PRI_HIGH],
        slot->rx_num[HAL_TRANSQ_PRI_HIGH]);

    tx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL] = construct_mask(0, slot->tx_num[HAL_TRANSQ_PRI_NORMAL]);
    tx_irq_mask[id][HAL_TRANSQ_PRI_HIGH] =  construct_mask(
        transq_slot_num[id]  - slot->tx_num[HAL_TRANSQ_PRI_HIGH],
        slot->tx_num[HAL_TRANSQ_PRI_HIGH]);

    rx_mask = (uint32_t)(rx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL] | rx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]);
    tx_mask = (uint32_t)(tx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL] | tx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]);

    transq[id]->RMT_INTMASK = rx_mask;
    transq[id]->LERR_INTMASK = tx_mask;

    if (transq_cfg[id].tx_handler) {
        transq[id]->LDONE_INTMASK = tx_mask;
    } else {
        transq[id]->LDONE_INTMASK = 0;
    }
    if (transq_slot_num[id] > 32) {
        rx_mask = (rx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL] | rx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]) >> 32;
        tx_mask = (tx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL] | tx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]) >> 32;
        transq[id]->RMT_INTMASK_E = rx_mask;
        transq[id]->LERR_INTMASK_E = tx_mask;
        if (transq_cfg[id].tx_handler) {
            transq[id]->LDONE_INTMASK_E = tx_mask;
        } else {
            transq[id]->LDONE_INTMASK_E = 0;
        }
    }

    TRACE(1, "transq:%d, tx_handler%p, rx_mask:" U64_STR U64_STR ", tx_mask" U64_STR U64_STR"",
        id, transq_cfg[id].tx_handler,
        U64_2_STR(rx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL]), U64_2_STR(rx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]),
        U64_2_STR(tx_irq_mask[id][HAL_TRANSQ_PRI_NORMAL]), U64_2_STR(tx_irq_mask[id][HAL_TRANSQ_PRI_HIGH]));

    return HAL_TRANSQ_RET_OK;
}

enum HAL_TRANSQ_RET_T hal_transq_open(enum HAL_TRANSQ_ID_T id, const struct HAL_TRANSQ_CFG_T *cfg)
{
    const struct HAL_TRANSQ_SLOT_NUM_T *slot;
    uint32_t ctrl;
    enum HAL_TRANSQ_RET_T ret;

    if (id >= HAL_TRANSQ_ID_QTY) {
        return HAL_TRANSQ_RET_BAD_ID;
    }
    if (cfg == NULL) {
        return HAL_TRANSQ_RET_BAD_CFG;
    }

    hal_cmu_clock_enable(transq_mod[id]);
    hal_cmu_reset_clear(transq_mod[id]);

    transq_cfg[id] = *cfg;

    slot = &cfg->slot;

    ret = hal_transq_update_num(id, slot);
    if (ret) {
        return ret;
    }

    next_tx_slot[id][HAL_TRANSQ_PRI_NORMAL] = slot->tx_num[HAL_TRANSQ_PRI_NORMAL] ? 0 :  transq_slot_num[id];
    active_tx_slot[id][HAL_TRANSQ_PRI_NORMAL] =  transq_slot_num[id];
    tx_slot_full[id][HAL_TRANSQ_PRI_NORMAL] = false;
    next_rx_slot[id][HAL_TRANSQ_PRI_NORMAL] = slot->rx_num[HAL_TRANSQ_PRI_NORMAL] ? 0 :  transq_slot_num[id];

    next_tx_slot[id][HAL_TRANSQ_PRI_HIGH] = slot->tx_num[HAL_TRANSQ_PRI_HIGH] ?
        ( transq_slot_num[id] - slot->tx_num[HAL_TRANSQ_PRI_HIGH]) :  transq_slot_num[id];
    active_tx_slot[id][HAL_TRANSQ_PRI_HIGH] =  transq_slot_num[id];
    tx_slot_full[id][HAL_TRANSQ_PRI_HIGH] = false;
    next_rx_slot[id][HAL_TRANSQ_PRI_HIGH] = slot->rx_num[HAL_TRANSQ_PRI_HIGH] ?
        ( transq_slot_num[id] - slot->rx_num[HAL_TRANSQ_PRI_HIGH]) :  transq_slot_num[id];

    transq[id]->RMT_ISC.RMT_INTCLR = ~0UL;
    transq[id]->LDONE_ISC.LDONE_INTCLR = ~0UL;
    transq[id]->LERR_ISC.LERR_INTCLR = ~0UL;
    if (transq_slot_num[id] > 32) {
        transq[id]->RMT_ISC_E.RMT_INTCLR = ~0UL;
        transq[id]->LDONE_ISC_E.LDONE_INTCLR = ~0UL;
        transq[id]->LERR_ISC_E.LERR_INTCLR = ~0UL;
    }
    ctrl = CTRL_REMOTE_IRQ_EN | CTRL_LOCAL_ERR_IRQ_EN;
    if (cfg->tx_handler) {
        ctrl |= CTRL_LOCAL_DONE_IRQ_EN;
    }
    transq[id]->CTRL = ctrl;

    if (cfg->rx_handler) {
        NVIC_SetVector(remote_irq_num[id], (uint32_t)hal_transq_remote_irq_handler);
        NVIC_SetPriority(remote_irq_num[id], IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(remote_irq_num[id]);
        NVIC_EnableIRQ(remote_irq_num[id]);
    }

    NVIC_SetVector(local_irq_num[id], (uint32_t)hal_transq_local_irq_handler);
    NVIC_SetPriority(local_irq_num[id], IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(local_irq_num[id]);
    NVIC_EnableIRQ(local_irq_num[id]);

#if defined(__ARM_ARCH_ISA_ARM)
    transq_lock_init();
#endif

    return HAL_TRANSQ_RET_OK;
}

enum HAL_TRANSQ_RET_T hal_transq_close(enum HAL_TRANSQ_ID_T id)
{
    if (id >= HAL_TRANSQ_ID_QTY) {
        return HAL_TRANSQ_RET_BAD_ID;
    }

    NVIC_DisableIRQ(remote_irq_num[id]);
    NVIC_DisableIRQ(local_irq_num[id]);

    transq[id]->CTRL = 0;

    hal_cmu_reset_set(transq_mod[id]);
    hal_cmu_clock_disable(transq_mod[id]);

    return HAL_TRANSQ_RET_OK;
}
enum HAL_TRANSQ_RET_T hal_transq_flush(enum HAL_TRANSQ_ID_T id)
{
    transq[id]->RMT_ISC.RMT_INTCLR = ~0UL;
    if (transq_slot_num[id] > 32) {
        transq[id]->RMT_ISC_E.RMT_INTCLR = ~0UL;
    }

    return HAL_TRANSQ_RET_OK;
}
uint8_t get_rx_irq_count(enum HAL_TRANSQ_ID_T id)
{
    return transq_cfg[id].rx_irq_count;
}

/* wifi specific */
struct wsm_hdr_t {
	uint16_t len;
	uint16_t id;
};

void show_reg(void)
{
    uint32_t i;
    struct wsm_hdr_t * wsm;
    TRACE(0, " peer_transq[0]->RMT_MIS: 0x%08x, RMT_RIS: 0x%08x, RMT_INTMASK: 0x%08x, slot=%d ",
        peer_transq[0]->RMT_MIS,
        peer_transq[0]->RMT_ISC.RMT_RIS,
        peer_transq[0]->RMT_INTMASK,
        next_rx_slot[0][0]);
    if (transq_slot_num[0] > 32) {
        TRACE(0, " peer_transq[0]->RMT_MIS_E: 0x%08x, RMT_RIS_E: 0x%08x, RMT_INTMASK_E: 0x%08x, slot=%d ",
            peer_transq[0]->RMT_MIS_E,
            peer_transq[0]->RMT_ISC_E.RMT_RIS,
            peer_transq[0]->RMT_INTMASK_E,
            next_rx_slot[0][0]);
    }
    for(i = 0; i < 32; i++) {
         wsm  = (struct wsm_hdr_t*) (peer_transq[0]->RSLOT[i].ADDR);
         if ((uint32_t)wsm >= 0x80000000 && (uint32_t)wsm <= 0x90000000)
            TRACE(0, "peer_transq[0]->RSLOT[%d].addr= 0x%08x, wsm->id: 0x%x, len= %d", i, (uint32_t)wsm, wsm->id,wsm->len);
         else
            TRACE(0, "error or unenabled transq msg. 0x%08x", (uint32_t)wsm);
    }
    for(i = 32; i < transq_slot_num[0]; i++) {
         wsm  = (struct wsm_hdr_t*) (peer_transq[0]->RSLOT[i].ADDR);
         if ((uint32_t)wsm >= 0x80000000 && (uint32_t)wsm <= 0x90000000)
            TRACE(0, "peer_transq[0]->RSLOT[%d].addr= 0x%08x, wsm->id: 0x%x, len= %d", i, (uint32_t)wsm, wsm->id,wsm->len);
         else
            TRACE(0, "error or unenabled transq msg. 0x%08x", (uint32_t)wsm);
    }
}
#endif