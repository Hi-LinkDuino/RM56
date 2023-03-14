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

#include "core/common/watch_dog.h"

#include <cerrno>
#include <csignal>
#include <pthread.h>
#include <shared_mutex>

#include "flutter/fml/thread.h"

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/thread/background_task_executor.h"
#include "base/utils/utils.h"
#include "bridge/common/utils/engine_helper.h"
#include "core/common/ace_application_info.h"
#include "core/common/ace_engine.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t NORMAL_CHECK_PERIOD = 3;
constexpr int32_t WARNING_CHECK_PERIOD = 2;
constexpr int32_t FREEZE_CHECK_PERIOD = 1;
constexpr char JS_THREAD_NAME[] = "JS";
constexpr char UI_THREAD_NAME[] = "UI";
constexpr char UNKNOWN_THREAD_NAME[] = "unknown thread";
constexpr uint64_t ANR_INPUT_FREEZE_TIME = 5000;
constexpr int32_t IMMEDIATELY_PERIOD = 0;
constexpr int32_t ANR_DIALOG_BLOCK_TIME = 20;

enum class State { NORMAL, WARNING, FREEZE };

using Task = std::function<void()>;
std::unique_ptr<fml::Thread> g_anrThread;

bool PostTaskToTaskRunner(Task&& task, uint32_t delayTime)
{
    if (!g_anrThread || !task) {
        return false;
    }

    auto anrTaskRunner = g_anrThread->GetTaskRunner();
    if (delayTime > 0) {
        anrTaskRunner->PostDelayedTask(std::move(task), fml::TimeDelta::FromSeconds(delayTime));
    } else {
        anrTaskRunner->PostTask(std::move(task));
    }
    return true;
}

#if defined(OHOS_PLATFORM) || defined(ANDROID_PLATFORM)
constexpr int32_t SIGNAL_FOR_GC = 60;
constexpr int32_t GC_CHECK_PERIOD = 1;
pthread_t g_signalThread;

void CheckGcSignal()
{
    // Check if GC signal is in pending signal set
    sigset_t sigSet;
    sigemptyset(&sigSet);
    sigaddset(&sigSet, SIGNAL_FOR_GC);
    struct timespec interval = {
        .tv_sec = 0,
        .tv_nsec = 0,
    };
    int32_t result = sigtimedwait(&sigSet, nullptr, &interval);
    if (result < 0) {
        if (errno != EAGAIN && errno != EINTR) {
            LOGE("Failed to wait signals, errno = %{public}d", errno);
            return;
        }
    } else {
        ACE_DCHECK(result == SIGNAL_FOR_GC);

        // Start GC
        LOGE("Receive GC signal");
        AceEngine::Get().TriggerGarbageCollection();
    }

    // Check again
    PostTaskToTaskRunner(CheckGcSignal, GC_CHECK_PERIOD);
}

inline int32_t BlockGcSignal()
{
    // Block GC signal on current thread.
    sigset_t sigSet;
    sigemptyset(&sigSet);
    sigaddset(&sigSet, SIGNAL_FOR_GC);
    return pthread_sigmask(SIG_BLOCK, &sigSet, nullptr);
}

void OnSignalReceive(int32_t sigNum)
{
    // Forward GC signal to signal handling thread
    pthread_kill(g_signalThread, sigNum);
    BlockGcSignal();
}

void InitializeGcTrigger()
{
    // Record watch dog thread as signal handling thread
    g_signalThread = pthread_self();

    int32_t result = BlockGcSignal();
    if (result != 0) {
        LOGE("Failed to block GC signal, errno = %{public}d", result);
        return;
    }

    // Start to receive GC signal
    signal(SIGNAL_FOR_GC, OnSignalReceive);
    // Start check GC signal
    CheckGcSignal();
}
#endif // #if defined(OHOS_PLATFORM) || defined(ANDROID_PLATFORM)

} // namespace

class ThreadWatcher final : public Referenced {
public:
    ThreadWatcher(int32_t instanceId, TaskExecutor::TaskType type, bool useUIAsJSThread = false);
    ~ThreadWatcher() override;

    void SetTaskExecutor(const RefPtr<TaskExecutor>& taskExecutor);

    void BuriedBomb(uint64_t bombId);
    void DefusingBomb();

private:
    void InitThreadName();
    void CheckAndResetIfNeeded();
    bool IsThreadStuck();
    void HiviewReport() const;
    void RawReport(RawEventType type) const;
    void PostCheckTask();
    void TagIncrease();
    void Check();
    void ShowDialog() const;
    void DefusingTopBomb();
    void DetonatedBomb();

    mutable std::shared_mutex mutex_;
    int32_t instanceId_ = 0;
    TaskExecutor::TaskType type_;
    std::string threadName_;
    int32_t loopTime_ = 0;
    int32_t threadTag_ = 0;
    int32_t freezeCount_ = 0;
    State state_ = State::NORMAL;
    WeakPtr<TaskExecutor> taskExecutor_;
    std::queue<uint64_t> inputTaskIds_;
    bool canShowDialog_ = true;
    int32_t showDialogCount_ = 0;
    bool useUIAsJSThread_ = false;
};

