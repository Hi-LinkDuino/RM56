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

#include <future>
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_installer_interface.h"
#include "bundle_mgr_interface.h"
#include "common_tool.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "status_receiver_host.h"

using namespace testing::ext;
namespace {
const std::string THIRD_BUNDLE_PATH = "/data/test/bms_bundle/";
const std::string PRESET_BUNDLE_PATH = "/system/vendor/";
const std::string SYSTEM_BUNDLE_PATH = "/system/app/";
const std::string BASE_BUNDLE_NAME = "com.third.hiworld.example";
const std::string BASE_SYSTEM_BUNDLE_NAME = "com.system.hiworld.example";
const std::string BASE_VENDOR_BUNDLE_NAME = "com.vendor.hiworld.example";
const std::string BASE_MODULE_PACKAGE = "com.third.hiworld.example";
const std::string BUNDLE_DATA_ROOT_PATH = "/data/app/el2/100/base/";
const std::string CODE_ROOT_PATH = "/data/app/el1/bundle/public/";
const std::string ERROR_INSTALL_FAILED = "install failed!";
const std::string ERROR_UNINSTALL_FAILED = "uninstall failed!";
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILED = "Failure";
const std::string OPERATION_SUCCESS = "Success";
const int32_t REPEAT_COUNT = 10;
const int32_t MAX_BUNDLE_NAME = 6;
}  // namespace

namespace OHOS {
namespace AppExecFwk {
class StatusReceiverImpl : public StatusReceiverHost {
public:
    StatusReceiverImpl();
    virtual ~StatusReceiverImpl();
    virtual void OnStatusNotify(const int progress) override;
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override;
    std::string GetResultMsg() const;

private:
    mutable std::promise<std::string> resultMsgSignal_;

    DISALLOW_COPY_AND_MOVE(StatusReceiverImpl);
};

StatusReceiverImpl::StatusReceiverImpl()
{
    APP_LOGI("create status receiver instance");
}

StatusReceiverImpl::~StatusReceiverImpl()
{
    APP_LOGI("destroy status receiver instance");
}
void StatusReceiverImpl::OnStatusNotify(const int progress)
{
    APP_LOGI("OnStatusNotify");
}
void StatusReceiverImpl::OnFinished(const int32_t resultCode, const std::string &resultMsg)
{
    APP_LOGD("on finished result is %{public}d, %{public}s", resultCode, resultMsg.c_str());
    resultMsgSignal_.set_value(resultMsg);
}

std::string StatusReceiverImpl::GetResultMsg() const
{
    auto future = resultMsgSignal_.get_future();
    future.wait();
    std::string resultMsg = future.get();
    if (resultMsg == MSG_SUCCESS) {
        return OPERATION_SUCCESS;
    } else {
        return OPERATION_FAILED + resultMsg;
    }
}

class BmsUninstallSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void Install(
        const std::string &bundleFilePath, const InstallFlag installFlag, std::vector<std::string> &resvec);
    static void Uninstall(const std::string &bundleName, std::vector<std::string> &resvec);
    static void UninstallWithoutResvec(const std::string &bundleName);
    static void HapUninstall(
        const std::string &bundleName, const std::string &modulePackage, std::vector<std::string> &resvec);
    static int32_t ExcuteMaintainCmd(const std::string &cmd, std::vector<std::string> &cmdRes);
    void CheckBundleInfo(const std::string &version, const std::string &bundleName) const;
    static sptr<IBundleMgr> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
    bool CheckInstallIsSuccess(const std::string &bundleName) const;
    bool CheckUninstallIsSuccess(const std::string &bundleName) const;
    bool CheckAppDirNotExist(const std::string &bundleName) const;
    bool CheckAppDirExist(const std::string &bundleName) const;
    bool CheckAppDirExist(const std::string &bundleName, const std::string &modulePackage) const;
    bool CheckAppAbilityExist(
        const std::string &bundleName, const std::string &modulePackage, const std::string &abilityName) const;
};

void BmsUninstallSystemTest::SetUpTestCase()
{}

void BmsUninstallSystemTest::TearDownTestCase()
{
    std::cout << "TearDownTestCase" << std::endl;
}

void BmsUninstallSystemTest::SetUp()
{
    std::vector<std::string> resvec;
    for (int32_t i = 0; i <= MAX_BUNDLE_NAME; i++) {
        std::string bundleName = BASE_BUNDLE_NAME + std::to_string(i);
        Uninstall(bundleName, resvec);
    }
}

void BmsUninstallSystemTest::TearDown()
{}

int32_t BmsUninstallSystemTest::ExcuteMaintainCmd(const std::string &cmd, std::vector<std::string> &cmdRes)
{
    CommonTool commonTool;
    int32_t bufSize = 1024;
    return commonTool.ExcuteCmd(cmd, cmdRes, bufSize);
}

sptr<IBundleMgr> BmsUninstallSystemTest::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_LOGE("fail to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_LOGE("fail to get bundle manager proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle manager proxy success.");
    return iface_cast<IBundleMgr>(remoteObject);
}

sptr<IBundleInstaller> BmsUninstallSystemTest::GetInstallerProxy()
{
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        return nullptr;
    }

