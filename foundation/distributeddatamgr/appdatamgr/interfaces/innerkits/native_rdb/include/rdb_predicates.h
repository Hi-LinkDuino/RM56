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

#ifndef NATIVE_RDB_RDBPREDICATES_H
#define NATIVE_RDB_RDBPREDICATES_H


#include "abs_rdb_predicates.h"

namespace OHOS {
namespace NativeRdb {
class RdbPredicates : public AbsRdbPredicates {
public:
    explicit RdbPredicates(std::string tableName);
    ~RdbPredicates() override {}
    void Clear() override;
    void InitialParam();
    std::vector<std::string> GetJoinTypes();
    void SetJoinTypes(const std::vector<std::string> joinTypes);
    std::vector<std::string> GetJoinTableNames();
    void SetJoinTableNames(const std::vector<std::string> joinTableNames);
    std::vector<std::string> GetJoinConditions();
    void SetJoinConditions(const std::vector<std::string> joinConditions);
    std::string GetJoinClause();
    int GetJoinCount() const;
    void SetJoinCount(int joinCount);
    RdbPredicates *CrossJoin(std::string tableName);
    RdbPredicates *InnerJoin(std::string tableName);
    RdbPredicates *LeftOuterJoin(std::string tableName);
    RdbPredicates *Using(std::vector<std::string> fields);
    RdbPredicates *On(std::vector<std::string> clauses);

private:
    std::vector<std::string> joinTypes;
    std::vector<std::string> joinTableNames;
    std::vector<std::string> joinConditions;
    int joinCount;
    std::string ProcessJoins();
    std::string GetGrammar(int type) const;
    RdbPredicates *Join(int join, std::string tableName);
};
} // namespace NativeRdb
} // namespace OHOS

#endif