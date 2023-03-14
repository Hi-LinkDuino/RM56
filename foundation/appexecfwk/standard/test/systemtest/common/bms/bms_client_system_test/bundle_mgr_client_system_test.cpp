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
#include <vector>

#include <gtest/gtest.h>

#include "ability_info.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_installer_interface.h"
#include "bundle_mgr_client.h"
#include "bundle_mgr_interface.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "common_tool.h"
#include "extension_ability_info.h"
#include "hap_module_info.h"
#include "iservice_registry.h"
#include "status_receiver_host.h"
#include "system_ability_definition.h"

using namespace testing::ext;

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string RESOURCE_PATH =
    "/data/app/el1/bundle/public/com.example.ohosproject.hmservice/entry_phone/resources.index";
const std::string THIRD_PATH = "/data/test/bms_bundle/";
const std::string BUNDLE_NAME = "com.example.ohosproject.hmservice";
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILURE = "Failure";
const std::string OPERATION_SUCCESS = "Success";
const int TIMEOUT = 10;
const int DEFAULT_USERID = 100;
} // namespace

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

class BundleMgrClientSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static void InstallBundle(
        const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMsg, const int userId);
    static void UninstallBundle(const std::string &bundleName, std::string &uninstallMsg, const int userId);
    static sptr<IBundleMgr> GetBundleMgrProxy();
    static sptr<IBundleInstaller> GetInstallerProxy();
};

void BundleMgrClientSystemTest::SetUpTestCase()
{}

void BundleMgrClientSystemTest::TearDownTestCase()
{}

void BundleMgrClientSystemTest::SetUp()
{}

void BundleMgrClientSystemTest::TearDown()
{}

sptr<IBundleMgr> BundleMgrClientSystemTest::GetBundleMgrProxy()
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

sptr<IBundleInstaller> BundleMgrClientSystemTest::GetInstallerProxy()
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

void BundleMgrClientSystemTest::InstallBundle(
    const std::string &bundleFilePath, const InstallFlag installFlag, std::string &installMsg,
    const int userId = Constants::ALL_USERID)
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

void BundleMgrClientSystemTest::UninstallBundle(
    const std::string &bundleName, std::string &uninstallMsg, const int userId = Constants::ALL_USERID)
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

