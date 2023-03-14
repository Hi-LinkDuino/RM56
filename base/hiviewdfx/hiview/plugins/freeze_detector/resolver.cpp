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

#include "resolver.h"

#include <sys/time.h>

#include "db_helper.h"
#include "file_util.h"
#include "logger.h"
#include "plugin.h"
#include "string_util.h"
#include "sys_event.h"
#include "sys_event_dao.h"
#include "vendor.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("FreezeDetector");

FreezeResolver::FreezeResolver() : startTime_(time(nullptr) * MILLISECOND)
{
}

FreezeResolver::~FreezeResolver()
{
}

bool FreezeResolver::Init()
{
    // freeze_rules.xml
    if (FreezeRuleCluster::GetInstance().Init() == false) {
        HIVIEW_LOGE("failed to init rule.");
        return false;
    }

    return true;
}

bool FreezeResolver::ResolveEvent(WatchPoint& watchPoint, WatchPoint& matchedWatchPoint,
    std::list<WatchPoint>& list, FreezeResult& result) const
{
    unsigned long window = 0;
    if (FreezeRuleCluster::GetInstance().GetTimeWindow(watchPoint, window) == false) {
        return false;
    }

    if (Vendor::GetInstance().IsApplicationEvent(watchPoint.GetDomain(), watchPoint.GetStringId())) {
        if (window > DEFAULT_TIME_WINDOW) {
            window = DEFAULT_TIME_WINDOW;
        }
    }

    unsigned long long start = watchPoint.GetTimestamp() - (window * MILLISECOND);
    unsigned long long end = watchPoint.GetTimestamp();
    if (window == 0) {
        list.push_back(watchPoint);
    } else {
        DBHelper::SelectEventFromDB(false, start, end, list);
    }

    HIVIEW_LOGI("list size %{public}zu", list.size());
    return FreezeRuleCluster::GetInstance().GetResult(watchPoint, matchedWatchPoint, list, result);
}

std::shared_ptr<PipelineEvent> FreezeResolver::ProcessEvent(WatchPoint &watchPoint) const
{
    HIVIEW_LOGI("process event [%{public}s, %{public}s]",
        watchPoint.GetDomain().c_str(), watchPoint.GetStringId().c_str());

    FreezeResult result;
    std::list<WatchPoint> list;
    WatchPoint matchedWatchPoint;
    if (ResolveEvent(watchPoint, matchedWatchPoint, list, result) == false) {
        HIVIEW_LOGI("no rule for event [%{public}s, %{public}s]",
            watchPoint.GetDomain().c_str(), watchPoint.GetStringId().c_str());
        return nullptr;
    }

    if (watchPoint.GetDomain() == matchedWatchPoint.GetDomain() &&
        watchPoint.GetStringId() == matchedWatchPoint.GetStringId()) {
        // self rule with non-zero time window
        if (list.size() > 1) {
            std::list<WatchPoint>::iterator it;
            for (it = list.begin(); it != list.end();) {
                if (it->GetDomain() == watchPoint.GetDomain() && it->GetStringId() == watchPoint.GetStringId()) {
                    it++;
                } else {
                    it = list.erase(it);
                }
            }
        }
        // self rule with zero time window
    }
    else {
        list.clear();
        list.push_back(matchedWatchPoint);
        list.push_back(watchPoint);
    }

    std::string digest;
    std::string logPath = Vendor::GetInstance().MergeEventLog(watchPoint, list, result, digest);

    return Vendor::GetInstance().MakeEvent(watchPoint, matchedWatchPoint, list, result, logPath, digest);
}

std::string FreezeResolver::GetTimeZone() const
{
    std::string timeZone = "";
    struct timeval tv;
    struct timezone tz;
    if (gettimeofday(&tv, &tz) != 0) {
        HIVIEW_LOGE("failed to gettimeofday");
        return timeZone;
    }

    int hour = (-tz.tz_minuteswest) / MINUTES_IN_HOUR;
    timeZone = (hour >= 0) ? "+" : "-";

    int absHour = std::abs(hour);
    if (absHour < 10) {
        timeZone.append("0");
    }
    timeZone.append(std::to_string(absHour));

    int minute = (-tz.tz_minuteswest) % MINUTES_IN_HOUR;
    int absMinute = std::abs(minute);
    if (absMinute < 10) {
        timeZone.append("0");
    }
    timeZone.append(std::to_string(absMinute));

    return timeZone;
}
} // namespace HiviewDFX
} // namespace OHOS
