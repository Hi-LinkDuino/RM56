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
 * @file ohos_bt_def.h
 *
 * @brief 公共定义头文件。
 *
 * 定义各蓝牙服务用到的基本数据结构，宏定义，枚举定义，以及结构体定义。\n
 *
 * @since 3
 */

#ifndef OHOS_BT_DEF_H
#define OHOS_BT_DEF_H

/**
 * @brief 蓝牙设备的地址长度。
 *
 */
#define OHOS_BD_ADDR_LEN 6

/**
 * @brief 蓝牙UUID的最大长度，单位字节。
 *
 */
#define OHOS_BLE_UUID_MAX_LEN 16

/**
 * @brief Characteristic Properties的定义。
 *
 * Characteristic Properties bit field决定了如何使用Characteristic Value以及如何访问characteristic descriptors。\n
 * 如果有多个Properties，可以把枚举值或起来，例如："0x01 | 0x02"表示Characteristic Value是可广播的也是可读的。\n
 *
 * @since 3
 */
typedef enum {
    /** 允许广播Characteristic Value */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_BROADCAST = 0x01,
    /** 允许读Characteristic Value */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_READ = 0x02,
    /** 允许写Characteristic Value，并且不需要给client回复response */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP = 0x04,
    /** 允许写Characteristic Value，并且需要给client回复response */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_WRITE = 0x08,
    /** 允许把Characteristic Value通知给client，并且不需要client回复确认 */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_NOTIFY = 0x10,
    /** 允许把Characteristic Value指示给client，并且不需要client回复确认 */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_INDICATE = 0x20,
    /** 允许对Characteristic Value进行签名写入 */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_SIGNED_WRITE = 0x40,
    /** 在Characteristic Extended Properties Descriptor中定义了附加的characteristic properties */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_EXTENDED_PROPERTY = 0x80
} GattCharacteristicProperty;

/**
 * @brief Attribute的权限定义
 *
 * 定义了当前Attribute的操作权限。\n
 * 如果有多个权限，可以把枚举值或起来，例如："0x01 | 0x02"表示权限是可读的也是可加密读的。\n
 *
 * @since 3
 */
typedef enum {
    /** 可读 */
    OHOS_GATT_PERMISSION_READ = 0x01,
    /** 可加密读 */
    OHOS_GATT_PERMISSION_READ_ENCRYPTED = 0x02,
    /** 允许MITM保护的加密读 */
    OHOS_GATT_PERMISSION_READ_ENCRYPTED_MITM = 0x04,
    /** 可写 */
    OHOS_GATT_PERMISSION_WRITE = 0x10,
    /** 可加密写 */
    OHOS_GATT_PERMISSION_WRITE_ENCRYPTED = 0x20,
    /** 允许MITM保护的加密写 */
    OHOS_GATT_PERMISSION_WRITE_ENCRYPTED_MITM = 0x40,
    /** 允许签名写 */
    OHOS_GATT_PERMISSION_WRITE_SIGNED = 0x80,
    /** 允许MITM保护的签名写 */
    OHOS_GATT_PERMISSION_WRITE_SIGNED_MITM = 0x100
} GattAttributePermission;

/**
 * @brief transport的定义
 *
 * @since 3
 */
typedef enum {
    /** 无效值 */
    OHOS_BT_TRANSPORT_INVALID = 0x00,
    /** BR/EDR */
    OHOS_BT_TRANSPORT_BR_EDR = 0x01,
    /** LE */
    OHOS_BT_TRANSPORT_LE = 0x02,
    /** BR/EDR & LE */
    OHOS_BT_TRANSPORT_DUAL_MONO = 0x03,
} BtTransportId;

/**
 * @brief BT状态及错误码的定义
 *
 * @since 3
 */
