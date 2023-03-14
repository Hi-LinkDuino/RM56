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
#include "event_log_task.h"

#include <unistd.h>

#include "securec.h"

#include "common_utils.h"
#include "logger.h"
#include "string_util.h"

#include "binder_catcher.h"
#include "cpu_utilization_catcher.h"
#include "memory_usage_catcher.h"
#include "open_stacktrace_catcher.h"
#include "peer_binder_catcher.h"
namespace OHOS {
namespace HiviewDFX {
namespace {
const std::string SYSTEM_STACK[] = {
    "foundation",
};
}
DEFINE_LOG_LABEL(0xD002D01, "EventLogger-EventLogTask");
EventLogTask::EventLogTask(int fd, std::shared_ptr<SysEvent> event)
    : targetFd_(fd),
      event_(event),
      maxLogSize_(DEFAULT_LOG_SIZE),
      taskLogSize_(0),
      status_(Status::TASK_RUNNABLE)
{
    captureList_.push_back(std::bind(&EventLogTask::AppStackCapture, this, std::placeholders::_1));
    captureList_.push_back(std::bind(&EventLogTask::SystemStackCapture, this, std::placeholders::_1));
    captureList_.push_back(std::bind(&EventLogTask::BinderLogCapture, this, std::placeholders::_1));
    captureList_.push_back(std::bind(&EventLogTask::PeerBinderCapture, this, std::placeholders::_1));
    captureList_.push_back(std::bind(&EventLogTask::CpuUtilizationCapture, this, std::placeholders::_1));
    captureList_.push_back(std::bind(&EventLogTask::MemoryUsageCapture, this, std::placeholders::_1));
}

void EventLogTask::AddLog(const std::string &cmd)
{
    if (tasks_.size() == 0) {
        status_ = Status::TASK_RUNNABLE;
    }

    if (cmd == "c" || cmd == "m") {
        if (!cmFlage_) {
            cmFlage_ = true;
        } else {
            return;
        }
    }

    for (auto tmp : captureList_) {
        if (tmp(cmd)) {
            return;
        }
    }
}

EventLogTask::Status EventLogTask::StartCompose()
{
    // nothing to do, return success
    if (status_ != Status::TASK_RUNNABLE) {
        return status_;
    }
    status_ = Status::TASK_RUNNING;
    // nothing to do, return success
    if (tasks_.size() == 0) {
        return Status::TASK_SUCCESS;
    }

    uint32_t catcherIndex = 0;
    for (auto& catcher : tasks_) {
        catcherIndex++;
        auto dupedFd = dup(targetFd_);
        if (dupedFd < 0) {
            status_ = Status::TASK_FAIL;
            AddStopReason(targetFd_, catcher, "Fail to dup file descriptor, exit!");
            return TASK_FAIL;
        }

        AddSeparator(dupedFd, catcher);
        int curLogSize = catcher->Catch(dupedFd);
        if (ShouldStopLogTask(dupedFd, catcherIndex, curLogSize, catcher)) {
            close(dupedFd);
            break;
        }
        close(dupedFd);
    }

    if (status_ == Status::TASK_RUNNING) {
        status_ = Status::TASK_SUCCESS;
    }
    return status_;
}

bool EventLogTask::ShouldStopLogTask(int fd, uint32_t curTaskIndex, int curLogSize,
    std::shared_ptr<EventLogCatcher> catcher)
{
    if (status_ == Status::TASK_TIMEOUT) {
        HIVIEW_LOGE("Break Log task, parent has timeout.");
        return true;
    }

    bool encounterErr = (curLogSize < 0);
    bool hasFinished = (curTaskIndex == tasks_.size());
    if (!encounterErr) {
        taskLogSize_ += curLogSize;
    }

    if (taskLogSize_ > maxLogSize_ && !hasFinished) {
        AddStopReason(fd, catcher, "Exceed max log size");
        status_ = Status::TASK_EXCEED_SIZE;
        return true;
    }

    if (encounterErr) {
        AddStopReason(fd, catcher, "Log catcher not successful");
        HIVIEW_LOGE("catcher %{public}s, Log catcher not successful", catcher->GetDescription().c_str());
    }
    return false;
}

void EventLogTask::AddStopReason(int fd, std::shared_ptr<EventLogCatcher> catcher, const std::string& reason)
{
    char buf[BUF_SIZE_512] = {0};
    int ret = -1;
    if (catcher != nullptr) {
        catcher->Stop();
        // sleep 1s for syncing log to the fd, then we could append failure reason ?
        sleep(1);
        std::string summary = catcher->GetDescription();
        ret = snprintf_s(buf, BUF_SIZE_512, BUF_SIZE_512 - 1, "\nTask stopped when running catcher:%s, Reason:%s \n",
                         summary.c_str(), reason.c_str());
    } else {
        ret = snprintf_s(buf, BUF_SIZE_512, BUF_SIZE_512 - 1, "\nTask stopped, Reason:%s \n", reason.c_str());
    }

    if (ret > 0) {
        write(fd, buf, strnlen(buf, BUF_SIZE_512));
        fsync(fd);
    }
}

void EventLogTask::AddSeparator(int fd, std::shared_ptr<EventLogCatcher> catcher) const
{
    char buf[BUF_SIZE_512] = {0};
    std::string summary = catcher->GetDescription();
    if (summary.empty()) {
        HIVIEW_LOGE("summary.empty() catcher is %{public}s", catcher->GetName().c_str());
        return;
    }

    int ret = snprintf_s(buf, BUF_SIZE_512, BUF_SIZE_512 - 1, "\n%s:\n", summary.c_str());
    if (ret > 0) {
        write(fd, buf, strnlen(buf, BUF_SIZE_512));
        fsync(fd);
    }
}

EventLogTask::Status EventLogTask::GetTaskStatus() const
{
    return status_;
}

long EventLogTask::GetLogSize() const
{
    return taskLogSize_;
}

bool EventLogTask::AppStackCapture(const std::string &cmd)
{
    if (cmd == "s") {
        auto capture = std::make_shared<OpenStacktraceCatcher>();
        int pid = event_->GetEventIntValue("PID");
        pid = pid ? pid : event_->GetPid();
        capture->Initialize(event_->GetEventValue("PACKAGE_NAME"), pid, 0);
        tasks_.push_back(capture);
        return true;
    }
    return false;
}

bool EventLogTask::SystemStackCapture(const std::string &cmd)
{
    if (cmd == "S") {
        for (auto packageName : SYSTEM_STACK) {
            auto capture = std::make_shared<OpenStacktraceCatcher>();
            capture->Initialize(packageName, 0, 0);
            tasks_.push_back(capture);
        }
        return true;
    }
    return false;
}

bool EventLogTask::BinderLogCapture(const std::string &cmd)
{
    if (cmd == "b") {
        auto capture = std::make_shared<BinderCatcher>();
        capture->Initialize("", 0, 0);
        tasks_.push_back(capture);
        return true;
    }
    return false;
}

bool EventLogTask::PeerBinderCapture(const std::string &cmd)
{
    auto find = cmd.find("pb");
    if (find == cmd.npos) {
        return false;
    }

    std::vector<std::string> cmdList;
    StringUtil::SplitStr(cmd, ":", cmdList, true);
    if (cmdList.front() != "pb") {
        return false;
    }

    int layer = 0;
    int pid = event_->GetEventIntValue("PID");
    pid = pid ? pid : event_->GetPid();
    StringUtil::StrToInt(cmdList.back(), layer);
    HIVIEW_LOGI("pid:%{public}d, value:%{public}d ", pid, layer);

    auto capture = std::make_shared<PeerBinderCatcher>();
    capture->Initialize("", pid, layer);
    capture->Init(event_, "");
    tasks_.push_back(capture);

    return true;
}

bool EventLogTask::CpuUtilizationCapture(const std::string &cmd)
{
    if (cmd == "c") {
        auto capture = std::make_shared<CpuUtilizationCatcher>();
        int pid = event_->GetEventIntValue("PID");
        pid = pid ? pid : event_->GetPid();
        capture->Initialize(event_->GetEventValue("PACKAGE_NAME"), pid, 0);
        tasks_.push_back(capture);
        return true;
    }
    return false;
}

bool EventLogTask::MemoryUsageCapture(const std::string &cmd)
{
    if (cmd == "m") {
        auto capture = std::make_shared<MemoryUsageCatcher>();
        int pid = event_->GetEventIntValue("PID");
        pid = pid ? pid : event_->GetPid();
        capture->Initialize(event_->GetEventValue("PACKAGE_NAME"), pid, 0);
        tasks_.push_back(capture);
        return true;
    }
    return false;
}
} // namespace HiviewDFX
} // namespace OHOS