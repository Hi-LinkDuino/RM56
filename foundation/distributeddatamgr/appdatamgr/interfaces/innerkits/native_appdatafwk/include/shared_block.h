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

#ifndef SHARED_BLOCK_H
#define SHARED_BLOCK_H

#include <cinttypes>

#include <string>
#include <ashmem.h>
#include "message_parcel.h"
#include "parcel.h"
#include "securec.h"

namespace OHOS {
namespace AppDataFwk {
static const uint32_t INVALID_ROW_RECORD = 0xFFFFFFFF;
/**
 * This class stores a set of rows from a database in a buffer，
 * which is used as the set of query result.
 */
class SharedBlock {
public:
    /* Cell Unit types. */
    enum {
        CELL_UNIT_TYPE_NULL = 0,
        CELL_UNIT_TYPE_INTEGER = 1,
        CELL_UNIT_TYPE_FLOAT = 2,
        CELL_UNIT_TYPE_STRING = 3,
        CELL_UNIT_TYPE_BLOB = 4,
    };

    /* SharedBlock error types. */
    enum {
        SHARED_BLOCK_OK = 0,
        SHARED_BLOCK_BAD_VALUE = 1,
        SHARED_BLOCK_NO_MEMORY = 2,
        SHARED_BLOCK_INVALID_OPERATION = 3,
        SHARED_BLOCK_ASHMEM_ERROR = 4,
        SHARED_BLOCK_SET_PORT_ERROR = 5,
    };

    /* Cell Unit */
    struct CellUnit {
        int32_t type;
        union {
            double doubleValue;
            int64_t longValue;
            struct {
                uint32_t offset;
                uint32_t size;
            } stringOrBlobValue;
        } cell;
    } __attribute((packed));

    /**
     * SharedBlock constructor.
     */
    SharedBlock(const std::string &name, sptr<Ashmem> ashmem, size_t size, bool readOnly);

    /**
     * SharedBlock constructor.
     */
    ~SharedBlock();

    /**
     * Init current shared block.
     */
    bool Init();

    /**
     * Create a shared block.
     */
    static int Create(const std::string &name, size_t size, SharedBlock *&outSharedBlock);

    /**
     * Clear current shared block.
     */
    int Clear();

    /**
     * Set a shared block column.
     */
    int SetColumnNum(uint32_t numColumns);

    /**
     * Allocate a row unit and its directory.
     */
    int AllocRow();

    /**
     * Release the value of the last row.
     */
    int FreeLastRow();

    /**
     * Put blob data to the shared block.
     */
    int PutBlob(uint32_t row, uint32_t column, const void *value, size_t Size);

    /**
     * Put string data to the shared block.
     */
    int PutString(uint32_t row, uint32_t column, const char *value, size_t sizeIncludingNull);

    /**
     * Put long data to the shared block.
     */
    int PutLong(uint32_t row, uint32_t column, int64_t value);

    /**
     * Put Double data to the shared block.
     */
    int PutDouble(uint32_t row, uint32_t column, double value);

    /**
     * Put Null data to the shared block.
     */
    int PutNull(uint32_t row, uint32_t column);

    /**
     * Gets the cell unit at the specified row and column.
     */
    CellUnit *GetCellUnit(uint32_t row, uint32_t column);

    /**
     * Get string type data from cell unit.
     */
    const char *GetCellUnitValueString(CellUnit *cellUnit, size_t *outSizeIncludingNull)
    {
        *outSizeIncludingNull = cellUnit->cell.stringOrBlobValue.size;
        return static_cast<char *>(
            OffsetToPtr(cellUnit->cell.stringOrBlobValue.offset, cellUnit->cell.stringOrBlobValue.size));
    }

    /**
     * Get blob type data from cell unit.
     */
    const void *GetCellUnitValueBlob(CellUnit *cellUnit, size_t *outSize)
    {
        *outSize = cellUnit->cell.stringOrBlobValue.size;
        return OffsetToPtr(cellUnit->cell.stringOrBlobValue.offset, cellUnit->cell.stringOrBlobValue.size);
    }

    /**
     * The mHeader of the current result set.
     */
    const void *GetHeader()
    {
        return mHeader;
    }

    /**
     * Size of the used byte in the block.
     */
    size_t GetUsedBytes()
    {
        return mHeader->unusedOffset;
    }

    /**
     * The name of the current result set.
     */
    std::string Name()
    {
        return mName;
    }

    /**
     * The size of the current result set.
     */
    size_t Size()
    {
        return mSize;
    }

    /**
     * The row number of the current result set.
     */
    uint32_t GetRowNum()
    {
        return mHeader->rowNums;
    }

    /**
     * The column number of the current result set.
     */
    uint32_t GetColumnNum()
    {
        return mHeader->columnNums;
    }

    int WriteMessageParcel(MessageParcel &parcel);

    static int ReadMessageParcel(MessageParcel &parcel, SharedBlock *&block);
    /**
     * Write raw data in block.
     */
    size_t SetRawData(const void *rawData, size_t size);
    /**
     * The fd of shared memory
     */
    int GetFd()
    {
        if (ashmem_ == nullptr) {
            return -1;
        }
        return ashmem_->GetAshmemFd();
    }
    
private:
    std::string mName;
    sptr<Ashmem> ashmem_;
    void *mData;
    size_t mSize;
    bool mReadOnly;
    static const size_t ROW_OFFSETS_NUM = 100;
    /**
    * Default setting for SQLITE_MAX_COLUMN is 2000.
    * We can set it at compile time to as large as 32767
    */
    static const size_t COL_MAX_NUM = 32767;

    struct SharedBlockHeader {
        /* Offset of the lowest unused byte in the block. */
        uint32_t unusedOffset;
        /* Offset of the first row group. */
        uint32_t firstRowGroupOffset;
        /* Row numbers of the row group block. */
        uint32_t rowNums;
        /* Column numbers of the row group block. */
        uint32_t columnNums;
    };

    struct RowGroupHeader {
        uint32_t rowOffsets[ROW_OFFSETS_NUM];
        uint32_t nextGroupOffset;
    };

    SharedBlockHeader *mHeader;

    /**
     * Allocate a portion of the block. Returns the offset of the allocation.
     * Returns 0 if there isn't enough space.
     */
    uint32_t Alloc(size_t size, bool aligned = false);

    uint32_t *GetRowOffset(uint32_t row);

    uint32_t *AllocRowOffset();

    int PutBlobOrString(uint32_t row, uint32_t column, const void *value, size_t size, int32_t type);

    static int CreateSharedBlock(const std::string &name, size_t size, sptr<Ashmem> ashmem,
        SharedBlock *&outSharedBlock);

    uint32_t OffsetFromPtr(void *ptr);

    void *OffsetToPtr(uint32_t offset, uint32_t bufferSize = 0);

    /**
     * Convert utf8 string to utf16.
     */
    static std::u16string ToUtf16(std::string str);

    /**
     * Convert utf16 string to utf8.
     */
    static std::string ToUtf8(std::u16string str16);
};
} // namespace AppDataFwk
} // namespace OHOS
#endif
