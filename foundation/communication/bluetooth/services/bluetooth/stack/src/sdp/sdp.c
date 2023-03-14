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

#include "sdp.h"

#include <string.h>

#include "sdp_client.h"
#include "sdp_connect.h"
#include "sdp_server.h"
#include "sdp_util.h"

#include "btstack.h"
#include "event.h"
#include "module.h"

#include "allocator.h"
#include "log.h"

#include "../btm/btm_thread.h"

#define WAIT_TIME (-1)

typedef struct {
    int traceLevel;
    Event *event;
} SdpInitializeInfo;

typedef struct {
    uint32_t handle;
    Event *event;
} SdpServiceRecordHandle;

typedef struct {
    uint32_t handle;
    Event *event;
    int result;
} SdpServiceRecord;

typedef struct {
    uint32_t handle;
    const BtUuid *classid;
    uint16_t classidNumber;
    Event *event;
    int result;
} SdpServiceClassID;

typedef struct {
    uint32_t handle;
    uint32_t state;
    Event *event;
    int result;
} SdpServiceRecordState;

typedef struct {
    uint32_t handle;
    BtUuid serviceid;
    Event *event;
    int result;
} SdpServiceId;

typedef struct {
    uint32_t handle;
    const SdpProtocolDescriptor *descriptor;
    uint16_t descriptorNumber;
    Event *event;
    int result;
} SdpProtocolDescriptorList;

typedef struct {
    uint32_t handle;
    const SdpAdditionalProtocolDescriptor *descriptorList;
    uint16_t descriptorListNumber;
    Event *event;
    int result;
} SdpAdditionalProtocolDescriptorList;

typedef struct {
    uint32_t handle;
    const BtUuid *browseUuid;
    uint16_t browseUuidNumber;
    Event *event;
    int result;
} SdpBrowseGroupList;

typedef struct {
    uint32_t handle;
    const SdpLanguageBaseAttributeId *baseAttributeId;
    uint16_t baseAttributeIdNumber;
    Event *event;
    int result;
} SdpLanguageBaseAttributeIdList;

typedef struct {
    uint32_t handle;
    uint32_t value;
    Event *event;
    int result;
} SdpServiceInfoTimeToLive;

typedef struct {
    uint32_t handle;
    uint8_t value;
    Event *event;
    int result;
} SdpServiceAvailability;

typedef struct {
    uint32_t handle;
    const SdpProfileDescriptor *profileDescriptor;
    uint16_t profileDescriptorNumber;
    Event *event;
    int result;
} SdpBluetoothProfileDescriptorList;

typedef struct {
    uint32_t handle;
    const uint8_t *url;
    uint16_t urlLen;
    Event *event;
    int result;
} SdpUrl;

typedef struct {
    uint32_t handle;
    uint16_t baseAttributeId;
    const char *name;
    uint16_t nameLen;
    Event *event;
    int result;
} SdpName;

typedef struct {
    uint32_t handle;
    uint16_t attributeId;
    SdpDataType type;
    void *attributeValue;
    uint16_t attributeValueLength;
    Event *event;
    int result;
} SdpAttributeInfo;

typedef struct {
    uint32_t handle;
    uint16_t attributeId;
    const uint8_t *attributeValue;
    uint16_t attributeValueLength;
    Event *event;
    int result;
} SdpSequenceAttributeInfo;

typedef struct {
    BtAddr addr;
    SdpUuid uuidArray;
    void *context;
    void (*ServiceSearchCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context);
    Event *event;
    int result;
} SdpServiceSearchInfo;

typedef struct {
    BtAddr addr;
    uint32_t handle;
    SdpAttributeIdList attributeIdList;
    void *context;
    void (*ServiceAttributeCb)(const BtAddr *addr, const SdpService *service, void *context);
    Event *event;
    int result;
} SdpServiceAttributeInfo;

typedef struct {
    BtAddr addr;
    SdpUuid uuidArray;
    SdpAttributeIdList attributeIdList;
    void *context;
    void (*ServiceSearchAttributeCb)(
        const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context);
    Event *event;
    int result;
} SdpServiceSearchAttributeInfo;

typedef struct {
    BtAddr addr;
    void *context;
    void (*ServiceBrowseCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context);
    Event *event;
    int result;
} SdpServiceBrowseInfo;

static void SdpInitializeTask(void *context)
{
    LOG_INFO("%{public}s", __func__);

    SdpInitializeInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }

    SdpInitializeClient();
    SdpInitializeServer();
    SdpRegisterToL2cap();
    SdpSetEnableState();

    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

