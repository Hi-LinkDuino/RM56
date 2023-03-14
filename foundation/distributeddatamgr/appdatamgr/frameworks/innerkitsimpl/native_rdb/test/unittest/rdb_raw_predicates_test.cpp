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
#include "raw_rdb_predicates.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;

class RDBRawPredicatesTest : public testing::Test {
};

/* *
 * @tc.name: RDBRawPredicatesTest_001
 * @tc.desc: test RDBRawPredicates
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RDBRawPredicatesTest, RawPredicatesTest_001, TestSize.Level1)
{
    RawRdbPredicates rawRdbPredicates("test");
    EXPECT_EQ("test", rawRdbPredicates.GetTableName());
}

/* *
 * @tc.name: RDBRawPredicatesTest_002
 * @tc.desc: test RDBRawPredicates
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RDBRawPredicatesTest, RawPredicatesTest_002, TestSize.Level1)
{
    RawRdbPredicates rawRdbPredicates("");
    EXPECT_EQ("", rawRdbPredicates.GetTableName());
}

/* *
 * @tc.name: RDBRawPredicatesTest_003
 * @tc.desc: test RDBRawPredicates
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RDBRawPredicatesTest, RawPredicatesTest_003, TestSize.Level1)
{
    RawRdbPredicates rawRdbPredicates("user");
    rawRdbPredicates.SetWhereClause("`firstName` = ?");
    std::vector<std::string> argsArray = {"zhang"};
LOG_INFO("333333333333");
    rawRdbPredicates.SetWhereArgs(argsArray);
LOG_INFO("4444444444");

    std::string tableName = rawRdbPredicates.GetTableName();
    EXPECT_EQ("user", tableName);

    std::string whereClause = rawRdbPredicates.GetWhereClause();
    EXPECT_EQ("`firstName` = ?", whereClause);

    std::vector<std::string> whereArgs = rawRdbPredicates.GetWhereArgs();
    EXPECT_EQ(true, whereArgs.size() == 1);
    EXPECT_EQ("zhang", whereArgs[0]);
}

/* *
 * @tc.name: RDBRawPredicatesTest_004
 * @tc.desc: test RDBRawPredicates
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RDBRawPredicatesTest, RawPredicatesTest_004, TestSize.Level1)
{
    std::vector<std::string> whereArgs = {"1"};
    RawRdbPredicates rawRdbPredicates("test", "id = ?", whereArgs);
    std::string tableName = rawRdbPredicates.GetTableName();
    EXPECT_EQ("test", tableName);

    std::string whereClause = rawRdbPredicates.GetWhereClause();
    EXPECT_EQ("id = ?", whereClause);

    std::vector<std::string> cwhereArgs = rawRdbPredicates.GetWhereArgs();
    EXPECT_EQ(true, cwhereArgs.size() == 1);
    EXPECT_EQ("1", cwhereArgs[0]);
}

/* *
 * @tc.name: RDBRawPredicatesTests_005
 * @tc.desc: test RDBRawPredicates
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RDBRawPredicatesTest, RawPredicatesTest_005, TestSize.Level1)
{
    std::vector<std::string> whereArgs;
    RawRdbPredicates rawRdbPredicates("", "", whereArgs);
    std::string tableName = rawRdbPredicates.GetTableName();
    EXPECT_EQ("", tableName);

    std::string whereClause = rawRdbPredicates.GetWhereClause();
    EXPECT_EQ("", whereClause);

    std::vector<std::string> cwhereArgs = rawRdbPredicates.GetWhereArgs();
    EXPECT_EQ(true, cwhereArgs.size() == 0);
}

/* *
 * @tc.name: RDBRawPredicatesTest_006
 * @tc.desc: test RDBRawPredicates
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RDBRawPredicatesTest, RawPredicatesTest_006, TestSize.Level1)
{
    std::vector<std::string> whereArgs;
    RawRdbPredicates rawRdbPredicates("test", "aaa", whereArgs);
    std::string tableName = rawRdbPredicates.GetTableName();
    EXPECT_EQ("test", tableName);

    std::string whereClause = rawRdbPredicates.GetWhereClause();
    EXPECT_EQ("aaa", whereClause);

    std::vector<std::string> cwhereArgs = rawRdbPredicates.GetWhereArgs();
    EXPECT_EQ(true, cwhereArgs.size() == 0);
}

/* *
 * @tc.name: RDBRawPredicatesTest_007
 * @tc.desc: test RDBRawPredicates
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(RDBRawPredicatesTest, RawPredicatesTest_007, TestSize.Level1)
{
    std::vector<std::string> whereArgs = {"zhangsan", "18"};
    RawRdbPredicates rawRdbPredicates("test", "name = ? AND age = ?", whereArgs);
    std::string tableName = rawRdbPredicates.GetTableName();
    EXPECT_EQ("test", tableName);

    std::string whereClause = rawRdbPredicates.GetWhereClause();
    EXPECT_EQ("name = ? AND age = ?", whereClause);

    std::vector<std::string> cwhereArgs = rawRdbPredicates.GetWhereArgs();
    EXPECT_EQ(true, cwhereArgs.size() == 2);
    EXPECT_EQ("zhangsan", cwhereArgs[0]);
    EXPECT_EQ("18", cwhereArgs[1]);
}