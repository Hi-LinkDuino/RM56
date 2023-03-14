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

#ifndef NATIVE_RDB_RESULT_SET_H
#define NATIVE_RDB_RESULT_SET_H

#include <string>
#include <vector>
namespace OHOS {
namespace NativeRdb {

/* Value returned by getColumnType(int) */
enum class ColumnType {
    TYPE_NULL = 0,
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BLOB,
};

class ResultSet {
public:
    virtual ~ResultSet() {}

    /**
     * Returns a string array holding the names of all of the columns in the
     * result set.
     *
     * return the names of the columns contains in this query result.
     */
    virtual int GetAllColumnNames(std::vector<std::string> &columnNames) = 0;

    /**
     * Return the total number of columns
     *
     * return the number of columns
     */
    virtual int GetColumnCount(int &count) = 0;

    /**
     * Returns data type of the given column's value.
     *
     * param columnIndex the zero-based index of the target column.
     * return column value type.
     */
    virtual int GetColumnType(int columnIndex, ColumnType &columnType) = 0;

    /**
     * Returns the zero-based index for the given column name.
     *
     * param columnName the name of the column.
     * return the column index for the given column, or -1 if
     *     the column does not exist.
     */
    virtual int GetColumnIndex(const std::string &columnName, int &columnIndex) = 0;

    /**
     * Returns the column name at the given column index.
     *
     * param columnIndex the zero-based index.
     * return the column name for the given index.
     */
    virtual int GetColumnName(int columnIndex, std::string &columnName) = 0;

    /**
     * return the numbers of rows in the result set.
     */
    virtual int GetRowCount(int &count) = 0;

    /**
     * Returns the current position of the cursor in the result set.
     * The value is zero-based. When the result set is first returned the cursor
     * will be at position -1, which is before the first row.
     * After the last row is returned another call to next() will leave the cursor past
     * the last entry, at a position of count().
     *
     * return the current cursor position.
     */
    virtual int GetRowIndex(int &position) const = 0;

    /**
     * Move the cursor a relative amount from current position. Positive offset move forward,
     * negative offset move backward.
     *
     * param offset the offset to be applied from the current position.
     * return whether the requested move succeeded.
     */
    virtual int GoTo(int offset) = 0;

    /**
     * Move the cursor to an absolute position.
     *
     * param position the zero-based position to move to.
     * return whether the requested move succeeded.
     */
    virtual int GoToRow(int position) = 0;

    /**
     * Move the cursor to the first row.
     *
     * return whether the requested move succeeded.
     */
    virtual int GoToFirstRow() = 0;

    /**
     * Move the cursor to the last row.
     *
     * return whether the requested move succeeded.
     */
    virtual int GoToLastRow() = 0;

    /**
     * Move the cursor to the next row.
     *
     * return whether the requested move succeeded.
     */
    virtual int GoToNextRow() = 0;

    /**
     * Move the cursor to the previous row.
     *
     * return whether the requested move succeeded.
     */
    virtual int GoToPreviousRow() = 0;

    /**
     * Returns whether the cursor is pointing to the position after the last
     * row.
     *
     * return whether the cursor is before the first row.
     */
    virtual int IsEnded(bool &result) = 0;

    /**
     * Returns whether the cursor is pointing to the position before the first
     * row.
     *
     * return whether the cursor is before the first row.
     */
    virtual int IsStarted(bool &result) const = 0;

    /**
     * Returns whether the cursor is pointing to the first row.
     *
     * return whether the cursor is pointing at the first entry.
     */
    virtual int IsAtFirstRow(bool &result) const = 0;

    /**
     * Returns whether the cursor is pointing to the last row.
     *
     * return whether the cursor is pointing at the last entry.
     */
    virtual int IsAtLastRow(bool &result) = 0;

    /**
     * Returns the value of the requested column as a byte array.
     *
     * param columnIndex the zero-based index of the target column.
     * return the value of the requested column as a byte array.
     */
    virtual int GetBlob(int columnIndex, std::vector<uint8_t> &blob) = 0;

    /**
     * Returns the value of the requested column as a String.
     *
     * param columnIndex the zero-based index of the target column.
     * return the value of the requested column as a String.
     */
    virtual int GetString(int columnIndex, std::string &value) = 0;

    /**
     * Returns the value of the requested column as a int.
     *
     * param columnIndex the zero-based index of the target column.
     * return the value of the requested column as a int.
     */
    virtual int GetInt(int columnIndex, int &value) = 0;

    /**
     * Returns the value of the requested column as a long.
     *
     * param columnIndex the zero-based index of the target column.
     * return the value of the requested column as a long.
     */
    virtual int GetLong(int columnIndex, int64_t &value) = 0;

    /**
     * Returns the value of the requested column as a double.
     *
     * param columnIndex the zero-based index of the target column.
     * return the value of the requested column as a double.
     */
    virtual int GetDouble(int columnIndex, double &value) = 0;

    /**
     * Whether the value of the requested column is null.
     *
     * param columnIndex the zero-based index of the target column.
     * return whether the column value is null.
     */
    virtual int IsColumnNull(int columnIndex, bool &isNull) = 0;

    /**
     * Return true if the result set is closed.
     *
     * return true if the result set is closed.
     */
    virtual bool IsClosed() const = 0;

    /**
     * Closes the result set, releasing all of its resources and making it
     * completely invalid.
     */
    virtual int Close() = 0;
};

} // namespace NativeRdb
} // namespace OHOS
#endif
