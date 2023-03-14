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

#include <stdbool.h>
#include <stdint.h>

#include "att.h"
#include "btm.h"

#include "platform/include/allocator.h"
#include "platform/include/list.h"

#include "hci/hci.h"
#include "hci/hci_error.h"
#include "l2cap/l2cap_core.h"
#include "rfcomm/rfcomm_defs.h"
#include "smp/smp_def.h"

#include "btm_snoop_filter.h"

#define HCI_PACKET_BOUNDARY_CONTINUING 0x01

#define L2CAP_SIGNALING_CHANNEL 0x0001
#define L2CAP_LE_ATTRIBUTE_PROTOCOL 0x0004
#define L2CAP_LE_SIGNALING_CHANNEL 0x0005
#define L2CAP_LE_SECURITY_MANAGER_PROTOCOL 0x0006
#define L2CAP_BREDR_SECURITY_MANAGER 0x0007

#define L2CAP_CONNECTION_REQUEST 0x02
#define L2CAP_CONNECTION_RESPONSE 0x03
#define L2CAP_CONFIGURATION_REQUEST 0x04
#define L2CAP_CONFIGURATION_RESPONSE 0x05
#define L2CAP_DISCONNECTION_REQUEST 0x06
#define L2CAP_DISCONNECTION_RESPONSE 0x07

#define L2CAP_CONNECTION_SUCCESSFUL 0x0000
#define L2CAP_CONNECTION_PENDING 0x0001

#define L2CAP_INVALID_CHANNEL_ID 0x0000

#define L2CAP_SDP_PSM 0x0001
#define L2CAP_RFCOMM_PSM 0x0003
#define L2CAP_AVCTP_PSM 0x0017
#define L2CAP_AVDTP_PSM 0x0019
#define L2CAP_AVCTP_BROWSING_PSM 0x001B
#define L2CAP_ATT_PSM 0x001F

#define AVDT_MEDIA_PACKET_HEADER_LEN 12

#define AVCT_PACKET_HEADER_LEN 3
#define AVRCP_COMPANY_ID 3
#define AVRCP_PDU_ID_GET_ELEMENT_ATTR 0x20
#define AVRCP_BROWSING_PDU_ID_GET_ITEM_ATTR 0x73

typedef enum {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
} ConnStatus;

typedef enum {
    BREDR,
    LE,
} AclType;

typedef struct {
    AclType type;
    uint16_t handle;
    BtAddr addr;
    bool prevPktIsFiltered;
} AclConnInfo;

typedef struct {
    ConnStatus status;
    uint8_t module;
    uint8_t headerLength;
    AclConnInfo *acl;
    uint16_t localCid;
    uint16_t remoteCid;
    bool isInitiator;
    bool isAvdtpMedia;
} FilterL2capChannelConnInfo;

typedef struct {
    uint16_t aclHandle;
    uint16_t remoteCid;
    uint8_t dlci;
} RemoteSearchInfo;

#pragma pack(1)
typedef struct {
    uint16_t handle : 12;
    uint16_t pbFlag : 2;
    uint16_t bcFlag : 2;
    uint16_t dataTotalLength;
} HciAclDataHeader;

typedef struct {
    uint8_t eventCode;
    uint8_t parameterTotalLength;
} HciEventHeader;

typedef struct {
    uint16_t length;
    uint16_t channelId;
} L2capBasicHeader;

typedef struct {
    uint16_t length;
    uint16_t channelId;
    uint16_t control;
} L2capNotBasicHeader;

typedef struct {
    uint8_t code;
    uint8_t identifier;
    uint16_t length;
} L2capSignalingHeader;
#pragma pack()

static List *g_aclConnList = NULL;
static List *g_filterL2capConnList = NULL;
static List *g_filterRfcommConnList = NULL;

static void FreeListNodeData(void *data)
{
    MEM_MALLOC.free(data);
}

void BtmEnableSnoopFilterAcl(void)
{
    if (g_aclConnList == NULL) {
        g_aclConnList = ListCreate(FreeListNodeData);
    }
    if (g_filterL2capConnList == NULL) {
        g_filterL2capConnList = ListCreate(FreeListNodeData);
    }
    if (g_filterRfcommConnList == NULL) {
        g_filterRfcommConnList = ListCreate(FreeListNodeData);
    }

    BTM_AddLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_RFCOMM, L2CAP_RFCOMM_PSM);
    BTM_AddLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_AVDTP, L2CAP_AVDTP_PSM);
    BTM_AddLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_AVCTP, L2CAP_AVCTP_PSM);
    BTM_AddLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_AVCTP_BROWSING, L2CAP_AVCTP_BROWSING_PSM);
    BTM_AddLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_ATT, L2CAP_ATT_PSM);
}

