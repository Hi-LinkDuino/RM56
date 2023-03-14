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

#include "sqlite_sql_builder.h"

#include <list>
#include <regex>

#include "logger.h"
#include "rdb_errno.h"
#include "string_utils.h"

namespace OHOS {
namespace NativeRdb {
std::vector<std::string> g_onConflictClause = {
    "", " OR ROLLBACK", " OR ABORT", " OR FAIL", " OR IGNORE", " OR REPLACE"
};
SqliteSqlBuilder::SqliteSqlBuilder() {}
SqliteSqlBuilder::~SqliteSqlBuilder() {}

/**
 * Build a delete SQL string using the given condition for SQLite.
 */
std::string SqliteSqlBuilder::BuildDeleteString(const std::string &tableName, const std::string &index,
    const std::string &whereClause, const std::string &group, const std::string &order, int limit, int offset)
{
    std::string sql;
    sql.append("Delete ").append("FROM ").append(tableName).append(
        BuildSqlStringFromPredicates(index, whereClause, group, order, limit, offset));
    return sql;
}

/**
 * Build a count SQL string using the given condition for SQLite.
 */
std::string SqliteSqlBuilder::BuildUpdateString(const ValuesBucket &values, const std::string &tableName,
    const std::vector<std::string> &whereArgs, const std::string &index, const std::string &whereClause,
    const std::string &group, const std::string &order, int limit, int offset, std::vector<ValueObject> &bindArgs,
    ConflictResolution conflictResolution)
{
    std::string sql;

    sql.append("UPDATE")
        .append(g_onConflictClause[static_cast<int>(conflictResolution)])
        .append(" ")
        .append(tableName)
        .append(" SET ");
    std::map<std::string, ValueObject> valuesMap;
    values.GetAll(valuesMap);
    for (auto iter = valuesMap.begin(); iter != valuesMap.end(); iter++) {
        sql.append((iter == valuesMap.begin()) ? "" : ",");
        sql.append(iter->first).append("=?");
        bindArgs.push_back(iter->second);
    }

    if (!whereArgs.empty()) {
        for (size_t i = 0; i < whereArgs.size(); i++) {
            bindArgs.push_back(ValueObject(whereArgs[i]));
        }
    }
    sql.append(BuildSqlStringFromPredicates(index, whereClause, group, order, limit, offset));
    return sql;
}

std::string SqliteSqlBuilder::BuildUpdateStringOnlyWhere(const ValuesBucket &values, const std::string &tableName,
    const std::vector<std::string> &whereArgs, const std::string &index, const std::string &whereClause,
    const std::string &group, const std::string &order, int limit, int offset, std::vector<ValueObject> &bindArgs,
    ConflictResolution conflictResolution)
{
    std::string sql;

    sql.append("UPDATE")
        .append(g_onConflictClause[static_cast<int>(conflictResolution)])
        .append(" ")
        .append(tableName)
        .append(" SET ");

    if (!whereArgs.empty()) {
        for (size_t i = 0; i < whereArgs.size(); i++) {
            bindArgs.push_back(ValueObject(whereArgs[i]));
        }
    }

    sql.append(BuildSqlStringFromPredicates(index, whereClause, group, order, limit, offset));
    return sql;
}

/**
 * Build a query SQL string using the given condition for SQLite.
 */
int SqliteSqlBuilder::BuildQueryString(bool distinct, const std::string &table, const std::vector<std::string> &columns,
    const std::string &where, const std::string &groupBy, const std::string &having, const std::string &orderBy,
    const std::string &limit, const std::string &offset, std::string &outSql)
{
    if (table.empty()) {
        return E_EMPTY_TABLE_NAME;
    }

    std::string sql;
    sql.append("SELECT ");
    if (distinct) {
        sql.append("DISTINCT ");
    }
    int errorCode = 0;
    if (columns.size() != 0) {
        AppendColumns(sql, columns, errorCode);
    } else {
        sql.append("* ");
    }
    int climit = std::stoi(limit);
    int coffset = std::stoi(offset);
    sql.append("FROM ").append(table).append(
        BuildSqlStringFromPredicates(having, where, groupBy, orderBy, climit, coffset));
    outSql = sql;

    return errorCode;
}

/**
 * Build a query SQL string using the given condition for SQLite.
 */
std::string SqliteSqlBuilder::BuildQueryStringWithExpr(const std::string &tableName, bool distinct,
    const std::string &index, const std::string &whereClause, const std::string &group, const std::string &order,
    int limit, int offset, std::vector<std::string> &expr)
{
    std::string sql;

    sql.append("SELECT ");
    if (distinct) {
        sql.append("DISTINCT ");
    }
    if (expr.size() != 0) {
        AppendExpr(sql, expr);
    } else {
        sql.append("* ");
    }
    sql.append("FROM ").append(tableName).append(
        BuildSqlStringFromPredicates(index, whereClause, group, order, limit, offset));

    return sql;
}

/**
 * Build a count SQL string using the given condition for SQLite.
 */
std::string SqliteSqlBuilder::BuildCountString(const std::string &tableName, const std::string &index,
    const std::string &whereClause, const std::string &group, const std::string &order, int limit, int offset)
{
    std::string sql;
    sql.append("SELECT COUNT(*) FROM ")
        .append(tableName)
        .append(BuildSqlStringFromPredicates(index, whereClause, group, order, limit, offset));
    return sql;
}

std::string SqliteSqlBuilder::BuildSqlStringFromPredicates(const std::string &index, const std::string &whereClause,
    const std::string &group, const std::string &order, int limit, int offset)
{
    std::string sqlString;

    std::string limitStr = (limit == -1) ? "" : std::to_string(limit);
    std::string offsetStr = (offset == -1) ? "" : std::to_string(offset);

    AppendClause(sqlString, " INDEXED BY ", index);
    AppendClause(sqlString, " WHERE ", whereClause);
    AppendClause(sqlString, " GROUP BY ", group);
    AppendClause(sqlString, " ORDER BY ", order);
    AppendClause(sqlString, " LIMIT ", limitStr);
    AppendClause(sqlString, " OFFSET ", offsetStr);

    return sqlString;
}

std::string SqliteSqlBuilder::BuildSqlStringFromPredicates(const AbsRdbPredicates &predicates)
{
    std::string sqlString;

    std::string limitStr = (predicates.GetLimit() == -1) ? "" : std::to_string(predicates.GetLimit());
    std::string offsetStr = (predicates.GetOffset() == -1) ? "" : std::to_string(predicates.GetOffset());

    AppendClause(sqlString, " INDEXED BY ", predicates.GetIndex());
    AppendClause(sqlString, " WHERE ", predicates.GetWhereClause());
    AppendClause(sqlString, " GROUP BY ", predicates.GetGroup());
    AppendClause(sqlString, " ORDER BY ", predicates.GetOrder());
    AppendClause(sqlString, " LIMIT ", limitStr);
    AppendClause(sqlString, " OFFSET ", offsetStr);

    return sqlString;
}

std::string SqliteSqlBuilder::BuildSqlStringFromPredicatesNoWhere(const std::string &index,
    const std::string &whereClause, const std::string &group, const std::string &order, int limit, int offset)
{
    std::string sqlString;
    std::string limitStr = (limit == -1) ? "" : std::to_string(limit);
    std::string offsetStr = (offset == -1) ? "" : std::to_string(offset);

    AppendClause(sqlString, " INDEXED BY ", index);
    AppendClause(sqlString, " ", whereClause);
    AppendClause(sqlString, " GROUP BY ", group);
    AppendClause(sqlString, " ORDER BY ", order);
    AppendClause(sqlString, " LIMIT ", limitStr);
    AppendClause(sqlString, " OFFSET ", offsetStr);

    return sqlString;
}

void SqliteSqlBuilder::AppendClause(std::string &builder, const std::string &name, const std::string &clause)
{
    if (clause.empty()) {
        return;
    }
    builder.append(name);
    builder.append(clause);
}

/**
 * Add the names that are non-null in columns to s, separating them with commas.
 */
void SqliteSqlBuilder::AppendColumns(std::string &builder, const std::vector<std::string> &columns, int &errorCode)
{
    size_t length = columns.size();
    for (size_t i = 0; i < length; i++) {
        std::string column = columns[i];

        if (column.size() != 0) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(Normalize(column, errorCode));
        }
    }

