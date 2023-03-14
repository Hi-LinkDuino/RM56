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
#include "permission_define.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using namespace OHOS::Security;

namespace OHOS {
namespace {
const std::string BUNDLE_NAME_SYSTEM_RESOURCE = "ohos.global.systemres";
const std::string BUNDLE_NAME = "com.example.bmsaccesstoken1";
const std::string WRONG_BUNDLE_NAME = "wrong_bundle_name.ha";
const std::string HAP_FILE_PATH1 = "/data/test/resource/bms/accesstoken_bundle/bmsAccessTokentest1.hap";
const std::string HAP_FILE_PATH2 = "/data/test/resource/bms/accesstoken_bundle/bmsAccessTokentest2.hap";
const std::string HAP_FILE_PATH3 = "/data/test/resource/bms/accesstoken_bundle/bmsAccessTokentest3.hap";
const std::string WRONG_HAP_FILE_PATH = "/data/test/resource/bms/accesstoken_bundle/wrong_bundle_name.ha";
const int32_t USERID = 100;
const uint32_t ZERO = 0;
const uint32_t INVALID_ACCESSTOKENID = 0;
const int32_t WAIT_TIME = 5; // init mocked bms
}  // namespace

class BmsBundleAccessTokenIdTest : public testing::Test {
public:
    BmsBundleAccessTokenIdTest();
    ~BmsBundleAccessTokenIdTest();
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

BmsBundleAccessTokenIdTest::BmsBundleAccessTokenIdTest()
{}

BmsBundleAccessTokenIdTest::~BmsBundleAccessTokenIdTest()
{}

void BmsBundleAccessTokenIdTest::SetUpTestCase()
{}

void BmsBundleAccessTokenIdTest::TearDownTestCase()
{}

void BmsBundleAccessTokenIdTest::SetUp()
{
    StartBundleService();
    StartInstalldService();
}

void BmsBundleAccessTokenIdTest::TearDown()
{}

ErrCode BmsBundleAccessTokenIdTest::InstallBundle(const std::string &bundlePath) const
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

ErrCode BmsBundleAccessTokenIdTest::UpdateBundle(const std::string &bundlePath) const
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

ErrCode BmsBundleAccessTokenIdTest::UnInstallBundle(const std::string &bundleName) const
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

void BmsBundleAccessTokenIdTest::StartInstalldService() const
{
    if (!installdService_->IsServiceReady()) {
        installdService_->Start();
    }
}

void BmsBundleAccessTokenIdTest::StartBundleService()
{
    if (!bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStart();
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
    }
}

const std::shared_ptr<BundleDataMgr> BmsBundleAccessTokenIdTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetApplicationInfo
 * @tc.name: test can create accessTokenId
 * @tc.desc: 1. system running normally
 *           2. test bundle accessTokenId
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0100, Function | SmallTest | Level0)
{
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    ApplicationInfo info;
    bool result = dataMgr->GetApplicationInfo(BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info);
    EXPECT_FALSE(result);
    EXPECT_EQ(info.accessTokenId, INVALID_ACCESSTOKENID);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetApplicationInfo
 * @tc.name: test can create accessTokenId
 * @tc.require: SR000GKG1R
 * @tc.desc: 1. system running normally
 *           2. install a hap
 *           3. test bundle accessTokenId
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0200, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH1);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    ApplicationInfo info;
    bool result = dataMgr->GetApplicationInfo(BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info);
    EXPECT_TRUE(result);
    EXPECT_NE(info.accessTokenId, INVALID_ACCESSTOKENID);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetApplicationInfo
 * @tc.name: test can create accessTokenId
 * @tc.require: AR000GKH9O
 * @tc.desc: 1. system running normally
 *           2. install a hap, install failed
 *           3. test bundle accessTokenId
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0300, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(WRONG_HAP_FILE_PATH);
    EXPECT_NE(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    ApplicationInfo info;
    bool result = dataMgr->GetApplicationInfo(BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info);
    EXPECT_FALSE(result);
    EXPECT_EQ(info.accessTokenId, INVALID_ACCESSTOKENID);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetApplicationInfo
 * @tc.name: test can create accessTokenId
 * @tc.require: AR000GKH9O
 * @tc.desc: 1. system running normally
 *           2. install a hap, install failed, install again
 *           3. test the failed
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0400, Function | SmallTest | Level0)
{
    ErrCode installResult1 = InstallBundle(WRONG_HAP_FILE_PATH);
    EXPECT_NE(installResult1, ERR_OK);
    ErrCode installResult2 = InstallBundle(WRONG_HAP_FILE_PATH);
    EXPECT_NE(installResult2, ERR_OK);
    EXPECT_EQ(installResult1, installResult2);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetApplicationInfo
 * @tc.name: test can create accessTokenId
 * @tc.require: AR000GKH9O
 * @tc.desc: 1. system running normally
 *           2. install a hap, update it
 *           3. test the accessTokenId
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0500, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH1);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    ApplicationInfo info;
    bool result = dataMgr->GetApplicationInfo(BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info);
    EXPECT_TRUE(result);
    ErrCode updateResult = UpdateBundle(HAP_FILE_PATH2);
    EXPECT_EQ(updateResult, ERR_OK);
    ApplicationInfo info2;
    result = dataMgr->GetApplicationInfo(BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info2);
    EXPECT_TRUE(result);
    EXPECT_EQ(info.accessTokenId, info2.accessTokenId);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetApplicationInfo
 * @tc.name: test can create accessTokenId
 * @tc.require: AR000GKH9O
 * @tc.desc: 1. system running normally
 *           2. install multi-hap
 *           3. test the accessTokenId
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0600, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH1);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    ApplicationInfo info;
    bool result = dataMgr->GetApplicationInfo(BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info);
    EXPECT_TRUE(result);
    installResult = InstallBundle(HAP_FILE_PATH3);
    EXPECT_EQ(installResult, ERR_OK);
    ApplicationInfo info2;
    result = dataMgr->GetApplicationInfo(BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, info2);
    EXPECT_TRUE(result);
    EXPECT_EQ(info.accessTokenId, info2.accessTokenId);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetBundleInfo
 * @tc.name: test can create accessTokenId and system permissions
 * @tc.require: SR000GM6NK
 * @tc.desc: 1. system running normally
 *           2. install a hap
 *           3. check defPermission
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0700, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH1);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool result = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION,
        bundleInfo, USERID);
    EXPECT_TRUE(result);
    EXPECT_NE(bundleInfo.defPermissions.size(), ZERO);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetBundleInfo
 * @tc.name: test can create accessTokenId and system permissions
 * @tc.require: AR000GM6NQ
 * @tc.desc: 1. system running normally
 *           2. check ohos.global.systemres accessTokenId and defpermissions
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0800, Function | SmallTest | Level0)
{
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    ApplicationInfo info;
    bool result = dataMgr->GetApplicationInfo(BUNDLE_NAME_SYSTEM_RESOURCE, ApplicationFlag::GET_BASIC_APPLICATION_INFO,
        USERID, info);
    EXPECT_TRUE(result);
    EXPECT_NE(info.accessTokenId, INVALID_ACCESSTOKENID);
    BundleInfo bundleInfo;
    result = dataMgr->GetBundleInfo(BUNDLE_NAME_SYSTEM_RESOURCE, BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION,
        bundleInfo, USERID);
    EXPECT_NE(bundleInfo.defPermissions.size(), ZERO);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetBundleInfo
 * @tc.name: test can create accessTokenId and system permissions
 * @tc.require: SR000GM5R6
 * @tc.desc: 1. system running normally
 *           2. install a hap
 *           3. check reqPermissionStates
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_0900, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH1);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool result = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION,
        bundleInfo, USERID);
    EXPECT_TRUE(result);
    EXPECT_NE(bundleInfo.reqPermissionStates.size(), ZERO);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetBundleInfo
 * @tc.name: test can create accessTokenId
 * @tc.require: AR000GM5R7
 * @tc.desc: 1. system running normally
 *           2. install a hap, update it
 *           3. check reqPermissionStates
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_1000, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH1);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);

    ErrCode updateResult = UpdateBundle(HAP_FILE_PATH2);
    EXPECT_EQ(updateResult, ERR_OK);
    BundleInfo bundleInfo;
    bool result = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION,
        bundleInfo, USERID);
    EXPECT_TRUE(result);
    EXPECT_NE(bundleInfo.reqPermissionStates.size(), ZERO);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: GetBundleInfo
 * @tc.name: test can create accessTokenId
 * @tc.require: AR000GM5R7
 * @tc.desc: 1. system running normally
 *           2. install multi-hap
 *           3. check reqPermissionStates
 */
HWTEST_F(BmsBundleAccessTokenIdTest, BmsBundleAccessTokenId_1100, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH1);
    EXPECT_EQ(installResult, ERR_OK);
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);

    installResult = InstallBundle(HAP_FILE_PATH3);
    EXPECT_EQ(installResult, ERR_OK);
    BundleInfo bundleInfo;
    bool result = dataMgr->GetBundleInfo(BUNDLE_NAME, BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION,
        bundleInfo, USERID);
    EXPECT_TRUE(result);
    EXPECT_NE(bundleInfo.reqPermissionStates.size(), ZERO);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}
} // OHOS