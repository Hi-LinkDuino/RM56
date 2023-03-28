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
#include "plat_types.h"
#include "bt_drv_interface.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "app_bt_sync.h"
#include "hal_timer.h"

#ifdef IBRT
#include "app_tws_ibrt_cmd_handler.h"
#include "app_ibrt_if.h"
#include "app_tws_ctrl_thread.h"
#endif

#define MAIL_INCOME_SIG (0x01)

#if defined(__BT_SYNC__) && defined(IBRT)
static osThreadId app_bt_sync_thread_tid = NULL;
static bool app_bt_sync_initilized = false;
static bt_trigger_info_s locTriggerInfo[APP_BT_SYNC_CHANNEL_MAX];
static app_bt_sync_env app_bt_sync_local_env;

static void app_bt_sync_thread(void const *argument);
osThreadDef(app_bt_sync_thread, osPriorityRealtime, 1, APP_BT_SYNC_THREAD_STACK_SIZE, "app_bt_sync");
osMailQDef (app_sync_mailbox, 10, APP_SYNC_MSG_BLOCK);
static osMailQId app_sync_mailbox = NULL;
static void app_bt_sync_send_trigger_info_timeout_handler(void const *param);
osTimerDef(APP_BT_SYNC_SEND_TRIGGER_INFO, app_bt_sync_send_trigger_info_timeout_handler);
static osTimerId app_bt_sync_send_trigger_info_timer = NULL;
osMutexDef(app_bt_sync_timer_mutex);
static osMutexId app_bt_sync_timer_mutex_id = NULL;

static app_bt_sync_instance_t* app_bt_sync_get_entry_pointer_from_cmd_code(uint8_t opCode)
{
    for (uint32_t index = 0;
        index < ((uint32_t)__app_bt_sync_command_handler_table_end-(uint32_t)__app_bt_sync_command_handler_table_start)/sizeof(app_bt_sync_instance_t);index++)
    {
        if (APP_BT_SYNC_COMMAND_PTR_FROM_ENTRY_INDEX(index)->opCode == opCode)
        {
            return APP_BT_SYNC_COMMAND_PTR_FROM_ENTRY_INDEX(index);
        }
    }

    return NULL;
}

static app_bt_sync_status_info_s* app_bt_sync_get_status_instance(uint8_t opCode)
{
    for (uint8_t i = 0; i < app_bt_sync_local_env.timeoutSupervisorCount; i++) {
        if (opCode == app_bt_sync_local_env.statusInstance[i].opCode) {
            return &app_bt_sync_local_env.statusInstance[i];
        }
    }

    return NULL;
}

static void app_bt_sync_refresh_supervisor_env(void)
{
    // do nothing if no supervisor was added
    if (app_bt_sync_local_env.timeoutSupervisorCount > 0)
    {
        uint32_t currentTicks = GET_CURRENT_TICKS();
        uint32_t passedTicks = 0;

        if (currentTicks >= app_bt_sync_local_env.lastSysTicks)
        {
            passedTicks = (currentTicks - app_bt_sync_local_env.lastSysTicks);
        }
        else
        {
            passedTicks = (hal_sys_timer_get_max() - app_bt_sync_local_env.lastSysTicks + 1) + currentTicks;
        }

        uint32_t deltaMs = TICKS_TO_MS(passedTicks);

        app_bt_sync_status_info_s* pSupervisor = &(app_bt_sync_local_env.statusInstance[0]);
        for (uint8_t index = 0;index < app_bt_sync_local_env.timeoutSupervisorCount;index++)
        {
            ASSERT(pSupervisor[index].msTillTimeout > deltaMs,
                "the waiting command response supervisor timer is missing!!!, \
                %d ms passed but the ms to trigger is %d", deltaMs, pSupervisor[index].msTillTimeout);

            pSupervisor[index].msTillTimeout -= deltaMs;
        }
    }

    app_bt_sync_local_env.lastSysTicks = GET_CURRENT_TICKS();
}

