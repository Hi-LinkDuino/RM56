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

#include <fstream>
#include <future>
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_installer_interface.h"
#include "bundle_mgr_interface.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "common_tool.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "status_receiver_host.h"

using namespace testing::ext;
using namespace std::chrono_literals;
namespace {
const std::string THIRD_BUNDLE_PATH = "/data/test/bms_bundle/";
const std::string SYSTEM_BUNDLE_PATH = "/system/app/";
const std::string CODE_ROOT_PATH = "/data/app/el1/bundle/public/";
const std::string DATA_ROOT_PATH = "/data/app/el2/100/base/";
const std::string THIRD_BASE_BUNDLE_NAME = "com.third.hiworld.example";
const std::string SYSTEM_BASE_BUNDLE_NAME = "com.system.hiworld.example";

const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILURE = "Failure";
const std::string OPERATION_SUCCESS = "Success";
const int TIMEOUT = 10;
}  // namespace

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;
class StatusReceiverImpl : public StatusReceiverHost {
public:
    StatusReceiverImpl();
    virtual ~StatusReceiverImpl() override;
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

void StatusReceiverImpl::OnStatusNotify(const int progress)
{
    EXPECT_GT(progress, iProgress_);
    iProgress_ = progress;
    APP_LOGI("OnStatusNotify progress:%{public}d", progress);
}

void StatusReceiverImpl::OnFinished(const int32_t resultCode, const std::string &resultMsg)
{
    APP_LOGD("on finished result is %{public}d, %{public}s", resultCode, resultMsg.c_str());
    resultMsgSignal_.set_value(resultMsg);
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

class SubscriberTest : public CommonEventSubscriber {
public:
    explicit SubscriberTest(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
    virtual ~SubscriberTest() = default;
    virtual void OnReceiveEvent(const CommonEventData &data) override;
    std::string GetSubscriberResultMsg() const;

private:
    mutable std::promise<std::string> subscriberMsgSignal_;

    DISALLOW_COPY_AND_MOVE(SubscriberTest);
};

void SubscriberTest::OnReceiveEvent(const CommonEventData &data)
{
    Want want = data.GetWant();
    subscriberMsgSignal_.set_value(want.GetAction());
    APP_LOGI("OnReceiveEvent:action = %{public}s", want.GetAction().c_str());
}

std::string SubscriberTest::GetSubscriberResultMsg() const
{
    auto future = subscriberMsgSignal_.get_future();
    future.wait();
    std::string resultMsg = future.get();
    return resultMsg;
}

class BmsInstallSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static int32_t ExcuteMaintainCmd(const std::string &cmd, std::vector<std::string> &cmdRes);
    static void InstallBundle(
        const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMsg, const int userId);
    static void InstallMultiBundle(const std::string bundleFilePath, bool installFlag);
    static void UninstallBundle(const std::string &bundleName, std::string &uninstallMsg, const int userId);
    void CheckBundleInfo(const std::string &version, const std::string &bundleName) const;
    bool CheckFilePath(const std::string &checkFilePath) const;
    void CheckInstallIsSuccess(const std::string &bundleName, const std::string &modulePackage,
        const std::vector<std::string> &abilityNames) const;
    void CheckFileNonExist(const std::string &bundleName) const;
    static sptr<IBundleMgr> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
    void ClearJsonFile() const;
};

sptr<IBundleMgr> BmsInstallSystemTest::GetBundleMgrProxy()
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

sptr<IBundleInstaller> BmsInstallSystemTest::GetInstallerProxy()
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

void BmsInstallSystemTest::InstallBundle(
    const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMsg, const int userId = 0)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer Failure.");
        installMsg = "Failure";
        return;
    }

    InstallParam installParam;
    installParam.installFlag = installFlag;
    if (userId != 0) {
        installParam.userId = userId;
    }
    sptr<StatusReceiverImpl> statusReceiver(new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    bool installResult = installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    EXPECT_TRUE(installResult);
    installMsg = statusReceiver->GetResultMsg();
}

void BmsInstallSystemTest::InstallMultiBundle(const std::string bundleFilePath, bool installFlag)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer Failure.");
        return;
    }
    InstallParam installParam;
    installParam.installFlag = (InstallFlag)installFlag;
    sptr<StatusReceiverImpl> statusReceiver(new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    bool installResult = installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    EXPECT_TRUE(installResult);
    APP_LOGI("Install MSG: %{public}s", statusReceiver->GetResultMsg().c_str());
}

void BmsInstallSystemTest::UninstallBundle(
    const std::string &bundleName, std::string &uninstallMsg, const int userId = 0)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        APP_LOGE("get bundle installer Failure.");
        uninstallMsg = "Failure";
        return;
    }

    sptr<StatusReceiverImpl> statusReceiver(new (std::nothrow) StatusReceiverImpl());
    EXPECT_NE(statusReceiver, nullptr);
    InstallParam installParam;
    if (userId != 0) {
        installParam.userId = userId;
    }
    bool uninstallResult = installerProxy->Uninstall(bundleName, installParam, statusReceiver);
    EXPECT_TRUE(uninstallResult);
    uninstallMsg = statusReceiver->GetResultMsg();
}

int32_t BmsInstallSystemTest::ExcuteMaintainCmd(const std::string &cmd, std::vector<std::string> &cmdRes)
{
    CommonTool commonTool;
    int32_t bufSize = 1024;
    return commonTool.ExcuteCmd(cmd, cmdRes, bufSize);
}

void BmsInstallSystemTest::CheckBundleInfo(const std::string &version, const std::string &bundleName) const
{
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool isGetInfoSuccess = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(isGetInfoSuccess);
    EXPECT_EQ(bundleInfo.name, bundleName);
    EXPECT_EQ(bundleInfo.versionName, version);
}

