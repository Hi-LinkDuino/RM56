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

#include "sys_event_stat.h"

#include <algorithm>
#include <cinttypes>
#include <cstdio>
#include <vector>

#include "time_util.h"

namespace OHOS {
namespace HiviewDFX {
constexpr int INVALID_SIZE_LIMIT = 32;
constexpr int INVALID_EVENT_SIZE_LIMIT = 512;

SysEventStat::SysEventStat(): inValidNum_(0), validNum_(0)
{}

SysEventStat::~SysEventStat()
{
    inValidNum_ = 0;
    validNum_ = 0;
}

void SysEventStat::AccumulateEvent(bool isValid)
{
    if (isValid) {
        validNum_++;
    } else {
        inValidNum_++;
    }
}

void SysEventStat::AddValidEvent(const std::string &domain, const std::string &eventName)
{
    auto it = d2es_.find(domain);
    if (it == d2es_.end()) {
        std::map<std::string, int64_t> events;
        events[eventName] = 1;
        d2es_[domain] = events;
        return;
    }

    std::map<std::string, int64_t> &events = it->second;
    auto eIt = events.find(eventName);
    if (eIt == events.end()) {
        events[eventName] = 1;
        return;
    }

    events[eventName] = events[eventName] + 1;
}

void SysEventStat::AddInValidEvent(const std::string &domain, const std::string &eventName)
{
    auto it = d2ies_.find(domain);
    if (it == d2ies_.end()) {
        if (d2ies_.size() >= INVALID_SIZE_LIMIT) {
            return;
        }
        d2ies_[domain].append(eventName + ";");
        return;
    }
    if (it->second.size() >= INVALID_EVENT_SIZE_LIMIT) {
        return;
    }
    std::size_t pos = it->second.find(eventName + ";");
    if (pos == std::string::npos) {
        d2ies_[domain].append(eventName + ";");
    }
}

void SysEventStat::AccumulateEvent(const std::string &domain, const std::string &eventName, bool isValid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    AccumulateEvent(isValid);
    if (isValid) {
        AddValidEvent(domain, eventName);
    } else {
        AddInValidEvent(domain, eventName);
    }
}

void SysEventStat::StatSummary(int fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const BaseStat &stat = SysEventSummaryStat(fd, validNum_, inValidNum_, d2es_);
    stat.Stat();
}

void SysEventStat::StatDetail(int fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const BaseStat &stat = SysEventDetailStat(fd, validNum_, inValidNum_, d2es_);
    stat.Stat();
}

void SysEventStat::StatInvaliDetail(int fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const BaseStat &stat = SysEventInvalidStat(fd, validNum_, inValidNum_, d2ies_);
    stat.Stat();
}

void SysEventStat::Clear(int fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    inValidNum_ = 0;
    validNum_ = 0;
    d2es_.clear();
    d2ies_.clear();
    dprintf(fd, "clean all stat info\n");
}

BaseStat::BaseStat(int fd, int64_t validNum, int64_t inValidNum): fd_(fd), validNum_(validNum), inValidNum_(inValidNum)
{}

BaseStat::~BaseStat()
{}

void BaseStat::StatHead() const
{
    dprintf(fd_, "total count=%" PRId64 ", valid count=%" PRId64 ", invalid count=%" PRId64 "\n",
        (validNum_ + inValidNum_), validNum_, inValidNum_);
}

void BaseStat::StatDetail() const
{
    dprintf(fd_, "stat from %" PRId64  " to now\n", TimeUtil::GetMilliseconds());
}

void BaseStat::Stat() const
{
    StatHead();
    CalcStat();
    StatDetail();
}

SysEventSummaryStat::SysEventSummaryStat(int fd, int64_t validNum, int64_t inValidNum,
    std::map<std::string, std::map<std::string, int64_t>> &d2es): BaseStat(fd, validNum, inValidNum), d2es_(d2es)
{}

SysEventSummaryStat::~SysEventSummaryStat()
{}

void SysEventSummaryStat::GetDomains(std::vector<std::string> &domains) const
{
    for (auto it = d2es_.begin(); it != d2es_.end(); it++) {
        domains.emplace_back(it->first);
    }
    std::sort(domains.begin(), domains.end());
}

int64_t SysEventSummaryStat::GetEventCount(std::map<std::string, int64_t> &events) const
{
    int64_t total = 0;
    for (auto it = events.begin(); it != events.end(); it++) {
        total = total + it->second;
    }
    return total;
}

void SysEventSummaryStat::StatDomainEvent(int fd, std::map<std::string, int64_t> &events) const
{}

void SysEventSummaryStat::CalcStat() const
{
    std::vector<std::string> domains;
    GetDomains(domains);
    for (auto it = domains.begin(); it != domains.end(); it++) {
        auto events = d2es_.at(*it);
        dprintf(fd_, "*domain=%s total count=%" PRId64 "\n", it->c_str(), GetEventCount(events));
        StatDomainEvent(fd_, events);
    }
}

SysEventDetailStat::SysEventDetailStat(int fd, int64_t validNum, int64_t inValidNum,
    std::map<std::string, std::map<std::string, int64_t>> &d2es): SysEventSummaryStat(fd, validNum, inValidNum, d2es)
{}

SysEventDetailStat::~SysEventDetailStat()
{}

void SysEventDetailStat::GetEventNames(std::map<std::string, int64_t> &events,
    std::vector<std::string> &eventNames) const
{
    for (auto it = events.begin(); it != events.end(); it++) {
        eventNames.emplace_back(it->first);
    }
    std::sort(eventNames.begin(), eventNames.end());
}

void SysEventDetailStat::StatDomainEvent(int fd, std::map<std::string, int64_t> &events) const
{
    std::vector<std::string> eventNames;
    GetEventNames(events, eventNames);
    for (auto it = eventNames.begin(); it != eventNames.end(); it++) {
        auto count = events.at(*it);
        dprintf(fd, "    event=%s, count=%" PRId64 "\n", it->c_str(), count);
    }
}

SysEventInvalidStat::SysEventInvalidStat(int fd, int64_t validNum, int64_t inValidNum,
    std::map<std::string, std::string> &d2ies): BaseStat(fd, validNum, inValidNum), d2ies_(d2ies)
{}

SysEventInvalidStat::~SysEventInvalidStat()
{}

void SysEventInvalidStat::CalcStat() const
{
    std::vector<std::string> domains;
    for (auto it = d2ies_.begin(); it != d2ies_.end(); it++) {
        domains.emplace_back(it->first);
    }
    std::sort(domains.begin(), domains.end());
    for (auto it = domains.begin(); it != domains.end(); it++) {
        std::string invalidEvent = d2ies_.at(*it);
        dprintf(fd_, "*invalid event name of domain=%s\n", it->c_str());
        dprintf(fd_, "%s\n", invalidEvent.c_str());
    }
}
} // namespace HiviewDFX
} // namespace OHOS