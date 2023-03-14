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
#include "abs_shared_result_set.h"

#include <securec.h>

#include <algorithm>
#include <codecvt>
#include <iostream>
#include <sstream>
#include <string>

#include "logger.h"
#include "parcel.h"
#include "rdb_errno.h"
#include "shared_block.h"
#include "string_ex.h"

namespace OHOS {
namespace NativeRdb {
AbsSharedResultSet::AbsSharedResultSet(std::string name)
{
    AppDataFwk::SharedBlock::Create(name, DEFAULT_BLOCK_SIZE, sharedBlock_);
}

AbsSharedResultSet::AbsSharedResultSet()
{
}

AbsSharedResultSet::~AbsSharedResultSet()
{
    ClosedBlock();
}

int AbsSharedResultSet::GetAllColumnNames(std::vector<std::string> &columnNames)
{
    return E_OK;
}

int AbsSharedResultSet::GetRowCount(int &count)
{
    return E_OK;
}

bool AbsSharedResultSet::OnGo(int oldRowIndex, int newRowIndex)
{
    return true;
}

void AbsSharedResultSet::FillBlock(int startRowIndex, AppDataFwk::SharedBlock *block)
{
    return;
}

/**
 * Get current shared block
 */
AppDataFwk::SharedBlock *AbsSharedResultSet::GetBlock() const
{
    return sharedBlock_;
}

int AbsSharedResultSet::GetColumnType(int columnIndex, ColumnType &columnType)
{
    AppDataFwk::SharedBlock::CellUnit *cellUnit = sharedBlock_->GetCellUnit((uint32_t)rowPos, (uint32_t)columnIndex);
    if (!cellUnit) {
        LOG_ERROR("AbsSharedResultSet::GetColumnType cellUnit is null!");
        return E_ERROR;
    }
    columnType = (ColumnType)cellUnit->type;
    return E_OK;
}

int AbsSharedResultSet::GoToRow(int position)
{
    int rowCnt = 0;
    GetRowCount(rowCnt);
    if (position >= rowCnt) {
        rowPos = rowCnt;
        return E_ERROR;
    }
    if (position < 0) {
        rowPos = INIT_POS;
        return E_ERROR;
    }
    if (position == rowPos) {
        return E_OK;
    }
    bool result = true;
    if (sharedBlock_ == nullptr || (uint32_t)position >= sharedBlock_->GetRowNum()) {
        result = OnGo(rowPos, position);
    }
    if (!result) {
        rowPos = INIT_POS;
        return E_ERROR;
    } else {
        rowPos = position;
        return E_OK;
    }
}

int AbsSharedResultSet::GetBlob(int columnIndex, std::vector<uint8_t> &value)
{
    int errorCode = CheckState(columnIndex);
    if (errorCode != E_OK) {
        return errorCode;
    }

    AppDataFwk::SharedBlock::CellUnit *cellUnit = sharedBlock_->GetCellUnit(rowPos, columnIndex);
    if (!cellUnit) {
        LOG_ERROR("AbsSharedResultSet::GetBlob cellUnit is null!");
        return E_ERROR;
    }

    value.resize(0);
    int type = cellUnit->type;
    if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB
        || type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_STRING) {
        size_t size;
        const auto *blob = static_cast<const uint8_t *>(sharedBlock_->GetCellUnitValueBlob(cellUnit, &size));
        if (size == 0 || blob == nullptr) {
            LOG_WARN("blob data is empty!");
        } else {
            value.resize(size);
            value.assign(blob, blob + size);
        }
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER) {
        LOG_ERROR("AbsSharedResultSet::GetDouble AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER!");
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        LOG_ERROR("AbsSharedResultSet::GetDouble AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL!");
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT) {
        LOG_ERROR("AbsSharedResultSet::GetDouble AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT!");
        return E_OK;
    } else {
        LOG_ERROR("AbsSharedResultSet::GetDouble AppDataFwk::SharedBlock::nothing !");
        return E_INVALID_OBJECT_TYPE;
    }
}

int AbsSharedResultSet::GetString(int columnIndex, std::string &value)
{
    int errorCode = CheckState(columnIndex);
    if (errorCode != E_OK) {
        return errorCode;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = sharedBlock_->GetCellUnit(rowPos, columnIndex);
    if (!cellUnit) {
        LOG_ERROR("AbsSharedResultSet::GetString cellUnit is null!");
        return E_ERROR;
    }
    int type = cellUnit->type;
    if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *tempValue = sharedBlock_->GetCellUnitValueString(cellUnit, &sizeIncludingNull);
        if ((sizeIncludingNull <= 1) || (tempValue == nullptr)) {
            value = "";
            return E_ERROR;
        }
        value = tempValue;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER) {
        int64_t tempValue = cellUnit->cell.longValue;
        value = std::to_string(tempValue);
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT) {
        double tempValue = cellUnit->cell.doubleValue;
        std::ostringstream os;
        if (os << tempValue)
            value = os.str();
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        LOG_ERROR("AbsSharedResultSet::AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL!");
        return E_ERROR;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB) {
        LOG_ERROR("AbsSharedResultSet::AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB!");
        return E_ERROR;
    } else {
        LOG_ERROR("AbsSharedResultSet::GetString is failed!");
        return E_ERROR;
    }
}

int AbsSharedResultSet::GetInt(int columnIndex, int &value)
{
    AppDataFwk::SharedBlock::CellUnit *cellUnit = sharedBlock_->GetCellUnit(rowPos, columnIndex);
    if (!cellUnit) {
        LOG_ERROR("AbsSharedResultSet::GetInt cellUnit is null!");
        return E_ERROR;
    }
    value = (int)cellUnit->cell.longValue;
    return E_OK;
}

int AbsSharedResultSet::GetLong(int columnIndex, int64_t &value)
{
    int errorCode = CheckState(columnIndex);
    if (errorCode != E_OK) {
        return errorCode;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = sharedBlock_->GetCellUnit(rowPos, columnIndex);
    if (!cellUnit) {
        LOG_ERROR("AbsSharedResultSet::GetLong cellUnit is null!");
        return E_ERROR;
    }

    int type = cellUnit->type;

    if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER) {
        value = cellUnit->cell.longValue;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *tempValue = sharedBlock_->GetCellUnitValueString(cellUnit, &sizeIncludingNull);
        value = ((sizeIncludingNull > 1) && (tempValue != nullptr)) ? int64_t(strtoll(tempValue, nullptr, 0)) : 0L;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT) {
        value = (int64_t)cellUnit->cell.doubleValue;
        LOG_ERROR("AbsSharedResultSet::GetLong AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT !");
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        LOG_ERROR("AbsSharedResultSet::GetLong AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL !");
        value = 0L;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB) {
        LOG_ERROR("AbsSharedResultSet::GetLong AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB !");
        value = 0L;
        return E_OK;
    } else {
        LOG_ERROR("AbsSharedResultSet::GetLong Nothing !");
        return E_INVALID_OBJECT_TYPE;
    }
}

int AbsSharedResultSet::GetDouble(int columnIndex, double &value)
{
    int errorCode = CheckState(columnIndex);
    if (errorCode != E_OK) {
        return errorCode;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = sharedBlock_->GetCellUnit(rowPos, columnIndex);
    if (!cellUnit) {
        LOG_ERROR("AbsSharedResultSet::GetDouble cellUnit is null!");
        return E_ERROR;
    }
    int type = cellUnit->type;
    if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_FLOAT) {
        value = cellUnit->cell.doubleValue;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *tempValue = sharedBlock_->GetCellUnitValueString(cellUnit, &sizeIncludingNull);
        value = ((sizeIncludingNull > 1) && (tempValue != nullptr)) ? strtod(tempValue, nullptr) : 0.0;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_INTEGER) {
        value = cellUnit->cell.longValue;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        LOG_ERROR("AbsSharedResultSet::GetDouble AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL!");
        value = 0.0;
        return E_OK;
    } else if (type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB) {
        LOG_ERROR("AbsSharedResultSet::GetDouble AppDataFwk::SharedBlock::CELL_UNIT_TYPE_BLOB!");
        value = 0.0;
        return E_OK;
    } else {
        LOG_ERROR("AbsSharedResultSet::GetDouble AppDataFwk::SharedBlock::nothing !");
        value = 0.0;
        return E_INVALID_OBJECT_TYPE;
    }
}

int AbsSharedResultSet::IsColumnNull(int columnIndex, bool &isNull)
{
    int errorCode = CheckState(columnIndex);
    if (errorCode != E_OK) {
        return errorCode;
    }
    AppDataFwk::SharedBlock::CellUnit *cellUnit = sharedBlock_->GetCellUnit(rowPos, columnIndex);
    if (!cellUnit) {
        LOG_ERROR("AbsSharedResultSet::IsColumnNull cellUnit is null!");
        return E_ERROR;
    }
    if (cellUnit->type == AppDataFwk::SharedBlock::CELL_UNIT_TYPE_NULL) {
        isNull = true;
        return E_OK;
    }
    isNull = false;
    return E_OK;
}

int AbsSharedResultSet::Close()
{
    AbsResultSet::Close();
    ClosedBlock();
    return E_OK;
}

/**
 * Allocates a new shared block to an {@link AbsSharedResultSet}
 */
void AbsSharedResultSet::SetBlock(AppDataFwk::SharedBlock *block)
{
    if (sharedBlock_ != block) {
        ClosedBlock();
        sharedBlock_ = block;
    }
}

/**
 * Checks whether an {@code AbsSharedResultSet} object contains shared blocks
 */
bool AbsSharedResultSet::HasBlock() const
{
    return sharedBlock_ != nullptr;
}

/**
 * Closes a shared block that is not empty in this {@code AbsSharedResultSet} object
 */
void AbsSharedResultSet::ClosedBlock()
{
    delete sharedBlock_;
    sharedBlock_ = nullptr;
}

void AbsSharedResultSet::ClearBlock()
{
    if (sharedBlock_ != nullptr) {
        sharedBlock_->Clear();
    }
}

void AbsSharedResultSet::Finalize()
{
    Close();
}

/**
 * Check current status
 */
int AbsSharedResultSet::CheckState(int columnIndex)
{
    if (sharedBlock_ == nullptr) {
        LOG_ERROR("AbsSharedResultSet::CheckState sharedBlock is null!");
        return E_ERROR;
    }
    int cnt = 0;
    GetColumnCount(cnt);
    if (columnIndex >= cnt || columnIndex < 0) {
        return E_INVALID_COLUMN_INDEX;
    }
    int rowCnt = 0;
    GetRowCount(rowCnt);
    if (rowPos < 0 || rowPos >= rowCnt) {
        return E_INVALID_STATEMENT;
    }
    return E_OK;
}

bool AbsSharedResultSet::Marshalling(MessageParcel &parcel)
{
    if (sharedBlock_ == nullptr) {
        LOG_ERROR("AbsSharedResultSet::Marshalling sharedBlock is null.");
        return false;
    }
    LOG_DEBUG("AbsSharedResultSet::Marshalling sharedBlock.");
    return sharedBlock_->WriteMessageParcel(parcel);
}

bool AbsSharedResultSet::Unmarshalling(MessageParcel &parcel)
{
    if (sharedBlock_ != nullptr) {
        return false;
    }
    int result = AppDataFwk::SharedBlock::ReadMessageParcel(parcel, sharedBlock_);
    if (result < 0) {
        LOG_ERROR("AbsSharedResultSet: create from parcel error is %{public}d.", result);
    }
    return true;
}
} // namespace NativeRdb
} // namespace OHOS