static void app_bt_sync_remove_waiting_rsp_timeout_supervision(uint32_t opCode)
{
    ASSERT(app_bt_sync_local_env.timeoutSupervisorCount > 0,
        "%s The BLE custom command time-out supervisor is already empty!!!", __FUNCTION__);

    TRACE(1, "%s+++ opcode %d", __func__, opCode);

    osMutexWait(app_bt_sync_timer_mutex_id, osWaitForever);

    uint32_t index = 0;
    for (index = 0;index < app_bt_sync_local_env.timeoutSupervisorCount;index++)
    {
        if (app_bt_sync_local_env.statusInstance[index].opCode == opCode)
        {
            memcpy(&(app_bt_sync_local_env.statusInstance[index]),
                &(app_bt_sync_local_env.statusInstance[index + 1]),
                (app_bt_sync_local_env.timeoutSupervisorCount - index - 1)*sizeof(app_bt_sync_status_info_s));
            break;
        }
    }

    // cannot find it, directly return
    if (index == app_bt_sync_local_env.timeoutSupervisorCount)
    {
        goto exit;
    }

    app_bt_sync_local_env.timeoutSupervisorCount--;

    if (app_bt_sync_local_env.timeoutSupervisorCount > 0)
    {
        // refresh supervisor environment firstly
        app_bt_sync_refresh_supervisor_env();

        // start timer, the first entry is the most close one
        osTimerStart(app_bt_sync_send_trigger_info_timer, app_bt_sync_local_env.statusInstance[0].msTillTimeout);
    }
    else
    {
        // no supervisor, directly stop the timer
        osTimerStop(app_bt_sync_send_trigger_info_timer);
    }

exit:
    osMutexRelease(app_bt_sync_timer_mutex_id);
}

static void app_bt_sync_add_waiting_irq_timeout_supervision(uint32_t opCode)
{
    ASSERT(app_bt_sync_local_env.timeoutSupervisorCount < APP_BT_SYNC_CHANNEL_MAX,
        "%s warning:time-out supervisor is full!!!", __func__);

    osMutexWait(app_bt_sync_timer_mutex_id, osWaitForever);

    // refresh supervisor environment firstly
    app_bt_sync_refresh_supervisor_env();

    app_bt_sync_local_env.statusInstance[app_bt_sync_local_env.timeoutSupervisorCount].initiator = true;

    app_bt_sync_local_env.statusInstance[app_bt_sync_local_env.timeoutSupervisorCount].opCode = opCode;
    app_bt_sync_local_env.statusInstance[app_bt_sync_local_env.timeoutSupervisorCount].msTillTimeout = APP_BT_SYNC_SENT_TRIGGER_INFO_TIMEOUT_IN_MS;

    app_bt_sync_local_env.timeoutSupervisorCount++;

    // start timer, the first entry is the most close one
    osTimerStart(app_bt_sync_send_trigger_info_timer, app_bt_sync_local_env.statusInstance[0].msTillTimeout);

    osMutexRelease(app_bt_sync_timer_mutex_id);
}

static int app_bt_sync_mailbox_free(APP_SYNC_MSG_BLOCK* msg_p)
{
    osStatus status;

    status = osMailFree(app_sync_mailbox, msg_p);
    if (osOK != status) {
        TRACE(1, "%s failed to free", __func__);
    }
    return (int)status;
}

static int app_bt_sync_mailbox_get(APP_SYNC_MSG_BLOCK** msg_p)
{
    osEvent evt;
    evt = osMailGet(app_sync_mailbox, osWaitForever);
    if (evt.status == osEventMail) {
        *msg_p = (APP_SYNC_MSG_BLOCK *)evt.value.p;
        return 0;
    }
    return -1;
}

static int app_bt_sync_mailbox_put(APP_SYNC_MSG_BLOCK* msg_src)
{
    osStatus status;
    APP_SYNC_MSG_BLOCK *msg_p = NULL;

    msg_p = (APP_SYNC_MSG_BLOCK*)osMailAlloc(app_sync_mailbox, 0);

    if (!msg_p) {
        TRACE(1, "%s failed to alloc", __func__);
        return -1;
    }

    *msg_p = *msg_src;
    status = osMailPut(app_sync_mailbox, msg_p);
    if (osOK != status) {
        TRACE(1, "%s failed to put", __func__);
    } else {
        osSignalSet(app_bt_sync_thread_tid, MAIL_INCOME_SIG);
    }

    return (int)status;
}

