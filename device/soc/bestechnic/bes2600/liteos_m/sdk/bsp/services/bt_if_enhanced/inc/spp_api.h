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

#ifndef __SPP_API_H__
#define __SPP_API_H__
#include "btapp.h"
#include "sdp_api.h"
#include "cmsis_os.h"
#include "cqueue.h"
#include "rfcomm_api.h"

#define SPP_DEVICE_NUM      (SPP_SERVICE_NUM*BT_DEVICE_NUM)

#ifndef SPP_SERVICE_NUM
#define SPP_SERVICE_NUM 5
#endif

typedef uint16_t spp_event_t;
#define BTIF_SPP_EVENT_REMDEV_CONNECTED         0
#define BTIF_SPP_EVENT_REMDEV_DISCONNECTED      1
#define BTIF_SPP_EVENT_DATA_SENT                2
#define BTIF_SPP_EVENT_REMDEV_DISCONNECTED_IND  3
#define BTIF_SPP_EVENT_REMDEV_CONNECTED_IND     4

typedef uint8_t spp_port_t;

#define BTIF_SPP_SERVER_PORT             0x01
#define BTIF_SPP_CLIENT_PORT             0x02

#define SPP_RX_SIGNAL_ID                 0x04

#define BTIF_SPP_MAXIMUM_CREDIT_LIMIT    4

struct spp_device;

struct spp_tx_done {
    uint8_t         *tx_buf;
    uint16_t        tx_data_length;
};

struct spp_client {
    btif_remote_device_t *remDev;
    uint8_t                  serverId;
    btif_sdp_query_token_t   *sdpToken;
    uint8_t* rfcommServiceSearchRequestPtr;
    uint8_t rfcommServiceSearchRequestLen;
};

struct spp_service {
    struct rf_service   rf_service;
    const uint8_t       *name;
    uint16_t            nameLen;
    uint16_t            numPorts;
    btif_sdp_record_t   *sdpRecord;
    uint8_t             service_used_flag;
};

struct spp_callback_parms {
    spp_event_t event;
    int status;
    union {
        void           *other;
        btif_remote_device_t *remDev;
    } p;
};

typedef void (*spp_callback_t)(uint8_t device_id, struct spp_device *locDev,
                            struct spp_callback_parms *Info);

typedef int (*spp_handle_data_event_func_t)(uint8_t device_id, void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen);

struct _spp_dev {
    union {
        struct spp_client   client;
        struct spp_service  *sppService;
    } type;
};

typedef struct {
    uint8_t len;
    const uint8_t *pdata;
} rfcomm_uuid_t;

struct spp_device {
    spp_port_t      portType;
    uint64_t        app_id;
    rfcomm_uuid_t   rfcomm_uuid;
    osThreadId      reader_thread_id;
    osMutexId       mutex_id;
    CQueue          rx_queue;
    uint8_t         *rx_buffer;
    uint32_t        rx_buffer_size;
    uint32_t        tx_packet_num;
    void            *priv;
    spp_callback_t  spp_app_callback;
    spp_handle_data_event_func_t spp_handle_data_event_func;
    uint8_t         serialNumber;
    uint8_t         spp_connected_flag;
    uint8_t         sppUsedFlag;
    uint8_t         close_pending;
    struct _spp_dev sppDev;
    void            *_channel;
    uint8_t         device_id;
    uint8_t         initial_credit;
    uint8_t         credit_give_limit;
    osMutexId       creditMutex;
    bt_bdaddr_t     btaddr;
};

