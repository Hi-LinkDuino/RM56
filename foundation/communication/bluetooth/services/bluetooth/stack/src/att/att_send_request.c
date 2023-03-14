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
 * @file att_send.c
 *
 * @brief implement send function to be called.
 *
 */

#include "att_common.h"
#include "att_receive.h"

#include <memory.h>

#include "buffer.h"
#include "list.h"
#include "packet.h"

#include "platform/include/allocator.h"

#include "log.h"

#include "gap_if.h"
#include "gap_le_if.h"

static AttClientSendDataCallback g_attClientSendDataCB;

static void AttSingedWriteCommandContextAssign(SigedWriteCommandGenerationContext *sigedWriteCommandGenerContextPtr,
    AttConnectInfo *connect, uint8_t *data, Packet *packet, uint16_t bufferSize);
static void AttSignedWriteCommandErrorFree(
    const AttConnectInfo *connect, int ret, Buffer *sigedWriteBuffPtr, Packet *packet);
static void AttSignedWriteGapSignatureDataAssign(
    GapSignatureData *gapSignatureDataPtr, const uint8_t *data, uint16_t bufferSize);
static void AttReadByGroupTypeRequestDataAssign(
    uint8_t *data, const ReadByGroupTypeRequesAsync *attReadByGroupRequestAsyncPtr, uint16_t attrGroupTypeLen);

static void AttExchangeMTURequestAsync(const void *context);
static void AttExchangeMTURequestAsyncDestroy(const void *context);
static void AttFindInformationRequestAsync(const void *context);
static void AttFindInformationRequestAsyncDestroy(const void *context);
static void AttFindByTypeValueRequestAsync(const void *context);
static void AttFindByTypeValueRequestAsyncDestroy(const void *context);
static void AttReadByTypeRequestAsync(const void *context);
static void AttReadByTypeRequestAsyncDestroy(const void *context);
static void AttReadRequestAsync(const void *context);
static void AttReadRequestAsyncDestroy(const void *context);
static void AttReadBlobRequestAsync(const void *context);
static void AttReadBlobRequestAsyncDestroy(const void *context);
static void AttReadMultipleRequestAsync(const void *context);
static void AttReadMultipleRequestAsyncDestroy(const void *context);
static void AttReadByGroupTypeRequestAsync(const void *context);
static void AttReadByGroupTypeRequestAsyncDestroy(const void *context);
static void AttWriteRequestAsync(const void *context);
static void AttWriteRequestAsyncDestroy(const void *context);
static void AttWriteCommandAsync(const void *context);
static void AttWriteCommandAsyncDestroy(const void *context);
static void AttSignedWriteCommandAsync(const void *context);
static void AttSignedWriteCommandAsyncDestroy(const void *context);
static void AttPrepareWriteRequestAsync(const void *context);
static void AttPrepareWriteRequestAsyncDestroy(const void *context);
static void AttExecuteWriteRequestAsync(const void *context);
static void AttExecuteWriteRequestAsyncDestroy(const void *context);
static void AttHandleValueConfirmationAsync(const void *context);
static void AttHandleValueConfirmationAsyncDestroy(const void *context);
static void AttClientSendDataRegisterAsync(const void *context);
static void AttClientSendDataRegisterAsyncDestroy(const void *context);
static void AttClientSendDataDeRegisterAsync(const void *context);
static void AttClientSendDataDeRegisterAsyncDestroy(const void *context);
static void AttGapSignatureGenerationResultAsync(const void *context);
static void AttGapSignatureGenerationResultAsyncDestroy(const void *context);

/**
 * @brief  callback of gap signature result async.
 *
 * @param context Indicates the pointer to context.
 */
void AttGapSignatureGenerationResultAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttGapSignatureGenerationContext *attGapSignaturePtr = (AttGapSignatureGenerationContext *)context;

    Buffer *bufferSig = (Buffer *)(attGapSignaturePtr->context);
    SigedWriteCommandGenerationContext *sigedWriteCommandGenerContextPtr = NULL;

    if (bufferSig == NULL) {
        LOG_INFO("%{public}s bufferSig == NULL", __FUNCTION__);
        goto GAPSIGNATUREGENERATIONRESULT_END;
    }

    sigedWriteCommandGenerContextPtr = (SigedWriteCommandGenerationContext *)BufferPtr(bufferSig);
    g_attClientSendDataCB.attSendDataCB(sigedWriteCommandGenerContextPtr->connect->retGattConnectHandle,
        attGapSignaturePtr->result,
        g_attClientSendDataCB.context);

    (void)memcpy_s(sigedWriteCommandGenerContextPtr->data + STEP_THREE + sigedWriteCommandGenerContextPtr->bufferSize,
        attGapSignaturePtr->signatureLen,
        attGapSignaturePtr->signaturePtr,
        attGapSignaturePtr->signatureLen);
    AttResponseSendData(sigedWriteCommandGenerContextPtr->connect, sigedWriteCommandGenerContextPtr->packet);

    PacketFree(sigedWriteCommandGenerContextPtr->packet);
    BufferFree(bufferSig);

GAPSIGNATUREGENERATIONRESULT_END:
    MEM_MALLOC.free(attGapSignaturePtr->signaturePtr);
    MEM_MALLOC.free(attGapSignaturePtr);

    return;
}

/**
 * @brief  callback of gap signature result async destroy..
 *
 * @param context Indicates the pointer to context.
 */
void AttGapSignatureGenerationResultAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttGapSignatureGenerationContext *attGapSignaturePtr = (AttGapSignatureGenerationContext *)context;

    MEM_MALLOC.free(attGapSignaturePtr->signaturePtr);
    MEM_MALLOC.free(attGapSignaturePtr);

    return;
}

/**
 * @brief callback of gap signature result.
 *
 * @param1 result Indicates the GAP_SignatureResult.
 * @param2 signature Indicates the pointer to array.
 * @param3 context Indicates the pointer to context.
 */
