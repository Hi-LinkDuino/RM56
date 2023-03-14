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

/**
 * @file att_receive.c
 *
 * @brief implement receive function to be called.
 *
 */

#include "att_receive.h"

#include <memory.h>

#include "buffer.h"
#include "list.h"
#include "packet.h"

#include "gap_le_if.h"

#include "platform/include/allocator.h"

#include "log.h"

typedef struct SigedWriteCommandConfirmationContext {
    AttConnectInfo *connect;
    Buffer *bufferallPtr;
    uint8_t *data;
    uint16_t bufferSize;
} SigedWriteCommandConfirmationContext;

typedef struct AttGapSignatureConfirmationAsync {
    GAP_SignatureResult result;
    void *context;
} AttGapSignatureConfirmationAsync;

static void AttCount(uint8_t format, uint16_t *inforLenPtr, uint16_t *uuidLenPtr);
static int AttJudgeInfoLen(uint16_t inforLen, size_t dataLen, uint16_t *inforNum);
static void AttFindInformationResAssign(uint16_t inforNum, AttFind *attFindObj, const uint8_t *data, uint16_t inforLen);
static void AttReadFree(AttRead *valueList, uint16_t num, uint8_t type);
static void AttReadAttrAssign(const AttRead *attReadPtr, uint8_t len, const uint8_t *data);
static void AttSignWriteCommConfContextAssign(SigedWriteCommandConfirmationContext *sigedWriteCommandConfirmContextPtr,
    const AttConnectInfo *connect, const uint8_t *data, const Buffer *bufferallPtr, size_t buffSize);
static void AttSignWriteCommConfDataAssign(
    uint8_t *data, const uint8_t *dataBuffer, size_t buffSize, uint8_t signature[12]);
static void AttSignedWriteCommandBufferFree(Buffer *bufferNew, Buffer *sigedWriteBuffPtr, Buffer *bufferallPtr);
static void AttReadByTypeResErrorAssign(AttError *attErrorObjPtr, const AttConnectInfo *connect);
static void AttGapSignatureConfirmationResultAsync(const void *context);
static void AttGapSignatureConfirmationResultAsyncDestroy(const void *context);
static void FindInformationResponseErrorAssign(AttError *attErrorObjPtr, const AttConnectInfo *connect);
static void FindByTypeValueResponseErrorAssign(AttError *attErrorObjPtr, const AttConnectInfo *connect);
static void ReadByGroupTypeResponseErrorAssign(AttError *attErrorObjPtr, const AttConnectInfo *connect);
static void AttSignedWriteCommandGapRetErrorAssign(AttWrite *attWriteObjPtr, const uint8_t *data);

/**
 * @brief gap security confirmation result async.
 *
 * @param context Indicates the pointer to context.
 */
static void AttGapSignatureConfirmationResultAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (context == NULL) {
        LOG_WARN("%{public}s:context == NULL, return.", __FUNCTION__);
        return;
    }

    AttGapSignatureConfirmationAsync *attGapSigConfirmationPtr = (AttGapSignatureConfirmationAsync *)context;
    Buffer *bufferSig = (Buffer *)(attGapSigConfirmationPtr->context);
    AttWrite attWriteObj;
    Buffer *bufferNew = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;
    SigedWriteCommandConfirmationContext *sigedWriteCommandConfirmContextPtr = NULL;

    if (bufferSig == NULL) {
        goto GAPSIGNATURECONFIRMATIONRESULT_END;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto GAPSIGNATURECONFIRMATIONRESULT_END;
    }

    sigedWriteCommandConfirmContextPtr = (SigedWriteCommandConfirmationContext *)BufferPtr(bufferSig);
    attWriteObj.signedWriteCommand.attHandleValueObj.attHandle =
        ((uint16_t *)(sigedWriteCommandConfirmContextPtr->data + 1))[0];
    (void)memcpy_s(attWriteObj.signedWriteCommand.authSignature,
        GAPSIGNATURESIZE,
        sigedWriteCommandConfirmContextPtr->data + STEP_TWO + sigedWriteCommandConfirmContextPtr->bufferSize,
        GAPSIGNATURESIZE);
    attWriteObj.signedWriteCommand.authSignatureLen = GAPSIGNATURESIZE;
    attWriteObj.signedWriteCommand.result = (AttSignedWriteCommandResult)attGapSigConfirmationPtr->result;
    bufferNew = BufferSliceMalloc(
        sigedWriteCommandConfirmContextPtr->bufferallPtr, STEP_THREE, sigedWriteCommandConfirmContextPtr->bufferSize);
    attServerDataCallback->attServerCallback(sigedWriteCommandConfirmContextPtr->connect->retGattConnectHandle,
        ATT_SIGNED_WRITE_COMMAND_ID,
        &attWriteObj,
        bufferNew,
        attServerDataCallback->context);

    BufferFree(bufferNew);
    BufferFree(sigedWriteCommandConfirmContextPtr->bufferallPtr);
    BufferFree(bufferSig);
GAPSIGNATURECONFIRMATIONRESULT_END:
    MEM_MALLOC.free(attGapSigConfirmationPtr);
    return;
}

/**
 * @brief gap security confirmation result async destroy.
 *
 * @param context Indicates the pointer to context.
 */
static void AttGapSignatureConfirmationResultAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (context == NULL) {
        LOG_WARN("%{public}s:context == NULL, return.", __FUNCTION__);
        return;
    }

    AttGapSignatureConfirmationAsync *attGapSigConfirmationPtr = (AttGapSignatureConfirmationAsync *)context;
    MEM_MALLOC.free(attGapSigConfirmationPtr);
    return;
}

/**
 * @brief gap security confirmation result.
 *
 * @param result Indicates the GAP_SignatureResult.
 * @param context Indicates the pointer to context.
 */
