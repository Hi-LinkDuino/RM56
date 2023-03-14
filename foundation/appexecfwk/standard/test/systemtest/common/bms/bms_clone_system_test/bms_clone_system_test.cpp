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
#include "nlohmann/json.hpp"

#include "ability_manager_interface.h"
#include "app_log_wrapper.h"
#include "bundle_installer_interface.h"
#include "bundle_mgr_proxy.h"
#include "bundle_mgr_interface.h"
#include "bundle_constants.h"
#include "common_tool.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "iservice_registry.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "status_receiver_host.h"

using OHOS::AAFwk::Want;
using namespace OHOS::EventFwk;
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
const std::string BUNDLE_CODE_ROOT_PATH = "/data/app/el1/bundle/public/";
const std::string ERROR_INSTALL_FAILED = "install failed!";
const std::string ERROR_UNINSTALL_FAILED = "uninstall failed!";
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILURE = "Failure";
const std::string OPERATION_SUCCESS = "Success";
const std::string START_SUCCESS = "start ability success";
using CloneInfo = struct {
    int uid;
    bool isCloned;
};
const int64_t TIMEOUT = 10;
const size_t CLONEINFO_SIZE = 2;
const size_t NONECLONEINFO_SIZE = 1;
const size_t ONE_SIZE = 1;
constexpr int USERID = 0;
constexpr int CLONEUID = 20000000;
const std::string THIRED1_EVENT = "ACTS_Third1_Clone_Publish_CommonEvent";
const std::string KEY_BUNDLENAME = "bundleName";
const std::string KEY_ABILITYNAME = "abilityName";
const std::string START_BUNDLENAME = "com.example.bmsstartability";
const std::string START_ABILITYNAME = "com.example.bmsstartability.MainAbility";
const std::string START_BUNDLE_PATH = "/data/test/bms_bundle/bmsStartAbility.hap";
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
    APP_LOGI("on finished result is %{public}d, %{public}s", resultCode, resultMsg.c_str());
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
    std::chrono::seconds timeout(TIMEOUT);
    if (future.wait_for(timeout) == std::future_status::timeout) {
        return OPERATION_FAILURE + " timeout";
    }
    std::string resultMsg = future.get();
    if (resultMsg == MSG_SUCCESS) {
        return OPERATION_SUCCESS;
    }
    return OPERATION_FAILURE + resultMsg;
}

class BmsCloneSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void InstallBundle(
        const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMsg);
    static void UninstallBundle(const std::string &bundleName, std::string &uninstallMsg);
    static void UninstallBundle(
        const std::string &bundleName, const std::string &modulePackage, std::string &uninstallMsg);
    static sptr<IBundleMgr> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
    int CheckFileExist(const std::string &str) const;
    static void Complete(const std::string message);
    static bool Wait(const std::string message);
    static void Clean()
    {
        message_ = "";
        completeMessage_.clear();
    }
    static void CheckCloneInfo(std::vector<CloneInfo> &cloneInfo);
    static void CheckAbilityInfo(AbilityInfo &originInfo, AbilityInfo &cloneInfo);
    static void GetCloneInfo(
        const std::vector<BundleInfo> &bundleInfos, const std::string &bundleName, std::vector<CloneInfo> &cloneInfos);

private:
    static std::mutex mutex_;
    static std::condition_variable cv_;
    static std::string message_;
    static std::vector<std::string> completeMessage_;
};

void BmsCloneSystemTest::SetUpTestCase()
{}

void BmsCloneSystemTest::TearDownTestCase()
{}

void BmsCloneSystemTest::SetUp()
{}

void BmsCloneSystemTest::TearDown()
{}

sptr<IBundleMgr> BmsCloneSystemTest::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_LOGI("fail to get system ability mgr.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_LOGI("fail to get bundle manager proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle manager proxy success.");
    return iface_cast<IBundleMgr>(remoteObject);
}

sptr<IBundleInstaller> BmsCloneSystemTest::GetInstallerProxy()
{
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGI("bundle mgr proxy is nullptr.");
        return nullptr;
    }

    sptr<IBundleInstaller> installerProxy = bundleMgrProxy->GetBundleInstaller();
    if (!installerProxy) {
        APP_LOGI("fail to get bundle installer proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle installer proxy success.");
    return installerProxy;
}
void BmsCloneSystemTest::InstallBundle(
    const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMsg)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGI("get bundle installer Failure.");
        installMsg = "Failure";
        return;
    }
    InstallParam installParam;
    installParam.installFlag = installFlag;
    sptr<StatusReceiverImpl> statusReceiver(new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    bool installResult = installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    EXPECT_TRUE(installResult);
    installMsg = statusReceiver->GetResultMsg();
}

