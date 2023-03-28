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
 * @file ohos_bt_gatt_server.h
 *
 * @brief Gatt server相关的功能接口定义。
 *
 * 定义Gatt server用到的基本数据结构及对外接口。\n
 *
 * @since 3
 */

#ifndef OHOS_BT_GATT_SERVER_H
#define OHOS_BT_GATT_SERVER_H

#include "ohos_bt_def.h"

/**
 * @brief 定义安全加密类型
 *
 * @since 3
 */
typedef enum {
    /** 无加密 */
    OHOS_BLE_SEC_NONE = 0x00,
    /** 加密 */
    OHOS_BLE_SEC_ENCRYPT,
    /** 无MITM的加密 */
    OHOS_BLE_SEC_ENCRYPT_NO_MITM,
    /** 有MITM的加密 */
    OHOS_BLE_SEC_ENCRYPT_MITM
} BleSecAct;

/**
 * @brief 响应远端client读或写的请求时携带的参数
 *
 * @since 3
 */
typedef struct {
    /** 连接索引 */
    int connectId;
    /** 读/写的结果状态，{@link GattStatus} */
    int status;
    /** attribute的handle号 */
    int attrHandle;
    /** 响应的数据长度 */
    int valueLen;
    /** 响应的数据 */
    char *value;
} GattsSendRspParam;

/**
 * @brief 当本端characteristic发生变化，给远端client发送indication/notification时携带的参数
 *
 * @since 3
 */
typedef struct {
    /** 连接索引 */
    int connectId;
    /** attribute的handle号 */
    int attrHandle;
    /** 1表示发送indication且client需要回复确认，0表示发送notification */
    int confirm;
    /** 发送的数据长度 */
    int valueLen;
    /** 发送的数据 */
    char *value;
} GattsSendIndParam;

/**
 * @brief 收到client请求读时回调给上层的接口参数
 *
 * @since 3
 */
typedef struct {
    /** 连接索引 */
    int connId;
    /** transport Id，{@link BtTransportId} */
    int transId;
    /** 设备地址 */
    BdAddr *bdAddr;
    /** 请求读的attribute的handle号 */
    int attrHandle;
    /** 从第一个字节偏移offset个字节开始读，从头读则设置为0 */
    int offset;
    /** 如果设置为true则这个请求读是Long Read方式 */
    bool isLong;
} BtReqReadCbPara;

/**
 * @brief 收到client请求写时回调给上层的接口参数
 *
 * @since 3
 */
typedef struct {
    /** 连接索引 */
    int connId;
    /** transport Id，{@link BtTransportId} */
    int transId;
    /** 设备地址 */
    BdAddr *bdAddr;
    /** 请求写的attribute的handle号 */
    int attrHandle;
    /** 从第一个字节偏移offset个字节开始写，从头写则设置为0 */
    int offset;
    /** 写的数据长度 */
    int length;
    /** 设置为true表示需要给远端client回复响应，设置为false则不需要回复响应 */
    bool needRsp;
    /** 设置为true表示Prepare write，设置为false表示立即写 */
    bool isPrep;
    /** 写的数据 */
    unsigned char *value;
} BtReqWriteCbPara;

/**
 * @brief 注册server后的回调，{@link BleGattsRegister}
 *
 * @since 3
 */
typedef void (*RegisterServerCallback)(int status, int serverId, BtUuid *appUuid);

/**
 * @brief 和远端client连接上时回调给应用
 *
 * @since 3
 */
typedef void (*ConnectServerCallback)(int connId, int serverId, const BdAddr *bdAddr);

/**
 * @brief 和远端client断连时回调给应用
 *
 * @since 3
 */
typedef void (*DisconnectServerCallback)(int connId, int serverId, const BdAddr *bdAddr);

/**
 * @brief 添加service后回调结果给应用，{@link BleGattsAddService}
 *
 * @since 3
 */
typedef void (*ServiceAddCallback)(int status, int serverId, BtUuid *uuid, int srvcHandle);

/**
 * @brief 给service添加Included service后回调结果给应用，{@link BleGattsAddIncludedService}
 *
 * @since 3
 */
typedef void (*IncludeServiceAddCallback)(int status, int serverId, int srvcHandle, int includeSrvcHandle);

/**
 * @brief 给service添加characteristic后回调结果给应用，{@link BleGattsAddCharacteristic}
 *
 * @since 3
 */
typedef void (*CharacteristicAddCallback)(int status, int serverId, BtUuid *uuid,
                                          int srvcHandle, int characteristicHandle);

/**
 * @brief 给characteristic添加descriptor后回调结果给应用，{@link BleGattsAddDescriptor}
 *
 * @since 3
 */
typedef void (*DescriptorAddCallback)(int status, int serverId, BtUuid *uuid,
                                      int srvcHandle, int descriptorHandle);

/**
 * @brief 启动service后回调结果给应用，{@link BleGattsStartService}
 *
 * @since 3
 */
