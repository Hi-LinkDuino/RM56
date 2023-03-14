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

#ifndef GRAPHIC_LITE_GRAPHIC_LOCKER_H
#define GRAPHIC_LITE_GRAPHIC_LOCKER_H

#include "stdint.h"
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
#include <pthread.h>
#endif

namespace OHOS {
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
class GraphicLocker {
public:
    explicit GraphicLocker(pthread_mutex_t& mutex) : mutex_(mutex)
    {
        pthread_mutex_lock(&mutex_);
    }

    ~GraphicLocker()
    {
        pthread_mutex_unlock(&mutex_);
    }

    GraphicLocker() = delete;
    GraphicLocker(const GraphicLocker&) = delete;
    GraphicLocker& operator=(const GraphicLocker&) = delete;

private:
    pthread_mutex_t& mutex_;
};

class GraphicMutex {
public:
    GraphicMutex()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mutex_, &attr);
    }
    ~GraphicMutex()
    {
        pthread_mutex_destroy(&mutex_);
    }
    void Lock()
    {
        pthread_mutex_lock(&mutex_);
    }
    void Unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }
    GraphicMutex(const GraphicMutex&) = delete;
    GraphicMutex(const GraphicMutex&&) = delete;
    GraphicMutex& operator=(const GraphicMutex&) = delete;
    GraphicMutex& operator=(const GraphicMutex&&) = delete;

private:
    pthread_mutex_t mutex_;
};
#else
class GraphicMutex {
public:
    GraphicMutex() = default;
    ~GraphicMutex() = default;
    void Lock() {}
    void Unlock() {}
    GraphicMutex(const GraphicMutex&) = delete;
    GraphicMutex(const GraphicMutex&&) = delete;
    GraphicMutex& operator=(const GraphicMutex&) = delete;
    GraphicMutex& operator=(const GraphicMutex&&) = delete;
};
#endif

// do not support multi-thread
class GraphicLockGuard {
public:
    explicit GraphicLockGuard(GraphicMutex& mutex) : mutex_(mutex)
    {
        Lock();
    }
    ~GraphicLockGuard()
    {
        Unlock();
    }
    void Lock()
    {
        mutex_.Lock();
        lockCnt_++;
    }
    void Unlock()
    {
        if (lockCnt_ > 0) {
            mutex_.Unlock();
            lockCnt_--;
        }
    }

    GraphicLockGuard() = delete;
    GraphicLockGuard(const GraphicLockGuard&) = delete;
    GraphicLockGuard(const GraphicLockGuard&&) = delete;
    GraphicLockGuard& operator=(const GraphicLockGuard&) = delete;
    GraphicLockGuard& operator=(const GraphicLockGuard&&) = delete;

private:
    GraphicMutex& mutex_;
    int8_t lockCnt_ = 0;
};
} // namespace OHOS
#endif