void BmsCloneSystemTest::UninstallBundle(const std::string &bundleName, std::string &uninstallMsg)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGI("get bundle installer Failure.");
        uninstallMsg = "Failure";
        return;
    }

    if (bundleName.empty()) {
        APP_LOGI("bundelname is null.");
        uninstallMsg = "Failure";
    } else {
        sptr<StatusReceiverImpl> statusReceiver(new (std::nothrow) StatusReceiverImpl());
        EXPECT_NE(statusReceiver, nullptr);
        InstallParam installParam;
        bool uninstallResult = installerProxy->Uninstall(bundleName, installParam, statusReceiver);
        EXPECT_TRUE(uninstallResult);
        uninstallMsg = statusReceiver->GetResultMsg();
    }
}

void BmsCloneSystemTest::UninstallBundle(
    const std::string &bundleName, const std::string &modulePackage, std::string &uninstallMsg)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGI("get bundle installer Failure.");
        uninstallMsg = "Failure";
        return;
    }

    if (bundleName.empty() || modulePackage.empty()) {
        APP_LOGI("bundelname is null.");
        uninstallMsg = "Failure";
    } else {
        sptr<StatusReceiverImpl> statusReceiver(new (std::nothrow) StatusReceiverImpl());
        EXPECT_NE(statusReceiver, nullptr);
        InstallParam installParam;
        bool uninstallResult = installerProxy->Uninstall(bundleName, modulePackage, installParam, statusReceiver);
        EXPECT_TRUE(uninstallResult);
        uninstallMsg = statusReceiver->GetResultMsg();
    }
}

int BmsCloneSystemTest::CheckFileExist(const std::string &str) const
{
    int bundleDataExist = access((str).c_str(), F_OK);
    return bundleDataExist;
}
std::string BmsCloneSystemTest::message_ = "";
std::vector<std::string> BmsCloneSystemTest::completeMessage_ {};
std::condition_variable BmsCloneSystemTest::cv_ = std::condition_variable();
std::mutex BmsCloneSystemTest::mutex_ = std::mutex();
void BmsCloneSystemTest::Complete(const std::string message)
{
    std::this_thread::sleep_for(500ms);
    std::unique_lock<std::mutex> lock(mutex_);
    if (message_.compare(message) == 0) {
        GTEST_LOG_(INFO) << "Complete.";
        cv_.notify_all();
        message_ = "";
        return;
    }
    GTEST_LOG_(INFO) << "Can't Complete." << message_;
    completeMessage_.emplace_back(message);
    return;
}

bool BmsCloneSystemTest::Wait(const std::string message)
{
    GTEST_LOG_(INFO) << "Wait start.";
    std::unique_lock<std::mutex> lock(mutex_);
    message_ = message;
    if (!message_.empty()) {
        for (size_t i = 0; i < completeMessage_.size(); i++) {
            if (message_.compare(completeMessage_.at(i)) == 0) {
                message_ = "";
                return true;
            }
        }
    }
    if (cv_.wait_for(lock, std::chrono::seconds(TIMEOUT)) == std::cv_status::timeout) {
        GTEST_LOG_(INFO) << "Time out.";
        message_ = "";
        return false;
    }
    GTEST_LOG_(INFO) << "Wait done.";
    return true;
}

void BmsCloneSystemTest::CheckCloneInfo(std::vector<CloneInfo> &cloneInfos)
{
    EXPECT_LT(cloneInfos[0].uid, CLONEUID);
    EXPECT_GT(cloneInfos[1].uid, CLONEUID);
    EXPECT_FALSE(cloneInfos[0].isCloned);
    EXPECT_TRUE(cloneInfos[1].isCloned);
}

