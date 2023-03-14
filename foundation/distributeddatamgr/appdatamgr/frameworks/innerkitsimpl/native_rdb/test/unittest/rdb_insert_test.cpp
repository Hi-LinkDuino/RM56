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

class RdbStoreInsertTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void CheckResultSet(std::shared_ptr<RdbStore> &store);
    void CheckAge(std::unique_ptr<ResultSet> &resultSet);
    void CheckSalary(std::unique_ptr<ResultSet> &resultSet);
    void CheckBlob(std::unique_ptr<ResultSet> &resultSet);

    static const std::string DATABASE_NAME;
    static std::shared_ptr<RdbStore> store;
    static const int E_SQLITE_ERROR;      // errno SQLITE_ERROR
    static const int E_SQLITE_CONSTRAINT; // errno SQLITE_CONSTRAINT
};

const std::string RdbStoreInsertTest::DATABASE_NAME = RDB_TEST_PATH + "insert_test.db";
std::shared_ptr<RdbStore> RdbStoreInsertTest::store = nullptr;
const int RdbStoreInsertTest::E_SQLITE_CONSTRAINT = -19;
const int RdbStoreInsertTest::E_SQLITE_ERROR = -1;

class InsertTestOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

const std::string InsertTestOpenCallback::CREATE_TABLE_TEST = std::string("CREATE TABLE IF NOT EXISTS test ")
                                                              + std::string("(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                            "name TEXT NOT NULL, age INTEGER, salary "
                                                                            "REAL, blobType BLOB)");

int InsertTestOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int InsertTestOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbStoreInsertTest::SetUpTestCase(void)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbStoreInsertTest::DATABASE_NAME);
    InsertTestOpenCallback helper;
    RdbStoreInsertTest::store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(RdbStoreInsertTest::store, nullptr);
}

void RdbStoreInsertTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbStoreInsertTest::DATABASE_NAME);
}

void RdbStoreInsertTest::SetUp(void)
{
    store->ExecuteSql("DELETE FROM test");
}

void RdbStoreInsertTest::TearDown(void)
{
}

