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

#include "sdp_client_parse.h"

#include <string.h>

#include "sdp.h"

#include "sdp_connect.h"
#include "sdp_util.h"

#include "allocator.h"
#include "bt_endian.h"
#include "list.h"
#include "log.h"
#include "packet.h"

/// Client request List
static List *g_requestList = NULL;
/// TransactionID
static uint16_t g_transactionId = 0;

typedef struct {
    uint8_t *buffer;
    uint16_t length;
} BufferInfo;

static void SdpCallbackError(const BtAddr *address, uint16_t transactionId);
static void SdpParseErrorResponse(
    const BtAddr *addr, uint16_t transactionId, uint16_t parameterLength, const Packet *packet);
static void SdpParseSearchResponse(const BtAddr *addr, uint16_t lcid, uint16_t transactionId, Packet *packet);
static void SdpParseAttributeResponse(const BtAddr *addr, uint16_t lcid, uint16_t transactionId, Packet *packet);
static void SdpParseSearchAttributeResponse(const BtAddr *addr, uint16_t lcid, uint16_t transactionId, Packet *packet);
static int SdpParseAttributeValue(BufferInfo *bufferInfo, uint16_t attributeId, SdpService *service);
static void SdpFreeService(SdpService *service);
static void SdpFreeServiceArray(SdpService *serviceArray, uint16_t serviceNum);

uint16_t SdpGetTransactionId()
{
    if (g_transactionId >= 0xFFFF) {
        g_transactionId = 0;
    }
    g_transactionId++;

    return g_transactionId;
}

static void SdpFreeClientRequest(void *data)
{
    if (data == NULL) {
        return;
    }
    SdpClientRequest *request = (SdpClientRequest *)data;
    if (request->packet != NULL) {
        PacketFree(request->packet);
        request->packet = NULL;
    }
    if (request->assemblePacket != NULL) {
        PacketFree(request->assemblePacket);
        request->assemblePacket = NULL;
    }

    MEM_MALLOC.free(request);
    request = NULL;
}

void SdpCreateRequestList()
{
    g_requestList = ListCreate(SdpFreeClientRequest);
}

void SdpDestroyRequestList()
{
    if (g_requestList != NULL) {
        ListDelete(g_requestList);
        g_requestList = NULL;
    }
}