static void app_bt_sync_thread(void const *argument)
{
    while (1) {
        APP_SYNC_MSG_BLOCK *msg_p = NULL;
        uint8_t opCode = APP_BT_SYNC_INVALID_CHANNEL;
        app_bt_sync_instance_t *pCallback = NULL;
        app_bt_sync_status_info_s *statusInfo = NULL;
        bool twsCmdSendStatus = false;
        bool triResult = false;

        osSignalWait(0x00, osWaitForever);
        while (!app_bt_sync_mailbox_get(&msg_p)) {
            opCode = msg_p->opCode;
            pCallback = app_bt_sync_get_entry_pointer_from_cmd_code(opCode);
            statusInfo = app_bt_sync_get_status_instance(opCode);

            TRACE(4, "%s callback %p opCode %d, triSrc 0x%x", __func__, pCallback, opCode, msg_p->triSrc);

            if (msg_p->triSrc == APP_BT_SYNC_INVALID_CHANNEL) {
                triResult = false;
            } else {
                triResult = true;
            }

            if (statusInfo && statusInfo->initiator) {
                app_bt_sync_remove_waiting_rsp_timeout_supervision(opCode);
                twsCmdSendStatus = statusInfo->twsCmdSendDone;
                if (pCallback && pCallback->statusNotify)
                    pCallback->statusNotify(opCode, triResult, twsCmdSendStatus);
            }

            if ((triResult) && (pCallback) &&
                (pCallback->syncCallback)) {
                pCallback->syncCallback();
            }

            app_bt_sync_mailbox_free(msg_p);
        }
    }
}

static void app_bt_sync_send_trigger_info_timeout_handler(void const *param)
{
    uint32_t opCode = app_bt_sync_local_env.statusInstance[0].opCode;
    TRACE(2, "%s:send trigger info timeout opCode = %d", __func__, opCode);

    APP_SYNC_MSG_BLOCK msg;
    msg.triSrc = APP_BT_SYNC_INVALID_CHANNEL;
    msg.opCode = opCode;
    app_bt_sync_mailbox_put(&msg);
}

static void app_bt_sync_irq_handler(enum HAL_CMU_BT_TRIGGER_SRC_T src)
{
    btdrv_syn_clr_trigger((uint8_t)src);
    hal_cmu_bt_trigger_disable(src);

    /// currently, channel0 reserved for A2DP/HFP, channel1 reserved for DAC2
    uint8_t triggerIdx = src - RESERVED_TRIGGER_CHNL_CNT;
    locTriggerInfo[triggerIdx].used = false;
    TRACE(3, "%s src %d opCode %d", __func__, src, locTriggerInfo[triggerIdx].opCode);

    APP_SYNC_MSG_BLOCK msg;
    msg.triSrc = triggerIdx;
    msg.opCode = locTriggerInfo[triggerIdx].opCode;
    app_bt_sync_mailbox_put(&msg);
}

static bool app_bt_sync_module_init(void)
{
    if (app_bt_sync_initilized == false) {
        memset(locTriggerInfo, 0x00, sizeof(locTriggerInfo[0]) * APP_BT_SYNC_CHANNEL_MAX);
        app_sync_mailbox = osMailCreate(osMailQ(app_sync_mailbox), NULL);
        if (app_sync_mailbox == NULL) {
            TRACE(0, "Failed to Create app_stamp_mailbox");
            return false;
        }

        app_bt_sync_thread_tid = osThreadCreate(osThread(app_bt_sync_thread), NULL);
        if (app_bt_sync_thread_tid == NULL) {
            TRACE(0, "Failed to Create app_bt_sync_thread");
            return false;
        }

        app_bt_sync_send_trigger_info_timer = osTimerCreate(osTimer(APP_BT_SYNC_SEND_TRIGGER_INFO), osTimerOnce, NULL);
        if (app_bt_sync_send_trigger_info_timer == NULL)
        {
            TRACE(0, "Warning: failed to Create app_bt_sync_timer");
        }

        app_bt_sync_timer_mutex_id = osMutexCreate(osMutex(app_bt_sync_timer_mutex));
        if (app_bt_sync_timer_mutex_id == NULL)
        {
            TRACE(0, "Warning: failed to Create app_bt_sync_timer_mutex");
        }

        memset(&app_bt_sync_local_env, 0x00, sizeof(app_bt_sync_env));
        app_bt_sync_initilized = true;
    }
    return true;
}

