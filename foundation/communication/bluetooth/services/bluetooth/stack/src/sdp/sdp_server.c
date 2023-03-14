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

#include "sdp_server.h"

#include <stdio.h>
#include <string.h>

#include "sdp_connect.h"
#include "sdp_util.h"

#include "allocator.h"
#include "bt_endian.h"
#include "packet.h"

#include "list.h"

#include "log.h"

#define SDP_RECURSION_LEVEL_MAX 3
#define SDP_RESERVE_LENGTH 10

typedef struct {
    uint16_t attributeId;
    uint16_t attributeLength;
    uint8_t *attributeValue;
} AttributeItem;

typedef struct {
    uint32_t serviceRecordHandle;
    uint16_t attributeNumber;                              /// Attribute count
    uint16_t totalLength;                                  /// Attribute length
    AttributeItem attributeItem[SDP_MAX_ATTRIBUTE_COUNT];  /// Attribute item
    bool flag;                                             /// 1-Register 0-Deregister
} ServiceRecordItem;

typedef struct {
    uint8_t *buffer;
    uint16_t length;
} BufferInfo;
/// Bluetooth Base UUID (00000000-0000-1000-8000-00805F9B34FB)
static const uint8_t G_BASE_UUID[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};
/// Handle start after the reserved range
static uint32_t g_nextServiceRecordHandle = (uint32_t)SDP_MAX_RESERVED_RECORD_HANDLE + 1;
/// Service records list
static List *g_serviceRecordList = NULL;

static int SdpAddServiceRecordHandle(uint32_t handle);
static uint16_t SdpAddAttributeForProtocolDescriptor(
    uint8_t *buffer, uint16_t offset, const SdpProtocolParameter *parameter, uint16_t parameterNumber);
static uint16_t SdpAddAttributeForUuidSequence(uint8_t *buffer, uint16_t offset, const BtUuid *uuid);
static uint16_t SdpAddAttributeForValue(uint8_t *buffer, uint16_t offset, uint32_t value);
static uint16_t SdpAddAttributeForString(uint8_t *buffer, uint16_t offset, const char *name, uint16_t nameLen);
static uint16_t SdpAddAttributeForUrl(uint8_t *buffer, uint16_t offset, const uint8_t *url, uint16_t urlLen);
static int SdpAddAttributeToServiceRecord(
    uint32_t handle, uint16_t attributeId, uint8_t attributeType, uint32_t attributeLength, const uint8_t *value);
static void SdpParseSearchRequest(uint16_t lcid, uint16_t transactionId, BufferInfo *bufferInfo);
static void SdpParseAttributeRequest(uint16_t lcid, uint16_t transactionId, BufferInfo *bufferInfo);
static void SdpParseSearchAttributeRequest(uint16_t lcid, uint16_t transactionId, BufferInfo *bufferInfo);
static void SortForAttributeId(AttributeItem *attributeItem, uint16_t attributeNumber);
static bool CompareUuid(const uint8_t *uuid1, uint16_t length1, const uint8_t *uuid2, uint16_t length2);
static uint16_t GetRecordHandleArray(
    uint8_t uuidArray[][20], int uuidNum, uint32_t *handleArray, uint16_t handleNum, uint16_t maxRecordCount);
static ServiceRecordItem *FindServiceRecordItem(uint32_t handle);

static void SdpFreeServiceRecord(void *data)
{
    ServiceRecordItem *item = (ServiceRecordItem *)data;

    for (int i = 0; i < item->attributeNumber; i++) {
        MEM_MALLOC.free(item->attributeItem[i].attributeValue);
    }
    MEM_MALLOC.free(item);
}

void SdpInitializeServer()
{
    /// Create service record list
    g_serviceRecordList = ListCreate(SdpFreeServiceRecord);
}

void SdpFinalizeServer()
{
    /// Destroy service record list
    if (g_serviceRecordList != NULL) {
        ListDelete(g_serviceRecordList);
        g_serviceRecordList = NULL;
    }
}

uint32_t SdpCreateServiceRecord()
{
    ServiceRecordItem *serviceRecordItem = MEM_MALLOC.alloc(sizeof(ServiceRecordItem));
    if (serviceRecordItem == NULL) {
        LOG_ERROR("point to NULL");
        return 0;
    }
    uint32_t handle = 0;
    int ret;

    if (!SdpGetEnableState()) {
        return handle;
    }

    (void)memset_s(serviceRecordItem, sizeof(ServiceRecordItem), 0, sizeof(ServiceRecordItem));
    /// Get free record handle
    handle = g_nextServiceRecordHandle;
    if (g_nextServiceRecordHandle >= UINT32_MAX) {
        g_nextServiceRecordHandle = (uint32_t)SDP_MAX_RESERVED_RECORD_HANDLE;
    }
    g_nextServiceRecordHandle++;

    /// Add record handle to list
    serviceRecordItem->serviceRecordHandle = handle;
    ListAddLast(g_serviceRecordList, serviceRecordItem);
    ret = SdpAddServiceRecordHandle(handle);
    if (ret != BT_NO_ERROR) {
        ListRemoveLast(g_serviceRecordList);
        handle = 0;
    }
    LOG_INFO("[%{public}s][%u] handle = [0x%08x]", __FUNCTION__, __LINE__, handle);
    return handle;
}

int SdpDestroyServiceRecord(uint32_t handle)
{
    LOG_INFO("[%{public}s][%u] handle = [0x%08x]", __FUNCTION__, __LINE__, handle);
    ServiceRecordItem *item = NULL;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Remove service record from list
    item = FindServiceRecordItem(handle);
    if (item == NULL || item->flag) {
        return BT_BAD_PARAM;
    }
    ListRemoveNode(g_serviceRecordList, item);

    return BT_NO_ERROR;
}

int SdpRegisterServiceRecord(uint32_t handle)
{
    LOG_INFO("[%{public}s][%u] handle = [0x%08x]", __FUNCTION__, __LINE__, handle);
    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }
    ServiceRecordItem *item = FindServiceRecordItem(handle);
    if (item == NULL || item->flag) {
        return BT_BAD_PARAM;
    }
    /// Sort attribute id
    SortForAttributeId(item->attributeItem, item->attributeNumber);
    /// Set registration flag
    item->flag = true;

    return BT_NO_ERROR;
}

int SdpDeregisterServiceRecord(uint32_t handle)
{
    LOG_INFO("[%{public}s][%u] handle = [0x%08x]", __FUNCTION__, __LINE__, handle);
    ServiceRecordItem *item = NULL;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    item = FindServiceRecordItem(handle);
    if ((item == NULL) || (!item->flag)) {
        LOG_ERROR("[%{public}s][%u] There is no handle [0x%08x].", __FUNCTION__, __LINE__, handle);
        return BT_BAD_PARAM;
    }
    /// Set deregistration flag
    item->flag = false;

    return BT_NO_ERROR;
}

/**
 * @brief    SdpAddServiceRecordHandle
 * @detail   Add ServiceRecordHandle Attribute to service record
 * @para[in] handle: record handle
 * @return   Success(0) or error code
 * @see      Refer to charter 5.1.1 of Core 5.0
 */
static int SdpAddServiceRecordHandle(uint32_t handle)
{
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset = 0;
    int ret;

    // Data Element: Unsigned Integer 4 bytes (0x0A)
    buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_32;
    offset++;
    /// 32-bit handle
    *(uint32_t *)(buffer + 1) = H2BE_32(handle);
    offset += SDP_SERVICE_RECORD_HANDLE_BYTE;

    /// Attribute ID: ServiceRecordHandle (0x0000)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_SERVICE_RECORD_HANDLE, 0, offset, buffer);

    return ret;
}

int SdpAddServiceClassIdList(uint32_t handle, const BtUuid *classid, uint16_t classidNumber)
{
    LOG_INFO("[%{public}s][%u] handle = [0x%08x], classidNumber = [%hu]", __FUNCTION__, __LINE__, handle, classidNumber);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset = 0;
    uint8_t type;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    // Data Element: Sequence uint8 (0x35)
    type = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
    for (int i = 0; i < classidNumber; i++, classid++) {
        uint16_t pos;
        pos = offset;
        offset = SdpAddAttributeForUuid(buffer, offset, classid);
        if (pos == offset) {
            LOG_ERROR("[%{public}s][%{public}d] Invalid uuid type [0x%x]", __FUNCTION__, __LINE__, classid->type);
            return BT_BAD_PARAM;
        }
    }
    /// Attribute ID: ServiceClassIDList (0x0001)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST, type, offset, buffer);

    return ret;
}

int SdpAddServiceRecordState(uint32_t handle, uint32_t state)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], state = [0x%08x]", __FUNCTION__, __LINE__, handle, state);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset = 0;
    int ret;

    // Data Element: Unsigned Integer 4 bytes (0x0A)
    buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_32;
    offset++;
    /// Data value
    *(uint32_t *)(buffer + offset) = H2BE_32(state);
    offset += SDP_UINT32_LENGTH;
    /// Attribute ID: ServiceRecordState (0x0002)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_SERVICE_RECORD_STATE, 0, offset, buffer);

    return ret;
}