static void SdpStartup(int traceLevel)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpInitializeInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpInitializeInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpInitializeInfo), 0x00, sizeof(SdpInitializeInfo));

    ret = BTM_CreateProcessingQueue(PROCESSING_QUEUE_ID_SDP, BTM_PROCESSING_QUEUE_SIZE_DEFAULT);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }

    ctx->traceLevel = traceLevel;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpInitializeTask, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx->event);
        MEM_MALLOC.free(ctx);
        return;
    }

    EventWait(ctx->event, WAIT_TIME);
    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
}

static void SdpFinalizeTask(void *context)
{
    Event *ctx = context;
    LOG_INFO("%{public}s enter", __FUNCTION__);
    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    SdpSetDisableState();
    SdpDeregisterToL2cap();
    SdpFinalizeServer();
    SdpFinalizeClient();

    BTM_DeleteProcessingQueue(PROCESSING_QUEUE_ID_SDP);
    EventSet(ctx);
}

static void SdpShutdown()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    Event *ctx = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpFinalizeTask, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx);
        return;
    }

    EventWait(ctx, WAIT_TIME);
    EventDelete(ctx);

    LOG_INFO("%{public}s end", __FUNCTION__);
}

static void SdpCreateServiceRecordTask(void *context)
{
    SdpServiceRecordHandle *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->handle = SdpCreateServiceRecord();
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

uint32_t SDP_CreateServiceRecord()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    int handle = 0;
    SdpServiceRecordHandle *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceRecordHandle));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return 0;
    }
    (void)memset_s(ctx, sizeof(SdpServiceRecordHandle), 0x00, sizeof(SdpServiceRecordHandle));

    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpCreateServiceRecordTask, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx->event);
        MEM_MALLOC.free(ctx);
        return handle;
    }

    EventWait(ctx->event, WAIT_TIME);
    EventDelete(ctx->event);
    handle = ctx->handle;

    MEM_MALLOC.free(ctx);
    return handle;
}

static void SdpDestroyServiceRecordTask(void *context)
{
    SdpServiceRecord *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpDestroyServiceRecord(ctx->handle);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_DestroyServiceRecord(uint32_t handle)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceRecord *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceRecord));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceRecord), 0x00, sizeof(SdpServiceRecord));

    ctx->handle = handle;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpDestroyServiceRecordTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);

    LOG_INFO("%{public}s exit", __FUNCTION__);
    return ret;
}

static void SdpRegisterServiceRecordTask(void *context)
{
    SdpServiceRecord *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpRegisterServiceRecord(ctx->handle);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_RegisterServiceRecord(uint32_t handle)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceRecord *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceRecord));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceRecord), 0x00, sizeof(SdpServiceRecord));

    ctx->handle = handle;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpRegisterServiceRecordTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpDeregisterServiceRecordTask(void *context)
{
    SdpServiceRecord *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpDeregisterServiceRecord(ctx->handle);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_DeregisterServiceRecord(uint32_t handle)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceRecord *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceRecord));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceRecord), 0x00, sizeof(SdpServiceRecord));

    ctx->handle = handle;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpDeregisterServiceRecordTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);

    LOG_INFO("%{public}s exit", __FUNCTION__);
    return ret;
}

