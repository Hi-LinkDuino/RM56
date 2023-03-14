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

#include "rdb_open_callback.h"

#include <gtest/gtest.h>

#include <string>

#include "common.h"
#include "logger.h"
#include "rdb_errno.h"
#include "rdb_helper.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;

class RdbOpenCallbackTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static const std::string DATABASE_NAME;
};

const std::string RdbOpenCallbackTest::DATABASE_NAME = RDB_TEST_PATH + "open_helper.db";

void RdbOpenCallbackTest::SetUpTestCase(void)
{
}

void RdbOpenCallbackTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbOpenCallbackTest::DATABASE_NAME);
}

void RdbOpenCallbackTest::SetUp(void)
{
}

void RdbOpenCallbackTest::TearDown(void)
{
}

class OpenCallbackA : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    int OnDowngrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    int OnOpen(RdbStore &rdbStore) override;

    static std::string CreateTableSQL(const std::string &tableName);
    static std::string DropTableSQL(const std::string &tableName);
};

std::string OpenCallbackA::CreateTableSQL(const std::string &tableName)
{
    return "CREATE TABLE IF NOT EXISTS " + tableName
           + " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, age INTEGER)";
}

std::string OpenCallbackA::DropTableSQL(const std::string &tableName)
{
    return "DROP TABLE IF EXISTS " + tableName + ";";
}

int OpenCallbackA::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CreateTableSQL("test1"));
}

int OpenCallbackA::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    LOG_INFO("RdbOpenCallbackTest onUpgrade begin.");
    if (oldVersion < newVersion) {
        if (oldVersion <= 1) {
            return store.ExecuteSql(CreateTableSQL("test2"));
        }
    }
    return E_OK;
}

int OpenCallbackA::OnDowngrade(RdbStore &store, int oldVersion, int newVersion)
{
    LOG_INFO("RdbOpenCallbackTest OnDowngrade begin");
    if (oldVersion > newVersion) {
        if (oldVersion >= 2) {
            return store.ExecuteSql(DropTableSQL("test2"));
        }
    }
    return E_OK;
}

int OpenCallbackA::OnOpen(RdbStore &store)
{
    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    int errCode = store.Replace(id, "test1", values);
    if (errCode != E_OK) {
        return errCode;
    }

    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 18);
    errCode = store.Replace(id, "test1", values);
    if (errCode != E_OK) {
        return errCode;
    }

    return E_OK;
}

/**
 * @tc.name: RdbOpenCallback_01
 * @tc.desc: test RdbOpenCallback
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL
 * @tc.author: chenxi
 */
HWTEST_F(RdbOpenCallbackTest, RdbOpenCallback_01, TestSize.Level1)
{
    RdbStoreConfig config(RdbOpenCallbackTest::DATABASE_NAME);
    OpenCallbackA helper;

    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);
    EXPECT_EQ(errCode, E_OK);

    int currentVersion;
    int ret = store->GetVersion(currentVersion);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentVersion, 1);

    int64_t id;
    int changedRows;
    ValuesBucket values;

    values.PutInt("id", 3);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 20);
    ret = store->Replace(id, "test1", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    values.Clear();
    values.PutInt("age", 20);
    ret = store->Update(changedRows, "test1", values, "age = ?", std::vector<std::string>{ "18" });
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, changedRows);

    ret = store->Delete(changedRows, "test1", "age = ?", std::vector<std::string>{ "20" });
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, changedRows);
}

/**
 * @tc.name: RdbOpenCallback_02
 * @tc.desc: test RdbOpenCallback
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL
 * @tc.author: chenxi
 */
HWTEST_F(RdbOpenCallbackTest, RdbOpenCallback_02, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbOpenCallbackTest::DATABASE_NAME);
    OpenCallbackA helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 2, helper, errCode);
    EXPECT_NE(store, nullptr);

    int currentVersion;
    int ret = store->GetVersion(currentVersion);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentVersion, 2);

    int64_t id;
    int changedRows;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    ret = store->Insert(id, "test2", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 18);
    ret = store->Insert(id, "test2", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);

    values.Clear();
    values.PutInt("id", 3L);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 20);
    ret = store->Insert(id, "test2", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    values.Clear();
    values.PutInt("age", 20);
    ret = store->Update(changedRows, "test2", values, "age = ?", std::vector<std::string>{ "18" });
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, changedRows);

    ret = store->Delete(changedRows, "test2", "age = ?", std::vector<std::string>{ "20" });
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, changedRows);
}

/**
 * @tc.name: RdbOpenCallback_03
 * @tc.desc: test RdbOpenCallback
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL
 * @tc.author: chenxi
 */
HWTEST_F(RdbOpenCallbackTest, RdbOpenCallback_03, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbOpenCallbackTest::DATABASE_NAME);
    OpenCallbackA helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    int currentVersion;
    int ret = store->GetVersion(currentVersion);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentVersion, 1);

    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    ret = store->Insert(id, "test2", values);
    EXPECT_NE(ret, E_OK);
}

class OpenCallbackB : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;

    static std::string CreateTableSQL(const std::string &tableName);
    static std::string DropTableSQL(const std::string &tableName);
};

std::string OpenCallbackB::CreateTableSQL(const std::string &tableName)
{
    return "CREATE TABLE IF NOT EXISTS " + tableName
           + " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, age INTEGER)";
}

std::string OpenCallbackB::DropTableSQL(const std::string &tableName)
{
    return "DROP TABLE IF EXISTS " + tableName + ";";
}

int OpenCallbackB::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CreateTableSQL("test1"));
}

int OpenCallbackB::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    if (oldVersion < newVersion) {
        if (oldVersion <= 1) {
            return store.ExecuteSql(CreateTableSQL("test2"));
        }
    }
    return E_OK;
}

/**
 * @tc.name: RdbOpenCallback_04
 * @tc.desc: test RdbOpenCallback
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbOpenCallbackTest, RdbOpenCallback_04, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbOpenCallbackTest::DATABASE_NAME);
    OpenCallbackB helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 2, helper, errCode);
    EXPECT_NE(store, nullptr);

    int currentVersion;
    int ret = store->GetVersion(currentVersion);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentVersion, 2);

    int64_t id;
    int changedRows;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    ret = store->Insert(id, "test2", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    values.Clear();
    values.PutInt("id", 2);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 18);
    ret = store->Insert(id, "test2", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, id);

    values.Clear();
    values.PutInt("id", 3L);
    values.PutString("name", std::string("lisi"));
    values.PutInt("age", 20);
    ret = store->Insert(id, "test2", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, id);

    values.Clear();
    values.PutInt("age", 20);
    ret = store->Update(changedRows, "test2", values, "age = ?", std::vector<std::string>{ "18" });
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(2, changedRows);

    ret = store->Delete(changedRows, "test2", "age = ?", std::vector<std::string>{ "20" });
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, changedRows);
}

/**
 * @tc.name: RdbOpenCallback_05
 * @tc.desc: test RdbOpenCallback
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbOpenCallbackTest, RdbOpenCallback_05, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbOpenCallbackTest::DATABASE_NAME);
    OpenCallbackB helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    int currentVersion;
    int ret = store->GetVersion(currentVersion);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentVersion, 1);

    int64_t id;
    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18);
    ret = store->Insert(id, "test2", values);
    EXPECT_EQ(ret, E_OK);

    ret = helper.OnDowngrade(*store, 2, 1);
    EXPECT_EQ(ret, E_OK);

    ret = helper.OnOpen(*store);
    EXPECT_EQ(ret, E_OK);
}
