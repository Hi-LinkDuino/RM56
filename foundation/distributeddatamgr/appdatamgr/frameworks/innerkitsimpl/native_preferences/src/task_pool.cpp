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

#include "task_pool.h"
#include <climits>
#include "logger.h"
#include "preferences_errno.h"

namespace OHOS {
namespace NativePreferences {
constexpr int TaskPool::IDLE_WAIT_PERIOD;

TaskPool::TaskPool(int maxThreads, int minThreads)
    : genericTasks_(false), genericTaskCount_(0), queuedTaskCount_(0), isStarted_(false), isStopping_(false),
      isGenericThreadIdle_(false), maxThreads_(maxThreads), minThreads_(minThreads), curThreads_(0), idleThreads_(0)
{
}

TaskPool::~TaskPool()
{
}

int TaskPool::Start()
{
    if (maxThreads_ < minThreads_) {
        LOG_ERROR("Start task pool failed, maxThreads(%d) < minThreads(%d).", maxThreads_, minThreads_);
        return E_INVALID_ARGS;
    }
    if (maxThreads_ <= 0) {
        LOG_ERROR("Start task pool failed, maxThreads(%d) <= 0.", maxThreads_);
        return E_INVALID_ARGS;
    }
    if (minThreads_ < 0) {
        LOG_ERROR("Start task pool failed, minThreads(%d) < 0.", minThreads_);
        return E_INVALID_ARGS;
    }

    std::lock_guard<std::mutex> guard(tasksMutex_);
    isStarted_ = true; // parameters checked ok.
    isStopping_ = false;
    int errCode = SpawnThreads(true);
    if (errCode != E_OK) {
        LOG_WARN("Spawn threads failed when starting the task pool.");
        // ignore the error, we will try when schedule().
    }
    return E_OK;
}

void TaskPool::Stop()
{
    std::unique_lock<std::mutex> lock(tasksMutex_);
    if (!isStarted_) {
        return;
    }
    isStopping_ = true;
    hasTasks_.notify_all();
    allThreadsExited_.wait(lock, [this]() { return this->curThreads_ <= 0; });
    isStarted_ = false;
}

int TaskPool::Schedule(const Task &task)
{
    if (!task) {
        return E_INVALID_ARGS;
    }
    std::lock_guard<std::mutex> guard(tasksMutex_);
    if (!isStarted_) {
        LOG_ERROR("Schedule failed, the task pool is not started.");
        return E_NOT_PERMIT;
    }
    if (isStopping_) {
        LOG_ERROR("Schedule failed, the task pool is stopping.");
        return E_STALE;
    }
    if (genericTaskCount_ == INT_MAX) {
        LOG_ERROR("Schedule failed, the task pool is full.");
        return E_ERROR;
    }
    genericTasks_.PutTask(task);
    ++genericTaskCount_;
    hasTasks_.notify_one();
    TryToSpawnThreads();
    return E_OK;
}

int TaskPool::Schedule(const std::string &queueTag, const Task &task)
{
    if (!task) {
        return E_INVALID_ARGS;
    }
    std::lock_guard<std::mutex> guard(tasksMutex_);
    if (!isStarted_) {
        LOG_ERROR("Schedule failed, the task pool is not started.");
        return E_NOT_PERMIT;
    }
    if (isStopping_) {
        LOG_ERROR("Schedule failed, the task pool is stopping.");
        return E_STALE;
    }
    if (queuedTaskCount_ == INT_MAX) {
        LOG_ERROR("Schedule failed, the task pool is full.");
        return E_ERROR;
    }
    queuedTasks_[queueTag].PutTask(task);
    ++queuedTaskCount_;
    hasTasks_.notify_all();
    TryToSpawnThreads();
    return E_OK;
}

void TaskPool::ShrinkMemory(const std::string &tag)
{
    std::lock_guard<std::mutex> guard(tasksMutex_);
    auto iter = queuedTasks_.find(tag);
    if (iter != queuedTasks_.end()) {
        if (iter->second.IsEmptyAndUnlocked()) {
            queuedTasks_.erase(iter);
        }
    }
}

void TaskPool::Report()
{
    std::lock_guard<std::mutex> guard(tasksMutex_);
    LOG_INFO("[Task pool report:1] maxThreads:%d, minThreads:%d, curThreads:%d, "
             "idleThreads:%d, genericTaskCount:%d, queuedTaskCount:%d.",
        maxThreads_, minThreads_, curThreads_, idleThreads_, genericTaskCount_, queuedTaskCount_);
    LOG_INFO("[Task pool report:2] taskQueueCount:%zu.", queuedTasks_.size());
}

bool TaskPool::IdleExit(std::unique_lock<std::mutex> &lock)
{
    if (isStopping_) {
        return true;
    }
    ++idleThreads_;
    bool isGenericWorker = IsGenericWorker();
    if (!isGenericWorker && (curThreads_ > minThreads_)) {
        std::cv_status status = hasTasks_.wait_for(lock, std::chrono::seconds(IDLE_WAIT_PERIOD));
        if (status == std::cv_status::timeout && genericTaskCount_ <= 0) {
            --idleThreads_;
            return true;
        }
    } else {
        if (isGenericWorker) {
            isGenericThreadIdle_ = true;
            hasTasks_.notify_all();
        }
        hasTasks_.wait(lock);
        if (isGenericWorker) {
            isGenericThreadIdle_ = false;
        }
    }
    --idleThreads_;
    return false;
}

void TaskPool::SetThreadFree()
{
    for (auto &pair : queuedTasks_) {
        TaskQueue *tq = &pair.second;
        tq->ReleaseLock();
    }
}

Task TaskPool::ReapTask(TaskQueue *&queue)
{
    Task task = genericTasks_.GetTaskAutoLock();
    if (task != nullptr) {
        queue = nullptr;
        return task;
    }

    queue = nullptr;
    if (IsGenericWorker() && (curThreads_ > 1)) { // 1 indicates self.
        SetThreadFree();
        return nullptr;
    }

    for (auto &pair : queuedTasks_) {
        TaskQueue *tq = &pair.second;
        task = tq->GetTaskAutoLock();
        if (task != nullptr) {
            queue = tq;
            return task;
        }
    }
    return nullptr;
}

int TaskPool::GetTask(Task &task, TaskQueue *&queue)
{
    std::unique_lock<std::mutex> lock(tasksMutex_);

    while (true) {
        task = ReapTask(queue);
        if (task != nullptr) {
            return E_OK;
        }

        if (IdleExit(lock)) {
            break;
        }
    }
    return E_OK;
}

int TaskPool::SpawnThreads(bool isStart)
{
    if (!isStarted_) {
        LOG_ERROR("Spawn task pool threads failed, pool is not started.");
        return E_NOT_PERMIT;
    }
    if (curThreads_ >= maxThreads_) {
        // the pool is full of threads.
        return E_OK;
    }

    int limits = isStart ? minThreads_ : (curThreads_ + 1);
    while (curThreads_ < limits) {
        ++curThreads_;
        std::thread thread([this]() { TaskWorker(); });
        LOG_INFO("Spawn task pool threads, min:%d cur:%d max:%d", minThreads_, curThreads_, maxThreads_);
        thread.detach();
    }
    return E_OK;
}

bool TaskPool::IsGenericWorker() const
{
    return genericThread_ == std::this_thread::get_id();
}

void TaskPool::BecomeGenericWorker()
{
    std::lock_guard<std::mutex> guard(tasksMutex_);
    if (genericThread_ == std::thread::id()) {
        genericThread_ = std::this_thread::get_id();
    }
}

void TaskPool::ExitGenericWorker()
{
    std::lock_guard<std::mutex> guard(tasksMutex_);
    if (IsGenericWorker()) {
        genericThread_ = std::thread::id();
    }
    --curThreads_;
    allThreadsExited_.notify_all();
    LOG_INFO("Task pool thread exit, min:%d cur:%d max:%d, genericTaskCount:%d, queuedTaskCount:%d.", minThreads_,
        curThreads_, maxThreads_, genericTaskCount_, queuedTaskCount_);
}

void TaskPool::TaskWorker()
{
    BecomeGenericWorker();

    while (true) {
        TaskQueue *taskQueue = nullptr;
        Task task = nullptr;

        int errCode = GetTask(task, taskQueue);
        if (errCode != E_OK) {
            LOG_ERROR("Thread worker gets task failed, err:'%d'.", errCode);
            break;
        }
        if (task == nullptr) {
            // Idle thread exit.
            break;
        }

        task();
        FinishExecuteTask(taskQueue);
    }

    ExitGenericWorker();
}

void TaskPool::FinishExecuteTask(TaskQueue *taskQueue)
{
    std::lock_guard<std::mutex> guard(tasksMutex_);
    if (taskQueue != nullptr) {
        taskQueue->ReleaseLock();
        --queuedTaskCount_;
    } else {
        --genericTaskCount_;
    }
}

void TaskPool::TryToSpawnThreads()
{
    if ((curThreads_ >= maxThreads_) || (curThreads_ >= (queuedTaskCount_ + genericTaskCount_))) {
        return;
    }
    SpawnThreads(false);
}
} // namespace NativePreferences
} // namespace OHOS
