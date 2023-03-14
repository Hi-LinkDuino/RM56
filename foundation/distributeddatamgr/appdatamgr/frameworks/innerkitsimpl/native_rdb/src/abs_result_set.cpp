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

#include "abs_result_set.h"
#include <algorithm>
#include <string>
#include "logger.h"
#include "rdb_errno.h"

namespace OHOS {
namespace NativeRdb {
AbsResultSet::AbsResultSet() : rowPos(INIT_POS), isClosed(false)
{}

AbsResultSet::~AbsResultSet() {}

int AbsResultSet::GetRowCount(int &count)
{
    return E_OK;
}

int AbsResultSet::GetAllColumnNames(std::vector<std::string> &columnNames)
{
    return E_OK;
}

int AbsResultSet::GetBlob(int columnIndex, std::vector<uint8_t> &blob)
{
    return E_OK;
}

int AbsResultSet::GetString(int columnIndex, std::string &value)
{
    return E_OK;
}

int AbsResultSet::GetInt(int columnIndex, int &value)
{
    return E_OK;
}

int AbsResultSet::GetLong(int columnIndex, int64_t &value)
{
    return E_OK;
}

int AbsResultSet::GetDouble(int columnIndex, double &value)
{
    return E_OK;
}

int AbsResultSet::IsColumnNull(int columnIndex, bool &isNull)
{
    return E_OK;
}

int AbsResultSet::GoToRow(int position)
{
    return E_OK;
}

int AbsResultSet::GetColumnType(int columnIndex, ColumnType &columnType)
{
    return E_OK;
}

int AbsResultSet::GetRowIndex(int &position) const
{
    position = rowPos;
    return E_OK;
}

int AbsResultSet::GoTo(int offset)
{
    int ret = GoToRow(rowPos + offset);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GoTo return ret is wrong!");
        return ret;
    }
    return E_OK;
}

int AbsResultSet::GoToFirstRow()
{
    int ret = GoToRow(0);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GoToFirstRow return ret is wrong!");
        return ret;
    }
    return E_OK;
}

int AbsResultSet::GoToLastRow()
{
    int rowCnt = 0;
    int ret = GetRowCount(rowCnt);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GoToLastRow  return GetRowCount::ret is wrong!");
        return ret;
    }

    ret = GoToRow(rowCnt - 1);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GoToLastRow  return GoToRow::ret is wrong!");
        return ret;
    }
    return E_OK;
}

int AbsResultSet::GoToNextRow()
{
    int ret = GoToRow(rowPos + 1);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GoToNextRow  return GoToRow::ret is wrong!");
        return ret;
    }
    return E_OK;
}

int AbsResultSet::GoToPreviousRow()
{
    int ret = GoToRow(rowPos - 1);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GoToPreviousRow  return GoToRow::ret is wrong!");
        return ret;
    }
    return E_OK;
}

int AbsResultSet::IsAtFirstRow(bool &result) const
{
    result = (rowPos == 0);
    return E_OK;
}

int AbsResultSet::IsAtLastRow(bool &result)
{
    int rowCnt = 0;
    int ret = GetRowCount(rowCnt);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::IsAtLastRow  return GetRowCount::ret is wrong!");
        return ret;
    }
    result = (rowPos == (rowCnt - 1));
    return E_OK;
}

int AbsResultSet::IsStarted(bool &result) const
{
    result = (rowPos != INIT_POS);
    return E_OK;
}

int AbsResultSet::IsEnded(bool &result)
{
    int rowCnt = 0;
    int ret =  GetRowCount(rowCnt);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::IsEnded  return GetRowCount::ret is wrong!");
        return ret;
    }
    result = (rowCnt == 0) ? true : (rowPos == rowCnt);
    return E_OK;
}

int AbsResultSet::GetColumnCount(int &count)
{
    std::vector<std::string> columnNames;
    int ret = GetAllColumnNames(columnNames);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GetColumnCount  return GetAllColumnNames::ret is wrong!");
        return ret;
    }
    count = static_cast<int>(columnNames.size());
    return E_OK;
}

int AbsResultSet::GetColumnIndex(const std::string &columnName, int &columnIndex)
{
    auto periodIndex = columnName.rfind('.');
    std::string columnNameLower = columnName;
    if (periodIndex != std::string::npos) {
        columnNameLower = columnNameLower.substr(periodIndex + 1);
    }
    transform(columnNameLower.begin(), columnNameLower.end(), columnNameLower.begin(), ::tolower);
    std::vector<std::string> columnNames;
    int ret = GetAllColumnNames(columnNames);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GetColumnIndex  return GetAllColumnNames::ret is wrong!");
        return ret;
    }

    columnIndex = 0;
    for (const auto& name : columnNames) {
        std::string lowerName = name;
        transform(name.begin(), name.end(), lowerName.begin(), ::tolower);
        if (lowerName == columnNameLower) {
            return E_OK;
        }
        columnIndex++;
    }
    columnIndex = -1;
    return E_ERROR;
}

int AbsResultSet::GetColumnName(int columnIndex, std::string &columnName)
{
    int rowCnt = 0;
    int ret = GetColumnCount(rowCnt);
    if (ret != E_OK) {
        LOG_ERROR("AbsResultSet::GetColumnName  return GetColumnCount::ret is wrong!");
        return ret;
    }
    if (columnIndex >= rowCnt || columnIndex < 0) {
        return E_INVALID_COLUMN_INDEX;
    }
    std::vector<std::string> columnNames;
    GetAllColumnNames(columnNames);
    columnName = columnNames[columnIndex];
    return E_OK;
}

bool AbsResultSet::IsClosed() const
{
    return isClosed;
}

int AbsResultSet::Close()
{
    isClosed = true;
    return E_OK;
}
} // namespace NativeRdb
} // namespace OHOS