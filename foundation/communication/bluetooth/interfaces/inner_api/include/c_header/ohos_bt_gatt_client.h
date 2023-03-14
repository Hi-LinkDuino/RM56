/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_BT_GATT_CLIENT_H
#define OHOS_BT_GATT_CLIENT_H

#include "ohos_bt_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OHOS_BT_TRANSPORT_TYPE_AUTO = 0x0,
    OHOS_BT_TRANSPORT_TYPE_LE,
    OHOS_BT_TRANSPORT_TYPE_CLASSIC,
} BtTransportType;

typedef enum {
    OHOS_BT_GATT_PRIORITY_BALANCED = 0x0,
    OHOS_BT_GATT_PRIORITY_HIGH,
    OHOS_BT_GATT_PRIORITY_LOW_POWER,
} BtGattPriority;

typedef struct {
    BtUuid serviceUuid;
    BtUuid characteristicUuid;
} BtGattCharacteristic;

typedef struct {
    BtGattCharacteristic characteristic;
    BtUuid descriptorUuid;
} BtGattDescriptor;

/* Parameters for GATT read operations */
typedef struct {
    union {
        BtGattCharacteristic characteristic;
        BtGattDescriptor descriptor;
    } attribute;
    unsigned short dataLen;
    unsigned char *data;
} BtGattReadData;

/**
 * @brief Callback invoked when the connection state changed.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param connectionState Indicate the connect state {@link BtConnectState}.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*ConnectionStateChangedCallback)(int clientId, int connectionState, int status);

/**
 * @brief Callback invoked when the connection parameters for a given connection changed.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param interval The connection interval used on this connection. If N is returned, it indicates N*1.25ms.
 * @param latency The slave latency for the connection in number of connection events.
 * @param timeout The supervision timeout for this connection. If N is returned, it indicates N*10ms.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*ConnectParaUpdateCallback)(int clientId, int interval, int latency, int timeout, int status);

/**
 * @brief Invoked in response to BleGattcSearchServices when the GATT service discovery has been completed.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*SearchServiceCompleteCallback)(int clientId, int status);

/**
 * @brief Reports result of a characteristic read operation.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param readData The characteristic value read from the server.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*ReadCharacteristicCallback)(int clientId, BtGattReadData *readData, int status);

/**
 * @brief GATT write characteristic operation callback.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param characteristic The UUID of the characteristic.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*WriteCharacteristicCallback)(int clientId, BtGattCharacteristic *characteristic, int status);

/**
 * @brief Reports result of a descriptor read operation.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param readData The descriptor value read from the server.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*ReadDescriptorCallback)(int clientId, BtGattReadData *readData, int status);

/**
 * @brief GATT write descriptor operation callback.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param descriptor The UUID of the descriptor.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*WriteDescriptorCallback)(int clientId, BtGattDescriptor *descriptor, int status);

/**
 * @brief Callback invoked when the MTU size for a given connection changed.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param mtuSize The size of MTU.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*ConfigureMtuSizeCallback)(int clientId, int mtuSize, int status);

/**
 * @brief Callback invoked in response to GATT notification registered.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*RegisterNotificationCallback)(int clientId, int status);

/**
 * @brief Callback invoked when a remote device sends a notification that a client has registered for.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param notifyData The characteristic value notifed by the server.
 * @param status Indicate the operation result status {@link BtStatus}.
 */
typedef void (*NotificationCallback)(int clientId, BtGattReadData *notifyData, int status);

typedef struct {
    ConnectionStateChangedCallback ConnectionStateCb;
    ConnectParaUpdateCallback connectParaUpdateCb;
    SearchServiceCompleteCallback searchServiceCompleteCb;
    ReadCharacteristicCallback readCharacteristicCb;
    WriteCharacteristicCallback writeCharacteristicCb;
    ReadDescriptorCallback readDescriptorCb;
    WriteDescriptorCallback writeDescriptorCb;
    ConfigureMtuSizeCallback configureMtuSizeCb;
    RegisterNotificationCallback registerNotificationCb;
    NotificationCallback notificationCb;
} BtGattClientCallbacks;

/**
 * @brief Registers a GATT client with a specified application UUID.
 *
 * @param appUuid Indicates the UUID of the application for which the GATT client is to be registered.
 * The UUID is defined by the application.
 * @return Returns the client ID.
 */
int BleGattcRegister(BtUuid appUuid);

/**
 * @brief Unregisters a GATT client with a specified ID.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcUnRegister(int clientId);

/**
 * @brief Set fastest connection of the Gatt connection, add address to the accelerate connection map
 *  before create a new connection.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param fastestConnFlag Indicates whether to enable the fastest connection.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcSetFastestConn(int clientId, bool fastestConnFlag);

/**
 * @brief Create a Gatt connection to a remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param bdAddr Indicates the remote device's address.
 * @param isAutoConnect Indicates whether it is a direct connection(false) or a background connection(true).
 * @param transport Indicates the transport of Gatt client {@link BtTransportType}.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcConnect(int clientId, BtGattClientCallbacks *func, const BdAddr *bdAddr,
    bool isAutoConnect, BtTransportType transport);

/**
 * @brief Set priority of the Gatt connection.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param bdAddr Indicates the remote device's address.
 * @param priority Indicates the priority of Gatt client {@link BtGattPriority}.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcSetPriority(int clientId, const BdAddr *bdAddr, BtGattPriority priority);

/**
 * @brief Disconnect a Gatt connection with a remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @Returns the operation result status {@link BtStatus}.
 */
int BleGattcDisconnect(int clientId);

/**
 * @brief Request a GATT service discovery on a remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcSearchServices(int clientId);

/**
 * @brief Check whether the expected service exists.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param serviceUuid Indicates the UUID of the service.
 * @return Returns true or false.
 */
bool BleGattcGetService(int clientId, BtUuid serviceUuid);

/**
 * @brief Read characteristic value from the remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param characteristic The specified characteristic {@link BtGattCharacteristic} to be read.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcReadCharacteristic(int clientId, BtGattCharacteristic characteristic);

/**
 * @brief Write characteristic value to the remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param characteristic The specified characteristic {@link BtGattCharacteristic} to be read.
 * @param writeType Indicates the characteristic write type.
 * @param value The value to be write.
 * @param len The length of the value.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcWriteCharacteristic(int clientId, BtGattCharacteristic characteristic,
    BtGattWriteType writeType, int len, const char *value);

/**
 * @brief Read descriptor value from the remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param descriptor The specified characteristic {@link BtGattDescriptor} to be read.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcReadDescriptor(int clientId, BtGattDescriptor descriptor);

/**
 * @brief Write descriptor value to the remote device.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param descriptor The specified descriptor {@link BtGattDescriptor} to be read.
 * @param value The value to be write.
 * @param len The length of the value.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcWriteDescriptor(int clientId, BtGattDescriptor descriptor, int len, const char *value);

/**
 * @brief Configure the ATT MTU size.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param mtuSize The size of MTU.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcConfigureMtuSize(int clientId, int mtuSize);

/**
 * @brief Enable or disable notifications for a given characteristic.
 *
 * @param clientId Indicates the ID of the GATT client.
 * @param characteristic The specified characteristic {@link BtGattCharacteristic}.
 * @param enable True or false.
 * @return Returns the operation result status {@link BtStatus}.
 */
int BleGattcRegisterNotification(int clientId, BtGattCharacteristic characteristic, bool enable);

#ifdef __cplusplus
}
#endif
#endif
/** @} */

