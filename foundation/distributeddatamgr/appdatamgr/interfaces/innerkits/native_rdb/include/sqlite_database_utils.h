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

#ifndef NATIVE_RDB_SQLITE_DATABASE_UTILS_H
#define NATIVE_RDB_SQLITE_DATABASE_UTILS_H

#include <map>
#include <string>
#include <mutex>

#include "rdb_store_config.h"

namespace OHOS {
namespace NativeRdb {
enum Type {
    STATEMENT_SELECT = 1,
    STATEMENT_UPDATE = 2,
    STATEMENT_ATTACH = 3,
    STATEMENT_DETACH = 4,
    STATEMENT_BEGIN = 5,
    STATEMENT_COMMIT = 6,
    STATEMENT_ROLLBACK = 7,
    STATEMENT_PRAGMA = 8,
    STATEMENT_DDL = 9,
    STATEMENT_OTHER = 99,
    DATA_TYPE_BOOLEAN = 5,
    DATA_TYPE_BLOB = 4,
    DATA_TYPE_STRING = 3,
    DATA_TYPE_FLOAT = 2,
    DATA_TYPE_INTEGER = 1,
    DATA_TYPE_NULL = 0,
    SQL_FIRST_CHARACTER = 3
};

class SqliteDatabaseUtils {
public:
    static std::map<std::string, int> MapInit();
    static int GetSqlStatementType(std::string sql);
    static void DeleteFile(std::string &fileName);
    static bool RenameFile(std::string &oldFileName, std::string &newFileName);
    static std::string GetDefaultDatabasePath(std::string &context, std::string &name, int &errorCode);
    static std::string GetCorruptPath(std::string &path, int &errorCode);
    static std::string GetDatabasePathNoName(std::string &context, RdbStoreConfig &fileConfig, int &errorCode);

private:
    static std::map<std::string, int> g_statementType;
    static std::mutex g_locker;
    static int g_mkdirMode;

    SqliteDatabaseUtils();
    ~SqliteDatabaseUtils();

    static std::string GetDatabaseDir(RdbStoreConfig &fileConfig, std::string &securityLevel);
};
} // namespace NativeRdb
} // namespace OHOS

#endif
