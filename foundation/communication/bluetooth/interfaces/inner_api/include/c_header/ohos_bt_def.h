/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Provides basic Bluetooth capabilities.
 *
 * This module allows you to enable and disable Bluetooth, and access basic Bluetooth capabilities.\n
 * Bluetooth uses profiles such as BT-GAP, BLE, BLE-GATT, BT-data transmission, HFP, A2DP, AVRCP, MAP, and PBAP.
 *
 * @since 6
 */

/**
 * @file ohos_bt_def.h
 *
 * @brief Declares basic data structures, macros, enumerations, and structures for Bluetooth services.
 *
 * @since 6
 */

#ifndef OHOS_BT_DEF_H
#define OHOS_BT_DEF_H

/**
 * @brief Defines the address length of a Bluetooth device.
 *
 */
#define OHOS_BD_ADDR_LEN 6

/**
 * @brief Defines the maximum length of a Bluetooth UUID, in bytes.
 *
 */
#define OHOS_BLE_UUID_MAX_LEN 16

/**
 * @brief Enumerates characteristic properties.
 *
 * Characteristic properties determine how characteristic values are used and\n
 * how characteristic descriptors are accessed. If there are multiple properties,\n
 * their values can be connected using the logical operator OR.\n
 * For example, <b>0x01 | 0x02</b> indicates that the characteristic value can be broadcast and read.
 *
 * @since 6
 */
typedef enum {
    /** The characteristic value can be broadcast. */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_BROADCAST = 0x01,
    /** The characteristic value can be read. */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_READ = 0x02,
    /** The characteristic value can be written, and no response needs to be sent to the client. */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP = 0x04,
    /** The characteristic value can be written, and a response needs to be sent to the client. */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_WRITE = 0x08,
    /**
     * The characteristic value can be sent to the client through a notification, and the client does not need to
     * reply with a confirmation message.
     */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_NOTIFY = 0x10,
    /**
     * The characteristic value can be sent to the client through an indication, and the client does not need to
     * reply with a confirmation message.
     */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_INDICATE = 0x20,
    /** The characteristic value can be written with a signature. */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_SIGNED_WRITE = 0x40,
    /** The characteristic has extended properties. */
    OHOS_GATT_CHARACTER_PROPERTY_BIT_EXTENDED_PROPERTY = 0x80
} GattCharacteristicProperty;

/**
 * @brief Enumerates permissions for an attribute.
 *
 * If there are multiple permissions, their values can be connected using the logical operator OR.\n
 * For example, <b>0x01 | 0x02</b> indicates the reading and encrypted reading permissions.
 *
 * @since 6
 */
typedef enum {
    /** Reading */
    OHOS_GATT_PERMISSION_READ = 0x01,
    /** Encrypted reading */
    OHOS_GATT_PERMISSION_READ_ENCRYPTED = 0x02,
    /** Encrypted reading with man-in-the-middle (MITM) protection */
    OHOS_GATT_PERMISSION_READ_ENCRYPTED_MITM = 0x04,
    /** Writing */
    OHOS_GATT_PERMISSION_WRITE = 0x10,
    /** Encrypted writing */
    OHOS_GATT_PERMISSION_WRITE_ENCRYPTED = 0x20,
    /** Encrypted writing with MITM protection */
    OHOS_GATT_PERMISSION_WRITE_ENCRYPTED_MITM = 0x40,
    /** Signed writing */
    OHOS_GATT_PERMISSION_WRITE_SIGNED = 0x80,
    /** Signed writing with MITM protection */
    OHOS_GATT_PERMISSION_WRITE_SIGNED_MITM = 0x100
} GattAttributePermission;

/**
 * @brief Enumerates transport IDs.
 *
 * @since 6
 */
typedef enum {
    /** Invalid transport ID */
    OHOS_BT_TRANSPORT_INVALID = 0x00,
    /** BR/EDR */
    OHOS_BT_TRANSPORT_BR_EDR = 0x01,
    /** LE */
    OHOS_BT_TRANSPORT_LE = 0x02
} BtTransportId;

