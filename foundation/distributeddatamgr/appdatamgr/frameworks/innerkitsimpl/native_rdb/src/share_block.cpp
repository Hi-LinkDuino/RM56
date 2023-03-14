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
#include "share_block.h"
#include <unistd.h>
#include "logger.h"
#include "shared_block_serializer_info.h"

namespace OHOS {
namespace NativeRdb {
const int ERROR_STATUS = -1;
const unsigned int SLEEP_TIME = 1000;
// move to the highest 32 bits of 64 bits number
const int RETRY_TIME = 50;
const unsigned int OFFSET = 32;

int SeriAddRow(void *pCtx, int addedRows)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->AddRow(addedRows);
}

int SeriReset(void *pCtx, int startPos)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->Reset(startPos);
}

int SeriFinish(void *pCtx, int addedRows, int totalRows)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->Finish(addedRows, totalRows);
}

int SeriPutString(void *pCtx, int addedRows, int column, const char *text, int size)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->PutString(addedRows, column, text, size);
}

int SeriPutLong(void *pCtx, int addedRows, int column, sqlite3_int64 value)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->PutLong(addedRows, column, value);
}

int SeriPutDouble(void *pCtx, int addedRows, int column, double value)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->PutDouble(addedRows, column, value);
}

int SeriPutBlob(void *pCtx, int addedRows, int column, const void *blob, int len)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->PutBlob(addedRows, column, blob, len);
}

int SeriPutNull(void *pCtx, int addedRows, int column)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->PutNull(addedRows, column);
}

int SeriPutOther(void *pCtx, int addedRows, int column)
{
    auto *serializer = static_cast<SharedBlockSerializerInfo *>(pCtx);
    return serializer->PutOther(addedRows, column);
}

int ClearSharedBlock(AppDataFwk::SharedBlock *sharedBlock)
{
    int status = sharedBlock->Clear();
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        return ERROR_STATUS;
    }
    return status;
}

int SharedBlockSetColumnNum(AppDataFwk::SharedBlock *sharedBlock, int columnNum)
{
    int status = sharedBlock->SetColumnNum(columnNum);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        return ERROR_STATUS;
    }
    return status;
}

void FillSharedBlockOpt(SharedBlockInfo *info)
{
    SharedBlockSerializerInfo serializer(info->sharedBlock, info->numColumns, info->startPos);
    Sqlite3SharedBlockMethods sqliteSharedBlock = {
        1,          &serializer,   info->isCountAllRows, info->startPos, info->requiredPos, SeriAddRow,  SeriReset,
        SeriFinish, SeriPutString, SeriPutLong,          SeriPutDouble,  SeriPutBlob,       SeriPutNull, SeriPutOther
    };

    int rc =
        sqlite3_db_config(info->connection->db, SQLITE_DBCONFIG_SET_SHAREDBLOCK, info->statement, &sqliteSharedBlock);
    if (rc != SQLITE_OK) {
        LOG_ERROR("set sqlite shared block methods error. rc=%{private}d", rc);
        return;
    }
    int retryCount = 0;
    bool gotException = false;
    while (!gotException) {
        int err = sqlite3_step(info->statement);
        if (err == SQLITE_DONE) {
            LOG_ERROR("Processed all rows after optimization.");
            break;
        } else if (err == SQLITE_LOCKED || err == SQLITE_BUSY) {
            LOG_ERROR("Database locked, retrying");
            if (retryCount > RETRY_TIME) {
                gotException = true;
            } else {
                usleep(SLEEP_TIME);
                retryCount++;
            }
        } else {
            gotException = true;
        }
    }
    info->totalRows = serializer.GetTotalRows();
    info->startPos = serializer.GetStartPos();
    info->addedRows = serializer.GetAddedRows();

    rc = sqlite3_db_config(info->connection->db, SQLITE_DBCONFIG_SET_SHAREDBLOCK, info->statement, 0);
    if (rc != SQLITE_OK) {
        LOG_ERROR("clear sqlite shared block methods error. rc=%{private}d", rc);
        return;
    }
}

