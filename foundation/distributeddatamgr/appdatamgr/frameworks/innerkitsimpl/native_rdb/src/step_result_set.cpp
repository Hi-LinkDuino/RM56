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

#include "step_result_set.h"

#include <unistd.h>

#include "logger.h"
#include "rdb_errno.h"
#include "sqlite3sym.h"
#include "sqlite_errno.h"

namespace OHOS {
namespace NativeRdb {
StepResultSet::StepResultSet(
    std::shared_ptr<RdbStoreImpl> rdb, const std::string &sql, const std::vector<std::string> &selectionArgs)
    : rdb(rdb), sql(sql), selectionArgs(selectionArgs), isAfterLast(false), rowCount(INIT_POS),
      sqliteStatement(nullptr)
{
}

StepResultSet::~StepResultSet()
{
    Close();
}

int StepResultSet::GetAllColumnNames(std::vector<std::string> &columnNames)
{
    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    int columnCount = 0;
    errCode = sqliteStatement->GetColumnCount(columnCount);
    if (errCode) {
        return errCode;
    }

    columnNames.clear();
    for (int i = 0; i < columnCount; i++) {
        std::string columnName;
        errCode = sqliteStatement->GetColumnName(i, columnName);
        if (errCode) {
            columnNames.clear();
            return errCode;
        }
        columnNames.push_back(columnName);
    }

    return E_OK;
}

int StepResultSet::GetColumnType(int columnIndex, ColumnType &columnType)
{
    if (rowPos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    int sqliteType;
    int errCode = sqliteStatement->GetColumnType(columnIndex, sqliteType);
    if (errCode) {
        return errCode;
    }

    switch (sqliteType) {
        case SQLITE_INTEGER:
            columnType = ColumnType::TYPE_INTEGER;
            break;
        case SQLITE_FLOAT:
            columnType = ColumnType::TYPE_FLOAT;
            break;
        case SQLITE_BLOB:
            columnType = ColumnType::TYPE_BLOB;
            break;
        case SQLITE_NULL:
            columnType = ColumnType::TYPE_NULL;
            break;
        default:
            columnType = ColumnType::TYPE_STRING;
    }

    return E_OK;
}

int StepResultSet::GetRowCount(int &count)
{
    if (rowCount != INIT_POS) {
        count = rowCount;
        return E_OK;
    }
    int oldPosition = 0;
    // Get the start position of the query result
    GetRowIndex(oldPosition);

    while (GoToNextRow() == E_OK) {
    }
    count = rowCount;
    // Reset the start position of the query result
    GoToRow(oldPosition);

    return E_OK;
}

/**
 * Moves the result set to a specified position
 */
int StepResultSet::GoToRow(int position)
{
    if (!rdb) {
        return E_ERROR;
    }
    // If the moved position is less than zero, reset the result and return an error
    if (position < 0) {
        Reset();
        return E_ERROR;
    }
    if (position == rowPos) {
        return E_OK;
    }
    if (position < rowPos) {
        Reset();
        return GoToRow(position);
    }
    while (position != rowPos) {
        int errCode = GoToNextRow();
        if (errCode) {
            return errCode;
        }
    }

    return E_OK;
}

/**
 * Move the result set to the next row
 */
int StepResultSet::GoToNextRow()
{
    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    int retryCount = 0;
    errCode = sqliteStatement->Step();

    while (errCode == SQLITE_LOCKED || errCode == SQLITE_BUSY) {
        // The table is locked, retry
        if (retryCount > STEP_QUERY_RETRY_MAX_TIMES) {
            LOG_ERROR("StepResultSet::GoToNextRow retrycount exceeded");
            return E_STEP_RESULT_QUERY_EXCEEDED;
        } else {
            // Sleep to give the thread holding the lock a chance to finish
            usleep(STEP_QUERY_RETRY_INTERVAL);
            errCode = sqliteStatement->Step();
            retryCount++;
        }
    }

    if (errCode == SQLITE_ROW) {
        rowPos++;
        return E_OK;
    } else if (errCode == SQLITE_DONE) {
        isAfterLast = true;
        rowCount = rowPos + 1;
        FinishStep();
        return E_STEP_RESULT_IS_AFTER_LAST;
    } else {
        LOG_ERROR("StepResultSet::GoToNextRow step err = %{public}d", errCode);
        FinishStep();
        return SQLiteError::ErrNo(errCode);
    }
}

/**
 * Checks whether the result set is positioned after the last row
 */
int StepResultSet::IsEnded(bool &result)
{
    result = isAfterLast;
    return E_OK;
}

/**
 * Checks whether the result set is moved
 */
int StepResultSet::IsStarted(bool &result) const
{
    result = (rowPos != INIT_POS);
    return E_OK;
}

/**
 * Check whether the result set is in the first row
 */
int StepResultSet::IsAtFirstRow(bool &result) const
{
    result = (rowPos == 0);
    return E_OK;
}

int StepResultSet::GetBlob(int columnIndex, std::vector<uint8_t> &blob)
{
    if (rowPos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    return sqliteStatement->GetColumnBlob(columnIndex, blob);
}

int StepResultSet::GetString(int columnIndex, std::string &value)
{
    if (rowPos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    int errCode = sqliteStatement->GetColumnString(columnIndex, value);
    if (errCode != E_OK) {
        LOG_ERROR("StepResultSet::GetString is err=%{public}d", errCode);
        return errCode;
    }
    return E_OK;
}

int StepResultSet::GetInt(int columnIndex, int &value)
{
    if (rowPos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    int64_t columnValue;
    int errCode = sqliteStatement->GetColumnLong(columnIndex, columnValue);
    if (errCode != E_OK) {
        return errCode;
    }
    value = static_cast<int>(columnValue);
    return E_OK;
}

int StepResultSet::GetLong(int columnIndex, int64_t &value)
{
    if (rowPos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    int errCode = sqliteStatement->GetColumnLong(columnIndex, value);
    if (errCode != E_OK) {
        return errCode;
    }
    return E_OK;
}

int StepResultSet::GetDouble(int columnIndex, double &value)
{
    if (rowPos == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    int errCode = sqliteStatement->GetColumnDouble(columnIndex, value);
    if (errCode != E_OK) {
        return errCode;
    }
    return E_OK;
}

int StepResultSet::IsColumnNull(int columnIndex, bool &isNull)
{
    ColumnType columnType;
    int errCode = GetColumnType(columnIndex, columnType);
    if (errCode != E_OK) {
        return errCode;
    }
    isNull = (columnType == ColumnType::TYPE_NULL);
    return E_OK;
}

/**
 * Check whether the result set is over
 */
bool StepResultSet::IsClosed() const
{
    return isClosed;
}

int StepResultSet::Close()
{
    if (isClosed) {
        return E_OK;
    }
    isClosed = true;
    int errCode = FinishStep();
    rdb = nullptr;
    return errCode;
}

int StepResultSet::CheckSession()
{
    if (std::this_thread::get_id() != tid) {
        LOG_ERROR("StepResultSet is passed cross threads!");
        return E_STEP_RESULT_SET_CROSS_THREADS;
    }
    return E_OK;
}

/**
 * Obtain session and prepare precompile statement for step query
 */
int StepResultSet::PrepareStep()
{
    if (isClosed) {
        return E_STEP_RESULT_CLOSED;
    }

    if (sqliteStatement != nullptr) {
        return CheckSession();
    }

    int errCode;
    sqliteStatement = rdb->BeginStepQuery(errCode, sql, selectionArgs);
    if (sqliteStatement == nullptr) {
        rdb->EndStepQuery();
        return errCode;
    }

    tid = std::this_thread::get_id();
    return E_OK;
}

/**
 * Release resource of step result set, this method can be called more than once
 */
int StepResultSet::FinishStep()
{
    int errCode = CheckSession();
    if (errCode != E_OK) {
        return errCode;
    }

    if (sqliteStatement == nullptr) {
        return E_OK;
    }

    sqliteStatement = nullptr;
    rowPos = INIT_POS;
    if (rdb != nullptr) {
        errCode = rdb->EndStepQuery();
    }
    if (errCode != E_OK) {
        LOG_ERROR("StepResultSet::FinishStep err = %{public}d", errCode);
    }
    return errCode;
}

/**
 * Reset the statement
 */
void StepResultSet::Reset()
{
    if (sqliteStatement != nullptr) {
        sqlite3_reset(sqliteStatement->GetSql3Stmt());
    }
    rowPos = INIT_POS;
    isAfterLast = false;
}
} // namespace NativeRdb
} // namespace OHOS
