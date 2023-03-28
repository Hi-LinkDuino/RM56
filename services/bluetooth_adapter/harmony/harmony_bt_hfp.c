#include "ohos_bt_hfp_hf.h"
#include "ohos_bt_def.h"
#include "btapp.h"
#include "hfp_api.h"
#include "harmony_utils.h"
#include "hal_trace.h"

#define ENABLE       1
#define DISABLE      0
BtHfpHfCallbacks *g_hfp_cb_func = NULL;

btif_hf_channel_t* app_get_hfp_channel_by_addr(uint8_t * addr)
{
    struct BT_DEVICE_T* curr_device = NULL;

    for (int i = 0; i < BT_DEVICE_NUM; i++)
    {
        curr_device = app_bt_get_device(i);
        if (memcmp(addr, curr_device->remote.address, sizeof(bt_bdaddr_t)) == 0)
        {
            return curr_device->hf_channel;
        }
    }
    return NULL;
}

int HfpHfConnect(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(hf_channel)
        bes_status = btif_ag_create_service_link(hf_channel,(bt_bdaddr_t*)bdAddr);
    return bes_status_to_ohos(bes_status);
}

int HfpHfDisconnect(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(hf_channel)
        bes_status = btif_ag_disconnect_service_link(hf_channel);
    return bes_status_to_ohos(bes_status);
}

int HfpHfConnectSco(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    bes_status = btif_ag_create_audio_link(hf_channel);
    return bes_status_to_ohos(bes_status);
}

int HfpHfDisconnectSco(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    bes_status = btif_ag_disc_audio_link(hf_channel);
    return bes_status_to_ohos(bes_status);
}

int HfpHfAcceptIncomingCall(const BdAddr *bdAddr, HfpHfAcceptCallActionType actionType)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    switch(actionType){
        case OHOS_HFP_HF_ACCEPT_CALL_ACTION_NONE:
            bes_status = btif_hf_answer_call(hf_channel);
            break;
        case OHOS_HFP_HF_ACCEPT_CALL_ACTION_HOLD:
            bes_status = btif_hf_call_hold(hf_channel,BTIF_HF_HOLD_HOLD_ACTIVE_CALLS,0);
            break;
        case OHOS_HFP_HF_ACCEPT_CALL_ACTION_FINISH:
            bes_status = btif_hf_call_hold(hf_channel,BTIF_HF_HOLD_RELEASE_HELD_CALLS,0);
            break;
        default:
            break;
    }
    return bes_status_to_ohos(bes_status);
}

int HfpHfHoldActiveCall(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    bes_status = btif_hf_call_hold(hf_channel,BTIF_HF_HOLD_HOLD_ACTIVE_CALLS,0);
    return bes_status_to_ohos(bes_status);
}

int HfpHfRejectIncomingCall(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    bes_status = btif_hf_hang_up_call(hf_channel);
    return bes_status_to_ohos(bes_status);
}

int HfpHfFinishCall(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    bes_status = btif_hf_hang_up_call(hf_channel);
    return bes_status_to_ohos(bes_status);
}

int HfpHfStartDial(const BdAddr *bdAddr, unsigned char *number, unsigned char len)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    bes_status = btif_hf_dial_number(hf_channel,number,len);
    return bes_status_to_ohos(bes_status);    
}

int HfpHfOpenVoiceRecognition(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    bes_status = btif_hf_enable_voice_recognition(hf_channel, ENABLE);
    return bes_status_to_ohos(bes_status);
}

int HfpHfCloseVoiceRecognition(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);
    bes_status = btif_hf_enable_voice_recognition(hf_channel, DISABLE);
    return bes_status_to_ohos(bes_status);
}

int HfpHfSetVolume(const BdAddr *bdAddr, BtHfpVolumeType type, unsigned char volume)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_hf_channel_t* hf_channel;
    hf_channel = app_get_hfp_channel_by_addr((uint8_t*)bdAddr->addr);

    if(type == OHOS_HFP_VOLUME_MIC)
        bes_status = btif_ag_set_microphone_gain(hf_channel, volume);
    else if(type == OHOS_HFP_VOLUME_SPEAKER)
        bes_status = btif_hf_report_speaker_volume(hf_channel, volume);

    return bes_status_to_ohos(bes_status);
}

int HfpHfSendVendorATCmd(const char *cmd)
{
    bt_status_t bes_status = BT_STS_SUCCESS;
    //send AT command
    bes_status = btif_hf_send_at_cmd((btif_hf_channel_t*)app_bt_get_device(BT_DEVICE_ID_1)->hf_channel, cmd);

    return bes_status_to_ohos(bes_status);
}

void HfpHfUserRportAudioDisconnected(unsigned char *addr, unsigned int addr_len)
{
    BdAddr dev_addr;
    if(!g_hfp_cb_func || !addr){
        return;
    }

    memcpy(dev_addr.addr, addr, OHOS_BD_ADDR_LEN);

    if(g_hfp_cb_func && g_hfp_cb_func->scoConnStateChangedCb){
        g_hfp_cb_func->scoConnStateChangedCb(&dev_addr,OHOS_SCO_STATE_DISCONNECTED);
    }    
}