SdpClientRequest *SdpFindRemainRequestByAddress(const BtAddr *addr, bool *flag)
{
    ListNode *node = NULL;
    SdpClientRequest *request = NULL;

    if (g_requestList == NULL) {
        return NULL;
    }

    node = ListGetFirstNode(g_requestList);
    while (node != NULL) {
        request = (SdpClientRequest *)ListGetNodeData(node);
        if ((memcmp(&request->addr.addr, addr->addr, SDP_BLUETOOTH_ADDRESS_LENGTH) == 0) &&
            ((request->packetState == SDP_PACKET_SEND) || (request->packetState == SDP_PACKET_SEND_FRAGMENT))) {
            *flag = true;
            return NULL;
        }
        node = ListGetNextNode(node);
    }

    node = ListGetFirstNode(g_requestList);
    while (node != NULL) {
        request = (SdpClientRequest *)ListGetNodeData(node);
        if ((memcmp(&request->addr.addr, addr->addr, SDP_BLUETOOTH_ADDRESS_LENGTH) == 0) &&
            (request->packetState == SDP_PACKET_WAIT)) {
            return request;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

SdpClientRequest *SdpFindRequestByAddress(const BtAddr *addr)
{
    ListNode *node = NULL;
    SdpClientRequest *request = NULL;

    if (g_requestList == NULL) {
        return NULL;
    }

    node = ListGetFirstNode(g_requestList);
    while (node != NULL) {
        request = (SdpClientRequest *)ListGetNodeData(node);
        if (memcmp(&request->addr.addr, addr->addr, SDP_BLUETOOTH_ADDRESS_LENGTH) == 0) {
            return request;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

SdpClientRequest *SdpFindRequestByTransactionId(uint16_t transactionId)
{
    ListNode *node = NULL;
    SdpClientRequest *request = NULL;

    if (g_requestList == NULL) {
        return NULL;
    }
    node = ListGetFirstNode(g_requestList);
    while (node != NULL) {
        request = (SdpClientRequest *)ListGetNodeData(node);
        if (request->transactionId == transactionId) {
            return request;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

void SdpAddRequest(SdpClientRequest *request)
{
    ListAddLast(g_requestList, request);
}

static void SdpRemoveRequestByTransactionId(uint16_t transactionId)
{
    SdpClientRequest *request = NULL;

    request = SdpFindRequestByTransactionId(transactionId);
    if (request == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    ListRemoveNode(g_requestList, request);
}

void SdpRemoveRequestByAddress(const BtAddr *addr)
{
    SdpClientRequest *request = NULL;

    request = SdpFindRequestByAddress(addr);
    if (request == NULL) {
        return;
    }
    SdpCallbackError(&request->addr, request->transactionId);
}

void SdpRemoveAllRequestByAddress(const BtAddr *addr)
{
    SdpClientRequest *request = NULL;
    ListNode *node = NULL;
    uint16_t num = 0;

    if (g_requestList == NULL) {
        return;
    }

    node = ListGetFirstNode(g_requestList);
    while (node != NULL) {
        request = (SdpClientRequest *)ListGetNodeData(node);
        if (memcmp(&request->addr.addr, addr->addr, SDP_BLUETOOTH_ADDRESS_LENGTH) == 0) {
            num++;
        }
        node = ListGetNextNode(node);
    }

    while (num) {
        SdpRemoveRequestByAddress(addr);
        num--;
    }
}

NO_SANITIZE("cfi") static void SdpCallbackError(const BtAddr *address, uint16_t transactionId)
{
    SdpClientRequest *request = NULL;

    request = SdpFindRequestByTransactionId(transactionId);
    if (request == NULL) {
        LOG_ERROR(
            "[%{public}s][%{public}d] Cannot find client request [0x%04x]", __FUNCTION__, __LINE__, transactionId);
        return;
    }
    LOG_DEBUG("[%{public}s][%{public}d] ErrorCallback start", __FUNCTION__, __LINE__);
    switch (request->pduId) {
        case SDP_SERVICE_SEARCH_REQUEST:
            if (request->callback.ServiceSearchCb != NULL) {
                request->callback.ServiceSearchCb(address, NULL, 0, request->context);
            }
            break;
        case SDP_SERVICE_ATTRIBUTE_REQUEST:
            if (request->callback.ServiceAttributeCb != NULL) {
                request->callback.ServiceAttributeCb(address, NULL, request->context);
            }
            break;
        case SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST:
            if (request->callback.ServiceSearchAttributeCb != NULL) {
                request->callback.ServiceSearchAttributeCb(address, NULL, 0, request->context);
            }
            break;
        default:
            break;
    }
    LOG_DEBUG("[%{public}s][%{public}d] ErrorCallback end", __FUNCTION__, __LINE__);

    SdpRemoveRequestByTransactionId(transactionId);
}

/// store assemble to request list, send request again, waiting for packet next time.
static void SdpSendRequestForAssemblePacket(uint16_t lcid, uint16_t transactionId, Packet *packet,
    const uint8_t *continuationState, uint8_t continuationStateLen)
{
    SdpClientRequest *request = NULL;

    request = SdpFindRequestByTransactionId(transactionId);
    if ((request == NULL) || (request->packetState == SDP_PACKET_WAIT)) {
        return;
    }
    if (request->packetState == SDP_PACKET_SEND) {
        request->assemblePacket = PacketRefMalloc(packet);
        if (request->assemblePacket == NULL) {
            LOG_ERROR("point to NULL");
            return;
        }
    } else {
        PacketAssemble(request->assemblePacket, packet);
    }

    request->transactionId = SdpGetTransactionId();
    request->packetState = SDP_PACKET_SEND_FRAGMENT;

    SdpSendRequest(lcid, request->transactionId, continuationStateLen, continuationState, request->packet);
}

static SdpServiceCallback SdpGetCallback(const BtAddr *addr, uint16_t transactionId, void **context)
{
    SdpClientRequest *request = NULL;
    SdpServiceCallback callback;

    (void)memset_s(&callback, sizeof(SdpServiceCallback), 0, sizeof(SdpServiceCallback));
    request = SdpFindRequestByTransactionId(transactionId);
    if (request == NULL) {
        return callback;
    }
    *context = request->context;

    switch (request->pduId) {
        case SDP_SERVICE_SEARCH_REQUEST:
            callback.ServiceSearchCb = request->callback.ServiceSearchCb;
            break;
        case SDP_SERVICE_ATTRIBUTE_REQUEST:
            callback.ServiceAttributeCb = request->callback.ServiceAttributeCb;
            break;
        case SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST:
            callback.ServiceSearchAttributeCb = request->callback.ServiceSearchAttributeCb;
            break;
        default:
            break;
    }
    return callback;
}

void SdpParseServerResponse(const BtAddr *addr, uint16_t lcid, const Packet *data)
{
    uint8_t header[SDP_PDU_HEADER_LENGTH] = {0};
    SdpPduId pduId;
    uint16_t transactionId;
    uint16_t parameterLength;
    Packet *packet = NULL;
    uint16_t offset = 0;

    packet = PacketRefMalloc(data);
    if (packet == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    PacketExtractHead(packet, header, sizeof(header));
    /// PDU ID
    pduId = header[offset];
    offset++;
    /// Transaction ID
    transactionId = BE2H_16(*(uint16_t *)(header + offset));
    offset += SDP_UINT16_LENGTH;
    /// ParameterLength
    parameterLength = BE2H_16(*(uint16_t *)(header + offset));
    if (parameterLength != PacketSize(packet)) {
        LOG_ERROR("[%{public}s][%{public}d] Invalid pdu size [%u]", __FUNCTION__, __LINE__, parameterLength);
        SdpCallbackError(addr, transactionId);
        PacketFree(packet);
        return;
    }

    switch (pduId) {
        case SDP_ERROR_RESPONSE:
            SdpParseErrorResponse(addr, transactionId, parameterLength, packet);
            break;
        case SDP_SERVICE_SEARCH_RESPONSE:
            SdpParseSearchResponse(addr, lcid, transactionId, packet);
            break;
        case SDP_SERVICE_ATTRIBUTE_RESPONSE:
            SdpParseAttributeResponse(addr, lcid, transactionId, packet);
            break;
        case SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE:
            SdpParseSearchAttributeResponse(addr, lcid, transactionId, packet);
            break;
        default:
            LOG_ERROR("[%{public}s][%{public}d] Invalid PDU ID [%u]", __FUNCTION__, __LINE__, pduId);
            break;
    }
    PacketFree(packet);
    packet = NULL;
}

static uint16_t SdpParseServiceRecordHandleList(
    const BtAddr *addr, uint16_t transactionId, uint16_t totalServiceRecordCount, Packet *data, uint32_t *handleArray)
{
    SdpClientRequest *request = NULL;
    uint16_t handleNum = 0;

    Packet *packet = NULL;
    uint8_t *buffer = NULL;
    uint16_t length;

    request = SdpFindRequestByTransactionId(transactionId);
    if (request == NULL) {
        LOG_ERROR("point to NULL");
        return 0;
    }
    if (request->assemblePacket != NULL) {
        packet = request->assemblePacket;
        PacketAssemble(packet, data);
    } else {
        packet = PacketRefMalloc(data);
    }

    length = PacketSize(packet);
    if (length != totalServiceRecordCount * SDP_SERVICE_RECORD_HANDLE_BYTE) {
        PacketFree(packet);
        packet = NULL;
        request->assemblePacket = NULL;
        return 0;
    }

    buffer = MEM_MALLOC.alloc(length);
    if (buffer == NULL) {
        LOG_ERROR("buffer is NULL");
        return 0;
    }
    (void)memset_s(buffer, length, 0, length);
    PacketRead(packet, buffer, 0, length);

    for (; handleNum < totalServiceRecordCount; handleNum++) {
        uint32_t handle = BE2H_32(*(uint32_t *)(buffer + handleNum * SDP_SERVICE_RECORD_HANDLE_BYTE));
        if (handle <= SDP_MAX_RESERVED_RECORD_HANDLE) {
            LOG_ERROR(
                "[%{public}s][%{public}d] Invalid Service Record Handle [0x%08x]", __FUNCTION__, __LINE__, handle);
            MEM_MALLOC.free(buffer);
            buffer = NULL;
            PacketFree(packet);
            packet = NULL;
            request->assemblePacket = NULL;
            return 0;
        }
        handleArray[handleNum] = handle;
    }
    MEM_MALLOC.free(buffer);
    buffer = NULL;
    PacketFree(packet);
    packet = NULL;
    request->assemblePacket = NULL;

    return handleNum;
}

static int SdpParseSingleAttributeList(BufferInfo *bufferInfo, SdpService *service)
{
    uint32_t attributeLength = 0;
    uint8_t type;
    uint16_t offset = 0;
    int pos;

    service->attributeNumber = 0;
    service->attribute = MEM_MALLOC.alloc(sizeof(SdpAttribute) * SDP_ATTRIBUTE_COUNT);
    if (service->attribute == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(
        service->attribute, sizeof(SdpAttribute) * SDP_ATTRIBUTE_COUNT, 0, sizeof(SdpAttribute) * SDP_ATTRIBUTE_COUNT);

    service->sequenceAttributeNumber = 0;
    service->sequenceAttribute = MEM_MALLOC.alloc(sizeof(SdpSequenceAttribute) * SDP_SEQUENCE_ATTRIBUTE_COUNT);
    if (service->sequenceAttribute == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }

    (void)memset_s(service->sequenceAttribute,
        sizeof(SdpSequenceAttribute) * SDP_SEQUENCE_ATTRIBUTE_COUNT,
        0,
        sizeof(SdpSequenceAttribute) * SDP_SEQUENCE_ATTRIBUTE_COUNT);

    /// Descriptor type
    type = bufferInfo->buffer[offset];
    offset++;
    /// Descriptor size
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_DES) {
        LOG_ERROR(
            "[%{public}s][%{public}d] There is wrong type [0x%02x] with attribute list.", __FUNCTION__, __LINE__, type);
        return BT_BAD_PARAM;
    }
    /// Sequence length
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &attributeLength);
    if (bufferInfo->length < attributeLength) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    offset += pos;
    uint8_t *bufferEnd = bufferInfo->buffer + attributeLength;

    while (bufferInfo->buffer + offset < bufferEnd) {
        /// AttributeID
        type = bufferInfo->buffer[offset];
        /// Data Element: Unsigned Integer 2 bytes (0x09)
        if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || ((type & 0x07) != DE_SIZE_16)) {
            LOG_ERROR(
                "[%{public}s][%{public}d] The type [0x%02x] of AttributeID is wrong.", __FUNCTION__, __LINE__, type);
            return BT_BAD_PARAM;
        }
        offset++;
        uint16_t attributeId = BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
        offset += SDP_UINT16_LENGTH;
        /// Attribute value
        BufferInfo parseBufferInfo = {
            .buffer = bufferInfo->buffer + offset,
            .length = bufferInfo->length
        };
        pos = SdpParseAttributeValue(&parseBufferInfo, attributeId, service);
        if (pos <= 0) {
            return BT_BAD_PARAM;
        }
        offset += pos;
    }

    return offset;
}

static int SdpParseAttributeList(const BtAddr *addr, uint16_t transactionId, Packet *data, SdpService *service)
{
    SdpClientRequest *request = NULL;
    Packet *packet = NULL;
    uint8_t *buffer = NULL;
    uint16_t length;
    int result;
    BufferInfo bufferInfo;

    request = SdpFindRequestByTransactionId(transactionId);
    if (request == NULL) {
        LOG_ERROR("point to NULL");
        return BT_BAD_PARAM;
    }
    if (request->assemblePacket != NULL) {
        packet = request->assemblePacket;
        PacketAssemble(packet, data);
    } else {
        packet = PacketRefMalloc(data);
    }

    length = PacketSize(packet);
    buffer = MEM_MALLOC.alloc(length);
    if (buffer == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(buffer, length, 0, length);
    PacketRead(packet, buffer, 0, length);
    bufferInfo.buffer = buffer;
    bufferInfo.length = length;
    result = SdpParseSingleAttributeList(&bufferInfo, service);

    MEM_MALLOC.free(buffer);
    PacketFree(packet);
    packet = NULL;
    request->assemblePacket = NULL;

    return result;
}

static void SdpPacketAndBufferFree(uint8_t *buffer, Packet *packet, SdpClientRequest *request)
{
    if (buffer != NULL) {
        MEM_MALLOC.free(buffer);
        buffer = NULL;
    }
    if (packet != NULL) {
        PacketFree(packet);
        packet = NULL;
    }
    if ((request != NULL) && (request->assemblePacket != NULL)) {
        request->assemblePacket = NULL;
    }
}

static uint16_t SdpParseAttributeListArray(
    const BtAddr *addr, uint16_t transactionId, Packet *data, SdpService *serviceArray)
{
    uint16_t serviceNum = 0;
    Packet *packet = NULL;
    uint32_t totalLength;
    uint32_t length = 0;
    uint16_t offset = 0;
    BufferInfo bufferInfo;

    SdpClientRequest *request = SdpFindRequestByTransactionId(transactionId);
    if ((request != NULL) && (request->assemblePacket != NULL)) {
        packet = request->assemblePacket;
        PacketAssemble(packet, data);
    } else {
        packet = PacketRefMalloc(data);
    }

    totalLength = PacketSize(packet);
    uint8_t *buffer = MEM_MALLOC.alloc(totalLength);
    if (buffer == NULL) {
        LOG_ERROR("buffer is NULL");
        return 0;
    }
    (void)memset_s(buffer, totalLength, 0, totalLength);
    PacketRead(packet, buffer, 0, totalLength);

    /// Descriptor type
    uint8_t type = buffer[offset];
    offset++;
    /// Descriptor size
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_DES) {
        LOG_ERROR("[%{public}s][%{public}d] There is wrong type [0x%02x] with attribute lists.",
            __FUNCTION__, __LINE__, type);
        SdpPacketAndBufferFree(buffer, packet, request);
        return 0;
    }
    /// Sequence length
    int pos = SdpGetLengthFromType(buffer + offset, type, &length);
    offset += pos;

    if (length != (totalLength - offset)) {
        LOG_ERROR("[%{public}s][%{public}d] total[%{public}d] current[%{public}d] offset[%{public}d]",
            __FUNCTION__, __LINE__, totalLength, length, offset);
        SdpPacketAndBufferFree(buffer, packet, request);
        return 0;
    }

    while (offset < totalLength) {
        if (serviceNum >= SDP_SERVICE_ARRAY_NUMBER) {
            LOG_INFO("The serviceNum is more than the specified number");
            break;
        }
        bufferInfo.buffer = buffer + offset;
        bufferInfo.length = totalLength;
        pos = SdpParseSingleAttributeList(&bufferInfo, &serviceArray[serviceNum]);
        if (pos <= 0) {
            SdpFreeServiceArray(serviceArray, serviceNum + 1);
            SdpPacketAndBufferFree(buffer, packet, request);
            return 0;
        }
        offset += pos;
        serviceNum++;
    }
    SdpPacketAndBufferFree(buffer, packet, request);

    return serviceNum;
}

static Packet *SdpParseResponseCommon(
    const BtAddr *addr, uint16_t lcid, uint16_t transactionId, Packet *packet, uint16_t length)
{
    uint8_t continuationStateLen;
    uint8_t continuationState[SDP_MAX_CONTINUATION_LEN + 1] = {0};

    if (length > (SDP_MAX_CONTINUATION_LEN + 1)) {
        LOG_ERROR("[%{public}s][%{public}d] Error length [%{public}d]", __FUNCTION__, __LINE__, length);
        SdpCallbackError(addr, transactionId);
        return NULL;
    }

    PacketExtractTail(packet, continuationState, length);
    continuationStateLen = continuationState[0];
    LOG_INFO("[%{public}s][%{public}d] continuation state length [%{public}d].",
        __FUNCTION__, __LINE__, continuationStateLen);
    if (continuationStateLen > SDP_MAX_CONTINUATION_LEN) {
        LOG_ERROR("[%{public}s][%{public}d] continuationStateLen [%{public}d] exceed",
            __FUNCTION__, __LINE__, continuationStateLen);
        SdpCallbackError(addr, transactionId);
        return NULL;
    }

    /// continuation state yes or no (is 0)
    if (continuationStateLen != 0) {
        SdpSendRequestForAssemblePacket(lcid, transactionId, packet, continuationState, continuationStateLen);
        return NULL;
    }
    return packet;
}

static void SdpParseErrorResponse(
    const BtAddr *addr, uint16_t transactionId, uint16_t parameterLength, const Packet *packet)
{
    if (PacketSize(packet) != parameterLength || parameterLength != SDP_UINT16_LENGTH) {
        LOG_ERROR("[%{public}s][%{public}d] Different length between [%u] and [%u].",
            __FUNCTION__,
            __LINE__,
            PacketSize(packet),
            parameterLength);
    } else {
        uint16_t errorCode;
        uint8_t buffer[2] = {0};
        PacketPayloadRead(packet, buffer, 0, parameterLength);
        errorCode = BE2H_16(*(uint16_t *)buffer);
        LOG_INFO("[%{public}s][%{public}d] Error Code [0x%04x].", __FUNCTION__, __LINE__, errorCode);
    }
    SdpCallbackError(addr, transactionId);
}

static void SdpParseSearchResponse(const BtAddr *addr, uint16_t lcid, uint16_t transactionId, Packet *packet)
{
    uint16_t totalServiceRecordCount;
    uint16_t currentServiceRecordCount;
    uint8_t buffer[2] = {0};
    uint16_t length;
    SdpServiceCallback callback;
    void *context = NULL;

    /// TotalServiceRecordCount
    PacketExtractHead(packet, buffer, SDP_UINT16_LENGTH);
    totalServiceRecordCount = BE2H_16(*(uint16_t *)buffer);
    if (totalServiceRecordCount == 0) {
        SdpCallbackError(addr, transactionId);
        return;
    }
    /// CurrentServiceRecordCount
    PacketExtractHead(packet, buffer, SDP_UINT16_LENGTH);
    currentServiceRecordCount = BE2H_16(*(uint16_t *)buffer);
    length = PacketSize(packet);
    if ((currentServiceRecordCount > totalServiceRecordCount) ||
        ((currentServiceRecordCount * SDP_SERVICE_RECORD_HANDLE_BYTE) >= length)) {
        LOG_ERROR("current[%{public}d], total[%{public}d] size[%{public}d]",
            totalServiceRecordCount, currentServiceRecordCount, length);
        SdpCallbackError(addr, transactionId);
        return;
    }

    /// ContinuationState
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    length = length - currentServiceRecordCount * SDP_SERVICE_RECORD_HANDLE_BYTE;
    packet = SdpParseResponseCommon(addr, lcid, transactionId, packet, length);
    if (packet == NULL) {
        return;
    }
    uint32_t *handleArray = (uint32_t *)MEM_MALLOC.alloc(SDP_UINT32_LENGTH * totalServiceRecordCount);
    if (handleArray == NULL) {
        LOG_ERROR("handleArray is NULL");
        return;
    }
    (void)memset_s(
        handleArray, SDP_UINT32_LENGTH * totalServiceRecordCount, 0, SDP_UINT32_LENGTH * totalServiceRecordCount);
    /// ServiceRecordHandleList
    uint16_t handleNum =
        SdpParseServiceRecordHandleList(addr, transactionId, totalServiceRecordCount, packet, handleArray);
    if (handleNum == 0) {
        LOG_ERROR("[%{public}s][%{public}d] Parse handle list failed.", __FUNCTION__, __LINE__);
        SdpCallbackError(addr, transactionId);
        MEM_MALLOC.free(handleArray);
        handleArray = NULL;
        return;
    }

    LOG_DEBUG("[%{public}s][%{public}d] ServiceSearchCallback start", __FUNCTION__, __LINE__);
    callback = SdpGetCallback(addr, transactionId, &context);
    if (callback.ServiceSearchCb != NULL) {
        callback.ServiceSearchCb(addr, handleArray, handleNum, context);
    }
    LOG_DEBUG("[%{public}s][%{public}d] ServiceSearchCallback end", __FUNCTION__, __LINE__);

    SdpRemoveRequestByTransactionId(transactionId);
    MEM_MALLOC.free(handleArray);
    handleArray = NULL;
}

static void SdpParseAttributeResponse(const BtAddr *addr, uint16_t lcid, uint16_t transactionId, Packet *packet)
{
    SdpService service;
    uint16_t attributeListByteCount;
    uint8_t buffer[2] = {0};
    uint16_t length;
    SdpServiceCallback callback;
    void *context = NULL;
    int ret;

    /// AttributeListByteCount <= mtu
    PacketExtractHead(packet, buffer, sizeof(buffer));
    length = PacketSize(packet);
    attributeListByteCount = BE2H_16(*(uint16_t *)buffer);
    if (attributeListByteCount >= length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong attribute list count [%{public}d]",
            __FUNCTION__, __LINE__, attributeListByteCount);
        SdpCallbackError(addr, transactionId);
        return;
    }

    /// ContinuationState
    length = length - attributeListByteCount;
    packet = SdpParseResponseCommon(addr, lcid, transactionId, packet, length);
    if (packet == NULL) {
        return;
    }

    (void)memset_s(&service, sizeof(SdpService), 0, sizeof(SdpService));
    ret = SdpParseAttributeList(addr, transactionId, packet, &service);
    if (ret <= 0) {
        LOG_ERROR("[%{public}s][%{public}d] Parse attribute list failed.", __FUNCTION__, __LINE__);
        SdpCallbackError(addr, transactionId);
        SdpFreeService(&service);
        return;
    }

    LOG_DEBUG("[%{public}s][%{public}d] ServiceAttributeCallback start", __FUNCTION__, __LINE__);
    callback = SdpGetCallback(addr, transactionId, &context);
    if (callback.ServiceAttributeCb != NULL) {
        callback.ServiceAttributeCb(addr, &service, context);
    }
    LOG_DEBUG("[%{public}s][%{public}d] ServiceAttributeCallback end", __FUNCTION__, __LINE__);

    SdpFreeService(&service);
    SdpRemoveRequestByTransactionId(transactionId);
}

NO_SANITIZE("cfi")
static void SdpParseSearchAttributeResponse(const BtAddr *addr, uint16_t lcid, uint16_t transactionId, Packet *packet)
{
    SdpService serviceArray[SDP_SERVICE_ARRAY_NUMBER] = {0};
    uint16_t serviceNum;
    uint16_t attributeListByteCount;
    uint8_t buffer[2] = {0};
    uint16_t length;
    SdpServiceCallback callback;
    void *context = NULL;

    /// AttributeListByteCount <= mtu
    PacketExtractHead(packet, buffer, sizeof(buffer));
    length = PacketSize(packet);
    attributeListByteCount = BE2H_16(*(uint16_t *)buffer);
    if (attributeListByteCount >= length) {
        SdpCallbackError(addr, transactionId);
        LOG_ERROR("[%{public}s][%{public}d] Wrong attribute list count [%{public}d]",
            __FUNCTION__, __LINE__, attributeListByteCount);
        return;
    }

    /// ContinuationState
    length = length - attributeListByteCount;
    packet = SdpParseResponseCommon(addr, lcid, transactionId, packet, length);
    if (packet == NULL) {
        return;
    }

    /// AttributeLists
    serviceNum = SdpParseAttributeListArray(addr, transactionId, packet, serviceArray);
    if (serviceNum == 0) {
        SdpCallbackError(addr, transactionId);
        return;
    }

    LOG_DEBUG("[%{public}s][%{public}d] ServiceSearchAttributeCallback start", __FUNCTION__, __LINE__);
    callback = SdpGetCallback(addr, transactionId, &context);
    if (callback.ServiceSearchAttributeCb != NULL) {
        callback.ServiceSearchAttributeCb(addr, serviceArray, serviceNum, context);
    }
    LOG_DEBUG("[%{public}s][%{public}d] ServiceSearchAttributeCallback end", __FUNCTION__, __LINE__);

    SdpFreeServiceArray(serviceArray, serviceNum);
    SdpRemoveRequestByTransactionId(transactionId);
}

static int SdpGetValue(BufferInfo *bufferInfo, uint32_t *value)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t pos;

    uint8_t type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || (length != SDP_UINT32_LENGTH)) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
        return BT_BAD_PARAM;
    }

    *value = BE2H_32(*(uint32_t *)(bufferInfo->buffer + offset));
    offset += pos;

    return offset;
}

static int SdpGetALLValue(BufferInfo *bufferInfo, uint32_t *data, SdpDataType *dataType)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    SdpDataType valueType = 0;
    uint32_t value = 0;

    /// Descriptor type
    uint8_t type = bufferInfo->buffer[offset];
    offset++;
    SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
        return BT_BAD_PARAM;
    }

    switch (length) {
        case SDP_UINT8_LENGTH:
            valueType = SDP_TYPE_UINT_8;
            value = bufferInfo->buffer[offset];
            break;
        case SDP_UINT16_LENGTH:
            valueType = SDP_TYPE_UINT_16;
            value = BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
            break;
        case SDP_UINT32_LENGTH:
            valueType = SDP_TYPE_UINT_32;
            value = BE2H_32(*(uint32_t *)(bufferInfo->buffer + offset));
            break;
        default:
            length = 0;
            break;
    }
    offset += length;

    *data = value;
    *dataType = valueType;
    return offset;
}

static int SdpGetString(BufferInfo *bufferInfo, char *name, SdpDescriptorType nameType)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t pos;

    uint8_t type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != nameType) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
        return BT_BAD_PARAM;
    }
    offset += pos;
    if (length >= SDP_MAX_ATTRIBUTE_LEN - 1) {
        length = SDP_MAX_ATTRIBUTE_LEN;
    }
    if (memcpy_s(name, length, bufferInfo->buffer + offset, length) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail", __FUNCTION__, __LINE__);
        return BT_OPERATION_FAILED;
    }
    name[length] = '\0';
    offset += length;

    return offset;
}