int SdpAddServiceId(uint32_t handle, const BtUuid *serviceid)
{
    LOG_INFO("[%{public}s][%u] handle = [0x%08x], serviceid->type = [0x%02x]", __FUNCTION__, __LINE__, handle, serviceid->type);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    if ((serviceid->type != BT_UUID_16) && (serviceid->type != BT_UUID_32) && (serviceid->type != BT_UUID_128)) {
        LOG_ERROR("[%{public}s][%u] Get wrong type [0x%02x]", __FUNCTION__, __LINE__, serviceid->type);
        return BT_BAD_PARAM;
    }

    // Attribute Value
    offset = SdpAddAttributeForUuid(buffer, 0, serviceid);
    /// Attribute ID: ServiceID (0x0003)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_SERVICE_ID, 0, offset, buffer);

    return ret;
}

int SdpAddProtocolDescriptorList(uint32_t handle, const SdpProtocolDescriptor *descriptor, uint16_t descriptorNumber)
{
    LOG_INFO("[%{public}s][%u] handle = [0x%08x], descriptorNumber = [%hu]", __FUNCTION__, __LINE__, handle, descriptorNumber);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint8_t attributeType;
    uint16_t offset = 0;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Data Element: Sequence uint8 (0x35)
    attributeType = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
    /// Attribute Value
    if (descriptorNumber > SDP_PROTOCOL_DESCRIPTOR_MAX_COUNT) {
        descriptorNumber = SDP_PROTOCOL_DESCRIPTOR_MAX_COUNT;
    }

    for (int i = 0; i < descriptorNumber; i++, descriptor++) {
        uint16_t result;
        uint16_t pos;
        pos = offset;
        result = SdpAddAttributeForUuidSequence(buffer, offset, &descriptor->protocolUuid);
        if (result == offset) {
            LOG_ERROR("[%{public}s][%{public}d] ", __FUNCTION__, __LINE__);
            return BT_BAD_PARAM;
        } else {
            offset = result;
        }
        offset =
            SdpAddAttributeForProtocolDescriptor(buffer, offset, descriptor->parameter, descriptor->parameterNumber);
        if (offset == 0) {
            return BT_BAD_PARAM;
        }
        // Data Element Var Size
        buffer[pos + 1] = offset - pos - SDP_UINT16_LENGTH;
    }
    /// Attribute ID: ProtocolDescriptorList (0x0004)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST, attributeType, offset, buffer);
    return ret;
}

int SdpAddAdditionalProtocolDescriptorList(
    uint32_t handle, const SdpAdditionalProtocolDescriptor *descriptorList, uint16_t descriptorListNumber)
{
    LOG_INFO("[%{public}s][%u] handle = [0x%08x], descriptorListNumber = [%hu]",
        __FUNCTION__,
        __LINE__,
        handle,
        descriptorListNumber);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset = 0;
    uint8_t attributeType;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Data Element: Sequence uint8 (0x35)
    attributeType = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
    // Attribute Value
    for (int i = 0; i < descriptorListNumber; i++, descriptorList++) {
        if (descriptorList->protocolDescriptorNumber == 0) {
            continue;
        }
        uint16_t posAdditional = offset;
        // Data Element: Sequence uint8 (0x35)
        buffer[offset] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
        offset += SDP_UINT16_LENGTH;

        const SdpProtocolDescriptor *descriptor = descriptorList->parameter;
        int j;
        for (j = 0; j < descriptorList->protocolDescriptorNumber; j++, descriptor++) {
            uint16_t pos = offset;
            uint16_t res = SdpAddAttributeForUuidSequence(buffer, offset, &descriptor->protocolUuid);
            if (res == offset) {
                LOG_ERROR("[%{public}s][%{public}d]", __FUNCTION__, __LINE__);
                return BT_BAD_PARAM;
            } else {
                offset = res;
            }
            offset = SdpAddAttributeForProtocolDescriptor(
                buffer, offset, descriptor->parameter, descriptor->parameterNumber);
            if (offset == 0) {
                return BT_BAD_PARAM;
            }
            // Data Element Var Size
            buffer[pos + 1] = offset - pos - SDP_UINT16_LENGTH;
        }
        buffer[posAdditional + 1] = offset - posAdditional - SDP_UINT16_LENGTH;
    }

    /// Attribute ID: AdditionalProtocolDescriptorList (0x000D)
    ret = SdpAddAttributeToServiceRecord(
        handle, SDP_ATTRIBUTE_ADDITIONAL_PROTOCOL_DESCRIPTOR_LIST, attributeType, offset, buffer);
    return ret;
}

int SdpAddBrowseGroupList(uint32_t handle, const BtUuid *browseUuid, uint16_t browseUuidNumber)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], browseUuidNumber = [%hu]", __FUNCTION__, __LINE__, handle, browseUuidNumber);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset = 0;
    uint8_t attributeType;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Data Element: Sequence uint8 (0x35)
    attributeType = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
    // Attribute Value
    // Data Element Var Size Length
    for (int i = 0; i < browseUuidNumber; i++, browseUuid++) {
        if (browseUuid->type == BT_UUID_16) {
            if ((offset + SDP_UUID16_LENGTH + SDP_UINT16_LENGTH + 1) > SDP_MAX_ATTRIBUTE_LEN) {
                break;
            }
            offset = SdpAddAttributeForUuid(buffer, offset, browseUuid);
        } else if (browseUuid->type == BT_UUID_32) {
            if ((offset + SDP_UUID32_LENGTH + SDP_UINT16_LENGTH + 1) > SDP_MAX_ATTRIBUTE_LEN) {
                break;
            }
            offset = SdpAddAttributeForUuid(buffer, offset, browseUuid);
        } else if (browseUuid->type == BT_UUID_128) {
            if ((offset + SDP_UUID128_LENGTH + SDP_UINT16_LENGTH + 1) > SDP_MAX_ATTRIBUTE_LEN) {
                break;
            }
            offset = SdpAddAttributeForUuid(buffer, offset, browseUuid);
        } else {
            LOG_ERROR("[%{public}s][%{public}d] Get wrong type [0x%02x]", __FUNCTION__, __LINE__, browseUuid->type);
            return BT_BAD_PARAM;
        }
    }
    /// Attribute ID: BrowseGroupList (0x0005)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_BROWSE_GROUP_LIST, attributeType, offset, buffer);

    return ret;
}

int SdpAddLanguageBaseAttributeIdList(
    uint32_t handle, const SdpLanguageBaseAttributeId *baseAttributeId, uint16_t baseAttributeIdNum)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], baseAttributeIdNumber = [%hu]",
        __FUNCTION__,
        __LINE__,
        handle,
        baseAttributeIdNum);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset = 0;
    uint8_t attributeType;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    // Attribute Value
    /* build the language base descriptor list. This consists of a data */
    /* element sequence. The sequence consists of 9 bytes (3 UINt16 fields) */

    for (int i = 0; i < baseAttributeIdNum; i++) {
        if ((offset + SDP_LANGUAGE_ATTRIBUTE_LENGTH) >= SDP_MAX_ATTRIBUTE_LEN) {
            break;
        }
        /// Data Element: Unsigned Integer 2 bytes (0x09)
        buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
        offset++;
        /// Language Code
        *(uint16_t *)(buffer + offset) = H2BE_16(baseAttributeId[i].languageIdentifier);
        offset += SDP_UINT16_LENGTH;
        /// Data Element: Unsigned Integer 2 bytes (0x09)
        buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
        offset++;
        // Language Encoding
        *(uint16_t *)(buffer + offset) = H2BE_16(baseAttributeId[i].characterEncodingIdentifier);
        offset += SDP_UINT16_LENGTH;
        /// Data Element: Unsigned Integer 2 bytes (0x09)
        buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
        offset++;
        /// Attribute Base
        *(uint16_t *)(buffer + offset) = H2BE_16(baseAttributeId[i].baseAttributeId);
        offset += SDP_UINT16_LENGTH;
    }
    /// Data Element Var Size Length
    if (offset > 0xFF) {
        /// Data Element: Sequence uint16 (0x36)
        attributeType = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_16;
    } else {
        /// Data Element: Sequence uint8 (0x35)
        attributeType = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
    }
    /// Attribute ID: LanguageBaseAttributeIDList (0x0006)
    ret = SdpAddAttributeToServiceRecord(
        handle, SDP_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST, attributeType, offset, buffer);

    return ret;
}

