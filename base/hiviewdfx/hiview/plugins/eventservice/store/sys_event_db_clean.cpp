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

#include "sys_event_db_clean.h"

#include "file_util.h"
#include "logger.h"
#include "sys_event_dao.h"
#include "time_util.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-SysEventDbClean");
#ifdef CUSTOM_MAX_FILE_SIZE_MB
const int MAX_FILE_SIZE = 1024 * 1024 * CUSTOM_MAX_FILE_SIZE_MB;
#else
const int MAX_FILE_SIZE = 1024 * 1024 * 100; // 100M
#endif // CUSTOM_MAX_FILE_SIZE
using EventStore::SysEventDao;
using EventStore::SysEventQuery;
using EventStore::EventCol;
using EventStore::Op;
using EventStore::ResultSet;

bool SysEventDbClean::CleanDbStore()
{
    std::string dbFile = SysEventDao::GetDataFile();
    if (FileUtil::GetFileSize(dbFile) < MAX_FILE_SIZE) {
        HIVIEW_LOGD("does not need to clean db file");
        return true;
    }
    int count = 0;
    int64_t zeroClock = TimeUtil::Get0ClockStampMs();
    while (true) {
        count++;
        zeroClock = zeroClock - TimeUtil::SECONDS_PER_DAY * TimeUtil::SEC_TO_MILLISEC;
        SysEventQuery sysEventQuery = SysEventDao::BuildQuery();
        ResultSet result = sysEventQuery.Where(EventCol::TS, Op::LT, zeroClock).Execute(1);
        if (!result.HasNext()) {
            break;
        }
        if (count > 7) { // 7 days of week
            break;
        }
    }
    zeroClock = zeroClock + TimeUtil::SECONDS_PER_DAY * TimeUtil::SEC_TO_MILLISEC;
    SysEventQuery sysEventQuery = SysEventDao::BuildQuery();
    sysEventQuery.Where(EventCol::TS, Op::LT, zeroClock);
    int retCode = SysEventDao::Delete(sysEventQuery);
    if (retCode != 0) {
        HIVIEW_LOGI("clean the db file error at timestamp %{public}" PRId64 "", zeroClock);
        return false;
    }
    HIVIEW_LOGI("clean the db file at timestamp %{public}" PRId64 " successful", zeroClock);
    return true;
}
} // namespace HiviewDFX
} // namespace OHOS