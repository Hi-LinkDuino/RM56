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

#include <gtest/gtest.h>

#include "bundle_data_storage_database.h"
#include "bundle_info.h"
#include "bundle_installer_host.h"
#include "bundle_mgr_service.h"
#include "directory_ex.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "mock_status_receiver.h"
#include "system_bundle_installer.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS::AppExecFwk;
using namespace OHOS::DistributedKv;
using namespace OHOS;
using OHOS::DelayedSingleton;

namespace {
const std::string BUNDLE_NAME = "com.example.l3jsdemo";
const std::string BUNDLE_NAME_INVALID = "";
const std::string OTHER_BUNDLE_NAME = "com.example.ohosproject.hmservice";
const std::string UNINSTALLED_BUNDLE_NAME = "com.example.l3jsdemo1";
const std::string RESOURCE_ROOT_PATH = "/data/test/resource/bms/install_bundle/";
const std::string RIGHT_BUNDLE_FIRST = "first_right.hap";
const std::string SANDBOX_TEST = "sandboxTest.hap";
const std::string ROOT_DIR = "/data/app";
const std::string BUNDLE_DATA_DIR1 = "/data/app/el1/100/base/";
const std::string BUNDLE_DATA_DIR2 = "/data/app/el1/100/database/";
const std::string BUNDLE_DATA_DIR3 = "/data/app/el2/100/base/";
const std::string BUNDLE_DATA_DIR4 = "/data/app/el2/100/database/";
const std::string BUNDLE_CODE_DIR = "/data/app/el1/bundle/public/com.example.l3jsdemo";
int32_t INVALID_DLP_TYPE = 0;
int32_t DLP_TYPE_1 = 1;
int32_t DLP_TYPE_2 = 2;
int32_t DLP_TYPE_3 = 3;
int32_t INVALID_APP_INDEX = 0;
int32_t APP_INDEX_1 = 1;
int32_t APP_INDEX_2 = 2;
const int32_t USERID = 100;
const int32_t INVALID_USERID = 300;
const int32_t ROOT_UID = 0;
const int32_t WAIT_TIME = 5; // init mocked bms
} // namespace

class BmsSandboxAppTest : public testing::Test {
public:
    BmsSandboxAppTest();
    ~BmsSandboxAppTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    ErrCode InstallSandboxApp(const std::string &bundleName, int32_t dplType, int32_t userId, int32_t &appIndex) const;
    ErrCode UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId) const;
    ErrCode GetSandboxAppBundleInfo(const std::string &bundleName, const int32_t &appIndex, const int32_t &userId,
        BundleInfo &info);
    ErrCode InstallBundles(const std::vector<std::string> &filePaths, bool &&flag) const;
    ErrCode UninstallBundle(const std::string &bundleName) const;
    void CheckPathAreExisted(const std::string &bundleName, int32_t appIndex);
    void CheckPathAreNonExisted(const std::string &bundleName, int32_t appIndex);

private:
    bool GetDataMgr();
    bool GetSandboxDataMgr();

    std::shared_ptr<InstalldService> installdService_ = std::make_shared<InstalldService>();
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
    std::shared_ptr<BundleDataMgr> dataMgr_ = nullptr;
    std::shared_ptr<BundleSandboxDataMgr> sandboxDataMgr_ = nullptr;
};

BmsSandboxAppTest::BmsSandboxAppTest()
{}

BmsSandboxAppTest::~BmsSandboxAppTest()
{}

void BmsSandboxAppTest::SetUpTestCase()
{
    if (access(ROOT_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(ROOT_DIR);
        EXPECT_TRUE(result) << "fail to create root dir";
    }
    if (chown(ROOT_DIR.c_str(), ROOT_UID, ROOT_UID) != 0) {
        EXPECT_TRUE(false) << "fail to change root dir own ship";
    }
    if (chmod(ROOT_DIR.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
        EXPECT_TRUE(false) << "fail to change root dir mode";
    }
}

void BmsSandboxAppTest::TearDownTestCase()
{
}

void BmsSandboxAppTest::SetUp()
{
    if (!installdService_->IsServiceReady()) {
        installdService_->Start();
    }
    if (!bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStart();
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
    }
}

void BmsSandboxAppTest::TearDown()
{
}

ErrCode BmsSandboxAppTest::InstallSandboxApp(const std::string &bundleName, int32_t dplType, int32_t userId,
    int32_t &appIndex) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
    }

    return installer->InstallSandboxApp(bundleName, dplType, userId, appIndex);
}

