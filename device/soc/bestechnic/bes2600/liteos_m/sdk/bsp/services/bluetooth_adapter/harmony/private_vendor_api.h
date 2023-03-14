/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: private interfaces
 */

#ifndef PRIVATE_VENDOR_H
#define PRIVATE_VENDOR_H

#include "ohos_bt_def.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char *stream;
    unsigned int length;
} PcmStreamInfo;

/*
 * @param: void
 * @return: void
 * function: 本地播放器开始播放
 */
void StartLocalMediaPlay(void);

/*
 * @param: void
 * @return: void
 * function: 本地播放器停止播放
 */
void StopLocalMediaPlay(void);

/*
 * @param: void
 * @return: void
 * function: 应用层在收到SCO connected事件后，调用此接口打开HFP
 */
int StartHfpAudioPlay(void);

/*
 * @param: void
 * @return: void
 * function: HFP通话结束后，调用此接口关闭HFP
 */
int StopHfpAudioPlay(void);

/*
 * @param: volume - 取值返回0~1.0之间的一个音量值
 * @return: void
 * function: 设置HFP的通话音量
 */
int SetHfpAudioVolume(float volume);

/*
 * @param: volume - 取值返回0~1.0之间的一个音量值
 * @return: void
 * function: 设置蓝牙音乐播放器音量
 */
int SetA2dpMediaVolume(float volume);

/*
 * @param: volume - 取值返回0~1.0之间的一个音量值
 * @return: void
 * function: 设置Le Audio音乐播放器音量
 */
int SetLeAudioMediaVolume(float volume);

/*
 * @param: void
 * @return: void
 * function: 播放器在非播放的状态下通知应用层有码流过来的callback，目的是应用层决策本地播放器是否能播放
 */
typedef void (*MediaStreamNotifyCallback)(void);

/*
 * @param1: 注册函数地址
 * @return: int 0 - succee, not 0 - failure
 * function: 注册播放器有码流通知的callback函数
 */
int RegisterMediaStreamNotifyCallback(MediaStreamNotifyCallback func);

/*
 * @param1: 流地址
 * @param2: 流长度
 * @return: int 0 - succee, not 0 - failure
 * function: 应用层送HFP处理后的pcm流结果给协议栈
 */
int AppSendHfpPcmStream(PcmStreamInfo *streamInfo);

/*
* @param1: 需要加密的raw data
* @param2: 12字节的加密向量
* @param3: raw data + 16字节的最终加密数据
* @return: int 0 - succee, not 0 - failure
* function: 协议栈采用应用层的hks加密算法加密link key
*/
typedef int (*HksEnDecryptLinkKey)(unsigned char *input, const unsigned char* random, unsigned char *output);

/*
* @param1: 应用层加密的函数地址
* @param2: 应用层解密的函数地址
* @return: int 0 - succee, not 0 - failure
* function: 应用层注册加解密函数地址到协议栈
*/
int RegisterEnDecryptCallback(HksEnDecryptLinkKey encryptCb, HksEnDecryptLinkKey decryptCb);

/*
* @param1: 应用层加密的函数地址
* @param2: 应用层解密的函数地址
* @return: int 0 - succee, not 0 - failure
* function: 应用层注册加解密函数地址到协议栈
*/
int RegisterBisEnDecryptCallback(HksEnDecryptLinkKey encryptCb, HksEnDecryptLinkKey decryptCb);

/*
 * @param: void
 * @return: void
 * function: 协议栈上报音乐卡顿事件给应用层
 */
typedef void (*BtReportAudioCatonEventToApp)(void);

/*
 * @param: reportEventCb - 应用层注册的打点数据通知函数
 * @return: void
 * function: 应用层注册打点通知函数到协议栈, 异常事件发生时, 通过此接口上报异常事件到应用层
 */
int RegisterDataReportCallback(BtReportAudioCatonEventToApp reportEventCb);