    sptr<IBundleInstaller> installerProxy = bundleMgrProxy->GetBundleInstaller();
    if (!installerProxy) {
        APP_LOGE("fail to get bundle installer proxy");
        return nullptr;
    }

    APP_LOGI("get bundle installer proxy success.");
    return installerProxy;
}

void BmsUninstallSystemTest::CheckBundleInfo(const std::string &version, const std::string &bundleName) const
{
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoRresult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);

    EXPECT_TRUE(getInfoRresult);
    EXPECT_EQ(bundleInfo.name, bundleName);
    EXPECT_EQ(bundleInfo.versionName, version);
}

bool BmsUninstallSystemTest::CheckInstallIsSuccess(const std::string &bundleName) const
{
    CommonTool commonTool;

    bool chkFileIsExist = commonTool.CheckFilePathISExist(BUNDLE_DATA_ROOT_PATH + bundleName);
    if (!chkFileIsExist) {
        APP_LOGE("[CheckInstallIsSuccess]app directory exist!");
        return false;
    }
    return true;
}

bool BmsUninstallSystemTest::CheckUninstallIsSuccess(const std::string &bundleName) const
{
    CommonTool commonTool;

    bool chkFileIsExist = commonTool.CheckFilePathISExist(BUNDLE_DATA_ROOT_PATH + bundleName);
    if (chkFileIsExist) {
        APP_LOGE("[CheckUninstallIsSuccess]app directory exist!");
        return false;
    }
    return true;
}

bool BmsUninstallSystemTest::CheckAppDirNotExist(const std::string &bundleName) const
{
    CommonTool commonTool;

    bool chkFileIsExist = commonTool.CheckFilePathISExist(BUNDLE_DATA_ROOT_PATH + bundleName);
    if (!chkFileIsExist) {
        APP_LOGE("[CheckAppDirNotExist]app directory exist!");
        return true;
    }
    return false;
}

bool BmsUninstallSystemTest::CheckAppDirExist(const std::string &bundleName) const
{
    CommonTool commonTool;

    bool chkFileIsExist = commonTool.CheckFilePathISExist(BUNDLE_DATA_ROOT_PATH + bundleName);
    if (!chkFileIsExist) {
        APP_LOGE("[CheckAppDirExist]app directory does not exist!");
        return false;
    }
    return true;
}
bool BmsUninstallSystemTest::CheckAppDirExist(const std::string &bundleName, const std::string &modulePackage) const
{
    CommonTool commonTool;

    bool chkFileIsExist = commonTool.CheckFilePathISExist(BUNDLE_DATA_ROOT_PATH + bundleName + "/" + modulePackage);
    if (!chkFileIsExist) {
        APP_LOGE("This hap does not exist!");
        return false;
    }
    return true;
}
bool BmsUninstallSystemTest::CheckAppAbilityExist(
    const std::string &bundleName, const std::string &modulePackage, const std::string &abilityName) const
{
    CommonTool commonTool;

    bool chkFileIsExist =
        commonTool.CheckFilePathISExist(CODE_ROOT_PATH + bundleName + "/" + modulePackage + "/" + abilityName);
    if (!chkFileIsExist) {
        APP_LOGE("This ability does not exist!");
        return false;
    }
    return true;
}