void BtmDisableSnoopFilterAcl(void)
{
    if (g_filterL2capConnList != NULL) {
        ListDelete(g_filterL2capConnList);
        g_filterL2capConnList = NULL;
    }
    if (g_filterRfcommConnList != NULL) {
        ListDelete(g_filterRfcommConnList);
        g_filterRfcommConnList = NULL;
    }
    if (g_aclConnList != NULL) {
        ListDelete(g_aclConnList);
        g_aclConnList = NULL;
    }

    BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_RFCOMM, L2CAP_RFCOMM_PSM);
    BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_AVDTP, L2CAP_AVDTP_PSM);
    BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_AVCTP, L2CAP_AVCTP_PSM);
    BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_AVCTP_BROWSING, L2CAP_AVCTP_BROWSING_PSM);
    BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_ATT, L2CAP_ATT_PSM);
}

static bool FindAclConnInfoByHandle(void *nodeData, void *handle)
{
    if (((AclConnInfo *)nodeData)->handle == *(uint16_t *)handle) {
        return true;
    } else {
        return false;
    }
}

static bool FindL2capConnByLocalCid(void *nodeData, void *cid)
{
    if (((FilterL2capChannelConnInfo *)nodeData)->localCid == *(uint16_t *)cid) {
        return true;
    } else {
        return false;
    }
}

static bool FindL2capConnByRemoteInfo(void *nodeData, void *info)
{
    if (((FilterL2capChannelConnInfo *)nodeData)->remoteCid == ((RemoteSearchInfo *)info)->remoteCid &&
        ((FilterL2capChannelConnInfo *)nodeData)->acl->handle == ((RemoteSearchInfo *)info)->aclHandle) {
        return true;
    } else {
        return false;
    }
}

static bool ExistL2capConnIsAvdtpSignal(uint16_t aclHandle)
{
    ListNode *node = ListGetFirstNode(g_filterL2capConnList);
    while (node != NULL) {
        FilterL2capChannelConnInfo *l2capConn = ListGetNodeData(node);
        node = ListGetNextNode(node);

        if (l2capConn->acl->handle == aclHandle && l2capConn->module == BTM_HCI_LOG_FILTER_MODULE_AVDTP &&
            l2capConn->status == CONNECTED && !l2capConn->isAvdtpMedia) {
            return true;
        }
    }
    return false;
}

static FilterL2capChannelConnInfo *AddL2capConnection(
    ConnStatus status, uint8_t module, uint16_t aclHandle, uint16_t localCid, uint16_t remoteCid)
{
    FilterL2capChannelConnInfo *l2capInfo = MEM_MALLOC.alloc(sizeof(FilterL2capChannelConnInfo));
    if (l2capInfo != NULL) {
        AclConnInfo *aclConnInfo = ListForEachData(g_aclConnList, FindAclConnInfoByHandle, &aclHandle);
        if (aclConnInfo != NULL) {
            l2capInfo->acl = aclConnInfo;
            l2capInfo->module = module;
            l2capInfo->status = status;
            l2capInfo->localCid = localCid;
            l2capInfo->remoteCid = remoteCid;
            l2capInfo->isAvdtpMedia = false;
            l2capInfo->isInitiator = false;
            l2capInfo->headerLength = sizeof(L2capBasicHeader);
            ListAddLast(g_filterL2capConnList, l2capInfo);
        }
    }

    return l2capInfo;
}

static void RemoveL2capConnectionByLocalCid(uint16_t localCid)
{
    FilterL2capChannelConnInfo *l2capInfo = ListForEachData(g_filterL2capConnList, FindL2capConnByLocalCid, &localCid);
    if (l2capInfo != NULL) {
        ListRemoveNode(g_filterL2capConnList, l2capInfo);
    }
}

static void RemoveL2capConnectionByAclHandle(uint16_t aclHandle)
{
    ListNode *node = ListGetFirstNode(g_filterL2capConnList);
    while (node != NULL) {
        FilterL2capChannelConnInfo *l2capInfo = ListGetNodeData(node);
        node = ListGetNextNode(node);

        if (l2capInfo->acl->handle == aclHandle) {
            ListRemoveNode(g_filterL2capConnList, l2capInfo);
        }
    }
}

static void AddAclConnection(AclType type, uint16_t handle, const HciBdAddr *addr, uint8_t addrType)
{
    AclConnInfo *info = MEM_MALLOC.alloc(sizeof(AclConnInfo));
    if (info != NULL) {
        info->handle = handle;
        info->type = type;
        (void)memcpy_s(info->addr.addr, sizeof(info->addr.addr), addr->raw, sizeof(addr->raw));
        info->addr.type = addrType;
        info->prevPktIsFiltered = false;
        ListAddLast(g_aclConnList, info);
    }
}

