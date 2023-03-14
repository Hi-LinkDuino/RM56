/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef UTILS_THREAD_EX_H
#define UTILS_THREAD_EX_H

#include <pthread.h>
#include <string>
#include <mutex>
#include <condition_variable>

namespace OHOS {

enum class ThreadStatus {
    OK,
    WOULD_BLOCK,
    INVALID_OPERATION,
    UNKNOWN_ERROR,
};

enum ThreadPrio {
    THREAD_PROI_NORMAL = 0,
    THREAD_PROI_LOW = 10,
    THREAD_PROI_LOWEST = 19,
};

constexpr int INVALID_PTHREAD_T = -1;
constexpr int MAX_THREAD_NAME_LEN = 15;

class Thread {
public:
    Thread();
    virtual ~Thread();

    ThreadStatus Start(const std::string& name, int32_t priority = THREAD_PROI_NORMAL, size_t stack = 0);

    ThreadStatus NotifyExitSync();
    virtual void NotifyExitAsync();

    virtual bool ReadyToWork();

    bool IsExitPending() const;
    bool IsRunning() const;

    pthread_t GetThread() const { return thread_; }
    
protected:
    virtual bool Run() = 0; // Derived class must implement Run()

private:
    Thread& operator=(const Thread&) = delete;
    static int ThreadStart(void* args);
    ThreadStatus Join(); // pthread created as detached

private:
    pthread_t thread_;
    mutable std::mutex lock_;
    std::condition_variable cvThreadExited_;
    ThreadStatus status_;
    volatile bool exitPending_;
    volatile bool running_;
};

} // namespace OHOS

#endif

