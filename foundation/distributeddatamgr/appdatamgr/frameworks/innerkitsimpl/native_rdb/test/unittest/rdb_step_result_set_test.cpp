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
#include "common.h"
#include "logger.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;

class RdbStepResultSetTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void GenerateDefaultTable();
    void GenerateDefaultEmptyTable();

    static const std::string DATABASE_NAME;
    static std::shared_ptr<RdbStore> store;
    static const int E_SQLITE_ERROR;
    static const int E_INVALID_COLUMN_TYPE;
};

const std::string RdbStepResultSetTest::DATABASE_NAME = RDB_TEST_PATH + "stepResultSet_test.db";
std::shared_ptr<RdbStore> RdbStepResultSetTest::store = nullptr;
const int RdbStepResultSetTest::E_SQLITE_ERROR = -1;          // errno SQLITE_ERROR
const int RdbStepResultSetTest::E_INVALID_COLUMN_TYPE = 1009; // errno SQLITE_NULL

class RdbStepResultSetOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

int RdbStepResultSetOpenCallback::OnCreate(RdbStore &store)
{
    return E_OK;
}

int RdbStepResultSetOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbStepResultSetTest::SetUpTestCase(void)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbStepResultSetTest::DATABASE_NAME);
    RdbStepResultSetOpenCallback helper;
    RdbStepResultSetTest::store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(RdbStepResultSetTest::store, nullptr);
    EXPECT_EQ(errCode, E_OK);
}

void RdbStepResultSetTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbStepResultSetTest::DATABASE_NAME);
}

void RdbStepResultSetTest::SetUp(void)
{
    store->ExecuteSql("DELETE FROM test");
}

void RdbStepResultSetTest::TearDown(void)
{
    RdbHelper::ClearCache();
}

void RdbStepResultSetTest::GenerateDefaultTable()
{
    std::string createTableSql = std::string("CREATE TABLE test (id INTEGER PRIMARY KEY AUTOINCREMENT, data1 TEXT, ") +
        std::string("data2 INTEGER, data3 FLOAT, data4 BLOB);");
    store->ExecuteSql(createTableSql);

    std::string insertSql = "INSERT INTO test (data1, data2, data3, data4) VALUES (?, ?, ?, ?);";

    /* insert first entry data */
    uint8_t uValue = 66;
    std::vector<uint8_t> typeBlob;
    typeBlob.push_back(uValue);
    store->ExecuteSql(insertSql, std::vector<ValueObject> {
            ValueObject(std::string("hello")), ValueObject((int)10),
            ValueObject((double)1.0), ValueObject((std::vector<uint8_t>)typeBlob)
        });

    /* insert second entry data */
    typeBlob.clear();
    store->ExecuteSql(insertSql, std::vector<ValueObject> {
            ValueObject(std::string("2")), ValueObject((int)-5),
            ValueObject((double)2.5), ValueObject() // set double value 2.5
        });

    /* insert third entry data */
    store->ExecuteSql(insertSql, std::vector<ValueObject> {
            ValueObject(std::string("hello world")),
            ValueObject((int)3), ValueObject((double)1.8), ValueObject() // set int value 3, double 1.8
        });
}

void RdbStepResultSetTest::GenerateDefaultEmptyTable()
{
    std::string createTableSql = std::string("CREATE TABLE test (id INTEGER PRIMARY KEY AUTOINCREMENT, data1 TEXT, ") +
        std::string("data2 INTEGER, data3 FLOAT, data4 BLOB);");
    store->ExecuteSql(createTableSql);
}

