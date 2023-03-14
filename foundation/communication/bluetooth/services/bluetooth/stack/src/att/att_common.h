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
 * @file att_common.h
 *
 * @brief declare common function to be called.
 *
 */

#ifndef ATT_COMMON_H
#define ATT_COMMON_H

#include "att.h"

#include <stdint.h>
#include <string.h>

#include "alarm.h"
#include "btstack.h"
#include "list.h"
#include "packet.h"

#include "l2cap_if.h"
#include "l2cap_le_if.h"

#include "gap_if.h"
#include "gap_le_if.h"

#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_PSM_ATT 0x001F

#define LE_CID 0x04

#define MAXCONNECT 22

#define GAPSIGNATURESIZE 12

#define ADDRESSLEN 6
#define UUID128LEN 16

#define DEFAULTBREDRMTU 48
#define DEFAULTLEATTMTU 23

#define STEP_TWO 2
#define STEP_THREE 3
#define STEP_FOUR 4
#define STEP_FIVE 5
#define STEP_SIX 6
#define STEP_FIFTEEN 15

#define MAXREADBYTYPERESLEN 253
#define MAXREADBYGROUPRESLEN 251

// initiative connect and passive connect flag
#define INITIATIVECONNECT 1
#define PASSIVECONNECT 2

// timeout
#define CONNECTTIMEOUT 40000
#define INSTRUCTIONTIMEOUT 30000

// attribute type len
#define UUID16BITTYPELEN 2
#define UUID128BITTYPELEN 16

#define HANDLEAND16BITBLUETOOTHUUID 0x01
#define HANDLEAND128BITUUID 0x02

#define READBYTYPERESPONSEFREE 1
#define READBYGROUPTYPERESPONSEFREE 2

#define FINDINFORRESINFOR16BITLEN 4
#define FINDINFORRESINFOR128BITLEN 18

// execute write request
#define IMMEDIATELY_WRITE_ALL_PENDING_PREPARED_VALUES 1
#define CANCEL_ALL_PREPARED_WRITES 0

typedef struct AttConnectInfo {
    uint16_t aclHandle;
    union {
        uint16_t bredrcid;
        uint16_t lecid;
    } AttConnectID;
    uint16_t retGattConnectHandle;
    uint8_t transportType;
    BtAddr addr;
    uint16_t mtu;
    uint16_t sendMtu;
    uint16_t receiveMtu;
    bool mtuFlag;
    uint8_t initPassConnFlag;
    List *instruct;
    Alarm *alarm;
    bool serverSendFlag;
} AttConnectInfo;

typedef struct AttConnectingInfo {
    uint16_t aclHandle;
    uint16_t cid;
    uint8_t id;
    uint16_t mtu;
    L2capConfigInfo locall2capConfigInfoObj;
    L2capConfigInfo remotel2capConfigInfoObj;
    BtAddr addr;
    uint16_t connectHandle;
    uint8_t initiativeConnectStatus;
    uint8_t passiveConnectSatatus;
    uint8_t initPassConnFlag;  // 1:initiative connect; 2:passive connect
    Alarm *bredrAlarm;
    Alarm *leAlarm;
    uint8_t transportType;
} AttConnectingInfo;

typedef void (*recvDataFunction)(AttConnectInfo *connect, const Buffer *buffer);

typedef struct {
    attCallback attClientCallback;
    void *context;
} AttClientDataCallback;

typedef struct {
    attCallback attServerCallback;
    void *context;
} AttServerDataCallback;

typedef struct {
    AttConnectCallback attConnect;
    void *context;
} AttConnectedCallback;

typedef struct {
    uint16_t connectHandle;
    AttError *ATTErrorPtr;
} ErrorResponseAsync;

typedef struct {
    uint16_t connectHandle;
    uint16_t mtu;
} ExchangeMTUAsync;

typedef struct {
    uint16_t connectHandle;
    AttHandleRange attHandleRange;
} FindInformationRequestAsync;

typedef struct {
    uint16_t connectHandle;
    AttFindInformationRsp attFindInformationResContext;
} FindInformationResponseAsync;

typedef struct {
    uint16_t connectHandle;
    AttFindByTypeValueReq *attFindByTypePtreve;
    Buffer *attValue;
} FindByTypeValueRequestAsync;

typedef struct {
    uint16_t connectHandle;
    AttFindByTypeValueRsp attFindByTypeResContext;
} FindByTypeValueResponseAsync;

typedef struct {
    uint16_t connectHandle;
    AttReadByTypeReq attReadByTypeReqContext;
} ReadByTypeRequestAsync;

typedef struct {
    uint16_t connectHandle;
    AttReadByTypeRsp attReadByTypeRspContext;
} ReadByTypeResponseAsync;

typedef struct {
    uint16_t connectHandle;
    uint16_t attHandle;
} ReadRequestAsync;

typedef struct {
    uint16_t connectHandle;
    Buffer *attValue;
} ReadResponseAsync;  // readresponse / readblobresponse / readmultipleresponse / readmultiplerequest