void AttGapSignatureConfirmationResult(GAP_SignatureResult result, void *context)
{
    LOG_INFO("%{public}s enter, result = %{public}d", __FUNCTION__, result);

    AttGapSignatureConfirmationAsync *attGapSigConfirmationPtr =
        MEM_MALLOC.alloc(sizeof(AttGapSignatureConfirmationAsync));
    if (attGapSigConfirmationPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attGapSigConfirmationPtr->result = result;
    attGapSigConfirmationPtr->context = context;

    AttAsyncProcess(AttGapSignatureConfirmationResultAsync,
        AttGapSignatureConfirmationResultAsyncDestroy,
        attGapSigConfirmationPtr);

    return;
}

/**
 * @brief count.
 *
 * @param1 format Indicates the format.
 * @param2 inforLenPtr Indicates the pointer to inforLenPtr.
 * @param3 uuidLenPtr Indicates the pointer to uuidLenPtr.
 */
static void AttCount(uint8_t format, uint16_t *inforLenPtr, uint16_t *uuidLenPtr)
{
    if (inforLenPtr == NULL || uuidLenPtr == NULL) {
        LOG_WARN("%{public}s:inforLenPtr or uuidLenPtr is null, return.", __FUNCTION__);
        return;
    }
    LOG_INFO("%{public}s enter,format = %hhu,inforLenPtr = %hu,uuidLenPtr = %hu",
        __FUNCTION__, format, *inforLenPtr, *uuidLenPtr);

    if (format == HANDLEAND16BITBLUETOOTHUUID) {
        *inforLenPtr = FINDINFORRESINFOR16BITLEN;
        *uuidLenPtr = UUID16BITTYPELEN;
    } else if (format == HANDLEAND128BITUUID) {
        *inforLenPtr = FINDINFORRESINFOR128BITLEN;
        *uuidLenPtr = UUID128BITTYPELEN;
    }
    return;
}

/**
 * @brief judge information len.
 *
 * @param1 inforLen Indicates the inforLen.
 * @param2 dataLen Indicates the dataLen.
 * @param3 inforNum Indicates the pointer to inforNum.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
static int AttJudgeInfoLen(uint16_t inforLen, size_t dataLen, uint16_t *inforNum)
{
    if (inforNum == NULL) {
        LOG_WARN("%{public}s:inforNum is null, return.", __FUNCTION__);
        return BT_OPERATION_FAILED;
    }
    LOG_INFO("%{public}s enter,inforLen = %hu,dataLen = %zu,inforNum = %hu ",
        __FUNCTION__, inforLen, dataLen, *inforNum);

    int ret = BT_OPERATION_FAILED;

    if (inforLen != 0) {
        if ((dataLen - 1) % inforLen) {
            ret = BT_OPERATION_FAILED;
            goto ATTJUDGEINFOLEN_END;
        } else {
            *inforNum = (dataLen - 1) / inforLen;
        }
    } else {
        ret = BT_OPERATION_FAILED;
        goto ATTJUDGEINFOLEN_END;
    }

    ret = BT_NO_ERROR;

ATTJUDGEINFOLEN_END:
    return ret;
}

/**
 * @brief assign find information response.
 *
 * @param1 inforNum Indicates the inforNum.
 * @param2 attFindObj Indicates the pointer to AttFind.
 * @param3 data Indicates the pointer to data.
 * @param4 inforLen Indicates the information len.
 */
static void AttFindInformationResAssign(uint16_t inforNum, AttFind *attFindObj, const uint8_t *data, uint16_t inforLen)
{
    LOG_INFO("%{public}s enter,inforNum = %hu, inforLen = %hu", __FUNCTION__, inforNum, inforLen);
    if (attFindObj == NULL || data == NULL) {
        LOG_WARN("%{public}s:attFindObj or data is null, return.", __FUNCTION__);
        return;
    }

    uint16_t index = 0;
    for (; index < inforNum; ++index) {
        attFindObj->findInforRsponse.handleUuidPairs[index].attHandle = ((uint16_t *)(data + 1 + index * inforLen))[0];
        if (attFindObj->findInforRsponse.format == HANDLEAND16BITBLUETOOTHUUID) {
            attFindObj->findInforRsponse.handleUuidPairs[index].uuid.type = BT_UUID_16;
            attFindObj->findInforRsponse.handleUuidPairs[index].uuid.uuid16 =
                ((uint16_t *)(data + STEP_THREE + index * inforLen))[0];
        } else if (attFindObj->findInforRsponse.format == HANDLEAND128BITUUID) {
            attFindObj->findInforRsponse.handleUuidPairs[index].uuid.type = BT_UUID_128;
            if (memcpy_s(&(attFindObj->findInforRsponse.handleUuidPairs[index].uuid.uuid128),
                UUID128LEN,
                data + STEP_THREE + index * inforLen,
                UUID128LEN) != EOK) {
                    LOG_ERROR("%{public}s enter, memcpy_s fail", __FUNCTION__);
                    return;
                }
        }
    }
    return;
}

/**
 * @brief free read data memory.
 *
 * @param1 valueList Indicates the pointer to AttRead.
 * @param2 num Indicates the num.
 * @param3 type Indicates the type.
 */
static void AttReadFree(AttRead *valueList, uint16_t num, uint8_t type)
{
    LOG_INFO("%{public}s enter,num = %{public}d, type = %{public}d", __FUNCTION__, num, type);
    if (valueList == NULL) {
        LOG_WARN("%{public}s:valueList is null, return.", __FUNCTION__);
        return;
    }

    uint16_t index = 0;
    for (; index < num; ++index) {
        if (type == READBYTYPERESPONSEFREE) {
            MEM_MALLOC.free(valueList->readHandleListNum.valueList[index].attributeValue);
        } else if (type == READBYGROUPTYPERESPONSEFREE) {
            MEM_MALLOC.free(valueList->readGroupResponse.attributeData[index].attributeValue);
        }
    }

    if (type == READBYTYPERESPONSEFREE) {
        MEM_MALLOC.free(valueList->readHandleListNum.valueList);
    } else if (type == READBYGROUPTYPERESPONSEFREE) {
        MEM_MALLOC.free(valueList->readGroupResponse.attributeData);
    }
    return;
}

/**
 * @brief read data attribute assign.
 *
 * @param1 attReadPtr Indicates the pointer to AttRead.
 * @param2 len Indicates the len.
 * @param3 data Indicates the pointer to data.
 */
static void AttReadAttrAssign(const AttRead *attReadPtr, uint8_t len, const uint8_t *data)
{
    LOG_INFO("%{public}s enter,len = %hhu", __FUNCTION__, len);
    if (attReadPtr == NULL || data == NULL) {
        LOG_WARN("%{public}s:attReadPtr or data is null, return.", __FUNCTION__);
        return;
    }

    uint16_t indexNum = 0;
    for (; indexNum < attReadPtr->readGroupResponse.num; ++indexNum) {
        attReadPtr->readGroupResponse.attributeData[indexNum].attributeValue = MEM_MALLOC.alloc(len - STEP_FOUR);
        attReadPtr->readGroupResponse.attributeData[indexNum].attHandle = ((uint16_t *)data)[0];
        attReadPtr->readGroupResponse.attributeData[indexNum].groupEndHandle = ((uint16_t *)(data + STEP_TWO))[0];
        (void)memcpy_s(attReadPtr->readGroupResponse.attributeData[indexNum].attributeValue,
            len - STEP_FOUR,
            data + STEP_FOUR,
            len - STEP_FOUR);
        data += len;
    }
    return;
}

/**
 * @brief received error response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttErrorResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL, return.", __FUNCTION__);
        return;
    }

    AttError attErrorTObj;
    uint8_t *data = NULL;
    AttClientDataCallback *attClientDataCallback = NULL;

    AlarmCancel(connect->alarm);

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTERRORRESPONSE_END;
    }

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTERRORRESPONSE_END;
    }

    data = (uint8_t *)BufferPtr(buffer);
    attErrorTObj.reqOpcode = data[0];
    attErrorTObj.attHandleInError = ((uint16_t *)(data + 1))[0];
    attErrorTObj.errorCode = data[STEP_THREE];
    LOG_INFO("%{public}s connectHandle = %hu, callback para : reqOpcode = %{public}d, attHandleInError = %{public}d, errorCode = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attErrorTObj.reqOpcode,
        attErrorTObj.attHandleInError,
        attErrorTObj.errorCode);

    attClientDataCallback->attClientCallback(
        connect->retGattConnectHandle, ATT_ERROR_RESPONSE_ID, &attErrorTObj, NULL, attClientDataCallback->context);

ATTERRORRESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received exchangeMTU request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttExchangeMTURequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL, return.", __FUNCTION__);
        return;
    }

    AttExchangeMTUType attExchangeMTUObj;
    uint8_t *data = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTEXCHANGEMTUREQUEST_END;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTEXCHANGEMTUREQUEST_END;
    }

    data = (uint8_t *)BufferPtr(buffer);
    attExchangeMTUObj.mtuSize = ((uint16_t *)data)[0];
    connect->receiveMtu = attExchangeMTUObj.mtuSize;
    LOG_INFO("%{public}s connectHandle = %hu, callback para : mtuSize = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attExchangeMTUObj.mtuSize);

    attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
        ATT_EXCHANGE_MTU_REQUEST_ID,
        &attExchangeMTUObj,
        NULL,
        attServerDataCallback->context);

ATTEXCHANGEMTUREQUEST_END:
    return;
}

/**
 * @brief received exchangeMTU response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttExchangeMTUResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL, return.", __FUNCTION__);
        return;
    }

    AttExchangeMTUType attExchangeMTUObj;
    uint16_t *data = NULL;
    AttClientDataCallback *attClientDataCallback = NULL;

    AlarmCancel(connect->alarm);

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTEXCHANGEMTURESPONSE_END;
    }

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTEXCHANGEMTURESPONSE_END;
    }

    data = (uint16_t *)BufferPtr(buffer);
    attExchangeMTUObj.mtuSize = data[0];
    connect->mtu = Min(connect->sendMtu, attExchangeMTUObj.mtuSize);
    LOG_INFO("%{public}s connectHandle = %hu,callback para : mtuSize = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attExchangeMTUObj.mtuSize);

    attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
        ATT_EXCHANGE_MTU_RESPONSE_ID,
        &attExchangeMTUObj,
        NULL,
        attClientDataCallback->context);

ATTEXCHANGEMTURESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received findinformation request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttFindInformationRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL, return.", __FUNCTION__);
        return;
    }

    AttFind attFindObj;
    uint8_t *data = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTFINDINFORMATIONREQUEST_END;
    }

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        ((AttConnectInfo *)connect)->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTFINDINFORMATIONREQUEST_END;
    }

    data = (uint8_t *)BufferPtr(buffer);
    attFindObj.findInformationRequest.startHandle = ((uint16_t *)data)[0];
    attFindObj.findInformationRequest.endHandle = ((uint16_t *)(data + STEP_TWO))[0];
    LOG_INFO("%{public}s connectHandle = %hu, callback para : startHandle = %{public}d, endHandle = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attFindObj.findInformationRequest.startHandle,
        attFindObj.findInformationRequest.endHandle);
    attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
        ATT_FIND_INFORMATION_REQUEST_ID,
        &attFindObj,
        NULL,
        attServerDataCallback->context);

ATTFINDINFORMATIONREQUEST_END:
    return;
}

/**
 * @brief received findinformation response error assign.
 *
 * @param1 attErrorObjPtr Indicates the pointer to AttError.
 * @param2 connect Indicates the pointer to AttConnectInfo.
 */
static void FindInformationResponseErrorAssign(AttError *attErrorObjPtr, const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (attErrorObjPtr == NULL || connect == NULL) {
        LOG_WARN("%{public}s:attErrorObjPtr or connect is NULL, return.", __FUNCTION__);
        return;
    }

    attErrorObjPtr->reqOpcode = FIND_INFORMATION_REQUEST;
    attErrorObjPtr->errorCode = ATT_INVALID_ATTRIBUTE_VALUE_LENGTH;
    attErrorObjPtr->attHandleInError = connect->aclHandle;
    return;
}

/**
 * @brief received findinformation response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttFindInformationResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    uint16_t inforLen = 0;
    uint16_t inforNum = 0;
    uint16_t uuidLen = 0;
    AttFind attFindObj;
    size_t dataLen;
    uint8_t *data = NULL;
    AttError attErrorObj;
    AttClientDataCallback *attClientDataCallback = NULL;

    AlarmCancel(connect->alarm);

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL and goto ATTFINDINFORMATIONRESPONSE_END", __FUNCTION__);
        goto ATTFINDINFORMATIONRESPONSE_END;
    }

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTFINDINFORMATIONRESPONSE_END;
    }
    dataLen = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    attFindObj.findInforRsponse.format = data[0];
    AttCount(attFindObj.findInforRsponse.format, &inforLen, &uuidLen);

    if (AttJudgeInfoLen(inforLen, dataLen, &inforNum) < 0) {
        FindInformationResponseErrorAssign(&attErrorObj, connect);
        attClientDataCallback->attClientCallback(
            connect->retGattConnectHandle, ATT_ERROR_RESPONSE_ID, &attErrorObj, NULL, attClientDataCallback->context);
        goto ATTFINDINFORMATIONRESPONSE_END;
    }

    attFindObj.findInforRsponse.handleUuidPairs = MEM_MALLOC.alloc(sizeof(AttHandleUuid) * inforNum);
    attFindObj.findInforRsponse.pairNum = inforNum;
    AttFindInformationResAssign(inforNum, &attFindObj, data, inforLen);
    attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
        ATT_FIND_INFORMATION_RESPONSE_ID,
        &attFindObj,
        NULL,
        attClientDataCallback->context);

    MEM_MALLOC.free(attFindObj.findInforRsponse.handleUuidPairs);

ATTFINDINFORMATIONRESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received findbytypevalue request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttFindByTypeValueRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    AttFind attFindObj;
    Buffer *bufferNew = NULL;
    uint8_t *data = NULL;
    size_t buffSize;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL and goto ATTFINDBYTYPEVALUEREQUEST_END", __FUNCTION__);
        goto ATTFINDBYTYPEVALUEREQUEST_END;
    }

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTFINDBYTYPEVALUEREQUEST_END;
    }

    buffSize = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    attFindObj.findByTypeValueRequest.handleRange.startHandle = ((uint16_t *)data)[0];
    attFindObj.findByTypeValueRequest.handleRange.endHandle = ((uint16_t *)(data + STEP_TWO))[0];
    attFindObj.findByTypeValueRequest.attType = ((uint16_t *)(data + STEP_FOUR))[0];

    if (buffSize - STEP_SIX > 0) {
        bufferNew = BufferSliceMalloc(buffer, STEP_SIX, buffSize - STEP_SIX);
        attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
            ATT_FIND_BY_TYPE_VALUE_REQUEST_ID,
            &attFindObj,
            bufferNew,
            attServerDataCallback->context);
        BufferFree(bufferNew);
    } else {
        attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
            ATT_FIND_BY_TYPE_VALUE_REQUEST_ID,
            &attFindObj,
            NULL,
            attServerDataCallback->context);
    }

ATTFINDBYTYPEVALUEREQUEST_END:
    return;
}

/**
 * @brief received findbytypevalue response error assign.
 *
 * @param1 attErrorObjPtr Indicates the pointer to AttError.
 * @param2 connect Indicates the pointer to AttConnectInfo.
 */
static void FindByTypeValueResponseErrorAssign(AttError *attErrorObjPtr, const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (attErrorObjPtr == NULL || connect == NULL) {
        LOG_WARN("%{public}s:attErrorObjPtr or connect is NULL, return.", __FUNCTION__);
        return;
    }

    attErrorObjPtr->reqOpcode = FIND_BY_TYPE_VALUE_REQUEST;
    attErrorObjPtr->errorCode = ATT_INVALID_ATTRIBUTE_VALUE_LENGTH;
    attErrorObjPtr->attHandleInError = connect->aclHandle;
    return;
}

/**
 * @brief received findbytypevalue response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttFindByTypeValueResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    size_t dataLen;
    uint16_t inforNum = 0;
    AttFind attFindObj;
    uint8_t *data = NULL;
    AttClientDataCallback *attClientDataCallback = NULL;
    AttError attErrorObj;

    AlarmCancel(connect->alarm);

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL) || (buffer == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTFINDBYTYPEVALUERESPONSE_END;
    }

    dataLen = BufferGetSize(buffer);
    if (dataLen % STEP_FOUR) {
        FindByTypeValueResponseErrorAssign(&attErrorObj, connect);
        attClientDataCallback->attClientCallback(
            connect->retGattConnectHandle, ATT_ERROR_RESPONSE_ID, &attErrorObj, NULL, attClientDataCallback->context);
        goto ATTFINDBYTYPEVALUERESPONSE_END;
    } else {
        inforNum = dataLen / STEP_FOUR;
    }

    attFindObj.findByTypeValueResponse.handleInfoList = MEM_MALLOC.alloc(sizeof(AttHandleInfo) * inforNum);
    attFindObj.findByTypeValueResponse.listNum = inforNum;
    data = (uint8_t *)BufferPtr(buffer);

    AttHandleInfo *handleInfoList = attFindObj.findByTypeValueResponse.handleInfoList;
    for (uint16_t indexNumber = 0; indexNumber < inforNum; ++indexNumber) {
        handleInfoList[indexNumber].attHandle = ((uint16_t *)(data + indexNumber * STEP_FOUR))[0];
        handleInfoList[indexNumber].groupEndHandle = ((uint16_t *)(data + STEP_TWO + indexNumber * STEP_FOUR))[0];
    }

    attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
        ATT_FIND_BY_TYPE_VALUE_RESPONSE_ID,
        &attFindObj,
        NULL,
        attClientDataCallback->context);
    MEM_MALLOC.free(attFindObj.findByTypeValueResponse.handleInfoList);

ATTFINDBYTYPEVALUERESPONSE_END:
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received readbytype request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadByTypeRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    size_t buffSize;
    AttRead attReadObj;
    uint8_t *data = NULL;
    uint16_t uuidLen;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        goto ATTREADBYTYPEREQUEST_END;
    }

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTREADBYTYPEREQUEST_END;
    }

    buffSize = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    attReadObj.readHandleRangeUuid.handleRange.startHandle = ((uint16_t *)data)[0];
    attReadObj.readHandleRangeUuid.handleRange.endHandle = ((uint16_t *)(data + STEP_TWO))[0];
    uuidLen = buffSize - STEP_FOUR;
    attReadObj.readHandleRangeUuid.uuid = MEM_MALLOC.alloc(sizeof(BtUuid));

    if (uuidLen == (uint16_t)UUID16BITTYPELEN) {
        attReadObj.readHandleRangeUuid.uuid->type = BT_UUID_16;
        attReadObj.readHandleRangeUuid.uuid->uuid16 = ((uint16_t *)(data + STEP_FOUR))[0];
        LOG_INFO("%{public}s enter, callback para : type = %{public}d, uuid = %{public}d",
            __FUNCTION__,
            attReadObj.readHandleRangeUuid.uuid->type,
            attReadObj.readHandleRangeUuid.uuid->uuid16);
    } else if (uuidLen == (uint16_t)UUID128BITTYPELEN) {
        attReadObj.readHandleRangeUuid.uuid->type = BT_UUID_128;
        (void)memcpy_s(attReadObj.readHandleRangeUuid.uuid->uuid128, uuidLen, data + STEP_FOUR, uuidLen);
    }

    attServerDataCallback->attServerCallback(
        connect->retGattConnectHandle, ATT_READ_BY_TYPE_REQUEST_ID, &attReadObj, NULL, attServerDataCallback->context);

    MEM_MALLOC.free(attReadObj.readHandleRangeUuid.uuid);

ATTREADBYTYPEREQUEST_END:
    return;
}

/**
 * @brief received readbytype response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadByTypeResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t dataLen;
    AttRead attReadObj;
    uint8_t len;
    uint16_t indexNum = 0;
    uint8_t *data = NULL;
    AttClientDataCallback *attClientDataCallback = NULL;
    AttError attErrorObj;

    AlarmCancel(connect->alarm);
    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL || (buffer == NULL))) {
        goto ATTREADBYTYPERESPONSE_END;
    }

    dataLen = BufferGetSize(buffer);
    attReadObj.readHandleListNum.len = *(uint8_t *)BufferPtr(buffer);
    len = attReadObj.readHandleListNum.len;
    LOG_INFO("%{public}s callback para : len = %hhu", __FUNCTION__, len);
    if (len <= STEP_TWO) {
        LOG_ERROR("%{public}s len is 0", __FUNCTION__);
        attReadObj.readHandleListNum.valueNum = 0;
    } else if ((dataLen - 1) % len) {
        AttReadByTypeResErrorAssign(&attErrorObj, connect);
        attClientDataCallback->attClientCallback(
            connect->retGattConnectHandle, ATT_ERROR_RESPONSE_ID, &attErrorObj, NULL, attClientDataCallback->context);
        goto ATTREADBYTYPERESPONSE_END;
    } else {
        attReadObj.readHandleListNum.valueNum = (dataLen - 1) / len;
    }

    data = (uint8_t *)BufferPtr(buffer) + 1;
    attReadObj.readHandleListNum.valueList =
        MEM_MALLOC.alloc(sizeof(AttReadByTypeRspDataList) * attReadObj.readHandleListNum.valueNum);
    for (; indexNum < attReadObj.readHandleListNum.valueNum; ++indexNum) {
        attReadObj.readHandleListNum.valueList[indexNum].attributeValue = MEM_MALLOC.alloc(len - STEP_TWO);
        (void)memcpy_s(
            &attReadObj.readHandleListNum.valueList[indexNum], sizeof(AttReadByTypeRspDataList), data, STEP_TWO);
        (void)memcpy_s(attReadObj.readHandleListNum.valueList[indexNum].attributeValue,
            len - STEP_TWO,
            data + STEP_TWO,
            len - STEP_TWO);
        data += len;
    }
    attClientDataCallback->attClientCallback(
        connect->retGattConnectHandle, ATT_READ_BY_TYPE_RESPONSE_ID, &attReadObj, NULL, attClientDataCallback->context);
    AttReadFree(&attReadObj, attReadObj.readHandleListNum.valueNum, READBYTYPERESPONSEFREE);

ATTREADBYTYPERESPONSE_END:
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received read request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    AttRead attReadObj;
    uint8_t *data = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTREADREQUEST_END;
    }

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTREADREQUEST_END;
    }

    data = (uint8_t *)BufferPtr(buffer);
    attReadObj.readHandle.attHandle = ((uint16_t *)data)[0];

    LOG_INFO("%{public}s connectHandle = %hu, callback para : attHandle = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attReadObj.readHandle.attHandle);

    attServerDataCallback->attServerCallback(
        connect->retGattConnectHandle, ATT_READ_REQUEST_ID, &attReadObj, NULL, attServerDataCallback->context);

ATTREADREQUEST_END:
    return;
}

/**
 * @brief received read response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    AttClientDataCallback *attClientDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
    }

    AlarmCancel(connect->alarm);

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTREADRESPONSE_END;
    }

    attClientDataCallback->attClientCallback(
        connect->retGattConnectHandle, ATT_READ_RESPONSE_ID, NULL, (Buffer *)buffer, attClientDataCallback->context);

ATTREADRESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received readblob request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadBlobRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    AttRead attReadObj;
    uint8_t *data = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTREADBLOBREQUEST_END;
    }

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTREADBLOBREQUEST_END;
    }
    data = (uint8_t *)BufferPtr(buffer);
    attReadObj.readBlob.attHandle = ((uint16_t *)data)[0];
    attReadObj.readBlob.offset = ((uint16_t *)(data + STEP_TWO))[0];

    LOG_INFO("%{public}s connectHandle = %hu, callback para : attHandle = %{public}d, offset = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attReadObj.readBlob.attHandle,
        attReadObj.readBlob.offset);

    attServerDataCallback->attServerCallback(
        connect->retGattConnectHandle, ATT_READ_BLOB_REQUEST_ID, &attReadObj, NULL, attServerDataCallback->context);

ATTREADBLOBREQUEST_END:
    return;
}

/**
 * @brief received readblob response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadBlobResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
    }

    AttClientDataCallback *attClientDataCallback = NULL;

    AlarmCancel(connect->alarm);

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTREADBLOBRESPONSE_END;
    }

    attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
        ATT_READ_BLOB_RESPONSE_ID,
        NULL,
        (Buffer *)buffer,
        attClientDataCallback->context);

ATTREADBLOBRESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received readmultiple request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadMultipleRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTREADMULTIPLEREQUEST_END;
    }

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTREADMULTIPLEREQUEST_END;
    }
    LOG_INFO("%{public}s connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
        ATT_READ_MULTIPLE_REQUEST_ID,
        NULL,
        (Buffer *)buffer,
        attServerDataCallback->context);

ATTREADMULTIPLEREQUEST_END:
    return;
}

/**
 * @brief received readmultiple response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadMultipleResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    AttClientDataCallback *attClientDataCallback = NULL;

    AlarmCancel(connect->alarm);

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
    }

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTREADMULTIPLERESPONSE_END;
    }

    attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
        ATT_READ_MULTIPLE_RESPONSE_ID,
        NULL,
        (Buffer *)buffer,
        attClientDataCallback->context);

ATTREADMULTIPLERESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received readbygrouptype request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadByGroupTypeRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_WARN("%{public}s:connect == NULL and return", __FUNCTION__);
        return;
    }

    AttRead attReadObj;
    uint8_t *data = NULL;
    size_t buffSize;
    AttServerDataCallback *attServerDataCallback = NULL;

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL) || (buffer == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTREADBYGROUPTYPEREQUEST_END;
    }

    data = (uint8_t *)BufferPtr(buffer);
    attReadObj.readGroupRequest.handleRange.startHandle = ((uint16_t *)data)[0];
    attReadObj.readGroupRequest.handleRange.endHandle = ((uint16_t *)(data + STEP_TWO))[0];
    buffSize = BufferGetSize(buffer);
    attReadObj.readGroupRequest.uuid = MEM_MALLOC.alloc(sizeof(BtUuid));

    if ((buffSize - STEP_FOUR) == UUID16BITTYPELEN) {
        attReadObj.readGroupRequest.uuid->type = BT_UUID_16;
        attReadObj.readGroupRequest.uuid->uuid16 = ((uint16_t *)(data + STEP_FOUR))[0];
        LOG_INFO("%{public}s enter, callback para : uuid_type = %{public}d, uuid = %{public}d",
            __FUNCTION__,
            attReadObj.readGroupRequest.uuid->type,
            attReadObj.readGroupRequest.uuid->uuid16);
    } else if ((buffSize - STEP_FOUR) == UUID128BITTYPELEN) {
        attReadObj.readGroupRequest.uuid->type = BT_UUID_128;
        (void)memcpy_s(
            attReadObj.readGroupRequest.uuid->uuid128, UUID128BITTYPELEN, data + STEP_FOUR, UUID128BITTYPELEN);
    }

    attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
        ATT_READ_BY_GROUP_TYPE_REQUEST_ID,
        &attReadObj,
        NULL,
        attServerDataCallback->context);
    MEM_MALLOC.free(attReadObj.readGroupRequest.uuid);

ATTREADBYGROUPTYPEREQUEST_END:
    return;
}

/**
 * @brief received readbygrouptype response error assign.
 *
 * @param1 attErrorObjPtr Indicates the pointer to AttError.
 * @param2 connect Indicates the pointer to AttConnectInfo.
 */
static void ReadByGroupTypeResponseErrorAssign(AttError *attErrorObjPtr, const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (attErrorObjPtr == NULL || connect == NULL) {
        LOG_WARN("%{public}s:attErrorObjPtr or connect is NULL, return", __FUNCTION__);
        return;
    }
    attErrorObjPtr->reqOpcode = READ_BY_GROUP_TYPE_REQUEST;
    attErrorObjPtr->errorCode = ATT_INVALID_ATTRIBUTE_VALUE_LENGTH;
    attErrorObjPtr->attHandleInError = connect->aclHandle;

    return;
}

/**
 * @brief received readbygrouptype response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadByGroupTypeResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t dataLen;
    AttRead attReadObj;
    uint8_t len;
    uint8_t *data = NULL;
    AttClientDataCallback *attClientDataCallback = NULL;
    AttError attErrorObj;

    AlarmCancel(connect->alarm);

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL) || (buffer == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTREADBYGROUPTYPERESPONSE_END;
    }

    dataLen = BufferGetSize(buffer);
    data = ((uint8_t *)BufferPtr(buffer) + 1);
    attReadObj.readGroupResponse.length = *(uint8_t *)BufferPtr(buffer);
    len = attReadObj.readGroupResponse.length;
    if (len == 0) {
        LOG_ERROR("%{public}s len is 0", __FUNCTION__);
        attReadObj.readGroupResponse.num = 0;
    } else if ((dataLen - 1) % len) {
        ReadByGroupTypeResponseErrorAssign(&attErrorObj, connect);
        attClientDataCallback->attClientCallback(
            connect->retGattConnectHandle, ATT_ERROR_RESPONSE_ID, &attErrorObj, NULL, attClientDataCallback->context);
        goto ATTREADBYGROUPTYPERESPONSE_END;
    } else {
        attReadObj.readGroupResponse.num = (dataLen - 1) / len;
    }
    attReadObj.readGroupResponse.attributeData = (AttReadGoupAttributeData *)MEM_MALLOC.alloc(
        sizeof(AttReadGoupAttributeData) * attReadObj.readGroupResponse.num);
    AttReadAttrAssign(&attReadObj, len, data);
    attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
        ATT_READ_BY_GROUP_TYPE_RESPONSE_ID,
        &attReadObj,
        NULL,
        attClientDataCallback->context);
    AttReadFree(&attReadObj, attReadObj.readGroupResponse.num, READBYGROUPTYPERESPONSEFREE);

ATTREADBYGROUPTYPERESPONSE_END:
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received write request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttWriteRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t buffSize;
    AttWrite attWriteObj;
    uint8_t *data = NULL;
    Buffer *bufferNew = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTWRITEREQUEST_END;
    }

    buffSize = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    attWriteObj.writeRequest.attHandle = ((uint16_t *)data)[0];
    bufferNew = BufferSliceMalloc(buffer, STEP_TWO, buffSize - STEP_TWO);
    LOG_INFO("%{public}s connectHandle = %hu, callback para : attHandle = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attWriteObj.writeRequest.attHandle);

    attServerDataCallback->attServerCallback(
        connect->retGattConnectHandle, ATT_WRITE_REQUEST_ID, &attWriteObj, bufferNew, attServerDataCallback->context);

    BufferFree(bufferNew);

ATTWRITEREQUEST_END:
    return;
}

/**
 * @brief received write response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttWriteResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    AttClientDataCallback *attClientDataCallback = NULL;

    AlarmCancel(connect->alarm);

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTWRITERESPONSE_END;
    }

    attClientDataCallback->attClientCallback(
        connect->retGattConnectHandle, ATT_WRITE_RESPONSE_ID, NULL, (Buffer *)buffer, attClientDataCallback->context);

ATTWRITERESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received write command.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttWriteCommand(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t buffSize;
    AttWrite attWriteObj;
    uint8_t *data = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTWRITECOMMAND_END;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTWRITECOMMAND_END;
    }

    buffSize = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    attWriteObj.writeCommand.attHandle = ((uint16_t *)data)[0];
    Buffer *bufferNew = BufferSliceMalloc(buffer, STEP_TWO, buffSize - STEP_TWO);
    LOG_INFO("%{public}s connectHandle = %hu, callback para : attHandle = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attWriteObj.writeCommand.attHandle);
    attServerDataCallback->attServerCallback(
        connect->retGattConnectHandle, ATT_WRITE_COMMAND_ID, &attWriteObj, bufferNew, attServerDataCallback->context);

    BufferFree(bufferNew);

ATTWRITECOMMAND_END:
    return;
}

/**
 * @brief received signedwrite command gap return value error assign.
 *
 * @param1 attWriteObjPtr Indicates the pointer to AttWrite.
 * @param2 ret Indicates the ret.
 * @param3 data Indicates the pdata.
 */
static void AttSignedWriteCommandGapRetErrorAssign(AttWrite *attWriteObjPtr, const uint8_t *data)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (attWriteObjPtr == NULL || data == NULL) {
        LOG_WARN("%{public}s:attWriteObjPtr or data is NULL, return", __FUNCTION__);
        return;
    }
    attWriteObjPtr->signedWriteCommand.attHandleValueObj.attHandle = ((uint16_t *)(data + 1))[0];
    attWriteObjPtr->signedWriteCommand.authSignatureLen = GAPSIGNATURESIZE;
    return;
}