static void RemoveAclConnection(uint16_t handle)
{
    RemoveL2capConnectionByAclHandle(handle);

    AclConnInfo *info = ListForEachData(g_aclConnList, FindAclConnInfoByHandle, &handle);
    if (info != NULL) {
        ListRemoveNode(g_aclConnList, info);
    }
}

static void SetAclPrevPktIsFiltered(uint16_t handle, bool isFiltered)
{
    AclConnInfo *info = ListForEachData(g_aclConnList, FindAclConnInfoByHandle, &handle);
    if (info != NULL) {
        info->prevPktIsFiltered = isFiltered;
    }
}

static void ProcessHciLeConnInfo(const uint8_t **data, uint16_t originalLength)
{
    uint16_t offset = sizeof(HciEventHeader);
    const uint8_t *subEventCode = *data + offset;
    offset += sizeof(uint8_t);
    switch (*subEventCode) {
        case HCI_LE_CONNECTION_COMPLETE_EVENT: {
            HciLeConnectionCompleteEventParam *evtParam = (HciLeConnectionCompleteEventParam *)(*data + offset);
            if (evtParam->status == HCI_SUCCESS) {
                AddAclConnection(LE, evtParam->connectionHandle, &evtParam->peerAddress, evtParam->peerAddressType);
            }
            AddL2capConnection(CONNECTED,
                BTM_HCI_LOG_FILTER_MODULE_ATT,
                evtParam->connectionHandle,
                L2CAP_LE_ATTRIBUTE_PROTOCOL,
                L2CAP_LE_ATTRIBUTE_PROTOCOL);
            AddL2capConnection(CONNECTED,
                BTM_HCI_LOG_FILTER_MODULE_SM,
                evtParam->connectionHandle,
                L2CAP_LE_SECURITY_MANAGER_PROTOCOL,
                L2CAP_LE_SECURITY_MANAGER_PROTOCOL);
            break;
        }
        case HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT: {
            HciLeEnhancedConnectionCompleteEventParam *evtParam =
                (HciLeEnhancedConnectionCompleteEventParam *)(*data + offset);
            if (evtParam->status == HCI_SUCCESS) {
                AddAclConnection(LE, evtParam->connectionHandle, &evtParam->peerAddress, evtParam->peerAddressType);
            }
            AddL2capConnection(CONNECTED,
                BTM_HCI_LOG_FILTER_MODULE_ATT,
                evtParam->connectionHandle,
                L2CAP_LE_ATTRIBUTE_PROTOCOL,
                L2CAP_LE_ATTRIBUTE_PROTOCOL);
            AddL2capConnection(CONNECTED,
                BTM_HCI_LOG_FILTER_MODULE_SM,
                evtParam->connectionHandle,
                L2CAP_LE_SECURITY_MANAGER_PROTOCOL,
                L2CAP_LE_SECURITY_MANAGER_PROTOCOL);
            break;
        }
        default:
            break;
    }
}

