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

#include "hc_task_thread.h"
#include "hal_error.h"
#include "hc_log.h"

#define TASK_ALLOC_UINT 5

IMPLEMENT_HC_VECTOR(TaskVec, HcTaskWrap, TASK_ALLOC_UINT)

static HcTaskBase* PopTask(HcTaskThread* thread)
{
    if (thread == NULL) {
        return NULL;
    }

    thread->queueLock.lock(&thread->queueLock);
    HcTaskWrap task;
    HcBool ret = thread->tasks.popFront(&thread->tasks, &task);
    thread->queueLock.unlock(&thread->queueLock);
    if (ret) {
        return task.task;
    }
    return NULL;
}

static void PushTask(struct HcTaskThreadT* thread, HcTaskBase* task)
{
    if (thread == NULL || task == NULL) {
        return;
    }

    thread->queueLock.lock(&thread->queueLock);
    HcTaskWrap taskWarp;
    taskWarp.task = task;
    thread->tasks.pushBack(&thread->tasks, &taskWarp);
    thread->thread.notify(&thread->thread);
    thread->queueLock.unlock(&thread->queueLock);
}

static void Clear(struct HcTaskThreadT* thread)
{
    thread->queueLock.lock(&thread->queueLock);
    HcTaskWrap *taskWarp = NULL;
    uint32_t index;
    FOR_EACH_HC_VECTOR(thread->tasks, index, taskWarp) {
        if (taskWarp->task->destroy) {
            taskWarp->task->destroy(taskWarp->task);
        }
        HcFree(taskWarp->task);
    }
    thread->tasks.clear(&thread->tasks);
    thread->queueLock.unlock(&thread->queueLock);
}

static void StopAndClear(struct HcTaskThreadT* thread)
{
    if (thread == NULL) {
        return;
    }
    thread->clear(thread);
    thread->quit = HC_TRUE;
    thread->thread.notify(&thread->thread);
    thread->thread.join(&thread->thread);
}

static int32_t StartTaskThread(struct HcTaskThreadT* thread)
{
    if (thread == NULL) {
        return HAL_ERR_BAD_ALLOC;
    }

    thread->quit = HC_FALSE;
    int32_t res = thread->thread.start(&thread->thread);
    if (res != HAL_SUCCESS) {
        LOGE("Start thread failed, res:%d", res);
    }
    return res;
}

static int TaskThreadLoop(void* args)
{
    HcTaskThread* thread = (HcTaskThread*)args;
    if (thread == NULL) {
        return -1;
    }

    while (1) {
        if (thread->quit) {
            break;
        }
        HcTaskBase* task = PopTask(thread);
        if (task != NULL) {
            if (task->doAction) {
                task->doAction(task);
            }
            if (task->destroy) {
                task->destroy(task);
            }
            HcFree(task);
        } else {
            thread->thread.wait(&thread->thread);
        }
    }
    return 0;
}

int32_t InitHcTaskThread(HcTaskThread* thread, size_t stackSize, const char* threadName)
{
    if (thread == NULL) {
        return -1;
    }

    thread->pushTask = PushTask;
    thread->startThread = StartTaskThread;
    thread->clear = Clear;
    thread->stopAndClear = StopAndClear;
    int32_t res = InitThread(&thread->thread, TaskThreadLoop, stackSize, threadName);
    if (res != 0) {
        return res;
    }
    res = InitHcMutex(&thread->queueLock);
    if (res != 0) {
        DestroyThread(&thread->thread);
        return res;
    }
    thread->tasks = CREATE_HC_VECTOR(TaskVec);
    return 0;
}

void DestroyHcTaskThread(HcTaskThread* thread)
{
    DESTROY_HC_VECTOR(TaskVec, &thread->tasks);
    DestroyHcMutex(&thread->queueLock);
    DestroyThread(&thread->thread);
}