static int SdpGetServiceRecordHandle(BufferInfo *bufferInfo, SdpService *service)
{
    return SdpGetValue(bufferInfo, &service->handle);
}

static uint16_t SdpGetServiceClassIdList(BufferInfo *bufferInfo, SdpService *service)
{
    uint16_t classIdNumber = 0;
    uint32_t length = 0;
    uint16_t offset = 0;
    uint8_t type;
    uint16_t pos;
    uint32_t i = 0;

    service->classId = MEM_MALLOC.alloc(sizeof(BtUuid) * SDP_MAX_UUID_COUNT);
    if (service->classId == NULL) {
        LOG_ERROR("point to NULL");
        return 0;
    }
    (void)memset_s(service->classId, sizeof(BtUuid) * SDP_MAX_UUID_COUNT, 0, sizeof(BtUuid) * SDP_MAX_UUID_COUNT);

    type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return 0;
    }
    offset += pos;
    pos = offset;

    while (i < length) {
        if (classIdNumber >= SDP_MAX_UUID_COUNT) {
            offset = length + pos;
            break;
        }
        pos = SdpGetUuid(bufferInfo->buffer + offset, &service->classId[classIdNumber]);
        i += pos;
        offset += pos;
        classIdNumber++;
    }
    service->classIdNumber = classIdNumber;

    return offset;
}

