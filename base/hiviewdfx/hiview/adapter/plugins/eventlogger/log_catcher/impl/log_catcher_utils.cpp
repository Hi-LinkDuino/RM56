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
#include "log_catcher_utils.h"

#include <sstream>
#include <string>

#include "dfx_dump_catcher.h"

#include "common_utils.h"
#include "file_util.h"
#include "logger.h"
#include "string_util.h"
namespace OHOS {
namespace HiviewDFX {
namespace LogCatcherUtils {
int DumpStacktrace(int fd, int pid)
{
    if (fd < 0) {
        return -1;
    }
    DfxDumpCatcher dumplog;
    std::string msg = "";
    bool ret = dumplog.DumpCatch(pid, 0, msg);
    if (ret) {
        FileUtil::SaveStringToFd(fd, msg);
    } else {
        msg = "Failed to dump stacktrace for " + std::to_string(pid) + "\n";
        FileUtil::SaveStringToFd(fd, msg);
    }
    return 0;
}

bool ReadCPUInfo(int fd, int pid)
{
    std::string content;
    bool ret1 = FileUtil::LoadStringFromFile("/proc/cpuinfo", content);
    FileUtil::SaveStringToFd(fd, content);
    return ret1;
}

bool ReadMemoryInfo(int fd, int pid)
{
    std::string content;
    bool ret1 = FileUtil::LoadStringFromFile("/proc/meminfo", content);
    FileUtil::SaveStringToFd(fd, content);
    return ret1;
}
}
} // namespace HiviewDFX
} // namespace OHOS