int SdpAddServiceInfoTimeToLive(uint32_t handle, uint32_t value)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], value = [0x%08x]", __FUNCTION__, __LINE__, handle, value);

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    uint8_t buffer[10] = {0};
    // Attribute Value
    uint16_t offset = SdpAddAttributeForValue(buffer, 0, value);
    /// Attribute ID: ServiceInfoTimeToLive (0x0007)
    int ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_SERVICE_INFO_TIME_TO_LIVE, 0, offset, buffer);

    return ret;
}

int SdpAddServiceAvailability(uint32_t handle, uint8_t value)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], value = [0x%02x]", __FUNCTION__, __LINE__, handle, value);
    uint8_t buffer[2] = {0};
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    // Attribute Value
    // Data Element: Unsigned Integer 1 bytes (0x08)
    buffer[0] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_8;
    buffer[1] = value;
    /// Attribute ID: ServiceAvailability (0x0008)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_SERVICE_AVAILABILITY, 0, SDP_UINT16_LENGTH, buffer);

    return ret;
}

int SdpAddBluetoothProfileDescriptorList(
    uint32_t handle, const SdpProfileDescriptor *profileDescriptor, uint16_t profileDescriptorNum)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], profileDescriptorNumber = [%hu]",
        __FUNCTION__,
        __LINE__,
        handle,
        profileDescriptorNum);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset = 0;
    uint8_t type;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Attribute Value
    /// Data Element: Sequence uint8 (0x35)
    type = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;

    for (int i = 0; i < profileDescriptorNum; i++, profileDescriptor++) {
        uint16_t pos;
        uint16_t result;
        pos = offset;
        result = SdpAddAttributeForUuidSequence(buffer, offset, &profileDescriptor->profileUuid);
        if (result == offset) {
            LOG_ERROR("[%{public}s][%{public}d]", __FUNCTION__, __LINE__);
            return BT_BAD_PARAM;
        } else {
            offset = result;
        }
        /// Data Element: Unsigned Integer 2 bytes (0x09)
        buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
        offset++;
        // Protocol Version uint16_t
        *(uint16_t *)(buffer + offset) = H2BE_16(profileDescriptor->versionNumber);
        offset += SDP_UINT16_LENGTH;
        /// Data Element Var Size
        buffer[pos + 1] = offset - pos - SDP_UINT16_LENGTH;
    }
    /// Attribute ID: BluetoothProfileDescriptorList (0x0009)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST, type, offset, buffer);

    return ret;
}

int SdpAddDocumentationUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], url = [%{public}s], urlLen = [%hu]", __FUNCTION__, __LINE__, handle, url, urlLen);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Attribute Value
    offset = SdpAddAttributeForUrl(buffer, 0, url, urlLen);
    /// Attribute ID: DocumentationURL (0x000A)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_DOCUMENTATION_URL, 0, offset, buffer);

    return ret;
}

int SdpAddClientExecutableUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], url = [%{public}s], urlLen = [%hu]", __FUNCTION__, __LINE__, handle, url, urlLen);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Attribute Value
    offset = SdpAddAttributeForUrl(buffer, 0, url, urlLen);
    /// Attribute ID: ClientExecutableURL (0x000B)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_CLIENT_EXECUTABLE_URL, 0, offset, buffer);

    return ret;
}

int SdpAddIconUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], url = [%{public}s], urlLen = [%hu]", __FUNCTION__, __LINE__, handle, url, urlLen);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Attribute Value
    offset = SdpAddAttributeForUrl(buffer, 0, url, urlLen);
    /// Attribute ID: IconURL (0x000C)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_ICON_URL, 0, offset, buffer);

    return ret;
}

int SdpAddServiceName(uint32_t handle, uint16_t baseAttributeId, const char *name, uint16_t nameLen)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], baseAttributeId = [0x%04x], name = [%{public}s], nameLen = [%hu]",
        __FUNCTION__,
        __LINE__,
        handle,
        baseAttributeId,
        name,
        nameLen);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Attribute Value
    offset = SdpAddAttributeForString(buffer, 0, name, nameLen);
    /// Attribute ID: ServiceName (0x0000 + baseAttributeId)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_SERVICE_NAME + baseAttributeId, 0, offset, buffer);

    return ret;
}

int SdpAddServiceDescription(
    uint32_t handle, uint16_t baseAttributeId, const char *description, uint16_t descriptionLen)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], baseAttributeId = [0x%04x], description = [%{public}s], descriptionLen = [%hu]",
        __FUNCTION__,
        __LINE__,
        handle,
        baseAttributeId,
        description,
        descriptionLen);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t offset;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Attribute Value
    offset = SdpAddAttributeForString(buffer, 0, description, descriptionLen);
    /// Attribute ID: ProviderName (0x0001 + baseAttributeId)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_DESCRIPTOR + baseAttributeId, 0, offset, buffer);

    return ret;
}

int SdpAddProviderName(uint32_t handle, uint16_t baseAttributeId, const char *name, uint16_t nameLen)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], baseAttributeId = [0x%04x], name = [%{public}s], nameLen = [%hu]",
        __FUNCTION__,
        __LINE__,
        handle,
        baseAttributeId,
        name,
        nameLen);
    uint16_t offset;
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    /// Attribute Value
    offset = SdpAddAttributeForString(buffer, 0, name, nameLen);
    /// Attribute ID: ProviderName (0x0002 + baseAttributeId)
    ret = SdpAddAttributeToServiceRecord(handle, SDP_ATTRIBUTE_PROVIDER_NAME + baseAttributeId, 0, offset, buffer);

    return ret;
}

