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

#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

#include "appexecfwk_errors.h"
#include "bundle_info.h"
#include "bundle_installer_host.h"
#include "bundle_mgr_service.h"
#include "bundle_data_storage_database.h"
#include "directory_ex.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "install_param.h"
#include "mock_status_receiver.h"
#include "system_bundle_installer.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
const std::string BUNDLE_NAME = "com.example.l3jsdemo";
const std::string MODULE_PACKAGE = "com.example.l3jsdemo";
const std::string MODULE_PACKAGE1 = "com.example.l2jsdemo";
const std::string ERROR_BUNDLE_NAME = "com.example.bundle.uninstall.error";
const std::string ERROR_MODULE_PACKAGE_NAME = "com.example.module.uninstall.error";
const std::string BUNDLE_FILE_PATH = "/data/test/resource/bms/uninstall_bundle/right.hap";
const std::string BUNDLE_FILE_PATH1 = "/data/test/resource/bms/uninstall_bundle/right1.hap";
const std::string BUNDLE_DATA_DIR1 = "/data/app/el1/100/base/com.example.l3jsdemo";
const std::string BUNDLE_DATA_DIR2 = "/data/app/el1/100/database/com.example.l3jsdemo";
const std::string BUNDLE_DATA_DIR3 = "/data/app/el2/100/base/com.example.l3jsdemo";
const std::string BUNDLE_DATA_DIR4 = "/data/app/el2/100/database/com.example.l3jsdemo";
const std::string BUNDLE_CODE_DIR = "/data/app/el1/bundle/public/com.example.l3jsdemo";
const std::string MODULE_CODE_DIR1 = "/data/app/el1/bundle/public/com.example.l3jsdemo/com.example.l3jsdemo";
const std::string MODULE_CODE_DIR2 = "/data/app/el1/bundle/public/com.example.l3jsdemo/com.example.l2jsdemo";
const std::string RESOURCE_TEST_PATH = "/data/test/resource/bms/install_bundle/test/";
const std::string ROOT_DIR = "/data/app";
const std::string DB_FILE_PATH = "/data/bundlemgr";
const int32_t INVALID_USERID = 0;
const int32_t USERID = 100;
const int32_t ROOT_UID = 0;
const int32_t WAIT_TIME = 5; // init mocked bms
}  // namespace

class BmsBundleUninstallerTest : public testing::Test {
public:
    BmsBundleUninstallerTest();
    ~BmsBundleUninstallerTest();

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    ErrCode InstallMultipleBundles(const std::vector<std::string> &filePaths, bool &&flag) const;
    ErrCode UninstallBundle(const std::string &bundleName, int32_t userId) const;
    ErrCode UninstallModule(const std::string &bundleName, const std::string &modulePackage, int32_t userId) const;
    void CheckFileExist() const;
    void CheckModuleFileExist(const std::string&codePath) const;
    void CheckFileNonExist() const;
    void CheckModuleFileNonExist(const std::string&codePath) const;
    void StopInstalldService() const;
    void StartInstalldService() const;
    void StartBundleService();
    void StopBundleService();
    void CheckBundleInfoExist() const;
    void CheckBundleInfoNonExist() const;
    const std::shared_ptr<BundleMgrService> GetBundleMgrService() const;
    const std::shared_ptr<BundleDataMgr> GetBundleDataMgr() const;
    void ClearBundleInfo(const std::string &bundleName) const;

private:
    std::shared_ptr<InstalldService> installdService_ = std::make_unique<InstalldService>();
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
};

BmsBundleUninstallerTest::BmsBundleUninstallerTest()
{}

BmsBundleUninstallerTest::~BmsBundleUninstallerTest()
{}