void BmsUninstallSystemTest::Install(
    const std::string &bundleFilePath, const InstallFlag installFlag, std::vector<std::string> &resvec)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        resvec.push_back(ERROR_INSTALL_FAILED);
        return;
    }
    InstallParam installParam;
    installParam.installFlag = installFlag;
    installParam.userId = Constants::DEFAULT_USERID;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    resvec.push_back(statusReceiver->GetResultMsg());
}

void BmsUninstallSystemTest::Uninstall(const std::string &bundleName, std::vector<std::string> &resvec)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        resvec.push_back(ERROR_UNINSTALL_FAILED);
        return;
    }

    if (bundleName.empty()) {
        APP_LOGE("bundelname is null.");
        resvec.push_back(ERROR_UNINSTALL_FAILED);
    } else {
        InstallParam installParam;
        installParam.userId = Constants::DEFAULT_USERID;
        sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
        EXPECT_NE(statusReceiver, nullptr);
        installerProxy->Uninstall(bundleName, installParam, statusReceiver);
        resvec.push_back(statusReceiver->GetResultMsg());
    }
}

void BmsUninstallSystemTest::HapUninstall(
    const std::string &bundleName, const std::string &modulePackage, std::vector<std::string> &resvec)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        resvec.push_back(ERROR_UNINSTALL_FAILED);
        return;
    }

    if (bundleName.empty()) {
        APP_LOGE("bundelname is null.");
        resvec.push_back(ERROR_UNINSTALL_FAILED);
    } else {
        InstallParam installParam;
        installParam.userId = Constants::DEFAULT_USERID;
        sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
        EXPECT_NE(statusReceiver, nullptr);
        installerProxy->Uninstall(bundleName, modulePackage, installParam, statusReceiver);
        resvec.push_back(statusReceiver->GetResultMsg());
    }
}

void BmsUninstallSystemTest::UninstallWithoutResvec(const std::string &bundleName)
{
    std::vector<std::string> resvec;
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        resvec.push_back(ERROR_UNINSTALL_FAILED);
        return;
    }

    if (bundleName.empty()) {
        APP_LOGE("bundelname is null.");
        resvec.push_back(ERROR_UNINSTALL_FAILED);
    } else {
        InstallParam installParam;
        sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
        EXPECT_NE(statusReceiver, nullptr);
        installerProxy->Uninstall(bundleName, installParam, statusReceiver);
        resvec.push_back(statusReceiver->GetResultMsg());
    }
}

