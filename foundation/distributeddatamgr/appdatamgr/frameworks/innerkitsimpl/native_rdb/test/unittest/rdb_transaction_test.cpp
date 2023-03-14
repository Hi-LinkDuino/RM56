/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

class RdbTransactionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static const std::string DATABASE_NAME;
    static std::shared_ptr<RdbStore> store;
    static const int E_SQLITE_ERROR; // errno SQLITE_ERROR
};

const std::string RdbTransactionTest::DATABASE_NAME = RDB_TEST_PATH + "transaction_test.db";
std::shared_ptr<RdbStore> RdbTransactionTest::store = nullptr;
const int RdbTransactionTest::E_SQLITE_ERROR = -1; // errno SQLITE_ERROR

class TransactionTestOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

const std::string TransactionTestOpenCallback::CREATE_TABLE_TEST = std::string("CREATE TABLE IF NOT EXISTS test ")
                                                                   + std::string("(id INTEGER PRIMARY KEY "
                                                                                 "AUTOINCREMENT, name TEXT NOT NULL, "
                                                                                 "age INTEGER, salary REAL, blobType "
                                                                                 "BLOB)");

int TransactionTestOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int TransactionTestOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbTransactionTest::SetUpTestCase(void)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbTransactionTest::DATABASE_NAME);
    TransactionTestOpenCallback helper;
    RdbTransactionTest::store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(RdbTransactionTest::store, nullptr);
    EXPECT_EQ(errCode, E_OK);
}

void RdbTransactionTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbTransactionTest::DATABASE_NAME);
}

void RdbTransactionTest::SetUp(void)
{
    store->ExecuteSql("DELETE FROM test");
}

void RdbTransactionTest::TearDown(void)
{
}

/**
 * @tc.name: RdbStore_Transaction_001
 * @tc.desc: test RdbStore BaseTransaction
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbTransactionTest, RdbStore_Transaction_001, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbTransactionTest::store;

    int64_t id;
    ValuesBucket values;

    int ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 19);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 4, 5, 6 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("name", std::string("wangyjing"));
    values.PutInt("age", 20);
    values.PutDouble("salary", 300.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 7, 8, 9 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    ret = store->Commit();
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    int deletedRows;
    ret = store->Delete(deletedRows, "test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(deletedRows, 3);
}

/**
 * @tc.name: RdbStore_Transaction_002
 * @tc.desc: test RdbStore BaseTransaction
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbTransactionTest, RdbStore_Transaction_002, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbTransactionTest::store;

    int64_t id;
    ValuesBucket values;

    int ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 19);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 4, 5, 6 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("name", std::string("wangyjing"));
    values.PutInt("age", 20);
    values.PutDouble("salary", 300.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 7, 8, 9 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    ret = store->Commit();
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);

    int deletedRows;
    ret = store->Delete(deletedRows, "test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(deletedRows, 3);
}

/**
 * @tc.name: RdbStore_NestedTransaction_001
 * @tc.desc: test RdbStore BaseTransaction
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbTransactionTest, RdbStore_NestedTransaction_001, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbTransactionTest::store;

    int64_t id;
    ValuesBucket values;

    int ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);
    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 19);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 4, 5, 6 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);
    ret = store->Commit(); // not commit
    EXPECT_EQ(ret, E_OK);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("name", std::string("wangyjing"));
    values.PutInt("age", 20);
    values.PutDouble("salary", 300.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 7, 8, 9 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    ret = store->Commit();
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);

    int deletedRows;
    ret = store->Delete(deletedRows, "test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(deletedRows, 3);
}

/**
 * @tc.name: RdbStore_NestedTransaction_002
 * @tc.desc: test RdbStore BaseTransaction
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbTransactionTest, RdbStore_NestedTransaction_002, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbTransactionTest::store;

    int64_t id;
    ValuesBucket values;

    int ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);
    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 19);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 4, 5, 6 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);
    ret = store->Commit();
    EXPECT_EQ(ret, E_OK);
    ret = store->Commit(); // commit
    EXPECT_EQ(ret, E_OK);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("name", std::string("wangyjing"));
    values.PutInt("age", 20);
    values.PutDouble("salary", 300.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 7, 8, 9 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);

    int deletedRows;
    ret = store->Delete(deletedRows, "test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(deletedRows, 3);
}

/**
 * @tc.name: RdbStore_NestedTransaction_003
 * @tc.desc: test RdbStore BaseTransaction
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbTransactionTest, RdbStore_NestedTransaction_003, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbTransactionTest::store;

    int64_t id;
    ValuesBucket values;

    int ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);
    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 19);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 4, 5, 6 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);
    ret = store->Commit(); // not commit
    EXPECT_EQ(ret, E_OK);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("name", std::string("wangyjing"));
    values.PutInt("age", 20);
    values.PutDouble("salary", 300.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 7, 8, 9 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    ret = store->Commit(); // not commit
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);

    int deletedRows;
    ret = store->Delete(deletedRows, "test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(deletedRows, 3);
}

/**
 * @tc.name: RdbStore_NestedTransaction_004
 * @tc.desc: test RdbStore BaseTransaction
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbTransactionTest, RdbStore_NestedTransaction_004, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbTransactionTest::store;

    int64_t id;
    ValuesBucket values;

    int ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);
    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 19);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 4, 5, 6 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);
    ret = store->Commit(); // commit
    EXPECT_EQ(ret, E_OK);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("name", std::string("wangyjing"));
    values.PutInt("age", 20);
    values.PutDouble("salary", 300.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 7, 8, 9 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    ret = store->Commit(); // commit
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);

    int deletedRows;
    ret = store->Delete(deletedRows, "test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(deletedRows, 3);
}