static void ProcessHciConnInfo(const uint8_t **data, uint16_t originalLength)
{
    uint16_t offset = 0;
    HciEventHeader *header = (HciEventHeader *)*data;
    offset += sizeof(HciEventHeader);

    switch (header->eventCode) {
        case HCI_CONNECTION_COMPLETE_EVENT: {
            HciConnectionCompleteEventParam *evtParam = (HciConnectionCompleteEventParam *)(*data + offset);
            if ((evtParam->linkType == HCI_LINK_TYPE_ACL) && (evtParam->status == HCI_SUCCESS)) {
                AddAclConnection(BREDR, evtParam->connectionHandle, &evtParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
                AddL2capConnection(CONNECTED,
                    BTM_HCI_LOG_FILTER_MODULE_SM,
                    evtParam->connectionHandle,
                    L2CAP_BREDR_SECURITY_MANAGER,
                    L2CAP_BREDR_SECURITY_MANAGER);
            }
            break;
        }
        case HCI_DISCONNECT_COMPLETE_EVENT: {
            HciDisconnectCompleteEventParam *evtParam = (HciDisconnectCompleteEventParam *)(*data + offset);
            if (evtParam->status == HCI_SUCCESS) {
                RemoveAclConnection(evtParam->connectionHandle);
            }
            break;
        }
        case HCI_LE_META_EVENT:
            ProcessHciLeConnInfo(data, originalLength);
            break;
        default:
            break;
    }
}

void BtmFilterCheckAndSaveAclConnInfo(const uint8_t **data, uint16_t originalLength)
{
    HciEventHeader *header = (HciEventHeader *)*data;

    switch (header->eventCode) {
        case HCI_CONNECTION_COMPLETE_EVENT:
            ProcessHciConnInfo(data, originalLength);
            break;
        case HCI_DISCONNECT_COMPLETE_EVENT:
            ProcessHciConnInfo(data, originalLength);
            break;
        case HCI_LE_META_EVENT: {
            uint16_t offset = sizeof(HciEventHeader);
            const uint8_t *subEventCode = *data + offset;

            switch (*subEventCode) {
                case HCI_LE_CONNECTION_COMPLETE_EVENT:
                    ProcessHciConnInfo(data, originalLength);
                    break;
                case HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT:
                    ProcessHciConnInfo(data, originalLength);
                    break;
                default:
                    break;
            }
        } break;
        default:
            break;
    }
}

static void ProcessL2capConnectionRequest(
    uint8_t type, const AclConnInfo *aclInfo, const L2capSignalingHeader *l2capsignalingHeader, const uint8_t *data)
{
    uint16_t offset = 0;
    uint16_t *psm = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);
    uint16_t *scid = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);

    if (l2capsignalingHeader->length < offset) {
        return;
    }

    BtmSnoopFilterInfo cmpinfo = {
        .isLocal = (type == TRANSMISSON_TYPE_H2C_DATA) ? false : true,
        .l2capPsm = *psm,
    };
    if (!cmpinfo.isLocal) {
        cmpinfo.remoteAddr = aclInfo->addr;
    }
    BtmSnoopFilterInfo *info = ListForEachData(BtmGetFilterInfoList(), BtmFindFilterInfoByInfoUsePsm, &cmpinfo);
    if (info != NULL) {
        FilterL2capChannelConnInfo *l2capInfo = NULL;
        if (type == TRANSMISSON_TYPE_H2C_DATA) {
            l2capInfo = AddL2capConnection(CONNECTING, info->module, aclInfo->handle, *scid, L2CAP_INVALID_CHANNEL_ID);
        } else {
            l2capInfo = AddL2capConnection(CONNECTING, info->module, aclInfo->handle, L2CAP_INVALID_CHANNEL_ID, *scid);
        }
        if (l2capInfo != NULL) {
            l2capInfo->isInitiator = (type == TRANSMISSON_TYPE_H2C_DATA);
            if (*psm == L2CAP_AVDTP_PSM && ExistL2capConnIsAvdtpSignal(aclInfo->handle)) {
                l2capInfo->isAvdtpMedia = true;
            }
        }
    }
}

static void ProcessL2capConnectionResponse(
    uint8_t type, const AclConnInfo *aclInfo, const L2capSignalingHeader *l2capsignalingHeader, const uint8_t *data)
{
    uint16_t offset = 0;
    uint16_t *dcid = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);
    uint16_t *scid = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);
    uint16_t *result = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);

    if (l2capsignalingHeader->length < offset || *result == L2CAP_CONNECTION_PENDING) {
        return;
    }

    FilterL2capChannelConnInfo *connInfo = NULL;
    if (type == TRANSMISSON_TYPE_H2C_DATA) {
        RemoteSearchInfo remoteInfo = {
            .aclHandle = aclInfo->handle,
            .remoteCid = *scid,
        };
        connInfo = ListForEachData(g_filterL2capConnList, FindL2capConnByRemoteInfo, &remoteInfo);
    } else {
        connInfo = ListForEachData(g_filterL2capConnList, FindL2capConnByLocalCid, scid);
    }

    if (connInfo != NULL) {
        if (*result == L2CAP_CONNECTION_SUCCESSFUL) {
            if (type == TRANSMISSON_TYPE_H2C_DATA) {
                connInfo->localCid = *dcid;
            } else {
                connInfo->remoteCid = *dcid;
            }
            connInfo->status = CONNECTED;
        } else {
            ListRemoveNode(g_filterL2capConnList, connInfo);
        }
    }
}

static void ProcessL2capConfigurationRequest(
    uint8_t type, const AclConnInfo *aclInfo, const L2capSignalingHeader *l2capsignalingHeader, const uint8_t *data)
{
    uint16_t offset = 0;
    uint16_t *dcid = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);
    offset += sizeof(uint16_t); /* flag */

    while (offset + sizeof(uint8_t) + sizeof(uint8_t) < l2capsignalingHeader->length) {
        uint8_t *optionType = (uint8_t *)(data + offset);
        offset += sizeof(uint8_t);
        uint8_t *length = (uint8_t *)(data + offset);
        offset += sizeof(uint8_t);
        switch (*optionType) {
            case L2CAP_OPTION_RETRANSMISSION_AND_FLOW_CONTROL: {
                uint8_t mode = *(uint8_t *)(data + offset);
                offset += sizeof(uint8_t);

                FilterL2capChannelConnInfo *connInfo = NULL;
                if (type == TRANSMISSON_TYPE_H2C_DATA) {
                    RemoteSearchInfo remoteInfo = {
                        .aclHandle = aclInfo->handle,
                        .remoteCid = *dcid,
                    };
                    connInfo = ListForEachData(g_filterL2capConnList, FindL2capConnByRemoteInfo, &remoteInfo);
                } else {
                    connInfo = ListForEachData(g_filterL2capConnList, FindL2capConnByLocalCid, dcid);
                }

                if (connInfo != NULL) {
                    connInfo->headerLength =
                        (mode == L2CAP_BASIC_MODE) ? sizeof(L2capBasicHeader) : sizeof(L2capNotBasicHeader);
                }
            } break;
            default:
                break;
        }
        offset += *length;
    }
}