/**
 * @tc.number: BMS_Uninstall_0100
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0100, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_0100 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);
    std::cout << "BMS_Uninstall_0100 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_0200
 * @tc.name: test uninstall the running bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.start the installed app
 *           4.uninstall the app
 *           5.check directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0200, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_0200 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string abilityName = "bmsThirdBundle1_A1";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    std::string cmd = "/system/bin/aa start -e 0 " + abilityName + " " + bundleName;
    std::vector<std::string> cmdRes;
    ExcuteMaintainCmd(cmd, cmdRes);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);
    std::cout << "BMS_Uninstall_0200 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_0300
 * @tc.name: test uninstall not exist bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app with wrong name
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0300, Function | MediumTest | Level2)
{
    std::cout << "BMS_Uninstall_0300 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    bundleName = BASE_BUNDLE_NAME + "notExist";
    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Failure[ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE]");

    resvec.clear();
    bundleName = BASE_BUNDLE_NAME + "1";
    Uninstall(bundleName, resvec);
    uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    std::cout << "BMS_Uninstall_0300 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_0400
 * @tc.name: repeat install and uninstall bundle 10 times
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check directory
 *           5.repeat step 10 times
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0400, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_0400 start" << std::endl;
    std::string bundleName = BASE_BUNDLE_NAME + "1";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";

    CommonTool commonTool;
    for (int32_t i = 0; i < REPEAT_COUNT; i++) {
        std::vector<std::string> resvec;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        std::string bundleName = BASE_BUNDLE_NAME + "1";
        bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
        EXPECT_TRUE(isInstallSucceed);

        resvec.clear();
        Uninstall(bundleName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
        EXPECT_TRUE(isUninstallSucceed);
    }
    std::cout << "BMS_Uninstall_0400 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_0500
 * @tc.name: test uninstall same bundle at the same time
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app twice at the same time
 *           4.check directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0500, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_0500 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    auto uninstallAction = std::async(std::launch::async, UninstallWithoutResvec, bundleName);
    auto nextUninstallAction = std::async(std::launch::async, UninstallWithoutResvec, bundleName);
    uninstallAction.wait();
    nextUninstallAction.wait();

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);
    std::cout << "BMS_Uninstall_0500 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_0600
 * @tc.name: test uninstall 3 bundles at the same time
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists three bundles
 *           2.install these app
 *           3.uninstall these bundles at the same time
 *           4.check bundleInfo
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0600, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_0600 start" << std::endl;
    CommonTool commonTool;
    for (int32_t i = 6; i < 9; i++) {
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        std::vector<std::string> resvec;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
    }

    std::vector<std::shared_future<void>> futureVec;
    for (int32_t i = 1; i < 4; i++) {
        std::string bundleName = BASE_BUNDLE_NAME + std::to_string(i);
        bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
        EXPECT_TRUE(isInstallSucceed);

        bundleName = BASE_BUNDLE_NAME + std::to_string(i);
        auto res = std::async(std::launch::async, UninstallWithoutResvec, bundleName);
        futureVec.push_back(res.share());
    }

    for (auto iter = futureVec.begin(); iter != futureVec.end(); iter++) {
        iter->wait();
    }

    for (int32_t i = 1; i < 4; i++) {
        std::string bundleName = BASE_BUNDLE_NAME + std::to_string(i);
        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoRresult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_FALSE(getInfoRresult);
    }
    std::cout << "BMS_Uninstall_0600 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_0700
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/system/app',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0700, Function | MediumTest | Level2)
{
    std::cout << "BMS_Uninstall_0700 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle1.hap";
    CommonTool commonTool;
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";

    std::string bundleName = BASE_SYSTEM_BUNDLE_NAME + "s1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success");

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);
    std::cout << "BMS_Uninstall_0700 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_0800
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/system/vendor',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0800, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_0800 start" << std::endl;
    CommonTool commonTool;
    std::vector<std::string> resvec;
    std::string bundleName = BASE_VENDOR_BUNDLE_NAME + "v1";
    std::string bundleFilePath = PRESET_BUNDLE_PATH + "bmsVendorBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);
    std::cout << "BMS_Uninstall_0800 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_0900
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_0900, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_0900 start" << std::endl;

    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    std::string modulePackage = BASE_MODULE_PACKAGE + ".h1";
    bool checkResult = CheckAppDirExist(bundleName, modulePackage);
    EXPECT_TRUE(checkResult);

    resvec.clear();
    HapUninstall(bundleName, modulePackage, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);

    std::cout << "BMS_Uninstall_0900 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_1000
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists two haps,whose type
               are entry and feature
 *           2.install these haps
 *           3.uninstall the "feature" hap
 *           4.check directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_1000, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_1000 start" << std::endl;

    CommonTool commonTool;
    std::vector<std::string> resvec;
    std::string bundleName = BASE_BUNDLE_NAME + "1";
    std::string modulePackage1 = BASE_MODULE_PACKAGE + ".h1";
    std::string modulePackage2 = BASE_MODULE_PACKAGE + ".h2";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    resvec.clear();
    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    bool checkResult = CheckAppDirExist(bundleName, modulePackage1);
    EXPECT_TRUE(checkResult);
    checkResult = CheckAppDirExist(bundleName, modulePackage2);
    EXPECT_TRUE(checkResult);

    resvec.clear();
    HapUninstall(bundleName, modulePackage2, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    checkResult = CheckAppDirExist(bundleName, modulePackage1);
    EXPECT_TRUE(checkResult);
    checkResult = CheckAppDirExist(bundleName, modulePackage2);
    EXPECT_FALSE(checkResult);

    bool result = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(result);

    bool isHapExist = false;
    for (auto hapModuleName : bundleInfo.hapModuleNames) {
        if (hapModuleName == modulePackage2) {
            isHapExist = true;
            break;
        }
    }
    EXPECT_FALSE(isHapExist);

    std::cout << "BMS_Uninstall_1000 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_1100
 * @tc.name: test during uninstall bundle can not search bundle informations
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a big app
 *           2.install the app
 *           3.uninstall the app
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_1100, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_1100 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle13.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "5";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);
    std::cout << "BMS_Uninstall_1100 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_1200
 * @tc.name: test during uninstall bundle can not search bundle informations
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a big app
 *           2.install the app
 *           3.uninstall the app and search bundle informations
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_1200, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_1200 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle13.hap";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";
    std::string bundleName = BASE_BUNDLE_NAME + "5";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    resvec.clear();
    Uninstall(bundleName, resvec);
    bool result = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(result);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);
    std::cout << "BMS_Uninstall_1200 end" << std::endl;
}

/**
 * @tc.number: BMS_Uninstall_1300
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exist two haps,whose type are entry and feature
 *           2.install these haps
 *           3.uninstall the entry hap
 *           4.check results
 */