typedef enum {
    /** 成功 */
    OHOS_BT_STATUS_SUCCESS = 0x00,
    /** 失败 */
    OHOS_BT_STATUS_FAIL,
    /** 未就绪 */
    OHOS_BT_STATUS_NOT_READY,
    /** 内存不足 */
    OHOS_BT_STATUS_NOMEM,
    /** 繁忙无法响应 */
    OHOS_BT_STATUS_BUSY,
    /** 完成 */
    OHOS_BT_STATUS_DONE,
    /** 当前版本或设备不支持 */
    OHOS_BT_STATUS_UNSUPPORTED,
    /** 参数错误 */
    OHOS_BT_STATUS_PARM_INVALID,
    /** 未处理的 */
    OHOS_BT_STATUS_UNHANDLED,
    /** 鉴权失败 */
    OHOS_BT_STATUS_AUTH_FAILURE,
    /** 远端设备关闭 */
    OHOS_BT_STATUS_RMT_DEV_DOWN,
    /** 鉴权被拒 */
    OHOS_BT_STATUS_AUTH_REJECTED
} BtStatus;

/**
 * @brief Gatt Attribute操作错误码
 *
 * 根据蓝牙协议，BLUETOOTH CORE SPECIFICATION Version 5.2 | Vol 3, Part F, table 3.4。\n
 *
 * @since 3
 */
typedef enum {
    /** 成功 */
    OHOS_GATT_SUCCESS = 0x00,
    /** attribute的handle是无效的 */
    OHOS_GATT_INVALID_HANDLE = 0x01,
    /** attribute不可读 */
    OHOS_GATT_READ_NOT_PERMITTED = 0x02,
    /** attribute不可写 */
    OHOS_GATT_WRITE_NOT_PERMITTED = 0x03,
    /** attribute PDU是无效的 */
    OHOS_GATT_INVALID_PDU = 0x04,
    /** 需要authentication才能读或写该attribute */
    OHOS_GATT_INSUFFICIENT_AUTHENTICATION = 0x05,
    /** 不支持client发送的请求 */
    OHOS_GATT_REQUEST_NOT_SUPPORTED = 0x06,
    /** 指定的offset超过了attribute的末尾 */
    OHOS_GATT_INVALID_OFFSET = 0x07,
    /** 需要authorization才能读或写该attribute */
    OHOS_GATT_INSUFFICIENT_AUTHORIZATION = 0x08,
    /** 排队的prepare writes太多，队列已满 */
    OHOS_GATT_PREPARE_QUEUE_FULL = 0x09,
    /** 给定的attribute handle范围内没找到目标attribute */
    OHOS_GATT_ATTRIBUTE_NOT_FOUND = 0x0A,
    /** 该attribute不是long attribute, 不能使用ATT_READ_BLOB_REQ PDU */
    OHOS_GATT_ATTRIBUTE_NOT_LONG = 0x0B,
    /** 用于加密此链接的加密密钥大小不足 */
    OHOS_GATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE = 0x0C,
    /** attribute value的长度是无效的 */
    OHOS_GATT_INVALID_ATTRIBUTE_VALUE_LENGTH = 0x0D,
    /** 遇到不可能的错误 */
    OHOS_GATT_UNLIKELY_ERROR = 0x0E,
    /** 需要encryption才能读或写该attribute */
    OHOS_GATT_INSUFFICIENT_ENCRYPTION = 0x0F,
    /** attribute type是不支持的grouping attribute */
    OHOS_GATT_UNSUPPORTED_GROUP_TYPE = 0x10,
    /** 资源不足 */
    OHOS_GATT_INSUFFICIENT_RESOURCES = 0x11,
    /** server需要请求client rediscover the database */
    OHOS_GATT_DATABASE_OUT_OF_SYNC = 0x12,
    /** 不允许的attribute参数值 */
    OHOS_GATT_VALUE_NOT_ALLOWED = 0x13,
} GattStatus;

/**
 * @brief attribute的类型定义
 *
 * @since 3
 */
