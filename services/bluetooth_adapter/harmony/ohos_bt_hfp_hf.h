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
 * @file ohos_bt_hfp_hf.h
 *
 * @brief Hands-Free unit (HF)端接口定义。
 *
 * 定义HFP HF用到的基本数据结构及对外接口。\n
 *
 * @since 3
 */

#ifndef OHOS_BT_HFP_HF_H
#define OHOS_BT_HFP_HF_H

#include "ohos_bt_def.h"

/**
 * @brief 接受一路呼叫时对其他呼叫的操作策略
 *
 * 具体参考"AT+CHLD"指令的定义。
 *
 * @since 3
 */
typedef enum {
    /** 无操作(默认) */
    OHOS_HFP_HF_ACCEPT_CALL_ACTION_NONE = 0,
    /** Hold */
    OHOS_HFP_HF_ACCEPT_CALL_ACTION_HOLD = 1,
    /** Finish */
    OHOS_HFP_HF_ACCEPT_CALL_ACTION_FINISH = 2
} HfpHfAcceptCallActionType;

/**
 * @brief 采样率的类型, mSBC/CVSD
 *
 * @since 3
 */
typedef enum {
    OHOS_HFP_HF_SCO_CODEC_NONE = 0,
    OHOS_HFP_HF_SCO_CODEC_CVSD = 1,
    OHOS_HFP_HF_SCO_CODEC_MSBC = 2
} HfpHfScoCodecType;

/**
 * @brief Profile连接状态变化回调函数
 *
 * @param bdAddr 设备地址
 * @param state 连接状态，参考{@link BtProfileConnectState}
 * @return void
 * @since 3
 */
typedef void (*HfpHfConnStateChangedCallback)(const BdAddr *bdAddr, BtProfileConnectState state);

/**
 * @brief Sco连接状态变化回调函数
 *
 * @param bdAddr 设备地址
 * @param state 连接状态，参考{@link BtHfpScoConnectState}
 * @return void
 * @since 3
 */
typedef void (*HfpHfScoConnStateChangedCallback)(const BdAddr *bdAddr, BtHfpScoConnectState state);

/**
 * @brief 收到AG电量变化指示回调函数
 *
 * 收到AG的指令"+CIEV: <Battery level Ind>,<Value>"后通过该接口回调给上层。\n
 * <Value>指示电量等级，取值范围0-5。
 *
 * @param bdAddr 设备地址
 * @param batteryLevel 电量等级，范围0-5
 * @return void
 * @since 3
 */
typedef void (*HfpHfBatteryLevelChangedCallback)(const BdAddr *bdAddr, int batteryLevel);

/**
 * @brief 收到AG信号强度变化指示回调函数
 *
 * 收到AG的指令"+CIEV: <Signal strength Ind>,<Value>"后通过该接口回调给上层。\n
 * <Value>指示信号强度等级，取值范围0-5
 *
 * @param bdAddr 设备地址
 * @param signalStrength 信号强度等级，范围0-5
 * @return void
 * @since 3
 */
typedef void (*HfpHfSignalStrengthChangedCallback)(const BdAddr *bdAddr, int signalStrength);

/**
 * @brief 收到AG注册状态变化指示回调函数
 *
 * 收到AG的指令"+CIEV: <Service Ind>,<Value>"后通过该接口回调给上层。\n
 * <Value>指示信号强度等级，取值为0或1。\n
 * <value>=0 implies no service. No Home/Roam network available.\n
 * <value>=1 implies presence of service. Home/Roam network available.
 *
 * @param bdAddr 设备地址
 * @param status 注册状态，取值为0或1。
 * @return void
 * @since 3
 */
typedef void (*HfpHfRegistrationStatusChangedCallback)(const BdAddr *bdAddr, int status);

/**
 * @brief 收到AG漫游状态变化指示回调函数
 *
 * 收到AG的指令"+CIEV: <Roaming Ind>,<Value>"后通过该接口回调给上层。\n
 * <Value>指示漫游状态，取值为0或1。\n
 * <value>=0 means roaming is not active.\n
 * <value>=1 means a roaming is active.
 *
 * @param bdAddr 设备地址
 * @param status 漫游状态，取值为0或1。
 * @return void
 * @since 3
 */
