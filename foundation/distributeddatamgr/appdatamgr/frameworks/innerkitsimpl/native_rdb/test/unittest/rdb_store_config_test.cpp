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
#include "unistd.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;

class RdbStoreConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class ConfigTestOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

const std::string ConfigTestOpenCallback::CREATE_TABLE_TEST = std::string("CREATE TABLE IF NOT EXISTS test ")
                                                              + std::string("(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                            "name TEXT NOT NULL, age INTEGER, salary "
                                                                            "REAL, blobType BLOB)");

int ConfigTestOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int ConfigTestOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbStoreConfigTest::SetUpTestCase(void)
{
}

void RdbStoreConfigTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RDB_TEST_PATH + "config_test.db");
}

void RdbStoreConfigTest::SetUp(void)
{
}

void RdbStoreConfigTest::TearDown(void)
{
    RdbHelper::ClearCache();
}

/**
 * @tc.name: RdbStoreConfig_001
 * @tc.desc: test RdbStoreConfig
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_001, TestSize.Level1)
{
    int errCode = E_OK;
    const std::string dbPath = RDB_TEST_PATH + "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    ConfigTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);
    EXPECT_EQ(errCode, E_OK);

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

    ret = access(dbPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    int currentVersion;
    ret = store->GetVersion(currentVersion);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, currentVersion);

    store = nullptr;
    ret = RdbHelper::DeleteRdbStore(dbPath);
    EXPECT_EQ(ret, E_OK);

    StorageMode mode = config.GetStorageMode();
    StorageMode targeMode = StorageMode::MODE_DISK;
    EXPECT_EQ(mode, targeMode);
}

/**
 * @tc.name: RdbStoreConfig_002
 * @tc.desc: test RdbStoreConfig
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_002, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config("", StorageMode::MODE_MEMORY);
    ConfigTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);
    EXPECT_EQ(errCode, E_OK);

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

    int currentVersion;
    ret = store->GetVersion(currentVersion);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, currentVersion);

    std::string journalMode;
    ret = store->ExecuteAndGetString(journalMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(journalMode, "memory");

    StorageMode mode = config.GetStorageMode();
    StorageMode targeMode = StorageMode::MODE_MEMORY;
    EXPECT_EQ(mode, targeMode);
}

/**
 * @tc.name: RdbStoreConfig_003
 * @tc.desc: test RdbStoreConfig
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_003, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config("", StorageMode::MODE_DISK, false);
    ConfigTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_EQ(store, nullptr);
    EXPECT_EQ(errCode, E_EMPTY_FILE_NAME);
}

/**
 * @tc.name: RdbStoreConfig_004
 * @tc.desc: test RdbStoreConfig
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_004, TestSize.Level1)
{
    int errCode = E_OK;
    const std::string dbPath = "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    ConfigTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_EQ(store, nullptr);
    EXPECT_EQ(errCode, E_RELATIVE_PATH);
}

/**
 * @tc.name: RdbStoreConfig_005
 * @tc.desc: test RdbStoreConfig journalMode
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_005, TestSize.Level1)
{
    const std::string dbPath = RDB_TEST_PATH + "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    std::string journalMode = config.GetJournalMode();
    EXPECT_EQ(journalMode, "");
    ConfigTestOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    std::string currentMode;
    int ret = store->ExecuteAndGetString(currentMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentMode, "wal");
}

/**
 * @tc.name: RdbStoreConfig_006
 * @tc.desc: test RdbStoreConfig journalMode
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_006, TestSize.Level1)
{
    const std::string dbPath = RDB_TEST_PATH + "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    config.SetJournalMode(JournalMode::MODE_DELETE);
    std::string journalMode = config.GetJournalMode();
    EXPECT_EQ(journalMode, "DELETE");
    ConfigTestOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    std::string currentMode;
    int ret = store->ExecuteAndGetString(currentMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentMode, "delete");
}

/**
 * @tc.name: RdbStoreConfig_007
 * @tc.desc: test RdbStoreConfig journalMode
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_007, TestSize.Level1)
{
    const std::string dbPath = RDB_TEST_PATH + "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    config.SetJournalMode(JournalMode::MODE_TRUNCATE);
    std::string journalMode = config.GetJournalMode();
    EXPECT_EQ(journalMode, "TRUNCATE");
    ConfigTestOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    std::string currentMode;
    int ret = store->ExecuteAndGetString(currentMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentMode, "truncate");
}

/**
 * @tc.name: RdbStoreConfig_008
 * @tc.desc: test RdbStoreConfig journalMode
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_008, TestSize.Level1)
{
    const std::string dbPath = RDB_TEST_PATH + "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    config.SetJournalMode(JournalMode::MODE_PERSIST);
    std::string journalMode = config.GetJournalMode();
    EXPECT_EQ(journalMode, "PERSIST");
    ConfigTestOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    std::string currentMode;
    int ret = store->ExecuteAndGetString(currentMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentMode, "persist");
}

/**
 * @tc.name: RdbStoreConfig_009
 * @tc.desc: test RdbStoreConfig journalMode
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_009, TestSize.Level1)
{
    LOG_INFO("RdbStoreConfig_007 start");
    const std::string dbPath = RDB_TEST_PATH + "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    config.SetJournalMode(JournalMode::MODE_MEMORY);
    std::string journalMode = config.GetJournalMode();
    EXPECT_EQ(journalMode, "MEMORY");
    ConfigTestOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    std::string currentMode;
    int ret = store->ExecuteAndGetString(currentMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentMode, "memory");
}
/**
 * @tc.name: RdbStoreConfig_010
 * @tc.desc: test RdbStoreConfig journalMode
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_010, TestSize.Level1)
{
    const std::string dbPath = RDB_TEST_PATH + "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    config.SetJournalMode(JournalMode::MODE_WAL);
    std::string journalMode = config.GetJournalMode();
    EXPECT_EQ(journalMode, "WAL");
    ConfigTestOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    std::string currentMode;
    int ret = store->ExecuteAndGetString(currentMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentMode, "wal");
}

/**
 * @tc.name: RdbStoreConfig_011
 * @tc.desc: test RdbStoreConfig journalMode
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreConfigTest, RdbStoreConfig_011, TestSize.Level1)
{
    const std::string dbPath = RDB_TEST_PATH + "config_test.db";
    RdbStoreConfig config(dbPath, StorageMode::MODE_DISK, false);
    config.SetJournalMode(JournalMode::MODE_OFF);
    std::string journalMode = config.GetJournalMode();
    EXPECT_EQ(journalMode, "OFF");
    ConfigTestOpenCallback helper;
    int errCode = E_OK;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    std::string currentMode;
    int ret = store->ExecuteAndGetString(currentMode, "PRAGMA journal_mode");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(currentMode, "off");
}
