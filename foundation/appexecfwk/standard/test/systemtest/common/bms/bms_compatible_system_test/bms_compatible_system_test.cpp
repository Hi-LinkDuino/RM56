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

#include <condition_variable>
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_installer_interface.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "common_tool.h"
#include "iservice_registry.h"
#include "status_receiver_host.h"
#include "system_ability_definition.h"

namespace {
const std::string THIRD_BUNDLE_PATH = "/data/test/bms_bundle/";
const std::string SYSTEM_BUNDLE_PATH = "/system/app/";
const std::string THIRD_BASE_BUNDLE_NAME = "com.example.third";
const std::string SYSTEM_BASE_BUNDLE_NAME = "com.example.system";
const std::string CAMERA = "ohos.permission.CAMERA";
const std::string RESOURCE_ROOT_PATH = "/data/test/";
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILED = "Failure";
const std::string OPERATION_SUCCESS = "Success";
const int MIN_HEIGHT = 50;
const int MIN_WIDTH = 100;
const int DEFAULT_HEIGHT = 100;
const int DEFAULT_WIDTH = 200;
const uint32_t FORM_NUM = 3;
const size_t CESINFO_LEN = 2;
}  // namespace
using OHOS::AAFwk::Want;
using namespace testing::ext;
using namespace std::chrono_literals;
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

class BmsCompatibleSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void Install(const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMessage);
    static void Install(
        const std::vector<std::string> &bundleFilePaths, const InstallFlag installFlag, std::string &installMessage);
    static void Uninstall(const std::string &bundleName, std::string &installMessage);
    static sptr<IBundleMgr> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
};

sptr<IBundleMgr> BmsCompatibleSystemTest::GetBundleMgrProxy()
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

sptr<IBundleInstaller> BmsCompatibleSystemTest::GetInstallerProxy()
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

void BmsCompatibleSystemTest::Install(
    const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMessage)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        installMessage = OPERATION_FAILED;
        return;
    }
    InstallParam installParam;
    installParam.installFlag = installFlag;
    installParam.userId = Constants::DEFAULT_USERID;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    installMessage = statusReceiver->GetResultMsg();
}

void BmsCompatibleSystemTest::Install(
    const std::vector<std::string> &bundleFilePaths, const InstallFlag installFlag, std::string &installMessage)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        installMessage = OPERATION_FAILED;
        return;
    }
    InstallParam installParam;
    installParam.installFlag = installFlag;
    installParam.userId = Constants::DEFAULT_USERID;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    installerProxy->Install(bundleFilePaths, installParam, statusReceiver);
    installMessage = statusReceiver->GetResultMsg();
}

void BmsCompatibleSystemTest::Uninstall(const std::string &bundleName, std::string &uninstallMessage)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer failed.");
        uninstallMessage = OPERATION_FAILED;
        return;
    }

    if (bundleName.empty()) {
        APP_LOGE("bundelname is null.");
        uninstallMessage = OPERATION_FAILED;
    } else {
        InstallParam installParam;
        installParam.userId = Constants::DEFAULT_USERID;
        sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
        EXPECT_NE(statusReceiver, nullptr);
        installerProxy->Uninstall(bundleName, installParam, statusReceiver);
        uninstallMessage = statusReceiver->GetResultMsg();
    }
}

void BmsCompatibleSystemTest::SetUpTestCase()
{}
void BmsCompatibleSystemTest::TearDownTestCase()
{}
void BmsCompatibleSystemTest::SetUp()
{}
void BmsCompatibleSystemTest::TearDown()
{}

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

