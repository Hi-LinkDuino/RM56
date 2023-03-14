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
#include "faultlogger_client_test.h"

#include <sys/types.h>
#include <unistd.h>

#include <cinttypes>
#include <mutex>
#include <string>

#include "file_util.h"
namespace OHOS {
namespace HiviewDFX {
std::string GetFaultLogName(const time_t& time, int32_t id, const std::string& type, const std::string& module)
{
    static std::mutex localMutex;
    std::lock_guard<std::mutex> lock(localMutex);
    const int32_t bufLen = 64;
    struct tm* timeInfo = localtime(&time);
    if (timeInfo == nullptr) {
        return "";
    }

    char buf[bufLen] = {0};
    strftime(buf, bufLen - 1, "%Y%m%d%H%M%S", timeInfo);
    auto fileName = type + "-" + module + "-" + std::to_string(id) + "-" + std::string(buf, strlen(buf));
    return fileName;
}

FaultLogInfoInner CreateFaultLogInfo(const time_t& time, int32_t id, int32_t type, const std::string& module)
{
    FaultLogInfoInner info;
    info.time = time;
    info.id = id;
    info.pid = getpid();
    info.faultLogType = type;
    info.module = module;
    info.summary = "faultlogger_client_test";
    info.sectionMaps["APPVERSION"] = "1.0";
    info.sectionMaps["FAULT_MESSAGE"] = "Nullpointer";
    info.sectionMaps["TRACEID"] = "0x1646145645646";
    info.sectionMaps["KEY_THREAD_INFO"] = "Test Thread Info";
    info.sectionMaps["REASON"] = "TestReason";
    info.sectionMaps["STACKTRACE"] = "#01 xxxxxx\n#02 xxxxxx\n";
    return info;
}

bool CheckLogFileExist(const time_t& time, int32_t id, const std::string& type, const std::string& module)
{
    auto fileName = GetFaultLogName(time, id, type, module);
    auto path = "/data/log/faultlog/faultlogger/" + fileName;
    return FileUtil::FileExists(path);
}
} // namespace HiviewDFX
} // namespace OHOS