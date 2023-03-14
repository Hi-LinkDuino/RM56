/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/common/flutter/flutter_task_executor.h"

#include <cerrno>
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#ifdef OHOS_STANDARD_SYSTEM
#include <sys/prctl.h>
#endif
#include <sys/resource.h>
#endif
#include <sys/time.h>
#include <unistd.h>

#ifdef FML_EMBEDDER_ONLY
#undef FML_EMBEDDER_ONLY
#define FML_EMBEDDER_ONLY
#endif
#include "flutter/fml/message_loop.h"
#ifdef OHOS_STANDARD_SYSTEM
#include "flutter/shell/platform/ohos/platform_task_runner.h"
#endif

#include "base/log/log.h"
#include "base/thread/background_task_executor.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t GPU_THREAD_PRIORITY = -10;
constexpr int32_t UI_THREAD_PRIORITY = -15;

inline std::string GenJsThreadName()
{
    static std::atomic<uint32_t> instanceCount { 1 };
    return std::string("jsThread-") + std::to_string(instanceCount.fetch_add(1, std::memory_order_relaxed));
}

TaskExecutor::Task WrapTaskWithContainer(TaskExecutor::Task&& task, int32_t id)
{
    auto wrappedTask = [originTask = std::move(task), id]() {
        ContainerScope scope(id);
        if (originTask) {
            originTask();
        }
    };
    return wrappedTask;
}

bool PostTaskToTaskRunner(const fml::RefPtr<fml::TaskRunner>& taskRunner, TaskExecutor::Task&& task, uint32_t delayTime)
{
    if (!taskRunner || !task) {
        return false;
    }

    if (delayTime > 0) {
        taskRunner->PostDelayedTask(std::move(task), fml::TimeDelta::FromMilliseconds(delayTime));
    } else {
        taskRunner->PostTask(std::move(task));
    }
    return true;
}

void SetThreadPriority(int32_t priority)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    if (setpriority(PRIO_PROCESS, gettid(), priority) < 0) {
        LOGW("Failed to set thread priority, errno = %{private}d", errno);
    }
#endif
}

} // namespace

FlutterTaskExecutor::FlutterTaskExecutor(const RefPtr<FlutterTaskExecutor>& taskExecutor)
{
    jsThread_ = std::make_unique<fml::Thread>(GenJsThreadName());
    jsRunner_ = jsThread_->GetTaskRunner();

    platformRunner_ = taskExecutor->platformRunner_;
    uiRunner_ = taskExecutor->uiRunner_;
    ioRunner_ = taskExecutor->ioRunner_;
    gpuRunner_ = taskExecutor->gpuRunner_;
}

FlutterTaskExecutor::FlutterTaskExecutor(const flutter::TaskRunners& taskRunners)
{
    jsThread_ = std::make_unique<fml::Thread>(GenJsThreadName());
    jsRunner_ = jsThread_->GetTaskRunner();

    platformRunner_ = taskRunners.GetPlatformTaskRunner();
    uiRunner_ = taskRunners.GetUITaskRunner();
    ioRunner_ = taskRunners.GetIOTaskRunner();
    gpuRunner_ = taskRunners.GetGPUTaskRunner();
}

FlutterTaskExecutor::~FlutterTaskExecutor()
{
    // To guarantee the jsThread released in platform thread
    auto rawPtr = jsThread_.release();
    PostTaskToTaskRunner(
        platformRunner_, [rawPtr] { std::unique_ptr<fml::Thread> jsThread(rawPtr); }, 0);
}

void FlutterTaskExecutor::InitPlatformThread(bool useCurrentEventRunner)
{
#ifdef OHOS_STANDARD_SYSTEM
    platformRunner_ = flutter::PlatformTaskRunner::CurrentTaskRunner(useCurrentEventRunner);
#else
    fml::MessageLoop::EnsureInitializedForCurrentThread();
    platformRunner_ = fml::MessageLoop::GetCurrent().GetTaskRunner();
#endif

    FillTaskTypeTable(TaskType::PLATFORM);
}

