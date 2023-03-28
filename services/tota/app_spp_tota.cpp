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
#include <stdio.h>
#include "cmsis_os.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_chipid.h"
#include "analog.h"
#include "app_audio.h"
#include "app_status_ind.h"
#include "app_bt_stream.h"
#include "nvrecord.h"
#include "nvrecord_env.h"
//#include "nvrecord_dev.h"

#include "bluetooth.h"
#include "cqueue.h"
#include "resources.h"
#include "app_spp_tota.h"
#include "app_tota_cmd_code.h"
#include "app_tota.h"
#include "app_spp.h"
#include "app_tota_cmd_handler.h"
#include "app_tota_data_handler.h"
#include "plat_types.h"
#include "spp_api.h"
#include "sdp_api.h"
//#include "app_bt_conn_mgr.h"
#ifdef APP_ANC_TEST
#include "anc_parse_data.h"
#endif

#define SPP_SHARE_BUF            0

#if 0
static void tota_spp_read_thread(const void *arg);
osThreadDef(tota_spp_read_thread, osPriorityAboveNormal, 1, 3072, "tota_spp_read");
osThreadId  tota_spp_read_thread_id = NULL;
#endif

static bool isTotaSppConnected = false;
static struct spp_device  *tota_spp_dev = NULL;
osMutexDef(tota_spp_mutex);

static app_spp_tota_tx_done_t app_spp_tota_tx_done_func = NULL;

#if (SPP_SHARE_BUF==1)
extern uint8_t tws_big_buff[PCM_BUFFER_SZ_TWS+TRANS_BUFFER_SZ_TWS+A2DP_BUFFER_SZ_TWS];
static uint8_t* totaSppTxBuf = &tws_big_buff[ANC_PARSE_DATA_BUFF_OFFSET-ANC_PARSE_DATA_BUFF_SIZE - TOTA_SPP_TX_BUF_SIZE];
#else
static uint8_t totaSppTxBuf[TOTA_SPP_TX_BUF_SIZE];
#endif

static uint32_t occupiedTotaSppTxBufSize;
static uint32_t offsetToFillTotaSppTxData;
static uint8_t* ptrTotaSppTxBuf;

uint16_t app_spp_tota_tx_buf_size(void)
{
    return TOTA_SPP_TX_BUF_SIZE;
}

void app_spp_tota_init_tx_buf(uint8_t* ptr)
{
    ptrTotaSppTxBuf = ptr;
    occupiedTotaSppTxBufSize = 0;
    offsetToFillTotaSppTxData = 0;
}

static void app_spp_tota_free_tx_buf(const uint8_t* ptrData, uint32_t dataLen)
{
    if (occupiedTotaSppTxBufSize > 0)
    {
        occupiedTotaSppTxBufSize -= dataLen;
    }
    TOTA_LOG_DBG(1,"occupiedTotaSppTxBufSize %d", occupiedTotaSppTxBufSize);
}

extern "C" bool app_spp_tota_tx_buf_is_available(uint32_t dataLenToSend)
{
    return (occupiedTotaSppTxBufSize + dataLenToSend) < TOTA_SPP_TX_BUF_SIZE;
}

#if 1
uint8_t* app_spp_tota_fill_data_into_tx_buf(const uint8_t* ptrData, uint32_t dataLen)
{
    ASSERT((occupiedTotaSppTxBufSize + dataLen) < TOTA_SPP_TX_BUF_SIZE, 
        "Pending SPP tx data has exceeded the tx buffer size !");

        
    if ((offsetToFillTotaSppTxData + dataLen) > TOTA_SPP_TX_BUF_SIZE)
    {
        offsetToFillTotaSppTxData = 0;
    }

    uint8_t* filledPtr = ptrTotaSppTxBuf + offsetToFillTotaSppTxData;
    memcpy(filledPtr, ptrData, dataLen);

    offsetToFillTotaSppTxData += dataLen;

    occupiedTotaSppTxBufSize += dataLen;

    TOTA_LOG_DBG(3,"dataLen %d offsetToFillTotaSppTxData %d occupiedTotaSppTxBufSize %d",
        dataLen, offsetToFillTotaSppTxData, occupiedTotaSppTxBufSize);
    
    return filledPtr;
}
#else
uint8_t* app_spp_tota_fill_data_into_tx_buf(uint8_t* ptrData, uint32_t dataLen)
{
    TOTA_LOG_DBG(3,"dataLen %d offsetToFillTotaSppTxData %d occupiedTotaSppTxBufSize %d",
        dataLen, offsetToFillTotaSppTxData, occupiedTotaSppTxBufSize);

    ASSERT((occupiedTotaSppTxBufSize + dataLen) < TOTA_SPP_TX_BUF_SIZE, 
        "Pending SPP tx data has exceeded the tx buffer size !");

    uint8_t* filledPtr = ptrTotaSppTxBuf + offsetToFillTotaSppTxData;
    memcpy(filledPtr, ptrData, dataLen);
        
    if ((offsetToFillTotaSppTxData + dataLen) > TOTA_SPP_TX_BUF_SIZE)
    {        
        offsetToFillTotaSppTxData = 0;
    }
    else
    {
        offsetToFillTotaSppTxData += dataLen;
    }

    occupiedTotaSppTxBufSize += dataLen;

    TOTA_LOG_DBG(3,"dataLen %d offsetToFillTotaSppTxData %d occupiedTotaSppTxBufSize %d",
        dataLen, offsetToFillTotaSppTxData, occupiedTotaSppTxBufSize);
    
    return filledPtr;
}
#endif