/**
 * @brief Enumerates Bluetooth statuses.
 *
 * @since 6
 */
typedef enum {
    /** Success */
    OHOS_BT_STATUS_SUCCESS = 0x00,
    /** Failure */
    OHOS_BT_STATUS_FAIL,
    /** Bluetooth not ready */
    OHOS_BT_STATUS_NOT_READY,
    /** Insufficient memory */
    OHOS_BT_STATUS_NOMEM,
    /** System busy */
    OHOS_BT_STATUS_BUSY,
    /** Operation completed */
    OHOS_BT_STATUS_DONE,
    /** Bluetooth not supported by the current version or device */
    OHOS_BT_STATUS_UNSUPPORTED,
    /** Invalid parameters */
    OHOS_BT_STATUS_PARM_INVALID,
    /** Request unhandled */
    OHOS_BT_STATUS_UNHANDLED,
    /** Authentication failure */
    OHOS_BT_STATUS_AUTH_FAILURE,
    /** Remote device shut down */
    OHOS_BT_STATUS_RMT_DEV_DOWN,
    /** Authentication rejected */
    OHOS_BT_STATUS_AUTH_REJECTED
} BtStatus;

/**
 * @brief Enumerates result codes for GATT attribute operations.
 *
 * The error codes are based on Bluetooth Core Specification Version 5.2 | Vol 3, Part F, Table 3.4.
 *
 * @since 6
 */
typedef enum {
    /** Success */
    OHOS_GATT_SUCCESS = 0x00,
    /** Invalid attribute handle */
    OHOS_GATT_INVALID_HANDLE = 0x01,
    /** Attribute unreadable */
    OHOS_GATT_READ_NOT_PERMITTED = 0x02,
    /** Attribute unwritable */
    OHOS_GATT_WRITE_NOT_PERMITTED = 0x03,
    /** Invalid attribute PDU */
    OHOS_GATT_INVALID_PDU = 0x04,
    /** Authentication required for reading or writing the attribute */
    OHOS_GATT_INSUFFICIENT_AUTHENTICATION = 0x05,
    /** Request not supported */
    OHOS_GATT_REQUEST_NOT_SUPPORTED = 0x06,
    /** Invalid offset */
    OHOS_GATT_INVALID_OFFSET = 0x07,
    /** Authorization required for reading or writing the attribute */
    OHOS_GATT_INSUFFICIENT_AUTHORIZATION = 0x08,
    /** The queue is full of prepare writes. */
    OHOS_GATT_PREPARE_QUEUE_FULL = 0x09,
    /** Attribute not found in the specified attribute handle */
    OHOS_GATT_ATTRIBUTE_NOT_FOUND = 0x0A,
    /** The attribute is not a long attribute and cannot use the <b>ATT_READ_BLOB_REQ</b> PDU. */
    OHOS_GATT_ATTRIBUTE_NOT_LONG = 0x0B,
    /** Insufficient size for the encryption key */
    OHOS_GATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE = 0x0C,
    /** Invalid attribute value length */
    OHOS_GATT_INVALID_ATTRIBUTE_VALUE_LENGTH = 0x0D,
    /** Unlikely error */
    OHOS_GATT_UNLIKELY_ERROR = 0x0E,
    /** Encryption required for reading or writing the attribute */
    OHOS_GATT_INSUFFICIENT_ENCRYPTION = 0x0F,
    /** Unsupported grouping attribute */
    OHOS_GATT_UNSUPPORTED_GROUP_TYPE = 0x10,
    /** Insufficient resources */
    OHOS_GATT_INSUFFICIENT_RESOURCES = 0x11,
    /** The server needs to request the client to rediscover the database. */
    OHOS_GATT_DATABASE_OUT_OF_SYNC = 0x12,
    /** Attribute value not allowed */
    OHOS_GATT_VALUE_NOT_ALLOWED = 0x13,
} GattStatus;

