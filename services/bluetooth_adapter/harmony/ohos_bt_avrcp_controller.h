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
 * @addtogroup 蓝牙服务
 * @{
 *
 * @brief 蓝牙服务模块提供使用蓝牙服务的基本功能接口。
 *
 * 通过本模块可以实现蓝牙服务的打开、关闭，提供常用的蓝牙服务。\n
 * 例如：BT-GAP、BLE、BLE-GATT、BT-数传、HFP、A2DP、AVRCP、MAP、PBAP。\n
 *
 * @since 3
 */

/**
 * @file ohos_bt_avrcp_controller.h
 *
 * @brief AVRCP控制端接口定义。
 *
 * 定义AVRCP CT用到的基本数据结构及对外接口。\n
 *
 * @since 3
 */

#ifndef OHOS_BT_AVRCP_CONTROLLER_H
#define OHOS_BT_AVRCP_CONTROLLER_H

#include "ohos_bt_def.h"

/**
 * @brief AVRCP的按键操作
 *
 * 取值的定义来自于规范：1394 Trade Association , AV/C Panel Subunit。\n
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
 * 在{@link AvrcpCtGetElementAttrPara}结构中使用。\n
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
 * @brief 定义notification事件对应的"EventIDs"。
 *
 * 将TG端支持的事件注册成功后，如果该事件改变会自动通知到CT端。\n
 * 在接口{@link AvrcpCtRegisterNotification}中使用。\n
 * 参考Audio/Video Remote Control 1.6.2 Section 28 Appendix H: list of defined notification events。\n
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
 * @brief 定义绝对音量范围
 *
 * 有效的取值范围[0x00, 0x7F]，表示0%~100%。\n
 * 在接口{@link AvrcpCtSetAbsoluteVolume}中使用。\n
 * 参考Audio/Video Remote Control 1.6.2 Section 6.13.1 Absolute Volume。\n
 *
 * @since 3
 */
typedef enum {
    /** 绝对音量0% */
    OHOS_AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_0 = 0x00,
    /** 绝对音量100% */
    OHOS_AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100 = 0x7F,
    /** 无效值 */
    OHOS_AVRCP_ABSOLUTE_VOLUME_INVALID = 0x80,
} AvrcpCtAbsoluteVolume;

/**
 * @brief AVRCP媒体状态
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
 * @brief AVRCP电量状态
 *
 * 在结构{@link AvrcpCtNotificationValueCb}中使用。\n
 * 与{@link OHOS_AVRCP_EVENT_BATT_STATUS_CHANGED}对应。\n
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
 * @brief AVRCP系统状态
 *
 * 在结构{@link AvrcpCtNotificationValueCb}中使用。\n
 * 与{@link OHOS_AVRCP_EVENT_SYSTEM_STATUS_CHANGED}对应。\n
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
 * @brief 待获取值的element attribute列表.
 *
 * If attrNum is set to zero, all attribute information shall be returned,\n
 * else attribute information for the specified attribute IDs shall be returned by the TG.\n
 * 在{@link AvrcpCtGetElementAttributes}接口中使用。\n
 *
 * @since 3
 */
typedef struct {
    /** attribute数量 */
    unsigned char attrNum;
    /** attribute ID列表，协议规定每个ID占4字节，取值参考{@link AvrcpCtElementAttrId} */
    unsigned int *attrIdList;
} AvrcpCtGetElementAttrPara;

/**
 * @brief 获取到的TG支持的company.
 *
 * The first COMPANY_ID returned is the Bluetooth SIG’s defined Metadata Transfer CompanyID.\n
 * 在回调接口{@link AvrcpCtSupportedCompaniesCallback}接口中使用。\n
 *
 * @since 3
 */
typedef struct {
    /** TG支持的company ID的数量 */
    unsigned char companyNum;
    /** company ID列表，  协议规定每个ID占3字节，接口这里每个ID分配4字节 */
    unsigned int *companyId;
} AvrcpCtSupportCompanyPara;

/**
 * @brief 获取到的TG支持的Event.
 *
 * 协议规定{@link AvrcpCtNotificationEventId}中的0x01和0x02是强制支持的，eventNum取值最小为2。\n
 *
 * 在回调接口{@link AvrcpCtSupportedEventCallback}接口中使用。\n
 *
 * @since 3
 */
typedef struct {
    /** TG支持的Event的数量 */
    unsigned char eventNum;
    /** event ID列表，协议规定每个eventId占1个字节，取值参考{@link AvrcpCtNotificationEventId} */
    unsigned char *eventId;
} AvrcpCtSupportEventPara;

/**
 * @brief TG返回的attribute值.
 *
 * 参考Audio/Video Remote Control 1.6.2 Section 6.6.1 Table 6.27.\n
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
    /** 根据valueLen大小动态分配空间 */
    const char *value;
} AvrcpCtElementAttrValueCb;

