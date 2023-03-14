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

#include "sqlite_statement.h"

#include <sstream>
#include <iomanip>

#include "logger.h"
#include "rdb_errno.h"
#include "sqlite_errno.h"

namespace OHOS {
namespace NativeRdb {
// Setting Data Precision
const int SET_DATA_PRECISION = 15;
SqliteStatement::SqliteStatement() : sql(""), stmtHandle(nullptr), readOnly(false), columnCount(0), numParameters(0)
{
}
SqliteStatement::~SqliteStatement()
{
    Finalize();
}

int SqliteStatement::Prepare(sqlite3 *dbHandle, const std::string &newSql)
{
    if (sql.compare(newSql) == 0) {
        return E_OK;
    }
    // prepare the new sqlite3_stmt
    sqlite3_stmt *stmt = nullptr;
    int errCode = sqlite3_prepare_v2(dbHandle, newSql.c_str(), newSql.length(), &stmt, nullptr);
    if (errCode != SQLITE_OK) {
        LOG_ERROR("SqliteStatement::Prepare failed err = %{public}d", errCode);
        if (stmt != nullptr) {
            sqlite3_finalize(stmt);
        }
        return SQLiteError::ErrNo(errCode);
    }
    Finalize(); // finalize the old
    sql = newSql;
    stmtHandle = stmt;
    readOnly = (sqlite3_stmt_readonly(stmtHandle) != 0) ? true : false;
    columnCount = sqlite3_column_count(stmtHandle);
    numParameters = sqlite3_bind_parameter_count(stmtHandle);
    return E_OK;
}

int SqliteStatement::Finalize()
{
    if (stmtHandle == nullptr) {
        return E_OK;
    }

    int errCode = sqlite3_finalize(stmtHandle);
    stmtHandle = nullptr;
    sql = "";
    readOnly = false;
    columnCount = 0;
    numParameters = 0;
    if (errCode != SQLITE_OK) {
        LOG_ERROR("SqliteStatement::Finalize failed err = %{public}d", errCode);
        return SQLiteError::ErrNo(errCode);
    }
    return E_OK;
}

int SqliteStatement::BindArguments(const std::vector<ValueObject> &bindArgs) const
{
    int count = static_cast<int>(bindArgs.size());
    std::vector<ValueObject> abindArgs;

    if (count == 0) {
        return E_OK;
    }
    // Obtains the bound parameter set.
    if ((numParameters != 0) && (count <= numParameters)) {
        for (const auto& i : bindArgs) {
            abindArgs.push_back(i);
        }

        for (int i = count; i < numParameters; i++) {
            ValueObject val;
            abindArgs.push_back(val);
        }
    }

    if (count > numParameters) {
        return E_INVALID_BIND_ARGS_COUNT;
    }

    return InnerBindArguments(abindArgs);
}

int SqliteStatement::InnerBindArguments(const std::vector<ValueObject> &bindArgs) const
{
    int index = 1;
    int errCode;
    for (auto arg : bindArgs) {
        switch (arg.GetType()) {
            case ValueObjectType::TYPE_NULL: {
                errCode = sqlite3_bind_null(stmtHandle, index);
                break;
            }
            case ValueObjectType::TYPE_INT: {
                int64_t value = 0;
                arg.GetLong(value);
                errCode = sqlite3_bind_int64(stmtHandle, index, value);
                break;
            }
            case ValueObjectType::TYPE_DOUBLE: {
                double doubleVal = 0;
                arg.GetDouble(doubleVal);
                errCode = sqlite3_bind_double(stmtHandle, index, doubleVal);
                break;
            }
            case ValueObjectType::TYPE_BLOB: {
                std::vector<uint8_t> blob;
                arg.GetBlob(blob);
                errCode = sqlite3_bind_blob(stmtHandle, index, static_cast<const void *>(blob.data()), blob.size(),
                    SQLITE_TRANSIENT);
                break;
            }
            case ValueObjectType::TYPE_BOOL: {
                bool boolVal = false;
                arg.GetBool(boolVal);
                errCode = sqlite3_bind_int64(stmtHandle, index, boolVal ? 1 : 0);
                break;
            }
            default: {
                std::string str;
                arg.GetString(str);
                errCode = sqlite3_bind_text(stmtHandle, index, str.c_str(), str.length(), SQLITE_TRANSIENT);
                break;
            }
        }

        if (errCode != SQLITE_OK) {
            return SQLiteError::ErrNo(errCode);
        }

        index++;
    }

    return E_OK;
}

int SqliteStatement::ResetStatementAndClearBindings() const
{
    if (stmtHandle == nullptr) {
        return E_OK;
    }

    int errCode = sqlite3_reset(stmtHandle);
    if (errCode != SQLITE_OK) {
        LOG_ERROR("Reset statement failed. %{public}d", errCode);
        return SQLiteError::ErrNo(errCode);
    }

    errCode = sqlite3_clear_bindings(stmtHandle);
    if (errCode != SQLITE_OK) {
        LOG_ERROR("Reset clear bindings failed. %{public}d", errCode);
        return SQLiteError::ErrNo(errCode);
    }

    return E_OK;
}

int SqliteStatement::Step() const
{
    int errCode = sqlite3_step(stmtHandle);
    return errCode;
}

int SqliteStatement::GetColumnCount(int &count) const
{
    if (stmtHandle == nullptr) {
        return E_INVALID_STATEMENT;
    }
    count = columnCount;
    return E_OK;
}

/**
 * Obtains the number that the statement has.
 */
int SqliteStatement::GetNumParameters(int &numParams) const
{
    if (stmtHandle == nullptr) {
        return E_INVALID_STATEMENT;
    }
    numParams = numParameters;
    return E_OK;
}

int SqliteStatement::GetColumnName(int index, std::string &columnName) const
{
    if (stmtHandle == nullptr) {
        return E_INVALID_STATEMENT;
    }

    if (index >= columnCount) {
        return E_INVALID_COLUMN_INDEX;
    }

    const char *name = sqlite3_column_name(stmtHandle, index);
    if (name == nullptr) {
        return E_ERROR;
    }
    columnName = std::string(name);
    return E_OK;
}

int SqliteStatement::GetColumnType(int index, int &columnType) const
{
    if (stmtHandle == nullptr) {
        return E_INVALID_STATEMENT;
    }

    if (index >= columnCount) {
        return E_INVALID_COLUMN_INDEX;
    }

    int type = sqlite3_column_type(stmtHandle, index);
    switch (type) {
        case SQLITE_INTEGER:
        case SQLITE_FLOAT:
        case SQLITE_BLOB:
        case SQLITE_NULL:
        case SQLITE_TEXT:
            columnType = type;
            return E_OK;
        default:
            return E_ERROR;
    }
}

int SqliteStatement::GetColumnBlob(int index, std::vector<uint8_t> &value) const
{
    if (stmtHandle == nullptr) {
        return E_INVALID_STATEMENT;
    }

    if (index >= columnCount) {
        return E_INVALID_COLUMN_INDEX;
    }

    int type = sqlite3_column_type(stmtHandle, index);
    if (type != SQLITE_BLOB && type != SQLITE_TEXT && type != SQLITE_NULL) {
        return E_INVALID_COLUMN_TYPE;
    }

    int size = sqlite3_column_bytes(stmtHandle, index);
    auto blob = static_cast<const uint8_t *>(sqlite3_column_blob(stmtHandle, index));
    if (size == 0 || blob == nullptr) {
        value.resize(0);
    } else {
        value.resize(size);
        value.assign(blob, blob + size);
    }

    return E_OK;
}

int SqliteStatement::GetColumnString(int index, std::string &value) const
{
    if (stmtHandle == nullptr) {
        return E_INVALID_STATEMENT;
    }

    if (index >= columnCount) {
        return E_INVALID_COLUMN_INDEX;
    }

    int type = sqlite3_column_type(stmtHandle, index);
    if (type == SQLITE_TEXT) {
        auto val = reinterpret_cast<const char *>(sqlite3_column_text(stmtHandle, index));
        value = (val == nullptr) ? "" : val;
    } else if (type == SQLITE_INTEGER) {
        int64_t val = sqlite3_column_int64(stmtHandle, index);
        value = std::to_string(val);
    } else if (type == SQLITE_FLOAT) {
        double val = sqlite3_column_double(stmtHandle, index);
        std::ostringstream os;
        if (os << std::setprecision(SET_DATA_PRECISION) << val)
            value = os.str();
    } else if (type == SQLITE_NULL) {
        value = "";
        return E_OK;
    } else if (type == SQLITE_BLOB) {
        return E_INVALID_COLUMN_TYPE;
    } else {
        return E_ERROR;
    }

    return E_OK;
}

int SqliteStatement::GetColumnLong(int index, int64_t &value) const
{
    if (stmtHandle == nullptr) {
        return E_INVALID_STATEMENT;
    }

    if (index >= columnCount) {
        return E_INVALID_COLUMN_INDEX;
    }
    char *errStr = nullptr;
    int type = sqlite3_column_type(stmtHandle, index);
    if (type == SQLITE_INTEGER) {
        value = sqlite3_column_int64(stmtHandle, index);
    } else if (type == SQLITE_TEXT) {
        auto val = reinterpret_cast<const char *>(sqlite3_column_text(stmtHandle, index));
        value = (val == nullptr) ? 0 : strtoll(val, &errStr, 0);
    } else if (type == SQLITE_FLOAT) {
        double val = sqlite3_column_double(stmtHandle, index);
        value = static_cast<int64_t>(val);
    } else if (type == SQLITE_NULL) {
        value = 0;
    } else if (type == SQLITE_BLOB) {
        return E_INVALID_COLUMN_TYPE;
    } else {
        return E_ERROR;
    }

    return E_OK;
}
int SqliteStatement::GetColumnDouble(int index, double &value) const
{
    if (stmtHandle == nullptr) {
        return E_INVALID_STATEMENT;
    }

    if (index >= columnCount) {
        return E_INVALID_COLUMN_INDEX;
    }
    char *ptr = nullptr;
    int type = sqlite3_column_type(stmtHandle, index);
    if (type == SQLITE_FLOAT) {
        value = sqlite3_column_double(stmtHandle, index);
    } else if (type == SQLITE_INTEGER) {
        int64_t val = sqlite3_column_int64(stmtHandle, index);
        value = static_cast<double>(val);
    } else if (type == SQLITE_TEXT) {
        auto val = reinterpret_cast<const char *>(sqlite3_column_text(stmtHandle, index));
        value = (val == nullptr) ? 0.0 : std::strtod(val, &ptr);
    } else if (type == SQLITE_NULL) {
        value = 0.0;
    } else if (type == SQLITE_BLOB) {
        return E_INVALID_COLUMN_TYPE;
    } else {
        return E_ERROR;
    }

    return E_OK;
}
bool SqliteStatement::IsReadOnly() const
{
    return readOnly;
}
} // namespace NativeRdb
} // namespace OHOS
