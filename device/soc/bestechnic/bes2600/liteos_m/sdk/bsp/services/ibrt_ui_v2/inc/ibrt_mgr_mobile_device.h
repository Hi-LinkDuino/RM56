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
#ifndef __IBRT__MGR_MOBILE_CONN_HSM__
#define __IBRT__MGR_MOBILE_CONN_HSM__


#include "stdint.h"
#include "cmsis_os.h"
#include "hsm.h"

typedef void (*notify_mobile_status_changed)(uint16_t link_id,ibrt_mgr_evt_t status,uint8_t reason);
typedef void (*notify_ibrt_status_changed)(uint16_t link_id,ibrt_mgr_evt_t status,tws_role_e role);
typedef bool (*unify_tws_role)(uint16_t link_id,tws_role_e curr_role);

typedef enum
{
    ROLE_UNCONFIRMED,
    ROLE_CONFIRMING,
    ROLE_CONFIRMED,
}mobile_link_role_status_e;

typedef struct
{
	notify_ibrt_status_changed notify_ibrt_status_changed_func;
	notify_mobile_status_changed on_mobile_status_changed_cb;
	unify_tws_role unify_role_cb;
}ibrt_mgr_mobile_link_cbs_t;

typedef struct
{
	ibrt_mgr_evt_t complete_evt;
	ibrt_mgr_extra_info_type_t extra_info;
    ibrt_mgr_evt_t latest_evt;
}link_evt_run_result_t;

typedef enum
{
    RECONNECT_NONE,
    LINK_LOSS,
    TRY_RECONNECT,
}reconnect_reason_t;

typedef struct
{
    Hsm mobile_sm;
    State mobile_disconnected;
    State mobile_connecting;
    State mobile_connected;
    State ibrt_connected;
    State role_switching;

    bool already_startup;
    osMutexId link_queue_mutex;
    osMutexDefEx_t link_queue_mutex_def;
    osTimerId reconnect_delay_timer_id;
    osTimerDefEx_t evt_delay_handler_timer_def;

    bool is_vaild;
    bool hold_run_permission;
    bool backup_run_permission;    /* only used as flag, remove it later */
    bt_bdaddr_t current_addr;
    uint16_t link_id;
    uint16_t running_group_mask;   /* identify which link is still running*/
    tws_role_e current_role;
    mobile_link_role_status_e role_status;
    ibrt_mgr_evt_t active_evt;
    bool last_evt_run_complete;
    ibrt_box_state box_state;
    ibrt_mgr_evt_queue_t local_link_evt_que;
    ibrt_mgr_evt_queue_t peer_link_evt_que;
    unify_tws_role unify_role_cb;
    notify_mobile_status_changed notify_mobile_status_changed_cb;
    notify_ibrt_status_changed notify_ibrt_status_cb;
    ibrt_conn_req_hdr_t   local_req_hdr;
    ibrt_share_link_info  peer_share_info;
    ibrt_req_info_type_t  notify_type;

    ibrt_mgr_evt_t latest_evt;         /* inner use */
    bool notify_flag;                       /* inner use */
    uint8_t mobile_try_reconnect_times;     /* mobile reconnect counter    */
    reconnect_reason_t reconnect_reason;    /* record reason for distinguish from open try or link loss try */              
    uint8_t disconnect_reason;              /* disconnect reason from ctl   */
    bool exchangeinfo_req;

    uint16_t peer_w4_disc_rsp_seq;

    link_evt_run_result_t evt_run_result;
} ibrt_mgr_mobile_sm_t;

#ifdef __cplusplus
extern "C" {
#endif

void ibrt_mgr_mobile_sm_init(uint16_t link_id,ibrt_mgr_mobile_sm_t* mobile_link_sm);
void ibrt_mgr_destory_mobile_device(ibrt_mgr_mobile_sm_t* mobile_link_sm);
void ibrt_mgr_mobile_reset_link_info(ibrt_mgr_mobile_sm_t* mobile_link_sm);
int ibrt_mgr_mobile_sm_on_event(ibrt_mgr_mobile_sm_t* sm_ptr,uint32_t event, uint32_t param0=0,uint32_t param1=0);
void ibrt_mgr_register_mobile_link_cbs(ibrt_mgr_mobile_sm_t* mobile_link_sm,const ibrt_mgr_mobile_link_cbs_t *cbs);
void ibrt_mgr_mobile_sm_release_hold_permission(ibrt_mgr_mobile_sm_t* mobile_link_sm);
bool ibrt_mgr_is_curr_mobile_link_sm_idle(ibrt_mgr_mobile_sm_t* mobile_link_sm);
bool ibrt_mgr_is_curr_link_hold_run_permission(ibrt_mgr_mobile_sm_t* mobile_link_sm);
void ibrt_mgr_safe_pop_link_queue_evt(ibrt_mgr_mobile_sm_t* mobile_link_sm,bool is_local_queue);
void ibrt_mgr_safe_push_event_to_link_queue(ibrt_mgr_mobile_sm_t* mobile_link_sm,ibrt_mgr_event_t *event,bool is_local_queue);
void ibrt_mgr_set_mobile_link_box_state_by_evt(ibrt_mgr_mobile_sm_t* mobile_link_sm,ibrt_mgr_evt_t box_evt);
void ibrt_mgr_set_mobile_link_box_state(ibrt_mgr_mobile_sm_t* mobile_link_sm,ibrt_box_state box_state);
bool ibrt_mgr_mobile_get_evt_run_status(ibrt_mgr_mobile_sm_t *me);
void ibrt_mgr_mobile_get_evt_run_result(ibrt_mgr_mobile_sm_t *mobile_link_sm,link_evt_run_result_t *run_result);
void ibrt_mgr_mobile_sm_catch_hold_permission(ibrt_mgr_mobile_sm_t* mobile_link_sm);
void ibrt_mgr_mobile_sm_on_event_ext(ibrt_mgr_mobile_sm_t* mobile_link_sm,uint32_t evt);
void ibrt_mgr_mobile_start_w4_mobile_disconnect(ibrt_mgr_mobile_sm_t *me,uint16_t rsp_seq);


#ifdef __cplusplus
}
#endif

#endif

