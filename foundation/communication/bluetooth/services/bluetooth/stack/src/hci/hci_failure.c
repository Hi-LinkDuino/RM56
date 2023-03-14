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

#include "hci_failure.h"

#include "btstack.h"
#include "hci.h"
#include "list.h"
#include "mutex.h"

static List *g_hciFailureCallbackList = NULL;
static Mutex *g_hciFailureCallbackListLock = NULL;

void HciInitFailure()
{
    g_hciFailureCallbackList = ListCreate(NULL);
    g_hciFailureCallbackListLock = MutexCreate();
}

void HciCloseFailure()
{
    if (g_hciFailureCallbackList != NULL) {
        ListDelete(g_hciFailureCallbackList);
        g_hciFailureCallbackList = NULL;
    }
    if (g_hciFailureCallbackListLock != NULL) {
        MutexDelete(g_hciFailureCallbackListLock);
        g_hciFailureCallbackListLock = NULL;
    }
}

int HCI_RegisterFailureCallback(const HciFailureCallbacks *callbacks)
{
    MutexLock(g_hciFailureCallbackListLock);

    ListAddLast(g_hciFailureCallbackList, (void *)callbacks);

    MutexUnlock(g_hciFailureCallbackListLock);
    return BT_NO_ERROR;
}

int HCI_DeregisterFailureCallback(const HciFailureCallbacks *callbacks)
{
    MutexLock(g_hciFailureCallbackListLock);

    ListRemoveNode(g_hciFailureCallbackList, (void *)callbacks);

    MutexUnlock(g_hciFailureCallbackListLock);

    return BT_NO_ERROR;
}

void HciOnCmdTimeout()
{
    HciFailureCallbacks *callbacks = NULL;
    MutexLock(g_hciFailureCallbackListLock);

    ListNode *node = ListGetFirstNode(g_hciFailureCallbackList);
    while (node != NULL) {
        callbacks = ListGetNodeData(node);
        if (callbacks != NULL) {
            if (callbacks->onCmdTimeout != NULL) {
                callbacks->onCmdTimeout();
            }
        }
        node = ListGetNextNode(node);
    }

    MutexUnlock(g_hciFailureCallbackListLock);
}