void FlutterTaskExecutor::InitJsThread(bool newThread)
{
    if (newThread) {
        jsThread_ = std::make_unique<fml::Thread>(GenJsThreadName());
        jsRunner_ = jsThread_->GetTaskRunner();
    } else {
        jsRunner_ = uiRunner_;
    }

    PostTaskToTaskRunner(
        jsRunner_, [weak = AceType::WeakClaim(this)] { FillTaskTypeTable(weak, TaskType::JS); }, 0);
}

void FlutterTaskExecutor::InitOtherThreads(const flutter::TaskRunners& taskRunners)
{
    uiRunner_ = taskRunners.GetUITaskRunner();
    ioRunner_ = taskRunners.GetIOTaskRunner();
    gpuRunner_ = taskRunners.GetGPUTaskRunner();

    PostTaskToTaskRunner(
        uiRunner_, [] { SetThreadPriority(UI_THREAD_PRIORITY); }, 0);
    PostTaskToTaskRunner(
        gpuRunner_, [] { SetThreadPriority(GPU_THREAD_PRIORITY); }, 0);

    PostTaskToTaskRunner(
        uiRunner_, [weak = AceType::WeakClaim(this)] { FillTaskTypeTable(weak, TaskType::UI); }, 0);
    PostTaskToTaskRunner(
        ioRunner_, [weak = AceType::WeakClaim(this)] { FillTaskTypeTable(weak, TaskType::IO); }, 0);
    PostTaskToTaskRunner(
        gpuRunner_, [weak = AceType::WeakClaim(this)] { FillTaskTypeTable(weak, TaskType::GPU); }, 0);
}

bool FlutterTaskExecutor::OnPostTask(Task&& task, TaskType type, uint32_t delayTime) const
{
    int32_t currentId = Container::CurrentId();
    TaskExecutor::Task wrappedTask =
        currentId >= 0 ? WrapTaskWithContainer(std::move(task), currentId) : std::move(task);

    switch (type) {
        case TaskType::PLATFORM:
            return PostTaskToTaskRunner(platformRunner_, std::move(wrappedTask), delayTime);
        case TaskType::UI:
            return PostTaskToTaskRunner(uiRunner_, std::move(wrappedTask), delayTime);
        case TaskType::IO:
            return PostTaskToTaskRunner(ioRunner_, std::move(wrappedTask), delayTime);
        case TaskType::GPU:
            return PostTaskToTaskRunner(gpuRunner_, std::move(wrappedTask), delayTime);
        case TaskType::JS:
            return PostTaskToTaskRunner(jsRunner_, std::move(wrappedTask), delayTime);
        case TaskType::BACKGROUND:
            // Ignore delay time
            return BackgroundTaskExecutor::GetInstance().PostTask(std::move(wrappedTask));
        default:
            return false;
    }
}

TaskExecutor::Task FlutterTaskExecutor::WrapTaskWithTraceId(Task&& task, int32_t id) const
{
    return WrapTaskWithContainer(std::move(task), id);
}

bool FlutterTaskExecutor::WillRunOnCurrentThread(TaskType type) const
{
    switch (type) {
        case TaskType::PLATFORM:
            return platformRunner_ ? platformRunner_->RunsTasksOnCurrentThread() : false;
        case TaskType::UI:
            return uiRunner_ ? uiRunner_->RunsTasksOnCurrentThread() : false;
        case TaskType::IO:
            return ioRunner_ ? ioRunner_->RunsTasksOnCurrentThread() : false;
        case TaskType::GPU:
            return gpuRunner_ ? gpuRunner_->RunsTasksOnCurrentThread() : false;
        case TaskType::JS:
            return jsRunner_ ? jsRunner_->RunsTasksOnCurrentThread() : false;
        case TaskType::BACKGROUND:
            // Always return false for background tasks.
            return false;
        default:
            return false;
    }
}

void FlutterTaskExecutor::AddTaskObserver(Task&& callback)
{
    fml::MessageLoop::GetCurrent().AddTaskObserver(reinterpret_cast<intptr_t>(this), std::move(callback));
}

