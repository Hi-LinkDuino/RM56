#include "string.h"
#include "plat_types.h"
#include "hal_trace.h"
#include "avrcp_api.h"
#include "ohos_bt_def.h"
#include "ohos_bt_avrcp_controller.h"
#include "harmony_utils.h"
#include "btapp.h"

static BtAvrcpCtCallbacks *g_avrcp_ct_cb_func = NULL;


/**
 * @brief 获取设备连接状态。
 *
 * @param bdAddr 设备地址
 * @return 返回{@link BtProfileConnectState}
 * @since 3
 */
int AvrcpCtGetDeviceConnectState(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    uint8_t bes_avrcp_connect_status = 0;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(!channel){
        return OHOS_BT_STATUS_FAIL;
    }

    bes_avrcp_connect_status = btif_avcp_get_connect_status(channel);
    if(bes_avrcp_connect_status == OHOS_PROFILE_STATE_CONNECTING)
        return OHOS_PROFILE_STATE_CONNECTING;
    else if(bes_avrcp_connect_status == BTIF_AVRCP_STATE_CONNECTED)
        return OHOS_PROFILE_STATE_CONNECTED;
    else
        return OHOS_PROFILE_STATE_DISCONNECTED;
}

/*
 * @brief 建立与TG的连接
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtConnect(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    bt_bdaddr_t bes_addr;

    memcpy(bes_addr.address, bdAddr->addr, 6);
    bes_status = btif_avrcp_connect(&bes_addr);
    return bes_status_to_ohos(bes_status );
}

/*
 * @brief 断开与TG的连接
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtDisconnect(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    bt_bdaddr_t bes_addr;

    memcpy(bes_addr.address, bdAddr->addr, 6);
    bes_status = btif_avrcp_disconnect((btif_avrcp_channel_t*)&bes_addr);
    return bes_status_to_ohos(bes_status );
}

static avrcp_panel_operation_t AvrcpCtKeyToBes(uint8_t keyOperation)
{
    avrcp_panel_operation_t key = BTIF_AVRCP_POP_RESERVED;

    switch(keyOperation){
        case OHOS_AVRCP_KEY_VOLUME_UP:
            key = BTIF_AVRCP_POP_VOLUME_UP;
            break;

        case OHOS_AVRCP_KEY_VOLUME_DOWN:
            key = BTIF_AVRCP_POP_VOLUME_DOWN;
            break;
            
        case OHOS_AVRCP_KEY_MUTE:
            key = BTIF_AVRCP_POP_MUTE;
            break;

        case OHOS_AVRCP_KEY_PLAY:
            key = BTIF_AVRCP_POP_PLAY;
            break;

        case OHOS_AVRCP_KEY_STOP:
            key = BTIF_AVRCP_POP_STOP;
            break;
            
        case OHOS_AVRCP_KEY_PAUSE:
            key = BTIF_AVRCP_POP_PAUSE;
            break;

        case OHOS_AVRCP_KEY_RECORD:
            key = BTIF_AVRCP_POP_RECORD;
            break;

        case OHOS_AVRCP_KEY_REWIND:
            key = BTIF_AVRCP_POP_REWIND;
            break;
            
        case OHOS_AVRCP_KEY_FAST_FORWARD:
            key = BTIF_AVRCP_POP_FAST_FORWARD;
            break;

        case OHOS_AVRCP_KEY_FORWARD:
            key = BTIF_AVRCP_POP_FORWARD;
            break;

        case OHOS_AVRCP_KEY_BACKWARD:
            key = BTIF_AVRCP_POP_BACKWARD;
            break;

        default:
            break;
    }

    return key;
}

static AvrcpCtKeyOperation AvrcpCtKeyToOh(uint8_t keyOperation)
{
    avrcp_panel_operation_t key = BTIF_AVRCP_POP_RESERVED;

    switch(keyOperation){
        case BTIF_AVRCP_POP_VOLUME_UP:
            key = OHOS_AVRCP_KEY_VOLUME_UP;
            break;

        case BTIF_AVRCP_POP_VOLUME_DOWN:
            key = OHOS_AVRCP_KEY_VOLUME_DOWN;
            break;
            
        case BTIF_AVRCP_POP_MUTE:
            key = OHOS_AVRCP_KEY_MUTE;
            break;

        case BTIF_AVRCP_POP_PLAY:
            key = OHOS_AVRCP_KEY_PLAY;
            break;

        case BTIF_AVRCP_POP_STOP:
            key = OHOS_AVRCP_KEY_STOP;
            break;
            
        case BTIF_AVRCP_POP_PAUSE:
            key = OHOS_AVRCP_KEY_PAUSE;
            break;

        case BTIF_AVRCP_POP_RECORD:
            key = OHOS_AVRCP_KEY_RECORD;
            break;

        case BTIF_AVRCP_POP_REWIND:
            key = OHOS_AVRCP_KEY_REWIND;
            break;
            
        case BTIF_AVRCP_POP_FAST_FORWARD:
            key = OHOS_AVRCP_KEY_FAST_FORWARD;
            break;

        case BTIF_AVRCP_POP_FORWARD:
            key = OHOS_AVRCP_KEY_FORWARD;
            break;

        case BTIF_AVRCP_POP_BACKWARD:
            key = OHOS_AVRCP_KEY_BACKWARD;
            break;

        default:
            break;
    }

    return key;
}

/*
 * @brief 按下button.
 *
 * @param bdAddr 设备地址
 * @param keyOperation 按键，参考{@link AvrcpCtKeyOperation}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtPressButton(const BdAddr *bdAddr, int keyOperation)
{
    if (bdAddr == NULL){
        return -1;
    }
    avrcp_panel_operation_t key = BTIF_AVRCP_POP_RESERVED;
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(!channel){
        return OHOS_BT_STATUS_FAIL;
    }

    key = AvrcpCtKeyToBes((uint8_t)keyOperation);
    if(key == BTIF_AVRCP_POP_RESERVED)
        return OHOS_BT_STATUS_FAIL;

    bes_status = btif_avrcp_set_panel_key(channel, key, true);
    return bes_status_to_ohos(bes_status);
}

/*
 * @brief 释放button.
 *
 * @param bdAddr 设备地址
 * @param keyOperation 按键，参考{@link AvrcpCtKeyOperation}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtReleaseButton(const BdAddr *bdAddr, int keyOperation)
{
    if (bdAddr == NULL){
        return -1;
    }
    avrcp_panel_operation_t key = BTIF_AVRCP_POP_RESERVED;
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(!channel){
        return OHOS_BT_STATUS_FAIL;
    }

    key = AvrcpCtKeyToBes(keyOperation);
    if(key == BTIF_AVRCP_POP_RESERVED)
        return OHOS_BT_STATUS_FAIL;

    bes_status = btif_avrcp_set_panel_key(channel, key, false);
    return bes_status_to_ohos(bes_status);
}

/*
 * @brief This interface gets the capabilities supported by remote device.
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtGetSupportedCompanies(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(!channel){
        return OHOS_BT_STATUS_FAIL;
    }

    bes_status = btif_avrcp_ct_get_capabilities(channel, BTIF_AVRCP_CAPABILITY_COMPANY_ID);
    return bes_status_to_ohos(bes_status);
}

/*
 * @brief This interface gets the capabilities supported by remote device.
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtGetSupportedEvents(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(!channel){
        return OHOS_BT_STATUS_FAIL;
    }

    bes_status = btif_avrcp_ct_get_capabilities(channel, BTIF_AVRCP_CAPABILITY_EVENTS_SUPPORTED);
    return bes_status_to_ohos(bes_status);
}

/**
 * @brief These interface requests the TG to provide the attributes of the element specified in the parameter.
 *
 * The Media Information PDUs are used to obtain detailed information on a particular media file,\n
 * like song information including title, album, artist, composer, year, etc.。\n
 *
 * @param bdAddr 设备地址
 * @param param attributeId数量及列表，参考{@link AvrcpCtGetElementAttrPara}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtGetElementAttributes(const BdAddr *bdAddr, const AvrcpCtGetElementAttrPara *param)
{
    if (bdAddr == NULL || param == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    avrcp_media_attrId_mask_t mediaMask = 0;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t *)bdAddr->addr);
    if(!channel){
        return OHOS_BT_STATUS_FAIL;
    }
    
    for(int i = 0 ; i < param->attrNum; i ++){
        if(i >= OHOS_AVRCP_ATTR_TRACK_PLAYING_TIME)
            break;

        switch(param->attrIdList[i]){
            case OHOS_AVRCP_ATTR_TITLE:
                mediaMask |= BTIF_AVRCP_ENABLE_MEDIA_ATTR_TITLE;
                break;

            case OHOS_AVRCP_ATTR_ARTIST_NAME:
                mediaMask |= BTIF_AVRCP_ENABLE_MEDIA_ATTR_ARTIST;
                break;

            case OHOS_AVRCP_ATTR_ALBUM_NAME:
                mediaMask |= BTIF_AVRCP_ENABLE_MEDIA_ATTR_ALBUM;
                break;

            case OHOS_AVRCP_ATTR_TRACK_NUMBER:
                mediaMask |= BTIF_AVRCP_ENABLE_MEDIA_ATTR_TRACK;
                break;

            case OHOS_AVRCP_ATTR_TOTAL_NUMBER_OF_TRACKS:
                mediaMask |= BTIF_AVRCP_ENABLE_MEDIA_ATTR_NUM_TRACKS;
                break;

            case OHOS_AVRCP_ATTR_TRACK_GENRE:
                mediaMask |= BTIF_AVRCP_ENABLE_MEDIA_ATTR_GENRE;
                break;
                
            case OHOS_AVRCP_ATTR_TRACK_PLAYING_TIME:
                mediaMask |= BTIF_AVRCP_ENABLE_MEDIA_ATTR_DURATION;
                break;

            default:
                break;
        }
    }

    if(mediaMask)
        bes_status = btif_avrcp_ct_get_media_Info(channel, mediaMask);
    else
        bes_status = BT_STS_FAILED;
    
    return bes_status_to_ohos(bes_status);
}

/**
 * @brief This interface is used by the CT to get the status of the currently playing media at the TG.
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtGetPlayStatus(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t *)bdAddr->addr);
    if(!channel){
        return OHOS_BT_STATUS_FAIL;
    }

    bes_status = btif_avrcp_ct_get_play_status(channel);
    return bes_status_to_ohos(bes_status);
}

/**
 * @brief This interface registers with the TG to receive notifications asynchronously
 * based on specific events occurring.
 *
 * 将TG端支持的事件注册成功后，如果该事件改变会自动通知到CT端。\n
 *
 * @param bdAddr 设备地址
 * @param eventId 需要TG通知的事件ID，参考{@link AvrcpCtNotificationEventId}
 * @param interval Specifies the time interval (in seconds) at which the change in playback position will be notified.
 * 这个参数只用于事件{@link OHOS_AVRCP_EVENT_PLAYBACK_POS_CHANGED}，注册其它事件忽略该参数。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtRegisterNotification(const BdAddr *bdAddr, unsigned char eventId, unsigned int interval)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(!channel){
        return OHOS_BT_STATUS_FAIL;
    }

    switch(eventId){
        case OHOS_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED:
            bes_status = btif_avrcp_ct_register_media_status_notification(channel, interval);
            break;

        case OHOS_AVRCP_EVENT_TRACK_CHANGED:
            bes_status = btif_avrcp_ct_register_track_change_notification(channel, interval);
            break;

        case OHOS_AVRCP_EVENT_VOLUME_CHANGED:
            bes_status = btif_avrcp_ct_register_volume_change_notification(channel, interval);
            break;

        case OHOS_AVRCP_EVENT_PLAYBACK_POS_CHANGED:
            bes_status = btif_avrcp_ct_register_play_pos_notification(channel, interval);
            break;

        default:
            bes_status = BT_STS_FAILED;
            break;
    }

    return bes_status_to_ohos(bes_status);
}

/**
 * @brief This interface is used to set an absolute volume to be used by the rendering device.
 *
 * @param bdAddr 设备地址
 * @param volume 绝对音量百分比，参考{@link AvrcpCtAbsoluteVolume}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtSetAbsoluteVolume(const BdAddr *bdAddr, unsigned char volume)
{
    if (bdAddr == NULL){
        return -1;
    }
    bt_status_t bes_status = BT_STS_SUCCESS;
    btif_avrcp_channel_t* channel = NULL;
    
    channel = btif_get_avrcp_channel_by_addr((uint8_t*)bdAddr->addr);
    if(channel){
#ifndef BT_USE_UX_VOLUME
        a2dp_volume_set(0,volume);
#endif
        bes_status = btif_avrcp_ct_send_volume_change_actual_rsp(channel, volume);
        
    }else{
        return OHOS_BT_STATUS_FAIL;
    }

    return bes_status_to_ohos(bes_status);
}

void AvrcpCtUserEventCallback(avrcp_user_event_e type, uint8_t* addr, uint32_t para)
{
    BdAddr dev_addr;
    if(!g_avrcp_ct_cb_func){
        return;
    }

    if(addr){
        memcpy(dev_addr.addr, addr, OHOS_BD_ADDR_LEN);
    }
    
    switch(type){
        case AVRCP_USER_EVENT_CONNECTION_STATE:
            {
                BtProfileConnectState profile_state = OHOS_PROFILE_STATE_DISCONNECTED;
                uint8_t state = para & 0x0f;

                if(state == BTIF_AVRCP_STATE_CONNECTED)
                    profile_state = OHOS_PROFILE_STATE_CONNECTED;
                else if(state == BTIF_AVRCP_STATE_DISCONNECTED)
                    profile_state = OHOS_PROFILE_STATE_DISCONNECTED;
                if(g_avrcp_ct_cb_func&&g_avrcp_ct_cb_func->connStateChangedCb)
                    g_avrcp_ct_cb_func->connStateChangedCb(&dev_addr, profile_state);
            }
            break;

        case AVRCP_USER_EVENT_KEY_PRESS_STATE:
            {
                AvrcpCtKeyOperation key_code;
                uint8_t ct_key = para & 0xff;
                key_code = AvrcpCtKeyToOh(ct_key);
                if(g_avrcp_ct_cb_func->pressButtonCb)
                    g_avrcp_ct_cb_func->pressButtonCb(&dev_addr, key_code);
            }
            break;

        case AVRCP_USER_EVENT_KEY_RELEASE_STATE:
            {
                AvrcpCtKeyOperation key_code;
                uint8_t ct_key = para & 0xff;
                key_code = AvrcpCtKeyToOh(ct_key);
                if(g_avrcp_ct_cb_func->releaseButtonCb)
                    g_avrcp_ct_cb_func->releaseButtonCb(&dev_addr, key_code);
            }
            break;

        case AVRCP_USER_EVENT_COMPANY_ID:
            {
                uint32_t company_id_data[12];
                AvrcpCtSupportCompanyPara ct_company_id;
                btif_avrcp_support_company_para * company_id = (btif_avrcp_support_company_para*)para;
                
                ct_company_id.companyId = company_id_data;
                ct_company_id.companyNum = company_id->companyNum;
                if(company_id->companyNum > 0){
                    memcpy(company_id_data, company_id->companyId, 4*company_id->companyNum);
                }
                if(g_avrcp_ct_cb_func->supportedCompaniesCb)
                    g_avrcp_ct_cb_func->supportedCompaniesCb(&dev_addr, &ct_company_id);
            }
            break;

        case AVRCP_USER_EVENT_CAPABILITY:
            {
                uint16_t event_mask = para & 0xffff;
                AvrcpCtSupportEventPara ctEventPara;
                uint8_t event_data[16];
                uint8_t index = 0;
                
                ctEventPara.eventId = event_data;
                for(int i = 0; i < 16; i ++){
                    if(event_mask & 0x01){
                        event_data[index] = 1 << i;
                        index ++;
                    }
                    event_mask >>= 1;
                }
                ctEventPara.eventNum = index;
                if(g_avrcp_ct_cb_func->supportedEventCb)
                    g_avrcp_ct_cb_func->supportedEventCb(&dev_addr, &ctEventPara);
            }
            break;

        case AVRCP_USER_EVENT_MEDIA_INFO:
            {
                AvrcpCtElementAttrParaCb attrPara;
                AvrcpCtElementAttrValueCb *tempAtt = NULL;
                AvrcpCtElementAttrValueCb attValue[BTIF_AVRCP_NUM_MEDIA_ATTRIBUTES];
                avrcp_media_attr_t * media_att_p = NULL;
                btif_avrcp_media_info_element * media_info = (btif_avrcp_media_info_element*)para;

                attrPara.attrValue = attValue;
                attrPara.attrNum = media_info->numIds;
                for(int i = 0; i < media_info->numIds; i ++){
                    tempAtt = &attValue[i];
                    media_att_p = &media_info->txt[i];
                    
                    tempAtt->attrId = media_att_p->attrId;
                    tempAtt->characterSetId = media_att_p->charSet;
                    tempAtt->value = media_att_p->string;
                    tempAtt->valueLen = media_att_p->length;
                }
                if(g_avrcp_ct_cb_func->elementAttrCb)
                    g_avrcp_ct_cb_func->elementAttrCb(&dev_addr, &attrPara);
            }
            break;

        case AVRCP_USER_EVENT_PLAY_STATUS:
            {
                adv_play_status_t * play_status = (adv_play_status_t *)para;
                AvrcpCtPlayStatusCb playStatus;

                playStatus.playStatus = (AvrcpCtPlayStatus)play_status->mediaStatus;
                playStatus.songLength = play_status->length;
                playStatus.songPosition = play_status->position;
                if(g_avrcp_ct_cb_func->playStatusCb)
                    g_avrcp_ct_cb_func->playStatusCb(&dev_addr, &playStatus);
            }
            break;

        case AVRCP_USER_EVENT_NOTIFICATION:
            {
                avrcp_adv_notify_parms_t*notify_param = (avrcp_adv_notify_parms_t*)para;
                AvrcpCtNotificationValueCb notificationValue;
                AvrcpCtNotificationEventId notifyEvent;

                switch(notify_param->event){
                    case BTIF_AVRCP_EID_MEDIA_STATUS_CHANGED:
                        {
                            notificationValue.playStatus = (AvrcpCtPlayStatus)notify_param->p.mediaStatus;
                            notifyEvent = OHOS_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED;
                        }
                        break;

                    case BTIF_AVRCP_EID_TRACK_CHANGED:
                        {
                            notificationValue.identifier = notify_param->p.track.lsU32;
                            notificationValue.identifier |= ((unsigned long long)notify_param->p.track.msU32) << 32; 
                            notifyEvent = OHOS_AVRCP_EVENT_TRACK_CHANGED;
                        }
                        break;

                     case BTIF_AVRCP_EID_PLAY_POS_CHANGED:
                        {
                            notificationValue.playbackPosition = (int)notify_param->p.position;
                            notifyEvent = OHOS_AVRCP_EVENT_PLAYBACK_POS_CHANGED;
                        }
                        break;

                    case BTIF_AVRCP_EID_BATT_STATUS_CHANGED:
                        {
                            notificationValue.batteryStatus = (AvrcpCtBatteryStatus)notify_param->p.battStatus;
                            notifyEvent = OHOS_AVRCP_EVENT_BATT_STATUS_CHANGED;
                        }
                        break;

                     case BTIF_AVRCP_EID_SYS_STATUS_CHANGED:
                        {
                            notificationValue.systemStatus = (AvrcpCtSystemStatus)notify_param->p.sysStatus;
                            notifyEvent = OHOS_AVRCP_EVENT_SYSTEM_STATUS_CHANGED;
                        }
                        break;

                    case BTIF_AVRCP_EID_ADDRESSED_PLAYER_CHANGED:
                        {
                            notificationValue.addressedPlayer.playerId = notify_param->p.addrPlayer.playerId;
                            notificationValue.addressedPlayer.uidCounter = notify_param->p.addrPlayer.uidCounter;
                            notifyEvent = OHOS_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED;
                        }
                        break;

                    case BTIF_AVRCP_EID_UIDS_CHANGED:
                        {
                            notificationValue.uidCounter = notify_param->p.uidCounter;
                            notifyEvent = OHOS_AVRCP_EVENT_UIDS_CHANGED;
                        }
                        break;

                    case BTIF_AVRCP_EID_VOLUME_CHANGED:
                        {
                            notificationValue.volume = notify_param->p.volume;
                            notifyEvent = OHOS_AVRCP_EVENT_VOLUME_CHANGED;
                        }
                        break;

                    default:
                        return;
                }
                if(g_avrcp_ct_cb_func->notificationCb)
                    g_avrcp_ct_cb_func->notificationCb(&dev_addr, notifyEvent, &notificationValue);
            }
            break;
            
        default:
            break;
    }
}


/**
 * @brief 注册Avrcp CT回调函数。
 *
 * @param func 回调函数原型，参考{@link BtAvrcpCtCallbacks}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtRegisterCallbacks(BtAvrcpCtCallbacks *func)
{
    g_avrcp_ct_cb_func = func;
    btif_avrcp_enable_hook(true);
    btif_avcp_register_user_event_callback(AvrcpCtUserEventCallback);
    return OHOS_BT_STATUS_SUCCESS;
}