bool BmsInstallSystemTest::CheckFilePath(const std::string &checkFilePath) const
{
    CommonTool commonTool;
    bool checkIsExist = commonTool.CheckFilePathISExist(checkFilePath);
    if (!checkIsExist) {
        APP_LOGE("%{private}s does not exist!", checkFilePath.c_str());
        return false;
    }
    return true;
}

void BmsInstallSystemTest::CheckInstallIsSuccess(
    const std::string &bundleName, const std::string &modulePackage, const std::vector<std::string> &abilityNames) const
{
    // applications dir check
    EXPECT_TRUE(CheckFilePath(CODE_ROOT_PATH + bundleName + "/" + modulePackage + "/config.json"));
    // appdata dir check
    std::string strDataPath = DATA_ROOT_PATH + bundleName + "/" + modulePackage + "/";
    EXPECT_TRUE(CheckFilePath(DATA_ROOT_PATH + bundleName + "/cache"));
    EXPECT_TRUE(CheckFilePath(DATA_ROOT_PATH + bundleName + "/files"));
    EXPECT_TRUE(CheckFilePath(DATA_ROOT_PATH + bundleName + "/sharedPreference"));
    EXPECT_TRUE(CheckFilePath(DATA_ROOT_PATH + bundleName + "/database"));

    for (auto abilityName : abilityNames) {
        EXPECT_TRUE(CheckFilePath(strDataPath + "shared"));
        EXPECT_TRUE(CheckFilePath(strDataPath + abilityName));
        EXPECT_TRUE(CheckFilePath(strDataPath + abilityName + "/cache"));
        EXPECT_TRUE(CheckFilePath(strDataPath + abilityName + "/files"));
        EXPECT_TRUE(CheckFilePath(strDataPath + abilityName + "/sharedPreference"));
        EXPECT_TRUE(CheckFilePath(strDataPath + abilityName + "/database"));
    }
}

void BmsInstallSystemTest::CheckFileNonExist(const std::string &bundleName) const
{
    int bundleDataExist = access((DATA_ROOT_PATH + bundleName).c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0) << "the bundle data dir exists: " << bundleName;
    int codeExist = access((CODE_ROOT_PATH + bundleName).c_str(), F_OK);
    EXPECT_NE(codeExist, 0) << "the bundle code dir exists: " << bundleName;
}

void BmsInstallSystemTest::ClearJsonFile() const
{
    std::string fileName = Constants::BUNDLE_DATA_BASE_FILE;
    std::ofstream o(fileName);
    if (!o.is_open()) {
        std::cout << "failed to open as out" << fileName << std::endl;
    } else {
        std::cout << "clear" << fileName << std::endl;
    }
    o.close();
}

void BmsInstallSystemTest::SetUpTestCase()
{}

void BmsInstallSystemTest::TearDownTestCase()
{}

void BmsInstallSystemTest::SetUp()
{}

void BmsInstallSystemTest::TearDown()
{}