/*
 * 以下接口是立体声部分的接口定义 begin
 */
typedef enum {
    STEREO_MASTER,
    STEREO_SLAVE,
    STEREO_UNKNOW = 0xFF,
} StereoRole;

typedef struct {
    BdAddr addr;
    StereoRole role;
} StereoInfo;

typedef enum {
    WIFI_TO_A2DP = 0,
    WIFI_TO_BIS,
    A2DP_TO_BIS,
    UNKNOWN_STATUS
} StereoPlayMode;

typedef enum {
    STEREO_START_PLAY = 0,
    STEREO_STOP_PLAY
} StereoPlayStatus;

typedef enum {
    LEFT_CHANNEL = 0,
    RIGHT_CHANNEL,
    STEREO_CHANNEL,
} StereoChannelSet;

typedef enum {
    STEREO_SETUP_SUCCESS = 0x00,
    STEREO_SETUP_FAILED,
    STEREO_EXIT_SUCCESS,
    STEREO_EXIT_FAILED,
    STEREO_EXIT_REQUEST,
    STEREO_CONNECTION_TIMEOUT = 0x08,
    STEREO_DISCONNECTION_NORMAL,
    STEREO_UNKNOWN_REASON = 0xff,
} StereoEvent;

/*
 * @param: isEnabled - 进行组建立体声的标志
 * 用户按键触发组建立体声，设置isEnable为true，组建完成或组建超时或解除立体声设isEnable为flase.
 * @return: void
 * function: 组建蓝牙立体声
 */
void SetTwsParingOngingFlag(bool isEnabled);

/*
 * @param: void
 * @return: void
 * function: 组建蓝牙立体声
 */
int SetUpStereo(StereoInfo *data);

/*
 * @param: void
 * @return: void
 * function: 拆除蓝牙立体声
 */
void ExitStereo(void);

/*
 * stereo reconnect
 */
void StereoReconnect(void);

/*
 * @param: status - used for future
 * @return: void
 * function: 立体声通信中断上报给应用层的接口
 */
typedef void (*StereoStateChangedCallback)(StereoEvent status);

/*
 * @param: StereoStateChangedCallback - 立体声通信中断的处理函数
 * @return: void
 * function: 注册立体声通信中断的函数到bt协议栈
 */
int RegisterStereoNotifyCallback(StereoStateChangedCallback func);

/*
 * @param1: buf - 发送数据的首地址
 * @param2: length - 发送数据的长度
 * @return: int 成功(0)/失败(非0)
 * function: 蓝牙立体声主从音箱发送数据接口
 */
int StereoSendData(unsigned char *buf, unsigned int length);

/*
 * @param1: buf - 接收数据的缓冲区
 * @param2: length - 接收数据的缓冲区大小
 * @return: int 成功(0)/失败(非0)
 * function: 蓝牙立体声主从音箱发送数据接口
 */
typedef int (*StereoReceiveData)(unsigned char *buf, unsigned int length);

/*
 * @param: StereoReceiveData - 发送数据的首地址
 * @return: int 成功(0)/失败(非0)
 * function: 注册蓝牙立体声主从音箱接收数据的处理函数到bt协议栈
 */
int RegisterStereoDateNotifyCallback(StereoReceiveData func);

/*
 * @param: playMode - 当前播放状态, see details {@link StereoPlayMode}
 *         playStatus - 播放状态, see details {@link StereoPlayStatus}
 *                      STEREO_START_PLAY - source端通过BIS转发音频数据
 *                      STEREO_STOP_PLAY - source端退出BIS模式并停止转发音频数据
 * @return: void
 * function: App通知协议栈当前处于哪种播放状态，有以下几种情况:
 *          1、WIFI_TO_A2DP
 *          2、WIFI_TO_BIS
 *          3、A2DP_TO_BIS
 *          4、UNKNOWN_STATUS
 */
void AppNotifyStereoPlayStatus(StereoPlayMode playMode, StereoPlayStatus playStatus);

/*
 * @param: playMode - 当前播放状态, see details {@link StereoPlayMode}
 *         playStatus - 播放状态, see details {@link StereoPlayStatus}
 *                      STEREO_START_PLAY - sink端开始接收数据并播放
 *                      STEREO_STOP_PLAY - sink端退出BIS模式并停止播放
 * @return: void
 * function: App通知Sink端接收source端BIS数据
 */
void AppNotifyStereoSinkPlayStatus(StereoPlayStatus playStatus);

/*
 * @param: addr - 当前连接音箱的手机地址
 *         role - 应用层调用该接口进行主从切换时, 该台音箱当前的角色(master/slave)
 * @return: int 成功(0)/失败(非0)
 * function: 立体声组网过程中, 应用层调用此接口进行主从切换
 */
int StereoMasterSlaveSwitch(BdAddr *addr, StereoRole role);

/*
 * @param: addr - 设置主音箱的声道
 * @return: int 成功(0)/失败(非0)
 * function: 立体声组网成功后, 设置主音箱的左右声道
 */
int StereoChannelSwitch(StereoChannelSet channel);

/*
 * @param: void
 * @return: BdAddr - 获取蓝牙的地址
 * function: 1、单箱模式, 获取的是factory的mac地址
 *           2、TWS模式, 获取的是master的mac地址
 */
BdAddr *GetExternalDisplayAddress(void);

/*
 * 以上接口是立体声部分的接口定义 end
 */

/* LE Audio Api begin */

typedef enum {
    LE_AUDIO_START,
    LE_AUDIO_STOP
} LeAudioCmdType;

typedef enum {
    LE_AUDIO_MASTER = 0,
    LE_AUDIO_SLAVE,
    LE_AUDIO_UNKNOWN,
} LeAudioRole;

typedef enum {
    LE_AUDIO_SINK_SYNC_SUCCESS = 0,
    LE_AUDIO_SINK_SYNC_FAILED,
    LE_AUDIO_SINK_DISCONNECT_SUCCESS,
    LE_AUDIO_SINK_DISCONNECT_FAILED,
    LE_AUDIO_SINK_LOST,
    LE_AUDIO_SINK_STATUS_UNKNOWN,
} LeAudioSinkStatus;

/*
 * @param: LeAudioCmdType - see details {@link LeAudioCmdType}
 *         status - 执行结果, 0(success) 非0(error)
 * @return: void
 * function: 主或从开始打开或关闭LE Audio功能
 */
typedef void (*ExecLeAudioCmdStatusResult)(LeAudioCmdType cmdType, unsigned char status);

/*
 * @param: status - 同步结果, 0(success) 非0(error)
 * @return: void
 * function: SINK是否同步到SOURCE
 */
typedef void (*ExecLeAudioSyncResult)(unsigned char status);

/*
 * @param: cmdResultCb 执行打开或关闭LE Audio的结果通知函数
 * @return: void
 * function: 应用层注册执行打开/关闭LE Audio的结果通知函数给协议栈
 */
void RegisterLeAudioCmdCallback(ExecLeAudioCmdStatusResult cmdResultCb);

/*
 * @param: syncResultCb SINK是否同步到SOURCE的结果通知函数
 * @return: void
 * function: 应用层注册SINK同步到SOURCE的结果通知函数给协议栈
 */
void RegisterLeAudioSyncCallback(ExecLeAudioSyncResult syncResultCb);

/*
 * @param: result - 设置结果, 0(success) 非0(error)
 * @return: void
 * function: 设置多台串联主从角色
 */
typedef void (*LeAudioSetRoleCallback)(int result);

/*
 * @param: setRoleCb - 通知设置结果的callback
 * @return: void
 * function: 注册多台串联主从角色设置的callback给协议栈
 */
void RegiterLeAduioMusicShareCallback(LeAudioSetRoleCallback setRoleCb);

/*
 * @param: role - see detail {@link LeAudioRole}
 * @return: void
 * function: 设置多台串联的主从角色
 */
void SetLeAudioRole(LeAudioRole role);

/*
 * @param: void
 * @return: LeAudioRole
 * function: 获取多台串联的当前音箱的角色(master/slave)
 */
LeAudioRole GetLeAudioRole(void);

/*
 * @param: void
 * @return: void
 * function: 退出多台串联功能, 角色设置等信息清空
 */
void ExitLeAudioMusicShareMode(void);

/* LE Audio Api end */

/* hiecho api begin */
typedef void (*BtHiechoCustomifEchoReqHandleInd)(uint8_t echoType, uint8_t* reqData,
    uint16_t reqDataLen, uint8_t* args);

typedef void (*BtHiechoCustomifEchoRspHandleInd)(uint8_t echoType, uint8_t* rspData,
    uint16_t rspDataLen, uint8_t* args);

void AppBtSendHiechoReqData(uint8_t* device_addr, uint8_t *data, uint16_t data_len);

/* hiecho comb req msg */
bool AppHiechoCombReqMsg(uint8_t *addr, uint8_t *source, uint16_t datalen);

/* hiecho comb rsp msg */
bool AppHiechoCombRspMsg(uint8_t *args, uint8_t *rspData, uint16_t dataLen);

/* regitster reqEvent and rspEvent callback to bt stack */
void AppRegisterHiechoEventInit(BtHiechoCustomifEchoReqHandleInd reqEventCallback,
    BtHiechoCustomifEchoRspHandleInd rspEventCallback);

/* hiecho api end */

typedef enum {
    GAP_DEV_MODE_SINGLE = 0,
    GAP_DEV_MODE_STEREO,
    GAP_DEV_MODE_UNKNOW = 0xFF,
} GAP_DEVICE_MODE_T;


typedef struct
{
    bool                            ux_bluetooth_enable;
    bool                            ux_dev_poweroff_status;
    GAP_DEVICE_MODE_T               ux_dev_mode;
    bool                            ux_doing_tws_paring_flag;
    bool                            ux_mobile_connected_before_tws_paring;
    StereoStateChangedCallback      ux_stereo_state_change_cb;
    /*workaround solution: UX device mode doesnot match the IBRT stack device mode.eg:
    * UX marks the device mode as stereo mode or single mode with tws link connection status,while the IBRT stack marks
    * the device as sterei mode or single mode with freeman event status.
    */
    bool                            ux_delay_setupstereo;
    uint8_t                         ux_dev_role;
    uint8_t                         ux_peer_addr[6];
} VENDOR_BT_GAP_IF_INFO_T;

VENDOR_BT_GAP_IF_INFO_T* GapBtInterfaceEnvCtxGet();

void GapBtInterfaceEnvInit(void);

/**
 * @brief 上报立体声组建的状态
 * @param 参考StereoEvent 枚举体
 * @return NULL
 */
void GapUxStereoStateChangeReport(StereoEvent status);

/**
 * @brief 设备正常关机前进行的蓝牙预处理操作(主从切换，断tws link和mobile link，关闭音频等)
 * @param NULL
 * @return NULL
 */
void DevicePowerOffPreHandler();

/**
 * @brief 立体声回连
 * @param NULL
 * @return NULL
 */
void StereoReconnect();

/**
 * @brief 游戏模式或正常模式的延时设置，系统启动后默认为正常模式
 * @param true-游戏模式时延模式; false-正常模式时延模式
 * @return NULL
 */
void AppGamingModeAudioLatencyConfigure(bool isGamingEnable);

typedef bool (*UXAcceptTwsConnResultCallback)(void);
void RegisterTwsAcceptConnectCallback(bool (*ArbitrationFunc)(void));
bool GetUxIsEnableAcceptTwsConnectReq(void);

#ifdef __cplusplus
}
#endif

#endif