typedef void (*HfpHfRoamingStatusChangedCallback)(const BdAddr *bdAddr, int status);

/**
 * @brief 查询AG运营商信息收到AG回复后的回调函数
 *
 * 收到AG的指令"+COPS: <mode>, 0, <operator>"后通过该接口回调给上层。
 *
 * @param bdAddr 设备地址
 * @param name 运营商名字
 * @param len 运营商名字长度
 * @return void
 * @since 3
 */
typedef void (*HfpHfOperatorSelectionCallback)(const BdAddr *bdAddr, unsigned char *name, unsigned int len);

/**
 * @brief 查询AG用户号码收到AG回复后回调函数
 *
 * 收到AG的指令"+CNUM: ,<number>,<type>[,,<service>]"后通过该接口回调给上层。\n
 * 例如：收到"+CNUM: ,"5551212",129,,4",用户号码是"5551212"。
 *
 * @param bdAddr 设备地址
 * @param name 运营商名字
 * @param len 运营商名字长度
 * @return void
 * @since 3
 */
typedef void (*HfpHfSubscriberNumberCallback)(const BdAddr *bdAddr, const char *number, unsigned int len);

/**
 * @brief 语音识别状态变化回调函数
 *
 * AG端主动active/deactive语音识别，HF收到指令"+BVRA: <vrect>"后通过该接口回调给上层。\n
 * HF端主动active/deactive语音识别，发送"AT+BVRA=<vrec>"指令，收到AG端"OK"后通过该接口回调给上层。\n
 *
 * <vrect>=0,Voice recognition is disabled in the AG.
 * <vrect>=1,Voice recognition is enabled in the AG.
 *
 * @param bdAddr 设备地址
 * @param status 语音识别状态取值参考<vrect>
 * @return void
 * @since 3
 */
typedef void (*HfpHfVoiceRecognitionStatusChangedCallback)(const BdAddr *bdAddr, int status);

/**
 * @brief In-band Ring tone状态变化回调函数
 *
 * HF收到指令"+BSIR: <bsir>"后通过该接口回调给上层。
 *
 * <bsir>=0,The AG provides no in-band ring tone.
 * <bsir>=1,The AG provides an in-band ring tone.
 *
 * @param bdAddr 设备地址
 * @param status In-band Ring tone状态取值参考<bsir>
 * @return void
 * @since 3
 */
typedef void (*HfpHfInBandRingToneChangedCallback)(const BdAddr *bdAddr, int status);

/**
 * @brief AG端调整HF音量回调函数
 *
 * HF收到指令"+VGM:<gain>"或"+VGS:<gain>"后通过该接口回调给上层。
 *
 * @param bdAddr 设备地址
 * @param type 调整microphone/speaker，参考{@link BtHfpVolumeType}
 * @param volume 音量增益，范围0-15
 * @return void
 * @since 3
 */
typedef void (*HfpHfVolumeChangedCallback)(const BdAddr *bdAddr, BtHfpVolumeType type, unsigned char volume);

/**
 * @brief AG通话事件回调
 *
 * HF收到指令"+CIEV"后通过该接口回调给上层。
 *
 * @param bdAddr 设备地址
 * @param status 状态取值0/1
 * @return void
 * @since 3
 */
typedef void (*HfpHfClientIndCallCallback)(const BdAddr *bdAddr, int status);

/**
 * @brief AG通话建立的状态回调
 *
 * HF收到指令"+CIEV"后通过该接口回调给上层。
 *
 * @param bdAddr 设备地址
 * @param status 状态取值0~3
 * @return void
 * @since 3
 */
typedef void (*HfpHfClientIndCallSetupCallback)(const BdAddr *bdAddr, int status);

/**
 * @brief AG来电铃声事件回调
 *
 * HF收到指令"+RING"后通过该接口回调给上层。
 *
 * @param bdAddr 设备地址
 * @return void
 * @since 3
 */
typedef void (*HfpHfClientRingIndCallback)(const BdAddr *bdAddr);