static int SdpGetServiceRecordState(BufferInfo *bufferInfo, SdpService *service)
{
    return SdpGetValue(bufferInfo, &service->state);
}

static int SdpGetServiceId(uint8_t *buffer, SdpService *service)
{
    return SdpGetUuid(buffer, &service->serviceId);
}

static int SdpGetCommonProtocolDescriptorEachList(SdpProtocolDescriptor *descriptor, uint16_t descriptorNumber,
    uint32_t currentLength, BufferInfo *bufferInfo, int offset)
{
    uint16_t parameterNumber = 0;
    uint32_t length = 0;
    int pos;

    while (currentLength) {
        if (bufferInfo->buffer[offset] == 0x35) {
            uint8_t type = 0x35;
            SdpGetLengthFromType(bufferInfo->buffer + offset + 1, type, &length);
            if (bufferInfo->length < length) {
                LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
                return BT_BAD_PARAM;
            }
            offset = offset + length + SDP_UINT16_LENGTH;
            currentLength = currentLength - length - SDP_UINT16_LENGTH;
            continue;
        }
        BufferInfo parseBufferInfo = {
            .buffer = bufferInfo->buffer + offset,
            .length = bufferInfo->length
        };
        pos = SdpGetALLValue(&parseBufferInfo,
            &descriptor[descriptorNumber].parameter[parameterNumber].value,
            &descriptor[descriptorNumber].parameter[parameterNumber].type);
        if (pos <= 0) {
            return BT_BAD_PARAM;
        }
        currentLength = currentLength - pos;
        offset += pos;
        parameterNumber++;
    }
    descriptor[descriptorNumber].parameterNumber = parameterNumber;

    return offset;
}