static void ProcessL2capConfigurationResponse(
    uint8_t type, const AclConnInfo *aclInfo, const L2capSignalingHeader *l2capsignalingHeader, const uint8_t *data)
{
    uint16_t offset = 0;
    uint16_t *scid = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);
    offset += sizeof(uint16_t); /* flag */
    offset += sizeof(uint16_t); /* result */

    if (offset == l2capsignalingHeader->length) {
        return;
    }

    while (offset + sizeof(uint8_t) + sizeof(uint8_t) < l2capsignalingHeader->length) {
        uint8_t *optionType = (uint8_t *)(data + offset);
        offset += sizeof(uint8_t);
        uint8_t *length = (uint8_t *)(data + offset);
        offset += sizeof(uint8_t);
        switch (*optionType) {
            case L2CAP_OPTION_RETRANSMISSION_AND_FLOW_CONTROL: {
                uint8_t mode = *(uint8_t *)(data + offset);
                offset += sizeof(uint8_t);

                FilterL2capChannelConnInfo *connInfo = NULL;
                if (type == TRANSMISSON_TYPE_H2C_DATA) {
                    RemoteSearchInfo remoteInfo = {
                        .aclHandle = aclInfo->handle,
                        .remoteCid = *scid,
                    };
                    connInfo = ListForEachData(g_filterL2capConnList, FindL2capConnByRemoteInfo, &remoteInfo);
                } else {
                    connInfo = ListForEachData(g_filterL2capConnList, FindL2capConnByLocalCid, scid);
                }

                if (connInfo != NULL) {
                    connInfo->headerLength =
                        (mode == L2CAP_BASIC_MODE) ? sizeof(L2capBasicHeader) : sizeof(L2capNotBasicHeader);
                }
            } break;
            default:
                break;
        }
        offset += *length;
    }
}

static void ProcessL2capDisconnectionResponse(
    uint8_t type, const AclConnInfo *aclInfo, const L2capSignalingHeader *l2capsignalingHeader, const uint8_t *data)
{
    uint16_t offset = 0;
    uint16_t *dcid = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);
    uint16_t *scid = (uint16_t *)(data + offset);
    offset += sizeof(uint16_t);

    if (l2capsignalingHeader->length < offset) {
        return;
    }

    RemoveL2capConnectionByLocalCid((type == TRANSMISSON_TYPE_H2C_DATA) ? *dcid : *scid);
}

static void ProcessL2capConnInfo(uint8_t type, const AclConnInfo *aclInfo, const uint8_t *data, uint16_t originalLength)
{
    uint16_t offset = 0;
    L2capBasicHeader *l2capHeader = (L2capBasicHeader *)(data + offset);
    offset += sizeof(L2capBasicHeader);
    L2capSignalingHeader *l2capsignalingHeader = (L2capSignalingHeader *)(data + offset);
    offset += sizeof(L2capSignalingHeader);

    if (l2capHeader->length < offset) {
        return;
    }

    switch (l2capsignalingHeader->code) {
        case L2CAP_CONNECTION_REQUEST:
            ProcessL2capConnectionRequest(type, aclInfo, l2capsignalingHeader, data + offset);
            break;
        case L2CAP_CONNECTION_RESPONSE:
            ProcessL2capConnectionResponse(type, aclInfo, l2capsignalingHeader, data + offset);
            break;
        case L2CAP_CONFIGURATION_REQUEST:
            ProcessL2capConfigurationRequest(type, aclInfo, l2capsignalingHeader, data + offset);
            break;
        case L2CAP_CONFIGURATION_RESPONSE:
            ProcessL2capConfigurationResponse(type, aclInfo, l2capsignalingHeader, data + offset);
            break;
        case L2CAP_DISCONNECTION_REQUEST:
            break;
        case L2CAP_DISCONNECTION_RESPONSE:
            ProcessL2capDisconnectionResponse(type, aclInfo, l2capsignalingHeader, data + offset);
            break;
        default:
            break;
    }
}

static FilterL2capChannelConnInfo *SearchL2capChannelConnInfo(uint8_t type, uint16_t aclHandle, uint16_t channelId)
{
    if (type == TRANSMISSON_TYPE_H2C_DATA) {
        RemoteSearchInfo remoteInfo = {
            .aclHandle = aclHandle,
            .remoteCid = channelId,
        };
        return ListForEachData(g_filterL2capConnList, FindL2capConnByRemoteInfo, &remoteInfo);
    } else {
        return ListForEachData(g_filterL2capConnList, FindL2capConnByLocalCid, &channelId);
    }
}