void AttGapSignatureGenerationResult(GAP_SignatureResult result, uint8_t signature[GAP_SIGNATURE_SIZE], void *context)
{
    LOG_INFO("%{public}s enter, result = %u", __FUNCTION__, result);

    uint8_t *signatureResultPtr = NULL;
    AttGapSignatureGenerationContext *attGapSignaturePtr = NULL;

    signatureResultPtr = MEM_MALLOC.alloc(GAPSIGNATURESIZE);
    if (signatureResultPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    for (int i = 0; i < GAP_SIGNATURE_SIZE; i++) {
        signatureResultPtr[i] = signature[i];
    }

    attGapSignaturePtr = MEM_MALLOC.alloc(sizeof(AttGapSignatureGenerationContext));
    if (attGapSignaturePtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attGapSignaturePtr->result = result;
    attGapSignaturePtr->signaturePtr = signatureResultPtr;
    attGapSignaturePtr->signatureLen = GAPSIGNATURESIZE;
    attGapSignaturePtr->context = context;

    AttAsyncProcess(
        AttGapSignatureGenerationResultAsync, AttGapSignatureGenerationResultAsyncDestroy, attGapSignaturePtr);

    return;
}

/**
 * @brief exchange mtu request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttExchangeMTURequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    uint16_t index = 0;
    ExchangeMTUAsync *exchangeMtuReqPtr = (ExchangeMTUAsync *)context;
    AttConnectInfo *connect = NULL;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    int ret;

    AttGetConnectInfoIndexByConnectHandle(exchangeMtuReqPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTEXCHANGEMTUREQUEST_END", __FUNCTION__);
        goto ATTEXCHANGEMTUREQUEST_END;
    }

    if ((exchangeMtuReqPtr->mtu) < DEFAULTLEATTMTU) {
        LOG_WARN("%{public}s:clientRxMTU < DEFAULTLEATTMTU", __FUNCTION__);
        ClientCallbackBTBADPARAM(connect);
        goto ATTEXCHANGEMTUREQUEST_END;
    }

    connect->sendMtu = exchangeMtuReqPtr->mtu;
    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(exchangeMtuReqPtr->mtu));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = EXCHANGE_MTU_REQUEST;
    ((uint16_t *)(data + 1))[0] = exchangeMtuReqPtr->mtu;

    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATTEXCHANGEMTUREQUEST_END:
    MEM_MALLOC.free(exchangeMtuReqPtr);
    return;
}

/**
 * @brief destroy exchange mtu request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttExchangeMTURequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ExchangeMTUAsync *exchangeMtuReqPtr = (ExchangeMTUAsync *)context;

    MEM_MALLOC.free(exchangeMtuReqPtr);

    return;
}

/**
 * @brief gatt send exchangeMTU request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 clientRxMTU Indicates the client receive MTU size.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_ExchangeMTURequest(uint16_t connectHandle, uint16_t clientRxMTU)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu, clientRxMTU = %hu", __FUNCTION__, connectHandle, clientRxMTU);

    ExchangeMTUAsync *exchangeMtuReqPtr = MEM_MALLOC.alloc(sizeof(ExchangeMTUAsync));
    if (exchangeMtuReqPtr == NULL) {
        if (g_attClientSendDataCB.attSendDataCB != NULL) {
            g_attClientSendDataCB.attSendDataCB(connectHandle, BT_NO_MEMORY, g_attClientSendDataCB.context);
        }
        return;
    }
    exchangeMtuReqPtr->connectHandle = connectHandle;
    exchangeMtuReqPtr->mtu = clientRxMTU;
    AttAsyncProcess(AttExchangeMTURequestAsync, AttExchangeMTURequestAsyncDestroy, exchangeMtuReqPtr);

    return;
}

/**
 * @brief find information request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttFindInformationRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    FindInformationRequestAsync *findInformReqPtr = (FindInformationRequestAsync *)context;
    AttConnectInfo *connect = NULL;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    int ret;

    AttGetConnectInfoIndexByConnectHandle(findInformReqPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTFINDINFORMATIONREQUEST_END", __FUNCTION__);
        goto ATTFINDINFORMATIONREQUEST_END;
    }

    packet = PacketMalloc(0,
        0,
        sizeof(uint8_t) + sizeof(findInformReqPtr->attHandleRange.startHandle) +
            sizeof(findInformReqPtr->attHandleRange.endHandle));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = FIND_INFORMATION_REQUEST;
    ((uint16_t *)(data + 1))[0] = findInformReqPtr->attHandleRange.startHandle;
    ((uint16_t *)(data + STEP_THREE))[0] = findInformReqPtr->attHandleRange.endHandle;
    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATTFINDINFORMATIONREQUEST_END:
    MEM_MALLOC.free(findInformReqPtr);
    return;
}

/**
 * @brief destroy find information request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttFindInformationRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    FindInformationRequestAsync *findInformReqPtr = (FindInformationRequestAsync *)context;

    MEM_MALLOC.free(findInformReqPtr);

    return;
}

/**
 * @brief gatt send findInformation request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 startHandle Indicates the first requested handle number.
 * @param3 endHandle Indicates the last requested handle number.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_FindInformationRequest(uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu, startHandle = %{public}d, endHandle =%{public}d",
        __FUNCTION__,
        connectHandle,
        startHandle,
        endHandle);

    FindInformationRequestAsync *findInformReqPtr = MEM_MALLOC.alloc(sizeof(FindInformationRequestAsync));
    if (findInformReqPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    findInformReqPtr->connectHandle = connectHandle;
    findInformReqPtr->attHandleRange.startHandle = startHandle;
    findInformReqPtr->attHandleRange.endHandle = endHandle;
    AttAsyncProcess(AttFindInformationRequestAsync, AttFindInformationRequestAsyncDestroy, findInformReqPtr);

    return;
}

/**
 * @brief find by type value request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttFindByTypeValueRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    int ret;
    FindByTypeValueRequestAsync *contextPtr = (FindByTypeValueRequestAsync *)context;
    Packet *packet = NULL;
    AttConnectInfo *connect = NULL;
    uint8_t *data = NULL;

    AttGetConnectInfoIndexByConnectHandle(contextPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATT_FINDBYTYPEVALUEREQUEST_END", __FUNCTION__);
        goto ATT_FINDBYTYPEVALUEREQUEST_END;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(AttFindByTypeValueReq));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = FIND_BY_TYPE_VALUE_REQUEST;
    ((uint16_t *)(data + 1))[0] = contextPtr->attFindByTypePtreve->handleRange.startHandle;
    ((uint16_t *)(data + STEP_THREE))[0] = contextPtr->attFindByTypePtreve->handleRange.endHandle;
    ((uint16_t *)(data + STEP_FIVE))[0] = contextPtr->attFindByTypePtreve->attType;

    PacketPayloadAddLast(packet, contextPtr->attValue);
    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATT_FINDBYTYPEVALUEREQUEST_END:
    MEM_MALLOC.free(contextPtr->attFindByTypePtreve);
    BufferFree(contextPtr->attValue);
    MEM_MALLOC.free(contextPtr);
    return;
}

/**
 * @brief find by type value request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttFindByTypeValueRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    FindByTypeValueRequestAsync *contextPtr = (FindByTypeValueRequestAsync *)context;

    MEM_MALLOC.free(contextPtr->attFindByTypePtreve);
    BufferFree(contextPtr->attValue);
    MEM_MALLOC.free(contextPtr);

    return;
}

/**
 * @brief gatt send findbytypevalue request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attFindByTypePtreve Indicates the pointer to const the parameter of AttFindByTypeValueReq.
 * @param3 attValue Indicates the pointer to attribute value to find.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_FindByTypeValueRequest(
    uint16_t connectHandle, const AttFindByTypeValueReq *attFindByTypePtreve, const Buffer *attValue)
{
    LOG_INFO("%{public}s enter,connectHandle = %hu, startHandle = %{public}d, endHandle = %{public}d, attType = %{public}d",
        __FUNCTION__,
        connectHandle,
        attFindByTypePtreve->handleRange.startHandle,
        attFindByTypePtreve->handleRange.endHandle,
        attFindByTypePtreve->attType);

    Buffer *bufferPtr = NULL;
    AttFindByTypeValueReq *attFindByTypePtr = MEM_MALLOC.alloc(sizeof(AttFindByTypeValueReq));
    if (attFindByTypePtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attFindByTypePtr->attType = attFindByTypePtreve->attType;
    attFindByTypePtr->handleRange.startHandle = attFindByTypePtreve->handleRange.startHandle;
    attFindByTypePtr->handleRange.endHandle = attFindByTypePtreve->handleRange.endHandle;
    bufferPtr = BufferRefMalloc(attValue);
    FindByTypeValueRequestAsync *findByTypeAsyncPtr = MEM_MALLOC.alloc(sizeof(FindByTypeValueRequestAsync));
    if (findByTypeAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    findByTypeAsyncPtr->connectHandle = connectHandle;
    findByTypeAsyncPtr->attFindByTypePtreve = attFindByTypePtr;
    findByTypeAsyncPtr->attValue = bufferPtr;

    AttAsyncProcess(AttFindByTypeValueRequestAsync, AttFindByTypeValueRequestAsyncDestroy, findByTypeAsyncPtr);

    return;
}

/**
 * @brief read by type request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadByTypeRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    int ret;
    uint16_t attrTypelen = 0;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    AttConnectInfo *connect = NULL;
    ReadByTypeRequestAsync *readByTypeReqAsyncPtr = (ReadByTypeRequestAsync *)context;

    AttGetConnectInfoIndexByConnectHandle(readByTypeReqAsyncPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL goto ATT_READBYTYPEREQUEST_END", __FUNCTION__);
        goto ATT_READBYTYPEREQUEST_END;
    }

    if (readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid->type == BT_UUID_16) {
        LOG_INFO("%{public}s enter,uuid_type = %{public}d, uuid = %{public}d",
            __FUNCTION__,
            readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid->type,
            readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid->uuid16);
        attrTypelen = UUID16BITTYPELEN;
    } else if (readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid->type == BT_UUID_128) {
        attrTypelen = UUID128BITTYPELEN;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t) + attrTypelen);
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = READ_BY_TYPE_REQUEST;
    ((uint16_t *)(data + 1))[0] = readByTypeReqAsyncPtr->attReadByTypeReqContext.handleRange.startHandle;
    ((uint16_t *)(data + STEP_THREE))[0] = readByTypeReqAsyncPtr->attReadByTypeReqContext.handleRange.endHandle;
    if (readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid->type == BT_UUID_16) {
        ((uint16_t *)(data + STEP_FIVE))[0] = readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid->uuid16;
    } else if (readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid->type == BT_UUID_128) {
        (void)memcpy_s(
            data + STEP_FIVE, attrTypelen, readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid->uuid128, attrTypelen);
    }

    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATT_READBYTYPEREQUEST_END:
    MEM_MALLOC.free(readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid);
    MEM_MALLOC.free(readByTypeReqAsyncPtr);
    return;
}

/**
 * @brief destroy read by type request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadByTypeRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ReadByTypeRequestAsync *readByTypeReqAsyncPtr = (ReadByTypeRequestAsync *)context;

    MEM_MALLOC.free(readByTypeReqAsyncPtr->attReadByTypeReqContext.uuid);
    MEM_MALLOC.free(readByTypeReqAsyncPtr);

    return;
}

/**
 * @brief gatt send readbytype request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 startHandle Indicates the first requested handle number.
 * @param3 endHandle Indicates the last requested handle number.
 * @param4 uuid Indicates the pointer to const 2 or 16 octet UUID.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_ReadByTypeRequest(uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, const BtUuid *uuid)
{
    LOG_INFO("%{public}s enter,connectHandle = %hu,startHandle=%{public}d,endHandle=%{public}d",
        __FUNCTION__,
        connectHandle,
        startHandle,
        endHandle);

    BtUuid *attReadUuidPtr = MEM_MALLOC.alloc(sizeof(BtUuid));
    if (attReadUuidPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    if (uuid->type == BT_UUID_16) {
        attReadUuidPtr->type = BT_UUID_16;
        attReadUuidPtr->uuid16 = uuid->uuid16;
    } else if (uuid->type == BT_UUID_128) {
        attReadUuidPtr->type = BT_UUID_128;
        (void)memcpy_s(attReadUuidPtr->uuid128, UUID128LEN, uuid->uuid128, UUID128LEN);
    }

    ReadByTypeRequestAsync *attReadByTypeReqAsyncPtr = MEM_MALLOC.alloc(sizeof(ReadByTypeRequestAsync));
    if (attReadByTypeReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attReadByTypeReqAsyncPtr->connectHandle = connectHandle;
    attReadByTypeReqAsyncPtr->attReadByTypeReqContext.handleRange.startHandle = startHandle;
    attReadByTypeReqAsyncPtr->attReadByTypeReqContext.handleRange.endHandle = endHandle;
    attReadByTypeReqAsyncPtr->attReadByTypeReqContext.uuid = attReadUuidPtr;

    AttAsyncProcess(AttReadByTypeRequestAsync, AttReadByTypeRequestAsyncDestroy, attReadByTypeReqAsyncPtr);

    return;
}

/**
 * @brief read request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    Packet *packet = NULL;
    ReadRequestAsync *readReqAsyncPtr = (ReadRequestAsync *)context;
    uint8_t *data = NULL;
    AttConnectInfo *connect = NULL;
    uint16_t index = 0;

    AttGetConnectInfoIndexByConnectHandle(readReqAsyncPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATT_READREQUEST_END", __FUNCTION__);
        goto ATT_READREQUEST_END;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(readReqAsyncPtr->attHandle));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = READ_REQUEST;
    ((uint16_t *)(data + 1))[0] = readReqAsyncPtr->attHandle;
    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATT_READREQUEST_END:
    MEM_MALLOC.free(readReqAsyncPtr);

    return;
}

/**
 * @brief destroy read request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ReadRequestAsync *readReqAsyncPtr = (ReadRequestAsync *)context;

    MEM_MALLOC.free(readReqAsyncPtr);

    return;
}

/**
 * @brief gatt send read request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute to be read.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_ReadRequest(uint16_t connectHandle, uint16_t attHandle)
{
    LOG_INFO("%{public}s enter,connectHandle = %hu,attHandle=%{public}d", __FUNCTION__, connectHandle, attHandle);

    ReadRequestAsync *readReqAsyncPtr = MEM_MALLOC.alloc(sizeof(ReadRequestAsync));
    if (readReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    readReqAsyncPtr->connectHandle = connectHandle;
    readReqAsyncPtr->attHandle = attHandle;

    AttAsyncProcess(AttReadRequestAsync, AttReadRequestAsyncDestroy, readReqAsyncPtr);

    return;
}

/**
 * @brief read blob request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadBlobRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    int ret;
    uint8_t *data = NULL;
    AttConnectInfo *connect = NULL;
    Packet *packet = NULL;
    ReadBlobRequestAsync *readBlobReqAsyncPtr = (ReadBlobRequestAsync *)context;

    AttGetConnectInfoIndexByConnectHandle(readBlobReqAsyncPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATT_READBLOBREQUEST_END", __FUNCTION__);
        goto ATT_READBLOBREQUEST_END;
    }

    packet = PacketMalloc(0,
        0,
        sizeof(uint8_t) + sizeof(readBlobReqAsyncPtr->attReadBlobContext.attHandle) +
            sizeof(readBlobReqAsyncPtr->attReadBlobContext.offset));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = READ_BLOB_REQUEST;
    ((uint16_t *)(data + 1))[0] = readBlobReqAsyncPtr->attReadBlobContext.attHandle;
    ((uint16_t *)(data + STEP_THREE))[0] = readBlobReqAsyncPtr->attReadBlobContext.offset;

    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATT_READBLOBREQUEST_END:
    MEM_MALLOC.free(readBlobReqAsyncPtr);
    return;
}

/**
 * @brief destroy read blob request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadBlobRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ReadBlobRequestAsync *readBlobReqAsyncPtr = (ReadBlobRequestAsync *)context;

    MEM_MALLOC.free(readBlobReqAsyncPtr);

    return;
}

/**
 * @brief gatt send readblob request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute to be read.
 * @param3 offset Indicates the offset of the first octet to be read.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_ReadBlobRequest(uint16_t connectHandle, uint16_t attHandle, uint16_t offset)
{
    LOG_INFO("%{public}s enter,connectHandle = %hu,attHandle = %{public}d,offset=%{public}d", __FUNCTION__, connectHandle, attHandle, offset);

    ReadBlobRequestAsync *readBlobReqAsyncPtr = MEM_MALLOC.alloc(sizeof(ReadBlobRequestAsync));
    if (readBlobReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    readBlobReqAsyncPtr->connectHandle = connectHandle;
    readBlobReqAsyncPtr->attReadBlobContext.attHandle = attHandle;
    readBlobReqAsyncPtr->attReadBlobContext.offset = offset;

    AttAsyncProcess(AttReadBlobRequestAsync, AttReadBlobRequestAsyncDestroy, readBlobReqAsyncPtr);

    return;
}

/**
 * @brief read multiple request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadMultipleRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    int ret;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    AttConnectInfo *connect = NULL;
    ReadResponseAsync *readMultipleReqAsyncPtr = (ReadResponseAsync *)context;

    AttGetConnectInfoIndexByConnectHandle(readMultipleReqAsyncPtr->connectHandle, &index, &connect);

    if (BufferGetSize(readMultipleReqAsyncPtr->attValue) < STEP_FOUR) {
        LOG_WARN("%{public}s BufferGetSize(readMultipleReqAsyncPtr->attValue) < 4", __FUNCTION__);
        ClientCallbackBTBADPARAM(connect);
        goto ATTREADMULTIPLEREQUEST_END;
    }

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTREADMULTIPLEREQUEST_END", __FUNCTION__);
        goto ATTREADMULTIPLEREQUEST_END;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = READ_MULTIPLE_REQUEST;

    PacketPayloadAddLast(packet, readMultipleReqAsyncPtr->attValue);
    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATTREADMULTIPLEREQUEST_END:
    BufferFree(readMultipleReqAsyncPtr->attValue);
    MEM_MALLOC.free(readMultipleReqAsyncPtr);
    return;
}

/**
 * @brief destroy read multiple request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadMultipleRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ReadResponseAsync *readMultipleReqAsyncPtr = (ReadResponseAsync *)context;

    BufferFree(readMultipleReqAsyncPtr->attValue);
    MEM_MALLOC.free(readMultipleReqAsyncPtr);

    return;
}

/**
 * @brief gatt send readmultiple request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 handleList Indicates the pointer to a set of two or more attribute handles.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_ReadMultipleRequest(uint16_t connectHandle, const Buffer *handleList)
{
    LOG_INFO("%{public}s enter,connectHandle = %hu", __FUNCTION__, connectHandle);

    Buffer *bufferPtr = NULL;
    ReadResponseAsync *readMultipleReqAsyncPtr = NULL;

    bufferPtr = BufferRefMalloc(handleList);
    readMultipleReqAsyncPtr = MEM_MALLOC.alloc(sizeof(ReadResponseAsync));
    if (readMultipleReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    readMultipleReqAsyncPtr->connectHandle = connectHandle;
    readMultipleReqAsyncPtr->attValue = bufferPtr;

    AttAsyncProcess(AttReadMultipleRequestAsync, AttReadMultipleRequestAsyncDestroy, readMultipleReqAsyncPtr);

    return;
}

/**
 * @brief read by group type request async data assign..
 *
 * @param1 data Indicates the pointer to data.
 * @param2 attReadByGroupRequestAsyncPtr Indicates the pointer to ReadByGroupTypeRequesAsync.
 * @param3 attrGroupTypeLen Indicates the attrGroupTypeLen.
 */
static void AttReadByGroupTypeRequestDataAssign(
    uint8_t *data, const ReadByGroupTypeRequesAsync *attReadByGroupRequestAsyncPtr, uint16_t attrGroupTypeLen)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    data[0] = READ_BY_GROUP_TYPE_REQUEST;
    ((uint16_t *)(data + 1))[0] = attReadByGroupRequestAsyncPtr->attReadGroupContext.handleRange.startHandle;
    ((uint16_t *)(data + STEP_THREE))[0] = attReadByGroupRequestAsyncPtr->attReadGroupContext.handleRange.endHandle;

    if (attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid->type == BT_UUID_16) {
        if (memcpy_s(data + STEP_FIVE,
            attrGroupTypeLen,
            &(attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid->uuid16),
            attrGroupTypeLen) != EOK) {
                LOG_ERROR("%{public}s BT_UUID_16 memcpy_s fail", __FUNCTION__);
                return;
            }
    } else if (attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid->type == BT_UUID_128) {
        if (memcpy_s(data + STEP_FIVE,
            attrGroupTypeLen,
            attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid->uuid128,
            attrGroupTypeLen) != EOK) {
                LOG_ERROR("%{public}s BT_UUID_128 memcpy_s fail", __FUNCTION__);
                return;
            }
    }

    return;
}

/**
 * @brief read by group type request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadByGroupTypeRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    uint16_t attrGroupTypeLen = 0;
    AttConnectInfo *connect = NULL;
    int ret;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    ReadByGroupTypeRequesAsync *attReadByGroupRequestAsyncPtr = NULL;

    attReadByGroupRequestAsyncPtr = (ReadByGroupTypeRequesAsync *)context;

    AttGetConnectInfoIndexByConnectHandle(attReadByGroupRequestAsyncPtr->connectHandle, &index, &connect);
    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTREADBYGROUPTYPEREQUEST_END", __FUNCTION__);
        goto ATTREADBYGROUPTYPEREQUEST_END;
    }

    if (attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid->type == BT_UUID_16) {
        LOG_INFO("%{public}s enter,uuid_type = %{public}d, uuid = %{public}d",
            __FUNCTION__,
            attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid->type,
            attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid->uuid16);
        attrGroupTypeLen = UUID16BITTYPELEN;
    } else if (attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid->type == BT_UUID_128) {
        attrGroupTypeLen = UUID128BITTYPELEN;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t) + attrGroupTypeLen);
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    AttReadByGroupTypeRequestDataAssign(data, attReadByGroupRequestAsyncPtr, attrGroupTypeLen);

    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATTREADBYGROUPTYPEREQUEST_END:
    MEM_MALLOC.free(attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid);
    MEM_MALLOC.free(attReadByGroupRequestAsyncPtr);
    return;
}

/**
 * @brief destroy read by group type request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttReadByGroupTypeRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ReadByGroupTypeRequesAsync *attReadByGroupRequestAsyncPtr = (ReadByGroupTypeRequesAsync *)context;

    MEM_MALLOC.free(attReadByGroupRequestAsyncPtr->attReadGroupContext.uuid);
    MEM_MALLOC.free(attReadByGroupRequestAsyncPtr);

    return;
}

/**
 * @brief gatt send readbygrouptype request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 startHandle Indicates the first requested handle number.
 * @param3 endHandle Indicates the last requested handle number.
 * @param4 uuid Indicates the pointer to 2 or 16 octet UUID.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_ReadByGroupTypeRequest(uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, const BtUuid *uuid)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu, startHandle=%{public}d, endHandle=%{public}d",
        __FUNCTION__,
        connectHandle,
        startHandle,
        endHandle);

    ReadByGroupTypeRequesAsync *attReadByGroupReqAsyncPtr = NULL;
    BtUuid *attReadByGroupReqUuidPtr = NULL;

    attReadByGroupReqUuidPtr = MEM_MALLOC.alloc(sizeof(BtUuid));
    if (attReadByGroupReqUuidPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    if (uuid->type == BT_UUID_16) {
        attReadByGroupReqUuidPtr->type = BT_UUID_16;
        attReadByGroupReqUuidPtr->uuid16 = uuid->uuid16;
    } else if (uuid->type == BT_UUID_128) {
        attReadByGroupReqUuidPtr->type = BT_UUID_128;
        (void)memcpy_s(attReadByGroupReqUuidPtr->uuid128, UUID128LEN, uuid->uuid128, UUID128LEN);
    }

    attReadByGroupReqAsyncPtr = MEM_MALLOC.alloc(sizeof(ReadByGroupTypeRequesAsync));
    if (attReadByGroupReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attReadByGroupReqAsyncPtr->connectHandle = connectHandle;
    attReadByGroupReqAsyncPtr->attReadGroupContext.handleRange.startHandle = startHandle;
    attReadByGroupReqAsyncPtr->attReadGroupContext.handleRange.endHandle = endHandle;
    attReadByGroupReqAsyncPtr->attReadGroupContext.uuid = attReadByGroupReqUuidPtr;

    AttAsyncProcess(AttReadByGroupTypeRequestAsync, AttReadByGroupTypeRequestAsyncDestroy, attReadByGroupReqAsyncPtr);

    return;
}

/**
 * @brief write request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttWriteRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    uint16_t bufferSize;
    int ret;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    AttConnectInfo *connect = NULL;
    WriteAsync *writeReqAsyncPtr = NULL;

    writeReqAsyncPtr = (WriteAsync *)context;
    bufferSize = BufferGetSize(writeReqAsyncPtr->attValue);
    AttGetConnectInfoIndexByConnectHandle(writeReqAsyncPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTWRITEREQUEST_END", __FUNCTION__);
        goto ATTWRITEREQUEST_END;
    }

    if (bufferSize > (connect->mtu - STEP_THREE)) {
        LOG_WARN("%{public}s bufferSize > (mtu - 3)", __FUNCTION__);
        ClientCallbackBTBADPARAM(connect);
        goto ATTWRITEREQUEST_END;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(writeReqAsyncPtr->attHandle));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = WRITE_REQUEST;
    ((uint16_t *)(data + 1))[0] = writeReqAsyncPtr->attHandle;

    PacketPayloadAddLast(packet, writeReqAsyncPtr->attValue);
    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATTWRITEREQUEST_END:
    BufferFree(writeReqAsyncPtr->attValue);
    MEM_MALLOC.free(writeReqAsyncPtr);
    return;
}

/**
 * @brief destroy write request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttWriteRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    WriteAsync *writeReqAsyncPtr = (WriteAsync *)context;

    BufferFree(writeReqAsyncPtr->attValue);
    MEM_MALLOC.free(writeReqAsyncPtr);

    return;
}

/**
 * @brief gatt send write request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute to be written.
 * @param3 attValue Indicates the pointer to the value to be written to the attribute.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_WriteRequest(uint16_t connectHandle, uint16_t attHandle, const Buffer *attValue)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu, attHandle=%{public}d", __FUNCTION__, connectHandle, attHandle);

    Buffer *bufferPtr = NULL;
    WriteAsync *writeRequestAsyncPtr = NULL;

    bufferPtr = BufferRefMalloc(attValue);
    writeRequestAsyncPtr = MEM_MALLOC.alloc(sizeof(WriteAsync));
    if (writeRequestAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    writeRequestAsyncPtr->connectHandle = connectHandle;
    writeRequestAsyncPtr->attHandle = attHandle;
    writeRequestAsyncPtr->attValue = bufferPtr;

    AttAsyncProcess(AttWriteRequestAsync, AttWriteRequestAsyncDestroy, writeRequestAsyncPtr);

    return;
}

/**
 * @brief write command in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttWriteCommandAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    int ret;
    uint16_t bufferSize;
    AttConnectInfo *connect = NULL;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    WriteAsync *writeCommandAsyncPtr = NULL;

    writeCommandAsyncPtr = (WriteAsync *)context;
    bufferSize = BufferGetSize(writeCommandAsyncPtr->attValue);

    AttGetConnectInfoIndexByConnectHandle(writeCommandAsyncPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTWRITECOMMAND_END", __FUNCTION__);
        goto ATTWRITECOMMAND_END;
    }

    if (bufferSize > (connect->mtu - STEP_THREE)) {
        LOG_WARN("%{public}s bufferSize > (mtu - 3)", __FUNCTION__);
        ClientCallbackBTBADPARAM(connect);
        goto ATTWRITECOMMAND_END;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(writeCommandAsyncPtr->attHandle));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = WRITE_COMMAND;
    ((uint16_t *)(data + 1))[0] = writeCommandAsyncPtr->attHandle;

    PacketPayloadAddLast(packet, writeCommandAsyncPtr->attValue);
    ret = AttResponseSendData(connect, packet);
    ClientCallbackReturnValue(ret, connect);
    PacketFree(packet);

ATTWRITECOMMAND_END:
    BufferFree(writeCommandAsyncPtr->attValue);
    MEM_MALLOC.free(writeCommandAsyncPtr);
    return;
}

/**
 * @brief destroy write command in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttWriteCommandAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    WriteAsync *writeCommandAsyncPtr = (WriteAsync *)context;

    BufferFree(writeCommandAsyncPtr->attValue);
    MEM_MALLOC.free(writeCommandAsyncPtr);

    return;
}

/**
 * @brief gatt send write command to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the handle of the attribute to be set.
 * @param3 attValue Indicates the pointer to the value of be written to the attribute.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_WriteCommand(uint16_t connectHandle, uint16_t attHandle, const Buffer *attValue)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu, attHandle=%{public}d", __FUNCTION__, connectHandle, attHandle);

    Buffer *bufferPtr = NULL;
    WriteAsync *writeCommandAsyncPtr = NULL;

    bufferPtr = BufferRefMalloc(attValue);
    writeCommandAsyncPtr = MEM_MALLOC.alloc(sizeof(WriteAsync));
    if (writeCommandAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    writeCommandAsyncPtr->connectHandle = connectHandle;
    writeCommandAsyncPtr->attHandle = attHandle;
    writeCommandAsyncPtr->attValue = bufferPtr;

    AttAsyncProcess(AttWriteCommandAsync, AttWriteCommandAsyncDestroy, writeCommandAsyncPtr);

    return;
}

/**
 * @brief  signed write command error free.
 *
 * @param1 connect Indicates the pointer to AttConnectInfo.
 * @param2 ret Indicates the ret.
 * @param3 sigedWriteBuffPtr Indicates the pointer to Buffer.
 * @param4 packet Indicates the pointer to Packet.
 */
static void AttSignedWriteCommandErrorFree(
    const AttConnectInfo *connect, int ret, Buffer *sigedWriteBuffPtr, Packet *packet)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    g_attClientSendDataCB.attSendDataCB(connect->retGattConnectHandle, ret, g_attClientSendDataCB.context);
    BufferFree(sigedWriteBuffPtr);
    PacketFree(packet);

    return;
}

