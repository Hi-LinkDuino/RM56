/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NATIVE_RDB_SQLITE_CONNECTION_H
#define NATIVE_RDB_SQLITE_CONNECTION_H

#include <mutex>
#include <memory>
#include <vector>

#include "sqlite3sym.h"
#include "sqlite_config.h"
#include "sqlite_statement.h"
#include "value_object.h"
#include "shared_block.h"

namespace OHOS {
namespace NativeRdb {

class SqliteConnection {
public:
    static SqliteConnection *Open(const SqliteConfig &config, bool isWriteConnection, int &errCode);
    ~SqliteConnection();
    bool IsWriteConnection() const;
    int Prepare(const std::string &sql, bool &outIsReadOnly);
    int PrepareAndGetInfo(const std::string &sql, bool &outIsReadOnly, int &numParameters,
        std::vector<std::string> &columnNames);
    int ExecuteSql(const std::string &sql, const std::vector<ValueObject> &bindArgs = std::vector<ValueObject>());
    int ExecuteForChangedRowCount(int &changedRows, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteForLastInsertedRowId(int64_t &outRowId, const std::string &sql,
        const std::vector<ValueObject> &bindArgs);
    int ExecuteGetLong(int64_t &outValue, const std::string &sql,
        const std::vector<ValueObject> &bindArgs = std::vector<ValueObject>());
    int ExecuteGetString(std::string &outValue, const std::string &sql,
        const std::vector<ValueObject> &bindArgs = std::vector<ValueObject>());
    std::shared_ptr<SqliteStatement> BeginStepQuery(int &errCode, const std::string &sql,
        const std::vector<std::string> &selectionArgs) const;
    int EndStepQuery();
    int ChangeEncryptKey(const std::vector<uint8_t> &newKey);
#ifdef RDB_SUPPORT_ICU
    int ConfigLocale(const std::string localeStr);
#endif
    int ExecuteForSharedBlock(int &rowNum, std::string sql, const std::vector<ValueObject> &bindArgs,
        AppDataFwk::SharedBlock *sharedBlock, int startPos, int requiredPos, bool isCountAllRows);

private:
    explicit SqliteConnection(bool isWriteConnection);
    int InnerOpen(const SqliteConfig &config);
    int Config(const SqliteConfig &config);
    int SetPageSize();
    int SetEncryptKey(const std::vector<uint8_t> &encryptKey);
    int SetJournalMode(const std::string &journalMode, const std::string &synclMode);
    int SetJournalSizeLimit();
    int SetAutoCheckpoint();
    int SetWalSyncMode(const std::string &syncMode);
    int PrepareAndBind(const std::string &sql, const std::vector<ValueObject> &bindArgs);
    void LimitPermission(const std::string &dbPath) const;

    int SetPersistWal();
    int SetBusyTimeout(int timeout);

    sqlite3 *dbHandle;
    bool isWriteConnection;
    bool isReadOnly;
    SqliteStatement statement;
    std::shared_ptr<SqliteStatement> stepStatement;
    std::string filePath;
    int openFlags;
    std::mutex rdbMutex;

    static constexpr int DEFAULT_BUSY_TIMEOUT_MS = 2000;
};

} // namespace NativeRdb
} // namespace OHOS
#endif
