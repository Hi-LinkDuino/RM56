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

#include <fstream>
#include <future>
#include <gtest/gtest.h>

#include "bundle_mgr_service.h"
#include "directory_ex.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "mock_status_receiver.h"

using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace std::chrono_literals;
using OHOS::DelayedSingleton;

namespace {
const std::string BUNDLE_CODE_PATH = "/data/app/el1/bundle/public/";
const std::string BUNDLE_DATA_PATH = "/data/app/el2/100/base/";
const std::string THIRD_BUNDLE_PATH = "/data/test/bms_bundle/";
const std::string BUNDLE_NAME = "com.third.hiworld.example1";
const std::string MODULE_PACKAGE = "com.third.hiworld.example.h1";
const std::string SYSTEM_BUNDLE_NAME = "com.system.hiworld.examples2";
}  // namespace

class BmsBundleUninstallerModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    ErrCode InstallBundle(const std::string &bundlePath) const;
    ErrCode UninstallBundle(const std::string &bundleName) const;
    ErrCode UninstallHap(const std::string &bundleName, const std::string &modulePackage) const;
    void CheckFileExist(const std::string bundlename) const;
    void CheckFileNonExist(const std::string bundlename) const;
    void CheckBundleInfoExist(const std::string bundlename) const;
    void CheckBundleInfoNonExist(const std::string bundlename) const;
    void CheckHapDirNonExist(const std::string bundlename, const std::string &modulePackage) const;

    void StartBundleMgrService()
    {
        bms_ = DelayedSingleton<BundleMgrService>::GetInstance();
        bms_->OnStart();
    }

    void StopBundleMgrService()
    {
        bms_->OnStop();
        DelayedSingleton<BundleMgrService>::DestroyInstance();
    }

    void StartInstalld()
    {
        installdService_->Start();
    }

    void StopInstalld()
    {
        installdService_->Stop();
        InstalldClient::GetInstance()->ResetInstalldProxy();
    }

private:
    std::shared_ptr<BundleMgrService> bms_ = nullptr;
    std::shared_ptr<InstalldService> installdService_ = std::make_shared<InstalldService>();
};

ErrCode BmsBundleUninstallerModuleTest::InstallBundle(const std::string &bundlePath) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (installer == nullptr) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (receiver == nullptr) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    installer->Install(bundlePath, installParam, receiver);
    return receiver->GetResultCode();
}

ErrCode BmsBundleUninstallerModuleTest::UninstallBundle(const std::string &bundleName) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (installer == nullptr) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (receiver == nullptr) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, receiver);
    return receiver->GetResultCode();
}

ErrCode BmsBundleUninstallerModuleTest::UninstallHap(
    const std::string &bundleName, const std::string &modulePackage) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (installer == nullptr) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (receiver == nullptr) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, modulePackage, installParam, receiver);
    return receiver->GetResultCode();
}

void BmsBundleUninstallerModuleTest::CheckFileExist(const std::string bundlename) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundlename).c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir does not exists: " << (BUNDLE_DATA_PATH + bundlename);
    int codeExist = access((BUNDLE_CODE_PATH + bundlename).c_str(), F_OK);
    EXPECT_EQ(codeExist, 0) << "the ability code file does not exist: " << (BUNDLE_CODE_PATH + bundlename);
}

void BmsBundleUninstallerModuleTest::CheckFileNonExist(const std::string bundlename) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundlename).c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0) << "the bundle data dir exists: " << (BUNDLE_DATA_PATH + bundlename);
    int codeExist = access((BUNDLE_CODE_PATH + bundlename).c_str(), F_OK);
    EXPECT_NE(codeExist, 0) << "the ability code exists: " << (BUNDLE_CODE_PATH + bundlename);
}

void BmsBundleUninstallerModuleTest::CheckBundleInfoExist(const std::string bundlename) const
{
    BundleInfo info;
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    auto dataMgr = bms->GetDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    bool isBundleExist = dataMgr->GetBundleInfo(bundlename, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_TRUE(isBundleExist);
}

void BmsBundleUninstallerModuleTest::CheckBundleInfoNonExist(const std::string bundlename) const
{
    BundleInfo info;
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    auto dataMgr = bms->GetDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    bool isBundleExist = dataMgr->GetBundleInfo(bundlename, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_FALSE(isBundleExist);
}
void BmsBundleUninstallerModuleTest::CheckHapDirNonExist(
    const std::string bundlename, const std::string &modulePackage) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundlename + modulePackage).c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0) << "the bundle data dir exists: " << bundlename;
    int codeExist = access((BUNDLE_CODE_PATH + bundlename + modulePackage).c_str(), F_OK);
    EXPECT_NE(codeExist, 0) << "the ability code exists: " << bundlename;
}

void BmsBundleUninstallerModuleTest::SetUpTestCase()
{}

void BmsBundleUninstallerModuleTest::TearDownTestCase()
{}

void BmsBundleUninstallerModuleTest::SetUp()
{
    StartInstalld();
    StartBundleMgrService();
}

void BmsBundleUninstallerModuleTest::TearDown()
{
    std::cout << "BmsBundleUninstallerModuleTest::TearDown begin" << std::endl;
    if (access((BUNDLE_CODE_PATH + BUNDLE_NAME).c_str(), F_OK) == 0) {
        ErrCode uninstallResult = UninstallBundle(BUNDLE_NAME);
        EXPECT_EQ(uninstallResult, ERR_OK);
    }

    StopInstalld();
    StopBundleMgrService();
    std::this_thread::sleep_for(50ms);
}