typedef void (*ServiceStartCallback)(int status, int serverId, int srvcHandle);

/**
 * @brief 停止service后回调结果给应用，{@link BleGattsStopService}
 *
 * @since 3
 */
typedef void (*ServiceStopCallback)(int status, int serverId, int srvcHandle);

/**
 * @brief 删除一个service后回调结果给应用，{@link BleGattsDeleteService}
 *
 * @since 3
 */
typedef void (*ServiceDeleteCallback)(int status, int serverId, int srvcHandle);

/**
 * @brief 收到远端client发送的读请求时回调给应用
 *
 * 应用需要调用{@link BleGattsSendResponse}给client发送响应回复结果。\n
 *
 * @since 3
 */
typedef void (*RequestReadCallback)(BtReqReadCbPara readCbPara);

/**
 * @brief 收到远端client发送的写请求时回调给应用
 *
 * 应用根据needRsp决定是否要给远端client回复响应，{@link BtReqWriteCbPara}。\n
 * 如果需要则调用给client发送响应回复结果，{@link BleGattsSendResponse}。\n
 *
 * @since 3
 */
typedef void (*RequestWriteCallback)(BtReqWriteCbPara writeCbPara);

/**
 * @brief 发送响应给远端client后回调结果给应用，{@link BleGattsSendResponse}
 *
 *
 * @since 3
 */
typedef void (*ResponseConfirmationCallback)(int status, int handle);

/**
 * @brief 发送indication/notification给远端client后回调结果给应用，{@link BleGattsSendIndication}
 *
 * 发送indication后需要client回复confirmation，收到confirmation后回调给应用。\n
 *
 * 发送notification后不需要client回复。\n
 *
 * @since 3
 */
typedef void (*IndicationSentCallback)(int connId, int status);

/**
 * @brief MTU发生变化时回调给应用。
 *
 * @since 3
 */
typedef void (*MtuChangeCallback)(int connId, int mtu);

/**
 * @brief Gatt server回调函数接口定义
 *
 * @since 3
 */
typedef struct {
    /** server注册回调 */
    RegisterServerCallback registerServerCb;
    /** 和远端client连接上回调 */
    ConnectServerCallback connectServerCb;
    /** 和远端client断连回调 */
    DisconnectServerCallback disconnectServerCb;
    /** 添加service后回调 */
    ServiceAddCallback serviceAddCb;
    /** 添加Included service后回调 */
    IncludeServiceAddCallback includeServiceAddCb;
    /** 添加characteristic后回调 */
    CharacteristicAddCallback characteristicAddCb;
    /** 添加descriptor后回调 */
    DescriptorAddCallback descriptorAddCb;
    /** 启动service后回调 */
    ServiceStartCallback serviceStartCb;
    /** 停止service后回调 */
    ServiceStopCallback serviceStopCb;
    /** 删除service后回调 */
    ServiceDeleteCallback serviceDeleteCb;
    /** 收到client请求读回调 */
    RequestReadCallback requestReadCb;
    /** 收到client请求写回调 */
    RequestWriteCallback requestWriteCb;
    /** 发送响应给远端client后回调 */
    ResponseConfirmationCallback responseConfirmationCb;
    /** 发送indication/notification的回调 */
    IndicationSentCallback indicationSentCb;
    /** MTU发生变化时回调 */
    MtuChangeCallback mtuChangeCb;
} BtGattServerCallbacks;

/**
 * @brief client请求读时server的回调
 *
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @since 3
 */
typedef int (*BleGattServiceRead)(unsigned char *buff, unsigned int *len);

/**
 * @brief client请求写时server的回调
 *
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @since 3
 */
typedef int (*BleGattServiceWrite)(unsigned char *buff, unsigned int len);

/**
 * @brief 给service发送indication/notification时的回调
 *
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @since 3
 */
typedef int (*BleGattServiceIndicate)(unsigned char *buff, unsigned int len);

/**
 * @brief gatt server的回调操作函数
 *
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @since 3
 */
typedef struct {
    /** 读操作回调 */
    BleGattServiceRead read;
    /** 写操作回调 */
    BleGattServiceWrite write;
    /** 发送indication/notification的回调 */
    BleGattServiceIndicate indicate;
} BleGattOperateFunc;

/**
 * @brief 构造attribute的数据结构
 *
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @since 3
 */
typedef struct {
    /** attribute的类型 */
    BleAttribType attrType;
    /** 操作权限，参考{@link GattAttributePermission} */
    unsigned int permission;
    /** UUID类型 */
    UuidType uuidType;
    /** UUID */
    unsigned char uuid[OHOS_BLE_UUID_MAX_LEN];
    /** 数据 */
    unsigned char *value;
    /** 数据长度 */
    unsigned char valLen;
    /** Property, 参考{@link GattCharacteristicProperty} */
    unsigned char properties;
    /** 回调操作函数 */
    BleGattOperateFunc func;
} BleGattAttr;

/**
 * @brief 创建启动service的参数
 *
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @since 3
 */
