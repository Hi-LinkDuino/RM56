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

#include "app_log_wrapper.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

class AppExecFwkLogTest : public testing::Test {
public:
    static void SetUpTestCase() const;
    static void TearDownTestCase() const;
    void SetUp() const;
    void TearDown() const;
};

void AppExecFwkLogTest::SetUpTestCase() const
{}

void AppExecFwkLogTest::TearDownTestCase() const
{}

void AppExecFwkLogTest::SetUp() const
{}

void AppExecFwkLogTest::TearDown() const
{}

/*
 * Feature: AppLogWrapper
 * Function: AppLogWrapper
 * SubFunction: NA
 * FunctionPoints: Dynamically control the log print level
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(AppExecFwkLogTest, AppLogWrapper_001, TestSize.Level0)
{
    EXPECT_EQ(AppLogLevel::DEBUG, AppLogWrapper::GetLogLevel());
    AppLogWrapper::SetLogLevel(AppLogLevel::INFO);
    EXPECT_EQ(AppLogLevel::INFO, AppLogWrapper::GetLogLevel());
}

/*
 * Feature: AppLogWrapper
 * Function: AppLogWrapper
 * SubFunction: NA
 * FunctionPoints: splice filename
 * EnvConditions: NA
 * CaseDescription: normal valid filename
 */
HWTEST_F(AppExecFwkLogTest, AppLogWrapper_002, TestSize.Level0)
{
    std::string fileName = "../foundation/appexecfwk/test.cpp";
    std::string exceptStr = "test.cpp";

    std::string result = AppLogWrapper::GetBriefFileName(fileName.c_str());
    EXPECT_EQ(exceptStr, result);
}

/*
 * Feature: AppLogWrapper
 * Function: AppLogWrapper
 * SubFunction: NA
 * FunctionPoints: splice filename
 * EnvConditions: NA
 * CaseDescription: invalid filename
 */
HWTEST_F(AppExecFwkLogTest, AppLogWrapper_003, TestSize.Level0)
{
    std::string fileName = "test.cpp";
    std::string exceptStr = "";

    std::string result = AppLogWrapper::GetBriefFileName(fileName.c_str());
    EXPECT_EQ(exceptStr, result);

    fileName = "";
    result = AppLogWrapper::GetBriefFileName(fileName.c_str());
    EXPECT_EQ(exceptStr, result);

    result = AppLogWrapper::GetBriefFileName(nullptr);
    EXPECT_EQ(exceptStr, result);
}