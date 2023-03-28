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
#ifndef __APP_TWS_PROFILE_SYNC__
#define __APP_TWS_PROFILE_SYNC__

#include "spp_api.h"
#include "app_tws_ibrt.h"

#define COMMON_DATA_ARRAY_LEN           (164)
#define RFC_MUX_DATA_ARRAY_LEN          (196)
#define HFP_DATA_ARRAY_LEN              (286)
#define A2DP_DATA_ARRAY_LEN             (256)
#define A2DP_CONTINUE_DATA_ARRAY_LEN    (564)
#define AVRCP_DATA_ARRAY_LEN            (612)
#define MAP_DATA_ARRAY_LEN              (256)
#define SPP_DATA_ARRAY_LEN              (256)
#define BTGATT_DATA_ARRAY_LEN           (256)

typedef struct
{
    uint8_t common_data[COMMON_DATA_ARRAY_LEN];
    uint8_t rfc_mux_data[RFC_MUX_DATA_ARRAY_LEN];
    uint8_t hfp_data[HFP_DATA_ARRAY_LEN];
    uint8_t a2dp_data[A2DP_DATA_ARRAY_LEN];
    uint8_t a2dp_continue_data[A2DP_CONTINUE_DATA_ARRAY_LEN];
    uint8_t avrcp_data[AVRCP_DATA_ARRAY_LEN];
    uint8_t map_data[MAP_DATA_ARRAY_LEN];
    uint8_t btgatt_data[BTGATT_DATA_ARRAY_LEN];
    uint8_t spp_data[SPP_DEVICE_NUM][SPP_DATA_ARRAY_LEN];

    uint16_t common_data_len;
    uint16_t rfc_mux_data_len;
    uint16_t hfp_data_len;
    uint16_t a2dp_data_len;
    uint16_t a2dp_continue_data_len;
    uint16_t avrcp_data_len;
    uint16_t map_data_len;
    uint16_t btgatt_data_len;
    uint16_t spp_data_len[SPP_DEVICE_NUM];
    uint64_t app_id[SPP_DEVICE_NUM];
    uint8_t  spp_amount;
    uint8_t  spp_num;
} data_store_mem_t;


void app_tws_profile_data_sync(uint8_t *p_buff, uint16_t length);
void app_tws_parse_profile_data(ibrt_mobile_info_t *p_mobile_info, uint8_t *p_buff, uint32_t length);
uint32_t app_tws_profile_data_tx(ibrt_mobile_info_t *p_mobile_info,uint8_t flag,uint8_t *buf);
uint32_t app_tws_profile_data_rx(ibrt_mobile_info_t *p_mobile_info, uint8_t flag,uint8_t *buf,uint32_t length);
void app_tws_profile_data_save_temporarily(ibrt_mobile_info_t *p_mobile_info,uint8_t *p_buff, uint32_t length);
bool app_tws_profile_data_rx_needed(ibrt_mobile_info_t *p_mobile_info,uint8_t profile_flag);
bool app_tws_profile_data_rx_completed(uint8_t final_flag);
void app_tws_profile_rx_parse(ibrt_mobile_info_t *p_mobile_info);
uint8_t app_tws_profile_data_tx_filter(ibrt_mobile_info_t *p_mobile_info,uint64_t profile_mask,uint8_t data_array[][8]);
bool app_tws_profile_data_tx_allowed(ibrt_mobile_info_t *p_mobile_info);
bool app_tws_profile_connecting(ibrt_mobile_info_t *p_mobile_info);
uint64_t app_tws_profile_mapping_data_fragment(uint8_t flag);
void app_tws_profile_resume_a2dp_hfp(ibrt_mobile_info_t *p_mobile_info);
void app_tws_profile_disconnect_old_profiles(ibrt_mobile_info_t *p_mobile_info,uint8_t profile_flag,uint8_t device_id);
void app_tws_profile_remove_from_basic_profiles(bt_bdaddr_t *p_mobile_addr,uint64_t profile_id);

#endif