static bool RfcommDlciFilterCheck(uint8_t type, uint16_t aclHandle, uint16_t channelId, uint8_t dlci, uint8_t *module)
{
    AclConnInfo *aclInfo = ListForEachData(g_aclConnList, FindAclConnInfoByHandle, &aclHandle);
    if (aclInfo == NULL) {
        return false;
    }

    FilterL2capChannelConnInfo *l2capInfo = SearchL2capChannelConnInfo(type, aclHandle, channelId);
    if (l2capInfo == NULL) {
        return false;
    }
    BtmSnoopFilterInfo cmpinfo = {
        .isLocal = !(l2capInfo->isInitiator ^ (dlci & 0x01)),
        .rfcommScn = dlci >> RFCOMM_DLCI_SHIFT_SCN,
    };
    if (!cmpinfo.isLocal) {
        cmpinfo.remoteAddr = aclInfo->addr;
    }
    BtmSnoopFilterInfo *btmSnoopFilterInfo =
        ListForEachData(BtmGetFilterInfoList(), BtmFindFilterInfoByInfoUseScn, &cmpinfo);
    if (btmSnoopFilterInfo == NULL) {
        return false;
    }
    *module = btmSnoopFilterInfo->module;

    return true;
}

static bool RfcommDataCheckFrameTypeUih(const uint8_t **data, uint16_t originalLength,
    FilterL2capChannelConnInfo *l2capInfo, uint16_t *offset, uint16_t *payloadLen)
{
    *offset = sizeof(HciAclDataHeader) + l2capInfo->headerLength;
    *offset += sizeof(uint8_t);  // address
    const uint8_t *frameType = *data + *offset;
    *offset += sizeof(uint8_t);  // frameType

    if ((*frameType & (~PF)) != FRAME_TYPE_UIH) {
        return false;
    }

    const uint8_t *length = *data + *offset;
    if (*length & 0x01) {
        *offset += sizeof(uint8_t);
        if (payloadLen != NULL) {
            *payloadLen = *length >> 1;
        }
    } else {
        *offset += sizeof(uint16_t);
        if (payloadLen != NULL) {
            *payloadLen = *(uint16_t *)length >> 1;
        }
    }

    if ((*frameType & (PF)) == PF) {
        *offset += sizeof(uint8_t); /* credit */
    }
    return true;
}

static bool RfcommDataFilterUseHfp(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    uint16_t offset;
    uint16_t payloadLen;
    char *filterAT[] = {"CLCC", "CLIP", "CNUM", "BINP", "CCWA"};

    if (!RfcommDataCheckFrameTypeUih(data, originalLength, l2capInfo, &offset, &payloadLen)) {
        return false;
    }

    if (payloadLen < strlen("AT+")) {
        return false;
    }

    {
        char *at = "ATD";
        if (!memcmp(*data + offset, at, strlen(at))) {
            offset += strlen(at);
            BtmChangeIncludeLength(includedLength, offset);
            uint8_t *copyData = BtmCreateFilterBuffer(includedLength, *data);
            *data = copyData;
            return true;
        }
    }

    offset += strlen("AT+"); /* "AT+" or "\r\n+" */
    for (uint8_t ii = 0; ii < sizeof(filterAT) / sizeof(filterAT[0]); ii++) {
        char *at = filterAT[ii];
        if (payloadLen < strlen("AT+") + strlen(at)) {
            continue;
        } else {
            if (!memcmp(*data + offset, at, strlen(at))) {
                offset += strlen(at) + 1; /* '?' '=' other */
                BtmChangeIncludeLength(includedLength, offset);
                *data = BtmCreateFilterBuffer(includedLength, *data);
                return true;
            }
        }
    }
    return false;
}

static bool RfcommDataFilterUseOther(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    uint16_t offset;

    if (RfcommDataCheckFrameTypeUih(data, originalLength, l2capInfo, &offset, NULL)) {
        BtmChangeIncludeLength(includedLength, offset);
        *data = BtmCreateFilterBuffer(includedLength, *data);
        return true;
    }
    return false;
}

static bool IsL2capSignalingChannelData(const uint8_t *data, uint16_t originalLength)
{
    uint16_t offset = 0;
    offset += sizeof(uint16_t);
    uint16_t channelID = *(uint16_t *)(data + offset);
    if (channelID == L2CAP_SIGNALING_CHANNEL) {
        return true;
    } else {
        return false;
    }
}

