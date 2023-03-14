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

#include "sqlite_connection_pool.h"

#include "logger.h"
#include "rdb_errno.h"
#include "sqlite_global_config.h"
#include "sqlite_utils.h"

#include <condition_variable>
#include <mutex>
#include <vector>
#include <sstream>
#include <iostream>
#include <iterator>
#include <base_transaction.h>

namespace OHOS {
namespace NativeRdb {
SqliteConnectionPool *SqliteConnectionPool::Create(const RdbStoreConfig &storeConfig, int &errCode)
{
    auto pool = new SqliteConnectionPool(storeConfig);
    errCode = pool->Init();
    if (errCode != E_OK) {
        delete pool;
        return nullptr;
    }
    return pool;
}

SqliteConnectionPool::SqliteConnectionPool(const RdbStoreConfig &storeConfig)
    : config(storeConfig), writeConnection(nullptr), writeConnectionUsed(true), readConnections(),
      readConnectionCount(0), idleReadConnectionCount(0), transactionStack()
{
}

int SqliteConnectionPool::Init()
{
    int errCode = E_OK;
    writeConnection = SqliteConnection::Open(config, true, errCode);
    if (writeConnection == nullptr) {
        return errCode;
    }

    InitReadConnectionCount();

    for (int i = 0; i < readConnectionCount; i++) {
        SqliteConnection *connection = SqliteConnection::Open(config, false, errCode);
        if (connection == nullptr) {
            CloseAllConnections();
            config.ClearEncryptKey();
            return errCode;
        }
        readConnections.push_back(connection);
    }

    writeConnectionUsed = false;
    idleReadConnectionCount = readConnectionCount;
    return E_OK;
}

SqliteConnectionPool::~SqliteConnectionPool()
{
    config.ClearEncryptKey();
    CloseAllConnections();
}

void SqliteConnectionPool::InitReadConnectionCount()
{
    if (config.GetStorageMode() == StorageMode::MODE_MEMORY) {
        readConnectionCount = 0;
    } else if (config.GetJournalMode() == "WAL") {
        readConnectionCount = SqliteGlobalConfig::GetReadConnectionCount();
    } else {
        readConnectionCount = 0;
    }
}

void SqliteConnectionPool::CloseAllConnections()
{
    if (writeConnection != nullptr) {
        delete writeConnection;
    }
    writeConnection = nullptr;
    writeConnectionUsed = true;

    for (auto &item : readConnections) {
        if (item != nullptr) {
            delete item;
            item = nullptr;
        }
    }
    readConnections.clear();
    idleReadConnectionCount = 0;
}

SqliteConnection *SqliteConnectionPool::AcquireConnection(bool isReadOnly)
{
    if (isReadOnly && readConnectionCount != 0) {
        return AcquireReadConnection();
    } else {
        return AcquireWriteConnection();
    }
}
void SqliteConnectionPool::ReleaseConnection(SqliteConnection *connection)
{
    if (connection == writeConnection) {
        ReleaseWriteConnection();
    } else {
        ReleaseReadConnection(connection);
    }
}

SqliteConnection *SqliteConnectionPool::AcquireWriteConnection()
{
    std::unique_lock<std::mutex> lock(writeMutex);
    writeCondition.wait(lock, [&] { return !writeConnectionUsed; });
    writeConnectionUsed = true;
    return writeConnection;
}

void SqliteConnectionPool::ReleaseWriteConnection()
{
    {
        std::unique_lock<std::mutex> lock(writeMutex);
        writeConnectionUsed = false;
    }
    writeCondition.notify_one();
}

/**
 * get last element from connectionPool
 * @return
 */
SqliteConnection *SqliteConnectionPool::AcquireReadConnection()
{
    std::unique_lock<std::mutex> lock(readMutex);
    readCondition.wait(lock, [&] { return idleReadConnectionCount > 0; });
    SqliteConnection *connection = readConnections.back();
    readConnections.pop_back();
    idleReadConnectionCount--;
    return connection;
}

/**
 * push connection back to last of connectionPool
 * @param connection
 */
void SqliteConnectionPool::ReleaseReadConnection(SqliteConnection *connection)
{
    {
        std::unique_lock<std::mutex> lock(readMutex);
        readConnections.push_back(connection);
        idleReadConnectionCount++;
    }
    readCondition.notify_one();
}

bool SqliteConnectionPool::IsOverLength(const std::vector<uint8_t> &newKey)
{
    if (newKey.empty()) {
        return false;
    }

    std::stringstream ss;
    copy(newKey.begin(), newKey.end(), std::ostream_iterator<uint8_t>(ss, ""));
    return ss.str().length() > LIMITATION;
}

int SqliteConnectionPool::ChangeEncryptKey(const std::vector<uint8_t> &newKey)
{
    if (!config.IsInitEncrypted()) {
        return E_CHANGE_UNENCRYPTED_TO_ENCRYPTED;
    }

    if (newKey.empty()) {
        return E_EMPTY_NEW_ENCRYPT_KEY;
    }

    if (IsOverLength(newKey)) {
        return E_ERROR;
    }

    std::unique_lock<std::mutex> writeLock(writeMutex);
    if (writeConnectionUsed) {
        return E_CHANGE_ENCRYPT_KEY_IN_BUSY;
    }

    std::unique_lock<std::mutex> readLock(readMutex);
    if (idleReadConnectionCount < readConnectionCount) {
        return E_CHANGE_ENCRYPT_KEY_IN_BUSY;
    }

    int errCode = writeConnection->ChangeEncryptKey(newKey);
    if (errCode != E_OK) {
        return errCode;
    }

    config.UpdateEncryptKey(newKey);

    errCode = InnerReOpenReadConnections();

    return errCode;
}

int SqliteConnectionPool::InnerReOpenReadConnections()
{
    int errCode = E_OK;
    for (auto &item : readConnections) {
        if (item != nullptr) {
            delete item;
            item = nullptr;
        }
    }
    readConnections.clear();

    for (int i = 0; i < readConnectionCount; i++) {
        SqliteConnection *connection = SqliteConnection::Open(config, false, errCode);
        if (connection == nullptr) {
            config.ClearEncryptKey();
            CloseAllConnections();
            return errCode;
        }
        readConnections.push_back(connection);
    }

    return errCode;
}


int SqliteConnectionPool::ReOpenAvailableReadConnections()
{
    std::unique_lock<std::mutex> lock(readMutex);
    return InnerReOpenReadConnections();
}

#ifdef RDB_SUPPORT_ICU
/**
 * The database locale.
 */
int SqliteConnectionPool::ConfigLocale(const std::string localeStr)
{
    std::unique_lock<std::mutex> lock(rdbMutex);
    if (idleReadConnectionCount != readConnectionCount) {
        return E_NO_ROW_IN_QUERY;
    }

    for (int i = 0; i < idleReadConnectionCount; i++) {
        SqliteConnection *connection = readConnections[i];
        if (connection == nullptr) {
            LOG_ERROR("Read Connection is null.");
            return E_ERROR;
        }
        connection->ConfigLocale(localeStr);
    }

    if (writeConnection == nullptr) {
        LOG_ERROR("Write Connection is null.");
        return E_ERROR;
    } else {
        writeConnection->ConfigLocale(localeStr);
    }

    return E_OK;
}
#endif

/**
 * Rename the backed up database.
 */
int SqliteConnectionPool::ChangeDbFileForRestore(const std::string newPath, const std::string backupPath,
    const std::vector<uint8_t> &newKey)
{
    if (writeConnectionUsed == true || idleReadConnectionCount != readConnectionCount) {
        LOG_ERROR("Connection pool is busy now!");
        return E_ERROR;
    }

    CloseAllConnections();

    std::string currentPath = config.GetPath();
    bool ret = SqliteUtils::DeleteFile(currentPath);
    if (ret == false) {
        LOG_ERROR("DeleteFile error");
    }
    SqliteUtils::DeleteFile(currentPath + "-shm");
    SqliteUtils::DeleteFile(currentPath + "-wal");
    SqliteUtils::DeleteFile(currentPath + "-journal");

    if (currentPath != newPath) {
        SqliteUtils::DeleteFile(newPath);
        SqliteUtils::DeleteFile(newPath + "-shm");
        SqliteUtils::DeleteFile(newPath + "-wal");
        SqliteUtils::DeleteFile(newPath + "-journal");
    }

    ret = SqliteUtils::RenameFile(backupPath, newPath);
    if (ret == false) {
        LOG_ERROR("RenameFile error");
    }

    config.SetPath(newPath);
    config.UpdateEncryptKey(newKey);
    return Init();
}

std::stack<BaseTransaction> &SqliteConnectionPool::getTransactionStack()
{
    return transactionStack;
}
} // namespace NativeRdb
} // namespace OHOS
