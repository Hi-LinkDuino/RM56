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
#include <thread>

#include "common.h"
#include "logger.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;

class RdbEncryptTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static void InsertThread(int n);
    static void QueryThread(int n);
    void QueryCheckID1(std::shared_ptr<RdbStore> &store);
    void QueryCheckID4(std::shared_ptr<RdbStore> &store);
    void QueryCheckID5(std::shared_ptr<RdbStore> &store);

    static const std::string ENCRYPTED_DATABASE_NAME;
    static const std::string UNENCRYPTED_DATABASE_NAME;
    static const std::vector<uint8_t> KEY1;
    static const std::vector<uint8_t> KEY2;
    static const std::vector<uint8_t> KEY11;
    static const std::vector<uint8_t> KEY12;
    static const std::vector<uint8_t> KEY13;
    static const int E_SQLITE_NOTADB; // errno SQLITE_NOTADB
    static std::shared_ptr<RdbStore> testStore;
};

const std::string RdbEncryptTest::ENCRYPTED_DATABASE_NAME = RDB_TEST_PATH + "encrypted.db";
const std::string RdbEncryptTest::UNENCRYPTED_DATABASE_NAME = RDB_TEST_PATH + "unencrypted.db";
const std::vector<uint8_t> RdbEncryptTest::KEY1 = { 'E', 'n', 'c', 'r', 'y', 'p', 't', 'T', 'e', 's', 't', '@', '1',
    '2', '3' };
const std::vector<uint8_t> RdbEncryptTest::KEY2 = { 'E', 'n', 'c', 'r', 'y', 'p', 't', 'T', 'e', 's', 't', '@', '4',
    '5', '6' };

const std::vector<uint8_t> RdbEncryptTest::KEY11 = { 'E', 'n', 'c', 'r', 'y', 'p', 't', 'T', 'e', 's', 't', '#', '1',
    '2', '3' };
const std::vector<uint8_t> RdbEncryptTest::KEY12 = { 'E', 'n', 'c', 'r', 'y', 'p', 't', 'T', 'e', 's', 't', '!', '4',
    '5', '6' };
const std::vector<uint8_t> RdbEncryptTest::KEY13 = { 'E', 'n', 'c', 'r', 'y', 'p', 't', 'T', 'e', 's', 't', '%', '4',
    '5', '6' };

const int RdbEncryptTest::E_SQLITE_NOTADB = -26; // errno SQLITE_NOTADB
std::shared_ptr<RdbStore> RdbEncryptTest::testStore = nullptr;

class EncryptTestOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

std::string const EncryptTestOpenCallback::CREATE_TABLE_TEST = std::string("CREATE TABLE IF NOT EXISTS test ")
                                                               + std::string("(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                             "name TEXT NOT NULL, age INTEGER, salary "
                                                                             "REAL, blobType BLOB)");

int EncryptTestOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int EncryptTestOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbEncryptTest::SetUpTestCase(void)
{
}

void RdbEncryptTest::TearDownTestCase(void)
{
    RdbHelper::DeleteRdbStore(RdbEncryptTest::ENCRYPTED_DATABASE_NAME);
    RdbHelper::DeleteRdbStore(RdbEncryptTest::UNENCRYPTED_DATABASE_NAME);
}

void RdbEncryptTest::SetUp(void)
{
}

void RdbEncryptTest::TearDown(void)
{
    RdbHelper::ClearCache();
}

