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
#ifndef __IBRT_MGR_EVT_MEDIATOR_H__
#define __IBRT_MGR_EVT_MEDIATOR_H__

void ibrt_mgr_dev_thread_init();
int app_ibrt_mgr_evt_put_mailbox(ibrt_conn_evt_header *evt);
uint8_t app_ibrt_mgr_get_event_packet_size(ibrt_conn_event type);
ibrt_mgr_evt_t app_ibrt_mgr_map_conn_event(ibrt_mgr_event_packet* ev_pkt);

void ibrt_mgr_evt_set_next_event(ibrt_mgr_evt_t evt,ibrt_mgr_evt_t extra_evt = IBRT_MGR_EV_NONE,uint8_t link_id = MOBILE_LINK_ID_INVALID);


#endif

