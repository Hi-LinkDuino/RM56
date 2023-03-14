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

#include "detector.h"
#include <gtest/gtest.h>

using namespace testing::ext;

class DetectorFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DetectorFileTest::SetUpTestCase(void)
{
    // step 2: input testsuit setup step
}

void DetectorFileTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void DetectorFileTest::SetUp(void)
{
    // step 3: input testcase setup step
}

void DetectorFileTest::TearDown(void)
{
    // step 3: input testcase teardown step
}

/**
 * @tc.name: IsExistOfFile_001
 * @tc.desc: Check whether the file exists.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorFileTest, IsExistOfFile_001, TestSize.Level0)
{
    // step 1:
    const char* filepath1 = "/data/test/resource/txt/1.txt";
    EXPECT_TRUE(FileExist(filepath1));

    // step 2:
    const char* filepath2 = "/data/test/resource/txt/3.txt";
    EXPECT_TRUE(FileExist(filepath2));
}

/**
 * @tc.name: IsExistOfFile_002
 * @tc.desc: Check whether the file exists.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorFileTest, IsExistOfFile_002, TestSize.Level1)
{
    // step 1:
    const char* filepath1 = "/data/test/resource/txt/2.txt";
    EXPECT_TRUE(FileExist(filepath1));

    // step 2:
    const char* filepath2 = "/data/test/resource/txt/4.txt";
    EXPECT_TRUE(FileExist(filepath2));
}
