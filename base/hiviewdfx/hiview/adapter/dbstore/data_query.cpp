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

#include "data_query.h"

#include <string>

namespace OHOS {
namespace HiviewDFX {
int DataQuery::defLimit_ = 100;
DataQuery::DataQuery() : limit_(defLimit_), offset_(0) {}

DataQuery::~DataQuery() {}

DataQuery& DataQuery::Reset()
{
    sql_.str("");
    orderFields_.clear();
    limit_ = defLimit_;
    offset_ = 0;
    return *this;
}

DOCSTORE_API DataQuery& DataQuery::Select(std::vector<std::string> fields)
{
    selects_.insert(selects_.end(), fields.begin(), fields.end());
    return *this;
}

DataQuery& DataQuery::StartWith(const std::string &field, const std::string &value)
{
    sql_ << "/[" << field << " re \"" << value << "\"]";
    return *this;
}

DataQuery& DataQuery::NotStartWith(const std::string &field, const std::string &value)
{
    sql_ << "/[" << field << " not re \"" << value << ".*\"]";
    return *this;
}

DataQuery& DataQuery::And()
{
    sql_ << " and ";
    return *this;
}

DataQuery& DataQuery::Or()
{
    sql_ << " or ";
    return *this;
}

DataQuery& DataQuery::OrderByAsc(const std::string &field)
{
    orderFields_.emplace_back(std::pair<bool, std::string>(true, field));
    return *this;
}

DataQuery& DataQuery::OrderByDesc(const std::string &field)
{
    orderFields_.emplace_back(std::pair<bool, std::string>(false, field));
    return *this;
}

DataQuery& DataQuery::Limit(const int number, const int offset)
{
    limit_ = number;
    offset_ = offset;
    return *this;
}

DataQuery& DataQuery::BeginGroup()
{
    sql_ << "(";
    return *this;
}

DataQuery& DataQuery::EndGroup()
{
    sql_ << ")";
    return *this;
}

std::string DataQuery::ToString() const
{
    std::stringstream sql;
    sql << sql_.str();
    if (!selects_.empty()) {
        sql << " | ";
        sql << "/{";
        for (std::size_t index = 0; index < selects_.size(); index++) {
            if (index != (selects_.size() - 1)) {
                sql << selects_[index] << ", ";
            } else {
                sql << selects_[index];
            }
        }
        sql << "}";
    }
    sql << " |";
    if (!orderFields_.empty()) {
        sql << " ";
        for (std::size_t index = 0; index < orderFields_.size(); index++) {
            std::pair<bool, std::string> pair = orderFields_[index];
            std::string order = pair.first ? "asc" : "desc";
            sql << order << " /" << pair.second << " ";
        }
    }
    if (offset_ > 0) {
        sql << " skip " << offset_;
    }
    sql << " limit " << limit_;
    return sql.str();
}

std::string DataQuery::ToDelString(int limit) const
{
    if (limit <= 0) {
        return sql_.str() + " | del | count";
    } else {
        return sql_.str() + " | del | limit " + std::to_string(limit) + " count";
    }
}
} // HiviewDFX
} // OHOS