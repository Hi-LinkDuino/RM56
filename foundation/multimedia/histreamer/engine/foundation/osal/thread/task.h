/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_FOUNDATION_OSAL_TASK_H
#define HISTREAMER_FOUNDATION_OSAL_TASK_H

#include <atomic>
#include <functional>
#include <string>

#include "foundation/osal/thread/condition_variable.h"
#include "foundation/osal/thread/mutex.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "foundation/osal/thread/thread.h"

namespace OHOS {
namespace Media {
namespace OSAL {

class Task {
public:
    explicit Task(std::string name, ThreadPriority priority = ThreadPriority::HIGH);

    explicit Task(std::string name, std::function<void()> handler, ThreadPriority priority = ThreadPriority::HIGH);

    virtual ~Task();

    virtual void Start();

    virtual void Stop();

    virtual void StopAsync();

    virtual void Pause();

    virtual void PauseAsync();

    virtual void DoTask();

    void RegisterHandler(std::function<void()> handler);

private:
    enum class RunningState {
        STARTED,
        PAUSING,
        PAUSED,
        STOPPING,
        STOPPED,
    };

    void Run();

    const std::string name_;
    const ThreadPriority priority_;
    std::atomic<RunningState> runningState_{RunningState::PAUSED};
    std::function<void()> handler_ = [this] { DoTask(); };
    std::unique_ptr<OSAL::Thread> loop_;
    OSAL::Mutex stateMutex_{};
    OSAL::ConditionVariable syncCond_{};
};
} // namespace OSAL
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_OSAL_TASK_H