void HfpHfUserEventCallback(uint8_t device_id, btif_hf_channel_t* chan, struct hfp_context * ctx)
{
    if (ctx == NULL){
        return;
    }
    BdAddr dev_addr;
    struct BT_DEVICE_T* curr_device = app_bt_get_device(device_id);
    if(!g_hfp_cb_func){
        return;
    }

    if (device_id == BT_DEVICE_INVALID_ID && ctx->event == BTIF_HF_EVENT_SERVICE_DISCONNECTED)
    {
        // hfp profile is closed due to acl created fail
        TRACE(1,"::HF_EVENT_SERVICE_DISCONNECTED acl created error=%x\n", ctx->disc_reason);
        return;
    }
    
    memcpy(dev_addr.addr, curr_device->remote.address, OHOS_BD_ADDR_LEN);

    switch(ctx->event){
        case BTIF_HF_EVENT_SERVICE_CONNECTED:
            if(g_hfp_cb_func && g_hfp_cb_func->connStateChangedCb){
                g_hfp_cb_func->connStateChangedCb(&dev_addr,OHOS_PROFILE_STATE_CONNECTED);
            }
            break;
        case BTIF_HF_EVENT_SERVICE_DISCONNECTED:
            if(g_hfp_cb_func && g_hfp_cb_func->connStateChangedCb){
                g_hfp_cb_func->connStateChangedCb(&dev_addr,OHOS_PROFILE_STATE_DISCONNECTED);
            }
            break;
        case BTIF_HF_EVENT_AUDIO_CONNECTED:
            if(g_hfp_cb_func && g_hfp_cb_func->scoConnStateChangedCb){
                g_hfp_cb_func->scoConnStateChangedCb(&dev_addr,OHOS_SCO_STATE_CONNECTED);
            }
            if(g_hfp_cb_func && g_hfp_cb_func->codecNotifyCb){
                g_hfp_cb_func->codecNotifyCb(&dev_addr,ctx->sco_codec);
            }
            break;
        case BTIF_HF_EVENT_AUDIO_DISCONNECTED:
            if(g_hfp_cb_func && g_hfp_cb_func->scoConnStateChangedCb){
                g_hfp_cb_func->scoConnStateChangedCb(&dev_addr,OHOS_SCO_STATE_DISCONNECTED);
            }
            break;
        case BTIF_HF_EVENT_BATTERY_IND:
            if(g_hfp_cb_func && g_hfp_cb_func->batteryLevelChangedCb){
                g_hfp_cb_func->batteryLevelChangedCb(&dev_addr,ctx->ciev_status);
            }
            break;
        case BTIF_HF_EVENT_SIGNAL_IND:
            if(g_hfp_cb_func && g_hfp_cb_func->signalStrengthChangedCb){
                g_hfp_cb_func->signalStrengthChangedCb(&dev_addr,ctx->ciev_status);
            }
            break;
        case BTIF_HF_EVENT_SERVICE_IND:
            if(g_hfp_cb_func && g_hfp_cb_func->registrationStatusChangedCb){
                g_hfp_cb_func->registrationStatusChangedCb(&dev_addr,ctx->ciev_status);
            }
            break;
        case BTIF_HF_EVENT_ROAM_IND:
            if(g_hfp_cb_func && g_hfp_cb_func->roamingStatusChangedCb){
                g_hfp_cb_func->roamingStatusChangedCb(&dev_addr,ctx->ciev_status);
            }
            break;
        //case HFP_HF_OPERATEOR_SECLECT:
        //    btif_hfp_hf_operator_name *operator = (btif_hfp_hf_operator_name *)para;
        //    g_hfp_cb_func->operatorSelectionCb(&dev_addr,operator->name,operator->namelen);
        //    break;
        case BTIF_HF_EVENT_CURRENT_CALL_STATE:
            if(g_hfp_cb_func && g_hfp_cb_func->subscriberNumberCb){
                g_hfp_cb_func->subscriberNumberCb(&dev_addr,ctx->call_number,strlen(ctx->call_number));
            }
            break;
        case BTIF_HF_EVENT_VOICE_REC_STATE:
            if(g_hfp_cb_func && g_hfp_cb_func->voiceRecognitionStatusChangedCb){
                g_hfp_cb_func->voiceRecognitionStatusChangedCb(&dev_addr,ctx->voice_rec_state);
            }
            break;
        case BTIF_HF_EVENT_IN_BAND_RING:
            if(g_hfp_cb_func && g_hfp_cb_func->inBandRingToneChangedCb){
                g_hfp_cb_func->inBandRingToneChangedCb(&dev_addr,ctx->bsir_enable);
            }
            break;
        case BTIF_HF_EVENT_SPEAKER_VOLUME:
            if(g_hfp_cb_func && g_hfp_cb_func->volumeChangedCb){
                g_hfp_cb_func->volumeChangedCb(&dev_addr,OHOS_HFP_VOLUME_SPEAKER,ctx->speaker_volume);
            }
            break;
        case BTIF_HF_EVENT_MIC_VOLUME:
            if(g_hfp_cb_func && g_hfp_cb_func->volumeChangedCb){
                g_hfp_cb_func->volumeChangedCb(&dev_addr,OHOS_HFP_VOLUME_MIC,ctx->mic_volume);
            }
            break;
        case BTIF_HF_EVENT_CALL_IND:
            if(g_hfp_cb_func && g_hfp_cb_func->hfClientIndCallCb){
                g_hfp_cb_func->hfClientIndCallCb(&dev_addr,ctx->call);
            }
            break;
        case BTIF_HF_EVENT_CALLSETUP_IND:
            if(g_hfp_cb_func && g_hfp_cb_func->hfClientIndCallSetupCb){
                g_hfp_cb_func->hfClientIndCallSetupCb(&dev_addr,ctx->call_setup);
            }
            break;
        case BTIF_HF_EVENT_RING_IND:
            if(g_hfp_cb_func && g_hfp_cb_func->hfClientRingIndCb){
               g_hfp_cb_func->hfClientRingIndCb(&dev_addr);
            }
            break;        
        default:
            break;
    }
}

int HfpHfRegisterCallbacks(BtHfpHfCallbacks *func)
{
    g_hfp_cb_func = func;
    btif_hfp_register_user_event_callback(HfpHfUserEventCallback);
    return 0;
}

