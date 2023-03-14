/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "broadcast_manager.h"
#include "common_defs.h"
#include "device_auth_defines.h"
#include "group_operation_common.h"
#include "hc_log.h"
#include "hc_mutex.h"
#include "hc_types.h"
#include "hc_vector.h"
#include "securec.h"

typedef struct {
    char *appId;
    DataChangeListener *listener;
} ListenerEntry;

DECLARE_HC_VECTOR(ListenerEntryVec, ListenerEntry);
IMPLEMENT_HC_VECTOR(ListenerEntryVec, ListenerEntry, 1);
static ListenerEntryVec g_listenerEntryVec;
static HcMutex *g_broadcastMutex = NULL;

static int32_t GenerateMessage(const TrustedGroupEntry *groupEntry, char **returnGroupInfo)
{
    CJson *message = CreateJson();
    if (message == NULL) {
        LOGE("Failed to allocate message memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t result = GenerateReturnGroupInfo(groupEntry, message);
    if (result != HC_SUCCESS) {
        FreeJson(message);
        return result;
    }
    char *messageStr = PackJsonToString(message);
    FreeJson(message);
    if (messageStr == NULL) {
        LOGE("Failed to convert json to string!");
        return HC_ERR_JSON_FAIL;
    }
    *returnGroupInfo = messageStr;
    return HC_SUCCESS;
}

static void PostOnGroupCreated(const TrustedGroupEntry *groupEntry)
{
    if (groupEntry == NULL) {
        LOGE("The groupEntry is NULL!");
        return;
    }
    char *messageStr = NULL;
    if (GenerateMessage(groupEntry, &messageStr) != HC_SUCCESS) {
        return;
    }
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if ((entry != NULL) && (entry->listener != NULL) && (entry->listener->onGroupCreated != NULL)) {
            LOGI("[Broadcaster]: PostOnGroupCreated! [AppId]: %s", entry->appId);
            entry->listener->onGroupCreated(messageStr);
        }
    }
    FreeJsonString(messageStr);
    g_broadcastMutex->unlock(g_broadcastMutex);
}

static void PostOnGroupDeleted(const TrustedGroupEntry *groupEntry)
{
    if (groupEntry == NULL) {
        LOGE("The groupEntry is NULL!");
        return;
    }
    char *messageStr = NULL;
    if (GenerateMessage(groupEntry, &messageStr) != HC_SUCCESS) {
        return;
    }
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if ((entry != NULL) && (entry->listener != NULL) && (entry->listener->onGroupDeleted != NULL)) {
            LOGI("[Broadcaster]: PostOnGroupDeleted! [AppId]: %s", entry->appId);
            entry->listener->onGroupDeleted(messageStr);
        }
    }
    FreeJsonString(messageStr);
    g_broadcastMutex->unlock(g_broadcastMutex);
}

static void PostOnDeviceBound(const char *peerUdid, const TrustedGroupEntry *groupEntry)
{
    if ((peerUdid == NULL) || (groupEntry == NULL)) {
        LOGE("The peerUdid or groupEntry is NULL!");
        return;
    }
    char *messageStr = NULL;
    if (GenerateMessage(groupEntry, &messageStr) != HC_SUCCESS) {
        return;
    }
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if ((entry != NULL) && (entry->listener != NULL) && (entry->listener->onDeviceBound != NULL)) {
            LOGI("[Broadcaster]: PostOnDeviceBound! [AppId]: %s", entry->appId);
            entry->listener->onDeviceBound(peerUdid, messageStr);
        }
    }
    FreeJsonString(messageStr);
    g_broadcastMutex->unlock(g_broadcastMutex);
}

