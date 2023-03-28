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
/***
 * besbt_cfg.h
 */

#ifndef BESBT_CFG_H
#define BESBT_CFG_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct besbt_cfg_t{
    uint16_t dip_vendor_id;
    uint16_t dip_product_id;
    uint16_t dip_product_version;
    bool apple_hf_at_support;
    bool hf_dont_support_cli_feature;
    bool hf_dont_support_enhanced_call;
    bool hf_support_hf_ind_feature;
    bool sniff;
    bool dont_auto_report_delay_report;
    bool vendor_codec_en;
    bool force_use_cvsd;
    bool support_enre_mode;
    bool bt_hid_cod_enable;
    bool bt_watch_enable;
    bool avdtp_cp_enable;
    bool bt_source_enable;
    bool bt_source_48k;
    bool bt_sink_enable;
    bool a2dp_sink_enable;
    bool lhdc_v3;
    bool hfp_ag_pts_enable;
    bool hfp_ag_pts_ecs_01;
    bool hfp_ag_pts_ecs_02;
    bool hfp_ag_pts_ecc;
    bool source_get_all_cap_flag;
    bool source_suspend_err_flag;
    bool source_unknown_cmd_flag;
    bool acl_tx_flow_debug;
    bool hci_tx_cmd_debug;
    bool dont_play_mute_when_a2dp_stuck;
    bool start_ibrt_reserve_buff;
    bool send_l2cap_echo_req;
    bool a2dp_play_stop_media_first;
    bool disc_acl_after_auth_key_missing;
    bool use_page_scan_repetition_mode_r1;
};
extern struct besbt_cfg_t besbt_cfg;

#ifdef __cplusplus
}
#endif
#endif /* BESBT_H */
