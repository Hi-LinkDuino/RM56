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

#include "rdb_predicates.h"
#include "string_utils.h"
#include "logger.h"

namespace OHOS {
namespace NativeRdb {
RdbPredicates::RdbPredicates(std::string tableName) : AbsRdbPredicates(tableName)
{
    InitialParam();
}

void RdbPredicates::Clear()
{
    AbsPredicates::Clear();
    InitialParam();
}

void RdbPredicates::InitialParam()
{
    joinTypes.clear();
    joinTableNames.clear();
    joinConditions.clear();
    joinCount = 0;
}

std::vector<std::string> RdbPredicates::GetJoinTypes()
{
    return joinTypes;
}

/**
 * Sets the join types in the predicates. The value can be {@code INNER JOIN}, {@code LEFT OUTER JOIN},
 * and {@code CROSS JOIN}.
 */
void RdbPredicates::SetJoinTypes(const std::vector<std::string> joinTypes)
{
    this->joinTypes = joinTypes;
}

/**
 * Obtains the database table names of the joins in the predicates.
 */
std::vector<std::string> RdbPredicates::GetJoinTableNames()
{
    return joinTableNames;
}
/**
 * Sets the database table names of the joins in the predicates.
 */
void RdbPredicates::SetJoinTableNames(const std::vector<std::string> joinTableNames)
{
    this->joinTableNames = joinTableNames;
}

/**
 * Obtains the join conditions in the predicates.
 */
std::vector<std::string> RdbPredicates::GetJoinConditions()
{
    return joinConditions;
}

/**
 * Sets the join conditions required in the predicates.
 */
void RdbPredicates::SetJoinConditions(const std::vector<std::string> joinConditions)
{
    this->joinConditions = joinConditions;
}
/**
 * Obtains the join clause in the predicates.
 */
std::string RdbPredicates::GetJoinClause()
{
    if (!joinTableNames.empty()) {
        return ProcessJoins();
    }
    return "";
}
/**
 * Obtains the number of joins in the predicates.
 */
int RdbPredicates::GetJoinCount() const
{
    return joinCount;
}
/**
 * Sets the number of joins in the predicates.
 */
void RdbPredicates::SetJoinCount(int joinCount)
{
    this->joinCount = joinCount;
}
/**
 * Adds a {@code cross join} condition to a SQL statement.
 */
RdbPredicates *RdbPredicates::CrossJoin(std::string tableName)
{
    return Join(JoinType::CROSS, tableName);
}
/**
 * Adds an {@code inner join} condition to a SQL statement.
 */
RdbPredicates *RdbPredicates::InnerJoin(std::string tableName)
{
    return Join(JoinType::INNER, tableName);
}
/**
 * Adds a {@code left outer join} condition to a SQL statement.
 */
RdbPredicates *RdbPredicates::LeftOuterJoin(std::string tableName)
{
    return Join(JoinType::LEFT, tableName);
}
/**
 * Adds a condition to a SQL statement.
 */
RdbPredicates *RdbPredicates::Join(int join, std::string tableName)
{
    if (tableName.empty()) {
        LOG_WARN("RdbPredicates join failed: table name is null or empty.");
        return this;
    }

    joinTypes.push_back(GetGrammar(join));
    joinTableNames.push_back(tableName);
    joinCount++;
    return this;
}

/**
 * Adds a {@code using} condition to the predicate. This method is similar to {@code using} of the SQL statement.
 */
RdbPredicates *RdbPredicates::Using(std::vector<std::string> fields)
{
    if (fields.size() == 0) {
        LOG_WARN("RdbPredicates Using failed : clauses is null.");
        return this;
    }
    if (joinCount <= 0) {
        LOG_WARN("No active join operation before using.");
        return this;
    }
    while (joinCount > 1) {
        joinConditions.push_back("");
        joinCount--;
    }
    joinCount--;
    joinConditions.push_back(StringUtils::SurroundWithFunction("USING", ",", fields));
    return this;
}
/**
 * Adds an {@code on} condition to the predicate.
 */
RdbPredicates *RdbPredicates::On(std::vector<std::string> clauses)
{
    if (clauses.size() == 0) {
        LOG_WARN("RdbPredicates on failed : clauses is null.");
        return this;
    }
    if (joinCount <= 0) {
        LOG_WARN("No active join operation before on.");
        return this;
    }
    while (joinCount > 1) {
        joinConditions.push_back("");
        joinCount--;
    }
    joinCount--;
    joinConditions.push_back(StringUtils::SurroundWithFunction("ON", "AND", clauses));
    return this;
}

std::string RdbPredicates::ProcessJoins()
{
    std::string builder = GetTableName();
    size_t size = joinTableNames.size();
    for (size_t i = 0; i < size; i++) {
        builder = builder + " " + joinTypes[i] + " " + joinTableNames[i];
        if (joinConditions[i] != "") {
            builder = builder + " " + joinConditions[i];
        }
    }
    return builder;
}

std::string RdbPredicates::GetGrammar(int type) const
{
    if (type == INNER) {
        return "INNER JOIN";
    }
    if (type == LEFT) {
        return "LEFT OUTER JOIN";
    }
    return "CROSS JOIN";
}
} // namespace NativeRdb
} // namespace OHOS