/**
 * @brief received signedwrite command.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttSignedWriteCommand(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t buffSize;
    AttWrite attWriteObj;
    uint8_t *data = NULL;
    uint8_t *dataBuffer = NULL;
    Buffer *bufferNew = NULL;
    Buffer *bufferallPtr = NULL;
    Buffer *sigedWriteBuffPtr = NULL;
    int ret;
    GapSignatureData gapSignatureDataObj;
    uint8_t signature[12] = {0};
    AttServerDataCallback *attServerDataCallback = NULL;
    SigedWriteCommandConfirmationContext *sigedWriteCommandConfirmContextPtr = NULL;

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL) || (buffer == NULL)) {
        goto ATTSIGNEDWRITECOMMAND_END;
    }

    dataBuffer = (uint8_t *)BufferPtr(buffer);
    buffSize = BufferGetSize(buffer);
    bufferallPtr = BufferMalloc(buffSize + 1);
    data = (uint8_t *)BufferPtr(bufferallPtr);
    AttSignWriteCommConfDataAssign(data, dataBuffer, buffSize, signature);
    gapSignatureDataObj.data = data;
    if (buffSize < (sizeof(signature) - 1)) {
        LOG_ERROR("%{public}s buffSize is invalid", __FUNCTION__);
        goto ATTSIGNEDWRITECOMMAND_END;
    }
    gapSignatureDataObj.dataLen = buffSize - (sizeof(signature) - 1);
    sigedWriteBuffPtr = BufferMalloc(sizeof(SigedWriteCommandConfirmationContext));
    sigedWriteCommandConfirmContextPtr = (SigedWriteCommandConfirmationContext *)BufferPtr(sigedWriteBuffPtr);
    AttSignWriteCommConfContextAssign(sigedWriteCommandConfirmContextPtr, connect, data, bufferallPtr, buffSize);

    uint8_t *arrayPtr = signature;
    ret = GAPIF_LeDataSignatureConfirmationAsync(
        &(connect->addr), gapSignatureDataObj, arrayPtr, AttGapSignatureConfirmationResult, sigedWriteBuffPtr);
    if (ret != BT_NO_ERROR) {
        AttSignedWriteCommandGapRetErrorAssign(&attWriteObj, data);
        attWriteObj.signedWriteCommand.result = GAP_REJECT;
        (void)memcpy_s(attWriteObj.signedWriteCommand.authSignature,
            sizeof(signature),
            data + (buffSize - sizeof(signature)),
            sizeof(signature));
        bufferNew = BufferSliceMalloc(buffer, STEP_TWO, buffSize - sizeof(signature) - sizeof(uint16_t));
        attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
            ATT_SIGNED_WRITE_COMMAND_ID,
            &attWriteObj,
            bufferNew,
            attServerDataCallback->context);
        AttSignedWriteCommandBufferFree(bufferNew, sigedWriteBuffPtr, bufferallPtr);
    }

ATTSIGNEDWRITECOMMAND_END:
    return;
}

/**
 * @brief received preparewrite request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttPrepareWriteRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t buffSize;
    AttWrite attWriteObj;
    uint8_t *data = NULL;
    Buffer *bufferNew = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTPREPAREWRITEREQUEST_END;
    }

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTPREPAREWRITEREQUEST_END;
    }

    buffSize = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    attWriteObj.prepareWrite.handleValue.attHandle = ((uint16_t *)data)[0];
    attWriteObj.prepareWrite.offset = ((uint16_t *)(data + STEP_TWO))[0];
    bufferNew = BufferSliceMalloc(buffer, STEP_FOUR, buffSize - STEP_FOUR);
    LOG_INFO("%{public}s connectHandle = %hu, callback para : attHandle = %{public}d, offset = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attWriteObj.prepareWrite.handleValue.attHandle,
        attWriteObj.prepareWrite.offset);

    attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
        ATT_PREPARE_WRITE_REQUEST_ID,
        &attWriteObj,
        bufferNew,
        attServerDataCallback->context);

    BufferFree(bufferNew);

ATTPREPAREWRITEREQUEST_END:
    return;
}

/**
 * @brief received preparewrite response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttPrepareWriteResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t buffSize;
    AttWrite attWriteObj;
    uint8_t *data = NULL;
    Buffer *bufferNew = NULL;
    AttClientDataCallback *attClientDataCallback = NULL;

    AlarmCancel(connect->alarm);
    if (buffer == NULL) {
        goto ATTPREPAREWRITERESPONSE_END;
    }

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTPREPAREWRITERESPONSE_END;
    }

    buffSize = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    attWriteObj.prepareWrite.handleValue.attHandle = ((uint16_t *)data)[0];
    attWriteObj.prepareWrite.offset = ((uint16_t *)(data + STEP_TWO))[0];

    if (buffSize - STEP_FOUR > 0) {
        bufferNew = BufferSliceMalloc(buffer, STEP_FOUR, buffSize - STEP_FOUR);
        attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
            ATT_PREPARE_WRITE_RESPONSE_ID,
            &attWriteObj,
            bufferNew,
            attClientDataCallback->context);
        BufferFree(bufferNew);
    } else {
        attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
            ATT_PREPARE_WRITE_RESPONSE_ID,
            &attWriteObj,
            NULL,
            attClientDataCallback->context);
    }

ATTPREPAREWRITERESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received executewrite request.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttExecuteWriteRequest(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    AttWrite attWriteObj;
    uint8_t *data = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTEXECUTEWRITEREQUEST_END;
    }

    if ((connect->transportType == BT_TRANSPORT_LE && connect->mtu == DEFAULTLEATTMTU) ||
        (connect->transportType == BT_TRANSPORT_BR_EDR && connect->mtu == DEFAULTBREDRMTU)) {
        connect->mtuFlag = true;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTEXECUTEWRITEREQUEST_END;
    }

    data = (uint8_t *)BufferPtr(buffer);
    attWriteObj.excuteWrite.flag = data[0];

    LOG_INFO("%{public}s connecthandle = %{public}d, callback para : flag = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        attWriteObj.excuteWrite.flag);

    attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
        ATT_EXECUTE_WRITE_REQUEST_ID,
        &attWriteObj,
        NULL,
        attServerDataCallback->context);

ATTEXECUTEWRITEREQUEST_END:
    return;
}

/**
 * @brief received executewrite response.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttExecuteWriteResponse(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    AttClientDataCallback *attClientDataCallback = NULL;

    AlarmCancel(connect->alarm);

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTEXECUTEWRITERESPONSE_END;
    }

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTEXECUTEWRITERESPONSE_END;
    }

    attClientDataCallback->attClientCallback(
        connect->retGattConnectHandle, ATT_EXECUTE_WRITE_RESPONSE_ID, NULL, NULL, attClientDataCallback->context);

ATTEXECUTEWRITERESPONSE_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief received handlevalue notification.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttHandleValueNotification(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t buffNotiSize;
    AttHandleValue AttHandleValueObj;
    uint8_t *data = NULL;
    Buffer *bufferNew = NULL;
    AttClientDataCallback *attClientDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTHANDLEVALUENOTIFICATION_END;
    }

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTHANDLEVALUENOTIFICATION_END;
    }

    buffNotiSize = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    AttHandleValueObj.attHandle = ((uint16_t *)data)[0];
    bufferNew = BufferSliceMalloc(buffer, STEP_TWO, buffNotiSize - STEP_TWO);

    LOG_INFO("%{public}s connectHandle = %hu, callback para : attHandle = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        AttHandleValueObj.attHandle);

    attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
        ATT_HANDLE_VALUE_NOTIFICATION_ID,
        &AttHandleValueObj,
        bufferNew,
        attClientDataCallback->context);

    BufferFree(bufferNew);

ATTHANDLEVALUENOTIFICATION_END:
    return;
}

/**
 * @brief received handlevalue indication.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttHandleValueIndication(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    size_t buffIndiSize;
    AttHandleValue AttHandleValueObj;
    uint8_t *data = NULL;
    Buffer *bufferNew = NULL;
    AttClientDataCallback *attClientDataCallback = NULL;

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTHANDLEVALUEINDICATION_END;
    }

    attClientDataCallback = AttGetATTClientCallback();
    if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
        LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        goto ATTHANDLEVALUEINDICATION_END;
    }

    buffIndiSize = BufferGetSize(buffer);
    data = (uint8_t *)BufferPtr(buffer);
    AttHandleValueObj.attHandle = ((uint16_t *)data)[0];
    bufferNew = BufferSliceMalloc(buffer, STEP_TWO, buffIndiSize - STEP_TWO);

    LOG_INFO("%{public}s connectHandle = %hu, callback para : attHandle = %{public}d",
        __FUNCTION__,
        connect->retGattConnectHandle,
        AttHandleValueObj.attHandle);

    attClientDataCallback->attClientCallback(connect->retGattConnectHandle,
        ATT_HANDLE_VALUE_INDICATION_ID,
        &AttHandleValueObj,
        bufferNew,
        attClientDataCallback->context);

    BufferFree(bufferNew);

ATTHANDLEVALUEINDICATION_END:
    return;
}

/**
 * @brief received handlevalue confirmation.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttHandleValueConfirmation(AttConnectInfo *connect, const Buffer *buffer)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (connect == NULL) {
        LOG_ERROR("%{public}s connect is NULL", __FUNCTION__);
        return;
    }

    AttServerDataCallback *attServerDataCallback = NULL;
    AttWrite attWrite;
    attWrite.confirmation.attHandle = 0x0000;

    AlarmCancel(connect->alarm);

    if (buffer == NULL) {
        LOG_WARN("%{public}s:buffer == NULL", __FUNCTION__);
        goto ATTHANDLEVALUECONFIRMATION_END;
    }

    attServerDataCallback = AttGetATTServerCallback();
    if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
        LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        goto ATTHANDLEVALUECONFIRMATION_END;
    }

    if (connect->serverSendFlag) {
        connect->serverSendFlag = false;
    }

    attServerDataCallback->attServerCallback(connect->retGattConnectHandle,
        ATT_HANDLE_VALUE_CONFIRMATION_ID,
        &attWrite,
        NULL,
        attServerDataCallback->context);

ATTHANDLEVALUECONFIRMATION_END:
    LOG_INFO("%{public}s return connect != NULL, connectHandle = %hu", __FUNCTION__, connect->retGattConnectHandle);
    ListRemoveFirst(connect->instruct);
    AttReceiveSequenceScheduling(connect);
    return;
}

/**
 * @brief assign signedWriteCommand context.
 *
 * @param1 sigedWriteCommandConfirmContextPtr Indicates the pointer to SigedWriteCommandConfirmationContext.
 * @param2 connect Indicates the pointer to AttConnectInfo.
 * @param3 data Indicates the pointer to uint8_t.
 * @param4 bufferallPtr  Indicates the pointer to Buffer.
 * @param5 buffSize Indicates the size_t.
 */