/**
 * @tc.name: RdbStore_Insert_001
 * @tc.desc: test RdbStore insert
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_Insert_001, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 20L);
    values.PutDouble("salary", 100.5f);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    RdbStoreInsertTest::CheckResultSet(store);
}

void RdbStoreInsertTest::CheckResultSet(std::shared_ptr<RdbStore> &store)
{
    std::unique_ptr<ResultSet> resultSet =
        store->QuerySql("SELECT * FROM test WHERE name = ?", std::vector<std::string>{ "zhangsan" });
    EXPECT_NE(resultSet, nullptr);

    int columnIndex;
    int intVal;
    std::string strVal;
    ColumnType columnType;
    int position;
    int ret = resultSet->GetRowIndex(position);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(position, -1);

    ret = resultSet->GetColumnType(0, columnType);
    EXPECT_EQ(ret, E_ERROR);

    ret = resultSet->GoToFirstRow();
    EXPECT_EQ(ret, E_OK);

    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(columnIndex, 0);
    ret = resultSet->GetColumnType(columnIndex, columnType);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(columnType, ColumnType::TYPE_INTEGER);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, intVal);

    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetColumnType(columnIndex, columnType);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(columnType, ColumnType::TYPE_STRING);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ("zhangsan", strVal);

    RdbStoreInsertTest::CheckAge(resultSet);
    RdbStoreInsertTest::CheckSalary(resultSet);
    RdbStoreInsertTest::CheckBlob(resultSet);

    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);

    ret = resultSet->GetColumnType(columnIndex, columnType);
    EXPECT_EQ(ret, E_ERROR);

    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}

void RdbStoreInsertTest::CheckAge(std::unique_ptr<ResultSet> &resultSet)
{
    int columnIndex;
    int intVal;
    ColumnType columnType;
    int ret = resultSet->GetColumnIndex("age", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetColumnType(columnIndex, columnType);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(columnType, ColumnType::TYPE_INTEGER);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(18, intVal);
}

void RdbStoreInsertTest::CheckSalary(std::unique_ptr<ResultSet> &resultSet)
{
    int columnIndex;
    double dVal;
    ColumnType columnType;
    int ret = resultSet->GetColumnIndex("salary", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetColumnType(columnIndex, columnType);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(columnType, ColumnType::TYPE_FLOAT);
    ret = resultSet->GetDouble(columnIndex, dVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(100.5, dVal);
}

void RdbStoreInsertTest::CheckBlob(std::unique_ptr<ResultSet> &resultSet)
{
    int columnIndex;
    std::vector<uint8_t> blob;
    ColumnType columnType;
    int ret = resultSet->GetColumnIndex("blobType", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetColumnType(columnIndex, columnType);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(columnType, ColumnType::TYPE_BLOB);
    ret = resultSet->GetBlob(columnIndex, blob);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, static_cast<int>(blob.size()));
    EXPECT_EQ(1, blob[0]);
    EXPECT_EQ(2, blob[1]);
    EXPECT_EQ(3, blob[2]);
}

/**
 * @tc.name: RdbStore_Insert_002
 * @tc.desc: test RdbStore insert
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_Insert_002, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->Insert(id, "", values); // empty table name
    EXPECT_EQ(ret, E_EMPTY_TABLE_NAME);

    ret = store->Insert(id, "wrongTable", values); // no such table
    EXPECT_EQ(ret, RdbStoreInsertTest::E_SQLITE_ERROR);
}

/**
 * @tc.name: RdbStore_Insert_003
 * @tc.desc: test RdbStore insert
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_Insert_003, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket emptyBucket;
    int ret = store->Insert(id, "test", emptyBucket);
    EXPECT_EQ(ret, E_EMPTY_VALUES_BUCKET);

    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("wrongColumn", 100.5); // no such column
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, RdbStoreInsertTest::E_SQLITE_ERROR);
}

/**
 * @tc.name: RdbStore_Replace_001
 * @tc.desc: test RdbStore replace
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_Replace_001, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->Replace(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);

    int columnIndex;
    int intVal;
    std::string strVal;
    double dVal;
    std::vector<uint8_t> blob;

    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, intVal);

    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ("zhangsan", strVal);

    ret = resultSet->GetColumnIndex("age", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(18, intVal);

    ret = resultSet->GetColumnIndex("salary", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetDouble(columnIndex, dVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(100.5, dVal);

    ret = resultSet->GetColumnIndex("blobType", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetBlob(columnIndex, blob);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, static_cast<int>(blob.size()));
    EXPECT_EQ(1, blob[0]);
    EXPECT_EQ(2, blob[1]);
    EXPECT_EQ(3, blob[2]);

    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);

    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: RdbStore_Replace_002
 * @tc.desc: test RdbStore replace
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_Replace_002, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Replace(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);

    int columnIndex;
    int intVal;
    std::string strVal;
    double dVal;
    std::vector<uint8_t> blob;

    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, intVal);

    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ("zhangsan", strVal);

    ret = resultSet->GetColumnIndex("age", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(18, intVal);

    ret = resultSet->GetColumnIndex("salary", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetDouble(columnIndex, dVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(200.5, dVal);

    ret = resultSet->GetColumnIndex("blobType", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetBlob(columnIndex, blob);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, static_cast<int>(blob.size()));
    EXPECT_EQ(1, blob[0]);
    EXPECT_EQ(2, blob[1]);
    EXPECT_EQ(3, blob[2]);

    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);

    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: RdbStore_Replace_003
 * @tc.desc: test RdbStore Replace
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_Replace_003, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->Replace(id, "", values); // empty table name
    EXPECT_EQ(ret, E_EMPTY_TABLE_NAME);

    ret = store->Replace(id, "wrongTable", values); // no such table
    EXPECT_EQ(ret, RdbStoreInsertTest::E_SQLITE_ERROR);
}

/**
 * @tc.name: RdbStore_Replace_004
 * @tc.desc: test RdbStore Replace
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_Replace_004, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket emptyBucket;
    int ret = store->Replace(id, "test", emptyBucket);
    EXPECT_EQ(ret, E_EMPTY_VALUES_BUCKET);

    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("wrongColumn", 100.5); // no such column
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Replace(id, "test", values);
    EXPECT_EQ(ret, RdbStoreInsertTest::E_SQLITE_ERROR);
}

/**
 * @tc.name: RdbStore_InsertWithConflictResolution_001_002
 * @tc.desc: test RdbStore InsertWithConflictResolution
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_InsertWithConflictResolution_001_002, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });

    // default is ConflictResolution::ON_CONFLICT_NONE
    int ret = store->InsertWithConflictResolution(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->InsertWithConflictResolution(id, "test", values);
    EXPECT_EQ(ret, RdbStoreInsertTest::E_SQLITE_CONSTRAINT);
}

/**
 * @tc.name: RdbStore_InsertWithConflictResolution_003_004
 * @tc.desc: test RdbStore InsertWithConflictResolution
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_InsertWithConflictResolution_003_004, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->InsertWithConflictResolution(id, "test", values, ConflictResolution::ON_CONFLICT_ROLLBACK);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->InsertWithConflictResolution(id, "test", values, ConflictResolution::ON_CONFLICT_ROLLBACK);
    EXPECT_EQ(ret, RdbStoreInsertTest::E_SQLITE_CONSTRAINT);
}

/**
 * @tc.name: RdbStore_InsertWithConflictResolution_005
 * @tc.desc: test RdbStore InsertWithConflictResolution
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_InsertWithConflictResolution_005, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->InsertWithConflictResolution(id, "test", values, ConflictResolution::ON_CONFLICT_IGNORE);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->InsertWithConflictResolution(id, "test", values, ConflictResolution::ON_CONFLICT_IGNORE);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(id, -1);
}

/**
 * @tc.name: RdbStore_InsertWithConflictResolution_006_007
 * @tc.desc: test RdbStore InsertWithConflictResolution
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInsertTest, RdbStore_InsertWithConflictResolution_006_007, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbStoreInsertTest::store;

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->InsertWithConflictResolution(id, "test", values, ConflictResolution::ON_CONFLICT_REPLACE);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 4, 5, 6 });
    ret = store->InsertWithConflictResolution(id, "test", values, ConflictResolution::ON_CONFLICT_REPLACE);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(id, 1);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);

    int columnIndex;
    int intVal;
    std::string strVal;
    double dVal;
    std::vector<uint8_t> blob;

    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, intVal);

    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ("zhangsan", strVal);

    ret = resultSet->GetColumnIndex("age", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(18, intVal);

    ret = resultSet->GetColumnIndex("salary", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetDouble(columnIndex, dVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(200.5, dVal);

    ret = resultSet->GetColumnIndex("blobType", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetBlob(columnIndex, blob);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, static_cast<int>(blob.size()));
    EXPECT_EQ(4, blob[0]);
    EXPECT_EQ(5, blob[1]);
    EXPECT_EQ(6, blob[2]);

    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);

    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}
