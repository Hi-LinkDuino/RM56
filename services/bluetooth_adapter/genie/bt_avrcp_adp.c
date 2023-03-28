/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    bt_avrcp_adp.c
 * @brief:   vendor bluetooth apis defination
 * @author:  
 * @date:    2021/12/10
 * @version: 1.0
 */

#include "tg_bluetooth.h"
#include "bt_adaptor_dbg.h"
#include "avrcp_api.h"
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
#include "factory_section.h"
#include "me_api.h"
#include "ddbif.h"
#include "bt_drv_interface.h"
#include "app_bt.h"
#include "app_bt_func.h"
#include "besbt.h"
#include "bt_service_utils.h"
#include "btapp.h"

extern struct BT_DEVICE_T  app_bt_device;
static TG_BT_AVRCP_IMPL_CB_FUNC_T g_avrcp_cb = {0};

/* AVRCP */
/**
 * @brief avrcp register callback functions set
 * @param[in] func:  avrcp callback functions set.
 * @return: None
 */
void tg_bt_avrcpCallbackRegister(TG_BT_AVRCP_IMPL_CB_FUNC_T *func)
{
    g_avrcp_cb.absoluteVolumeCB = func->absoluteVolumeCB;
    g_avrcp_cb.avrcpCmdSrcCB = func->avrcpCmdSrcCB;
    g_avrcp_cb.avrcpPlayStateCB = func->avrcpPlayStateCB;
    g_avrcp_cb.avrcpStateCB = func->avrcpStateCB;
    g_avrcp_cb.volumeChangeCB = func->volumeChangeCB;
}

/**
 * @brief avrcp connect 
 * @param[in] mac:  bluetooth address need connect.
 * @return: None
 */
void tg_bt_avrcpConnect(const int8_t *mac)
{
    bt_bdaddr_t btAddr;

    macToBtAddress((uint8_t *)mac, (uint8_t *)btAddr.address);
    btif_avrcp_connect(btAddr.address);
    LOG_E("%s \n", __func__);
}

/**
 * @brief avrcp disconnect 
 * @param[in]:  None
 * @return: None
 */
void tg_bt_avrcpDisconnect(void)
{
    btif_remote_device_t *remoteDev = app_bt_get_connected_mobile_device_ptr();
    bt_bdaddr_t * rbtAddr = NULL;
    btif_avrcp_channel_t* channel = NULL;

    rbtAddr = btif_me_get_remote_device_bdaddr(remoteDev);
    channel = btif_get_avrcp_channel_by_addr(rbtAddr->address);
    if(channel){
        btif_avrcp_disconnect(channel);
    }

    LOG_E("%s \n", __func__);
}

/**
 * @brief avrcp send passthrough command 
 * @param[in] cmd_type:  passthrough command type.
 * @return: None
 */
void tg_bt_avrcpSendPassthroughCmd(TG_BT_AVRCP_CMD_TYPE cmd_type)
{
    avrcp_panel_operation_t op;

    switch(cmd_type){
        case TG_BT_AVRCP_CMD_TYPE_PLAY:
            op = BTIF_AVRCP_POP_PLAY;
        break;

        case TG_BT_AVRCP_CMD_TYPE_PAUSE:
            op = BTIF_AVRCP_POP_PAUSE;
        break;

        case TG_BT_AVRCP_CMD_TYPE_FWD:
            op = BTIF_AVRCP_POP_FORWARD;
        break;

        case TG_BT_AVRCP_CMD_TYPE_BWD:
            op = BTIF_AVRCP_POP_BACKWARD;
        break;

        case TG_BT_AVRCP_CMD_TYPE_FFWD:
            op = BTIF_AVRCP_POP_FAST_FORWARD;
        break;

        case TG_BT_AVRCP_CMD_TYPE_RWD:
            op = BTIF_AVRCP_POP_REWIND;
        break;

        case TG_BT_AVRCP_CMD_TYPE_STOP:
            op = BTIF_AVRCP_POP_STOP;
        break;

        case TG_BT_AVRCP_CMD_TYPE_VOL_UP:
            op = BTIF_AVRCP_POP_VOLUME_UP;
        break;

        case TG_BT_AVRCP_CMD_TYPE_VOL_DOWN:
            op = BTIF_AVRCP_POP_VOLUME_DOWN;
        break;
        
        default:
            return;
    }
    // ag_adapter_send_panel_key(device_id ,op);
    LOG_E("%s op:%02x \n", __func__, op);
}

/**
 * @brief avrcp get current play status 
 * @param[in] mac:  bluetooth address.
 * @return: None
 */
int32_t tg_bt_avrcpGetPlayStatus(const int8_t *mac)
{
    int ret = 0;
    btif_avrcp_channel_t* channel = NULL;
    btif_remote_device_t *remoteDev = app_bt_get_connected_mobile_device_ptr();
    bt_bdaddr_t btAddr;
    macToBtAddress((uint8_t *)mac, (uint8_t *)btAddr.address);

    bt_bdaddr_t * rbtAddr = NULL;
    rbtAddr = btif_me_get_remote_device_bdaddr(remoteDev);
    if(rbtAddr != NULL){
        if(!memcmp(btAddr.address, rbtAddr->address, 6)){
        channel = btif_get_avrcp_channel_by_addr(rbtAddr->address);
        ret = btif_avrcp_ct_get_play_status(channel);
        }
    }

    LOG_E("%s \n", __func__);
    return ret;
}


