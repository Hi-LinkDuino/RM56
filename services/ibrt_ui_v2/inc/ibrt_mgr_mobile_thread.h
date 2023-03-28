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
#ifndef __IBRT_MGR_MOBILE_THREAD_H__
#define __IBRT_MGR_MOBILE_THREAD_H__

typedef struct {
    uint32_t msg_id;
    uint32_t param0;
    uint32_t param1;
    uint32_t param2;
}mobile_thread_msg_t;

enum IBRT_MGR_MOBLE_THREAD_REQ_T {
    REQ_MOBILE_HANDLE_EVENT,       /* mobile thread handle IBRT core event */
    REQ_NOTIFY_EVENT,              /* mobile thread handle notify event */
    REQ_MGR_EVENT,                 /* mobile thread handle super mgr submit event */ 

    REQ_NUM
};

osThreadId ibrt_mgr_mobile_thread_init();
void ibrt_mgr_mobile_thread_send_msg(uint32_t message_id,uint32_t param0,uint32_t param1, uint32_t param2 = 0);
void ibrt_mgr_mobile_push_link_to_ready_queue(ibrt_mgr_mobile_sm_t* mobile_link_sm,ibrt_mgr_evt_t event,uint32_t param0 = 0,uint32_t param1 = 0);

#endif