/* *
 * @tc.name: RdbStore_StepResultSet_001
 * @tc.desc: test StepResultSet
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_001, TestSize.Level1)
{
    GenerateDefaultTable();

    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    bool bResultSet = false;
    int iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);
    EXPECT_EQ(resultSet->GoTo(1), E_OK);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    ColumnType type;
    iRet = resultSet->GetColumnType(0, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_INTEGER, type);

    iRet = resultSet->GetColumnType(1, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_STRING, type);

    iRet = resultSet->GetColumnType(2, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_INTEGER, type);

    iRet = resultSet->GetColumnType(3, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_FLOAT, type);

    iRet = resultSet->GetColumnType(4, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_BLOB, type);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    EXPECT_EQ(E_OK, resultSet->GoToNextRow());

    int position = -1;
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, position);
    int count = -1;
    iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);

    std::string stringValue;
    iRet = resultSet->GetString(1, stringValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("2", stringValue);

    int iValue;
    iRet = resultSet->GetInt(2, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-5, iValue);

    double dValue;
    iRet = resultSet->GetDouble(3, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2.5, dValue);

    std::vector<uint8_t> blobValue;
    iRet = resultSet->GetBlob(4, blobValue);
    EXPECT_EQ(E_OK, iRet);
}

/* *
 * @tc.name: RdbStore_StepResultSet_002
 * @tc.desc: normal testcase of StepResultSet
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_002, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    int count = -1;
    resultSet->GetRowCount(count);
    EXPECT_EQ(3, count);

    int position = -1;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    bool bResultSet = false;
    resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(bResultSet, false);

    EXPECT_EQ(E_OK, resultSet->GoToRow(2));

    iRet = resultSet->IsAtLastRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(true, bResultSet);

    EXPECT_EQ(E_OK, resultSet->GoToPreviousRow());

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, position);

    EXPECT_EQ(E_OK, resultSet->GoToLastRow());

    iRet = resultSet->IsAtLastRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(true, bResultSet);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);
}

/* *
 * @tc.name: RdbStore_StepResultSet_003
 * @tc.desc: normal testcase of StepResultSet
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_003, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    int position = -1;
    bool bResultSet = false;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    int moveTimes = 0;
    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    moveTimes++;

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);
    while (E_OK == resultSet->GoToNextRow()) {
        moveTimes++;
    }
    /* Cursor is before first */

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);
}

