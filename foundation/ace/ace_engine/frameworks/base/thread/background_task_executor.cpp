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

#include "base/thread/background_task_executor.h"

#include <pthread.h>
#include <string>

#include "base/log/log.h"
#include "base/memory/memory_monitor.h"

namespace OHOS::Ace {
namespace {

constexpr size_t MAX_BACKGROUND_THREADS = 8;
constexpr uint32_t PURGE_FLAG_MASK = (1 << MAX_BACKGROUND_THREADS) - 1;

void SetThreadName(uint32_t threadNo)
{
    std::string name("ace.bg.");
    name.append(std::to_string(threadNo));
#ifdef MAC_PLATFORM
    pthread_setname_np(name.c_str());
#else
    pthread_setname_np(pthread_self(), name.c_str());
#endif
}

} // namespace

BackgroundTaskExecutor& BackgroundTaskExecutor::GetInstance()
{
    static BackgroundTaskExecutor instance;
    return instance;
}

BackgroundTaskExecutor::BackgroundTaskExecutor() : maxThreadNum_(MAX_BACKGROUND_THREADS)
{
    if (maxThreadNum_ > 1) {
        // Start other threads in the first created thread.
        PostTask([this, num = maxThreadNum_ - 1]() { StartNewThreads(num); });
    }

    // Make sure there is at least 1 thread in background thread pool.
    StartNewThreads(1);
}

BackgroundTaskExecutor::~BackgroundTaskExecutor()
{
    std::list<std::thread> threads;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        condition_.notify_all();
        threads = std::move(threads_);
    }

    for (auto& threadInPool : threads) {
        threadInPool.join();
    }
}

bool BackgroundTaskExecutor::PostTask(Task&& task, BgTaskPriority priority)
{
    if (!task) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) {
        return false;
    }
    switch (priority) {
        case BgTaskPriority::LOW:
            lowPriorityTasks_.emplace_back(std::move(task));
            break;
        default:
            tasks_.emplace_back(std::move(task));
            break;
    }
    condition_.notify_one();
    return true;
}

bool BackgroundTaskExecutor::PostTask(const Task& task, BgTaskPriority priority)
{
    if (!task) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) {
        return false;
    }
    switch (priority) {
        case BgTaskPriority::LOW:
            lowPriorityTasks_.emplace_back(task);
            break;
        default:
            tasks_.emplace_back(task);
            break;
    }
    condition_.notify_one();
    return true;
}

void BackgroundTaskExecutor::StartNewThreads(size_t num)
{
    uint32_t currentThreadNo = 0;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_ || currentThreadNum_ >= maxThreadNum_) {
            return;
        }
        if (currentThreadNum_ + num > maxThreadNum_) {
            num = maxThreadNum_ - currentThreadNum_;
        }
        currentThreadNo = currentThreadNum_ + 1;
        currentThreadNum_ += num;
    }

    // Start new threads.
    std::list<std::thread> newThreads;
    for (size_t idx = 0; idx < num; ++idx) {
        newThreads.emplace_back(std::bind(&BackgroundTaskExecutor::ThreadLoop, this, currentThreadNo + idx));
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) {
            threads_.splice(threads_.end(), newThreads);
        }
    }

    for (auto& newThread : newThreads) {
        // Join the new thread if stop running.
        if (newThread.joinable()) {
            newThread.join();
        }
    }
}

void BackgroundTaskExecutor::ThreadLoop(uint32_t threadNo)
{
    LOGI("Background thread is started");

    SetThreadName(threadNo);

    Task task;
    const uint32_t purgeFlag = (1 << (threadNo - 1));
    std::unique_lock<std::mutex> lock(mutex_);
    while (running_) {
        if (tasks_.empty() && lowPriorityTasks_.empty()) {
            if ((purgeFlags_ & purgeFlag) != purgeFlag) {
                condition_.wait(lock);
                continue;
            }

            lock.unlock();
            LOGD("Purge malloc cache for background thread %{public}u", threadNo);
            PurgeMallocCache();
            lock.lock();
            purgeFlags_ &= ~purgeFlag;
            continue;
        }
        // deal with tasks_ first. do lowPriorityTasks_ only when all tasks_ done.
        if (!tasks_.empty()) {
            task = std::move(tasks_.front());
            tasks_.pop_front();
        } else {
            task = std::move(lowPriorityTasks_.front());
            lowPriorityTasks_.pop_front();
        }

        lock.unlock();
        // Execute the task and clear after execution.
        task();
        task = nullptr;
        lock.lock();
    }

    LOGD("Background thread is stopped");
}

void BackgroundTaskExecutor::TriggerGarbageCollection()
{
    std::lock_guard<std::mutex> lock(mutex_);
    purgeFlags_ = PURGE_FLAG_MASK;
    condition_.notify_all();
}

} // namespace OHOS::Ace
