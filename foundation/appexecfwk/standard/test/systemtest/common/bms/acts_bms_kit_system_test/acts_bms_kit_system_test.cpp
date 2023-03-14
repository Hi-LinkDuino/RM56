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

#include <fcntl.h>
#include <fstream>
#include <future>
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_mgr_proxy.h"
#include "bundle_status_callback_host.h"
#include "clean_cache_callback_host.h"
#include "common_tool.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "status_receiver_host.h"
#include "system_ability_definition.h"
#include "testConfigParser.h"

using OHOS::AAFwk::Want;
using namespace testing::ext;
using namespace std::chrono_literals;

namespace {
const std::string THIRD_BUNDLE_PATH = "/data/test/bms_bundle/";
const std::string SYSTEM_BUNDLE_PATH = "/system/app/";
const std::string BASE_BUNDLE_NAME = "com.third.hiworld.example";
const std::string SYSTEM_LAUNCHER_BUNDLE_NAME = "com.ohos.launcher";
const std::string SYSTEM_SETTINGS_BUNDLE_NAME = "com.ohos.settings";
const std::string SYSTEM_SYSTEMUI_BUNDLE_NAME = "com.ohos.systemui";
const std::string BUNDLE_DATA_ROOT_PATH = "/data/app/el2/100/base/";
const std::string ERROR_INSTALL_FAILED = "install failed!";
const std::string ERROR_UNINSTALL_FAILED = "uninstall failed!";
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILED = "Failure";
const std::string OPERATION_SUCCESS = "Success";
const int COMPATIBLEVERSION = 3;
const int TARGETVERSION = 3;
}  // namespace

namespace OHOS {
namespace AppExecFwk {
class BundleStatusCallbackImpl : public BundleStatusCallbackHost {
public:
    BundleStatusCallbackImpl();
    virtual ~BundleStatusCallbackImpl() override;
    virtual void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode, const std::string &resultMsg,
        const std::string &bundleName) override;
    virtual void OnBundleAdded(const std::string &bundleName, const int userId) override {};
    virtual void OnBundleUpdated(const std::string &bundleName, const int userId) override {};
    virtual void OnBundleRemoved(const std::string &bundleName, const int userId) override {};

private:
    DISALLOW_COPY_AND_MOVE(BundleStatusCallbackImpl);
};

BundleStatusCallbackImpl::BundleStatusCallbackImpl()
{
    APP_LOGI("create bundle status instance");
}

BundleStatusCallbackImpl::~BundleStatusCallbackImpl()
{
    APP_LOGI("destroy bundle status instance");
}

void BundleStatusCallbackImpl::OnBundleStateChanged(
    const uint8_t installType, const int32_t resultCode, const std::string &resultMsg, const std::string &bundleName)
{
    APP_LOGI("BMS_Kit_St OnBundleStateChanged results are %{public}d, %{public}d, %{public}s, %{public}s",
        installType,
        resultCode,
        resultMsg.c_str(),
        bundleName.c_str());
}

class CleanCacheCallBackImpl : public CleanCacheCallbackHost {
public:
    CleanCacheCallBackImpl();
    virtual ~CleanCacheCallBackImpl() override;