static int SdpAddAttributeForCommon(SdpDataType type, const void *attributeValue, uint8_t *buffer, uint16_t length)
{
    if (type == SDP_TYPE_BOOL) {
        buffer[0] = (DE_TYPE_BOOL << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_8;
        buffer[1] = *(uint8_t *)attributeValue;
        length = SDP_UINT8_LENGTH + 1;
    } else if (type == SDP_TYPE_UINT_8) {
        buffer[0] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_8;
        buffer[1] = *(uint8_t *)attributeValue;
        length = SDP_UINT8_LENGTH + 1;
    } else if (type == SDP_TYPE_UINT_16) {
        buffer[0] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
        *(uint16_t *)(buffer + 1) = H2BE_16(*(uint16_t *)attributeValue);
        length = SDP_UINT16_LENGTH + 1;
    } else if (type == SDP_TYPE_UINT_32) {
        buffer[0] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_32;
        *(uint32_t *)(buffer + 1) = H2BE_32(*(uint32_t *)attributeValue);
        length = SDP_UINT32_LENGTH + 1;
    } else if (type == SDP_TYPE_INT_8) {
        buffer[0] = (DE_TYPE_INT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_8;
        buffer[1] = *(int8_t *)attributeValue;
        length = SDP_INT8_LENGTH + 1;
    } else if (type == SDP_TYPE_INT_16) {
        buffer[0] = (DE_TYPE_INT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
        int16_t value = *(int16_t *)attributeValue;
        *(uint16_t *)(buffer + 1) = H2BE_16(value);
        length = SDP_INT16_LENGTH + 1;
    } else if (type == SDP_TYPE_INT_32) {
        buffer[0] = (DE_TYPE_INT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_32;
        *(uint32_t *)(buffer + 1) = H2BE_32(*(int32_t *)attributeValue);
        length = SDP_INT32_LENGTH + 1;
    }

    return length;
}

int SdpAddAttribute(
    uint32_t handle, uint16_t attributeId, SdpDataType type, void *attributeValue, uint16_t attributeValueLength)
{
    LOG_INFO("[%{public}s] attributeId[0x%04x] type[%u] Length [%hu]", __FUNCTION__, attributeId, type, attributeValueLength);
    uint8_t buffer[SDP_MAX_ATTRIBUTE_LEN] = {0};
    uint16_t length = 0;
    BtUuid uuid;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    // Attribute Value
    switch (type) {
        case SDP_TYPE_BOOL:
        case SDP_TYPE_UINT_8:
        case SDP_TYPE_UINT_16:
        case SDP_TYPE_UINT_32:
        case SDP_TYPE_INT_8:
        case SDP_TYPE_INT_16:
        case SDP_TYPE_INT_32:
            length = SdpAddAttributeForCommon(type, attributeValue, buffer, 0);
            break;
        case SDP_TYPE_UUID_16:
            uuid.type = BT_UUID_16;
            uuid.uuid16 = *(uint16_t *)attributeValue;
            length = SdpAddAttributeForUuid(buffer, 0, &uuid);
            break;
        case SDP_TYPE_UUID_32:
            uuid.type = BT_UUID_32;
            uuid.uuid32 = *(uint32_t *)attributeValue;
            length = SdpAddAttributeForUuid(buffer, 0, &uuid);
            break;
        case SDP_TYPE_UUID_128:
            uuid.type = BT_UUID_128;
            SdpReverseForBigEndian((uint8_t *)attributeValue, uuid.uuid128, SDP_UUID128_LENGTH);
            length = SdpAddAttributeForUuid(buffer, 0, &uuid);
            break;
        case SDP_TYPE_TEXT:
            length = SdpAddAttributeForString(buffer, 0, attributeValue, attributeValueLength);
            break;
        case SDP_TYPE_URL:
            length = SdpAddAttributeForUrl(buffer, 0, attributeValue, attributeValueLength);
            break;
        default:
            return BT_BAD_PARAM;
    }
    ret = SdpAddAttributeToServiceRecord(handle, attributeId, 0, length, buffer);

    return ret;
}

int SdpAddSequenceAttribute(
    uint32_t handle, uint16_t attributeId, const uint8_t *attributeValue, uint16_t attributeValueLength)
{
    LOG_INFO("[%{public}s][%{public}d] handle = [0x%08x], attributeId = [0x%04x], attributeValueLength = [%hu]",
        __FUNCTION__,
        __LINE__,
        handle,
        attributeId,
        attributeValueLength);
    uint8_t type;
    int ret;

    if (!SdpGetEnableState()) {
        return BT_BAD_STATUS;
    }

    if (attributeValueLength > 0xFF) {
        // Length > 0xFF
        // Data Element: Sequence uint16 (0x36)
        type = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_16;
    } else {
        /// Data Element: Sequence uint8 (0x35)
        type = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
    }
    ret = SdpAddAttributeToServiceRecord(handle, attributeId, type, attributeValueLength, attributeValue);

    return ret;
}

static uint16_t SdpAddAttributeForProtocolDescriptor(
    uint8_t *buffer, uint16_t offset, const SdpProtocolParameter *parameter, uint16_t parameterNumber)
{
    if (parameterNumber == 0) {
        return offset;
    }
    if (parameterNumber > SDP_PROTOCOL_PARAMETER_MAX_COUNT) {
        parameterNumber = SDP_PROTOCOL_PARAMETER_MAX_COUNT;
    }

    for (int i = 0; i < parameterNumber; i++, parameter++) {
        switch (parameter->type) {
            case SDP_TYPE_UINT_8:
                // Data Element: Unsigned Integer 1 bytes (0x08)
                buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_8;
                offset++;
                // Data Value
                buffer[offset] = parameter->value & 0xFF;
                offset += SDP_UINT8_LENGTH;
                break;
            case SDP_TYPE_UINT_16:
                // Data Element: Unsigned Integer 2 bytes (0x09)
                buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
                offset++;
                // Data Value
                *(uint16_t *)(buffer + offset) = H2BE_16(parameter->value & 0xFFFF);
                offset += SDP_UINT16_LENGTH;
                break;
            case SDP_TYPE_UINT_32:
                // Data Element: Unsigned Integer 4 bytes (0x0A)
                buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_32;
                offset++;
                // Data Value
                *(uint32_t *)(buffer + offset) = H2BE_32(parameter->value);
                offset += SDP_UINT32_LENGTH;
                break;
            default:
                return 0;
        }
    }

    return offset;
}

static uint16_t SdpAddAttributeForUuidSequence(uint8_t *buffer, uint16_t offset, const BtUuid *uuid)
{
    switch (uuid->type) {
        case BT_UUID_16:
            /// Data Element: Sequence uint8 (0x35)
            buffer[offset] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
            offset++;
            /// Data Element Var Size (0x03)
            buffer[offset] = 0x03;
            offset++;
            offset = SdpAddAttributeForUuid(buffer, offset, uuid);
            break;
        case BT_UUID_32:
            /// Data Element: Sequence uint8 (0x35)
            buffer[offset] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
            offset++;
            /// Data Element Var Size (0x05)
            buffer[offset] = 0x05;
            offset++;
            offset = SdpAddAttributeForUuid(buffer, offset, uuid);
            break;
        case BT_UUID_128:
            /// Data Element: Sequence uint8 (0x35)
            buffer[offset] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
            offset++;
            /// Data Element Var Size (0x11)
            buffer[offset] = 0x11;
            offset++;
            offset = SdpAddAttributeForUuid(buffer, offset, uuid);
            break;
        default:
            LOG_ERROR("[%{public}s][%{public}d] Get wrong type [0x%02x]", __FUNCTION__, __LINE__, uuid->type);
            break;
    }

    return offset;
}

static uint16_t SdpAddAttributeForValue(uint8_t *buffer, uint16_t offset, uint32_t value)
{
    // Data Element: Unsigned Integer 4 bytes (0x0A)
    buffer[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_32;
    offset++;
    *(uint32_t *)(buffer + offset) = H2BE_32(value);
    offset += SDP_UINT32_LENGTH;

    return offset;
}

static uint16_t SdpAddAttributeForUrl(uint8_t *buffer, uint16_t offset, const uint8_t *url, uint16_t urlLen)
{
    // URL Length
    if (urlLen > 0xFF) {
        // Data Element: Url uint16 (0x46)
        buffer[offset] = (DE_TYPE_URL << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_16;
        offset++;
        *(uint16_t *)(buffer + offset) = H2BE_16(urlLen);
        offset += SDP_UINT16_LENGTH;
    } else {
        // Data Element: Url uint8 (0x45)
        buffer[offset] = (DE_TYPE_URL << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
        offset++;
        buffer[offset] = urlLen;
        offset++;
    }
    // URL
    if (urlLen > (SDP_MAX_ATTRIBUTE_LEN - offset)) {
        urlLen = SDP_MAX_ATTRIBUTE_LEN - offset;
    }
    if (memcpy_s(buffer + offset, urlLen, url, urlLen) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail", __FUNCTION__, __LINE__);
        return offset;
    }
    offset += urlLen;

    return offset;
}

static uint16_t SdpAddAttributeForString(uint8_t *buffer, uint16_t offset, const char *name, uint16_t nameLen)
{
    // Name Length
    if (nameLen > 0xff) {
        // Data Element: Text string uint16 (0x26)
        buffer[offset] = (DE_TYPE_STRING << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_16;
        offset++;
        *(uint16_t *)(buffer + offset) = H2BE_16(nameLen);
        offset += SDP_UINT16_LENGTH;
    } else {
        // Data Element: Text string uint8 (0x25)
        buffer[offset] = (DE_TYPE_STRING << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
        offset++;
        buffer[offset] = nameLen;
        offset++;
    }
    // Name
    if (nameLen > SDP_MAX_ATTRIBUTE_LEN - offset) {
        nameLen = SDP_MAX_ATTRIBUTE_LEN - offset;
    }
    if (memcpy_s(buffer + offset, nameLen, name, nameLen) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail", __FUNCTION__, __LINE__);
        return offset;
    }
    offset += nameLen;

    return offset;
}

static int SdpAddAttributeToServiceRecord(
    uint32_t handle, uint16_t attributeId, uint8_t attributeType, uint32_t attributeLength, const uint8_t *value)
{
    uint8_t *attributeValue = NULL;
    uint16_t offset = 0;
    ServiceRecordItem *item = NULL;

    item = FindServiceRecordItem(handle);
    if (item == NULL) {
        return BT_BAD_PARAM;
    }
    /// Find duplicate attribute id
    for (int i = 0; i < item->attributeNumber; i++) {
        if (item->attributeItem[i].attributeId == attributeId) {
            LOG_ERROR("[%{public}s][%{public}d] Add duplicate attribute [0x%04x]", __FUNCTION__, __LINE__, attributeId);
            return BT_BAD_PARAM;
        }
    }
    item->attributeItem[item->attributeNumber].attributeValue =
        MEM_MALLOC.alloc(attributeLength + SDP_SERVICE_RECORD_OTHER);
    (void)memset_s(item->attributeItem[item->attributeNumber].attributeValue,
        attributeLength + SDP_SERVICE_RECORD_OTHER,
        0,
        attributeLength + SDP_SERVICE_RECORD_OTHER);
    attributeValue = item->attributeItem[item->attributeNumber].attributeValue;
    /// Data element: Unsigned Integer 2 bytes (0x09)
    attributeValue[offset] = (DE_TYPE_UINT << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_16;
    offset++;
    /// Attribute id
    *(uint16_t *)(attributeValue + offset) = H2BE_16(attributeId);
    item->attributeItem[item->attributeNumber].attributeId = attributeId;
    offset += SDP_UINT16_LENGTH;

    /// Attribute value
    /// Data element
    if (attributeType != 0) {
        attributeValue[offset] = attributeType;
        offset++;
        // Data element var size
        if (attributeLength > SDP_MAX_ATTRIBUTE_LEN) {
            attributeLength = SDP_MAX_ATTRIBUTE_LEN;
        }
        if (attributeLength > 0xFF) {
            *(uint16_t *)(attributeValue + offset) = H2BE_16(attributeLength);
            offset += SDP_UINT16_LENGTH;
        } else {
            attributeValue[offset] = attributeLength & 0xFF;
            offset++;
        }
    }
    /// Data element var
    if (memcpy_s(attributeValue + offset, attributeLength, value, attributeLength) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail", __FUNCTION__, __LINE__);
        return BT_OPERATION_FAILED;
    }
    offset += attributeLength;

    item->attributeItem[item->attributeNumber].attributeLength = offset;
    item->totalLength += offset;
    item->attributeNumber++;
    return BT_NO_ERROR;
}

void SdpParseClientRequest(uint16_t lcid, const Packet *data)
{
    uint8_t header[SDP_PDU_HEADER_LENGTH] = {0};
    uint16_t transactionId;
    uint16_t parameterLength;
    Packet *packet = PacketRefMalloc(data);
    PacketExtractHead(packet, header, sizeof(header));
    size_t size = PacketSize(packet);
    BufferInfo bufferInfo;

    /// PDU ID
    SdpPduId pduId = header[0];
    /// Transaction ID
    transactionId = BE2H_16(*(uint16_t *)(header + 1));
    /// ParameterLength
    parameterLength = BE2H_16(*(uint16_t *)(header + SDP_UINT16_LENGTH + 1));
    if (parameterLength != size) {
        LOG_ERROR("[%{public}s][%{public}d] Invalid pdu size [%hu] [%zu]", __FUNCTION__, __LINE__, parameterLength, size);
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_PDU_SIZE);
        PacketFree(packet);
        packet = NULL;
        return;
    }
    LOG_INFO("[%{public}s][%{public}d] pduId[0x%x] transactionId[0x%02x] parameterLength[0x%02x]",
        __FUNCTION__,
        __LINE__,
        pduId,
        transactionId,
        parameterLength);

    uint8_t *buffer = MEM_MALLOC.alloc(parameterLength);
    if (buffer == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    PacketRead(packet, buffer, 0, parameterLength);

    bufferInfo.buffer = buffer;
    bufferInfo.length = parameterLength;

    switch (pduId) {
        case SDP_SERVICE_SEARCH_REQUEST:
            SdpParseSearchRequest(lcid, transactionId, &bufferInfo);
            break;
        case SDP_SERVICE_ATTRIBUTE_REQUEST:
            SdpParseAttributeRequest(lcid, transactionId, &bufferInfo);
            break;
        case SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST:
            SdpParseSearchAttributeRequest(lcid, transactionId, &bufferInfo);
            break;
        default:
            LOG_ERROR("[%{public}s][%{public}d] Invalid PDU ID [%u]", __FUNCTION__, __LINE__, pduId);
            SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_REQ_SYNTAX);
            break;
    }
    MEM_MALLOC.free(buffer);
    PacketFree(packet);
    packet = NULL;
}

static void SdpCreateSearchResponse(
    uint16_t lcid, uint16_t transactionId, uint8_t uuidArray[][20], int uuidNum, uint16_t maximumServiceRecordCount)
{
    uint8_t *buffer = NULL;
    uint32_t *handleArray = NULL;
    uint16_t handleNum = 0;
    uint16_t offset = 0;

    handleArray = (uint32_t *)MEM_MALLOC.alloc(maximumServiceRecordCount * SDP_SERVICE_RECORD_HANDLE_BYTE);
    if (handleArray == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    buffer = MEM_MALLOC.alloc(SDP_MTU_SIZE);
    if (buffer == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(buffer, SDP_MTU_SIZE, 0, SDP_MTU_SIZE);

    handleNum = GetRecordHandleArray(uuidArray, uuidNum, handleArray, handleNum, maximumServiceRecordCount);

    /// ServiceRecordHandleList
    for (int i = 0; i < handleNum; i++) {
        *(uint32_t *)(buffer + offset) = H2BE_32(handleArray[i]);
        offset += SDP_SERVICE_RECORD_HANDLE_BYTE;
    }

    SdpSendSearchResponse(lcid, transactionId, offset, buffer, maximumServiceRecordCount);
    MEM_MALLOC.free(buffer);
    MEM_MALLOC.free(handleArray);
}

static int SdpGetUuidArray(uint8_t *buffer, uint16_t pos, uint16_t length, uint8_t uuidArray[][20])
{
    uint16_t uuidNum = 0;
    uint16_t offset = 0;

    while (offset < length) {
        uint8_t type;
        if (uuidNum >= SDP_MAX_UUID_COUNT) {
            break;
        }
        type = buffer[pos + offset];
        if (type == 0x19) {
            /// UUID 2 bytes (0x19)
            (void)memcpy_s(uuidArray + uuidNum, SDP_UUID16_LENGTH + 1, buffer + offset + pos, SDP_UUID16_LENGTH + 1);
            offset += SDP_UUID16_LENGTH + 1;
        } else if (type == 0x1A) {
            /// UUID 4 bytes (0x1A)
            (void)memcpy_s(uuidArray + uuidNum, SDP_UUID32_LENGTH + 1, buffer + offset + pos, SDP_UUID32_LENGTH + 1);
            offset += SDP_UUID32_LENGTH + 1;
        } else if (type == 0x1C) {
            /// UUID 16 bytes (0x1C)
            (void)memcpy_s(uuidArray + uuidNum, SDP_UUID128_LENGTH + 1, buffer + offset + pos, SDP_UUID128_LENGTH + 1);
            offset += SDP_UUID128_LENGTH + 1;
        } else {
            /// Not UUID
            return BT_BAD_PARAM;
        }
        uuidNum++;
    }
    return uuidNum;
}

static void SdpParseSearchRequest(uint16_t lcid, uint16_t transactionId, BufferInfo *bufferInfo)
{
    uint8_t uuidArray[SDP_MAX_UUID_COUNT][20];
    int uuidNum;
    uint16_t maximumServiceRecordCount;
    uint8_t continuationStateLen;
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t pos;

    /// ServiceSearchPattern
    uint8_t type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return;
    }
    offset += pos;
    pos = offset;

    /// MaximumServiceRecordCount - uint16_t
    offset += length;
    maximumServiceRecordCount = BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
    offset += SDP_UINT16_LENGTH;
    LOG_INFO("maxServiceRecordCount [%hu]", maximumServiceRecordCount);

    /// ContinuationState
    continuationStateLen = bufferInfo->buffer[offset];
    offset++;
    LOG_INFO("[%{public}s][%{public}d] continuationStateLen [%hhu].", __FUNCTION__, __LINE__, continuationStateLen);
    /// ContinuationStateLen
    if (continuationStateLen > SDP_MAX_CONTINUATION_LEN) {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_CONT_STATE);
        return;
    }
    /// continuation state yes or no (is 0)
    if (continuationStateLen != 0) {
        uint8_t continuationState[SDP_MAX_CONTINUATION_LEN] = {0};
        if (memcpy_s(continuationState, SDP_MAX_CONTINUATION_LEN,
            bufferInfo->buffer + offset, continuationStateLen) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
            return;
        }
        SdpSendSearchFragmentResponse(lcid, transactionId, maximumServiceRecordCount, NULL);
        return;
    }

    (void)memset_s(uuidArray, sizeof(uuidArray), 0, sizeof(uuidArray));
    uuidNum = SdpGetUuidArray(bufferInfo->buffer, pos, length, uuidArray);
    if (uuidNum < 0) {
        LOG_ERROR("[%{public}s][%{public}d] transactionId[%02x] Uuid is NULL.", __FUNCTION__, __LINE__, transactionId);
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_REQ_SYNTAX);
        return;
    }

    SdpCreateSearchResponse(lcid, transactionId, uuidArray, uuidNum, maximumServiceRecordCount);
}

static int BuildAttributeListByIdRange(uint32_t handle, uint8_t *buffer, uint8_t *attributeList)
{
    ServiceRecordItem *item = NULL;
    uint16_t offset = 0;
    uint16_t start;
    uint16_t end;

    /// All attribute range 0x0000 - 0xFFFF
    uint8_t type = buffer[0];
    if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || ((type & 0x07) != DE_SIZE_32)) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong type with AtriuteID Range [0x%02x].", __FUNCTION__, __LINE__, type);
        return BT_BAD_PARAM;
    }
    start = BE2H_16(*(uint16_t *)(buffer + 1));
    end = BE2H_16(*(uint16_t *)(buffer + SDP_UINT16_LENGTH + 1));

    item = FindServiceRecordItem(handle);
    if (item == NULL) {
        return BT_BAD_PARAM;
    }
    for (int i = 0; i < item->attributeNumber; i++) {
        if ((item->attributeItem[i].attributeId >= start) && (item->attributeItem[i].attributeId <= end)) {
            if ((item->attributeItem[i].attributeLength + offset) >= (SDP_MAX_LIST_BYTE_COUNT - SDP_RESERVE_LENGTH)) {
                return BT_BAD_PARAM;
            }
            if (memcpy_s(attributeList + offset,
                item->attributeItem[i].attributeLength,
                item->attributeItem[i].attributeValue,
                item->attributeItem[i].attributeLength) != EOK) {
                    LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
                    return BT_NO_MEMORY;
                }
            offset += item->attributeItem[i].attributeLength;
        }
    }
    return offset;
}

static int BuildAttributeListByIdList(uint32_t handle, uint16_t length, uint8_t *buffer, uint8_t *attributeList)
{
    ServiceRecordItem *item = NULL;
    uint16_t offset = 0;
    uint16_t pos = 0;
    uint16_t number;

    item = FindServiceRecordItem(handle);
    if (item == NULL) {
        return BT_BAD_PARAM;
    }
    number = length / (SDP_UINT16_LENGTH + 1);
    for (int i = 0; i < number; i++) {
        uint16_t attributeId;
        uint8_t type;
        type = buffer[pos];
        if (((type >> SDP_DESCRIPTOR_SIZE_BIT) != DE_TYPE_UINT) || ((type & 0x07) != DE_SIZE_16)) {
            LOG_ERROR("[%{public}s][%{public}d] Wrong type with AtriuteID Range [0x%02x].", __FUNCTION__, __LINE__, type);
            return BT_BAD_PARAM;
        }
        pos++;
        attributeId = BE2H_16(*(uint16_t *)(buffer + pos));
        pos += SDP_UINT16_LENGTH;

        for (int j = 0; j < item->attributeNumber; j++) {
            if (item->attributeItem[j].attributeId == attributeId) {
                (void)memcpy_s(attributeList + offset,
                    item->attributeItem[j].attributeLength,
                    item->attributeItem[j].attributeValue,
                    item->attributeItem[j].attributeLength);
                offset += item->attributeItem[j].attributeLength;
                break;
            }
        }
    }

    return offset;
}

static void SdpCreateAttributeResponse(
    uint16_t lcid, uint16_t transactionId, uint8_t *attributeList, int length, uint16_t maximumAttributeByteCount)
{
    uint8_t *buffer = NULL;
    Packet *packet = NULL;
    uint16_t offset = 0;

    if (length <= 0xFF) {
        buffer = MEM_MALLOC.alloc(length + 1 + SDP_UINT8_LENGTH);
        if (buffer == NULL) {
            LOG_ERROR("point to NULL");
            return;
        }
        buffer[0] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
        buffer[1] = length & 0xFF;
        offset += 1 + SDP_UINT8_LENGTH;
    } else if (length <= 0xFFFF) {
        buffer = MEM_MALLOC.alloc(length + 1 + SDP_UINT16_LENGTH);
        if (buffer == NULL) {
            LOG_ERROR("point to NULL");
            return;
        }
        buffer[0] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_16;
        *(uint16_t *)(buffer + 1) = H2BE_16(length);
        offset += 1 + SDP_UINT16_LENGTH;
    } else {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_PDU_SIZE);
        return;
    }
    if (memcpy_s(buffer + offset, length, attributeList, length) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        MEM_MALLOC.free(buffer);
        return;
    }
    offset += length;

    packet = PacketMalloc(0, 0, offset);
    PacketPayloadWrite(packet, buffer, 0, offset);
    SdpSendAttributeResponse(lcid, transactionId, SDP_SERVICE_ATTRIBUTE_RESPONSE, maximumAttributeByteCount, packet);

    MEM_MALLOC.free(buffer);
    PacketFree(packet);
    packet = NULL;
}

static void SdpParseAttributeRequest(uint16_t lcid, uint16_t transactionId, BufferInfo *bufferInfo)
{
    ServiceRecordItem *item = NULL;
    uint16_t maximumAttributeByteCount;
    uint8_t attributeList[SDP_MAX_LIST_BYTE_COUNT] = {0};
    uint8_t continuationStateLen;
    uint8_t type;
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t pos;
    int result;

    /// ServiceRecordHandle
    uint32_t handle = BE2H_32(*(uint32_t *)(bufferInfo->buffer + offset));
    offset += SDP_SERVICE_RECORD_HANDLE_BYTE;

    /// MaximumAttributeByteCount
    maximumAttributeByteCount = BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
    offset += SDP_UINT16_LENGTH;

    /// AttributeIDList
    type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &length);
    if (bufferInfo->length < length) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return;
    }
    offset += pos;
    pos = offset;

    /// ContinuationState
    offset += length;
    continuationStateLen = bufferInfo->buffer[offset];
    offset++;
    LOG_INFO("[%{public}s][%{public}d] continuation state Length [%hhu].", __FUNCTION__, __LINE__, continuationStateLen);
    /// ContinuationStateLen
    if (continuationStateLen > SDP_MAX_CONTINUATION_LEN) {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_CONT_STATE);
        return;
    }
    /// continuation state yes or no (is 0)
    if (continuationStateLen != 0) {
        uint8_t continuationState[SDP_MAX_CONTINUATION_LEN] = {0};
        if (memcpy_s(continuationState, SDP_MAX_CONTINUATION_LEN,
            bufferInfo->buffer + offset, continuationStateLen) != EOK) {
            return;
        }
        SdpSendAttributeFragmentResponse(
            lcid, SDP_SERVICE_ATTRIBUTE_RESPONSE, transactionId, maximumAttributeByteCount, NULL);
        return;
    }

    item = FindServiceRecordItem(handle);
    if (item == NULL) {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_SERV_REC_HDL);
        return;
    }

    if (length == (SDP_UINT32_LENGTH + 1)) {
        /// Range of attribute id
        result = BuildAttributeListByIdRange(handle, bufferInfo->buffer + pos, attributeList);
    } else {
        /// List of attribute id
        result = BuildAttributeListByIdList(handle, length, bufferInfo->buffer + pos, attributeList);
    }
    if (result < 0) {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_REQ_SYNTAX);
        return;
    }

    SdpCreateAttributeResponse(lcid, transactionId, attributeList, result, maximumAttributeByteCount);
}