/**
 * @brief 协议栈采样率上报应用层回调函数
 *
 * @param bdAddr 设备地址
 * @param codeType 采样率类型，参考{@link HfpHfScoCodecType}
 * @return void
 * @since 3
 */
typedef void (*HfpHfCodecTypeNotifyCallback)(const BdAddr *bdAddr, HfpHfScoCodecType codeType);

/**
 * @brief HFP HF回调函数接口定义
 *
 * @since 3
 */
typedef struct {
    /** profile连接状态变化回调 */
    HfpHfConnStateChangedCallback connStateChangedCb;
    /** sco连接状态变化回调*/
    HfpHfScoConnStateChangedCallback scoConnStateChangedCb;
    /** AG电量指示变化回调 */
    HfpHfBatteryLevelChangedCallback batteryLevelChangedCb;
    /** AG信号强度指示变化回调 */
    HfpHfSignalStrengthChangedCallback signalStrengthChangedCb;
    /** AG注册状态指示变化回调 */
    HfpHfRegistrationStatusChangedCallback registrationStatusChangedCb;
    /** AG漫游状态变化指示变化回调 */
    HfpHfRoamingStatusChangedCallback roamingStatusChangedCb;
    /** 查询AG运营商信息回调 */
    HfpHfOperatorSelectionCallback operatorSelectionCb;
    /** 查询查询AG用户号码回调 */
    HfpHfSubscriberNumberCallback subscriberNumberCb;
    /** 语音识别状态变化回调 */
    HfpHfVoiceRecognitionStatusChangedCallback voiceRecognitionStatusChangedCb;
    /** In-band Ring tone状态变化回调 */
    HfpHfInBandRingToneChangedCallback inBandRingToneChangedCb;
    /** AG调整HF音量变化回调 */
    HfpHfVolumeChangedCallback volumeChangedCb;
    /** AG通话事件回调 */
    HfpHfClientIndCallCallback 	hfClientIndCallCb;
    /** AG通话建立的状态回调 */
    HfpHfClientIndCallSetupCallback hfClientIndCallSetupCb;
	/** AG来电铃声事件回调 */
    HfpHfClientRingIndCallback hfClientRingIndCb;
    /** 采样率上报给应用层回调 */
    HfpHfCodecTypeNotifyCallback codecNotifyCb;
} BtHfpHfCallbacks;

/**
 * @brief 建立与AG的SLC(service level connection)
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfConnect(const BdAddr *bdAddr);

/**
 * @brief 释放与AG的SLC(service level connection)
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfDisconnect(const BdAddr *bdAddr);

/**
 * @brief 建立与AG的音频连接(audio connection)
 *
 * HF端对应发送"AT+BCC"指令
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfConnectSco(const BdAddr *bdAddr);

/**
 * @brief 断开与AG的音频连接(audio connection)
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfDisconnectSco(const BdAddr *bdAddr);

/**
 * @brief 获取设备Profile连接状态。
 *
 * @param bdAddr 设备地址
 * @return 返回{@link BtProfileConnectState}
 * @since 3
 */
int HfpHfGetDeviceConnectState(const BdAddr *bdAddr);

/**
 * @brief 获取SCO连接状态。
 *
 * @param bdAddr 设备地址
 * @return 返回{@link BtHfpScoConnectState}
 * @since 3
 */
int HfpHfGetScoConnectState(const BdAddr *bdAddr);