static void PostOnDeviceUnBound(const char *peerUdid, const TrustedGroupEntry *groupEntry)
{
    if ((peerUdid == NULL) || (groupEntry == NULL)) {
        LOGE("The peerUdid or groupEntry is NULL!");
        return;
    }
    char *messageStr = NULL;
    if (GenerateMessage(groupEntry, &messageStr) != HC_SUCCESS) {
        return;
    }
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if ((entry != NULL) && (entry->listener != NULL) && (entry->listener->onDeviceUnBound != NULL)) {
            LOGI("[Broadcaster]: PostOnDeviceUnBound! [AppId]: %s", entry->appId);
            entry->listener->onDeviceUnBound(peerUdid, messageStr);
        }
    }
    FreeJsonString(messageStr);
    g_broadcastMutex->unlock(g_broadcastMutex);
}

static void PostOnDeviceNotTrusted(const char *peerUdid)
{
    if (peerUdid == NULL) {
        LOGE("The peerUdid is NULL!");
        return;
    }
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if ((entry != NULL) && (entry->listener != NULL) && (entry->listener->onDeviceNotTrusted != NULL)) {
            LOGI("[Broadcaster]: PostOnDeviceNotTrusted! [AppId]: %s", entry->appId);
            entry->listener->onDeviceNotTrusted(peerUdid);
        }
    }
    g_broadcastMutex->unlock(g_broadcastMutex);
}

static void PostOnLastGroupDeleted(const char *peerUdid, int groupType)
{
    if (peerUdid == NULL) {
        LOGE("The peerUdid is NULL!");
        return;
    }
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if ((entry != NULL) && (entry->listener != NULL) && (entry->listener->onLastGroupDeleted != NULL)) {
            LOGI("[Broadcaster]: PostOnLastGroupDeleted! [AppId]: %s, [GroupType]: %d", entry->appId, groupType);
            entry->listener->onLastGroupDeleted(peerUdid, groupType);
        }
    }
    g_broadcastMutex->unlock(g_broadcastMutex);
}

static void PostOnTrustedDeviceNumChanged(int curTrustedDeviceNum)
{
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if ((entry != NULL) && (entry->listener != NULL) && (entry->listener->onTrustedDeviceNumChanged != NULL)) {
            LOGI("[Broadcaster]: PostOnTrustedDeviceNumChanged! [AppId]: %s", entry->appId);
            entry->listener->onTrustedDeviceNumChanged(curTrustedDeviceNum);
        }
    }
    g_broadcastMutex->unlock(g_broadcastMutex);
}

static int32_t UpdateListenerIfExist(const char *appId, const DataChangeListener *listener)
{
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if ((entry != NULL) && (strcmp(entry->appId, appId) == 0)) {
            if (memcpy_s(entry->listener, sizeof(DataChangeListener),
                listener, sizeof(DataChangeListener)) != HC_SUCCESS) {
                g_broadcastMutex->unlock(g_broadcastMutex);
                LOGE("Failed to copy listener!");
                return HC_ERR_MEMORY_COPY;
            }
            g_broadcastMutex->unlock(g_broadcastMutex);
            return HC_SUCCESS;
        }
    }
    g_broadcastMutex->unlock(g_broadcastMutex);
    return HC_ERR_LISTENER_NOT_EXIST;
}