    builder += ' ';
}

void SqliteSqlBuilder::AppendExpr(std::string &builder, std::vector<std::string> &exprs)
{
    size_t length = exprs.size();

    for (size_t i = 0; i < length; i++) {
        std::string expr = exprs[i];

        if (expr.size() != 0) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(expr);
        }
    }

    builder += ' ';
}

bool SqliteSqlBuilder::IsNotEmptyString(const std::string &str)
{
    return (!str.empty());
}

std::string SqliteSqlBuilder::BuildQueryString(const AbsRdbPredicates &predicates,
    const std::vector<std::string> &columns)
{
    bool distinct = predicates.IsDistinct();
    std::string tableNameStr = predicates.GetTableName();
    std::string whereClauseStr = predicates.GetWhereClause();
    std::string groupStr = predicates.GetGroup();
    std::string indexStr = predicates.GetIndex();
    std::string orderStr = predicates.GetOrder();
    std::string limitStr = std::to_string(predicates.GetLimit());
    std::string offsetStr = std::to_string(predicates.GetOffset());
    std::string sqlStr;
    BuildQueryString(distinct, tableNameStr, columns, whereClauseStr, groupStr, indexStr, orderStr, limitStr,
        offsetStr, sqlStr);
    LOG_DEBUG("sqlStr:%{public}s", sqlStr.c_str());
    return sqlStr;
}