void FlutterTaskExecutor::RemoveTaskObserver()
{
    if (!fml::MessageLoop::IsInitializedForCurrentThread()) {
        return;
    }
    fml::MessageLoop::GetCurrent().RemoveTaskObserver(reinterpret_cast<intptr_t>(this));
}

thread_local TaskExecutor::TaskType FlutterTaskExecutor::localTaskType = TaskExecutor::TaskType::UNKNOWN;

#ifdef ACE_DEBUG
static const char* TaskTypeToString(TaskExecutor::TaskType type)
{
    switch (type) {
        case TaskExecutor::TaskType::PLATFORM:
            return "PLATFORM";
        case TaskExecutor::TaskType::UI:
            return "UI";
        case TaskExecutor::TaskType::IO:
            return "IO";
        case TaskExecutor::TaskType::GPU:
            return "GPU";
        case TaskExecutor::TaskType::JS:
            return "JS";
        case TaskExecutor::TaskType::BACKGROUND:
            return "BACKGROUND";
        case TaskExecutor::TaskType::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

bool FlutterTaskExecutor::OnPreSyncTask(TaskType type) const
{
    std::lock_guard<std::mutex> lock(tableMutex_);
    auto it = taskTypeTable_.find(type);
    // when task type not filled, just skip
    if (it == taskTypeTable_.end()) {
        return true;
    }

    auto itSync = syncTaskTable_.find(it->second.threadId);
    while (itSync != syncTaskTable_.end()) {
        if (itSync->second == std::this_thread::get_id()) {
            DumpDeadSyncTask(localTaskType, type);
            ACE_DCHECK(itSync->second != std::this_thread::get_id() && "DEAD LOCK HAPPENED !!!");
            return false;
        }

        itSync = syncTaskTable_.find(itSync->second);
    }

    syncTaskTable_.emplace(std::this_thread::get_id(), it->second.threadId);
    return true;
}

void FlutterTaskExecutor::OnPostSyncTask() const
{
    std::lock_guard<std::mutex> lock(tableMutex_);
    syncTaskTable_.erase(std::this_thread::get_id());
}

void FlutterTaskExecutor::DumpDeadSyncTask(TaskType from, TaskType to) const
{
    auto itFrom = taskTypeTable_.find(from);
    auto itTo = taskTypeTable_.find(to);

    ACE_DCHECK(itFrom != taskTypeTable_.end());
    ACE_DCHECK(itTo != taskTypeTable_.end());

    LOGE("DEAD LOCK HAPPEN: %{public}s(%{public}d, %{public}s) -> %{public}s(%{public}d, %{public}s)",
        TaskTypeToString(from), itFrom->second.tid, itFrom->second.threadName.c_str(), TaskTypeToString(to),
        itTo->second.tid, itTo->second.threadName.c_str());
}
#endif

void FlutterTaskExecutor::FillTaskTypeTable(TaskType type)
{
    constexpr size_t MAX_THREAD_NAME_SIZE = 32;
    char threadNameBuf[MAX_THREAD_NAME_SIZE] = { 0 };
    const char* threadName = threadNameBuf;
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#ifdef OHOS_STANDARD_SYSTEM
    if (prctl(PR_GET_NAME, threadNameBuf) < 0) {
        threadName = "unknown";
    }
#else
    if (pthread_getname_np(pthread_self(), threadNameBuf, sizeof(threadNameBuf)) != 0) {
        threadName = "unknown";
    }
#endif
#endif

    localTaskType = type;
    ThreadInfo info = {
        .threadId = std::this_thread::get_id(),
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        .tid = gettid(),
#endif
        .threadName = threadName,
    };

    std::lock_guard<std::mutex> lock(tableMutex_);
    taskTypeTable_.emplace(type, info);
}

void FlutterTaskExecutor::FillTaskTypeTable(const WeakPtr<FlutterTaskExecutor>& weak, TaskType type)
{
    auto taskExecutor = weak.Upgrade();
    if (taskExecutor) {
        taskExecutor->FillTaskTypeTable(type);
    }
}

} // namespace OHOS::Ace
