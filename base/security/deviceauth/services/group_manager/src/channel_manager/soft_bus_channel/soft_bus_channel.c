/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "soft_bus_channel.h"

#include "common_defs.h"
#include "device_auth.h"
#include "device_auth_defines.h"
#include "group_manager.h"
#include "hc_log.h"
#include "inner_session.h"
#include "session.h"
#include "session_manager.h"
#include "task_manager.h"

typedef struct {
    HcTaskBase base;
    int64_t requestId;
    int64_t channelId;
    CJson *params;
} SoftBusTask;

typedef struct {
    int64_t requestId;
    int64_t channelId;
} ChannelEntry;

DECLARE_HC_VECTOR(ChannelEntryVec, ChannelEntry);
IMPLEMENT_HC_VECTOR(ChannelEntryVec, ChannelEntry, 1)
static ChannelEntryVec g_channelVec;
static HcMutex *g_channelMutex = NULL;

static int32_t GetReqIdByChannelId(int64_t channelId, int64_t *returnReqId)
{
    uint32_t index;
    ChannelEntry *entry = NULL;
    g_channelMutex->lock(g_channelMutex);
    FOR_EACH_HC_VECTOR(g_channelVec, index, entry) {
        if ((entry != NULL) && (channelId == entry->channelId)) {
            *returnReqId = entry->requestId;
            g_channelMutex->unlock(g_channelMutex);
            return HC_SUCCESS;
        }
    }
    g_channelMutex->unlock(g_channelMutex);
    return HC_ERR_REQUEST_NOT_FOUND;
}

static int32_t AddChannelEntry(int64_t requestId, int64_t channelId)
{
    int64_t tmpReqId = DEFAULT_REQUEST_ID;
    if (GetReqIdByChannelId(channelId, &tmpReqId) == HC_SUCCESS) {
        LOGE("A request to use the channel already exists!");
        return HC_ERR_REQUEST_EXIST;
    }
    ChannelEntry entry = {
        .channelId = channelId,
        .requestId = requestId
    };
    g_channelMutex->lock(g_channelMutex);
    g_channelVec.pushBack(&g_channelVec, &entry);
    g_channelMutex->unlock(g_channelMutex);
    return HC_SUCCESS;
}

static void RemoveChannelEntry(int64_t channelId)
{
    uint32_t index;
    ChannelEntry *entry = NULL;
    g_channelMutex->lock(g_channelMutex);
    FOR_EACH_HC_VECTOR(g_channelVec, index, entry) {
        if ((entry != NULL) && (channelId == entry->channelId)) {
            ChannelEntry tmpEntry;
            HC_VECTOR_POPELEMENT(&g_channelVec, &tmpEntry, index);
            g_channelMutex->unlock(g_channelMutex);
            return;
        }
    }
    g_channelMutex->unlock(g_channelMutex);
}

static void DoOnChannelOpened(HcTaskBase *baseTask)
{
    if (baseTask == NULL) {
        LOGE("The input task is NULL!");
        return;
    }
    LOGI("[Start]: DoOnChannelOpened!");
    SoftBusTask *task = (SoftBusTask *)baseTask;
    OnChannelOpened(task->requestId, task->channelId);
}

static void InitSoftBusTask(SoftBusTask *task, int64_t requestId, int64_t channelId)
{
    task->base.doAction = DoOnChannelOpened;
    task->base.destroy = NULL;
    task->channelId = channelId;
    task->requestId = requestId;
}

