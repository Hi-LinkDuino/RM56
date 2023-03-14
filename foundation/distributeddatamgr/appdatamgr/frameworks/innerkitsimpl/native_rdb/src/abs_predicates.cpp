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

#include "abs_predicates.h"

#include <algorithm>
#include <initializer_list>

#include "logger.h"
#include "string_utils.h"

namespace OHOS {
namespace NativeRdb {
AbsPredicates::AbsPredicates()
{
    Initial();
}

AbsPredicates::~AbsPredicates()
{
}

void AbsPredicates::Clear()
{
    Initial();
}

bool AbsPredicates::IsNeedAnd() const
{
    return isNeedAnd;
}

/**
 * Restricts the value of the field to be greater than the specified value.
 */
AbsPredicates *AbsPredicates::EqualTo(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("equalTo", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: EqualTo() fails because Invalid parameter.");
        return this;
    }
    if (isNeedAnd) {
        whereClause += "AND ";
    } else {
        isNeedAnd = true;
    }
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " = ? ";
    whereArgs.push_back(value);
    return this;
}

/**
 * Restricts the value of the field to be unequal to the specified value.
 */
AbsPredicates *AbsPredicates::NotEqualTo(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("notEqualTo", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: NotEqualTo() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " <> ? ";
    whereArgs.push_back(value);
    return this;
}

AbsPredicates *AbsPredicates::BeginWrap()
{
    if (isNeedAnd) {
        whereClause += "AND ";
        isNeedAnd = false;
    }
    whereClause += " ( ";
    return this;
}

AbsPredicates *AbsPredicates::EndWrap()
{
    if (!isNeedAnd) {
        LOG_WARN("AbsPredicates.endGroup(): you cannot use function or() before end parenthesis,\
            start a AbsPredicates with endGroup(), or use endGroup() right after beginGroup().");
        return this;
    }
    whereClause += " ) ";
    return this;
}

AbsPredicates *AbsPredicates::Or()
{
    if (!isNeedAnd) {
        LOG_WARN("QueryImpl.or(): you are starting a sql request with predicate \"or\" or \
            using function or() immediately after another or(). that is ridiculous.");
        return this;
    }
    whereClause += " OR ";
    isNeedAnd = false;
    return this;
}

AbsPredicates *AbsPredicates::And()
{
    if (!isNeedAnd) {
        LOG_WARN("QueryImpl.and(): you should not start a request with \"and\" or use or() before this function.");
        return this;
    }
    return this;
}

/**
 * Restricts the value of the field to contain the specified string.
 */
AbsPredicates *AbsPredicates::Contains(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("contains", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: Contains() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " LIKE ? ";
    whereArgs.push_back("%" + value + "%");
    return this;
}

/**
 * Restricts the field to start with the specified string.
 */
AbsPredicates *AbsPredicates::BeginsWith(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("beginsWith", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: BeginsWith() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " LIKE ? ";
    whereArgs.push_back(value + "%");
    return this;
}

/**
 * Restricts the field to end with the specified string.
 */
AbsPredicates *AbsPredicates::EndsWith(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("endsWith", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: EndsWith() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " LIKE ? ";
    whereArgs.push_back("%" + value);
    return this;
}

/**
 * Restricts the value of the field to be null.
 */
AbsPredicates *AbsPredicates::IsNull(std::string field)
{
    bool chekParaFlag = CheckParameter("isNull", field, {});
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: IsNull() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " is null ";
    return this;
}

/**
 * estricts the value of the field not to be null.
 */
AbsPredicates *AbsPredicates::IsNotNull(std::string field)
{
    bool chekParaFlag = CheckParameter("isNotNull", field, {});
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: IsNotNull() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " is not null ";
    return this;
}

/**
 * Restricts the value of the field to have a pattern like field.
 */
AbsPredicates *AbsPredicates::Like(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("like", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: Like() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " LIKE ? ";
    whereArgs.push_back(value);
    return this;
}

/**
 * Configures to match the specified field whose data type is String and the value contains a wildcard.
 */
AbsPredicates *AbsPredicates::Glob(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("glob", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: Glob() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " GLOB ? ";
    whereArgs.push_back(value);
    return this;
}

/**
 * Restricts the value of the field to be unequal to the specified value.
 */
AbsPredicates *AbsPredicates::Between(std::string field, std::string low, std::string high)
{
    bool chekParaFlag = CheckParameter("between", field, { low, high });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: Between() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " BETWEEN ? AND ? ";
    whereArgs.push_back(low);
    whereArgs.push_back(high);
    return this;
}

/**
 * Configures to match the specified field whose data type is String and value is out of a given range.
 */
AbsPredicates *AbsPredicates::NotBetween(std::string field, std::string low, std::string high)
{
    bool chekParaFlag = CheckParameter("notBetween", field, { low, high });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: NotBetween() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " NOT BETWEEN ? AND ? ";
    whereArgs.push_back(low);
    whereArgs.push_back(high);
    return this;
}

/**
 * Restricts the value of the field to be greater than the specified value.
 */
AbsPredicates *AbsPredicates::GreaterThan(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("greaterThan", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: GreaterThan() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " > ? ";
    whereArgs.push_back(value);
    return this;
}

/**
 * Restricts the value of the field to be smaller than the specified value.
 */
AbsPredicates *AbsPredicates::LessThan(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("lessThan", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: LessThan() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " < ? ";
    whereArgs.push_back(value);
    return this;
}

/**
 * Restricts the value of the field to be greater than or equal to the specified value.
 */
AbsPredicates *AbsPredicates::GreaterThanOrEqualTo(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("greaterThanOrEqualTo", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: GreaterThanOrEqualTo() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " >= ? ";
    whereArgs.push_back(value);
    return this;
}

/**
 * Restricts the value of the field to be smaller than or equal to the specified value.
 */
AbsPredicates *AbsPredicates::LessThanOrEqualTo(std::string field, std::string value)
{
    bool chekParaFlag = CheckParameter("greaterThanOrEqualTo", field, { value });
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: LessThanOrEqualTo() fails because Invalid parameter.");
        return this;
    }
    CheckIsNeedAnd();
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + " <= ? ";
    whereArgs.push_back(value);
    return this;
}

/**
 * Restricts the ascending order of the return list. When there are several orders,
 * the one close to the head has the highest priority.
 */
AbsPredicates *AbsPredicates::OrderByAsc(std::string field)
{
    bool chekParaFlag = CheckParameter("orderByAsc", field, {});
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: OrderByAsc() fails because Invalid parameter.");
        return this;
    }
    if (isSorted) {
        order += ',';
    }
    std::string normalizedField = Normalized(RemoveQuotes(field));
    order = order + normalizedField + " ASC ";
    isSorted = true;
    return this;
}

/**
 * Restricts the descending order of the return list. When there are several orders,
 * the one close to the head has the highest priority.
 */
AbsPredicates *AbsPredicates::OrderByDesc(std::string field)
{
    bool chekParaFlag = CheckParameter("orderByDesc", field, {});
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: OrderByDesc() fails because Invalid parameter.");
        return this;
    }
    if (isSorted) {
        order += ',';
    }
    std::string normalizedField = Normalized(RemoveQuotes(field));
    order = order + normalizedField + " DESC ";
    isSorted = true;
    return this;
}

AbsPredicates *AbsPredicates::Distinct()
{
    distinct = true;
    return this;
}

/**
 * Restricts the max number of return records.
 */
AbsPredicates *AbsPredicates::Limit(int value)
{
    if (limit != -1) {
        LOG_WARN("AbsPredicates limit(): limit cannot be set twice.");
        return this;
    }
    if (value < 1) {
        LOG_WARN("AbsPredicates limit(): limit cannot be less than or equal to zero.");
        return this;
    }
    limit = value;
    return this;
}

/**
 * Configures to specify the start position of the returned result.
 */
AbsPredicates *AbsPredicates::Offset(int rowOffset)
{
    if (offset != -1) {
        LOG_WARN("AbsPredicates offset(): offset cannot be set twice.");
        return this;
    }
    if (rowOffset < 1) {
        LOG_WARN("AbsPredicates offset(): the value of offset can't be less than or equal to zero.");
        return this;
    }
    offset = rowOffset;
    return this;
}

/**
 * Configures {@code AbsPredicates} to group query results by specified columns.
 */
AbsPredicates *AbsPredicates::GroupBy(std::vector<std::string> fields)
{
    if (fields.empty()) {
        LOG_WARN("AbsPredicates: groupBy() fails because fields can't be null.");
        return this;
    }
    for (auto field : fields) {
        bool chekParaFlag = CheckParameter("GroupBy", field, {});
        if (!chekParaFlag) {
            LOG_WARN("AbsPredicates: GroupBy() fails because Invalid parameter.");
            return this;
        }
        std::string normalizedField = Normalized(RemoveQuotes(field));
        group = group + normalizedField + ",";
    }
    size_t pos = group.find_last_of(",");
    if (pos != group.npos) {
        group.erase(pos, 1);
    }
    return this;
}

/**
 * Configures {@code AbsPredicates} to specify the index column.
 */
AbsPredicates *AbsPredicates::IndexedBy(std::string indexName)
{
    bool chekParaFlag = CheckParameter("indexedBy", indexName, {});
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: IndexedBy() fails because Invalid parameter.");
        return this;
    }
    index = RemoveQuotes(indexName);
    return this;
}

/**
 * Configures to match the specified field whose data type is String array and values are within a given range.
 */
AbsPredicates *AbsPredicates::In(std::string field, std::vector<std::string> values)
{
    bool chekParaFlag = CheckParameter("in", field, {});
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: In() fails because Invalid parameter.");
        return this;
    }
    if (values.empty()) {
        LOG_WARN("AbsPredicates: in() fails because values can't be null.");
        return this;
    }

    CheckIsNeedAnd();

    std::vector<std::string> replaceValues;
    for (auto i : values) {
        replaceValues.push_back("?");
        whereArgs.push_back(i);
    }
    AppendWhereClauseWithInOrNotIn(" IN ", field, replaceValues);
    return this;
}

/**
 * Configures to match the specified field whose data type is String array and values are out of a given range.
 */
AbsPredicates *AbsPredicates::NotIn(std::string field, std::vector<std::string> values)
{
    bool chekParaFlag = CheckParameter("notIn", field, {});
    if (!chekParaFlag) {
        LOG_WARN("AbsPredicates: NotIn() fails because Invalid parameter.");
        return this;
    }
    if (values.empty()) {
        LOG_WARN("AbsPredicates: notIn() fails because values is null.");
        return this;
    }
    CheckIsNeedAnd();
    std::vector<std::string> replaceValues;
    for (auto i : values) {
        replaceValues.push_back("?");
        whereArgs.push_back(i);
    }
    AppendWhereClauseWithInOrNotIn(" NOT IN ", field, replaceValues);
    return this;
}

void AbsPredicates::Initial()
{
    distinct = false;
    isNeedAnd = false;
    isSorted = false;
    whereArgs.clear();
    whereClause.clear();
    order.clear();
    group.clear();
    index.clear();
    limit = -1;
    offset = -1;
}

/**
 * Check the parameter validity.
 */
bool AbsPredicates::CheckParameter(
    std::string methodName, std::string field, std::initializer_list<std::string> args) const
{
    if (field.empty()) {
        LOG_WARN("QueryImpl(): string 'field' is empty.");
        return false;
    }
    if (args.size() != 0) {
        for (auto i : args) {
            if (i.empty()) {
                LOG_WARN("QueryImpl(): value is empty.");
                return false;
            }
        }
    }
    return true;
}

std::string AbsPredicates::RemoveQuotes(std::string source) const
{
    if (source.empty()) {
        return source;
    }
    source.erase(std::remove(source.begin(), source.end(), '\''), source.end());
    source.erase(std::remove(source.begin(), source.end(), '\"'), source.end());
    source.erase(std::remove(source.begin(), source.end(), '`'), source.end());
    return source;
}

std::string AbsPredicates::Normalized(std::string source)
{
    // split contains className, attributeName
    const int splitSize = 2;
    if (source.empty()) {
        return source;
    }
    if (source.find(".") == source.npos) {
        return StringUtils::SurroundWithQuote(source, "`");
    }
    size_t pos = source.find(".");
    size_t left = 0;
    std::vector<std::string> split;
    while (pos != source.npos) {
        split.push_back(source.substr(left, pos - left));
        left = pos + 1;
        pos = source.find(".", left);
    }
    if (left < source.length()) {
        split.push_back(source.substr(left));
    }
    if (split.size() != splitSize) {
        return source;
    }
    source = StringUtils::SurroundWithQuote(split[0], "`") + "." + StringUtils::SurroundWithQuote(split[1], "`");
    return source;
}

void AbsPredicates::CheckIsNeedAnd()
{
    if (isNeedAnd) {
        whereClause += " AND ";
    } else {
        isNeedAnd = true;
    }
}

void AbsPredicates::AppendWhereClauseWithInOrNotIn(
    std::string methodName, std::string field, std::vector<std::string> replaceValues)
{
    std::string normalizedField = Normalized(RemoveQuotes(field));
    whereClause = whereClause + normalizedField + StringUtils::SurroundWithFunction(methodName, ",", replaceValues);
}

std::string AbsPredicates::GetWhereClause() const
{
    return whereClause;
}

void AbsPredicates::SetWhereClause(std::string whereClause)
{
    if (whereClause.empty()) {
        return;
    }
    this->whereClause = whereClause;
}

std::vector<std::string> AbsPredicates::GetWhereArgs() const
{
    return whereArgs;
}

void AbsPredicates::SetWhereArgs(std::vector<std::string> whereArgs)
{
    this->whereArgs = whereArgs;
}

std::string AbsPredicates::GetOrder() const
{
    return order;
}

void AbsPredicates::SetOrder(std::string order)
{
    if (order.empty()) {
        return;
    }
    this->order = order;
}

int AbsPredicates::GetLimit() const
{
    return limit;
}

int AbsPredicates::GetOffset() const
{
    return offset;
}

bool AbsPredicates::IsDistinct() const
{
    return distinct;
}

bool AbsPredicates::IsSorted() const
{
    return isSorted;
}

std::string AbsPredicates::GetGroup() const
{
    return group;
}

std::string AbsPredicates::GetIndex() const
{
    return index;
}
} // namespace NativeRdb
} // namespace OHOS