void BmsBundleUninstallerTest::SetUpTestCase()
{
    if (access(ROOT_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(ROOT_DIR);
        EXPECT_TRUE(result);
    }
    if (chown(ROOT_DIR.c_str(), ROOT_UID, ROOT_UID) != 0) {
        EXPECT_TRUE(false);
    }
    if (chmod(ROOT_DIR.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
        EXPECT_TRUE(false);
    }
}

void BmsBundleUninstallerTest::TearDownTestCase()
{}

void BmsBundleUninstallerTest::SetUp()
{
    StartBundleService();
    StartInstalldService();
}

void BmsBundleUninstallerTest::TearDown()
{
}

ErrCode BmsBundleUninstallerTest::UninstallBundle(const std::string &bundleName, int32_t userId) const
{
    if (!bundleMgrService_) {
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    auto installer = bundleMgrService_->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (!receiver) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.userId = userId;
    installParam.installFlag = InstallFlag::NORMAL;
    bool result = installer->Uninstall(bundleName, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

ErrCode BmsBundleUninstallerTest::UninstallModule(const std::string &bundleName, const std::string &modulePackage,
    int32_t userId) const
{
    if (!bundleMgrService_) {
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    auto installer = bundleMgrService_->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (!receiver) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.userId = userId;
    installParam.installFlag = InstallFlag::NORMAL;
    bool result = installer->Uninstall(bundleName, modulePackage, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

void BmsBundleUninstallerTest::CheckFileExist() const
{
    int bundleDataExist = access(BUNDLE_DATA_DIR1.c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0);

    bundleDataExist = access(BUNDLE_DATA_DIR2.c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0);

    bundleDataExist = access(BUNDLE_DATA_DIR3.c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0);

    bundleDataExist = access(BUNDLE_DATA_DIR4.c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0);

    int bundleCodeExist = access(BUNDLE_CODE_DIR.c_str(), F_OK);
    EXPECT_EQ(bundleCodeExist, 0);
}

void BmsBundleUninstallerTest::CheckModuleFileExist(const std::string&codePath) const
{
    int moduleCodeExist = access(codePath.c_str(), F_OK);
    EXPECT_EQ(moduleCodeExist, 0);
}

void BmsBundleUninstallerTest::CheckFileNonExist() const
{
    int bundleDataExist = access(BUNDLE_DATA_DIR1.c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0);

    bundleDataExist = access(BUNDLE_DATA_DIR2.c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0);

    bundleDataExist = access(BUNDLE_DATA_DIR3.c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0);

    bundleDataExist = access(BUNDLE_DATA_DIR4.c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0);

    int bundleCodeExist = access(BUNDLE_CODE_DIR.c_str(), F_OK);
    EXPECT_NE(bundleCodeExist, 0);
}

void BmsBundleUninstallerTest::CheckModuleFileNonExist(const std::string&codePath) const
{
    int moduleCodeExist = access(codePath.c_str(), F_OK);
    EXPECT_NE(moduleCodeExist, 0);
}

void BmsBundleUninstallerTest::StopInstalldService() const
{
    if (installdService_->IsServiceReady()) {
        installdService_->Stop();
        InstalldClient::GetInstance()->ResetInstalldProxy();
    }
}

void BmsBundleUninstallerTest::StartInstalldService() const
{
    if (!installdService_->IsServiceReady()) {
        installdService_->Start();
    }
}

void BmsBundleUninstallerTest::StartBundleService()
{
    if (!bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStart();
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
    }
}

void BmsBundleUninstallerTest::StopBundleService()
{
    if (bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStop();
        bundleMgrService_ = nullptr;
    }
}

void BmsBundleUninstallerTest::ClearBundleInfo(const std::string &bundleName) const
{
    if (bundleMgrService_ == nullptr) {
        return;
    }
    auto dataMgt = bundleMgrService_->GetDataMgr();
    if (dataMgt == nullptr) {
        return;
    }
    auto dataStorage = dataMgt->GetDataStorage();
    if (dataStorage == nullptr) {
        return;
    }

    // clear innerBundleInfo from data manager
    dataMgt->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_START);
    dataMgt->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_SUCCESS);

    InnerBundleInfo innerBundleInfo;
    ApplicationInfo applicationInfo;
    applicationInfo.bundleName = bundleName;
    innerBundleInfo.SetBaseApplicationInfo(applicationInfo);
    // clear innerBundleInfo from data storage
    bool result = dataStorage->DeleteStorageBundleInfo(innerBundleInfo);
    EXPECT_TRUE(result) << "the bundle info in db clear fail: " << bundleName;
}

void BmsBundleUninstallerTest::CheckBundleInfoExist() const
{
    EXPECT_NE(bundleMgrService_, nullptr);
    auto dataMgr = bundleMgrService_->GetDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo info;
    bool isBundleExist = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_TRUE(isBundleExist);
}

void BmsBundleUninstallerTest::CheckBundleInfoNonExist() const
{
    EXPECT_NE(bundleMgrService_, nullptr);
    auto dataMgr = bundleMgrService_->GetDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo info;
    bool isBundleExist = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_FALSE(isBundleExist);
}

const std::shared_ptr<BundleMgrService> BmsBundleUninstallerTest::GetBundleMgrService() const
{
    return bundleMgrService_;
}

const std::shared_ptr<BundleDataMgr> BmsBundleUninstallerTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

ErrCode BmsBundleUninstallerTest::InstallMultipleBundles(const std::vector<std::string> &filePaths,
    bool &&flag) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    if (!receiver) {
        EXPECT_FALSE(true) << "the receiver is nullptr";
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    InstallParam installParam;
    installParam.userId = USERID;
    installParam.installFlag = flag ? InstallFlag::NORMAL : InstallFlag::REPLACE_EXISTING;
    bool result = installer->Install(filePaths, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

/**
 * @tc.number: Bundle_Uninstall_0100
 * @tc.name: test the empty bundle name will return fail
 * @tc.desc: 1. the bundle name is empty
 *           2. the empty bundle name will return fail
 * @tc.require: AR000GHLL7
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0100, Function | SmallTest | Level0)
{
    ErrCode result = UninstallBundle("", USERID);
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_INVALID_NAME);
}

/**
 * @tc.number: Bundle_Uninstall_0200
 * @tc.name: test the error bundle name will return fail
 * @tc.desc: 1. the bundle name is error
 *           2. the error bundle name will return fail
 * @tc.require: AR000GHLL7
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0200, Function | SmallTest | Level0)
{
    ErrCode result = UninstallBundle(ERROR_BUNDLE_NAME, USERID);
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE);
}

/**
 * @tc.number: Bundle_Uninstall_0300
 * @tc.name: test the uninstall installed bundle with incorrect userId
 * @tc.desc: 1. the bundle name is correct
 *           2. the bundle will be uninstalled failed
 * @tc.require: AR000GHLL7
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0300, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    filePaths.emplace_back(RESOURCE_TEST_PATH);
    ErrCode installRes = InstallMultipleBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);
    CheckFileExist();
    CheckModuleFileExist(MODULE_CODE_DIR1);
    CheckModuleFileExist(MODULE_CODE_DIR2);

    ErrCode result = UninstallBundle(BUNDLE_NAME, INVALID_USERID);
    EXPECT_NE(result, ERR_OK);

    ClearBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: Bundle_Uninstall_0400
 * @tc.name: test the uninstall installed bundle with correct userId
 * @tc.desc: 1. the bundle name is correct
 *           2. the bundle will be uninstalled successfully
 * @tc.require: AR000GHLL7
 */
HWTEST_F(BmsBundleUninstallerTest, Bundle_Uninstall_0400, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    filePaths.emplace_back(RESOURCE_TEST_PATH);
    ErrCode installRes = InstallMultipleBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);
    CheckFileExist();
    CheckModuleFileExist(MODULE_CODE_DIR1);
    CheckModuleFileExist(MODULE_CODE_DIR2);

    ErrCode result = UninstallBundle(BUNDLE_NAME, USERID);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.number: Module_Uninstall_0100
 * @tc.name: test the installed Module can be uninstalled
 * @tc.desc: 1. the bundle name is empty
 *           2. the empty bundle name will return fail
 * @tc.require: AR000GHQ9D
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0100, Function | SmallTest | Level0)
{
    ErrCode result = UninstallModule("", MODULE_PACKAGE1, USERID);
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_INVALID_NAME);
}

/**
 * @tc.number: Module_Uninstall_0200
 * @tc.name: test the installed Module can be uninstalled
 * @tc.desc: 1. the bundle name is error name
 *           2. the error bundle name will return fail
 * @tc.require: AR000GHQ9D
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0200, Function | SmallTest | Level0)
{
    ErrCode result = UninstallModule(ERROR_BUNDLE_NAME, MODULE_PACKAGE1, USERID);
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE);
}

/**
 * @tc.number: Module_Uninstall_0300
 * @tc.name: test the installed Module can be uninstalled
 * @tc.desc: 1. the Module name is empty
 *           2. the empty Module name will return fail
 * @tc.require: AR000GHQ9D
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0300, Function | SmallTest | Level0)
{
    ErrCode result = UninstallModule(BUNDLE_NAME, "", USERID);
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_INVALID_NAME);
}

/**
 * @tc.number: Module_Uninstall_0400
 * @tc.name: test the error Module name will return fail.
 * @tc.desc: 1. the Module name is error.
 *           2. the error Module name will return fail.
 * @tc.require: AR000GHQ9D
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0400, Function | SmallTest | Level0)
{
    ErrCode result = UninstallModule(BUNDLE_NAME, ERROR_MODULE_PACKAGE_NAME, USERID);
    EXPECT_EQ(result, ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE);
}

/**
 * @tc.number: Module_Uninstall_0500
 * @tc.name: test the uninstall module with correct bundle name, module name and invalid userId
 * @tc.desc: 1. the bundl name and module name are corrct and userId is invalid.
 *           2. the uninstallation will be sfailed
 * @tc.require: AR000GHQ9D
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0500, Function | SmallTest | Level0)
{
    ErrCode result = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE1, INVALID_USERID);
    EXPECT_NE(result, ERR_OK);
}

/**
 * @tc.number: Module_Uninstall_0600
 * @tc.name: test the correct Module name will be uninstalled successfully.
 * @tc.desc: 1. the Module name is com.example.l2jsdemo.
 *           2. the correct Module will be uninstalled successfully.
 * @tc.require: AR000GHQ9D
 */
HWTEST_F(BmsBundleUninstallerTest, Module_Uninstall_0600, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    filePaths.emplace_back(RESOURCE_TEST_PATH);
    ErrCode installRes = InstallMultipleBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);
    CheckFileExist();
    CheckModuleFileExist(MODULE_CODE_DIR1);
    CheckModuleFileExist(MODULE_CODE_DIR2);

    installRes = UninstallModule(BUNDLE_NAME, MODULE_PACKAGE1, USERID);
    EXPECT_EQ(installRes, ERR_OK);

    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);

    ApplicationInfo info;
    bool result = dataMgr->GetApplicationInfo(BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info);
    EXPECT_TRUE(result);

    ClearBundleInfo(BUNDLE_NAME);
}