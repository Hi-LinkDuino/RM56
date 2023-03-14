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
 * @file ohos_bt_gatt_server.h
 *
 * @brief Declares basic data structures and functions of the GATT server.
 *
 * @since 6
 */

#ifndef OHOS_BT_GATT_SERVER_H
#define OHOS_BT_GATT_SERVER_H

#include "ohos_bt_def.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Enumerates security encryption types.
 *
 * @since 6
 */
typedef enum {
    /** No encryption */
    OHOS_BLE_SEC_NONE = 0x00,
    /** Encryption */
    OHOS_BLE_SEC_ENCRYPT,
    /** Encryption without MITM */
    OHOS_BLE_SEC_ENCRYPT_NO_MITM,
    /** Encryption with MITM */
    OHOS_BLE_SEC_ENCRYPT_MITM
} BleSecAct;

/**
 * @brief Defines the parameters in the response to a read or write request from the client.
 *
 * @since 6
 */
typedef struct {
    /** Connection ID */
    int connectId;
    /** Read/write status, as enumerated in {@link GattStatus} */
    int status;
    /** Attribute handle ID */
    int attrHandle;
    /** Length of the response data */
    int valueLen;
    /** Response data */
    char *value;
} GattsSendRspParam;

/**
 * @brief Defines the parameters in the indication or notification sent to the client when a characteristic change
 * occurs on the GATT server.
 *
 * @since 6
 */
typedef struct {
    /** Connection ID */
    int connectId;
    /** Attribute handle ID */
    int attrHandle;
    /**
     * Whether to send an indication or notification. Value <b>1</b> means to send an indication that requires
     * a response from the client, and <b>0</b> means to send a notification.
     */
    int confirm;
    /** Length of the data to be sent */
    int valueLen;
    /** Data to be sent */
    char *value;
} GattsSendIndParam;

/**
 * @brief Defines the parameters in the callback that is invoked
 * when the GATT server receives a read request from the client.
 *
 * @since 6
 */
typedef struct {
    /** Connection ID */
    int connId;
    /** Transport ID, as enumerated in {@link BtTransportId} */
    int transId;
    /** Device address */
    BdAddr *bdAddr;
    /** Handle ID of the attribute to be read */
    int attrHandle;
    /**
     * Offset to the first byte, from where the data reading starts.
     * If data is to be read from the beginning, the value is <b>0</b>.
     */
    int offset;
    /** Whether the read request is a long read. Value <b>true</b> indicates a long read. */
    bool isLong;
} BtReqReadCbPara;

/**
 * @brief Defines the parameters in the callback that is invoked when the GATT server
 * receives a write request from the client.
 *
 * @since 6
 */
typedef struct {
    /** Connection ID */
    int connId;
    /** Transport ID, as enumerated in {@link BtTransportId} */
    int transId;
    /** Device address */
    BdAddr *bdAddr;
    /** Handle ID of the attribute to be written */
    int attrHandle;
    /**
     * Offset to the first byte, from where the data writing starts.
     * If data is to be written from the beginning, the value is <b>0</b>.
     */
    int offset;
    /** Length of the data to be written */
    int length;
    /**
     * Whether the GATT server needs to send a response to the client.
     * Value <b>true</b> indicates that a response to the client is required,
     * and <b>false</b> indicates the opposite.
     */
    bool needRsp;
    /**
     * Whether a prepare write is required.
     * Value <b>true</b> indicates that it is required, and <b>false</b> indicates the opposite.
     */
    bool isPrep;
    /** Data to be written */
    unsigned char *value;
} BtReqWriteCbPara;

/**
 * @brief Called when a GATT server is registered via {@link BleGattsRegister}.
 *
 * @since 6
 */
typedef void (*RegisterServerCallback)(int status, int serverId, BtUuid *appUuid);

/**
 * @brief Called when the GATT server is connected to the client.
 *
 * @since 6
 */
typedef void (*ConnectServerCallback)(int connId, int serverId, const BdAddr *bdAddr);

/**
 * @brief Called when the GATT server is disconnected from the client.
 *
 * @since 6
 */
