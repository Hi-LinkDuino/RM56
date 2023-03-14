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

#ifndef NATIVE_RDB_SQLITE_UTILS_H
#define NATIVE_RDB_SQLITE_UTILS_H

#include <map>
#include <string>

namespace OHOS {
namespace NativeRdb {

class SqliteUtils {
public:
    static const int STATEMENT_SELECT;
    static const int STATEMENT_UPDATE;
    static const int STATEMENT_ATTACH;
    static const int STATEMENT_DETACH;
    static const int STATEMENT_BEGIN;
    static const int STATEMENT_COMMIT;
    static const int STATEMENT_ROLLBACK;
    static const int STATEMENT_PRAGMA;
    static const int STATEMENT_DDL;
    static const int STATEMENT_OTHER;
    static const int CONFLICT_CLAUSE_COUNT = 6;

    static int GetSqlStatementType(const std::string &sql);
    static bool IsSqlReadOnly(int sqlType);
    static bool IsSpecial(int sqlType);
    static int GetConflictClause(int conflictResolution, std::string &conflictClause);
    static std::string StrToUpper(std::string s);
    static bool DeleteFile(const std::string path);
    static bool RenameFile(const std::string srcFile, const std::string destFile);

private:
    static const std::map<std::string, int> SQL_TYPE_MAP;
    static const std::string ON_CONFLICT_CLAUSE[CONFLICT_CLAUSE_COUNT];
};

} // namespace NativeRdb
} // namespace OHOS
#endif