ThreadWatcher::ThreadWatcher(int32_t instanceId, TaskExecutor::TaskType type, bool useUIAsJSThread)
    : instanceId_(instanceId), type_(type), useUIAsJSThread_(useUIAsJSThread)
{
    InitThreadName();
    PostTaskToTaskRunner(
        [weak = Referenced::WeakClaim(this)]() {
            auto sp = weak.Upgrade();
            if (sp) {
                sp->Check();
            }
        },
        NORMAL_CHECK_PERIOD);
}

ThreadWatcher::~ThreadWatcher() {}

void ThreadWatcher::SetTaskExecutor(const RefPtr<TaskExecutor>& taskExecutor)
{
    taskExecutor_ = taskExecutor;
}

void ThreadWatcher::BuriedBomb(uint64_t bombId)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    inputTaskIds_.emplace(bombId);
}

void ThreadWatcher::DefusingBomb()
{
    auto taskExecutor = taskExecutor_.Upgrade();
    if (taskExecutor) {
        taskExecutor->PostTask(
            [weak = Referenced::WeakClaim(this)]() {
                auto sp = weak.Upgrade();
                if (sp) {
                    sp->DefusingTopBomb();
                }
            },
            type_);
    }
}

void ThreadWatcher::DefusingTopBomb()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (inputTaskIds_.empty()) {
        return;
    }

    inputTaskIds_.pop();
}

void ThreadWatcher::InitThreadName()
{
    switch (type_) {
        case TaskExecutor::TaskType::JS:
            threadName_ = JS_THREAD_NAME;
            break;
        case TaskExecutor::TaskType::UI:
            threadName_ = UI_THREAD_NAME;
            break;
        default:
            threadName_ = UNKNOWN_THREAD_NAME;
            break;
    }
}

void ThreadWatcher::DetonatedBomb()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (inputTaskIds_.empty()) {
        return;
    }

    uint64_t currentTime = GetMilliseconds();
    uint64_t bombId = inputTaskIds_.front();

    if (currentTime - bombId > ANR_INPUT_FREEZE_TIME) {
        LOGE("Detonated the Bomb, which bombId is %{public}s and currentTime is %{public}s",
            std::to_string(bombId).c_str(), std::to_string(currentTime).c_str());
        if (canShowDialog_) {
            ShowDialog();
            canShowDialog_ = false;
            showDialogCount_ = 0;
        } else {
            LOGE("Can not show dialog when detonated the Bomb.");
        }

        std::queue<uint64_t> empty;
        std::swap(empty, inputTaskIds_);
    }
}

void ThreadWatcher::Check()
{
    int32_t period = NORMAL_CHECK_PERIOD;
    if (!IsThreadStuck()) {
        if (state_ == State::FREEZE) {
            RawReport(RawEventType::RECOVER);
        }
        freezeCount_ = 0;
        state_ = State::NORMAL;
        canShowDialog_ = true;
        showDialogCount_ = 0;
    } else {
        if (state_ == State::NORMAL) {
            HiviewReport();
            RawReport(RawEventType::WARNING);
            state_ = State::WARNING;
            period = WARNING_CHECK_PERIOD;
        } else if (state_ == State::WARNING) {
            RawReport(RawEventType::FREEZE);
            state_ = State::FREEZE;
            period = FREEZE_CHECK_PERIOD;
            DetonatedBomb();
        } else {
            if (!canShowDialog_) {
                showDialogCount_++;
                if (showDialogCount_ >= ANR_DIALOG_BLOCK_TIME) {
                    canShowDialog_ = true;
                    showDialogCount_ = 0;
                }
            }

            if (++freezeCount_ >= 5) {
                RawReport(RawEventType::FREEZE);
                freezeCount_ = 0;
            }
            period = FREEZE_CHECK_PERIOD;
            DetonatedBomb();
        }
    }

    PostTaskToTaskRunner(
        [weak = Referenced::WeakClaim(this)]() {
            auto sp = weak.Upgrade();
            if (sp) {
                sp->Check();
            }
        },
        period);
}

void ThreadWatcher::CheckAndResetIfNeeded()
{
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (loopTime_ < INT32_MAX) {
            return;
        }
    }

    std::unique_lock<std::shared_mutex> lock(mutex_);
    loopTime_ = 0;
    threadTag_ = 0;
}

bool ThreadWatcher::IsThreadStuck()
{
    bool res = false;
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (threadTag_ != loopTime_) {
            std::string abilityName;
            if (AceEngine::Get().GetContainer(instanceId_) != nullptr) {
                abilityName = AceEngine::Get().GetContainer(instanceId_)->GetHostClassName();
            }
            LOGE("thread stuck, ability: %{public}s, instanceId: %{public}d, thread: %{public}s", abilityName.c_str(),
                instanceId_, threadName_.c_str());
            res = true;
        }
    }
    CheckAndResetIfNeeded();
    PostCheckTask();
    return res;
}

