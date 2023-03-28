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
#ifndef __APP_IBRT_CUSTOMIF_UI_UI__
#define __APP_IBRT_CUSTOMIF_UI_UI__

#define  IBRT_UI_SCAN_INTERVAL_IN_SCO_TWS_DISCONNECTED           (BTIF_BT_DEFAULT_PAGE_SCAN_INTERVAL)
#define  IBRT_UI_SCAN_WINDOW_IN_SCO_TWS_DISCONNECTED             (BTIF_BT_DEFAULT_PAGE_SCAN_WINDOW)

#define  IBRT_UI_SCAN_INTERVAL_IN_SCO_TWS_CONNECTED              (BTIF_BT_DEFAULT_PAGE_SCAN_INTERVAL+0x400)
#define  IBRT_UI_SCAN_WINDOW_IN_SCO_TWS_CONNECTED                (BTIF_BT_DEFAULT_PAGE_SCAN_WINDOW)

#define  IBRT_UI_SCAN_INTERVAL_IN_A2DP_TWS_DISCONNECTED          (BTIF_BT_DEFAULT_PAGE_SCAN_INTERVAL)
#define  IBRT_UI_SCAN_WINDOW_IN_A2DP_TWS_DISCONNECTED            (BTIF_BT_DEFAULT_PAGE_SCAN_WINDOW)

#define  IBRT_UI_SCAN_INTERVAL_IN_A2DP_TWS_CONNECTED             (BTIF_BT_DEFAULT_PAGE_SCAN_INTERVAL+0x400)
#define  IBRT_UI_SCAN_WINDOW_IN_A2DP_TWS_CONNECTED               (BTIF_BT_DEFAULT_PAGE_SCAN_WINDOW)

int app_ibrt_customif_ui_start(void);
void app_ibrt_customif_ui_tws_switch(void);
bool app_ibrt_customif_ui_is_tws_switching(void);
void app_ibrt_customif_tws_ui_role_updated(uint8_t newRole);
void app_ibrt_customif_get_tws_side_handler(APP_TWS_SIDE_T* twsSide);
#ifdef USE_BT_ADAPTER
typedef struct {
    void (*harmony_gap_tws_pairing_changed_evt)(ibrt_conn_pairing_state state,uint8_t reason_code, uint8_t ui_role);
    void (*harmony_gap_tws_acl_state_changed_evt)(ibrt_conn_acl_state state,uint8_t reason_code);
    void (*harmony_gap_mobile_acl_state_changed_evt)(const bt_bdaddr_t *addr,ibrt_conn_acl_state state,uint8_t reason_code);
    void (*harmony_gap_access_mode_changed_evt)(btif_accessible_mode_t newAccessMode);
    void (*harmony_gap_mobile_ibrt_state_changed_evt)(const bt_bdaddr_t *addr, ibrt_conn_ibrt_state state, ibrt_role_e role, uint8_t reason_code);
} app_harmony_bt_gap_event_cbs;
void app_harmony_bt_gap_register_ctx_cb(const app_harmony_bt_gap_event_cbs *cb);
#endif


#endif /*__APP_IBRT_CUSTOMIF_UI_UI__*/
