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

#include "store_session.h"
#include <chrono>
#include <stack>
#include <thread>
#include "logger.h"
#include "rdb_errno.h"
#include "shared_block.h"
#include "sqlite_database_utils.h"
#include "sqlite_utils.h"
#include "base_transaction.h"

namespace OHOS::NativeRdb {
StoreSession::StoreSession(SqliteConnectionPool &connectionPool)
    : connectionPool(connectionPool), connection(nullptr), connectionUseCount(0), isInStepQuery(false)
{
}

StoreSession::~StoreSession()
{
}

void StoreSession::AcquireConnection(bool isReadOnly)
{
    if (connection == nullptr) {
        connection = connectionPool.AcquireConnection(isReadOnly);
    }

    connectionUseCount += 1;
}

void StoreSession::ReleaseConnection()
{
    if ((connection == nullptr) || (connectionUseCount <= 0)) {
        LOG_ERROR("SQLiteSession ReleaseConnection repeated release");
        return;
    }

    if (--connectionUseCount == 0) {
        connectionPool.ReleaseConnection(connection);
        connection = nullptr;
    }
}

int StoreSession::PrepareAndGetInfo(
    const std::string &sql, bool &outIsReadOnly, int &numParameters, std::vector<std::string> &columnNames)
{
    // Obtains the type of SQL statement.
    int type = SqliteUtils::GetSqlStatementType(sql);
    if (SqliteUtils::IsSpecial(type)) {
        return E_TRANSACTION_IN_EXECUTE;
    }
    bool assumeReadOnly = SqliteUtils::IsSqlReadOnly(type);

    AcquireConnection(assumeReadOnly);
    int errCode = connection->PrepareAndGetInfo(sql, outIsReadOnly, numParameters, columnNames);
    if (errCode != 0) {
        ReleaseConnection();
        return errCode;
    }

    ReleaseConnection();
    return E_OK;
}

int StoreSession::BeginExecuteSql(const std::string &sql)
{
    int type = SqliteUtils::GetSqlStatementType(sql);
    if (SqliteUtils::IsSpecial(type)) {
        return E_TRANSACTION_IN_EXECUTE;
    }

    bool assumeReadOnly = SqliteUtils::IsSqlReadOnly(type);
    bool isReadOnly = false;
    AcquireConnection(assumeReadOnly);
    int errCode = connection->Prepare(sql, isReadOnly);
    if (errCode != 0) {
        ReleaseConnection();
        return errCode;
    }

    if (isReadOnly == connection->IsWriteConnection()) {
        ReleaseConnection();
        AcquireConnection(isReadOnly);
        if (!isReadOnly && !connection->IsWriteConnection()) {
            LOG_ERROR("StoreSession BeginExecutea : read connection can not execute write operation");
            ReleaseConnection();
            return E_EXECUTE_WRITE_IN_READ_CONNECTION;
        }
    }

    return E_OK;
}
int StoreSession::ExecuteSql(const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }

    errCode = connection->ExecuteSql(sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteForChangedRowCount(
    int &changedRows, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }

    errCode = connection->ExecuteForChangedRowCount(changedRows, sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteForLastInsertedRowId(
    int64_t &outRowId, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        LOG_ERROR("rdbStore BeginExecuteSql failed");
        return errCode;
    }

    errCode = connection->ExecuteForLastInsertedRowId(outRowId, sql, bindArgs);
    if (errCode != E_OK) {
        LOG_ERROR("rdbStore ExecuteForLastInsertedRowId FAILED");
    }
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteGetLong(int64_t &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }

    errCode = connection->ExecuteGetLong(outValue, sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteGetString(
    std::string &outValue, const std::string &sql, const std::vector<ValueObject> &bindArgs)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != 0) {
        return errCode;
    }
    std::string sqlstr = sql;
    int type = SqliteDatabaseUtils::GetSqlStatementType(sqlstr);
    if (type == STATEMENT_PRAGMA) {
        ReleaseConnection();
        AcquireConnection(false);
    }
    errCode = connection->ExecuteGetString(outValue, sql, bindArgs);
    ReleaseConnection();
    return errCode;
}

int StoreSession::Backup(const std::string databasePath, const std::vector<uint8_t> destEncryptKey)
{
    std::vector<ValueObject> bindArgs;
    bindArgs.push_back(ValueObject(databasePath));
    if (destEncryptKey.size() != 0) {
        bindArgs.push_back(ValueObject(destEncryptKey));
    } else {
        std::string str = "";
        bindArgs.push_back(ValueObject(str));
    }

    int errCode = ExecuteSql(ATTACH_BACKUP_SQL, bindArgs);
    if (errCode != E_OK) {
        LOG_ERROR("ExecuteSql ATTACH_BACKUP_SQL error %{public}d", errCode);
        return errCode;
    }
    int64_t count;
    errCode = ExecuteGetLong(count, EXPORT_SQL, std::vector<ValueObject>());
    if (errCode != E_OK) {
        LOG_ERROR("ExecuteSql EXPORT_SQL error %{public}d", errCode);
        return errCode;
    }

    errCode = ExecuteSql(DETACH_BACKUP_SQL, std::vector<ValueObject>());
    if (errCode != E_OK) {
        LOG_ERROR("ExecuteSql DETACH_BACKUP_SQL error %{public}d", errCode);
        return errCode;
    }
    return E_OK;
}

// Checks whether this thread holds a database connection.
bool StoreSession::IsHoldingConnection() const
{
    if (connection == nullptr) {
        return false;
    } else {
        return true;
    }
}

int StoreSession::CheckNoTransaction() const
{
    int errorCode = 0;
    if (connectionPool.getTransactionStack().empty()) {
        errorCode = E_STORE_SESSION_NO_CURRENT_TRANSACTION;
        return errorCode;
    }
    return E_OK;
}

int StoreSession::GiveConnectionTemporarily(int64_t milliseconds)
{
    int errorCode = CheckNoTransaction();
    if (errorCode != E_OK) {
        return errorCode;
    }
    BaseTransaction transaction = connectionPool.getTransactionStack().top();
    if (transaction.IsMarkedSuccessful() || connectionPool.getTransactionStack().size() > 1) {
        errorCode = E_STORE_SESSION_NOT_GIVE_CONNECTION_TEMPORARILY;
        return errorCode;
    }

    MarkAsCommit();
    EndTransaction();
    if (milliseconds > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
    BeginTransaction();
    return E_OK;
}

int StoreSession::Attach(
    const std::string &alias, const std::string &pathName, const std::vector<uint8_t> destEncryptKey)
{
    std::string journalMode;
    int errCode = ExecuteGetString(journalMode, "PRAGMA journal_mode", std::vector<ValueObject>());
    if (errCode != E_OK) {
        LOG_ERROR("RdbStoreImpl CheckAttach fail to get journal mode : %{public}d", errCode);
        return errCode;
    }
    journalMode = SqliteUtils::StrToUpper(journalMode);
    if (journalMode == "WAL") {
        LOG_ERROR("RdbStoreImpl attach is not supported in WAL mode");
        return E_NOT_SUPPORTED_ATTACH_IN_WAL_MODE;
    }

    std::vector<ValueObject> bindArgs;
    bindArgs.push_back(ValueObject(pathName));
    bindArgs.push_back(ValueObject(alias));
    if (destEncryptKey.size() != 0) {
        bindArgs.push_back(ValueObject(destEncryptKey));
    } else {
        std::string str = "";
        bindArgs.push_back(ValueObject(str));
    }
    errCode = ExecuteSql(ATTACH_SQL, bindArgs);
    if (errCode != E_OK) {
        LOG_ERROR("ExecuteSql ATTACH_SQL error %{public}d", errCode);
        return errCode;
    }

    return E_OK;
}

int StoreSession::BeginTransaction(TransactionObserver *transactionObserver)
{
    if (connectionPool.getTransactionStack().empty()) {
        AcquireConnection(false);
        if (!connection->IsWriteConnection()) {
            LOG_ERROR("StoreSession BeginExecutea : read connection can not begin transaction");
            ReleaseConnection();
            return E_BEGIN_TRANSACTION_IN_READ_CONNECTION;
        }

        int errCode = connection->ExecuteSql("BEGIN EXCLUSIVE;");
        if (errCode != E_OK) {
            ReleaseConnection();
            return errCode;
        }
    }

    if (transactionObserver != nullptr) {
        transactionObserver->OnBegin();
    }

    BaseTransaction transaction(connectionPool.getTransactionStack().size());
    connectionPool.getTransactionStack().push(transaction);

    return E_OK;
}

int StoreSession::MarkAsCommitWithObserver(TransactionObserver *transactionObserver)
{
    if (connectionPool.getTransactionStack().empty()) {
        return E_NO_TRANSACTION_IN_SESSION;
    }
    connectionPool.getTransactionStack().top().SetMarkedSuccessful(true);
    return E_OK;
}

int StoreSession::EndTransactionWithObserver(TransactionObserver *transactionObserver)
{
    if (connectionPool.getTransactionStack().empty()) {
        return E_NO_TRANSACTION_IN_SESSION;
    }

    BaseTransaction transaction = connectionPool.getTransactionStack().top();
    bool isSucceed = transaction.IsAllBeforeSuccessful() && transaction.IsMarkedSuccessful();
    connectionPool.getTransactionStack().pop();

    if (transactionObserver != nullptr) {
        if (isSucceed) {
            transactionObserver->OnCommit();
        } else {
            transactionObserver->OnRollback();
        }
    }

    if (!connectionPool.getTransactionStack().empty()) {
        if (transactionObserver != nullptr) {
            transactionObserver->OnRollback();
        }

        if (!isSucceed) {
            connectionPool.getTransactionStack().top().SetAllBeforeSuccessful(false);
        }
    } else {
        int errCode;
        if (isSucceed) {
            errCode = connection->ExecuteSql("COMMIT;");
        } else {
            errCode = connection->ExecuteSql("ROLLBACK;");
        }

        ReleaseConnection();
        return errCode;
    }

    return E_OK;
}

int StoreSession::MarkAsCommit()
{
    if (connectionPool.getTransactionStack().empty()) {
        return E_NO_TRANSACTION_IN_SESSION;
    }
    connectionPool.getTransactionStack().top().SetMarkedSuccessful(true);
    return E_OK;
}

int StoreSession::EndTransaction()
{
    if (connectionPool.getTransactionStack().empty()) {
        return E_NO_TRANSACTION_IN_SESSION;
    }

    BaseTransaction transaction = connectionPool.getTransactionStack().top();
    bool isSucceed = transaction.IsAllBeforeSuccessful() && transaction.IsMarkedSuccessful();
    connectionPool.getTransactionStack().pop();
    if (!connectionPool.getTransactionStack().empty()) {
        if (!isSucceed) {
            connectionPool.getTransactionStack().top().SetAllBeforeSuccessful(false);
        }
    } else {
        int errCode = connection->ExecuteSql(isSucceed ? "COMMIT;" : "ROLLBACK;");
        ReleaseConnection();
        return errCode;
    }

    return E_OK;
}
bool StoreSession::IsInTransaction() const
{
    return !connectionPool.getTransactionStack().empty();
}

std::shared_ptr<SqliteStatement> StoreSession::BeginStepQuery(
    int &errCode, const std::string &sql, const std::vector<std::string> &selectionArgs)
{
    if (isInStepQuery == true) {
        LOG_ERROR("StoreSession BeginStepQuery fail : begin more step query in one session !");
        errCode = E_MORE_STEP_QUERY_IN_ONE_SESSION;
        return nullptr; // fail,already in
    }

    if (SqliteUtils::GetSqlStatementType(sql) != SqliteUtils::STATEMENT_SELECT) {
        LOG_ERROR("StoreSession BeginStepQuery fail : not select sql !");
        errCode = E_EXECUTE_IN_STEP_QUERY;
        return nullptr;
    }

    AcquireConnection(true);
    std::shared_ptr<SqliteStatement> statement = connection->BeginStepQuery(errCode, sql, selectionArgs);
    if (statement == nullptr) {
        ReleaseConnection();
        return nullptr;
    }
    isInStepQuery = true;
    return statement;
}

int StoreSession::EndStepQuery()
{
    if (isInStepQuery == false) {
        return E_OK;
    }

    int errCode = connection->EndStepQuery();
    isInStepQuery = false;
    ReleaseConnection();
    return errCode;
}

int StoreSession::ExecuteForSharedBlock(int &rowNum, std::string sql, const std::vector<ValueObject> &bindArgs,
    AppDataFwk::SharedBlock *sharedBlock, int startPos, int requiredPos, bool isCountAllRows)
{
    int errCode = BeginExecuteSql(sql);
    if (errCode != E_OK) {
        return errCode;
    }
    errCode =
        connection->ExecuteForSharedBlock(rowNum, sql, bindArgs, sharedBlock, startPos, requiredPos, isCountAllRows);
    ReleaseConnection();
    return errCode;
}

int StoreSession::BeginTransaction()
{
    AcquireConnection(false);

    BaseTransaction transaction(connectionPool.getTransactionStack().size());
    int errCode = connection->ExecuteSql(transaction.getTransactionStr());
    if (errCode != E_OK) {
        LOG_DEBUG("storeSession BeginTransaction Failed");
        ReleaseConnection();
        return errCode;
    }
    connectionPool.getTransactionStack().push(transaction);
    ReleaseConnection();
    return E_OK;
}

int StoreSession::Commit()
{
    if (connectionPool.getTransactionStack().empty()) {
        return E_OK;
    }
    BaseTransaction transaction = connectionPool.getTransactionStack().top();
    std::string sqlStr = transaction.getCommitStr();
    if (sqlStr.size() <= 1) {
        connectionPool.getTransactionStack().pop();
        return E_OK;
    }

    AcquireConnection(false);
    int errCode = connection->ExecuteSql(sqlStr);
    ReleaseConnection();
    if (errCode != E_OK) {
        // if error the transaction is leaving for rollback
        return errCode;
    }
    connectionPool.getTransactionStack().pop();
    return E_OK;
}

int StoreSession::RollBack()
{
    std::stack<BaseTransaction> transactionStack = connectionPool.getTransactionStack();
    if (transactionStack.empty()) {
        return E_NO_TRANSACTION_IN_SESSION;
    }
    BaseTransaction transaction = transactionStack.top();
    transactionStack.pop();
    if (transaction.getType() != TransType::ROLLBACK_SELF && !transactionStack.empty()) {
        transactionStack.top().setChildFailure(true);
    }
    AcquireConnection(false);
    int errCode = connection->ExecuteSql(transaction.getRollbackStr());
    ReleaseConnection();
    if (errCode != E_OK) {
        LOG_ERROR("storeSession RollBack Fail");
    }

    return errCode;
}
} // namespace OHOS::NativeRdb
