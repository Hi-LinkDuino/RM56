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

#include <gtest/gtest.h>
#include <string>

#include "sqlite_shared_result_set.h"
#include "shared_block.h"
#include "logger.h"
#include "common.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;
class RdbSqliteSharedResultSetTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void GenerateDefaultTable();

    static const std::string DATABASE_NAME;
    static std::shared_ptr<RdbStore> store;
    static const int E_SQLITE_ERROR;
};

const std::string RdbSqliteSharedResultSetTest::DATABASE_NAME = RDB_TEST_PATH + "shared_test.db";
std::shared_ptr<RdbStore> RdbSqliteSharedResultSetTest::store = nullptr;
const int RdbSqliteSharedResultSetTest::E_SQLITE_ERROR = -1;

class SqliteSharedOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

std::string const SqliteSharedOpenCallback::CREATE_TABLE_TEST =
    "CREATE TABLE test (id INTEGER PRIMARY KEY AUTOINCREMENT, data1 TEXT,data2 INTEGER, data3 FLOAT, data4 BLOB);";


int SqliteSharedOpenCallback::OnCreate(RdbStore &rdbStore)
{
    return rdbStore.ExecuteSql(CREATE_TABLE_TEST);
}

int SqliteSharedOpenCallback::OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbSqliteSharedResultSetTest::SetUpTestCase(void)
{
    RdbStoreConfig sqliteSharedRstConfig(RdbSqliteSharedResultSetTest::DATABASE_NAME);
    SqliteSharedOpenCallback sqliteSharedRstHelper;
    int errCode = E_OK;
    RdbSqliteSharedResultSetTest::store =
        RdbHelper::GetRdbStore(sqliteSharedRstConfig, 1, sqliteSharedRstHelper, errCode);
    EXPECT_NE(RdbSqliteSharedResultSetTest::store, nullptr);
}

void RdbSqliteSharedResultSetTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbSqliteSharedResultSetTest::DATABASE_NAME);
}

void RdbSqliteSharedResultSetTest::SetUp()
{
    store->ExecuteSql("DELETE FROM test");
}

void RdbSqliteSharedResultSetTest::TearDown()
{}

void RdbSqliteSharedResultSetTest::GenerateDefaultTable()
{
    std::shared_ptr<RdbStore> &store = RdbSqliteSharedResultSetTest::store;

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("data1", std::string("hello"));
    values.PutInt("data2", 10);
    values.PutDouble("data3", 1.0);
    values.PutBlob("data4", std::vector<uint8_t> { 66 });
    store->Insert(id, "test", values);

    values.Clear();
    values.PutInt("id", 2);
    values.PutString("data1", std::string("2"));
    values.PutInt("data2", -5);
    values.PutDouble("data3", 2.5);
    values.PutBlob("data4", std::vector<uint8_t> {});
    store->Insert(id, "test", values);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("data1", std::string("hello world"));
    values.PutInt("data2", 3);
    values.PutDouble("data3", 1.8);
    values.PutBlob("data4", std::vector<uint8_t> {});
    store->Insert(id, "test", values);
}