static void AttSignWriteCommConfContextAssign(SigedWriteCommandConfirmationContext *sigedWriteCommandConfirmContextPtr,
    const AttConnectInfo *connect, const uint8_t *data, const Buffer *bufferallPtr, const size_t buffSize)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (sigedWriteCommandConfirmContextPtr == NULL) {
        LOG_ERROR("%{public}s sigedWriteCommandConfirmContextPtr is NULL", __FUNCTION__);
        return;
    }

    sigedWriteCommandConfirmContextPtr->connect = (AttConnectInfo *)connect;
    sigedWriteCommandConfirmContextPtr->data = (uint8_t *)data;
    sigedWriteCommandConfirmContextPtr->bufferallPtr = (Buffer *)bufferallPtr;
    sigedWriteCommandConfirmContextPtr->bufferSize = buffSize - STEP_TWO - GAPSIGNATURESIZE;

    return;
}

/**
 * @brief assign signedWriteCommand data.
 *
 * @param1 data Indicates the pointer to uint8_t.
 * @param2 dataBuffer  Indicates the pointer to Buffer.
 * @param3 buffSize Indicates the size_t.
 * @param4 signature  Indicates the pointer to uint8_t.
 */
static void AttSignWriteCommConfDataAssign(
    uint8_t *data, const uint8_t *dataBuffer, size_t buffSize, uint8_t signature[12])
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (data == NULL || dataBuffer == NULL) {
        LOG_ERROR("%{public}s data or dataBuffer is NULL", __FUNCTION__);
        return;
    }
    if (buffSize < STEP_TWO + GAPSIGNATURESIZE) {
        LOG_ERROR("%{public}s buffSize is small", __FUNCTION__);
        return;
    }

    data[0] = SIGNED_WRITE_COMMAND;
    (void)memcpy_s(data + 1, buffSize, dataBuffer, buffSize);
    (void)memcpy_s(signature,
        GAPSIGNATURESIZE,
        dataBuffer + STEP_TWO + (buffSize - STEP_TWO - GAPSIGNATURESIZE),
        GAPSIGNATURESIZE);

    return;
}