void ThreadWatcher::HiviewReport() const
{
    if (type_ == TaskExecutor::TaskType::JS) {
        EventReport::SendJsException(JsExcepType::JS_THREAD_STUCK);
    } else if (type_ == TaskExecutor::TaskType::UI) {
        EventReport::SendRenderException(RenderExcepType::UI_THREAD_STUCK);
    }
}

void ThreadWatcher::RawReport(RawEventType type) const
{
    std::string message;
    if (type == RawEventType::FREEZE &&
        (type_ == TaskExecutor::TaskType::JS || (useUIAsJSThread_ && (type_ == TaskExecutor::TaskType::UI)))) {
        auto engine = EngineHelper::GetEngine(instanceId_);
        message = engine ? engine->GetStacktraceMessage() : "";
    }
    int32_t tid = 0;
    auto taskExecutor = taskExecutor_.Upgrade();
    if (taskExecutor) {
        tid = taskExecutor->GetTid(type_);
    }
    std::string threadInfo = "Blocked thread id = " + std::to_string(tid) + "\n";
    threadInfo += "JSVM instance id = " + std::to_string(instanceId_) + "\n";
    message = threadInfo + message;
    EventReport::ANRRawReport(type, AceApplicationInfo::GetInstance().GetUid(),
        AceApplicationInfo::GetInstance().GetPackageName(), AceApplicationInfo::GetInstance().GetProcessName(),
        message);
}

void ThreadWatcher::ShowDialog() const
{
    EventReport::ANRShowDialog(AceApplicationInfo::GetInstance().GetUid(),
        AceApplicationInfo::GetInstance().GetPackageName(), AceApplicationInfo::GetInstance().GetProcessName());
}

void ThreadWatcher::PostCheckTask()
{
    auto taskExecutor = taskExecutor_.Upgrade();
    if (taskExecutor) {
        // post task to specified thread to check it
        taskExecutor->PostTask(
            [weak = Referenced::WeakClaim(this)]() {
                auto sp = weak.Upgrade();
                if (sp) {
                    sp->TagIncrease();
                }
            },
            type_);
        std::unique_lock<std::shared_mutex> lock(mutex_);
        ++loopTime_;
    } else {
        LOGW("task executor with instanceId %{public}d invalid when check %{public}s thread whether stuck or not",
            instanceId_, threadName_.c_str());
    }
}

void ThreadWatcher::TagIncrease()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    ++threadTag_;
}

WatchDog::WatchDog()
{
    if (!g_anrThread) {
        g_anrThread = std::make_unique<fml::Thread>("anr");
    }
#if defined(OHOS_PLATFORM) || defined(ANDROID_PLATFORM)
    PostTaskToTaskRunner(InitializeGcTrigger, GC_CHECK_PERIOD);
#endif
}

WatchDog::~WatchDog()
{
    g_anrThread.reset();
}

void WatchDog::Register(int32_t instanceId, const RefPtr<TaskExecutor>& taskExecutor, bool useUIAsJSThread)
{
    Watchers watchers = {
        .jsWatcher = AceType::MakeRefPtr<ThreadWatcher>(instanceId, TaskExecutor::TaskType::JS),
        .uiWatcher = AceType::MakeRefPtr<ThreadWatcher>(instanceId, TaskExecutor::TaskType::UI, useUIAsJSThread),
    };
    watchers.uiWatcher->SetTaskExecutor(taskExecutor);
    if (!useUIAsJSThread) {
        watchers.jsWatcher->SetTaskExecutor(taskExecutor);
    } else {
        watchers.jsWatcher = nullptr;
    }
    const auto resExecutor = watchMap_.try_emplace(instanceId, watchers);
    if (!resExecutor.second) {
        LOGW("Duplicate instance id: %{public}d when register to watch dog", instanceId);
    }
}

void WatchDog::Unregister(int32_t instanceId)
{
    int32_t num = static_cast<int32_t>(watchMap_.erase(instanceId));
    if (num == 0) {
        LOGW("Unregister from watch dog failed with instanceID %{public}d", instanceId);
    }
}

void WatchDog::BuriedBomb(int32_t instanceId, uint64_t bombId)
{
    auto iter = watchMap_.find(instanceId);
    if (iter == watchMap_.end()) {
        return;
    }

    Watchers watchers = iter->second;
    PostTaskToTaskRunner(
        [watchers, bombId]() {
            if (watchers.jsWatcher) {
                watchers.jsWatcher->BuriedBomb(bombId);
            }

            if (watchers.uiWatcher) {
                watchers.uiWatcher->BuriedBomb(bombId);
            }
        },
        IMMEDIATELY_PERIOD);
}

void WatchDog::DefusingBomb(int32_t instanceId)
{
    auto iter = watchMap_.find(instanceId);
    if (iter == watchMap_.end()) {
        return;
    }

    Watchers watchers = iter->second;
    PostTaskToTaskRunner(
        [watchers]() {
            if (watchers.jsWatcher) {
                watchers.jsWatcher->DefusingBomb();
            }

            if (watchers.uiWatcher) {
                watchers.uiWatcher->DefusingBomb();
            }
        },
        IMMEDIATELY_PERIOD);
}

} // namespace OHOS::Ace