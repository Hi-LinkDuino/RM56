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

class RdbStoreInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class MyOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override;
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override;
    static const std::string CREATE_TABLE_TEST;
};

const std::string MyOpenCallback::CREATE_TABLE_TEST = std::string("CREATE TABLE IF NOT EXISTS test ")
                                                      + std::string("(id INTEGER PRIMARY KEY AUTOINCREMENT, name "
                                                                    "TEXT, "
                                                                    "age INTEGER, salary REAL, blobType BLOB)");

int MyOpenCallback::OnCreate(RdbStore &store)
{
    return store.ExecuteSql(CREATE_TABLE_TEST);
}

int MyOpenCallback::OnUpgrade(RdbStore &store, int oldVersion, int newVersion)
{
    return E_OK;
}

void RdbStoreInterfaceTest::SetUpTestCase(void)
{
}

void RdbStoreInterfaceTest::TearDownTestCase(void)
{
}

void RdbStoreInterfaceTest::SetUp(void)
{
}

void RdbStoreInterfaceTest::TearDown(void)
{
}

/**
 * @tc.name: ValueObject_TEST_001
 * @tc.desc: test ValueObject
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInterfaceTest, ValueObject_TEST_001, TestSize.Level1)
{
    ValueObject obj = ValueObject();
    ValueObjectType type = obj.GetType();
    EXPECT_EQ(type, ValueObjectType::TYPE_NULL);
}

/**
 * @tc.name: ValueObject_TEST_002
 * @tc.desc: test ValueObject
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInterfaceTest, ValueObject_TEST_002, TestSize.Level1)
{
    int inputVal = 5;
    int outputVal = 0;
    ValueObject obj = ValueObject(inputVal);
    ValueObjectType type = obj.GetType();
    EXPECT_EQ(type, ValueObjectType::TYPE_INT);
    int ret = obj.GetInt(outputVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(outputVal, 5);
}

/**
 * @tc.name: ValueObject_TEST_003
 * @tc.desc: test ValueObject
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInterfaceTest, ValueObject_TEST_003, TestSize.Level1)
{
    bool inputVal = true;
    bool outputVal = false;
    ValueObject obj = ValueObject(inputVal);
    ValueObjectType type = obj.GetType();
    EXPECT_EQ(type, ValueObjectType::TYPE_BOOL);
    int ret = obj.GetBool(outputVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(outputVal, true);
}

/**
 * @tc.name: ValueObject_TEST_004
 * @tc.desc: test ValueObject
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInterfaceTest, ValueObject_TEST_004, TestSize.Level1)
{
    std::string inputVal = "hello";
    std::string outputVal = "";
    ValueObject obj = ValueObject(inputVal);
    ValueObjectType type = obj.GetType();
    EXPECT_EQ(type, ValueObjectType::TYPE_STRING);
    int ret = obj.GetString(outputVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(outputVal, "hello");
}

/**
 * @tc.name: ValueObject_TEST_005
 * @tc.desc: test ValueObject
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInterfaceTest, ValueObject_TEST_005, TestSize.Level1)
{
    std::vector<uint8_t> inputVal = { 'h', 'e', 'l', 'l', 'o' };
    std::vector<uint8_t> outputVal;
    ValueObject obj = ValueObject(inputVal);
    ValueObjectType type = obj.GetType();
    EXPECT_EQ(type, ValueObjectType::TYPE_BLOB);
    int ret = obj.GetBlob(outputVal);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(static_cast<int>(outputVal.size()), 5);
    EXPECT_EQ(outputVal[0], 'h');
    EXPECT_EQ(outputVal[1], 'e');
    EXPECT_EQ(outputVal[2], 'l');
    EXPECT_EQ(outputVal[3], 'l');
    EXPECT_EQ(outputVal[4], 'o');
}

/**
 * @tc.name: ValuesBucket_001
 * @tc.desc: test ValuesBucket
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInterfaceTest, ValuesBucket_001, TestSize.Level1)
{
    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutNull("name");
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });

    int size = values.Size();
    EXPECT_EQ(size, 5);
    bool contains = values.HasColumn("name");
    EXPECT_EQ(contains, true);
    ValueObject obj;
    contains = values.GetObject("salary", obj);
    double val = 0.0;
    ValueObjectType type = obj.GetType();
    EXPECT_EQ(type, ValueObjectType::TYPE_DOUBLE);
    int ret = obj.GetDouble(val);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(val, 100.5);

    values.Delete("name");
    size = values.Size();
    EXPECT_EQ(size, 4);
    contains = values.HasColumn("name");
    EXPECT_EQ(contains, false);

    values.Clear();
    size = values.Size();
    EXPECT_EQ(size, 0);
    contains = values.HasColumn("salary");
    EXPECT_EQ(contains, false);
}

/**
 * @tc.name: ValuesBucket_002
 * @tc.desc: test ValuesBucket
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInterfaceTest, ValuesBucket_002, TestSize.Level1)
{
    int errCode = E_OK;
    const std::string dbPath = RDB_TEST_PATH + "InterfaceTest.db";
    RdbStoreConfig config(dbPath);
    MyOpenCallback helper;
    std::shared_ptr<RdbStore> store = RdbHelper::GetRdbStore(config, 1, helper, errCode);
    EXPECT_NE(store, nullptr);
    EXPECT_EQ(errCode, E_OK);

    int64_t id;
    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutNull("name");
    values.PutInt("age", 18);
    values.PutDouble("salary", 100.5);
    values.PutBlob("blobType", std::vector<uint8_t>{ 1, 2, 3 });
    int ret = store->Insert(id, "test", values);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(1, id);

    std::unique_ptr<ResultSet> resultSet = store->QuerySql("SELECT * FROM test");
    EXPECT_NE(resultSet, nullptr);

    int columnIndex;
    std::string strVal;

    ret = resultSet->GoToFirstRow();
    EXPECT_EQ(ret, E_OK);

    ret = resultSet->GetColumnIndex("name", columnIndex);
    EXPECT_EQ(ret, E_OK);
    ret = resultSet->GetString(columnIndex, strVal);
    EXPECT_EQ(ret, E_ERROR);

    store = nullptr;
    ret = RdbHelper::DeleteRdbStore(dbPath);
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: ValuesBucket_003
 * @tc.desc: test ValuesBucket
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: chenxi
 */
HWTEST_F(RdbStoreInterfaceTest, ValuesBucket_003, TestSize.Level1)
{
    ValuesBucket values;
    values.PutBool("boolType", true);
    values.PutLong("longType", 1);

    int size = values.Size();
    EXPECT_EQ(size, 2);
    bool contains = values.HasColumn("boolType");
    EXPECT_EQ(contains, true);
    ValueObject obj;
    contains = values.GetObject("boolType", obj);
    ValueObjectType type = obj.GetType();
    EXPECT_EQ(type, ValueObjectType::TYPE_BOOL);
    bool val1 = false;
    int ret = obj.GetBool(val1);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(val1, true);

    contains = values.HasColumn("longType");
    EXPECT_EQ(contains, true);
    contains = values.GetObject("longType", obj);
    type = obj.GetType();
    EXPECT_EQ(type, ValueObjectType::TYPE_INT);
    int64_t val2 = 0;
    ret = obj.GetLong(val2);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(val2, 1);
}
