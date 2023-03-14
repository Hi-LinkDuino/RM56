/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "dispatcher.h"
#include "log.h"

namespace utility {
static void EmptyTask()
{}

Dispatcher::Dispatcher(const std::string &name) : name_(name), taskQueue_(128)
{}

Dispatcher::~Dispatcher()
{}

void Dispatcher::Initialize()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!start_) {
        start_ = true;
        std::promise<void> startPromise;
        std::future<void> startFuture = startPromise.get_future();
        thread_ = std::make_unique<std::thread>(&Dispatcher::Run, this, std::move(startPromise));
        startFuture.wait();
    }
}

void Dispatcher::Uninitialize()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (start_) {
        start_ = false;
        taskQueue_.Push(std::bind(EmptyTask));
        if (thread_ && thread_->joinable()) {
            thread_->join();
            thread_ = nullptr;
        }
    }
}

void Dispatcher::PostTask(const std::function<void()> &task)
{
    if (start_) {
        taskQueue_.Push(std::move(task));
    }
}

const std::string &Dispatcher::Name() const
{
    return name_;
}

void Dispatcher::Run(std::promise<void> promise)
{
    pthread_setname_np(pthread_self(), name_.c_str());
    promise.set_value();

    while (start_) {
        std::function<void()> task;
        taskQueue_.Pop(task);
        task();
    }

    // If there are tasks in the queue. will not execute them.
}
}  // namespace utility