extern "C" APP_TOTA_CMD_RET_STATUS_E app_tota_data_received(uint8_t* ptrData, uint32_t dataLength);
extern "C" APP_TOTA_CMD_RET_STATUS_E app_tota_cmd_received(uint8_t* ptrData, uint32_t dataLength);


/****************************************************************************
 * TOTA SPP SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList 
 */
static const U8 TotaSppClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(3),        /* Data Element Sequence, 6 bytes */ 
    SDP_UUID_16BIT(SC_SERIAL_PORT),     /* Hands-Free UUID in Big Endian */ 
};

static const U8 TotaSppProtoDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(12),  /* Data element sequence, 12 bytes */ 

    /* Each element of the list is a Protocol descriptor which is a 
     * data element sequence. The first element is L2CAP which only 
     * has a UUID element.  
     */ 
    SDP_ATTRIB_HEADER_8BIT(3),   /* Data element sequence for L2CAP, 3 
                                  * bytes 
                                  */ 

    SDP_UUID_16BIT(PROT_L2CAP),  /* Uuid16 L2CAP */ 

    /* Next protocol descriptor in the list is RFCOMM. It contains two 
     * elements which are the UUID and the channel. Ultimately this 
     * channel will need to filled in with value returned by RFCOMM.  
     */ 

    /* Data element sequence for RFCOMM, 5 bytes */
    SDP_ATTRIB_HEADER_8BIT(5),

    SDP_UUID_16BIT(PROT_RFCOMM), /* Uuid16 RFCOMM */

    /* Uint8 RFCOMM channel number - value can vary */
    SDP_UINT_8BIT(RFCOMM_CHANNEL_TOTA)
};

/*
 * BluetoothProfileDescriptorList
 */
static const U8 TotaSppProfileDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(8),        /* Data element sequence, 8 bytes */

    /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_ATTRIB_HEADER_8BIT(6),

    SDP_UUID_16BIT(SC_SERIAL_PORT),   /* Uuid16 SPP */
    SDP_UINT_16BIT(0x0102)            /* As per errata 2239 */ 
};

/*
 * * OPTIONAL *  ServiceName
 */
static const U8 TotaSppServiceName1[] = {
    SDP_TEXT_8BIT(5),          /* Null terminated text string */ 
    'S', 'p', 'p', '1', '\0'
};

static const U8 TotaSppServiceName2[] = {
    SDP_TEXT_8BIT(5),          /* Null terminated text string */ 
    'S', 'p', 'p', '2', '\0'
};

/* SPP attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * SPP SDP record.
 */
//static const SdpAttribute TotaSppSdpAttributes1[] = {
static sdp_attribute_t TotaSppSdpAttributes1[] = {

    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, TotaSppClassId), 

    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, TotaSppProtoDescList),

    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, TotaSppProfileDescList),

    /* SPP service name*/
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), TotaSppServiceName1),
};

/*
static sdp_attribute_t TotaSppSdpAttributes2[] = {

    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, TotaSppClassId), 

    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, TotaSppProtoDescList),

    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, TotaSppProfileDescList),

    
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), TotaSppServiceName2),
};
*/