typedef enum {
    /** Service */
    OHOS_BLE_ATTRIB_TYPE_SERVICE = 0x00,
    /** Characteristic */
    OHOS_BLE_ATTRIB_TYPE_CHAR,
    /** Characteristic Value */
    OHOS_BLE_ATTRIB_TYPE_CHAR_VALUE,
    /** Client Characteristic Configuration */
    OHOS_BLE_ATTRIB_TYPE_CHAR_CLIENT_CONFIG,
    /** Characteristic User Description */
    OHOS_BLE_ATTRIB_TYPE_CHAR_USER_DESCR,
} BleAttribType;

/**
 * @brief UUID的类型定义
 *
 * @since 3
 */
typedef enum {
    /** 无效值 */
    OHOS_UUID_TYPE_NULL = 0x00,
    /** 16bit长度 */
    OHOS_UUID_TYPE_16_BIT,
    /** 32bit长度 */
    OHOS_UUID_TYPE_32_BIT,
    /** 128bit长度 */
    OHOS_UUID_TYPE_128_BIT,
} UuidType;

/**
 * @brief Gatt client写characteristic和descriptor时的写类型
 *
 * @since 3
 */
typedef enum {
    /** 只写，且不需要server回复响应 */
    OHOS_GATT_WRITE_NO_RSP = 0x01,
    /** 写，且需要server回复响应 */
    OHOS_GATT_WRITE_DEFAULT = 0x02,
    /** prepare写，且需要server回复响应 */
    OHOS_GATT_WRITE_PREPARE = 0x03,
    /** write with an authentication signature */
    OHOS_GATT_WRITE_SIGNED = 0x04
} BtGattWriteType;

/**
 * @brief Profile的连接状态
 *
 * @since 3
 */
typedef enum {
    /** 正在连接 */
    OHOS_PROFILE_STATE_CONNECTING = 0x01,
    /** 已连接 */
    OHOS_PROFILE_STATE_CONNECTED = 0x02,
    /** 正在断连 */
    OHOS_PROFILE_STATE_DISCONNECTING = 0x03,
    /** 已断连 */
    OHOS_PROFILE_STATE_DISCONNECTED = 0x04
} BtProfileConnectState;

/**
 * @brief 设置连接策略
 *
 * @since 3
 */
typedef enum {
    /** 未知 */
    OHOS_CONNECTION_UNKNOWN = 0x00,
    /** 允许连接 */
    OHOS_CONNECTION_ALLOWED,
    /** 禁止连接 */
    OHOS_CONNECTION_FORBIDDEN
} BtConnectStrategyType;

/**
 * @brief 定义设备的媒体播放状态
 *
 * @since 3
 */
typedef enum {
    /** 未播放 */
    OHOS_A2DP_NOT_PLAYING = 0x00,
    /** 正在播放 */
    OHOS_A2DP_IS_PLAYING
} BtA2dpPlayingState;

/**
 * @brief sco的连接状态
 *
 * @since 3
 */
typedef enum {
    /** 正在连接 */
    OHOS_SCO_STATE_CONNECTING = 0x01,
    /** 已连接 */
    OHOS_SCO_STATE_CONNECTED = 0x02,
    /** 正在断连 */
    OHOS_SCO_STATE_DISCONNECTING = 0x03,
    /** 已断连 */
    OHOS_SCO_STATE_DISCONNECTED = 0x04
} BtHfpScoConnectState;

/**
 * @brief HFP音量调整类型
 *
 * @since 3
 */
typedef enum {
    /** 调整microphone增益 */
    OHOS_HFP_VOLUME_MIC = 0x00,
    /** 调整speaker增益 */
    OHOS_HFP_VOLUME_SPEAKER
} BtHfpVolumeType;

/**
 * @brief 设备的蓝牙地址
 *
 * @since 3
 */
typedef struct {
    /** 地址 */
    unsigned char addr[OHOS_BD_ADDR_LEN];
} BdAddr;

/**
 * @brief UUID结构
 *
 * @since 3
 */
typedef struct {
    /** UUID长度 */
    unsigned char uuidLen;
    /** UUID字段 */
    char *uuid;
} BtUuid;
#endif
/** @} */
