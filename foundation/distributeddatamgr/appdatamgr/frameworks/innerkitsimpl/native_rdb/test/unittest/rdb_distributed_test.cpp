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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>

#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;

static std::shared_ptr<RdbStore> rdbStore;

class RdbStoreDistributedTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase() {
        RdbHelper::DeleteRdbStore(RdbStoreDistributedTest::DRDB_PATH + RdbStoreDistributedTest::DRDB_NAME);
    }
    void SetUp() {}
    void TearDown() {}

    void InsertValue(std::shared_ptr<RdbStore> &store);
    void CheckResultSet(std::shared_ptr<RdbStore> &store);

    static constexpr int ddmsGroupId_ = 1000;
    static const std::string DRDB_NAME;
    static const  std::string DRDB_PATH;
};

const std::string RdbStoreDistributedTest::DRDB_NAME = "distributed_rdb.db";
const std::string RdbStoreDistributedTest::DRDB_PATH = "/data/test/";

class TestOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore& store) override
    {
        std::cout << "on create" << std::endl;
        std::string sql = "CREATE TABLE IF NOT EXISTS employee ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "name TEXT NOT NULL,"
                          "age INTEGER,"
                          "salary REAL,"
                          "data BLOB)";
        std::cout << "create table return " << store.ExecuteSql(sql) << std::endl;
        return 0;
    }

    int OnOpen(RdbStore& store) override
    {
        std::cout << "on open" << std::endl;
        return 0;
    }

    int OnUpgrade(RdbStore& store, int currentVersion, int targetVersion) override
    {
        std::cout << "on upgrade" << std::endl;
        return 0;
    }
};

void RdbStoreDistributedTest::SetUpTestCase()
{
    int errCode = 0;
    std::string path = RdbStoreDistributedTest::DRDB_PATH + RdbStoreDistributedTest::DRDB_NAME;
    int fd = open(path.c_str(), O_CREAT, S_IRWXU | S_IRWXG);
    if (fd < 0) {
        std::cout << "open file failed" << std::endl;
    }
    if (fd > 0) {
        close(fd);
    }
    chown(path.c_str(), 0, ddmsGroupId_);

    RdbStoreConfig config(path);
    config.SetBundleName("com.example.distributed.rdb");
    config.SetName(RdbStoreDistributedTest::DRDB_NAME);
    TestOpenCallback callback;
    rdbStore = RdbHelper::GetRdbStore(config, 1, callback, errCode);
    if (rdbStore == nullptr) {
        std::cout << "get rdb store failed" << std::endl;
    } else {
        std::cout << "get rdb store success" << std::endl;
    }
}

void RdbStoreDistributedTest::InsertValue(std::shared_ptr<RdbStore> &store)
{
    int64_t id;
    ValuesBucket values;

    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutInt("age", 18); // 18 age
    values.PutDouble("salary", 100.5); // 100.5
    values.PutBlob("data", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->Insert(id, "employee", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);
}

void RdbStoreDistributedTest::CheckResultSet(std::shared_ptr<RdbStore> &store)
{
    std::unique_ptr<ResultSet> resultSet =
        store->QuerySql("SELECT * FROM employee WHERE name = ?", { "zhangsan" });
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
    std::cout << "strVal=" << strVal << std::endl;
    EXPECT_EQ("zhangsan", strVal);
}

/**
 * @tc.name: RdbStore_Distributed_Test_001
 * @tc.desc: test RdbStore set distributed tables
 * @tc.type: FUNC
 * @tc.require: AR000GK58F
 * @tc.author: wuchunbo
 */
HWTEST_F(RdbStoreDistributedTest, RdbStore_Distributed_001, TestSize.Level1)
{
    EXPECT_NE(rdbStore, nullptr) << "get rdb store failed";
    EXPECT_EQ(rdbStore->SetDistributedTables({ "employee" }), true) << "set distributed tables failed";
}

/**
 * @tc.name: RdbStore_Distributed_Test_002
 * @tc.desc: rdbStore can work after distributeddata process reboot
 * @tc.type: FUNC
 * @tc.require: AR000GK58F
 * @tc.author: wuchunbo
 */

HWTEST_F(RdbStoreDistributedTest, RdbStore_Distributed_002, TestSize.Level1)
{
    EXPECT_NE(rdbStore, nullptr) << "get rdb store failed";
    std::vector<std::string> empty;
    EXPECT_EQ(rdbStore->SetDistributedTables(empty), true) << "set distributed tables failed";

    InsertValue(rdbStore);
    CheckResultSet(rdbStore);
}