/**
 * @brief assign signedWriteCommand data.
 *
 * @param1 bufferNew  Indicates the pointer to Buffer.
 * @param2 sigedWriteBuffPtr  Indicates the pointer to Buffer.
 * @param3 bufferallPtr  Indicates the pointer to Buffer.
 */
static void AttSignedWriteCommandBufferFree(Buffer *bufferNew, Buffer *sigedWriteBuffPtr, Buffer *bufferallPtr)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BufferFree(bufferNew);
    BufferFree(sigedWriteBuffPtr);
    BufferFree(bufferallPtr);

    return;
}

/**
 * @brief assign signedWriteCommand data.
 *
 * @param1 attErrorObjPtr  Indicates the pointer to AttError.
 * @param2 connect  Indicates the pointer to AttConnectInfo.
 */
static void AttReadByTypeResErrorAssign(AttError *attErrorObjPtr, const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (attErrorObjPtr == NULL || connect == NULL) {
        LOG_ERROR("%{public}s attErrorObjPtr or connect is NULL", __FUNCTION__);
        return;
    }

    attErrorObjPtr->reqOpcode = READ_BY_TYPE_REQUEST;
    attErrorObjPtr->errorCode = ATT_INVALID_ATTRIBUTE_VALUE_LENGTH;
    attErrorObjPtr->attHandleInError = connect->aclHandle;

    return;
}

