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

#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "installd_client.h"
#include "bundle_clone_mgr.h"
#include "bundle_mgr_service.h"
#include "ability_manager_interface.h"
#include "system_ability_helper.h"
#include "ohos/aafwk/content/want.h"
#include "system_ability_definition.h"
#include "bundle_data_mgr.h"

#include "directory_ex.h"
#include "appexecfwk_errors.h"
#include "bundle_info.h"
#include "bundle_installer_host.h"
#include "bundle_data_storage_database.h"
#include "installd/installd_service.h"
#include "mock_status_receiver.h"
#include "mock_clean_cache.h"
#include "mock_bundle_status.h"
#include "install_param.h"
#include "system_bundle_installer.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS;

namespace {
    const std::string TEST_BUNDLE = "com.example.third1";
    const std::string DEMO_BUNDLE = "com.example.third2";
    const std::string ILLEGAL_BUNDLE = "com.example.third3";
    const std::string OBTAIN_BUNDLE = "com.ohos.amsst.appAbilityVisibleServiceB";
    const std::string FEATURE_MODULE_PACKAGE = "com.example.third2.entry";
    const std::string ENTRY_MODULE_PACKAGE = "com.example.third1.entry";
    const std::string ERROR_BUNDLE = "ERROR";
    const std::string NULL_STR = "";
    const std::string BUNDLE_DATA_DIR = "/data/app/el2/100/base/";
    const std::string BUNDLE_DATA_DIR1 = "/data/app/el2/100/base/com.example.third*";
    const std::string BUNDLE_CODE_DIR = "/data/app/el1/bundle/public/bmsThirdBundleTest*";
    const std::string TEST_BUNDLE_DIR = "/data/bmsThirdBundleTest1.hap";
    const std::string DEMO_BUNDLE_DIR = "/data/bmsThirdBundleTest2.hap";
    const std::string ILLEGAL_BUNDLE_DIR = "/data/bmsThirdBundleTest3.hap";
    const std::string MULTIPLE_HAP_DIR = "/data/bmsThirdBundleTest4.hap";
    const std::string OBTAIN_BUNDLE_DIR = "/data/bmsDemoBundle.hap";
    const std::string TEST_ABILITY_NAME = "com.example.third1.MainAbility";
    const std::string DARA_ABILITY = "dataability:///com.ohos.amsst.appAbilityVisibleServiceB";
    const std::string ABILITY_VISIBLE_TEST_DATA = ".AmsAbilityVisibleTestData";
}

