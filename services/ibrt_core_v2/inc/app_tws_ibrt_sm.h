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
#ifndef __APP_IBRT_SM_H__
#define __APP_IBRT_SM_H__

#if defined(IBRT) && defined(IBRT_CORE_V2_ENABLE)

#include "hsm.h"

#define IBRT_SM_EVENT_BASE    0x4000

typedef enum
{
    //Messages for handling connect/disconnect requests.
    SIGNAL_START_IBRT = IBRT_SM_EVENT_BASE,
    SIGNAL_START_MSS,
    SIGNAL_START_SET_ENV,
    SIGNAL_MOBILE_ENTER_ACTIVE_MODE,
    SIGNAL_STOP_IBRT,
    SIG_RETRY_START_IBRT,

    // Messages for handling error conditions.
    
    // Messages for feedback from custom API.
    EVT_MOBILE_MSS_COMPLETEED,
    EVT_SET_ENV_COMPLETEED,
    EVT_START_IBRT_COMPLETED,
    EVT_STOP_IBRT_COMPLETED,
    EVT_ENTER_ACTIVE_MODE_COMPLETED,
    EVT_DATA_EXCHANGE_COMPLETED,

    EVT_IBRT_TWS_DISCONNECTED,
    EVT_IBRT_MOBILE_DISCONNECTED,

    EVT_IBRT_MAX_NUM,
} ibrt_sm_message_e;
typedef void (*ibrt_sm_post_func)(void);
typedef void (*ibrt_state_changed_func)(const bt_bdaddr_t *addr,ibrt_conn_ibrt_state state,ibrt_role_e role,uint8_t reason_code);

typedef struct
{
    Hsm ibrt_sm;

    State idle;
    State w4_profile_exchanged;
    State w4_mss_changed;
    State w4_set_env;
    State w4_start_ibrt_complete;
    State w4_active_mode;
    State w4_stop_ibrt_complete;

    uint8_t used;
    uint8_t device_id;
    bt_bdaddr_t current_mobile_addr;
    ibrt_sm_post_func stop_ibrt_post_func;
    ibrt_state_changed_func state_change_cb;
    ibrt_mobile_info_t* ibrt_extra_mobile_info;
    uint8_t reconnect_ibrt_max_times;
    uint8_t reconnect_ibrt_try_times;
    uint8_t restop_ibrt_try_times;
    uint8_t try_mss_max_times;
    uint8_t try_mss_times;

    osTimerId           restart_ibrt_timer_id;
    osTimerDefEx_t      restart_ibrt_timer_def;

    // inner use
    bool already_report_ibrt_connected;
} ibrt_state_machine_t;

#ifdef __cplusplus
extern "C" {
#endif

void app_tws_ibrt_sm_initialize(ibrt_state_machine_t *p_ibrt_link_sm);
void app_tws_ibrt_sm_start(ibrt_state_machine_t *ibrt_sm);
bool app_tws_ibrt_sm_is_idle_state(ibrt_state_machine_t* ibrt_sm);
bool app_tws_ibrt_sm_is_w4_data_exchange(ibrt_state_machine_t* ibrt_sm);
bool app_tws_ibrt_sm_is_w4_ibrt(ibrt_state_machine_t* ibrt_sm);
int app_tws_ibrt_send_ibrt_msg(uint32_t ibrt_sm_ptr,ibrt_sm_message_e evt,uint32_t param0 = 0,uint32_t param1 = 0);
int app_tws_ibrt_sm_on_event(unsigned int event,unsigned int ibrt_link_ptr,unsigned int param0 = 0, unsigned int param1 = 0);
void app_tws_ibrt_set_exchange_profiles_status(ibrt_state_machine_t *ibrt_sm,bool status);
void app_tws_ibrt_fast_free_ibrt_link_sm(ibrt_state_machine_t *p_ibrt_link_sm,uint8_t reason);


#ifdef __cplusplus
}
#endif

#endif

#endif /*__APP_IBRT_SM_H__ */