typedef void (*DisconnectServerCallback)(int connId, int serverId, const BdAddr *bdAddr);

/**
 * @brief Called when a service is added via {@link BleGattsAddService}.
 *
 * @since 6
 */
typedef void (*ServiceAddCallback)(int status, int serverId, BtUuid *uuid, int srvcHandle);

/**
 * @brief Called when an included service is added to a service via {@link BleGattsAddIncludedService}.
 *
 * @since 6
 */
typedef void (*IncludeServiceAddCallback)(int status, int serverId, int srvcHandle, int includeSrvcHandle);

/**
 * @brief Called when a characteristic is added to a service via {@link BleGattsAddCharacteristic}.
 *
 * @since 6
 */
typedef void (*CharacteristicAddCallback)(int status, int serverId, BtUuid *uuid,
                                          int srvcHandle, int characteristicHandle);

/**
 * @brief Called when a descriptor is added to a characteristic via {@link BleGattsAddDescriptor}.
 *
 * @since 6
 */
typedef void (*DescriptorAddCallback)(int status, int serverId, BtUuid *uuid,
                                      int srvcHandle, int descriptorHandle);

/**
 * @brief Called when a service is started via {@link BleGattsStartService}.
 *
 * @since 6
 */
typedef void (*ServiceStartCallback)(int status, int serverId, int srvcHandle);

/**
 * @brief Called when a service is stopped via {@link BleGattsStopService}.
 *
 * @since 6
 */
typedef void (*ServiceStopCallback)(int status, int serverId, int srvcHandle);

/**
 * @brief Called when a service is deleted via {@link BleGattsDeleteService}.
 *
 * @since 6
 */
typedef void (*ServiceDeleteCallback)(int status, int serverId, int srvcHandle);

/**
 * @brief Called when the GATT server receives a read request from the client.
 *
 * You need to call {@link BleGattsSendResponse} to send a response to the client. \n
 *
 * @since 6
 */
typedef void (*RequestReadCallback)(BtReqReadCbPara readCbPara);

/**
 * @brief Called when the GATT server receives a write request from the client.
 *
 * You can determine whether to send a response to the client based on the <b>needRsp</b> parameter\n
 * in {@link BtReqWriteCbPara}.\n
 * If a response is needed, you can call {@link BleGattsSendResponse} to send a response to the client.
 *
 * @since 6
 */
typedef void (*RequestWriteCallback)(BtReqWriteCbPara writeCbPara);

/**
 * @brief Called when the GATT server sends a response to the client via {@link BleGattsSendResponse}.
 *
 * @since 6
 */
typedef void (*ResponseConfirmationCallback)(int status, int handle);

/**
 * @brief Called when the GATT server sends an indication or notification to the client
 * via {@link BleGattsSendIndication}.
 *
 * Upon receiving an indication, the client returns a confirmation.\n
 * When the GATT server receives the confirmation, this callback is invoked.
 *
 * Upon receiving a notification, the client does not respond.
 *
 * @since 6
 */
typedef void (*IndicationSentCallback)(int connId, int status);

/**
 * @brief Called when the MTU changes.
 *
 * @since 6
 */
typedef void (*MtuChangeCallback)(int connId, int mtu);

/**
 * @brief Defines callbacks for the GATT server.
 *
 * @since 6
 */