/**
 * @brief Enumerates attribute types.
 *
 * @since 6
 */
typedef enum {
    /** Service */
    OHOS_BLE_ATTRIB_TYPE_SERVICE = 0x00,
    /** Characteristic */
    OHOS_BLE_ATTRIB_TYPE_CHAR,
    /** Characteristic value */
    OHOS_BLE_ATTRIB_TYPE_CHAR_VALUE,
    /** Client characteristic configuration */
    OHOS_BLE_ATTRIB_TYPE_CHAR_CLIENT_CONFIG,
    /** Characteristic user description */
    OHOS_BLE_ATTRIB_TYPE_CHAR_USER_DESCR,
} BleAttribType;

/**
 * @brief Enumerates UUID types.
 *
 * @since 6
 */
typedef enum {
    /** Invalid UUID */
    OHOS_UUID_TYPE_NULL = 0x00,
    /** 16-bit UUID */
    OHOS_UUID_TYPE_16_BIT,
    /** 32-bit UUID */
    OHOS_UUID_TYPE_32_BIT,
    /** 128-bit UUID */
    OHOS_UUID_TYPE_128_BIT,
} UuidType;

/**
 * @brief Enumerates types of characteristic and descriptor write operations performed by the GATT client.
 *
 * @since 6
 */
typedef enum {
    /** Write operation without requiring a response from the server */
    OHOS_GATT_WRITE_NO_RSP = 0x01,
    /** Write operation requiring a response from the server */
    OHOS_GATT_WRITE_DEFAULT = 0x02,
    /** Prepare write requiring a response from the server */
    OHOS_GATT_WRITE_PREPARE = 0x03,
    /** Write operation with an authentication signature */
    OHOS_GATT_WRITE_SIGNED = 0x04
} BtGattWriteType;

/**
 * @brief Enumerates profile connection statuses.
 *
 * @since 6
 */
typedef enum {
    /** Connecting */
    OHOS_PROFILE_STATE_CONNECTING = 0x01,
    /** Connected */
    OHOS_PROFILE_STATE_CONNECTED = 0x02,
    /** Disconnecting */
    OHOS_PROFILE_STATE_DISCONNECTING = 0x03,
    /** Disconnected */
    OHOS_PROFILE_STATE_DISCONNECTED = 0x04
} BtProfileConnectState;

/**
 * @brief Enumerates connection strategies.
 *
 * @since 6
 */
typedef enum {
    /** Unknown strategy */
    OHOS_CONNECTION_UNKNOWN = 0x00,
    /** Allowing connections */
    OHOS_CONNECTION_ALLOWED,
    /** Forbidding connections */
    OHOS_CONNECTION_FORBIDDEN
} BtConnectStrategyType;

/**
 * @brief Enumerates A2DP playing states of the device.
 *
 * @since 6
 */
typedef enum {
    /** Not playing */
    OHOS_A2DP_NOT_PLAYING = 0x00,
    /** Playing */
    OHOS_A2DP_IS_PLAYING
} BtA2dpPlayingState;

typedef enum {
    OHOS_STATE_CONNECTING = 0x00,
    OHOS_STATE_CONNECTED,
    OHOS_STATE_DISCONNECTING,
    OHOS_STATE_DISCONNECTED,
} BtConnectState;

/**
 * @brief Defines the Bluetooth address of the device.
 *
 * @since 6
 */
typedef struct {
    /** Bluetooth address */
    unsigned char addr[OHOS_BD_ADDR_LEN];
} BdAddr;

/**
 * @brief Defines the UUID.
 *
 * @since 6
 */
typedef struct {
    /** UUID length */
    unsigned char uuidLen;
    /** UUID field */
    char *uuid;
} BtUuid;
#endif
/** @} */
