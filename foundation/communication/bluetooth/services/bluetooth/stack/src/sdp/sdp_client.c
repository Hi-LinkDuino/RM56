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

#include "sdp_client.h"

#include <string.h>

#include "allocator.h"

#include "sdp_client_parse.h"
#include "sdp_connect.h"
#include "sdp_util.h"

#include "bt_endian.h"
#include "log.h"
#include "packet.h"

#define SDP_UUID_ATTRIBUTE_LENGTH 17
#define SDP_SEARCH_LENGTH 5
#define SDP_ATTRIBUTE_LENGTH 15
#define SDP_SEARCH_ATTRIBUTE_LENGTH 20
#define SDP_BROWSE_LENGTH 8

/**
 * @brief   The function is used to initialize sdp client.
 *
 */
void SdpInitializeClient()
{
    SdpCreateConnectList();
    SdpCreateRequestList();
}

/**
 * @brief The function is used to finalize sdp client.
 *
 */
void SdpFinalizeClient()
{
    SdpDestroyConnectList();
    SdpDestroyRequestList();
}

static Packet *SdpBuildPacket(const uint8_t *buffer, uint16_t length)
{
    Packet *packet = NULL;

    packet = PacketMalloc(0, 0, length);
    PacketPayloadWrite(packet, buffer, 0, length);

    return packet;
}

static uint16_t SdpBuildAttributeList(SdpAttributeIdList attributeIdList, uint8_t *buffer, uint16_t offset)
{
    if (attributeIdList.type == SDP_TYPE_RANGE) {
        uint8_t idRange[] = {0x35, 0x05, 0x0A, 0x00, 0x00, 0x00, 0x00};
        uint16_t pos = 1;
        /// Range of Attribute IDs (All Attribute: 0x0000 - 0xffff)
        pos += SDP_UINT16_LENGTH;
        *(uint16_t *)(idRange + pos) = H2BE_16(attributeIdList.attributeIdRange.start);
        pos += SDP_UINT16_LENGTH;
        *(uint16_t *)(idRange + pos) = H2BE_16(attributeIdList.attributeIdRange.end);
        pos += SDP_UINT16_LENGTH;
        if (memcpy_s(buffer + offset, pos, idRange, pos) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] memcpy_s failed", __FUNCTION__, __LINE__);
            return offset;
        }
        offset += pos;
    } else {
        if (attributeIdList.attributeIdList.attributeIdNumber > SDP_ATTRIBUTE_ID_LIST_MAX_COUNT) {
            attributeIdList.attributeIdList.attributeIdNumber = SDP_ATTRIBUTE_ID_LIST_MAX_COUNT;
        }
        /// Data Element Sequence uint8 (0x35)
        buffer[offset] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
        offset++;
        /// Data Element Var Size
        buffer[offset] = attributeIdList.attributeIdList.attributeIdNumber * (SDP_UINT16_LENGTH + 1);
        offset++;
        /// Each Attribute ID
        for (int i = 0; i < attributeIdList.attributeIdList.attributeIdNumber; i++) {
            /// Data Element Unsigned Integer 2 bytes (0x09)
            buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
            offset++;
            /// Data Value 16-bit unsigned integer
            *(uint16_t *)(buffer + offset) = H2BE_16(attributeIdList.attributeIdList.attributeId[i]);
            offset += SDP_UINT16_LENGTH;
        }
    }

    return offset;
}

static uint16_t SdpCreateSearchRequest(const BtUuid *uuidArray, uint16_t uuidNum, uint8_t *buffer)
{
    uint16_t offset = 0;

    // Data Element: Sequence uint8 (0x35)
    buffer[offset] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
    offset += SDP_UINT16_LENGTH;
    /// ServiceSearchPattern (min 1, max 12)
    for (int i = 0; i < uuidNum; i++) {
        offset = SdpAddAttributeForUuid(buffer, offset, &uuidArray[i]);
    }
    /// ParameterLength
    buffer[1] = offset - SDP_UINT16_LENGTH;
    /// MaximumServiceRecordCount - uint16_t (0x0001 - 0xFFFF)
    *(uint16_t *)(buffer + offset) = H2BE_16(SDP_MAX_RECORD_HANDLE_COUNT);
    offset += SDP_UINT16_LENGTH;

    return offset;
}

static uint16_t SdpCreateAttributeRequest(uint32_t handle, SdpAttributeIdList attributeIdList, uint8_t *buffer)
{
    uint16_t offset = 0;

    /// ServiceRecordHandle
    *(uint32_t *)(buffer + offset) = H2BE_32(handle);
    offset += SDP_SERVICE_RECORD_HANDLE_BYTE;
    /// MaximumAttributeByteCount - uint16_t (0x0001 - 0xFFFF)
    *(uint16_t *)(buffer + offset) = H2BE_16(SDP_MAX_ATTRIBUTE_BYTE_COUNT);
    offset += SDP_UINT16_LENGTH;
    /// AttributeIDList
    offset = SdpBuildAttributeList(attributeIdList, buffer, offset);

    return offset;
}

