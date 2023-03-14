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
#include "event_log_catcher.h"

#include <string>

#include <climits>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "securec.h"

#include "common_utils.h"
#include "defines.h"
#include "file_util.h"
#include "logger.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_LABEL(0xD002D01, "EventLogger-EventLogCatcher");
namespace {
    constexpr char SED_EXEC_PATH[] = "/system/bin/sed";
}

int EventLogCatcher::GetLogSize() const
{
    return logSize_;
};

void EventLogCatcher::SetLogSize(int size)
{
    logSize_ = size;
}

bool EventLogCatcher::Initialize(const std::string &strParam1 __UNUSED, int intParam1 __UNUSED, int intParam2 __UNUSED)
{
    useStreamFilter_ = CommonUtils::IsSpecificCmdExist(SED_EXEC_PATH);
    catcherStartTime_ = time(nullptr);
    return true;
}

int EventLogCatcher::Catch(int fd __UNUSED)
{
    return 0;
}

void EventLogCatcher::Stop()
{
    needStop_ = true;
}

int EventLogCatcher::AppendFile(int fd, const std::string &fileName) const
{
    char buf[BUF_SIZE_4096] = {0};

    if (fd < 0) {
        HIVIEW_LOGW("parameter err, fd:%{public}d, filename:%{public}s.", fd, fileName.c_str());
        return 0;
    }

    char path[PATH_MAX] = {0};
    if (realpath(fileName.c_str(), path) == nullptr) {
        HIVIEW_LOGW("canonicalize failed.");
        return 0;
    }

    if (fileName != std::string(path)) {
        HIVIEW_LOGW("fail to check consistency.");
        return 0;
    }

    int srcFd = open(path, O_RDONLY);
    if (srcFd < 0) {
        HIVIEW_LOGW("open %{public}s failed.", fileName.c_str());
        return 0;
    }

    int wn = 0;
    while (true) {
        int rn = read(srcFd, buf, sizeof(buf));
        if (rn == -1) {
            if (errno == EAGAIN) {
                continue;
            } else {
                break;
            }
        } else if (rn == 0) {
            break;
        }
        wn += write(fd, buf, rn);
    }
    close(srcFd);
    return wn;
}

std::string EventLogCatcher::GetDescription() const
{
    return description_;
}

int EventLogCatcher::GetFdSize(int32_t fd)
{
    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        return 0;
    }
    return fileStat.st_size;
}
} // namespace HiviewDFX
} // namespace OHOS
