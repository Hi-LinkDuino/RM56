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

class RdbUpgradeTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void InsertValues(std::shared_ptr<RdbStore> &store);

    static const std::string DATABASE_NAME;
    static std::shared_ptr<RdbStore> store;
};

const std::string RdbUpgradeTest::DATABASE_NAME = RDB_TEST_PATH + "upgrade_test.db";
std::shared_ptr<RdbStore> RdbUpgradeTest::store = nullptr;

class UpgradeTestOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

std::string const UpgradeTestOpenCallback::CREATE_TABLE_TEST = std::string("CREATE TABLE IF NOT EXISTS test ")
                                                               + std::string("(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                             "name TEXT NOT NULL, age INTEGER, salary "
                                                                             "REAL, blobType BLOB)");

int UpgradeTestOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int UpgradeTestOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

class UpgradeOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string UPGRADE_TABLE_TEST;
};

std::string const UpgradeOpenCallback::UPGRADE_TABLE_TEST = std::string("ALTER TABLE test ADD COLUMN address TEXT;");

int UpgradeOpenCallback::OnCreate(RdbStore &store)
{
    return E_OK;
}

int UpgradeOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return store.ExecuteSql(UPGRADE_TABLE_TEST);
}

void RdbUpgradeTest::SetUpTestCase(void)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbUpgradeTest::DATABASE_NAME);
    UpgradeTestOpenCallback helper;
    RdbUpgradeTest::store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(RdbUpgradeTest::store, nullptr);
}

void RdbUpgradeTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbUpgradeTest::DATABASE_NAME);
}

void RdbUpgradeTest::SetUp(void)
{
    store->ExecuteSql("DELETE FROM test");
}

void RdbUpgradeTest::TearDown(void)
{
}

/**
 * @tc.name: RdbStore_Upgrade_001
 * @tc.desc: test RdbStore upgrade
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: liumengyue
 */
HWTEST_F(RdbUpgradeTest, RdbStore_Upgrade_001, TestSize.Level1)
{
    std::shared_ptr<RdbStore> &store = RdbUpgradeTest::store;

    RdbUpgradeTest::InsertValues(store);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);
    int ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);

    int errCode = E_OK;
    RdbStoreConfig config(RdbUpgradeTest::DATABASE_NAME);
    UpgradeOpenCallback callBack;
    std::shared_ptr<RdbStore> upgradeStore = RdbHelper::GetRdbStore(config, 2, callBack, errCode);
    EXPECT_NE(upgradeStore, nullptr);
    std::unique_ptr<ResultSet> upgradeResultSet = upgradeStore->QuerySql("SELECT * FROM test");
    EXPECT_NE(upgradeResultSet, nullptr);
    int columnCount;
    ret = upgradeResultSet->GetColumnCount(columnCount);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(columnCount, 6);
    ret = upgradeResultSet->GoToNextRow();
    int columnIndex;
    ret = upgradeResultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = upgradeResultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = upgradeResultSet->GetColumnIndex("age", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = upgradeResultSet->GetColumnIndex("salary", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = upgradeResultSet->GetColumnIndex("blobType", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = upgradeResultSet->GetColumnIndex("address", columnIndex);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(columnIndex, 5);
    ret = upgradeResultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = upgradeResultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = upgradeResultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = upgradeResultSet->Close();
    EXPECT_EQ(ret, E_OK);
}

void RdbUpgradeTest::InsertValues(std::shared_ptr<RdbStore> &store)
{
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
}