/**
 * @tc.number: BMS_Install_0100
 * @tc.name:  test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a third-party bundle
 *           2.install the bundle
 *           3.check the bundle info
 * @tc.require: AR000GHLL7
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_0100" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);
    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_0100" << std::endl;
}

/**
 * @tc.number: BMS_Install_0200
 * @tc.name: test the duplicate installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle'，there is a third-party bundle
 *           2.install the bundle
 *           3.reinstall the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0200, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_0200" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_ALREADY_EXIST]");

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    std::this_thread::sleep_for(50ms);
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_0200" << std::endl;
}

/**
 * @tc.number: BMS_Install_0300
 * @tc.name: test the installation of three different third-party bundles at the same time
 * @tc.desc: 1.under '/data/test/bms_bundle',there are three third-party bundles
 *           2.install the bundles at the same time
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0300, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_0300" << std::endl;
    std::string installMsg;
    int32_t max = 8;

    for (int32_t i = 6; i <= max; i++) {
        std::future<void> futureVec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        bool installFlag = 0;
        futureVec = std::async(std::launch::async, InstallMultiBundle, bundleFilePath, installFlag);
        futureVec.wait();
    }

    GTEST_LOG_(INFO) << "finish to install all bundle!";
    for (int32_t i = 1; i <= 3; i++) {
        std::string bundleName = THIRD_BASE_BUNDLE_NAME + std::to_string(i);
        std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
        std::vector<std::string> abilityNames = {};
        CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

        std::string uninstallMsg;
        UninstallBundle(bundleName, uninstallMsg);
        EXPECT_EQ(uninstallMsg, "Success");
    }
    std::cout << "END BMS_Install_0300" << std::endl;
}

/**
 * @tc.number: BMS_Install_0400
 * @tc.name: test the installation of a big third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a big third-party bundle
 *           2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0400, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_0400" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle13.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "5";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    std::vector<BundleInfo> bundleInfos;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGI("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfosResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
    EXPECT_TRUE(getInfosResult);

    bool isSubStrExist = false;
    for (auto iter = bundleInfos.begin(); iter != bundleInfos.end(); iter++) {
        if (IsSubStr(iter->name, bundleName)) {
            isSubStrExist = true;
            break;
        }
    }
    EXPECT_TRUE(isSubStrExist);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_0400" << std::endl;
}

/**
 * @tc.number: BMS_Install_0500
 * @tc.name: test the replacement of a third-party bundle,which has been installed in system
 * @tc.desc: 1.a third-party bundle has been installed in system
 *           2.under '/data/test/bms_bundle',there is a third-party bundle,whose version is lower than
 *                    the installed one
 *           3.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0500, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_0500" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "2";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> abilityNames = {};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_VERSION_DOWNGRADE]");

    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_0500" << std::endl;
}

/**
 * @tc.number: BMS_Install_0600
 * @tc.name: test the replacement of a third-party bundle ,which has been installed in system
 * @tc.desc: 1.a third-party bundle has been installed in system
 *           2.under '/data/test/bms_bundle',there is a third-party bundle,whose version is higher than
 *                    the installed bundle
 *           3.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0600, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_0600" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "2";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string bundleReFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";
    std::this_thread::sleep_for(50ms);
    InstallBundle(bundleReFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string version = "3.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_0600" << std::endl;
}

/**
 * @tc.number: BMS_Install_0700
 * @tc.name: test the installation of abnormal bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two abnormal bundles
 *           2.install the bundles
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0700, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_0700" << std::endl;
    std::string installMsg;
    std::vector<std::string> bundlePaths = {
        THIRD_BUNDLE_PATH + "bmsThirdBundle12.rpk", THIRD_BUNDLE_PATH + "bmsThirdBundle11.hap"};
    std::vector<std::string> bundleNames = {THIRD_BASE_BUNDLE_NAME + "12", THIRD_BASE_BUNDLE_NAME + "11"};
    std::vector<std::string> errorCodes = {
        "Failure[ERR_INSTALL_INVALID_HAP_NAME]", "Failure[ERR_INSTALL_PARSE_NO_PROFILE]"};

    int32_t size = bundlePaths.size();
    for (int i = 0; i < size; i++) {
        InstallBundle(bundlePaths[i], InstallFlag::NORMAL, installMsg);
        EXPECT_EQ(installMsg, errorCodes[i]);
        CheckFileNonExist(bundleNames[i]);
    }

    std::vector<BundleInfo> bundleInfos;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);

    bool isSubStrExist = false;
    for (auto iter = bundleInfos.begin(); iter != bundleInfos.end(); iter++) {
        for (std::string bundleName : bundleNames) {
            isSubStrExist = IsSubStr(iter->name, bundleName);
            EXPECT_FALSE(isSubStrExist);
        }
    }
    std::cout << "END BMS_Install_0700" << std::endl;
}

/**
 * @tc.number: BMS_Install_0800
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,which have no bundlename
 *                    in config.json
 *           2.install the bundle
 *           3.install the bundle once again
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0800, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_0800" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle14.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_MISSING_PROP]");
    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_MISSING_PROP]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "14";

    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_0800" << std::endl;
}

/**
 * @tc.number: BMS_Install_0900
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.install a third-party bundle,which is not exist
 *           2.install the bundle once again
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_0900, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_0900" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "notexist.hap";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[MSG_ERR_INSTALL_FILE_PATH_INVALID]");

    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Failure[MSG_ERR_INSTALL_FILE_PATH_INVALID]");
    std::cout << "END BMS_Install_0900" << std::endl;
}

/**
 * @tc.number: BMS_Install_1000
 * @tc.name:  test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a third-party bundle without signa
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1000, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_1000" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundleNoSign.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_NO_SIGNATURE_INFO]");

    std::string bundleName = "com.third.hiworld.example1";
    CheckFileNonExist(bundleName);
    std::cout << "END BMS_Install_1000" << std::endl;
}

/**
 * @tc.number: BMS_Install_1100
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there are two haps,whose type are entry and feature
 *           2.install the feature hap,and then install the entry hap
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1100" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle5.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap2AbilityNames);

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1100" << std::endl;
}

/**
 * @tc.number: BMS_Install_1200
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes a hap without an ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1200" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);

    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1200" << std::endl;
}

/**
 * @tc.number: BMS_Install_1300
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes a hap with an ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1300, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1300" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);

    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1300" << std::endl;
}

/**
 * @tc.number: BMS_Install_1400
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes a hap with two abilities
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1400, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1400" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);

    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1400" << std::endl;
}

/**
 * @tc.number: BMS_Install_1500
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes two haps without an ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1500, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1500" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle42.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1500" << std::endl;
}

/**
 * @tc.number: BMS_Install_1600
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes two haps,the first has one ability,the other
 *             doesn't any ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1600, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1600" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap2AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1600" << std::endl;
}

/**
 * @tc.number: BMS_Install_1700
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes two haps,the first has two abilities,the other
 *             doesn't any ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1700, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1700" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle3.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle5.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap2AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1700" << std::endl;
}

/**
 * @tc.number: BMS_Install_1800
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes two haps,the first has an ability,the other
 *             doesn't any ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1800, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1800" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle42.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1800" << std::endl;
}

/**
 * @tc.number: BMS_Install_1900
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes two haps with an ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_1900, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_1900" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap2AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_1900" << std::endl;
}

/**
 * @tc.number: BMS_Install_2000
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes two haps,the first has an ability,the other
 *             has two abilities
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2000, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_2000" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle5.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap2AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_2000" << std::endl;
}

/**
 * @tc.number: BMS_Install_2100
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there are two haps,whose type is entry,the first has two abilities,the other
 *             doesn't have an ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_2100" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle6.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[MSG_ERR_INSTALL_ENTRY_ALREADY_EXIST]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "START BMS_Install_2100" << std::endl;
}

/**
 * @tc.number: BMS_Install_2200
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes two haps,the first has two abilities,the other
 *             has an ability
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_2200" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle4.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap2AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "START BMS_Install_2200" << std::endl;
}

/**
 * @tc.number: BMS_Install_2300
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.there is an app which includes two haps with two abilities
 *           2.install the bundle
 *           3.check the bundle info
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2300, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_2300" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle5.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap2AbilityNames);

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_Install_2300" << std::endl;
}

/**
 * @tc.number: BMS_Install_2400
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,whose module-abilities-name is null
 *             in config.json
 *           2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2400, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_2400" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e23.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_CHECK_ERROR]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_2400" << std::endl;
}

/**
 * @tc.number: BMS_Install_2500
 * @tc.name:  test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two third-party bundles
 *           2.install these bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2500, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_2500" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);
    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    std::string bundleFilePath2 = THIRD_BUNDLE_PATH + "e14.hap";
    InstallBundle(bundleFilePath2, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_CHECK_ERROR]");

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_Install_2500" << std::endl;
}

/**
 * @tc.number: BMS_Install_2600
 * @tc.name:  test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,whose module-distro-moduleName is null
 *              in config.json
 *           2.install these bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2600, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_2600" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e22.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_CHECK_ERROR]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_2600" << std::endl;
}

/**
 * @tc.number: BMS_Install_2700
 * @tc.name:  test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a bundle with three abilities
 *           2.install these bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2700, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_2700" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle41.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);
    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    BundleInfo bundleInfo;
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
    EXPECT_TRUE(getInfoResult);
    for (const auto &abilityInfo : bundleInfo.abilityInfos) {
        switch (abilityInfo.type) {
            case (AbilityType::DATA):
                EXPECT_EQ(abilityInfo.name, "bmsThirdBundle_A1");
                break;
            case (AbilityType::SERVICE):
                EXPECT_EQ(abilityInfo.name, "bmsThirdBundle_A2");
                break;
            case (AbilityType::PAGE):
                EXPECT_EQ(abilityInfo.name, "bmsThirdBundle_A3");
                break;
            default:
                std::cout << "no this ability" << std::endl;
                break;
        }
    }

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_Install_2700" << std::endl;
}

/**
 * @tc.number: BMS_Install_2800
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,whose bundlename is not according to
 * specification in config.json 2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2800, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_2800" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e3.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_CHECK_ERROR]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_2800" << std::endl;
}

/**
 * @tc.number: BMS_Install_2900
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,whose module-abilities-type is null
 *             in config.json
 *           2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_2900, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_2900" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e21.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_CHECK_ERROR]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_2900" << std::endl;
}

/**
 * @tc.number: BMS_Install_3000
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,whose modulename is null
 *             in config.json
 *           2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_3000, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_3000" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e14.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_CHECK_ERROR]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_3000" << std::endl;
}

/**
 * @tc.number: BMS_Install_3100
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,whose module package is null
 *             in config.json
 *           2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_3100, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_3100" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e12.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_CHECK_ERROR]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_3100" << std::endl;
}

/**
 * @tc.number: BMS_Install_3200
 * @tc.name:  test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two third-party bundles
 *             with different app name
 *           2.install these bundle
 *           3.check results
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_3200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Install_3200" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);
    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName2 = THIRD_BASE_BUNDLE_NAME + "2";
    modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    CheckInstallIsSuccess(bundleName2, modulePackage, abilityNames);
    CheckBundleInfo(version, bundleName2);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    UninstallBundle(bundleName2, uninstallMsg);
    std::cout << "END BMS_Install_3200" << std::endl;
}

/**
 * @tc.number: BMS_Install_3300
 * @tc.name:  test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two third-party bundles
 *           2.the bundle which has the same bundleName and version as system bundle
 *           3.install these bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_3300, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_3300" << std::endl;
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle1.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + "s1";
    std::string message;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    std::string modulePackage = SYSTEM_BASE_BUNDLE_NAME + ".h1";
    std::string version = "1.0";
    std::vector<std::string> hap1AbilityNames = {"bmsSystemBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);
    CheckBundleInfo(version, bundleName);

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundles1.hap";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Failure[ERR_INSTALL_ALREADY_EXIST]") << "Success!" << bundleFilePath;

    UninstallBundle(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    std::cout << "END BMS_Install_3300" << std::endl;
}

/**
 * @tc.number: BMS_Install_3400
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,whose versionCode is null
 *                    in config.json
 *           2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_3400, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_3400" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e4.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_TYPE_ERROR]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_3400" << std::endl;
}

/**
 * @tc.number: BMS_Install_3500
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,which versionCode's type is not int
 *             in config.json
 *           2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_3500, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_3500" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e5.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_PROFILE_PROP_CHECK_ERROR]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_3500" << std::endl;
}

/**
 * @tc.number: BMS_Install_3600
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is an abnormal bundle,whose versionCode is minus
 *             in config.json
 *           2.install the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Install_3600, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Install_3600" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "e6.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_BAD_PROFILE]");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(getInfoResult);
    std::cout << "END BMS_Install_3600" << std::endl;
}
/**
 * @tc.number: BMS_InstallProgressInfo_0100
 * @tc.name: test the Installation progress of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a third-party bundle
 *           2.install the bundle
 *           3.check the progress info
 */