static int SdpGetCommonProtocolDescriptorList(
    BufferInfo *bufferInfo, SdpProtocolDescriptor *descriptor, uint16_t *protocolDescriptorNumber)
{
    uint16_t descriptorNumber = 0;
    uint32_t totalLength = 0;
    uint32_t currentLength = 0;
    int offset = 0;
    uint16_t pos;
    uint8_t type = bufferInfo->buffer[offset];

    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &totalLength);
    if (bufferInfo->length < totalLength) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_DES) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
        return BT_BAD_PARAM;
    }
    offset += pos;

    while (totalLength) {
        if (descriptorNumber >= SDP_PROTOCOL_DESCRIPTOR_MAX) {
            break;
        }
        type = bufferInfo->buffer[offset];
        totalLength--;
        offset++;
        pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &currentLength);
        if (totalLength < currentLength) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
            return BT_BAD_PARAM;
        }
        if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_DES) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
            return BT_BAD_PARAM;
        }
        totalLength = totalLength - pos;
        offset += pos;
        pos = SdpGetUuid(bufferInfo->buffer + offset, &descriptor[descriptorNumber].protocolUuid);
        totalLength = totalLength - currentLength;
        currentLength = currentLength - pos;
        offset += pos;
        offset = SdpGetCommonProtocolDescriptorEachList(descriptor, descriptorNumber,
		    currentLength, bufferInfo, offset);
        if (offset <= 0) {
            return offset;
        }
        descriptorNumber++;
    }
    *protocolDescriptorNumber = descriptorNumber;

    return offset;
}

static int SdpGetProtocolDescriptorList(BufferInfo *bufferInfo, SdpService *service)
{
    int offset;

    service->descriptor = MEM_MALLOC.alloc(sizeof(SdpProtocolDescriptor) * SDP_PROTOCOL_DESCRIPTOR_MAX);
    if (service->descriptor == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->descriptor,
        sizeof(SdpProtocolDescriptor) * SDP_PROTOCOL_DESCRIPTOR_MAX,
        0,
        sizeof(SdpProtocolDescriptor) * SDP_PROTOCOL_DESCRIPTOR_MAX);

    offset = SdpGetCommonProtocolDescriptorList(bufferInfo, service->descriptor, &service->descriptorNumber);
    return offset;
}

static int SdpGetAdditionalProtocolDescriptorList(BufferInfo *bufferInfo, SdpService *service)
{
    uint16_t descriptorListNumber = 0;
    uint32_t descriptorLength = 0;
    uint16_t offset = 0;
    int pos;

    uint8_t type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &descriptorLength);
    if (bufferInfo->length < descriptorLength) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_DES) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
        return BT_BAD_PARAM;
    }
    offset += pos;
    pos = offset;
    service->descriptorList =
        MEM_MALLOC.alloc(sizeof(SdpAdditionalProtocolDescriptor) * SDP_PROTOCOL_DESCRIPTOR_LIST_MAX);
    if (service->descriptorList == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->descriptorList,
        sizeof(SdpAdditionalProtocolDescriptor) * SDP_PROTOCOL_DESCRIPTOR_LIST_MAX,
        0,
        sizeof(SdpAdditionalProtocolDescriptor) * SDP_PROTOCOL_DESCRIPTOR_LIST_MAX);

    while (descriptorLength) {
        if (descriptorListNumber >= SDP_PROTOCOL_DESCRIPTOR_LIST_MAX) {
            offset = descriptorLength + pos;
            break;
        }
        BufferInfo parseBufferInfo = {
            .buffer = bufferInfo->buffer + offset,
            .length = bufferInfo->length
        };
        pos = SdpGetCommonProtocolDescriptorList(&parseBufferInfo,
            service->descriptorList[descriptorListNumber].parameter,
            &service->descriptorList[descriptorListNumber].protocolDescriptorNumber);
        if (pos <= 0) {
            return BT_BAD_PARAM;
        }
        offset += pos;
        descriptorLength = descriptorLength - pos;
        descriptorListNumber++;
    }
    service->descriptorListNumber = descriptorListNumber;

    return offset;
}

static int SdpGetBrowseGroupList(BufferInfo *bufferInfo, SdpService *service)
{
    uint16_t browseUuidNumber = 0;
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t pos;

    uint8_t type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    offset += pos;

    service->browseUuid = MEM_MALLOC.alloc(sizeof(BtUuid) * SDP_MAX_UUID_COUNT);
    if (service->browseUuid == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->browseUuid, sizeof(BtUuid) * SDP_MAX_UUID_COUNT, 0, sizeof(BtUuid) * SDP_MAX_UUID_COUNT);

    while (length) {
        if (browseUuidNumber >= SDP_MAX_UUID_COUNT) {
            break;
        }
        pos = SdpGetUuid(bufferInfo->buffer + offset, &service->browseUuid[browseUuidNumber]);
        length = length - pos;
        offset += pos;
        browseUuidNumber++;
    }
    service->browseUuidNumber = browseUuidNumber;

    return offset;
}