/**
 * @tc.name: RdbStore_Encrypt_001
 * @tc.desc: test RdbStore Encrypt, create with key
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_001, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY1);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    EXPECT_NE(store, nullptr);
    EXPECT_EQ(config.GetEncryptKey().empty(), true);

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
}

void RdbEncryptTest::QueryCheckID1(std::shared_ptr<RdbStore> &store)
{
    std::unique_ptr<ResultSet> resultSet =
        store->QuerySql("SELECT * FROM test WHERE name = ?", std::vector<std::string>{ "zhangsan" });
    EXPECT_NE(resultSet, nullptr);

    int columnIndex;
    int intVal;
    std::string strVal;
    double dVal;
    std::vector<uint8_t> blob;
    int ret = resultSet->GoToFirstRow();
    EXPECT_EQ(ret, E_OK);
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
 * @tc.name: RdbStore_Encrypt_002
 * @tc.desc: test RdbStore Encrypt, open with correct key
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_002, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY1);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    EXPECT_NE(store, nullptr);

    RdbEncryptTest::QueryCheckID1(store);
    store = nullptr;
}

/**
 * @tc.name: RdbStore_Encrypt_003
 * @tc.desc: test RdbStore Encrypt, incorrect key
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_003, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY2);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    EXPECT_EQ(store, nullptr);
    EXPECT_EQ(errCode, E_SQLITE_NOTADB);
}

/**
 * @tc.name: RdbStore_Encrypt_004
 * @tc.desc: test RdbStore Encrypt, null key
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_004, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_EQ(store, nullptr);
    EXPECT_EQ(errCode, E_SQLITE_NOTADB);
}

void RdbEncryptTest::QueryCheckID4(std::shared_ptr<RdbStore> &store)
{
    std::unique_ptr<ResultSet> resultSet =
        store->QuerySql("SELECT * FROM test WHERE name = ?", std::vector<std::string>{ "zhao" });
    EXPECT_NE(resultSet, nullptr);

    int columnIndex;
    int intVal;
    std::string strVal;
    double dVal;
    std::vector<uint8_t> blob;
    int ret = resultSet->GoToFirstRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(4, intVal);
    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ("zhao", strVal);
    ret = resultSet->GetColumnIndex("age", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(40, intVal);
    ret = resultSet->GetColumnIndex("salary", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetDouble(columnIndex, dVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(400.5, dVal);
    ret = resultSet->GetColumnIndex("blobType", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetBlob(columnIndex, blob);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, static_cast<int>(blob.size()));
    EXPECT_EQ(10, blob[0]);
    EXPECT_EQ(11, blob[1]);
    EXPECT_EQ(12, blob[2]);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: RdbStore_Encrypt_005
 * @tc.desc: test RdbStore Encrypt, change key
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_005, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY1);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    EXPECT_NE(store, nullptr);

    int ret = store->ChangeEncryptKey(RdbEncryptTest::KEY2);
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 3);

    int64_t id;
    ValuesBucket values;
    values.Clear();
    values.PutInt("id", 4);
    values.PutString("name", std::string("zhao"));
    values.PutInt("age", 40);
    values.PutDouble("salary", 400.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 10, 11, 12 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(4, id);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 4);

    RdbEncryptTest::QueryCheckID1(store);
    RdbEncryptTest::QueryCheckID4(store);
}

/**
 * @tc.name: RdbStore_Encrypt_006
 * @tc.desc: test RdbStore Encrypt, open with old key
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_006, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY1);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    EXPECT_EQ(store, nullptr);
    EXPECT_EQ(errCode, E_SQLITE_NOTADB);
}

/**
 * @tc.name: RdbStore_Encrypt_007
 * @tc.desc: test RdbStore Encrypt, open with null key
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_007, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(
        RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, std::vector<uint8_t>());
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    EXPECT_EQ(store, nullptr);
    EXPECT_EQ(errCode, E_SQLITE_NOTADB);
}

/**
 * @tc.name: RdbStore_Encrypt_008
 * @tc.desc: test RdbStore Encrypt, open with new key
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_008, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY2);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    EXPECT_NE(store, nullptr);

    int64_t count;
    int ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 4);
}

void RdbEncryptTest::QueryCheckID5(std::shared_ptr<RdbStore> &store)
{
    std::unique_ptr<ResultSet> resultSet =
        store->QuerySql("SELECT * FROM test WHERE name = ?", std::vector<std::string>{ "sun" });
    EXPECT_NE(resultSet, nullptr);

    int columnIndex;
    int intVal;
    std::string strVal;
    double dVal;
    std::vector<uint8_t> blob;
    int ret = resultSet->GoToFirstRow();
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetColumnIndex("id", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(5, intVal);
    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ("sun", strVal);
    ret = resultSet->GetColumnIndex("age", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetInt(columnIndex, intVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(50, intVal);
    ret = resultSet->GetColumnIndex("salary", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetDouble(columnIndex, dVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(500.5, dVal);
    ret = resultSet->GetColumnIndex("blobType", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetBlob(columnIndex, blob);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(3, static_cast<int>(blob.size()));
    EXPECT_EQ(13, blob[0]);
    EXPECT_EQ(14, blob[1]);
    EXPECT_EQ(15, blob[2]);
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_ERROR);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}
/**
 * @tc.name: RdbStore_Encrypt_009
 * @tc.desc: test RdbStore Encrypt, change key but new key is null
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_009, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY2);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    EXPECT_NE(store, nullptr);

    int ret = store->ChangeEncryptKey(std::vector<uint8_t>());
    EXPECT_EQ(ret, E_EMPTY_NEW_ENCRYPT_KEY);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 4);

    int64_t id;
    ValuesBucket values;
    values.Clear();
    values.PutInt("id", 5);
    values.PutString("name", std::string("sun"));
    values.PutInt("age", 50);
    values.PutDouble("salary", 500.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 13, 14, 15 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(5, id);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 5);

    RdbEncryptTest::QueryCheckID1(store);
    RdbEncryptTest::QueryCheckID4(store);
    RdbEncryptTest::QueryCheckID5(store);
}

/**
 * @tc.name: RdbStore_Encrypt_010
 * @tc.desc: test RdbStore Encrypt, change key but the database is unencrypted
 * @tc.type: FUNC
 * @tc.require: AR000CU2BP
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_010, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::UNENCRYPTED_DATABASE_NAME);
    EncryptTestOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);

    int ret = store->ChangeEncryptKey(RdbEncryptTest::KEY2);
    EXPECT_EQ(ret, E_CHANGE_UNENCRYPTED_TO_ENCRYPTED);
}

void RdbEncryptTest::InsertThread(int n)
{
    std::shared_ptr<RdbStore> &store = RdbEncryptTest::testStore;
    int ret = store->BeginTransaction();
    EXPECT_EQ(ret, E_OK);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ret = store->MarkAsCommit();
    EXPECT_EQ(ret, E_OK);
    ret = store->EndTransaction();
    EXPECT_EQ(ret, E_OK);
}

void RdbEncryptTest::QueryThread(int n)
{
    std::shared_ptr<RdbStore> &store = RdbEncryptTest::testStore;
    std::unique_ptr<ResultSet> resultSet =
        store->QueryByStep("SELECT * FROM test WHERE name = ?", std::vector<std::string> { "zhangsan" });
    EXPECT_NE(resultSet, nullptr);
    int ret = resultSet->GoToFirstRow();
    EXPECT_EQ(ret, E_OK);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ret = resultSet->GoToNextRow();
    EXPECT_EQ(ret, E_STEP_RESULT_IS_AFTER_LAST);
    ret = resultSet->Close();
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: RdbStore_Encrypt_011
 * @tc.desc: test RdbStore Encrypt, change key but the database is busy
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_011, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY2);
    EncryptTestOpenCallback helper;
    RdbEncryptTest::testStore = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    std::shared_ptr<RdbStore> &store = RdbEncryptTest::testStore;
    EXPECT_NE(store, nullptr);

    std::thread insertThread = std::thread(RdbEncryptTest::InsertThread, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    int ret = store->ChangeEncryptKey(RdbEncryptTest::KEY11);
    EXPECT_EQ(ret, E_CHANGE_ENCRYPT_KEY_IN_BUSY);

    insertThread.join();
    ret = store->ChangeEncryptKey(RdbEncryptTest::KEY11);
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 5);

    int64_t id;
    ValuesBucket values;
    values.Clear();
    values.PutInt("id", 6);
    values.PutString("name", std::string("he"));
    values.PutInt("age", 60);
    values.PutDouble("salary", 600.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 16, 17, 18 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(6, id);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 6);

    RdbEncryptTest::QueryCheckID1(store);
}

/**
 * @tc.name: RdbStore_Encrypt_012
 * @tc.desc: test RdbStore Encrypt, change key but the database is busy
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_012, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(
        RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY11);
    EncryptTestOpenCallback helper;
    RdbEncryptTest::testStore = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    std::shared_ptr<RdbStore> &store = RdbEncryptTest::testStore;
    EXPECT_NE(store, nullptr);

    std::thread queryThread = std::thread(RdbEncryptTest::QueryThread, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    int ret = store->ChangeEncryptKey(RdbEncryptTest::KEY12);
    EXPECT_EQ(ret, E_CHANGE_ENCRYPT_KEY_IN_BUSY);

    queryThread.join();
    ret = store->ChangeEncryptKey(RdbEncryptTest::KEY12);
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 6);

    int64_t id;
    ValuesBucket values;
    values.Clear();
    values.PutInt("id", 7);
    values.PutString("name", std::string("he"));
    values.PutInt("age", 30);
    values.PutDouble("salary", 700.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 16, 17, 18 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(7, id);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 7);

    RdbEncryptTest::QueryCheckID1(store);
}

/**
 * @tc.name: RdbStore_Encrypt_013
 * @tc.desc: test RdbStore Encrypt, change key but the database is busy
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL
 * @tc.author: chenxi
 */
