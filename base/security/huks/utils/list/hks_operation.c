/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hks_operation.h"

#include <pthread.h>
#include <stdio.h>

#include "hks_log.h"
#include "hks_mem.h"

#define MAX_OPERATIONS 15

static struct DoubleList g_operationList;
static uint32_t g_operationCount = 0;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

/* Need to lock before calling FreeOperation */
static void FreeOperation(struct HksOperation **operation)
{
    if (operation == NULL || *operation == NULL) {
        return;
    }
    RemoveDoubleListNode(&(*operation)->listHead);
    HKS_FREE_BLOB((*operation)->processName);
    HKS_FREE_PTR(*operation);
}

static int32_t AddOperation(struct HksOperation *operation)
{
    pthread_mutex_lock(&g_lock);
    if (g_operationCount >= MAX_OPERATIONS) {
        if (!DeleteFirstAbortableOperation()) {
            pthread_mutex_unlock(&g_lock);
            return HKS_FAILURE;
        }
    }

    AddNodeAtDoubleListTail(&g_operationList, &operation->listHead);
    ++g_operationCount;
    pthread_mutex_unlock(&g_lock);
    return HKS_SUCCESS;
}

int32_t CreateOperation(const struct HksBlob *processName, uint64_t operationHandle, bool abortable)
{
    struct HksOperation *operation = (struct HksOperation *)HksMalloc(sizeof(struct HksOperation));
    if (operation == NULL) {
        HKS_LOG_E("malloc hks operation failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    operation->processName.data = (uint8_t *)HksMalloc(processName->size);
    if (operation->processName.data == NULL) {
        HKS_LOG_E("malloc operation processName failed");
        HKS_FREE_PTR(operation);
        return HKS_ERROR_MALLOC_FAIL;
    }

    operation->processName.size = processName->size;
    operation->abortable = abortable;
    operation->handle = operationHandle;

    if (memcpy_s(operation->processName.data, operation->processName.size,
        processName->data, processName->size) != EOK) {
        HKS_LOG_E("memcpy_s operation processName failed");
        HKS_FREE_BLOB(operation->processName);
        HKS_FREE_PTR(operation);
        return HKS_ERROR_BAD_STATE;
    }

    if (AddOperation(operation) != HKS_SUCCESS) {
        HKS_FREE_BLOB(operation->processName);
        HKS_FREE_PTR(operation);
        return HKS_FAILURE;
    }

    return HKS_SUCCESS;
}

struct HksOperation *QueryOperation(uint64_t operationHandle)
{
    struct HksOperation *operation = NULL;

    pthread_mutex_lock(&g_lock);
    HKS_DLIST_ITER(operation, &g_operationList) {
        if ((operation != NULL) && (operation->handle == operationHandle)) {
            pthread_mutex_unlock(&g_lock);
            return operation;
        }
    }
    pthread_mutex_unlock(&g_lock);

    return NULL;
}

void DeleteOperation(uint64_t operationHandle)
{
    struct HksOperation *operation = NULL;

    pthread_mutex_lock(&g_lock);
    HKS_DLIST_ITER(operation, &g_operationList) {
        if ((operation != NULL) && (operation->handle == operationHandle)) {
            FreeOperation(&operation);
            --g_operationCount;
            pthread_mutex_unlock(&g_lock);
            return;
        }
    }
    pthread_mutex_unlock(&g_lock);
}

void DeleteSession(const struct HksBlob *processName)
{
    struct HksOperation *operation = NULL;

    pthread_mutex_lock(&g_lock);
    HKS_DLIST_ITER(operation, &g_operationList) {
        if ((operation != NULL) && (processName->size == operation->processName.size) &&
            (memcmp(processName->data, operation->processName.data, processName->size) == 0)) {
            FreeOperation(&operation);
            --g_operationCount;
        }
    }
    pthread_mutex_unlock(&g_lock);
}

bool DeleteFirstAbortableOperation(void)
{
    struct HksOperation *operation = NULL;

    pthread_mutex_lock(&g_lock);
    HKS_DLIST_ITER(operation, &g_operationList) {
        if (operation != NULL && operation->abortable) {
            FreeOperation(&operation);
            --g_operationCount;
            pthread_mutex_unlock(&g_lock);
            return true;
        }
    }
    pthread_mutex_unlock(&g_lock);

    return false;
}

uint32_t GetAbortableOperationCount(void)
{
    uint32_t count = 0;
    struct HksOperation *operation = NULL;

    pthread_mutex_lock(&g_lock);
    HKS_DLIST_ITER(operation, &g_operationList) {
        if (operation != NULL && operation->abortable) {
            ++count;
        }
    }
    pthread_mutex_unlock(&g_lock);

    return count;
}
