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

#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <queue>
#include <thread>
#include <functional>

namespace OHOS {
namespace NativePreferences {
using Task = std::function<void(void)>;

class TaskQueue {
public:
    explicit TaskQueue(bool lockable = true);
    ~TaskQueue();
    void PutTask(const Task &task);
    Task GetTaskAutoLock();
    void ReleaseLock();
    bool IsEmptyAndUnlocked();

private:
    bool lockable_;
    std::thread::id lockThread_;
    std::queue<Task> tasks_;
};
} // namespace NativePreferences
} // namespace OHOS
#endif // TASK_QUEUE_H
