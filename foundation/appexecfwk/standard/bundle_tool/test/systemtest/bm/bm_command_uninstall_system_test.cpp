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

#include <thread>
#include "bundle_command.h"
#include "tool_system_test.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace {
const std::string STRING_BUNDLE_PATH = "/data/test/resource/bm/pageAbilityBundleForUninstall.hap";
const std::string STRING_BUNDLE_NAME = "com.ohos.tools.pageAbilityBundleForUninstall";
const std::string STRING_BUNDLE_NAME_INVALID = STRING_BUNDLE_NAME + ".invalid";
const std::string STRING_MODULE_NAME = "com.ohos.tools.pageAbilityForUninstall";
const std::string STRING_MODULE_NAME_INVALID = STRING_MODULE_NAME + ".invalid";
}  // namespace

class BmCommandUninstallSystemTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BmCommandUninstallSystemTest::SetUpTestCase()
{}

void BmCommandUninstallSystemTest::TearDownTestCase()
{}

void BmCommandUninstallSystemTest::SetUp()
{
    // reset optind to 0
    optind = 0;
}

void BmCommandUninstallSystemTest::TearDown()
{}

/**
 * @tc.number: Bm_Command_Uninstall_SystemTest_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -n <bundle-name>" command.
 */
HWTEST_F(BmCommandUninstallSystemTest, Bm_Command_Uninstall_SystemTest_0100, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install the bundle
    ToolSystemTest::InstallBundle(STRING_BUNDLE_PATH, true);

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);
}

/**
 * @tc.number: Bm_Command_Uninstall_SystemTest_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -n <bundle-name>" command.
 */
HWTEST_F(BmCommandUninstallSystemTest, Bm_Command_Uninstall_SystemTest_0200, Function | MediumTest | Level1)
{
    // uninstall an invalid bundle
    std::string command = "bm uninstall -n " + STRING_BUNDLE_NAME_INVALID;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_EQ(commandResult, STRING_UNINSTALL_BUNDLE_NG + "\n");
}

/**
 * @tc.number: Bm_Command_Uninstall_SystemTest_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -n <bundle-name> -m <module-name>" command.
 */
HWTEST_F(BmCommandUninstallSystemTest, Bm_Command_Uninstall_SystemTest_0300, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install the bundle
    ToolSystemTest::InstallBundle(STRING_BUNDLE_PATH, true);

    // uninstall an invalid bundle
    std::string command = "bm uninstall -n " + STRING_BUNDLE_NAME + " -m " + STRING_MODULE_NAME;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_EQ(commandResult, STRING_UNINSTALL_BUNDLE_OK + "\n");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);
}

/**
 * @tc.number: Bm_Command_Uninstall_SystemTest_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -n <bundle-name> -m <module-name>" command.
 */
HWTEST_F(BmCommandUninstallSystemTest, Bm_Command_Uninstall_SystemTest_0400, Function | MediumTest | Level1)
{
    // uninstall an invalid module
    std::string command = "bm uninstall -n " + STRING_BUNDLE_NAME + " -m " + STRING_MODULE_NAME_INVALID;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_EQ(commandResult, STRING_UNINSTALL_BUNDLE_NG + "\n");
}