static int SdpGetLanguageBaseAttributeIdList(BufferInfo *bufferInfo, SdpService *service)
{
    uint16_t baseAttributeIdNumber = 0;
    uint32_t length = 0;
    uint16_t offset = 0;
    uint8_t type = bufferInfo->buffer[offset];

    offset++;
    uint16_t pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_DES) {
        return BT_BAD_PARAM;
    }
    offset += pos;
    if ((length / SDP_LANGUAGE_ATTRIBUTE_LENGTH) == 0) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong count [%{public}d]", __FUNCTION__, __LINE__, length);
        return BT_BAD_PARAM;
    }
    service->baseAttributeId = MEM_MALLOC.alloc(sizeof(SdpLanguageBaseAttributeId) * SDP_LANGUAGE_ATTRIBUTE_MAX);
    if (service->baseAttributeId == NULL) {
        LOG_ERROR("baseAttributeId is NULL");
        return BT_NO_MEMORY;
    }
    uint16_t setLength = sizeof(SdpLanguageBaseAttributeId) * SDP_LANGUAGE_ATTRIBUTE_MAX;
    (void)memset_s(service->baseAttributeId, setLength, 0, setLength);

    while (length) {
        /// Language Code
        type = bufferInfo->buffer[offset];
        offset++;
        if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || ((type & 0x07) != DE_SIZE_16)) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
            return BT_BAD_PARAM;
        }
        service->baseAttributeId[baseAttributeIdNumber].languageIdentifier =
            BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
        offset += SDP_UINT16_LENGTH;

        /// Language Encoding
        type = bufferInfo->buffer[offset];
        offset++;
        if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || ((type & 0x07) != DE_SIZE_16)) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
            return BT_BAD_PARAM;
        }
        service->baseAttributeId[baseAttributeIdNumber].characterEncodingIdentifier =
            BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
        offset += SDP_UINT16_LENGTH;

        /// Attribute Base
        type = bufferInfo->buffer[offset];
        offset++;
        if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || ((type & 0x07) != DE_SIZE_16)) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
            return BT_BAD_PARAM;
        }
        service->baseAttributeId[baseAttributeIdNumber].baseAttributeId =
            BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
        offset += SDP_UINT16_LENGTH;

        length = length - SDP_LANGUAGE_ATTRIBUTE_LENGTH;
        baseAttributeIdNumber++;
    }
    service->baseAttributeIdNumber = baseAttributeIdNumber;

    return offset;
}

static int SdpGetServiceInfoTimeToLive(BufferInfo *bufferInfo, SdpService *service)
{
    return SdpGetValue(bufferInfo, &service->serviceInfoTimeToLive);
}

static int SdpGetServiceAvailability(uint8_t *buffer, SdpService *service)
{
    uint16_t offset = 0;
    uint8_t type = buffer[offset];

    offset++;
    if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || ((type & 0x07) != DE_SIZE_8)) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
        return BT_BAD_PARAM;
    }
    service->serviceAvailability = buffer[offset];
    offset++;

    return offset;
}

static int SdpGetBluetoothProfileDescriptorList(BufferInfo *bufferInfo, SdpService *service)
{
    uint16_t profileDescriptorNumber = 0;
    uint32_t length = 0;
    uint32_t currentLength = 0;
    uint16_t offset = 0;
    uint8_t type = bufferInfo->buffer[offset];

    offset++;
    uint16_t pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_DES) {
        return BT_BAD_PARAM;
    }
    offset += pos;

    service->profileDescriptor = MEM_MALLOC.alloc(sizeof(SdpProfileDescriptor) * SDP_PROTOCOL_PARAMETER_MAX_COUNT);
    if (service->profileDescriptor == NULL) {
        LOG_ERROR("profileDescriptor is NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->profileDescriptor,
        sizeof(SdpProfileDescriptor) * SDP_PROTOCOL_PARAMETER_MAX_COUNT,
        0,
        sizeof(SdpProfileDescriptor) * SDP_PROTOCOL_PARAMETER_MAX_COUNT);

    while (length) {
        /// UUID
        type = *(bufferInfo->buffer + offset);
        length--;
        offset++;
        pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &currentLength);
        if (bufferInfo->length < currentLength) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
            return BT_BAD_PARAM;
        }
        if ((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_DES) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
            return BT_BAD_PARAM;
        }
        length = length - pos;
        offset += pos;
        pos = SdpGetUuid(bufferInfo->buffer + offset,
            &service->profileDescriptor[profileDescriptorNumber].profileUuid);
        length = length - currentLength;
        offset += pos;
        if (currentLength == pos) {
            profileDescriptorNumber++;
            continue;
        }

        /// Protocol Version
        type = *(bufferInfo->buffer + offset);
        offset++;
        pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &currentLength);
        if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || (currentLength != SDP_UINT16_LENGTH)) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong type [0x%02x]", __FUNCTION__, __LINE__, type);
            return BT_BAD_PARAM;
        }
        service->profileDescriptor[profileDescriptorNumber].versionNumber =
            BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
        offset += pos;

        profileDescriptorNumber++;
    }
    service->profileDescriptorNumber = profileDescriptorNumber;

    return offset;
}

static int SdpGetDocumentationUrl(BufferInfo *bufferInfo, SdpService *service)
{
    service->documentationUrl = MEM_MALLOC.alloc(SDP_MAX_ATTRIBUTE_LEN);
    if (service->documentationUrl == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->documentationUrl, SDP_MAX_ATTRIBUTE_LEN, 0, SDP_MAX_ATTRIBUTE_LEN);

    return SdpGetString(bufferInfo, service->documentationUrl, DE_TYPE_URL);
}

static int SdpGetClientExecutableUrl(BufferInfo *bufferInfo, SdpService *service)
{
    service->clientExecutableUrl = MEM_MALLOC.alloc(SDP_MAX_ATTRIBUTE_LEN);
    if (service->clientExecutableUrl == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->clientExecutableUrl, SDP_MAX_ATTRIBUTE_LEN, 0, SDP_MAX_ATTRIBUTE_LEN);

    return SdpGetString(bufferInfo, service->clientExecutableUrl, DE_TYPE_URL);
}

static int SdpGetIconUrl(BufferInfo *bufferInfo, SdpService *service)
{
    service->iconUrl = MEM_MALLOC.alloc(SDP_MAX_ATTRIBUTE_LEN);
    if (service->iconUrl == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->iconUrl, SDP_MAX_ATTRIBUTE_LEN, 0, SDP_MAX_ATTRIBUTE_LEN);

    return SdpGetString(bufferInfo, service->iconUrl, DE_TYPE_URL);
}

static int SdpGetServiceName(BufferInfo *bufferInfo, SdpService *service)
{
    service->serviceName = MEM_MALLOC.alloc(SDP_MAX_ATTRIBUTE_LEN);
    if (service->serviceName == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->serviceName, SDP_MAX_ATTRIBUTE_LEN, 0, SDP_MAX_ATTRIBUTE_LEN);

    return SdpGetString(bufferInfo, service->serviceName, DE_TYPE_STRING);
}

static int SdpGetServiceDescription(BufferInfo *bufferInfo, SdpService *service)
{
    service->serviceDescription = MEM_MALLOC.alloc(SDP_MAX_ATTRIBUTE_LEN);
    if (service->serviceDescription == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->serviceDescription, SDP_MAX_ATTRIBUTE_LEN, 0, SDP_MAX_ATTRIBUTE_LEN);

    return SdpGetString(bufferInfo, service->serviceDescription, DE_TYPE_STRING);
}

static int SdpGetProviderName(BufferInfo *bufferInfo, SdpService *service)
{
    service->providerName = MEM_MALLOC.alloc(SDP_MAX_ATTRIBUTE_LEN);
    if (service->providerName == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->providerName, SDP_MAX_ATTRIBUTE_LEN, 0, SDP_MAX_ATTRIBUTE_LEN);

    return SdpGetString(bufferInfo, service->providerName, DE_TYPE_STRING);
}

static int SdpGetSequenceAttribute(uint16_t attributeId, BufferInfo *bufferInfo, SdpService *service)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t pos;
    uint8_t type;

    if (service->sequenceAttributeNumber >= SDP_SEQUENCE_ATTRIBUTE_COUNT) {
        return BT_BAD_PARAM;
    }
    type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }
    offset += pos;

    service->sequenceAttribute[service->sequenceAttributeNumber].attributeValue = MEM_MALLOC.alloc(length);
    if (service->sequenceAttribute[service->sequenceAttributeNumber].attributeValue == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->sequenceAttribute[service->sequenceAttributeNumber].attributeValue, length, 0, length);

    service->sequenceAttribute[service->sequenceAttributeNumber].attributeId = attributeId;
    service->sequenceAttribute[service->sequenceAttributeNumber].attributeValueLength = length;
    (void)memcpy_s(service->sequenceAttribute[service->sequenceAttributeNumber].attributeValue,
        length, bufferInfo->buffer + offset, length);
    service->sequenceAttributeNumber++;
    offset += length;

    return offset;
}

