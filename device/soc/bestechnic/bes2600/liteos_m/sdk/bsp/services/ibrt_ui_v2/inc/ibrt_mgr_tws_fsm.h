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
#ifndef __IBRT_MGR_TWS_FSM_H__
#define __IBRT_MGR_TWS_FSM_H__

typedef enum {
  TWS_STATE_DISCONNECTED = 0,
  TWS_STATE_CONNECTING,
  TWS_STATE_CONNECTED,
} tws_fsm_states;

typedef void (*submit_msg_to_super)(ibrt_mgr_evt_t evt,ibrt_mgr_evt_t extra_evt);

typedef struct
{
    tws_fsm_states curr_state;
    ibrt_mgr_evt_t active_event;
    osTimerId   nv_slave_delay_tws_timer_id;
    osTimerId   tws_reconnect_timer_id;

    bool is_running;
    submit_msg_to_super submit_msg_func;

    uint8_t tws_try_reconnect_times;
    uint8_t open_box_try_reconnect_times;
    ibrt_mgr_evt_t reconnect_event;
}ibrt_mgr_tws_link_fsm_t;


void ibrt_mgr_tws_fsm_init(submit_msg_to_super submit_msg_cb);
void ibrt_mgr_tws_fsm_handle_event(ibrt_mgr_evt_t evt, uint8_t reasonCode = 0);


#endif

