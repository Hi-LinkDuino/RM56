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

#ifndef NATIVE_RDB_RDB_STORE_SESSION_H
#define NATIVE_RDB_RDB_STORE_SESSION_H

#include <stack>
#include <iostream>
#include <memory>

#include "sqlite_connection.h"
#include "sqlite_connection_pool.h"
#include "value_object.h"
#include "transaction_observer.h"

namespace OHOS {
namespace NativeRdb {

class StoreSession {
public:
    explicit StoreSession(SqliteConnectionPool &connectionPool);
    ~StoreSession();
    int ExecuteSql(const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteForChangedRowCount(int &changedRows, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteForLastInsertedRowId(
        int64_t &outRowId, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteGetLong(int64_t &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int ExecuteGetString(std::string &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs);
    int Backup(const std::string databasePath, const std::vector<uint8_t> destEncryptKey);
    bool IsHoldingConnection() const;
    int GiveConnectionTemporarily(int64_t milliseconds);
    int CheckNoTransaction() const;
    int BeginTransaction(TransactionObserver *transactionObserver);
    int MarkAsCommitWithObserver(TransactionObserver *transactionObserver);
    int EndTransactionWithObserver(TransactionObserver *transactionObserver);
    int Attach(const std::string &alias, const std::string &pathName, const std::vector<uint8_t> destEncryptKey);
    int MarkAsCommit();
    int EndTransaction();
    bool IsInTransaction() const;
    std::shared_ptr<SqliteStatement> BeginStepQuery(
        int &errCode, const std::string &sql, const std::vector<std::string> &selectionArgs);
    int EndStepQuery();
    int PrepareAndGetInfo(const std::string &sql, bool &outIsReadOnly, int &numParameters,
        std::vector<std::string> &columnNames);
    int ExecuteForSharedBlock(int &rowNum, std::string sql, const std::vector<ValueObject> &bindArgs,
        AppDataFwk::SharedBlock *sharedBlock, int startPos, int requiredPos, bool isCountAllRows);

    int BeginTransaction();
    int Commit();
    int RollBack();

private:

    void AcquireConnection(bool isReadOnly);
    void ReleaseConnection();
    int BeginExecuteSql(const std::string &sql);
    SqliteConnectionPool &connectionPool;
    SqliteConnection *connection;
    int connectionUseCount;
    bool isInStepQuery;

    const std::string ATTACH_BACKUP_SQL = "ATTACH ? AS backup KEY ?";
    const std::string ATTACH_SQL = "ATTACH ? AS ? KEY ?";
    const std::string EXPORT_SQL = "SELECT export_database('backup')";
    const std::string DETACH_BACKUP_SQL = "detach backup";
};

} // namespace NativeRdb
} // namespace OHOS
#endif