bt_trigger_info_s *app_bt_sync_save_trigger_info(uint8_t triSrc, uint32_t opCode, uint32_t triTick)
{
    uint8_t index = APP_BT_SYNC_CHANNEL_MAX;

    if (!app_bt_sync_module_init()) {
        ASSERT(0, "%s: failed to initilize", __func__);
    }

    if (APP_BT_SYNC_INVALID_CHANNEL != triSrc) { //!< trigger source already configured
        ASSERT((RESERVED_TRIGGER_CHNL_CNT <= triSrc) && (HW_SUPPORTED_TRIGGER_CHNL > triSrc), "invalid trigger source:%d", triSrc);
        index = triSrc - RESERVED_TRIGGER_CHNL_CNT;
    } else { //!< alloc a new trigger configuration
        for (index = 0; index < APP_BT_SYNC_CHANNEL_MAX; index++) {
            if (!locTriggerInfo[index].used) {
                break;
            }
        }
    }

    if (index >= APP_BT_SYNC_CHANNEL_MAX) {
        TRACE(1, "%s: channel exhausted", __func__);
        return NULL;
    }

    locTriggerInfo[index].used = true;
    locTriggerInfo[index].opCode = opCode;
    /// currently, channel0 reserved for A2DP/HFP, channel1 reserved for DAC2
    locTriggerInfo[index].triSrc = index + RESERVED_TRIGGER_CHNL_CNT;
    locTriggerInfo[index].triTick = triTick;

    return &locTriggerInfo[index];
}

void app_bt_sync_set_handler(uint8_t triSrc)
{
    hal_cmu_bt_trigger_set_handler((enum HAL_CMU_BT_TRIGGER_SRC_T)triSrc, app_bt_sync_irq_handler);
    hal_cmu_bt_trigger_enable((enum HAL_CMU_BT_TRIGGER_SRC_T)triSrc);
}

bool app_bt_sync_enable(uint32_t opCode)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    bt_trigger_info_s *pBuff = NULL;
    bt_trigger_info_s trigger_info_slave;
    uint32_t tg_tick = 0;
    uint32_t curr_ticks = 0;
    uint8_t btRole = BTIF_BCR_MASTER;
    uint32_t delayUs = 0;
    uint16_t twsSnifferInterval = 0;

    btif_cmgr_handler_t *cmgrHandler = btif_lock_free_cmgr_get_acl_handler(p_ibrt_ctrl->p_tws_remote_dev);

    TRACE(4, "%s++ tws handle 0x%x opCode %d", __func__,
        p_ibrt_ctrl->tws_conhandle, opCode);

    if ((p_ibrt_ctrl->tws_conhandle == INVALID_HANDLE) || (cmgrHandler == NULL)) {
        TRACE(1, "%s: TWS link not exist", __func__);
        return false;
    }

    btRole = btif_me_get_remote_device_role(p_ibrt_ctrl->p_tws_remote_dev);

    if (p_ibrt_ctrl->tws_mode == IBRT_SNIFF_MODE) {
        app_tws_ibrt_exit_sniff_with_tws();
        twsSnifferInterval = btif_cmgr_get_cmgrhandler_sniff_interval(cmgrHandler);
        TRACE(1, "%s: TWS in sniff mode,interval %d", __func__, twsSnifferInterval);
        delayUs = twsSnifferInterval * 1000 * 2;
    } else {
        delayUs = p_ibrt_ctrl->acl_interval * 625 * 3;
    }

    if (BTIF_BCR_MASTER == btRole) {
        curr_ticks = bt_syn_get_curr_ticks(p_ibrt_ctrl->tws_conhandle);
        tg_tick = curr_ticks + US_TO_BTCLKS(delayUs);

        pBuff = app_bt_sync_save_trigger_info(APP_BT_SYNC_INVALID_CHANNEL, opCode, tg_tick);
        if (pBuff == NULL) {
            TRACE(1, "%s: channel exhausted", __func__);
            return false;
        }
        TRACE(5, "%s: triSrc %d, tg_tick 0x%x delayUs %d", __func__,
            pBuff->triSrc, tg_tick, delayUs);
    } else {
        pBuff = &trigger_info_slave;
        trigger_info_slave.opCode = opCode;
    }

    if (tws_ctrl_send_cmd_high_priority(APP_TWS_CMD_SET_SYNC_TIME, (uint8_t*)pBuff, sizeof(bt_trigger_info_s))) {
        return false;
    } else {
        if (btRole == BTIF_BCR_MASTER) {
            bt_syn_set_tg_ticks(pBuff->triTick, p_ibrt_ctrl->tws_conhandle,    \
                BT_TRIG_MASTER_ROLE, pBuff->triSrc, false);

            app_bt_sync_set_handler((enum HAL_CMU_BT_TRIGGER_SRC_T)pBuff->triSrc);
        }
    }

    app_bt_sync_add_waiting_irq_timeout_supervision(opCode);
    return true;
}
#endif

