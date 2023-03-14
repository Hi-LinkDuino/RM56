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

#ifndef NATIVE_RDB_SQLITE_CONNECTION_POOL_H
#define NATIVE_RDB_SQLITE_CONNECTION_POOL_H

#include <condition_variable>
#include <mutex>
#include <vector>
#include <sstream>
#include <iostream>
#include <iterator>
#include <stack>

#include "rdb_store_config.h"
#include "sqlite_config.h"
#include "sqlite_connection.h"
#include "base_transaction.h"

namespace OHOS {
namespace NativeRdb {

class SqliteConnectionPool {
public:
    static SqliteConnectionPool *Create(const RdbStoreConfig &storeConfig, int &errCode);
    ~SqliteConnectionPool();
    SqliteConnection *AcquireConnection(bool isReadOnly);
    void ReleaseConnection(SqliteConnection *connection);
    int ChangeEncryptKey(const std::vector<uint8_t> &newKey);
    int ReOpenAvailableReadConnections();
#ifdef RDB_SUPPORT_ICU
    int ConfigLocale(const std::string localeStr);
#endif
    int ChangeDbFileForRestore(const std::string newPath, const std::string backupPath,
        const std::vector<uint8_t> &newKey);
    std::stack<BaseTransaction> &getTransactionStack();

private:
    explicit SqliteConnectionPool(const RdbStoreConfig &storeConfig);
    int Init();
    void InitReadConnectionCount();
    SqliteConnection *AcquireWriteConnection();
    void ReleaseWriteConnection();
    SqliteConnection *AcquireReadConnection();
    void ReleaseReadConnection(SqliteConnection *connection);
    void CloseAllConnections();
    bool IsOverLength(const std::vector<uint8_t> &newKey);
    int InnerReOpenReadConnections();

    SqliteConfig config;
    SqliteConnection *writeConnection;
    std::mutex writeMutex;
    std::condition_variable writeCondition;
    bool writeConnectionUsed;

    std::vector<SqliteConnection *> readConnections;
    std::mutex readMutex;
    std::mutex rdbMutex;
    std::condition_variable readCondition;
    int readConnectionCount;
    int idleReadConnectionCount;
    const static int LIMITATION = 1024;

    std::stack<BaseTransaction> transactionStack;
};

} // namespace NativeRdb
} // namespace OHOS
#endif
