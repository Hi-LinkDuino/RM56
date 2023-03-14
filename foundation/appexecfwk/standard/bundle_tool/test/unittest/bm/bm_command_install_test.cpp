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
#include "bundle_constants.h"
#include "bundle_installer_interface.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "mock_bundle_mgr_host.h"
#include "mock_bundle_installer_host.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

class BmCommandInstallTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects();
    void SetMockObjects(BundleManagerShellCommand &cmd) const;

    std::string cmd_ = "install";
    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BmCommandInstallTest::SetUpTestCase()
{}

void BmCommandInstallTest::TearDownTestCase()
{}

void BmCommandInstallTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void BmCommandInstallTest::TearDown()
{}

void BmCommandInstallTest::MakeMockObjects()
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

void BmCommandInstallTest::SetMockObjects(BundleManagerShellCommand &cmd) const
{
    // set the mock mgr proxy
    cmd.bundleMgrProxy_ = mgrProxyPtr_;

    // set the mock installer proxy
    cmd.bundleInstallerProxy_ = installerProxyPtr_;
}

/**
 * @tc.number: Bm_Command_Install_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0100, Function | MediumTest | Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install xxx" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0200, Function | MediumTest | Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -x" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0300, Function | MediumTest | Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -xxx" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0400, Function | MediumTest | Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --x" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0500, Function | MediumTest | Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --xxx" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0600, Function | MediumTest | Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --h" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0700, Function | MediumTest | Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --help" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0800, Function | MediumTest | Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0900, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_1000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -r" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1000, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-r",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(
        cmd.ExecCommand(), "error: you must specify a bundle path with '-p' or '--bundle-path'.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_1100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1100, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -r" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1200, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)"-r",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -r -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1300, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-r",
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path>" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1600, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_OTHER_BUNDLE_PATH.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1700, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_INSTALL_PATH1.c_str(),
        (char *)STRING_BUNDLE_INSTALL_PATH2.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1800, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_BUNDLE_INSTALL_PATH2.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --bundle-path <bundle-path> <bundle-path>" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1900, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--bundle-path",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_OTHER_BUNDLE_PATH.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2000, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--bundle-path",
        (char *)STRING_BUNDLE_INSTALL_PATH1.c_str(),
        (char *)STRING_BUNDLE_INSTALL_PATH2.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2100, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"--bundle-path",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_BUNDLE_INSTALL_PATH2.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path> -r" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2300, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_OTHER_BUNDLE_PATH.c_str(),
        (char *)"-r",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -r -p <bundle-path> <bundle-path>" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2400, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-r",
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_OTHER_BUNDLE_PATH.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p -r <bundle-path> <bundle-path>" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2600, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)"-r",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_OTHER_BUNDLE_PATH.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_2700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path> -u xxx" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2700, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_OTHER_BUNDLE_PATH.c_str(),
        (char *)"-u",
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_2800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path> -u" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2800, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_OTHER_BUNDLE_PATH.c_str(),
        (char *)"-u",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_2900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path> -u" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2900, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)cmd_.c_str(),
        (char *)"-p",
        (char *)STRING_BUNDLE_PATH.c_str(),
        (char *)STRING_OTHER_BUNDLE_PATH.c_str(),
        (char *)"-u",
        (char *)DEFAULT_USER_ID.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}