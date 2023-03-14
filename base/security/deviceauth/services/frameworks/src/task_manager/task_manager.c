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

#include "task_manager.h"

#include "device_auth_defines.h"
#include "hc_log.h"

#ifndef HICHAIN_THREAD_STACK_SIZE
#define HICHAIN_THREAD_STACK_SIZE 4096
#endif

static HcTaskThread *g_taskThread = NULL;

int32_t PushTask(HcTaskBase *baseTask)
{
    if (g_taskThread == NULL) {
        LOGE("Task thread is NULL!");
        return HC_ERR_NULL_PTR;
    }
    g_taskThread->pushTask(g_taskThread, baseTask);
    return HC_SUCCESS;
}

int32_t InitTaskManager(void)
{
    if (g_taskThread != NULL) {
        LOGD("Task thread is running!");
        return HC_SUCCESS;
    }
    g_taskThread = (HcTaskThread *)HcMalloc(sizeof(HcTaskThread), 0);
    if (g_taskThread == NULL) {
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t res = InitHcTaskThread(g_taskThread, HICHAIN_THREAD_STACK_SIZE, "HichainThread");
    if (res != HC_SUCCESS) {
        LOGE("Failed to init task thread! res: %d", res);
        HcFree(g_taskThread);
        g_taskThread = NULL;
        return HC_ERR_INIT_FAILED;
    }
    res = g_taskThread->startThread(g_taskThread);
    if (res != HC_SUCCESS) {
        DestroyHcTaskThread(g_taskThread);
        HcFree(g_taskThread);
        g_taskThread = NULL;
        LOGE("Failed to start thread! res: %d", res);
        return HC_ERR_INIT_FAILED;
    }
    return HC_SUCCESS;
}

void DestroyTaskManager(void)
{
    if (g_taskThread != NULL) {
        g_taskThread->stopAndClear(g_taskThread);
        DestroyHcTaskThread(g_taskThread);
        HcFree(g_taskThread);
        g_taskThread = NULL;
    }
}