/**
 * @tc.number: Uninstall_0100
 * @tc.name: test the third-party bundle can be uninstalled.
 * @tc.desc: the installed bundle can be uninstalled successfully.
 */
HWTEST_F(BmsBundleUninstallerModuleTest, Uninstall_0100, Function | MediumTest | Level1)
{
    std::string bundlePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    ErrCode installResult = InstallBundle(bundlePath);
    EXPECT_EQ(installResult, ERR_OK);

    CheckFileExist(BUNDLE_NAME);
    CheckBundleInfoExist(BUNDLE_NAME);

    ErrCode uninstallResult = UninstallBundle(BUNDLE_NAME);
    EXPECT_EQ(uninstallResult, ERR_OK);
    CheckFileNonExist(BUNDLE_NAME);
    CheckBundleInfoNonExist(BUNDLE_NAME);
}

/**
 * @tc.number: Uninstall_0200
 * @tc.name: test the third-party bundle can not be uninstalled by wrong bundle name.
 * @tc.desc: the installed bundle can not be uninstalled successfully.
 */
HWTEST_F(BmsBundleUninstallerModuleTest, Uninstall_0200, Function | MediumTest | Level1)
{
    std::string bundlePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    ErrCode installResult = InstallBundle(bundlePath);
    EXPECT_EQ(installResult, ERR_OK);

    CheckFileExist(BUNDLE_NAME);
    CheckBundleInfoExist(BUNDLE_NAME);

    std::string errorBundleName = "com.third.test0";
    ErrCode uninstallResult = UninstallBundle(errorBundleName);
    EXPECT_EQ(uninstallResult, ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE);

    CheckFileNonExist(errorBundleName);
    CheckBundleInfoNonExist(errorBundleName);
}

/**
 * @tc.number: Uninstall_0300
 * @tc.name: test the third-party bundle can't be uninstalled twice
 * @tc.desc: The first is successful but second is failed
 */
HWTEST_F(BmsBundleUninstallerModuleTest, Uninstall_0300, Function | MediumTest | Level2)
{
    std::string bundlePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    ErrCode installResult = InstallBundle(bundlePath);
    EXPECT_EQ(installResult, ERR_OK);

    CheckFileExist(BUNDLE_NAME);
    CheckBundleInfoExist(BUNDLE_NAME);

    ErrCode uninstallResult = UninstallBundle(BUNDLE_NAME);
    EXPECT_EQ(uninstallResult, ERR_OK);
    CheckFileNonExist(BUNDLE_NAME);
    CheckBundleInfoNonExist(BUNDLE_NAME);

    uninstallResult = UninstallBundle(BUNDLE_NAME);
    EXPECT_EQ(uninstallResult, ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE);
}

/**
 * @tc.number: Uninstall_0400
 * @tc.name: test the third-party app with one hap can be uninstalled.
 * @tc.desc: the installed app can be uninstalled successfully.
 */
HWTEST_F(BmsBundleUninstallerModuleTest, Uninstall_0400, Function | MediumTest | Level1)
{
    std::string bundlePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    ErrCode installResult = InstallBundle(bundlePath);
    EXPECT_EQ(installResult, ERR_OK);

    CheckFileExist(BUNDLE_NAME);
    CheckBundleInfoExist(BUNDLE_NAME);

    ErrCode uninstallResult = UninstallHap(BUNDLE_NAME, MODULE_PACKAGE);
    EXPECT_EQ(uninstallResult, ERR_OK);
    CheckFileNonExist(BUNDLE_NAME);
    CheckBundleInfoNonExist(BUNDLE_NAME);
}

/**
 * @tc.number: Uninstall_0500
 * @tc.name: test the third-party app with two haps can be uninstalled
 * @tc.desc: the installed bundle can not be uninstalled successfully.
 */
HWTEST_F(BmsBundleUninstallerModuleTest, Uninstall_0500, Function | MediumTest | Level1)
{
    std::string bundlePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    ErrCode installResult = InstallBundle(bundlePath);
    EXPECT_EQ(installResult, ERR_OK);

    bundlePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installResult = InstallBundle(bundlePath);
    EXPECT_EQ(installResult, ERR_OK);

    CheckFileExist(BUNDLE_NAME);
    CheckBundleInfoExist(BUNDLE_NAME);

    ErrCode uninstallResult = UninstallHap(BUNDLE_NAME, MODULE_PACKAGE);
    EXPECT_EQ(uninstallResult, ERR_OK);
    CheckHapDirNonExist(BUNDLE_NAME, MODULE_PACKAGE);
    CheckFileExist(BUNDLE_NAME);
    CheckBundleInfoExist(BUNDLE_NAME);
}

/**
 * @tc.number: Uninstall_0600
 * @tc.name: test the big third-party app can be uninstalled
 * @tc.desc: the installed bundle can not be uninstalled successfully.
 */
HWTEST_F(BmsBundleUninstallerModuleTest, Uninstall_0600, Function | MediumTest | Level1)
{
    BundleInfo info;
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    auto dataMgr = bms->GetDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    std::string bundlePath = THIRD_BUNDLE_PATH + "bmsThirdBundle13" + Constants::INSTALL_FILE_SUFFIX;
    std::string bundleName = "com.third.hiworld.example5";
    ErrCode installResult = InstallBundle(bundlePath);
    EXPECT_EQ(installResult, ERR_OK);
    bool isBundleExist = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_TRUE(isBundleExist);
    CheckFileExist(bundleName);
    CheckBundleInfoExist(bundleName);

    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    InstallParam installParam;
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK);
    isBundleExist = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_FALSE(isBundleExist);
    CheckFileNonExist(bundleName);
    CheckBundleInfoNonExist(bundleName);
}