typedef struct {
    /** Callback that is invoked when a GATT server is registered */
    RegisterServerCallback registerServerCb;
    /** Callback that is invoked when the GATT server is connected to the client */
    ConnectServerCallback connectServerCb;
    /** Callback that is invoked when the GATT server is disconnected from the client */
    DisconnectServerCallback disconnectServerCb;
    /** Callback that is invoked when a service is added */
    ServiceAddCallback serviceAddCb;
    /** Callback that is invoked when an included service is added */
    IncludeServiceAddCallback includeServiceAddCb;
    /** Callback that is invoked when a characteristic is added */
    CharacteristicAddCallback characteristicAddCb;
    /** Callback that is invoked when a descriptor is added */
    DescriptorAddCallback descriptorAddCb;
    /** Callback that is invoked when a service is started */
    ServiceStartCallback serviceStartCb;
    /** Callback that is invoked when a service is stopped */
    ServiceStopCallback serviceStopCb;
    /** Callback that is invoked when a service is deleted */
    ServiceDeleteCallback serviceDeleteCb;
    /** Callback that is invoked when the GATT server receives a read request from the client */
    RequestReadCallback requestReadCb;
    /** Callback that is invoked when the GATT server receives a write request from the client */
    RequestWriteCallback requestWriteCb;
    /** Callback that is invoked when the GATT server sends a response to the client */
    ResponseConfirmationCallback responseConfirmationCb;
    /** Callback that is invoked when the GATT server sends an indication or notification to the client */
    IndicationSentCallback indicationSentCb;
    /** Callback that is invoked when the MTU changes */
    MtuChangeCallback mtuChangeCb;
} BtGattServerCallbacks;

/**
 * @brief Called when the GATT client requests to read data from the GATT server.
 *
 * This callback is available for system applications only.
 *
 * @since 6
 */
typedef int (*BleGattServiceRead)(unsigned char *buff, unsigned int *len);

/**
 * @brief Called when the GATT client requests to write data to the GATT server.
 *
 * This callback is available for system applications only.
 *
 * @since 6
 */
typedef int (*BleGattServiceWrite)(unsigned char *buff, unsigned int len);

/**
 * @brief Called when an indication or notification is sent to a service.
 *
 * This callback is available for system applications only.
 *
 * @since 6
 */
typedef int (*BleGattServiceIndicate)(unsigned char *buff, unsigned int len);

/**
 * @brief Defines callbacks for the operations performed by the GATT client on the GATT server.
 *
 * This structure is available for system applications only.
 *
 * @since 6
 */
typedef struct {
    /** Callback that is invoked when the GATT client requests to read data from the GATT server */
    BleGattServiceRead read;
    /** Callback that is invoked when the GATT client requests to write data to the GATT server */
    BleGattServiceWrite write;
    /** Callback that is invoked when an indication or notification is sent to a service */
    BleGattServiceIndicate indicate;
} BleGattOperateFunc;

/**
 * @brief Defines a GATT attribute.
 *
 * This structure is available for system applications only.
 *
 * @since 6
 */
typedef struct {
    /** Attribute type */
    BleAttribType attrType;
    /** Operation permission. For details, see {@link GattAttributePermission}. */
    unsigned int permission;
    /** UUID type */
    UuidType uuidType;
    /** UUID */
    unsigned char uuid[OHOS_BLE_UUID_MAX_LEN];
    /** Data */
    unsigned char *value;
    /** Data length */
    unsigned char valLen;
    /** Property. For details, see {@link GattCharacteristicProperty}. */
    unsigned char properties;
    /** Operation callback */
    BleGattOperateFunc func;
} BleGattAttr;

/**
 * @brief Defines a GATT service.
 *
 * This structure is available for system applications only.
 *
 * @since 6
 */
typedef struct {
    /** Number of attributes */
    unsigned int attrNum;
    /** Attribute list */
    BleGattAttr *attrList;
} BleGattService;