static int BuildServiceRecordHandleList(const uint8_t *buffer, uint16_t pos, uint8_t *bufferEnd, uint32_t *handleArray)
{
    uint8_t uuidArray[SDP_MAX_UUID_COUNT][20] = {0};
    int uuidNum = 0;
    uint16_t handleNum = 0;

    while (buffer + pos < bufferEnd) {
        uint8_t type;
        if (uuidNum >= SDP_MAX_UUID_COUNT) {
            LOG_INFO("UuidNum [%{public}d] is more than 12.", uuidNum);
            break;
        }
        type = *(buffer + pos);
        if (type == 0x19) {
            /// UUID 2 bytes (0x19)
            if (memcpy_s(uuidArray + uuidNum, SDP_UUID16_LENGTH + 1, buffer + pos, SDP_UUID16_LENGTH + 1) != EOK) {
                LOG_ERROR("[%{public}s][%{public}d] memcpy_s SDP_UUID16 fail.", __FUNCTION__, __LINE__);
                return BT_NO_MEMORY;
            }
            pos += SDP_UUID16_LENGTH + 1;
        } else if (type == 0x1A) {
            /// UUID 4 bytes (0x1A)
            if (memcpy_s(uuidArray + uuidNum, SDP_UUID32_LENGTH + 1, buffer + pos, SDP_UUID32_LENGTH + 1) != EOK) {
                LOG_ERROR("[%{public}s][%{public}d] memcpy_s SDP_UUID32 fail.", __FUNCTION__, __LINE__);
                return BT_NO_MEMORY;
            }
            pos += SDP_UUID32_LENGTH + 1;
        } else if (type == 0x1C) {
            /// UUID 16 bytes (0x1C)
            if (memcpy_s(uuidArray + uuidNum, SDP_UUID128_LENGTH + 1, buffer + pos, SDP_UUID128_LENGTH + 1) != EOK) {
                LOG_ERROR("[%{public}s][%{public}d] memcpy_s SDP_UUID128 fail.", __FUNCTION__, __LINE__);
                return BT_NO_MEMORY;
            }
            pos += SDP_UUID128_LENGTH + 1;
        } else {
            LOG_ERROR("The type [0x%02x] is wrong.", type);
            return BT_BAD_PARAM;
        }
        uuidNum++;
    }

    handleNum = GetRecordHandleArray(uuidArray, uuidNum, handleArray, handleNum, 0);

    return handleNum;
}