static bool L2capDataFilterUseGoepPbap(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    BtmChangeIncludeLength(includedLength, sizeof(HciAclDataHeader) + l2capInfo->headerLength);
    *data = BtmCreateFilterBuffer(includedLength, *data);
    return true;
}

static bool L2capDataFilterUseGoepMap(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    BtmChangeIncludeLength(includedLength, sizeof(HciAclDataHeader) + l2capInfo->headerLength);
    *data = BtmCreateFilterBuffer(includedLength, *data);
    return true;
}

static bool L2capDataFilterUseRfcomm(uint8_t type, const uint8_t **data, uint16_t originalLength,
    uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    uint16_t offset = 0;
    HciAclDataHeader *hciHeader = (HciAclDataHeader *)(*data + offset);
    offset += sizeof(HciAclDataHeader);
    L2capBasicHeader *l2capHeader = (L2capBasicHeader *)(*data + offset);
    offset += l2capInfo->headerLength;
    const uint8_t *address = *data + offset;

    uint8_t dlci = *address >> RFCOMM_SHIFT_DLCI;
    if (dlci == 0) {
        return false;
    }
    uint8_t module;
    bool isFiltered = false;

    if (RfcommDlciFilterCheck(type, hciHeader->handle, l2capHeader->channelId, dlci, &module)) {
        switch (module) {
            case BTM_HCI_LOG_FILTER_MODULE_HFP:
                isFiltered = RfcommDataFilterUseHfp(data, originalLength, includedLength, l2capInfo);
                break;
            default:
                isFiltered = RfcommDataFilterUseOther(data, originalLength, includedLength, l2capInfo);
                break;
        }
    } else {
        isFiltered = RfcommDataFilterUseOther(data, originalLength, includedLength, l2capInfo);
    }
    return isFiltered;
}

static bool L2capDataFilterUseAvdtp(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    uint16_t offset = sizeof(HciAclDataHeader) + l2capInfo->headerLength + AVDT_MEDIA_PACKET_HEADER_LEN;

    BtmChangeIncludeLength(includedLength, offset);
    *data = BtmCreateFilterBuffer(includedLength, *data);
    return true;
}

static bool L2capDataFilterUseAvctp(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    uint16_t offset = sizeof(HciAclDataHeader) + l2capInfo->headerLength + AVCT_PACKET_HEADER_LEN;
    offset += sizeof(uint8_t) + sizeof(uint8_t);
    const uint8_t *opCode = *data + offset;
    offset += sizeof(uint8_t);

    if (*opCode != 0x00) {
        return false;
    }

    offset += AVRCP_COMPANY_ID;
    const uint8_t *pduId = *data + offset;
    offset += sizeof(uint8_t);
    if (*pduId == AVRCP_PDU_ID_GET_ELEMENT_ATTR) {
        BtmChangeIncludeLength(includedLength, offset);
        *data = BtmCreateFilterBuffer(includedLength, *data);
    }

    return false;
}

static bool L2capDataFilterUseAvctpBrowsing(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    uint16_t offset = sizeof(HciAclDataHeader) + l2capInfo->headerLength + AVCT_PACKET_HEADER_LEN;
    const uint8_t *pduId = *data + offset;
    offset += sizeof(uint8_t);
    if (*pduId == AVRCP_BROWSING_PDU_ID_GET_ITEM_ATTR) {
        BtmChangeIncludeLength(includedLength, offset);
        *data = BtmCreateFilterBuffer(includedLength, *data);
    }

    return false;
}

static bool L2capDataFilterUseAtt(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    uint16_t offset = sizeof(HciAclDataHeader) + l2capInfo->headerLength;
    const uint8_t *opcode = (*data + offset);
    offset += sizeof(uint8_t);

    switch (*opcode) {
        case FIND_INFORMATION_RESPONSE:
        case FIND_BY_TYPE_VALUE_REQUEST:
        case READ_BY_TYPE_RESPONSE:
        case READ_RESPONSE:
        case READ_BLOB_RESPONSE:
        case READ_MULTIPLE_RESPONSE:
        case READ_BY_GROUP_TYPE_RESPONSE:
        case WRITE_REQUEST:
        case WRITE_COMMAND:
        case PREPARE_WRITE_REQUEST:
        case PREPARE_WRITE_RESPONSE:
        case READ_MULTIPLE_VARIABLE_RESPONSE:
        case MULTIPLE_HANDLE_VALUE_NOTIFICATION:
        case HANDLE_VALUE_NOTIFICATION:
        case HANDLE_VALUE_INDICATION:
        case SIGNED_WRITE_COMMAND:
            BtmChangeIncludeLength(includedLength, offset);
            *data = BtmCreateFilterBuffer(includedLength, *data);
            break;
        default:
            return false;
    }

    return true;
}

