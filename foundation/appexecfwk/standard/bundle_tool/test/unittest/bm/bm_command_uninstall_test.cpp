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

#define private public
#include "bundle_command.h"
#undef private
#include "bundle_installer_interface.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "mock_bundle_installer_host.h"
#include "mock_bundle_mgr_host.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

class BmCommandUninstallTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects();
    void SetMockObjects(BundleManagerShellCommand &cmd) const;

    std::string cmd_ = "uninstall";
    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BmCommandUninstallTest::SetUpTestCase()
{}

void BmCommandUninstallTest::TearDownTestCase()
{}

void BmCommandUninstallTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void BmCommandUninstallTest::TearDown()
{}

void BmCommandUninstallTest::MakeMockObjects()
{
    // mock a mgr host
    auto mgrHostPtr = sptr<IRemoteObject>(new MockBundleMgrHost());
    // mock a mgr proxy
    mgrProxyPtr_ = iface_cast<IBundleMgr>(mgrHostPtr);

    // mock a installer host
    auto installerHostPtr = sptr<IRemoteObject>(new MockBundleInstallerHost());
    // mock a installer proxy
    installerProxyPtr_ = iface_cast<IBundleInstaller>(installerHostPtr);
}

void BmCommandUninstallTest::SetMockObjects(BundleManagerShellCommand &cmd) const
{
    // set the mock mgr proxy
    cmd.bundleMgrProxy_ = mgrProxyPtr_;

    // set the mock installer proxy
    cmd.bundleInstallerProxy_ = installerProxyPtr_;
}

/**
 * @tc.number: Bm_Command_Uninstall_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0100, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall xxx" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0200, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -x" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0300, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-x",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -xxx" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0400, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_0500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall --x" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0500, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--x",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_0600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall --xxx" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0600, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_0700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall --h" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0700, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-h",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_0800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall --help" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0800, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--help",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_0900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -n" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_0900, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-n",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_1300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -n <bundle-name>" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_1000, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_UNINSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Uninstall_1100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -n <bundle-name> -m" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_1100, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-m",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_1200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -m <module-name>" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_1200, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-m",
        (char *)STRING_MODULE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n" + HELP_MSG_UNINSTALL);
}

/**
 * @tc.number: Bm_Command_Uninstall_1300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm uninstall -n <bundle-name> -m <module-name>" command.
 */
HWTEST_F(BmCommandUninstallTest, Bm_Command_Uninstall_1300, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-m",
        (char *)STRING_MODULE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_UNINSTALL_BUNDLE_OK + "\n");
}
