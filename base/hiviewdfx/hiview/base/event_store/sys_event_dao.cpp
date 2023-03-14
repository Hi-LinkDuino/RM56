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

#include "sys_event_dao.h"

#include "file_util.h"
#include "hiview_global.h"
#include "logger.h"
#include "store_mgr_proxy.h"

namespace OHOS {
namespace HiviewDFX {
namespace EventStore {
DEFINE_LOG_TAG("HiView-SysEventDao");
SysEventQuery SysEventDao::BuildQuery()
{
    SysEventQuery sysEventQuery(GetDataFile());
    return sysEventQuery;
}

int SysEventDao::Insert(std::shared_ptr<SysEvent> sysEvent)
{
    std::string dbFile = GetDataFile();
    HIVIEW_LOGD("insert db file %{public}s with %{public}s", dbFile.c_str(), sysEvent->eventName_.c_str());
    std::shared_ptr<DocStore> docStore = StoreMgrProxy::GetInstance().GetDocStore(dbFile);
    Entry entry;
    entry.id = 0;
    entry.value = sysEvent->jsonExtraInfo_;
    if (docStore->Put(entry) != 0) {
        HIVIEW_LOGE("insert error for event %{public}s", sysEvent->eventName_.c_str());
        return -1;
    }
    sysEvent->SetSeq(entry.id);
    return 0;
}

int SysEventDao::Update(std::shared_ptr<SysEvent> sysEvent, bool isNotifyChange)
{
    std::string dbFile = GetDataFile();
    HIVIEW_LOGD("update db file %{public}s", dbFile.c_str());
    std::shared_ptr<DocStore> docStore = StoreMgrProxy::GetInstance().GetDocStore(dbFile);
    Entry entry;
    entry.id = sysEvent->GetSeq();
    entry.value = sysEvent->jsonExtraInfo_;
    if (docStore->Merge(entry) != 0) {
        HIVIEW_LOGE("update error for event %{public}s", sysEvent->eventName_.c_str());
        return -1;
    }

    if (isNotifyChange) {
        HiviewGlobal::GetInstance()->PostUnorderedEvent(sysEvent);
    }
    return 0;
}

int SysEventDao::Delete(SysEventQuery &sysEventQuery)
{
    std::string dbFile = GetDataFile();
    HIVIEW_LOGD("delete db file %{public}s", dbFile.c_str());
    std::shared_ptr<DocStore> docStore = StoreMgrProxy::GetInstance().GetDocStore(dbFile);
    DataQuery dataQuery;
    sysEventQuery.GetDataQeury(dataQuery);
    if (docStore->Delete(dataQuery) != 0) {
        HIVIEW_LOGE("delete event error");
        return -1;
    }
    return 0;
}

int SysEventDao::BackupDB(const std::string &dbBakFile)
{
    std::string dbFile = GetDataFile();
    return StoreMgrProxy::GetInstance().BackupDocStore(dbFile, dbBakFile);
}

int SysEventDao::DeleteDB()
{
    std::string dbFile = GetDataFile();
    return StoreMgrProxy::GetInstance().DeleteDocStore(dbFile);
}

int SysEventDao::CloseDB()
{
    std::string dbFile = GetDataFile();
    return StoreMgrProxy::GetInstance().CloseDocStore(dbFile);
}

std::string SysEventDao::GetDataDir()
{
    std::string workPath = HiviewGlobal::GetInstance()->GetHiViewDirectory(
        HiviewContext::DirectoryType::WORK_DIRECTORY);
    if (workPath[workPath.size() - 1] != '/') {
        workPath = workPath + "/";
    }
    std::string sysEventDbFilePath = workPath + "sys_event_db/";
    if (!FileUtil::FileExists(sysEventDbFilePath)) {
        if (FileUtil::ForceCreateDirectory(sysEventDbFilePath, FileUtil::FILE_PERM_770)) {
            HIVIEW_LOGE("create sys_event_db path successful");
        } else {
            sysEventDbFilePath = workPath;
            HIVIEW_LOGE("create sys_event_db path fail, use default");
        }
    }
    return sysEventDbFilePath;
}

std::string SysEventDao::GetDataFile()
{
    std::string dbFile = GetDataDir() + "hisysevent.db";
    return dbFile;
}
} // EventStore
} // namespace HiviewDFX
} // namespace OHOS
