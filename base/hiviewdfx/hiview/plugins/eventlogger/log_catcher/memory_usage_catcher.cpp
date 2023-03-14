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
#include "memory_usage_catcher.h"

#include "securec.h"

#include "common_utils.h"
#include "log_catcher_utils.h"
namespace OHOS {
namespace HiviewDFX {
MemoryUsageCatcher::MemoryUsageCatcher() : EventLogCatcher()
{
    name_ = "MemoryUsageCatcher";
}

bool MemoryUsageCatcher::Initialize(const std::string& packageNam, int pid, int intParam2)
{
    if (pid <= 0 && packageNam.length() == 0) {
        description_ = "MemoryUsageCatcher -- pid==-1 packageName is null\n";
        return false;
    }
    pid_ = pid;
    packageName_ = packageNam;

    if (pid_ <= 0) {
        pid_ = CommonUtils::GetPidByName(packageName_);
    }

    char buf[BUF_SIZE_512] = {0};
    int ret = snprintf_s(buf, BUF_SIZE_512, BUF_SIZE_512 - 1,
        "MemoryUsageCatcher -- pid==%d packageName is %s\n", pid_, packageName_.c_str());
    if (ret > 0) {
        description_ = buf;
    }

    return EventLogCatcher::Initialize(packageNam, pid, intParam2);
}

int MemoryUsageCatcher::Catch(int fd)
{
    if (pid_ <= 0) {
        return -1;
    }
    auto originSize = GetFdSize(fd);

    std::string content;
    FileUtil::LoadStringFromFile("/proc/meminfo", content);
    FileUtil::SaveStringToFd(fd, content);

    CommonUtils::WriteCommandResultToFile(fd, "hidumper -c system");
    logSize_ = GetFdSize(fd) - originSize;
    return logSize_;
}
} // namespace HiviewDFX
} // namespace OHOS