static void CheckCompatibleAbilityInfo(
    const CompatibleAbilityInfo &compatibleAbilityInfo, const std::string &bundleName, const std::string &abilityName)
{
    CommonTool commonTool;
    EXPECT_EQ(compatibleAbilityInfo.name, abilityName);
    EXPECT_EQ(compatibleAbilityInfo.label, "$string:app_name");
    EXPECT_EQ(compatibleAbilityInfo.description, "$string:mainability_description");
    EXPECT_EQ(compatibleAbilityInfo.iconPath, "$media:icon");
    EXPECT_EQ(compatibleAbilityInfo.moduleName, "entry");
    EXPECT_TRUE(compatibleAbilityInfo.visible);
    EXPECT_EQ(compatibleAbilityInfo.type, AbilityType::PAGE);
    EXPECT_EQ(compatibleAbilityInfo.orientation, DisplayOrientation::UNSPECIFIED);
    EXPECT_EQ(compatibleAbilityInfo.launchMode, LaunchMode::STANDARD);
    EXPECT_EQ(commonTool.VectorToStr(compatibleAbilityInfo.deviceTypes), "phone");
    EXPECT_FALSE(compatibleAbilityInfo.supportPipMode);
    EXPECT_EQ(compatibleAbilityInfo.bundleName, bundleName);
    EXPECT_EQ(compatibleAbilityInfo.formEntity, FORM_NUM);
    EXPECT_EQ(compatibleAbilityInfo.minFormHeight, MIN_HEIGHT);
    EXPECT_EQ(compatibleAbilityInfo.defaultFormHeight, DEFAULT_HEIGHT);
    EXPECT_EQ(compatibleAbilityInfo.minFormWidth, MIN_WIDTH);
    EXPECT_EQ(compatibleAbilityInfo.defaultFormWidth, DEFAULT_WIDTH);
    EXPECT_TRUE(compatibleAbilityInfo.enabled);
}
static void CheckCompatibleApplicationInfo(
    const CompatibleApplicationInfo &compatibleApplicationInfo, const std::string &bundleName)
{
    EXPECT_EQ(compatibleApplicationInfo.name, bundleName);
    EXPECT_EQ(compatibleApplicationInfo.label, "$string:app_name");
    EXPECT_EQ(compatibleApplicationInfo.description, "$string:mainability_description");
    if (!compatibleApplicationInfo.moduleInfos.empty()) {
        EXPECT_EQ(compatibleApplicationInfo.moduleInfos[0].moduleName, "entry");
        EXPECT_EQ(compatibleApplicationInfo.moduleInfos[0].moduleSourceDir,
            "/data/app/el1/bundle/public/com.example.third1/com.example.third1");
    }
    EXPECT_TRUE(compatibleApplicationInfo.enabled);
}

