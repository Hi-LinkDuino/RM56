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

#ifndef HIVIEW_PLUGINS_EVENT_SERVICE_INCLUDE_SYS_EVENT_STAT_H
#define HIVIEW_PLUGINS_EVENT_SERVICE_INCLUDE_SYS_EVENT_STAT_H

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
class SysEventStat {
public:
    SysEventStat();
    ~SysEventStat();

public:
    void AccumulateEvent(bool isValid = true);
    void AccumulateEvent(const std::string &domain, const std::string &eventName, bool isValid = true);
    void StatSummary(int fd);
    void StatDetail(int fd);
    void StatInvaliDetail(int fd);
    void Clear(int fd);

private:
    void AddValidEvent(const std::string &domain, const std::string &eventName);
    void AddInValidEvent(const std::string &domain, const std::string &eventName);

private:
    std::mutex mutex_;
    int64_t inValidNum_;
    int64_t validNum_;
    std::map<std::string, std::map<std::string, int64_t>> d2es_;
    std::map<std::string, std::string> d2ies_;
}; // SysEventStat

class BaseStat {
public:
    BaseStat(int fd, int64_t validNum, int64_t inValidNum);
    virtual ~BaseStat();

public:
    void Stat() const;

protected:
    virtual void CalcStat() const = 0;

protected:
    int fd_;

private:
    void StatHead() const;
    void StatDetail() const;

private:
    int64_t validNum_;
    int64_t inValidNum_;
}; // BaseStat

class SysEventSummaryStat : public BaseStat {
public:
    SysEventSummaryStat(int fd, int64_t validNum, int64_t inValidNum,
        std::map<std::string, std::map<std::string, int64_t>> &d2es);
    ~SysEventSummaryStat();

    void CalcStat() const override;

protected:
    virtual void StatDomainEvent(int fd, std::map<std::string, int64_t> &events) const;

protected:
    std::map<std::string, std::map<std::string, int64_t>> &d2es_;

private:
    void GetDomains(std::vector<std::string> &domains) const;
    int64_t GetEventCount(std::map<std::string, int64_t> &events) const;
}; // SysEventSummaryStat

class SysEventDetailStat : public SysEventSummaryStat {
public:
    SysEventDetailStat(int fd, int64_t validNum, int64_t inValidNum,
        std::map<std::string, std::map<std::string, int64_t>> &d2es);
    ~SysEventDetailStat();

protected:
    void StatDomainEvent(int fd, std::map<std::string, int64_t> &events) const override;

private:
    void GetEventNames(std::map<std::string, int64_t> &events, std::vector<std::string> &eventNames) const;
}; // SysEventDetailStat

class SysEventInvalidStat : public BaseStat {
public:
    SysEventInvalidStat(int fd, int64_t validNum, int64_t inValidNum, std::map<std::string, std::string> &d2ies);
    ~SysEventInvalidStat();

public:
    void CalcStat() const override;

private:
    std::map<std::string, std::string> &d2ies_;
}; // SysEventInvalidStat
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_PLUGINS_EVENT_SERVICE_INCLUDE_SYS_EVENT_STAT_H