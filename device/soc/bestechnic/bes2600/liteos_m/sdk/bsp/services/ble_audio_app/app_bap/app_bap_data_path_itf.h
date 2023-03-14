/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef APP_BAP_DATA_PATH_ITF_H_
#define APP_BAP_DATA_PATH_ITF_H_
#if BLE_AUDIO_ENABLED

/*****************************header include********************************/
#include "data_path.h"

/******************************macro defination*****************************/

/******************************type defination******************************/
/// SDU Buffer structure
typedef struct
{
    /// Time_Stamp
    uint32_t        time_stamp;
    /// Packet Sequence Number
    uint16_t        pkt_seq_nb;
    /// length of the ISO SDU (in bytes)
    uint16_t        sdu_length;
    /// Channel identifier
    uint8_t         channel;
    /// Reception status (@see enum hci_iso_pkt_stat_flag)
    uint8_t         status;
    /// SDU
    uint8_t         *sdu;
} dp_itf_iso_buffer_t;

/****************************function declearation**************************/
#ifdef __cplusplus
extern "C" {
#endif

void app_bap_set_rx_dp_itf(struct data_path_itf *itf);
struct data_path_itf *app_bap_get_rx_dp_itf(void);
bool app_bap_dp_itf_get_rx_data(uint8_t channel, dp_itf_iso_buffer_t *iso_buffer);
void app_bap_dp_itf_rx_data_done(uint8_t channel, uint16_t sdu_len, uint32_t ref_time, uint8_t* p_buf);
void app_bap_set_tx_dp_itf(struct data_path_itf *itf);
struct data_path_itf *app_bap_get_tx_dp_itf(void);
void app_bap_dp_itf_send_data(uint8_t channel, uint8_t *payload, uint16_t payload_len, uint32_t ref_time);
void app_bap_dp_itf_data_come_callback_register(void *callback);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_DATA_PATH_ITF_H_

/// @} APP_BAP