static int SdpGetAttributeForBool(uint8_t *buffer, uint8_t size, SdpService *service)
{
    uint16_t offset = 0;
    uint8_t data;

    if (service->attributeNumber >= SDP_ATTRIBUTE_COUNT) {
        return BT_BAD_PARAM;
    }
    if (size != DE_SIZE_8) {
        return BT_BAD_PARAM;
    }

    data = buffer[1];
    service->attribute[service->attributeNumber].type = SDP_TYPE_BOOL;
    service->attribute[service->attributeNumber].attributeValueLength = SDP_UINT8_LENGTH;
    service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_UINT8_LENGTH);
    if (service->attribute[service->attributeNumber].attributeValue == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(service->attribute[service->attributeNumber].attributeValue, SDP_UINT8_LENGTH, 0, SDP_UINT8_LENGTH);
    (void)memcpy_s(
        service->attribute[service->attributeNumber].attributeValue, SDP_UINT8_LENGTH, &data, SDP_UINT8_LENGTH);
    offset += 1 + SDP_UINT8_LENGTH;

    return offset;
}

static int SdpGetAttributeForUint(uint8_t *buffer, uint8_t size, SdpService *service)
{
    uint16_t offset = 0;

    if (service->attributeNumber >= SDP_ATTRIBUTE_COUNT) {
        return BT_BAD_PARAM;
    }
    switch (size) {
        case DE_SIZE_8:
            service->attribute[service->attributeNumber].type = SDP_TYPE_UINT_8;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_UINT8_LENGTH;
            uint8_t data1 = buffer[1];
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_UINT8_LENGTH);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memset_s(
                service->attribute[service->attributeNumber].attributeValue, SDP_UINT8_LENGTH, 0, SDP_UINT8_LENGTH);
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                SDP_UINT8_LENGTH,
                &data1,
                SDP_UINT8_LENGTH);
            offset += 1 + SDP_UINT8_LENGTH;
            break;
        case DE_SIZE_16:
            service->attribute[service->attributeNumber].type = SDP_TYPE_UINT_16;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_UINT16_LENGTH;
            uint16_t data2 = BE2H_16(*(uint16_t *)(buffer + 1));
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_UINT16_LENGTH);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memset_s(
                service->attribute[service->attributeNumber].attributeValue, SDP_UINT16_LENGTH, 0, SDP_UINT16_LENGTH);
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                SDP_UINT16_LENGTH,
                &data2,
                SDP_UINT16_LENGTH);
            offset += 1 + SDP_UINT16_LENGTH;
            break;
        case DE_SIZE_32:
            service->attribute[service->attributeNumber].type = SDP_TYPE_UINT_32;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_UINT32_LENGTH;
            uint32_t data3 = BE2H_32(*(uint32_t *)(buffer + 1));
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_UINT32_LENGTH);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memset_s(
                service->attribute[service->attributeNumber].attributeValue, SDP_UINT32_LENGTH, 0, SDP_UINT32_LENGTH);
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                SDP_UINT32_LENGTH,
                &data3,
                SDP_UINT32_LENGTH);
            offset += 1 + SDP_UINT32_LENGTH;
            break;
        default:
            return BT_BAD_PARAM;
    }

    return offset;
}

static int SdpGetAttributeForInt(uint8_t *buffer, uint8_t size, SdpService *service)
{
    uint16_t offset = 0;

    if (service->attributeNumber >= SDP_ATTRIBUTE_COUNT) {
        return BT_BAD_PARAM;
    }
    switch (size) {
        case DE_SIZE_8:
            service->attribute[service->attributeNumber].type = SDP_TYPE_INT_8;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_INT8_LENGTH;
            uint8_t data1 = buffer[1];
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_INT8_LENGTH);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(
                service->attribute[service->attributeNumber].attributeValue, SDP_INT8_LENGTH, &data1, SDP_INT8_LENGTH);
            offset += 1 + SDP_INT8_LENGTH;
            break;
        case DE_SIZE_16:
            service->attribute[service->attributeNumber].type = SDP_TYPE_INT_16;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_INT16_LENGTH;
            uint16_t data2 = BE2H_16(*(uint16_t *)(buffer + 1));
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_INT16_LENGTH);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                SDP_INT16_LENGTH,
                &data2,
                SDP_INT16_LENGTH);
            offset += 1 + SDP_INT16_LENGTH;
            break;
        case DE_SIZE_32:
            service->attribute[service->attributeNumber].type = SDP_TYPE_INT_32;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_INT32_LENGTH;
            uint32_t data3 = BE2H_32(*(uint32_t *)(buffer + 1));
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_INT32_LENGTH);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                SDP_INT32_LENGTH,
                &data3,
                SDP_INT32_LENGTH);
            offset += 1 + SDP_INT32_LENGTH;
            break;
        default:
            return BT_BAD_PARAM;
    }

    return offset;
}

static int SdpGetAttributeForUuid(uint8_t *buffer, uint8_t size, SdpService *service)
{
    uint16_t offset = 0;

    if (service->attributeNumber >= SDP_ATTRIBUTE_COUNT) {
        return BT_BAD_PARAM;
    }
    switch (size) {
        case DE_SIZE_16:
            service->attribute[service->attributeNumber].type = SDP_TYPE_UUID_16;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_UUID16_LENGTH;
            uint16_t data1 = BE2H_16(*(uint16_t *)(buffer + 1));
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_UUID16_LENGTH);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                SDP_UUID16_LENGTH,
                &data1,
                SDP_UUID16_LENGTH);
            offset += 1 + SDP_UUID16_LENGTH;
            break;
        case DE_SIZE_32:
            service->attribute[service->attributeNumber].type = SDP_TYPE_UUID_32;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_UUID32_LENGTH;
            uint32_t data2 = BE2H_32(*(uint32_t *)(buffer + 1));
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(SDP_UUID32_LENGTH);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                SDP_UUID32_LENGTH,
                &data2,
                SDP_UUID32_LENGTH);
            offset += 1 + SDP_UUID32_LENGTH;
            break;
        case DE_SIZE_128:
            service->attribute[service->attributeNumber].type = SDP_TYPE_UUID_128;
            service->attribute[service->attributeNumber].attributeValueLength = SDP_UUID128_LENGTH;
            SdpReverseForBigEndian(
                buffer + 1, service->attribute[service->attributeNumber].attributeValue, SDP_UUID128_LENGTH);
            offset += 1 + SDP_UUID128_LENGTH;
            break;
        default:
            return BT_BAD_PARAM;
    }

    return offset;
}

static int SdpGetAttributeForString(uint8_t *buffer, uint8_t size, SdpService *service)
{
    uint16_t offset = 0;
    uint16_t length;

    if (service->attributeNumber >= SDP_ATTRIBUTE_COUNT) {
        return BT_BAD_PARAM;
    }
    switch (size) {
        case DE_SIZE_VAR_16:
            length = BE2H_16(*(uint16_t *)(buffer + 1));
            service->attribute[service->attributeNumber].attributeValueLength = length;
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(length);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                length,
                buffer + 1 + SDP_UINT16_LENGTH,
                length);
            offset += length + SDP_UINT16_LENGTH + 1;
            break;
        case DE_SIZE_VAR_8:
            length = buffer[1];
            service->attribute[service->attributeNumber].attributeValueLength = length;
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(length);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                length,
                buffer + 1 + SDP_UINT8_LENGTH,
                length);
            offset += length + SDP_UINT8_LENGTH + 1;
            break;
        default:
            return BT_BAD_PARAM;
    }
    service->attribute[service->attributeNumber].type = SDP_TYPE_TEXT;

    return offset;
}