typedef struct {
    /** attribute数量 */
    unsigned int attrNum;
    /** attribute列表 */
    BleGattAttr *attrList;
} BleGattService;

/**
 * @brief Gatt server注册。
 *
 * 根据传入的UUID注册server，回调函数返回server接口ID。\n
 *
 * @param appUuid 要注册的应用UUID，由应用定义。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsRegister(BtUuid appUuid);

/**
 * @brief Gatt server去注册。
 *
 * @param serverId server接口ID。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsUnRegister(int serverId);

/**
 * @brief server发起和远端client断连。
 *
 * @param serverId server接口ID。
 * @param bdAddr 远端设备地址。
 * @param connId 连接ID(server注册时返回)。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsDisconnect(int serverId, BdAddr bdAddr, int connId);

/**
 * @brief 添加service。
 *
 * 采用分开添加的方式，先添加service再添加characteristic，以及添加descriptor。\n
 * Service完成特定功能或特性的数据和相关行为的集合，包含service declaration、一个或多个Include和Characteristic。\n
 *
 * @param serverId server接口ID。
 * @param srvcUuid service的UUID。
 * @param isPrimary 设置为true表示添加的Primary服务，false为Secondary服务。
 * @param number attribute handle的数量。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number);

/**
 * @brief 添加included service到指定的service。
 *
 * Include Service表示将server上存在的另一种服务定义引用到要定义的服务中。\n
 *
 * @param serverId server接口ID。
 * @param srvcHandle service的handle号。
 * @param includedHandle 待添加的included服务的attribute handle号。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsAddIncludedService(int serverId, int srvcHandle, int includedHandle);

/**
 * @brief 添加characteristic到指定的service。
 *
 * Characteristic包括数据，访问数据的方法、数据格式和表示方式等信息。\n
 *
 * @param serverId server接口ID。
 * @param srvcHandle service的handle号。
 * @param characUuid 待添加的characteristic的UUID。
 * @param properties 当前Characteristic支持的访问方法信息，{@link GattCharacteristicProperty}
 * @param permissions 当前Characteristic支持的访问权限，{@link GattAttributePermission}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                              int properties, int permissions);

/**
 * @brief 添加descriptor到对应的characteristic。
 *
 * 包含当前Characteristic的描述信息、配置信息、表示格式信息等。\n
 *
 * @param serverId server接口ID。
 * @param srvcHandle service的handle号。
 * @param descUuid 待添加的descriptor的UUID。
 * @param permissions 当前descriptor支持的访问权限，{@link GattAttributePermission}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions);

/**
 * @brief 启动service。
 *
 * @param serverId server接口ID。
 * @param srvcHandle service的handle号。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsStartService(int serverId, int srvcHandle);

/**
 * @brief 停止service。
 *
 * @param serverId server接口ID。
 * @param srvcHandle service的handle号。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsStopService(int serverId, int srvcHandle);

/**
 * @brief 删除service。
 *
 * 删除指定的service。\n
 *
 * @param serverId server接口ID。
 * @param srvcHandle service的handle号。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsDeleteService(int serverId, int srvcHandle);

/**
 * @brief 清除service。
 *
 * 清除所有的service。\n
 *
 * @param serverId server接口ID。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsClearServices(int serverId);

/**
 * @brief 收到远端client的读/写请求时发送响应。
 *
 * @param serverId server接口ID。
 * @param param 响应参数，参考{@link GattsSendRspParam}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsSendResponse(int serverId, GattsSendRspParam *param);

/**
 * @brief 给远端client发送indication/notification。
 *
 * indication和notification的共用接口，通过入参中confirm字段值进行区分，{@link GattsSendIndParam}。\n
 *
 * @param serverId server接口ID。
 * @param param 发送参数，参考{@link GattsSendIndParam}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsSendIndication(int serverId, GattsSendIndParam *param);

/**
 * @brief 设置安全连接加密类型。
 *
 * @param bdAddr 远端设备地址。
 * @param secAct 加密类型，参考{@link BleSecAct}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsSetEncryption(BdAddr bdAddr, BleSecAct secAct);

/**
 * @brief 注册Gatt server回调函数。
 *
 * @param func 回调函数原型，参考{@link BtGattServerCallbacks}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsRegisterCallbacks(BtGattServerCallbacks *func);

/**
 * @brief 启动服务扩展接口。
 *
 * 批量完成service/characteristic/descriptor的添加，并启动服务。\n
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @param srvcHandle 出参由被接口实现者返回，service的handle号。
 * @param srvcInfo service相关的信息。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsStartServiceEx(int *srvcHandle, BleGattService *srvcInfo);

/**
 * @brief 停止服务扩展接口。
 *
 * 批量完成service/characteristic/descriptor的添加，并启动服务。\n
 * 仅提供给系统应用使用，不对三方应用开放。\n
 *
 * @param srvcHandle service的handle号。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 * @since 3
 */
int BleGattsStopServiceEx(int srvcHandle);
#endif
/** @} */