/**
 * @tc.number: BMS_ConvertToCompatible_0100
 * @tc.name: test the interface of ConvertToCompatiableAbilityInfo
 * @tc.desc: 1.install a normal hap
 *           2.query ability info by want
 *           3.get the compatible ability info of the hap by ConvertToCompatiableAbilityInfo
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0100";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0100";
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);
    AbilityInfo abilityInfo;
    CompatibleAbilityInfo compatibleAbilityInfo;
    bool result = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0100";
    abilityInfo.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
    EXPECT_EQ(compatibleAbilityInfo.name, abilityName);
    EXPECT_EQ(compatibleAbilityInfo.bundleName, bundleName);
    CheckCompatibleAbilityInfo(compatibleAbilityInfo, bundleName, abilityName);
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0100";
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0100";
}
/**
 * @tc.number: BMS_ConvertToCompatible_0200
 * @tc.name: test the interface of ConvertToCompatiableAbilityInfo
 * @tc.desc: 1.install a low version hap
 *           2.install a high version hap
 *           3.get the compatible ability info by ConvertToCompatiableAbilityInfo
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0200";
    std::string bundleFilePath1 = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string abilityName = "com.example.third1.AMainAbility";
    std::string message;

    Install(bundleFilePath1, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    Install(bundleFilePath2, InstallFlag::REPLACE_EXISTING, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    AbilityInfo abilityInfo;
    CompatibleAbilityInfo compatibleAbilityInfo;
    bool result = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
    EXPECT_TRUE(result);
    abilityInfo.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
    CheckCompatibleAbilityInfo(compatibleAbilityInfo, bundleName, abilityName);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0200";
}
/**
 * @tc.number: BMS_ConvertToCompatible_0300
 * @tc.name: test the interface of ConvertToCompatiableAbilityInfo
 * @tc.desc: 1.install a hap with two ability
 *           2.query the ability infos by want
 *           3.get the compatible ability info by ConvertToCompatiableAbilityInfo
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0300";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle5.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "5";
    std::vector<std::string> abilityNames = {"com.example.third5.AMainAbility", "com.example.third5.BMainAbility"};
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    want.SetElement(name);
    want.SetAction("action.system.home");
    std::vector<AbilityInfo> abilityInfos;
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfos(want, abilityInfos));
    for (size_t i = 0; i < abilityInfos.size(); i++) {
        CompatibleAbilityInfo compatibleAbilityInfo;
        abilityInfos[i].ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
        GTEST_LOG_(INFO) << "ability name is :" << compatibleAbilityInfo.name;
        EXPECT_EQ(compatibleAbilityInfo.name, abilityNames[i]);
    }
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0300";
}
/**
 * @tc.number: BMS_ConvertToCompatible_0400
 * @tc.name: test the interface of ConvertToCompatiableAbilityInfo
 * @tc.desc: 1.install a hap with moduletype of entry
 *           2.install a hap with moduletype of feature
 *           2.get compatible ability infos of two types of haps
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0400";
    std::string bundleFilePath1 = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::vector<std::string> abilityNames = {"com.example.third1.MainAbility", "com.example.third3.MainAbility"};
    std::string bundleName = "com.example.third1";
    std::string message;

    Install(bundleFilePath1, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    Install(bundleFilePath2, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    want.SetElement(name);
    want.SetAction("action.system.home");
    std::vector<AbilityInfo> abilityInfos;
    CompatibleAbilityInfo compatibleAbilityInfo;
    EXPECT_TRUE(bundleMgrProxy->QueryAbilityInfos(want, abilityInfos));
    EXPECT_TRUE(!abilityInfos.empty());
    for (size_t i = 0; i < abilityInfos.size(); i++) {
        abilityInfos[i].ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
        GTEST_LOG_(INFO) << "ability name is :" << compatibleAbilityInfo.name;
        EXPECT_EQ(compatibleAbilityInfo.name, abilityNames[i]);
    }
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0400";
}
/**
 * @tc.number: BMS_ConvertToCompatible_0500
 * @tc.name: test the interface of ConvertToCompatiableAbilityInfo
 * @tc.desc: 1.install a normal hap with an ability
 *           2.get compatible ability info after uninstalling the hap
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0500, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0500";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    AbilityInfo abilityInfo;
    CompatibleAbilityInfo compatibleAbilityInfo;
    bool result = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
    EXPECT_FALSE(result);
    abilityInfo.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
    GTEST_LOG_(INFO) << "compatibleAbilityInfo.name" << compatibleAbilityInfo.name;
    EXPECT_EQ(compatibleAbilityInfo.name, "");
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0500";
}
/**
 * @tc.number: BMS_ConvertToCompatible_0600
 * @tc.name: test the interface of ConvertToCompatiableAbilityInfo
 * @tc.desc: get compatible ability info of invalid bundleName and abilityName
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0600, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0600";
    std::string bundleName = "";
    std::string abilityName = "";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    Want want;
    ElementName name;
    name.SetBundleName(bundleName);
    name.SetAbilityName(abilityName);
    want.SetElement(name);
    AbilityInfo abilityInfo;
    CompatibleAbilityInfo compatibleAbilityInfo;
    bool result = bundleMgrProxy->QueryAbilityInfo(want, abilityInfo);
    EXPECT_FALSE(result);
    abilityInfo.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
    EXPECT_EQ(compatibleAbilityInfo.name, "");
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0600";
}
/**
 * @tc.number: BMS_ConvertToCompatible_0700
 * @tc.name: test the interface of ConvertToCompatibleApplicationInfo
 * @tc.desc: 1.install a normal hap
 *           2.query application info by bundleName
 *           3.get the compatible application info of the hap by ConvertToCompatibleApplicationInfo
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0700";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string abilityName = "com.example.third1.MainAbility";
    std::string message;
    int userId = Constants::DEFAULT_USERID;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    CompatibleApplicationInfo compatibleApplicationInfo;
    bool result =
        bundleMgrProxy->GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
    EXPECT_TRUE(result);
    appInfo.ConvertToCompatibleApplicationInfo(compatibleApplicationInfo);
    EXPECT_EQ(compatibleApplicationInfo.name, bundleName);
    CheckCompatibleApplicationInfo(compatibleApplicationInfo, bundleName);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0700";
}
/**
 * @tc.number: BMS_ConvertToCompatible_0800
 * @tc.name: test the interface of ConvertToCompatibleApplicationInfo
 * @tc.desc: 1.install a normal hap with permission
 *           2.query application info by bundleName
 *           3.get the compatible application info of the hap by ConvertToCompatibleApplicationInfo
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0800";
    std::string message;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle6.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "6";
    std::string CAMERA = "ohos.permission.CAMERA";
    int userId = Constants::DEFAULT_USERID;

    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    CompatibleApplicationInfo compatibleApplicationInfo;
    bool result = bundleMgrProxy->GetApplicationInfo(
        bundleName, ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, appInfo);
    EXPECT_TRUE(result);
    appInfo.ConvertToCompatibleApplicationInfo(compatibleApplicationInfo);
    EXPECT_EQ(compatibleApplicationInfo.name, bundleName);
    CommonTool commonTool;
    std::string permissions = commonTool.VectorToStr(compatibleApplicationInfo.permissions);
    EXPECT_EQ(permissions, CAMERA);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0800";
}
/**
 * @tc.number: BMS_ConvertToCompatible_0900
 * @tc.name: test the interface of ConvertToCompatibleApplicationInfo
 * @tc.desc: get the compatible application info of the system hap by ConvertToCompatibleApplicationInfo
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_0900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_0900";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle1.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + "1";
    std::string abilityName = "com.example.system1.MainAbility";
    int userId = Constants::DEFAULT_USERID;
    std::string message;

    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    CompatibleApplicationInfo compatibleApplicationInfo;
    bool result =
        bundleMgrProxy->GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
    EXPECT_TRUE(result);
    appInfo.ConvertToCompatibleApplicationInfo(compatibleApplicationInfo);
    EXPECT_EQ(compatibleApplicationInfo.name, bundleName);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_0900";
}
/**
 * @tc.number: BMS_ConvertToCompatible_1000
 * @tc.name: test the interface of ConvertToCompatibleApplicationInfo
 * @tc.desc: 1.install a low version hap
 *           2.install a high version hap
 *           3.get the compatible application info by ConvertToCompatibleApplicationInfo
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_1000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_1000";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string bundleFilePath1 = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";
    std::string message;
    int userId = Constants::DEFAULT_USERID;

    Install(bundleFilePath1, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    Install(bundleFilePath2, InstallFlag::REPLACE_EXISTING, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    CompatibleApplicationInfo compatibleApplicationInfo;
    bool result =
        bundleMgrProxy->GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
    EXPECT_TRUE(result);
    appInfo.ConvertToCompatibleApplicationInfo(compatibleApplicationInfo);
    EXPECT_EQ(compatibleApplicationInfo.name, bundleName);
    CheckCompatibleApplicationInfo(compatibleApplicationInfo, bundleName);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_1000";
}
/**
 * @tc.number: BMS_ConvertToCompatible_1100
 * @tc.name: test the interface of ConvertToCompatibleApplicationInfo
 * @tc.desc: 1.install a normal hap with an ability
 *           2.get compatible application info after uninstalling the hap
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_1100, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_1100";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = "com.example.third1";
    std::string message;
    int userId = Constants::DEFAULT_USERID;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    CompatibleApplicationInfo compatibleApplicationInfo;
    bool result =
        bundleMgrProxy->GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
    EXPECT_FALSE(result);
    appInfo.ConvertToCompatibleApplicationInfo(compatibleApplicationInfo);
    EXPECT_EQ(compatibleApplicationInfo.name, "");
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_1100";
}
/**
 * @tc.number: BMS_ConvertToCompatible_1200
 * @tc.name: test the interface of ConvertToCompatibleApplicationInfo
 * @tc.desc: get compatible application info of invalid bundleName
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_ConvertToCompatible_1200, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_ConvertToCompatible_1200";
    std::string bundleName = "";
    int userId = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    CompatibleApplicationInfo compatibleApplicationInfo;
    bool result =
        bundleMgrProxy->GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
    EXPECT_FALSE(result);
    appInfo.ConvertToCompatibleApplicationInfo(compatibleApplicationInfo);
    EXPECT_EQ(compatibleApplicationInfo.name, "");
    GTEST_LOG_(INFO) << "END BMS_ConvertToCompatible_1200";
}
/**
 * @tc.number: BMS_QueryAbilityInfoByUri_0100
 * @tc.name: test the interface of QueryAbilityInfoByUri
 * @tc.desc: 1.install a third hap with data ability
 *           2.get the ability info by the correct ability uri
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_QueryAbilityInfoByUri_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfoByUri_0100";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundleDataAbility.hap";
    std::string bundleName = "com.example.dataability";
    std::string abilityName = "com.example.dataability.MainAbility";
    std::string uri = "dataability://com.test.demo.dataability.UserADataAbility";
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    std::string abilityUri = "dataability:///com.test.demo.dataability.UserADataAbility";
    AbilityInfo abilityInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_TRUE(false);
    }
    bool result = bundleMgrProxy->QueryAbilityInfoByUri(abilityUri, 0, abilityInfo);
    EXPECT_TRUE(result);
    EXPECT_EQ(abilityInfo.name, abilityName);
    EXPECT_EQ(abilityInfo.bundleName, bundleName);
    EXPECT_EQ(abilityInfo.uri, uri);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfoByUri_0100";
}
/**
 * @tc.number: BMS_QueryAbilityInfoByUri_0200
 * @tc.name: test the interface of QueryAbilityInfoByUri
 * @tc.desc: get the ability info by the invalid ability uri
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_QueryAbilityInfoByUri_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfoByUri_0200";
    std::string abilityUri = "";
    AbilityInfo abilityInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_TRUE(false);
    }
    bool result = bundleMgrProxy->QueryAbilityInfoByUri(abilityUri, 0, abilityInfo);
    EXPECT_FALSE(result);
    EXPECT_EQ(abilityInfo.name, "");
    EXPECT_EQ(abilityInfo.bundleName, "");
    EXPECT_EQ(abilityInfo.uri, "");
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfoByUri_0200";
}
/**
 * @tc.number: BMS_QueryAbilityInfoByUri_0300
 * @tc.name: test the interface of QueryAbilityInfoByUri
 * @tc.desc: get the ability info by the ability uri with wrong form
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_QueryAbilityInfoByUri_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_QueryAbilityInfoByUri_0300";
    std::string abilityUri = "err://com.test.demo.weatherfa.UserADataAbility";
    AbilityInfo abilityInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_TRUE(false);
    }
    bool result = bundleMgrProxy->QueryAbilityInfoByUri(abilityUri, 0, abilityInfo);
    EXPECT_FALSE(result);
    EXPECT_EQ(abilityInfo.name, "");
    EXPECT_EQ(abilityInfo.bundleName, "");
    EXPECT_EQ(abilityInfo.uri, "");
    GTEST_LOG_(INFO) << "END BMS_QueryAbilityInfoByUri_0300";
}

/**
 * @tc.number: BMS_GetAllCommonEventInfo_0100
 * @tc.name: test the interface of GetAllCommonEventInfo
 * @tc.desc: 1. install a system app
 *           2. get the commonEventInfos by the eventKey
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetAllCommonEventInfo_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetAllCommonEventInfo_0100";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    std::vector<CommonEventInfo> commonEventInfos;
    std::string eventKey = "BMS_TESTCOMMONEVNET_SYSTEM2A";
    EXPECT_TRUE(bundleMgrProxy->GetAllCommonEventInfo(eventKey, commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == 1);
    BundleInfo bundleInfo;
    bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    for (size_t i = 0, len = commonEventInfos.size(); i < len; i++) {
        EXPECT_EQ(commonEventInfos[i].name, ".MainAbility");
        EXPECT_EQ(commonEventInfos[i].bundleName, bundleName);
        EXPECT_EQ(commonEventInfos[i].uid, bundleInfo.uid);
        EXPECT_EQ(commonEventInfos[i].permission, "BMS_COMMEVENT_PERMISSIONA");
        for (size_t j = 0, lenth = commonEventInfos[i].data.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[i].data[j], "data_system2" + std::string(1, 'A' + j));
        }
        for (size_t j = 0, lenth = commonEventInfos[i].type.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[i].type[j], "type_system2" + std::string(1, 'A' + j));
        }
        for (size_t j = 0, lenth = commonEventInfos[i].events.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[i].events[j], "BMS_TESTCOMMONEVNET_SYSTEM2" + std::string(1, 'A' + j));
        }
    }
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetAllCommonEventInfo_0100";
}

/**
 * @tc.number: BMS_GetAllCommonEventInfo_0200
 * @tc.name: test the interface of GetAllCommonEventInfo
 * @tc.desc: get the commonEventInfos by the empty eventKey
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetAllCommonEventInfo_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetAllCommonEventInfo_0200";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    std::vector<CommonEventInfo> commonEventInfos;
    EXPECT_FALSE(bundleMgrProxy->GetAllCommonEventInfo("", commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == 0);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetAllCommonEventInfo_0200";
}

/**
 * @tc.number: BMS_GetAllCommonEventInfo_0300
 * @tc.name: test the interface of GetAllCommonEventInfo
 * @tc.desc: 1. install two system app, both configs have the same events
 *           2. get the commonEventInfos by the eventKey
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetAllCommonEventInfo_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetAllCommonEventInfo_0300";
    std::string bundleFilePath1 = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2.hap";
    std::string bundleFilePath2 = SYSTEM_BUNDLE_PATH + "bmsSystemBundle3.hap";
    std::string bundleName1 = SYSTEM_BASE_BUNDLE_NAME + '2';
    std::string bundleName2 = SYSTEM_BASE_BUNDLE_NAME + '3';
    std::string message;
    Install(bundleFilePath1, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    Install(bundleFilePath2, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    std::vector<CommonEventInfo> commonEventInfos;
    std::string eventKey = " ";
    EXPECT_TRUE(bundleMgrProxy->GetAllCommonEventInfo(eventKey, commonEventInfos));
    EXPECT_EQ(commonEventInfos.size(), CESINFO_LEN);
    for (size_t i = 0, len = commonEventInfos.size(); i < len; i++) {
        EXPECT_EQ(commonEventInfos[i].bundleName, SYSTEM_BASE_BUNDLE_NAME + std::to_string(i + 2));
        EXPECT_EQ(commonEventInfos[i].permission, "BMS_COMMEVENT_PERMISSIONA");
        for (size_t j = 0, lenth = commonEventInfos[i].data.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[i].data[j], "data_system" + std::to_string(i + 2) + std::string(1, 'C' + j));
        }
        for (size_t j = 0, lenth = commonEventInfos[i].type.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[i].type[j], "type_system" + std::to_string(i + 2) + std::string(1, 'C' + j));
        }
        EXPECT_EQ(commonEventInfos[i].events[0], " ");
        EXPECT_EQ(commonEventInfos[i].events[1], "6666666");
    }
    Uninstall(bundleName1, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    Uninstall(bundleName2, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetAllCommonEventInfo_0300";
}

/**
 * @tc.number: BMS_GetAllCommonEventInfo_0400
 * @tc.name: test the interface of GetAllCommonEventInfo
 * @tc.desc: 1. install a system app, then update it by a new hap which config has different events from it
 *           2. get the commonEventInfos by the eventKey
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetAllCommonEventInfo_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetAllCommonEventInfo_0400";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2.hap";
    std::string bundleFileUpdatePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2Update.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    std::vector<CommonEventInfo> commonEventInfos;
    std::string eventKey = "BMS_TESTCOMMONEVNET_SYSTEM2A";
    EXPECT_TRUE(bundleMgrProxy->GetAllCommonEventInfo(eventKey, commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == 1);
    Install(bundleFileUpdatePath, InstallFlag::REPLACE_EXISTING, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    commonEventInfos.clear();
    EXPECT_FALSE(bundleMgrProxy->GetAllCommonEventInfo(eventKey, commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == 0);
    eventKey = "BMS_TESTCOMMONEVNET_SYSTEM2UPDATE_A";
    EXPECT_TRUE(bundleMgrProxy->GetAllCommonEventInfo(eventKey, commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == 1);
    BundleInfo bundleInfo;
    bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    if (!commonEventInfos.empty()) {
        EXPECT_EQ(commonEventInfos[0].bundleName, bundleName);
        EXPECT_EQ(commonEventInfos[0].uid, bundleInfo.uid);
        EXPECT_EQ(commonEventInfos[0].permission, "BMS_COMMEVENT_UPDATE_PERMISSIONA");
        for (size_t j = 0, lenth = commonEventInfos[0].data.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[0].data[j], "data_system2U" + std::string(1, 'A' + j));
        }
        for (size_t j = 0, lenth = commonEventInfos[0].type.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[0].type[j], "type_system2U" + std::string(1, 'A' + j));
        }
        EXPECT_EQ(commonEventInfos[0].events[0], "BMS_TESTCOMMONEVNET_SYSTEM2UPDATE_A");
    }
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetAllCommonEventInfo_0400";
}

/**
 * @tc.number: BMS_GetAllCommonEventInfo_0500
 * @tc.name: test the interface of GetAllCommonEventInfo
 * @tc.desc: 1. install a system app
 *           2. get the commonEventInfos by the eventKey
 *           3. uninstall this system app
 *           4. get the commonEventInfos by the eventKey
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetAllCommonEventInfo_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetAllCommonEventInfo_0500";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    std::vector<CommonEventInfo> commonEventInfos;
    std::string eventKey = "BMS_TESTCOMMONEVNET_SYSTEM2A";
    EXPECT_TRUE(bundleMgrProxy->GetAllCommonEventInfo(eventKey, commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == 1);
    EXPECT_EQ(commonEventInfos[0].bundleName, bundleName);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    commonEventInfos.clear();
    EXPECT_FALSE(bundleMgrProxy->GetAllCommonEventInfo(eventKey, commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == 0);
    GTEST_LOG_(INFO) << "END BMS_GetAllCommonEventInfo_0500";
}

/**
 * @tc.number: BMS_GetAllCommonEventInfo_0600
 * @tc.name: test the interface of GetAllCommonEventInfo
 * @tc.desc: 1. install a system app includs two haps, both configs have the same events
 *           2. get the commonEventInfos by the eventKey
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetAllCommonEventInfo_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetAllCommonEventInfo_0600";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2.hap";
    std::string bundleFeatureFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2Feature.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    Install(bundleFeatureFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    std::vector<CommonEventInfo> commonEventInfos;
    std::string eventKey = "BMS_TESTCOMMONEVNET_SYSTEM2A";
    EXPECT_TRUE(bundleMgrProxy->GetAllCommonEventInfo(eventKey, commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == CESINFO_LEN);
    BundleInfo bundleInfo;
    bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    for (size_t i = 0, len = commonEventInfos.size(); i < len; i++) {
        EXPECT_EQ(commonEventInfos[i].bundleName, bundleName);
        EXPECT_EQ(commonEventInfos[i].uid, bundleInfo.uid);
        for (size_t j = 0, lenth = commonEventInfos[i].data.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[i].data[j], "data_system" + std::to_string(i + 2) + std::string(1, 'A' + j));
        }
        for (size_t j = 0, lenth = commonEventInfos[i].type.size(); j < lenth; j++) {
            EXPECT_EQ(commonEventInfos[i].type[j], "type_system" + std::to_string(i + 2) + std::string(1, 'A' + j));
        }
        EXPECT_EQ(commonEventInfos[i].events.size(), CESINFO_LEN);
        if (commonEventInfos[i].events.size() == CESINFO_LEN) {
            EXPECT_EQ(commonEventInfos[0].events[0], "BMS_TESTCOMMONEVNET_SYSTEM2A");
            EXPECT_EQ(commonEventInfos[0].events[1], "BMS_TESTCOMMONEVNET_SYSTEM2B");
            EXPECT_EQ(commonEventInfos[1].events[0], "BMS_TESTCOMMONEVNET");
            EXPECT_EQ(commonEventInfos[1].events[1], "BMS_TESTCOMMONEVNET_SYSTEM2A");
        }
    }
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetAllCommonEventInfo_0600";
}

/**
 * @tc.number: BMS_GetAllCommonEventInfo_0700
 * @tc.name: test the interface of GetAllCommonEventInfo
 * @tc.desc: 1. install a third-party app
 *           2. get the commonEventInfos by the eventKey
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetAllCommonEventInfo_0700, Function | MediumTest | Level2)
{
    GTEST_LOG_(INFO) << "START BMS_GetAllCommonEventInfo_0700";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    std::vector<CommonEventInfo> commonEventInfos;
    EXPECT_FALSE(bundleMgrProxy->GetAllCommonEventInfo("BMS_TESTCOMMONEVNET_THIRD2", commonEventInfos));
    EXPECT_TRUE(commonEventInfos.size() == 0);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetAllCommonEventInfo_0700";
}

/**
 * @tc.number: BMS_GetDebug_0100
 * @tc.name: get new fileds Debug
 * @tc.desc: 1. install app
 *           2. call GetApplicationInfo to get debug
 *           3. uninstall the app
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetDebug_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetDebug_0100";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + '1';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    EXPECT_TRUE(bundleMgrProxy->GetApplicationInfo(
        bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, Constants::DEFAULT_USERID, appInfo));
    EXPECT_TRUE(appInfo.debug);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetDebug_0100";
}

/**
 * @tc.number: BMS_GetDebug_0200
 * @tc.name: get new fileds Debug of an app which doesn't cofig fileds
 * @tc.desc: 1. install app
 *           2. call GetApplicationInfo to get debug
 *           3. uninstall the app
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetDebug_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetDebug_0200";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    EXPECT_TRUE(bundleMgrProxy->GetApplicationInfo(
        bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, Constants::DEFAULT_USERID, appInfo));
    EXPECT_FALSE(appInfo.debug);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetDebug_0200";
}

/**
 * @tc.number: BMS_GetDebug_0300
 * @tc.name: get new fileds Debug of an app which has two haps, both filed are cofiged different value
 * @tc.desc: 1. install app's two at the different time
 *           2. call GetApplicationInfo to get debug
 *           3. uninstall the app
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetDebug_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetDebug_0300";
    std::string bundleFilePath1 = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + '1';
    std::string message;
    Install(bundleFilePath1, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    Install(bundleFilePath2, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    ApplicationInfo appInfo;
    EXPECT_TRUE(bundleMgrProxy->GetApplicationInfo(
        bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, Constants::DEFAULT_USERID, appInfo));
    EXPECT_TRUE(appInfo.debug);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetDebug_0300";
}

/**
 * @tc.number: BMS_GetIsKeepAliveAndSingleton_0100
 * @tc.name: Get the isKeepAlive and singleton information of the system application
 * @tc.desc: Get the isKeepAlive and singleton information of the system application, KeepAlive is configured to true,
 *              Singleton is configured to false
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetIsKeepAliveAndSingleton_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetIsKeepAliveAndSingleton_0100";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    BundleInfo bundleInfo;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo));
    EXPECT_TRUE(bundleInfo.isKeepAlive);
    EXPECT_FALSE(bundleInfo.singleton);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetIsKeepAliveAndSingleton_0100";
}

/**
 * @tc.number: BMS_GetIsKeepAliveAndSingleton_0200
 * @tc.name: Get the isKeepAlive and singleton information of the third-party application
 * @tc.desc: Get the isKeepAlive and singleton information of the third-party application by getBundleInfo
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetIsKeepAliveAndSingleton_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetIsKeepAliveAndSingleton_0100";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    BundleInfo bundleInfo;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo));
    EXPECT_FALSE(bundleInfo.isKeepAlive);
    EXPECT_FALSE(bundleInfo.singleton);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetIsKeepAliveAndSingleton_0200";
}

/**
 * @tc.number: BMS_GetIsKeepAliveAndSingleton_0300
 * @tc.name: Get the isKeepAlive and singleton information of the application which doesn't config those fields
 * @tc.desc: Get the isKeepAlive and singleton information of the application which doesn't config those fields
 *               by getBundleInfo(application is system)
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetIsKeepAliveAndSingleton_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetIsKeepAliveAndSingleton_0300";
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle3.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + '3';
    std::string message;
    Install(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    BundleInfo bundleInfo;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo));
    EXPECT_FALSE(bundleInfo.isKeepAlive);
    EXPECT_FALSE(bundleInfo.singleton);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetIsKeepAliveAndSingleton_0300";
}

/**
 * @tc.number: BMS_GetIsKeepAliveAndSingleton_0400
 * @tc.name: Get the isKeepAlive and singleton information of the application includes two haps
 * @tc.desc: Install two hap packages of the application at the different time, and the configuration properties in the
 *              two haps are different (application is system)
 */
HWTEST_F(BmsCompatibleSystemTest, BMS_GetIsKeepAliveAndSingleton_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "START BMS_GetIsKeepAliveAndSingleton_0400";
    std::string bundleFilePath1 = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2.hap";
    std::string bundleFilePath2 = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2Feature2.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + '2';
    std::string message;
    Install(bundleFilePath1, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        GTEST_LOG_(INFO) << ("bundle mgr proxy is nullptr.");
        EXPECT_NE(bundleMgrProxy, nullptr);
    }
    BundleInfo bundleInfo;
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo));
    EXPECT_TRUE(bundleInfo.isKeepAlive);
    EXPECT_FALSE(bundleInfo.singleton);
    Install(bundleFilePath2, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";
    EXPECT_TRUE(bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo));
    EXPECT_TRUE(bundleInfo.isKeepAlive);
    EXPECT_FALSE(bundleInfo.singleton);
    Uninstall(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    GTEST_LOG_(INFO) << "END BMS_GetIsKeepAliveAndSingleton_0400";
}
}  // namespace AppExecFwk
}  // namespace OHOS