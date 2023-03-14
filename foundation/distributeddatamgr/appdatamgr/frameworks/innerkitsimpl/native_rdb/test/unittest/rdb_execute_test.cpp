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

class RdbExecuteTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static const std::string DATABASE_NAME;
    static std::shared_ptr<RdbStore> store;
    static const int E_SQLITE_ERROR; // errno SQLITE_ERROR
};

const std::string RdbExecuteTest::DATABASE_NAME = RDB_TEST_PATH + "execute_test.db";
std::shared_ptr<RdbStore> RdbExecuteTest::store = nullptr;
const int RdbExecuteTest::E_SQLITE_ERROR = -1; // errno SQLITE_ERROR

class ExecuteTestOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

const std::string ExecuteTestOpenCallback::CREATE_TABLE_TEST = std::string("CREATE TABLE IF NOT EXISTS test ")
                                                               + std::string("(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                             "name TEXT NOT NULL, age INTEGER, salary "
                                                                             "REAL, blobType BLOB)");

int ExecuteTestOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int ExecuteTestOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbExecuteTest::SetUpTestCase(void)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbExecuteTest::DATABASE_NAME);
    ExecuteTestOpenCallback helper;
    RdbExecuteTest::store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(RdbExecuteTest::store, nullptr);
    EXPECT_EQ(errCode, E_OK);
}

void RdbExecuteTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbExecuteTest::DATABASE_NAME);
}

void RdbExecuteTest::SetUp(void)
{
    store->ExecuteSql("DELETE FROM test");
}

void RdbExecuteTest::TearDown(void)
{
}

/**
 * @tc.name: RdbStore_Execute_001
 * @tc.desc: test RdbStore Execute
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbExecuteTest, RdbStore_Execute_001, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbExecuteTest::store;

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

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    ret = store->ExecuteSql("DELETE FROM test WHERE age = ? OR age = ?",
        std::vector<ValueObject>{ ValueObject(std::string("18")), ValueObject(std ::string("20")) });
    EXPECT_EQ(ret, E_OK);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test where age = 19");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 1);

    ret = store->ExecuteSql("DELETE FROM test WHERE age = 19");
    EXPECT_EQ(ret, E_OK);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 0);
}

/**
 * @tc.name: RdbStore_Execute_002
 * @tc.desc: test RdbStore Execute
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbExecuteTest, RdbStore_Execute_002, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbExecuteTest::store;

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
    values.PutInt("age", 19);
    values.PutDouble("salary", 200.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 4, 5, 6 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(2, id);

    values.Clear();
    values.PutInt("id", 3);
    values.PutString("name", std::string("wangyjing"));
    values.PutInt("age", 20);
    values.PutDouble("salary", 300.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 7, 8, 9 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(3, id);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test", std::vector<ValueObject>());
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    ret = store->ExecuteSql("DELETE FROM test WHERE age = ? OR age = ?",
        std::vector<ValueObject>{ ValueObject(std::string("18")), ValueObject(std ::string("20")) });
    EXPECT_EQ(ret, E_OK);

    ret = store->ExecuteAndGetLong(
        count, "SELECT COUNT(*) FROM test where age = ?", std::vector<ValueObject>{ ValueObject(std::string("19")) });
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 1);

    ret = store->ExecuteSql("DELETE FROM test WHERE age = 19");
    EXPECT_EQ(ret, E_OK);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test", std::vector<ValueObject>());
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 0);

    ret = store->ExecuteSql("DROP TABLE IF EXISTS test");
    EXPECT_EQ(ret, E_OK);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: RdbStore_Execute_003
 * @tc.desc: test RdbStore Execute
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbExecuteTest, RdbStore_Execute_003, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbExecuteTest::store;

    int64_t pageSize;
    int ret = store->ExecuteAndGetLong(pageSize, "PRAGMA page_size");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(pageSize, 4096);

    std::string journalMode;
    ret = store->ExecuteAndGetString(journalMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(journalMode, "wal");
}