static char *GenRecvData(int64_t channelId, const void *data, uint32_t dataLen, int64_t *requestId)
{
    char *dataStr = (char *)HcMalloc(dataLen + 1, 0);
    if (dataStr == NULL) {
        LOGE("Failed to allocate dataStr memory!");
        return NULL;
    }
    if (memcpy_s(dataStr, dataLen + 1, data, dataLen) != HC_SUCCESS) {
        LOGE("Failed to copy data!");
        HcFree(dataStr);
        return NULL;
    }
    CJson *recvData = CreateJsonFromString(dataStr);
    HcFree(dataStr);
    if (recvData == NULL) {
        LOGE("Failed to create recvData from string!");
        return NULL;
    }
    if (GetInt64FromJson(recvData, FIELD_REQUEST_ID, requestId) != HC_SUCCESS) {
        LOGE("Failed to get requestId from recvData!");
        FreeJson(recvData);
        return NULL;
    }
    if (AddByteToJson(recvData, FIELD_CHANNEL_ID, (uint8_t *)&channelId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to add channelId to recvData!");
        FreeJson(recvData);
        return NULL;
    }
    char *recvDataStr = PackJsonToString(recvData);
    FreeJson(recvData);
    if (recvDataStr == NULL) {
        LOGE("Failed to convert json to string!");
        return NULL;
    }
    return recvDataStr;
}

static bool IsServer(int sessionId)
{
    return GetSessionSide(sessionId) == 0;
}

static int OnChannelOpenedCb(int sessionId, int result)
{
    if (IsServer(sessionId)) {
        LOGD("Peer device open channel!");
        return HC_SUCCESS;
    }
    int64_t requestId = 0;
    if (GetReqIdByChannelId(sessionId, &requestId) != HC_SUCCESS) {
        LOGE("The request corresponding to the channel is not found!");
        return HC_ERR_REQUEST_NOT_FOUND;
    }
    if (result != HC_SUCCESS) {
        LOGE("[SoftBus][Out]: Failed to open channel! res: %d", result);
        DestroySession(requestId);
        return HC_ERR_SOFT_BUS;
    }
    LOGD("[Start]: OnChannelOpened! [ReqId]: %" PRId64 ", [ChannelId]: %d", requestId, sessionId);
    SoftBusTask *task = (SoftBusTask *)HcMalloc(sizeof(SoftBusTask), 0);
    if (task == NULL) {
        LOGE("Failed to allocate task memory!");
        DestroySession(requestId);
        return HC_ERR_ALLOC_MEMORY;
    }
    InitSoftBusTask(task, requestId, sessionId);
    if (PushTask((HcTaskBase *)task) != HC_SUCCESS) {
        DestroySession(requestId);
        HcFree(task);
        return HC_ERR_INIT_TASK_FAIL;
    }
    LOGD("[End]: OnChannelOpened!");
    return HC_SUCCESS;
}

static void OnChannelClosedCb(int sessionId)
{
    LOGI("[SoftBus][Out]: OnChannelClosed! sessionId: %d", sessionId);
    if (IsServer(sessionId)) {
        return;
    }
    RemoveChannelEntry(sessionId);
}

static void OnBytesReceivedCb(int sessionId, const void *data, unsigned int dataLen)
{
    if ((data == NULL) || (dataLen == 0) || (dataLen > MAX_DATA_BUFFER_SIZE)) {
        LOGE("Invalid input params!");
        return;
    }
    LOGD("[Start]: OnMsgReceived! [ChannelId]: %d", sessionId);
    int64_t requestId = DEFAULT_REQUEST_ID;
    char *recvDataStr = GenRecvData(sessionId, data, dataLen, &requestId);
    if (recvDataStr == NULL) {
        return;
    }
    (void)GetGmInstance()->processData(requestId, (uint8_t *)recvDataStr, HcStrlen(recvDataStr) + 1);
    FreeJsonString(recvDataStr);
}

static int32_t OpenSoftBusChannel(const char *connectParams, int64_t requestId, int64_t *returnChannelId)
{
    if ((connectParams == NULL) || (returnChannelId == NULL)) {
        LOGE("The input connectParams or returnChannelId is NULL!");
        return HC_ERR_NULL_PTR;
    }
    LOGD("[SoftBus][In]: OpenChannel!");
    int64_t channelId = (int64_t)OpenAuthSession(GROUP_MANAGER_PACKAGE_NAME, NULL, 0, connectParams);
    LOGD("[SoftBus][Out]: OpenChannel! channelId: %" PRId64, channelId);
    /* If the value of channelId is less than 0, the soft bus fails to open the channel */
    if (channelId < 0) {
        LOGE("Failed to open soft bus channel!");
        return HC_ERR_SOFT_BUS;
    }
    int32_t res = AddChannelEntry(requestId, channelId);
    if (res != HC_SUCCESS) {
        return res;
    }
    *returnChannelId = channelId;
    return HC_SUCCESS;
}

static void CloseSoftBusChannel(int64_t channelId)
{
    if (IsServer(channelId)) {
        return;
    }
    RemoveChannelEntry(channelId);
    LOGD("[SoftBus][In]: CloseSession!");
    CloseSession(channelId);
    LOGD("[SoftBus][Out]: CloseSession!");
}

static int32_t SendSoftBusMsg(int64_t channelId, const uint8_t *data, uint32_t dataLen)
{
    LOGD("[SoftBus][In]: SendMsg!");
    int32_t res = SendBytes(channelId, data, dataLen);
    LOGD("[SoftBus][Out]: SendMsg! res: %d", res);
    if (res != HC_SUCCESS) {
        LOGE("An error occurs when the softbus sends data!");
        return HC_ERR_SOFT_BUS;
    }
    return HC_SUCCESS;
}

static void NotifySoftBusBindResult(int64_t channelId)
{
    LOGD("[SoftBus][In]: NotifyAuthSuccess!");
    NotifyAuthSuccess(channelId);
    LOGD("[SoftBus][Out]: NotifyAuthSuccess!");
}

SoftBus g_softBus = {
    .openChannel = OpenSoftBusChannel,
    .closeChannel = CloseSoftBusChannel,
    .sendMsg = SendSoftBusMsg,
    .notifyResult = NotifySoftBusBindResult
};

int32_t InitSoftBusChannelModule(void)
{
    if (g_channelMutex == NULL) {
        g_channelMutex = (HcMutex *)HcMalloc(sizeof(HcMutex), 0);
        if (g_channelMutex == NULL) {
            LOGE("Failed to allocate channel mutex memory!");
            return HC_ERR_ALLOC_MEMORY;
        }
        if (InitHcMutex(g_channelMutex) != HC_SUCCESS) {
            LOGE("Init mutex failed!");
            HcFree(g_channelMutex);
            g_channelMutex = NULL;
            return HC_ERR_INIT_FAILED;
        }
    }
    g_channelVec = CREATE_HC_VECTOR(ChannelEntryVec);
    ISessionListener softBusListener = {
        .OnSessionOpened = OnChannelOpenedCb,
        .OnSessionClosed = OnChannelClosedCb,
        .OnBytesReceived = OnBytesReceivedCb,
        .OnMessageReceived = NULL
    };
    LOGD("[SoftBus][In]: CreateSessionServer!");
    int32_t res = CreateSessionServer(GROUP_MANAGER_PACKAGE_NAME, GROUP_MANAGER_PACKAGE_NAME, &softBusListener);
    LOGD("[SoftBus][Out]: CreateSessionServer! res: %d", res);
    return res;
}

void DestroySoftBusChannelModule(void)
{
    g_channelMutex->lock(g_channelMutex);
    DESTROY_HC_VECTOR(ChannelEntryVec, &g_channelVec);
    g_channelMutex->unlock(g_channelMutex);
    if (g_channelMutex != NULL) {
        DestroyHcMutex(g_channelMutex);
        HcFree(g_channelMutex);
        g_channelMutex = NULL;
    }
    LOGD("[SoftBus][In]: RemoveSessionServer!");
    int32_t res = RemoveSessionServer(GROUP_MANAGER_PACKAGE_NAME, GROUP_MANAGER_PACKAGE_NAME);
    LOGD("[SoftBus][Out]: RemoveSessionServer! res: %d", res);
}

SoftBus *GetSoftBusInstance(void)
{
    return &g_softBus;
}

bool IsSoftBusChannelSupported(void)
{
    return true;
}