HWTEST_F(BmsInstallSystemTest, BMS_InstallProgressInfo_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_InstallProgressInfo_0100" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);
    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_InstallProgressInfo_0100" << std::endl;
}

/**
 * @tc.number: BMS_MultiHapInstall_0100
 * @tc.name: test install three different bundles at the same time
 * @tc.desc: 1.under '/data/test/bms_bundle',there are three bundles
 *           2.install bundles
 *           3.check the install results
 */
HWTEST_F(BmsInstallSystemTest, BMS_MultiHapInstall_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_MultiHapInstall_0100" << std::endl;
    std::string installMsg;
    std::vector<std::shared_future<void>> futureVec;

    for (int32_t i = 6; i <= 8; i++) {
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        bool installFlag = 0;
        auto res = std::async(std::launch::async, InstallMultiBundle, bundleFilePath, installFlag);
        futureVec.push_back(res.share());
    }

    for (auto iter = futureVec.begin(); iter != futureVec.end(); iter++) {
        iter->wait();
    }

    GTEST_LOG_(INFO) << "finish to install all bundle!";
    for (int32_t i = 1; i <= 3; i++) {
        std::string bundleName = THIRD_BASE_BUNDLE_NAME + std::to_string(i);
        std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
        std::vector<std::string> hap1AbilityNames = {};
        CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);
    }

    std::vector<BundleInfo> bundleInfos;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool getInfosResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
    EXPECT_TRUE(getInfosResult);
    bool isSubStrExist = false;
    for (int32_t i = 1; i <= 3; i++) {
        std::string bundleName = THIRD_BASE_BUNDLE_NAME + std::to_string(i);
        for (auto iter = bundleInfos.begin(); iter != bundleInfos.end(); iter++) {
            if (IsSubStr(iter->name, bundleName)) {
                isSubStrExist = true;
                break;
            }
        }
        EXPECT_TRUE(isSubStrExist);
        std::string uninstallMsg;
        UninstallBundle(bundleName, uninstallMsg);
    }
    std::cout << "END BMS_MultiHapInstall_0100" << std::endl;
}

