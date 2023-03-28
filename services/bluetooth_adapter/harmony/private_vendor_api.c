#include "private_vendor_api.h"
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
#include "bluetooth.h"
#include "factory_section.h"

extern void GapSetNeedEnterFreemanMode(bool freeman);
extern void app_ibrt_conn_state_change_cb_register(void (*cb)(void));

MediaStreamNotifyCallback MediaStreamNotifyCb = NULL;
StereoReceiveData   ReceiveTwsDataCb = NULL;

void StartLocalMediaPlay(void)
{
    app_bt_af_stream_start();
}

void StopLocalMediaPlay(void)
{
    app_bt_af_stream_stop();
}

void app_notify_receive_media_data(void)
{
    if(MediaStreamNotifyCb)
        MediaStreamNotifyCb();
}

#ifdef VAD_IN_BACKGROUND
extern void bt_sco_put_uplink_data(uint8_t*buf , uint16_t len);
#endif
int AppSendHfpPcmStream(PcmStreamInfo *streamInfo)
{
    if (streamInfo == NULL) {
        return -1;
    }
#ifdef VAD_IN_BACKGROUND
    bt_sco_put_uplink_data((uint8_t*)streamInfo->stream, streamInfo->length);
#endif
    return 0;
}

int SetUpStereo(StereoInfo *data)
{
    if (data)
    {
        LOG_D("%s role:%d addr:", __func__, data->role);
        DUMP8("%02x ", data->addr.addr, 6);
    }
#ifdef IBRT
    app_ibrt_if_start_tws_pairing(data->role, data->addr.addr);
    GapSetNeedEnterFreemanMode(false);
#endif
    return 0;
}

void ExitStereo(void)
{
    LOG_D("%s", __func__);
    bt_bdaddr_t invaild_addr = {{0x00,0x00,0x00,0x00,0x00,0x00}};
#ifdef IBRT
    bool ibrt_enabled = false;

    struct nvrecord_env_t *nvrecord_env = NULL;
    nv_record_env_get(&nvrecord_env);

    if(!nvrecord_env)
        return;
    
    ibrt_enabled = app_tws_ibrt_tws_link_connected();
    if(!ibrt_enabled){
        nv_record_env_get(&nvrecord_env);
        nv_record_ddbrec_delete(&nvrecord_env->ibrt_mode.record.bdAddr);
        nv_record_update_ibrt_info(STEREO_NONE, &invaild_addr);
        nv_record_clean_tws_name();
        nv_record_ddbrec_delete_all();
        nv_record_flash_flush();
        app_ibrt_if_enter_freeman_pairing();
    }else{
        app_ibrt_conn_disconnect_ibrt(&nvrecord_env->ibrt_mode.record.bdAddr);
        GapSetNeedEnterFreemanMode(true);
     }
#endif
}

int StereoSendData(unsigned char *buf, unsigned int length)
{
#ifdef IBRT
    tws_ctrl_send_cmd(APP_IBRT_CUSTOM_CMD_TEST1, buf, length);
#endif
    return 0;
}
uint8_t user_choose_chnl_enable=0;
static A2DP_AUDIO_CHANNEL_SELECT_E s_current_channel=A2DP_AUDIO_CHANNEL_SELECT_STEREO;
void user_stereo_set_channel(A2DP_AUDIO_CHANNEL_SELECT_E channel)
{
    s_current_channel=channel;
}
A2DP_AUDIO_CHANNEL_SELECT_E user_stereo_get_channel(void)
{
    return s_current_channel;
}
/*
 * @param: channel  left/right channel
 * @return: int 0--success  not 0--fail
 * function: config the left/right channel after stereo group established
 */
int StereoChannelSwitch(StereoChannelSet channel)
{
    user_choose_chnl_enable=1;
    if(LEFT_CHANNEL==channel){
        user_stereo_set_channel(A2DP_AUDIO_CHANNEL_SELECT_LCHNL);
    }else if(RIGHT_CHANNEL==channel){
        user_stereo_set_channel(A2DP_AUDIO_CHANNEL_SELECT_RCHNL);
    }else{
        user_stereo_set_channel(A2DP_AUDIO_CHANNEL_SELECT_STEREO);
    }
    return 0;
}

int StereoMasterSlaveSwitch(BdAddr *addr, StereoRole role)
{
    uint32_t ibrt_role = 0xff;
#ifdef IBRT
    if(role == STEREO_MASTER)
        ibrt_role = IBRT_MASTER;
    else if(role == STEREO_SLAVE)
        ibrt_role = IBRT_SLAVE;
    else
        return -1;

    if(app_ibrt_conn_get_ui_role() == IBRT_UNKNOW)
        return -1;

    if(ibrt_role != app_ibrt_conn_get_ui_role())
        app_ibrt_if_tws_role_switch_request();

    return 0;
#else
    return 0;
#endif
}

BdAddr *GetExternalDisplayAddress(void)
{
    static BdAddr addr;
    bt_bdaddr_t local_addr;
#ifdef IBRT
    if(app_ibrt_conn_get_ui_role() != IBRT_UNKNOW){
        ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
        if(!p_ibrt_ctrl)
            return NULL;
        
        if(app_ibrt_conn_get_ui_role() == IBRT_MASTER){
            memcpy(addr.addr, &p_ibrt_ctrl->local_addr.address, OHOS_BD_ADDR_LEN);
        }else{
            memcpy(addr.addr, &p_ibrt_ctrl->peer_addr.address, OHOS_BD_ADDR_LEN);
        }    
    }else{
        factory_section_original_btaddr_get((uint8_t*)&local_addr.address);
        memcpy(addr.addr, local_addr.address, OHOS_BD_ADDR_LEN);
    }
#else
    factory_section_original_btaddr_get(&local_addr.address);
    memcpy(addr.addr, local_addr.address, OHOS_BD_ADDR_LEN);
#endif
    return &addr;
}

void app_tws_rec_cmd_handler(uint8_t *buf,uint32_t len)
{
    if(ReceiveTwsDataCb){
        ReceiveTwsDataCb(buf,len);
    }
}

int RegisterMediaStreamNotifyCallback(MediaStreamNotifyCallback cb)
{
    MediaStreamNotifyCb = cb;
    app_a2dp_notify_receive_stream_callback_register(app_notify_receive_media_data);
    return 0;
}

int RegisterStereoDateNotifyCallback(StereoReceiveData func)
{
#ifdef IBRT
    ReceiveTwsDataCb = func;
    app_ibrt_custom_cmd_rx_register(app_tws_rec_cmd_handler);
#endif
    return 0;
}

int RegisterStereoNotifyCallback(StereoStateChangedCallback func)
{
#ifdef IBRT
    app_ibrt_conn_state_change_cb_register((void*)func);
#endif
    return 0;
}

int RegisterEnDecryptCallback(HksEnDecryptLinkKey encryptCb, HksEnDecryptLinkKey decryptCb)
{
#ifdef LINK_KEY_ENCRYPT_BY_CUSTOMER
    AES256_GCM_encrypt_register(encryptCb);
    AES256_GCM_decrypt_register(decryptCb);
#endif
    return 0;
}

void AppNotifyStereoPlayStatus(StereoPlayMode status)
{

}

