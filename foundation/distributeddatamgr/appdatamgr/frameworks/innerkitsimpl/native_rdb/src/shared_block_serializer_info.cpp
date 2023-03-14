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

#include "shared_block_serializer_info.h"
#include "logger.h"

namespace OHOS {
namespace NativeRdb {
SharedBlockSerializerInfo::SharedBlockSerializerInfo(AppDataFwk::SharedBlock *sharedBlock, int numColumns, int startPos)
    :sharedBlock_(sharedBlock), anumColumns(numColumns), atotalRows(0), astartPos(startPos), raddedRows(0),
    risFull(false)
{
}

SharedBlockSerializerInfo::~SharedBlockSerializerInfo() {}

int SharedBlockSerializerInfo::AddRow(int addedRows)
{
    // Allocate a new field directory for the row.
    int status = sharedBlock_->AllocRow();
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        risFull = true;
        return SQLITE_FULL;
    }

    raddedRows = addedRows + 1;
    return SQLITE_OK;
}

int SharedBlockSerializerInfo::Reset(int startPos)
{
    int status = sharedBlock_->Clear();
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        LOG_ERROR("reset : Failed in clearing, error=%{public}d", status);
        return SQLITE_ERROR;
    }
    status = sharedBlock_->SetColumnNum(anumColumns);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        return SQLITE_ERROR;
    }
    astartPos = startPos;
    raddedRows = 0;
    risFull = false;
    return SQLITE_OK;
}

int SharedBlockSerializerInfo::Finish(int addedRows, int totalRows)
{
    raddedRows = addedRows;
    atotalRows = totalRows;
    return SQLITE_OK;
}

int SharedBlockSerializerInfo::PutString(int row, int column, const char *text, int sizeIncludingNull)
{
    int status = sharedBlock_->PutString(row, column, text, sizeIncludingNull);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        sharedBlock_->FreeLastRow();
        risFull = true;
        return SQLITE_FULL;
    }

    return SQLITE_OK;
}

int SharedBlockSerializerInfo::PutLong(int row, int column, sqlite3_int64 value)
{
    int status = sharedBlock_->PutLong(row, column, value);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        sharedBlock_->FreeLastRow();
        risFull = true;
        return SQLITE_FULL;
    }

    return SQLITE_OK;
}

int SharedBlockSerializerInfo::PutDouble(int row, int column, double value)
{
    int status = sharedBlock_->PutDouble(row, column, value);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        sharedBlock_->FreeLastRow();
        risFull = true;
        return SQLITE_FULL;
    }

    return SQLITE_OK;
}

int SharedBlockSerializerInfo::PutBlob(int row, int column, const void *blob, int len)
{
    int status = sharedBlock_->PutBlob(row, column, blob, len);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        sharedBlock_->FreeLastRow();
        risFull = true;
        return SQLITE_FULL;
    }

    return SQLITE_OK;
}

int SharedBlockSerializerInfo::PutNull(int row, int column)
{
    int status = sharedBlock_->PutNull(row, column);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        sharedBlock_->FreeLastRow();
        risFull = true;
        LOG_ERROR("Failed allocating space for a null in column %{public}d, error=%{public}d", column, status);
        return SQLITE_FULL;
    }

    return SQLITE_OK;
}

int SharedBlockSerializerInfo::PutOther(int row, int column)
{
    sharedBlock_->FreeLastRow();
    return SQLITE_ERROR;
}

int SharedBlockSerializerInfo::GetTotalRows() const
{
    return atotalRows;
}

int SharedBlockSerializerInfo::GetAddedRows() const
{
    return raddedRows;
}

int SharedBlockSerializerInfo::GetStartPos() const
{
    return astartPos;
}
} // namespace NativeRdb
} // namespace OHOS