/**
 * @tc.number: BMS_MultiHapInstall_0200
 * @tc.name: test install two abnormal bundles at the same time
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two abnormal bundles
 *           2.install bundles
 *           3.check the install results
 */
HWTEST_F(BmsInstallSystemTest, BMS_MultiHapInstall_0200, Function | MediumTest | Level2)
{
    std::cout << "START BMS_MultiHapInstall_0200" << std::endl;
    std::vector<std::shared_future<void>> futureVec;

    for (int32_t i = 11; i <= 12; i++) {
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        bool installFlag = 0;
        auto res = std::async(std::launch::async, InstallMultiBundle, bundleFilePath, installFlag);
        futureVec.push_back(res.share());
    }

    for (auto iter = futureVec.begin(); iter != futureVec.end(); iter++) {
        iter->wait();
    }

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "11";
    CheckFileNonExist(bundleName);
    bundleName = THIRD_BASE_BUNDLE_NAME + "12";
    CheckFileNonExist(bundleName);
    std::cout << "END BMS_MultiHapInstall_0200" << std::endl;
}

/**
 * @tc.number: BMS_MultiHapInstall_0300
 * @tc.name: test install two abnormal bundles at the same time
 * @tc.desc: 1.under '/data/test/bms_bundle',there are six bundles
 *           2.install bundles
 *           3.check the install results
 */
HWTEST_F(BmsInstallSystemTest, BMS_MultiHapInstall_0300, Function | MediumTest | Level2)
{
    std::cout << "START BMS_MultiHapInstall_0300" << std::endl;
    std::vector<std::shared_future<void>> futureVec;

    for (int32_t i = 0; i < 6; i++) {
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        bool installFlag = 1;
        auto res = std::async(std::launch::async, InstallMultiBundle, bundleFilePath, installFlag);
        futureVec.push_back(res.share());
    }

    for (auto iter = futureVec.begin(); iter != futureVec.end(); iter++) {
        iter->wait();
    }

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    for (int32_t i = 0; i < 6; i++) {
        std::string bundleName = THIRD_BASE_BUNDLE_NAME + std::to_string(i);
        std::string uninstallMsg;
        UninstallBundle(bundleName, uninstallMsg);
    }

    std::cout << "END BMS_MultiHapInstall_0300" << std::endl;
}

/**
 * @tc.number: BMS_SystemAppInstall_0100
 * @tc.name: test system bundle install
 * @tc.desc: 1.under '/system/app',there is a system bundle
 *           2.TriggerScan and check install result
 */
HWTEST_F(BmsInstallSystemTest, BMS_SystemAppInstall_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SystemAppInstall_0100" << std::endl;
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle1.hap";
    std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + "s1";
    std::string message;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    std::string modulePackage = SYSTEM_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsSystemBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);
    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);

    UninstallBundle(bundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    std::cout << "END BMS_SystemAppInstall_0100" << std::endl;
}


/**
 * @tc.number: BMS_SystemAppInstall_0200
 * @tc.name: test install two bundles at the same time
 * @tc.desc: 1.under '/system/app',there are two system bundles
 *           2.TriggerScan and check install result
 */
HWTEST_F(BmsInstallSystemTest, BMS_SystemAppInstall_0200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_SystemAppInstall_0200" << std::endl;
    int bundleNum = 2;

    for (int i = 1; i <= bundleNum; i++) {
        std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle" + std::to_string(i) + ".hap";
        std::string message;
        InstallBundle(bundleFilePath, InstallFlag::NORMAL, message);
        EXPECT_EQ(message, "Success") << "install fail!";

        std::string bundleName = SYSTEM_BASE_BUNDLE_NAME + "s" + std::to_string(i);
        std::string modulePackage = SYSTEM_BASE_BUNDLE_NAME + ".h1";
        std::vector<std::string> hap1AbilityNames = {};
        CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

        UninstallBundle(bundleName, message);
        EXPECT_EQ(message, "Success") << "uninstall fail!";
    }
    std::cout << "END BMS_SystemAppInstall_0200" << std::endl;
}

/**
 * @tc.number: BMS_SystemAppInstall_0300
 * @tc.name: test install abnormal system bundle,and check bundle info
 * @tc.desc: 1.under '/system/app',there are three bundles,one of them is normal,others are abnormal
 *           2.TriggerScan and check install result
 */