void BmsCloneSystemTest::CheckAbilityInfo(AbilityInfo &originInfo, AbilityInfo &cloneInfo)
{
    GTEST_LOG_(INFO) << "================CheckAbilityInfo==============:";
    EXPECT_EQ(originInfo.package, cloneInfo.package);
    EXPECT_EQ(originInfo.name, cloneInfo.name);
    EXPECT_EQ(originInfo.description, cloneInfo.description);
    EXPECT_EQ(originInfo.iconPath, cloneInfo.iconPath);
    EXPECT_EQ(originInfo.uri, cloneInfo.uri);
    EXPECT_EQ(originInfo.moduleName, cloneInfo.moduleName);
    EXPECT_EQ(originInfo.process, cloneInfo.process);
    EXPECT_EQ(originInfo.targetAbility, cloneInfo.targetAbility);
    EXPECT_EQ(originInfo.appName, cloneInfo.appName);
    EXPECT_EQ(originInfo.privacyUrl, cloneInfo.privacyUrl);
    EXPECT_EQ(originInfo.privacyName, cloneInfo.privacyName);
    EXPECT_EQ(originInfo.downloadUrl, cloneInfo.downloadUrl);
    EXPECT_EQ(originInfo.versionName, cloneInfo.versionName);
    EXPECT_EQ(originInfo.backgroundModes, cloneInfo.backgroundModes);
    EXPECT_EQ(originInfo.packageSize, cloneInfo.packageSize);
    EXPECT_EQ(originInfo.visible, cloneInfo.visible);
    EXPECT_EQ(originInfo.formEnabled, cloneInfo.formEnabled);
    EXPECT_EQ(originInfo.multiUserShared, cloneInfo.multiUserShared);
    EXPECT_EQ(originInfo.type, cloneInfo.type);
    EXPECT_EQ(originInfo.launchMode, cloneInfo.launchMode);
    EXPECT_EQ(originInfo.permissions, cloneInfo.permissions);
    EXPECT_EQ(originInfo.deviceTypes, cloneInfo.deviceTypes);
    EXPECT_EQ(originInfo.bundleName, cloneInfo.bundleName);
    EXPECT_EQ(originInfo.className, cloneInfo.className);
    EXPECT_EQ(originInfo.deviceId, cloneInfo.deviceId);
    EXPECT_EQ(originInfo.iconId, cloneInfo.iconId);
    EXPECT_EQ(originInfo.labelId, cloneInfo.labelId);
}

void BmsCloneSystemTest::GetCloneInfo(
    const std::vector<BundleInfo> &bundleInfos, const std::string &bundleName, std::vector<CloneInfo> &cloneInfos)
{
    for (auto bundleInfo : bundleInfos) {
        if (bundleInfo.name == bundleName) {
            CloneInfo info;
            info.uid = bundleInfo.uid;
            info.isCloned = bundleInfo.applicationInfo.isCloned;
            cloneInfos.emplace_back(info);
        }
    }
}

/**
 * @tc.number: BMS_BundleClone_0100
 * @tc.name: test the BundleClone interface
 * @tc.desc: 1. install a third-party app
 *           2. clone this third-party app
 *           3. check data dir and code dir of the cloned app
 */
HWTEST_F(BmsCloneSystemTest, BMS_BundleClone_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_BundleClone_0100";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    bool cloneResult = bundleMgrProxy->BundleClone(bundleName);
    EXPECT_TRUE(cloneResult);
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        std::string cloneDataPath = BUNDLE_DATA_ROOT_PATH + bundleName + "#" + std::to_string(cloneInfos[1].uid);
        std::string cloneCodePath = BUNDLE_CODE_ROOT_PATH + bundleName + "#" + std::to_string(cloneInfos[1].uid);
        EXPECT_EQ(CheckFileExist(cloneDataPath), 0);
        EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    }
    UninstallBundle(bundleName, installresvec);
    EXPECT_EQ(installresvec, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_BundleClone_0100";
}

/**
 * @tc.number: BMS_BundleClone_0200
 * @tc.name: test repeately call the BundleClone interface
 * @tc.desc: 1. install a third-party app
 *           2. repeately clone this third-party app
 */
