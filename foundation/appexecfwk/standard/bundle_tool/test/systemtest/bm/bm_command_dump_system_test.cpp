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
const std::string STRING_BUNDLE_NAME = "com.ohos.tools.pageAbilityBundleForInstall";
const std::string STRING_BUNDLE_NAME_INVALID = STRING_BUNDLE_NAME + ".invalid";
}  // namespace

class BmCommandDumpSystemTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BmCommandDumpSystemTest::SetUpTestCase()
{}

void BmCommandDumpSystemTest::TearDownTestCase()
{}

void BmCommandDumpSystemTest::SetUp()
{
    // reset optind to 0
    optind = 0;
}

void BmCommandDumpSystemTest::TearDown()
{}

/**
 * @tc.number: Bm_Command_Dump_SystemTest_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -a" command.
 */
HWTEST_F(BmCommandDumpSystemTest, Bm_Command_Dump_SystemTest_0100, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install a valid bundle
    ToolSystemTest::InstallBundle(STRING_BUNDLE_PATH, true);

    // dump all bundle
    std::string command = "bm dump -a";
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_NE(commandResult, "");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);
}

/**
 * @tc.number: Bm_Command_Dump_SystemTest_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n <bundle-name>" command.
 */
HWTEST_F(BmCommandDumpSystemTest, Bm_Command_Dump_SystemTest_0200, Function | MediumTest | Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install a valid bundle
    ToolSystemTest::InstallBundle(STRING_BUNDLE_PATH, true);

    // dump a valid bundle
    std::string command = "bm dump -n " + STRING_BUNDLE_NAME;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_NE(commandResult, "");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);
}

/**
 * @tc.number: Bm_Command_Dump_SystemTest_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n <bundle-name>" command.
 */
HWTEST_F(BmCommandDumpSystemTest, Bm_Command_Dump_SystemTest_0300, Function | MediumTest | Level1)
{
    // dump an invalid bundle
    std::string command = "bm dump -n " + STRING_BUNDLE_NAME_INVALID;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_EQ(commandResult, "");
}