static uint16_t SdpCreateSearchAttributeRequest(
    const BtUuid *uuidArray, uint16_t uuidNum, SdpAttributeIdList attributeIdList, uint8_t *buffer)
{
    uint16_t offset = 0;

    // Data Element: Sequence uint8 (0x35)
    buffer[offset] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
    offset += SDP_UINT16_LENGTH;
    /// ServiceSearchPattern (min 1, max 12)
    for (int i = 0; i < uuidNum; i++) {
        offset = SdpAddAttributeForUuid(buffer, offset, &uuidArray[i]);
    }
    /// ParameterLength
    buffer[1] = offset - SDP_UINT16_LENGTH;
    /// MaximumAttributeByteCount - uint16_t (0x0001 - 0xFFFF)
    *(uint16_t *)(buffer + offset) = H2BE_16(SDP_MAX_ATTRIBUTE_BYTE_COUNT);
    offset += SDP_UINT16_LENGTH;
    /// AttributeIDList
    offset = SdpBuildAttributeList(attributeIdList, buffer, offset);

    return offset;
}

static uint16_t SdpCreateServiceBrowseRequest(uint8_t *buffer)
{
    uint16_t length;
    BtUuid uuid;

    uuid.type = BT_UUID_16;
    uuid.uuid16 = SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID;
    length = SdpCreateSearchRequest(&uuid, 1, buffer);

    return length;
}

