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
#ifndef HIVIEW_FAULTLOG_INFO_H
#define HIVIEW_FAULTLOG_INFO_H
#include <cstdint>
#include <string>
namespace OHOS {
namespace HiviewDFX {
class FaultLogInfo {
public:
    ~FaultLogInfo();

    int32_t GetId() const;
    int32_t GetProcessId() const;

    // the fd is managed by fault log info obj
    // it will be closed in destructor
    int32_t GetRawFileDescriptor() const;
    int32_t GetFaultType() const;
    int64_t GetTimeStamp() const;
    std::string GetStringFaultType() const;
    std::string GetFaultReason() const;
    std::string GetModuleName() const;
    std::string GetFaultSummary() const;

    void SetId(int32_t id);
    void SetProcessId(int32_t pid);
    void SetTimeStamp(int64_t ts);
    void SetFaultType(int32_t faultType);
    void SetFaultReason(const std::string &reason);
    void SetModuleName(const std::string &module);
    void SetFaultSummary(const std::string &summary);
    void SetRawFileDescriptor(int32_t fd);

private:
    int32_t pid_ {-1};
    int32_t uid_ {-1};
    int32_t type_ {0};
    int32_t fd_ {-1};
    int64_t ts_ {0};
    std::string reason_;
    std::string module_;
    std::string summary_;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEW_FAULTLOG_INFO_H