/* *
 * @tc.name: RdbStore_StepResultSet_004
 * @tc.desc: normal testcase of StepResultSet
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_004, TestSize.Level1)
{
    GenerateDefaultEmptyTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    int position = -1;
    bool bResultSet = false;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    EXPECT_NE(E_OK, resultSet->GoToNextRow());

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);
}

/* *
 * @tc.name: RdbStore_StepResultSet_005
 * @tc.desc: normal testcase of StepResultSet
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_005, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());

    int position = -1;
    bool bResultSet = false;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);
}

/* *
 * @tc.name: RdbStore_StepResultSet_006
 * @tc.desc: normal testcase of StepResultSet for moveFirstWithoutEntry
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_006, TestSize.Level1)
{
    GenerateDefaultEmptyTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    EXPECT_NE(E_OK, resultSet->GoToFirstRow());

    int position = -1;
    bool bResultSet = false;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    EXPECT_NE(E_OK, resultSet->GoToNextRow());
    EXPECT_NE(E_OK, resultSet->GoToFirstRow());

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);
}

/* *
 * @tc.name: RdbStore_StepResultSet_007
 * @tc.desc: normal testcase of StepResultSet for goToNextRow
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_007, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    int moveTimes = 0;
    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    moveTimes++;
    int position = -1;
    bool bResultSet = false;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    while (E_OK == resultSet->GoToNextRow()) {
        moveTimes++;
    }
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);
}
/* *
 * @tc.name: RdbStore_StepResultSet_008
 * @tc.desc: normal testcase of StepResultSet for moveNextWithoutEntry
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_008, TestSize.Level1)
{
    GenerateDefaultEmptyTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    int moveTimes = 0;

    EXPECT_NE(E_OK, resultSet->GoToFirstRow());
    moveTimes++;
    int position = -1;
    bool bResultSet = false;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    while (E_OK == resultSet->GoToNextRow()) {
        moveTimes++;
    }
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);
}
/* *
 * @tc.name: RdbStore_StepResultSet_009
 * @tc.desc: normal testcase of StepResultSet for getInt
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_009, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    int iValue;
    int iRet = resultSet->GetInt(0, iValue);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());

    iRet = resultSet->GetInt(0, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, iValue);

    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(10, iValue);

    iRet = resultSet->GetInt(2, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, iValue);

    iRet = resultSet->GetInt(3, iValue);
    EXPECT_NE(E_OK, iRet);

    int columnCount = 0;
    iRet = resultSet->GetColumnCount(columnCount);
    EXPECT_EQ(4, columnCount);
    iRet = resultSet->GetInt(columnCount, iValue);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    iRet = resultSet->GetInt(0, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2, iValue);

    int64_t longValue;
    iRet = resultSet->GetLong(0, longValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2, longValue);

    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-5, iValue);

    iRet = resultSet->GetLong(1, longValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-5, longValue);

    iRet = resultSet->GetInt(2, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2, iValue);
}
/* *
 * @tc.name: RdbStore_StepResultSet_010
 * @tc.desc: normal testcase of StepResultSet for getString
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_010, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    std::string strValue;
    int iRet = resultSet->GetString(0, strValue);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());

    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("hello", strValue);
    iRet = resultSet->GetString(1, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("10", strValue);
    iRet = resultSet->GetString(2, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("1", strValue);

    iRet = resultSet->GetString(3, strValue);
    EXPECT_NE(E_OK, iRet);

    int columnCount = 0;
    iRet = resultSet->GetColumnCount(columnCount);
    EXPECT_EQ(4, columnCount);
    iRet = resultSet->GetString(columnCount, strValue);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("2", strValue);
    iRet = resultSet->GetString(1, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("-5", strValue);
    iRet = resultSet->GetString(2, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("2.5", strValue);
}

/* *
 * @tc.name: RdbStore_StepResultSet_011
 * @tc.desc: normal testcase of StepResultSet for GetDouble
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_011, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    double dValue;
    int iRet = resultSet->GetDouble(0, dValue);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    iRet = resultSet->GetDouble(0, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0.0, dValue);
    iRet = resultSet->GetDouble(1, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(10.0, dValue);

    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1.0, dValue);

    iRet = resultSet->GetDouble(3, dValue);
    EXPECT_NE(E_OK, iRet);
    int columnCount = 0;
    iRet = resultSet->GetColumnCount(columnCount);
    EXPECT_EQ(4, columnCount);
    iRet = resultSet->GetDouble(columnCount, dValue);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    iRet = resultSet->GetDouble(0, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2.0, dValue);
    iRet = resultSet->GetDouble(1, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-5.0, dValue);
    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2.5, dValue);
}

/* *
 * @tc.name: RdbStore_StepResultSet_012
 * @tc.desc: normal testcase of StepResultSet for getBlob
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_012, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    std::vector<uint8_t> blobValue;
    int iRet = resultSet->GetBlob(0, blobValue);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    iRet = resultSet->GetBlob(0, blobValue);
    EXPECT_EQ(E_OK, iRet);

    string strBlob;
    for (size_t i = 0; i < blobValue.size(); i++) {
        strBlob += char(blobValue[i]);
    }
    EXPECT_EQ("hello", strBlob);


    iRet = resultSet->GetBlob(1, blobValue);
    EXPECT_NE(E_OK, iRet);

    iRet = resultSet->GetBlob(2, blobValue);
    EXPECT_NE(E_OK, iRet);

    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);

    strBlob.clear();
    for (size_t i = 0; i < blobValue.size(); i++) {
        strBlob += char(blobValue[i]);
    }
    char cValue = 66;
    string strTmpValue(1, cValue);
    EXPECT_EQ(strTmpValue, strBlob);

    int columnCount = 0;
    iRet = resultSet->GetColumnCount(columnCount);
    EXPECT_EQ(4, columnCount);
    iRet = resultSet->GetBlob(columnCount, blobValue);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);
}


/* *
 * @tc.name: RdbStore_StepResultSet_013
 * @tc.desc: normal testcase of StepResultSet for getBlob
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_013, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    ColumnType type;
    int iRet = resultSet->GetColumnType(0, type);
    EXPECT_NE(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    iRet = resultSet->GetColumnType(0, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_INTEGER, type);

    iRet = resultSet->GetColumnType(1, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_STRING, type);

    iRet = resultSet->GetColumnType(2, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_INTEGER, type);

    iRet = resultSet->GetColumnType(3, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_FLOAT, type);

    iRet = resultSet->GetColumnType(4, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_BLOB, type);

    int columnCount = 0;
    iRet = resultSet->GetColumnCount(columnCount);
    EXPECT_EQ(5, columnCount);
    iRet = resultSet->GetColumnType(columnCount, type);
    EXPECT_NE(E_OK, iRet);
}

/* *
 * @tc.name: RdbStore_StepResultSet_014
 * @tc.desc: normal testcase of StepResultSet for getColumnIndexForName
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_014, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    int columnIndex;
    int iRet = resultSet->GetColumnIndex("data1", columnIndex);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, columnIndex);

    iRet = resultSet->GetColumnIndex("data2", columnIndex);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2, columnIndex);

    iRet = resultSet->GetColumnIndex("data3", columnIndex);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, columnIndex);

    iRet = resultSet->GetColumnIndex("data4", columnIndex);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(4, columnIndex);

    iRet = resultSet->GetColumnIndex("jank.data1", columnIndex);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, columnIndex);

    iRet = resultSet->GetColumnIndex("datax", columnIndex);
    EXPECT_EQ(E_ERROR, iRet);
    EXPECT_EQ(-1, columnIndex);
}

/* *
 * @tc.name: RdbStore_StepResultSet_015
 * @tc.desc: normal testcase of StepResultSet for getColumnNameForIndex
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_015, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    std::vector<std::string> allColumnNames;
    int iRet = resultSet->GetAllColumnNames(allColumnNames);
    EXPECT_EQ(E_OK, iRet);

    std::string columnName;
    iRet = resultSet->GetColumnName(1, columnName);
    EXPECT_EQ("data1", columnName);
    EXPECT_EQ(allColumnNames[1], columnName);

    iRet = resultSet->GetColumnName(2, columnName);
    EXPECT_EQ("data2", columnName);
    EXPECT_EQ(allColumnNames[2], columnName);

    iRet = resultSet->GetColumnName(3, columnName);
    EXPECT_EQ("data3", columnName);
    EXPECT_EQ(allColumnNames[3], columnName);

    iRet = resultSet->GetColumnName(4, columnName);
    EXPECT_EQ("data4", columnName);
    EXPECT_EQ(allColumnNames[4], columnName);

    int columnCount = 0;
    iRet = resultSet->GetColumnCount(columnCount);
    EXPECT_EQ(5, columnCount);
    iRet = resultSet->GetColumnName(columnCount, columnName);
    EXPECT_NE(E_OK, iRet);
}

/* *
 * @tc.name: RdbStore_StepResultSet_016
 * @tc.desc: normal testcase of StepResultSet
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, RdbStore_StepResultSet_016, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    bool bResultSet = false;
    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    int iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    int iValue;
    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(10, iValue);

    int64_t longValue;
    iRet = resultSet->GetLong(1, longValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(10, longValue);
}

/* *
 * @tc.name: testGetRowCount003
 * @tc.desc: normal testcase of StepResultSet for getRowCount
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, testGetRowCount003, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    bool bResultSet = false;
    int iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    int count = -1;
    iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);
    EXPECT_EQ(E_OK, resultSet->GoToNextRow());

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    std::string strValue;
    iRet = resultSet->GetString(1, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("hello", strValue);

    int iValue;
    iRet = resultSet->GetInt(2, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(10, iValue);

    double dValue;
    iRet = resultSet->GetDouble(3, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1.0, dValue);

    std::vector<uint8_t> blobValue;
    iRet = resultSet->GetBlob(4, blobValue);
    EXPECT_EQ(E_OK, iRet);

    std::string strBlob;
    strBlob.clear();
    for (size_t i = 0; i < blobValue.size(); i++) {
        strBlob += char(blobValue[i]);
    }
    char cValue = 66;
    string strTmpValue(1, cValue);
    EXPECT_EQ(strTmpValue, strBlob);

    iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    int position = -1;
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, position);

    iRet = resultSet->GetString(1, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("2", strValue);

    iRet = resultSet->GetInt(2, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-5, iValue);

    iRet = resultSet->GetDouble(3, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2.5, dValue);

    iRet = resultSet->GetBlob(4, blobValue);
    EXPECT_EQ(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2, position);

    iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);
}

/* *
 * @tc.name: testGetRowCount004
 * @tc.desc: normal testcase of StepResultSet for getRowCount
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, testGetRowCount004, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    bool bResultSet = false;
    int iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);
    EXPECT_EQ(E_OK, resultSet->GoToNextRow());

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    int count = -1;
    iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);

    std::string strValue;
    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("hello", strValue);

    int iValue;
    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(10, iValue);

    double dValue;
    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1.0, dValue);

    std::vector<uint8_t> blobValue;
    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);

    std::string strBlob;
    strBlob.clear();
    for (size_t i = 0; i < blobValue.size(); i++) {
        strBlob += char(blobValue[i]);
    }
    char cValue = 66;
    string strTmpValue(1, cValue);
    EXPECT_EQ(strTmpValue, strBlob);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());

    int position = -1;
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, position);

    EXPECT_EQ(E_OK, resultSet->GoToNextRow());

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2, position);

    iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);

    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("hello world", strValue);

    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, iValue);

    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1.8, dValue);

    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);
}

/* *
 * @tc.name: testGoToRow005
 * @tc.desc: normal testcase of StepResultSet for goToRow
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, testGoToRow005, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    bool bResultSet = false;
    int iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);
    EXPECT_EQ(E_OK, resultSet->GoToNextRow());

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    int position = -1;
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);

    EXPECT_EQ(E_OK, resultSet->GoToRow(2));

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2, position);

    std::string strValue;
    int iValue;
    double dValue;
    std::vector<uint8_t> blobValue;
    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("hello world", strValue);

    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, iValue);

    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1.8, dValue);

    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToRow(1));

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, position);


    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("2", strValue);

    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-5, iValue);

    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2.5, dValue);

    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);
}

/* *
 * @tc.name: testGo006
 * @tc.desc: normal testcase of StepResultSet for goToRow
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, testGo006, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    int position = -1;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    int count = -1;
    iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);

    EXPECT_EQ(resultSet->GoTo(2), E_OK);

    std::string strValue;
    int iValue;
    double dValue;
    std::vector<uint8_t> blobValue;
    iRet = resultSet->GetString(1, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("hello world", strValue);

    iRet = resultSet->GetInt(2, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, iValue);

    iRet = resultSet->GetDouble(3, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1.8, dValue);

    iRet = resultSet->GetBlob(4, blobValue);
    EXPECT_EQ(E_OK, iRet);

    EXPECT_EQ(resultSet->GoTo(-2), E_OK);

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);

    iRet = resultSet->GetString(1, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("hello", strValue);

    iRet = resultSet->GetInt(2, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(10, iValue);

    iRet = resultSet->GetDouble(3, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1.0, dValue);

    iRet = resultSet->GetBlob(4, blobValue);
    EXPECT_EQ(E_OK, iRet);

    std::string strBlob;
    strBlob.clear();
    for (size_t i = 0; i < blobValue.size(); i++) {
        strBlob += char(blobValue[i]);
    }
    char cValue = 66;
    string strTmpValue(1, cValue);
    EXPECT_EQ(strTmpValue, strBlob);
}

/* *
 * @tc.name: testGoToPrevious007
 * @tc.desc: normal testcase of StepResultSet for go
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, testGoToPrevious007, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    int count = -1;
    int iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);

    bool bResultSet = false;
    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    std::string strValue;
    int iValue;
    double dValue;
    std::vector<uint8_t> blobValue;

    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("hello", strValue);

    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(10, iValue);

    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1.0, dValue);

    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);

    std::string strBlob;
    strBlob.clear();
    for (size_t i = 0; i < blobValue.size(); i++) {
        strBlob += char(blobValue[i]);
    }
    char cValue = 66;
    string strTmpValue(1, cValue);
    EXPECT_EQ(strTmpValue, strBlob);

    int ret = resultSet->GoToPreviousRow();
    EXPECT_NE(E_OK, ret);

    int position = -1;
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    EXPECT_EQ(resultSet->GoTo(2), E_OK);

    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("2", strValue);

    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-5, iValue);

    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2.5, dValue);

    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);

    EXPECT_EQ(E_OK, resultSet->GoToLastRow());

    iRet = resultSet->IsAtLastRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(true, bResultSet);

    EXPECT_EQ(E_OK, resultSet->GoToPreviousRow());

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, position);

    EXPECT_NE(E_OK, resultSet->GoTo(3));

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);
}

/* *
 * @tc.name: testSqlStep008
 * @tc.desc: normal testcase of SqlStep for go
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, testSqlStep008, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    bool bResultSet = false;
    int iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    EXPECT_EQ(E_OK, resultSet->GoTo(1));

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    ColumnType type;
    iRet = resultSet->GetColumnType(0, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_STRING, type);

    iRet = resultSet->GetColumnType(1, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_INTEGER, type);

    iRet = resultSet->GetColumnType(2, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_FLOAT, type);

    iRet = resultSet->GetColumnType(3, type);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(ColumnType::TYPE_BLOB, type);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    EXPECT_EQ(E_OK, resultSet->GoToNextRow());

    int position = -1;
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, position);

    int count = -1;
    iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);

    std::string strValue;
    int iValue;
    double dValue;
    std::vector<uint8_t> blobValue;

    iRet = resultSet->GetString(0, strValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ("2", strValue);

    iRet = resultSet->GetInt(1, iValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-5, iValue);

    iRet = resultSet->GetDouble(2, dValue);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(2.5, dValue);

    iRet = resultSet->GetBlob(3, blobValue);
    EXPECT_EQ(E_OK, iRet);
}

/* *
 * @tc.name: testSqlStep009
 * @tc.desc: normal testcase of SqlStep for go
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, testSqlStep009, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    int count = -1;
    int iRet = resultSet->GetRowCount(count);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(3, count);

    int position = -1;
    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    bool bResultSet = false;
    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    EXPECT_EQ(E_OK, resultSet->GoToRow(2));

    iRet = resultSet->IsAtLastRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(true, bResultSet);

    EXPECT_EQ(E_OK, resultSet->GoToPreviousRow());

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(1, position);

    EXPECT_EQ(E_OK, resultSet->GoToLastRow());

    iRet = resultSet->IsAtLastRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(true, bResultSet);

    iRet = resultSet->IsAtLastRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(true, bResultSet);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);
}

/* *
 * @tc.name: testSqlStep010
 * @tc.desc: normal testcase of SqlStep for go
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RdbStepResultSetTest, testSqlStep010, TestSize.Level1)
{
    GenerateDefaultTable();
    std::unique_ptr<ResultSet> resultSet = store->QueryByStep("SELECT data1, data2, data3, data4 FROM test");
    EXPECT_NE(resultSet, nullptr);

    int position = -1;
    int iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);
    bool bResultSet = false;
    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    int moveTimes = 0;
    EXPECT_EQ(E_OK, resultSet->GoToNextRow());
    moveTimes++;

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(0, position);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    while (E_OK == resultSet->GoToNextRow()) {
        moveTimes++;
    }

    iRet = resultSet->GetRowIndex(position);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(-1, position);

    iRet = resultSet->IsStarted(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsAtFirstRow(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, false);

    iRet = resultSet->IsEnded(bResultSet);
    EXPECT_EQ(E_OK, iRet);
    EXPECT_EQ(bResultSet, true);
}