void FillSharedBlock(SharedBlockInfo *info)
{
    int retryCount = 0;
    info->totalRows = info->addedRows = 0;
    bool isFull = false;
    bool hasException = false;
    while (!hasException && (!isFull || info->isCountAllRows)) {
        int err = sqlite3_step(info->statement);
        if (err == SQLITE_ROW) {
            retryCount = 0;
            info->totalRows += 1;
            if (info->startPos >= info->totalRows || isFull) {
                continue;
            }
            FillRow(info);
            isFull = info->isFull;
            hasException = info->hasException;
        } else if (err == SQLITE_DONE) {
            LOG_ERROR("Processed all rows");
            break;
        } else if (err == SQLITE_LOCKED || err == SQLITE_BUSY) {
            LOG_ERROR("Database locked, retrying");
            if (retryCount > RETRY_TIME) {
                LOG_ERROR("Bailing on database busy retry");
                hasException = true;
            } else {
                usleep(SLEEP_TIME);
                retryCount++;
            }
        } else {
            hasException = true;
        }
    }
}

void FillRow(SharedBlockInfo *info)
{
    FillOneRowResult fillOneRowResult =
        FillOneRow(info->sharedBlock, info->statement, info->columnNum, info->startPos, info->addedRows);
    if (fillOneRowResult == SHARED_BLOCK_IS_FULL && info->addedRows &&
        info->startPos + info->addedRows <= info->requiredPos) {
        info->sharedBlock->Clear();
        info->sharedBlock->SetColumnNum(info->columnNum);
        info->startPos += info->addedRows;
        info->addedRows = 0;
        fillOneRowResult =
            FillOneRow(info->sharedBlock, info->statement, info->columnNum, info->startPos, info->addedRows);
    }

    if (fillOneRowResult == FILL_ONE_ROW_SUCESS) {
        info->addedRows += 1;
    } else if (fillOneRowResult == SHARED_BLOCK_IS_FULL) {
        info->isFull = true;
    } else {
        info->hasException = true;
    }
}

FillOneRowResult FillOneRow(AppDataFwk::SharedBlock *sharedBlock, sqlite3_stmt *statement, int numColumns,
    int startPos, int addedRows)
{
    int status = sharedBlock->AllocRow();
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        LOG_ERROR("Failed allocating fieldDir at startPos %{public}d row %{public}d, error=%{public}d", startPos,
            addedRows, status);
        return SHARED_BLOCK_IS_FULL;
    }

    FillOneRowResult result = FILL_ONE_ROW_SUCESS;
    for (int i = 0; i < numColumns; i++) {
        int type = sqlite3_column_type(statement, i);
        if (type == SQLITE_TEXT) {
            // TEXT data
            result = FillOneRowOfString(sharedBlock, statement, startPos, addedRows, i);
        } else if (type == SQLITE_INTEGER) {
            // INTEGER data
            result = FillOneRowOfLong(sharedBlock, statement, startPos, addedRows, i);
        } else if (type == SQLITE_FLOAT) {
            // FLOAT data
            result = FillOneRowOfFloat(sharedBlock, statement, startPos, addedRows, i);
        } else if (type == SQLITE_BLOB) {
            // BLOB data
            result = FillOneRowOfBlob(sharedBlock, statement, startPos, addedRows, i);
        } else if (type == SQLITE_NULL) {
            // NULL field
            result = FillOneRowOfNull(sharedBlock, statement, startPos, addedRows, i);
        } else {
            // Unknown data
            LOG_ERROR("Unknown column type when filling database shared block.");
            result = FILL_ONE_ROW_FAIL;
            break;
        }

        if (result == SHARED_BLOCK_IS_FULL) {
            break;
        }
    }

    // Free the last row if if was not successfully copied.
    if (result != FILL_ONE_ROW_SUCESS) {
        sharedBlock->FreeLastRow();
    }
    return result;
}