extern "C" void reset_programmer_state(unsigned char **buf, size_t *len);
extern unsigned char *g_buf;
extern size_t g_len;

int tota_spp_handle_data_event_func(uint8_t device_id, void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen)
{
    TOTA_LOG_DBG(2,"[%s]data receive length = %d", __func__, dataLen);
    TOTA_LOG_DUMP("[0x%x]", pData, dataLen);
#if defined(APP_ANC_TEST)
    app_anc_tota_cmd_received(pData, (uint32_t)dataLen);
#else
    // the first two bytes of the data packet is the fixed value 0xFFFF
    app_tota_handle_received_data(pData, dataLen);
#endif

    return 0;
}

#if 0
static void app_spp_tota_create_read_thread(void)
{
    TOTA_LOG_DBG(2,"%s %d\n", __func__, __LINE__);
    tota_spp_read_thread_id = osThreadCreate(osThread(tota_spp_read_thread), NULL);
}

static void app_spp_tota_close_read_thread(void)
{
    TOTA_LOG_DBG(2,"%s %d\n", __func__, __LINE__);
    if(tota_spp_read_thread_id)
    {
        osThreadTerminate(tota_spp_read_thread_id);
        tota_spp_read_thread_id = NULL;
    }
}
#endif

#if defined(APP_ANC_TEST)
static void app_synccmd_timehandler(void const *param);
osTimerDef (APP_SYNCCMD, app_synccmd_timehandler);
osTimerId app_check_send_synccmd_timer = NULL;
extern "C" void send_sync_cmd_to_tool();
static void app_synccmd_timehandler(void const *param)
{
    send_sync_cmd_to_tool();
}
#endif

static void spp_tota_callback(uint8_t device_id, struct spp_device *locDev, struct spp_callback_parms *Info)
{
    if (BTIF_SPP_EVENT_REMDEV_CONNECTED == Info->event)
    {
        TOTA_LOG_DBG(1,"::BTIF_SPP_EVENT_REMDEV_CONNECTED %d\n", Info->event);
        isTotaSppConnected = true;
        //app_spp_tota_create_read_thread();
        app_tota_connected(APP_TOTA_CONNECTED);
        app_tota_update_datapath(APP_TOTA_VIA_SPP);
        //conn_stop_connecting_mobile_supervising();
#if defined(APP_ANC_TEST)
        anc_data_buff_init();
        //add a send sync timer
        osTimerStop(app_check_send_synccmd_timer);
        osTimerStart(app_check_send_synccmd_timer, 2000);
#endif
    }
    else if (BTIF_SPP_EVENT_REMDEV_DISCONNECTED == Info->event)
    {
        TOTA_LOG_DBG(1,"::BTIF_SPP_EVENT_REMDEV_DISCONNECTED %d\n", Info->event);
        isTotaSppConnected = false;
        //app_spp_tota_close_read_thread();
        app_tota_disconnected(APP_TOTA_DISCONNECTED);
        app_tota_update_datapath(APP_TOTA_PATH_IDLE);

#if defined(APP_ANC_TEST)
        anc_data_buff_deinit();
        osTimerStop(app_check_send_synccmd_timer);
#endif
        app_spp_tota_tx_done_func = NULL;
    }
    else if (BTIF_SPP_EVENT_DATA_SENT == Info->event)
    {
        //app_spp_tota_free_tx_buf(Info->tx_buf, Info->tx_data_len);
        struct spp_tx_done *pTxDone = (struct spp_tx_done *)(Info->p.other);
        app_spp_tota_free_tx_buf(pTxDone->tx_buf, pTxDone->tx_data_length);
        if (app_spp_tota_tx_done_func)
        {
            app_spp_tota_tx_done_func();
        }
    }
    else
    {
        TOTA_LOG_DBG(1,"::unknown event %d\n", Info->event);
    }
}

static void app_spp_tota_send_data(uint8_t* ptrData, uint16_t length)
{
    if (!isTotaSppConnected)
    {
        return;
    }

    if (BT_STS_SUCCESS != btif_spp_write(tota_spp_dev, (char *)ptrData, &length))
    {
        app_spp_tota_free_tx_buf(ptrData, length);
    }
}

void app_tota_send_cmd_via_spp(uint8_t* ptrData, uint32_t length)
{
    uint8_t* ptrBuf = app_spp_tota_fill_data_into_tx_buf(ptrData, length);
    app_spp_tota_send_data(ptrBuf, (uint16_t)length);
}