HWTEST_F(BmsCloneSystemTest, BMS_BundleClone_0200, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_BundleClone_0200";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    EXPECT_FALSE(bundleMgrProxy->BundleClone(bundleName));
    EXPECT_FALSE(bundleMgrProxy->BundleClone(bundleName));
    UninstallBundle(bundleName, installresvec);
    EXPECT_EQ(installresvec, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_BundleClone_0200";
}

/**
 * @tc.number: BMS_BundleClone_0300
 * @tc.name: test the BundleClone interface by empty bundleName
 * @tc.desc: 1.install a third-party application
 *           2.clone this application by empty bundleName
 */
HWTEST_F(BmsCloneSystemTest, BMS_BundleClone_0300, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_BundleClone_0300";
    std::string bundleName = "";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    bool cloneResult = bundleMgrProxy->BundleClone(bundleName);
    EXPECT_FALSE(cloneResult);
    GTEST_LOG_(INFO) << "END BMS_BundleClone_0300";
}

/**
 * @tc.number: BMS_BundleClone_0400
 * @tc.name: test the  BundleClone interface used bundleName that is not in trustlist
 * @tc.desc: 1.install a third-party application that is not in trustlist
 *           2.clone this third-party app
 */
HWTEST_F(BmsCloneSystemTest, BMS_BundleClone_0400, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_BundleClone_0400";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string bundleName = "com.example.third2";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    bool cloneResult = bundleMgrProxy->BundleClone(bundleName);
    EXPECT_FALSE(cloneResult);
    UninstallBundle(bundleName, installresvec);
    EXPECT_EQ(installresvec, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_BundleClone_0400";
}

/**
 * @tc.number: BMS_BundleClone_0500
 * @tc.name: test the  BundleClone interface by invalid bundleName
 * @tc.desc: 1.install a third-party application
 *           2.clone this application with an invalid bundle name
 */
HWTEST_F(BmsCloneSystemTest, BMS_BundleClone_0500, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_BundleClone_0500";
    std::string bundleName = "com.example.error";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    bool cloneResult = bundleMgrProxy->BundleClone(bundleName);
    EXPECT_FALSE(cloneResult);
    GTEST_LOG_(INFO) << "END BMS_BundleClone_0500";
}

/**
 * @tc.number: BMS_BundleClone_0600
 * @tc.name: test the  BundleClone interface by update bundleName
 * @tc.desc: 1.install a third-party app
 *           2.clone this app
 *           3.update this app
 *           4.clone the updated app
 */
HWTEST_F(BmsCloneSystemTest, BMS_BundleClone_0600, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_BundleClone_0600";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleUpFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
    }
    InstallBundle(bundleUpFilePath, InstallFlag::REPLACE_EXISTING, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    EXPECT_FALSE(bundleMgrProxy->BundleClone(bundleName));
    bundleInfos.clear();
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    std::vector<uint32_t> versionCodes;
    for (auto bundleInfo : bundleInfos) {
        if (bundleInfo.name == bundleName) {
            versionCodes.emplace_back(bundleInfo.versionCode);
        }
    }
    EXPECT_EQ(versionCodes.size(), CLONEINFO_SIZE);
    if (!versionCodes.empty()) {
        EXPECT_NE(*(versionCodes.begin()), *(versionCodes.end() - 1));
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_BundleClone_0600";
}

/**
 * @tc.number: BMS_BundleClone_0700
 * @tc.name: clone a application includes two haps
 * @tc.desc: 1.install a third-party application includes two haps
 *           2.check cloned application's data directory doesn't have module directory, ontology application has module
 *               directory
 */
HWTEST_F(BmsCloneSystemTest, BMS_BundleClone_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_BundleClone_0700";
    std::string installresvec;
    std::string bundleFilePath1 = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::string bundleName = "com.example.third1";
    std::string entryModule = "com.example.third1";
    std::string featureModule = "com.example.third3";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath1, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    InstallBundle(bundleFilePath2, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    EXPECT_EQ(CheckFileExist(BUNDLE_DATA_ROOT_PATH + bundleName + "/" + entryModule), 0);
    EXPECT_EQ(CheckFileExist(BUNDLE_DATA_ROOT_PATH + bundleName + "/" + featureModule), 0);
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        EXPECT_EQ(CheckFileExist(BUNDLE_DATA_ROOT_PATH + bundleName + "#" + std::to_string(cloneInfos[1].uid)), 0);
        int isExist = 0;
        isExist = CheckFileExist(
            BUNDLE_DATA_ROOT_PATH + bundleName + "#" + std::to_string(cloneInfos[1].uid) + "/" + entryModule);
        EXPECT_NE(isExist, 0);
        isExist = CheckFileExist(
            BUNDLE_DATA_ROOT_PATH + bundleName + "#" + std::to_string(cloneInfos[1].uid) + "/" + featureModule);
        EXPECT_NE(isExist, 0);
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_BundleClone_0700";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0100
 * @tc.name: test the interface of RemoveClonedBundle
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.remove the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0100";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        std::string cloneDataPath = BUNDLE_DATA_ROOT_PATH + bundleName + "#" + std::to_string(cloneInfos[1].uid);
        std::string cloneCodePath = BUNDLE_CODE_ROOT_PATH + bundleName + "#" + std::to_string(cloneInfos[1].uid);
        EXPECT_EQ(CheckFileExist(cloneDataPath), 0);
        EXPECT_NE(CheckFileExist(cloneCodePath), 0);
        EXPECT_TRUE(bundleMgrProxy->RemoveClonedBundle(bundleName, cloneInfos[1].uid));
        EXPECT_NE(CheckFileExist(cloneDataPath), 0);
        EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0100";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0200
 * @tc.name: uninstall app to remove the cloned app
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.uninstall the application to remove the cloneed application
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0200";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    std::string cloneDataPath = BUNDLE_DATA_ROOT_PATH + bundleName + "#";
    std::string cloneCodePath = BUNDLE_CODE_ROOT_PATH + bundleName + "#";
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        cloneDataPath += std::to_string(cloneInfos[1].uid);
        cloneCodePath += std::to_string(cloneInfos[1].uid);
    }
    EXPECT_EQ(CheckFileExist(cloneDataPath), 0);
    EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    EXPECT_NE(CheckFileExist(cloneDataPath), 0);
    EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0200";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0300
 * @tc.name: update the application doesn't change cloned application
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.update the application
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0300";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleUpFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    std::string cloneDataPath = BUNDLE_DATA_ROOT_PATH + bundleName + "#";
    std::string cloneCodePath = BUNDLE_CODE_ROOT_PATH + bundleName + "#";
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        cloneDataPath += std::to_string(cloneInfos[1].uid);
        cloneCodePath += std::to_string(cloneInfos[1].uid);
    }
    EXPECT_EQ(CheckFileExist(cloneDataPath), 0);
    EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    InstallBundle(bundleUpFilePath, InstallFlag::REPLACE_EXISTING, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    EXPECT_EQ(CheckFileExist(cloneDataPath), 0);
    EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    EXPECT_NE(CheckFileExist(cloneDataPath), 0);
    EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0300";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0400
 * @tc.name: remove cloned apps from uncloned apps
 * @tc.desc: 1.install a third-party application
 *           2.use the bundleName and use the uid of the ontology application to stop using the clone
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0400, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0400";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    BundleInfo bundleInfo;
    bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(bundleMgrProxy->RemoveClonedBundle(bundleName, bundleInfo.uid));
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0400";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0500
 * @tc.name: test the RemoveClonedBundle interface by invalid param
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.use invalid bundleName and uid to stop the application clone
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0500, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0500";
    std::string bundleName = "com.example.error";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_FALSE(bundleMgrProxy->RemoveClonedBundle(bundleName, Constants::INVALID_UID));
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0500";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0600
 * @tc.name: test the RemoveClonedBundle interface by ontology application uid
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.use the uid of the ontology application to remove the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0600, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0600";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        EXPECT_FALSE(bundleMgrProxy->RemoveClonedBundle(bundleName, cloneInfos[0].uid));
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0600";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0700
 * @tc.name: test the RemoveClonedBundle interface by empty bundleName
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.use an empty bundleName to remove the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0700, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0700";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        EXPECT_FALSE(bundleMgrProxy->RemoveClonedBundle(bundleName, cloneInfos[0].uid));
        EXPECT_FALSE(bundleMgrProxy->RemoveClonedBundle("", cloneInfos[1].uid));
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0700";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0800
 * @tc.name: test repeately call the interface RemoveClonedBundle
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.repeately remove the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0800, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0800";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        EXPECT_TRUE(bundleMgrProxy->RemoveClonedBundle(bundleName, cloneInfos[1].uid));
        EXPECT_FALSE(bundleMgrProxy->RemoveClonedBundle(bundleName, cloneInfos[1].uid));
        EXPECT_FALSE(bundleMgrProxy->RemoveClonedBundle(bundleName, cloneInfos[1].uid));
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0800";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_0900
 * @tc.name: uninstall app to remove the cloned app
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.uninstall the application's feature module
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_0900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_0900";
    std::string installresvec;
    std::string bundleFilePath1 = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::string entryModule = "com.example.third1";
    std::string featureModule = "com.example.third3";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath1, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    InstallBundle(bundleFilePath2, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    std::string cloneUid;
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        cloneUid = std::to_string(cloneInfos[1].uid);
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, featureModule, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    EXPECT_EQ(CheckFileExist(BUNDLE_DATA_ROOT_PATH + bundleName + "#" + cloneUid), 0);
    EXPECT_EQ(CheckFileExist(BUNDLE_DATA_ROOT_PATH + bundleName + "/" + entryModule), 0);
    EXPECT_NE(CheckFileExist(BUNDLE_DATA_ROOT_PATH + bundleName + "/" + featureModule), 0);
    EXPECT_EQ(CheckFileExist(BUNDLE_CODE_ROOT_PATH + bundleName + "/" + entryModule), 0);
    EXPECT_NE(CheckFileExist(BUNDLE_CODE_ROOT_PATH + bundleName + "/" + featureModule), 0);
    UninstallBundle(bundleName, entryModule, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    EXPECT_NE(CheckFileExist(BUNDLE_CODE_ROOT_PATH + bundleName), 0);
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_0900";
}

/**
 * @tc.number: BMS_RemoveClonedBundle_1000
 * @tc.name: clone system preset application, remove cloned application.
 * @tc.desc: 1.clone this application
 *           2.uninstall the application to remove the cloneed application failed
 *           3.call RemoveClonedBundle interface to remove the cloneed application success
 */
HWTEST_F(BmsCloneSystemTest, BMS_RemoveClonedBundle_1000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_RemoveClonedBundle_1000";
    std::string bundleName = "com.ohos.settings";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<BundleInfo> bundleInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos));
    GetCloneInfo(bundleInfos, bundleName, cloneInfos);
    std::string cloneDataPath = BUNDLE_DATA_ROOT_PATH + bundleName + "#";
    std::string cloneCodePath = BUNDLE_CODE_ROOT_PATH + bundleName + "#";
    uint32_t cloneUid = 0;
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
        cloneUid = cloneInfos[1].uid;
        cloneDataPath += std::to_string(cloneInfos[1].uid);
        cloneCodePath += std::to_string(cloneInfos[1].uid);
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Failure[MSG_ERR_UNINSTALL_SYSTEM_APP_ERROR]") << "uninstall success!";
    EXPECT_EQ(CheckFileExist(cloneDataPath), 0);
    EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    EXPECT_TRUE(bundleMgrProxy->RemoveClonedBundle(bundleName, cloneUid));
    EXPECT_NE(CheckFileExist(cloneDataPath), 0);
    EXPECT_NE(CheckFileExist(cloneCodePath), 0);
    GTEST_LOG_(INFO) << "END BMS_RemoveClonedBundle_1000";
}

/**
 * @tc.number: BMS_CloneGetApplicationInfos_0100
 * @tc.name: test the GetApplicationInfos interface to get cloned application's info
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.get the application information of the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_CloneGetApplicationInfos_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_CloneGetApplicationInfos_0100";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    std::vector<ApplicationInfo> appInfos;
    std::vector<CloneInfo> cloneInfos;
    EXPECT_TRUE(bundleMgrProxy->GetApplicationInfos(ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, appInfos));
    for (auto appInfo : appInfos) {
        if (appInfo.name == bundleName) {
            CloneInfo info;
            info.uid = appInfo.uid;
            info.isCloned = appInfo.isCloned;
            cloneInfos.emplace_back(info);
        }
    }
    EXPECT_EQ(cloneInfos.size(), CLONEINFO_SIZE);
    if (cloneInfos.size() == CLONEINFO_SIZE) {
        CheckCloneInfo(cloneInfos);
    }
    UninstallBundle(bundleName, installresvec);
    EXPECT_EQ(installresvec, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_CloneGetApplicationInfos_0100";
}

/**
 * @tc.number: BMS_CloneGetApplicationInfos_0200
 * @tc.name: test the GetApplicationInfos interface
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.remove this cloned application, get the application information of the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_CloneGetApplicationInfos_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_CloneGetApplicationInfos_0200";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    std::vector<ApplicationInfo> appInfos;
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName("com.example.third1.MainAbility");
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_TRUE(bundleMgrProxy->RemoveClonedBundle(bundleName, abilityInfos[0].applicationInfo.uid));
    EXPECT_TRUE(bundleMgrProxy->GetApplicationInfos(ApplicationFlag::GET_BASIC_APPLICATION_INFO, USERID, appInfos));
    for (auto appInfo : appInfos) {
        if (appInfo.name == bundleName) {
            EXPECT_FALSE(appInfo.isCloned);
            EXPECT_LT(appInfo.uid, CLONEUID);
        }
    }
    UninstallBundle(bundleName, installresvec);
    EXPECT_EQ(installresvec, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_CloneGetApplicationInfos_0200";
}

/**
 * @tc.number: BMS_QueryAbilityInfosForClone_0100
 * @tc.name: test the QueryAbilityInfosForClone interface to get ontology and cloned applications' info
 * @tc.name: test the QueryAbilityInfosForClone interface to get uncloned application's abilityInfo
 * @tc.desc: 1.install a third-party application
 *           2.call the QueryAbilityInfosForClone interface to get the ability information of
 *              the ontology and cloned applications
 */
HWTEST_F(BmsCloneSystemTest, BMS_QueryAbilityInfosForClone_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfosForClone_0100";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    bool cloneResult = bundleMgrProxy->BundleClone(bundleName);
    EXPECT_TRUE(cloneResult);
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    if (abilityInfos.size() == CLONEINFO_SIZE) {
        CheckAbilityInfo(abilityInfos[0], abilityInfos[1]);
        EXPECT_TRUE(abilityInfos.at(0).applicationInfo.isCloned);
        EXPECT_FALSE(abilityInfos.at(1).applicationInfo.isCloned);
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_BundleClone_0100";
}

/**
 * @tc.number: BMS_QueryAbilityInfosForClone_0200
 * @tc.name: test the QueryAbilityInfosForClone interface to get uncloned application's abilityInfo
 * @tc.desc: 1.install a third-party application
 *           2.call the QueryAbilityInfosForClone interface to get uncloned application's abilityInfo
 */
HWTEST_F(BmsCloneSystemTest, BMS_QueryAbilityInfosForClone_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfosForClone_0200";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_EQ(abilityInfos.size(), NONECLONEINFO_SIZE);
    if (abilityInfos.size() == NONECLONEINFO_SIZE) {
        EXPECT_EQ(abilityInfos.at(0).name, abilityName);
        EXPECT_EQ(abilityInfos.at(0).bundleName, bundleName);
        EXPECT_FALSE(abilityInfos.at(0).applicationInfo.isCloned);
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfosForClone_0200";
}

/**
 * @tc.number: BMS_QueryAbilityInfosForClone_0300
 * @tc.name: test the QueryAbilityInfosForClone interface to get abilityInfo which has removed cloned application
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.remove this cloned application
 *           4.get the ability information of the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_QueryAbilityInfosForClone_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfosForClone_0300";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    EXPECT_TRUE(bundleMgrProxy->RemoveClonedBundle(bundleName, abilityInfos.at(0).applicationInfo.uid));
    abilityInfos.clear();
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_EQ(abilityInfos.size(), NONECLONEINFO_SIZE);
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfosForClone_0300";
}

/**
 * @tc.number: BMS_QueryAbilityInfosForClone_0400
 * @tc.name: test the QueryAbilityInfosForClone interface to get abilityInfo
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.uninstall this application
 *           4.get the ability information of the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_QueryAbilityInfosForClone_0400, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfosForClone_0400";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    EXPECT_FALSE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfosForClone_0400";
}

/**
 * @tc.number: BMS_QueryAbilityInfosForClone_0500
 * @tc.name: test the QueryAbilityInfosForClone interface to get abilityInfo from an application includes two haps
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.get the ability information of the cloned application
 */
HWTEST_F(BmsCloneSystemTest, BMS_QueryAbilityInfosForClone_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfosForClone_0500";
    std::string installresvec;
    std::string bundleFilePath1 = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::string bundleName = "com.example.third1";
    std::string entryModule = "com.example.third1";
    std::string featureModule = "com.example.third3";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath1, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    InstallBundle(bundleFilePath2, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    if (abilityInfos.size() == CLONEINFO_SIZE) {
        EXPECT_EQ(abilityInfos.at(0).name, abilityInfos.at(1).name);
        EXPECT_EQ(abilityInfos.at(0).name, abilityName);
        EXPECT_EQ(abilityInfos.at(0).bundleName, abilityInfos.at(1).bundleName);
        EXPECT_TRUE(abilityInfos.at(0).applicationInfo.isCloned);
        EXPECT_FALSE(abilityInfos.at(1).applicationInfo.isCloned);
    }
    abilityInfos.clear();
    std::string uninstallResult;
    UninstallBundle(bundleName, entryModule, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    EXPECT_NE(CheckFileExist(BUNDLE_CODE_ROOT_PATH + bundleName + "/" + entryModule), 0);
    EXPECT_EQ(CheckFileExist(BUNDLE_CODE_ROOT_PATH + bundleName + "/" + featureModule), 0);
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_EQ(abilityInfos.size(), NONECLONEINFO_SIZE);
    if (!abilityInfos.empty()) {
        EXPECT_TRUE(abilityInfos[0].applicationInfo.isCloned);
    }
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfosForClone_0500";
}

/**
 * @tc.number: BMS_QueryAbilityInfosForClone_0600
 * @tc.name: test the QueryAbilityInfosForClone interface to get abilityInfo from an application includes two haps
 * @tc.desc: 1.install a third-party application
 *           2.call the QueryAbilityInfosForClone interface to get application's abilityInfo by invalid bundleName
 */
HWTEST_F(BmsCloneSystemTest, BMS_QueryAbilityInfosForClone_0600, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfosForClone_0600";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetBundleName("");
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_FALSE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_TRUE(abilityInfos.size() == 0);
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfosForClone_0600";
}

/**
 * @tc.number: BMS_QueryAbilityInfosForClone_0700
 * @tc.name: test the QueryAbilityInfosForClone interface to get abilityInfo from an application includes two haps
 * @tc.desc: 1.install a third-party application
 *           2.call the QueryAbilityInfosForClone interface to get application's abilityInfo by invalid abilityName
 */
HWTEST_F(BmsCloneSystemTest, BMS_QueryAbilityInfosForClone_0700, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfosForClone_0700";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third3.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName("");
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_FALSE(bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos));
    EXPECT_TRUE(abilityInfos.size() == 0);
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfosForClone_0700";
}

/**
 * @tc.number: BMS_CloneTestGetBundleNameForUid_0100
 * @tc.name: test the GetBundleNameForUid interface to get bundleName by cloned application's uid
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.get the bundleName by cloned application's uid
 */
HWTEST_F(BmsCloneSystemTest, BMS_CloneTestGetBundleNameForUid_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_CloneTestGetBundleNameForUid_0100";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    bool cloneResult = bundleMgrProxy->BundleClone(bundleName);
    EXPECT_TRUE(cloneResult);
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos);
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    if (abilityInfos.size() == CLONEINFO_SIZE) {
        std::string getBundleName1;
        std::string getBundleName2;
        EXPECT_TRUE(bundleMgrProxy->GetBundleNameForUid(abilityInfos.at(0).applicationInfo.uid, getBundleName1));
        EXPECT_TRUE(bundleMgrProxy->GetBundleNameForUid(abilityInfos.at(1).applicationInfo.uid, getBundleName2));
        EXPECT_EQ(getBundleName1, getBundleName2);
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_CloneTestGetBundleNameForUid_0100";
}

/**
 * @tc.number: BMS_CloneTestGetBundlesForUid_0100
 * @tc.name: test the GetBundlesForUid interface to get bundleInfos by cloned application's uid
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.get the bundleInfos by cloned application's uid
 */
HWTEST_F(BmsCloneSystemTest, BMS_CloneTestGetBundlesForUid_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_CloneTestGetBundlesForUid_0100";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    bool cloneResult = bundleMgrProxy->BundleClone(bundleName);
    EXPECT_TRUE(cloneResult);
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos);
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    if (abilityInfos.size() == CLONEINFO_SIZE) {
        std::vector<std::string> bundleNames1;
        std::vector<std::string> bundleNames2;
        EXPECT_TRUE(bundleMgrProxy->GetBundlesForUid(abilityInfos.at(0).applicationInfo.uid, bundleNames1));
        EXPECT_TRUE(bundleMgrProxy->GetBundlesForUid(abilityInfos.at(1).applicationInfo.uid, bundleNames2));
        EXPECT_EQ(bundleNames1.size(), ONE_SIZE);
        EXPECT_EQ(bundleNames2.size(), ONE_SIZE);
        EXPECT_EQ(bundleNames1.at(0), bundleNames2.at(0));
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_CloneTestGetBundlesForUid_0100";
}

/**
 * @tc.number: BMS_CloneTestGetNameForUid_0100
 * @tc.name: test the BMS_CloneTestGetNameForUid_0100 interface to get name by cloned application's uid
 * @tc.desc: 1.install a third-party application
 *           2.clone this application
 *           3.get the name by cloned application's uid
 */
HWTEST_F(BmsCloneSystemTest, BMS_CloneTestGetNameForUid_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_CloneTestGetNameForUid_0100";
    std::string installresvec;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installresvec);
    EXPECT_EQ(installresvec, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        GTEST_LOG_(INFO) << "bundle mgr proxy is nullptr.";
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    bool cloneResult = bundleMgrProxy->BundleClone(bundleName);
    EXPECT_TRUE(cloneResult);
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    std::vector<AbilityInfo> abilityInfos;
    bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos);
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    if (abilityInfos.size() == CLONEINFO_SIZE) {
        std::string getName1;
        std::string getName2;
        EXPECT_TRUE(bundleMgrProxy->GetNameForUid(abilityInfos.at(0).applicationInfo.uid, getName1));
        EXPECT_TRUE(bundleMgrProxy->GetNameForUid(abilityInfos.at(1).applicationInfo.uid, getName2));
        EXPECT_EQ(getName1, getName2);
    }
    std::string uninstallResult;
    UninstallBundle(bundleName, uninstallResult);
    EXPECT_EQ(uninstallResult, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_CloneTestGetNameForUid_0100";
}

/**
 * @tc.number: BMS_CloneTestQueryAbilityInfosByUri_0100
 * @tc.name: test the BMS_CloneTestQueryAbilityInfosByUri_0100 interface to get cloned data ability's ablityInfos
 * @tc.desc: 1.install a third-party application include data ability
 *           2.clone this application
 *           3.get the ablityInfos by application's uri
 */
HWTEST_F(BmsCloneSystemTest, BMS_CloneTestQueryAbilityInfosByUri_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_CloneTestQueryAbilityInfosByUri_0100";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundleDataAbility.hap";
    std::string bundleName = "com.example.dataability";
    std::string abilityName = "com.example.dataability.MainAbility";
    std::string uri = "dataability://com.test.demo.dataability.UserADataAbility";
    std::string message;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    std::string abilityUri = "dataability:///com.test.demo.dataability.UserADataAbility";
    std::vector<AbilityInfo> abilityInfos;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_TRUE(false);
    }
    EXPECT_TRUE(bundleMgrProxy->BundleClone(bundleName));
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfosByUri(abilityUri, abilityInfos));
    EXPECT_EQ(abilityInfos.size(), CLONEINFO_SIZE);
    for (size_t i = 0, len = abilityInfos.size(); i < len; i++) {
        EXPECT_EQ(abilityInfos[i].name, abilityName);
        EXPECT_EQ(abilityInfos[i].bundleName, bundleName);
        EXPECT_EQ(abilityInfos[i].uri, uri);
    }
    UninstallBundle(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_CloneTestQueryAbilityInfosByUri_0100";
}
}  // namespace AppExecFwk
}  // namespace OHOS