HWTEST_F(BmsUninstallSystemTest, BMS_Uninstall_1300, Function | MediumTest | Level1)
{
    std::cout << "BMS_Uninstall_1000 start" << std::endl;

    CommonTool commonTool;
    std::vector<std::string> resvec;
    std::string bundleName = BASE_BUNDLE_NAME + "1";
    std::string modulePackage1 = BASE_MODULE_PACKAGE + ".h1";
    std::string modulePackage2 = BASE_MODULE_PACKAGE + ".h2";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    resvec.clear();
    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    bool checkResult = CheckAppDirExist(bundleName, modulePackage1);
    EXPECT_TRUE(checkResult);
    checkResult = CheckAppDirExist(bundleName, modulePackage2);
    EXPECT_TRUE(checkResult);

    resvec.clear();
    HapUninstall(bundleName, modulePackage1, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    checkResult = CheckAppDirExist(bundleName, modulePackage2);
    EXPECT_TRUE(checkResult);
    checkResult = CheckAppDirExist(bundleName, modulePackage1);
    EXPECT_FALSE(checkResult);

    bool result = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(result);

    bool hapExit = false;
    for (auto hapName : bundleInfo.hapModuleNames) {
        if (hapName == modulePackage1) {
            hapExit = true;
            break;
        }
    }
    EXPECT_FALSE(hapExit);

    std::cout << "BMS_Uninstall_1300 end" << std::endl;
}

/**
 * @tc.number: BMS_BundleInfoDeletion_0100
 * @tc.name: test delete bundleinfo after uninstall app
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check bundleinfo
 */
HWTEST_F(BmsUninstallSystemTest, BMS_BundleInfoDeletion_0100, Function | MediumTest | Level1)
{
    std::cout << "BMS_BundleInfoDeletion_0100 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);

    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool result = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(result);
    std::cout << "BMS_BundleInfoDeletion_0100 end" << std::endl;
}

/**
 * @tc.number: BMS_BundleInfoDeletion_0200
 * @tc.name: test delete bundleinfo after uninstall app
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check app permission
 */
HWTEST_F(BmsUninstallSystemTest, BMS_BundleInfoDeletion_0200, Function | MediumTest | Level1)
{
    std::cout << "BMS_BundleInfoDeletion_0200 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);

    std::cout << "BMS_BundleInfoDeletion_0200 end" << std::endl;
}

/**
 * @tc.number: BMS_BundleInfoDeletion_0300
 * @tc.name: test delete bundleinfo after uninstall app
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check UID
 */
HWTEST_F(BmsUninstallSystemTest, BMS_BundleInfoDeletion_0300, Function | MediumTest | Level1)
{
    std::cout << "BMS_BundleInfoDeletion_0300 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);
    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);

    int userId = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    int result = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    EXPECT_EQ(result, -1);
    std::cout << "BMS_BundleInfoDeletion_0300 end" << std::endl;
}

/**
 * @tc.number: BMS_BundleInfoDeletion_0400
 * @tc.name: test delete bundleinfo after uninstall app
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check bundleinfo,permission and Uid
 */
HWTEST_F(BmsUninstallSystemTest, BMS_BundleInfoDeletion_0400, Function | MediumTest | Level1)
{
    std::cout << "BMS_BundleInfoDeletion_0400 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool infoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(infoResult);

    int userId = Constants::DEFAULT_USERID;
    int uidResult = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    EXPECT_NE(uidResult, 0);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);

    infoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(infoResult);

    uidResult = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    EXPECT_EQ(uidResult, -1);
    std::cout << "BMS_BundleInfoDeletion_0400 end" << std::endl;
}

