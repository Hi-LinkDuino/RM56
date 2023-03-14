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

#ifndef NATIVE_RDB_SQLITE_GLOBAL_CONFIG_H
#define NATIVE_RDB_SQLITE_GLOBAL_CONFIG_H

#include <string>

namespace OHOS {
namespace NativeRdb {

class SqliteGlobalConfig {
public:
    SqliteGlobalConfig();
    ~SqliteGlobalConfig();
    static void InitSqliteGlobalConfig();
    static void SqliteLogCallback(const void *data, int err, const char *msg);
    static int GetReadConnectionCount();
    static std::string GetMemoryDbPath();
    static int GetPageSize();
    static std::string GetWalSyncMode();
    static int GetJournalFileSize();
    static int GetWalAutoCheckpoint();
    static std::string GetDefaultJournalMode();

private:
    static const int SOFT_HEAP_LIMIT;
    static const bool CALLBACK_LOG_SWITCH;
    static const int CONNECTION_POOL_SIZE;
    static const std::string MEMORY_DB_PATH;
    static const int DB_PAGE_SIZE;
    static const std::string DEFAULT_JOURNAL_MODE;
    static const std::string WAL_SYNC_MODE;
    static const int JOURNAL_FILE_SIZE;
    static const int WAL_AUTO_CHECKPOINT;
};

} // namespace NativeRdb
} // namespace OHOS

#endif