static Packet *BuildAttributeListArrayCommon(int offset, uint8_t *attributeList, Packet *packet)
{
    Buffer *newBuffer = NULL;
    uint8_t *bufferPtr = NULL;

    if (offset < 0) {
        return NULL;
    } else if (offset <= 0xFF) {
        newBuffer = BufferMalloc(offset + SDP_UINT8_LENGTH + 1);
        bufferPtr = BufferPtr(newBuffer);
        bufferPtr[0] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
        bufferPtr[1] = offset;
        if (memcpy_s(bufferPtr + SDP_UINT8_LENGTH + 1, offset, attributeList, offset) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
            BufferFree(newBuffer);
            return NULL;
        }
    } else if (offset <= 0xFFFF) {
        newBuffer = BufferMalloc(offset + SDP_UINT16_LENGTH + 1);
        bufferPtr = BufferPtr(newBuffer);
        bufferPtr[0] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_16;
        *(uint16_t *)(bufferPtr + 1) = H2BE_16(offset);
        if (memcpy_s(bufferPtr + SDP_UINT16_LENGTH + 1, (uint16_t)offset, attributeList, (uint16_t)offset) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
            BufferFree(newBuffer);
            return NULL;
        }
    } else {
        return NULL;
    }
    PacketPayloadAddLast(packet, newBuffer);
    BufferFree(newBuffer);

    return packet;
}

