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

class RdbDeleteTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static const std::string DATABASE_NAME;
    static std::shared_ptr<RdbStore> store;
    static const int E_SQLITE_ERROR; // errno SQLITE_ERROR
};

const std::string RdbDeleteTest::DATABASE_NAME = RDB_TEST_PATH + "delete_test.db";
std::shared_ptr<RdbStore> RdbDeleteTest::store = nullptr;
const int RdbDeleteTest::E_SQLITE_ERROR = -1; // errno SQLITE_ERROR

class DeleteTestOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

std::string const DeleteTestOpenCallback::CREATE_TABLE_TEST = std::string("CREATE TABLE IF NOT EXISTS test ")
                                                              + std::string("(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                            "name TEXT NOT NULL, age INTEGER, salary "
                                                                            "REAL, blobType BLOB)");

int DeleteTestOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int DeleteTestOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbDeleteTest::SetUpTestCase(void)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbDeleteTest::DATABASE_NAME);
    DeleteTestOpenCallback helper;
    RdbDeleteTest::store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(RdbDeleteTest::store, nullptr);
}

void RdbDeleteTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbDeleteTest::DATABASE_NAME);
}

void RdbDeleteTest::SetUp(void)
{
    store->ExecuteSql("DELETE FROM test");
}

void RdbDeleteTest::TearDown(void)
{
}

/**
 * @tc.name: RdbStore_Delete_001
 * @tc.desc: test RdbStore update, select id and update one row
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbDeleteTest, RdbStore_Delete_001, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbDeleteTest::store;

    int64_t id;
    ValuesBucket values;
    int deletedRows;

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

    ret = store->Delete(deletedRows, "test", "id = 1");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, deletedRows);

    std::unique_ptr<ResultSet> resultSet =
        store->QuerySql("SELECT * FROM test WHERE id = ?", std::vector<std::string>{ "1" });
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);

    resultSet = store->QuerySql("SELECT * FROM test WHERE id = ?", std::vector<std::string>{ "2" });
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToFirstRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);

    resultSet = store->QuerySql("SELECT * FROM test WHERE id = 3", std::vector<std::string>());
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToFirstRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: RdbStore_Delete_002
 * @tc.desc: test RdbStore update, select id and update one row
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbDeleteTest, RdbStore_Delete_002, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbDeleteTest::store;

    int64_t id;
    ValuesBucket values;
    int deletedRows;

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

    ret = store->Delete(deletedRows, "test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, deletedRows);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: RdbStore_Delete_003
 * @tc.desc: test RdbStore update, select id and update one row
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbDeleteTest, RdbStore_Delete_003, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbDeleteTest::store;

    int64_t id;
    ValuesBucket values;
    int deletedRows;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    ret = store->Delete(deletedRows, "", "id = ?", std::vector<std::string>{ "1" });
    EXPECT_EQ(ret, E_EMPTY_TABLE_NAME);

    ret = store->Delete(deletedRows, "wrongTable", "id = ?", std::vector<std::string>{ "1" });
    EXPECT_EQ(ret, RdbDeleteTest::E_SQLITE_ERROR);

    ret = store->Delete(deletedRows, "test", "wrong sql id = ?", std::vector<std::string>{ "1" });
    EXPECT_EQ(ret, RdbDeleteTest::E_SQLITE_ERROR);

    ret = store->Delete(deletedRows, "test", "id = 1", std::vector<std::string>());
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(deletedRows, 1);
}
