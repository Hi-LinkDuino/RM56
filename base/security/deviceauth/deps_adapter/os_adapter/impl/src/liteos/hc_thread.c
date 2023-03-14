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

#include "hc_thread.h"

#define MAX_THREAD_STACK_SIZE (8 * 1024 * 1024)

void *StaticThreadFunc(void *args)
{
    HcThread *thread = (HcThread*)args;
    if (thread == NULL) {
        return NULL;
    }

    (void)pthread_setname_np(pthread_self(), StringGet(&thread->name));

    if (thread->threadFunc) {
        thread->threadFunc(args);
    }
    thread->threadLock.lock(&thread->threadLock);
    thread->running = HC_FALSE;
    thread->threadWaitObj.notifyWithoutLock(&thread->threadWaitObj);
    thread->threadLock.unlock(&thread->threadLock);
    return NULL;
}

int Start(struct HcThreadT *thread)
{
    if (thread == NULL) {
        return -1;
    }
    thread->threadLock.lock(&thread->threadLock);
    if (thread->running) {
        thread->threadLock.unlock(&thread->threadLock);
        return 0;
    }
    thread->running = HC_TRUE;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if (thread->stackSize > 0 && thread->stackSize <= MAX_THREAD_STACK_SIZE) {
        pthread_attr_setstacksize(&attr, thread->stackSize);
    }

    int result = pthread_create(&thread->thread, &attr, StaticThreadFunc, thread);
    pthread_attr_destroy(&attr);
    if (result != 0) {
        thread->running = HC_FALSE;
    }
    thread->threadLock.unlock(&thread->threadLock);
    return result;
}

void Join(struct HcThreadT *thread)
{
    if (thread == NULL) {
        return;
    }

    if (thread->running) {
        thread->threadWaitObj.waitWithoutLock(&thread->threadWaitObj);
    }
    void *status = NULL;
    pthread_join(thread->thread, &status);
}

void BizWait(struct HcThreadT *thread)
{
    if (thread == NULL) {
        return;
    }
    thread->bizWaitObj.wait(&thread->bizWaitObj);
}

void BizNotify(struct HcThreadT *thread)
{
    if (thread == NULL) {
        return;
    }
    thread->bizWaitObj.notify(&thread->bizWaitObj);
}

int32_t InitThread(HcThread *thread, ThreadFunc func, size_t stackSize, const char *threadName)
{
    if (thread == NULL) {
        return -1;
    }

    thread->threadFunc = func;
    thread->start = Start;
    thread->wait = BizWait;
    thread->notify = BizNotify;
    thread->join = Join;
    thread->stackSize = stackSize;
    thread->running = HC_FALSE;
    thread->name = CreateString();
    if (StringSetPointer(&thread->name, threadName) != HC_TRUE) {
        return -1;
    }

    int32_t res = InitHcMutex(&thread->threadLock);
    if (res != 0) {
        DeleteString(&thread->name);
        return res;
    }
    res = InitHcCond(&thread->threadWaitObj, &thread->threadLock);
    if (res != 0) {
        DeleteString(&thread->name);
        DestroyHcMutex(&thread->threadLock);
        return res;
    }
    res = InitHcCond(&thread->bizWaitObj, NULL);
    if (res != 0) {
        DeleteString(&thread->name);
        DestroyHcMutex(&thread->threadLock);
        DestroyHcCond(&thread->threadWaitObj);
    }
    return res;
}

void DestroyThread(HcThread *thread)
{
    if (thread == NULL) {
        return;
    }

    DestroyHcCond(&thread->bizWaitObj);
    DestroyHcCond(&thread->threadWaitObj);
    DestroyHcMutex(&thread->threadLock);
    DeleteString(&thread->name);
}