HWTEST_F(BmsInstallSystemTest, BMS_SystemAppInstall_0300, Function | MediumTest | Level2)
{
    std::cout << "START BMS_SystemAppInstall_0300" << std::endl;
    std::vector<std::string> bundleName = {SYSTEM_BASE_BUNDLE_NAME + "s3", SYSTEM_BASE_BUNDLE_NAME + "s4"};
    std::string normalSystemBundleName = SYSTEM_BASE_BUNDLE_NAME + "s1";
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle1.hap";
    std::string message;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, message);
    EXPECT_EQ(message, "Success") << "install fail!";

    std::vector<BundleInfo> bundleInfos;
    bool getInfosResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
    EXPECT_TRUE(getInfosResult);
    bool isSubStrExist = false;
    for (std::string item : bundleName) {
        CheckFileNonExist(item);
        for (auto iter = bundleInfos.begin(); iter != bundleInfos.end(); iter++) {
            isSubStrExist = IsSubStr(iter->name, item);
            EXPECT_FALSE(isSubStrExist);
        }
    }
    std::string modulePackage = SYSTEM_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsSystemBundle_A1"};
    CheckInstallIsSuccess(SYSTEM_BASE_BUNDLE_NAME + "s1", modulePackage, hap1AbilityNames);

    BundleInfo bundleInfo;
    bool getInfoResult =
        bundleMgrProxy->GetBundleInfo(normalSystemBundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(getInfoResult);
    EXPECT_EQ(bundleInfo.name, normalSystemBundleName);

    EXPECT_EQ(bundleInfo.versionName, "1.0");
    EXPECT_GE(bundleInfo.uid, Constants::BASE_SYS_UID);
    EXPECT_LE(bundleInfo.uid, Constants::MAX_SYS_UID);
    EXPECT_GE(bundleInfo.gid, Constants::BASE_SYS_UID);
    EXPECT_LE(bundleInfo.gid, Constants::MAX_SYS_UID);

    UninstallBundle(normalSystemBundleName, message);
    EXPECT_EQ(message, "Success") << "uninstall fail!";
    std::cout << "END BMS_SystemAppInstall_0300" << std::endl;
}

/**
 * @tc.number: BMS_Upgrade_0100
 * @tc.name: test the upgrade of a third-party bundle
 * @tc.desc: 1.install a third-party bundle
 *           2.under '/data/test/bms_bundle',there is an app,whose version is
 *             higher than the installed one
 *           3.upgrade the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Upgrade_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Upgrade_0100" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "2";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    std::this_thread::sleep_for(50ms);
    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";
    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string version = "3.0";
    hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckBundleInfo(version, bundleName);
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_Upgrade_0100" << std::endl;
}

/**
 * @tc.number: BMS_Upgrade_0200
 * @tc.name: test the upgrade of a third-party bundle
 * @tc.desc: 1.install a third-party bundle
 *           2.under '/data/test/bms_bundle',there is a third-party bundle,whose version
 *             is equal to origin one
 *           3.upgrade the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Upgrade_0200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Upgrade_0200" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "2";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    std::this_thread::sleep_for(50ms);
    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle10.hap";
    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string version = "1.0";
    CheckBundleInfo(version, bundleName);
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_Upgrade_0200" << std::endl;
}

/**
 * @tc.number: BMS_Upgrade_0300
 * @tc.name: test the upgrade of a third-party bundle
 * @tc.desc: 1.install a third-party bundle
 *           2.under '/data/test/bms_bundle',there is a third-party bundle,whose version is
 *             lower than the installed one
 *           3.upgrade the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Upgrade_0300, Function | MediumTest | Level2)
{
    std::cout << "START BMS_Upgrade_0300" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "2";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_VERSION_DOWNGRADE]");

    std::string version = "3.0";
    CheckBundleInfo(version, bundleName);

    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_Upgrade_0300" << std::endl;
}

/**
 * @tc.number: BMS_Upgrade_0400
 * @tc.name: test the upgrade of a running third-party bundle
 * @tc.desc: 1.install a third-party bundle
 *           2.startup the app
 *           3.under '/data/test/bms_bundle',there is a third-party bundle,whose version is
 *             higher than the installed one
 *           4.upgrade the bundle
 */
HWTEST_F(BmsInstallSystemTest, BMS_Upgrade_0400, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Upgrade_0400" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "2";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    std::string cmd = "/system/bin/aa start -e aa bmsThirdBundle_A1 " + bundleName;
    std::vector<std::string> cmdRes;
    ExcuteMaintainCmd(cmd, cmdRes);

    std::this_thread::sleep_for(50ms);
    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";
    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Success");

    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_Upgrade_0400" << std::endl;
}

/**
 * @tc.number: BMS_MultiHapUpgrade_0100
 * @tc.name: test the upgrade of three normal bundles at the same time
 * @tc.desc: 1.install three normal bundles
 *           2.under '/data/test/bms_bundle',there are three upgrade bundles,whose versions
 *             are higher than installed bundles
 *           3.upgrade bundles and check results
 */
HWTEST_F(BmsInstallSystemTest, BMS_MultiHapUpgrade_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_MultiHapUpgrade_0100" << std::endl;
    int32_t max = 8;

    for (int32_t i = 6; i <= max; i++) {
        std::future<void> futureVec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        std::string bundleName = THIRD_BASE_BUNDLE_NAME + std::to_string(i - 5);
        bool installFlag = 0;
        futureVec = std::async(std::launch::async, InstallMultiBundle, bundleFilePath, installFlag);
        futureVec.wait();
        std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
        std::vector<std::string> hap1AbilityNames = {};
        CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

        std::this_thread::sleep_for(50ms);
        std::string upBundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i + 12) + ".hap";
        std::future<void> replaceExisting;
        installFlag = 1;
        replaceExisting = std::async(std::launch::async, InstallMultiBundle, upBundleFilePath, installFlag);
        replaceExisting.wait();
        CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);
    }

    for (int32_t i = 6; i <= max; i++) {
        std::string bundleName = THIRD_BASE_BUNDLE_NAME + std::to_string(i - 5);
        std::string uninstallMsg;
        UninstallBundle(bundleName, uninstallMsg);
    }

    std::cout << "END BMS_MultiHapUpgrade_0100" << std::endl;
}