/**
 * @brief  signed write data assign.
 *
 * @param1 gapSignatureDataPtr Indicates the pointer to GapSignatureData.
 * @param2 data Indicates the pointer to data.
 * @param3 bufferSize Indicates the bufferSize.
 */
static void AttSignedWriteGapSignatureDataAssign(
    GapSignatureData *gapSignatureDataPtr, const uint8_t *data, uint16_t bufferSize)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    gapSignatureDataPtr->data = data;
    gapSignatureDataPtr->dataLen = bufferSize + STEP_THREE;

    return;
}

/**
 * @brief signed write command in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttSignedWriteCommandAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    int ret;
    GapSignatureData gapSignatureDataObj;
    Buffer *sigedWriteBuffPtr = NULL;
    uint8_t *attValuePtr = NULL;
    AttConnectInfo *connect = NULL;
    SigedWriteCommandGenerationContext *sigedWriteCommandGenerContextPtr = NULL;
    uint16_t bufferSize;
    WriteAsync *signedWriteCommandAsyncPtr = NULL;

    signedWriteCommandAsyncPtr = (WriteAsync *)context;
    bufferSize = BufferGetSize(signedWriteCommandAsyncPtr->attValue);

    AttGetConnectInfoIndexByConnectHandle(signedWriteCommandAsyncPtr->connectHandle, &index, &connect);
    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTSIGNEDWRITECOMMAND_END", __FUNCTION__);
        goto ATTSIGNEDWRITECOMMAND_END;
    }
    if (bufferSize > (connect->mtu - STEP_FIFTEEN)) {
        LOG_INFO("%{public}s bufferSize > (connect->mtu - 15) and goto ATTSIGNEDWRITECOMMAND_END", __FUNCTION__);
        ClientCallbackBTBADPARAM(connect);
        goto ATTSIGNEDWRITECOMMAND_END;
    }
    uint8_t signature[12] = {0};
    Packet *packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(uint16_t) + bufferSize + sizeof(signature));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    uint8_t *data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = SIGNED_WRITE_COMMAND;
    ((uint16_t *)(data + 1))[0] = signedWriteCommandAsyncPtr->attHandle;
    attValuePtr = (uint8_t *)BufferPtr(signedWriteCommandAsyncPtr->attValue);
    (void)memcpy_s(data + STEP_THREE, bufferSize, attValuePtr, bufferSize);
    AttSignedWriteGapSignatureDataAssign(&gapSignatureDataObj, data, bufferSize);
    sigedWriteBuffPtr = BufferMalloc(sizeof(SigedWriteCommandGenerationContext));
    sigedWriteCommandGenerContextPtr = (SigedWriteCommandGenerationContext *)BufferPtr(sigedWriteBuffPtr);
    AttSingedWriteCommandContextAssign(sigedWriteCommandGenerContextPtr, connect, data, packet, bufferSize);
    ret = GAPIF_LeDataSignatureGenerationAsync(
        &(connect->addr), gapSignatureDataObj, AttGapSignatureGenerationResult, sigedWriteBuffPtr);
    if (ret != BT_NO_ERROR) {
        if (g_attClientSendDataCB.attSendDataCB != NULL) {
            AttSignedWriteCommandErrorFree(connect, ret, sigedWriteBuffPtr, packet);
        }
    }

ATTSIGNEDWRITECOMMAND_END:
    BufferFree(signedWriteCommandAsyncPtr->attValue);
    MEM_MALLOC.free(signedWriteCommandAsyncPtr);
    return;
}

/**
 * @brief destroy signed write command in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttSignedWriteCommandAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    WriteAsync *signedWriteCommandAsyncPtr = (WriteAsync *)context;

    BufferFree(signedWriteCommandAsyncPtr->attValue);
    MEM_MALLOC.free(signedWriteCommandAsyncPtr);

    return;
}

/**
 * @brief gatt send signedwrite command to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attHandle Indicates the the handle of the attribute to be set.
 * @param3 attValue Indicates the pointer to the value to be written to the attribute.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_SignedWriteCommand(uint16_t connectHandle, uint16_t attHandle, const Buffer *attValue)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu, attHandle=%{public}d", __FUNCTION__, connectHandle, attHandle);

    Buffer *bufferPtr = NULL;
    WriteAsync *signedWriteCommandAsyncPtr = NULL;

    bufferPtr = BufferRefMalloc(attValue);
    signedWriteCommandAsyncPtr = MEM_MALLOC.alloc(sizeof(WriteAsync));
    if (signedWriteCommandAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    signedWriteCommandAsyncPtr->connectHandle = connectHandle;
    signedWriteCommandAsyncPtr->attHandle = attHandle;
    signedWriteCommandAsyncPtr->attValue = bufferPtr;

    AttAsyncProcess(AttSignedWriteCommandAsync, AttSignedWriteCommandAsyncDestroy, signedWriteCommandAsyncPtr);

    return;
}

/**
 * @brief prepare write request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttPrepareWriteRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    int ret;
    uint16_t bufferSize;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    AttConnectInfo *connect = NULL;
    PrepareWriteAsync *prepareWriteReqAsyncPtr = NULL;

    prepareWriteReqAsyncPtr = (PrepareWriteAsync *)context;
    bufferSize = BufferGetSize(prepareWriteReqAsyncPtr->attValue);

    AttGetConnectInfoIndexByConnectHandle(prepareWriteReqAsyncPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTPREPAREWRITEREQUEST_END", __FUNCTION__);
        goto ATTPREPAREWRITEREQUEST_END;
    }

    if (bufferSize > (connect->mtu - STEP_FIVE)) {
        LOG_INFO("%{public}s bufferSize > (connect->mtu - 5)", __FUNCTION__);
        ClientCallbackBTBADPARAM(connect);
        goto ATTPREPAREWRITEREQUEST_END;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = PREPARE_WRITE_REQUEST;
    ((uint16_t *)(data + 1))[0] = prepareWriteReqAsyncPtr->attReadBlobObj.attHandle;
    ((uint16_t *)(data + STEP_THREE))[0] = prepareWriteReqAsyncPtr->attReadBlobObj.offset;

    PacketPayloadAddLast(packet, prepareWriteReqAsyncPtr->attValue);
    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATTPREPAREWRITEREQUEST_END:
    BufferFree(prepareWriteReqAsyncPtr->attValue);
    MEM_MALLOC.free(prepareWriteReqAsyncPtr);
    return;
}

/**
 * @brief destroy prepare write request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttPrepareWriteRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    PrepareWriteAsync *prepareWriteReqAsyncPtr = (PrepareWriteAsync *)context;

    BufferFree(prepareWriteReqAsyncPtr->attValue);
    MEM_MALLOC.free(prepareWriteReqAsyncPtr);

    return;
}

/**
 * @brief gatt send preparewrite request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 attReadBlobObj Indicates the value of the struct AttReadBlobReqPrepareWriteValue.
 * @param3 attValue Indicates the pointer to the value of the attribute to be written.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_PrepareWriteRequest(
    uint16_t connectHandle, AttReadBlobReqPrepareWriteValue attReadBlobObj, const Buffer *attValue)
{
    LOG_INFO("%{public}s enter,connectHandle = %hu, attHandle=%{public}d, offset=%{public}d",
        __FUNCTION__,
        connectHandle,
        attReadBlobObj.attHandle,
        attReadBlobObj.offset);

    Buffer *bufferPtr = NULL;
    PrepareWriteAsync *prepareWriteReqAsyncPtr = NULL;

    bufferPtr = BufferRefMalloc(attValue);
    prepareWriteReqAsyncPtr = MEM_MALLOC.alloc(sizeof(PrepareWriteAsync));
    if (prepareWriteReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    prepareWriteReqAsyncPtr->connectHandle = connectHandle;
    prepareWriteReqAsyncPtr->attReadBlobObj.attHandle = attReadBlobObj.attHandle;
    prepareWriteReqAsyncPtr->attReadBlobObj.offset = attReadBlobObj.offset;
    prepareWriteReqAsyncPtr->attValue = bufferPtr;

    AttAsyncProcess(AttPrepareWriteRequestAsync, AttPrepareWriteRequestAsyncDestroy, prepareWriteReqAsyncPtr);

    return;
}

/**
 * @brief execute write request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttExecuteWriteRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    AttConnectInfo *connect = NULL;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    int ret;
    ExecuteWriteRequestAsync *executeWriteReqAsyncPtr = (ExecuteWriteRequestAsync *)context;

    AttGetConnectInfoIndexByConnectHandle(executeWriteReqAsyncPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATT_EXECUTEWRITEREQUEST_END", __FUNCTION__);
        goto ATT_EXECUTEWRITEREQUEST_END;
    }

    if ((executeWriteReqAsyncPtr->flag != IMMEDIATELY_WRITE_ALL_PENDING_PREPARED_VALUES) &&
        (executeWriteReqAsyncPtr->flag != CANCEL_ALL_PREPARED_WRITES)) {
        LOG_WARN("%{public}s:flag != IMMEDIATELY_WRITE_ALL_PENDING_PREPARED_VALUES", __FUNCTION__);
        ClientCallbackBTBADPARAM(connect);
        goto ATT_EXECUTEWRITEREQUEST_END;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t) + sizeof(executeWriteReqAsyncPtr->flag));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = EXECUTE_WRITE_REQUEST;
    data[1] = executeWriteReqAsyncPtr->flag;

    ListAddLast(connect->instruct, packet);
    ret = AttSendSequenceScheduling(connect);
    ClientCallbackReturnValue(ret, connect);

ATT_EXECUTEWRITEREQUEST_END:
    MEM_MALLOC.free(executeWriteReqAsyncPtr);
    return;
}

/**
 * @brief destroy execute write request in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttExecuteWriteRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ExecuteWriteRequestAsync *executeWriteReqAsyncPtr = (ExecuteWriteRequestAsync *)context;

    MEM_MALLOC.free(executeWriteReqAsyncPtr);

    return;
}

/**
 * @brief gatt send execute write request to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 flag Indicates the value of flags.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_ExecuteWriteRequest(uint16_t connectHandle, uint8_t flag)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu, flag=%{public}d", __FUNCTION__, connectHandle, flag);

    ExecuteWriteRequestAsync *executeWriteReqAsyncPtr = MEM_MALLOC.alloc(sizeof(ExecuteWriteRequestAsync));
    if (executeWriteReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    executeWriteReqAsyncPtr->connectHandle = connectHandle;
    executeWriteReqAsyncPtr->flag = flag;

    AttAsyncProcess(AttExecuteWriteRequestAsync, AttExecuteWriteRequestAsyncDestroy, executeWriteReqAsyncPtr);

    return;
}

/**
 * @brief handle value confirmation in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttHandleValueConfirmationAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectInfo *connect = NULL;
    uint16_t index = 0;
    int ret;
    Packet *packet = NULL;
    uint8_t *data = NULL;
    WriteResponseAsync *handleConfirmationAsyncPtr = NULL;

    handleConfirmationAsyncPtr = (WriteResponseAsync *)context;

    AttGetConnectInfoIndexByConnectHandle(handleConfirmationAsyncPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATT_HANDLEVALUECONFIRMATION_END", __FUNCTION__);
        goto ATT_HANDLEVALUECONFIRMATION_END;
    }

    packet = PacketMalloc(0, 0, sizeof(uint8_t));
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    data = BufferPtr(PacketContinuousPayload(packet));
    data[0] = HANDLE_VALUE_CONFIRMATION;

    ret = AttResponseSendData(connect, packet);
    ClientCallbackReturnValue(ret, connect);
    PacketFree(packet);

ATT_HANDLEVALUECONFIRMATION_END:
    MEM_MALLOC.free(handleConfirmationAsyncPtr);
    return;
}

/**
 * @brief destroy handle value confirmation in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttHandleValueConfirmationAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    WriteResponseAsync *handleConfirmationAsyncPtr = (WriteResponseAsync *)context;

    MEM_MALLOC.free(handleConfirmationAsyncPtr);

    return;
}

/**
 * @brief gatt send handlevalue confirmation to att.
 *
 * @param connectHandle Indicates the connect handle.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
void ATT_HandleValueConfirmation(uint16_t connectHandle)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu", __FUNCTION__, connectHandle);

    WriteResponseAsync *handleConfirmationAsyncPtr = MEM_MALLOC.alloc(sizeof(WriteResponseAsync));
    if (handleConfirmationAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    handleConfirmationAsyncPtr->connectHandle = connectHandle;

    AttAsyncProcess(
        AttHandleValueConfirmationAsync, AttHandleValueConfirmationAsyncDestroy, handleConfirmationAsyncPtr);

    return;
}

/**
 * @brief client send data register..
 *
 * @param context Indicates the pointer to context.
 */
void AttClientSendDataRegisterAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttClientSendDataCallback *attClientSendDataCallbackPtr = (AttClientSendDataCallback *)context;

    g_attClientSendDataCB.attSendDataCB = attClientSendDataCallbackPtr->attSendDataCB;
    g_attClientSendDataCB.context = attClientSendDataCallbackPtr->context;

    AttClientCallBackCopyToCommon(attClientSendDataCallbackPtr->attSendDataCB, attClientSendDataCallbackPtr->context);

    MEM_MALLOC.free(attClientSendDataCallbackPtr);

    return;
}

/**
 * @brief client send data register.destroy.
 *
 * @param context Indicates the pointer to context.
 */
void AttClientSendDataRegisterAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttClientSendDataCallback *attClientSendDataCallbackPtr = (AttClientSendDataCallback *)context;

    MEM_MALLOC.free(attClientSendDataCallbackPtr);

    return;
}

/**
 * @brief client gatt send data register.
 *
 * @param1 attSendDataCB Indicates the pointer of attSendDataCallback.
 * @param2 context Indicates the pointer of context.
 */
void ATT_ClientSendDataRegister(attSendDataCallback attSendDataCB, void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttClientSendDataCallback *attClientSendDataCallbackPtr = MEM_MALLOC.alloc(sizeof(AttClientSendDataCallback));
    if (attClientSendDataCallbackPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attClientSendDataCallbackPtr->attSendDataCB = attSendDataCB;
    attClientSendDataCallbackPtr->context = context;

    AttAsyncProcess(
        AttClientSendDataRegisterAsync, AttClientSendDataRegisterAsyncDestroy, attClientSendDataCallbackPtr);

    return;
}

/**
 * @brief client send data deregister.
 *
 * @param context Indicates the pointer to context.
 */
void AttClientSendDataDeRegisterAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    g_attClientSendDataCB.attSendDataCB = NULL;
    g_attClientSendDataCB.context = NULL;

    AttCallBackDelectCopyToCommon();

    return;
}

