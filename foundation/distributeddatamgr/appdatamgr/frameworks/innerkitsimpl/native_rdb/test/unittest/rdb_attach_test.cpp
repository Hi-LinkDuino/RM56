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

class RdbAttachTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void QueryCheck1(std::shared_ptr<RdbStore> &store) const;
    void QueryCheck2(std::shared_ptr<RdbStore> &store) const;

    static const std::string MAIN_DATABASE_NAME;
    static const std::string ATTACHED_DATABASE_NAME;
};

const std::string RdbAttachTest::MAIN_DATABASE_NAME = RDB_TEST_PATH + "main.db";

class MainOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

std::string const MainOpenCallback::CREATE_TABLE_TEST = "CREATE TABLE IF NOT EXISTS test1(id INTEGER PRIMARY KEY "
                                                        "AUTOINCREMENT, name TEXT NOT NULL)";

int MainOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int MainOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

const std::string RdbAttachTest::ATTACHED_DATABASE_NAME = RDB_TEST_PATH + "attached.db";

class AttachedOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

std::string const AttachedOpenCallback::CREATE_TABLE_TEST = "CREATE TABLE IF NOT EXISTS test2(id INTEGER PRIMARY KEY "
                                                            "AUTOINCREMENT, name TEXT NOT NULL)";

int AttachedOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int AttachedOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbAttachTest::SetUpTestCase(void)
{
    RdbStoreConfig attachedConfig(RdbAttachTest::ATTACHED_DATABASE_NAME);
    AttachedOpenCallback attachedHelper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> attachedStore = RdbHelper::GetRdbStore(attachedConfig, 1, attachedHelper, errCode);
    EXPECT_NE(attachedStore, nullptr);
}

void RdbAttachTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(MAIN_DATABASE_NAME);
    RdbHelper::DeleteRdbStore(ATTACHED_DATABASE_NAME);
}

void RdbAttachTest::SetUp(void)
{
}

void RdbAttachTest::TearDown(void)
{
    RdbHelper::ClearCache();
}

/**
 * @tc.name: RdbStore_Attach_001
 * @tc.desc: test attach, attach is not supported in wal mode
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbAttachTest, RdbStore_Attach_001, TestSize.Level1)
{
    RdbStoreConfig config(RdbAttachTest::MAIN_DATABASE_NAME);
    MainOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    int ret = store->ExecuteSql("ATTACH '" + ATTACHED_DATABASE_NAME + "' as attached");
    EXPECT_EQ(ret, E_NOT_SUPPORTED_ATTACH_IN_WAL_MODE);

    ret = store->ExecuteSql("attach '" + ATTACHED_DATABASE_NAME + "' as attached");
    EXPECT_EQ(ret, E_NOT_SUPPORTED_ATTACH_IN_WAL_MODE);
}

/**
 * @tc.name: RdbStore_Attach_002
 * @tc.desc: test RdbStore attach
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbAttachTest, RdbStore_Attach_002, TestSize.Level1)
{
    RdbStoreConfig config(RdbAttachTest::MAIN_DATABASE_NAME);
    config.SetJournalMode(JournalMode::MODE_TRUNCATE);
    MainOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    int ret = store->ExecuteSql("ATTACH DATABASE '" + ATTACHED_DATABASE_NAME + "' as 'attached'");
    EXPECT_EQ(ret, E_OK);

    int64_t id;
    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    ret = store->Insert(id, "test1", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(id, 1);

    values.Clear();
    values.PutInt("id", 1);
    values.PutString("name", std::string("lisi"));
    ret = store->Insert(id, "test2", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(id, 1);

    QueryCheck1(store);

    ret = store->ExecuteSql("DETACH DATABASE 'attached'");
    EXPECT_EQ(ret, E_OK);

    QueryCheck2(store);

    ret = store->ExecuteSql("attach database '" + ATTACHED_DATABASE_NAME + "' as 'attached'");
    EXPECT_EQ(ret, E_OK);

    ret = store->ExecuteSql("detach database 'attached'");
    EXPECT_EQ(ret, E_OK);
}

void RdbAttachTest::QueryCheck1(std::shared_ptr<RdbStore> &store) const
{
    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test1");
    EXPECT_NE(resultSet, nullptr);
    int ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    int columnIndex;
    int intVal;
    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(intVal, 1);
    std::string strVal;
    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(strVal, "zhangsan");

    resultSet = store->QuerySql("SELECT * FROM test2");
    EXPECT_NE(resultSet, nullptr);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(intVal, 1);
    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(strVal, "lisi");
}

void RdbAttachTest::QueryCheck2(std::shared_ptr<RdbStore> &store) const
{
    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test1");
    EXPECT_NE(resultSet, nullptr);
    int ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_OK);
    int columnIndex;
    int intVal;
    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(intVal, 1);
    std::string strVal;
    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(strVal, "zhangsan");

    // detached, no table test2
    resultSet = store->QuerySql("SELECT * FROM test2");
    EXPECT_NE(resultSet, nullptr);
}