static Packet *BuildAttributeListArray(uint8_t *buffer, uint16_t length, uint32_t *handleArray, int handleNum)
{
    uint8_t attributeList[SDP_MAX_LIST_BYTE_COUNT] = {0};
    Packet *packet = NULL;
    int offset;
    int i;

    packet = PacketMalloc(0, 0, 0);
    if (length == (SDP_UINT32_LENGTH + 1)) {
        /// Range of attribute id
        if (buffer[0] != 0x0A) {
            return NULL;
        }

        for (i = 0; i < handleNum; i++) {
            offset = BuildAttributeListByIdRange(handleArray[i], buffer, attributeList);
            packet = BuildAttributeListArrayCommon(offset, attributeList, packet);
            if (packet == NULL) {
                return NULL;
            }
        }
    } else {
        if (((length % (SDP_UINT16_LENGTH + 1)) != 0) || (buffer[0] != 0x09)) {
            return NULL;
        }
        /// List of attribute id
        for (i = 0; i < handleNum; i++) {
            offset = BuildAttributeListByIdList(handleArray[i], length, buffer, attributeList);
            packet = BuildAttributeListArrayCommon(offset, attributeList, packet);
            if (packet == NULL) {
                return NULL;
            }
        }
    }

    return packet;
}

static void SdpCreateSearchAttributeResponse(
    uint16_t lcid, uint16_t transactionId, Packet *payloadPacket, uint16_t maximumAttributeByteCount)
{
    Packet *packet = NULL;
    uint8_t *header = NULL;
    uint16_t length = PacketPayloadSize(payloadPacket);
    if (length <= 0xFF) {
        packet = PacketInheritMalloc(payloadPacket, SDP_UINT8_LENGTH + 1, 0);
        header = BufferPtr(PacketHead(packet));
        header[0] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_8;
        header[1] = length;
    } else {
        packet = PacketInheritMalloc(payloadPacket, SDP_UINT16_LENGTH + 1, 0);
        header = BufferPtr(PacketHead(packet));
        header[0] = (DE_TYPE_DES << SDP_DESCRIPTOR_SIZE_BIT) | DE_SIZE_VAR_16;
        *(uint16_t *)(header + 1) = H2BE_16(length);
    }
    SdpSendAttributeResponse(
        lcid, transactionId, SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE, maximumAttributeByteCount, packet);

    PacketFree(packet);
    packet = NULL;
}

static uint16_t SdpParseSearchAttributeRequestCommon(
    uint16_t lcid, uint16_t transactionId, uint16_t maximumAttributeByteCount, uint8_t *buffer, uint16_t offset)
{
    /// ContinuationState
    uint8_t continuationStateLen = buffer[offset];

    LOG_INFO("[%{public}s][%{public}d] continuation state Length [%hhu].", __FUNCTION__, __LINE__, continuationStateLen);
    offset++;
    /// ContinuationStateLen
    if (continuationStateLen > SDP_MAX_CONTINUATION_LEN) {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_CONT_STATE);
        return 0;
    }
    /// continuation state yes or no (is 0)
    if (continuationStateLen != 0) {
        uint8_t continuationState[SDP_MAX_CONTINUATION_LEN] = {0};
        if (memcpy_s(continuationState, SDP_MAX_CONTINUATION_LEN, buffer + offset, continuationStateLen) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
            return 0;
        }
        SdpSendAttributeFragmentResponse(
            lcid, SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE, transactionId, maximumAttributeByteCount, NULL);
        return 0;
    }

    return offset;
}

static void SdpParseSearchAttributeRequest(uint16_t lcid, uint16_t transactionId, BufferInfo *bufferInfo)
{
    uint32_t serviceSearchPatternLength = 0;
    uint32_t attributeIDListLength = 0;
    uint16_t attributeIDListPos;
    uint16_t offset = 0;

    /// ServiceSearchPattern
    uint8_t type = bufferInfo->buffer[offset];
    offset++;
    uint16_t pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &serviceSearchPatternLength);
    if (bufferInfo->length < serviceSearchPatternLength) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return;
    }
    offset += pos;

    uint16_t serviceSearchPatternPos = offset;
    offset += serviceSearchPatternLength;

    /// MaximumAttributeByteCount
    uint16_t maximumAttributeByteCount = BE2H_16(*(uint16_t *)(bufferInfo->buffer + offset));
    offset += SDP_UINT16_LENGTH;

    /// AttributeIDList
    type = bufferInfo->buffer[offset];
    offset++;
    pos = SdpGetLengthFromType(bufferInfo->buffer + offset, type, &attributeIDListLength);
    if (bufferInfo->length < attributeIDListLength) {
        LOG_ERROR("[%{public}s][%{public}d] Wrong length.", __FUNCTION__, __LINE__);
        return;
    }
    offset += pos;

    attributeIDListPos = offset;
    offset += attributeIDListLength;

    offset = SdpParseSearchAttributeRequestCommon(lcid, transactionId, maximumAttributeByteCount,
        bufferInfo->buffer, offset);
    if (offset == 0) {
        return;
    }

    uint16_t size = (uint16_t)ListGetSize(g_serviceRecordList);
    uint32_t *handleArray = (uint32_t *)MEM_MALLOC.alloc(SDP_UINT32_LENGTH * size);
    if (handleArray == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    uint8_t *bufferEnd = bufferInfo->buffer + serviceSearchPatternPos + serviceSearchPatternLength;
    (void)memset_s(handleArray, SDP_UINT32_LENGTH * size, 0, SDP_UINT32_LENGTH * size);
    int handleNum = BuildServiceRecordHandleList(bufferInfo->buffer, serviceSearchPatternPos, bufferEnd, handleArray);
    LOG_INFO("[%{public}s][%{public}d] handleNum = [%{public}d]", __FUNCTION__, __LINE__, handleNum);

    if (handleNum >= 0) {
        Packet *packet =
            BuildAttributeListArray(bufferInfo->buffer + attributeIDListPos, attributeIDListLength,
                handleArray, handleNum);
        if (packet != NULL) {
            SdpCreateSearchAttributeResponse(lcid, transactionId, packet, maximumAttributeByteCount);
            PacketFree(packet);
            packet = NULL;
        } else {
            LOG_ERROR("[%{public}s][%{public}d] transactionId[%02x] packet is NULL.",
                __FUNCTION__, __LINE__, transactionId);
            SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_REQ_SYNTAX);
        }
    } else {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_REQ_SYNTAX);
    }
    MEM_MALLOC.free(handleArray);
    handleArray = NULL;
}

/**
 * @brief    SortForAttributeId
 * @detail   Sort attribute by attribute id in ascending order.
 * @para[in] attributeItem: attribute items
 * @para[in] attributeNumber:       the number of attribute items
 * @return   void
 */
static void SortForAttributeId(AttributeItem *attributeItem, uint16_t attributeNumber)
{
    int i;
    int j;
    AttributeItem item;

    for (i = 0; i < attributeNumber - 1; i++) {
        for (j = 0; j < attributeNumber - 1 - i; j++) {
            if (attributeItem[j].attributeId > attributeItem[j + 1].attributeId) {
                item = attributeItem[j + 1];
                attributeItem[j + 1] = attributeItem[j];
                attributeItem[j] = item;
            }
        }
    }
}

/**
 * @brief  If two UUIDs of differing sizes are to be compared, the shorter UUID must be converted to
 *         the longer UUID format before comparison.
 *
 * @return True or false.
 */