/**
 * @brief function list init.
 *
 */
void FunctionListInit()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    recvDataFunction *functionList = GetFunctionArrayDress();

    functionList[ERROR_RESPONSE] = AttErrorResponse;
    functionList[FIND_INFORMATION_REQUEST] = AttFindInformationRequest;
    functionList[FIND_INFORMATION_RESPONSE] = AttFindInformationResponse;
    functionList[FIND_BY_TYPE_VALUE_REQUEST] = AttFindByTypeValueRequest;
    functionList[FIND_BY_TYPE_VALUE_RESPONSE] = AttFindByTypeValueResponse;
    functionList[READ_BY_TYPE_REQUEST] = AttReadByTypeRequest;
    functionList[READ_BY_TYPE_RESPONSE] = AttReadByTypeResponse;
    functionList[READ_REQUEST] = AttReadRequest;
    functionList[READ_RESPONSE] = AttReadResponse;
    functionList[READ_BLOB_REQUEST] = AttReadBlobRequest;
    functionList[READ_BLOB_RESPONSE] = AttReadBlobResponse;
    functionList[READ_MULTIPLE_REQUEST] = AttReadMultipleRequest;
    functionList[READ_MULTIPLE_RESPONSE] = AttReadMultipleResponse;
    functionList[READ_BY_GROUP_TYPE_REQUEST] = AttReadByGroupTypeRequest;
    functionList[READ_BY_GROUP_TYPE_RESPONSE] = AttReadByGroupTypeResponse;
    functionList[WRITE_REQUEST] = AttWriteRequest;
    functionList[WRITE_RESPONSE] = AttWriteResponse;
    functionList[WRITE_COMMAND] = AttWriteCommand;
    functionList[SIGNED_WRITE_COMMAND] = AttSignedWriteCommand;
    functionList[PREPARE_WRITE_REQUEST] = AttPrepareWriteRequest;
    functionList[PREPARE_WRITE_RESPONSE] = AttPrepareWriteResponse;
    functionList[EXECUTE_WRITE_REQUEST] = AttExecuteWriteRequest;
    functionList[EXECUTE_WRITE_RESPONSE] = AttExecuteWriteResponse;
    functionList[HANDLE_VALUE_NOTIFICATION] = AttHandleValueNotification;
    functionList[HANDLE_VALUE_INDICATION] = AttHandleValueIndication;
    functionList[HANDLE_VALUE_CONFIRMATION] = AttHandleValueConfirmation;
    functionList[EXCHANGE_MTU_REQUEST] = AttExchangeMTURequest;
    functionList[EXCHANGE_MTU_RESPONSE] = AttExchangeMTUResponse;

    return;
}
