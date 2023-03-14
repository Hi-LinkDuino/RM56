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

#ifndef HIVIEW_ADAPTER_DBSTORE_INCLUDE_DATA_QUERY_H
#define HIVIEW_ADAPTER_DBSTORE_INCLUDE_DATA_QUERY_H

#include <string>
#include <vector>
#include <sstream>

#include "visibility.h"

namespace OHOS {
namespace HiviewDFX {
class DataQuery {
public:
    DOCSTORE_API DataQuery();

    DOCSTORE_API ~DataQuery();

    DOCSTORE_API DataQuery& Reset();

    DOCSTORE_API DataQuery& Select(std::vector<std::string> fields);

    template<typename T>
    DOCSTORE_API DataQuery& EqualTo(const std::string &field, const T &value)
    {
        sql_ << "(/[" << field << "=";
        AppendValue(sql_, value) << "])";
        return *this;
    }

    template<typename T>
    DOCSTORE_API DataQuery& NotEqualTo(const std::string &field, const T &value)
    {
        sql_ << "(/[" << field << "!=";
        AppendValue(sql_, value) << "])";
        return *this;
    }

    template<typename T>
    DOCSTORE_API DataQuery& GreaterThan(const std::string &field, const T &value)
    {
        sql_ << "(/[" << field << ">";
        AppendValue(sql_, value) << "])";
        return *this;
    }

    template<typename T>
    DOCSTORE_API  DataQuery& LessThan(const std::string &field, const T &value)
    {
        sql_ << "(/[" << field << "<";
        AppendValue(sql_, value) << "])";
        return *this;
    }

    template<typename T>
    DOCSTORE_API DataQuery& GreaterThanOrEqualTo(const std::string &field, const T &value)
    {
        sql_ << "(/[" << field << ">=";
        AppendValue(sql_, value) << "])";
        return *this;
    }

    template<typename T>
    DOCSTORE_API  DataQuery& LessThanOrEqualTo(const std::string &field, const T &value)
    {
        sql_ << "(/[" << field << "<=";
        AppendValue(sql_, value) << "])";
        return *this;
    }

    template<typename T>
    DOCSTORE_API  DataQuery& In(const std::string &field, const std::vector<T> &valueList)
    {
        sql_ << "/[" << field << " in [";
        for (int index = 0; index < valueList.size(); index++) {
            if (index != (valueList.size() - 1)) {
                AppendValue(sql_, valueList[index]) << ", ";
            }
            else {
                AppendValue(sql_, valueList[index]);
            }
        }
        sql_ << "]]";
        return *this;
    }

    template<typename T>
    DOCSTORE_API  DataQuery& NotIn(const std::string &field, const std::vector<T> &valueList)
    {
        sql_ << "/[" << field << " not in [";
        for (int index = 0; index < valueList.size(); index++) {
            if (index != (valueList.size() - 1)) {
                AppendValue(sql_, valueList[index]) << ", ";
            }
            else {
                AppendValue(sql_, valueList[index]);
            }
        }
        sql_ << "]]";
        return *this;
    }

    DOCSTORE_API DataQuery& StartWith(const std::string &field, const std::string &value);

    DOCSTORE_API DataQuery& NotStartWith(const std::string &field, const std::string &value);

    DOCSTORE_API DataQuery& And();

    DOCSTORE_API DataQuery& Or();

    DOCSTORE_API DataQuery& OrderByAsc(const std::string &field);

    DOCSTORE_API DataQuery& OrderByDesc(const std::string &field);

    DOCSTORE_API DataQuery& Limit(const int number, const int offset = 0);

    DOCSTORE_API DataQuery& BeginGroup();

    DOCSTORE_API DataQuery& EndGroup();

    DOCSTORE_API std::string ToString() const;

private:
    template<typename T> static std::stringstream& AppendValue(std::stringstream &sql, const T &value)
    {
        sql << value;
        return sql;
    }

    static std::stringstream& AppendValue(std::stringstream &sql, const std::string &value)
    {
        sql << "\"" << value << "\"";
        return sql;
    }

    static std::stringstream& AppendValue(std::stringstream &sql, const char* value)
    {
        sql << "\"" << value << "\"";
        return sql;
    }

    static std::stringstream& AppendValue(std::stringstream &sql, char* value)
    {
        sql << "\"" << value << "\"";
        return sql;
    }

    std::string ToDelString(int limit = 0) const;
private:
    static int defLimit_;
    friend class DocStore;
    std::stringstream sql_;
    std::vector<std::string> selects_;
    std::vector<std::pair<bool, std::string>> orderFields_;
    int limit_;
    int offset_;
}; // DataQuery
} // HiviewDFX
} // OHOS
#endif // HIVIEW_ADAPTER_DBSTORE_INCLUDE_DATA_QUERY_H