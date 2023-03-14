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

#include "task_queue.h"

namespace OHOS {
namespace NativePreferences {
TaskQueue::TaskQueue(bool lockable) : lockable_(lockable)
{
}

TaskQueue::~TaskQueue()
{
}

void TaskQueue::PutTask(const Task &task)
{
    if (!task) {
        return;
    }
    tasks_.push(task);
}

Task TaskQueue::GetTaskAutoLock()
{
    if (lockable_) {
        std::thread::id thisId = std::this_thread::get_id();
        if (thisId != lockThread_) {
            if (lockThread_ == std::thread::id()) {
                lockThread_ = thisId;
            } else {
                return nullptr;
            }
        }
    }
    if (tasks_.empty()) {
        ReleaseLock();
        return nullptr;
    }
    // copy and return
    Task task = tasks_.front();
    tasks_.pop();
    return task;
}

void TaskQueue::ReleaseLock()
{
    if (!lockable_) {
        return;
    }
    if (lockThread_ == std::this_thread::get_id()) {
        lockThread_ = std::thread::id();
    }
}

bool TaskQueue::IsEmptyAndUnlocked()
{
    if (lockable_) {
        if (lockThread_ != std::thread::id()) {
            return false;
        }
    }
    return tasks_.empty();
}
} // namespace NativePreferences
} // namespace OHOS