/**
 * @tc.number: GetResourceConfigFile_001
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. extensionAbilityInfo is invalid, resourcePath is empty
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_001, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_001");
    GTEST_LOG_(INFO) << name << " start";
    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = "";
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_002
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. extensionAbilityInfo is invalid, resourcePath is not existed
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_002, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_002");
    GTEST_LOG_(INFO) << name << " start";
    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = "resourcePath";
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_003
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. extensionAbilityInfo is invalid, metadata is not existed
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_003, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_003");
    GTEST_LOG_(INFO) << name << " start";
    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "extensionName";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_004
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. extensionAbilityInfo is valid
 *           2. metadataName is not empty
 *           3. extensionAbilityInfo does not have metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_004, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_004");
    GTEST_LOG_(INFO) << name << " start";
    ExtensionAbilityInfo info;
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_005
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. extensionAbilityInfo is valid
 *           2. metadataName is not empty
 *           3. metadataName cannot be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_005, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_005");
    GTEST_LOG_(INFO) << name << " start";
    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms1";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_006
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. extensionAbilityInfo is valid and has only one metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_006, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_006");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_006" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_007
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. extensionAbilityInfo is valid and has multiple metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_007, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_007");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms1";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_007" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_008
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. extensionAbilityInfo is valid and has multiple metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_008, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_008");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(profileInfo.size(), 2);
    std::cout << profileInfo[0] << std::endl;
    std::cout << profileInfo[1] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_008" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_009
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. metadataName is empty
 *           2. extensionAbilityInfo has only one metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_009, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_009");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName;
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_009" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_010
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. metadataName is empty
 *           2. extensionAbilityInfo has only multiple metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_010, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_010");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms1";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName;
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(profileInfo.size(), 2);
    std::cout << profileInfo[0] << std::endl;
    std::cout << profileInfo[1] << std::endl;
    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_010" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_011
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile's suffix is not .json
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_011, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_011");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config1";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_011" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_012
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile is empty file
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_012, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_012");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config2";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_012" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_013
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile is not json-format
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_013, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_013");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config3";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_013" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_014
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profiles both have json-format file and non-json-format file
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_014, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_014");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    ExtensionAbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    Metadata data1;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config1";

    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_014" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_015
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. AbilityInfo is invalid, resourcePath is empty
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_015, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_015");
    GTEST_LOG_(INFO) << name << " start";
    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = "";
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_016
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. AbilityInfo is invalid, resourcePath is not existed
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_016, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_016");
    GTEST_LOG_(INFO) << name << " start";
    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = "resourcePath";
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_017
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. AbilityInfo is invalid, metadata is not existed
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_017, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_017");
    GTEST_LOG_(INFO) << name << " start";
    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "extensionName";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_018
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. AbilityInfo is valid
 *           2. metadataName is not empty
 *           3. AbilityInfo does not have metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_018, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_018");
    GTEST_LOG_(INFO) << name << " start";
    AbilityInfo info;
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_019
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. AbilityInfo is valid
 *           2. metadataName is not empty
 *           3. metadataName cannot be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_019, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_019");
    GTEST_LOG_(INFO) << name << " start";
    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms1";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_020
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. AbilityInfo is valid and has only one metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_020, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_020");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_020" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_021
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. AbilityInfo is valid and has multiple metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_021, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_021");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms1";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_021" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_022
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. AbilityInfo is valid and has multiple metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_022, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_022");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(profileInfo.size(), 2);
    std::cout << profileInfo[0] << std::endl;
    std::cout << profileInfo[1] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_022" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_023
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. metadataName is empty
 *           2. AbilityInfo has only one metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_023, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_023");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName;
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_023" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_024
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. metadataName is empty
 *           2. AbilityInfo has only multiple metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_024, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_024");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms1";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName;
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(profileInfo.size(), 2);
    std::cout << profileInfo[0] << std::endl;
    std::cout << profileInfo[1] << std::endl;
    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_024" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_025
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile's suffix is not .json
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_025, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_025");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config1";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_025" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_026
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile is empty file
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_026, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_026");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config2";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_026" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_027
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile is not json-format
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_027, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_027");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config3";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_027" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_028
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profiles both have json-format file and non-json-format file
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_028, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_028");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    AbilityInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    Metadata data1;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config1";

    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_028" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_029
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. HapModuleInfo is invalid, resourcePath is empty
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_029, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_029");
    GTEST_LOG_(INFO) << name << " start";
    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = "";
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_030
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. HapModuleInfo is invalid, resourcePath is not existed
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_030, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_030");
    GTEST_LOG_(INFO) << name << " start";
    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = "resourcePath";
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_031
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. HapModuleInfo is invalid, metadata is not existed
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_031, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_031");
    GTEST_LOG_(INFO) << name << " start";
    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "extensionName";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_032
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. HapModuleInfo is valid
 *           2. metadataName is not empty
 *           3. HapModuleInfo does not have metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_032, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_032");
    GTEST_LOG_(INFO) << name << " start";
    HapModuleInfo info;
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_033
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. HapModuleInfo is valid
 *           2. metadataName is not empty
 *           3. metadataName cannot be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_033, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_033");
    GTEST_LOG_(INFO) << name << " start";
    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms1";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_034
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. HapModuleInfo is valid and has only one metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_034, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_034");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_034" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_035
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. HapModuleInfo is valid and has multiple metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_035, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_035");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms1";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_035" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_036
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. HapModuleInfo is valid and has multiple metadata
 *           2. metadataName is not empty
 *           3. metadataName can be found
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_036, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_036");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(profileInfo.size(), 2);
    std::cout << profileInfo[0] << std::endl;
    std::cout << profileInfo[1] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_036" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_037
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. metadataName is empty
 *           2. HapModuleInfo has only one metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_037, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_037");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName;
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    std::cout << profileInfo[0] << std::endl;

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_037" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_038
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. metadataName is empty
 *           2. HapModuleInfo has only multiple metadata
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_038, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_038");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";

    Metadata data1;
    data1.name = "ohos.extension.forms1";
    data1.resource = "@profile:shortcuts_config";

    metadata.emplace_back(data);
    metadata.emplace_back(data1);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName;
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(profileInfo.size(), 2);
    std::cout << profileInfo[0] << std::endl;
    std::cout << profileInfo[1] << std::endl;
    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_038" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_039
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile's suffix is not .json
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_039, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_039");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config1";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_039" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_040
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile is empty file
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_040, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_040");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config2";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_040" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_041
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profile is not json-format
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_041, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_041");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config3";
    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_041" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_042
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of GetResConfigFile
 *           1. profiles both have json-format file and non-json-format file
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, GetResourceConfigFile_042, TestSize.Level1)
{
    auto name = std::string("GetResourceConfigFile_042");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    HapModuleInfo info;
    std::vector<Metadata> &metadata = info.metadata;
    Metadata data;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config";
    Metadata data1;
    data.name = "ohos.extension.forms";
    data.resource = "@profile:form_config1";

    metadata.emplace_back(data);
    info.resourcePath = RESOURCE_PATH;
    BundleMgrClient bundleMgrClient;
    std::string metadataName = "ohos.extension.forms";
    std::vector<std::string> profileInfo;
    auto ret = bundleMgrClient.GetResConfigFile(info, metadataName, profileInfo);
    EXPECT_FALSE(ret);

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END GetResourceConfigFile_042" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}

/**
 * @tc.number: GetResourceConfigFile_04
 * @tc.name: GetResConfigFile
 * @tc.desc: Test the interface of QueryExtensionAbilityInfos
 * @tc.require: AR000GNT9D
 */
HWTEST_F(BundleMgrClientSystemTest, QueryExtensionAbilityInfos_0001, TestSize.Level1)
{
    auto name = std::string("QueryExtensionAbilityInfos_0001");
    GTEST_LOG_(INFO) << name << " start";
    std::string bundleFilePath = THIRD_PATH + "bundleClient1.hap";
    std::string installMsg;
    InstallBundle(bundleFilePath, InstallFlag::NORMAL, installMsg);
    EXPECT_EQ(installMsg, "Success") << "install fail!" << bundleFilePath;

    std::vector<ExtensionAbilityInfo> infos;
    bool ret =
        GetBundleMgrProxy()->QueryExtensionAbilityInfos(ExtensionAbilityType::FORM, DEFAULT_USERID, infos);
    EXPECT_TRUE(ret);
    EXPECT_EQ(1, infos.size());

    std::string uninstallMsg;
    UninstallBundle(BUNDLE_NAME, uninstallMsg);
    EXPECT_EQ(uninstallMsg, "Success") << "uninstall fail!" << bundleFilePath;
    std::cout << "END QueryExtensionAbilityInfos_0001" << std::endl;
    GTEST_LOG_(INFO) << name << " end";
}
}  // namespace AppExecFwk
}  // namespace OHOS