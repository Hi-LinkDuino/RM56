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
#ifndef __IBRT_MGR_ENTRY_H__
#define __IBRT_MGR_ENTRY_H__


void ibrt_mgr_mobile_device_on_event(ibrt_mgr_mobile_sm_t *p_mobile_link_sm,ibrt_mgr_evt_t mgr_evt,uint32_t param);
void ibrt_mgr_mobile_link_mgr_recv_notify_event(uint32_t param0,uint32_t param1);
void ibrt_mgr_mobile_link_mgr_recv_mgr_event(ibrt_mgr_event_t *p_mgr_evt);
void ibrt_mgr_start_run_evt_on_mobile_link(ibrt_mgr_mobile_sm_t* mobile_link_sm,ibrt_mgr_evt_t trigger_evt, uint32_t param0 = 0,uint32_t param1 = 0);

void ibrt_mgr_super_state_machine_on_event(ibrt_mgr_evt_t evt,ibrt_mgr_evt_t extra_evt = IBRT_MGR_EV_NONE,uint8_t link_id = MOBILE_LINK_ID_INVALID);;
void ibrt_mgr_handle_event(ibrt_mgr_event_packet *pkt);




#endif
