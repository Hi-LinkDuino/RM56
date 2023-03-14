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

#include "btm_pm.h"

#include "btstack.h"
#include "hci/hci.h"
#include "hci/hci_error.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "btm.h"
#include "btm_acl.h"

#define FOREACH_CALLBACKS_START(x)                        \
    MutexLock(g_pmCallbackListLock);                      \
    ListNode *node_ = ListGetFirstNode(g_pmCallbackList); \
    while (node_ != NULL) {                               \
        x = ListGetNodeData(node_);                       \
        if ((x) != NULL) {

#define FOREACH_CALLBACKS_END       \
    }                               \
    node_ = ListGetNextNode(node_); \
    }                               \
    MutexUnlock(g_pmCallbackListLock);

#define STATUS_NONE 0
#define STATUS_INITIALIZED 1

#define IS_INITIALIZED() (g_status == STATUS_INITIALIZED)

typedef struct {
    const BtmPmCallbacks *callbacks;
    void *context;
} BtmPmCallbackBlock;

typedef struct {
    uint16_t connectionHandle;
    uint8_t currentMode;
} BtmConnectionMode;

static List *g_pmCallbackList = NULL;
static Mutex *g_pmCallbackListLock = NULL;
static HciEventCallbacks g_hciEventCallbacks;

static List *g_connectionModeList = NULL;
static Mutex *g_connectionModeListLock = NULL;

static uint8_t g_status = STATUS_NONE;

static BtmPmCallbackBlock *BtmPmAllocCallbackBlock(const BtmPmCallbacks *callbacks, void *context)
{
    BtmPmCallbackBlock *block = MEM_MALLOC.alloc(sizeof(BtmPmCallbackBlock));
    if (block != NULL) {
        block->callbacks = callbacks;
        block->context = context;
    }
    return block;
}

static void BtmPmFreeCallbackBlock(void *block)
{
    MEM_MALLOC.free(block);
}

static BtmConnectionMode *BtmAllocConnectionMode(uint16_t connectionHandle, uint8_t currentMode)
{
    BtmConnectionMode *connectionMode = MEM_MALLOC.alloc(sizeof(BtmConnectionMode));
    if (connectionMode != NULL) {
        connectionMode->connectionHandle = connectionHandle;
        connectionMode->currentMode = currentMode;
    }
    return connectionMode;
}

static void BtmFreeConnectionMode(void *connectionMode)
{
    MEM_MALLOC.free(connectionMode);
}

static BtmConnectionMode *BtmFindConnectionModeByConnectionHandle(uint16_t connectionHandle)
{
    BtmConnectionMode *connectionMode = NULL;
    ListNode *node = ListGetFirstNode(g_connectionModeList);

    while (node != NULL) {
        connectionMode = ListGetNodeData(node);
        if (connectionMode->connectionHandle == connectionHandle) {
            break;
        } else {
            connectionMode = NULL;
        }
        node = ListGetNextNode(node);
    }

    return connectionMode;
}

void BtmInitPm()
{
    g_pmCallbackList = ListCreate(BtmPmFreeCallbackBlock);
    g_pmCallbackListLock = MutexCreate();

    g_connectionModeList = ListCreate(BtmFreeConnectionMode);
    g_connectionModeListLock = MutexCreate();

    g_status = STATUS_INITIALIZED;
}

void BtmClosePm()
{
    g_status = STATUS_NONE;

    if (g_connectionModeListLock != NULL) {
        MutexDelete(g_connectionModeListLock);
        g_connectionModeListLock = NULL;
    }
    if (g_connectionModeList != NULL) {
        ListDelete(g_connectionModeList);
        g_connectionModeList = NULL;
    }
    if (g_pmCallbackListLock != NULL) {
        MutexDelete(g_pmCallbackListLock);
        g_pmCallbackListLock = NULL;
    }
    if (g_pmCallbackList != NULL) {
        ListDelete(g_pmCallbackList);
        g_pmCallbackList = NULL;
    }
}

void BtmStartPm()
{
    HCI_RegisterEventCallbacks(&g_hciEventCallbacks);
}

void BtmStopPm()
{
    HCI_DeregisterEventCallbacks(&g_hciEventCallbacks);

    MutexLock(g_connectionModeListLock);
    ListClear(g_connectionModeList);
    MutexUnlock(g_connectionModeListLock);

    MutexLock(g_pmCallbackListLock);
    ListClear(g_pmCallbackList);
    MutexUnlock(g_pmCallbackListLock);
}

static uint8_t BtmGetConnectionModeByConnectionHandle(uint16_t connectionHandle)
{
    uint8_t currentMode = BTM_PM_ACTIVE_MODE;

    MutexLock(g_connectionModeListLock);

    BtmConnectionMode *connectionMode = BtmFindConnectionModeByConnectionHandle(connectionHandle);
    if (connectionMode != NULL) {
        currentMode = connectionMode->currentMode;
    }

    MutexUnlock(g_connectionModeListLock);

    return currentMode;
}

