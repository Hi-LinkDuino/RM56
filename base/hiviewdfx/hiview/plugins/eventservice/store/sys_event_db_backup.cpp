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

#include "sys_event_db_backup.h"

#include <cinttypes>

#include "file_util.h"
#include "logger.h"
#include "sys_event_dao.h"
#include "time_util.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-SysEventDbBak");
using EventStore::SysEventDao;
using EventStore::SysEventQuery;
using EventStore::EventCol;
using EventStore::Op;
using EventStore::ResultSet;

static void RemoveSysEventFile()
{
    if (FileUtil::FileExists(SysEventDao::GetDataFile())) {
        if (!FileUtil::RemoveFile(SysEventDao::GetDataFile())) {
            HIVIEW_LOGW("remove sys event wal db failed");
        }
    }

    std::string walDbFile = SysEventDao::GetDataFile() + "-wal";
    if (FileUtil::FileExists(walDbFile)) {
        if (!FileUtil::RemoveFile(walDbFile)) {
            HIVIEW_LOGW("remove sys event wal db failed");
        }
    }
}

SysEventDbBackup::SysEventDbBackup(int64_t &backupTime) : backupTime_(backupTime)
{
    dbBakFile_ = SysEventDao::GetDataDir() + "back_hisysevent.db";
}

SysEventDbBackup::~SysEventDbBackup()
{
}

void SysEventDbBackup::CheckDbStoreBroken()
{
    if (!IsBroken()) {
        BackupOnline();
        return;
    }

    Recover();
}

bool SysEventDbBackup::IsBroken()
{
    SysEventQuery sysEventQuery = SysEventDao::BuildQuery();
    ResultSet result = sysEventQuery.Where(EventCol::TS, Op::GT, 0).And(EventCol::TS, Op::LT, 1000).Execute(1); // 1s
    if (result.GetErrCode() != 0) {
        HIVIEW_LOGE("sys event db is broken");
        return true;
    }
    return false;
}

bool SysEventDbBackup::NeedBackup()
{
    int64_t now = TimeUtil::GetMilliseconds();
    if (backupTime_ == 0) {
        backupTime_ = now;
        return false;
    }

    int64_t oneDayInMis = TimeUtil::SECONDS_PER_DAY * TimeUtil::SEC_TO_MILLISEC;
    HIVIEW_LOGI("sys event backup db pass time %" PRId64 "", (now - backupTime_));
    if ((now - backupTime_) > oneDayInMis) {
        backupTime_ = now;
        return true;
    } else if (now < backupTime_) {
        backupTime_ = now;
        return true;
    }
    return false;
}

bool SysEventDbBackup::BackupOnline()
{
    if (!NeedBackup()) {
        return true;
    }

    if (SysEventDao::BackupDB(dbBakFile_) < 0) {
        HIVIEW_LOGE("sys event backup db failed");
        return false;
    }

    HIVIEW_LOGW("sys event backup db success");
    return true;
}

bool SysEventDbBackup::Recover()
{
    HIVIEW_LOGW("sys event recover db");
    if (!RecoverFromBackup()) {
        return RecoverByRebuild();
    }
    if (IsBroken()) {
        return RecoverByRebuild();
    }
    return true;
}

bool SysEventDbBackup::RecoverFromBackup()
{
    if (!FileUtil::FileExists(dbBakFile_)) {
        HIVIEW_LOGE("sys event backup db does not exists");
        return false;
    }

    if (SysEventDao::DeleteDB() < 0) {
        HIVIEW_LOGE("can not delete sys event db");
        return false;
    }

    HIVIEW_LOGW("recover sys event db with backup db file");
    RemoveSysEventFile();

    if (!FileUtil::RenameFile(dbBakFile_, SysEventDao::GetDataFile())) {
        HIVIEW_LOGW("recover sys event db from backup db failed");
        return false;
    }
    return true;
}

bool SysEventDbBackup::RecoverByRebuild()
{
    HIVIEW_LOGW("recover sys event db by rebuild");
    if (SysEventDao::CloseDB() < 0) {
        return false;
    }
    RemoveSysEventFile();
    IsBroken();
    return true;
}
} // namespace HiviewDFX
} // namespace OHOS