#ifndef container_of
#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#ifdef __cplusplus
extern "C" {
#endif

void btif_spp_server_register_accept_channel_callback(bool (*cb)(uint8_t device_id, uint32_t rfcomm_handle, const void* remote, uint8_t server_channel));
struct spp_device *btif_spp_get_device_from_addr(const void* remote, uint64_t app_id);
struct spp_service *btif_spp_find_registered_service(uint8_t service_id);

bt_status_t btif_spp_init_device(struct spp_device *dev, uint16_t numPackets, osMutexId mid);
bt_status_t btif_spp_init_rx_buf(struct spp_device *dev, uint8_t *rx_buf, uint32_t size);
bool btif_sppos_is_txpacket_available(struct spp_device *dev);
bt_status_t btif_spp_open(struct spp_device *dev,
                        btif_remote_device_t  *btDevice,
                        spp_callback_t callback);
bt_status_t btif_spp_open_client(struct spp_device *dev,
                        bt_bdaddr_t *bdaddr,
                        spp_callback_t callback);
bt_status_t btif_spp_open_server(struct spp_device *dev,
                        spp_callback_t callback);
bt_status_t btif_spp_disconnect(struct spp_device *dev,uint8_t reason);
bt_status_t btif_spp_close(struct spp_device *dev);
void btif_spp_close_device(struct spp_device *dev);
bt_status_t btif_spp_read(struct spp_device *dev, char *buffer, uint16_t *nBytes);
bt_status_t btif_spp_write(struct spp_device *dev, char *buffer, uint16_t *nBytes);
bt_status_t btif_spp_service_setup(struct spp_device *dev,
                                struct spp_service *service,
                                btif_sdp_record_t *record);
bt_status_t btif_ccmp_open(struct spp_device *dev,
                        bt_bdaddr_t  *remote,
                        spp_callback_t callback,
                        uint8_t port);
struct spp_device *btif_create_spp_device(void);
void btif_destroy_spp_device(struct spp_device *dev_t);
struct spp_device *btif_spp_get_device(uint64_t app_id);
struct spp_service *btif_create_spp_service(void);
void btif_destroy_spp_service(struct spp_service *spp_service_p);
uint64_t btif_spp_get_app_id(struct spp_device *dev);
uint32_t btif_spp_get_rfcomm_handle(struct spp_device *dev);
uint8_t btif_spp_get_server_channel(struct spp_device *dev);
const char *btif_spp_event2str(spp_event_t event);

typedef void (*btif_bt_spp_app_callback_func)(uint8_t device_id, void* spp_devi, void* spp_para);
void btif_register_bt_spp_callback_func(btif_bt_spp_app_callback_func func);

#if defined(IBRT)
uint32_t btif_spp_profile_save_ctx(uint64_t app_id,btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t btif_spp_profile_restore_ctx(bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);
void btif_spp_force_disconnect_spp_profile(bt_bdaddr_t *addr,uint64_t app_id,uint8_t reason);
bool btif_is_this_spp_connected(uint64_t app_id,btif_remote_device_t *rem_dev);
#endif

uint16_t btif_spp_get_rfcomm_framesize(struct spp_device *dev);
/*---------------------------------------------------------------------------
 * rfcomm channel number
 *      should be from 1 to 30
 */
enum RFCOMM_CHANNEL_NUM_ {
    RFCOMM_CHANNEL_1 = 10,
    RFCOMM_CHANNEL_2,
    RFCOMM_CHANNEL_3,
    RFCOMM_CHANNEL_4,
    RFCOMM_CHANNEL_5,
    RFCOMM_CHANNEL_6,
    RFCOMM_CHANNEL_7,
    RFCOMM_CHANNEL_8,
    RFCOMM_CHANNEL_9,
    RFCOMM_CHANNEL_10,
    RFCOMM_CHANNEL_11,
    RFCOMM_CHANNEL_12,
    RFCOMM_CHANNEL_13,
    RFCOMM_CHANNEL_14,
    RFCOMM_CHANNEL_15,
    RFCOMM_CHANNEL_16,
    RFCOMM_CHANNEL_17,
    RFCOMM_CHANNEL_18,
    RFCOMM_CHANNEL_19,
    RFCOMM_CHANNEL_20,
    RFCOMM_CHANNEL_21,
};

#ifdef __cplusplus
}
#endif

#endif /* __SPP_API_H__ */
