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
#include <future>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "bundle_info.h"
#include "bundle_installer_interface.h"
#include "bundle_mgr_client.h"
#include "bundle_mgr_interface.h"
#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "datetime_ex.h"
#include "iservice_registry.h"
#include "status_receiver_host.h"
#include "system_ability_definition.h"

using namespace testing::ext;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace AppExecFwk {
namespace {
std::mutex mtx_;
const std::string THIRD_PATH = "/data/test/bms_bundle/";
const std::string BUNDLE_NAME = "com.example.ohosproject.hmservice";
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILURE = "Failure";
const std::string OPERATION_SUCCESS = "Success";
const std::string BUNDLE_DATA_DIR1 = "/data/app/el1/100/base/";
const std::string BUNDLE_DATA_DIR2 = "/data/app/el1/100/database/";
const std::string BUNDLE_DATA_DIR3 = "/data/app/el2/100/base/";
const std::string BUNDLE_DATA_DIR4 = "/data/app/el2/100/database/";
const time_t TIME_OUT_SECONDS_FIVE = 5;
const int32_t TIMEOUT = 10;
const int32_t DEFAULT_USERID = 100;
const int32_t DLP_TYPE_1 = 1;
const int32_t DLP_TYPE_2 = 2;
const int32_t MAX_NUMBER_SANDBOX_APP = 100;
bool g_ReceivedInstallSandbox = false;
bool g_RceivedUninstallSandbox = false;
} // namespace

class StatusReceiverImpl : public StatusReceiverHost {
public:
    StatusReceiverImpl();
    virtual ~StatusReceiverImpl() override;
    virtual void OnStatusNotify(const int32_t progress) override;
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override;
    std::string GetResultMsg() const;

private:
    mutable std::promise<std::string> resultMsgSignal_;
    int32_t iProgress_ = 0;

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

void StatusReceiverImpl::OnStatusNotify(const int32_t progress)
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

class CommonEventSubscriberTest : public CommonEventSubscriber {
public:
    explicit CommonEventSubscriberTest(const CommonEventSubscribeInfo &subscribeInfo);
    virtual ~CommonEventSubscriberTest() {};
    virtual void OnReceiveEvent(const CommonEventData &data);
};

CommonEventSubscriberTest::CommonEventSubscriberTest(const CommonEventSubscribeInfo &subscribeInfo)
    : CommonEventSubscriber(subscribeInfo)
{}

void CommonEventSubscriberTest::OnReceiveEvent(const CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SANDBOX_PACKAGE_ADDED) {
        g_ReceivedInstallSandbox = true;
    }
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SANDBOX_PACKAGE_REMOVED) {
        g_RceivedUninstallSandbox = true;
    }
    mtx_.unlock();
}

class BundleMgrSandboxAppSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void InstallBundle(
        const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMsg,
        const int32_t userId);
    static void UninstallBundle(const std::string &bundleName, std::string &uninstallMsg, const int32_t userId);
    static void CheckPathAreExisted(const std::string &bundleName, int32_t appIndex);
    static void CheckPathAreNonExisted(const std::string &bundleName, int32_t appIndex);
    static void CheckSandboxAppInfo(const std::string &bundleName, int32_t appIndex);
    static sptr<IBundleMgr> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
};

void BundleMgrSandboxAppSystemTest::SetUpTestCase()
{}

void BundleMgrSandboxAppSystemTest::TearDownTestCase()
{}

void BundleMgrSandboxAppSystemTest::SetUp()
{}

void BundleMgrSandboxAppSystemTest::TearDown()
{}

sptr<IBundleMgr> BundleMgrSandboxAppSystemTest::GetBundleMgrProxy()
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

sptr<IBundleInstaller> BundleMgrSandboxAppSystemTest::GetInstallerProxy()
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

void BundleMgrSandboxAppSystemTest::InstallBundle(
    const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMsg,
    const int32_t userId = Constants::ALL_USERID)
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

void BundleMgrSandboxAppSystemTest::UninstallBundle(
    const std::string &bundleName, std::string &uninstallMsg, const int32_t userId = Constants::ALL_USERID)
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

void BundleMgrSandboxAppSystemTest::CheckPathAreExisted(const std::string &bundleName, int32_t appIndex)
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

void BundleMgrSandboxAppSystemTest::CheckPathAreNonExisted(const std::string &bundleName, int32_t appIndex)
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

void BundleMgrSandboxAppSystemTest::CheckSandboxAppInfo(const std::string &bundleName, int32_t appIndex)
{
    BundleInfo info;
    BundleMgrClient bundleMgrClient;
    auto ret = bundleMgrClient.GetSandboxBundleInfo(bundleName, appIndex, DEFAULT_USERID, info);
    EXPECT_EQ(ret, ERR_OK);

    BundleInfo info2;
    auto res = bundleMgrClient.GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, info2, DEFAULT_USERID);
    EXPECT_TRUE(res);

    std::cout << "uid : " << info.uid << std::endl;
    EXPECT_NE(info.applicationInfo.uid, info2.applicationInfo.uid);
    EXPECT_NE(info.applicationInfo.accessTokenId, info2.applicationInfo.accessTokenId);
    EXPECT_NE(info.applicationInfo.dataBaseDir, info2.applicationInfo.dataBaseDir);
}