/**
 * @brief client send data deregister.destroy.
 *
 * @param context Indicates the pointer to context.
 */
void AttClientSendDataDeRegisterAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return;
}

/**
 * @brief client gatt send data deregister.
 *
 */
void ATT_ClientSendDataDeRegister()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttAsyncProcess(AttClientSendDataDeRegisterAsync, AttClientSendDataDeRegisterAsyncDestroy, NULL);

    return;
}

/**
 * @brief find information response data assign.
 *
 * @param1 sigedWriteCommandGenerContextPtr Indicates the pointer of SigedWriteCommandGenerationContext.
 * @param2 connect Indicates the pointer of AttConnectInfo.
 * @param3 data Indicates the pointer of uint8_t.
 * @param4 packet Indicates the pointer of Packet.
 * @param5 bufferSize Indicates the buffer Size.
 */
static void AttSingedWriteCommandContextAssign(SigedWriteCommandGenerationContext *sigedWriteCommandGenerContextPtr,
    AttConnectInfo *connect, uint8_t *data, Packet *packet, uint16_t bufferSize)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    sigedWriteCommandGenerContextPtr->connect = connect;
    sigedWriteCommandGenerContextPtr->data = data;
    sigedWriteCommandGenerContextPtr->packet = packet;
    sigedWriteCommandGenerContextPtr->bufferSize = bufferSize;

    return;
}
