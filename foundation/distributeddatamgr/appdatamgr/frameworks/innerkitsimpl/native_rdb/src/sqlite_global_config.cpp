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

#include "sqlite_global_config.h"

#include <sys/types.h>
#include <sys/stat.h>

#include "logger.h"
#include "sqlite3sym.h"

namespace OHOS {
namespace NativeRdb {
const int SqliteGlobalConfig::SOFT_HEAP_LIMIT = 8 * 1024 * 1024; /* 8MB */
const bool SqliteGlobalConfig::CALLBACK_LOG_SWITCH = true;       /* Sqlite callback log switch */
const int SqliteGlobalConfig::CONNECTION_POOL_SIZE = 4;
const std::string SqliteGlobalConfig::MEMORY_DB_PATH = ":memory:";
const int SqliteGlobalConfig::DB_PAGE_SIZE = 4096;
const std::string SqliteGlobalConfig::DEFAULT_JOURNAL_MODE = "WAL";
const std::string SqliteGlobalConfig::WAL_SYNC_MODE = "FULL";
const int SqliteGlobalConfig::JOURNAL_FILE_SIZE = 524288; /* 512KB */
const int SqliteGlobalConfig::WAL_AUTO_CHECKPOINT = 100;  /* 100 pages */
constexpr int APP_DEFAULT_UMASK = 0002;

void SqliteGlobalConfig::InitSqliteGlobalConfig()
{
    static SqliteGlobalConfig globalConfig;
}

SqliteGlobalConfig::SqliteGlobalConfig()
{
    umask(APP_DEFAULT_UMASK);

    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);

    sqlite3_config(SQLITE_CONFIG_LOG, &SqliteLogCallback, CALLBACK_LOG_SWITCH ? reinterpret_cast<void *>(1) : NULL);

    sqlite3_soft_heap_limit(SOFT_HEAP_LIMIT);

    sqlite3_initialize();
}

SqliteGlobalConfig::~SqliteGlobalConfig()
{
}

void SqliteGlobalConfig::SqliteLogCallback(const void *data, int err, const char *msg)
{
    bool verboseLog = (data != nullptr);
    auto errType = static_cast<unsigned int>(err);
    errType &= 0xFF;
    if (errType == 0 || errType == SQLITE_CONSTRAINT || errType == SQLITE_SCHEMA || errType == SQLITE_NOTICE
        || err == SQLITE_WARNING_AUTOINDEX) {
        if (verboseLog) {
            LOG_INFO("SQLite Error(%{public}d) %{public}s ", err, msg);
        }
    } else {
        LOG_ERROR("SQLite Error(%{public}d) %{public}s", err, msg);
    }
}

int SqliteGlobalConfig::GetReadConnectionCount()
{
    return CONNECTION_POOL_SIZE - 1;
}

std::string SqliteGlobalConfig::GetMemoryDbPath()
{
    return MEMORY_DB_PATH;
}

int SqliteGlobalConfig::GetPageSize()
{
    return DB_PAGE_SIZE;
}

std::string SqliteGlobalConfig::GetWalSyncMode()
{
    return WAL_SYNC_MODE;
}

int SqliteGlobalConfig::GetJournalFileSize()
{
    return JOURNAL_FILE_SIZE;
}

int SqliteGlobalConfig::GetWalAutoCheckpoint()
{
    return WAL_AUTO_CHECKPOINT;
}

std::string SqliteGlobalConfig::GetDefaultJournalMode()
{
    return DEFAULT_JOURNAL_MODE;
}
} // namespace NativeRdb
} // namespace OHOS
