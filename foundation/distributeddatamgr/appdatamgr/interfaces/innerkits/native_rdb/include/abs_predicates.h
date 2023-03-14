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

#ifndef NATIVE_RDB_ABSPREDICATES_H
#define NATIVE_RDB_ABSPREDICATES_H

#include <string>
#include <vector>

namespace OHOS {
namespace NativeRdb {
class AbsPredicates {
public:
    AbsPredicates();
    virtual ~AbsPredicates();

    enum JoinType {
        INNER,
        LEFT,
        CROSS
    };

    std::string GetWhereClause() const;
    void SetWhereClause(std::string whereClause);
    std::vector<std::string> GetWhereArgs() const;
    void SetWhereArgs(std::vector<std::string> whereArgs);
    std::string GetOrder() const;
    void SetOrder(std::string order);
    int GetLimit() const;
    int GetOffset() const;
    bool IsDistinct() const;
    std::string GetGroup() const;
    std::string GetIndex() const;
    bool IsNeedAnd() const;
    bool IsSorted() const;

public:
    virtual void Clear();
    virtual AbsPredicates *EqualTo(std::string field, std::string value);
    virtual AbsPredicates *NotEqualTo(std::string field, std::string value);
    virtual AbsPredicates *BeginWrap();
    virtual AbsPredicates *EndWrap();
    virtual AbsPredicates *Or();
    virtual AbsPredicates *And();
    virtual AbsPredicates *Contains(std::string field, std::string value);
    virtual AbsPredicates *BeginsWith(std::string field, std::string value);
    virtual AbsPredicates *EndsWith(std::string field, std::string value);
    virtual AbsPredicates *IsNull(std::string field);
    virtual AbsPredicates *IsNotNull(std::string field);
    virtual AbsPredicates *Like(std::string field, std::string value);
    virtual AbsPredicates *Glob(std::string field, std::string value);
    virtual AbsPredicates *Between(std::string field, std::string low, std::string high);
    virtual AbsPredicates *NotBetween(std::string field, std::string low, std::string high);
    virtual AbsPredicates *GreaterThan(std::string field, std::string value);
    virtual AbsPredicates *LessThan(std::string field, std::string value);
    virtual AbsPredicates *GreaterThanOrEqualTo(std::string field, std::string value);
    virtual AbsPredicates *LessThanOrEqualTo(std::string field, std::string value);
    virtual AbsPredicates *OrderByAsc(std::string field);
    virtual AbsPredicates *OrderByDesc(std::string field);
    virtual AbsPredicates *Distinct();
    virtual AbsPredicates *Limit(int value);
    virtual AbsPredicates *Offset(int rowOffset);
    virtual AbsPredicates *GroupBy(std::vector<std::string> fields);
    virtual AbsPredicates *IndexedBy(std::string indexName);
    virtual AbsPredicates *In(std::string field, std::vector<std::string> values);
    virtual AbsPredicates *NotIn(std::string field, std::vector<std::string> values);

private:
    std::string whereClause;
    std::vector<std::string> whereArgs;
    std::string order;
    std::string group;
    std::string index;
    int limit;
    int offset;
    bool distinct;
    bool isNeedAnd;
    bool isSorted;

    void Initial();
    bool CheckParameter(std::string methodName, std::string field, std::initializer_list<std::string> args) const;
    std::string RemoveQuotes(std::string source) const;
    std::string Normalized(std::string source);
    void CheckIsNeedAnd();
    void AppendWhereClauseWithInOrNotIn(std::string methodName, std::string field,
        std::vector<std::string> replaceValues);
};
} // namespace NativeRdb
} // namespace OHOS

#endif