/**
 * @brief 指示AG发送DTMF tone code.
 * 
 * During an ongoing call, the HF transmits the AT+VTS command to instruct the AG to\n
 * transmit a specific DTMF code to its network connection.\n
 * DTMF(Dual Tone Multi Frequency)\n
 * 发送"AT+VTS=<DTMF>"指令,<DTMF> A single ASCII character in the set 0-9, #,*,A-D.
 *
 * @param bdAddr 设备地址
 * @param code DTMF tone code. 就是<DTMF>对应的字符
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfSendDTMFTone(const BdAddr *bdAddr, unsigned char code);

/**
 * @brief 指示AG打开voice recognition功能。
 *
 * 发送"AT+BVRA=1"指令。
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfOpenVoiceRecognition(const BdAddr *bdAddr);

/**
 * @brief 指示AG关闭voice recognition功能。
 *
 * 发送"AT+BVRA=0"指令。
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfCloseVoiceRecognition(const BdAddr *bdAddr);

/**
 * @brief 接听呼入电话。
 *
 * 本接口涉及的指令可能为"ATA"，接通呼入的电话。\n
 * 或者为"AT+CHLD=<n>",其中<n>的取值如下为0, 1, 1<idx>, 2, 2<idx>, 3或4.\n
 * 0 = Releases all held calls or sets User Determined User Busy (UDUB) for a waiting call.\n
 * 1 = Releases all active calls (if any exist) and accepts the other (held or waiting) call.\n
 * 1<idx> = Releases call with specified index (<idx>).\n
 * 2 = Places all active calls (if any exist) on hold and accepts the other (held or waiting) call.\n
 * 2<idx> = Request private consultation mode with specified call (<idx>).\n
 * (Place all calls on hold EXCEPT the call indicated by <idx>.)\n
 * 3 = Adds a held call to the conversation.\n
 * 4 = Connects the two calls and disconnects the subscriber from both calls (Explicit Call Transfer).\n
 * Support for this value and its associated functionality is optional for the HF.
 *
 * @param bdAddr 设备地址
 * @param actionType 操作类型，参考{@link HfpHfAcceptCallActionType}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfAcceptIncomingCall(const BdAddr *bdAddr, HfpHfAcceptCallActionType actionType);

/**
 * @brief Hold当前通话状态是active状态的通话。
 *
 * 发送"AT+CHLD=2"指令,参考{@link HfpHfAcceptIncomingCall}接口中的描述，。
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfHoldActiveCall(const BdAddr *bdAddr);

/**
 * @brief Hold当前的通话状态是active状态的通话。
 *
 * 可能发送"AT+CHUP"或"AT+CHLD=0"指令,参考{@link HfpHfAcceptIncomingCall}接口中的描述。\n
 * 例如：如果存在incoming call则发送"AT+CHUP"，否则若存在Held或Waiting状态的call则发送"AT+CHLD=0"。
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfRejectIncomingCall(const BdAddr *bdAddr);

/**
 * @brief 结束通话。
 *
 * 可能发送"AT+CHUP"或"AT+CHLD=0"指令,参考{@link HfpHfAcceptIncomingCall}接口中的描述。\n
 * 例如：如果存在Active或Dialing或Alerting状态的call则发送"AT+CHUP"，否则若存在Held状态的call则发送"AT+CHLD=0"。
 *
 * @param bdAddr 设备地址
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfFinishCall(const BdAddr *bdAddr);

/**
 * @brief 拨号发起主叫。
 *
 * 发送"ATDdd…dd;"指令,例如拨号123456,则指令为"ATD123456;"
 *
 * @param bdAddr 设备地址
 * @param number 电话号码
 * @param len 电话号码长度
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfStartDial(const BdAddr *bdAddr, unsigned char *number, unsigned char len);

/**
 * @brief HF端发起音量调整。
 *
 * 调整HF端microphone增益，调整完后通过"AT+VGM=<gain>"指令通知AG。\n
 * 调整HF端speaker增益，调整完后通过"AT+VGS=<gain>"指令通知AG。\n
 * 注意：调整完后仅仅通知AG，不会改变AG端的microphone/speaker增益。
 *
 * @param bdAddr 设备地址
 * @param type 调整microphone/speaker，参考{@link BtHfpVolumeType}
 * @param volume 音量增益，范围0-15
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfSetVolume(const BdAddr *bdAddr, BtHfpVolumeType type, unsigned char volume);

/**
 * @brief HF send user defined AT cmd, like 'AT+cmd'.
 * @param cmd user defined AT cmd.
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfSendVendorATCmd(const char *cmd);

/**
 * @brief 注册HFP HF回调函数。
 *
 * @param func 回调函数原型，参考{@link BtHfpHfCallbacks}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int HfpHfRegisterCallbacks(BtHfpHfCallbacks *func);

#endif
/** @} */