/* *
 * @tc.name: RdbStore_Delete_001
 * @tc.desc: normal testcase of SqliteSharedResultSet for move
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_001, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int ret = rstSet->GoToRow(1);
    EXPECT_EQ(ret, E_OK);

    int rowCnt = -1;
    ret = rstSet->GetRowCount(rowCnt);
    EXPECT_EQ(rowCnt, 3);

    std::string colName = "";
    rstSet->GetColumnName(1, colName);
    EXPECT_EQ(colName, "data1");

    rstSet->GetColumnName(2, colName);
    EXPECT_EQ(colName, "data2");

    rstSet->GetColumnName(3, colName);
    EXPECT_EQ(colName, "data3");

    rstSet->GetColumnName(4, colName);
    EXPECT_EQ(colName, "data4");

    std::string valueStr = "";
    rstSet->GetString(0, valueStr);
    EXPECT_EQ(valueStr, "2");

    rstSet->GetString(1, valueStr);
    EXPECT_EQ(valueStr, "2");

    int64_t valuelg = 0;
    rstSet->GetLong(2, valuelg);
    EXPECT_EQ(valuelg, -5);

    double valueDb = 0.0;
    rstSet->GetDouble(3, valueDb);
    EXPECT_EQ(valueDb, 2.5);

    std::vector<uint8_t> blob;
    rstSet->GetBlob(4, blob);
    int sz = blob.size();
    EXPECT_EQ(sz, 0);

    rstSet->GoTo(1);
    rstSet->GetString(0, valueStr);
    EXPECT_EQ(valueStr, "3");

    rstSet->GetString(1, valueStr);
    EXPECT_EQ(valueStr, "hello world");

    rstSet->GetLong(2, valuelg);
    EXPECT_EQ(valuelg, 3);

    rstSet->GetDouble(3, valueDb);
    EXPECT_EQ(valueDb, 1.8);

    rstSet->GetBlob(4, blob);
    sz = blob.size();
    EXPECT_EQ(sz, 0);

    bool isNull = false;
    rstSet->IsColumnNull(4, isNull);
    EXPECT_EQ(isNull, true);

    ret = -1;
    ret = rstSet->GoToPreviousRow();
    EXPECT_EQ(ret, E_OK);
    ret = -1;
    ret = rstSet->GoToPreviousRow();
    EXPECT_EQ(ret, E_OK);

    rstSet->GetString(0, valueStr);
    EXPECT_EQ(valueStr, "1");

    rstSet->GetString(1, valueStr);
    EXPECT_EQ(valueStr, "hello");

    rstSet->GetLong(2, valuelg);
    EXPECT_EQ(valuelg, 10);

    rstSet->GetDouble(3, valueDb);
    EXPECT_EQ(valueDb, 1.0);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_002
 * @tc.desc: normal testcase of SqliteSharedResultSet for goToNextRow
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_002, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int pos = -2;
    rstSet->GetRowIndex(pos);
    EXPECT_EQ(pos, -1);
    bool isStart = true;
    rstSet->IsStarted(isStart);
    EXPECT_EQ(isStart, false);
    bool isAtFirstRow = true;
    rstSet->IsAtFirstRow(isAtFirstRow);
    EXPECT_EQ(isAtFirstRow, false);
    bool isEnded = true;
    rstSet->IsEnded(isEnded);
    EXPECT_EQ(isEnded, false);

    int retN1 = rstSet->GoToNextRow();
    EXPECT_EQ(retN1, E_OK);
    rstSet->GetRowIndex(pos);
    EXPECT_EQ(pos, 0);
    rstSet->IsStarted(isStart);
    EXPECT_EQ(isStart, true);
    rstSet->IsAtFirstRow(isAtFirstRow);
    EXPECT_EQ(isAtFirstRow, true);
    isEnded = true;
    rstSet->IsEnded(isEnded);
    EXPECT_EQ(isEnded, false);

    int retN2 = rstSet->GoToNextRow();
    EXPECT_EQ(retN2, E_OK);
    rstSet->GetRowIndex(pos);
    EXPECT_EQ(pos, 1);
    isStart = false;
    rstSet->IsStarted(isStart);
    EXPECT_EQ(isStart, true);
    isAtFirstRow = true;
    rstSet->IsAtFirstRow(isAtFirstRow);
    EXPECT_EQ(isAtFirstRow, false);
    isEnded = true;
    rstSet->IsEnded(isEnded);
    EXPECT_EQ(isEnded, false);

    int retN3 = rstSet->GoToNextRow();
    EXPECT_EQ(retN3, E_OK);
    rstSet->GetRowIndex(pos);
    EXPECT_EQ(pos, 2);
    isStart = false;
    rstSet->IsStarted(isStart);
    EXPECT_EQ(isStart, true);
    isAtFirstRow = true;
    rstSet->IsAtFirstRow(isAtFirstRow);
    EXPECT_EQ(isAtFirstRow, false);
    bool isAtLastRow = false;
    rstSet->IsAtLastRow(isAtLastRow);
    EXPECT_EQ(isAtLastRow, true);

    int retN = rstSet->GoToNextRow();
    EXPECT_EQ(retN, E_ERROR);
    rstSet->GetRowIndex(pos);
    EXPECT_EQ(pos, 3);
    isStart = false;
    rstSet->IsStarted(isStart);
    EXPECT_EQ(isStart, true);
    isAtFirstRow = true;
    rstSet->IsAtFirstRow(isAtFirstRow);
    EXPECT_EQ(isAtFirstRow, false);
    isEnded = false;
    rstSet->IsEnded(isEnded);
    EXPECT_EQ(isEnded, true);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_003
 * @tc.desc: normal testcase of SqliteSharedResultSet for moveFirst
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_003, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);
    int index = -1;
    rstSet->GetRowIndex(index);
    EXPECT_EQ(index, 0);
    bool isAtFirstRow = false;
    rstSet->IsAtFirstRow(isAtFirstRow);
    EXPECT_EQ(isAtFirstRow, true);
    bool isStd = false;
    rstSet->IsStarted(isStd);
    EXPECT_EQ(isStd, true);

    int retN = rstSet->GoToNextRow();
    EXPECT_EQ(retN, E_OK);
    rstSet->GetRowIndex(index);
    EXPECT_EQ(index, 1);
    isAtFirstRow = true;
    rstSet->IsAtFirstRow(isAtFirstRow);
    EXPECT_EQ(isAtFirstRow, false);
    isStd = false;
    rstSet->IsStarted(isStd);
    EXPECT_EQ(isStd, true);

    int retGf = rstSet->GoToFirstRow();
    EXPECT_EQ(retGf, E_OK);
    rstSet->GetRowIndex(index);
    EXPECT_EQ(index, 0);
    isAtFirstRow = false;
    rstSet->IsAtFirstRow(isAtFirstRow);
    EXPECT_EQ(isAtFirstRow, true);
    isStd = false;
    rstSet->IsStarted(isStd);
    EXPECT_EQ(isStd, true);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_004
 * @tc.desc: normal testcase of SqliteSharedResultSet for getInt
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_004, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int64_t valueInt = 0;
    int ret = rstSet->GetLong(0, valueInt);
    EXPECT_EQ(ret, E_INVALID_STATEMENT);

    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);
    rstSet->GetLong(0, valueInt);
    EXPECT_EQ(valueInt, 1);
    rstSet->GetLong(2, valueInt);
    EXPECT_EQ(valueInt, 10);
    rstSet->GetLong(3, valueInt);
    EXPECT_EQ(valueInt, 1);

    int retN = rstSet->GoToNextRow();
    EXPECT_EQ(retN, E_OK);
    rstSet->GetLong(0, valueInt);
    EXPECT_EQ(valueInt, 2);
    valueInt = 0;
    rstSet->GetLong(0, valueInt);
    EXPECT_EQ(valueInt, 2);
    valueInt = 0;
    rstSet->GetLong(1, valueInt);
    EXPECT_EQ(valueInt, 2);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_005
 * @tc.desc: normal testcase of SqliteSharedResultSet for getString
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */

HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_005, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    std::string valueStr = "";
    int ret1 = rstSet->GetString(0, valueStr);
    EXPECT_EQ(ret1, E_INVALID_STATEMENT);

    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);
    valueStr = "";
    rstSet->GetString(1, valueStr);
    EXPECT_EQ(valueStr, "hello");
    rstSet->GetString(2, valueStr);
    EXPECT_EQ(valueStr, "10");
    rstSet->GetString(3, valueStr);
    EXPECT_EQ(valueStr, "1");

    int ret2 = rstSet->GetString(4, valueStr);
    EXPECT_EQ(ret2, E_ERROR);

    valueStr = "";
    int colCnt = 0;
    rstSet->GetColumnCount(colCnt);
    int ret3 = rstSet->GetString(colCnt, valueStr);
    EXPECT_EQ(ret3, E_INVALID_COLUMN_INDEX);

    int retN = rstSet->GoToNextRow();
    EXPECT_EQ(retN, E_OK);
    rstSet->GetString(0, valueStr);
    EXPECT_EQ(valueStr, "2");
    valueStr = "";
    rstSet->GetString(1, valueStr);
    EXPECT_EQ(valueStr, "2");
    rstSet->GetString(2, valueStr);
    EXPECT_EQ(valueStr, "-5");
    rstSet->GetString(3, valueStr);
    EXPECT_EQ(valueStr, "2.5");

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_006
 * @tc.desc: normal testcase of SqliteSharedResultSet for getDouble
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_006, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    double valueDb = 0.0;
    int ret = rstSet->GetDouble(0, valueDb);
    EXPECT_EQ(ret, E_INVALID_STATEMENT);

    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);
    rstSet->GetDouble(0, valueDb);
    EXPECT_EQ(valueDb, 1.0);
    std::string valueStr = "";
    rstSet->GetString(1, valueStr);
    EXPECT_EQ(valueStr, "hello");
    rstSet->GetDouble(2, valueDb);
    EXPECT_EQ(valueDb, 10.0);
    rstSet->GetDouble(3, valueDb);
    EXPECT_EQ(valueDb, 1.0);

    int colCnt = 0;
    rstSet->GetColumnCount(colCnt);
    int ret1 = rstSet->GetDouble(colCnt, valueDb);
    EXPECT_EQ(ret1, E_INVALID_COLUMN_INDEX);

    int retN = rstSet->GoToNextRow();
    EXPECT_EQ(retN, E_OK);
    rstSet->GetDouble(0, valueDb);
    EXPECT_EQ(valueDb, 2.0);
    valueDb = 0.0;
    rstSet->GetDouble(1, valueDb);
    EXPECT_EQ(valueDb, 2.0);

    rstSet->GetDouble(2, valueDb);
    EXPECT_EQ(valueDb, -5.0);
    rstSet->GetDouble(3, valueDb);
    EXPECT_EQ(valueDb, 2.5);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_007
 * @tc.desc: normal testcase of SqliteSharedResultSet for getBlob
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_007, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);

    std::vector<uint8_t> blobVec;
    rstSet->GetBlob(4, blobVec);
    EXPECT_EQ(blobVec[0], 66);

    int retN = rstSet->GoToNextRow();
    EXPECT_EQ(retN, E_OK);
    blobVec.clear();
    rstSet->GetBlob(4, blobVec);
    int blobSz = blobVec.size();
    EXPECT_EQ(blobSz, 0);

    int retN1 = rstSet->GoToNextRow();
    EXPECT_EQ(retN1, E_OK);
    blobVec.clear();
    rstSet->GetBlob(4, blobVec);
    EXPECT_EQ(blobSz, 0);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_008
 * @tc.desc: normal testcase of SqliteSharedResultSet for getColumnTypeForIndex
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */

HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_008, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    ColumnType colType;
    int ret = rstSet->GetColumnType(0, colType);
    EXPECT_EQ(ret, E_ERROR);
    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);

    rstSet->GetColumnType(0, colType);
    EXPECT_EQ(colType, ColumnType::TYPE_INTEGER);

    bool isColNull = true;
    rstSet->IsColumnNull(0, isColNull);
    EXPECT_EQ(isColNull, false);

    rstSet->GetColumnType(1, colType);
    EXPECT_EQ(colType, ColumnType::TYPE_STRING);

    isColNull = true;
    rstSet->IsColumnNull(0, isColNull);
    EXPECT_EQ(isColNull, false);

    rstSet->GetColumnType(2, colType);
    EXPECT_EQ(colType, ColumnType::TYPE_INTEGER);
    rstSet->GetColumnType(3, colType);
    EXPECT_EQ(colType, ColumnType::TYPE_FLOAT);
    rstSet->GetColumnType(4, colType);
    EXPECT_EQ(colType, ColumnType::TYPE_BLOB);

    int colCnt = 0;
    rstSet->GetColumnCount(colCnt);
    int ret1 = rstSet->GetColumnType(colCnt, colType);
    EXPECT_EQ(ret1, E_ERROR);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_009
 * @tc.desc:  normal testcase of SqliteSharedResultSet for getColumnIndexForName
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_009, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int colIndex = 0;
    rstSet->GetColumnIndex("data1", colIndex);
    EXPECT_EQ(colIndex, 1);

    rstSet->GetColumnIndex("data2", colIndex);
    EXPECT_EQ(colIndex, 2);

    rstSet->GetColumnIndex("data3", colIndex);
    EXPECT_EQ(colIndex, 3);

    rstSet->GetColumnIndex("data4", colIndex);
    EXPECT_EQ(colIndex, 4);

    rstSet->GetColumnIndex("datax", colIndex);
    EXPECT_EQ(colIndex, -1);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_010
 * @tc.desc:  normal testcase of SqliteSharedResultSet for getColumnNameForIndex
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_010, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    std::vector<std::string> allColNamesVec;
    rstSet->GetAllColumnNames(allColNamesVec);

    std::string colName = "";
    rstSet->GetColumnName(1, colName);
    EXPECT_EQ(colName, "data1");
    EXPECT_EQ(allColNamesVec[1], colName);

    rstSet->GetColumnName(2, colName);
    EXPECT_EQ(colName, "data2");
    EXPECT_EQ(allColNamesVec[2], colName);

    rstSet->GetColumnName(3, colName);
    EXPECT_EQ(colName, "data3");
    rstSet->GetColumnName(4, colName);
    EXPECT_EQ(colName, "data4");

    int colCnt = 0;
    rstSet->GetColumnCount(colCnt);
    int ret = rstSet->GetColumnName(colCnt, colName);
    EXPECT_EQ(ret, E_INVALID_COLUMN_INDEX);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_011
 * @tc.desc:  normal testcase of SqliteSharedResultSet
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_011, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);

    bool isAtFrtRow = false;
    rstSet->IsAtFirstRow(isAtFrtRow);
    EXPECT_EQ(isAtFrtRow, true);

    bool isStarted = false;
    rstSet->IsStarted(isStarted);
    EXPECT_EQ(isStarted, true);

    int64_t valueInt = 0;
    rstSet->GetLong(2, valueInt);
    EXPECT_EQ(valueInt, 10);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_012
 * @tc.desc: normal testcase of SqliteSharedResultSet for getLong
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_012, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int64_t valueInt = 0;
    int ret = rstSet->GetLong(0, valueInt);
    EXPECT_EQ(ret, E_INVALID_STATEMENT);

    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);
    rstSet->GetLong(0, valueInt);
    EXPECT_EQ(valueInt, 1.0);
    std::string  valueStr = "";
    rstSet->GetString(1, valueStr);
    EXPECT_EQ(valueStr, "hello");
    rstSet->GetLong(2, valueInt);
    EXPECT_EQ(valueInt, 10.0);
    rstSet->GetLong(3, valueInt);
    EXPECT_EQ(valueInt, 1.0);

    int colCnt = 0;
    rstSet->GetColumnCount(colCnt);
    int ret1 = rstSet->GetLong(colCnt, valueInt);
    EXPECT_EQ(ret1, E_INVALID_COLUMN_INDEX);

    int retN = rstSet->GoToNextRow();
    EXPECT_EQ(retN, E_OK);
    rstSet->GetLong(0, valueInt);
    EXPECT_EQ(valueInt, 2.0);
    valueInt = 0;
    rstSet->GetLong(1, valueInt);
    EXPECT_EQ(valueInt, 2.0);
    rstSet->GetLong(2, valueInt);
    EXPECT_EQ(valueInt, -5.0);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_013
 * @tc.desc: normal testcase of SqliteSharedResultSet for fillBlock
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_013, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    SqliteSharedResultSet *pSqlSharedRstSet = static_cast<SqliteSharedResultSet *>(rstSet.get());
    bool isBk = pSqlSharedRstSet->HasBlock();
    EXPECT_EQ(isBk, true);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}