/**
 * @tc.number: InstallSandboxAppTest001
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of InstallSandboxApp
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, InstallSandboxAppTest001, TestSize.Level1)
{
    auto name = std::string("InstallSandboxAppTest_001");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    ErrCode ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 1);

    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    CheckPathAreNonExisted(BUNDLE_NAME, 1);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: InstallSandboxAppTest002
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of InstallSandboxApp
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, InstallSandboxAppTest002, TestSize.Level1)
{
    auto name = std::string("InstallSandboxAppTest002");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    ErrCode ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 1);
    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    int32_t secondAppIndex = 0;
    ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, 2);
    CheckPathAreExisted(BUNDLE_NAME, 2);
    CheckSandboxAppInfo(BUNDLE_NAME, 2);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    CheckPathAreNonExisted(BUNDLE_NAME, 1);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: InstallSandboxAppTest003
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of InstallSandboxApp
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, InstallSandboxAppTest003, TestSize.Level1)
{
    auto name = std::string("InstallSandboxAppTest003");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    ErrCode ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 1);

    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    CheckPathAreNonExisted(BUNDLE_NAME, 1);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: InstallSandboxAppTest004
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of InstallSandboxApp
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, InstallSandboxAppTest004, TestSize.Level1)
{
    auto name = std::string("InstallSandboxAppTest004");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    ErrCode ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 1);
    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    int32_t secondAppIndex = 0;
    ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(secondAppIndex, 2);
    CheckPathAreExisted(BUNDLE_NAME, 2);
    CheckSandboxAppInfo(BUNDLE_NAME, 2);

    InstallBundle(bundleFilePath, InstallFlag::REPLACE_EXISTING, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    CheckPathAreNonExisted(BUNDLE_NAME, 1);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: InstallSandboxAppTest005
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of InstallSandboxApp
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, InstallSandboxAppTest005, TestSize.Level1)
{
    auto name = std::string("InstallSandboxAppTest005");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    int cnt = 0;
    while (cnt++ < MAX_NUMBER_SANDBOX_APP) {
        ErrCode ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, DEFAULT_USERID, appIndex);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_NE(0, appIndex);
        CheckPathAreExisted(BUNDLE_NAME, appIndex);
        CheckSandboxAppInfo(BUNDLE_NAME, appIndex);
    }
    int32_t secondAppIndex = 0;
    ErrCode ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, DEFAULT_USERID, secondAppIndex);
    EXPECT_EQ(ret, ERR_APPEXECFWK_SANDBOX_INSTALL_INVALID_APP_INDEX);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: InstallSandboxAppTest006
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of InstallSandboxApp
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, InstallSandboxAppTest006, TestSize.Level1)
{
    auto name = std::string("InstallSandboxAppTest006");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    // subscribe common event
    Want wantTest;
    wantTest.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SANDBOX_PACKAGE_ADDED);
    CommonEventData commonEventData(wantTest);

    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SANDBOX_PACKAGE_ADDED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventSubscriberTest>(subscribeInfo);
    EXPECT_EQ(CommonEventManager::SubscribeCommonEvent(subscriberPtr), true);

    mtx_.lock();
    struct tm startTime = {0};
    EXPECT_EQ(OHOS::GetSystemCurrentTime(&startTime), true);

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    ErrCode ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 1);

    struct tm doingTime = {0};
    int64_t seconds = 0;
    while (!mtx_.try_lock()) {
        EXPECT_EQ(OHOS::GetSystemCurrentTime(&doingTime), true);
        seconds = OHOS::GetSecondsBetween(startTime, doingTime);
        if (seconds >= TIME_OUT_SECONDS_FIVE) {
            break;
        }
    }
    mtx_.unlock();
    EXPECT_TRUE(g_ReceivedInstallSandbox);
    EXPECT_EQ(CommonEventManager::UnSubscribeCommonEvent(subscriberPtr), true);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: UninstallSandboxAppTest001
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of UninstallSandboxApp
 *           1. Install application 
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, UninstallSandboxAppTest001, TestSize.Level1)
{
    auto name = std::string("UninstallSandboxAppTest001");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    ErrCode ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 1);
    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    auto res = bundleMgrClient.UninstallSandboxApp(BUNDLE_NAME, 1, DEFAULT_USERID);
    EXPECT_EQ(res, ERR_OK);
    CheckPathAreNonExisted(BUNDLE_NAME, 1);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: UninstallSandboxAppTest002
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of UninstallSandboxApp
 *           1. Install application 
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, UninstallSandboxAppTest002, TestSize.Level1)
{
    auto name = std::string("UninstallSandboxAppTest002");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    auto ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 1);
    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 2);
    CheckPathAreExisted(BUNDLE_NAME, 2);
    CheckSandboxAppInfo(BUNDLE_NAME, 2);

    auto res = bundleMgrClient.UninstallSandboxApp(BUNDLE_NAME, 1, DEFAULT_USERID);
    EXPECT_EQ(res, ERR_OK);
    CheckPathAreNonExisted(BUNDLE_NAME, 1);
    CheckPathAreExisted(BUNDLE_NAME, 2);
    CheckSandboxAppInfo(BUNDLE_NAME, 2);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: UninstallSandboxAppTest003
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of UninstallSandboxApp
 *           1. Install application 
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, UninstallSandboxAppTest003, TestSize.Level1)
{
    auto name = std::string("UninstallSandboxAppTest003");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    auto ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 1);
    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 2);
    CheckPathAreExisted(BUNDLE_NAME, 2);
    CheckSandboxAppInfo(BUNDLE_NAME, 2);

    auto res = bundleMgrClient.UninstallSandboxApp(BUNDLE_NAME, 2, DEFAULT_USERID);
    EXPECT_EQ(res, ERR_OK);
    CheckPathAreNonExisted(BUNDLE_NAME, 2);
    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: UninstallSandboxAppTest004
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of UninstallSandboxApp
 *           1. Install application 
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, UninstallSandboxAppTest004, TestSize.Level1)
{
    auto name = std::string("UninstallSandboxAppTest004");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    auto ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(1, appIndex);
    CheckPathAreExisted(BUNDLE_NAME, 1);
    CheckSandboxAppInfo(BUNDLE_NAME, 1);

    ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(appIndex, 2);
    CheckPathAreExisted(BUNDLE_NAME, 2);
    CheckSandboxAppInfo(BUNDLE_NAME, 2);

    auto res = bundleMgrClient.UninstallSandboxApp(BUNDLE_NAME, 1, DEFAULT_USERID);
    EXPECT_EQ(res, ERR_OK);
    res = bundleMgrClient.UninstallSandboxApp(BUNDLE_NAME, 2, DEFAULT_USERID);
    EXPECT_EQ(res, ERR_OK);
    CheckPathAreNonExisted(BUNDLE_NAME, 1);
    CheckPathAreNonExisted(BUNDLE_NAME, 2);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: UninstallSandboxAppTest005
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of InstallSandboxApp
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, UninstallSandboxAppTest005, TestSize.Level1)
{
    auto name = std::string("UninstallSandboxAppTest005");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    // subscribe common event
    Want wantTest;
    wantTest.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SANDBOX_PACKAGE_REMOVED);
    CommonEventData commonEventData(wantTest);

    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SANDBOX_PACKAGE_REMOVED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventSubscriberTest>(subscribeInfo);

    EXPECT_EQ(CommonEventManager::SubscribeCommonEvent(subscriberPtr), true);

    mtx_.lock();
    struct tm startTime = {0};
    EXPECT_EQ(OHOS::GetSystemCurrentTime(&startTime), true);

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    auto ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_2, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(1, appIndex);

    auto res = bundleMgrClient.UninstallSandboxApp(BUNDLE_NAME, 1, DEFAULT_USERID);
    EXPECT_EQ(res, ERR_OK);

    struct tm doingTime = {0};
    int64_t seconds = 0;
    while (!mtx_.try_lock()) {
        EXPECT_EQ(OHOS::GetSystemCurrentTime(&doingTime), true);
        seconds = OHOS::GetSecondsBetween(startTime, doingTime);
        if (seconds >= TIME_OUT_SECONDS_FIVE) {
            break;
        }
    }
    mtx_.unlock();
    EXPECT_TRUE(g_RceivedUninstallSandbox);
    EXPECT_EQ(CommonEventManager::UnSubscribeCommonEvent(subscriberPtr), true);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetSandboxBundleInfoTest001
 * @tc.name: InstallSandboxApp
 * @tc.desc: Test the interface of UninstallSandboxApp
 *           1. Install application 
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrSandboxAppSystemTest, GetSandboxBundleInfoTest001, TestSize.Level1)
{
    auto name = std::string("GetSandboxBundleInfoTest001");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    BundleMgrClient bundleMgrClient;
    int32_t appIndex = 0;
    auto ret = bundleMgrClient.InstallSandboxApp(BUNDLE_NAME, DLP_TYPE_1, DEFAULT_USERID, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(1, appIndex);
    CheckPathAreExisted(BUNDLE_NAME, 1);

    BundleInfo info;
    auto res = bundleMgrClient.GetSandboxBundleInfo(BUNDLE_NAME, 1, DEFAULT_USERID, info);
    EXPECT_EQ(res, ERR_OK);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;

    GTEST_LOG_(INFO) << name << " end";
}
} // AppExecFwk
} // OHOS