/**
 * @brief Registers a GATT server with a specified application UUID.
 *
 * The <b>RegisterServerCallback</b> is invoked to return the GATT server ID.
 *
 * @param appUuid Indicates the UUID of the application for which the GATT server is to be registered.
 * The UUID is defined by the application.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the GATT server is registered;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsRegister(BtUuid appUuid);

/**
 * @brief Unregisters a GATT server with a specified ID.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the GATT server is unregistered;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsUnRegister(int serverId);

/**
 * @brief Disconnects the GATT server from the client.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param bdAddr Indicates the address of the client.
 * @param connId Indicates the connection ID, which is returned during the server registration.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the GATT server is disconnected from the client;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsDisconnect(int serverId, BdAddr bdAddr, int connId);

/**
 * @brief Adds a service.
 *
 * This function adds the service, its characteristics, and descriptors separately in sequence.\n
 * A service is a collection of data and related behavior that enable a specific capability or feature.\n
 * It consists of a service declaration and one or more included services and characteristics.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param srvcUuid Indicates the UUID of the service.
 * @param isPrimary Specifies whether the service is primary or secondary.
 * Value <b>true</b> indicates that the service is primary, and <b>false</b> indicates that the service is secondary.
 * @param number Indicates the number of attribute handles.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the service is added;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number);

/**
 * @brief Adds an included service to a specified service.
 *
 * An included service is referenced to define another service on the GATT server.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param srvcHandle Indicates the handle ID of the service.
 * @param includedHandle Indicates the attribute handle ID of the included service.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the included service is added to the service;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsAddIncludedService(int serverId, int srvcHandle, int includedHandle);

/**
 * @brief Adds a characteristic to a specified service.
 *
 * A characteristic consists of data, the data access method, data format, and how the data manifests itself.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param srvcHandle Indicates the handle ID of the service.
 * @param characUuid Indicates the UUID of the characteristic to add.
 * @param properties Indicates the access methods supported by the characteristic,
 * as enumerated in {@link GattCharacteristicProperty}.
 * @param permissions Indicates the access permissions supported by the characteristic,
 * as enumerated in {@link GattAttributePermission}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the characteristic is added to the service;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                              int properties, int permissions);

/**
 * @brief Adds a descriptor to a specified characteristic.
 *
 * A descriptor contains the description, configuration, and format of a characteristic.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param srvcHandle Indicates the handle ID of the service to which the characteristic belongs.
 * @param descUuid Indicates the UUID of the descriptor to add.
 * @param permissions Indicates the access permissions supported by the descriptor,
 * as enumerated in {@link GattAttributePermission}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the descriptor is added to the characteristic;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions);

/**
 * @brief Starts a service.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param srvcHandle Indicates the handle ID of the service.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the service is started;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsStartService(int serverId, int srvcHandle);

/**
 * @brief Stops a service.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param srvcHandle Indicates the handle ID of the service.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the service is stopped;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsStopService(int serverId, int srvcHandle);

/**
 * @brief Deletes a service.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param srvcHandle Indicates the handle ID of the service.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the service is deleted;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsDeleteService(int serverId, int srvcHandle);

/**
 * @brief Clears all services.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the services are cleared;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsClearServices(int serverId);

/**
 * @brief Sends a response to the client from which a read or write request has been received.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param param Indicates the pointer to the response parameters. For details, see {@link GattsSendRspParam}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the response is sent;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsSendResponse(int serverId, GattsSendRspParam *param);

/**
 * @brief Sends an indication or notification to the client.
 *
 * The <b>confirm</b> field in <b>param</b> determines whether to send an indication or a notification.
 *
 * @param serverId Indicates the ID of the GATT server.
 * @param param Indicates the pointer to the sending parameters. For details, see {@link GattsSendIndParam}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the indication or notification is sent;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsSendIndication(int serverId, GattsSendIndParam *param);

/**
 * @brief Sets the encryption type for the GATT connection.
 *
 * @param bdAddr Indicates the address of the client.
 * @param secAct Indicates the encryption type, as enumerated in {@link BleSecAct}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the encryption type is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsSetEncryption(BdAddr bdAddr, BleSecAct secAct);

/**
 * @brief Registers GATT server callbacks.
 *
 * @param func Indicates the pointer to the callbacks to register, as enumerated in {@link BtGattServerCallbacks}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the callbacks are registered;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsRegisterCallbacks(BtGattServerCallbacks *func);

/**
 * @brief Adds a service, its characteristics, and descriptors and starts the service.
 *
 * This function is available for system applications only.
 *
 * @param srvcHandle Indicates the pointer to the handle ID of the service,
 * which is returned by whoever implements this function.
 * @param srvcInfo Indicates the pointer to the service information.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsStartServiceEx(int *srvcHandle, BleGattService *srvcInfo);

/**
 * @brief Stops a service.
 *
 * This function is available for system applications only.
 *
 * @param srvcHandle Indicates the handle ID of the service.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattsStopServiceEx(int srvcHandle);

#ifdef __cplusplus
}
#endif
#endif
/** @} */