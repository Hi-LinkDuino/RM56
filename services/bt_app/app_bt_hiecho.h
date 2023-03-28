
/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#ifndef __APP_BT_HIECHO_H__
#define __APP_BT_HIECHO_H__

#ifdef HIECHO_ENABLE

#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

//bit19 = 1, support hiaudio
#define HIECHO_SUPPORT_AUDIO_CAP    (1<<19)

//mode = 1, snoop mode
#define HIECHO_SUPPORT_MODE         0x01

//default version:0
#define HIECHO_TWS_VERSION          0x00

enum HiechoProfile {
    HIECHO_ERR = 0,
    HIECHO_IDLE = 1,
    HIECHO_BUSY = 2
};

enum HiechoPairType {
    PAIR_NORMAL = 0x00,
    PAIR_ABNORMAL = 0x01
};

enum HiechoProduct {
    HIECHO_PHONE = 0x01,
    HIECHO_WATCH = 0x02,
    HIECHO_EARBUD = 0x03
};

enum HiechoSupportCap {
    SERVICE_CAP_INQUIRY     = 0x1,
    SCENE_MANAGE            = 0x2,
    PAIRING_CONNECTION      = 0x5,
    PORTABLE_RING           = 0x6,
    AVOID_KEY_PAIRING       = 0x7,
    HIECHO_AUDIO            = 0x8,
    UNSUPPORT_CAP           = 0xFF
};

typedef void (*APP_BT_HIECHO_RSP_HANDLE_IND) (bt_bdaddr_t* device_addr, uint8_t* rsp_data, uint8_t rsp_data_len);
typedef void (*APP_BT_HIECHO_REQ_HANDLE_IND) (bt_bdaddr_t* device_addr,uint8_t id, uint8_t* req_data, uint8_t req_data_len);

void app_bt_recv_hiecho_req_data(uint8_t echoType, uint8_t* rsp_data, uint16_t rsp_data_len, uint8_t* args);
void app_bt_recv_hiecho_rsp_data(uint8_t echoType, uint8_t* rsp_data, uint16_t rsp_data_len, uint8_t* args);
void app_bt_send_hiecho_req_data(uint8_t* device_addr, uint8_t *data, uint16_t data_len);
void app_bt_send_hiecho_rsp_data(uint8_t* args, uint8_t *data, uint16_t len);

void app_bt_hiecho_req_data_handle_cb(bt_bdaddr_t* device_addr, uint8_t id, uint8_t* req_data, uint8_t req_data_len);
void app_bt_hiecho_rsp_data_handle_cb(bt_bdaddr_t* device_addr, uint8_t* rsp_data, uint8_t rsp_data_len);
void app_bt_register_hiecho_req_data_handle(APP_BT_HIECHO_REQ_HANDLE_IND handler);
void app_bt_register_hiecho_rsp_data_handle(APP_BT_HIECHO_RSP_HANDLE_IND handler);
void app_bt_hiecho_init(void);

void app_bt_hiecho_set_device_audio_output_to_speaker(const bt_bdaddr_t* addr,bool sco,bool a2dp);
void app_bt_hiecho_set_device_audio_to_bluetooth(const bt_bdaddr_t* addr,bool sco,bool a2dp);
void app_bt_hiecho_capability_inquiry(const bt_bdaddr_t* device_addr);

typedef void (*APP_BT_HIECHO_CUSTOMIF_ECHO_REQ_HANDLE_IND) (uint8_t echoType, uint8_t* req_data, uint16_t req_data_len, uint8_t* args);
void register_l2cap_echo_req_handler_callback(APP_BT_HIECHO_CUSTOMIF_ECHO_REQ_HANDLE_IND handler);

typedef void (*APP_BT_HIECHO_CUSTOMIF_ECHO_RSP_HANDLE_IND) (uint8_t echoType, uint8_t* rsp_data, uint16_t rsp_data_len, uint8_t* args);
void register_l2cap_echo_rsp_handler_callback(APP_BT_HIECHO_CUSTOMIF_ECHO_RSP_HANDLE_IND handler);

void app_bt_send_hiecho_req_data_by_adapter(uint8_t* addr,uint8_t* source,uint16_t dataLen);
void app_bt_send_hiecho_rsp_data_by_adapter(uint8_t* args,uint8_t* rspDta,uint16_t dataLen);
void app_bt_hiecho_sdp_query_capability(const bt_bdaddr_t* device_addr);


#ifdef __cplusplus
}
#endif

#endif /* HIECHO_ENABLE */

#endif /* __APP_BT_HIECHO_H__ */

