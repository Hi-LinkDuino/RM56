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

#ifndef NATIVE_RDB_SQLITE_SQL_BUILDER_H
#define NATIVE_RDB_SQLITE_SQL_BUILDER_H

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "rdb_store.h"
namespace OHOS {
namespace NativeRdb {

class SqliteSqlBuilder {
public:
    SqliteSqlBuilder();
    ~SqliteSqlBuilder();
    static std::string BuildDeleteString(const std::string &tableName, const std::string &index,
        const std::string &whereClause, const std::string &group, const std::string &order, int limit, int offset);
    static std::string BuildUpdateString(const ValuesBucket &values, const std::string &tableName,
        const std::vector<std::string> &whereArgs, const std::string &index, const std::string &whereClause,
        const std::string &group, const std::string &order, int limit, int offset, std::vector<ValueObject> &bindArgs,
        ConflictResolution conflictResolution);
    static std::string BuildUpdateStringOnlyWhere(const ValuesBucket &values, const std::string &tableName,
        const std::vector<std::string> &whereArgs, const std::string &index, const std::string &whereClause,
        const std::string &group, const std::string &order, int limit, int offset, std::vector<ValueObject> &bindArgs,
        ConflictResolution conflictResolution);
    static int BuildQueryString(bool distinct, const std::string &table, const std::vector<std::string> &columns,
        const std::string &where, const std::string &groupBy, const std::string &having, const std::string &orderBy,
        const std::string &limit, const std::string &offset, std::string &outSql);
    static std::string BuildQueryStringWithExpr(const std::string &tableName, bool distinct, const std::string &index,
        const std::string &whereClause, const std::string &group, const std::string &order, int limit, int offset,
        std::vector<std::string> &expr);
    static std::string BuildCountString(const std::string &tableName, const std::string &index,
        const std::string &whereClause, const std::string &group, const std::string &order, int limit, int offset);
    static std::string BuildSqlStringFromPredicates(const std::string &index, const std::string &whereClause,
        const std::string &group, const std::string &order, int limit, int offset);
    static std::string BuildSqlStringFromPredicatesNoWhere(const std::string &index, const std::string &whereClause,
        const std::string &group, const std::string &order, int limit, int offset);
    static std::string Normalize(const std::string &source, int &errorCode);

    static std::string BuildQueryString(const AbsRdbPredicates &predicates,
        const std::vector<std::string> &columns);
    static std::string BuildCountString(const AbsRdbPredicates &predicates);
    static std::string BuildSqlStringFromPredicates(const AbsRdbPredicates &predicates);

private:
    static void AppendClause(std::string &builder, const std::string &name, const std::string &clause);
    static void AppendColumns(std::string &builder, const std::vector<std::string> &columns, int &errorCode);
    static void AppendExpr(std::string &builder, std::vector<std::string> &exprs);
    static bool IsNotEmptyString(const std::string &str);
    static std::string NormalizeComplexPattern(const std::string &source, int &errorCode);
};

} // namespace NativeRdb
} // namespace OHOS
#endif
