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
#ifndef __APP_SPP_H__
#define __APP_SPP_H__

#include "spp_api.h"
#include "sdp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__3M_PACK__)
#define L2CAP_MTU                           980
#else
#define L2CAP_MTU                           672
#endif

#define SPP_RECV_BUFFER_SIZE   L2CAP_MTU*4
#define SPP_MAX_DATA_PACKET_SIZE    L2CAP_MTU


/*---------------------------------------------------------------------------
 * rfcomm channel number
 *      should be from 1 to 30
 */
enum RFCOMM_CHANNEL_NUM {
    RFCOMM_CHANNEL_GS_CONTROL   = RFCOMM_CHANNEL_1,
    RFCOMM_CHANNEL_GS_AUDIO     = RFCOMM_CHANNEL_2,
    RFCOMM_CHANNEL_TOTA         = RFCOMM_CHANNEL_3,
    RFCOMM_CHANNEL_BES_OTA      = RFCOMM_CHANNEL_4,
    RFCOMM_CHANNEL_AI_VOICE     = RFCOMM_CHANNEL_5,
    RFCOMM_CHANNEL_GREEN        = RFCOMM_CHANNEL_6,
    RFCOMM_CHANNEL_RED          = RFCOMM_CHANNEL_7,
    RFCOMM_CHANNEL_FP           = RFCOMM_CHANNEL_8,
    RFCOMM_CHANNEL_AMA          = RFCOMM_CHANNEL_9,
    RFCOMM_CHANNEL_BES          = RFCOMM_CHANNEL_10,
    RFCOMM_CHANNEL_BAIDU        = RFCOMM_CHANNEL_11,
    RFCOMM_CHANNEL_TENCENT      = RFCOMM_CHANNEL_12,
    RFCOMM_CHANNEL_ALI          = RFCOMM_CHANNEL_13,
    RFCOMM_CHANNEL_COMMON        = RFCOMM_CHANNEL_14,
    RFCOMM_CHANNEL_COMMON_RECORD = RFCOMM_CHANNEL_15,

    RFCOMM_CHANNEL_CUSTOM_0     = RFCOMM_CHANNEL_16,
    RFCOMM_CHANNEL_CUSTOM_1     = RFCOMM_CHANNEL_17,
    RFCOMM_CHANNEL_CUSTOM_2     = RFCOMM_CHANNEL_18,
    RFCOMM_CHANNEL_CUSTOM_3     = RFCOMM_CHANNEL_19,
    RFCOMM_CHANNEL_CUSTOM_4     = RFCOMM_CHANNEL_20,
    RFCOMM_CHANNEL_CUSTOM_5     = RFCOMM_CHANNEL_21,
};

/*---------------------------------------------------------------------------
 *            app_spp_alloc_server_id
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    To alloc a spp server ID
 *
 * Parameters:
 *    void
 *
 * Return:
 *    uint64_t: the app_id that spp server get
 */
uint64_t app_spp_alloc_server_id(void);
/*---------------------------------------------------------------------------
 *            app_spp_free_server_id
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    To free a spp server ID
 *
 * Parameters:
 *    app_id: the app_id that spp server want to free
 *
 * Return:
 *    true: free success
 *    false: free fail
 */
bool app_spp_free_server_id(uint64_t app_id);
/*---------------------------------------------------------------------------
 *            app_spp_alloc_client_id
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    To alloc a spp client ID
 *
 * Parameters:
 *    void
 *
 * Return:
 *    uint64_t: the app_id that spp client get
 */
uint64_t app_spp_alloc_client_id(void);
/*---------------------------------------------------------------------------
 *            app_spp_free_client_id
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    To free a spp server ID
 *
 * Parameters:
 *    app_id: the app_id that spp client want to free
 *
 * Return:
 *    true: free success
 *    false: free fail
 */
bool app_spp_free_client_id(uint64_t app_id);

struct spp_device *app_create_spp_device(void);

bt_status_t app_spp_send_data(struct spp_device *osDev_t, uint8_t* ptrData, uint16_t *length);
void app_spp_open(struct spp_device *osDev_t, btif_remote_device_t  *btDevice, btif_sdp_record_param_t *param, osMutexId mid, uint8_t service_id, spp_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif

