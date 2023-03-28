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
 * @file ohos_bt_gatt.h
 *
 * @brief Gatt相关的功能接口定义。
 *
 * 定义Gatt用到的基本数据结构及对外接口，如广播/扫描等。\n
 *
 * @since 3
 */


#ifndef OHOS_BT_GATT_H
#define OHOS_BT_GATT_H

#include "ohos_bt_def.h"

/**
 * @brief 广播过滤参数
 *
 * 设置广播方处理扫描方的扫描请求或连接请求时是否通过白名单过滤。\n
 *
 * @since 3
 */
typedef enum {
    /** 处理来自所有设备的扫描和连接请求 */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = 0x00,
    /** 处理来自所有设备的连接请求，仅处理白名单中扫描请求 */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = 0x01,
    /** 处理来自所有设备的扫描请求，仅处理白名单中连接请求 */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = 0x02,
    /** 仅处理白名单中扫描请求和连接请求 */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = 0x03,
} BleAdvFilter;

/**
 * @brief 广播类型
 *
 * 设置广播方发送广播数据的类型。\n
 *
 * @since 3
 */
typedef enum {
    /** 可连接可扫描非定向广播(默认) */
    OHOS_BLE_ADV_IND = 0x00,
    /** 可连接高占空比定向广播 */
    OHOS_BLE_ADV_DIRECT_IND_HIGH = 0x01,
    /** 可扫描非定向广播 */
    OHOS_BLE_ADV_SCAN_IND = 0x02,
    /** 不可连接非定向广播 */
    OHOS_BLE_ADV_NONCONN_IND = 0x03,
    /** 可连接低占空比定向广播 */
    OHOS_BLE_ADV_DIRECT_IND_LOW  = 0x04,
} BleAdvType;

/**
 * @brief IO能力模式
 *
 * @since 3
 */
typedef enum {
    /** DisplayOnly, 只有输出显示的能力 */
    OHOS_BLE_IO_CAP_OUT = 0x00,
    /** DisplayYesNo, 只有输入YES和NO的能力，有显示能力 */
    OHOS_BLE_IO_CAP_IO,
    /** KeyboardOnly, 可以输入0-9，确认键和YES/NO的能力，没有显示的能力 */
    OHOS_BLE_IO_CAP_IN,
    /** NoInputNoOutput, 没有输入和输出能力 */
    OHOS_BLE_IO_CAP_NONE,
    /** KeyboardDisplay, 可以输入0-9，确认键和YES/NO的能力，有显示的能力 */
    OHOS_BLE_IO_CAP_KBDISP
} BleIoCapMode;

/**
 * @brief 鉴权请求模式
 *
 * @since 3
 */
typedef enum {
    /** no-boding */
    OHOS_BLE_AUTH_NO_BOND = 0x00,
    /** boding */
    OHOS_BLE_AUTH_BOND,
    /** 仅MITM */
    OHOS_BLE_AUTH_REQ_MITM,
    /** 仅安全连接 */
    OHOS_BLE_AUTH_REQ_SC_ONLY,
    /** 安全连接和bonding */
    OHOS_BLE_AUTH_REQ_SC_BOND,
    /** 安全连接和MITM */
    OHOS_BLE_AUTH_REQ_SC_MITM,
    /** 安全连接和MITM，以及bonding */
    OHOS_BLE_AUTH_REQ_SC_MITM_BOND
} BleAuthReqMode;

/**
 * @brief BLE扫描类型
 *
 * @since 3
 */
typedef enum {
    /** 被动扫描，不会发送扫描请求 */
    OHOS_BLE_SCAN_TYPE_PASSIVE = 0x00,
    /** 主动扫描，可能会发送扫描请求 */
    OHOS_BLE_SCAN_TYPE_ACTIVE,
} BleScanType;

/**
 * @brief BLE扫描芯片过滤策略
 *
 * @since 3
 */
typedef enum {
    /** 接受所有广播，忽略不是发给该设备的定向广播(默认) */
    OHOS_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL = 0x00,
    /** 只接受白名单里设备的广播，忽略不是发给该设备的定向广播 */
    OHOS_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST,
    /** 接受所有的非定向广播、地址是可解析私有地址的广播方发送的定向广播以及发给该设备的定向广播 */
    OHOS_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL_AND_RPA,
    /** 接受白名单中的所有非定向广播、地址是可解析私有地址的广播方发送的定向广播以及发给该设备的定向广播 */
    OHOS_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST_AND_RPA
} BleScanFilterPolicy;

/**
 * @brief BLE扫描结果广播类型
 *
 * @since 3
 */
typedef enum {
    /** 扩展的不可连接不可扫描非定向广播 */
    OHOS_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE = 0x00,
    /** 扩展的不可连接不可扫描定向广播 */
    OHOS_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED = 0x04,
    /** 扩展的可连接非定向广播 */
    OHOS_BLE_EVT_CONNECTABLE = 0x01,
    /** 扩展的可连接定向广播 */
    OHOS_BLE_EVT_CONNECTABLE_DIRECTED = 0x05,
    /** 扩展的可扫描非定向广播 */
    OHOS_BLE_EVT_SCANNABLE = 0x02,
    /** 扩展的可扫描定向广播 */
    OHOS_BLE_EVT_SCANNABLE_DIRECTED = 0x06,

    /** 传统的不可连接非定向广播 */
    OHOS_BLE_EVT_LEGACY_NON_CONNECTABLE = 0x10,
    /** 传统的可扫描非定向广播 */
    OHOS_BLE_EVT_LEGACY_SCANNABLE = 0x12,
    /** 传统的可连接可扫描非定向广播 */
    OHOS_BLE_EVT_LEGACY_CONNECTABLE = 0x13,
    /** 传统的可连接定向广播 */
    OHOS_BLE_EVT_LEGACY_CONNECTABLE_DIRECTED = 0x15,
    /** 传统的与ADV_SCAN_IND对应的扫描响应 */
    OHOS_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV_SCAN = 0x1A,
    /** 传统的与ADV_IND对应的扫描响应 */
    OHOS_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV = 0x1B
} BleScanResultEvtType;

/**
 * @brief BLE扫描结果数据完整性
 *
 * @since 3
 */
typedef enum {
    /** 完整数据或最后一个片段 */
    OHOS_BLE_DATA_COMPLETE = 0x00,
    /** 不完整的数据，更多数据即将到来 */
    OHOS_BLE_DATA_INCOMPLETE_MORE_TO_COME = 0x01,
    /** 被截断不完整的数据，没有更多的数据来 */
    OHOS_BLE_DATA_INCOMPLETE_TRUNCATED = 0x02,
} BleScanResultDataStatus;

/**
 * @brief BLE扫描结果地址类型
 *
 * @since 3
 */
typedef enum {
    /** 公共设备地址 */
    OHOS_BLE_PUBLIC_DEVICE_ADDRESS = 0x00,
    /** 随机设备地址 */
    OHOS_BLE_RANDOM_DEVICE_ADDRESS = 0x01,
    /** Public Identity Address */
    OHOS_BLE_PUBLIC_IDENTITY_ADDRESS = 0x02,
    /** Random (static) Identity Address */
    OHOS_BLE_RANDOM_STATIC_IDENTITY_ADDRESS = 0x03,
    /** 不可解析随机设备地址 */
    OHOS_BLE_UNRESOLVABLE_RANDOM_DEVICE_ADDRESS = 0xFE,
    /** 无地址(匿名广播) */
    OHOS_BLE_NO_ADDRESS = 0xFF,
} BleScanResultAddrType;

/**
 * @brief BLE扫描结果PHY类型
 *
 * @since 3
 */
typedef enum {
    /** 无广播包 */
    OHOS_BLE_SCAN_PHY_NO_PACKET = 0x00,
    /** 1M PHY */
    OHOS_BLE_SCAN_PHY_1M = 0x01,
    /** 2M PHY */
    OHOS_BLE_SCAN_PHY_2M = 0x02,
    /** Coded PHY */
    OHOS_BLE_SCAN_PHY_CODED = 0x03
} BleScanResultPhyType;

/**
 * @brief 设置BLE广播/扫描响应数据
 *
 * @since 3
 */
typedef struct {
    /** 广播数据长度 */
    unsigned short advLength;
    /** 广播数据 */
    char *advData;
    /** 扫描响应数据长度 */
    unsigned short scanRspLength;
    /** 扫描响应数据 */
    char *scanRspData;
} BleConfigAdvData;

/**
 * @brief 设置BLE广播参数
 *
 * @since 3
 */
typedef struct {
    /** 最小的广播间隔. [N * 0.625ms] */
    int minInterval;
    /** 最大的广播间隔. [N * 0.625ms] */
    int maxInterval;
    /** 广播类型 */
    BleAdvType advType;
    /** 本端地址类型 */
    unsigned char ownAddrType;
    /** 对端地址类型 */
    unsigned char peerAddrType;
    /** 对端地址 */
    BdAddr peerAddr;
    /** 37/38/39三个广播通道选择bitMap，例如：0x01-37通道使用, 0x07-37/38/39三个通道都使用 */
    int channelMap;
    /** 白名单过滤策略 */
    BleAdvFilter advFilterPolicy;
    /** 发送功率，单位dbm */
    int txPower;
    /** 广播持续发送时长，[N * 10 ms] */
    int duration;
} BleAdvParams;

/**
 * @brief 设置BLE扫描参数
 *
 * @since 3
 */
typedef struct {
    /** 扫描间隔，[N * 0.625 ms] */
    unsigned short scanInterval;
    /** 扫描窗长，[N * 0.625 ms] */
    unsigned short scanWindow;
    /** 扫描类型，参考{@link BleScanType} */
    unsigned char scanType;
    /** PHY类型，0x01表示1M phy， 0x01表示2M phy， 0x02表示LE Coded phy */
    unsigned char scanPhy;
    /** 扫描过滤策略，参考{@link BleScanFilterPolicy} */
    unsigned char scanFilterPolicy;
} BleScanParams;

/**
 * @brief 设置广播/扫描响应数据
 *
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @since 3
 */
typedef struct {
    /** 广播数据 */
    unsigned char *advData;
    /** 广播数据长度 */
    unsigned int advDataLen;
    /** 扫描响应数据 */
    unsigned char *rspData;
    /** 扫描响应数据长度 */
    unsigned int rspDataLen;
} StartAdvRawData;

/**
 * @brief 设置扫描结果数据
 *
 * @since 3
 */
typedef struct {
    /** 广播类型，参考{@link BleScanResultEvtType} */
    unsigned char eventType;
    /** 扫描结果数据状态，参考{@link BleScanResultDataStatus} */
    unsigned char dataStatus;
    /** 地址类型，参考{@link BleScanResultAddrType}，0xFE不可选 */
    unsigned char addrType;
    /** 地址 */
    BdAddr addr;
    /** 主广播PHY类型，参考{@link BleScanResultPhyType}，只可取值0x01或0x03 */
    unsigned char primaryPhy;
    /** 辅广播PHY类型，参考{@link BleScanResultPhyType}，可取全部枚举值 */
    unsigned char secondaryPhy;
    /** Value of the Advertising SID subfield in the ADI field of the PDU or, for scan responses,
       in the ADI field of the original scannable advertisement, 0xFF->No ADI field provided */
    unsigned char advSid;
    /** 发送功率，范围: -127 ~ +20dBm, 设置为0x7F表示字段无效 */
    char txPower;
    /** rssi，范围: -127 ~ +20dBm, 设置为0x7F表示字段无效 */
    char rssi;
    /** 周期广播间隔，[N * 1.25 ms]，设置为0x00表示字段无效 */
    unsigned short periodicAdvInterval;
    /** 定向广播地址类型，参考{@link BleScanResultAddrType}，0xFF不可取 */
    unsigned char directAddrType;
    /** 定向广播地址，定向广播才有效 */
    BdAddr directAddr;
    /** 广播数据长度 */
    unsigned char advLen;
    /** 广播数据 */
    unsigned char *advData;
} BtScanResultData;

/**
 * @brief 启动广播后的回调，参考{@link BleStartAdv}
 *
 * @since 3
 */
typedef void (*AdvEnableCallback)(int advId, int status);

/**
 * @brief 停止广播后的回调，参考{@link BleStopAdv}
 *
 * @since 3
 */
typedef void (*AdvDisableCallback)(int advId, int status);

/**
 * @brief 设置广播数据后的回调，参考{@link BleSetAdvData}
 *
 * @since 3
 */
typedef void (*AdvDataCallback)(int advId, int status);

/**
 * @brief 更新广播参数后的回调，参考{@link BleUpdateAdv}
 *
 * @since 3
 */
typedef void (*AdvUpdateCallback)(int advId, int status);

/**
 * @brief 收到安全访问请求，调用{@link BleGattSecurityRsp}授予安全请求访问权限
 *
 * @since 3
 */
typedef void (*SecurityRespondCallback)(const BdAddr *bdAddr);

/**
 * @brief 收到扫描结果回调给上层
 *
 * @since 3
 */
typedef void (*ScanResultCallback)(BtScanResultData *scanResultdata);

/**
 * @brief 扫描参数设置完成后回调
 *
 * @since 3
 */
typedef void (*ScanParameterSetCompletedCallback)(int clientId, int status);

/**
 * @brief Gatt回调函数接口定义
 *
 * @since 3
 */
typedef struct {
    /** 开启广播完成 */
    AdvEnableCallback advEnableCb;
    /** 停止广播完成 */
    AdvDisableCallback advDisableCb;
    /** 设置广播数据完成 */
    AdvDataCallback advDataCb;
    /** 更新广播参数完成 */
    AdvUpdateCallback advUpdateCb;
    /** 收到安全访问请求 */
    SecurityRespondCallback securityRespondCb;
    /** 收到扫描结果 */
    ScanResultCallback scanResultCb;
    /** 扫描参数设置完成 */
    ScanParameterSetCompletedCallback scanParamSetCb;
} BtGattCallbacks;

/**
 * @brief 初始化蓝牙协议栈。
 *
 * @param void
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int InitBtStack(void);

/**
 * @brief 使能蓝牙协议栈。
 *
 * @param void
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int EnableBtStack(void);

/**
 * @brief 去使能蓝牙协议栈。
 *
 * @param void
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int DisableBtStack(void);

/**
 * @brief 设置蓝牙设备名称。
 *
 * @param name 蓝牙名称
 * @param len 蓝牙名称数据长度
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int SetDeviceName(const char *name, unsigned int len);

/**
 * @brief 设置广播数据。
 *
 * @param advId 广播ID，由广播方上层分配
 * @param data 广播数据，参考{@link BleConfigAdvData}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleSetAdvData(int advId, const BleConfigAdvData *data);

/**
 * @brief 开始发送广播。
 *
 * @param advId 广播ID，由广播方上层分配
 * @param param 广播参数，参考{@link BleAdvParams}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleStartAdv(int advId, const BleAdvParams *param);

/**
 * @brief 停止发送广播。
 *
 * @param advId 广播ID，由发送广播时广播方上层分配
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleStopAdv(int advId);

/**
 * @brief 更新广播参数。
 *
 * @param advId 广播ID，由发送广播时广播方上层分配
 * @param param 广播参数，参考{@link BleAdvParams}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleUpdateAdv(int advId, const BleConfigAdvData *data, const BleAdvParams *param);

/**
 * @brief 设置安全IO能力。
 *
 * @param mode 能力模式，参考{@link BleIoCapMode}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleSetSecurityIoCap(BleIoCapMode mode);

/**
 * @brief 设置安全连接鉴权请求模式。
 *
 * @param mode 鉴权模式，参考{@link BleAuthReqMode}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleSetSecurityAuthReq(BleAuthReqMode mode);

/**
 * @brief 响应安全连接请求。
 *
 * @param bdAddr 设备地址
 * @param accept true表示接受请求，false表示拒绝请求
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattSecurityRsp(BdAddr bdAddr, bool accept);

/**
 * @brief 获取设备MAC地址。
 *
 * @param mac 出参，设备MAC地址
 * @param len 地址长度
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int ReadBtMacAddr(unsigned char *mac, unsigned int len);

/**
 * @brief 设置扫描参数。
 *
 * @param clientId client接口ID，client注册时获取
 * @param param 扫描参数，参考{@link BleScanParams}
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleSetScanParameters(int clientId, BleScanParams *param);

/**
 * @brief 启动扫描。
 *
 * @param void
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleStartScan(void);

/**
 * @brief 停止扫描。
 *
 * @param void
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleStopScan(void);

/**
 * @brief 注册Gatt回调函数。
 *
 * @param func 回调函数原型，参考{@link BtGattCallbacks}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattRegisterCallbacks(BtGattCallbacks *func);

/**
 * @brief 启动广播扩展接口。
 *
 * 完成广播数据设置/广播参数设置/启动广播。\n
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @param advId 广播ID
 * @param rawData 广播数据，参考{@link StartAdvRawData}。
 * @param advParam 广播参数，参考{@link BleAdvParams}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleStartAdvEx(int *advId, const StartAdvRawData rawData, BleAdvParams advParam);
#endif
/** @} */