/**
 * @tc.number: BMS_MultiHapUpgrade_0200
 * @tc.name: test the upgrade of three abnormal bundles at the same time
 * @tc.desc: 1.install three abnormal bundles
 *           2.under '/data/test/bms_bundle',there is three upgrade bundles,whose versions
 *             are higher than origin bundles
 *           3.install bundles and check results
 */
HWTEST_F(BmsInstallSystemTest, BMS_MultiHapUpgrade_0200, Function | MediumTest | Level2)
{
    std::cout << "START BMS_MultiHapUpgrade_0200" << std::endl;
    int32_t max = 8;

    for (int32_t i = 6; i <= max; i++) {
        std::future<void> futureVec;
        std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i) + ".hap";
        std::string bundleName = THIRD_BASE_BUNDLE_NAME + std::to_string(i - 5);
        bool installFlag = 0;
        futureVec = std::async(std::launch::async, InstallMultiBundle, bundleFilePath, installFlag);
        futureVec.wait();
        std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h2";
        std::vector<std::string> hap1AbilityNames = {};
        CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

        std::this_thread::sleep_for(50ms);
        std::string upBundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle" + std::to_string(i + 15) + ".hap";
        std::future<void> replaceExisting;
        installFlag = 1;
        replaceExisting = std::async(std::launch::async, InstallMultiBundle, upBundleFilePath, installFlag);
        replaceExisting.wait();
        CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);
    }

    for (int32_t i = 6; i <= max; i++) {
        std::string bundleName = THIRD_BASE_BUNDLE_NAME + std::to_string(i - 5);
        std::string uninstallMsg;
        UninstallBundle(bundleName, uninstallMsg);
    }
    std::cout << "END BMS_MultiHapUpgrade_0200" << std::endl;
}

/**
 * @tc.number: BMS_BundleDataStorage_0100
 * @tc.name: test the bundle info storage
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a third-party bundle
 *           2.install the bundle
 *           3.GetBundleInfo
 */
HWTEST_F(BmsInstallSystemTest, BMS_BundleDataStorage_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_BundleDataStorage_0100" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string installMsg;

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);

    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    BundleInfo bundleInfo;
    bool isGetInfoSuccess = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(isGetInfoSuccess);

    EXPECT_EQ(bundleInfo.name, "com.third.hiworld.example1");
    EXPECT_EQ(bundleInfo.vendor, "example");
    EXPECT_EQ(bundleInfo.label, "bmsThirdBundle_A1 Ability");
    EXPECT_EQ(bundleInfo.versionName, "1.0");
    uint32_t versionCode = 1;
    EXPECT_EQ(bundleInfo.versionCode, versionCode);
    EXPECT_GE(bundleInfo.uid, Constants::BASE_APP_UID);
    EXPECT_GE(bundleInfo.gid, Constants::BASE_APP_UID);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_BundleDataStorage_0100" << std::endl;
}

/**
 * @tc.number: BMS_BundleDataStorage_0200
 * @tc.name: test the bundle info storage
 * @tc.desc: 1.under '/data/test/bms_bundle',there are two third-party bundles,
 *             one is normal,the other doesn't have the 'config.json' file
 *           2.install the bundle
 *           3.GetBundleInfo
 */
HWTEST_F(BmsInstallSystemTest, BMS_BundleDataStorage_0200, Function | MediumTest | Level2)
{
    std::cout << "START BMS_BundleDataStorage_0200" << std::endl;
    BundleInfo bundleInfo;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }

    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckInstallIsSuccess(bundleName, modulePackage, hap1AbilityNames);

    bool isGetInfoSuccess = bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(isGetInfoSuccess);

    EXPECT_EQ(bundleInfo.name, "com.third.hiworld.example1");
    EXPECT_EQ(bundleInfo.vendor, "example");
    EXPECT_EQ(bundleInfo.label, "bmsThirdBundle_A1 Ability");
    EXPECT_EQ(bundleInfo.versionName, "1.0");
    uint32_t versionCode = 1;
    EXPECT_EQ(bundleInfo.versionCode, versionCode);
    EXPECT_GE(bundleInfo.uid, Constants::BASE_APP_UID);
    EXPECT_GE(bundleInfo.gid, Constants::BASE_APP_UID);

    bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle11.hap";
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Failure[ERR_INSTALL_PARSE_NO_PROFILE]");

    std::string eBundleName = THIRD_BASE_BUNDLE_NAME + "11";
    CheckFileNonExist(eBundleName);

    isGetInfoSuccess = bundleMgrProxy->GetBundleInfo(eBundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_FALSE(isGetInfoSuccess);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    std::cout << "END BMS_BundleDataStorage_0200" << std::endl;
}

/**
 * @tc.number: BMS_Broadcasting_0100
 * @tc.name: test the broadcasting of install event
 * @tc.desc: 1.subscribe the install event
 *           2.publish the event
 *           3.compare the action with the event
 */
HWTEST_F(BmsInstallSystemTest, BMS_Broadcasting_0100, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Broadcasting_0100" << std::endl;
    MatchingSkills matchingSkillsObj;
    matchingSkillsObj.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);

    CommonEventSubscribeInfo subscriberInfo(matchingSkillsObj);
    std::shared_ptr<SubscriberTest> subScriber = std::make_shared<SubscriberTest>(subscriberInfo);
    bool isSubscribeSuccess = CommonEventManager::SubscribeCommonEvent(subScriber);
    EXPECT_TRUE(isSubscribeSuccess);

    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";
    if (installMsg == "Success") {
        std::string subScriberResult = subScriber->GetSubscriberResultMsg();
        EXPECT_EQ(subScriberResult, CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    }

    bool isUnSubscribeSuccess = CommonEventManager::UnSubscribeCommonEvent(subScriber);
    EXPECT_TRUE(isUnSubscribeSuccess);
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!";
    std::cout << "END BMS_Broadcasting_0100" << std::endl;
}

