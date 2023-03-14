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


#include "sqlite_shared_result_set.h"
#include <algorithm>
#include <memory>
#include <rdb_errno.h>
#include "logger.h"

namespace OHOS {
namespace NativeRdb {
SqliteSharedResultSet::SqliteSharedResultSet(std::shared_ptr<RdbStoreImpl> rdbSreImpl, std::string path,
    std::string sql, const std::vector<std::string> &bindArgs)
    : AbsSharedResultSet(path), resultSetBlockCapacity(0), isOnlyFillResultSetBlock(false), rdbStoreImpl(rdbSreImpl),
      qrySql(sql), selectionArgVec(bindArgs), rowNum(NO_COUNT)
{}

SqliteSharedResultSet::~SqliteSharedResultSet() {}

int SqliteSharedResultSet::GetAllColumnNames(std::vector<std::string> &columnNames)
{
    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }
    int columnCount = 0;
    // Get the total number of columns
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

    rdbStoreImpl->EndStepQuery();
    sqliteStatement = nullptr;

    return E_OK;
}

int SqliteSharedResultSet::GetRowCount(int &count)
{
    if (rowNum == NO_COUNT) {
        FillSharedBlock(0);
    }
    count = rowNum;
    return E_OK;
}

int SqliteSharedResultSet::Close()
{
    std::lock_guard<std::mutex> lock(sessionMutex);

    AbsSharedResultSet::Close();

    return E_OK;
}

/**
 * Get the rdb store that this result set is associated with.
 */
std::shared_ptr<RdbStore> SqliteSharedResultSet::GetRdbStore() const
{
    return rdbStoreImpl;
}

bool SqliteSharedResultSet::OnGo(int oldPosition, int newPosition)
{
    if (GetBlock() == nullptr) {
        FillSharedBlock(newPosition);
        return true;
    }
    if (newPosition >= (int)GetBlock()->GetRowNum()) {
        FillSharedBlock(newPosition);
    }
    return true;
}

/**
 * Calculate a proper start position to fill the block.
 */
int SqliteSharedResultSet::PickFillBlockStartPosition(int resultSetPosition, int blockCapacity) const
{
    return std::max(resultSetPosition - blockCapacity / PICK_POS, 0);
}

void SqliteSharedResultSet::FillSharedBlock(int requiredPos)
{
    ClearBlock();

    if (!HasBlock()) {
        LOG_ERROR("SqliteSharedResultSet::FillSharedBlock sharedBlock is null.");
        return;
    }

    std::vector<ValueObject> bindArgs;
    size_t size = selectionArgVec.size();

    for (size_t i = 0; i < size; i++) {
        ValueObject vauObj(selectionArgVec[i]);
        bindArgs.push_back(vauObj);
    }

    if (rowNum == NO_COUNT) {
        rdbStoreImpl->ExecuteForSharedBlock(rowNum, GetBlock(), requiredPos, requiredPos, true, qrySql, bindArgs);
        resultSetBlockCapacity = static_cast<int>(GetBlock()->GetRowNum());
    } else {
        int startPos =
            isOnlyFillResultSetBlock ? requiredPos : PickFillBlockStartPosition(requiredPos, resultSetBlockCapacity);
        rdbStoreImpl->ExecuteForSharedBlock(rowNum, GetBlock(), startPos, requiredPos, false, qrySql, bindArgs);
    }
}

void SqliteSharedResultSet::SetBlock(AppDataFwk::SharedBlock *block)
{
    AbsSharedResultSet::SetBlock(block);
    rowNum = NO_COUNT;
}

/**
 * If isOnlyFillResultSetBlockInput is true, use the input requiredPos to fill the block, otherwise pick the value
 * from requirePos and resultSetBlockCapacity.
 */
void SqliteSharedResultSet::SetFillBlockForwardOnly(bool isOnlyFillResultSetBlockInput)
{
    isOnlyFillResultSetBlock = isOnlyFillResultSetBlockInput;
}

void SqliteSharedResultSet::Finalize()
{
    if (!AbsSharedResultSet::IsClosed()) {
        if (rdbStoreImpl->IsOpen()) {
            std::string filePath = rdbStoreImpl->GetPath();
            std::string name = rdbStoreImpl->IsMemoryRdb() ? ":memory" : filePath;
        }
        Close();
    }
}

int SqliteSharedResultSet::PrepareStep()
{
    if (IsClosed()) {
        return E_STEP_RESULT_CLOSED;
    }
    if (sqliteStatement != nullptr) {
        return CheckSession();
    }
    int errCode;
    sqliteStatement = rdbStoreImpl->BeginStepQuery(errCode, qrySql, selectionArgVec);
    if (sqliteStatement == nullptr) {
        rdbStoreImpl->EndStepQuery();
        return errCode;
    }
    tid = std::this_thread::get_id();
    return E_OK;
}

int SqliteSharedResultSet::CheckSession()
{
    if (std::this_thread::get_id() != tid) {
        return E_STEP_RESULT_SET_CROSS_THREADS;
    }
    return E_OK;
}
} // namespace NativeRdb
} // namespace OHOS