/* *
 * @tc.name: Sqlite_Shared_Result_Set_014
 * @tc.desc: normal testcase of SqliteSharedResultSet for getBlock
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_014, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    SqliteSharedResultSet *pSqlSharedRstSet = static_cast<SqliteSharedResultSet *>(rstSet.get());
    bool isBk = pSqlSharedRstSet->HasBlock();
    EXPECT_EQ(isBk, true);

    int retF = rstSet->GoToFirstRow();
    EXPECT_EQ(retF, E_OK);
    OHOS::AppDataFwk::SharedBlock*  pBk = pSqlSharedRstSet->GetBlock();
    EXPECT_NE(pBk, nullptr);

    std::string path = RdbSqliteSharedResultSetTest::store->GetPath();
    std::string path1 = pBk->Name();
	
    EXPECT_EQ(path,  "/data/test/shared_test.db");
    EXPECT_EQ(path1, "/data/test/shared_test.db");

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}
/* *
 * @tc.name: Sqlite_Shared_Result_Set_015
 * @tc.desc: normal testcase of SqliteSharedResultSet for setBlock
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_015, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    SqliteSharedResultSet *pSqlSharedRstSet = static_cast<SqliteSharedResultSet *>(rstSet.get());
    bool isBk = pSqlSharedRstSet->HasBlock();
    EXPECT_EQ(isBk, true);

    int retN = rstSet->GoToNextRow();
    EXPECT_EQ(retN, E_OK);

    std::string path = RdbSqliteSharedResultSetTest::store->GetPath();
    OHOS::AppDataFwk::SharedBlock*  pBk = pSqlSharedRstSet->GetBlock();
    std::string path1 = pBk->Name();

    EXPECT_EQ(path,  "/data/test/shared_test.db");
    EXPECT_EQ(path1, "/data/test/shared_test.db");

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_016
 * @tc.desc: normal testcase of SqliteSharedResultSet for setFillWindowForwardOnly
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_016, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    SqliteSharedResultSet *pSqlSharedRstSet = static_cast<SqliteSharedResultSet *>(rstSet.get());
    bool isBk = pSqlSharedRstSet->HasBlock();
    EXPECT_EQ(isBk, true);

    pSqlSharedRstSet->PickFillBlockStartPosition(0, 0);
    pSqlSharedRstSet->SetFillBlockForwardOnly(true);
    pSqlSharedRstSet->GoToFirstRow();

    OHOS::AppDataFwk::SharedBlock*  pBk = pSqlSharedRstSet->GetBlock();
    EXPECT_NE(pBk, nullptr);
    std::string path = RdbSqliteSharedResultSetTest::store->GetPath();
    std::string path1 = pBk->Name();

    EXPECT_EQ(path,  "/data/test/shared_test.db");
    EXPECT_EQ(path1, "/data/test/shared_test.db");

    int rowCnt = 0;
    pSqlSharedRstSet->GetRowCount(rowCnt);
    int rowCntBk = pBk->GetRowNum();

    EXPECT_EQ(rowCnt, rowCntBk);

    rstSet->Close();
    bool isClosedFlag = rstSet->IsClosed();
    EXPECT_EQ(isClosedFlag, true);
}

/* *
 * @tc.name: Sqlite_Shared_Result_Set_017
 * @tc.desc: normal testcase of SqliteSharedResultSet for setExtensions and getExtensions
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbSqliteSharedResultSetTest, Sqlite_Shared_Result_Set_017, TestSize.Level1)
{
    GenerateDefaultTable();
    std::vector<std::string> selectionArgs;
    std::unique_ptr<ResultSet> rstSet =
        RdbSqliteSharedResultSetTest::store->QuerySql("SELECT * FROM test", selectionArgs);
    EXPECT_NE(rstSet, nullptr);

    int rowCnt = 0;
    rstSet->GetRowCount(rowCnt);
    EXPECT_EQ(rowCnt, 3);
    int ret = rstSet->GoToLastRow();
    EXPECT_EQ(ret, E_OK);
}
