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
const std::string STRING_BUNDLE_PATH = "/data/test/resource/bm/pageAbilityBundleForInstall.hap";
const std::string STRING_BUNDLE_PATH_INVALID = STRING_BUNDLE_PATH + ".invalid";
const std::string STRING_BUNDLE_NAME = "com.ohos.tools.pageAbilityBundleForInstall";

const std::string STRING_BUNDLE_PATH_NO_SIGNATURE = "/data/test/resource/bm/pageAbilityBundleForInstallNoSignature.hap";
}  // namespace

class BmCommandInstallSystemTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BmCommandInstallSystemTest::SetUpTestCase()
{}

void BmCommandInstallSystemTest::TearDownTestCase()
{}

void BmCommandInstallSystemTest::SetUp()
{
    // reset optind to 0
    optind = 0;
}

void BmCommandInstallSystemTest::TearDown()
{}

/**
 * @tc.number: Bm_Command_Install_SystemTest_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallSystemTest, Bm_Command_Install_SystemTest_0100, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install a valid bundle
    ToolSystemTest::InstallBundle(STRING_BUNDLE_PATH, true);

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);
}

/**
 * @tc.number: Bm_Command_Install_SystemTest_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallSystemTest, Bm_Command_Install_SystemTest_0200, Function | MediumTest | Level1)
{
    // install an invalid bundle
    std::string command = "bm install -p " + STRING_BUNDLE_PATH_INVALID;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_EQ(commandResult, STRING_INSTALL_BUNDLE_NG + "\n");
}

/**
 * @tc.number: Bm_Command_Install_SystemTest_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -r" command.
 */
HWTEST_F(BmCommandInstallSystemTest, Bm_Command_Install_SystemTest_0300, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install a valid bundle
    ToolSystemTest::InstallBundle(STRING_BUNDLE_PATH, true);

    // install the same bundle
    std::string command = "bm install -p " + STRING_BUNDLE_PATH + " -r";
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_EQ(commandResult, STRING_INSTALL_BUNDLE_OK + "\n");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);
}

/**
 * @tc.number: Bm_Command_Install_SystemTest_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallSystemTest, Bm_Command_Install_SystemTest_0400, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install a valid bundle with no signature
    std::string command = "bm install -p " + STRING_BUNDLE_PATH_NO_SIGNATURE;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_NE(commandResult, STRING_INSTALL_BUNDLE_OK + "\n");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);
}