int SdpServiceSearch(const BtAddr *addr, const SdpUuid *uuidArray, void *context,
    void (*serviceSearchCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context))
{
    LOG_INFO("[%{public}s][%{public}d] uuidNum = [%hu]", __FUNCTION__, __LINE__, uuidArray->uuidNum);
    SdpClientRequest *request = NULL;
    uint8_t *buffer = NULL;
    uint16_t length;
    int ret;

    if (SdpGetEnableState() == false) {
        return BT_BAD_STATUS;
    }

    /// Create service search request
    request = MEM_MALLOC.alloc(sizeof(SdpClientRequest));
    (void)memset_s(request, sizeof(SdpClientRequest), 0, sizeof(SdpClientRequest));
    buffer = MEM_MALLOC.alloc(uuidArray->uuidNum * SDP_UUID_ATTRIBUTE_LENGTH + SDP_SEARCH_LENGTH);
    (void)memset_s(buffer,
        (uuidArray->uuidNum * SDP_UUID_ATTRIBUTE_LENGTH + SDP_SEARCH_LENGTH),
        0,
        (uuidArray->uuidNum * SDP_UUID_ATTRIBUTE_LENGTH + SDP_SEARCH_LENGTH));

    length = SdpCreateSearchRequest(uuidArray->uuid, uuidArray->uuidNum, buffer);
    if (length == 0) {
        LOG_ERROR("[%{public}s][%{public}d] Create search request failed", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }

    (void)memcpy_s(&request->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    request->pduId = SDP_SERVICE_SEARCH_REQUEST;
    request->transactionId = SdpGetTransactionId();
    request->resentFlag = false;
    request->packetState = SDP_PACKET_WAIT;
    request->packet = SdpBuildPacket(buffer, length);
    request->assemblePacket = NULL;
    request->callback.ServiceSearchCb = serviceSearchCb;
    request->context = context;

    ret = SdpClientConnect(request);
    MEM_MALLOC.free(buffer);

    return ret;
}

int SdpServiceAttribute(const BtAddr *addr, uint32_t handle, SdpAttributeIdList attributeIdList, void *context,
    void (*serviceAttributeCb)(const BtAddr *addr, const SdpService *service, void *context))
{
    LOG_INFO("[%{public}s][%{public}d] handle = [%08x], type = [%u]", __FUNCTION__, __LINE__, handle, attributeIdList.type);
    uint16_t length;
    uint8_t *buffer = NULL;
    SdpClientRequest *request = NULL;
    int ret;

    if (SdpGetEnableState() == false) {
        return BT_BAD_STATUS;
    }

    request = MEM_MALLOC.alloc(sizeof(SdpClientRequest));
    (void)memset_s(request, sizeof(SdpClientRequest), 0, sizeof(SdpClientRequest));
    /// Create service attribute request
    buffer = MEM_MALLOC.alloc(sizeof(SdpAttributeIdList) + SDP_ATTRIBUTE_LENGTH);
    (void)memset_s(buffer,
        (sizeof(SdpAttributeIdList) + SDP_ATTRIBUTE_LENGTH),
        0,
        (sizeof(SdpAttributeIdList) + SDP_ATTRIBUTE_LENGTH));

    length = SdpCreateAttributeRequest(handle, attributeIdList, buffer);
    if (length == 0) {
        LOG_ERROR("[%{public}s][%{public}d] Create attribute request failed", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }

    (void)memcpy_s(&request->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    request->pduId = SDP_SERVICE_ATTRIBUTE_REQUEST;
    request->transactionId = SdpGetTransactionId();
    request->resentFlag = false;
    request->packetState = SDP_PACKET_WAIT;
    request->packet = SdpBuildPacket(buffer, length);
    request->assemblePacket = NULL;
    request->callback.ServiceAttributeCb = serviceAttributeCb;
    request->context = context;

    ret = SdpClientConnect(request);
    MEM_MALLOC.free(buffer);

    return ret;
}

int SdpServiceSearchAttribute(const BtAddr *addr, const SdpUuid *uuidArray, SdpAttributeIdList attributeIdList,
    void *context,
    void (*searchAttributeCb)(const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context))
{
    LOG_INFO("[%{public}s][%{public}d] uuidNum = [%hu], type = [%u]", __FUNCTION__, __LINE__, uuidArray->uuidNum, attributeIdList.type);
    uint16_t length;
    uint8_t *buffer = NULL;
    SdpClientRequest *request = NULL;
    int ret;

    if (SdpGetEnableState() == false) {
        return BT_BAD_STATUS;
    }

    /// Create service search attribute request
    request = MEM_MALLOC.alloc(sizeof(SdpClientRequest));
    (void)memset_s(request, sizeof(SdpClientRequest), 0, sizeof(SdpClientRequest));
    buffer = MEM_MALLOC.alloc(
        sizeof(SdpAttributeIdList) + (uuidArray->uuidNum * SDP_UUID_ATTRIBUTE_LENGTH) + SDP_SEARCH_ATTRIBUTE_LENGTH);
    (void)memset_s(buffer,
        (sizeof(SdpAttributeIdList) + (uuidArray->uuidNum * SDP_UUID_ATTRIBUTE_LENGTH) + SDP_SEARCH_ATTRIBUTE_LENGTH),
        0,
        (sizeof(SdpAttributeIdList) + (uuidArray->uuidNum * SDP_UUID_ATTRIBUTE_LENGTH) + SDP_SEARCH_ATTRIBUTE_LENGTH));

    length = SdpCreateSearchAttributeRequest(uuidArray->uuid, uuidArray->uuidNum, attributeIdList, buffer);
    if (length == 0) {
        LOG_ERROR("[%{public}s][%{public}d] Create search attribute request failed", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }

    (void)memcpy_s(&request->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    request->pduId = SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST;
    request->transactionId = SdpGetTransactionId();
    request->resentFlag = false;
    request->packetState = SDP_PACKET_WAIT;
    request->packet = SdpBuildPacket(buffer, length);
    request->assemblePacket = NULL;
    request->callback.ServiceSearchAttributeCb = searchAttributeCb;
    request->context = context;

    ret = SdpClientConnect(request);
    MEM_MALLOC.free(buffer);

    return ret;
}

int SdpServiceBrowse(const BtAddr *addr, void *context,
    void (*serviceBrowseCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context))
{
    SdpClientRequest *request = NULL;
    uint8_t *buffer = NULL;
    uint16_t length;
    int ret;

    if (SdpGetEnableState() == false) {
        return BT_BAD_STATUS;
    }

    /// Create service search request
    request = MEM_MALLOC.alloc(sizeof(SdpClientRequest));
    (void)memset_s(request, sizeof(SdpClientRequest), 0, sizeof(SdpClientRequest));

    buffer = MEM_MALLOC.alloc(SDP_BROWSE_LENGTH);
    (void)memset_s(buffer, SDP_BROWSE_LENGTH, 0, SDP_BROWSE_LENGTH);

    length = SdpCreateServiceBrowseRequest(buffer);
    if (length == 0) {
        LOG_ERROR("[%{public}s][%{public}d] Setup search request failed", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }

    (void)memcpy_s(&request->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    request->pduId = SDP_SERVICE_SEARCH_REQUEST;
    request->transactionId = SdpGetTransactionId();
    request->resentFlag = false;
    request->packetState = SDP_PACKET_WAIT;
    request->packet = SdpBuildPacket(buffer, length);
    request->assemblePacket = NULL;
    request->callback.ServiceSearchCb = serviceBrowseCb;
    request->context = context;

    ret = SdpClientConnect(request);
    MEM_MALLOC.free(buffer);

    return ret;
}