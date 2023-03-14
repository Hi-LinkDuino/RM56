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
#include "faultlog_info.h"

#include <cstdint>
#include <string>

#include <unistd.h>

#include "faultlogger_client.h"
namespace OHOS {
namespace HiviewDFX {
FaultLogInfo::~FaultLogInfo()
{
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

int32_t FaultLogInfo::GetId() const
{
    return uid_;
}

int32_t FaultLogInfo::GetProcessId() const
{
    return pid_;
}

int32_t FaultLogInfo::GetRawFileDescriptor() const
{
    return fd_;
}

int32_t FaultLogInfo::GetFaultType() const
{
    return type_;
}

int64_t FaultLogInfo::GetTimeStamp() const
{
    return ts_;
}

std::string FaultLogInfo::GetModuleName() const
{
    return module_;
}

std::string FaultLogInfo::GetFaultReason() const
{
    return reason_;
}

std::string FaultLogInfo::GetFaultSummary() const
{
    return summary_;
}

void FaultLogInfo::SetId(int32_t id)
{
    uid_ = id;
}

void FaultLogInfo::SetProcessId(int32_t pid)
{
    pid_ = pid;
}

void FaultLogInfo::SetFaultType(int32_t faultType)
{
    type_ = faultType;
}

void FaultLogInfo::SetRawFileDescriptor(int32_t fd)
{
    fd_ = fd;
}

void FaultLogInfo::SetTimeStamp(int64_t ts)
{
    ts_ = ts;
}

void FaultLogInfo::SetFaultReason(const std::string &reason)
{
    reason_ = reason;
}

void FaultLogInfo::SetModuleName(const std::string &module)
{
    module_ = module;
}

void FaultLogInfo::SetFaultSummary(const std::string &summary)
{
    summary_ = summary;
}

std::string FaultLogInfo::GetStringFaultType() const
{
    switch (type_) {
        case JAVA_CRASH:
            return "JavaCrash";
        case CPP_CRASH:
            return "CppCrash";
        case JS_CRASH:
            return "JsCrash";
        case APP_FREEZE:
            return "AppFreeze";
        case SYS_FREEZE:
            return "SysFreeze";
        default:
            return "UnknownFaultType";
    }
}
}  // namespace HiviewDFX
}  // namespace OHOS