typedef struct {
    uint16_t connectHandle;
    AttReadBlobReqPrepareWriteValue attReadBlobContext;
} ReadBlobRequestAsync;

typedef struct {
    uint16_t connectHandle;
    AttReadByTypeReq attReadGroupContext;
} ReadByGroupTypeRequesAsync;

typedef struct {
    uint16_t connectHandle;
    AttReadGroupRes attReadGroupResContext;
} ReadByGroupTypeResponseAsync;

typedef struct {
    uint16_t connectHandle;
    uint16_t attHandle;
    Buffer *attValue;
} WriteAsync;  // writerequest / writecommand / signedwritecommand / handlenotification / handleindication

typedef struct {
    uint16_t connectHandle;
} WriteResponseAsync;  // writeresponse / executewriterresponse / handleconfirmation

typedef struct {
    uint16_t connectHandle;
    AttReadBlobReqPrepareWriteValue attReadBlobObj;
    Buffer *attValue;
} PrepareWriteAsync;  // preparewriterequest / preparewriteresponse

typedef struct {
    uint16_t connectHandle;
    uint8_t flag;
} ExecuteWriteRequestAsync;

typedef struct SigedWriteCommandGenerationContext {
    AttConnectInfo *connect;
    Packet *packet;
    uint8_t *data;
    uint16_t bufferSize;
} SigedWriteCommandGenerationContext;

typedef struct AttGapSignatureGenerationContext {
    GAP_SignatureResult result;
    uint8_t *signaturePtr;
    uint16_t signatureLen;
    void *context;
} AttGapSignatureGenerationContext;

/**
 * @brief get AttConnectInfo information.
 *
 * @return Returns the pointer to AttConnectInfo.
 */
AttConnectInfo *AttGetConnectStart();

/**
 * @brief lookup AttConnectInfo info by aclHandle.
 *
 * @param1 aclHandle Indicates the aclHandle.
 * @param2 connect Indicates the second rank pointer to AttConnectInfo.
 */
void AttGetConnectInfoIndexByAclHandle(uint16_t aclHandle, AttConnectInfo **connect);

/**
 * @brief lookup AttConnectInfo info by cid.
 *
 * @param1 cid Indicates the cid.
 * @param2 connect Indicates the second rank pointer to AttConnectInfo.
 */
void AttGetConnectInfoIndexByCid(uint16_t cid, AttConnectInfo **connect);

/**
 * @brief lookup AttConnectInfo info by cid and output parameter index.
 *
 * @param1 cid Indicates the cid.
 * @param2 index Indicates the pointer to index.
 * @param3 connect Indicates second rank pointer to AttConnectInfo.
 */
void AttGetConnectInfoIndexByCidOutIndex(uint16_t cid, uint16_t *index, AttConnectInfo **connect);

/**
 * @brief lookup AttConnectInfo info by connectHandle and output parameter index.
 *
 * @param1 connectHandle Indicates the connectHandle.
 * @param2 index Indicates the pointer to index.
 * @param3 connect Indicates second rank pointer to AttConnectInfo.
 */
void AttGetConnectInfoIndexByConnectHandle(uint16_t connectHandle, uint16_t *index, AttConnectInfo **connect);

/**
 * @brief get AttConnectingInfo information.
 *
 * @return Returns the pointer to AttConnectingInfo.
 */
AttConnectingInfo *AttGetConnectingStart();

/**
 * @brief lookup AttConnectingInfo info by cid.
 *
 * @param1 cid Indicates the cid.
 * @param2 connecting Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByCid(uint16_t cid, AttConnectingInfo **connecting);

/**
 * @brief lookup AttConnectingInfo info by cid and output parameter index.
 *
 * @param1 cid Indicates the cid.
 * @param2 index Indicates the pointer to index.
 * @param3 connecting Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByCidOutIndex(uint16_t cid, uint16_t *index, AttConnectingInfo **connecting);

/**
 * @brief lookup AttConnectingInfo info by connectHandle.
 *
 * @param1 connectHandle Indicates the connectHandle.
 * @param2 connecting Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByConnectHandle(uint16_t connectHandle, AttConnectingInfo **connecting);

/**
 * @brief lookup AttConnectingInfo info by cid and connectHandle, result to output parameter index.
 *
 * @param1 cid Indicates the cid.
 * @param2 connectHandle Indicates the connectHandle.
 * @param3 index Indicates the pointer to index.
 * @param4 connecting Indicates second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByCidConnectHandle(
    uint16_t cid, uint16_t connectHandle, uint16_t *index, AttConnectingInfo **connecting);

/**
 * @brief lookup AttConnectingInfo info by addr.
 *
 * @param1 addr Indicates pointer to addr.
 * @param2 connecting Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByAddr(const BtAddr *addr, AttConnectingInfo **connecting);

/**
 * @brief lookup AttConnectingInfo info by addr uninitialized cid.
 *
 * @param1 addr Indicates pointer to addr.
 * @param2 connecting Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByAddrUninitializedCid(const BtAddr *addr, AttConnectingInfo **connecting);

/**
 * @brief lookup AttConnectingInfo info by addr aclhandle cid.
 *
 * @param1 addr Indicates pointer to addr.
 * @param2 addr Indicates the aclHandle.
 * @param3 addr Indicates the cid.
 * @param4 connecting Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByAddrAclhandleCid(
    const BtAddr *addr, uint16_t aclHandle, uint16_t cid, AttConnectingInfo **connecting);

/**
 * @brief get AttClientDataCallback information.
 *
 * @return Returns the pointer to AttClientDataCallback.
 */