static bool CompareUuid(const uint8_t *uuid1, uint16_t length1, const uint8_t *uuid2, uint16_t length2)
{
    uint8_t value1[SDP_UUID128_LENGTH] = {0};
    uint8_t value2[SDP_UUID128_LENGTH] = {0};
    bool result = false;

    if (((length1 != SDP_UUID16_LENGTH) && (length1 != SDP_UUID32_LENGTH) && (length1 != SDP_UUID128_LENGTH)) ||
        ((length2 != SDP_UUID16_LENGTH) && (length2 != SDP_UUID32_LENGTH) && (length2 != SDP_UUID128_LENGTH))) {
        LOG_ERROR("[%{public}s][%{public}d] Uuid length is invalid.", __FUNCTION__, __LINE__);
        return false;
    }
    if (length1 == SDP_UUID16_LENGTH) {
        /// uuid1 length is 2
        (void)memcpy_s(value1, SDP_UUID128_LENGTH, G_BASE_UUID, SDP_UUID128_LENGTH);
        (void)memcpy_s(value1 + SDP_UUID16_LENGTH, SDP_UUID16_LENGTH, uuid1, SDP_UUID16_LENGTH);
    } else if (length1 == SDP_UUID32_LENGTH) {
        // uuid2 length is 4
        (void)memcpy_s(value1, SDP_UUID128_LENGTH, G_BASE_UUID, SDP_UUID128_LENGTH);
        (void)memcpy_s(value1, SDP_UUID32_LENGTH, uuid1, SDP_UUID32_LENGTH);
    } else {
        (void)memcpy_s(value1, SDP_UUID128_LENGTH, uuid1, SDP_UUID128_LENGTH);
    }

    if (length2 == SDP_UUID16_LENGTH) {
        /// uuid1 length is 2
        (void)memcpy_s(value2, SDP_UUID128_LENGTH, G_BASE_UUID, SDP_UUID128_LENGTH);
        (void)memcpy_s(value2 + SDP_UUID16_LENGTH, SDP_UUID16_LENGTH, uuid2, SDP_UUID16_LENGTH);
    } else if (length2 == SDP_UUID32_LENGTH) {
        // uuid2 length is 4
        (void)memcpy_s(value2, SDP_UUID128_LENGTH, G_BASE_UUID, SDP_UUID128_LENGTH);
        if (memcpy_s(value2, SDP_UUID128_LENGTH, uuid2, SDP_UUID32_LENGTH) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
            return false;
        }
    } else {
        (void)memcpy_s(value2, SDP_UUID128_LENGTH, uuid2, SDP_UUID128_LENGTH);
    }

    result = (memcmp(value1, value2, SDP_UUID128_LENGTH) == 0) ? true : false;

    return result;
}

static bool GetUuidfromSequenceLevelThree(uint8_t *buffer, uint16_t bufferLen, uint8_t *uuid, uint16_t uuidLen)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t index = 0;

    while (index < bufferLen) {
        uint8_t type;
        type = buffer[offset];
        offset++;
        SdpGetLengthFromType(buffer + offset, type, &length);
        if ((type >> SDP_DESCRIPTOR_SIZE_BIT) == DE_TYPE_UUID) {
            if (CompareUuid(buffer + offset, length, uuid, uuidLen)) {
                return true;
            }
            offset += length;
        } else if ((type >> SDP_DESCRIPTOR_SIZE_BIT) == DE_TYPE_DES) {
            return false;
        }
        index += offset;
    }
    return false;
}

static bool GetUuidfromSequenceLevelTwo(uint8_t *buffer, uint16_t bufferLen, uint8_t *uuid, uint16_t uuidLen)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t index = 0;

    while (index < bufferLen) {
        uint8_t type;
        type = buffer[offset];
        offset++;
        uint16_t position = SdpGetLengthFromType(buffer + offset, type, &length);
        if ((type >> SDP_DESCRIPTOR_SIZE_BIT) == DE_TYPE_UUID) {
            if (CompareUuid(buffer + offset, length, uuid, uuidLen)) {
                return true;
            }
            offset += length;
        } else if ((type >> SDP_DESCRIPTOR_SIZE_BIT) == DE_TYPE_DES) {
            offset += position;
            if (GetUuidfromSequenceLevelThree(buffer + offset, length, uuid, uuidLen)) {
                return true;
            }
            offset += length;
        }
        index += offset;
    }
    return false;
}

static bool GetUuidfromSequenceLevelOne(uint8_t *buffer, uint16_t bufferLen, uint8_t *uuid, uint16_t uuidLen)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t index = 0;

    while (index < bufferLen) {
        uint8_t type;
        type = buffer[offset];
        offset++;
        uint16_t pos = SdpGetLengthFromType(buffer + offset, type, &length);
        if ((type >> SDP_DESCRIPTOR_SIZE_BIT) == DE_TYPE_UUID) {
            if (CompareUuid(buffer + offset, length, uuid, uuidLen)) {
                return true;
            }
            offset += length;
        } else if ((type >> SDP_DESCRIPTOR_SIZE_BIT) == DE_TYPE_DES) {
            offset += pos;
            if (GetUuidfromSequenceLevelTwo(buffer + offset, length, uuid, uuidLen)) {
                return true;
            }
            offset += length;
        }
        index += offset;
    }
    return false;
}

static bool GetUuidfromSequence(uint8_t *buffer, uint16_t bufferLen, uint8_t *uuid, uint16_t uuidLen)
{
    uint32_t length = 0;
    uint16_t offset = 0;
    uint16_t index = 0;

    while (index < bufferLen) {
        uint8_t type;
        uint16_t pos;
        type = buffer[offset];
        offset++;
        pos = SdpGetLengthFromType(buffer + offset, type, &length);
        if ((type >> SDP_DESCRIPTOR_SIZE_BIT) == DE_TYPE_UUID) {
            if (CompareUuid(buffer + offset, length, uuid, uuidLen)) {
                return true;
            }
            offset += length;
        } else if ((type >> SDP_DESCRIPTOR_SIZE_BIT) == DE_TYPE_DES) {
            offset += pos;
            if (GetUuidfromSequenceLevelOne(buffer + offset, length, uuid, uuidLen)) {
                return true;
            }
            offset += length;
        }
        index += offset;
    }
    return false;
}

static bool GetRecordHandleArrayByUuid(ServiceRecordItem *item, uint8_t *uuid)
{
    const uint16_t offset = 3;
    uint16_t uuidLen = 0;
    // Uuid type of request
    uint8_t uuidType = uuid[0];

    if (uuidType == 0x19) {
        uuidLen = SDP_UUID16_LENGTH;
    } else if (uuidType == 0x1A) {
        uuidLen = SDP_UUID32_LENGTH;
    } else if (uuidType == 0x1C) {
        uuidLen = SDP_UUID128_LENGTH;
    } else {
        return false;
    }

    for (int i = 0; i < item->attributeNumber; i++) {
        // Uuid type of attributeValue
        uint8_t type;
        type = item->attributeItem[i].attributeValue[offset] >> SDP_DESCRIPTOR_SIZE_BIT;
        bool ret;
        if (type == DE_TYPE_UUID) {
            /// Only one uuid
            ret = CompareUuid(item->attributeItem[i].attributeValue + offset + 1,
                item->attributeItem[i].attributeLength - offset - 1,
                uuid + 1,
                uuidLen);
            if (ret) {
                return true;
            }
        } else if (type == DE_TYPE_DES) {
            /// A sequence of uuids
            ret = GetUuidfromSequence(item->attributeItem[i].attributeValue + offset,
                item->attributeItem[i].attributeLength - offset,
                uuid + 1,
                uuidLen);
            if (ret) {
                return true;
            }
        }
    }
    return false;
}

static uint16_t GetRecordHandleNumber(
    ServiceRecordItem *item, uint8_t uuidArray[][20], int uuidNum, uint32_t *handleArray, uint16_t handleNum)
{
    for (int i = 0; i < uuidNum; i++) {
        bool ret;
        ret = GetRecordHandleArrayByUuid(item, uuidArray[i]);
        if (ret) {
            handleArray[handleNum] = item->serviceRecordHandle;
            handleNum++;
            break;
        }
    }
    return handleNum;
}

static uint16_t GetRecordHandleArray(
    uint8_t uuidArray[][20], int uuidNum, uint32_t *handleArray, uint16_t handleNum, uint16_t maxRecordCount)
{
    ServiceRecordItem *item = NULL;
    ListNode *node = NULL;

    if (g_serviceRecordList == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] ServiceRecordList is empty.", __FUNCTION__, __LINE__);
        return 0;
    }
    node = ListGetFirstNode(g_serviceRecordList);
    while (node != NULL) {
        item = ListGetNodeData(node);
        handleNum = GetRecordHandleNumber(item, uuidArray, uuidNum, handleArray, handleNum);
        if ((maxRecordCount != 0) && (handleNum >= maxRecordCount)) {
            break;
        }
        node = ListGetNextNode(node);
    }
    return handleNum;
}

static ServiceRecordItem *FindServiceRecordItem(uint32_t handle)
{
    ServiceRecordItem *item = NULL;
    ListNode *node = NULL;

    if (g_serviceRecordList == NULL) {
        return NULL;
    }
    node = ListGetFirstNode(g_serviceRecordList);
    while (node != NULL) {
        item = ListGetNodeData(node);
        if (item->serviceRecordHandle == handle) {
            return item;
        }
        node = ListGetNextNode(node);
    }
    return NULL;
}