void app_tota_send_data_via_spp(uint8_t* ptrData, uint32_t length)
{
    TOTA_LOG_DBG(2,"[%s]tota send data length = %d",__func__,length);
    uint8_t* ptrBuf = app_spp_tota_fill_data_into_tx_buf(ptrData, length);
    app_spp_tota_send_data(ptrBuf, (uint16_t)length);
}

void app_spp_tota_register_tx_done(app_spp_tota_tx_done_t callback)
{
    app_spp_tota_tx_done_func = callback;
}

#if (SPP_SHARE_BUF==0)
static uint8_t spp_rx_buf[SPP_RECV_BUFFER_SIZE];
#endif

#ifdef __BT_ONE_BRING_TWO__
static bool app_spp_tota_accept_channel_request(uint8_t device_id, uint32_t rfcomm_handle, const void* remote, uint8_t server_channel)
{
    TRACE(0,"%s server_channel %d,isTotaSppConnected %d",__func__,server_channel,isTotaSppConnected);

    if (server_channel == RFCOMM_CHANNEL_TOTA)
    {
        return isTotaSppConnected ? false : true;
    }

    return true;
}
#endif

void app_spp_tota_init(void)
{
    uint8_t *rx_buf;
    osMutexId mid;
    btif_sdp_record_param_t param;
    btif_sdp_record_t *tota_sdp_record = NULL;
    struct spp_service * totaSppService = NULL;

#ifdef __BT_ONE_BRING_TWO__
    btif_spp_server_register_accept_channel_callback(app_spp_tota_accept_channel_request);
#endif

    if(tota_spp_dev == NULL)
        tota_spp_dev = btif_create_spp_device();

#if (SPP_SHARE_BUF==1)
	rx_buf = &tws_big_buff[ANC_PARSE_DATA_BUFF_OFFSET - ANC_PARSE_DATA_BUFF_SIZE - TOTA_SPP_TX_BUF_SIZE - SPP_RECV_BUFFER_SIZE];
#else
    rx_buf = tota_spp_dev->rx_buffer = &spp_rx_buf[0];
#endif

    TOTA_LOG_DBG(1,"totaSppTxBuf: 0x%08x", (uint32_t)totaSppTxBuf);
    app_spp_tota_init_tx_buf(totaSppTxBuf);
    btif_spp_init_rx_buf(tota_spp_dev, rx_buf, SPP_RECV_BUFFER_SIZE);

    mid = osMutexCreate(osMutex(tota_spp_mutex));
    if (!mid) {
        ASSERT(0, "cannot create mutex");
    }

    tota_sdp_record = btif_sdp_create_record();

    param.attrs = &TotaSppSdpAttributes1[0],
    param.attr_count = ARRAY_SIZE(TotaSppSdpAttributes1);
    param.COD = BTIF_COD_MAJOR_PERIPHERAL;
    btif_sdp_record_setup(tota_sdp_record, &param);

    totaSppService = btif_create_spp_service();

    totaSppService->rf_service.serviceId = RFCOMM_CHANNEL_TOTA;
    totaSppService->numPorts = 0;
    btif_spp_service_setup(tota_spp_dev, totaSppService, tota_sdp_record);

    tota_spp_dev->portType = BTIF_SPP_SERVER_PORT;
    tota_spp_dev->app_id = app_spp_alloc_server_id();
    tota_spp_dev->spp_handle_data_event_func = tota_spp_handle_data_event_func;
    btif_spp_init_device(tota_spp_dev, 5, mid);
#if defined(APP_ANC_TEST)
    reset_programmer_state(&g_buf, &g_len);
#endif

    btif_spp_open(tota_spp_dev, NULL,  spp_tota_callback);
#if defined(APP_ANC_TEST)
    if (app_check_send_synccmd_timer == NULL)
        app_check_send_synccmd_timer = osTimerCreate (osTimer(APP_SYNCCMD), osTimerPeriodic, NULL);
#endif
}

//tota client
static struct spp_device *_app_spp_tota_client_dev;
static uint8_t spp_client_rx_buf[SPP_RECV_BUFFER_SIZE];
static bool isTotaClientSppConnected = false;