void app_bt_sync_send_tws_cmd_done(uint8_t *ptrParam, uint16_t paramLen)
{
#if defined(__BT_SYNC__)
    if (paramLen < sizeof(bt_trigger_info_s)) {
        TRACE(1, "%s: length error", __func__);
        return;
    }

    bt_trigger_info_s triInfo;
    memcpy((uint8_t*)&triInfo, ptrParam, sizeof(bt_trigger_info_s));

    app_bt_sync_status_info_s *statusInfo = NULL;

    statusInfo = app_bt_sync_get_status_instance(triInfo.opCode);
    if (statusInfo) {
        statusInfo->twsCmdSendDone = true;
    }
#endif
}

void app_bt_sync_tws_cmd_handler(uint8_t *p_buff, uint16_t length)
{
#if defined(__BT_SYNC__)
    if (length < sizeof(bt_trigger_info_s)) {
        TRACE(1, "%s: length error", __func__);
        return;
    }

    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    uint32_t curr_ticks = 0;
    uint8_t btRole = BTIF_BCR_MASTER;
    bt_trigger_info_s *pBuff = NULL;
    bt_trigger_info_s triInfo;
    memcpy((uint8_t*)&triInfo, p_buff, sizeof(bt_trigger_info_s));

    if (p_ibrt_ctrl->tws_conhandle == INVALID_HANDLE) {
        TRACE(1, "%s: TWS link not exist", __func__);
        return;
    }

    btRole = btif_me_get_remote_device_role(p_ibrt_ctrl->p_tws_remote_dev);

    TRACE(3, "%s++ triSrc %d opCode %d role %d tick 0x%x", __func__,
        triInfo.triSrc, triInfo.opCode, btRole, triInfo.triTick);

    if (BTIF_BCR_SLAVE == btRole) {
        curr_ticks = bt_syn_get_curr_ticks(p_ibrt_ctrl->tws_conhandle);
        if (curr_ticks >= triInfo.triTick){
            TRACE(1, "%s: exceed curTicks", __func__);
        }

        pBuff = app_bt_sync_save_trigger_info(triInfo.triSrc, triInfo.opCode, triInfo.triTick);
        if (pBuff) {
            bt_syn_set_tg_ticks(pBuff->triTick, p_ibrt_ctrl->tws_conhandle,
                BT_TRIG_SLAVE_ROLE, pBuff->triSrc, false);
            app_bt_sync_set_handler((enum HAL_CMU_BT_TRIGGER_SRC_T)pBuff->triSrc);
        }
    }
    else if (BTIF_BCR_MASTER == btRole)
    {
        app_bt_sync_enable(triInfo.opCode);
    } else {
        TRACE(1, "%s: error case", __func__);
    }
#endif
}