/**
 * @tc.number: BMS_DirDeletion_0100
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app and run
 *           3.uninstall the app
 *           4.check the app directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_DirDeletion_0100, Function | MediumTest | Level1)
{
    std::cout << "BMS_DirDeletion_0100 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string abilityName = "bmsThirdBundle1_A1";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);
    bool dirCheckResult = CheckAppDirExist(bundleName);
    EXPECT_TRUE(dirCheckResult);

    std::string cmd = "/system/bin/aa start -e 0 " + abilityName + " " + bundleName;
    std::vector<std::string> cmdRes;
    ExcuteMaintainCmd(cmd, cmdRes);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);

    dirCheckResult = CheckAppDirNotExist(bundleName);
    EXPECT_TRUE(dirCheckResult);
    std::cout << "BMS_DirDeletion_0100 end" << std::endl;
}

/**
 * @tc.number: BMS_DirDeletion_0200
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app
 *           4.check the app directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_DirDeletion_0200, Function | MediumTest | Level1)
{
    std::cout << "BMS_DirDeletion_0200 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);

    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_TRUE(isUninstallSucceed);

    bool dirCheckResult = CheckAppDirNotExist(bundleName);
    EXPECT_TRUE(dirCheckResult);
    std::cout << "BMS_DirDeletion_0200 end" << std::endl;
}

/**
 * @tc.number: BMS_DirDeletion_0300
 * @tc.name: test uninstall bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an app
 *           2.install the app
 *           3.uninstall the app with wrong bundle name
 *           4.unistall unsuccessfully and check the app directory
 */
HWTEST_F(BmsUninstallSystemTest, BMS_DirDeletion_0300, Function | MediumTest | Level2)
{
    std::cout << "BMS_DirDeletion_0300 start" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);
    bool dirCheckResult = CheckAppDirExist(bundleName);
    EXPECT_TRUE(dirCheckResult);

    std::string errBundleName = BASE_BUNDLE_NAME + "e";
    resvec.clear();
    Uninstall(errBundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);

    EXPECT_EQ(uninstallResult, "Failure[ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE]");

    dirCheckResult = CheckAppDirNotExist(errBundleName);
    EXPECT_TRUE(dirCheckResult);

    dirCheckResult = CheckAppDirExist(bundleName);
    EXPECT_TRUE(dirCheckResult);

    resvec.clear();
    Uninstall(bundleName, resvec);
    uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    std::cout << "BMS_DirDeletion_0300 end" << std::endl;
}

/**
 * @tc.number: BMS_UidTest_0100
 * @tc.name: test whether the uid assignment is correct
 * @tc.desc: 1.under '/data/test/bms_bundle',there are three bundles
 *           2.install these bundles
 *           3.get uid through the interface of 'GetUidByBundleName'
 *           4.uninstall these bundles
 */
HWTEST_F(BmsUninstallSystemTest, BMS_UidTest_0100, Function | MediumTest | Level1)
{
    std::cout << "BMS_UidTest_0100 start" << std::endl;
    CommonTool commonTool;
    for (int32_t i = 6; i < 9; i++) {
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        std::vector<std::string> resvec;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        std::string bundleName = BASE_BUNDLE_NAME + std::to_string(i - 5);
        bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
        EXPECT_TRUE(isInstallSucceed);

        int userId = Constants::DEFAULT_USERID;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
        EXPECT_GE(uid, Constants::BASE_APP_UID);
        EXPECT_LE(uid, Constants::MAX_APP_UID);
    }

    std::vector<std::shared_future<void>> futureVec;
    for (int32_t i = 1; i <= 3; i++) {
        std::string bundleName = BASE_BUNDLE_NAME + std::to_string(i);
        auto res = std::async(std::launch::async, UninstallWithoutResvec, bundleName);
        futureVec.push_back(res.share());
    }

    for (auto iter = futureVec.begin(); iter != futureVec.end(); iter++) {
        iter->wait();
    }

    for (int32_t i = 1; i <= 3; i++) {
        std::string bundleName = BASE_BUNDLE_NAME + std::to_string(i);
        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoRresult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_FALSE(getInfoRresult);
    }
    std::cout << "BMS_UidTest_0100 start" << std::endl;
}

