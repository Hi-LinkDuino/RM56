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

#ifndef NATIVE_RDB_SQLITE_STATEMENT_H
#define NATIVE_RDB_SQLITE_STATEMENT_H

#include <vector>

#include "sqlite3sym.h"
#include "value_object.h"

namespace OHOS {
namespace NativeRdb {
class SqliteStatement {
public:
    SqliteStatement();
    ~SqliteStatement();
    int Prepare(sqlite3 *dbHandle, const std::string &sql);
    int Finalize();
    int BindArguments(const std::vector<ValueObject> &bindArgs) const;
    int ResetStatementAndClearBindings() const;
    int Step() const;

    int GetColumnCount(int &count) const;
    int GetColumnName(int index, std::string &columnName) const;
    int GetColumnType(int index, int &columnType) const;
    int GetColumnBlob(int index, std::vector<uint8_t> &value) const;
    int GetColumnString(int index, std::string &value) const;
    int GetColumnLong(int index, int64_t &value) const;
    int GetColumnDouble(int index, double &value) const;
    bool IsReadOnly() const;
    int GetNumParameters(int &numParams) const;
    sqlite3_stmt *GetSql3Stmt() const
    {
        return stmtHandle;
    }

private:
    int InnerBindArguments(const std::vector<ValueObject> &bindArgs) const;
    std::string sql;
    sqlite3_stmt *stmtHandle;
    bool readOnly;
    int columnCount;
    int numParameters;
};

} // namespace NativeRdb
} // namespace OHOS
#endif