namespace OHOS {
namespace AppExecFwk {
class BmsBundleCloneTest : public testing::Test {
public:
    using Want = OHOS::AAFwk::Want;
    BmsBundleCloneTest();
    ~BmsBundleCloneTest();
    std::shared_ptr<BundleDataMgr> GetBundleDataMgr() const;
    std::shared_ptr<BundleCloneMgr> GetBundleCloneMgr() const;
    void CheckFileExist(const std::string &bundleName, const std::string &dir) const;
    void CheckDataDirFileNonExist(const std::string &bundleName, const std::string &dir) const;
    ErrCode UnInstallBundle(const std::string &bundleName) const;
    ErrCode UnInstallBundle(const std::string &bundleName, const std::string &modulePackage) const;
    ErrCode InstallBundle(const std::string &filePath) const;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void StopBundleService();
    void StopInstalldService() const;
private:
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
    std::shared_ptr<InstalldService> installdService_ = std::make_shared<InstalldService>();
};

BmsBundleCloneTest::BmsBundleCloneTest()
{}

BmsBundleCloneTest::~BmsBundleCloneTest()
{}

void BmsBundleCloneTest::SetUpTestCase()
{}

void BmsBundleCloneTest::TearDownTestCase()
{}

void BmsBundleCloneTest::SetUp()
{
    bundleMgrService_->OnStart();
    installdService_->Start();
}

void BmsBundleCloneTest::TearDown()
{
    StopInstalldService();
    StopBundleService();

    OHOS::ForceRemoveDirectory(BUNDLE_DATA_DIR1);
    OHOS::ForceRemoveDirectory(BUNDLE_CODE_DIR);
}

std::shared_ptr<BundleDataMgr> BmsBundleCloneTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

std::shared_ptr<BundleCloneMgr> BmsBundleCloneTest::GetBundleCloneMgr() const
{
    return bundleMgrService_->GetCloneMgr();
}

void BmsBundleCloneTest::StopInstalldService() const
{
    installdService_->Stop();
    InstalldClient::GetInstance()->ResetInstalldProxy();
}

void BmsBundleCloneTest::StopBundleService()
{
    if (bundleMgrService_ == nullptr) {
        return;
    }
    auto dataMgr = bundleMgrService_->GetDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    dataMgr->UpdateBundleInstallState(TEST_BUNDLE, InstallState::UNINSTALL_START);
    dataMgr->UpdateBundleInstallState(TEST_BUNDLE, InstallState::UNINSTALL_SUCCESS);
    bundleMgrService_->OnStop();
    bundleMgrService_.reset();
}

void BmsBundleCloneTest::CheckFileExist(const std::string &bundleName, const std::string &dir) const
{
    std::string dataDir = dir + bundleName;
    int bundleDataExist = access(dataDir.c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir does not exists: " << dataDir;
}

void BmsBundleCloneTest::CheckDataDirFileNonExist(const std::string &bundleName, const std::string &dir) const
{
    std::string dataDir = dir + bundleName;
    int bundleDataExist = access(dataDir.c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0);
}

ErrCode BmsBundleCloneTest::UnInstallBundle(const std::string &bundleName) const
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
    bool result = installer->Uninstall(bundleName, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}


ErrCode BmsBundleCloneTest::UnInstallBundle(const std::string &bundleName, const std::string &modulePackage) const
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
    bool result = installer->Uninstall(bundleName, modulePackage, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

ErrCode BmsBundleCloneTest::InstallBundle(const std::string &filePath) const
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
    installParam.installFlag = InstallFlag::NORMAL;
    bool result = installer->Install(filePath, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

/**
 * @tc.number: BundleClone_0100.
 * @tc.name: test the right bundle name can be cloned.
 * @tc.desc: 1.the bundle name is right.
 *           2.the bundle can be cloned success and can get bundle info success.
 */
HWTEST_F(BmsBundleCloneTest, BundleClone_0100, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    EXPECT_TRUE(result);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: BundleClone_0200.
 * @tc.name: test the can be cloned more than two bundle at the same time.
 * @tc.desc: 1.the bundle name is right.
 *           2.the bundle can be cloned success and can get bundle info success.
 */
HWTEST_F(BmsBundleCloneTest, BundleClone_0200, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    InstallBundle(DEMO_BUNDLE_DIR);
    std::string testCloneName;
    std::string demoCloneName;
    auto testResult = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    auto demoResult = GetBundleCloneMgr()->BundleClone(DEMO_BUNDLE);
    EXPECT_TRUE(testResult);
    EXPECT_TRUE(demoResult);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, testCloneName);
    GetBundleDataMgr()->GetClonedBundleName(DEMO_BUNDLE, demoCloneName);
    CheckFileExist(testCloneName, BUNDLE_DATA_DIR);
    CheckFileExist(demoCloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
    UnInstallBundle(DEMO_BUNDLE);
}

/**
 * @tc.number: BundleClone_0300.
 * @tc.name: test the error bundle name can't be cloned.
 * @tc.desc: 1.the bundle name is error.
 *           2.the bundle can't be cloned success.
 */
HWTEST_F(BmsBundleCloneTest, BundleClone_0300, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(ERROR_BUNDLE);
    EXPECT_FALSE(result);
    GetBundleDataMgr()->GetClonedBundleName(ERROR_BUNDLE, cloneName);
    std::cout<<cloneName<<std::endl;
    EXPECT_EQ(cloneName, NULL_STR);
    if (cloneName.empty()) {
        cloneName = cloneName + ERROR_BUNDLE;
    }
    CheckDataDirFileNonExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: BundleClone_0400.
 * @tc.name: test the error bundle name can't be cloned.
 * @tc.desc: 1.the bundle name is error.
 *           2.the bundle can't be cloned success.
 */
HWTEST_F(BmsBundleCloneTest, BundleClone_0400, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    auto result = GetBundleCloneMgr()->BundleClone(NULL_STR);
    EXPECT_FALSE(result);
    std::string cloneName;
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName);
    EXPECT_EQ(cloneName, NULL_STR);
    if (cloneName.empty()) {
        cloneName = cloneName + ERROR_BUNDLE;
    }
    CheckDataDirFileNonExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: BundleClone_0500.
 * @tc.name: test The bundle name is not in the trustlist.
 * @tc.desc: 1.the bundle name is not in the trustlist.
 *           2.the bundle can't be cloned success.
 */
HWTEST_F(BmsBundleCloneTest, BundleClone_0500, Function | SmallTest | Level0)
{
    InstallBundle(ILLEGAL_BUNDLE_DIR);
    auto result = GetBundleCloneMgr()->BundleClone(ILLEGAL_BUNDLE);
    EXPECT_FALSE(result);
    std::string cloneName;
    GetBundleDataMgr()->GetClonedBundleName(ILLEGAL_BUNDLE, cloneName);
    EXPECT_EQ(cloneName, NULL_STR);
    if (cloneName.empty()) {
        cloneName = cloneName + ERROR_BUNDLE;
    }
    CheckDataDirFileNonExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(ILLEGAL_BUNDLE);
}

/**
 * @tc.number: BundleClone_0600.
 * @tc.name: test cloned the same bundle twice.
 * @tc.desc: 1.the bundle cloned twice.
 *           2.the bundle can't be cloned success.
 */
HWTEST_F(BmsBundleCloneTest, BundleClone_0600, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto firstResult = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    auto secondResult = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    EXPECT_TRUE(firstResult);
    EXPECT_FALSE(secondResult);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: RemoveClonedBundle_0100.
 * @tc.name: test the cloned bundle can be removed.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the cloned bundle can be removed successfully.
 */
HWTEST_F(BmsBundleCloneTest, RemoveClonedBundle_0100, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    EXPECT_TRUE(result);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    auto ret = GetBundleCloneMgr()->RemoveClonedBundle(TEST_BUNDLE, cloneName);
    EXPECT_TRUE(ret);
    std::string cloneName1;
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName1);
    CheckDataDirFileNonExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: RemoveClonedBundle_0200.
 * @tc.name: test the cloned bundle can be removed when the ontology application uninstall.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the cloned bundle can be removed when the successfully.
 */
HWTEST_F(BmsBundleCloneTest, RemoveClonedBundle_0200, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    EXPECT_TRUE(result);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
    std::string cloneName1;
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName1);
    EXPECT_EQ(cloneName1, NULL_STR);
    if (cloneName1.empty()) {
        cloneName1 = cloneName1 + ERROR_BUNDLE;
    }
    CheckDataDirFileNonExist(cloneName1, BUNDLE_DATA_DIR);
}

/**
 * @tc.number: RemoveClonedBundle_0300.
 * @tc.name: test the cloned bundle can be removed right when cloned twice bundle.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the cloned bundle can be removed when the successfully.
 */
HWTEST_F(BmsBundleCloneTest, RemoveClonedBundle_0300, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    InstallBundle(DEMO_BUNDLE_DIR);
    std::string testCloneName;
    std::string demoCloneName;
    auto testResult = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    auto demoResult = GetBundleCloneMgr()->BundleClone(DEMO_BUNDLE);
    EXPECT_TRUE(testResult);
    EXPECT_TRUE(demoResult);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, testCloneName);
    GetBundleDataMgr()->GetClonedBundleName(DEMO_BUNDLE, demoCloneName);
    CheckFileExist(testCloneName, BUNDLE_DATA_DIR);
    CheckFileExist(demoCloneName, BUNDLE_DATA_DIR);
    auto ret = GetBundleCloneMgr()->RemoveClonedBundle(TEST_BUNDLE, testCloneName);
    EXPECT_TRUE(ret);
    std::string testClonedName;
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, testClonedName);
    EXPECT_EQ(testClonedName, NULL_STR);
    if (testClonedName.empty()) {
        testClonedName = testClonedName + ERROR_BUNDLE;
    }
    CheckDataDirFileNonExist(testClonedName, BUNDLE_DATA_DIR);
    std::string demoClonedName;
    GetBundleDataMgr()->GetClonedBundleName(DEMO_BUNDLE, demoClonedName);
    CheckFileExist(demoClonedName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
    UnInstallBundle(DEMO_BUNDLE);
}

/**
 * @tc.number: RemoveClonedBundle_0400.
 * @tc.name: test the cloned bundle can't be removed when unInstall a feature hap.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the cloned bundle can't be removed successfully.
 */
HWTEST_F(BmsBundleCloneTest, RemoveClonedBundle_0400, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    InstallBundle(MULTIPLE_HAP_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    EXPECT_TRUE(result);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE, FEATURE_MODULE_PACKAGE);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: RemoveClonedBundle_0500.
 * @tc.name: test the cloned bundle can be removed when unInstall only one hap.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the cloned bundle can be removed successfully.
 */
HWTEST_F(BmsBundleCloneTest, RemoveClonedBundle_0500, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    InstallBundle(MULTIPLE_HAP_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    EXPECT_TRUE(result);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE, FEATURE_MODULE_PACKAGE);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE, ENTRY_MODULE_PACKAGE);
    CheckDataDirFileNonExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: RemoveClonedBundle_0600.
 * @tc.name: test the cloned bundle can't be removed when the bundle name is error.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the cloned bundle can't be removed successfully.
 */
HWTEST_F(BmsBundleCloneTest, RemoveClonedBundle_0600, Function | SmallTest | Level0)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    EXPECT_TRUE(result);
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    auto ret = GetBundleCloneMgr()->RemoveClonedBundle(TEST_BUNDLE, ERROR_BUNDLE);
    EXPECT_FALSE(ret);
    std::string cloneName1;
    GetBundleDataMgr()->GetClonedBundleName(TEST_BUNDLE, cloneName1);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: QueryAbilityInfosByUri_0100.
 * @tc.name: test can get all the ability infos by uri.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the test can get ability infos by uri.
 */
HWTEST_F(BmsBundleCloneTest, QueryAbilityInfosByUri_0100, Function | SmallTest | Level0)
{
    InstallBundle(OBTAIN_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(OBTAIN_BUNDLE);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    AbilityInfo abilityInfo;
    auto type = abilityInfo.type;
    EXPECT_TRUE(result);
    std::vector<AbilityInfo> abilityInfos;
    auto ret = GetBundleDataMgr()->QueryAbilityInfosByUri(DARA_ABILITY + ABILITY_VISIBLE_TEST_DATA, abilityInfos);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(abilityInfos.size() > 0);
    for (auto &info : abilityInfos) {
        EXPECT_TRUE(info.name.size() > 0);
        EXPECT_NE(info.type, type);
    }
    UnInstallBundle(OBTAIN_BUNDLE);
}

/**
 * @tc.number: QueryAbilityInfosByUri_0200.
 * @tc.name: test can't get all the ability infos by uri when uri is error.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the test can't get ability infos by error uri.
 */
HWTEST_F(BmsBundleCloneTest, QueryAbilityInfosByUri_0200, Function | SmallTest | Level1)
{
    InstallBundle(OBTAIN_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(OBTAIN_BUNDLE);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    EXPECT_TRUE(result);
    std::vector<AbilityInfo> abilityInfos;
    auto ret = GetBundleDataMgr()->QueryAbilityInfosByUri(ERROR_BUNDLE, abilityInfos);
    EXPECT_FALSE(ret);
    UnInstallBundle(OBTAIN_BUNDLE);
}

/**
 * @tc.number: QueryAbilityInfosByUri_0300.
 * @tc.name: test can't get all the ability infos by uri when uri is null.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the test can't get ability infos by null uri.
 */
HWTEST_F(BmsBundleCloneTest, QueryAbilityInfosByUri_0300, Function | SmallTest | Level1)
{
    InstallBundle(OBTAIN_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(OBTAIN_BUNDLE);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    EXPECT_TRUE(result);
    std::vector<AbilityInfo> abilityInfos;
    auto ret = GetBundleDataMgr()->QueryAbilityInfosByUri(ERROR_BUNDLE, abilityInfos);
    EXPECT_FALSE(ret);
    UnInstallBundle(OBTAIN_BUNDLE);
}

/**
 * @tc.number: QueryAbilityInfosByUri_0400.
 * @tc.name: test can't get all the ability infos by uri when the bundle isn't install already.
 * @tc.desc: 1. the bundle isn't already cloned.
 *           2. the test can't get ability infos .
 */
HWTEST_F(BmsBundleCloneTest, QueryAbilityInfosByUri_0400, Function | SmallTest | Level1)
{
    std::vector<AbilityInfo> abilityInfos;
    auto ret = GetBundleDataMgr()->QueryAbilityInfosByUri(OBTAIN_BUNDLE, abilityInfos);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: QueryAbilityInfoForClone_0100.
 * @tc.name: test can get all the ability infos by cloned bundle.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the test can get ability infos by cloned bundle.
 */
HWTEST_F(BmsBundleCloneTest, QueryAbilityInfoForClone_0100, Function | SmallTest | Level1)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    EXPECT_TRUE(result);
    std::vector<AbilityInfo> abilityInfos;
    AbilityInfo abilityInfo;
    auto type = abilityInfo.type;
    Want want;
    want.SetElementName(TEST_BUNDLE, TEST_ABILITY_NAME);
    auto ret = GetBundleDataMgr()->QueryAbilityInfosForClone(want, abilityInfos);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(abilityInfos.size() > 0);
    for (auto &info : abilityInfos) {
        EXPECT_TRUE(info.name.size() > 0);
        EXPECT_NE(info.type, type);
    }
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: QueryAbilityInfoForClone_0200.
 * @tc.name: test can't get all the ability infos by cloned bundle when the want is error.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the test can't get ability infos when the want is error.
 */
HWTEST_F(BmsBundleCloneTest, QueryAbilityInfoForClone_0200, Function | SmallTest | Level1)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    EXPECT_TRUE(result);
    std::vector<AbilityInfo> abilityInfos;
    Want want;
    want.SetElementName(DEMO_BUNDLE, TEST_ABILITY_NAME);
    auto ret = GetBundleDataMgr()->QueryAbilityInfosForClone(want, abilityInfos);
    EXPECT_FALSE(ret);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: QueryAbilityInfoForClone_0300.
 * @tc.name: test can't get all the ability infos by cloned bundle when the want is null.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the test can't get ability infos when the want is null.
 */
HWTEST_F(BmsBundleCloneTest, QueryAbilityInfoForClone_0300, Function | SmallTest | Level1)
{
    InstallBundle(TEST_BUNDLE_DIR);
    std::string cloneName;
    auto result = GetBundleCloneMgr()->BundleClone(TEST_BUNDLE);
    CheckFileExist(cloneName, BUNDLE_DATA_DIR);
    EXPECT_TRUE(result);
    std::vector<AbilityInfo> abilityInfos;
    Want want;
    auto ret = GetBundleDataMgr()->QueryAbilityInfosForClone(want, abilityInfos);
    EXPECT_FALSE(ret);
    UnInstallBundle(TEST_BUNDLE);
}

/**
 * @tc.number: QueryAbilityInfoForClone_0400.
 * @tc.name: test can't get all the ability infos when the bundle isn't install already.
 * @tc.desc: 1. the bundle isn't install .
 *           2. the test can't get ability infos when the bundle isn't install already.
 */
HWTEST_F(BmsBundleCloneTest, QueryAbilityInfoForClone_0400, Function | SmallTest | Level1)
{
    std::vector<AbilityInfo> abilityInfos;
    Want want;
    auto ret = GetBundleDataMgr()->QueryAbilityInfosForClone(want, abilityInfos);
    EXPECT_FALSE(ret);
}
}  // namespace AppExecFwk
}  // namespace OHOS