AttClientDataCallback *AttGetATTClientCallback();

/**
 * @brief get AttServerDataCallback information.
 *
 * @return Returns the pointer to AttServerDataCallback.
 */
AttServerDataCallback *AttGetATTServerCallback();

/**
 * @brief initiative execut instructions by Scheduling.
 *
 * @param connect Indicates the pointer to AttConnectInfo.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
int AttSendSequenceScheduling(const AttConnectInfo *connect);

/**
 * @brief execut instructions by Scheduling after receiving response.
 *
 * @param connect Indicates the pointer to AttConnectInfo.
 */
void AttReceiveSequenceScheduling(const AttConnectInfo *connect);

/**
 * @brief client call back copy.
 *
 * @param1 attSendDataCB Indicates the pointer to attSendDataCallback.
 * @param2 context Indicates the pointer to context.
 */
void AttClientCallBackCopyToCommon(attSendDataCallback attSendDataCB, const void *context);

/**
 * @brief server call back copy.
 *
 * @param1 attSendDataCB Indicates the pointer to attSendDataCallback.
 * @param2 context Indicates the pointer to context.
 */
void AttServerCallBackCopyToCommon(attSendDataCallback attSendDataCB, const void *context);
/**
 * @brief receive senddata callback.
 *
 * @param1 aclHandle Indicates the aclHandle.
 * @param2 result Indicates the result.
 */
void LeRecvSendDataCallback(uint16_t aclHandle, int result);

/**
 * @brief bredr receive senddata callback.
 *
 * @param1 aclHandle Indicates the aclHandle.
 * @param2 result Indicates the result.
 * @param3 cb Indicates the pointer to cb.
 */
void BREDRRecvSendDataCallback(uint16_t lcid, int result);

/**
 * @brief receive delect callback.
 *
 */
void AttCallBackDelectCopyToCommon();

/**
 * @brief switch thread.
 *
 * @param1 callback Indicates the pointer to function pointer.
 * @param2 destroyCallback Indicates the pointer to function pointer.
 * @param3 context Indicates the pointer to context.
 */
void AttAsyncProcess(
    void (*callback)(const void *context), void (*destroyCallback)(const void *context), const void *context);

/**
 * @brief shut down clear connect information.
 *
 * @param connectInfo Indicates the pointer to AttConnectInfo.
 */
void AttShutDownClearConnectInfo(AttConnectInfo *connectInfo);

/**
 * @brief client call back btbadparam.
 *
 * @param connect Indicates the pointer to AttConnectInfo.
 */
void ClientCallbackBTBADPARAM(const AttConnectInfo *connect);

/**
 * @brief server call back btbadparam.
 *
 * @param connect Indicates the pointer to AttConnectInfo.
 */
void ServerCallbackBTBADPARAM(const AttConnectInfo *connect);

/**
 * @brief client call back return value.
 *
 * @param1 ret Indicates the ret.
 * @param2 connect Indicates the pointer to AttConnectInfo.
 */
void ClientCallbackReturnValue(int ret, const AttConnectInfo *connect);

/**
 * @brief server call back return value.
 *
 * @param1 ret Indicates the ret.
 * @param2 connect Indicates the pointer to AttConnectInfo.
 */
void ServerCallbackReturnValue(int ret, const AttConnectInfo *connect);

/**
 * @brief received bredr connect instructions data information.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 packet Indicates the pointer to Packet.
 * @param3 ctx Indicates the pointer to context.
 */
void AttRecvData(uint16_t lcid, const Packet *packet, const void *ctx);

/**
 * @brief received le connect instructions data information.
 *
 * @param1 aclHandle Indicates the aclHandle.
 * @param2 packet Indicates the pointer to Packet.
 */
void AttRecvLeData(uint16_t aclHandle, const Packet *packet);

AttConnectedCallback *AttGetATTConnectCallback();

int AttResponseSendData(const AttConnectInfo *connect, const Packet *packet);

/**
 * @brief received error opcode.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 error opcode.
 */
void AttErrorCode(const AttConnectInfo *connect, uint8_t opcode);

/**
 * @brief get function array dress.
 *
 * @return Returns the pointer to recvDataFunction.
 */
recvDataFunction *GetFunctionArrayDress();

/**
 * @brief return minute number
 *
 * @param1 number1
 * @param2 number2
 */
uint16_t Min(uint16_t param1, uint16_t param2);

void AttShutDownClearConnectInfo(AttConnectInfo *connectInfo);

#ifdef __cplusplus
}
#endif

#endif  // ATT_COMMON_H
