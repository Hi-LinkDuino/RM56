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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_THREAD_BACKGROUND_TASK_EXECUTOR_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_THREAD_BACKGROUND_TASK_EXECUTOR_H

#include <list>
#include <thread>

#include "base/utils/noncopyable.h"

namespace OHOS::Ace {

enum class BgTaskPriority {
    DEFAULT,
    LOW,
};

class BackgroundTaskExecutor {
    ACE_DISALLOW_COPY_AND_MOVE(BackgroundTaskExecutor);

public:
    using Task = std::function<void()>;

    static BackgroundTaskExecutor& GetInstance();

    bool PostTask(Task&& task, BgTaskPriority priority = BgTaskPriority::DEFAULT);
    bool PostTask(const Task& task, BgTaskPriority priority = BgTaskPriority::DEFAULT);

    void TriggerGarbageCollection();

private:
    BackgroundTaskExecutor();
    ~BackgroundTaskExecutor();

    void StartNewThreads(size_t num = 1);
    void ThreadLoop(uint32_t threadNo);

    std::mutex mutex_;
    std::condition_variable condition_;
    std::list<Task> tasks_;
    std::list<Task> lowPriorityTasks_;
    std::list<std::thread> threads_;
    size_t currentThreadNum_ { 0 };
    size_t maxThreadNum_ { 0 };
    bool running_ { true };
    uint32_t purgeFlags_ { 0 };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_THREAD_BACKGROUND_TASK_EXECUTOR_H
