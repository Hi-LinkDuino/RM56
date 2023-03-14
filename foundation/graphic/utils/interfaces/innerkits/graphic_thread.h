/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_THREAD_H
#define GRAPHIC_LITE_THREAD_H

#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
#include <pthread.h>
#endif // WIN32

typedef void* ThreadId;
#ifdef _WIN32
typedef DWORD(WINAPI* Runnable)(LPVOID lpThreadParameter);
#else
typedef void* (*Runnable)(void* argv);
#endif
typedef struct ThreadAttr ThreadAttr;
struct ThreadAttr {
    const char* name;   // name of the thread
    uint32_t stackSize; // size of stack
    uint8_t priority;   // initial thread priority
    uint8_t reserved1;  // reserved1 (must be 0)
    uint16_t reserved2; // reserved2 (must be 0)
};

static inline ThreadId ThreadCreate(Runnable entry, void* arg, const ThreadAttr* attr)
{
#ifdef _WIN32
    HANDLE handle = CreateThread(NULL, 0, entry, arg, 0, NULL);
    return (ThreadId)handle;
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    if (attr != NULL) {
        pthread_attr_setstacksize(&threadAttr, attr->stackSize);
        struct sched_param sched = {attr->priority};
        pthread_attr_setschedparam(&threadAttr, &sched);
    }
    pthread_t threadId;
    int ret = pthread_create(&threadId, &threadAttr, entry, arg);
    if (ret != 0) {
        return NULL;
    }
    return (ThreadId)threadId;
#else
    return NULL;
#endif
}
static inline void ThreadYield(void)
{
#ifdef _WIN32
    SwitchToThread();
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
    sched_yield();
#endif
}
#endif // GRAPHIC_LITE_THREAD_H