static int32_t AddListenerIfNotExist(const char *appId, const DataChangeListener *listener)
{
    uint32_t appIdLen = HcStrlen(appId) + 1;
    char *copyAppId = (char *)HcMalloc(appIdLen, 0);
    if (copyAppId == NULL) {
        LOGE("Failed to allocate copyAppId memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (strcpy_s(copyAppId, appIdLen, appId) != HC_SUCCESS) {
        LOGE("Failed to copy appId!");
        HcFree(copyAppId);
        return HC_ERR_MEMORY_COPY;
    }
    DataChangeListener *copyListener = (DataChangeListener *)HcMalloc(sizeof(DataChangeListener), 0);
    if (copyListener == NULL) {
        LOGE("Failed to allocate saveCallback memory!");
        HcFree(copyAppId);
        return HC_ERR_ALLOC_MEMORY;
    }
    if (memcpy_s(copyListener, sizeof(DataChangeListener),
        listener, sizeof(DataChangeListener)) != HC_SUCCESS) {
        LOGE("Failed to copy listener!");
        HcFree(copyAppId);
        HcFree(copyListener);
        return HC_ERR_MEMORY_COPY;
    }
    ListenerEntry entry;
    entry.appId = copyAppId;
    entry.listener = copyListener;
    g_broadcastMutex->lock(g_broadcastMutex);
    g_listenerEntryVec.pushBack(&g_listenerEntryVec, &entry);
    g_broadcastMutex->unlock(g_broadcastMutex);
    LOGI("[End]: Service register listener successfully!");
    return HC_SUCCESS;
}

static Broadcaster g_broadcaster = {
    .postOnGroupCreated = PostOnGroupCreated,
    .postOnGroupDeleted = PostOnGroupDeleted,
    .postOnDeviceBound = PostOnDeviceBound,
    .postOnDeviceUnBound = PostOnDeviceUnBound,
    .postOnDeviceNotTrusted = PostOnDeviceNotTrusted,
    .postOnLastGroupDeleted = PostOnLastGroupDeleted,
    .postOnTrustedDeviceNumChanged = PostOnTrustedDeviceNumChanged
};

bool IsBroadcastSupported(void)
{
    return true;
}

int32_t InitBroadcastManager(void)
{
    if (g_broadcastMutex == NULL) {
        g_broadcastMutex = (HcMutex *)HcMalloc(sizeof(HcMutex), 0);
        if (g_broadcastMutex == NULL) {
            LOGE("Failed to allocate broadcast mutex memory!");
            return HC_ERR_ALLOC_MEMORY;
        }
        if (InitHcMutex(g_broadcastMutex) != HC_SUCCESS) {
            LOGE("Init mutex failed");
            HcFree(g_broadcastMutex);
            g_broadcastMutex = NULL;
            return HC_ERROR;
        }
    }
    g_listenerEntryVec = CREATE_HC_VECTOR(ListenerEntryVec);
    LOGI("[Broadcaster]: Init broadcast manager module successfully!");
    return HC_SUCCESS;
}

void DestroyBroadcastManager(void)
{
    uint32_t index;
    ListenerEntry *entry = NULL;
    g_broadcastMutex->lock(g_broadcastMutex);
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if (entry != NULL) {
            HcFree(entry->appId);
            HcFree(entry->listener);
        }
    }
    DESTROY_HC_VECTOR(ListenerEntryVec, &g_listenerEntryVec);
    g_broadcastMutex->unlock(g_broadcastMutex);
    if (g_broadcastMutex != NULL) {
        DestroyHcMutex(g_broadcastMutex);
        HcFree(g_broadcastMutex);
        g_broadcastMutex = NULL;
    }
}

Broadcaster *GetBroadcaster(void)
{
    return &g_broadcaster;
}

int32_t AddListener(const char *appId, const DataChangeListener *listener)
{
    if ((appId == NULL) || (listener == NULL)) {
        LOGE("The input appId or listener is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    if (UpdateListenerIfExist(appId, listener) == HC_SUCCESS) {
        return HC_SUCCESS;
    }
    return AddListenerIfNotExist(appId, listener);
}

int32_t RemoveListener(const char *appId)
{
    if (appId == NULL) {
        LOGE("The input appId is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    uint32_t index;
    ListenerEntry *entry = NULL;
    FOR_EACH_HC_VECTOR(g_listenerEntryVec, index, entry) {
        if (strcmp(entry->appId, appId) == 0) {
            HcFree(entry->appId);
            HcFree(entry->listener);
            ListenerEntry tempEntry;
            HC_VECTOR_POPELEMENT(&g_listenerEntryVec, &tempEntry, index);
            LOGI("[End]: Service deregister listener successfully!");
            return HC_SUCCESS;
        }
    }
    LOGI("[End]: The listener does not exist!");
    return HC_SUCCESS;
}
