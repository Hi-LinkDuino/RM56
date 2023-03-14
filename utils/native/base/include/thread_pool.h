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
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "nocopyable.h"

#include <thread>
#include <mutex>
#include <functional>
#include <string>
#include <condition_variable>
#include <deque>
#include <vector>

namespace OHOS {

const int INVALID_SEMA_VALUE = -1;

class ThreadPool : public NoCopyable {
public:
    typedef std::function<void()> Task;

    explicit ThreadPool(const std::string &name = std::string());
    ~ThreadPool();

    uint32_t Start(int threadsNum);
    void Stop();
    void AddTask(const Task& f);
    void SetMaxTaskNum(int maxSize) { maxTaskNum_ = maxSize; }

    // for testability
    size_t GetMaxTaskNum() const { return maxTaskNum_; }
    size_t GetCurTaskNum();
    size_t GetThreadsNum() const { return threads_.size(); }
    std::string GetName() const { return myName_; }

private:
    // tasks in the queue reach the maximum set by maxQueueSize, means thread pool is full load.
    bool Overloaded() const;
    void WorkInThread(); // main        function in each thread.
    Task ScheduleTask(); // fetch a task from the queue and execute

private:
    std::string myName_;
    std::mutex mutex_;
    std::condition_variable hasTaskToDo_;
    std::condition_variable acceptNewTask_;
    std::vector<std::thread> threads_;
    std::deque<Task> tasks_;
    size_t maxTaskNum_;
    bool running_;
};

} // namespace OHOS

#endif

