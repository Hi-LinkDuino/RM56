/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    bt_a2dp_adp.c
 * @brief:   vendor bluetooth apis defination
 * @author:  
 * @date:    2021/12/10
 * @version: 1.0
 */

#include "tg_bluetooth.h"
#include "bt_adaptor_dbg.h"
#include "app_a2dp.h"
#ifdef IBRT
#include "app_ibrt_customif_cmd.h"
#include "app_tws_ctrl_thread.h"
#include "hal_trace.h"
#include "app_ibrt_if.h"
#include "app_tws_ibrt.h"
#endif
#include "nvrecord_env.h"
#include "nvrecord_bt.h"
#include "a2dp_decoder.h"
#include "bt_adaptor_dbg.h"
//#include "bluetooth.h"
#include "factory_section.h"
#include "me_api.h"
#include "ddbif.h"
#include "bt_drv_interface.h"
#include "app_bt.h"
#include "app_bt_func.h"
#include "besbt.h"
#include "bt_service_utils.h"

uint8_t user_choose_chnl_enable=0;
static A2DP_AUDIO_CHANNEL_SELECT_E s_current_channel=A2DP_AUDIO_CHANNEL_SELECT_STEREO;
static TG_BT_A2DP_IMPL_CB_FUNC_T g_a2dp_cb = {0};

/* A2DP */

void user_stereo_set_channel(A2DP_AUDIO_CHANNEL_SELECT_E channel)
{
    s_current_channel=channel;
}
A2DP_AUDIO_CHANNEL_SELECT_E user_stereo_get_channel(void)
{
    return s_current_channel;
}

/**
 * @brief a2dp register callback functions set
 * @param[in] func:  a2dp callback functions set.
 * @return: None
 */
void tg_bt_a2dpCallbackRegister(TG_BT_A2DP_IMPL_CB_FUNC_T *func)
{
    g_a2dp_cb.a2dpStateCB = func->a2dpStateCB;
    g_a2dp_cb.a2dpStreamCB = func->a2dpStreamCB;
}

/**
 * @brief a2dp connect 
 * @param[in] mac:  bluetooth address need connect.
 * @param[in] role: a2dp role set, 0 means sink; 1 means source.
 * @return: None
 */
extern a2dp_stream_t* app_bt_get_steam(enum BT_DEVICE_ID_T id);
void tg_bt_a2dpConnect(const int8_t *mac, TG_BT_A2DP_ROLE role)
{
    bt_bdaddr_t btAddr;
    a2dp_stream_t *stream = NULL;
    // enum BT_DEVICE_ID_T device_id;
    macToBtAddress((uint8_t *)mac, (uint8_t *)btAddr.address);

    stream = (a2dp_stream_t*)app_bt_get_steam(BT_DEVICE_ID_1);
    app_bt_A2DP_OpenStream(stream, &btAddr);
    LOG_E("%s \n", __func__); 
}

/**
 * @brief a2dp disconnect 
 * @param[in] mac:  bluetooth address need disconnect.
 * @param[in] role: a2dp role set, 0 means sink; 1 means source.
 * @return: None
 */
void tg_bt_a2dpDisconnect(const int8_t *mac, TG_BT_A2DP_ROLE role)
{
    bt_bdaddr_t btAddr;
    a2dp_stream_t *stream = NULL;
    // enum BT_DEVICE_ID_T device_id;
    macToBtAddress((uint8_t *)mac, (uint8_t *)btAddr.address);
    if(role != TG_BT_A2DP_ROLE_SINK)
        return;

    stream = (a2dp_stream_t*)app_bt_get_steam(BT_DEVICE_ID_1);
    app_bt_A2DP_CloseStream(stream);

    LOG_E("%s \n", __func__);    
}

