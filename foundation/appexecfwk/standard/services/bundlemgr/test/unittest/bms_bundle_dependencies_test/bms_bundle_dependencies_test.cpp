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

#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "bundle_info.h"
#include "bundle_installer_host.h"
#include "bundle_mgr_service.h"
#include "bundle_permission_mgr.h"
#include "bundle_verify_mgr.h"
#include "inner_bundle_info.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "mock_status_receiver.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using namespace OHOS::Security;

namespace OHOS {
namespace {
const std::string BUNDLE_NAME = "com.example.dependenciestest";
const std::string HAP_FILE_PATH = "/data/test/resource/bms/dependencies/";
const std::string HAP_NAME_ENTRY = "dependencies_entry.hap";
const std::string HAP_NAME_ENTRY_A = "dependencies_entry_A.hap";
const std::string HAP_NAME_ENTRY_B = "dependencies_entry_B.hap";
const std::string HAP_NAME_ENTRY_C = "dependencies_entry_C.hap";
const std::string MODULE_NAME_ENTRY = "entry";
const std::string MODULE_NAME_ENTRY_A = "entry_A";
const std::string MODULE_NAME_ENTRY_B = "entry_B";
const std::string MODULE_NAME_ENTRY_C = "entry_C";
const int32_t USERID = 100;
const size_t ZERO = 0;
const size_t ONE = 1;
const size_t THREE = 3;
const int32_t WAIT_TIME = 5; // init mocked bms
}  // namespace

class BmsBundleDependenciesTest : public testing::Test {
public:
    BmsBundleDependenciesTest();
    ~BmsBundleDependenciesTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    ErrCode InstallBundle(const std::string &bundlePath) const;
    ErrCode UpdateBundle(const std::string &bundlePath) const;
    ErrCode UnInstallBundle(const std::string &bundleName) const;
    const std::shared_ptr<BundleDataMgr> GetBundleDataMgr() const;
    void StartInstalldService() const;
    void StartBundleService();

private:
    std::shared_ptr<InstalldService> installdService_ = std::make_shared<InstalldService>();
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
};

BmsBundleDependenciesTest::BmsBundleDependenciesTest()
{}

BmsBundleDependenciesTest::~BmsBundleDependenciesTest()
{}

void BmsBundleDependenciesTest::SetUpTestCase()
{}

void BmsBundleDependenciesTest::TearDownTestCase()
{}

void BmsBundleDependenciesTest::SetUp()
{
    StartBundleService();
    StartInstalldService();
}

void BmsBundleDependenciesTest::TearDown()
{}

ErrCode BmsBundleDependenciesTest::InstallBundle(const std::string &bundlePath) const
{
    if (!bundleMgrService_) {
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
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
    installParam.installFlag = InstallFlag::NORMAL;
    installParam.userId = USERID;
    bool result = installer->Install(bundlePath, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

ErrCode BmsBundleDependenciesTest::UpdateBundle(const std::string &bundlePath) const
{
    if (!bundleMgrService_) {
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
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
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installParam.userId = USERID;
    bool result = installer->Install(bundlePath, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

ErrCode BmsBundleDependenciesTest::UnInstallBundle(const std::string &bundleName) const
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
    installParam.installFlag = InstallFlag::NORMAL;
    installParam.userId = USERID;
    bool result = installer->Uninstall(bundleName, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

void BmsBundleDependenciesTest::StartInstalldService() const
{
    if (!installdService_->IsServiceReady()) {
        installdService_->Start();
    }
}

void BmsBundleDependenciesTest::StartBundleService()
{
    if (!bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStart();
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
    }
}

const std::shared_ptr<BundleDataMgr> BmsBundleDependenciesTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

/**
 * @tc.number: BmsBundleDependenciesTest
 * Function: GetAllDependentModuleName
 * @tc.name: test wrong bundleName
 * @tc.require: SR000H0DUR
 * @tc.desc: 1. system running normally
 *           2. get the dependentModuleName
 */
HWTEST_F(BmsBundleDependenciesTest, BmsBundleDependenciesTest_0100, Function | SmallTest | Level0)
{
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    std::vector<std::string> dependentModuleNames;
    bool result = dataMgr->GetAllDependentModuleNames(BUNDLE_NAME, MODULE_NAME_ENTRY_C, dependentModuleNames);
    EXPECT_FALSE(result);
    EXPECT_EQ(dependentModuleNames.size(), ZERO);
}

/**
 * @tc.number: BmsBundleDependenciesTest
 * Function: GetAllDependentModuleName
 * @tc.name: test dependencies is empty
 * @tc.require: SR000H0DUR
 * @tc.desc: 1. system running normally
 *           2. install a hap
 *           3. get the dependentModuleName
 */
HWTEST_F(BmsBundleDependenciesTest, BmsBundleDependenciesTest_0200, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_C);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    std::vector<std::string> dependentModuleNames;
    bool result = dataMgr->GetAllDependentModuleNames(BUNDLE_NAME, MODULE_NAME_ENTRY_C, dependentModuleNames);
    EXPECT_TRUE(result);
    EXPECT_EQ(dependentModuleNames.size(), ZERO);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleDependenciesTest
 * Function: GetAllDependentModuleName
 * @tc.name: test dependencies is not empty
 * @tc.require: SR000H0DUR
 * @tc.desc: 1. system running normally
 *           2. install two haps
 *           3. get the dependentModuleName
 */
HWTEST_F(BmsBundleDependenciesTest, BmsBundleDependenciesTest_0300, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_C);
    EXPECT_EQ(installResult, ERR_OK);
    installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_A);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    std::vector<std::string> dependentModuleNames;
    bool result = dataMgr->GetAllDependentModuleNames(BUNDLE_NAME, MODULE_NAME_ENTRY_A, dependentModuleNames);
    EXPECT_TRUE(result);
    EXPECT_EQ(dependentModuleNames.size(), ONE);
    if (dependentModuleNames.size() > 0) {
        EXPECT_EQ(dependentModuleNames[0], MODULE_NAME_ENTRY_C);
    }
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleDependenciesTest
 * Function: GetAllDependentModuleName
 * @tc.name: test dependencies is not empty
 * @tc.require: SR000H0DUR
 * @tc.desc: 1. system running normally
 *           2. install three haps
 *           3. get the dependentModuleName
 */
HWTEST_F(BmsBundleDependenciesTest, BmsBundleDependenciesTest_0400, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_C);
    EXPECT_EQ(installResult, ERR_OK);
    installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_A);
    EXPECT_EQ(installResult, ERR_OK);
    installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_B);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    std::vector<std::string> dependentModuleNames;
    bool result = dataMgr->GetAllDependentModuleNames(BUNDLE_NAME, MODULE_NAME_ENTRY_B, dependentModuleNames);
    EXPECT_TRUE(result);
    EXPECT_EQ(dependentModuleNames.size(), ONE);
    if (dependentModuleNames.size() > 0) {
        EXPECT_EQ(dependentModuleNames[0], MODULE_NAME_ENTRY_C);
    }
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleDependenciesTest
 * Function: GetAllDependentModuleName
 * @tc.name: test dependencies is not empty
 * @tc.require: SR000H0DUR
 * @tc.desc: 1. system running normally
 *           2. install four haps
 *           3. get the dependentModuleName
 */
HWTEST_F(BmsBundleDependenciesTest, BmsBundleDependenciesTest_0500, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_C);
    EXPECT_EQ(installResult, ERR_OK);
    installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_A);
    EXPECT_EQ(installResult, ERR_OK);
    installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY_B);
    EXPECT_EQ(installResult, ERR_OK);
    installResult = InstallBundle(HAP_FILE_PATH + HAP_NAME_ENTRY);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    std::vector<std::string> dependentModuleNames;
    bool result = dataMgr->GetAllDependentModuleNames(BUNDLE_NAME, MODULE_NAME_ENTRY, dependentModuleNames);
    EXPECT_TRUE(result);
    EXPECT_EQ(dependentModuleNames.size(), THREE);
    if (dependentModuleNames.size() >= THREE) {
        EXPECT_EQ(dependentModuleNames[0], MODULE_NAME_ENTRY_A);
        EXPECT_EQ(dependentModuleNames[1], MODULE_NAME_ENTRY_B);
        EXPECT_EQ(dependentModuleNames[2], MODULE_NAME_ENTRY_C);
    }
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}
} // OHOS