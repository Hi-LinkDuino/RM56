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
#if (defined(ARM_CMSE) || defined(ARM_CMNS))
#include "plat_addr_map.h"
#include "cmsis_nvic.h"
#include "hal_location.h"
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_cache.h"
#include "hal_mcu2cp.h"

#include CHIP_SPECIFIC_HDR(reg_cmu)

typedef unsigned int (*HAL_TZ_TRIGGER_HANDLER)(void *param);

static const IRQn_Type  rx_irq_id =
#if defined(ARM_CMSE)
    MCU2CP_DONE2_IRQn
#else
    CP2MCU_DATA2_IRQn
#endif
;

static HAL_TZ_TRIGGER_HANDLER  rx_irq_handler;

static bool chan_opened;


static struct CMU_T * const  cmu = (struct CMU_T *)CMU_BASE;
uint32_t int_lock_smp_com();
void int_unlock_smp_com(uint32_t flags);

static int hal_mcu2cp_peer_irq_set()
{
#if defined(ARM_CMSE)
    cmu->CP2MCU_IRQ_SET = CMU_CP2MCU_DATA2_IND_SET;
#else
    cmu->CP2MCU_IRQ_SET = CMU_CP2MCU_DATA2_DONE_SET;
#endif
    return 0;
}

static int hal_mcu2cp_local_irq_clear()
{
#if defined(ARM_CMSE)
    cmu->CP2MCU_IRQ_CLR = CMU_CP2MCU_DATA2_DONE_CLR;
#else
    cmu->CP2MCU_IRQ_CLR = CMU_CP2MCU_DATA2_IND_CLR;
#endif
    return 0;
}

static void hal_tz_trigger_rx_irq(void)
{
    uint32_t lock = int_lock_smp_com();
    //TRACE(0,"hal_tz_trigger_rx_irq id=%d,act=%d,rx_irq_handler=%p",id,NVIC_GetActive(rx_irq_id[id]),rx_irq_handler);
    if (NVIC_GetActive(rx_irq_id)) {
        hal_mcu2cp_local_irq_clear();
        int_unlock_smp_com(lock);
        rx_irq_handler(NULL);
        lock = int_lock_smp_com();
    }
    int_unlock_smp_com(lock);
}

void hal_tz_trigger_init()
{
#if defined(ARM_CMSE)
    return;
#endif
#if !defined(WIFI_ON_CP) && !defined(CONFIG_BES_DUALCORE_AMP) || defined(CONFIG_BES_RPTUN_DELAY_BOOT)
    hal_cmu_dbg_set_val(0, 0);
    hal_cmu_dbg_set_val(1, 0);
    hal_mcu2cp_set_send_msg_list_mcu();
#endif
#if defined(CONFIG_RPTUN)
    bes_rptun_heap_init();
#endif
}
int hal_tz_trigger_open(HAL_TZ_TRIGGER_HANDLER rxhandler)
{
#if defined(ARM_CMSE)
    /* wait mcu init send msg addr */
    while (hal_mcu2cp_get_send_msg_list_mcu() == NULL)
        osDelay(100);
    osDelay(100);
#endif
    uint32_t lock = int_lock_smp_com();
    if (chan_opened) {
        int_unlock_smp_com(lock);
        return 3;
    } else {
        hal_mcu2cp_local_irq_clear();
        NVIC_SetVector(rx_irq_id, (uint32_t)hal_tz_trigger_rx_irq);
        NVIC_SetPriority(rx_irq_id, IRQ_PRIORITY_NORMAL);
        NVIC_DisableIRQ(rx_irq_id);
    }
    chan_opened = true;
    rx_irq_handler = rxhandler;
    NVIC_EnableIRQ(rx_irq_id);
    int_unlock_smp_com(lock);
    return 0;
}

int hal_tz_trigger_close()
{
    uint32_t lock = int_lock_smp_com();

    chan_opened = false;
    rx_irq_handler = NULL;
    NVIC_DisableIRQ(rx_irq_id);
    int_unlock_smp_com(lock);
    return 0;
}

int hal_tz_trigger_set()
{
     if (!chan_opened) {
        return 3;
    }
    //TRACE(0,"hal_tz_trigger_set id=%d",id);
    uint32_t lock = int_lock_smp_com();
    hal_mcu2cp_peer_irq_set();
    int_unlock_smp_com(lock);
    return 0;
}

#endif
