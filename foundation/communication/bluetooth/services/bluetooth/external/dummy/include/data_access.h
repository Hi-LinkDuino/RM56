/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef DATA_ACCESS_H
#define DATA_ACCESS_H

#include <memory>
#include <string>

namespace OHOS {
namespace bluetooth {
class IDataResult {
public:
    virtual ~IDataResult() = default;
    virtual bool Next()
    {
        return true;
    }
    virtual int GetColumnCount()
    {
        return 0;
    }
    virtual std::string GetColumnName(int index)
    {
        return "";
    }
    virtual bool IsNotNull(int index)
    {
        return true;
    }
    virtual int GetInt(int index)
    {
        return 0;
    }
    virtual int64_t GetInt64(int index)
    {
        return 0;
    }
    virtual std::string GetString(int index)
    {
        return "stub";
    }
};

class IDataStatement {
public:
    virtual ~IDataStatement() = default;
    virtual std::unique_ptr<IDataResult> Query()
    {
        return nullptr;
    }
    virtual int Update()
    {
        return 0;
    }
    virtual int Delete()
    {
        return 0;
    }
    virtual int Insert()
    {
        return 0;
    }
    virtual int ClearParams()
    {
        return 0;
    }
    virtual int SetParamInt64(int index, int64_t value)
    {
        return 0;
    }
    virtual int SetParamInt(int index, int value)
    {
        return 0;
    }
    virtual int SetParamString(int index, const std::string &value)
    {
        return 0;
    }
    virtual int SetParam16String(int index, const std::u16string &value)
    {
        return 0;
    }
};

class DataAccess {
public:
    static std::unique_ptr<DataAccess> GetConnection(
        const std::string &dbUri, bool readOnly = true, bool create = false)
    {
        return nullptr;
    }
    virtual ~DataAccess();
    std::unique_ptr<IDataStatement> CreateStatement(std::string sql)
    {
        return nullptr;
    }
    int64_t GetLastInsertRowid()
    {
        return 0;
    }
    bool BeginTransaction()
    {
        return true;
    }
    bool Commit()
    {
        return true;
    }
    bool Rollback()
    {
        return true;
    }
};
}  // namespace bluetooth
}  // namespace OHOS
#endif