static void SdpAddServiceClassIdListTask(void *context)
{
    SdpServiceClassID *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddServiceClassIdList(ctx->handle, ctx->classid, ctx->classidNumber);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddServiceClassIdList(uint32_t handle, const BtUuid *classid, uint16_t classidNumber)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceClassID *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceClassID));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceClassID), 0x00, sizeof(SdpServiceClassID));

    ctx->handle = handle;
    ctx->classid = classid;
    ctx->classidNumber = classidNumber;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddServiceClassIdListTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddServiceRecordStateTask(void *context)
{
    SdpServiceRecordState *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddServiceRecordState(ctx->handle, ctx->state);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddServiceRecordState(uint32_t handle, uint32_t state)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceRecordState *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceRecordState));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceRecordState), 0x00, sizeof(SdpServiceRecordState));

    ctx->handle = handle;
    ctx->state = state;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddServiceRecordStateTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddServiceIdTask(void *context)
{
    SdpServiceId *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddServiceId(ctx->handle, &ctx->serviceid);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddServiceId(uint32_t handle, const BtUuid *serviceid)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceId *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceId));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceId), 0x00, sizeof(SdpServiceId));

    ctx->handle = handle;
    (void)memcpy_s(&ctx->serviceid, sizeof(BtUuid), serviceid, sizeof(BtUuid));
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddServiceIdTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddProtocolDescriptorListTask(void *context)
{
    SdpProtocolDescriptorList *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddProtocolDescriptorList(ctx->handle, ctx->descriptor, ctx->descriptorNumber);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddProtocolDescriptorList(uint32_t handle, const SdpProtocolDescriptor *descriptor, uint16_t descriptorNumber)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpProtocolDescriptorList *ctx = MEM_MALLOC.alloc(sizeof(SdpProtocolDescriptorList));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpProtocolDescriptorList), 0x00, sizeof(SdpProtocolDescriptorList));

    ctx->handle = handle;
    ctx->descriptor = descriptor;
    ctx->descriptorNumber = descriptorNumber;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddProtocolDescriptorListTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddAdditionalProtocolDescriptorListTask(void *context)
{
    SdpAdditionalProtocolDescriptorList *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddAdditionalProtocolDescriptorList(ctx->handle, ctx->descriptorList, ctx->descriptorListNumber);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddAdditionalProtocolDescriptorList(
    uint32_t handle, const SdpAdditionalProtocolDescriptor *descriptorList, uint16_t descriptorListNumber)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpAdditionalProtocolDescriptorList *ctx = MEM_MALLOC.alloc(sizeof(SdpAdditionalProtocolDescriptorList));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpAdditionalProtocolDescriptorList), 0x00, sizeof(SdpAdditionalProtocolDescriptorList));

    ctx->handle = handle;
    ctx->descriptorList = descriptorList;
    ctx->descriptorListNumber = descriptorListNumber;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddAdditionalProtocolDescriptorListTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddBrowseGroupListTask(void *context)
{
    SdpBrowseGroupList *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddBrowseGroupList(ctx->handle, ctx->browseUuid, ctx->browseUuidNumber);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddBrowseGroupList(uint32_t handle, const BtUuid *browseUuid, uint16_t browseUuidNumber)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpBrowseGroupList *ctx = MEM_MALLOC.alloc(sizeof(SdpBrowseGroupList));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpBrowseGroupList), 0x00, sizeof(SdpBrowseGroupList));

    ctx->handle = handle;
    ctx->browseUuid = browseUuid;
    ctx->browseUuidNumber = browseUuidNumber;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddBrowseGroupListTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddLanguageBaseAttributeIdListTask(void *context)
{
    SdpLanguageBaseAttributeIdList *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddLanguageBaseAttributeIdList(ctx->handle, ctx->baseAttributeId, ctx->baseAttributeIdNumber);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddLanguageBaseAttributeIdList(
    uint32_t handle, const SdpLanguageBaseAttributeId *baseAttributeId, uint16_t baseAttributeIdNum)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpLanguageBaseAttributeIdList *ctx = MEM_MALLOC.alloc(sizeof(SdpLanguageBaseAttributeIdList));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpLanguageBaseAttributeIdList), 0x00, sizeof(SdpLanguageBaseAttributeIdList));

    ctx->handle = handle;
    ctx->baseAttributeId = baseAttributeId;
    ctx->baseAttributeIdNumber = baseAttributeIdNum;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddLanguageBaseAttributeIdListTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddServiceInfoTimeToLiveTask(void *context)
{
    SdpServiceInfoTimeToLive *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddServiceInfoTimeToLive(ctx->handle, ctx->value);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddServiceInfoTimeToLive(uint32_t handle, uint32_t value)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceInfoTimeToLive *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceInfoTimeToLive));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceInfoTimeToLive), 0x00, sizeof(SdpServiceInfoTimeToLive));

    ctx->handle = handle;
    ctx->value = value;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddServiceInfoTimeToLiveTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddServiceAvailabilityTask(void *context)
{
    SdpServiceAvailability *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddServiceAvailability(ctx->handle, ctx->value);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddServiceAvailability(uint32_t handle, uint8_t value)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceAvailability *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceAvailability));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceAvailability), 0x00, sizeof(SdpServiceAvailability));

    ctx->handle = handle;
    ctx->value = value;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddServiceAvailabilityTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddBluetoothProfileDescriptorListTask(void *context)
{
    SdpBluetoothProfileDescriptorList *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result =
        SdpAddBluetoothProfileDescriptorList(ctx->handle, ctx->profileDescriptor, ctx->profileDescriptorNumber);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddBluetoothProfileDescriptorList(
    uint32_t handle, const SdpProfileDescriptor *profileDescriptor, uint16_t profileDescriptorNum)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpBluetoothProfileDescriptorList *ctx = MEM_MALLOC.alloc(sizeof(SdpBluetoothProfileDescriptorList));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpBluetoothProfileDescriptorList), 0x00, sizeof(SdpBluetoothProfileDescriptorList));

    ctx->handle = handle;
    ctx->profileDescriptor = profileDescriptor;
    ctx->profileDescriptorNumber = profileDescriptorNum;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddBluetoothProfileDescriptorListTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddDocumentationUrlTask(void *context)
{
    SdpUrl *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddDocumentationUrl(ctx->handle, ctx->url, ctx->urlLen);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddDocumentationUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpUrl *ctx = MEM_MALLOC.alloc(sizeof(SdpUrl));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpUrl), 0x00, sizeof(SdpUrl));

    ctx->handle = handle;
    ctx->url = url;
    ctx->urlLen = urlLen;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddDocumentationUrlTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddClientExecutableUrlTask(void *context)
{
    SdpUrl *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddClientExecutableUrl(ctx->handle, ctx->url, ctx->urlLen);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddClientExecutableUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpUrl *ctx = MEM_MALLOC.alloc(sizeof(SdpUrl));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpUrl), 0x00, sizeof(SdpUrl));

    ctx->handle = handle;
    ctx->url = url;
    ctx->urlLen = urlLen;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddClientExecutableUrlTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddIconUrlTask(void *context)
{
    SdpUrl *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddIconUrl(ctx->handle, ctx->url, ctx->urlLen);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddIconUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpUrl *ctx = MEM_MALLOC.alloc(sizeof(SdpUrl));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpUrl), 0x00, sizeof(SdpUrl));

    ctx->handle = handle;
    ctx->url = url;
    ctx->urlLen = urlLen;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddIconUrlTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddServiceNameTask(void *context)
{
    SdpName *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddServiceName(ctx->handle, ctx->baseAttributeId, ctx->name, ctx->nameLen);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddServiceName(uint32_t handle, uint16_t baseAttributeId, const char *name, uint16_t nameLen)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpName *ctx = MEM_MALLOC.alloc(sizeof(SdpName));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpName), 0x00, sizeof(SdpName));

    ctx->handle = handle;
    ctx->baseAttributeId = baseAttributeId;
    ctx->name = name;
    ctx->nameLen = nameLen;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddServiceNameTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddServiceDescriptionTask(void *context)
{
    SdpName *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddServiceDescription(ctx->handle, ctx->baseAttributeId, ctx->name, ctx->nameLen);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddServiceDescription(
    uint32_t handle, uint16_t baseAttributeId, const char *description, uint16_t descriptionLen)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpName *ctx = MEM_MALLOC.alloc(sizeof(SdpName));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpName), 0x00, sizeof(SdpName));

    ctx->handle = handle;
    ctx->name = description;
    ctx->nameLen = descriptionLen;
    ctx->baseAttributeId = baseAttributeId;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddServiceDescriptionTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddProviderNameTask(void *context)
{
    SdpName *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpAddProviderName(ctx->handle, ctx->baseAttributeId, ctx->name, ctx->nameLen);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddProviderName(uint32_t handle, uint16_t baseAttributeId, const char *name, uint16_t nameLen)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpName *ctx = MEM_MALLOC.alloc(sizeof(SdpName));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpName), 0x00, sizeof(SdpName));

    ctx->handle = handle;
    ctx->name = name;
    ctx->nameLen = nameLen;
    ctx->baseAttributeId = baseAttributeId;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddProviderNameTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddAttributeTask(void *context)
{
    SdpAttributeInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result =
        SdpAddAttribute(ctx->handle, ctx->attributeId, ctx->type, ctx->attributeValue, ctx->attributeValueLength);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddAttribute(
    uint32_t handle, uint16_t attributeId, SdpDataType type, void *attributeValue, uint16_t attributeValueLength)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpAttributeInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpAttributeInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpAttributeInfo), 0x00, sizeof(SdpAttributeInfo));

    ctx->handle = handle;
    ctx->attributeId = attributeId;
    ctx->type = type;
    ctx->attributeValue = attributeValue;
    ctx->attributeValueLength = attributeValueLength;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddAttributeTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpAddSequenceAttributeTask(void *context)
{
    SdpSequenceAttributeInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result =
        SdpAddSequenceAttribute(ctx->handle, ctx->attributeId, ctx->attributeValue, ctx->attributeValueLength);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_AddSequenceAttribute(
    uint32_t handle, uint16_t attributeId, const uint8_t *attributeValue, uint16_t attributeValueLength)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpSequenceAttributeInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpSequenceAttributeInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpSequenceAttributeInfo), 0x00, sizeof(SdpSequenceAttributeInfo));

    ctx->handle = handle;
    ctx->attributeId = attributeId;
    ctx->attributeValue = attributeValue;
    ctx->attributeValueLength = attributeValueLength;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpAddSequenceAttributeTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpServiceSearchTask(void *context)
{
    SdpServiceSearchInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpServiceSearch(&ctx->addr, &ctx->uuidArray, ctx->context, ctx->ServiceSearchCb);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_ServiceSearch(const BtAddr *addr, const SdpUuid *uuidArray, void *context,
    void (*serviceSearchCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context))
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceSearchInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceSearchInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceSearchInfo), 0x00, sizeof(SdpServiceSearchInfo));

    (void)memcpy_s(&ctx->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    (void)memcpy_s(&ctx->uuidArray, sizeof(SdpUuid), uuidArray, sizeof(SdpUuid));
    ctx->context = context;
    ctx->ServiceSearchCb = serviceSearchCb;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpServiceSearchTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpServiceAttributeTask(void *context)
{
    SdpServiceAttributeInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result =
        SdpServiceAttribute(&ctx->addr, ctx->handle, ctx->attributeIdList, ctx->context, ctx->ServiceAttributeCb);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_ServiceAttribute(const BtAddr *addr, uint32_t handle, SdpAttributeIdList attributeIdList, void *context,
    void (*serviceAttributeCb)(const BtAddr *addr, const SdpService *service, void *context))
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceAttributeInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceAttributeInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceAttributeInfo), 0x00, sizeof(SdpServiceAttributeInfo));

    (void)memcpy_s(&ctx->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    ctx->handle = handle;
    (void)memcpy_s(&ctx->attributeIdList, sizeof(SdpAttributeIdList), &attributeIdList, sizeof(SdpAttributeIdList));
    ctx->context = context;
    ctx->ServiceAttributeCb = serviceAttributeCb;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpServiceAttributeTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpServiceSearchAttributeTask(void *context)
{
    SdpServiceSearchAttributeInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpServiceSearchAttribute(
        &ctx->addr, &ctx->uuidArray, ctx->attributeIdList, ctx->context, ctx->ServiceSearchAttributeCb);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_ServiceSearchAttribute(const BtAddr *addr, const SdpUuid *uuidArray, SdpAttributeIdList attributeIdList,
    void *context,
    void (*searchAttributeCb)(const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context))
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceSearchAttributeInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceSearchAttributeInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceSearchAttributeInfo), 0x00, sizeof(SdpServiceSearchAttributeInfo));

    (void)memcpy_s(&ctx->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    (void)memcpy_s(&ctx->uuidArray, sizeof(SdpUuid), uuidArray, sizeof(SdpUuid));
    (void)memcpy_s(&ctx->attributeIdList, sizeof(SdpAttributeIdList), &attributeIdList, sizeof(SdpAttributeIdList));
    ctx->context = context;
    ctx->ServiceSearchAttributeCb = searchAttributeCb;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpServiceSearchAttributeTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpServiceBrowseTask(void *context)
{
    SdpServiceBrowseInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] context is NULL ", __FUNCTION__, __LINE__);
        return;
    }
    ctx->result = SdpServiceBrowse(&ctx->addr, ctx->context, ctx->ServiceBrowseCb);
    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

int SDP_ServiceBrowse(const BtAddr *addr, void *context,
    void (*serviceBrowseCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context))
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;
    SdpServiceBrowseInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpServiceBrowseInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return BT_NO_MEMORY;
    }
    (void)memset_s(ctx, sizeof(SdpServiceBrowseInfo), 0x00, sizeof(SdpServiceBrowseInfo));

    (void)memcpy_s(&ctx->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    ctx->context = context;
    ctx->ServiceBrowseCb = serviceBrowseCb;
    ctx->event = EventCreate(true);

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpServiceBrowseTask, ctx);
    if (ret == BT_NO_ERROR) {
        EventWait(ctx->event, WAIT_TIME);
        ret = ctx->result;
    }

    EventDelete(ctx->event);
    MEM_MALLOC.free(ctx);
    return ret;
}

static void SdpInitialize(int traceLevel)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    return;
}

static void SdpFinalize()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);
    return;
}

Module g_sdp = {
    .name = MODULE_NAME_SDP,
    .init = SdpInitialize,
    .startup = SdpStartup,
    .shutdown = SdpShutdown,
    .cleanup = SdpFinalize,
    .dependencies = {MODULE_NAME_L2CAP},
};

MODULE_DECL(g_sdp)