FillOneRowResult FillOneRowOfString(AppDataFwk::SharedBlock *sharedBlock, sqlite3_stmt *statement, int startPos,
    int addedRows, int pos)
{
    const char *text = reinterpret_cast<const char *>(sqlite3_column_text(statement, pos));
    if (text == nullptr) {
        LOG_ERROR("Text is null.");
        return SHARED_BLOCK_IS_FULL;
    }

    auto sizeIncludingNull = sqlite3_column_bytes(statement, pos) + 1;
    int status = sharedBlock->PutString(addedRows, pos, text, sizeIncludingNull);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        LOG_ERROR("Failed allocating %{public}d bytes for text at %{public}d,%{public}d, error=%{public}d",
            sizeIncludingNull, startPos + addedRows, pos, status);
        return SHARED_BLOCK_IS_FULL;
    }

    return FILL_ONE_ROW_SUCESS;
}

FillOneRowResult FillOneRowOfLong(AppDataFwk::SharedBlock *sharedBlock, sqlite3_stmt *statement, int startPos,
    int addedRows, int pos)
{
    int64_t value = sqlite3_column_int64(statement, pos);
    int status = sharedBlock->PutLong(addedRows, pos, value);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        LOG_ERROR("Failed allocating space for a long in column %{public}d, error=%{public}d", pos, status);
        return SHARED_BLOCK_IS_FULL;
    }

    return FILL_ONE_ROW_SUCESS;
}

FillOneRowResult FillOneRowOfFloat(AppDataFwk::SharedBlock *sharedBlock, sqlite3_stmt *statement, int startPos,
    int addedRows, int pos)
{
    double value = sqlite3_column_double(statement, pos);
    int status = sharedBlock->PutDouble(addedRows, pos, value);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        LOG_ERROR("Failed allocating space for a double in column %{public}d, error=%{public}d", pos, status);
        return SHARED_BLOCK_IS_FULL;
    }

    return FILL_ONE_ROW_SUCESS;
}

FillOneRowResult FillOneRowOfBlob(AppDataFwk::SharedBlock *sharedBlock, sqlite3_stmt *statement, int startPos,
    int addedRows, int pos)
{
    const void *blob = sqlite3_column_blob(statement, pos);
    auto size = sqlite3_column_bytes(statement, pos);
    int status = sharedBlock->PutBlob(addedRows, pos, blob, size);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        LOG_ERROR("Failed allocating %{public}d bytes for blob at %{public}d,%{public}d, error=%{public}d", size,
            startPos + addedRows, pos, status);
        return SHARED_BLOCK_IS_FULL;
    }

    return FILL_ONE_ROW_SUCESS;
}

FillOneRowResult FillOneRowOfNull(AppDataFwk::SharedBlock *sharedBlock, sqlite3_stmt *statement, int startPos,
    int addedRows, int pos)
{
    int status = sharedBlock->PutNull(addedRows, pos);
    if (status != AppDataFwk::SharedBlock::SHARED_BLOCK_OK) {
        LOG_ERROR("Failed allocating space for a null in column %{public}d, error=%{public}d", pos, status);
        return SHARED_BLOCK_IS_FULL;
    }
    return FILL_ONE_ROW_SUCESS;
}

bool ResetStatement(SharedBlockInfo *sharedBlockInfo)
{
    sqlite3_reset(sharedBlockInfo->statement);

    if (sharedBlockInfo->startPos > sharedBlockInfo->totalRows) {
        LOG_ERROR("startPos %{public}d > actual rows %{public}d", sharedBlockInfo->startPos,
            sharedBlockInfo->totalRows);
    }

    if (sharedBlockInfo->totalRows > 0 && sharedBlockInfo->addedRows == 0) {
        return false;
    }
    return true;
}

int64_t GetCombinedData(int startPos, int totalRows)
{
    if (startPos > totalRows) {
        LOG_ERROR("startPos %{public}d > actual rows %{public}d", startPos, totalRows);
    }

    auto high = static_cast<uint64_t>(static_cast<uint32_t>(startPos));
    auto low = static_cast<uint64_t>(static_cast<uint32_t>(totalRows));
    return static_cast<int64_t>((high << OFFSET) | low);
}
} // namespace NativeRdb
} // namespace OHOS