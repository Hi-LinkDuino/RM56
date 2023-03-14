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

#include "thread_pool.h"
#include "errors.h"

#include <memory>

namespace OHOS {

ThreadPool::ThreadPool(const std::string& name)
    : myName_(name), maxTaskNum_(0), running_(false)
{
}

ThreadPool::~ThreadPool()
{
    if (running_) {
        Stop();
    }
}

uint32_t ThreadPool::Start(int numThreads)
{
    if (!threads_.empty()) {
        return ERR_INVALID_OPERATION;
    }

    if (numThreads <= 0) {
        return ERR_INVALID_VALUE;
    }
    running_ = true;
    threads_.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads_.push_back(std::thread(&ThreadPool::WorkInThread, this));
    }
    return ERR_OK;
}

void ThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex>  lock(mutex_);
        running_ = false;
        hasTaskToDo_.notify_all();
    }

    for (auto& e : threads_) {
        e.join();
    }
}

void ThreadPool::AddTask(const Task &f)
{
    if (threads_.empty()) {
        f();
    } else {
        std::unique_lock<std::mutex> lock(mutex_);
        while (Overloaded()) {
            acceptNewTask_.wait(lock);
        }

        tasks_.push_back(f);
        hasTaskToDo_.notify_one();
    }
}

size_t ThreadPool::GetCurTaskNum()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return tasks_.size();
}


ThreadPool::Task ThreadPool::ScheduleTask()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (tasks_.empty() && running_) {
        hasTaskToDo_.wait(lock);
    }

    Task task;
    if (!tasks_.empty()) {
        task = tasks_.front();
        tasks_.pop_front();

        if (maxTaskNum_ > 0) {
            acceptNewTask_.notify_one();
        }
    }
    return task;
}

bool ThreadPool::Overloaded() const
{
    return (maxTaskNum_ > 0) && (tasks_.size() >= maxTaskNum_);
}

void ThreadPool::WorkInThread()
{
    while (running_) {
        Task task = ScheduleTask();
        if (task) {
            task();
        }
    }
}

} // namespace OHOS