/**
 * @tc.number: BMS_Broadcasting_0200
 * @tc.name: test the broadcasting of upgrade event
 * @tc.desc: 1.subscribe the upgrade event
 *           2.publish the event
 *           3.compare the action with the event
 */
HWTEST_F(BmsInstallSystemTest, BMS_Broadcasting_0200, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Broadcasting_0200" << std::endl;
    MatchingSkills matchingSkillsObj;
    matchingSkillsObj.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    CommonEventSubscribeInfo subScriberInfo(matchingSkillsObj);
    std::shared_ptr<SubscriberTest> subScriber = std::make_shared<SubscriberTest>(subScriberInfo);
    bool isSubscribeSuccess = CommonEventManager::SubscribeCommonEvent(subScriber);
    EXPECT_TRUE(isSubscribeSuccess);

    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle7.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "2";
    std::string bundleReFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle9.hap";
    std::this_thread::sleep_for(50ms);
    InstallBundle(bundleReFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";
    if (installMsg == "Success") {
        std::string subScriberResult = subScriber->GetSubscriberResultMsg();
        EXPECT_EQ(subScriberResult, CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    }
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!";

    bool isUnSubscribeSuccess = CommonEventManager::UnSubscribeCommonEvent(subScriber);
    EXPECT_TRUE(isUnSubscribeSuccess);
    std::cout << "END BMS_Broadcasting_0200" << std::endl;
}

/**
 * @tc.number: BMS_Broadcasting_0300
 * @tc.name: test the broadcasting of uninstall event
 * @tc.desc: 1.subscribe the uninstall event
 *           2.publish the event
 *           3.compare the action with the event
 */
HWTEST_F(BmsInstallSystemTest, BMS_Broadcasting_0300, Function | MediumTest | Level1)
{
    std::cout << "START BMS_Broadcasting_0300" << std::endl;
    MatchingSkills matchingSkillsObj;
    matchingSkillsObj.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkillsObj);
    std::shared_ptr<SubscriberTest> subScriber = std::make_shared<SubscriberTest>(subscriberInfo);
    bool isSubscribeSuccess = CommonEventManager::SubscribeCommonEvent(subScriber);
    EXPECT_TRUE(isSubscribeSuccess);
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle2.hap";
    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!";

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!";
    if (uninstallMsg == "Success") {
        std::string subScriberResult = subScriber->GetSubscriberResultMsg();
        EXPECT_EQ(subScriberResult, CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    }
    bool isUnSubscribeSuccess = CommonEventManager::UnSubscribeCommonEvent(subScriber);
    EXPECT_TRUE(isUnSubscribeSuccess);
    std::cout << "END BMS_Broadcasting_0300" << std::endl;
}

/**
 * @tc.number: BMS_DFX_0100
 * @tc.name: test error code
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists a hap
 *           2.install the hap with wrong user id
 *           3.get MSG_ERR_INSTALL_PARAM_ERROR
 */
HWTEST_F(BmsInstallSystemTest, BMS_DFX_0100, Function | MediumTest | Level2)
{
    std::cout << "START BMS_DFX_0100" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg, Constants::INVALID_USERID);
    EXPECT_EQ(installMsg, "Failure[MSG_ERR_INSTALL_PARAM_ERROR]");
    std::cout << "END BMS_DFX_0100" << std::endl;
}

/**
 * @tc.number: BMS_DFX_0200
 * @tc.name: test error code
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists a hap
 *           2.install the hap
 *           3.uninstall hap with wrong user id
 *           4.get ERR_UNINSTALL_PARAM_ERROR
 */
HWTEST_F(BmsInstallSystemTest, BMS_DFX_0200, Function | MediumTest | Level2)
{
    std::cout << "START BMS_DFX_0200" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg, Constants::INVALID_USERID);
    EXPECT_EQ(uninstallMsg, "Failure[ERR_UNINSTALL_PARAM_ERROR]");
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "install fail!" << bundleFilePath;
    std::cout << "END BMS_DFX_0200" << std::endl;
}

/**
 * @tc.number: BMS_DFX_0300
 * @tc.name: test error code
 * @tc.desc: 1.under '/data/test/bms_bundle',there exists a hap
 *           2.install the hap
 *           3.uninstall hap with invalid appName
 *           4.get ERR_UNINSTALL_INVALID_NAME
 */
HWTEST_F(BmsInstallSystemTest, BMS_DFX_0300, Function | MediumTest | Level2)
{
    std::cout << "START BMS_DFX_0300" << std::endl;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::string bundleName = THIRD_BASE_BUNDLE_NAME + "1";
    std::string errBundleName = "";
    std::string modulePackage = THIRD_BASE_BUNDLE_NAME + ".h1";
    std::vector<std::string> abilityNames = {"bmsThirdBundle_A1"};
    CheckInstallIsSuccess(bundleName, modulePackage, abilityNames);

    std::string uninstallMsg;
    UninstallBundle(errBundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Failure[ERR_UNINSTALL_INVALID_NAME]");
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_DFX_0300" << std::endl;
}

/**
 * @tc.number: BMS_DFX_0400
 * @tc.name: test the installation of a third-party bundle
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a big bundle,whose size is over 50M
 *           2.install the bundle
 *           3.get ERR_INSTALL_INVALID_HAP_SIZE
 */
HWTEST_F(BmsInstallSystemTest, BMS_DFX_0400, Function | MediumTest | Level2)
{
    std::cout << "START BMS_DFX_0400" << std::endl;
    std::string installMsg;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "bmsThirdBundle43.hap";

    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success");

    std::string bundleName = "com.third.hiworld.big";
    std::string uninstallMsg;
    UninstallBundle(bundleName, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END BMS_DFX_0400" << std::endl;
}
}  // namespace AppExecFwk
}  // namespace OHOS