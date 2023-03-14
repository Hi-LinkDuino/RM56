/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
class BmCommandTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects();
    void SetMockObjects(BundleManagerShellCommand &cmd) const;

    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BmCommandTest::SetUpTestCase()
{}

void BmCommandTest::TearDownTestCase()
{}

void BmCommandTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void BmCommandTest::TearDown()
{}

void BmCommandTest::MakeMockObjects()
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

void BmCommandTest::SetMockObjects(BundleManagerShellCommand &cmd) const
{
    // set the mock mgr proxy
    cmd.bundleMgrProxy_ = mgrProxyPtr_;

    // set the mock installer proxy
    cmd.bundleInstallerProxy_ = installerProxyPtr_;
}

/**
 * @tc.number: Bm_Command_0001
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0001, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG);
}

/**
 * @tc.number: Bm_Command_0002
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm xxx" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0002, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Bm_Command_0003
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm -xxx" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0003, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"-xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Bm_Command_0004
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm --xxx" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0004, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"--xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), cmd.GetCommandErrorMsg() + HELP_MSG);
}

/**
 * @tc.number: Bm_Command_0005
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm help" command.
 */
HWTEST_F(BmCommandTest, Bm_Command_0005, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"help",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG);
}

/**
 * @tc.number: Bm_Command_Clean_0001
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0001, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_CLEAN);
}

/**
 * @tc.number: Bm_Command_Clean_0002
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean xx" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0002, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_CLEAN);
}

/**
 * @tc.number: Bm_Command_Clean_0003
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean -n" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0003, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"-n",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_CLEAN);
}

/**
 * @tc.number: Bm_Command_Clean_0004
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean -n <bundle-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0004, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_DATA_OR_CACHE_OPTION + "\n" + HELP_MSG_CLEAN);
}

/**
 * @tc.number: Bm_Command_Clean_0005
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean -n <bundle-name> xxx" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0005, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_DATA_OR_CACHE_OPTION + "\n" + HELP_MSG_CLEAN);
}

/**
 * @tc.number: Bm_Command_Clean_0006
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean -n <bundle-name> -d" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0006, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-d",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_CLEAN_DATA_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Clean_0007
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean -n <bundle-name> -c" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0007, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-c",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_CLEAN_CACHE_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Clean_0008
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean -c" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0008, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"-c",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n" + HELP_MSG_CLEAN);
}

/**
 * @tc.number: Bm_Command_Clean_0009
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm clean -d" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Clean_0009, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"clean",
        (char *)"-d",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n" + HELP_MSG_CLEAN);
}

/**
 * @tc.number: Bm_Command_Enable_0001
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm enable" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Enable_0001, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"enable",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_ENABLE);
}

/**
 * @tc.number: Bm_Command_Enable_0002
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm enable -n <bundle-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Enable_0002, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"enable",
        (char *)"-n",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_ENABLE);
}

/**
 * @tc.number: Bm_Command_Enable_0003
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm enable -n <bundle-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Enable_0003, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"enable",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_ENABLE_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Enable_0004
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm enable -n <bundle-name> -a <ability-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Enable_0004, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"enable",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-a",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_ENABLE);
}

/**
 * @tc.number: Bm_Command_Enable_0005
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm enable -n <bundle-name> -a <ability-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Enable_0005, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"enable",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_ENABLE_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Disable_0001
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm disable" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Disable_0001, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"disable",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_DISABLE);
}

/**
 * @tc.number: Bm_Command_Disable_0002
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm disable -n <bundle-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Disable_0002, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"disable",
        (char *)"-n",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_DISABLE);
}

/**
 * @tc.number: Bm_Command_Disable_0003
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm disable -n <bundle-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Disable_0003, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"disable",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_DISABLE_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Disable_0004
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm disable -n <bundle-name> -a <ability-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Disable_0004, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"disable",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-a",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_DISABLE);
}

/**
 * @tc.number: Bm_Command_Disable_0005
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm disable -n <bundle-name> -a <ability-name>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Disable_0005, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"disable",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-a",
        (char *)STRING_ABILITY_NAME.c_str(),
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_DISABLE_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Disable_0006
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm disable -n <bundle-name> -u <user-id>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Disable_0006, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"disable",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-u",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_DISABLE);
}

/**
 * @tc.number: Bm_Command_Disable_0007
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm disable -n <bundle-name> -u <user-id>" command.
 * @tc.require: AR000GJUII
 */
HWTEST_F(BmCommandTest, Bm_Command_Disable_0007, Function | MediumTest | Level1)
{
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"disable",
        (char *)"-n",
        (char *)STRING_BUNDLE_NAME.c_str(),
        (char *)"-u",
        (char *)"100",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_DISABLE_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Get_0001
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0001, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_GET);
}

/**
 * @tc.number: Bm_Command_Get_0002
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get -u" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0002, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"-u",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    std::string result = cmd.ExecCommand();
    auto pos = result.find(STRING_GET_UDID_OK);

    EXPECT_NE(pos, std::string::npos);
}

/**
 * @tc.number: Bm_Command_Get_0003
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get -x" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0003, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"-x",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INCORRECT_OPTION + "\n" + HELP_MSG_GET);
}

/**
 * @tc.number: Bm_Command_Get_0004
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get -u -x" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0004, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"-u",
        (char *)"-x",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_GET);
}

/**
 * @tc.number: Bm_Command_Get_0005
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get -u xxx" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0005, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"-u",
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_GET);
}

/**
 * @tc.number: Bm_Command_Get_0006
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get --udid" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0006, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"--udid",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    std::string result = cmd.ExecCommand();
    auto pos = result.find(STRING_GET_UDID_OK);

    EXPECT_NE(pos, std::string::npos);
}

/**
 * @tc.number: Bm_Command_Get_0007
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get --xxx" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0007, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"--xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INCORRECT_OPTION + "\n" + HELP_MSG_GET);
}

/**
 * @tc.number: Bm_Command_Get_0008
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get --udid -x" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0008, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"--udid",
        (char *)"-x",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_GET);
}

/**
 * @tc.number: Bm_Command_Get_0009
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm get -u xxx" command.
 * @tc.require: AR000GJ4K9
 */
HWTEST_F(BmCommandTest, Bm_Command_Get_0009, Function | MediumTest | Level1)
{
    // install a bundle
    char *argv[] = {
        (char *)TOOL_NAME.c_str(),
        (char *)"get",
        (char *)"--udid",
        (char *)"xxx",
        (char *)"",
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_GET);
}
} // namespace OHOS