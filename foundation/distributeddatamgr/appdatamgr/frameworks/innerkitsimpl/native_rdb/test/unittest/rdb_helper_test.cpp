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

#include "rdb_helper.h"

#include <gtest/gtest.h>

#include <string>

#include "rdb_errno.h"
#include "rdb_open_callback.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;

class RdbHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void RdbHelperTest::SetUpTestCase(void)
{
}

void RdbHelperTest::TearDownTestCase(void)
{
}

void RdbHelperTest::SetUp(void)
{
}

void RdbHelperTest::TearDown(void)
{
}

/**
 * @tc.name: DeleteDatabase_001
 * @tc.desc: delete db file
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL
 * @tc.author: chenxi
 */
HWTEST_F(RdbHelperTest, DeleteDatabase_001, TestSize.Level1)
{
    int ret = RdbHelper::DeleteRdbStore("test");
    EXPECT_EQ(ret, E_OK);
}