static void app_spp_tota_client_send_data(const uint8_t* ptrData, uint16_t length)
{
    if (!isTotaClientSppConnected)
    {
        return;
    }

    uint8_t* ptrBuf = app_spp_tota_fill_data_into_tx_buf(ptrData, length);
    if (BT_STS_SUCCESS != btif_spp_write(_app_spp_tota_client_dev, (char *)ptrBuf, &length))
    {
        app_spp_tota_free_tx_buf(ptrData, length);
    }
}

void app_spp_tota_client_callback(uint8_t device_id, struct spp_device *locDev, struct spp_callback_parms *Info)
{
    static const uint8_t data_buf[8] = {0x23, 0x67, 0x87, 0x45, 0x11, 0x22, 0x33, 0x44};
    TRACE(0,"spp client event.");
    switch (Info->event)
    {
        case BTIF_SPP_EVENT_REMDEV_CONNECTED:
        {
            isTotaClientSppConnected = true;
            TRACE(1,"::SPP_EVENT_REMDEV_CONNECTED %d\n", Info->event);
            app_spp_tota_client_send_data(data_buf, 8);
            break;
        }

        case BTIF_SPP_EVENT_REMDEV_DISCONNECTED:
        {
            isTotaClientSppConnected = false;
            TRACE(1,"::SPP_EVENT_REMDEV_DISCONNECTED %d\n", Info->event);
            break;
        }

        case BTIF_SPP_EVENT_DATA_SENT:
        {
            struct spp_tx_done *pTxDone = (struct spp_tx_done *)(Info->p.other);
            app_spp_tota_free_tx_buf(pTxDone->tx_buf, pTxDone->tx_data_length);
            break;
        }

        default:
        {
            TRACE(1,"::unknown event %d\n", Info->event);
            break;
        }
    }
}

#define APP_TOTA_PORT_SPP 0x1101
const uint8_t SppServiceSearchReq[] = {
    /* First parameter is the search pattern in data element format. It
    * is a list of 3 UUIDs.
    */
    /* Data Element Sequence, 9 bytes */
    SDP_ATTRIB_HEADER_8BIT(9),
    SDP_UUID_16BIT(APP_TOTA_PORT_SPP),
    SDP_UUID_16BIT(PROT_L2CAP),  /* L2CAP UUID in Big Endian */
    SDP_UUID_16BIT(PROT_RFCOMM), /* UUID for RFCOMM in Big Endian */
    /* The second parameter is the maximum number of bytes that can be
    * be received for the attribute list.
    */
    0x00,
    0x64, /* Max number of bytes for attribute is 100 */
    SDP_ATTRIB_HEADER_8BIT(9),
    SDP_UINT_16BIT(AID_PROTOCOL_DESC_LIST),
    SDP_UINT_16BIT(AID_BT_PROFILE_DESC_LIST),
    SDP_UINT_16BIT(AID_ADDITIONAL_PROT_DESC_LISTS)
};

void app_spp_tota_client_open(btif_remote_device_t *remDev, uint8_t *pServiceSearchRequest, uint8_t serviceSearchRequestLen)
{
    TRACE(1,"[%s]", __func__);
    bt_status_t status;

    if(_app_spp_tota_client_dev == NULL)
    {
        _app_spp_tota_client_dev = btif_create_spp_device();
    }
    app_spp_tota_init_tx_buf(totaSppTxBuf);
    btif_spp_init_rx_buf(_app_spp_tota_client_dev, spp_client_rx_buf, SPP_RECV_BUFFER_SIZE);

    _app_spp_tota_client_dev->portType = BTIF_SPP_CLIENT_PORT;
    _app_spp_tota_client_dev->app_id = app_spp_alloc_client_id();
    _app_spp_tota_client_dev->spp_handle_data_event_func = tota_spp_handle_data_event_func;

    btif_spp_init_device(_app_spp_tota_client_dev, 4, NULL);

    _app_spp_tota_client_dev->sppDev.type.client.rfcommServiceSearchRequestPtr = pServiceSearchRequest;
    _app_spp_tota_client_dev->sppDev.type.client.rfcommServiceSearchRequestLen = serviceSearchRequestLen;

    status  = btif_spp_open(_app_spp_tota_client_dev, remDev, app_spp_tota_client_callback);

    TRACE(1,"spp open status is %d", status);
}

void app_spp_tota_client_open(btif_remote_device_t *remDev)
{
    app_spp_tota_client_open(remDev, (uint8_t*)SppServiceSearchReq, sizeof(SppServiceSearchReq));
}