HWTEST_F(RdbEncryptTest, RdbStore_Encrypt_013, TestSize.Level1)
{
    int errCode = E_OK;
    RdbStoreConfig config(
        RdbEncryptTest::ENCRYPTED_DATABASE_NAME, StorageMode::MODE_DISK, false, RdbEncryptTest::KEY12);
    EncryptTestOpenCallback helper;
    RdbEncryptTest::testStore = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    config.ClearEncryptKey();
    std::shared_ptr<RdbStore> &store = RdbEncryptTest::testStore;
    EXPECT_NE(store, nullptr);

    std::thread insertThread = std::thread(RdbEncryptTest::InsertThread, 0);
    std::thread queryThread = std::thread(RdbEncryptTest::QueryThread, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    int ret = store->ChangeEncryptKey(RdbEncryptTest::KEY13);
    EXPECT_EQ(ret, E_CHANGE_ENCRYPT_KEY_IN_BUSY);

    insertThread.join();
    queryThread.join();
    ret = store->ChangeEncryptKey(RdbEncryptTest::KEY13);
    EXPECT_EQ(ret, E_OK);

    int64_t count;
    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 7);

    int64_t id;
    ValuesBucket values;
    values.Clear();
    values.PutInt("id", 8);
    values.PutString("name", std::string("wu"));
    values.PutInt("age", 40);
    values.PutDouble("salary", 800.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 16, 17, 18 });
    ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(8, id);

    ret = store->ExecuteAndGetLong(count, "SELECT COUNT(*) FROM test");
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(count, 8);

    RdbEncryptTest::QueryCheckID1(store);
}