ErrCode BmsSandboxAppTest::UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId) const
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (!installer) {
        EXPECT_FALSE(true) << "the installer is nullptr";
    }

    return installer->UninstallSandboxApp(bundleName, appIndex, userId);
}

ErrCode BmsSandboxAppTest::GetSandboxAppBundleInfo(const std::string &bundleName, const int32_t &appIndex,
    const int32_t &userId, BundleInfo &info)
{
    bool ret = GetSandboxDataMgr();
    EXPECT_TRUE(ret);

    return sandboxDataMgr_->GetSandboxAppBundleInfo(bundleName, appIndex, userId, info);
}

bool BmsSandboxAppTest::GetSandboxDataMgr()
{
    if (!sandboxDataMgr_) {
        if (!GetDataMgr()) {
            APP_LOGE("Get dataMgr shared_ptr failed");
            return false;
        }

        sandboxDataMgr_ = dataMgr_->GetSandboxDataMgr();
        if (!sandboxDataMgr_) {
            APP_LOGE("Get sandbox dataMgr shared_ptr nullptr");
            return false;
        }
    }
    return true;
}

bool BmsSandboxAppTest::GetDataMgr()
{
    if (bundleMgrService_ && !dataMgr_) {
        dataMgr_ = bundleMgrService_->GetDataMgr();
        if (!dataMgr_) {
            APP_LOGE("Get dataMgr shared_ptr nullptr");
            return false;
        }
    }
    return true;
}

ErrCode BmsSandboxAppTest::InstallBundles(const std::vector<std::string> &filePaths,
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

ErrCode BmsSandboxAppTest::UninstallBundle(const std::string &bundleName) const
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
    installParam.userId = USERID;
    installParam.installFlag = InstallFlag::NORMAL;
    bool result = installer->Uninstall(bundleName, installParam, receiver);
    EXPECT_TRUE(result);
    return receiver->GetResultCode();
}

void BmsSandboxAppTest::CheckPathAreExisted(const std::string &bundleName, int32_t appIndex)
{
    auto innerBundleName = bundleName + Constants::FILE_UNDERLINE + std::to_string(appIndex);
    auto dataPath = BUNDLE_DATA_DIR1 + innerBundleName;
    int32_t ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    dataPath = BUNDLE_DATA_DIR2 + innerBundleName;
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/cache";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/el3";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/el4";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/haps";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/preferences";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/temp";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);

    dataPath = BUNDLE_DATA_DIR4 + innerBundleName;
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_EQ(ret, 0);
}

