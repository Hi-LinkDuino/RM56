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


#ifndef NATIVE_RDB_ABSRDBPREDICATES_H
#define NATIVE_RDB_ABSRDBPREDICATES_H

#include "abs_predicates.h"
#include "rdb_types.h"

namespace OHOS::NativeRdb {
class AbsRdbPredicates : public AbsPredicates {
public:
    explicit AbsRdbPredicates(std::string tableName);
    
    ~AbsRdbPredicates() override {}
    
    std::string ToString() const;
    
    std::string GetTableName() const;
    
    AbsRdbPredicates *InDevices(std::vector<std::string>& devices);
    
    AbsRdbPredicates *InAllDevices();

    AbsRdbPredicates* EqualTo(std::string field, std::string value) override;
    AbsRdbPredicates* NotEqualTo(std::string field, std::string value) override;
    AbsRdbPredicates* And() override;
    AbsRdbPredicates* Or() override;
    AbsRdbPredicates* OrderByAsc(std::string field) override;
    AbsRdbPredicates* OrderByDesc(std::string field) override;

    const DistributedRdb::RdbPredicates& GetDistributedPredicates() const;

private:
    std::string tableName;
    mutable DistributedRdb::RdbPredicates predicates_;
};
} // namespace OHOS::NativeRdb

#endif