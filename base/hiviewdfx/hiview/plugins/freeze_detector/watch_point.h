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

#ifndef FREEZE_WATCHPOINT_H
#define FREEZE_WATCHPOINT_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace HiviewDFX {
class WatchPoint {
public:
    class Builder {
    public:
        Builder();
        ~Builder();
        Builder& InitSeq(long seq);
        Builder& InitTimestamp(unsigned long long timestamp);
        Builder& InitPid(long pid);
        Builder& InitTid(long tid);
        Builder& InitUid(long uid);
        Builder& InitDomain(const std::string& domain);
        Builder& InitStringId(const std::string& stringId);
        Builder& InitMsg(const std::string& msg);
        Builder& InitPackageName(const std::string& packageName);
        Builder& InitProcessName(const std::string& processNam);
        Builder& InitLogPath(const std::string& logPath);
        WatchPoint Build() const;

    private:
        long seq_;
        unsigned long long timestamp_;
        long pid_;
        long uid_;
        long tid_;
        std::string domain_;
        std::string stringId_;
        std::string msg_;
        std::string packageName_;
        std::string processName_;
        std::string logPath_;
        friend class WatchPoint;
    };

    WatchPoint();
    explicit WatchPoint(const WatchPoint::Builder& builder);
    ~WatchPoint() {};

    long GetSeq() const;
    unsigned long long GetTimestamp() const;
    long GetPid() const;
    long GetTid() const;
    long GetUid() const;
    std::string GetDomain() const;
    std::string GetStringId() const;
    std::string GetMsg() const;
    std::string GetPackageName() const;
    std::string GetProcessName() const;
    std::string GetLogPath() const;
    void SetLogPath(const std::string& logPath);
    void SetSeq(long seq);
    bool operator<(const WatchPoint& node) const;
    bool operator==(const WatchPoint& node) const;

private:
    long seq_;
    unsigned long long timestamp_;
    long pid_;
    long uid_;
    long tid_;
    std::string domain_;
    std::string stringId_;
    std::string msg_;
    std::string packageName_;
    std::string processName_;
    std::string logPath_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
