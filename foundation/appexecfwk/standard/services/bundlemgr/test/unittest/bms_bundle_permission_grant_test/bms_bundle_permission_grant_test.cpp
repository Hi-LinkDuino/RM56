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
const std::string BUNDLE_NAME = "com.example.bmsgrantpermission";
const std::string HAP_FILE_PATH = "/data/test/resource/bms/permission_bundle/";
const int32_t USERID = 100;
const int32_t WAIT_TIME = 5; // init mocked bms
}  // namespace

class BmsBundlePermissionGrantTest : public testing::Test {
public:
    BmsBundlePermissionGrantTest();
    ~BmsBundlePermissionGrantTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    ErrCode InstallBundle(const std::string &bundlePath) const;
    ErrCode UnInstallBundle(const std::string &bundleName) const;
    const std::shared_ptr<BundleDataMgr> GetBundleDataMgr() const;
    void StartInstalldService() const;
    void StartBundleService();
private:
    std::shared_ptr<InstalldService> installdService_ = std::make_shared<InstalldService>();
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
};

BmsBundlePermissionGrantTest::BmsBundlePermissionGrantTest()
{}

BmsBundlePermissionGrantTest::~BmsBundlePermissionGrantTest()
{}

void BmsBundlePermissionGrantTest::SetUpTestCase()
{}

void BmsBundlePermissionGrantTest::TearDownTestCase()
{}

void BmsBundlePermissionGrantTest::SetUp()
{
    StartInstalldService();
    StartBundleService();
}

void BmsBundlePermissionGrantTest::TearDown()
{}

ErrCode BmsBundlePermissionGrantTest::InstallBundle(const std::string &bundlePath) const
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

ErrCode BmsBundlePermissionGrantTest::UnInstallBundle(const std::string &bundleName) const
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

void BmsBundlePermissionGrantTest::StartInstalldService() const
{
    if (!installdService_->IsServiceReady()) {
        installdService_->Start();
    }
}

void BmsBundlePermissionGrantTest::StartBundleService()
{
    if (!bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStart();
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
    }
}

const std::shared_ptr<BundleDataMgr> BmsBundlePermissionGrantTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

/**
 * @tc.number: BmsBundlePermissionGrantTest
 * Function: Install
 * @tc.name: test install a hap with normal permission and normal apl
 * @tc.require: SR000GP3LF
 * @tc.desc: 1. system running normally
 *           2. install a hap with normal permission and normal apl
 *           3. install successed
 */
HWTEST_F(BmsBundlePermissionGrantTest, BmsBundlePermissionGrantTest_0100, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + "bmsGrantPermission1.hap");
    EXPECT_EQ(installResult, ERR_OK);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: Install
 * @tc.name: test install a hap with system_bacic/system_core permission and normal apl
 * @tc.require: AR000GP3NU
 * @tc.desc: 1. system running normally
 *           2. install a hap, the request permission exists in acls
 *           3. install successed
 */
HWTEST_F(BmsBundlePermissionGrantTest, BmsBundlePermissionGrantTest_0300, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + "bmsGrantPermission3.hap");
    EXPECT_EQ(installResult, ERR_OK);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: Install
 * @tc.name: test install a hap with system_bacic/normal permission and system_basic apl
 * @tc.require: AR000GP3NU
 * @tc.desc: 1. system running normally
 *           2. install a hap
 *           3. install successed
 */
HWTEST_F(BmsBundlePermissionGrantTest, BmsBundlePermissionGrantTest_0400, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + "bmsGrantPermission4.hap");
    EXPECT_EQ(installResult, ERR_OK);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: Install
 * @tc.name: test install a hap with system_core permission and system_basic apl
 * @tc.require: AR000GP3NU
 * @tc.desc: 1. system running normally
 *           2. install a hap, the request permission exists in acls
 *           3. install successed
 */
HWTEST_F(BmsBundlePermissionGrantTest, BmsBundlePermissionGrantTest_0600, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + "bmsGrantPermission6.hap");
    EXPECT_EQ(installResult, ERR_OK);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}

/**
 * @tc.number: BmsBundleAccessTokenIdTest
 * Function: Install
 * @tc.name: test install a hap with normal/system_basic/system_core permission and system_core apl
 * @tc.require: AR000GP3NU
 * @tc.desc: 1. system running normally
 *           2. install a hap
 *           3. install successed
 */
HWTEST_F(BmsBundlePermissionGrantTest, BmsBundlePermissionGrantTest_0700, Function | SmallTest | Level0)
{
    ErrCode installResult = InstallBundle(HAP_FILE_PATH + "bmsGrantPermission7.hap");
    EXPECT_EQ(installResult, ERR_OK);
    ErrCode unInstallResult = UnInstallBundle(BUNDLE_NAME);
    EXPECT_EQ(unInstallResult, ERR_OK);
}
} // OHOS