/**
 * @brief avrcp volume change notification 
 * @param[in] vol:  current volume, range 0-0x7f
 * @param[in] role:  a2dp role, 0 means sink; 1 means source
 * @return: None
 */
int32_t tg_bt_avrcpChangeVolume(uint8_t vol, TG_BT_A2DP_ROLE role)
{
    LOG_E("%s \n", __func__);
    if(role == TG_BT_A2DP_ROLE_SINK)
        return -1;

    btif_avrcp_channel_t* channel = NULL;
    btif_remote_device_t *remoteDev = app_bt_get_connected_mobile_device_ptr();
    bt_bdaddr_t * rbtAddr = NULL;

    rbtAddr = btif_me_get_remote_device_bdaddr(remoteDev);
    channel = btif_get_avrcp_channel_by_addr(rbtAddr->address);
    if(channel){
// #ifndef BT_USE_UX_VOLUME
//         a2dp_volume_set(0,vol);
// #endif
        btif_avrcp_ct_send_volume_change_actual_rsp(channel, vol);
    }

    return vol;
}

/**
 * @brief avrcp get element attribute tile 
 * @param[in] cmd_type:  bluetooth address.
 * @return: None
 */
int32_t tg_bt_avrcpGetElementAttrTitle(const int8_t *mac)
{
    int ret = 0;
    btif_avrcp_channel_t* channel = NULL;
    bt_bdaddr_t btAddr;
    macToBtAddress((uint8_t *)mac, (uint8_t *)btAddr.address);

    channel = btif_get_avrcp_channel_by_addr(btAddr.address);
    if(!channel){
        return -1;
    }

    ret = btif_avrcp_ct_get_media_Info(channel, 0x7f);
    LOG_E("%s \n", __func__);
    return ret;
}

extern void avrcpStateChangedNotify(unsigned char state)
{
    TG_BT_AVRCP_STATUS avState;

    LOG_E("I AM avrcpStateChangedNotify state %d", state);
    if(state)
        avState = TG_BT_AVRCP_CONNECTED;
    else
        avState = TG_BT_AVRCP_DISCONNECTED;
    if(g_avrcp_cb.avrcpStateCB){
        g_avrcp_cb.avrcpStateCB(avState);
    }
}

extern void avrcpPlayStateChangeNotify(unsigned char mediaStatus)
{   
    TG_BT_AVRCP_PLAY_STATUS avPlaytate;

    LOG_E("I AM avrcpPlayStateChangeNotify mediaStatus %d", mediaStatus);
    switch(mediaStatus){
        case BTIF_AVRCP_MEDIA_STOPPED:
            avPlaytate = TG_BT_AVRCP_PLAY_STATUS_STOPPED;
            break;

        case  BTIF_AVRCP_MEDIA_PLAYING:
            avPlaytate = TG_BT_AVRCP_PLAY_STATUS_PLAYING;
            break;

        case BTIF_AVRCP_MEDIA_PAUSED:
            avPlaytate = TG_BT_AVRCP_PLAY_STATUS_PAUSEED;
            break;

        default:
            avPlaytate = TG_BT_AVRCP_PLAY_STATUS_MAX;
            break;
    }
    
    
    if(g_avrcp_cb.avrcpPlayStateCB){
        g_avrcp_cb.avrcpPlayStateCB(avPlaytate);
    }
}

extern void avrcpVolumeChangeNotify(unsigned short volOp)
{   
    TG_BT_AVRCP_VOLUME_CHANGE avVolChange;

    LOG_E("I AM avrcpVolumeChangeNotify volOp %d", volOp);
    if(volOp == BTIF_AVRCP_POP_VOLUME_UP){
        avVolChange = TG_BT_AVRCP_VOLUME_UP;
        LOG_E("VolumeChange UP");
    }else{
        avVolChange =  TG_BT_AVRCP_VOLUME_DOWN;
        LOG_E("VolumeChange DOWN");
    }

    if(g_avrcp_cb.volumeChangeCB){
        g_avrcp_cb.volumeChangeCB(avVolChange);
    }
}

extern void avrcpAbsoluteVolumeNotify(unsigned char vol)
{   
    LOG_E("I AM avrcpAbsoluteVolumeNotify vol %d", vol);
    if(g_avrcp_cb.absoluteVolumeCB){
        g_avrcp_cb.absoluteVolumeCB(vol);
    }
}

extern void avrcpCmdForSrcNotify(unsigned char cmd)
{   
    if(g_avrcp_cb.avrcpCmdSrcCB){
        //not support source now
        g_avrcp_cb.avrcpCmdSrcCB(TG_BT_AVRCP_CMD_TYPE_MAX);
    }
    LOG_E("%s \n", __func__);
}


