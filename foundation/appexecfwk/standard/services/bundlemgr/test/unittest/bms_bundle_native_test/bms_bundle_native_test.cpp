/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <chrono>
#include <fstream>
#include <gtest/gtest.h>

#include "native_interface_bundle.h"

using namespace testing::ext;
namespace OHOS {
class BmsBundleNativeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BmsBundleNativeTest::SetUpTestCase()
{}

void BmsBundleNativeTest::TearDownTestCase()
{}

void BmsBundleNativeTest::SetUp()
{}

void BmsBundleNativeTest::TearDown()
{}

/**
 * @tc.number: BmsBundleNativeTest
 * Function: OH_NativeBundle_GetAppIdByBundleName
 * @tc.name: test wrong bundleName
 * @tc.require: AR000H0AMV
 * @tc.desc: 1. system running normally
 */
HWTEST_F(BmsBundleNativeTest, BmsBundleNativeTest_0200, Function | SmallTest | Level1)
{
    char* appId = OH_NativeBundle_GetAppIdByBundleName("wrong name");
    EXPECT_TRUE(appId == nullptr);
}

/**
 * @tc.number: BmsBundleNativeTest
 * Function: OH_NativeBundle_GetAppIdByBundleName
 * @tc.name: test bundleName is nullptr
 * @tc.require: AR000H0AMV
 * @tc.desc: 1. system running normally
 */
HWTEST_F(BmsBundleNativeTest, BmsBundleNativeTest_0300, Function | SmallTest | Level1)
{
    char* appId = OH_NativeBundle_GetAppIdByBundleName(nullptr);
    EXPECT_TRUE(appId == nullptr);
}

/**
 * @tc.number: BmsBundleNativeTest
 * Function: OH_NativeBundle_GetAppIdByBundleName
 * @tc.name: test bundleName is empty
 * @tc.require: SR000H0ALQ
 * @tc.desc: 1. system running normally
 */
HWTEST_F(BmsBundleNativeTest, BmsBundleNativeTest_0400, Function | SmallTest | Level1)
{
    char* appId = OH_NativeBundle_GetAppIdByBundleName("");
    EXPECT_TRUE(appId == nullptr);
}
} // OHOS