int BTM_EnterSniffMode(const BtAddr *addr, const BtmSniffParam *param)
{
    if (addr == NULL || param == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    uint16_t connectionHandle = 0;
    int result = BtmGetAclHandleByAddress(addr, &connectionHandle);
    if (result != BT_NO_ERROR) {
        return BT_BAD_PARAM;
    }

    uint8_t currentMode = BtmGetConnectionModeByConnectionHandle(connectionHandle);
    if (currentMode != BTM_PM_ACTIVE_MODE) {
        return BT_BAD_STATUS;
    }

    HciSniffModeParam sniffModeParam = {
        .connectionHandle = connectionHandle,
        .sniffMaxInterval = param->maxInterval,
        .sniffMinInterval = param->minInterval,
        .sniffAttempt = param->attempt,
        .sniffTimeout = param->timeout,
    };

    return HCI_SniffMode(&sniffModeParam);
}

int BTM_ExitSniffMode(const BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    uint16_t connectionHandle = 0;
    int result = BtmGetAclHandleByAddress(addr, &connectionHandle);
    if (result != BT_NO_ERROR) {
        return BT_BAD_PARAM;
    }

    uint8_t currentMode = BtmGetConnectionModeByConnectionHandle(connectionHandle);
    if (currentMode != BTM_PM_SNIFF_MODE) {
        return BT_BAD_STATUS;
    }

    HciExitSniffModeParam param = {
        .connectionHandle = connectionHandle,
    };

    return HCI_ExitSniffMode(&param);
}

int BTM_SetSniffSubrating(const BtAddr *addr, const BtmSniffSubrating *subRating)
{
    if ((addr == NULL) || (subRating == NULL)) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    uint16_t connectionHandle = 0;
    int result = BtmGetAclHandleByAddress(addr, &connectionHandle);
    if (result != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    HciSniffSubratingParam param = {
        .connectionHandle = connectionHandle,
        .maximumLatency = subRating->maximumLatency,
        .minimumRemoteTimeout = subRating->minimumRemoteTimeout,
        .minimumLocalTimeout = subRating->minimumLocalTimeout,
    };

    return HCI_SniffSubrating(&param);
}

static void BtmUpdateConnectionMode(uint16_t connectionHandle, uint8_t currentMode)
{
    MutexLock(g_connectionModeListLock);

    BtmConnectionMode *connectionMode = BtmFindConnectionModeByConnectionHandle(connectionHandle);
    if (connectionMode != NULL) {
        connectionMode->currentMode = currentMode;
    } else {
        connectionMode = BtmAllocConnectionMode(connectionHandle, currentMode);
        ListAddLast(g_connectionModeList, connectionMode);
    }

    MutexUnlock(g_connectionModeListLock);
}

NO_SANITIZE("cfi") static void BtmPmOnModeChange(const HciModeChangeEventParam *param)
{
    BtAddr addr;
    int result = BtmGetAclAddressByHandle(param->connectionHandle, &addr);
    if (result != BT_NO_ERROR) {
        return;
    }

    if (param->status == HCI_SUCCESS) {
        BtmUpdateConnectionMode(param->connectionHandle, param->currentMode);
    }

    BtmPmCallbackBlock *block = NULL;
    FOREACH_CALLBACKS_START(block);
    if (block->callbacks->modeChange != NULL) {
        block->callbacks->modeChange(param->status, &addr, param->currentMode, param->interval, block->context);
    }
    FOREACH_CALLBACKS_END;
}

NO_SANITIZE("cfi") static void BtmPmOnSniffSubratingComplete(const HciSniffSubratingReturnParam *param)
{
    BtAddr addr;
    int result = BtmGetAclAddressByHandle(param->connectionHandle, &addr);
    if (result != BT_NO_ERROR) {
        return;
    }

    BtmPmCallbackBlock *block = NULL;
    FOREACH_CALLBACKS_START(block);
    if (block->callbacks->setSniffSubratingComplete != NULL) {
        block->callbacks->setSniffSubratingComplete(param->status, &addr, block->context);
    }
    FOREACH_CALLBACKS_END;
}

void BtmPmOnDisconnectComplete(const HciDisconnectCompleteEventParam *eventParam)
{
    if (eventParam->status != HCI_SUCCESS) {
        return;
    }

    MutexLock(g_connectionModeListLock);

    BtmConnectionMode *connectionMode = BtmFindConnectionModeByConnectionHandle(eventParam->connectionHandle);
    if (connectionMode != NULL) {
        ListRemoveNode(g_connectionModeList, connectionMode);
    }

    MutexUnlock(g_connectionModeListLock);
}

int BTM_RegisterPmCallbacks(const BtmPmCallbacks *callbacks, void *context)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    BtmPmCallbackBlock *block = BtmPmAllocCallbackBlock(callbacks, context);
    if (block == NULL) {
        return BT_NO_MEMORY;
    }

    MutexLock(g_pmCallbackListLock);
    ListAddLast(g_pmCallbackList, block);
    MutexUnlock(g_pmCallbackListLock);

    return BT_NO_ERROR;
}

int BTM_DeregisterPmCallbacks(const BtmPmCallbacks *callbacks)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    BtmPmCallbackBlock *block = NULL;
    FOREACH_CALLBACKS_START(block);
    if (block->callbacks == callbacks) {
        ListRemoveNode(g_pmCallbackList, block);
        break;
    }
    FOREACH_CALLBACKS_END;
    return BT_NO_ERROR;
}

static HciEventCallbacks g_hciEventCallbacks = {
    .disconnectComplete = BtmPmOnDisconnectComplete,
    .modeChange = BtmPmOnModeChange,
    .sniffSubratingComplete = BtmPmOnSniffSubratingComplete,
};
