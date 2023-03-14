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
 * @file att.h
 *
 * @brief Declares extern interface, including register interface, callback interface etc.
 *
 */

#ifndef ATT_H
#define ATT_H

#include <stdint.h>
#include "btstack.h"
#include "buffer.h"
#ifdef __cplusplus
extern "C" {
#endif

// Transport type
#define BT_TRANSPORT_INVALID 0
#define BT_TRANSPORT_BR_EDR 1
#define BT_TRANSPORT_LE 2

// client callback event id
#define ATT_ERROR_RESPONSE_ID 0x0101
#define ATT_EXCHANGE_MTU_RESPONSE_ID 0x0103
#define ATT_FIND_INFORMATION_RESPONSE_ID 0x0105
#define ATT_FIND_BY_TYPE_VALUE_RESPONSE_ID 0x0107
#define ATT_READ_BY_TYPE_RESPONSE_ID 0x0109
#define ATT_READ_RESPONSE_ID 0x010B
#define ATT_READ_BLOB_RESPONSE_ID 0x010D
#define ATT_READ_MULTIPLE_RESPONSE_ID 0x010F
#define ATT_READ_BY_GROUP_TYPE_RESPONSE_ID 0x0202
#define ATT_WRITE_RESPONSE_ID 0x0204
#define ATT_PREPARE_WRITE_RESPONSE_ID 0x0208
#define ATT_EXECUTE_WRITE_RESPONSE_ID 0x020A
#define ATT_HANDLE_VALUE_NOTIFICATION_ID 0x020B
#define ATT_HANDLE_VALUE_INDICATION_ID 0x020C
#define ATT_TRANSACTION_TIME_OUT_ID 0x020E

// server callback event id
#define ATT_EXCHANGE_MTU_REQUEST_ID 0x0102
#define ATT_FIND_INFORMATION_REQUEST_ID 0x0104
#define ATT_FIND_BY_TYPE_VALUE_REQUEST_ID 0x0106
#define ATT_READ_BY_TYPE_REQUEST_ID 0x0108
#define ATT_READ_REQUEST_ID 0x010A
#define ATT_READ_BLOB_REQUEST_ID 0x010C
#define ATT_READ_MULTIPLE_REQUEST_ID 0x010E
#define ATT_READ_BY_GROUP_TYPE_REQUEST_ID 0x0201
#define ATT_WRITE_REQUEST_ID 0x0203
#define ATT_WRITE_COMMAND_ID 0x0205
#define ATT_SIGNED_WRITE_COMMAND_ID 0x0206
#define ATT_PREPARE_WRITE_REQUEST_ID 0x0207
#define ATT_EXECUTE_WRITE_REQUEST_ID 0x0209
#define ATT_HANDLE_VALUE_CONFIRMATION_ID 0x020D
#define ATT_UNKNOW_OPCODE_ID 0x020F

// error response code
#define ATT_INVALID_HANDLE 0x01
#define ATT_READ_NOT_PERMITTED 0x02
#define ATT_WRITE_NOT_PERMITTED 0x03
#define ATT_INVALID_PDU 0x04
#define ATT_INSUFFICIENT_AUTHENTICATION 0x05
#define ATT_REQUEST_NOT_SUPPORTED 0x06
#define ATT_INVALID_OFFSET 0x07
#define ATT_INSUFFICIENT_AUTHORIZATION 0x08
#define ATT_PREPARE_QUEUE_FULL 0x09
#define ATT_ATTRIBUTE_NOT_FOUND 0x0A
#define ATT_ATTRIBUTE_NOT_LONG 0x0B
#define ATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE 0x0C
#define ATT_INVALID_ATTRIBUTE_VALUE_LENGTH 0x0D
#define ATT_UNLIKELY_ERROR 0x0E
#define ATT_INSUFFICIENT_ENCRYPTION 0x0F
#define ATT_UNSUPPORTED_GROUP_TYPE 0x10
#define ATT_INSUFFICIENT_RESOURECES 0x11
#define ATT_WRITE_REQUEST_REJECTED 0xFC
#define ATT_CLIENT_CHARACTERISTIC_CONFIGURATION_DESCRIPTOR_IMPROPERLY_CONFIGURED 0xFD
#define ATT_PROCEDURE_ALREADY_IN_PROGRESS 0xFE
#define ATT_OUT_OF_RANGE 0xFF

// opcode
#define ERROR_RESPONSE 0x01
#define EXCHANGE_MTU_REQUEST 0x02
#define EXCHANGE_MTU_RESPONSE 0x03
#define FIND_INFORMATION_REQUEST 0x04
#define FIND_INFORMATION_RESPONSE 0x05
#define FIND_BY_TYPE_VALUE_REQUEST 0x06
#define FIND_BY_TYPE_VALUE_RESPONSE 0x07
#define READ_BY_TYPE_REQUEST 0x08
#define READ_BY_TYPE_RESPONSE 0x09
#define READ_REQUEST 0x0A
#define READ_RESPONSE 0x0B
#define READ_BLOB_REQUEST 0x0C
#define READ_BLOB_RESPONSE 0x0D
#define READ_MULTIPLE_REQUEST 0x0E
#define READ_MULTIPLE_RESPONSE 0x0F
#define READ_BY_GROUP_TYPE_REQUEST 0x10
#define READ_BY_GROUP_TYPE_RESPONSE 0x11
#define WRITE_REQUEST 0x12
#define WRITE_RESPONSE 0x13
#define WRITE_COMMAND 0x52
#define SIGNED_WRITE_COMMAND 0xD2
#define PREPARE_WRITE_REQUEST 0x16
#define PREPARE_WRITE_RESPONSE 0x17
#define EXECUTE_WRITE_REQUEST 0x18
#define EXECUTE_WRITE_RESPONSE 0x19
#define HANDLE_VALUE_NOTIFICATION 0x1B
#define HANDLE_VALUE_INDICATION 0x1D
#define HANDLE_VALUE_CONFIRMATION 0x1E
#define READ_MULTIPLE_VARIABLE_RESPONSE 0x21
#define MULTIPLE_HANDLE_VALUE_NOTIFICATION 0x23

// gap security authentication result code
#define SECURITY_AUTHENTICATION_SUCCESS 0x45
#define SECURITY_AUTHENTICATION_FAIL 0x46

// bredr connect callback result code
#define BREDR_CONNECT_SUCCESS 0x49
#define BREDR_CONNECT_FAIL 0x50

// le connect callback result code
#define LE_CONNECT_SUCCESS 0x51
#define LE_CONNECT_FAIL 0x52

// le disconnect result code
#define LE_DISCONNECT_SUCCESS 0x53
#define LE_DISCONNECT_FAIL 0x54

// bredr disconnect callback result code
#define INITIATIVECONNECT_DISCONNECT_SUCCESS 0x55
#define INITIATIVECONNECT_DISCONNECT_FAIL 0x56
#define PASSIVECONNECT_DISCONNECT_SUCCESS 0x57
#define PASSIVECONNECT_DISCONNECT_FAIL 0x58
#define DISCONNECT_ABNORMAL 0x59

// bredr connect response result
#define BREDR_CONNECT_ACCEPT 0x00
#define BREDR_CONNECT_REJECTED 0x04

// att connect timeout
#define ATT_CONNECT_TIMEOUT 0x0060

typedef struct {
    uint16_t attHandle;
    BtUuid uuid;
} AttHandleUuid;

typedef struct {
    uint16_t attHandle;
    uint16_t groupEndHandle;
} AttHandleInfo;

typedef struct {
    uint8_t properties;
    uint16_t attributeHandle;
    BtUuid *uuid;
} AttSpecificAttributeValue;

typedef struct {
    uint16_t attHandle;
} AttHandleValue;

typedef struct {
    uint16_t attHandle;
    uint16_t groupEndHandle;
    uint8_t *attributeValue;
} AttReadGoupAttributeData;

typedef struct {
    uint16_t mtuSize;
} AttExchangeMTUType;

typedef struct {
    uint16_t startHandle;
    uint16_t endHandle;
} AttHandleRange;

typedef struct {
    uint8_t reqOpcode;
    uint16_t attHandleInError;
    uint8_t errorCode;
} AttError;

typedef struct {
    AttHandleUuid *handleUuidPairs;
    uint16_t pairNum;
    uint8_t format;
} AttFindInformationRsp;

typedef struct {
    AttHandleRange handleRange;
    uint16_t attType;
} AttFindByTypeValueReq;

typedef struct {
    AttHandleInfo *handleInfoList;
    uint16_t listNum;
} AttFindByTypeValueRsp;

typedef union {
    AttHandleRange findInformationRequest;
    AttFindInformationRsp findInforRsponse;
    AttFindByTypeValueReq findByTypeValueRequest;
    AttFindByTypeValueRsp findByTypeValueResponse;
} AttFind;

typedef struct {
    AttHandleRange handleRange;
    BtUuid *uuid;
} AttReadByTypeReq;

typedef struct {
    AttHandleValue attHandle;
    uint8_t *attributeValue;
} AttReadByTypeRspDataList;

typedef struct {
    AttReadByTypeRspDataList *valueList;
    uint16_t valueNum;
    uint8_t len;
} AttReadByTypeRsp;

typedef struct {
    uint16_t attHandle;
} AttReadReq;

typedef struct {
    uint16_t attHandle;
    uint16_t offset;
} AttReadBlobReqPrepareWriteValue;

typedef struct {
    uint16_t *setHandles;
} AttReadMultipleReq;

typedef struct {
    uint8_t *setValues;
} AttReadMultipleRes;

typedef struct {
    AttHandleRange handleRange;
    BtUuid *uuid;
} AttReadGroupReq;

typedef struct {
    uint8_t length;
    uint16_t num;
    AttReadGoupAttributeData *attributeData;
} AttReadGroupRes;

typedef union {
    AttReadByTypeReq readHandleRangeUuid;
    AttReadByTypeRsp readHandleListNum;
    AttReadReq readHandle;
    AttReadBlobReqPrepareWriteValue readBlob;
    AttReadMultipleReq readMultRequest;
    AttReadMultipleRes readMultResponse;
    AttReadGroupReq readGroupRequest;
    AttReadGroupRes readGroupResponse;
} AttRead;

typedef enum {
    SIGNATURE_OK,
    SIGNATURE_ERR_EXECUTION,
    SIGNATURE_ERR_COUNTER,
    SIGNATURE_ERR_ALGORITHM,
    GAP_REJECT,
} AttSignedWriteCommandResult;

typedef struct {
    AttHandleValue attHandleValueObj;
    uint8_t authSignature[12];
    uint16_t authSignatureLen;
    AttSignedWriteCommandResult result;
} AttSignedWriteComm;

typedef struct {
    AttHandleValue handleValue;
    uint16_t offset;
} AttPrepareWrite;

typedef struct {
    uint8_t flag;
} AttExecuteWrite;

typedef union {
    AttHandleValue writeRequest;
    AttHandleValue writeCommand;
    AttHandleValue confirmation;
    AttSignedWriteComm signedWriteCommand;
    AttPrepareWrite prepareWrite;
    AttExecuteWrite excuteWrite;
} AttWrite;

typedef struct {
    uint8_t status;
    uint8_t role;
    BtAddr addr;
} AttLeConnectCallback;

typedef struct {
    uint8_t status;
    uint8_t reason;
} AttLeDisconnectCallback;

typedef struct {
    BtAddr addr;
    uint8_t status;
    uint16_t mtu;
} AttBredrConnectCallback;

typedef struct {
    uint8_t reason;
} AttBredrDisconnectCallback;

typedef struct {
    uint16_t connIntervalMin;
    uint16_t connIntervalMax;
    uint16_t connLatency;
    uint16_t supervisionTimeout;
} AttLeConnect;

typedef struct {
    uint16_t mtu;
    uint16_t flushTimeout;
    uint8_t mode;
} AttBredrConnect;

typedef union {
    AttLeConnect leConnParaVar;
    AttBredrConnect bredrConnParaVar;
} AttConnect;

typedef union AttEventData {
    AttError attErrorResponse;
    AttExchangeMTUType attExchangeMtuRequest;
    AttExchangeMTUType attExchangeMtuResponse;
    AttFind attFindInformationRequest;
    AttFind attFindInformationResponse;
    AttFind attFindByTypeValueRequest;
    AttFind attFindByTypeValueResponse;
    AttRead attReadByTypeRequest;
    AttRead attReadByTypeResponse;
    AttRead attReadRequest;
    AttRead attReadResponse;
    AttRead attReadBlobRequest;
    AttRead attReadBlobResponse;
    AttRead attReadMultipleRequest;
    AttRead attReadMultipleResponse;
    AttRead attReadByGroupTypeRequest;
    AttRead attReadByGroupTypeResponse;
    AttWrite attWriteRequest;
    AttWrite attWriteResponse;
    AttWrite attWriteCommand;
    AttWrite attSignedWriteCommand;
    AttWrite attPreprareWriteRequest;
    AttWrite attPrepareWriteResponse;
    AttWrite attExecuteWriteRequest;
    AttWrite attExecuteWriteResponse;
    AttHandleValue attNotification;
    AttHandleValue attIndication;
    AttHandleValue attHandleValueConfirmation;
    AttWrite attTimeOut;
} AttEventData;

typedef struct {
    void (*attLEConnectCompleted)(uint16_t connectHandle, AttLeConnectCallback *data, void *context);
    void (*attLEDisconnectCompleted)(uint16_t connectHandle, AttLeDisconnectCallback *data, void *context);
    void (*attBREDRConnectCompleted)(uint16_t connectHandle, AttBredrConnectCallback *data, void *context);
    void (*attBREDRDisconnectCompleted)(uint16_t connectHandle, AttBredrDisconnectCallback *data, void *context);
    void (*attBREDRConnectInd)(uint16_t connectHandle, void *context);
} AttConnectCallback;

typedef void (*attCallback)(uint16_t connectHandle, uint16_t event, void *eventData, Buffer *buffer, void *context);
typedef void (*attSendDataCallback)(uint16_t retGattConnectHandle, int result, void *context);

typedef struct {
    attSendDataCallback attSendDataCB;
    void *context;
} AttClientSendDataCallback;

typedef struct {
    attSendDataCallback attSendDataCB;
    void *context;
} AttServerSendDataCallback;

/**
 * @brief Gatt register the client data callback to att.
 *
 * @param1 dataCallback Indicates the pointer to callback.
 * @param2 context Indicates the pointer to context.
 */
void BTSTACK_API ATT_ClientDataRegister(attCallback dataCallback, void *context);

/**
 * @brief Gatt deregister the client data callback to att.
 *
 */
void BTSTACK_API ATT_ClientDataDeregister();

/**
 * @brief Gatt register the server data callback to att.
 *
 * @param1 dataCallback Indicates the pointer to callback.
 * @param2 context Indicates the pointer to context.
 */
void BTSTACK_API ATT_ServerDataRegister(attCallback dataCallback, void *context);

/**
 * @brief Gatt deregister the server data callback to att.
 *
 */
void BTSTACK_API ATT_ServerDataDeregister();

/**
 * @brief Gatt register the connection callback to att.
 *
 * @param1 connectBack Indicates the struct to callback.
 * @param2 context Indicates the pointer to context.
 */
void BTSTACK_API ATT_ConnectRegister(AttConnectCallback connectBack, void *context);

/**
 * @brief Gatt deregister the connection callback to att.
 *
 */
void BTSTACK_API ATT_ConnectDeregister();

/**
 * @brief Gatt client register the send data callback to att.
 *
 * @param1 attSendDataCB Indicates pointer of attSendDataCallback.
 * @param2 context Indicates the pointer to context.
 */
void BTSTACK_API ATT_ClientSendDataRegister(attSendDataCallback attSendDataCB, void *context);

/**
 * @brief Gatt client deregister the send data callback to att.
 */
void BTSTACK_API ATT_ClientSendDataDeRegister();

/**
 * @brief Gatt server register the send data callback to att.
 *
 * @param1 AttSendDataCB Indicates pointer of attSendDataCallback.
 * @param2 context Indicates the pointer to context.
 */
void BTSTACK_API ATT_ServerSendDataRegister(attSendDataCallback AttSendDataCB, void *context);

/**
 * @brief Gatt server deregister the send data callback to att.
 */
void BTSTACK_API ATT_ServerSendDataDeRegister();

/**
 * @brief Send a connect request.
 *
 * @param1 transportType Indicates the connect type.
 * @param2 connParaPtr Indicates the pointer to const connect parameter.
 * @param3 btAddress Indicates the pointer to const address.
 * @param4 bredrconnectHandle Indicates the pointer to connecthandle be outputted.
 */
void BTSTACK_API ATT_ConnectReq(
    uint8_t transportType, const AttConnect *connParaPtr, const BtAddr *btAddress, const uint16_t *bredrconnectHandle);

/**
 * @brief Send a connect response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 result Indicates the variable of result.
 * @param3 status Indicates the result of status.
 * @param4 cfg Indicates the pointer to const connect parameter.
 */
void BTSTACK_API ATT_ConnectRsp(uint16_t connectHandle, uint16_t result, uint16_t status, const AttConnect *cfg);

/**
 * @brief Send a disconnect request.
 *
 * @param connectHandle Indicates the connect handle.
 */
void BTSTACK_API ATT_DisconnectReq(uint16_t connectHandle);

/**
 * @brief Send an error response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 ATTErrorPtr Indicates the pointer to const error response parameter.
 */
void BTSTACK_API ATT_ErrorResponse(uint16_t connectHandle, const AttError *ATTErrorPtr);

/**
 * @brief Send an exchange MTU request .
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 clientRxMTU Indicates the client receive MTU size.
 */
void BTSTACK_API ATT_ExchangeMTURequest(uint16_t connectHandle, uint16_t clientRxMTU);

/**
 * @brief Send an exchange MTU response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 serverRxMTU Indicates the attribute server receive MTU size.
 */
void BTSTACK_API ATT_ExchangeMTUResponse(uint16_t connectHandle, uint16_t serverRxMTU);

/**
 * @brief Send a find Information request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 startHandle Indicates the first requested handle number.
 * @param3 endHandle Indicates the last requested handle number.
 */
void BTSTACK_API ATT_FindInformationRequest(uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle);

/**
 * @brief Send a find Information response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 format Indicates the format of the information data.
 * @param3 handleUUIDPairs Indicates the pointer to const information data.
 * @param4 pairNum Indicates the paris number of the Information Data.
 */
void BTSTACK_API ATT_FindInformationResponse(
    uint16_t connectHandle, uint8_t format, AttHandleUuid *handleUUIDPairs, uint16_t pairNum);

/**
 * @brief Send a find by type value request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attFindByTypePtreve Indicates the pointer to const the parameter of AttFindByTypeValueReq.
 * @param3 attValue Indicates the pointer to attribute value to find.
 */
void BTSTACK_API ATT_FindByTypeValueRequest(
    uint16_t connectHandle, const AttFindByTypeValueReq *attFindByTypePtreve, const Buffer *attValue);

/**
 * @brief Send a find by type value response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 handleInfoList Indicates the pointer to const a list of 1 or more Handle Information.
 * @param3 listNum Indicates the number of handles information list.
 */
void BTSTACK_API ATT_FindByTypeValueResponse(
    uint16_t connectHandle, const AttHandleInfo *handleInfoList, uint16_t listNum);

/**
 * @brief Send a read by type request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 startHandle Indicates the first requested handle number.
 * @param3 endHandle Indicates the last requested handle number.
 * @param4 uuid Indicates the pointer to const 2 or 16 octet UUID.
 */
void BTSTACK_API ATT_ReadByTypeRequest(
    uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, const BtUuid *uuid);

/**
 * @brief Send a read by type response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 length Indicates the size of each attribute handlevalue pair.
 * @param3 valueList Indicates the pointer to const a list of attribute data.
 * @param4 attrValueNum Indicates the value of attribute value number.
 */
void BTSTACK_API ATT_ReadByTypeResponse(
    uint16_t connectHandle, uint8_t length, const AttReadByTypeRspDataList *valueList, uint16_t attrValueNum);

/**
 * @brief Send a read request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute to be read.
 */
void BTSTACK_API ATT_ReadRequest(uint16_t connectHandle, uint16_t attHandle);

/**
 * @brief Send a read response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attValue Indicates the pointer to the value of the attribute with the handle given.
 */
void BTSTACK_API ATT_ReadResponse(uint16_t connectHandle, const Buffer *attValue);

/**
 * @brief Send a read blob request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute to be read.
 * @param3 offset Indicates the offset of the first octet to be read.
 */
void BTSTACK_API ATT_ReadBlobRequest(uint16_t connectHandle, uint16_t attHandle, uint16_t offset);

/**
 * @brief Send a read blob response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attReadBlobResObj Indicates the pointer to part of the value of the attribute with the handle given.
 */
void BTSTACK_API ATT_ReadBlobResponse(uint16_t connectHandle, const Buffer *attReadBlobResObj);

/**
 * @brief Send a read multiple request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 handleList Indicates the pointer to a set of two or more attribute handles.
 */
void BTSTACK_API ATT_ReadMultipleRequest(uint16_t connectHandle, const Buffer *handleList);

/**
 * @brief Send a read multiple response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 valueList Indicates the pointer to a set of two or more values.
 */
void BTSTACK_API ATT_ReadMultipleResponse(uint16_t connectHandle, const Buffer *valueList);

/**
 * @brief Send a read by group type request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 startHandle Indicates the first requested handle number.
 * @param3 endHandle Indicates the last requested handle number.
 * @param4 uuid Indicates the pointer to 2 or 16 octet UUID.
 */
void BTSTACK_API ATT_ReadByGroupTypeRequest(
    uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, const BtUuid *uuid);

/**
 * @brief Send a read by group type response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 length Indicates the size of each attribute data.
 * @param3 serviceList Indicates the pointer to const a list of attribute data.
 * @param4 serviceNum Indicates the number of attribute data.
 */
void BTSTACK_API ATT_ReadByGroupTypeResponse(
    uint16_t connectHandle, uint8_t length, const AttReadGoupAttributeData *serviceList, uint16_t serviceNum);

/**
 * @brief Send a write request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute to be written.
 * @param3 attValue Indicates the pointer to the value to be written to the attribute.
 */
void BTSTACK_API ATT_WriteRequest(uint16_t connectHandle, uint16_t attHandle, const Buffer *attValue);

/**
 * @brief Send a write response.
 *
 * @param connectHandle Indicates the connect handle.
 */
void BTSTACK_API ATT_WriteResponse(uint16_t connectHandle);

/**
 * @brief Send a write command.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute to be set.
 * @param3 attValue Indicates the pointer to the value of be written to the attribute.
 */
void BTSTACK_API ATT_WriteCommand(uint16_t connectHandle, uint16_t attHandle, const Buffer *attValue);

/**
 * @brief Send a signed write command.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the the handle of the attribute to be set.
 * @param3 attValue Indicates the pointer to the value to be written to the attribute.
 */
void BTSTACK_API ATT_SignedWriteCommand(uint16_t connectHandle, uint16_t attHandle, const Buffer *attValue);

/**
 * @brief Send a prepare write request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attReadBlobObj Indicates the value of the struct AttReadBlobReqPrepareWriteValue.
 * @param3 attValue Indicates the pointer to the value of the attribute to be written.
 */
void BTSTACK_API ATT_PrepareWriteRequest(
    uint16_t connectHandle, AttReadBlobReqPrepareWriteValue attReadBlobObj, const Buffer *attValue);

/**
 * @brief Send a prepare write response.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attReadBlobObj Indicates the value of the struct AttReadBlobReqPrepareWriteValue.
 * @param3 attValue Indicates the pointer to the value of the attribute to be written.
 */
void BTSTACK_API ATT_PrepareWriteResponse(
    uint16_t connectHandle, AttReadBlobReqPrepareWriteValue attReadBlobObj, const Buffer *attValue);

/**
 * @brief Send a execute write request.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 flag Indicates the value of flags.
 */
void BTSTACK_API ATT_ExecuteWriteRequest(uint16_t connectHandle, uint8_t flag);

/**
 * @brief Send a execute write response.
 *
 * @param connectHandle Indicates the connect handle.
 */
void BTSTACK_API ATT_ExecuteWriteResponse(uint16_t connectHandle);

/**
 * @brief Send a handle value notification.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute.
 * @param3 attValue Indicates the pointer to the current value of the attribute.
 */
void BTSTACK_API ATT_HandleValueNotification(uint16_t connectHandle, uint16_t attHandle, const Buffer *attValue);

/**
 * @brief Send a handle value indication.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute.
 * @param3 attValue Indicates the pointer to the current value of the attribute.
 */
void BTSTACK_API ATT_HandleValueIndication(uint16_t connectHandle, uint16_t attHandle, const Buffer *attValue);

/**
 * @brief Send a handle value confirmation.
 *
 * @param connectHandle Indicates the connect handle.
 */
void BTSTACK_API ATT_HandleValueConfirmation(uint16_t connectHandle);

/**
 * @brief Cancel Le ACL connection
 *
 * @return Returns BT_NO_ERROR if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API ATT_LeConnectCancel(const BtAddr *addr);

#ifdef __cplusplus
}
#endif

#endif  // ATT_H
