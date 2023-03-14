/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup ��������
 * @{
 *
 * @brief ��������ģ���ṩʹ����������Ļ������ܽӿڡ�
 *
 * ͨ����ģ�����ʵ����������Ĵ򿪡��رգ��ṩ���õ���������\n
 * ���磺BT-GAP��BLE��BLE-GATT��BT-������HFP��A2DP��AVRCP��MAP��PBAP��\n
 *
 * @since 3
 */

/**
 * @file ohos_bt_avrcp_controller.h
 *
 * @brief AVRCP���ƶ˽ӿڶ��塣
 *
 * ����AVRCP CT�õ��Ļ������ݽṹ������ӿڡ�\n
 *
 * @since 3
 */

#ifndef OHOS_BT_AVRCP_CONTROLLER_H
#define OHOS_BT_AVRCP_CONTROLLER_H

#include "ohos_bt_def.h"

/**
 * @brief AVRCP�İ�������
 *
 * ȡֵ�Ķ��������ڹ淶��1394 Trade Association , AV/C Panel Subunit��\n
 *
 * @since 3
 */
typedef enum {
    /** Turns the volume to high. */
    OHOS_AVRCP_KEY_VOLUME_UP = 0x41,
    /** Turns the volume to low. */
    OHOS_AVRCP_KEY_VOLUME_DOWN = 0x42,
    /** Mute */
    OHOS_AVRCP_KEY_MUTE = 0x43,
    /** Play */
    OHOS_AVRCP_KEY_PLAY = 0x44,
    /** Stop */
    OHOS_AVRCP_KEY_STOP = 0x45,
    /** Pause */
    OHOS_AVRCP_KEY_PAUSE = 0x46,
    /** Record */
    OHOS_AVRCP_KEY_RECORD = 0x47,
    /** Moves the position toward the beginning of the medium. */
    OHOS_AVRCP_KEY_REWIND = 0x48,
    /** Fast forward, moves the position away from the beginning of the medium. */
    OHOS_AVRCP_KEY_FAST_FORWARD = 0x49,
    /** Switches the contents, such as music tune or video chapter to the forward one. */
    OHOS_AVRCP_KEY_FORWARD = 0x4B,
    /** Switches the contents, such as music tune or video chapter to the backward one */
    OHOS_AVRCP_KEY_BACKWARD = 0x4C,
    /** Invalid */
    OHOS_AVRCP_KEY_INVALID = 0x7F,
} AvrcpCtKeyOperation;

/**
 * @brief AVRCP element attribute ID.
 *
 * ��{@link AvrcpCtGetElementAttrPara}�ṹ��ʹ�á�\n
 *
 * @since 3
 */
typedef enum {
    /** Title, song name or content description. */
    OHOS_AVRCP_ATTR_TITLE = 0x01,
    /** Artist(s), performer(s) or group coded per specified character set. */
    OHOS_AVRCP_ATTR_ARTIST_NAME = 0x02,
    /** Album */
    OHOS_AVRCP_ATTR_ALBUM_NAME = 0x03,
    /* The order number of the audio-file on its original recording. */
    OHOS_AVRCP_ATTR_TRACK_NUMBER = 0x04,
    /** The total number of tracks/elements on the original recording */
    OHOS_AVRCP_ATTR_TOTAL_NUMBER_OF_TRACKS = 0x05,
    /** Genre */
    OHOS_AVRCP_ATTR_TRACK_GENRE = 0x06,
    /** The length of the audio file in milliseconds */
    OHOS_AVRCP_ATTR_TRACK_PLAYING_TIME = 0x07
} AvrcpCtElementAttrId;

/**
 * @brief ����notification�¼���Ӧ��"EventIDs"��
 *
 * ��TG��֧�ֵ��¼�ע��ɹ���������¼��ı���Զ�֪ͨ��CT�ˡ�\n
 * �ڽӿ�{@link AvrcpCtRegisterNotification}��ʹ�á�\n
 * �ο�Audio/Video Remote Control 1.6.2 Section 28 Appendix H: list of defined notification events��\n
 *
 * @since 3
 */
typedef enum {
    /** Change in playback status of the current track. */
    OHOS_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED = 0x01,
    /** Change of current track. */
    OHOS_AVRCP_EVENT_TRACK_CHANGED = 0x02,
    /** Reached end of a track. */
    OHOS_AVRCP_EVENT_TRACK_REACHED_END = 0x03,
    /** Reached start of a track. */
    OHOS_AVRCP_EVENT_TRACK_REACHED_START = 0x04,
    /** Change in playback position. */
    OHOS_AVRCP_EVENT_PLAYBACK_POS_CHANGED = 0x05,
    /** Change in battery status. */
    OHOS_AVRCP_EVENT_BATT_STATUS_CHANGED = 0x06,
    /** Change in system status. */
    OHOS_AVRCP_EVENT_SYSTEM_STATUS_CHANGED = 0x07,
    /** Change in player application setting. */
    OHOS_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = 0x08,
    /** The content of the Now Playing list has changed. */
    OHOS_AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED = 0x09,
    /** The available players have changed. */
    OHOS_AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED = 0x0A,
    /** The Addressed Player has been changed. */
    OHOS_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED = 0x0B,
    /** The UIDs have changed. */
    OHOS_AVRCP_EVENT_UIDS_CHANGED = 0x0C,
    /** The volume has been changed locally on the TG. */
    OHOS_AVRCP_EVENT_VOLUME_CHANGED = 0x0D,
    /** Reserved for future use. */
    OHOS_AVRCP_EVENT_RESERVED = 0x0E
} AvrcpCtNotificationEventId;

/**
 * @brief �������������Χ
 *
 * ��Ч��ȡֵ��Χ[0x00, 0x7F]����ʾ0%~100%��\n
 * �ڽӿ�{@link AvrcpCtSetAbsoluteVolume}��ʹ�á�\n
 * �ο�Audio/Video Remote Control 1.6.2 Section 6.13.1 Absolute Volume��\n
 *
 * @since 3
 */
typedef enum {
    /** ��������0% */
    OHOS_AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_0 = 0x00,
    /** ��������100% */
    OHOS_AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100 = 0x7F,
    /** ��Чֵ */
    OHOS_AVRCP_ABSOLUTE_VOLUME_INVALID = 0x80,
} AvrcpCtAbsoluteVolume;

/**
 * @brief AVRCPý��״̬
 *
 * @since 3
 */
typedef enum {
    /** STOPPED */
    OHOS_AVRCP_PLAY_STATUS_STOPPED = 0x00,
    /** PLAYING */
    OHOS_AVRCP_PLAY_STATUS_PLAYING = 0x01,
    /** PAUSED */
    OHOS_AVRCP_PLAY_STATUS_PAUSED = 0x02,
    /** FWD_SEEK */
    OHOS_AVRCP_PLAY_STATUS_FWD_SEEK = 0x03,
    /** REV_SEEK */
    OHOS_AVRCP_PLAY_STATUS_REV_SEEK = 0x04,
    /** ERROR */
    OHOS_AVRCP_PLAY_STATUS_ERROR = 0xFF
} AvrcpCtPlayStatus;

/**
 * @brief AVRCP����״̬
 *
 * �ڽṹ{@link AvrcpCtNotificationValueCb}��ʹ�á�\n
 * ��{@link OHOS_AVRCP_EVENT_BATT_STATUS_CHANGED}��Ӧ��\n
 *
 * @since 3
 */
typedef enum {
    /** Battery operation is in normal state. */
    OHOS_AVRCP_BATTERY_STATUS_NORMAL = 0x00,
    /** unable to operate soon. Is provided when the battery level is going down. */
    OHOS_AVRCP_BATTERY_STATUS_WARNING = 0x01,
    /** Cannot operate any more. Is provided when the battery level is going down. */
    OHOS_AVRCP_BATTERY_STATUS_CRITICAL = 0x02,
    /** Plugged to external power supply. */
    OHOS_AVRCP_BATTERY_STATUS_EXTERNAL = 0x03,
    /** when the device is completely charged from the external power supp */
    OHOS_AVRCP_BATTERY_STATUS_FULL_CHARGE = 0x04,
} AvrcpCtBatteryStatus;

/**
 * @brief AVRCPϵͳ״̬
 *
 * �ڽṹ{@link AvrcpCtNotificationValueCb}��ʹ�á�\n
 * ��{@link OHOS_AVRCP_EVENT_SYSTEM_STATUS_CHANGED}��Ӧ��\n
 *
 * @since 3
 */
typedef enum {
    /** Power on. */
    OHOS_AVRCP_SYSTEM_STATUS_POWER_ON = 0x00,
    /** Power off. */
    OHOS_AVRCP_SYSTEM_STATUS_POWER_OFF = 0x01,
    /** unplugged. */
    OHOS_AVRCP_SYSTEM_STATUS_UNPLUGGED = 0x02
} AvrcpCtSystemStatus;

/**
 * @brief ����ȡֵ��element attribute�б�.
 *
 * If attrNum is set to zero, all attribute information shall be returned,\n
 * else attribute information for the specified attribute IDs shall be returned by the TG.\n
 * ��{@link AvrcpCtGetElementAttributes}�ӿ���ʹ�á�\n
 *
 * @since 3
 */
typedef struct {
    /** attribute���� */
    unsigned char attrNum;
    /** attribute ID�б���Э��涨ÿ��IDռ4�ֽڣ�ȡֵ�ο�{@link AvrcpCtElementAttrId} */
    unsigned int *attrIdList;
} AvrcpCtGetElementAttrPara;

/**
 * @brief ��ȡ����TG֧�ֵ�company.
 *
 * The first COMPANY_ID returned is the Bluetooth SIG��s defined Metadata Transfer CompanyID.\n
 * �ڻص��ӿ�{@link AvrcpCtSupportedCompaniesCallback}�ӿ���ʹ�á�\n
 *
 * @since 3
 */
typedef struct {
    /** TG֧�ֵ�company ID������ */
    unsigned char companyNum;
    /** company ID�б���  Э��涨ÿ��IDռ3�ֽڣ��ӿ�����ÿ��ID����4�ֽ� */
    unsigned int *companyId;
} AvrcpCtSupportCompanyPara;

/**
 * @brief ��ȡ����TG֧�ֵ�Event.
 *
 * Э��涨{@link AvrcpCtNotificationEventId}�е�0x01��0x02��ǿ��֧�ֵģ�eventNumȡֵ��СΪ2��\n
 *
 * �ڻص��ӿ�{@link AvrcpCtSupportedEventCallback}�ӿ���ʹ�á�\n
 *
 * @since 3
 */
typedef struct {
    /** TG֧�ֵ�Event������ */
    unsigned char eventNum;
    /** event ID�б���Э��涨ÿ��eventIdռ1���ֽڣ�ȡֵ�ο�{@link AvrcpCtNotificationEventId} */
    unsigned char *eventId;
} AvrcpCtSupportEventPara;

/**
 * @brief TG���ص�attributeֵ.
 *
 * �ο�Audio/Video Remote Control 1.6.2 Section 6.6.1 Table 6.27.\n
 *
 * @since 3
 */
typedef struct {
    /** Specifies the attribute ID to be written. */
    unsigned int attrId;
    /** Specifies the character set ID to be displayed on CT. */
    unsigned short characterSetId;
    /** Length of the value of the attribute. 0-65535 (0, if no name is provided) */
    unsigned short valueLen;
    /** ����valueLen��С��̬����ռ� */
    const char *value;
} AvrcpCtElementAttrValueCb;

/**
 * @brief TG���ص�GetElementAttributes��Ӧ����.
 *
 * �ڽӿ�{@link AvrcpCtGetElementAttributeCallback}��ʹ��.\n
 *
 * @since 3
 */
typedef struct {
    /** TG���ص�attribute������1-255 */
    unsigned char attrNum;
    /** ����attrNum��С��̬����ռ䣬�ο�{@link AvrcpCtElementAttrValueCb} */
    AvrcpCtElementAttrValueCb *attrValue;
} AvrcpCtElementAttrParaCb;

/**
 * @brief TG���ص�Addressed Player Changed notification��Ӧ����.
 *
 * �ڽṹ{@link AvrcpCtNotificationValueCb}��ʹ��.\n
 *
 * @since 3
 */
typedef struct {
    /** Unique Media Player Id. */
    unsigned short playerId;
    /** UID Counter. */
    unsigned short uidCounter;
} AvrcpCtAddressedPlayerChangedCb;

/**
 * @brief TG���صĲ���״̬��Ӧ����.
 *
 * �ڽӿ�{@link AvrcpCtGetPlayStatusCallback}��ʹ��.\n
 * If TG does not support songLength and songPosition on TG, then TG shall return 0xFFFFFFFF.\n
 *
 * @since 3
 */
typedef struct {
    /** The total length of the playing song in milliseconds. */
    unsigned int songLength;
    /** The current position of the playing in milliseconds elapsed. */
    unsigned int songPosition;
    /** Current Status of playing. */
    AvrcpCtPlayStatus playStatus;
} AvrcpCtPlayStatusCb;

/**
 * @brief TG����Notification��ֵ.
 *
 * �����壬�ڽӿ�{@link AvrcpCtNotificationCallback}��ʹ��.\n
 *
 * @since 3
 */
typedef union {
    /** {@link OHOS_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED} */
    AvrcpCtPlayStatus playStatus;
    /** {@link OHOS_AVRCP_EVENT_TRACK_CHANGED} */
    unsigned long long identifier;
    /** {@link OHOS_AVRCP_EVENT_PLAYBACK_POS_CHANGED}.
    Current playback position in millisecond.
    If no track currently selected, then return 0xFFFFFFFF in the INTERIM response. */
    unsigned int playbackPosition;
    /** {@link OHOS_AVRCP_EVENT_BATT_STATUS_CHANGED} */
    AvrcpCtBatteryStatus batteryStatus;
    /** {@link OHOS_AVRCP_EVENT_SYSTEM_STATUS_CHANGED } */
    AvrcpCtSystemStatus systemStatus;
    /** {@link OHOS_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED } */
    AvrcpCtAddressedPlayerChangedCb addressedPlayer;
    /** {@link OHOS_AVRCP_EVENT_UIDS_CHANGED } */
    unsigned short uidCounter;
    /** {@link OHOS_AVRCP_EVENT_VOLUME_CHANGED} */
    unsigned char volume;
} AvrcpCtNotificationValueCb;

/**
 * @brief ����״̬�仯�ص�����
 *
 * @param bdAddr �豸��ַ
 * @param state ����״̬���ο�{@link BtProfileConnectState}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtConnStateChangedCallback)(const BdAddr *bdAddr, BtProfileConnectState state);

/**
 * @brief �յ�����TG��press button��Ӧʱ�ص�����
 *
 * CT����{@link AvrcpCtPressButton}�ӿں�TG�ظ���Ӧ
 *
 * @param bdAddr �豸��ַ
 * @param keyOperation ����ֵ���ο�{@link AvrcpCtKeyOperation}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtPressButtonResponseCallback)(const BdAddr *bdAddr, int keyOperation);

/**
 * @brief �յ�����TG��release button��Ӧʱ�ص�����
 *
 * CT����{@link AvrcpCtReleaseButton}�ӿں�TG�ظ���Ӧ
 *
 * @param bdAddr �豸��ַ
 * @param keyOperation ����ֵ���ο�{@link AvrcpCtKeyOperation}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtReleaseButtonResponseCallback)(const BdAddr *bdAddr, int keyOperation);

/**
 * @brief �յ�����TG��֧�ֵ�company�б�ʱ�ص�����
 *
 * CT����{@link AvrcpCtGetSupportedCompanies}�ӿں�TG�ظ�
 *
 * @param bdAddr �豸��ַ
 * @param company ֧�ֵ�company������ID���ο�{@link AvrcpCtSupportCompanyPara}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtSupportedCompaniesCallback)(const BdAddr *bdAddr, AvrcpCtSupportCompanyPara *company);

/**
 * @brief �յ�����TG��֧�ֵ�event�б�ʱ�ص�����
 *
 * @param bdAddr �豸��ַ
 * @param event ֧�ֵ�event������ID���ο�{@link AvrcpCtSupportEventPara}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtSupportedEventCallback)(const BdAddr *bdAddr, AvrcpCtSupportEventPara *event);

/**
 * @brief �յ�����TG���ص�GetElementAttributes��Ӧʱ�ص�����
 *
 * @param bdAddr �豸��ַ
 * @param attrCbPara TG���ص����ݣ��ο�{@link AvrcpCtElementAttrParaCb}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtGetElementAttributeCallback)(const BdAddr *bdAddr, AvrcpCtElementAttrParaCb *attrCbPara);

/**
 * @brief �յ�����TG���ص�GetPlayStatus��Ӧʱ�ص�����
 *
 * @param bdAddr �豸��ַ
 * @param status TG���ص����ݣ��ο�{@link AvrcpCtPlayStatusCb}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtGetPlayStatusCallback)(const BdAddr *bdAddr, AvrcpCtPlayStatusCb *status);

/**
 * @brief �յ�����TG���ص�Notificationʱ�ص�����
 *
 * ע�⣺{@link AvrcpCtNotificationEventId}��0x03/0x04/0x09/0x0AЭ�鶨���ǲ����ؾ���ֵ��\n
 * �����{@link AvrcpCtNotificationValueCb}�в����֡�\n
 * {@OHOS_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED}�赥������notification�ص����������ڱ��ӿ�ע�ᡣ\n
 *
 * @param bdAddr �豸��ַ
 * @param eventId ��ǰTG����֪ͨ��Ӧ��eventId.
 * @param notification eventId��Ӧ��ֵ.
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtNotificationCallback)(const BdAddr *bdAddr, AvrcpCtNotificationEventId eventId,
                                            AvrcpCtNotificationValueCb *notification);

/**
 * @brief Avrcp CT�ص������ӿڶ���
 *
 * @since 3
 */
typedef struct {
    /** ����״̬�仯 */
    AvrcpCtConnStateChangedCallback connStateChangedCb;
    /** ����button */
    AvrcpCtPressButtonResponseCallback pressButtonCb;
    /** �ͷ�button */
    AvrcpCtReleaseButtonResponseCallback releaseButtonCb;
    /** ֧�ֵ�Companies */
    AvrcpCtSupportedCompaniesCallback supportedCompaniesCb;
    /** ֧�ֵ�events */
    AvrcpCtSupportedEventCallback supportedEventCb;
    /** Element attribute */
    AvrcpCtGetElementAttributeCallback elementAttrCb;
    /** ����״̬ */
    AvrcpCtGetPlayStatusCallback playStatusCb;
    /** Notification */
    AvrcpCtNotificationCallback notificationCb;
} BtAvrcpCtCallbacks;

/**
 * @brief ��ȡ�豸����״̬��
 *
 * @param bdAddr �豸��ַ
 * @return ����{@link BtProfileConnectState}
 * @since 3
 */
int AvrcpCtGetDeviceConnectState(const BdAddr *bdAddr);

/*
 * @brief ������TG������
 *
 * @param bdAddr �豸��ַ
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtConnect(const BdAddr *bdAddr);

/*
 * @brief �Ͽ���TG������
 *
 * @param bdAddr �豸��ַ
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtDisconnect(const BdAddr *bdAddr);

/*
 * @brief ����button.
 *
 * @param bdAddr �豸��ַ
 * @param keyOperation �������ο�{@link AvrcpCtKeyOperation}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtPressButton(const BdAddr *bdAddr, int keyOperation);

/*
 * @brief �ͷ�button.
 *
 * @param bdAddr �豸��ַ
 * @param keyOperation �������ο�{@link AvrcpCtKeyOperation}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtReleaseButton(const BdAddr *bdAddr, int keyOperation);

/*
 * @brief This interface gets the capabilities supported by remote device.
 *
 * @param bdAddr �豸��ַ
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtGetSupportedCompanies(const BdAddr *bdAddr);

/*
 * @brief This interface gets the capabilities supported by remote device.
 *
 * @param bdAddr �豸��ַ
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtGetSupportedEvents(const BdAddr *bdAddr);

/**
 * @brief These interface requests the TG to provide the attributes of the element specified in the parameter.
 *
 * The Media Information PDUs are used to obtain detailed information on a particular media file,\n
 * like song information including title, album, artist, composer, year, etc.��\n
 *
 * @param bdAddr �豸��ַ
 * @param param attributeId�������б����ο�{@link AvrcpCtGetElementAttrPara}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtGetElementAttributes(const BdAddr *bdAddr, const AvrcpCtGetElementAttrPara *param);

/**
 * @brief This interface is used by the CT to get the status of the currently playing media at the TG.
 *
 * @param bdAddr �豸��ַ
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtGetPlayStatus(const BdAddr *bdAddr);

/**
 * @brief This interface registers with the TG to receive notifications asynchronously
 * based on specific events occurring.
 *
 * ��TG��֧�ֵ��¼�ע��ɹ���������¼��ı���Զ�֪ͨ��CT�ˡ�\n
 *
 * @param bdAddr �豸��ַ
 * @param eventId ��ҪTG֪ͨ���¼�ID���ο�{@link AvrcpCtNotificationEventId}
 * @param interval Specifies the time interval (in seconds) at which the change in playback position will be notified.
 * �������ֻ�����¼�{@link OHOS_AVRCP_EVENT_PLAYBACK_POS_CHANGED}��ע�������¼����Ըò�����
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtRegisterNotification(const BdAddr *bdAddr, unsigned char eventId, unsigned int interval);

/**
 * @brief This interface is used to set an absolute volume to be used by the rendering device.
 *
 * @param bdAddr �豸��ַ
 * @param volume ���������ٷֱȣ��ο�{@link AvrcpCtAbsoluteVolume}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtSetAbsoluteVolume(const BdAddr *bdAddr, unsigned char volume);

/**
 * @brief This interface is used to send avrcp vendor data to target device, such as report a2dp latency to mobile.
 *
 * @param bdAddr �豸��ַ
 * @param is_press:        true/flase
 * @param vendor_data_len: ����˽���������ݵ��ֽ���
 * @param vendor_data:     ����˽����������
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtSendVendorCmd(const BdAddr *bdAddr, bool is_press, unsigned short vendor_data_len, unsigned char *vendor_data);

/**
 * @brief ע��Avrcp CT�ص�������
 *
 * @param func �ص�����ԭ�ͣ��ο�{@link BtAvrcpCtCallbacks}��
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int AvrcpCtRegisterCallbacks(BtAvrcpCtCallbacks *func);

#endif
/** @} */

