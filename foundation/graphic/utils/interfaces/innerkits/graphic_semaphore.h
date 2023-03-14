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

#ifndef GRAPHIC_LITE_GRAPHIC_SEMAPHORE_H
#define GRAPHIC_LITE_GRAPHIC_SEMAPHORE_H
#include "stdbool.h"
#include "stdint.h"
#ifdef _WIN32
#include <windows.h>
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
#include <limits.h>
#include <semaphore.h>
#else
#include "los_sem.h"
#endif // WIN32
#include "gfx_utils/heap_base.h"

namespace OHOS {
static constexpr int32_t MAX_SEM_COUNT = 1000; // 1000: max number of semaphore count

/** @brief Semaphore adapter for different platform. */
class GraphicSemaphore : public HeapBase {
public:
    /** Default constructor */
    GraphicSemaphore() : GraphicSemaphore(0, MAX_SEM_COUNT) {}

    GraphicSemaphore(int32_t init) : GraphicSemaphore(init, MAX_SEM_COUNT) {}

    GraphicSemaphore(int32_t init, int32_t max)
    {
        if (max > MAX_SEM_COUNT) {
            max = MAX_SEM_COUNT;
        }
        if (init > max) {
            init = max;
        }
#ifdef _WIN32
        sem_ = CreateSemaphore(NULL, init, max, NULL);
        initFlag_ = (sem_ != nullptr);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        initFlag_ = (sem_init(&sem_, 0, init) == 0);
#else
        if (max == 1) {
            initFlag_ = (LOS_BinarySemCreate((uint16_t)init, &sem_) == LOS_OK);
        } else {
            initFlag_ = (LOS_SemCreate((uint16_t)init, &sem_) == LOS_OK);
        }
#endif // WIN32
    }

    /** Default destructor */
    ~GraphicSemaphore()
    {
        if (!initFlag_) {
            return;
        }
#ifdef _WIN32
        CloseHandle(sem_);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        sem_destroy(&sem_);
#else
        LOS_SemDelete(sem_);
#endif // WIN32
    }

    /** Increases the count of the specified semaphore object by a specified amount. */
    inline bool Notify()
    {
        if (!initFlag_) {
            return false;
        }
#ifdef _WIN32
        return ReleaseSemaphore(sem_, 1, NULL);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        return (sem_post(&sem_) == 0);
#else
        return (LOS_SemPost(sem_) == LOS_OK);
#endif // WIN32
    }

    /** Waits until the specified object is in the signaled state. */
    inline bool Wait()
    {
        if (!initFlag_) {
            return false;
        }
#ifdef _WIN32
        return (WaitForSingleObject(sem_, INFINITE) == WAIT_OBJECT_0);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        return (sem_wait(&sem_) == 0);
#else
        return (LOS_SemPend(sem_, LOS_WAIT_FOREVER) == LOS_OK);
#endif // WIN32
    }

private:
    bool initFlag_;
#ifdef _WIN32
    HANDLE sem_;
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
    sem_t sem_;
#else
    uint32_t sem_;
#endif // WIN32
};
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_SEMAPHORE_H