static bool L2capDataFilterUseSmp(
    const uint8_t **data, uint16_t originalLength, uint16_t *includedLength, FilterL2capChannelConnInfo *l2capInfo)
{
    uint16_t offset = sizeof(HciAclDataHeader) + l2capInfo->headerLength;
    const uint8_t *opcode = (*data + offset);
    offset += sizeof(uint8_t);

    switch (*opcode) {
        case SMP_CODE_PAIRING_CFM:
        case SMP_CODE_PAIRING_RAND:
        case SMP_CODE_ENCRYPTION_INFO:
        case SMP_CODE_MASTER_IDENTITY:
        case SMP_CODE_IDENTITY_INFO:
        case SMP_CODE_SIGNING_INFO:
        case SMP_CODE_PAIRING_PUBLIC_KEY:
        case SMP_CODE_PAIRING_DHKEY_CHECK:
            BtmChangeIncludeLength(includedLength, offset);
            *data = BtmCreateFilterBuffer(includedLength, *data);
            break;
        case SMP_CODE_IDENTITY_ADDR_INFO:
            offset += sizeof(uint8_t);
            uint8_t *copyData = BtmCreateFilterBuffer(includedLength, *data);
            BtmFilterAddress(copyData + offset, BT_ADDRESS_SIZE);
            *data = copyData;
            break;
        default:
            return false;
    }

    return true;
}

static void L2capDataCheckFilter(uint8_t type, FilterL2capChannelConnInfo *connInfo, const uint8_t **data,
    uint16_t originalLength, uint16_t *includedLength)
{
    bool isFiltered;

    switch (connInfo->module) {
        case BTM_HCI_LOG_FILTER_MODULE_PBAP:
            isFiltered = L2capDataFilterUseGoepPbap(data, originalLength, includedLength, connInfo);
            break;
        case BTM_HCI_LOG_FILTER_MODULE_MAP:
            isFiltered = L2capDataFilterUseGoepMap(data, originalLength, includedLength, connInfo);
            break;
        case BTM_HCI_LOG_FILTER_MODULE_RFCOMM:
            isFiltered = L2capDataFilterUseRfcomm(type, data, originalLength, includedLength, connInfo);
            break;
        case BTM_HCI_LOG_FILTER_MODULE_AVCTP:
            isFiltered = L2capDataFilterUseAvctp(data, originalLength, includedLength, connInfo);
            break;
        case BTM_HCI_LOG_FILTER_MODULE_AVCTP_BROWSING:
            isFiltered = L2capDataFilterUseAvctpBrowsing(data, originalLength, includedLength, connInfo);
            break;
        case BTM_HCI_LOG_FILTER_MODULE_AVDTP:
            if (connInfo->isAvdtpMedia) {
                isFiltered = L2capDataFilterUseAvdtp(data, originalLength, includedLength, connInfo);
            } else {
                isFiltered = false;
            }
            break;
        case BTM_HCI_LOG_FILTER_MODULE_ATT:
            isFiltered = L2capDataFilterUseAtt(data, originalLength, includedLength, connInfo);
            break;
        case BTM_HCI_LOG_FILTER_MODULE_SM:
            isFiltered = L2capDataFilterUseSmp(data, originalLength, includedLength, connInfo);
            break;
        default:
            return;
    }
    SetAclPrevPktIsFiltered(connInfo->acl->handle, isFiltered);
}

void BtmFilterAclData(uint8_t type, const uint8_t **data, uint16_t originalLength, uint16_t *includedLength)
{
    uint16_t offset = 0;
    HciAclDataHeader *hciHeader = (HciAclDataHeader *)(*data + offset);
    offset += sizeof(HciAclDataHeader);

    uint16_t handle = hciHeader->handle;
    AclConnInfo *aclInfo = ListForEachData(g_aclConnList, FindAclConnInfoByHandle, &handle);
    if (aclInfo == NULL) {
        return;
    }

    if (hciHeader->pbFlag == HCI_PACKET_BOUNDARY_CONTINUING) {
        if (aclInfo->prevPktIsFiltered) {
            BtmChangeIncludeLength(includedLength, sizeof(HciAclDataHeader));
            *data = BtmCreateFilterBuffer(includedLength, *data);
        }
        return;
    } else {
        aclInfo->prevPktIsFiltered = false;
    }

    if (IsL2capSignalingChannelData(*data + offset, originalLength)) {
        ProcessL2capConnInfo(type, aclInfo, *data + offset, originalLength);
        return;
    }

    L2capBasicHeader *l2capHeader = (L2capBasicHeader *)(*data + offset);

    FilterL2capChannelConnInfo *connInfo = SearchL2capChannelConnInfo(type, hciHeader->handle, l2capHeader->channelId);
    if (connInfo == NULL) {
        return;
    }

    L2capDataCheckFilter(type, connInfo, data, originalLength, includedLength);
}