    virtual void OnCleanCacheFinished(bool succeeded) override;
    bool GetSucceededResult() const;

private:
    mutable std::promise<bool> resultSucceededSignal_;
    DISALLOW_COPY_AND_MOVE(CleanCacheCallBackImpl);
};

CleanCacheCallBackImpl::CleanCacheCallBackImpl()
{
    APP_LOGI("create bundle status instance");
}

CleanCacheCallBackImpl::~CleanCacheCallBackImpl()
{
    APP_LOGI("destroy bundle status instance");
}

void CleanCacheCallBackImpl::OnCleanCacheFinished(bool succeeded)
{
    APP_LOGI("BMS_Kit_St OnCleanCacheFinished results are %{public}d", succeeded);
    resultSucceededSignal_.set_value(!succeeded);
}

bool CleanCacheCallBackImpl::GetSucceededResult() const
{
    auto future = resultSucceededSignal_.get_future();
    future.wait();
    return future.get();
}

class StatusReceiverImpl : public StatusReceiverHost {
public:
    StatusReceiverImpl();
    virtual ~StatusReceiverImpl();
    virtual void OnStatusNotify(const int progress) override;
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override;
    std::string GetResultMsg() const;

private:
    mutable std::promise<std::string> resultMsgSignal_;
    int iProgress_ = 0;

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

void StatusReceiverImpl::OnFinished(const int32_t resultCode, const std::string &resultMsg)
{
    APP_LOGD("OnFinished result is %{public}d, %{public}s", resultCode, resultMsg.c_str());
    resultMsgSignal_.set_value(resultMsg);
}
void StatusReceiverImpl::OnStatusNotify(const int progress)
{
    EXPECT_GT(progress, iProgress_);
    iProgress_ = progress;
    APP_LOGI("OnStatusNotify progress:%{public}d", progress);
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

class ActsBmsKitSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void Install(
        const std::string &bundleFilePath, const InstallFlag installFlag, std::vector<std::string> &resvec);
    static void Uninstall(const std::string &bundleName, std::vector<std::string> &resvec);
    static void HapUninstall(
        const std::string &bundleName, const std::string &modulePackage, std::vector<std::string> &resvec);
    static sptr<IBundleMgr> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
    void CheckBundleInfo(const uint32_t index, BundleInfo &bundleInfo) const;
    void CreateDir(const std::string &path) const;
    void CheckFileExist(const std::string &bundleName) const;
    void CheckFileExist(const std::string &bundleName, const std::string &modulePackage) const;
    void CheckFileNonExist(const std::string &bundleName) const;
    void CheckFileNonExist(const std::string &bundleName, const std::string &modulePackage) const;
    static StressTestLevel stLevel_;
};
StressTestLevel ActsBmsKitSystemTest::stLevel_{};

void ActsBmsKitSystemTest::SetUpTestCase()
{
    TestConfigParser tcp;
    tcp.ParseFromFile4StressTest(STRESS_TEST_CONFIG_FILE_PATH, stLevel_);
    std::cout << "stress test level : "
              << "BMS : " << stLevel_.BMSLevel << std::endl;
}

void ActsBmsKitSystemTest::TearDownTestCase()
{
    std::cout << "BmsInstallSystemTest TearDownTestCase" << std::endl;
}

void ActsBmsKitSystemTest::SetUp()
{}

void ActsBmsKitSystemTest::TearDown()
{}

void ActsBmsKitSystemTest::Install(
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

void ActsBmsKitSystemTest::Uninstall(const std::string &bundleName, std::vector<std::string> &resvec)
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

void ActsBmsKitSystemTest::HapUninstall(
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

sptr<IBundleMgr> ActsBmsKitSystemTest::GetBundleMgrProxy()
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

sptr<IBundleInstaller> ActsBmsKitSystemTest::GetInstallerProxy()
{
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        return nullptr;
    }

    sptr<IBundleInstaller> installerProxy = bundleMgrProxy->GetBundleInstaller();
    if (!installerProxy) {
        APP_LOGE("fail to get bundle installer proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle installer proxy success.");
    return installerProxy;
}

void ActsBmsKitSystemTest::CheckFileExist(const std::string &bundleName) const
{
    int bundleDataExist = access((BUNDLE_DATA_ROOT_PATH + bundleName).c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir doesn't exist: " << bundleName;
}

void ActsBmsKitSystemTest::CheckFileExist(const std::string &bundleName, const std::string &modulePackage) const
{
    int bundleDataExist = access((BUNDLE_DATA_ROOT_PATH + bundleName + "/" + modulePackage).c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir doesn't exist: " << bundleName;
}

void ActsBmsKitSystemTest::CheckFileNonExist(const std::string &bundleName) const
{
    int bundleDataExist = access((BUNDLE_DATA_ROOT_PATH + bundleName).c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0) << "the bundle data dir exists: " << bundleName;
}

void ActsBmsKitSystemTest::CheckFileNonExist(const std::string &bundleName, const std::string &modulePackage) const
{
    int bundleDataExist = access((BUNDLE_DATA_ROOT_PATH + bundleName + "/" + modulePackage).c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0) << "the bundle data dir exists: " << bundleName;
}

void ActsBmsKitSystemTest::CheckBundleInfo(const uint32_t index, BundleInfo &bundleInfo) const
{
    EXPECT_EQ(bundleInfo.name, BASE_BUNDLE_NAME + std::to_string(index));
    EXPECT_EQ(bundleInfo.minSdkVersion, 0);
    EXPECT_EQ(bundleInfo.maxSdkVersion, 0);
    EXPECT_GE(bundleInfo.uid, Constants::BASE_APP_UID);
    EXPECT_LE(bundleInfo.uid, Constants::MAX_APP_UID);
    EXPECT_EQ(bundleInfo.vendor, "example");
    EXPECT_EQ(bundleInfo.versionCode, index);
    std::string strVersion = std::to_string(index) + ".0";
    EXPECT_EQ(bundleInfo.versionName, strVersion);
    EXPECT_EQ(bundleInfo.jointUserId, "");
    EXPECT_EQ(bundleInfo.compatibleVersion, COMPATIBLEVERSION);
    EXPECT_EQ(bundleInfo.targetVersion, TARGETVERSION);
    std::vector<AbilityInfo> abilities = bundleInfo.abilityInfos;
    for (auto iter = abilities.begin(); iter != abilities.end(); iter++) {
        EXPECT_EQ(iter->bundleName, BASE_BUNDLE_NAME + std::to_string(index));
        EXPECT_EQ(iter->description, "");
        EXPECT_EQ(iter->label, "bmsThirdBundle_A2 Ability");
        EXPECT_EQ(iter->moduleName, "testability");
        std::cout << "abilityInfo-moduleName:" << iter->moduleName << std::endl;
        EXPECT_EQ(iter->uri, "");
        EXPECT_EQ(iter->visible, true);
        int iLaunchMode = (int)iter->launchMode;
        EXPECT_EQ(iLaunchMode, 0);
        int iOrientation = (int)iter->orientation;
        EXPECT_EQ(iOrientation, 0);
        int iType = (int)iter->type;
        EXPECT_EQ(iType, 1);
    }
    ApplicationInfo applicationInfo = bundleInfo.applicationInfo;
    EXPECT_EQ(applicationInfo.name, (BASE_BUNDLE_NAME + std::to_string(index)));
    EXPECT_FALSE(applicationInfo.isSystemApp);
    EXPECT_EQ(applicationInfo.supportedModes, 0);
    for (auto appModuleInfo : applicationInfo.moduleInfos) {
        std::cout << "applicationInfo-moduleName:" << appModuleInfo.moduleName << std::endl;
        std::cout << "applicationInfo-moduleSourceDir:" << appModuleInfo.moduleSourceDir << std::endl;
    }
    std::cout << "applicationInfo-entryDir:" << applicationInfo.entryDir << std::endl;
}

void ActsBmsKitSystemTest::CreateDir(const std::string &path) const
{
    if (access(path.c_str(), F_OK) != 0) {
        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            APP_LOGE("CreateDir:%{private}s error", path.c_str());
        }
    }
}

/**
 * @tc.number: GetBundleInfo_0100
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query bundleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfo_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle24.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        BundleInfo bundleInfo;
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        CheckBundleInfo(1, bundleInfo);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (!getInfoResult) {
            APP_LOGI("GetBundleInfo_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0100" << std::endl;
}

/**
 * @tc.number: GetBundleInfo_0200
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query bundleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfo_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle25.hap";
        std::string appName = BASE_BUNDLE_NAME + "2";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        BundleInfo bundleInfo;
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        CheckBundleInfo(2, bundleInfo);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!getInfoResult) {
            APP_LOGI("GetBundleInfo_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0200" << std::endl;
}

/**
 * @tc.number: GetBundleInfo_0300
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query bundleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0300, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfo_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle26.hap";
        std::string appName = BASE_BUNDLE_NAME + "3";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        BundleInfo bundleInfo;
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        CheckBundleInfo(3, bundleInfo);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!getInfoResult) {
            APP_LOGI("GetBundleInfo_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0300" << std::endl;
}

/**
 * @tc.number: GetBundleInfo_0400
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal hap
 *           2.install the hap
 *           3.query bundleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0400, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfo_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle27.hap";
        std::string appName = BASE_BUNDLE_NAME + "4";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_PARSE_NO_PROFILE]") << "Success!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        BundleInfo bundleInfo;
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
        EXPECT_FALSE(getInfoResult);

        if (getInfoResult) {
            APP_LOGI("GetBundleInfo_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0400" << std::endl;
}

/**
 * @tc.number: GetBundleInfo_0500
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query bundleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0500, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfo_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle28.rpk";
        std::string appName = BASE_BUNDLE_NAME + "5";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_INVALID_HAP_NAME]");
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        BundleInfo bundleInfo;
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
        EXPECT_FALSE(getInfoResult);
        if (getInfoResult) {
            APP_LOGI("GetBundleInfo_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0500" << std::endl;
}

/**
 * @tc.number: GetBundleInfo_0600
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query bundleInfo with wrong appname
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0600, Function | MediumTest | Level2)
{
    std::cout << "START GetBundleInfo_0600" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        appName = BASE_BUNDLE_NAME + "e";
        BundleInfo bundleInfo;
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_FALSE(getInfoResult);
        resvec.clear();
        appName = BASE_BUNDLE_NAME + "1";
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (getInfoResult) {
            APP_LOGI("GetBundleInfo_0600 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0600 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0600" << std::endl;
}

/**
 * @tc.number: GetBundleInfo_0700
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/system/app/',there is a hap
 *           2.install the hap
 *           3.query bundleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0700, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfo_0700" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string appName = "com.ohos.systemui";

        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }

        BundleInfo bundleInfo;
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(bundleInfo.name, appName);
        EXPECT_EQ(bundleInfo.minSdkVersion, 0);
        EXPECT_EQ(bundleInfo.maxSdkVersion, 0);
        EXPECT_GE(bundleInfo.uid, Constants::BASE_SYS_UID);
        EXPECT_LE(bundleInfo.uid, Constants::MAX_SYS_UID);
        EXPECT_GE(bundleInfo.gid, Constants::BASE_SYS_UID);
        EXPECT_LE(bundleInfo.gid, Constants::MAX_SYS_UID);
        if (!getInfoResult) {
            APP_LOGI("GetBundleInfo_0700 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0700 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0700" << std::endl;
}

/**
 * @tc.number: GetBundleInfo_0800
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/system/app/',there is a hap
 *           2.install the hap
 *           3.query bundleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0800, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfo_0800" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_FALSE(bundleInfo.isDifferentName);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (!getInfoResult) {
            APP_LOGI("GetBundleInfo_0800 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0800 seccess - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0800" << std::endl;
}

/**
 * @tc.number: GetBundleInfo_0900
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/system/app/',there is a hap
 *           2.install the hap
 *           3.query bundleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfo_0900, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfo_0900" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_TRUE(bundleInfo.isDifferentName);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (!getInfoResult) {
            APP_LOGI("GetBundleInfo_0900 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfo_0900 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfo_0900" << std::endl;
}

/**
 * @tc.number: GetBundleInfos_0100
 * @tc.name: test query bundleinfos
 * @tc.desc: 1.under '/data/test/bms_bundle',there exist three bundles
 *           2.install the bundles
 *           3.query all bundleinfos
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfos_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfos_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        CommonTool commonTool;
        std::string installResult;
        for (int i = 6; i < 9; i++) {
            std::vector<std::string> resvec;
            std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
            Install(hapFilePath, InstallFlag::NORMAL, resvec);
            installResult = commonTool.VectorToStr(resvec);
            EXPECT_EQ(installResult, "Success") << "install fail!";
        }
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::vector<BundleInfo> bundleInfos;
        bool getInfoResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
        EXPECT_TRUE(getInfoResult);

        bool isSubStrExist = false;
        for (int i = 1; i <= 3; i++) {
            std::string appName = BASE_BUNDLE_NAME + std::to_string(i);
            for (auto iter = bundleInfos.begin(); iter != bundleInfos.end(); iter++) {
                if (IsSubStr(iter->name, appName)) {
                    isSubStrExist = true;
                    break;
                }
            }
            EXPECT_TRUE(isSubStrExist);
            std::vector<std::string> resvec2;
            Uninstall(appName, resvec2);
            std::string uninstallResult = commonTool.VectorToStr(resvec2);
            EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        }
        if (!getInfoResult) {
            APP_LOGI("GetBundleInfos_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfos_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfos_0100" << std::endl;
}

/**
 * @tc.number: GetBundleInfos_0200
 * @tc.name: test query bundleinfos
 * @tc.desc: 1.under '/system/app/bms_bundle',there exist some hap
 *           2.query all bundleinfos
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfos_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfos_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }

        std::vector<BundleInfo> bundleInfos;
        bool getInfoResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
        EXPECT_TRUE(getInfoResult);
        int count = 0;
        for (auto bundleInfo : bundleInfos) {
            if (IsSubStr(bundleInfo.name, SYSTEM_LAUNCHER_BUNDLE_NAME)) {
                count++;
            } else if (IsSubStr(bundleInfo.name, SYSTEM_SETTINGS_BUNDLE_NAME)) {
                count++;
            } else if (IsSubStr(bundleInfo.name, SYSTEM_SYSTEMUI_BUNDLE_NAME)) {
                count++;
            }
            if (count == 3) {
                break;
            }
        }
        EXPECT_GE(count, 2);

        if (!getInfoResult) {
            APP_LOGI("GetBundleInfos_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfos_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfos_0200" << std::endl;
}

/**
 * @tc.number: GetApplicationInfo_0100
 * @tc.name: test query application information
 * EnvConditions: system running normally
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query appinfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfo_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetApplicationInfo_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        ApplicationInfo appInfo;
        int userId = Constants::DEFAULT_USERID;
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfo(appName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(appInfo.name, appName);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!getInfoResult) {
            APP_LOGI("GetApplicationInfo_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfo_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfo_0100" << std::endl;
}

/**
 * @tc.number: GetApplicationInfo_0200
 * @tc.name: test query application information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query appinfo with permission
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfo_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetApplicationInfo_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        ApplicationInfo appInfo;
        int userId = Constants::DEFAULT_USERID;
        bool getInfoResult = bundleMgrProxy->GetApplicationInfo(
            appName, ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, appInfo);
        std::string permission = commonTool.VectorToStr(appInfo.permissions);
        EXPECT_TRUE(getInfoResult);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!getInfoResult) {
            APP_LOGI("GetApplicationInfo_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfo_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfo_0200" << std::endl;
}

/**
 * @tc.number: GetApplicationInfo_0300
 * @tc.name: test query application information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query appInfo with wrong appname
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfo_0300, Function | MediumTest | Level2)
{
    std::cout << "START GetApplicationInfo_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        ApplicationInfo appInfo;
        int userId = Constants::DEFAULT_USERID;
        appName = BASE_BUNDLE_NAME + "e";
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfo(appName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
        EXPECT_FALSE(getInfoResult);
        resvec.clear();
        appName = BASE_BUNDLE_NAME + "1";
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (getInfoResult) {
            APP_LOGI("GetApplicationInfo_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfo_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfo_0300" << std::endl;
}

/**
 * @tc.number: GetApplicationInfo_0400
 * @tc.name: test GetApplicationInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call GetApplicationInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfo_0400, Function | MediumTest | Level1)
{
    std::cout << "START GetApplicationInfo_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        ApplicationInfo appInfo;
        int userId = Constants::DEFAULT_USERID;
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfo(appName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(appInfo.name, appName);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!getInfoResult) {
            APP_LOGI("GetApplicationInfo_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfo_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfo_0400" << std::endl;
}

/**
 * @tc.number: GetApplicationInfo_0500
 * @tc.name: test GetApplicationInfo interface
 * @tc.desc: 1.under '/system/app',there is a hap
 *           2.install the hap
 *           3.call GetApplicationInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfo_0500, Function | MediumTest | Level1)
{
    std::cout << "START GetApplicationInfo_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }

        ApplicationInfo appInfo;
        int userId = 0;
        bool getInfoResult = bundleMgrProxy->GetApplicationInfo(
            SYSTEM_SETTINGS_BUNDLE_NAME, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(appInfo.name, SYSTEM_SETTINGS_BUNDLE_NAME);
        if (!getInfoResult) {
            APP_LOGI("GetApplicationInfo_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfo_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfo_0500" << std::endl;
}

/**
 * @tc.number: GetApplicationInfo_0600
 * @tc.name: test GetApplicationInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.uninstall the hap
 *           4.call GetApplicationInfo to get application info
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfo_0600, Function | MediumTest | Level1)
{
    std::cout << "START GetApplicationInfo_0600" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success");

        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        ApplicationInfo appInfo;
        int userId = -1;
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfo(appName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
        EXPECT_FALSE(getInfoResult);
        if (getInfoResult) {
            APP_LOGI("GetApplicationInfo_0600 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfo_0600 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfo_0600" << std::endl;
}

/**
 * @tc.number: GetApplicationInfo_0700
 * @tc.name: test GetApplicationInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.uninstall the hap
 *           4.call GetApplicationInfo to get application info
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfo_0700, Function | MediumTest | Level1)
{
    std::cout << "START GetApplicationInfo_0700" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        ApplicationInfo appInfo;
        int userId = Constants::DEFAULT_USERID;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfo(appName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(appInfo.flags, 0);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (!getInfoResult) {
            APP_LOGI("GetApplicationInfo_0700 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfo_0700 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfo_0700" << std::endl;
}

/**
 * @tc.number: GetApplicationInfos_0100
 * @tc.name: test query applicationinfos
 * @tc.desc: 1.under '/data/test/bms_bundle',there exist three bundles
 *           2.install these bundles
 *           3.query all appinfos
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfos_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetApplicationInfos_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        CommonTool commonTool;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string installResult;
        int userId = Constants::DEFAULT_USERID;
        for (int i = 6; i <= 8; i++) {
            std::vector<std::string> resvec;
            std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
            std::string appName = BASE_BUNDLE_NAME + std::to_string(i - 5);
            Install(hapFilePath, InstallFlag::NORMAL, resvec);
            installResult = commonTool.VectorToStr(resvec);
            EXPECT_EQ(installResult, "Success") << "install fail!";

            std::vector<ApplicationInfo> appInfos;
            bool getInfoResult =
                bundleMgrProxy->GetApplicationInfos(
                    ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, appInfos);
            EXPECT_TRUE(getInfoResult);
            resvec.clear();
            Uninstall(appName, resvec);
            std::string uninstallResult = commonTool.VectorToStr(resvec);
            EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

            bool isSubStrExist = false;
            for (auto iter = appInfos.begin(); iter != appInfos.end(); iter++) {
                if (IsSubStr(iter->name, appName)) {
                    isSubStrExist = true;
                    break;
                }
            }
            EXPECT_TRUE(isSubStrExist);
            if (!getInfoResult) {
                APP_LOGI("GetApplicationInfos_0100 failed - cycle count: %{public}d", i);
                break;
            }
            result = true;
        }
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfos_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfos_0100" << std::endl;
}

/**
 * @tc.number: GetApplicationInfos_0200
 * @tc.name: test query applicationinfos
 * @tc.desc: 1.there are some system-app installed in system
 *           2.query all appinfos
 */
HWTEST_F(ActsBmsKitSystemTest, GetApplicationInfos_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetApplicationInfos_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        int userId = Constants::DEFAULT_USERID;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::vector<ApplicationInfo> appInfos;
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfos(
                ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, appInfos);
        EXPECT_TRUE(getInfoResult);
        int count = 0;
        for (auto appInfo : appInfos) {
            if (IsSubStr(appInfo.name, SYSTEM_LAUNCHER_BUNDLE_NAME)) {
                count++;
            } else if (IsSubStr(appInfo.name, SYSTEM_SETTINGS_BUNDLE_NAME)) {
                count++;
            } else if (IsSubStr(appInfo.name, SYSTEM_SYSTEMUI_BUNDLE_NAME)) {
                count++;
            }
            if (count == 3) {
                break;
            }
        }
        EXPECT_GE(count, 2);

        if (!getInfoResult) {
            APP_LOGI("GetApplicationInfos_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetApplicationInfos_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetApplicationInfos_0200" << std::endl;
}

/**
 * @tc.number: GetBundleArchiveInfo_0100
 * @tc.name: test query archive information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.query archive information without an ability information
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleArchiveInfo_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleArchiveInfo_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";

        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult =
            bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(bundleInfo.name, appName);
        std::string version = "1.0";
        EXPECT_EQ(bundleInfo.versionName, version);

        if (!getInfoResult) {
            APP_LOGI("GetBundleArchiveInfo_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleArchiveInfo_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleArchiveInfo_0100" << std::endl;
}

/**
 * @tc.number: GetBundleArchiveInfo_0200
 * @tc.name: test query archive information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.query archive with ability information
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleArchiveInfo_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleArchiveInfo_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string abilityName = "bmsThirdBundle_A1";
        std::string appName = BASE_BUNDLE_NAME + "1";

        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult =
            bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(bundleInfo.name, appName);
        std::string version = "1.0";
        EXPECT_EQ(bundleInfo.versionName, version);

        bool isSubStrExist = false;
        for (auto abilityInfo : bundleInfo.abilityInfos) {
            if (IsSubStr(abilityInfo.name, abilityName)) {
                isSubStrExist = true;
                break;
            }
        }
        EXPECT_TRUE(isSubStrExist);

        if (!getInfoResult) {
            APP_LOGI("GetBundleArchiveInfo_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleArchiveInfo_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleArchiveInfo_0200" << std::endl;
}

/**
 * @tc.number: GetBundleArchiveInfo_0300
 * @tc.name: test query hap information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.query hap information with wrong name
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleArchiveInfo_0300, Function | MediumTest | Level2)
{
    std::cout << "START GetBundleArchiveInfo_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string hapFilePath = THIRD_BUNDLE_PATH + "tt.hap";
        bool getInfoResult =
            bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_FALSE(getInfoResult);

        if (getInfoResult) {
            APP_LOGI("GetBundleArchiveInfo_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleArchiveInfo_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleArchiveInfo_0300" << std::endl;
}

/**
 * @tc.number: GetBundleArchiveInfo_0400
 * @tc.name: test query archive information
 * @tc.desc: 1.under '/system/app',there is a hap
 *           2.query archive information
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleArchiveInfo_0400, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleArchiveInfo_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string hapFilePath = SYSTEM_BUNDLE_PATH + "Settings.hap";

        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult =
            bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(bundleInfo.name, SYSTEM_SETTINGS_BUNDLE_NAME);

        if (!getInfoResult) {
            APP_LOGI("GetBundleArchiveInfo_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleArchiveInfo_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleArchiveInfo_0400" << std::endl;
}

/**
 * @tc.number: GetBundleArchiveInfo_0500
 * @tc.name: test query  ".rpk" information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap with invalid suffix
 *           2.query the archive information
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleArchiveInfo_0500, Function | MediumTest | Level2)
{
    std::cout << "START GetBundleArchiveInfo_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle28.rpk";

        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult =
            bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_FALSE(getInfoResult);

        if (getInfoResult) {
            APP_LOGI("GetBundleArchiveInfo_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleArchiveInfo_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleArchiveInfo_0500" << std::endl;
}

/**
 * @tc.number: GetUidByBundleName_0100
 * @tc.name: test query UID
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query UID by bundleName
 */
HWTEST_F(ActsBmsKitSystemTest, GetUidByBundleName_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetUidByBundleName_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string bundleName = BASE_BUNDLE_NAME + "1";
        Install(hapFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        int userId = Constants::DEFAULT_USERID;
        int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
        EXPECT_GE(uid, Constants::BASE_APP_UID);
        EXPECT_LE(uid, Constants::MAX_APP_UID);
        resvec.clear();
        Uninstall(bundleName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (uid == Constants::INVALID_UID) {
            APP_LOGI("GetUidByBundleName_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetUidByBundleName_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetUidByBundleName_0100" << std::endl;
}

/**
 * @tc.number: GetUidByBundleName_0200
 * @tc.name: test query UID
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query UID by bundleName with wrong userid
 */
HWTEST_F(ActsBmsKitSystemTest, GetUidByBundleName_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetUidByBundleName_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string bundleName = BASE_BUNDLE_NAME + "1";
        Install(hapFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        int userId = Constants::INVALID_USERID;
        int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
        EXPECT_NE(uid, Constants::INVALID_USERID);
        resvec.clear();
        Uninstall(bundleName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (uid == Constants::INVALID_UID) {
            APP_LOGI("GetUidByBundleName_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetUidByBundleName_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetUidByBundleName_0200" << std::endl;
}

/**
 * @tc.number: GetUidByBundleName_0300
 * @tc.name: test query UID
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query UID by wrong bundleName
 */
HWTEST_F(ActsBmsKitSystemTest, GetUidByBundleName_0300, Function | MediumTest | Level2)
{
    std::cout << "START GetUidByBundleName_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string bundleName = BASE_BUNDLE_NAME + "1";
        Install(hapFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        int userId = Constants::DEFAULT_USERID;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleName = BASE_BUNDLE_NAME + "e";
        int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
        EXPECT_EQ(uid, Constants::INVALID_UID);

        resvec.clear();
        bundleName = BASE_BUNDLE_NAME + "1";
        Uninstall(bundleName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (uid != Constants::INVALID_UID) {
            APP_LOGI("GetUidByBundleName_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetUidByBundleName_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetUidByBundleName_0300" << std::endl;
}

/**
 * @tc.number: GetUidByBundleName_0400
 * @tc.name: test GetUidByBundleName interface
 * @tc.desc: 1.under '/system/app',there is a hap
 *           2.call GetUidByBundleName
 */
HWTEST_F(ActsBmsKitSystemTest, GetUidByBundleName_0400, Function | MediumTest | Level1)
{
    std::cout << "START GetUidByBundleName_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }

        int userId = Constants::DEFAULT_USERID;
        int uid = bundleMgrProxy->GetUidByBundleName(SYSTEM_SETTINGS_BUNDLE_NAME, userId);
        EXPECT_GE(uid, Constants::BASE_SYS_UID);
        EXPECT_LE(uid, Constants::MAX_SYS_UID);
        if (uid == Constants::INVALID_UID) {
            APP_LOGI("GetUidByBundleName_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetUidByBundleName_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetUidByBundleName_0400" << std::endl;
}

/**
 * @tc.number: GetBundleNameForUid_0100
 * @tc.name: test query bundlenames
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query bundlename by uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleNameForUid_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleNameForUid_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(hapFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        BundleInfo bundleInfo;
        bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        int uid = bundleInfo.uid;

        std::string bundleName;
        bool getInfoResult = bundleMgrProxy->GetBundleNameForUid(uid, bundleName);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(bundleName, appName);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!getInfoResult) {
            APP_LOGI("GetBundleNameForUid_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleNameForUid_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleNameForUid_0100" << std::endl;
}

/**
 * @tc.number: GetBundleNameForUid_0200
 * @tc.name: test query bundlenames
 * @tc.desc: 1.query bundlename by uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleNameForUid_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleNameForUid_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }

        BundleInfo bundleInfo;
        bundleMgrProxy->GetBundleInfo(SYSTEM_SETTINGS_BUNDLE_NAME, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        int uid = bundleInfo.uid;

        std::string bundleName;
        bool getInfoResult = bundleMgrProxy->GetBundleNameForUid(uid, bundleName);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(bundleName, SYSTEM_SETTINGS_BUNDLE_NAME);

        if (!getInfoResult) {
            APP_LOGI("GetBundleNameForUid_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleNameForUid_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleNameForUid_0200" << std::endl;
}

/**
 * @tc.number: GetBundleNameForUid_0300
 * @tc.name: test query bundlenames
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.query bundlenames by wrong uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleNameForUid_0300, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleNameForUid_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        int uid = Constants::INVALID_UID;
        std::string bundleName;
        bool getInfoResult = bundleMgrProxy->GetBundleNameForUid(uid, bundleName);
        EXPECT_FALSE(getInfoResult);

        if (getInfoResult) {
            APP_LOGI("GetBundleNameForUid_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleNameForUid_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleNameForUid_0300" << std::endl;
}

/**
 * @tc.number: GetAppType_0100
 * @tc.name: test GetAppType interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call GetAppType
 */
HWTEST_F(ActsBmsKitSystemTest, GetAppType_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetAppType_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string appType = bundleMgrProxy->GetAppType(appName);
        EXPECT_EQ(appType, "third-party");
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (std::strcmp(appType.c_str(), "third-party") != 0) {
            APP_LOGI("GetAppType_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAppType_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAppType_0100" << std::endl;
}

/**
 * @tc.number: GetAppType_0200
 * @tc.name: test GetAppType interface
 * @tc.desc: 1.under '/system/app/',there is a hap
 *           2.install the hap
 *           3.call GetAppType
 */
HWTEST_F(ActsBmsKitSystemTest, GetAppType_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetAppType_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string appType = bundleMgrProxy->GetAppType(SYSTEM_SETTINGS_BUNDLE_NAME);
        EXPECT_EQ(appType, "system");

        if (std::strcmp(appType.c_str(), "system") != 0) {
            APP_LOGI("GetAppType_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAppType_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAppType_0200" << std::endl;
}

/**
 * @tc.number: GetAppType_0300
 * @tc.name: test GetAppType interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call GetAppType by wrong appName
 */
HWTEST_F(ActsBmsKitSystemTest, GetAppType_0300, Function | MediumTest | Level2)
{
    std::cout << "START GetAppType_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string errName = BASE_BUNDLE_NAME + "e";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string appType = bundleMgrProxy->GetAppType(errName);
        EXPECT_EQ(appType, Constants::EMPTY_STRING);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (std::strcmp(appType.c_str(), (Constants::EMPTY_STRING).c_str()) != 0) {
            APP_LOGI("GetAppType_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAppType_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAppType_0300" << std::endl;
}

/**
 * @tc.number: GetAppType_0400
 * @tc.name: test GetAppType interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap without config.json
 *           2.install the hap
 *           3.call GetAppType
 */
HWTEST_F(ActsBmsKitSystemTest, GetAppType_0400, Function | MediumTest | Level2)
{
    std::cout << "START GetAppType_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle27.hap";
        std::string appName = BASE_BUNDLE_NAME + "4";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(installResult, "Success");
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string appType = bundleMgrProxy->GetAppType(appName);
        EXPECT_EQ(appType, Constants::EMPTY_STRING);

        if (std::strcmp(appType.c_str(), (Constants::EMPTY_STRING).c_str()) != 0) {
            APP_LOGI("GetAppType_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAppType_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAppType_0400" << std::endl;
}

/**
 * @tc.number: GetAppType_0500
 * @tc.name: test GetAppType interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap with invalid suffix
 *           2.install the hap
 *           3.call GetAppType
 */
HWTEST_F(ActsBmsKitSystemTest, GetAppType_0500, Function | MediumTest | Level2)
{
    std::cout << "START GetAppType_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle28.rpk";
        std::string appName = BASE_BUNDLE_NAME + "5";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(installResult, "Success");
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string appType = bundleMgrProxy->GetAppType(appName);
        EXPECT_EQ(appType, Constants::EMPTY_STRING);

        if (std::strcmp(appType.c_str(), (Constants::EMPTY_STRING).c_str()) != 0) {
            APP_LOGI("GetAppType_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAppType_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAppType_0500" << std::endl;
}

/**
 * @tc.number: GetAbilityLabel_0100
 * @tc.name: test GetAbilityLabel interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call GetAbilityLabel
 */
HWTEST_F(ActsBmsKitSystemTest, GetAbilityLabel_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetAbilityLabel_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string abilityLabel = bundleMgrProxy->GetAbilityLabel(appName, abilityName);
        EXPECT_NE(abilityLabel, "EMPTY_STRING");
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (std::strcmp(abilityLabel.c_str(), "EMPTY_STRING") == 0) {
            APP_LOGI("GetAbilityLabel_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAbilityLabel_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAbilityLabel_0100" << std::endl;
}

/**
 * @tc.number: GetAbilityLabel_0200
 * @tc.name: test GetAbilityLabel interface
 * @tc.desc: 1.under '/system/app',there is a hap
 *           2.install the hap
 *           3.call GetAbilityLabel
 */
HWTEST_F(ActsBmsKitSystemTest, GetAbilityLabel_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetAbilityLabel_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string appName = SYSTEM_SETTINGS_BUNDLE_NAME;
        std::string abilityName = "com.ohos.settings.MainAbility";

        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string abilityLabel = bundleMgrProxy->GetAbilityLabel(appName, abilityName);
        EXPECT_NE(abilityLabel, "EMPTY_STRING");

        if (std::strcmp(abilityLabel.c_str(), "EMPTY_STRING") == 0) {
            APP_LOGI("GetAbilityLabel_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAbilityLabel_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAbilityLabel_0200" << std::endl;
}

/**
 * @tc.number: GetAbilityLabel_0300
 * @tc.name: test GetAbilityLabel interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call GetAbilityLabel with wrong appName
 */
HWTEST_F(ActsBmsKitSystemTest, GetAbilityLabel_0300, Function | MediumTest | Level1)
{
    std::cout << "START GetAbilityLabel_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string errAppName = BASE_BUNDLE_NAME + "e";
        std::string abilityLabel = bundleMgrProxy->GetAbilityLabel(errAppName, abilityName);
        EXPECT_EQ(abilityLabel, Constants::EMPTY_STRING);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (std::strcmp(abilityLabel.c_str(), (Constants::EMPTY_STRING).c_str()) != 0) {
            APP_LOGI("GetAbilityLabel_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAbilityLabel_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAbilityLabel_0300" << std::endl;
}

/**
 * @tc.number: GetAbilityLabel_0400
 * @tc.name: test GetAbilityLabel interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call GetAbilityLabel with wrong abilityname
 */
HWTEST_F(ActsBmsKitSystemTest, GetAbilityLabel_0400, Function | MediumTest | Level1)
{
    std::cout << "START GetAbilityLabel_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string errAbilityName = "MainAbility";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string abilityLabel = bundleMgrProxy->GetAbilityLabel(appName, errAbilityName);
        EXPECT_EQ(abilityLabel, Constants::EMPTY_STRING);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (std::strcmp(abilityLabel.c_str(), (Constants::EMPTY_STRING).c_str()) != 0) {
            APP_LOGI("GetAbilityLabel_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAbilityLabel_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAbilityLabel_0400" << std::endl;
}

/**
 * @tc.number: GetAbilityLabel_0500
 * @tc.name: test GetAbilityLabel interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap without config.json
 *           2.install the hap
 *           3.call GetAbilityLabel
 */
HWTEST_F(ActsBmsKitSystemTest, GetAbilityLabel_0500, Function | MediumTest | Level2)
{
    std::cout << "START GetAbilityLabel_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle27.hap";
        std::string appName = BASE_BUNDLE_NAME + "4";
        std::string abilityName = "MainAbility";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(installResult, "Success");
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string abilityLabel = bundleMgrProxy->GetAbilityLabel(appName, abilityName);
        EXPECT_EQ(abilityLabel, Constants::EMPTY_STRING);

        if (std::strcmp(abilityLabel.c_str(), (Constants::EMPTY_STRING).c_str()) != 0) {
            APP_LOGI("GetAbilityLabel_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAbilityLabel_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAbilityLabel_0500" << std::endl;
}

/**
 * @tc.number: GetAbilityLabel_0600
 * @tc.name: test GetAbilityLabel interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap with invalid suffix
 *           2.install the hap
 *           3.call GetAbilityLabel
 */
HWTEST_F(ActsBmsKitSystemTest, GetAbilityLabel_0600, Function | MediumTest | Level2)
{
    std::cout << "START GetAbilityLabel_0600" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle28.rpk";
        std::string appName = BASE_BUNDLE_NAME + "5";
        std::string abilityName = "MainAbility";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(installResult, "Success");
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string abilityLabel = bundleMgrProxy->GetAbilityLabel(appName, abilityName);
        EXPECT_EQ(abilityLabel, Constants::EMPTY_STRING);

        if (std::strcmp(abilityLabel.c_str(), (Constants::EMPTY_STRING).c_str()) != 0) {
            APP_LOGI("GetAbilityLabel_0600 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetAbilityLabel_0600 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetAbilityLabel_0600" << std::endl;
}

/**
 * @tc.number: GetHapModuleInfo_0100
 * @tc.name: test GetHapModuleInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap with one ability
 *           2.install the hap
 *           3.call GetHapModuleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetHapModuleInfo_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetHapModuleInfo_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        AbilityInfo abilityInfo;
        abilityInfo.bundleName = appName;
        abilityInfo.package = BASE_BUNDLE_NAME + ".h1";
        HapModuleInfo hapModuleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->GetHapModuleInfo(abilityInfo, hapModuleInfo);
        EXPECT_TRUE(queryResult);

        EXPECT_EQ(hapModuleInfo.name, "com.third.hiworld.example.h1");
        EXPECT_EQ(hapModuleInfo.moduleName, "testability");
        EXPECT_EQ(hapModuleInfo.description, "");
        EXPECT_EQ(hapModuleInfo.label, "bmsThirdBundle_A1 Ability");
        EXPECT_EQ(commonTool.VectorToStr(hapModuleInfo.deviceTypes), "tvcar");

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!queryResult) {
            APP_LOGI("GetHapModuleInfo_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetHapModuleInfo_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetHapModuleInfo_0100" << std::endl;
}

/**
 * @tc.number: GetHapModuleInfo_0200
 * @tc.name: test GetHapModuleInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap with two abilities
 *           2.install the hap
 *           3.call GetHapModuleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetHapModuleInfo_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetHapModuleInfo_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        AbilityInfo abilityInfo;
        abilityInfo.bundleName = appName;
        abilityInfo.package = BASE_BUNDLE_NAME + ".h1";
        HapModuleInfo hapModuleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->GetHapModuleInfo(abilityInfo, hapModuleInfo);
        EXPECT_TRUE(queryResult);
        EXPECT_EQ(hapModuleInfo.name, "com.third.hiworld.example.h1");
        EXPECT_EQ(hapModuleInfo.moduleName, "testability");
        EXPECT_EQ(hapModuleInfo.label, "bmsThirdBundle_A1 Ability");
        EXPECT_EQ(commonTool.VectorToStr(hapModuleInfo.deviceTypes), "tvcar");
        bool isSubStrExist = false;
        for (int i = 1; i <= 2; i++) {
            std::string abilityName = "" + std::to_string(i);
            for (auto hapModuleInfo : hapModuleInfo.abilityInfos) {
                if (IsSubStr(hapModuleInfo.name, abilityName)) {
                    isSubStrExist = true;
                    break;
                }
            }
            EXPECT_TRUE(isSubStrExist);
        }
        resvec.clear();
        Uninstall(appName, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";

        if (!queryResult) {
            APP_LOGI("GetHapModuleInfo_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetHapModuleInfo_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetHapModuleInfo_0200" << std::endl;
}

/**
 * @tc.number: GetHapModuleInfo_0300
 * @tc.name: test GetHapModuleInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap without an ability
 *           2.install the hap
 *           3.call GetHapModuleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetHapModuleInfo_0300, Function | MediumTest | Level1)
{
    std::cout << "START GetHapModuleInfo_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        AbilityInfo abilityInfo;
        abilityInfo.bundleName = appName;
        abilityInfo.package = BASE_BUNDLE_NAME + ".h1";
        HapModuleInfo hapModuleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->GetHapModuleInfo(abilityInfo, hapModuleInfo);
        EXPECT_TRUE(queryResult);
        EXPECT_EQ(hapModuleInfo.name, "com.third.hiworld.example.h1");
        EXPECT_EQ(hapModuleInfo.moduleName, "testability");
        resvec.clear();

        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!queryResult) {
            APP_LOGI("GetHapModuleInfo_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetHapModuleInfo_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetHapModuleInfo_0300" << std::endl;
}

/**
 * @tc.number: GetHapModuleInfo_0400
 * @tc.name: test GetHapModuleInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.use error bundleName to get moduleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetHapModuleInfo_0400, Function | MediumTest | Level1)
{
    std::cout << "START GetHapModuleInfo_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
        std::string appName = BASE_BUNDLE_NAME + "2";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        AbilityInfo abilityInfo;
        abilityInfo.bundleName = "error_bundleName";
        abilityInfo.package = BASE_BUNDLE_NAME + ".h2";
        HapModuleInfo hapModuleInfo;

        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->GetHapModuleInfo(abilityInfo, hapModuleInfo);
        EXPECT_FALSE(queryResult);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (queryResult) {
            APP_LOGI("GetHapModuleInfo_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetHapModuleInfo_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetHapModuleInfo_0400" << std::endl;
}

/**
 * @tc.number: GetHapModuleInfo_0500
 * @tc.name: test GetHapModuleInfo interface
 * @tc.desc: 1.under '/system/app',there is a hap
 *           2.install the hap
 *           3.call GetHapModuleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetHapModuleInfo_0500, Function | MediumTest | Level1)
{
    std::cout << "START GetHapModuleInfo_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string appName = SYSTEM_SETTINGS_BUNDLE_NAME;
        AbilityInfo abilityInfo;
        abilityInfo.bundleName = appName;
        abilityInfo.package = "com.ohos.settings";
        HapModuleInfo hapModuleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->GetHapModuleInfo(abilityInfo, hapModuleInfo);
        EXPECT_TRUE(queryResult);
        EXPECT_EQ(hapModuleInfo.name, "com.ohos.settings");
        EXPECT_EQ(hapModuleInfo.moduleName, "entry");
        CommonTool commonTool;
        EXPECT_EQ(commonTool.VectorToStr(hapModuleInfo.deviceTypes), "phone");

        if (!queryResult) {
            APP_LOGI("GetHapModuleInfo_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetHapModuleInfo_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetHapModuleInfo_0500" << std::endl;
}

/**
 * @tc.number: GetHapModuleInfo_0600
 * @tc.name: test GetHapModuleInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap without config.json file
 *           2.install the hap
 *           3.call GetHapModuleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetHapModuleInfo_0600, Function | MediumTest | Level2)
{
    std::cout << "START GetHapModuleInfo_0600" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle27.hap";
        std::string appName = BASE_BUNDLE_NAME + "4";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(installResult, "Success");

        AbilityInfo abilityInfo;
        abilityInfo.bundleName = appName;
        abilityInfo.package = BASE_BUNDLE_NAME + ".h1";
        HapModuleInfo hapModuleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->GetHapModuleInfo(abilityInfo, hapModuleInfo);
        EXPECT_FALSE(queryResult);

        if (queryResult) {
            APP_LOGI("GetHapModuleInfo_0600 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetHapModuleInfo_0600 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetHapModuleInfo_0600" << std::endl;
}

/**
 * @tc.number: GetHapModuleInfo_0700
 * @tc.name: test GetHapModuleInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap with invalid suffix
 *           2.install the hap
 *           3.call GetHapModuleInfo
 */
HWTEST_F(ActsBmsKitSystemTest, GetHapModuleInfo_0700, Function | MediumTest | Level2)
{
    std::cout << "START GetHapModuleInfo_0700" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle28.rpk";
        std::string appName = BASE_BUNDLE_NAME + "5";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(installResult, "Success") << "install fail!";

        AbilityInfo abilityInfo;
        abilityInfo.bundleName = appName;
        abilityInfo.package = BASE_BUNDLE_NAME + ".h2";
        HapModuleInfo hapModuleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->GetHapModuleInfo(abilityInfo, hapModuleInfo);
        EXPECT_FALSE(queryResult);

        if (queryResult) {
            APP_LOGI("GetHapModuleInfo_0700 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetHapModuleInfo_0700 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetHapModuleInfo_0700" << std::endl;
}

/**
 * @tc.number: Callback_0100
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test UnregisterBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists a normal hap
 *           2.call RegisterBundleStatusCallback
 *           3.install the hap
 *           4.call UnregisterBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0100, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";

        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(bundleStatusCallback, nullptr);
        bundleStatusCallback->SetBundleName(appName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(bundleStatusCallback);
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        bool unRegResult = bundleMgrProxy->UnregisterBundleStatusCallback();
        EXPECT_TRUE(unRegResult);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!unRegResult) {
            APP_LOGI("Callback_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0100" << std::endl;
}

/**
 * @tc.number: Callback_0200
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test UnregisterBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal hap
 *           2.call RegisterBundleStatusCallback
 *           3.install the hap
 *           4.call UnregisterBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0200, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle11.hap";
        std::string appName = BASE_BUNDLE_NAME + "11";

        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(bundleStatusCallback, nullptr);
        bundleStatusCallback->SetBundleName(appName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(bundleStatusCallback);
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(installResult, "Success");
        bool unRegResult = bundleMgrProxy->UnregisterBundleStatusCallback();
        EXPECT_TRUE(unRegResult);

        if (!unRegResult) {
            APP_LOGI("Callback_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0200" << std::endl;
}

/**
 * @tc.number: Callback_0300
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test UnregisterBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists two bundles,one's version is
 *                    higher than the other
 *           2.call RegisterBundleStatusCallback
 *           3.install the hap
 *           4.upgrade the hap
 *           5.call UnregisterBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0300, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string firstFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
        std::string appName = BASE_BUNDLE_NAME + "2";
        std::string secondFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";

        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(bundleStatusCallback, nullptr);
        bundleStatusCallback->SetBundleName(appName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(bundleStatusCallback);
        Install(firstFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        resvec.clear();
        Install(secondFilePath, InstallFlag::REPLACE_EXISTING, resvec);
        std::string upgradeResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(upgradeResult, "Success") << "upgrade fail!";
        bool unRegResult = bundleMgrProxy->UnregisterBundleStatusCallback();
        EXPECT_TRUE(unRegResult);

        std::vector<std::string> resvec2;
        Uninstall(appName, resvec2);
        std::string uninstallResult = commonTool.VectorToStr(resvec2);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!unRegResult) {
            APP_LOGI("Callback_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0300" << std::endl;
}

/**
 * @tc.number: Callback_0400
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test UnregisterBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists two bundles,one's version is
 *                    equal than the other
 *           2.call RegisterBundleStatusCallback
 *           3.install the hap
 *           4.upgrade the hap
 *           5.call UnregisterBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0400, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string firstFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
        std::string appName = BASE_BUNDLE_NAME + "2";
        std::string secondFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle10.hap";

        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(bundleStatusCallback, nullptr);
        bundleStatusCallback->SetBundleName(appName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(bundleStatusCallback);
        Install(firstFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        resvec.clear();
        Install(secondFilePath, InstallFlag::REPLACE_EXISTING, resvec);
        std::string upgradeResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(upgradeResult, "Success") << "upgrade fail!";
        bool unRegResult = bundleMgrProxy->UnregisterBundleStatusCallback();
        EXPECT_TRUE(unRegResult);

        std::vector<std::string> resvec2;
        Uninstall(appName, resvec2);
        std::string uninstallResult = commonTool.VectorToStr(resvec2);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!unRegResult) {
            APP_LOGI("Callback_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0400" << std::endl;
}

/**
 * @tc.number: Callback_0500
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test UnregisterBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists two bundles,one's version is
 *                    lower than the other
 *           2.call RegisterBundleStatusCallback
 *           3.install the hap
 *           4.upgrade the hap
 *           5.call UnregisterBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0500, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string firstFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";
        std::string appName = BASE_BUNDLE_NAME + "2";
        std::string secondFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle10.hap";

        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(bundleStatusCallback, nullptr);
        bundleStatusCallback->SetBundleName(appName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(bundleStatusCallback);
        Install(firstFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        resvec.clear();
        Install(secondFilePath, InstallFlag::REPLACE_EXISTING, resvec);
        std::string upgradeResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(upgradeResult, "Success") << "upgrade success!";
        bool unRegResult = bundleMgrProxy->UnregisterBundleStatusCallback();
        EXPECT_TRUE(unRegResult);

        std::vector<std::string> resvec2;
        Uninstall(appName, resvec2);
        std::string uninstallResult = commonTool.VectorToStr(resvec2);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!unRegResult) {
            APP_LOGI("Callback_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0500" << std::endl;
}

/**
 * @tc.number: Callback_0600
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test ClearBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.call RegisterBundleStatusCallback
 *           3.install the hap
 *           4.call ClearBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0600, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0600" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string filePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";

        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(bundleStatusCallback, nullptr);
        bundleStatusCallback->SetBundleName(appName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(bundleStatusCallback);
        Install(filePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        bool clearResult = bundleMgrProxy->ClearBundleStatusCallback(bundleStatusCallback);
        EXPECT_TRUE(clearResult);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!clearResult) {
            APP_LOGI("Callback_0600 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0600 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0600" << std::endl;
}

/**
 * @tc.number: Callback_0700
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test ClearBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there 1exists two bundles
 *           2.call RegisterBundleStatusCallback
 *           3.install the first hap
 *           4.call RegisterBundleStatusCallback
 *           5.install the second hap
 *           6.call ClearBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0700, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0700" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string firstFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string firstAppName = BASE_BUNDLE_NAME + "1";
        std::string secondFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
        std::string secondAppName = BASE_BUNDLE_NAME + "2";
        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> firstBundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(firstBundleStatusCallback, nullptr);
        firstBundleStatusCallback->SetBundleName(firstAppName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(firstBundleStatusCallback);
        Install(firstFilePath, InstallFlag::NORMAL, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        resvec.clear();
        sptr<BundleStatusCallbackImpl> secondBundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(secondBundleStatusCallback, nullptr);
        secondBundleStatusCallback->SetBundleName(secondAppName);
        bundleMgrProxy->RegisterBundleStatusCallback(secondBundleStatusCallback);
        Install(secondFilePath, InstallFlag::NORMAL, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        bool clearResult = bundleMgrProxy->ClearBundleStatusCallback(firstBundleStatusCallback);
        EXPECT_TRUE(clearResult);
        std::vector<std::string> resvec2;
        Uninstall(firstAppName, resvec2);
        EXPECT_EQ(commonTool.VectorToStr(resvec2), "Success") << "uninstall fail!";
        resvec2.clear();
        Uninstall(secondAppName, resvec2);
        EXPECT_EQ(commonTool.VectorToStr(resvec2), "Success") << "uninstall fail!";
        if (!clearResult) {
            APP_LOGI("Callback_0700 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0700 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0700" << std::endl;
}

/**
 * @tc.number: Callback_0800
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test ClearBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists two bundles
 *           2.call RegisterBundleStatusCallback
 *           3.install the first hap
 *           4.call RegisterBundleStatusCallback
 *           5.install the second hap
 *           6.call ClearBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0800, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0800" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string firstFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string firstAppName = BASE_BUNDLE_NAME + "1";
        std::string secondFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
        std::string secondAppName = BASE_BUNDLE_NAME + "2";
        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> firstBundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(firstBundleStatusCallback, nullptr);
        firstBundleStatusCallback->SetBundleName(firstAppName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(firstBundleStatusCallback);
        Install(firstFilePath, InstallFlag::NORMAL, resvec);
        std::string firstinstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(firstinstallResult, "Success") << "install fail!";
        resvec.clear();
        sptr<BundleStatusCallbackImpl> secondBundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(secondBundleStatusCallback, nullptr);
        secondBundleStatusCallback->SetBundleName(secondAppName);
        bundleMgrProxy->RegisterBundleStatusCallback(secondBundleStatusCallback);
        Install(secondFilePath, InstallFlag::NORMAL, resvec);
        std::string secondinstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(secondinstallResult, "Success") << "install fail!";
        bool clearResult = bundleMgrProxy->ClearBundleStatusCallback(secondBundleStatusCallback);
        EXPECT_TRUE(clearResult);
        std::vector<std::string> resvec2;
        Uninstall(firstAppName, resvec2);
        EXPECT_EQ(commonTool.VectorToStr(resvec2), "Success") << "uninstall fail!";
        resvec2.clear();
        Uninstall(secondAppName, resvec2);
        EXPECT_EQ(commonTool.VectorToStr(resvec2), "Success") << "uninstall fail!";
        if (!clearResult) {
            APP_LOGI("Callback_0800 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0800 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0800" << std::endl;
}

/**
 * @tc.number: Callback_0900
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test ClearBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists two bundles
 *           2.call RegisterBundleStatusCallback
 *           3.install the first hap
 *           4.call RegisterBundleStatusCallback
 *           5.install the second hap
 *           6.call ClearBundleStatusCallback
 *           7.call ClearBundleStatusCallback
 */
HWTEST_F(ActsBmsKitSystemTest, Callback_0900, Function | MediumTest | Level1)
{
    std::cout << "START Callback_0900" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string firstFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string firstAppName = BASE_BUNDLE_NAME + "1";
        std::string secondFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
        std::string secondAppName = BASE_BUNDLE_NAME + "2";
        CommonTool commonTool;
        sptr<BundleStatusCallbackImpl> firstBundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(firstBundleStatusCallback, nullptr);
        firstBundleStatusCallback->SetBundleName(firstAppName);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->RegisterBundleStatusCallback(firstBundleStatusCallback);
        Install(firstFilePath, InstallFlag::NORMAL, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        resvec.clear();
        sptr<BundleStatusCallbackImpl> secondBundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
        EXPECT_NE(secondBundleStatusCallback, nullptr);
        secondBundleStatusCallback->SetBundleName(secondAppName);
        bundleMgrProxy->RegisterBundleStatusCallback(secondBundleStatusCallback);
        Install(secondFilePath, InstallFlag::NORMAL, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        bool clearResult1 = bundleMgrProxy->ClearBundleStatusCallback(firstBundleStatusCallback);
        EXPECT_TRUE(clearResult1);
        bool clearResult2 = bundleMgrProxy->ClearBundleStatusCallback(secondBundleStatusCallback);
        EXPECT_TRUE(clearResult2);
        std::vector<std::string> resvec2;
        Uninstall(firstAppName, resvec2);
        EXPECT_EQ(commonTool.VectorToStr(resvec2), "Success") << "uninstall fail!";
        resvec2.clear();
        Uninstall(secondAppName, resvec2);
        EXPECT_EQ(commonTool.VectorToStr(resvec2), "Success") << "uninstall fail!";
        if (!clearResult1 && !clearResult2) {
            APP_LOGI("Callback_0900 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Callback_0900 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END Callback_0900" << std::endl;
}

/**
 * @tc.number: CleanBundleCacheFiles_0100
 * @tc.name: test CleanBundleCacheFiles interface
 * @tc.desc: 1.install a third-party bundle
 *           2.start this hap
 *           3.call CleanBundleCacheFiles
 *           4.check if the files exist in the cache directory
 */
HWTEST_F(ActsBmsKitSystemTest, CleanBundleCacheFiles_0100, Function | MediumTest | Level1)
{
    std::cout << "START CleanBundleCacheFiles_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle24.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";

        CommonTool commonTool;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        std::this_thread::sleep_for(3000ms);
        system("aa start -a PageAbilityDemo -b com.third.hiworld.example1");
        std::this_thread::sleep_for(3000ms);
        const std::string testCacheFileNamE1 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/name1.txt";
        const std::string testCacheFileNamE2 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/name2.txt";
        CreateDir(testCacheFileNamE1);
        CreateDir(testCacheFileNamE2);
        int name1Exist = access(testCacheFileNamE1.c_str(), F_OK);
        EXPECT_EQ(name1Exist, 0);
        int name2Exist = access(testCacheFileNamE2.c_str(), F_OK);
        EXPECT_EQ(name2Exist, 0);

        sptr<CleanCacheCallBackImpl> bundleCleanCacheCallback = (new (std::nothrow) CleanCacheCallBackImpl());
        EXPECT_NE(bundleCleanCacheCallback, nullptr);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->CleanBundleCacheFiles(appName, bundleCleanCacheCallback);
        bool cleanCacheResult = bundleCleanCacheCallback->GetSucceededResult();
        EXPECT_TRUE(cleanCacheResult);
        name1Exist = access(testCacheFileNamE1.c_str(), F_OK);
        EXPECT_NE(name1Exist, 0) << "the cache test file1 exists.";
        name2Exist = access(testCacheFileNamE2.c_str(), F_OK);
        EXPECT_NE(name2Exist, 0) << "the cache test file2 exists.";

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!cleanCacheResult) {
            APP_LOGI("CleanBundleCacheFiles_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("CleanBundleCacheFiles_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END CleanBundleCacheFiles_0100" << std::endl;
}

/**
 * @tc.number: CleanBundleCacheFiles_0200
 * @tc.name: test CleanBundleCacheFiles interface
 * @tc.desc: 1.install a third-party bundle
 *           2.start this hap
 *           3.call CleanBundleCacheFiles
 *           4.check if the files exist in the cache directory
 */
HWTEST_F(ActsBmsKitSystemTest, CleanBundleCacheFiles_0200, Function | MediumTest | Level1)
{
    std::cout << "START CleanBundleCacheFiles_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle25.hap";
        std::string appName = BASE_BUNDLE_NAME + "2";

        CommonTool commonTool;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        std::this_thread::sleep_for(3000ms);
        system("aa start -a PageAbilityDemo -b com.third.hiworld.example2");
        std::this_thread::sleep_for(3000ms);
        const std::string testCacheDiR1 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/testDir1";
        const std::string testCacheDiR2 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/testDir2";
        CreateDir(testCacheDiR1);
        CreateDir(testCacheDiR2);
        int name1Exist = access(testCacheDiR1.c_str(), F_OK);
        EXPECT_EQ(name1Exist, 0);
        int name2Exist = access(testCacheDiR2.c_str(), F_OK);
        EXPECT_EQ(name2Exist, 0);
        sptr<CleanCacheCallBackImpl> bundleCleanCacheCallback = (new (std::nothrow) CleanCacheCallBackImpl());
        EXPECT_NE(bundleCleanCacheCallback, nullptr);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->CleanBundleCacheFiles(appName, bundleCleanCacheCallback);
        bool cleanCacheResult = bundleCleanCacheCallback->GetSucceededResult();
        EXPECT_TRUE(cleanCacheResult);
        name1Exist = access(testCacheDiR1.c_str(), F_OK);
        EXPECT_NE(name1Exist, 0) << "the cache test dir1 exists.";
        name2Exist = access(testCacheDiR2.c_str(), F_OK);
        EXPECT_NE(name2Exist, 0) << "the cache test dir2 exists.";

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!cleanCacheResult) {
            APP_LOGI("CleanBundleCacheFiles_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("CleanBundleCacheFiles_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END CleanBundleCacheFiles_0200" << std::endl;
}

/**
 * @tc.number: CleanBundleCacheFiles_0300
 * @tc.name: test CleanBundleCacheFiles interface
 * @tc.desc: 1.install a third-party bundle
 *           2.start this hap
 *           3.call CleanBundleCacheFiles
 *           4.check if the files exist in the cache directory
 */
HWTEST_F(ActsBmsKitSystemTest, CleanBundleCacheFiles_0300, Function | MediumTest | Level1)
{
    std::cout << "START CleanBundleCacheFiles_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle44.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";

        CommonTool commonTool;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        std::this_thread::sleep_for(3000ms);
        system("aa start -a PageAbilityDemo -b com.third.hiworld.example1");
        std::this_thread::sleep_for(3000ms);
        const std::string testCacheFileName = BUNDLE_DATA_ROOT_PATH + appName + "/cache/dir1/dir2/dir3/dir4/dir5/dir6";
        const std::string testCacheDir = BUNDLE_DATA_ROOT_PATH + appName + "/cache/dir1";
        CreateDir(testCacheDir);
        int isExist = access(testCacheDir.c_str(), F_OK);
        EXPECT_EQ(isExist, 0);
        sptr<CleanCacheCallBackImpl> bundleCleanCacheCallback = (new (std::nothrow) CleanCacheCallBackImpl());
        EXPECT_NE(bundleCleanCacheCallback, nullptr);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->CleanBundleCacheFiles(appName, bundleCleanCacheCallback);
        bool cleanCacheResult = bundleCleanCacheCallback->GetSucceededResult();
        EXPECT_TRUE(cleanCacheResult);
        isExist = access(testCacheDir.c_str(), F_OK);
        EXPECT_NE(isExist, 0) << "the cache test dir exists.";

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!cleanCacheResult) {
            APP_LOGI("CleanBundleCacheFiles_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("CleanBundleCacheFiles_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END CleanBundleCacheFiles_0300" << std::endl;
}

/**
 * @tc.number: CleanBundleCacheFiles_0400
 * @tc.name: test CleanBundleCacheFiles interface
 * @tc.desc: 1.install a third-party bundle
 *           2.start this hap
 *           3.call CleanBundleCacheFiles
 *           4.check if the files exist in the cache directory
 */
HWTEST_F(ActsBmsKitSystemTest, CleanBundleCacheFiles_0400, Function | MediumTest | Level1)
{
    std::cout << "START CleanBundleCacheFiles_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle45.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        CommonTool commonTool;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        std::this_thread::sleep_for(3000ms);
        system("aa start -a PageAbilityDemo -b com.third.hiworld.example1");
        std::this_thread::sleep_for(3000ms);
        const std::string testCacheFileNamE1 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/testDir1/name1.txt";
        const std::string testCacheDir1 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/testDir1";
        const std::string testCacheFileNamE2 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/testDir2/name2.txt";
        const std::string testCacheDir2 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/testDir2";
        CreateDir(testCacheDir1);
        CreateDir(testCacheDir2);
        int name1Exist = access(testCacheDir1.c_str(), F_OK);
        EXPECT_EQ(name1Exist, 0);
        int name2Exist = access(testCacheDir2.c_str(), F_OK);
        EXPECT_EQ(name2Exist, 0);
        sptr<CleanCacheCallBackImpl> bundleCleanCacheCallback = (new (std::nothrow) CleanCacheCallBackImpl());
        EXPECT_NE(bundleCleanCacheCallback, nullptr);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bundleMgrProxy->CleanBundleCacheFiles(appName, bundleCleanCacheCallback);
        bool cleanCacheResult = bundleCleanCacheCallback->GetSucceededResult();
        EXPECT_TRUE(cleanCacheResult);
        name1Exist = access(testCacheDir1.c_str(), F_OK);
        EXPECT_NE(name1Exist, 0) << "the cache test dir1 exists.";
        name2Exist = access(testCacheDir2.c_str(), F_OK);
        EXPECT_NE(name2Exist, 0) << "the cache test dir2 exists.";
        resvec.clear();
        Uninstall(appName, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
        if (!cleanCacheResult) {
            APP_LOGI("CleanBundleCacheFiles_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("CleanBundleCacheFiles_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END CleanBundleCacheFiles_0400" << std::endl;
}

/**
 * @tc.number: CleanBundleCacheFiles_0500
 * @tc.name: test CleanBundleCacheFiles interface
 * @tc.desc: 1.install an abnormal third-party bundle without config.json
 *           2.call CleanBundleCacheFiles
 */
HWTEST_F(ActsBmsKitSystemTest, CleanBundleCacheFiles_0500, Function | MediumTest | Level1)
{
    std::cout << "START CleanBundleCacheFiles_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle27.hap";
        std::string appName = BASE_BUNDLE_NAME + "4";
        CommonTool commonTool;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_PARSE_NO_PROFILE]");
        sptr<CleanCacheCallBackImpl> bundleCleanCacheCallback = (new (std::nothrow) CleanCacheCallBackImpl());
        EXPECT_NE(bundleCleanCacheCallback, nullptr);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool cleanCacheResult = bundleMgrProxy->CleanBundleCacheFiles(appName, bundleCleanCacheCallback);
        EXPECT_FALSE(cleanCacheResult);

        if (cleanCacheResult) {
            APP_LOGI("CleanBundleCacheFiles_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("CleanBundleCacheFiles_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END CleanBundleCacheFiles_0500" << std::endl;
}

/**
 * @tc.number: CleanBundleCacheFiles_0600
 * @tc.name: test CleanBundleCacheFiles interface
 * @tc.desc: 1.install an abnormal third-party bundle with invalid suffix
 *           2.call CleanBundleCacheFiles
 */
HWTEST_F(ActsBmsKitSystemTest, CleanBundleCacheFiles_0600, Function | MediumTest | Level1)
{
    std::cout << "START CleanBundleCacheFiles_0600" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle28.rpk";
        std::string appName = BASE_BUNDLE_NAME + "5";

        CommonTool commonTool;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_NE(installResult, "Success");

        sptr<CleanCacheCallBackImpl> bundleCleanCacheCallback = (new (std::nothrow) CleanCacheCallBackImpl());
        EXPECT_NE(bundleCleanCacheCallback, nullptr);
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool cleanCacheResult = bundleMgrProxy->CleanBundleCacheFiles(appName, bundleCleanCacheCallback);
        EXPECT_FALSE(cleanCacheResult);

        if (cleanCacheResult) {
            APP_LOGI("CleanBundleCacheFiles_0600 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("CleanBundleCacheFiles_0600 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END CleanBundleCacheFiles_0600" << std::endl;
}

/**
 * @tc.number: GetLaunchWantForBundle_0100
 * @tc.name: test GetLaunchWantForBundle interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call GetLaunchWantForBundle
 */
HWTEST_F(ActsBmsKitSystemTest, GetLaunchWantForBundle_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetLaunchWantForBundle_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        Want want;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool launchWantResult = bundleMgrProxy->GetLaunchWantForBundle(appName, want);
        EXPECT_TRUE(launchWantResult);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!launchWantResult) {
            APP_LOGI("GetLaunchWantForBundle_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetLaunchWantForBundle_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetLaunchWantForBundle_0100" << std::endl;
}

/**
 * @tc.number: GetLaunchWantForBundle_0200
 * @tc.name: test GetLaunchWantForBundle interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap without a main ability
 *           2.install the hap
 *           3.call GetLaunchWantForBundle
 */
HWTEST_F(ActsBmsKitSystemTest, GetLaunchWantForBundle_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetLaunchWantForBundle_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        Want want;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool launchWantResult = bundleMgrProxy->GetLaunchWantForBundle(appName, want);
        EXPECT_FALSE(launchWantResult);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (launchWantResult) {
            APP_LOGI("GetLaunchWantForBundle_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetLaunchWantForBundle_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetLaunchWantForBundle_0200" << std::endl;
}

/**
 * @tc.number: QueryAbilityInfo_0100
 * @tc.name: test QueryAbilityInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call QueryAbilityInfo
 */
HWTEST_F(ActsBmsKitSystemTest, QueryAbilityInfo_0100, Function | MediumTest | Level1)
{
    std::cout << "START QueryAbilityInfo_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        Want want;
        ElementName name;
        name.SetAbilityName(abilityName);
        name.SetBundleName(appName);
        want.SetElement(name);

        AbilityInfo abilityInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
        EXPECT_TRUE(queryResult);
        EXPECT_EQ(abilityInfo.name, abilityName);
        EXPECT_EQ(abilityInfo.bundleName, appName);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!queryResult) {
            APP_LOGI("QueryAbilityInfo_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("QueryAbilityInfo_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END QueryAbilityInfo_0100" << std::endl;
}

/**
 * @tc.number: QueryAbilityInfo_0200
 * @tc.name: QueryAbilityInfo
 * @tc.desc: query data then verify
 */
HWTEST_F(ActsBmsKitSystemTest, QueryAbilityInfo_0200, Function | MediumTest | Level0)
{
    std::cout << "START QueryAbilityInfo_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        Want want;
        ElementName name;
        name.SetBundleName("xxx");
        want.SetElement(name);

        AbilityInfo abilityInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
        EXPECT_FALSE(queryResult);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (queryResult) {
            APP_LOGI("QueryAbilityInfo_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("QueryAbilityInfo_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END QueryAbilityInfo_0200" << std::endl;
}

/**
 * @tc.number: QueryAbilityInfo_0300
 * @tc.name: test QueryAbilityInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call QueryAbilityInfo
 */
HWTEST_F(ActsBmsKitSystemTest, QueryAbilityInfo_0300, Function | MediumTest | Level1)
{
    std::cout << "START QueryAbilityInfo_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string abilityName = "bmsThirdBundle_A1";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        Want want;
        ElementName name;
        name.SetAbilityName(abilityName);
        name.SetBundleName(appName);
        want.SetElement(name);

        AbilityInfo abilityInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
        EXPECT_TRUE(queryResult);
        EXPECT_EQ(abilityInfo.targetAbility, "");

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (!queryResult) {
            APP_LOGI("QueryAbilityInfo_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("QueryAbilityInfo_0300 succecc - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END QueryAbilityInfo_0300" << std::endl;
}

/**
 * @tc.number: GetBundleInfosByMetaData_0100
 * @tc.name: test GetBundleInfosByMetaData interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call GetBundleInfosByMetaData
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfosByMetaData_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfosByMetaData_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle17.hap";
        std::string appName = BASE_BUNDLE_NAME + "6";

        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        std::vector<BundleInfo> bundleInfos;

        std::string metadata = "string";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getResult = bundleMgrProxy->GetBundleInfosByMetaData(metadata, bundleInfos);
        EXPECT_TRUE(getResult);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!getResult) {
            APP_LOGI("GetBundleInfosByMetaData_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfosByMetaData_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfosByMetaData_0100" << std::endl;
}

/**
 * @tc.number: GetBundleInfosByMetaData_0200
 * @tc.name: test GetBundleInfosByMetaData interface
 * @tc.desc: 1.call GetBundleInfosByMetaData
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundleInfosByMetaData_0200, Function | MediumTest | Level1)
{
    std::cout << "START GetBundleInfosByMetaData_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<BundleInfo> bundleInfos;
        std::string metadata = "not_exist";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getResult = bundleMgrProxy->GetBundleInfosByMetaData(metadata, bundleInfos);
        EXPECT_FALSE(getResult);
        if (getResult) {
            APP_LOGI("GetBundleInfosByMetaData_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundleInfosByMetaData_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundleInfosByMetaData_0200" << std::endl;
}

/**
 * @tc.number: AbilityDump_0100
 * @tc.name: Dump
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists a hap
 *           2.install the hap
 *           3.call "QueryAbilityInfo" kit
 *           4.Dump abilityInfo
 */
HWTEST_F(ActsBmsKitSystemTest, AbilityDump_0100, Function | MediumTest | Level0)
{
    std::cout << "START AbilityDump_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        Want want;
        ElementName name;
        name.SetAbilityName(abilityName);
        name.SetBundleName(appName);
        want.SetElement(name);

        AbilityInfo abilityInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool queryResult = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
        EXPECT_EQ(abilityInfo.name, abilityName);
        EXPECT_TRUE(queryResult);
        std::string path = "/data/test/abilityInfo.txt";
        std::ofstream file(path);
        file.close();
        int fd = open(path.c_str(), O_WRONLY | O_CLOEXEC);
        EXPECT_NE(fd, -1) << "open file error";
        std::string prefix = "[ability]";
        abilityInfo.Dump(prefix, fd);
        long length = lseek(fd, 0, SEEK_END);
        EXPECT_GT(length, 0);
        close(fd);
        resvec.clear();
        Uninstall(appName, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
        if (!queryResult) {
            APP_LOGI("AbilityDump_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("AbilityDump_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END AbilityDump_0100" << std::endl;
}

/**
 * @tc.number: ApplicationInfoDump_0100
 * @tc.name: Dump
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists a hap
 *           2.install the hap
 *           3.call "GetApplicationInfo" kit
 *           4.Dump appInfo
 */
HWTEST_F(ActsBmsKitSystemTest, ApplicationInfoDump_0100, Function | MediumTest | Level1)
{
    std::cout << "START ApplicationInfoDump_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        ApplicationInfo appInfo;
        int userId = Constants::DEFAULT_USERID;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfo(appName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(appInfo.name, appName);

        std::string path = "/data/test/appInfo.txt";
        std::ofstream file(path);
        file.close();
        int fd = open(path.c_str(), O_WRONLY | O_CLOEXEC);
        EXPECT_NE(fd, -1) << "open file error";
        std::string prefix = "[appInfo]";
        appInfo.Dump(prefix, fd);
        long length = lseek(fd, 0, SEEK_END);
        EXPECT_GT(length, 0);
        close(fd);

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!getInfoResult) {
            APP_LOGI("ApplicationInfoDump_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("ApplicationInfoDump_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END ApplicationInfoDump_0100" << std::endl;
}

/**
 * @tc.number: Errors_0100
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an error hap
 *           2.install the hap
 *           3.get ERR_INSTALL_ALREADY_EXIST
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0100, Function | MediumTest | Level1)
{
    std::cout << "Errors_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;

        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        resvec.clear();
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_ALREADY_EXIST]");

        std::vector<std::string> resvec2;
        Uninstall(appName, resvec2);
        std::string uninstallResult = commonTool.VectorToStr(resvec2);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (std::strcmp(installResult.c_str(), "Success") == 0) {
            APP_LOGI("Errors_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0100" << std::endl;
}

/**
 * @tc.number: Errors_0200
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an error hap
 *           2.install the hap
 *           3.get ERR_INSTALL_VERSION_DOWNGRADE
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0200, Function | MediumTest | Level1)
{
    std::cout << "Errors_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";
        std::string bundleName = BASE_BUNDLE_NAME + "2";
        std::vector<std::string> resvec;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        resvec.clear();
        bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
        Install(bundleFilePath, InstallFlag::REPLACE_EXISTING, resvec);
        installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_VERSION_DOWNGRADE]");

        std::vector<std::string> resvec2;
        Uninstall(bundleName, resvec2);
        std::string uninstallResult = commonTool.VectorToStr(resvec2);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (std::strcmp(installResult.c_str(), "Success") == 0) {
            APP_LOGI("Errors_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0200" << std::endl;
}

/**
 * @tc.number: Errors_0300
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an error hap
 *           2.install the hap
 *           3.get ERR_INSTALL_PARSE_PROFILE_MISSING_PROP
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0300, Function | MediumTest | Level1)
{
    std::cout << "Errors_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle14.hap";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_PARSE_PROFILE_MISSING_PROP]");

        resvec.clear();
        Install(bundleFilePath, InstallFlag::REPLACE_EXISTING, resvec);
        installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_PARSE_PROFILE_MISSING_PROP]");

        std::string bundleName = BASE_BUNDLE_NAME + "14";

        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        EXPECT_FALSE(getInfoResult);
        if (getInfoResult) {
            APP_LOGI("Errors_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0300" << std::endl;
}

/**
 * @tc.number: Errors_0400
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an error hap
 *           2.install the hap
 *           3.get ERR_INSTALL_PARSE_NO_PROFILE
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0400, Function | MediumTest | Level1)
{
    std::cout << "Errors_0400" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle11.hap";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_PARSE_NO_PROFILE]");
        if (std::strcmp(installResult.c_str(), "Success") == 0) {
            APP_LOGI("Errors_0400 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0400 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0400" << std::endl;
}

/**
 * @tc.number: Errors_0500
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists a invalid hap
 *           2.install the hap
 *           3.get ERR_INSTALL_INVALID_HAP_NAME
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0500, Function | MediumTest | Level1)
{
    std::cout << "Errors_0500" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle12.rpk";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[ERR_INSTALL_INVALID_HAP_NAME]");
        if (std::strcmp(installResult.c_str(), "Success") == 0) {
            APP_LOGI("Errors_0500 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0500 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0500" << std::endl;
}

/**
 * @tc.number: Errors_0600
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an error hap
 *           2.install the hap
 *           3.get MSG_ERR_INSTALL_FILE_PATH_INVALID
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0600, Function | MediumTest | Level1)
{
    std::cout << "Errors_0600" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "e.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;

        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Failure[MSG_ERR_INSTALL_FILE_PATH_INVALID]");
        if (std::strcmp(installResult.c_str(), "Success") == 0) {
            APP_LOGI("Errors_0600 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0600 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0600" << std::endl;
}

/**
 * @tc.number: Errors_0700
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an error hap
 *           2.install the hap
 *           3.uninstall hap with wrong appName
 *           4.get ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0700, Function | MediumTest | Level1)
{
    std::cout << "Errors_0700" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;

        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        resvec.clear();
        appName = BASE_BUNDLE_NAME + "1";
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        std::vector<std::string> resvec2;
        appName = BASE_BUNDLE_NAME + "e";
        Uninstall(appName, resvec2);
        uninstallResult = commonTool.VectorToStr(resvec2);
        EXPECT_EQ(uninstallResult, "Failure[ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE]");

        if (std::strcmp(uninstallResult.c_str(), "Success") == 0) {
            APP_LOGI("Errors_0700 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0700 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0700" << std::endl;
}

/**
 * @tc.number: Errors_0800
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists an error hap
 *           2.install the hap
 *           3.uninstall hap twice
 *           4.get ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0800, Function | MediumTest | Level1)
{
    std::cout << "Errors_0800" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);

        CommonTool commonTool;

        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";

        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        resvec.clear();
        Uninstall(appName, resvec);
        uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Failure[ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE]");
        if (std::strcmp(uninstallResult.c_str(), "Success") == 0) {
            APP_LOGI("Errors_0800 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0800 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0800" << std::endl;
}

/**
 * @tc.number: Errors_0900
 * @tc.name: test error hap
 * @tc.desc: 1.under '/data/test/bms_bundle',there not exists a hap
 *           2.uninstall the hap
 *           3.get MSG_ERR_UNINSTALL_SYSTEM_APP_ERROR
 */
HWTEST_F(ActsBmsKitSystemTest, Errors_0900, Function | MediumTest | Level1)
{
    std::cout << "Errors_0900" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string appName = "com.ohos.systemui";
        CommonTool commonTool;

        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Failure[MSG_ERR_UNINSTALL_SYSTEM_APP_ERROR]");
        if (std::strcmp(uninstallResult.c_str(), "Success") == 0) {
            APP_LOGI("Errors_0900 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("Errors_0900 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "Errors_0900" << std::endl;
}

/**
 * @tc.number: ApplicationInfo_0100
 * @tc.name: struct ApplicationInfo
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists a hap
 *           2.install the hap
 *           3.call dump
 *           4.check the appInfo in file
 */
HWTEST_F(ActsBmsKitSystemTest, ApplicationInfo_0100, Function | MediumTest | Level1)
{
    std::cout << "START ApplicationInfo_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap", InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        ApplicationInfo appInfo;
        int userId = Constants::DEFAULT_USERID;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfo(appName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
        EXPECT_TRUE(getInfoResult);
        EXPECT_EQ(appInfo.name, appName);
        ApplicationInfo *pAppInfo = &appInfo;
        std::string path = "/data/test/pAppInfo_01.txt";
        std::ofstream file(path);
        file.close();
        int fd = open(path.c_str(), O_RDWR);
        EXPECT_NE(fd, -1) << "open file error";
        pAppInfo->Dump("[pAppInfo]", fd);
        long length = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        std::string strAppInfo;
        strAppInfo.resize(length - 1);
        ssize_t retVal = read(fd, strAppInfo.data(), length);
        EXPECT_GT(retVal, 0);
        EXPECT_TRUE(IsSubStr(strAppInfo, appName));
        std::string cacheDir = BUNDLE_DATA_ROOT_PATH + appName + "/cache";
        EXPECT_TRUE(IsSubStr(strAppInfo, cacheDir));
        close(fd);
        resvec.clear();
        Uninstall(appName, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
        if (retVal <= 0) {
            APP_LOGI("ApplicationInfo_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("ApplicationInfo_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END ApplicationInfo_0100" << std::endl;
}

/**
 * @tc.number: ApplicationInfo_0200
 * @tc.name: struct ApplicationInfo
 * @tc.desc: 1.init appInfo structure
 *           2.Dump the pAppInfo
 */
HWTEST_F(ActsBmsKitSystemTest, ApplicationInfo_0200, Function | MediumTest | Level1)
{
    std::cout << "START ApplicationInfo_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        ApplicationInfo appInfo;
        appInfo.bundleName = "com.third.hiworld.example_02";
        appInfo.label = "bmsThirdBundle_A1 Ability";
        appInfo.description = "example helloworld";
        appInfo.deviceId = Constants::CURRENT_DEVICE_ID;
        appInfo.isSystemApp = false;

        ApplicationInfo *pAppInfo = &appInfo;
        std::string path = "/data/test/pAppInfo_02.txt";
        std::ofstream file(path);
        file.close();
        int fd = open(path.c_str(), O_RDWR | O_CLOEXEC);
        EXPECT_NE(fd, -1) << "open file error";
        std::string prefix = "[pAppInfo]";
        pAppInfo->Dump(prefix, fd);
        long length = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        std::string strAppInfo;
        strAppInfo.resize(length - 1);
        ssize_t retVal = read(fd, strAppInfo.data(), length);
        EXPECT_GT(retVal, 0);
        EXPECT_TRUE(IsSubStr(strAppInfo, appInfo.bundleName));
        EXPECT_TRUE(IsSubStr(strAppInfo, appInfo.label));
        EXPECT_TRUE(IsSubStr(strAppInfo, appInfo.description));
        close(fd);

        if (retVal <= 0) {
            APP_LOGI("ApplicationInfo_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("ApplicationInfo_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END ApplicationInfo_0200" << std::endl;
}

/**
 * @tc.number: QueryKeepAliveBundleInfos_0100
 * @tc.name: test QueryKeepAliveBundleInfos interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap,whose keepAlive-property's value is true
 *           2.install the hap
 *           3.call QueryKeepAliveBundleInfos
 */
HWTEST_F(ActsBmsKitSystemTest, QueryKeepAliveBundleInfos_0100, Function | MediumTest | Level1)
{
    std::cout << "START QueryKeepAliveBundleInfos_0100" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string appName = BASE_BUNDLE_NAME + "1";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGI("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    std::vector<BundleInfo> bundleInfos;
    bool result = bundleMgrProxy->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_TRUE(result);

    resvec.clear();
    Uninstall(appName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    std::cout << "END QueryKeepAliveBundleInfos_0100" << std::endl;
}

/**
 * @tc.number: QueryKeepAliveBundleInfos_0200
 * @tc.name: test QueryKeepAliveBundleInfos interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap,whose keepAlive-property's value is false
 *           2.install the hap
 *           3.call QueryKeepAliveBundleInfos
 */
HWTEST_F(ActsBmsKitSystemTest, QueryKeepAliveBundleInfos_0200, Function | MediumTest | Level2)
{
    std::cout << "START QueryKeepAliveBundleInfos_0100" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string appName = BASE_BUNDLE_NAME + "1";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGI("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    std::vector<BundleInfo> bundleInfos;
    bool result = bundleMgrProxy->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_FALSE(result);

    resvec.clear();
    Uninstall(appName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    std::cout << "END QueryKeepAliveBundleInfos_0100" << std::endl;
}

/**
 * @tc.number: Uninstall_KeepData_0100
 * @tc.name: test whether to keep user data when the app is uninstalled
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.uninstall the hap
 */
HWTEST_F(ActsBmsKitSystemTest, Uninstall_KeepData_0100, Function | MediumTest | Level2)
{
    std::cout << "START Uninstall_KeepData_0100" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";

    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        EXPECT_EQ(installerProxy, nullptr);
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    installParam.userId = Constants::DEFAULT_USERID;
    installParam.isKeepData = false;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    std::string installMsg = statusReceiver->GetResultMsg();
    EXPECT_EQ(installMsg, "Success") << "install fail!";
    std::string appName = BASE_BUNDLE_NAME + "1";

    std::vector<std::string> resvec;
    Uninstall(appName, resvec);
    CommonTool commonTool;
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    CheckFileNonExist(appName);
    std::cout << "END Uninstall_KeepData_0100" << std::endl;
}

/**
 * @tc.number: Uninstall_KeepData_0200
 * @tc.name: test whether to keep user data when the app is uninstalled
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.uninstall the hap
 */
HWTEST_F(ActsBmsKitSystemTest, Uninstall_KeepData_0200, Function | MediumTest | Level2)
{
    std::cout << "START Uninstall_KeepData_0200" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";

    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        EXPECT_EQ(installerProxy, nullptr);
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    installParam.userId = Constants::DEFAULT_USERID;
    installParam.isKeepData = true;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    std::string installMsg = statusReceiver->GetResultMsg();
    EXPECT_EQ(installMsg, "Success") << "install fail!";
    std::string appName = BASE_BUNDLE_NAME + "1";

    std::vector<std::string> resvec;
    Uninstall(appName, resvec);
    CommonTool commonTool;
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    CheckFileExist(appName);
    std::cout << "END Uninstall_KeepData_0200" << std::endl;
}

/**
 * @tc.number: Uninstall_KeepData_0300
 * @tc.name: test whether to keep user data when the hap is uninstalled
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.uninstall the hap
 */
HWTEST_F(ActsBmsKitSystemTest, Uninstall_KeepData_0300, Function | MediumTest | Level2)
{
    std::cout << "START Uninstall_KeepData_0300" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";

    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        EXPECT_EQ(installerProxy, nullptr);
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    installParam.userId = Constants::DEFAULT_USERID;
    installParam.isKeepData = false;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    std::string installMsg = statusReceiver->GetResultMsg();
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    sptr<StatusReceiverImpl> statusReceiver2 = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver2, nullptr);
    installerProxy->Install(bundleFilePath, installParam, statusReceiver2);
    installMsg = statusReceiver2->GetResultMsg();
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string appName = BASE_BUNDLE_NAME + "1";

    std::vector<std::string> resvec;
    std::string bundleName = BASE_BUNDLE_NAME + "1";
    std::string modulePackage = BASE_BUNDLE_NAME + ".h1";
    HapUninstall(bundleName, modulePackage, resvec);
    CommonTool commonTool;
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall hap fail!";
    CheckFileNonExist(appName, modulePackage);
    resvec.clear();
    Uninstall(appName, resvec);
    uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    std::cout << "END Uninstall_KeepData_0300" << std::endl;
}

/**
 * @tc.number: Uninstall_KeepData_0400
 * @tc.name: test whether to keep user data when the hap is uninstalled
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.uninstall the hap
 */
HWTEST_F(ActsBmsKitSystemTest, Uninstall_KeepData_0400, Function | MediumTest | Level2)
{
    std::cout << "START Uninstall_KeepData_0400" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";

    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        EXPECT_EQ(installerProxy, nullptr);
    }
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    installParam.userId = Constants::DEFAULT_USERID;
    installParam.isKeepData = true;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    std::string installMsg = statusReceiver->GetResultMsg();
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    sptr<StatusReceiverImpl> statusReceiver2 = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver2, nullptr);
    installerProxy->Install(bundleFilePath, installParam, statusReceiver2);
    installMsg = statusReceiver2->GetResultMsg();
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string appName = BASE_BUNDLE_NAME + "1";
    std::vector<std::string> resvec;
    std::string bundleName = BASE_BUNDLE_NAME + "1";
    std::string modulePackage = BASE_BUNDLE_NAME + ".h1";
    HapUninstall(bundleName, modulePackage, resvec);
    CommonTool commonTool;
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall hap fail!";
    CheckFileExist(appName, modulePackage);
    resvec.clear();
    Uninstall(appName, resvec);
    uninstallResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    std::cout << "END Uninstall_KeepData_0400" << std::endl;
}

/**
 * @tc.number: GetBundlesForUid_0100
 * @tc.name: test GetBundlesForUid interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there exist three bundles
 *           2.install these apps
 *           3.query all bundles by uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundlesForUid_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetBundlesForUid_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        CommonTool commonTool;
        for (int i = 6; i <= 8; i++) {
            std::vector<std::string> resvec;
            std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
            Install(hapFilePath, InstallFlag::NORMAL, resvec);
            EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        }
        bool ret;
        for (int i = 1; i <= 3; i++) {
            std::string appName = BASE_BUNDLE_NAME + std::to_string(i);
            BundleInfo bundleInfo;
            bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
            std::vector<std::string> bundleNames;
            ret = bundleMgrProxy->GetBundlesForUid(bundleInfo.uid, bundleNames);
            EXPECT_TRUE(ret);
            for (auto bundleName : bundleNames) {
                EXPECT_EQ(bundleName, appName);
            }
        }
        for (int i = 1; i <= 3; i++) {
            std::vector<std::string> resvec;
            std::string appName = BASE_BUNDLE_NAME + std::to_string(i);
            Uninstall(appName, resvec);
            EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
        }
        if (!ret) {
            APP_LOGI("GetBundlesForUid_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundlesForUid_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundlesForUid_0100" << std::endl;
}

/**
 * @tc.number: GetBundlesForUid_0200
 * @tc.name: test GetBundlesForUid interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a bundle
 *           2.install the hap
 *           3.query bundles by wrong uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundlesForUid_0200, Function | MediumTest | Level2)
{
    std::cout << "START GetBundlesForUid_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::vector<std::string> bundleNames;
        bool ret = bundleMgrProxy->GetBundlesForUid(Constants::INVALID_UID, bundleNames);
        EXPECT_FALSE(ret);
        if (ret) {
            APP_LOGI("GetBundlesForUid_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundlesForUid_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundlesForUid_0200" << std::endl;
}

/**
 * @tc.number: GetBundlesForUid_0300
 * @tc.name: test GetBundlesForUid interface
 * @tc.desc: 1.query bundles by uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetBundlesForUid_0300, Function | MediumTest | Level1)
{
    std::cout << "START GetBundlesForUid_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string appName = SYSTEM_SETTINGS_BUNDLE_NAME;
        BundleInfo bundleInfo;
        bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        int uid = bundleInfo.uid;
        std::vector<std::string> bundleNames;
        bool ret = bundleMgrProxy->GetBundlesForUid(uid, bundleNames);
        EXPECT_TRUE(ret);
        for (auto bundleName : bundleNames) {
            EXPECT_EQ(bundleName, SYSTEM_SETTINGS_BUNDLE_NAME);
        }

        if (!ret) {
            APP_LOGI("GetBundlesForUid_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetBundlesForUid_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetBundlesForUid_0300" << std::endl;
}

/**
 * @tc.number: GetNameForUid_0100
 * @tc.name: test GetNameForUid interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two apps
 *           2.install fist hap
 *           3.get first hap name by uid
 *           4.uninstall first hap
 *           5.install second hap
 *           6.install first hap
 *           7.get first hap name by uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetNameForUid_0100, Function | MediumTest | Level1)
{
    std::cout << "START GetNameForUid_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        CommonTool commonTool;
        std::vector<std::string> resvec;
        std::string name1, name2;
        Install(THIRD_BUNDLE_PATH + "bmsThirdBundle6.hap", InstallFlag::NORMAL, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        resvec.clear();
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        BundleInfo bundleInfo1;
        bundleMgrProxy->GetBundleInfo(BASE_BUNDLE_NAME + '1', BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo1);
        bool ret = bundleMgrProxy->GetNameForUid(bundleInfo1.uid, name1);
        EXPECT_TRUE(ret);
        Uninstall(BASE_BUNDLE_NAME + '1', resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
        resvec.clear();
        Install(THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap", InstallFlag::NORMAL, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        resvec.clear();
        Install(THIRD_BUNDLE_PATH + "bmsThirdBundle6.hap", InstallFlag::NORMAL, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";
        BundleInfo bundleInfo2;
        bundleMgrProxy->GetBundleInfo(BASE_BUNDLE_NAME + '1', BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo2);
        ret = bundleMgrProxy->GetNameForUid(bundleInfo2.uid, name2);
        EXPECT_TRUE(ret);
        EXPECT_NE(bundleInfo1.uid, bundleInfo2.uid);
        EXPECT_EQ(name1, name2);
        for (int i = 1; i <= 2; i++) {
            std::string appName = BASE_BUNDLE_NAME + std::to_string(i);
            std::vector<std::string> resvec2;
            Uninstall(appName, resvec2);
            EXPECT_EQ(commonTool.VectorToStr(resvec2), "Success") << "uninstall fail!";
        }
        if (!ret) {
            APP_LOGI("GetNameForUid_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetNameForUid_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetNameForUid_0100" << std::endl;
}

/**
 * @tc.number: GetNameForUid_0200
 * @tc.name: test GetNameForUid interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.get name by wrong uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetNameForUid_0200, Function | MediumTest | Level2)
{
    std::cout << "START GetNameForUid_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        CommonTool commonTool;
        std::string installResult;
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + '1';
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        BundleInfo bundleInfo;
        bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        std::string name;
        bool ret = bundleMgrProxy->GetNameForUid(Constants::INVALID_UID, name);
        EXPECT_FALSE(ret);
        resvec.clear();
        Uninstall(appName, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
        if (ret) {
            APP_LOGI("GetNameForUid_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetNameForUid_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetNameForUid_0200" << std::endl;
}

/**
 * @tc.number: GetNameForUid_0300
 * @tc.name: test GetNameForUid interface
 * @tc.desc: 1.get name by uid
 */
HWTEST_F(ActsBmsKitSystemTest, GetNameForUid_0300, Function | MediumTest | Level1)
{
    std::cout << "START GetNameForUid_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string appName = SYSTEM_SETTINGS_BUNDLE_NAME;
        BundleInfo bundleInfo;
        bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        std::string name;
        bool ret = bundleMgrProxy->GetNameForUid(bundleInfo.uid, name);
        EXPECT_TRUE(ret);
        EXPECT_EQ(name, SYSTEM_SETTINGS_BUNDLE_NAME);

        if (!ret) {
            APP_LOGI("GetNameForUid_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("GetNameForUid_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END GetNameForUid_0300" << std::endl;
}

/**
 * @tc.number: SetAbilityEnabled_0100
 * @tc.name: test SetAbilityEnabled interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call SetAbilityEnabled by a hap's abilityInfo
 */
HWTEST_F(ActsBmsKitSystemTest, SetAbilityEnabled_0100, Function | MediumTest | Level1)
{
    std::cout << "START SetAbilityEnabled_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
        EXPECT_TRUE(ret);
        for (auto abilityInfo : bundleInfo.abilityInfos) {
            EXPECT_TRUE(bundleMgrProxy->SetAbilityEnabled(abilityInfo, false));
            EXPECT_TRUE(bundleMgrProxy->SetAbilityEnabled(abilityInfo, true));
        }
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (!ret) {
            APP_LOGI("SetAbilityEnabled_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("SetAbilityEnabled_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END SetAbilityEnabled_0100" << std::endl;
}

/**
 * @tc.number: SetAbilityEnabled_0200
 * @tc.name: test SetAbilityEnabled interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call SetAbilityEnabled by wrong abilityInfo
 */
HWTEST_F(ActsBmsKitSystemTest, SetAbilityEnabled_0200, Function | MediumTest | Level2)
{
    std::cout << "START SetAbilityEnabled_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        AbilityInfo abilityInfo;
        abilityInfo.bundleName = BASE_BUNDLE_NAME + "2";
        abilityInfo.name = "bmsThirdBundle_A1";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->SetAbilityEnabled(abilityInfo, false);
        EXPECT_FALSE(ret);
        ret = bundleMgrProxy->SetAbilityEnabled(abilityInfo, true);
        EXPECT_FALSE(ret);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (ret) {
            APP_LOGI("SetAbilityEnabled_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("SetAbilityEnabled_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END SetAbilityEnabled_0200" << std::endl;
}

/**
 * @tc.number: SetAbilityEnabled_0300
 * @tc.name: test SetAbilityEnabled interface
 * @tc.desc: 1.call SetAbilityEnabled by com.ohos.settings abilityInfo
 */
HWTEST_F(ActsBmsKitSystemTest, SetAbilityEnabled_0300, Function | MediumTest | Level1)
{
    std::cout << "START SetAbilityEnabled_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string appName = SYSTEM_SETTINGS_BUNDLE_NAME;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        AbilityInfo abilityInfo;
        abilityInfo.bundleName = appName;
        abilityInfo.name = "com.ohos.settings.MainAbility";
        bool ret = bundleMgrProxy->SetAbilityEnabled(abilityInfo, false);
        EXPECT_TRUE(ret);
        EXPECT_FALSE(bundleMgrProxy->IsAbilityEnabled(abilityInfo));
        ret = bundleMgrProxy->SetAbilityEnabled(abilityInfo, true);
        EXPECT_TRUE(ret);
        EXPECT_TRUE(bundleMgrProxy->IsAbilityEnabled(abilityInfo));
        if (!ret) {
            APP_LOGI("SetAbilityEnabled_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("SetAbilityEnabled_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END SetAbilityEnabled_0300" << std::endl;
}

/**
 * @tc.number: IsAbilityEnabled_0100
 * @tc.name: test IsAbilityEnabled interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call IsAbilityEnabled by a hap's abilityInfo
 */
HWTEST_F(ActsBmsKitSystemTest, IsAbilityEnabled_0100, Function | MediumTest | Level1)
{
    std::cout << "START IsAbilityEnabled_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        BundleInfo bundleInfo;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
        EXPECT_TRUE(ret);
        for (auto abilityInfo : bundleInfo.abilityInfos) {
            ret = bundleMgrProxy->IsAbilityEnabled(abilityInfo);
            EXPECT_TRUE(ret);
        }
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (!ret) {
            APP_LOGI("IsAbilityEnabled_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }
    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("IsAbilityEnabled_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END IsAbilityEnabled_0100" << std::endl;
}

/**
 * @tc.number: IsAbilityEnabled_0200
 * @tc.name: test IsAbilityEnabled interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call IsAbilityEnabled by wrong abilityInfo
 */
HWTEST_F(ActsBmsKitSystemTest, IsAbilityEnabled_0200, Function | MediumTest | Level2)
{
    std::cout << "START IsAbilityEnabled_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        std::string abilityName = "bmsThirdBundle_A1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        AbilityInfo abilityInfo;
        abilityInfo.bundleName = BASE_BUNDLE_NAME + "1";
        abilityInfo.name = "bmsThirdBundle_A3";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->IsAbilityEnabled(abilityInfo);
        EXPECT_FALSE(ret);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";

        if (ret) {
            APP_LOGI("IsAbilityEnabled_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("IsAbilityEnabled_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END IsAbilityEnabled_0200" << std::endl;
}

/**
 * @tc.number: IsAbilityEnabled_0300
 * @tc.name: test IsAbilityEnabled interface
 * @tc.desc: 1.call IsAbilityEnabled
 */
HWTEST_F(ActsBmsKitSystemTest, IsAbilityEnabled_0300, Function | MediumTest | Level1)
{
    std::cout << "START IsAbilityEnabled_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::string appName = SYSTEM_SETTINGS_BUNDLE_NAME;
        std::string abilityName = "com.ohos.settings.MainAbility";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        AbilityInfo abilityInfo;
        abilityInfo.bundleName = appName;
        abilityInfo.name = abilityName;
        bool ret = bundleMgrProxy->IsAbilityEnabled(abilityInfo);
        EXPECT_TRUE(ret);
        if (!ret) {
            APP_LOGI("IsAbilityEnabled_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("IsAbilityEnabled_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END IsAbilityEnabled_0300" << std::endl;
}

/**
 * @tc.number: SetApplicationEnabled_0100
 * @tc.name: test SetApplicationEnabled interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two haps
 *           2.install the haps
 *           3.call SetApplicationEnabled
 */
HWTEST_F(ActsBmsKitSystemTest, SetApplicationEnabled_0100, Function | MediumTest | Level1)
{
    std::cout << "START SetApplicationEnabled_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        CommonTool commonTool;
        for (int i = 6; i <= 7; i++) {
            std::vector<std::string> resvec;
            std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
            Install(hapFilePath, InstallFlag::NORMAL, resvec);
            std::string installResult = commonTool.VectorToStr(resvec);
            EXPECT_EQ(installResult, "Success") << "install fail!";
        }
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->SetApplicationEnabled(appName, false);
        EXPECT_TRUE(ret);
        ret = bundleMgrProxy->SetApplicationEnabled(appName, true);
        EXPECT_TRUE(ret);
        for (int i = 1; i <= 2; i++) {
            std::vector<std::string> resvec;
            std::string appName = BASE_BUNDLE_NAME + std::to_string(i);
            Uninstall(appName, resvec);
            std::string uninstallResult = commonTool.VectorToStr(resvec);
            EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        }
        if (!ret) {
            APP_LOGI("SetApplicationEnabled_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("SetApplicationEnabled_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END SetApplicationEnabled_0100" << std::endl;
}

/**
 * @tc.number: SetApplicationEnabled_0200
 * @tc.name: test SetApplicationEnabled interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call SetApplicationEnabled
 */
HWTEST_F(ActsBmsKitSystemTest, SetApplicationEnabled_0200, Function | MediumTest | Level2)
{
    std::cout << "START SetApplicationEnabled_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        CommonTool commonTool;
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        resvec.clear();
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        std::string bundleName = BASE_BUNDLE_NAME + "2";
        bool ret = bundleMgrProxy->SetApplicationEnabled(bundleName, false);
        EXPECT_FALSE(ret);
        ret = bundleMgrProxy->SetApplicationEnabled(bundleName, true);
        EXPECT_FALSE(ret);
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (ret) {
            APP_LOGI("SetApplicationEnabled_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("SetApplicationEnabled_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END SetApplicationEnabled_0200" << std::endl;
}

/**
 * @tc.number: SetApplicationEnabled_0300
 * @tc.name: test SetApplicationEnabled interface
 * @tc.desc: 1.call SetApplicationEnabled
 */
HWTEST_F(ActsBmsKitSystemTest, SetApplicationEnabled_0300, Function | MediumTest | Level1)
{
    std::cout << "START SetApplicationEnabled_0300" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->SetApplicationEnabled(SYSTEM_SETTINGS_BUNDLE_NAME, false);
        EXPECT_TRUE(ret);
        EXPECT_FALSE(bundleMgrProxy->IsApplicationEnabled(SYSTEM_SETTINGS_BUNDLE_NAME));
        ret = bundleMgrProxy->SetApplicationEnabled(SYSTEM_SETTINGS_BUNDLE_NAME, true);
        EXPECT_TRUE(ret);
        EXPECT_TRUE(bundleMgrProxy->IsApplicationEnabled(SYSTEM_SETTINGS_BUNDLE_NAME));
        if (!ret) {
            APP_LOGI("SetApplicationEnabled_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("SetApplicationEnabled_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END SetApplicationEnabled_0300" << std::endl;
}

/**
 * @tc.number: IsApplicationEnabled_0100
 * @tc.name: test IsApplicationEnabled interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call IsApplicationEnabled
 */
HWTEST_F(ActsBmsKitSystemTest, IsApplicationEnabled_0100, Function | MediumTest | Level1)
{
    std::cout << "START IsApplicationEnabled_0100" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->IsApplicationEnabled(appName);
        EXPECT_TRUE(ret);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (!ret) {
            APP_LOGI("IsApplicationEnabled_0100 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("IsApplicationEnabled_0100 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END IsApplicationEnabled_0100" << std::endl;
}

/**
 * @tc.number: IsApplicationEnabled_0200
 * @tc.name: test IsApplicationEnabled interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.install the hap
 *           3.call IsApplicationEnabled
 */
HWTEST_F(ActsBmsKitSystemTest, IsApplicationEnabled_0200, Function | MediumTest | Level2)
{
    std::cout << "START IsApplicationEnabled_0200" << std::endl;
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::vector<std::string> resvec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
        std::string appName = BASE_BUNDLE_NAME + "1";
        Install(bundleFilePath, InstallFlag::NORMAL, resvec);
        CommonTool commonTool;
        std::string installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        std::string appName2 = BASE_BUNDLE_NAME + "2";
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->IsApplicationEnabled(appName2);
        EXPECT_FALSE(ret);
        resvec.clear();
        Uninstall(appName, resvec);
        std::string uninstallResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
        if (ret) {
            APP_LOGI("IsApplicationEnabled_0200 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("IsApplicationEnabled_0200 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END IsApplicationEnabled_0200" << std::endl;
}

/**
 * @tc.number: IsApplicationEnabled_0300
 * @tc.name: test IsApplicationEnabled interface
 * @tc.desc: 1.call IsApplicationEnabled
 */
HWTEST_F(ActsBmsKitSystemTest, IsApplicationEnabled_0300, Function | MediumTest | Level1)
{
    bool result = false;
    for (int i = 1; i <= stLevel_.BMSLevel; i++) {
        std::cout << "START IsApplicationEnabled_0300" << std::endl;
        sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
        if (!bundleMgrProxy) {
            APP_LOGE("bundle mgr proxy is nullptr.");
            EXPECT_EQ(bundleMgrProxy, nullptr);
        }
        bool ret = bundleMgrProxy->IsApplicationEnabled(SYSTEM_SETTINGS_BUNDLE_NAME);
        EXPECT_TRUE(ret);
        if (!ret) {
            APP_LOGI("IsApplicationEnabled_0300 failed - cycle count: %{public}d", i);
            break;
        }
        result = true;
    }

    if (result && stLevel_.BMSLevel > 1) {
        APP_LOGI("IsApplicationEnabled_0300 success - cycle count: %{public}d", stLevel_.BMSLevel);
    }
    EXPECT_TRUE(result);
    std::cout << "END IsApplicationEnabled_0300" << std::endl;
}
}  // namespace AppExecFwk
}  // namespace OHOS