/**
 * @brief TG返回的GetElementAttributes响应参数.
 *
 * 在接口{@link AvrcpCtGetElementAttributeCallback}中使用.\n
 *
 * @since 3
 */
typedef struct {
    /** TG返回的attribute数量，1-255 */
    unsigned char attrNum;
    /** 根据attrNum大小动态分配空间，参考{@link AvrcpCtElementAttrValueCb} */
    AvrcpCtElementAttrValueCb *attrValue;
} AvrcpCtElementAttrParaCb;

/**
 * @brief TG返回的Addressed Player Changed notification响应数据.
 *
 * 在结构{@link AvrcpCtNotificationValueCb}中使用.\n
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
 * @brief TG返回的播放状态响应数据.
 *
 * 在接口{@link AvrcpCtGetPlayStatusCallback}中使用.\n
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
 * @brief TG发送Notification的值.
 *
 * 联合体，在接口{@link AvrcpCtNotificationCallback}中使用.\n
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
 * @brief 连接状态变化回调函数
 *
 * @param bdAddr 设备地址
 * @param state 连接状态，参考{@link BtProfileConnectState}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtConnStateChangedCallback)(const BdAddr *bdAddr, BtProfileConnectState state);

/**
 * @brief 收到来自TG的press button响应时回调函数
 *
 * CT调用{@link AvrcpCtPressButton}接口后，TG回复响应
 *
 * @param bdAddr 设备地址
 * @param keyOperation 按键值，参考{@link AvrcpCtKeyOperation}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtPressButtonResponseCallback)(const BdAddr *bdAddr, int keyOperation);

/**
 * @brief 收到来自TG的release button响应时回调函数
 *
 * CT调用{@link AvrcpCtReleaseButton}接口后，TG回复响应
 *
 * @param bdAddr 设备地址
 * @param keyOperation 按键值，参考{@link AvrcpCtKeyOperation}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtReleaseButtonResponseCallback)(const BdAddr *bdAddr, int keyOperation);

/**
 * @brief 收到来自TG的支持的company列表时回调函数
 *
 * CT调用{@link AvrcpCtGetSupportedCompanies}接口后，TG回复
 *
 * @param bdAddr 设备地址
 * @param company 支持的company数量及ID，参考{@link AvrcpCtSupportCompanyPara}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtSupportedCompaniesCallback)(const BdAddr *bdAddr, AvrcpCtSupportCompanyPara *company);

/**
 * @brief 收到来自TG的支持的event列表时回调函数
 *
 * @param bdAddr 设备地址
 * @param event 支持的event数量及ID，参考{@link AvrcpCtSupportEventPara}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtSupportedEventCallback)(const BdAddr *bdAddr, AvrcpCtSupportEventPara *event);

/**
 * @brief 收到来自TG返回的GetElementAttributes响应时回调函数
 *
 * @param bdAddr 设备地址
 * @param attrCbPara TG返回的数据，参考{@link AvrcpCtElementAttrParaCb}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtGetElementAttributeCallback)(const BdAddr *bdAddr, AvrcpCtElementAttrParaCb *attrCbPara);

/**
 * @brief 收到来自TG返回的GetPlayStatus响应时回调函数
 *
 * @param bdAddr 设备地址
 * @param status TG返回的数据，参考{@link AvrcpCtPlayStatusCb}
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtGetPlayStatusCallback)(const BdAddr *bdAddr, AvrcpCtPlayStatusCb *status);

/**
 * @brief 收到来自TG返回的Notification时回调函数
 *
 * 注意：{@link AvrcpCtNotificationEventId}中0x03/0x04/0x09/0x0A协议定义是不返回具体值。\n
 * 因此在{@link AvrcpCtNotificationValueCb}中不体现。\n
 * {@OHOS_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED}需单独定义notification回调函数，不在本接口注册。\n
 *
 * @param bdAddr 设备地址
 * @param eventId 当前TG返回通知对应的eventId.
 * @param notification eventId对应的值.
 * @return void
 * @since 3
 */
