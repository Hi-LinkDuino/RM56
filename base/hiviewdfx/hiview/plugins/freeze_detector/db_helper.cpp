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

#include "db_helper.h"

#include <regex>

#include "logger.h"
#include "plugin.h"
#include "string_util.h"
#include "sys_event_dao.h"
#include "vendor.h"
#include "watch_point.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("FreezeDetector");

void DBHelper::SelectEventFromDB(
    bool all, unsigned long long start, unsigned long long end, std::list<WatchPoint>& list)
{
    if (start > end) {
        return;
    }

    EventStore::SysEventQuery eventQuery = EventStore::SysEventDao::BuildQuery();
    eventQuery.Select({EventStore::EventCol::TS})
        .Where(EventStore::EventCol::TS, EventStore::Op::GE, static_cast<int64_t>(start))
        .And(EventStore::EventCol::TS, EventStore::Op::LE, static_cast<int64_t>(end));
    if (all == false) { // with or without resolved events
        eventQuery.And(EventStore::Cond(EventStore::EventCol::INFO, EventStore::Op::NSW, "isResolved"));
        //    .Or(EventStore::Cond(EventStore::EventCol::INFO, EventStore::Op::NU)));
    }

    EventStore::ResultSet set = eventQuery.Execute();
    if (set.GetErrCode() != 0) {
        HIVIEW_LOGE("failed to select event from db, error:%{public}d.", set.GetErrCode());
        return;
    }

    while (set.HasNext()) {
        auto record = set.Next();

        std::string domain = record->GetEventValue(EventStore::EventCol::DOMAIN);
        std::string stringId = record->GetEventValue(EventStore::EventCol::NAME);
        if (Vendor::GetInstance().IsFreezeEvent(domain, stringId) == false) {
            continue;
        }

        long pid = record->GetEventIntValue(FreezeDetectorPlugin::EVENT_PID);
        pid = pid ? pid : record->GetPid();
        long uid = record->GetEventIntValue(FreezeDetectorPlugin::EVENT_UID);
        uid = uid ? uid : record->GetUid();
        long tid = std::strtoul(record->GetEventValue(EventStore::EventCol::TID).c_str(), nullptr, 0);

        WatchPoint watchPoint = WatchPoint::Builder()
            .InitSeq(record->GetSeq())
            .InitDomain(domain)
            .InitStringId(stringId)
            .InitTimestamp(record->happenTime_)
            .InitPid(pid)
            .InitUid(uid)
            .InitTid(tid)
            .InitPackageName(record->GetEventValue(FreezeDetectorPlugin::EVENT_PACKAGE_NAME))
            .InitProcessName(record->GetEventValue(FreezeDetectorPlugin::EVENT_PROCESS_NAME))
            .InitMsg(StringUtil::ReplaceStr(record->GetEventValue(FreezeDetectorPlugin::EVENT_MSG), "\\n", "\n"))
            .Build();

        std::string info = record->GetEventValue(EventStore::EventCol::INFO);
        std::regex reg("logPath:([^,]+)");
        std::smatch result;
        if (std::regex_search(info, result, reg)) {
            watchPoint.SetLogPath(result[1].str());
        }

        list.push_back(watchPoint);
    }

    list.sort();
    HIVIEW_LOGI("select event from db, size =%{public}zu.", list.size());
}

void DBHelper::UpdateEventIntoDB(const WatchPoint& watchPoint, int id)
{
    EventStore::SysEventQuery eventQuery = EventStore::SysEventDao::BuildQuery();
    EventStore::ResultSet set = eventQuery.Select({EventStore::EventCol::TS})
        .Where(EventStore::EventCol::TS, EventStore::Op::EQ, static_cast<int64_t>(watchPoint.GetTimestamp()))
        .Execute();
    if (set.GetErrCode() != 0) {
        HIVIEW_LOGE("failed to select event from db, error:%{public}d.", set.GetErrCode());
        return;
    }

    if (set.HasNext()) {
        auto record = set.Next();

        std::string info = "isResolved,eventId:" + std::to_string(id);
        std::string logPath = watchPoint.GetLogPath();
        if (logPath != "" && logPath != "nolog") {
            info += ",logPath:" + logPath;
        }
        record->SetEventValue(EventStore::EventCol::INFO, info);

        std::shared_ptr<SysEvent> sysEvent = std::make_shared<SysEvent>(*record);
        if (EventStore::SysEventDao::Update(sysEvent, false) != 0) {
            HIVIEW_LOGE("failed to update info into db, stringId:%{public}s.", watchPoint.GetStringId().c_str());
        }
    }
}
} // namespace HiviewDFX
} // namespace OHOS
