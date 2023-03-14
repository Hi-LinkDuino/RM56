/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "allocator.h"
#include "cmd/hci_cmd.h"
#include "log.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "hci_vendor_if.h"

typedef struct {
    const HCIVendorEventCallback *callback;
    void *context;
} HCIVendorEventCallbackBlock;

static List *g_hciVendorCallbackList = NULL;
static Mutex *g_hciVendorCallbackListLock = NULL;

static HCIVendorEventCallbackBlock *HCIVendorAllocCallbackBlock(const HCIVendorEventCallback *callback, void *context)
{
    HCIVendorEventCallbackBlock *block = MEM_MALLOC.alloc(sizeof(HCIVendorEventCallbackBlock));
    if (block != NULL) {
        block->callback = (HCIVendorEventCallback *)callback;
        block->context = context;
    }
    return block;
}

static void HCIVendorFreeCallbacksBlock(void *block)
{
    MEM_MALLOC.free(block);
}

NO_SANITIZE("cfi") void HciVendorClosed(void)
{
    if (g_hciVendorCallbackListLock == NULL) {
        LOG_ERROR("g_hciVendorCallbackListLock is null");
        return;
    }
    HCIVendorEventCallbackBlock *block = NULL;
    MutexLock(g_hciVendorCallbackListLock);
    ListNode *node = ListGetFirstNode(g_hciVendorCallbackList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block->callback != NULL && block->callback->hciVendorClosed != NULL) {
            block->callback->hciVendorClosed(block->context);
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_hciVendorCallbackListLock);
}

void HciVendorInit(void)
{
    g_hciVendorCallbackList = ListCreate(HCIVendorFreeCallbacksBlock);
    if (g_hciVendorCallbackList == NULL) {
        LOG_ERROR("%{public}s:malloc error.", __FUNCTION__);
    }

    g_hciVendorCallbackListLock = MutexCreate();
    if (g_hciVendorCallbackListLock == NULL) {
        LOG_ERROR("%{public}s:create mutex error.", __FUNCTION__);
    }
}

void HciVendorClose(void)
{
    HciVendorClosed();
    if (g_hciVendorCallbackListLock != NULL) {
        MutexDelete(g_hciVendorCallbackListLock);
        g_hciVendorCallbackListLock = NULL;
    }

    if (g_hciVendorCallbackList != NULL) {
        ListDelete(g_hciVendorCallbackList);
        g_hciVendorCallbackList = NULL;
    }
}

int HCIVIF_RegisterVendorEventCallback(
    const HCIVendorEventCallback *callback, void *context)
{
    if (callback == NULL) {
        LOG_ERROR("%{public}s: callback is null", __FUNCTION__);
        return BT_BAD_PARAM;
    }

    if ((g_hciVendorCallbackList == NULL) || ((g_hciVendorCallbackListLock == NULL))) {
        LOG_ERROR("%{public}s: hci vendor not ready", __FUNCTION__);
        return BT_BAD_STATUS;
    }

    HCIVendorEventCallbackBlock *block = HCIVendorAllocCallbackBlock(callback, context);
    if (block == NULL) {
        return BT_NO_MEMORY;
    }

    MutexLock(g_hciVendorCallbackListLock);
    ListAddLast(g_hciVendorCallbackList, block);
    MutexUnlock(g_hciVendorCallbackListLock);

    return BT_NO_ERROR;
}

int HCIVIF_DeregisterVendorEventCallback(const HCIVendorEventCallback *callback)
{
    if (callback == NULL) {
        return BT_BAD_PARAM;
    }

    if ((g_hciVendorCallbackList == NULL) || ((g_hciVendorCallbackListLock == NULL))) {
        LOG_ERROR("%{public}s: hci vendor not ready", __FUNCTION__);
        return BT_BAD_STATUS;
    }

    MutexLock(g_hciVendorCallbackListLock);

    HCIVendorEventCallbackBlock *block = NULL;
    ListNode *node = ListGetFirstNode(g_hciVendorCallbackList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block != NULL) {
            if (block->callback == callback) {
                ListRemoveNode(g_hciVendorCallbackList, block);
                break;
            }
        }
        node = ListGetNextNode(node);
    }

    MutexUnlock(g_hciVendorCallbackListLock);

    return BT_NO_ERROR;
}

int HCIVIF_SendCmd(uint16_t opCode, const void *param, size_t paramLength)
{
    HciCmd *cmd = HciAllocCmd(opCode, param, paramLength);
    return HciSendCmd(cmd);
}

NO_SANITIZE("cfi") void HciEventOnVendorCommandComplete(uint16_t opCode, const void *param, uint8_t paramLength)
{
    if (g_hciVendorCallbackListLock == NULL) {
        LOG_ERROR("g_hciVendorCallbackListLock is null");
        return;
    }
    HCIVendorEventCallbackBlock *block = NULL;
    MutexLock(g_hciVendorCallbackListLock);
    ListNode *node = ListGetFirstNode(g_hciVendorCallbackList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block->callback != NULL && block->callback->hciCommandCompleteEvent != NULL) {
            block->callback->hciCommandCompleteEvent(opCode, param, paramLength, block->context);
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_hciVendorCallbackListLock);
}