static int SdpGetAttributeForUrl(uint8_t *buffer, uint8_t size, SdpService *service)
{
    uint16_t offset = 0;
    uint16_t length;

    if (service->attributeNumber >= SDP_ATTRIBUTE_COUNT) {
        return BT_BAD_PARAM;
    }
    switch (size) {
        case DE_SIZE_VAR_8:
            length = buffer[1];
            service->attribute[service->attributeNumber].attributeValueLength = length;
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(length);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                length,
                buffer + 1 + SDP_UINT8_LENGTH,
                length);
            offset += length + SDP_UINT8_LENGTH + 1;
            break;
        case DE_SIZE_VAR_16:
            length = BE2H_16(*(uint16_t *)(buffer + 1));
            service->attribute[service->attributeNumber].attributeValueLength = length;
            service->attribute[service->attributeNumber].attributeValue = MEM_MALLOC.alloc(length);
            if (service->attribute[service->attributeNumber].attributeValue == NULL) {
                LOG_ERROR("attributeValue is NULL");
                return BT_NO_MEMORY;
            }
            (void)memcpy_s(service->attribute[service->attributeNumber].attributeValue,
                length,
                buffer + 1 + SDP_UINT16_LENGTH,
                length);
            offset += length + SDP_UINT16_LENGTH + 1;
            break;
        default:
            return BT_BAD_PARAM;
    }
    service->attribute[service->attributeNumber].type = SDP_TYPE_URL;

    return offset;
}

static int SdpGetAttribute(uint16_t attributeId, BufferInfo *bufferInfo, SdpService *service)
{
    int offset = 0;
    // Attribute type
    uint8_t type = bufferInfo->buffer[0] >> SDP_DESCRIPTOR_SIZE_BIT;
    uint8_t size = bufferInfo->buffer[0] & 0x07;

    switch (type) {
        case DE_TYPE_UINT:
            offset = SdpGetAttributeForUint(bufferInfo->buffer, size, service);
            break;
        case DE_TYPE_INT:
            offset = SdpGetAttributeForInt(bufferInfo->buffer, size, service);
            break;
        case DE_TYPE_UUID:
            offset = SdpGetAttributeForUuid(bufferInfo->buffer, size, service);
            break;
        case DE_TYPE_STRING:
            offset = SdpGetAttributeForString(bufferInfo->buffer, size, service);
            break;
        case DE_TYPE_BOOL:
            offset = SdpGetAttributeForBool(bufferInfo->buffer, size, service);
            break;
        case DE_TYPE_DES:
        case DE_TYPE_DEA:
            offset = SdpGetSequenceAttribute(attributeId, bufferInfo, service);
            return offset;
        case DE_TYPE_URL:
            offset = SdpGetAttributeForUrl(bufferInfo->buffer, size, service);
            break;
        default:
            break;
    }
    if (offset <= 0) {
        return BT_BAD_PARAM;
    }
    service->attribute[service->attributeNumber].attributeId = attributeId;
    service->attributeNumber++;

    return offset;
}

static int SdpParseAttributeValue(BufferInfo *bufferInfo, uint16_t attributeId, SdpService *service)
{
    int offset;
    LOG_INFO("[%{public}s][%{public}d] attributeId [0x%04x]", __FUNCTION__, __LINE__, attributeId);

    if (attributeId == SDP_ATTRIBUTE_SERVICE_RECORD_HANDLE) {
        offset = SdpGetServiceRecordHandle(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST) {
        offset = SdpGetServiceClassIdList(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_SERVICE_RECORD_STATE) {
        offset = SdpGetServiceRecordState(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_SERVICE_ID) {
        offset = SdpGetServiceId(bufferInfo->buffer, service);
    } else if (attributeId == SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST) {
        offset = SdpGetProtocolDescriptorList(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_ADDITIONAL_PROTOCOL_DESCRIPTOR_LIST) {
        offset = SdpGetAdditionalProtocolDescriptorList(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_BROWSE_GROUP_LIST) {
        offset = SdpGetBrowseGroupList(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST) {
        offset = SdpGetLanguageBaseAttributeIdList(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_SERVICE_INFO_TIME_TO_LIVE) {
        offset = SdpGetServiceInfoTimeToLive(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_SERVICE_AVAILABILITY) {
        offset = SdpGetServiceAvailability(bufferInfo->buffer, service);
    } else if (attributeId == SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST) {
        offset = SdpGetBluetoothProfileDescriptorList(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_DOCUMENTATION_URL) {
        offset = SdpGetDocumentationUrl(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_CLIENT_EXECUTABLE_URL) {
        offset = SdpGetClientExecutableUrl(bufferInfo, service);
    } else if (attributeId == SDP_ATTRIBUTE_ICON_URL) {
        offset = SdpGetIconUrl(bufferInfo, service);
    } else if (attributeId == (SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE + SDP_ATTRIBUTE_SERVICE_NAME)) {
        offset = SdpGetServiceName(bufferInfo, service);
    } else if (attributeId == (SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE + SDP_ATTRIBUTE_DESCRIPTOR)) {
        offset = SdpGetServiceDescription(bufferInfo, service);
    } else if (attributeId == (SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE + SDP_ATTRIBUTE_PROVIDER_NAME)) {
        offset = SdpGetProviderName(bufferInfo, service);
    } else {
        offset = SdpGetAttribute(attributeId, bufferInfo, service);
    }
    return offset;
}

static void SdpFreeServiceRemained(SdpService *service)
{
    if (service->serviceName) {
        MEM_MALLOC.free(service->serviceName);
        service->serviceName = NULL;
    }
    if (service->serviceDescription) {
        MEM_MALLOC.free(service->serviceDescription);
        service->serviceDescription = NULL;
    }
    if (service->providerName) {
        MEM_MALLOC.free(service->providerName);
        service->providerName = NULL;
    }
    if (service->attribute) {
        for (int i = 0; i < service->attributeNumber; i++) {
            if (service->attribute[i].attributeValue) {
                MEM_MALLOC.free(service->attribute[i].attributeValue);
                service->attribute[i].attributeValue = NULL;
            }
        }
        MEM_MALLOC.free(service->attribute);
        service->attribute = NULL;
    }
    if (service->sequenceAttribute) {
        for (int i = 0; i < service->sequenceAttributeNumber; i++) {
            if (service->sequenceAttribute[i].attributeValue) {
                MEM_MALLOC.free(service->sequenceAttribute[i].attributeValue);
                service->sequenceAttribute[i].attributeValue = NULL;
            }
        }
        MEM_MALLOC.free(service->sequenceAttribute);
        service->sequenceAttribute = NULL;
    }
}

static void SdpFreeService(SdpService *service)
{
    if (service->classId) {
        MEM_MALLOC.free(service->classId);
        service->classId = NULL;
    }
    if (service->descriptor) {
        MEM_MALLOC.free(service->descriptor);
        service->descriptor = NULL;
    }
    if (service->descriptorList) {
        MEM_MALLOC.free(service->descriptorList);
        service->descriptorList = NULL;
    }
    if (service->browseUuid) {
        MEM_MALLOC.free(service->browseUuid);
        service->browseUuid = NULL;
    }
    if (service->baseAttributeId) {
        MEM_MALLOC.free(service->baseAttributeId);
        service->baseAttributeId = NULL;
    }
    if (service->profileDescriptor) {
        MEM_MALLOC.free(service->profileDescriptor);
        service->profileDescriptor = NULL;
    }
    if (service->documentationUrl) {
        MEM_MALLOC.free(service->documentationUrl);
        service->documentationUrl = NULL;
    }
    if (service->clientExecutableUrl) {
        MEM_MALLOC.free(service->clientExecutableUrl);
        service->clientExecutableUrl = NULL;
    }
    if (service->iconUrl) {
        MEM_MALLOC.free(service->iconUrl);
        service->iconUrl = NULL;
    }
    SdpFreeServiceRemained(service);
}

static void SdpFreeServiceArray(SdpService *serviceArray, uint16_t serviceNum)
{
    for (int i = 0; i < serviceNum; i++) {
        SdpFreeService(&serviceArray[i]);
    }
}