/**
 * @brief local a2dp role set 
 * @param[in] role: a2dp role set, 0 means sink; 1 means source.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_a2dpSetRole(TG_BT_A2DP_ROLE role)
{
    LOG_E("enter %s ", __FUNCTION__);
    if(role != TG_BT_A2DP_ROLE_SINK)
        return -1;
    else
        return 0;
}

/**
 * @brief local a2dp role get 
 * @param[in]: None
 * @return: 0 means sink; 1 means source.
 */
TG_BT_A2DP_ROLE tg_bt_a2dpGetRole(void)
{
    //only support a2dp sink now
    return TG_BT_A2DP_ROLE_SINK;
}

/**
 * @brief a2dp paired device list get
 * @param[out] info: paired device info list.
 * @param[in] role: a2dp role set, 0 means sink; 1 means source.
 * @return: Zero on success or error code otherwise
 */
void tg_bt_getA2dpPairedList(TG_BT_A2DP_DEV_INFO_LIST *info, TG_BT_A2DP_ROLE role)
{
    int devNum = 0;
    btif_device_record_t devRec;
    bt_status_t status;

    info->dev_num = 0;
    if(role == TG_BT_A2DP_ROLE_SINK){
        return;
    }

    devNum = nv_record_get_paired_dev_count();
    if(!devNum){
        return ;
    }

    info->head_idx = 0;
    for(int i = 0; i <  devNum; i ++){
        status = ddbif_enum_device_records(devNum - (i + 1), &devRec);
        if(status == BT_STS_SUCCESS){
            if(info->dev_num >= TG_MAX_BT_DEV_NUM)
                break;
            btAddressToMac((uint8_t *)info->device_list[info->dev_num].bdAddr, (uint8_t *)devRec.bdAddr.address);
            info->dev_num++;
        }
    }
    
    LOG_E("%s get A2dp Paired dev num:%d \n", __func__, info->dev_num);
}

/**
 * @brief send a2dp command dependently for BQB Test
 * @param[in] mac: bluetooth address.
 * @param[in] signal_id: avdtp command id which want send out.
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_a2dpSendCmd(const int8_t *mac, uint8_t signal_id)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
    return 0;
}

extern void a2dpStateChangedNotify(unsigned char event, bt_bdaddr_t * btAddr, unsigned char reason)
{
    char    mac[TG_BDADDR_MAX_LEN];
    TG_BT_A2DP_STATE state;
    
    LOG_E("I AM a2dpStateChangedNotify event =  %d", event);
    switch(event){
        case BTIF_A2DP_EVENT_STREAM_OPEN:
            state = TG_BT_A2DP_CONNECTED;
            break;
        case BTIF_A2DP_EVENT_CODEC_INFO:
             state = TG_BT_A2DP_CONNECTING;
             break;
        case BTIF_A2DP_EVENT_AVDTP_DISCONNECT:
            if(reason == BTIF_BEC_CONNECTION_TIMEOUT)
                state = TG_BT_A2DP_LINK_LOSS;
            else if(reason == BTIF_BEC_PAGE_TIMEOUT)
                state =TG_BT_A2DP_CONNECT_TIMEOUT;
            else
                state = TG_BT_A2DP_DISCONNECTED;
            break;
            
        default:
           state = TG_BT_A2DP_EVENT_MAX; 
    }
    
    if(g_a2dp_cb.a2dpStateCB){
        btAddressToMac((unsigned char*)mac, btAddr->address);
        g_a2dp_cb.a2dpStateCB(state, (int8_t*)mac);
    }

}

extern void a2dpStreamStateNotify(unsigned char streamState)
{
    TG_BT_A2DP_STREAM_STATE stState;

    LOG_E("I AM a2dpStreamStateNotify a2dp event = %d", streamState);
    if(streamState == BTIF_A2DP_EVENT_STREAM_STARTED)
        stState = TG_BT_A2DP_STREAM_STATE_PLAYING;
    else
        stState = TG_BT_A2DP_STREAM_STATE_STOP;

    if(g_a2dp_cb.a2dpStreamCB){
        g_a2dp_cb.a2dpStreamCB(stState);
    } 

}