typedef void (*AvrcpCtNotificationCallback)(const BdAddr *bdAddr, AvrcpCtNotificationEventId eventId,
                                            AvrcpCtNotificationValueCb *notification);

/**
 * @brief Avrcp CT回调函数接口定义
 *
 * @since 3
 */
typedef struct {
    /** 连接状态变化 */
    AvrcpCtConnStateChangedCallback connStateChangedCb;
    /** 按下button */
    AvrcpCtPressButtonResponseCallback pressButtonCb;
    /** 释放button */
    AvrcpCtReleaseButtonResponseCallback releaseButtonCb;
    /** 支持的Companies */
    AvrcpCtSupportedCompaniesCallback supportedCompaniesCb;
    /** 支持的events */
    AvrcpCtSupportedEventCallback supportedEventCb;
    /** Element attribute */
    AvrcpCtGetElementAttributeCallback elementAttrCb;
    /** 播放状态 */
    AvrcpCtGetPlayStatusCallback playStatusCb;
    /** Notification */
    AvrcpCtNotificationCallback notificationCb;
} BtAvrcpCtCallbacks;

/**
 * @brief 获取设备连接状态。
 *
 * @param bdAddr 设备地址
 * @return 返回{@link BtProfileConnectState}
 * @since 3
 */
int AvrcpCtGetDeviceConnectState(const BdAddr *bdAddr);

/*
 * @brief 建立与TG的连接
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtConnect(const BdAddr *bdAddr);

/*
 * @brief 断开与TG的连接
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtDisconnect(const BdAddr *bdAddr);

/*
 * @brief 按下button.
 *
 * @param bdAddr 设备地址
 * @param keyOperation 按键，参考{@link AvrcpCtKeyOperation}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtPressButton(const BdAddr *bdAddr, int keyOperation);

/*
 * @brief 释放button.
 *
 * @param bdAddr 设备地址
 * @param keyOperation 按键，参考{@link AvrcpCtKeyOperation}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtReleaseButton(const BdAddr *bdAddr, int keyOperation);

/*
 * @brief This interface gets the capabilities supported by remote device.
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtGetSupportedCompanies(const BdAddr *bdAddr);

/*
 * @brief This interface gets the capabilities supported by remote device.
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtGetSupportedEvents(const BdAddr *bdAddr);

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
int AvrcpCtGetElementAttributes(const BdAddr *bdAddr, const AvrcpCtGetElementAttrPara *param);

/**
 * @brief This interface is used by the CT to get the status of the currently playing media at the TG.
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtGetPlayStatus(const BdAddr *bdAddr);

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
int AvrcpCtRegisterNotification(const BdAddr *bdAddr, unsigned char eventId, unsigned int interval);

/**
 * @brief This interface is used to set an absolute volume to be used by the rendering device.
 *
 * @param bdAddr 设备地址
 * @param volume 绝对音量百分比，参考{@link AvrcpCtAbsoluteVolume}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtSetAbsoluteVolume(const BdAddr *bdAddr, unsigned char volume);

/**
 * @brief 注册Avrcp CT回调函数。
 *
 * @param func 回调函数原型，参考{@link BtAvrcpCtCallbacks}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int AvrcpCtRegisterCallbacks(BtAvrcpCtCallbacks *func);

#endif
/** @} */