void BmsSandboxAppTest::CheckPathAreNonExisted(const std::string &bundleName, int32_t appIndex)
{
    auto innerBundleName = bundleName + Constants::FILE_UNDERLINE + std::to_string(appIndex);
    auto dataPath = BUNDLE_DATA_DIR1 + innerBundleName;
    int32_t ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);

    dataPath = BUNDLE_DATA_DIR2 + innerBundleName;
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/cache";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/el3";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/el4";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/haps";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/preferences";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);

    dataPath = BUNDLE_DATA_DIR3 + innerBundleName + "/temp";
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);

    dataPath = BUNDLE_DATA_DIR4 + innerBundleName;
    ret = access(dataPath.c_str(), F_OK);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0100
 * @tc.name: test anormal input parameters
 * @tc.desc: 1.the input bundleName is empty
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0100, Function | SmallTest | Level0)
{
    int32_t appIndex = 0;
    auto ret = InstallSandboxApp("", DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0200
 * @tc.name: test anormal input parameters
 * @tc.desc: 1.the input dlp type is anormal
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0200, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, INVALID_DLP_TYPE, USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0300
 * @tc.name: test anormal input parameters
 * @tc.desc: 1.the input dlp type is anormal
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0300, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_3, USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0400
 * @tc.name: test anormal input parameters
 * @tc.desc: 1.the input userId is anormal
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0400, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, INVALID_USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_USER_NOT_EXIST);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0500
 * @tc.name: test anormal input parameters
 * @tc.desc: 1.the input bundleName and dlp type are anormal
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0500, Function | SmallTest | Level0)
{
    int32_t appIndex = 0;
    auto ret = InstallSandboxApp("", INVALID_DLP_TYPE, USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0600
 * @tc.name: test anormal input parameters
 * @tc.desc: 1.the input bundleName and useId are anormal
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0600, Function | SmallTest | Level0)
{
    int32_t appIndex = 0;
    auto ret = InstallSandboxApp("", DLP_TYPE_1, INVALID_USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0700
 * @tc.name: test anormal input parameters
 * @tc.desc: 1.the input dlp type and useId are anormal
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0700, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, INVALID_DLP_TYPE, INVALID_USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0800
 * @tc.name: test anormal input parameters
 * @tc.desc: 1.the input bundleName, dlp type and useId are anormal
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0800, Function | SmallTest | Level0)
{
    int32_t appIndex = 0;
    auto ret = InstallSandboxApp("", INVALID_DLP_TYPE, INVALID_USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_0900
 * @tc.name: test original bundle has not been installed
 * @tc.desc: 1. the original bundle has not been installed
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_0900, Function | SmallTest | Level0)
{
    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(UNINSTALLED_BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_APP_NOT_EXISTED);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1000
 * @tc.name: test original bundle has been installed at other userId
 * @tc.desc: 1. the original bundle has been installed at other userId
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1000, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, INVALID_USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_USER_NOT_EXIST);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1100
 * @tc.name: test original bundle has been installed at other userId
 * @tc.desc: 1. the original bundle has been installed at other userId
 *           2.the sandbox app install failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1100, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, INVALID_USERID, appIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_USER_NOT_EXIST);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1200
 * @tc.name: test original bundle has been installed at other userId
 * @tc.desc: 1. the original bundle has been installed at current userId
 *           2.the sandbox app install successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1200, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1300
 * @tc.name: test original bundle has been installed at other userId
 * @tc.desc: 1. the original bundle has been installed at current userId
 *           2.the sandbox app install successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1300, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    UninstallBundle(BUNDLE_NAME);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1400
 * @tc.name: test install sandbox app multiple times
 * @tc.desc: 1.install sandbox app multiple times
 *           2.the sandbox app install successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1400, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile1 = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile1);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_2);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_2);

    UninstallBundle(BUNDLE_NAME);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_2);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1500
 * @tc.name: test install sandbox app multiple times
 * @tc.desc: 1.install sandbox app multiple times
 *           2.the sandbox app install successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1500, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile1 = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile1);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_2);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_2);

    UninstallBundle(BUNDLE_NAME);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_2);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1600
 * @tc.name: test install sandbox app multiple times
 * @tc.desc: 1.install sandbox app multiple times
 *           2.the sandbox app install successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1600, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile1 = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile1);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_2);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_2);

    UninstallBundle(BUNDLE_NAME);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_2);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1700
 * @tc.name: test install sandbox app multiple times
 * @tc.desc: 1.install sandbox app multiple times
 *           2.the sandbox app install successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1700, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile1 = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile1);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_2);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_2);

    UninstallBundle(BUNDLE_NAME);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_2);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1800
 * @tc.name: test install sandbox app multiple times
 * @tc.desc: 1.install sandbox app multiple times
 *           2.the sandbox app install successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1800, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile1 = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile1);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    auto bundleFile2 = RESOURCE_ROOT_PATH + SANDBOX_TEST;
    filePaths.clear();
    filePaths.emplace_back(bundleFile2);
    installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(OTHER_BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_1);
    CheckPathAreExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);

    UninstallBundle(BUNDLE_NAME);
    UninstallBundle(OTHER_BUNDLE_NAME);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
    CheckPathAreNonExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_1900
 * @tc.name: test install sandbox app multiple times
 * @tc.desc: 1.install sandbox app multiple times
 *           2.the sandbox app install successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_1900, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile1 = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile1);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    auto bundleFile2 = RESOURCE_ROOT_PATH + SANDBOX_TEST;
    filePaths.clear();
    filePaths.emplace_back(bundleFile2);
    installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(OTHER_BUNDLE_NAME, DLP_TYPE_2, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_1);
    CheckPathAreExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);

    UninstallBundle(BUNDLE_NAME);
    UninstallBundle(OTHER_BUNDLE_NAME);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
    CheckPathAreNonExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_2000
 * @tc.name: test update original bundle after installing sandbox app
 * @tc.desc: 1.update original bundle after installing sandbox app
 *           2.the sandbox app will be installed successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_2000, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile1 = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile1);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_2);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_2);

    installRes = InstallBundles(filePaths, false);
    EXPECT_EQ(installRes, ERR_OK);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_2);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppInstallTest_2100
 * @tc.name: test uninstall original bundle after installing sandbox app
 * @tc.desc: 1.uninstall original bundle after installing sandbox app
 *           2.the sandbox app will be installed successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppInstallTest_2100, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile1 = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile1);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_2);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_2);

    installRes = UninstallBundle(BUNDLE_NAME);
    EXPECT_EQ(installRes, ERR_OK);

    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_1);
    CheckPathAreNonExisted(BUNDLE_NAME, APP_INDEX_2);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0100
 * @tc.name: test anormal input parameters
 * @tc.desc: 1. input bundleName is anormal
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0100, Function | SmallTest | Level0)
{
    auto ret = UninstallSandboxApp("", APP_INDEX_1, USERID);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0200
 * @tc.name: test anormal input parameters
 * @tc.desc: 1. input app index is anormal
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0200, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(BUNDLE_NAME, INVALID_APP_INDEX, USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0300
 * @tc.name: test anormal input parameters
 * @tc.desc: 1. input userId is anormal
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0300, Function | SmallTest | Level0)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(BUNDLE_NAME, APP_INDEX_1, INVALID_USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_USER_NOT_EXIST);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0400
 * @tc.name: test anormal input parameters
 * @tc.desc: 1. input bundleName and app index are anormal
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0400, Function | SmallTest | Level1)
{
    auto res = UninstallSandboxApp("", INVALID_APP_INDEX, USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0500
 * @tc.name: test anormal input parameters
 * @tc.desc: 1. input bundleName and userId are anormal
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0500, Function | SmallTest | Level1)
{
    auto res = UninstallSandboxApp("", APP_INDEX_1, INVALID_USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0600
 * @tc.name: test anormal input parameters
 * @tc.desc: 1. input app index and userId are anormal
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0600, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(BUNDLE_NAME, INVALID_APP_INDEX, INVALID_USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0700
 * @tc.name: test anormal input parameters
 * @tc.desc: 1. input bundleName, app index and userId are anormal
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0700, Function | SmallTest | Level1)
{
    auto res = UninstallSandboxApp("", INVALID_APP_INDEX, INVALID_USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0800
 * @tc.name: test uninstall sandbox without installing the original bundle
 * @tc.desc: 1. uninstall sandbox without installing the original bundle
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0800, Function | SmallTest | Level1)
{
    auto res = UninstallSandboxApp(UNINSTALLED_BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_NO_SANDBOX_APP_INFO);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_0900
 * @tc.name: test uninstall sandbox with installing the original bundle
 * @tc.desc: 1. uninstall sandbox without installing the sandbox
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_0900, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    auto res = UninstallSandboxApp(BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_NO_SANDBOX_APP_INFO);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_1000
 * @tc.name: test uninstall sandbox with installing the original bundle
 * @tc.desc: 1. uninstall sandbox with installing the sandbox at other userId
 *           2.the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_1000, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(BUNDLE_NAME, APP_INDEX_1, INVALID_USERID);
    EXPECT_EQ(res, ERR_APPEXECFWK_SANDBOX_INSTALL_USER_NOT_EXIST);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_1100
 * @tc.name: test uninstall sandbox with installing the original bundle
 * @tc.desc: 1. uninstall sandbox with installing the sandbox at current userId
 *           2.the sandbox app will be uninstalled successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_1100, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_OK);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_1200
 * @tc.name: test uninstall sandbox multiple times
 * @tc.desc: 1. uninstall sandbox multiple times
 *           2. the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_1200, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    bundleFile = RESOURCE_ROOT_PATH + SANDBOX_TEST;
    filePaths.clear();
    filePaths.emplace_back(bundleFile);
    installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(OTHER_BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_1);
    CheckPathAreExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_OK);
    CheckPathAreExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);

    UninstallBundle(BUNDLE_NAME);
    UninstallBundle(OTHER_BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_1300
 * @tc.name: test uninstall sandbox multiple times
 * @tc.desc: 1. uninstall sandbox multiple times
 *           2. the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_1300, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    bundleFile = RESOURCE_ROOT_PATH + SANDBOX_TEST;
    filePaths.clear();
    filePaths.emplace_back(bundleFile);
    installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(OTHER_BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_1);
    CheckPathAreExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(OTHER_BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_OK);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    UninstallBundle(BUNDLE_NAME);
    UninstallBundle(OTHER_BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_1400
 * @tc.name: test uninstall sandbox multiple times
 * @tc.desc: 1. uninstall sandbox multiple times
 *           2. the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_1400, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    bundleFile = RESOURCE_ROOT_PATH + SANDBOX_TEST;
    filePaths.clear();
    filePaths.emplace_back(bundleFile);
    installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(OTHER_BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_1);
    CheckPathAreExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_OK);
    CheckPathAreExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);

    res = UninstallSandboxApp(OTHER_BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_OK);

    UninstallBundle(BUNDLE_NAME);
    UninstallBundle(OTHER_BUNDLE_NAME);
}

/**
 * @tc.number: BmsSandboxAppUnInstallTest_1500
 * @tc.name: test uninstall sandbox multiple times
 * @tc.desc: 1. uninstall sandbox multiple times
 *           2. the sandbox app will be uninstalled failed
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsSandboxAppUnInstallTest_1500, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    bundleFile = RESOURCE_ROOT_PATH + SANDBOX_TEST;
    filePaths.clear();
    filePaths.emplace_back(bundleFile);
    installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    int32_t secondAppIndex = 0;
    ret = InstallSandboxApp(OTHER_BUNDLE_NAME, DLP_TYPE_1, USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, APP_INDEX_1);
    CheckPathAreExisted(OTHER_BUNDLE_NAME, APP_INDEX_1);

    auto res = UninstallSandboxApp(OTHER_BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_OK);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    res = UninstallSandboxApp(BUNDLE_NAME, APP_INDEX_1, USERID);
    EXPECT_EQ(res, ERR_OK);

    UninstallBundle(BUNDLE_NAME);
    UninstallBundle(OTHER_BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0100
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. the sandbox app install successfully
 *           3. get sandbox app bundleInfo information failed by empty bundlename
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0100, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME_INVALID, APP_INDEX_1, USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0200
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. the sandbox app install successfully
 *           3. get sandbox app bundleInfo information failed by invalid appIndex
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0200, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME, INVALID_APP_INDEX, USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0300
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. the sandbox app install successfully
 *           3. get sandbox app bundleInfo information failed by invalid userId
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0300, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME, APP_INDEX_1, INVALID_USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_NOT_INSTALLED_AT_SPECIFIED_USERID);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0400
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. the sandbox app install successfully
 *           3. get sandbox app bundleInfo information failed 
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0400, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME_INVALID, APP_INDEX_1, INVALID_USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0500
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. the sandbox app install successfully
 *           3. get sandboxApp bundleInfo information failed 
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0500, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME, INVALID_APP_INDEX, INVALID_USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0600
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. the sandbox app install successfully
 *           3. get sandbox app bundleInfo information failed 
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0600, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME_INVALID, INVALID_APP_INDEX, INVALID_USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0700
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. the sandbox app install failed
 *           2. get sandbox app bundleInfo information failed by not installed hap
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0700, Function | SmallTest | Level1)
{
    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME, APP_INDEX_1, USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_NO_SANDBOX_APP_INFO);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0800
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. get sandbox app bundleInfo information failed by not installed sandbox app
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0800, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME, APP_INDEX_1, USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_NO_SANDBOX_APP_INFO);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_0900
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. the sandbox app install failed
 *           3. get sandbox app bundleInfo information by not installed sandbox app under the specified user
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_0900, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME, APP_INDEX_1, INVALID_USERID, info);
    EXPECT_EQ(testRet, ERR_APPEXECFWK_SANDBOX_INSTALL_NOT_INSTALLED_AT_SPECIFIED_USERID);

    UninstallBundle(BUNDLE_NAME);
}

/**
 * @tc.number: BmsGETSandboxAppMSG_1000
 * @tc.name: get sandbox app bundleInfo information
 * @tc.desc: 1. install a hap successfully
 *           2. the sandbox app install successfully
 *           3. get sandbox app bundleInfo information successfully
 * @tc.require: AR000H02C4
 */
HWTEST_F(BmsSandboxAppTest, BmsGETSandboxAppMSG_1000, Function | SmallTest | Level1)
{
    std::vector<std::string> filePaths;
    auto bundleFile = RESOURCE_ROOT_PATH + RIGHT_BUNDLE_FIRST;
    filePaths.emplace_back(bundleFile);
    auto installRes = InstallBundles(filePaths, true);
    EXPECT_EQ(installRes, ERR_OK);

    int32_t appIndex = 0;
    auto ret = InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, APP_INDEX_1);
    CheckPathAreExisted(BUNDLE_NAME, APP_INDEX_1);

    BundleInfo info;
    auto testRet = GetSandboxAppBundleInfo(BUNDLE_NAME, APP_INDEX_1, USERID, info);
    EXPECT_EQ(testRet, ERR_OK);

    UninstallBundle(BUNDLE_NAME);
}