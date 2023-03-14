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

using OHOS::AAFwk::Want;
using namespace testing::ext;

namespace {
const std::string THIRD_BUNDLE_PATH = "/data/test/bms_bundle/";
const std::string SYSTEM_BUNDLE_PATH = "/system/app/";
const std::string VENDOR_BUNDLE_PATH = "/system/vendor/";
const std::string BASE_BUNDLE_NAME = "com.third.hiworld.example";
const std::string SYSTEM_BASE_BUNDLE_NAME = "com.system.hiworld.example";
const std::string BUNDLE_DATA_ROOT_PATH = "/data/app/el2/100/base/";
const std::string ERROR_INSTALL_FAILED = "install failed!";
const std::string ERROR_UNINSTALL_FAILED = "uninstall failed!";
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILED = "Failure";
const std::string OPERATION_SUCCESS = "Success";
static constexpr int INDEX_TWO = 2;
static constexpr int INDEX_THREE = 3;
static constexpr int INDEX_SEVEN = 7;
static constexpr int INDEX_EIGHT = 8;
static constexpr int INDEX_NINE = 9;
}  // namespace

namespace OHOS {
namespace AppExecFwk {
class BundleStatusCallbackImpl : public BundleStatusCallbackHost {
public:
    BundleStatusCallbackImpl();
    virtual ~BundleStatusCallbackImpl() override;
    virtual void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode, const std::string &resultMsg,
        const std::string &bundleName) override;
    virtual void OnBundleAdded(const std::string &bundleName, const int userId) override{};
    virtual void OnBundleUpdated(const std::string &bundleName, const int userId) override{};
    virtual void OnBundleRemoved(const std::string &bundleName, const int userId) override{};

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
    APP_LOGI("Bms_Search_St OnBundleStateChanged results are %{public}d, %{public}d, %{public}s, %{public}s",
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
    APP_LOGI("Bms_Search_St OnCleanCacheFinished results are %{public}d", succeeded);
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
    APP_LOGI("OnStatusNotify");
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

class BmsSearchSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void Install(
        const std::string &bundleFilePath, const InstallFlag installFlag, std::vector<std::string> &resvec);
    static void Uninstall(const std::string &bundleName, std::vector<std::string> &resvec);
    static sptr<IBundleMgr> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
    bool QueryJsonFile(const std::string &bundleName) const;
};

void BmsSearchSystemTest::SetUpTestCase()
{
    CommonTool commonTool;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string appName = BASE_BUNDLE_NAME + "1";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install bmsThirdBundle1.hap fail!";
}

void BmsSearchSystemTest::TearDownTestCase()
{
    CommonTool commonTool;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    std::string bundleName = BASE_BUNDLE_NAME + "1";
    std::vector<std::string> resvec;
    Uninstall(bundleName, resvec);
    std::string uninstallResult = commonTool.VectorToStr(resvec);
    APP_LOGI("BmsInstallSystemTest TearDown--uninstall result is %{public}s", uninstallResult.c_str());
    std::cout << "BmsInstallSystemTest TearDownTestCase" << std::endl;
}

void BmsSearchSystemTest::SetUp()
{}

void BmsSearchSystemTest::TearDown()
{}

sptr<IBundleMgr> BmsSearchSystemTest::GetBundleMgrProxy()
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

sptr<IBundleInstaller> BmsSearchSystemTest::GetInstallerProxy()
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

bool BmsSearchSystemTest::QueryJsonFile(const std::string &bundleName) const
{
    CommonTool commonTool;

    bool chkFileIsExist = commonTool.CheckFilePathISExist(BUNDLE_DATA_ROOT_PATH + bundleName);
    if (!chkFileIsExist) {
        APP_LOGE("config.json does not exist!");
        return false;
    }
    return true;
}

void BmsSearchSystemTest::Install(
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

void BmsSearchSystemTest::Uninstall(const std::string &bundleName, std::vector<std::string> &resvec)
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

/**
 * @tc.number: BMS_Search_0100
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query bundleInfo
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Search_0100" << std::endl;
    std::string appName = BASE_BUNDLE_NAME + "1";
    CommonTool commonTool;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    BundleInfo bundleInfo;
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(getInfoResult);
    EXPECT_EQ(bundleInfo.name, appName);
    std::cout << bundleInfo.name << std::endl;
    std::cout << bundleInfo.label << std::endl;
    std::cout << bundleInfo.vendor << std::endl;
    std::cout << bundleInfo.versionCode << std::endl;
    std::cout << bundleInfo.versionName << std::endl;
    std::cout << bundleInfo.appId << std::endl;
    std::cout << commonTool.VectorToStr(bundleInfo.moduleDirs) << std::endl;
    std::cout << commonTool.VectorToStr(bundleInfo.moduleNames) << std::endl;
    std::cout << commonTool.VectorToStr(bundleInfo.hapModuleNames) << std::endl;
    std::cout << commonTool.VectorToStr(bundleInfo.modulePublicDirs) << std::endl;
    std::cout << "END BMS_Search_0100" << std::endl;
}

/**
 * @tc.number: BMS_Search_0200
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query bundleInfo with abilities
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_0200" << std::endl;
    std::string appName = BASE_BUNDLE_NAME + "1";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    BundleInfo bundleInfo;
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
    EXPECT_TRUE(getInfoResult);
    EXPECT_EQ(bundleInfo.name, appName);
    bool isSubStrExist = false;
    for (auto ability : bundleInfo.abilityInfos) {
        if (IsSubStr(ability.name, "bmsThirdBundle_A1")) {
            isSubStrExist = true;
            break;
        }
    }
    EXPECT_TRUE(isSubStrExist);
    std::cout << "END BMS_SEARCH_0200" << std::endl;
}

/**
 * @tc.number: BMS_Search_0300
 * @tc.name: test query bundle information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query bundleInfo with wrong appname
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0300, Function | MediumTest | Level2)
{
    std::cout << "START BMS_SEARCH_0300" << std::endl;
    std::string appName = BASE_BUNDLE_NAME + "1";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    appName = BASE_BUNDLE_NAME + "e";
    BundleInfo bundleInfo;
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_SEARCH_0300" << std::endl;
}

/**
 * @tc.number: BMS_Search_0400
 * @tc.name: test query application information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query appinfo
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0400, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_0400" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";
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
    std::cout << "END BMS_SEARCH_0400" << std::endl;
}

/**
 * @tc.number: BMS_Search_0500
 * @tc.name: test query application information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query appinfo with permission
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0500, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_0500" << std::endl;
    std::string appName = BASE_BUNDLE_NAME + "1";

    CommonTool commonTool;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    ApplicationInfo appInfo;
    int userId = Constants::DEFAULT_USERID;
    bool getInfoResult =
        bundleMgrProxy->GetApplicationInfo(
            appName, ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, appInfo);
    std::string permission = commonTool.VectorToStr(appInfo.permissions);
    EXPECT_TRUE(getInfoResult);
    std::cout << permission << std::endl;
    std::cout << "END BMS_SEARCH_0500" << std::endl;
}

/**
 * @tc.number: BMS_Search_0600
 * @tc.name: test query application information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query appInfo with wrong appname
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0600, Function | MediumTest | Level2)
{
    std::cout << "START BMS_SEARCH_0600" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";

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
    std::cout << "END BMS_SEARCH_0600" << std::endl;
}

/**
 * @tc.number: BMS_Search_0700
 * @tc.name: test query  ".hap" information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query ".hap" information
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0700, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_0700" << std::endl;

    std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string appName = BASE_BUNDLE_NAME + "1";

    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(getInfoResult);
    std::cout << "END BMS_SEARCH_0700" << std::endl;
}

/**
 * @tc.number: BMS_Search_0800
 * @tc.name: test query ".hap" information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query ".hap" with ability information
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0800, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_0800" << std::endl;
    std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
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
    bool isSubStrExist = false;
    for (auto ability : bundleInfo.abilityInfos) {
        if (IsSubStr(ability.name, "bmsThirdBundle_A1")) {
            isSubStrExist = true;
            break;
        }
    }
    EXPECT_TRUE(isSubStrExist);
    std::cout << "END BMS_SEARCH_0800" << std::endl;
}

/**
 * @tc.number: BMS_Search_0900
 * @tc.name: test query hap information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query hap information with wrong name
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_0900, Function | MediumTest | Level2)
{
    std::cout << "START BMS_SEARCH_0900" << std::endl;

    std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string appName = BASE_BUNDLE_NAME + "1";

    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    hapFilePath = THIRD_BUNDLE_PATH + "tt.hap";
    bool getInfoResult = bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_SEARCH_0900" << std::endl;
}

/**
 * @tc.number: BMS_Search_1000
 * @tc.name: test query UID
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query UID by bundleName
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1000, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_1000" << std::endl;

    std::string bundleName = BASE_BUNDLE_NAME + "1";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    int userId = Constants::DEFAULT_USERID;
    int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    EXPECT_GE(uid, Constants::BASE_APP_UID);
    EXPECT_LE(uid, Constants::MAX_APP_UID);
    std::cout << "END BMS_SEARCH_1000" << std::endl;
}

/**
 * @tc.number: BMS_Search_1100
 * @tc.name: test query UID
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query UID by wrong bundleName
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1100, Function | MediumTest | Level2)
{
    std::cout << "START BMS_SEARCH_1100" << std::endl;

    std::string bundleName = BASE_BUNDLE_NAME + "1";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    int userId = Constants::DEFAULT_USERID;

    bundleName = BASE_BUNDLE_NAME + "e";
    int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    EXPECT_EQ(uid, Constants::INVALID_UID);
    std::cout << "END BMS_SEARCH_1100" << std::endl;
}

/**
 * @tc.number: BMS_Search_1200
 * @tc.name: test query bundlenames
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query bundlenames by uid
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_1200" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";

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
    std::cout << "END BMS_SEARCH_1200" << std::endl;
}

/**
 * @tc.number: BMS_Search_1300
 * @tc.name: test query app permission
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.query app permission by bundleName
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1300, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_1300" << std::endl;
    CommonTool commonTool;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";

    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success");
    resvec.clear();

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle8.hap";
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success");

    std::string bundleName = BASE_BUNDLE_NAME + "3";

    std::vector<std::string> resvec2;
    Uninstall(bundleName, resvec2);
    std::string uninstallResult = commonTool.VectorToStr(resvec2);
    EXPECT_EQ(uninstallResult, "Success");
    resvec2.clear();
    bundleName = BASE_BUNDLE_NAME + "2";
    Uninstall(bundleName, resvec2);
    uninstallResult = commonTool.VectorToStr(resvec2);
    EXPECT_EQ(uninstallResult, "Success");
    std::cout << "END BMS_SEARCH_1300" << std::endl;
}

/**
 * @tc.number: BMS_Search_1500
 * @tc.name: test query bundleinfos
 * @tc.desc: 1.under '/data/test/bms_bundle',there exist two bundles
 *           2.install the bundles
 *           3.query all bundleinfos
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1500, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_1500" << std::endl;

    CommonTool commonTool;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    std::string installResult;
    for (int i = INDEX_SEVEN; i < INDEX_NINE; i++) {
        std::vector<std::string> resvec;
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        Install(hapFilePath, InstallFlag::NORMAL, resvec);
        installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
    }

    std::vector<BundleInfo> bundleInfos;
    bool getInfoResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
    EXPECT_TRUE(getInfoResult);
    for (int i = INDEX_TWO; i <= INDEX_THREE; i++) {
        std::string appName = BASE_BUNDLE_NAME + std::to_string(i);
        bool queryResult = QueryJsonFile(appName);
        EXPECT_TRUE(queryResult);
        bool isSubStrExist = false;
        for (auto iter = bundleInfos.begin(); iter != bundleInfos.end(); iter++) {
            if (IsSubStr(iter->name, appName)) {
                isSubStrExist = true;
                break;
            }
        }
        EXPECT_TRUE(isSubStrExist);
    }
    std::vector<std::string> resvec;
    std::string bundleName = BASE_BUNDLE_NAME + "2";
    Uninstall(bundleName, resvec);
    EXPECT_EQ(commonTool.VectorToStr(resvec), "Success");
    resvec.clear();
    bundleName = BASE_BUNDLE_NAME + "3";
    Uninstall(bundleName, resvec);
    EXPECT_EQ(commonTool.VectorToStr(resvec), "Success");
    resvec.clear();
    std::cout << "END BMS_SEARCH_1500" << std::endl;
}

/**
 * @tc.number: BMS_Search_1600
 * @tc.name: test query bundleinfos
 * @tc.desc: 1.under '/system/app',there exist some bundles
 *           2.query all bundleinfos
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1600, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_1600" << std::endl;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    std::vector<BundleInfo> bundleInfos;
    bool getInfoResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
    EXPECT_TRUE(getInfoResult);
    std::cout << "END BMS_SEARCH_1600" << std::endl;
}

/**
 * @tc.number: BMS_Search_1700
 * @tc.name: test query applicationinfos
 * @tc.desc: 1.under '/data/test/bms_bundle',there exist two bundles
 *           2.install these bundles
 *           3.query all appinfos
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1700, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_1700" << std::endl;

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    CommonTool commonTool;
    for (int i = INDEX_SEVEN; i < INDEX_NINE; i++) {
        std::string installResult;
        std::vector<std::string> resvec;
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        Install(hapFilePath, InstallFlag::NORMAL, resvec);
        installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        resvec.clear();
    }
    int userId = Constants::DEFAULT_USERID;
    for (int i = INDEX_SEVEN; i <= INDEX_EIGHT; i++) {
        std::vector<std::string> resvec;
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        std::string appName = BASE_BUNDLE_NAME + std::to_string(i - 5);

        bool queryResult = QueryJsonFile(appName);
        EXPECT_TRUE(queryResult);

        std::vector<ApplicationInfo> appInfos;
        bool getInfoResult =
            bundleMgrProxy->GetApplicationInfos(
                ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, appInfos);
        EXPECT_TRUE(getInfoResult);
        bool isSubStrExist = false;
        for (auto iter = appInfos.begin(); iter != appInfos.end(); iter++) {
            if (IsSubStr(iter->name, appName)) {
                isSubStrExist = true;
                break;
            }
        }
        EXPECT_TRUE(isSubStrExist);
    }
    for (int i = INDEX_TWO; i <= INDEX_THREE; i++) {
        std::vector<std::string> resvec;
        std::string appName = BASE_BUNDLE_NAME + std::to_string(i);
        Uninstall(appName, resvec);
        EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
        resvec.clear();
    }
    std::cout << "END BMS_SEARCH_1700" << std::endl;
}

/**
 * @tc.number: BMS_Search_1800
 * @tc.name: test query applicationinfos
 * @tc.desc: 1.there is only system app installed in system
 *           2.query all appinfos
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1800, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_1800" << std::endl;
    int userId = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    std::vector<ApplicationInfo> appInfos;
    bool getInfoResult =
        bundleMgrProxy->GetApplicationInfos(ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, appInfos);
    EXPECT_TRUE(getInfoResult);
    std::cout << "END BMS_SEARCH_1800" << std::endl;
}

/**
 * @tc.number: BMS_Search_1900
 * @tc.name: test GetUidByBundleName interface
 * @tc.desc: 1.under '/system/app',there is an app
 *           2.install the app
 *           3.call GetUidByBundleName
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_1900, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_1900" << std::endl;
    std::string bundleName = "com.system.hiworld.examples1";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle1.hap";
    std::vector<std::string> resvec;
    CommonTool commonTool;
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    int userId = Constants::DEFAULT_USERID;
    int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    EXPECT_GE(uid, Constants::BASE_SYS_UID);
    EXPECT_LE(uid, Constants::MAX_SYS_UID);

    resvec.clear();
    Uninstall(bundleName, resvec);
    EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
    std::cout << "END BMS_SEARCH_1900" << std::endl;
}

/**
 * @tc.number: BMS_Search_2000
 * @tc.name: test GetUidByBundleName interface
 * @tc.desc: 1.under '/system/vendor',there is an app
 *           2.install the app
 *           3.call GetUidByBundleName
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2000, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2000" << std::endl;
    std::string bundleFilePath = VENDOR_BUNDLE_PATH + "bmsVendorBundle3.hap";
    std::string bundleName = "com.vendor.hiworld.examplev3";
    CommonTool commonTool;
    std::vector<std::string> resvec;
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    bool queryResult = QueryJsonFile(bundleName);
    EXPECT_TRUE(queryResult);

    int userId = Constants::DEFAULT_USERID;
    int uid = bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    EXPECT_GE(uid, Constants::BASE_APP_UID);
    EXPECT_LE(uid, Constants::MAX_APP_UID);

    Uninstall(bundleName, resvec);
    EXPECT_EQ(commonTool.VectorToStr(resvec), "Success") << "uninstall fail!";
    std::cout << "END BMS_SEARCH_2000" << std::endl;
}

/**
 * @tc.number: BMS_Search_2100
 * @tc.name: test CheckPublicKeys interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two bundles
 *           2.install these bundles
 *           3.call CheckPublicKeys
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2100" << std::endl;
    CommonTool commonTool;
    std::vector<std::string> resvec;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    for (int i = INDEX_SEVEN; i < INDEX_NINE; i++) {
        std::string installResult;
        std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        Install(hapFilePath, InstallFlag::NORMAL, resvec);
        installResult = commonTool.VectorToStr(resvec);
        EXPECT_EQ(installResult, "Success") << "install fail!";
        resvec.clear();
    }
    std::string hapFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    std::string firstBundleName = BASE_BUNDLE_NAME + "2";

    bool queryResult = QueryJsonFile(firstBundleName);
    EXPECT_TRUE(queryResult);

    std::string hapFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle8.hap";
    std::string secondBundleName = BASE_BUNDLE_NAME + "3";

    queryResult = QueryJsonFile(secondBundleName);
    EXPECT_TRUE(queryResult);

    bundleMgrProxy->CheckPublicKeys(firstBundleName, secondBundleName);
    for (int32_t i = INDEX_TWO; i <= INDEX_THREE; i++) {
        std::string bundleName = BASE_BUNDLE_NAME + std::to_string(i);
        std::vector<std::string> resvec;
        Uninstall(bundleName, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        APP_LOGI("uninstall result is %{public}s", installResult.c_str());
    }
    std::cout << "END BMS_SEARCH_2100" << std::endl;
}

/**
 * @tc.number: BMS_Search_2200
 * @tc.name: test GetLaunchWantForBundle interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call GetLaunchWantForBundle
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2200" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";
    std::string abilityName = "bmsThirdBundle1_A1";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    want.SetElement(name);
    want.SetBundle(appName);
    want.SetElementName(appName, abilityName);
    bool launchWantResult = bundleMgrProxy->GetLaunchWantForBundle(appName, want);
    EXPECT_TRUE(launchWantResult);

    std::cout << "END BMS_SEARCH_2200" << std::endl;
}

/**
 * @tc.number: BMS_Search_2300
 * @tc.name: test GetBundleGids interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call GetBundleGids
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2300, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2300" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    std::vector<int> gids;
    bool getGidsResult = bundleMgrProxy->GetBundleGids(appName, gids);
    EXPECT_TRUE(getGidsResult);
    std::cout << "END BMS_SEARCH_2300" << std::endl;
}

/**
 * @tc.number: BMS_Search_2400
 * @tc.name: test HasSystemCapability interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call HasSystemCapability
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2400, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2400" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";
    std::string abilityName = "bmsThirdBundle1_A1";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    bool result = bundleMgrProxy->HasSystemCapability(abilityName);
    EXPECT_TRUE(result);
    std::cout << "END BMS_SEARCH_2400" << std::endl;
}

/**
 * @tc.number: BMS_Search_2500
 * @tc.name: test IsSafeMode interface
 * @tc.desc: 1.call IsSafeMode
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2500, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2500" << std::endl;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    bool result = bundleMgrProxy->IsSafeMode();
    EXPECT_TRUE(result);
    std::cout << "END BMS_SEARCH_2500" << std::endl;
}

/**
 * @tc.number: BMS_Search_2600
 * @tc.name: test GetAppType interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call GetAppType
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2600, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2600" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";
    std::string abilityName = "bmsThirdBundle1_A1";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    std::string result = bundleMgrProxy->GetAppType(appName);
    EXPECT_EQ(result, "third-party");
    std::cout << "END BMS_SEARCH_2600" << std::endl;
}

/**
 * @tc.number: BMS_Search_2700
 * @tc.name: test GetSystemAvailableCapabilities interface
 * @tc.desc: 1.call GetSystemAvailableCapabilities
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2700, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2700" << std::endl;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    std::vector<std::string> systemCaps = {"bmsSystemBundle_A1"};
    bool result = bundleMgrProxy->GetSystemAvailableCapabilities(systemCaps);
    EXPECT_TRUE(result);
    std::cout << "END BMS_SEARCH_2700" << std::endl;
}

/**
 * @tc.number: BMS_Search_2800
 * @tc.name: test GetBundleInfosByMetaData interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call GetBundleInfosByMetaData
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2800, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2800" << std::endl;
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle17.hap";
    std::string appName = BASE_BUNDLE_NAME + "6";

    Install(bundleFilePath, InstallFlag::NORMAL, resvec);

    CommonTool commonTool;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";
    bool queryResult = QueryJsonFile(appName);
    EXPECT_TRUE(queryResult);

    std::vector<BundleInfo> bundleInfos;
    std::string metadata = "string";
    bool result = bundleMgrProxy->GetBundleInfosByMetaData(metadata, bundleInfos);
    EXPECT_TRUE(result);

    Uninstall(appName, resvec);
    APP_LOGI("BmsInstallSystemTest TearDown--uninstall result is %{public}s", installResult.c_str());
    std::cout << "END BMS_SEARCH_2800" << std::endl;
}

/**
 * @tc.number: BMS_Search_2900
 * @tc.name: test QueryKeepAliveBundleInfos interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call QueryKeepAliveBundleInfos
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_2900, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_2900" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    std::vector<BundleInfo> bundleInfos;
    bool result = bundleMgrProxy->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_TRUE(result);
    std::cout << "END BMS_SEARCH_2900" << std::endl;
}

/**
 * @tc.number: BMS_Search_3100
 * @tc.name: test GetAbilityLabel interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call GetAbilityLabel
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_3100" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";
    std::string abilityName = "MainAbility";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    std::vector<PermissionDef> permissionDefs;
    std::string result = bundleMgrProxy->GetAbilityLabel(appName, abilityName);
    EXPECT_NE(result, "EMPTY_STRING");
    std::cout << "END BMS_SEARCH_3100" << std::endl;
}

/**
 * @tc.number: BMS_Search_3200
 * @tc.name: test QueryAbilityInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call QueryAbilityInfo
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_3200" << std::endl;
    std::string appName = BASE_BUNDLE_NAME + "1";
    std::string abilityName = "bmsThirdBundle_A1";

    CommonTool commonTool;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(appName);
    want.SetElement(name);

    AbilityInfo abilityInfo;
    bool queryResult = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
    EXPECT_TRUE(queryResult);

    EXPECT_EQ(abilityInfo.name, "bmsThirdBundle_A1");
    EXPECT_EQ(abilityInfo.bundleName, "com.third.hiworld.example1");
    EXPECT_EQ(commonTool.VectorToStr(abilityInfo.deviceTypes), "tvcar");
    std::cout << "END BMS_SEARCH_3200" << std::endl;
}

/**
 * @tc.number: BMS_Search_3300
 * @tc.name: test GetBundleInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call GetBundleInfo
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3300, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Search_3300" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";

    CommonTool commonTool;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    BundleInfo bundleInfo;
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(appName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(getInfoResult);
    EXPECT_EQ(bundleInfo.name, appName);
    EXPECT_EQ(bundleInfo.label, "bmsThirdBundle_A1 Ability");
    EXPECT_EQ(commonTool.VectorToStr(bundleInfo.hapModuleNames), "com.third.hiworld.example.h1");
    EXPECT_EQ(commonTool.VectorToStr(bundleInfo.moduleNames), "testability");
    std::cout << "END BMS_Search_3300" << std::endl;
}

/**
 * @tc.number: BMS_Search_3400
 * @tc.name: test GetApplicationInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call GetApplicationInfo
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3400, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_3400" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    ApplicationInfo appInfo;
    int userId = 0;
    bool getInfoResult =
        bundleMgrProxy->GetApplicationInfo(appName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
    EXPECT_TRUE(getInfoResult);
    EXPECT_EQ(appInfo.name, appName);
    EXPECT_EQ(appInfo.description, "himusic main ability");
    EXPECT_EQ(appInfo.iconPath, "$media:ic_launcher");
    EXPECT_EQ(appInfo.supportedModes, 0);
    std::cout << "END BMS_SEARCH_3400" << std::endl;
}

/**
 * @tc.number: BMS_Search_3500
 * @tc.name: test GetHapModuleInfo interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call GetHapModuleInfo
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3500, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_3500" << std::endl;

    std::string appName = BASE_BUNDLE_NAME + "1";
    std::string abilityName = "bmsThirdBundle1_A1";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = appName;
    abilityInfo.package = BASE_BUNDLE_NAME + ".h1";
    HapModuleInfo hapModuleInfo;

    bool queryResult = bundleMgrProxy->GetHapModuleInfo(abilityInfo, hapModuleInfo);
    EXPECT_EQ(hapModuleInfo.name, "com.third.hiworld.example.h1");
    EXPECT_EQ(hapModuleInfo.moduleName, "testability");
    EXPECT_TRUE(queryResult);
    std::cout << "END BMS_SEARCH_3500" << std::endl;
}

/**
 * @tc.number: BMS_Search_3600
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test UnregisterBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.call RegisterBundleStatusCallback
 *           3.install the app
 *           4.call UnregisterBundleStatusCallback
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3600, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_3600" << std::endl;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    std::vector<std::string> resvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    std::string appName = BASE_BUNDLE_NAME + "2";

    CommonTool commonTool;
    sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
    EXPECT_NE(bundleStatusCallback, nullptr);
    bundleStatusCallback->SetBundleName(appName);
    bundleMgrProxy->RegisterBundleStatusCallback(bundleStatusCallback);
    Install(bundleFilePath, InstallFlag::NORMAL, resvec);
    std::string installResult = commonTool.VectorToStr(resvec);
    EXPECT_EQ(installResult, "Success") << "install fail!";
    bool queryResult = QueryJsonFile(appName);
    EXPECT_TRUE(queryResult);
    Uninstall(appName, resvec);
    std::cout << "END BMS_SEARCH_3600" << std::endl;
    bundleMgrProxy->UnregisterBundleStatusCallback();
}

/**
 * @tc.number: BMS_Search_3700
 * @tc.name: 1.test RegisterBundleStatusCallback interface
 *           2.test ClearBundleStatusCallback interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.call RegisterBundleStatusCallback
 *           3.install the app
 *           4.call ClearBundleStatusCallback
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3700, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_3700" << std::endl;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    std::vector<std::string> resvec;
    std::string firstFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle8.hap";
    std::string firstAppName = BASE_BUNDLE_NAME + "3";
    std::string secondFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    std::string secondAppName = BASE_BUNDLE_NAME + "2";

    CommonTool commonTool;
    sptr<BundleStatusCallbackImpl> firstBundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
    EXPECT_NE(firstBundleStatusCallback, nullptr);
    firstBundleStatusCallback->SetBundleName(firstAppName);
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

    bundleMgrProxy->ClearBundleStatusCallback(firstBundleStatusCallback);
    for (int32_t i = INDEX_TWO; i <= INDEX_THREE; i++) {
        std::string bundleName = BASE_BUNDLE_NAME + std::to_string(i);
        std::vector<std::string> resvec;
        Uninstall(bundleName, resvec);
        std::string installResult = commonTool.VectorToStr(resvec);
        APP_LOGI("BmsInstallSystemTest TearDown--uninstall result is %{public}s", installResult.c_str());
    }
    std::cout << "END BMS_SEARCH_3700" << std::endl;
}

/**
 * @tc.number: BMS_Search_3800
 * @tc.name: test CleanBundleCacheFiles interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call CleanBundleCacheFiles
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3800, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_3800" << std::endl;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    std::string appName = BASE_BUNDLE_NAME + "1";

    const std::string testCacheFileNamE1 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/name1.txt";
    const std::string testCacheFileNamE2 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/name2.txt";
    std::ofstream file1(testCacheFileNamE1);
    std::ofstream file2(testCacheFileNamE2);
    file1.close();
    file2.close();
    int name1Exist = access(testCacheFileNamE1.c_str(), F_OK);
    EXPECT_EQ(name1Exist, 0);
    int name2Exist = access(testCacheFileNamE2.c_str(), F_OK);
    EXPECT_EQ(name2Exist, 0);
    sptr<CleanCacheCallBackImpl> bundleCleanCacheCallback = (new (std::nothrow) CleanCacheCallBackImpl());
    EXPECT_NE(bundleCleanCacheCallback, nullptr);
    bundleMgrProxy->CleanBundleCacheFiles(appName, bundleCleanCacheCallback);
    EXPECT_TRUE(bundleCleanCacheCallback->GetSucceededResult());
    name1Exist = access(testCacheFileNamE1.c_str(), F_OK);
    EXPECT_NE(name1Exist, 0) << "the cache test file1 exists.";
    name2Exist = access(testCacheFileNamE2.c_str(), F_OK);
    EXPECT_NE(name2Exist, 0) << "the cache test file2 exists.";
    std::cout << "END BMS_SEARCH_3800" << std::endl;
}

/**
 * @tc.number: BMS_Search_3900
 * @tc.name: test CleanBundleDataFiles interface
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an app
 *           2.install the app
 *           3.call CleanBundleDataFiles
 */
HWTEST_F(BmsSearchSystemTest, BMS_Search_3900, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SEARCH_3900" << std::endl;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    std::string appName = BASE_BUNDLE_NAME + "1";

    const std::string testCacheFileNamE1 = BUNDLE_DATA_ROOT_PATH + appName + "/files/name1.txt";
    const std::string testCacheFileNamE2 = BUNDLE_DATA_ROOT_PATH + appName + "/files/name2.txt";
    std::ofstream file1(testCacheFileNamE1);
    std::ofstream file2(testCacheFileNamE2);
    file1.close();
    file2.close();
    int name1Exist = access(testCacheFileNamE1.c_str(), F_OK);
    EXPECT_EQ(name1Exist, 0) << "the test file1 exists.";
    int name2Exist = access(testCacheFileNamE2.c_str(), F_OK);
    EXPECT_EQ(name2Exist, 0) << "the test file2 exists.";
    bundleMgrProxy->CleanBundleDataFiles(appName, 0);
    name1Exist = access(testCacheFileNamE1.c_str(), F_OK);
    EXPECT_NE(name1Exist, 0) << "the test file1 exists.";
    name2Exist = access(testCacheFileNamE2.c_str(), F_OK);
    EXPECT_NE(name2Exist, 0) << "the test file2 exists.";
    std::cout << "END BMS_SEARCH_3900" << std::endl;
}
}  // namespace AppExecFwk
}  // namespace OHOS
