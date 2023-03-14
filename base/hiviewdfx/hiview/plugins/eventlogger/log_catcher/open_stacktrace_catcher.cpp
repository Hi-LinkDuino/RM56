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
#include "open_stacktrace_catcher.h"

#include <ctime>
#include <string>

#include <sys/wait.h>
#include <unistd.h>

#include "securec.h"

#include "common_utils.h"
#include "file_util.h"
#include "log_catcher_utils.h"
#include "logger.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_LABEL(0xD002D01, "EventLogger-OpenStacktraceCatcher");
OpenStacktraceCatcher::OpenStacktraceCatcher() : EventLogCatcher()
{
    name_ = "OpenStacktraceCatcher";
}

bool OpenStacktraceCatcher::Initialize(const std::string& packageNam, int pid, int intParam2)
{
    if (pid <= 0 && packageNam.length() == 0) {
        description_ = "OpenStacktraceCatcher -- pid==-1 packageName is null\n";
        return false;
    }
    pid_ = pid;
    packageName_ = packageNam;

    if (pid_ <= 0) {
        pid_ = CommonUtils::GetPidByName(packageName_);
    }

    char buf[BUF_SIZE_512] = {0};
    int ret = snprintf_s(buf, BUF_SIZE_512, BUF_SIZE_512 - 1,
        "OpenStacktraceCatcher -- pid==%d packageName is %s\n", pid_, packageName_.c_str());
    if (ret > 0) {
        description_ = buf;
    }

    return EventLogCatcher::Initialize(packageNam, pid, intParam2);
};

// may block, run in another thread
int OpenStacktraceCatcher::Catch(int fd)
{
    if (pid_ <= 0) {
        return 0;
    }
    int originSize = GetFdSize(fd);
    auto str = CommonUtils::GetProcNameByPid(pid_);
    if (str.empty()) {
        HIVIEW_LOGE("pid is invalid %{public}d", pid_);
        FileUtil::SaveStringToFd(fd, "pid is invalid");
    } else {
        FileUtil::SaveStringToFd(fd, str + "\n");
    }
#ifdef DUMP_STACK_IN_PROCESS
    LogCatcherUtils::DumpStacktrace(fd, pid_);
#else
    ForkAndDumpStackTrace(fd);
#endif
    logSize_ = GetFdSize(fd) - originSize;
    return logSize_;
}

inline void OpenStacktraceCatcher::WaitChildPid(pid_t pid)
{
    int retryCount = 0;
    while (retryCount < MAX_RETRY_COUNT) {
        if (waitpid(pid, NULL, WNOHANG) != 0) {
            break;
        }
        retryCount++;
        usleep(SLEEP_TIME_US);
    }
}

int32_t OpenStacktraceCatcher::ForkAndDumpStackTrace(int32_t fd)
{
    int pid = -1;
    int leftTimeMicroSecond = 30000000; // 30000000us
    if ((pid = fork()) < 0) {
        HIVIEW_LOGE("Fork error, err:%{public}d", errno);
        return 0;
    }

    if (pid == 0) {
        auto newFd = dup(fd);
        int ret = LogCatcherUtils::DumpStacktrace(newFd, pid_);
        HIVIEW_LOGD("LogCatcherUtils::DumpStacktrace ret %{public}d", ret);
        close(newFd);
        _exit(ret);
    }

    while (true) {
        int status = 0;
        pid_t p = waitpid(pid, &status, WNOHANG);
        if (p < 0) {
            HIVIEW_LOGW("Waitpid return p=%{public}d, err:%{public}d", p, errno);
            return -1;
        }

        if (p == pid) {
            HIVIEW_LOGD("Dump process exited status is %{public}d", status);
            return WEXITSTATUS(status);
        }

        if (needStop_ || leftTimeMicroSecond <= 0) {
            HIVIEW_LOGW("Dump stacktrace timeout, killing pid %{public}d.", pid);
            std::string str = "Dump stacktrace timeout, Catch for " + std::to_string(pid_);
            FileUtil::SaveStringToFd(fd, str);
            kill(pid, SIGKILL);
            WaitChildPid(pid);
            return -1;
        }

        usleep(SLEEP_TIME_US); // poll every 0.1 sec
        leftTimeMicroSecond -= SLEEP_TIME_US;
    }
}
} // namespace HiviewDFX
} // namespace OHOS