/**
 * @tc.number: BMS_UidTest_0200
 * @tc.name: test whether the uid assignment is correct
 * @tc.desc: 1.under '/system/vendor',there are two bundles
 *           2.install the app
 *           3.get uid through the interface of 'GetUidByBundleName'
 *           4.uninstall the app
 */
HWTEST_F(BmsUninstallSystemTest, BMS_UidTest_0200, Function | MediumTest | Level1)
{
    std::cout << "BMS_UidTest_0200 start" << std::endl;
    std::vector<std::string> resvec;
    for (int32_t i = 2; i <= 3; i++) {
        std::string bundleFilePath = PRESET_BUNDLE_PATH + "bmsVendorBundle" + std::to_string(i) + ".hap";
        std::string bundleName = BASE_VENDOR_BUNDLE_NAME + "v" + std::to_string(i);

        std::vector<std::string> resvec;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        int userId = Constants::DEFAULT_USERID;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
        EXPECT_GE(uid, Constants::BASE_APP_UID);
        EXPECT_LE(uid, Constants::MAX_APP_UID);
        resvec.clear();
        Uninstall(bundleName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    }
    std::cout << "BMS_UidTest_0200 end" << std::endl;
}

/**
 * @tc.number: BMS_UidTest_0300
 * @tc.name: test whether the uid assignment is correct
 * @tc.desc: 1.under '/system/app',there is a bundle
 *           2.cheak install status of the app
 *           3.get uid through the interface of 'GetUidByBundleName'
 *           4.try to unistall the app
 */
HWTEST_F(BmsUninstallSystemTest, BMS_UidTest_0300, Function | MediumTest | Level1)
{
    std::cout << "BMS_UidTest_0300 start" << std::endl;
    CommonTool commonTool;
    std::vector<std::string> resvec;
    std::string bundleName = "com.ohos.settings";
    bool isInstallSucceed = CheckInstallIsSuccess(bundleName);
    EXPECT_TRUE(isInstallSucceed);
    int userId = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    EXPECT_GE(uid, Constants::BASE_SYS_UID);
    EXPECT_LE(uid, Constants::MAX_SYS_UID);
    resvec.clear();
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Failure[MSG_ERR_UNINSTALL_SYSTEM_APP_ERROR]");
    bool isUninstallSucceed = CheckUninstallIsSuccess(bundleName);
    EXPECT_FALSE(isUninstallSucceed);
    APP_LOGE("Do not have permission to uninstall!");

    std::cout << "BMS_UidTest_0300 end" << std::endl;
}

/**
 * @tc.number: BMS_DFX_0500
 * @tc.name: test error code
 * @tc.desc: 1.there is an app which includes two haps
 *           2.install two haps
 *           3.uninstall hap with the first module package
 *           4.uninstall hap with the first module package
 *           5.get ERR_UNINSTALL_MISSING_INSTALLED_MODULE
 */
HWTEST_F(BmsUninstallSystemTest, BMS_DFX_0500, Function | MediumTest | Level2)
{
    std::cout << "START BMS_DFX_0500" << std::endl;
    std::string bundleFilePath1 = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string modulePackage1 = BASE_MODULE_PACKAGE + ".h1";
    std::string modulePackage2 = BASE_MODULE_PACKAGE + ".h2";
    std::vector<std::string> resvec;
    std::string installMsg;
    CommonTool commonTool;
    Install(bundleFilePath1, InstallFlag::NORMAL, resvec);
    installMsg = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath1;
    resvec.clear();

    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    Install(bundleFilePath2, InstallFlag::NORMAL, resvec);
    installMsg = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath2;
    resvec.clear();

    std::string bundleName = BASE_BUNDLE_NAME + "1";

    HapUninstall(bundleName, modulePackage1, resvec);
    std::string uninstallMsg = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallMsg, "Success") << "unistall fail!";
    resvec.clear();

    HapUninstall(bundleName, modulePackage1, resvec);
    uninstallMsg = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallMsg, "Failure[ERR_UNINSTALL_MISSING_INSTALLED_MODULE]");
    resvec.clear();

    HapUninstall(bundleName, modulePackage2, resvec);
    uninstallMsg = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!";
    std::cout << "END BMS_DFX_0500" << std::endl;
}
}  // namespace AppExecFwk
}  // namespace OHOS