std::string SqliteSqlBuilder::BuildCountString(const AbsRdbPredicates &predicates)
{
    std::string tableName = predicates.GetTableName();
    return "SELECT COUNT(*) FROM " + tableName + BuildSqlStringFromPredicates(predicates);
}

std::string SqliteSqlBuilder::Normalize(const std::string &source, int &errorCode)
{
    if (StringUtils::IsEmpty(source)) {
        return "";
    }
    // Input source is like "address"
    std::regex pattern("^(\\w+)$");
    std::smatch result;
    auto wordMatcher = std::regex_match(source, result, pattern);
    if (wordMatcher) {
        return StringUtils::SurroundWithQuote(source, "`");
    }
    // Input source is like "*"
    std::string obj = "*";
    if (obj == source) {
        return "*";
    }

    // Input source is like "`address name`"
    std::regex apattern("^(['\"`])?([^`\"']+)\\1$");
    std::smatch aresult;
    auto matcher = std::regex_match(source, aresult, apattern);
    if (matcher) {
        return source;
    }
    return NormalizeComplexPattern(source, errorCode);
}

std::string SqliteSqlBuilder::NormalizeComplexPattern(const std::string &source, int &errorCode)
{
    // Remove quotations
    std::regex quotePattern("['\"`]");
    std::string replaceResult = regex_replace(source, quotePattern, "");
    replaceResult.erase(0, replaceResult.find_first_not_of(" "));
    replaceResult.erase(replaceResult.find_last_not_of(" ") + 1);
    std::regex pattern2("^(\\w+|\\*)\\s*([.]\\s*(\\w+|\\*))?\\s*(AS|as)?\\s*(\\w+)?$");
    std::smatch result;
    bool columnMatcher = std::regex_match(replaceResult, result, pattern2);
    if (!columnMatcher) {
        return StringUtils::SurroundWithQuote(replaceResult, "`");
    }

    // If group 3 is not empty, group 3 is column name.
    const int three = 3;
    // Group 5 is alias.
    const int five = 5;
    // If group 3 is empty, group 1 is column name, else group 1 is table name.
    std::string firstName = result[1];
    std::string lastName = result[three];
    std::string alias = result[five];
    if (StringUtils::IsEmpty(firstName)) {
        return StringUtils::SurroundWithQuote(replaceResult, "`");
    }
    std::string aresult(StringUtils::SurroundWithQuote(firstName, "`"));
    if (!StringUtils::IsEmpty(lastName)) {
        std::string obj = "*";
        if (obj == lastName) {
            // table.*
            aresult.append(".").append(lastName);
        } else {
            // table.column
            aresult.append(".").append(StringUtils::SurroundWithQuote(lastName, "`"));
        }
    }
    if (!StringUtils::IsEmpty(alias)) {
        std::string obj = "*";

        if (obj != lastName) {
            // (table.)column as alias
            aresult.append(" as ").append(StringUtils::SurroundWithQuote(alias, "`"));
        } else {
            // Illegal pattern: table.* as alias
            errorCode = E_SQLITE_SQL_BUILDER_NORMALIZE_FAIL;
            return aresult;
        }
    }
    return aresult;
}
} // namespace NativeRdb
} // namespace OHOS
