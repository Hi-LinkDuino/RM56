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

#include "store_manager.h"

#include <cstdio>
#include <memory>
#include <string>

#include "doc_db.h"
#include "doc_store.h"
#include "ejdb2.h"
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
constexpr HiLogLabel LABEL = {LOG_CORE, 0xD002D10, "HiView-DOCDB"};
static iwkv_openflags GetEjdbFlag(const Option& option)
{
    switch (option.flag) {
        case Option::RDONLY:
            return IWKV_RDONLY;
        case Option::TRUNC:
            return IWKV_TRUNC;
        default:
            return IWKV_NO_TRIM_ON_CLOSE;
    }
}

std::shared_ptr<DocStore> StoreManager::GetDocStore(const Option& option)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = stores_.find(option.db);
    if (it != stores_.end()) {
        return it->second;
    }

    std::shared_ptr<DocStore> docStore = std::make_shared<DocStore>();
    docStore->dbPtr = std::make_shared<DocDB>();
    int retCode = docStore->dbPtr->OpenDB(option.db, GetEjdbFlag(option));
    if (retCode != 0) {
        HiLog::Error(LABEL, "can not open doc store");
    } else {
        HiLog::Info(LABEL, "open doc store");
    }
    stores_[option.db] = docStore;
    return docStore;
}

int StoreManager::OnlineBackupDocStore(const Option& option, const std::string &bakFile)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = stores_.find(option.db);
    if (it == stores_.end()) {
        HiLog::Error(LABEL, "can not find db to backup");
        return 1;
    }

    std::shared_ptr<DocStore> docStore = it->second;
    if (docStore->dbPtr == nullptr) {
        HiLog::Error(LABEL, "do not init db ptr");
        return -1;
    }

    int retCode = docStore->dbPtr->OnlineBackupDB(bakFile);
    if (retCode != 0) {
        HiLog::Error(LABEL, "can not backup doc store");
        return retCode;
    }
    return 0;
}

int StoreManager::InnerCloseDocStore(const Option& option)
{
    auto it = stores_.find(option.db);
    if (it == stores_.end()) {
        HiLog::Error(LABEL, "can not find db to close");
        return 1;
    }

    std::shared_ptr<DocStore> docStore = it->second;
    if (docStore->dbPtr == nullptr) {
        stores_.erase(it);
        return -1;
    }
    int retCode = docStore->dbPtr->CloseDB();
    if (retCode != 0) {
        HiLog::Error(LABEL, "can not close doc store");
        return retCode;
    }
    stores_.erase(it);
    return 0;
}

int StoreManager::CloseDocStore(const Option& option)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return InnerCloseDocStore(option);
}

int StoreManager::DeleteDocStore(const Option& option)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int retCode = InnerCloseDocStore(option);
    if (retCode != 0) {
        if (retCode > 0) {
            return retCode;
        }
        HiLog::Error(LABEL, "close doc store fail to delete");
        return retCode;
    }

    HiLog::Debug(LABEL, "delete ejdb file %{public}s", option.db.c_str());
    retCode = std::remove(option.db.c_str());
    if (retCode != 0) {
        HiLog::Error(LABEL, "remove doc store fail");
        return -1;
    }

    const std::string walDbFile = option.db + "-wal";
    HiLog::Debug(LABEL, "delete ejdb wal file %{public}s", walDbFile.c_str());
    retCode = std::remove(walDbFile.c_str());
    if (retCode != 0) {
        HiLog::Error(LABEL, "remove wal doc store fail");
        return -1;
    }
    HiLog::Info(LABEL, "remove doc store success");
    return 0;
}
} // HiviewDFX
} // OHOS