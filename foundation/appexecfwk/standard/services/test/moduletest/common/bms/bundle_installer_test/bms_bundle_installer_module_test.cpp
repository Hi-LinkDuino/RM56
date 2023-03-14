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
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "bundle_data_mgr.h"
#include "bundle_data_storage_database.h"
#include "bundle_mgr_service.h"
#include "bundle_profile.h"
#include "common_tool.h"
#include "directory_ex.h"
#include "inner_bundle_info.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "mock_status_receiver.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS::DistributedKv;
using namespace std::chrono_literals;
using namespace OHOS;
using OHOS::DelayedSingleton;
using OHOS::AAFwk::Want;

namespace OHOS {
namespace {
const std::string BUNDLE_TMPPATH = "/data/test/bms_bundle/";
const std::string SYSTEM_BUNDLE_PATH = "/system/app/";
const std::string THIRD_BUNDLE_NAME = "com.third.hiworld.example";
const std::string SYSTEM_BUNDLE_NAME = "com.system.hiworld.example";
const std::string BUNDLE_CODE_PATH = "/data/app/el1/bundle/public/";
const std::string BUNDLE_DATA_PATH = "/data/app/el2/100/base/";
const std::string ROOT_DIR = "/data/app";
const std::string ERROR_SUFFIX = ".rpk";
const int32_t ROOT_UID = 0;
}  // namespace

class BmsBundleInstallerModuleTest : public testing::Test {
public:
    BmsBundleInstallerModuleTest();
    ~BmsBundleInstallerModuleTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    void StartBundleMgrService()
    {
        if (!bms_) {
            bms_ = DelayedSingleton<BundleMgrService>::GetInstance();
        }
        if (bms_) {
            bms_->OnStart();
        }
    }

    void StopBundleMgrService()
    {
        if (bms_) {
            bms_->OnStop();
            bms_ = nullptr;
        }
    }

    void StartInstalld()
    {
        installdService_->Start();
    }

    void StopInstalld()
    {
        installdService_->Stop();
        InstalldClient::GetInstance()->ResetInstalldProxy();
    }

    std::shared_ptr<BundleDataMgr> GetBundleDataMgr()
    {
        return bms_->GetDataMgr();
    }

    bool GetServiceStatus()
    {
        return bms_->IsServiceReady();
    }

    void SaveBundleDataToStorage(std::shared_ptr<SingleKvStore> &kvStorePtr, const std::string &bundleName,
        const std::string &data);
    void DeleteBundleDataToStorage(
        std::shared_ptr<SingleKvStore> &kvStorePtr, const std::string &bundleName);
    void DeleteBundleDataToStorage();
    std::shared_ptr<SingleKvStore> GetKvStorePtr(DistributedKvDataManager &dataManager);
    InnerBundleInfo CreateInnerBundleInfo(const std::string &bundleName);
    void CheckFileExist(const std::string &bundleName) const;
    void CheckFileExist(const std::string &bundleName, const std::string &modulePackage) const;
    void CheckFileExist(const std::string &bundleName, const std::string &modulePackage,
        const std::vector<std::string> &abilityNames) const;
    void CheckFileNonExist(const std::string &bundleName) const;

protected:
    nlohmann::json innerBundleInfoJson_ = R"(
        {
            "appFeature": "ohos_system_app",
            "appType": 0,
            "baseAbilityInfos": {
                "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility": {
                    "applicationName": "com.ohos.launcher",
                    "bundleName": "com.ohos.launcher",
                    "codePath": "",
                    "description": "$string:mainability_description",
                    "theme": "mytheme",
                    "deviceCapabilities": [],
                    "deviceId": "",
                    "deviceTypes": [
                        "phone"
                    ],
                    "iconPath": "$media:icon",
                    "enabled": true,
                    "readPermission": "readPermission",
                    "writePermission": "writePermission",
                    "configChanges": [
                        "locale"
                    ],
                    "formEnabled": true,
                    "formEntity": 1,
                    "minFormHeight": 0,
                    "defaultFormHeight": 100,
                    "minFormWidth": 0,
                    "defaultFormWidth": 200,
                    "metaData": {
                        "customizeData": [{
                            "name" : "string",
                            "value" : "string",
                            "extra" : "$string:customizeData_description"
                        }],
                        "parameters": [{
                            "name" : "string",
                            "type" : "Float",
                            "description" : "$string:parameters_description"
                        }],
                        "results": [{
                            "name" : "string",
                            "type" : "Float",
                            "description" : "$string:results_description"
                        }]
                    },
                    "isLauncherAbility": true,
                    "isNativeAbility": false,
                    "supportPipMode" : false,
                    "kind": "page",
                    "label": "Launcher",
                    "launchMode": 0,
                    "libPath": "",
                    "moduleName": ".MyApplication",
                    "name": "com.ohos.launcher.MainAbility",
                    "orientation": 0,
                    "package": "com.ohos.launcher",
                    "permissions": [],
                    "process": "",
                    "resourcePath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher/assets/launcher/resources.index",
                    "targetAbility": "",
                    "type": 1,
                    "uri": "",
                    "visible": false,
                    "labelId": 1,
                    "descriptionId": 1,
                    "iconId": 1,
                    "srcLanguage": "C++",
                    "srcPath": ""
                }
            },
            "baseApplicationInfo": {
                "bundleName": "com.ohos.launcher",
                "cacheDir": "/data/app/el2/100/base/com.ohos.launcher/cache",
                "codePath": "/data/app/el1/bundle/public/com.ohos.launcher",
                "dataBaseDir": "/data/app/el2/100/database/com.ohos.launcher",
                "dataDir": "/data/app/el2/100/base/com.ohos.launcher/files",
                "description": "$string:mainability_description",
                "descriptionId": 16777217,
                "deviceId": "PHONE-001",
                "enabled": true,
                "removable": true,
                "entryDir": "",
                "flags": 0,
                "iconId": 16777218,
                "iconPath": "$media:icon",
                "isLauncherApp": true,
                "isSystemApp": false,
                "label": "Launcher",
                "labelId": 0,
                "moduleInfos": [],
                "moduleSourceDirs": [],
                "name": "com.ohos.launcher",
                "permissions": [],
                "process": "",
                "signatureKey": "",
                "supportedModes": 0,
                "debug": true
            },
            "baseBundleInfo": {
                "abilityInfos": [],
                "appId": "BNtg4JBClbl92Rgc3jm/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=",
                "applicationInfo": {
                    "bundleName": "",
                    "cacheDir": "",
                    "codePath": "",
                    "dataBaseDir": "",
                    "dataDir": "",
                    "description": "",
                    "descriptionId": 0,
                    "deviceId": "",
                    "enabled": false,
                    "removable": true,
                    "entryDir": "",
                    "flags": 0,
                    "iconId": 0,
                    "iconPath": "",
                    "isLauncherApp": false,
                    "isSystemApp": false,
                    "label": "",
                    "labelId": 0,
                    "moduleInfos": [],
                    "moduleSourceDirs": [],
                    "name": "",
                    "permissions": [],
                    "process": "",
                    "signatureKey": "",
                    "supportedModes": 0,
                    "debug": true
                },
                "compatibleVersion": 3,
                "cpuAbi": "",
                "defPermissions": [],
                "description": "",
                "entryModuleName": "",
                "gid": 10000,
                "hapModuleNames": [],
                "installTime": 17921,
                "isKeepAlive": false,
                "isNativeApp": false,
                "isDifferentName":false,
                "jointUserId": "",
                "label": "Launcher",
                "mainEntry": "",
                "maxSdkVersion": 0,
                "minSdkVersion": 0,
                "moduleDirs": [],
                "moduleNames": [],
                "modulePublicDirs": [],
                "moduleResPaths": [],
                "name": "com.ohos.launcher",
                "releaseType": "Release",
                "reqPermissions": [],
                "seInfo": "",
                "targetVersion": 3,
                "uid": 10000,
                "updateTime": 17921,
                "vendor": "ohos",
                "versionCode": 1,
                "versionName": "1.0"
            },
            "baseDataDir": "/data/app/el2/100/base/com.ohos.launcher",
            "bundleStatus": 1,
            "gid": 10000,
            "hasEntry": true,
            "innerModuleInfos": {
                "com.ohos.launcher": {
                    "abilityKeys": [
                        "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility"
                    ],
                    "defPermissions": [],
                    "description": "",
                    "distro": {
                        "deliveryWithInstall": true,
                        "moduleName": "launcher",
                        "moduleType": "entry"
                    },
                    "hapPath": "/data/app/el1/bundle/public/com.ohos.launcher/com_ohos_launcher.hap",
                    "hashValue": "",
                    "isEntry": true,
                    "metaData": {
                        "customizeData": [],
                        "parameters": [],
                        "results": []
                    },
                    "moduleDataDir": "/data/app/el2/100/base/com.ohos.launcher/com.ohos.launcher",
                    "moduleName": ".MyApplication",
                    "modulePackage": "com.ohos.launcher",
                    "modulePath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher",
                    "moduleResPath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher/assets/launcher/resources.index",
                    "reqCapabilities": [],
                    "reqPermissions": [],
                    "colorMode": -1,
                    "skillKeys": [
                        "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility"
                    ],
                    "srcPath": ""
                }
            },
            "isKeepData": false,
            "isSupportBackup": false,
            "mainAbility": "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility",
            "skillInfos": {
                "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility": [
                    {
                        "actions": [
                            "action.system.home",
                            "com.ohos.action.main"
                        ],
                        "entities": [
                            "entity.system.home",
                            "flag.home.intent.from.system"
                        ],
                        "uris": []
                    }
                ]
            },
            "formInfos": {
                "com.ohos.launchercom.ohos.launchercom.ohos.launcher.MainAbility": [
                    {
                        "package": "com.ohos.launcher",
                        "bundleName": "com.ohos.launcher",
                        "originalBundleName": "com.ohos.launcher",
                        "relatedBundleName": "com.ohos.launcher",
                        "moduleName": "launcher",
                        "abilityName": "com.ohos.launcher.MainAbility",
                        "name": "Form_JS",
                        "description": "It's JS Form",
                        "jsComponentName": "com.ohos.launcher",
                        "deepLink": "com.example.myapplication.fa/.MainAbility",
                        "formConfigAbility": "com.example.myapplication.fa/.MainAbility",
                        "scheduledUpdateTime": "21:05",
                        "descriptionId": 125,
                        "updateDuration": 1,
                        "defaultDimension": 1,
                        "defaultFlag": true,
                        "formVisibleNotify": true,
                        "updateEnabled": true,
                        "type": 0,
                        "colorMode": 0,
                        "supportDimensions": [
                            1
                        ],
                        "landscapeLayouts": [],
                        "portraitLayouts": [],
                        "customizeData": [
                            {
                                "name": "originWidgetName",
                                "value": "com.weather.testWidget"
                            }
                        ]
                    }
                ]
            },
            "shortcutInfos": {
                "com.example.myapplication1com.example.myapplication.h1id": {
                    "bundleName": "com.example.myapplication1",
                    "disableMessage": "",
                    "hostAbility": "",
                    "icon": "$string:mainability_description",
                    "id": "id",
                    "intents": [
                        {
                            "targetBundle": "com.example.myapplication1",
                            "targetClass": "com.example.myapplication.MainAbility"
                        }
                    ],
                    "isEnables": false,
                    "isHomeShortcut": false,
                    "isStatic": false,
                    "label": "$string:mainability_description"
                }
            },
            "uid": 10000,
            "userId_": 0,
            "isPreInstallApp": true
        }
    )"_json;

private:
    std::shared_ptr<InstalldService> installdService_ = std::make_unique<InstalldService>();
    std::shared_ptr<BundleMgrService> bms_ = DelayedSingleton<BundleMgrService>::GetInstance();
};

BmsBundleInstallerModuleTest::BmsBundleInstallerModuleTest()
{
}

BmsBundleInstallerModuleTest::~BmsBundleInstallerModuleTest()
{
    bms_.reset();
}

void BmsBundleInstallerModuleTest::SaveBundleDataToStorage(std::shared_ptr<SingleKvStore> &kvStorePtr,
    const std::string &bundleName, const std::string &data)
{
    Status status;
    Key key(bundleName);
    Value value(data);
    status = kvStorePtr->Put(key, value);
    if (status != Status::SUCCESS) {
        std::cout << static_cast<int32_t>(status) << std::endl;
        EXPECT_TRUE(false) << "save fail";
    }
}

void BmsBundleInstallerModuleTest::DeleteBundleDataToStorage(
    std::shared_ptr<SingleKvStore> &kvStorePtr, const std::string &bundleName)
{
    Key key(bundleName);
    status = kvStorePtr->Delete(key);
    if (status != Status::SUCCESS) {
        EXPECT_TRUE(false) << "delete fail";
    }
}

void BmsBundleInstallerModuleTest::DeleteBundleDataToStorage()
{
    GTEST_LOG_(INFO) << "begin";
    DistributedKvDataManager dataManager;

    AppId appId{Constants::APP_ID};
    StoreId storeId{Constants::STORE_ID};
    auto status = dataManager.DeleteKvStore(appId, storeId);
    if (status != Status::SUCCESS) {
        GTEST_LOG_(INFO) << static_cast<int32_t>(status);
    }
    GTEST_LOG_(INFO) << "end";
}

std::shared_ptr<SingleKvStore> BmsBundleInstallerModuleTest::GetKvStorePtr(DistributedKvDataManager &dataManager)
{
    Options options;
    options.createIfMissing = true;
    options.encrypt = false;
    options.autoSync = true;
    options.kvStoreType = KvStoreType::SINGLE_VERSION;
    AppId appId{Constants::APP_ID};
    StoreId storeId{Constants::STORE_ID};
    std::shared_ptr<SingleKvStore> kvStorePtr = nullptr;
    Status status = dataManager.GetSingleKvStore(options, appId, storeId, kvStorePtr);
    if (status != Status::SUCCESS) {
        APP_LOGE("BundleDataStorage::GetKvStore return error: %{public}d", status);
    }
    return kvStorePtr;
}

InnerBundleInfo BmsBundleInstallerModuleTest::CreateInnerBundleInfo(const std::string &bundleName)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfoJson_["baseApplicationInfo"]["bundleName"] = bundleName;
    innerBundleInfoJson_["baseBundleInfo"]["name"] = bundleName;
    innerBundleInfo.FromJson(innerBundleInfoJson_);
    return innerBundleInfo;
}

void BmsBundleInstallerModuleTest::CheckFileExist(const std::string &bundleName) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundleName).c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir does not exists: " << bundleName;
    int codeExist = access((BUNDLE_CODE_PATH + bundleName).c_str(), F_OK);
    EXPECT_EQ(codeExist, 0) << "the ability code file does not exist: " << bundleName;
}

void BmsBundleInstallerModuleTest::CheckFileExist(const std::string &bundleName, const std::string &modulePackage) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundleName + "/" + modulePackage).c_str(), F_OK);
    EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir does not exists: " << modulePackage;
    int codeExist = access((BUNDLE_DATA_PATH + bundleName + "/" + modulePackage).c_str(), F_OK);
    EXPECT_EQ(codeExist, 0) << "the ability code file does not exist: " << modulePackage;
}

void BmsBundleInstallerModuleTest::CheckFileNonExist(const std::string &bundleName) const
{
    int bundleDataExist = access((BUNDLE_DATA_PATH + bundleName).c_str(), F_OK);
    EXPECT_NE(bundleDataExist, 0) << "the bundle data dir exists: " << bundleName;
    int codeExist = access((BUNDLE_CODE_PATH + bundleName).c_str(), F_OK);
    EXPECT_NE(codeExist, 0) << "the ability code exists: " << bundleName;
}

void BmsBundleInstallerModuleTest::CheckFileExist(
    const std::string &bundleName, const std::string &modulePackage, const std::vector<std::string> &abilityNames) const
{
    int bundleDataExist = 1;
    int codeExist = 1;
    if (abilityNames.size() == 0) {
        CheckFileExist(bundleName, modulePackage);
    }
    for (auto iter = abilityNames.begin(); iter != abilityNames.end(); iter++) {
        bundleDataExist = access((BUNDLE_DATA_PATH + bundleName + "/" + modulePackage + "/" + *iter).c_str(), F_OK);
        EXPECT_EQ(bundleDataExist, 0) << "the bundle data dir does not exists: " << *iter;
        codeExist = access((BUNDLE_DATA_PATH + bundleName + "/" + modulePackage + "/" + *iter).c_str(), F_OK);
        EXPECT_EQ(codeExist, 0) << "the ability code file does not exist: " << *iter;
    }
}

void BmsBundleInstallerModuleTest::SetUpTestCase()
{
    if (access(ROOT_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(ROOT_DIR);
        EXPECT_TRUE(result);
    }
    if (chown(ROOT_DIR.c_str(), ROOT_UID, ROOT_UID) != 0) {
        EXPECT_TRUE(false);
    }
    if (chmod(ROOT_DIR.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
        EXPECT_TRUE(false);
    }
}

void BmsBundleInstallerModuleTest::TearDownTestCase()
{}

void BmsBundleInstallerModuleTest::SetUp()
{
    std::cout << "BmsBundleInstallerModuleTest SetUp Begin" << std::endl;
    StartInstalld();
    StartBundleMgrService();
    std::cout << "BmsBundleInstallerModuleTest SetUp End" << std::endl;
}

void BmsBundleInstallerModuleTest::TearDown()
{
    std::cout << "BmsBundleInstallerModuleTest TearDown Begin" << std::endl;
    StopInstalld();
    StopBundleMgrService();
    std::cout << "BmsBundleInstallerModuleTest TearDown End" << std::endl;
}

/**
 * @tc.number: SystemAppInstall_0100
 * @tc.name: test a system bundle can be installed and bundle files exist
 * @tc.desc: 1.under '/system/app/',there is a hap
 *           2.TriggerScan and check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0100, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;

    std::string bundleName = SYSTEM_BUNDLE_NAME + "s1";
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    int checkCount = 0;
    do {
        std::this_thread::sleep_for(50ms);
        ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        checkCount++;
    } while (!ret && (checkCount < 10));

    CheckFileExist(bundleName);
    EXPECT_FALSE((bundleInfo.name).empty());

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: SystemAppInstall_0200
 * @tc.name: test install ten system bundles when the system starts
 * @tc.desc: 1.under '/system/app/',there are two haps
 *           2.TriggerScan and check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0200, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;

    bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;

    int bundleNum = 2;
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;

    for (int i = 1; i <= bundleNum; i++) {
        std::string bundleName = SYSTEM_BUNDLE_NAME + "s" + std::to_string(i);

        bool ret = false;
        int checkCount = 0;
        do {
            std::this_thread::sleep_for(50ms);
            ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
            checkCount++;
        } while (!ret && (checkCount < 10));
        CheckFileExist(bundleName);
        EXPECT_FALSE((bundleInfo.name).empty());

        OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
        EXPECT_NE(recUninstall, nullptr);
        installParam.userId = Constants::DEFAULT_USERID;
        installer->Uninstall(bundleName, installParam, recUninstall);
        EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
    }
}

/**
 * @tc.number: SystemAppInstall_0300
 * @tc.name: test install abnormal system bundles, and check bundle's informations
 * @tc.desc: 1.under '/system/app/',there are three hap bundles, one of them is normal,
 *             others are abnormal
 *           2.TriggerScan and check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0300, Function | MediumTest | Level2)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = SYSTEM_BUNDLE_PATH + "bmsSystemBundle2" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;

    std::string norBundleName = SYSTEM_BUNDLE_NAME + "s2";
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    int checkCount = 0;
    do {
        std::this_thread::sleep_for(50ms);
        ret = dataMgr->GetBundleInfo(norBundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        checkCount++;
    } while (!ret && (checkCount < 10));
    CheckFileExist(norBundleName);
    EXPECT_FALSE((bundleInfo.name).empty());

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(norBundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << norBundleName;

    int bundleNum = 5;
    for (int i = 3; i < bundleNum; i++) {
        std::string invalidBundleName = SYSTEM_BUNDLE_NAME + "s" + std::to_string(i);
        CheckFileNonExist(invalidBundleName);
        BundleInfo invalidBundleInfo;
        dataMgr->GetBundleInfo(invalidBundleName, BundleFlag::GET_BUNDLE_DEFAULT, invalidBundleInfo);
        EXPECT_TRUE((invalidBundleInfo.name).empty());
    }
}

/**
 * @tc.number: SystemAppInstall_0400
 * @tc.name: test install an abnormal system bundle, and check bundle's informations
 * @tc.desc: 1.under '/system/app/',there is a hap, the config.json file of which
 *             does not contain bundle name
 *           2.TriggerScan and check install result
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0400, Function | MediumTest | Level2)
{
    std::string bundleName = SYSTEM_BUNDLE_NAME + "s3";
    CheckFileNonExist(bundleName);
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    int checkCount = 0;
    do {
        std::this_thread::sleep_for(1ms);
        ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        checkCount++;
    } while (!ret && (checkCount < 10));

    EXPECT_TRUE((bundleInfo.name).empty());
}

/**
 * @tc.number: SystemAppInstall_0500
 * @tc.name: test install an abnormal system bundle, and check bundle's informations
 * @tc.desc: 1.under '/system/app/',there is a hap, which doesn't have the config.json
 *           2.TriggerScan and check install result
 */
HWTEST_F(BmsBundleInstallerModuleTest, SystemAppInstall_0500, Function | MediumTest | Level2)
{
    std::string bundleName = SYSTEM_BUNDLE_NAME + "s4";
    CheckFileNonExist(bundleName);
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    int checkCount = 0;
    do {
        std::this_thread::sleep_for(1ms);
        ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        checkCount++;
    } while (!ret && (checkCount < 10));

    EXPECT_TRUE((bundleInfo.name).empty());
}

/**
 * @tc.number: ThirdAppInstall_0100
 * @tc.name: test third-party bundle install
 * @tc.desc: 1.under '/data/test/bms_bundle/',there is a hap
 *           2.install the bundle and check results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0100, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    CheckFileExist(bundleName);
    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0200
 * @tc.name: test third-party bundle install and bundle info store
 * @tc.desc: 1.under '/data/test/bms_bundle/',there is a hap
 *           2.check install results
 *           3.restart bundmgrservice
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0200, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK);
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    CheckFileExist(bundleName);

    StopBundleMgrService();
    StartBundleMgrService();
    CheckFileExist(bundleName);
    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    BundleInfo bundleInfo;
    bool ret = false;
    ret = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(bundleInfo.name, bundleName);

    installParam.userId = Constants::DEFAULT_USERID;
    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK);
}

/**
 * @tc.number: ThirdAppInstall_0300
 * @tc.name: test third-party bundle install and bundle info store
 * @tc.desc: 1.under '/data/test/bms_bundle/',there a hap,the suffix
 *             of which is wrong
 *           2.check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0300, Function | MediumTest | Level2)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle12" + ERROR_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME);
    std::string bundleName = THIRD_BUNDLE_NAME + "12";
    CheckFileNonExist(bundleName);

    BundleInfo info;
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    auto dataMgr = bms->GetDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    bool isBundleExist = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_FALSE(isBundleExist);
}

/**
 * @tc.number: ThirdAppInstall_0400
 * @tc.name: test third bundle install and bundle info store
 * @tc.desc: 1.under '/data/test/bms_bundle/',there is a big hap
 *           2.install and check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0400, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleName = THIRD_BUNDLE_NAME + "5";
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle13" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    StopInstalld();
    std::this_thread::sleep_for(1ms);

    EXPECT_NE(receiver->GetResultCode(), ERR_OK);

    std::this_thread::sleep_for(5ms);
    CheckFileNonExist(bundleName);

    StartInstalld();
    std::this_thread::sleep_for(1ms);
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK);
    CheckFileExist(bundleName);
    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK);
}

/**
 * @tc.number: ThirdAppInstall_0500
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.one hap has been installed in system
 *           2.under '/data/test/bms_bundle',there a hap,the version of which is lower than the installed one
 *           3.check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0500, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> normalInstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(normalInstall, nullptr);

    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle9" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, normalInstall);
    EXPECT_EQ(normalInstall->GetResultCode(), ERR_OK);
    std::string bundleName = THIRD_BUNDLE_NAME + "2";
    CheckFileExist(bundleName);
    std::string upgradeFilePath = BUNDLE_TMPPATH + "bmsThirdBundle7" + Constants::INSTALL_FILE_SUFFIX;
    OHOS::sptr<MockStatusReceiver> replaceInstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(replaceInstall, nullptr);
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installer->Install(upgradeFilePath, installParam, replaceInstall);
    EXPECT_EQ(replaceInstall->GetResultCode(), ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE);
    CheckFileExist(bundleName);

    OHOS::sptr<MockStatusReceiver> uninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(uninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, uninstall);
    EXPECT_EQ(uninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0600
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.one hap has been installed in system
 *           2.under '/data/test/bms_bundle',there a hap,the version of which is higher than the installed one
 *           3.check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0600, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> normalInstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(normalInstall, nullptr);

    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle7" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, normalInstall);
    EXPECT_EQ(normalInstall->GetResultCode(), ERR_OK);
    std::string bundleName = THIRD_BUNDLE_NAME + "2";
    CheckFileExist(bundleName);
    std::string upgradeFilePath = BUNDLE_TMPPATH + "bmsThirdBundle9" + Constants::INSTALL_FILE_SUFFIX;
    OHOS::sptr<MockStatusReceiver> replaceInstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(replaceInstall, nullptr);
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installer->Install(upgradeFilePath, installParam, replaceInstall);
    EXPECT_EQ(replaceInstall->GetResultCode(), ERR_OK);
    CheckFileExist(bundleName);

    StopBundleMgrService();
    StartBundleMgrService();
    CheckFileExist(bundleName);

    BundleInfo info;
    std::shared_ptr<BundleMgrService> bms = DelayedSingleton<BundleMgrService>::GetInstance();
    auto dataMgr = bms->GetDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    bool isBundleExist = dataMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, info);
    EXPECT_TRUE(isBundleExist);

    std::cout << "info-name:" << info.name << std::endl;

    OHOS::sptr<MockStatusReceiver> uninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(uninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, uninstall);
    EXPECT_EQ(uninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0700
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.one hap has been installed in system
 *           2.under '/data/test/bms_bundle',there a hap,the version of which is equal to the installed one
 *           3.check install results
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0700, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> normalInstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(normalInstall, nullptr);

    InstallParam installParam;
    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle7" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, normalInstall);
    EXPECT_EQ(normalInstall->GetResultCode(), ERR_OK);
    std::string bundleName = THIRD_BUNDLE_NAME + "2";
    CheckFileExist(bundleName);
    std::string upgradeFilePath = BUNDLE_TMPPATH + "bmsThirdBundle10" + Constants::INSTALL_FILE_SUFFIX;
    OHOS::sptr<MockStatusReceiver> replaceInstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(replaceInstall, nullptr);
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installer->Install(upgradeFilePath, installParam, replaceInstall);
    EXPECT_EQ(replaceInstall->GetResultCode(), ERR_OK);
    CheckFileExist(bundleName);

    OHOS::sptr<MockStatusReceiver> uninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(uninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, uninstall);
    EXPECT_EQ(uninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0800
 * @tc.name: test third-party bundle install
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps with one ability,whose appname is equal
 *           2.install the app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0800, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    CheckFileExist(bundleName, modulePackage);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    CheckFileExist(bundleName, modulePackage2);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_0900
 * @tc.name: test third-party bundle install
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps without an ability,whose appname is equal
 *           2.install the app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_0900, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle3" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle6" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1000
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,one without an ability,the other with an ability
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1000, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle3" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1100
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,
 *             one without an ability,the other with two abilities
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1100, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle3" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle5" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1200
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,one without an ability,the other with an ability
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1200, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle6" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1300
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps with an ability,
 *                  2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1300, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1400
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,one with an ability,the other with two abilities
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1400, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle1" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle5" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1500
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,
 *             one without an ability,the other with two abilities
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1500, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle2" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle6" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1600
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps,one with an ability,the other with two abilities
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1600, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle2" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle4" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: ThirdAppInstall_1700
 * @tc.name: test third-party bundle upgrade
 * @tc.desc: 1.under '/data/test/bms_bundle/',there are two haps with two abilities,
 *           2.install this app
 */
HWTEST_F(BmsBundleInstallerModuleTest, ThirdAppInstall_1700, Function | MediumTest | Level1)
{
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    EXPECT_NE(installer, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver, nullptr);
    OHOS::sptr<MockStatusReceiver> receiver2 = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(receiver2, nullptr);
    InstallParam installParam;

    installParam.installFlag = InstallFlag::NORMAL;
    std::string bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle2" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver);
    EXPECT_EQ(receiver->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string bundleName = THIRD_BUNDLE_NAME + "1";
    std::string modulePackage = "com.third.hiworld.example.h1";
    std::vector<std::string> hap1AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage, hap1AbilityNames);

    bundleFilePath = BUNDLE_TMPPATH + "bmsThirdBundle5" + Constants::INSTALL_FILE_SUFFIX;
    installer->Install(bundleFilePath, installParam, receiver2);
    EXPECT_EQ(receiver2->GetResultCode(), ERR_OK) << "install fail!" << bundleFilePath;
    std::string modulePackage2 = "com.third.hiworld.example.h2";
    std::vector<std::string> hap2AbilityNames = {"bmsThirdBundle_A1", "bmsThirdBundle_A2"};
    CheckFileExist(bundleName, modulePackage2, hap2AbilityNames);

    OHOS::sptr<MockStatusReceiver> recUninstall = new (std::nothrow) MockStatusReceiver();
    EXPECT_NE(recUninstall, nullptr);
    installParam.userId = Constants::DEFAULT_USERID;
    installer->Uninstall(bundleName, installParam, recUninstall);
    EXPECT_EQ(recUninstall->GetResultCode(), ERR_OK) << "uninstall fail!" << bundleName;
}

/**
 * @tc.number: BundleDataStorage_0100
 * @tc.name: save bundle install information to persist storage
 * @tc.desc: test storage too many data, and query data
 */
HWTEST_F(BmsBundleInstallerModuleTest, BundleDataStorage_0100, Function | MediumTest | Level1)
{
    BundleDataStorageDatabase bundleDataStorage;
    int repeatTimes = 10;
    int saveTimes = 100;
    std::string baseBundleName = "ohos.CR";
    std::vector<InnerBundleInfo> innerBundleInfos;
    for (int i = 0; i < repeatTimes; i++) {
        /**
         * @tc.steps: step1. save too many data in datastorage
         * @tc.expected: step1. save success
         */
        std::vector<std::string> bundleNames;
        std::vector<std::string> innerBundleInfoStrs;
        for (int j = 0; j < saveTimes; j++) {
            std::string bundleName = baseBundleName + std::to_string(i * saveTimes + j);
            InnerBundleInfo innerBundleInfo = CreateInnerBundleInfo(bundleName);
            bool res = bundleDataStorage.SaveStorageBundleInfo(innerBundleInfo);
            EXPECT_TRUE(res) << "save bundle data fail";
            bundleNames.emplace_back(bundleName);
            innerBundleInfoStrs.emplace_back(innerBundleInfo.ToString());
            innerBundleInfos.emplace_back(innerBundleInfo);
        }
        /**
         * @tc.steps: step2. query data
         * @tc.expected: step2. query success
         */
        std::map<std::string, std::map<std::string, InnerBundleInfo>> dataMap;
        bool loadRes = bundleDataStorage.LoadAllData(dataMap);
        EXPECT_TRUE(loadRes) << "load data fail!!!";
        for (std::string item : bundleNames) {
            std::map<std::string, InnerBundleInfo> innerBundleInfoMap = dataMap[item];
            for (auto iter = innerBundleInfoMap.begin(); iter != innerBundleInfoMap.end(); ++iter) {
                std::string innerBundleInfoStr = iter->second.ToString();
                for (auto innerBundleInfo = innerBundleInfoStrs.begin(); innerBundleInfo != innerBundleInfoStrs.end();
                     ++innerBundleInfo) {
                    if (*innerBundleInfo != innerBundleInfoStr && innerBundleInfo == innerBundleInfoStrs.end()) {
                        EXPECT_TRUE(false) << "get bundleinfo fail!";
                    } else {
                        break;
                    }
                }
            }
        }
    }
    for (InnerBundleInfo inner : innerBundleInfos) {
        bool delRes = bundleDataStorage.DeleteStorageBundleInfo(inner);
        EXPECT_TRUE(delRes) << "delete data fail!!!";
    }
}

/**
 * @tc.number: BundleDataStorage_0200
 * @tc.name: save bundle install information to persist storage
 * @tc.desc: test storage too many data, load data to memory
 */
HWTEST_F(BmsBundleInstallerModuleTest, BundleDataStorage_0200, Function | MediumTest | Level1)
{
    /**
     * @tc.steps: step1. save too many data in datastorage
     */
    BundleDataStorageDatabase bundleDataStorage;
    int saveTimes = 100;
    std::string baseBundleName = "ohos.bundlestoragetest";
    std::vector<std::string> bundleNames;
    std::vector<InnerBundleInfo> innerBundleInfos;
    for (int i = 0; i < saveTimes; i++) {
        std::string bundleName = baseBundleName + std::to_string(i);
        InnerBundleInfo innerBundleInfo = CreateInnerBundleInfo(bundleName);
        bool res = bundleDataStorage.SaveStorageBundleInfo(innerBundleInfo);
        EXPECT_TRUE(res) << "save bundle data fail";
        bundleNames.emplace_back(bundleName);
        innerBundleInfos.emplace_back(innerBundleInfo);
    }

    /**
     * @tc.steps: step2. load date to memory
     * @tc.expected: step2. load success
     */
    StopBundleMgrService();
    StartBundleMgrService();

    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    if (dataMgr == nullptr) {
        EXPECT_TRUE(false);
    }

    for (std::string item : bundleNames) {
        ApplicationInfo appInfo;
        bool getAppInfo =
            dataMgr->GetApplicationInfo(item, ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, 0, appInfo);
        EXPECT_TRUE(getAppInfo) << "Get Application: " + item + " fail!";
        EXPECT_EQ(appInfo.bundleName, item) << "appinfo bundleName is wrong";
    }

    for (InnerBundleInfo inner : innerBundleInfos) {
        bool delRes = bundleDataStorage.DeleteStorageBundleInfo(inner);
        EXPECT_TRUE(delRes) << "delete data fail!!!";
    }
}

/**
 * @tc.number: BundleDataStorage_0200
 * @tc.name: save bundle install information to persist storage
 * @tc.desc: test when storage abnormal data,it does not influence other data to read
 */
HWTEST_F(BmsBundleInstallerModuleTest, BundleDataStorage003, TestSize.Level3)
{
    /**
     * @tc.steps: step1. load data to storageDb, which have normal and abnormal data
     */
    StartBundleMgrService();
    DistributedKvDataManager dataManager;
    std::shared_ptr<SingleKvStore> kvStorePtr = GetKvStorePtr(dataManager);
    if (!kvStorePtr) {
        EXPECT_TRUE(false) << "kvStorePtr is nullptr";
    }

    int saveTimes = 100;
    int pos = 50;
    std::string abnormalData = "{";
    std::vector<std::string> bundleNames;
    for (int i = 0; i < saveTimes; i++) {
        std::string bundleName = "ohos.system" + std::to_string(i);
        if (i == pos) {
            SaveBundleDataToStorage(kvStorePtr, bundleName, abnormalData);
        } else {
            InnerBundleInfo innerBundleInfo = CreateInnerBundleInfo(bundleName);
            SaveBundleDataToStorage(kvStorePtr, bundleName, innerBundleInfo.ToString());
        }
        bundleNames.emplace_back(bundleName);
    }

    /**
     * @tc.steps: step2. load data to memory
     * @tc.expected: step2. load success, normal data can save success, abnormal data can save fail
     */
    BundleDataStorageDatabase bundleDataStorage;
    std::map<std::string, std::map<std::string, InnerBundleInfo>> bundleDatas;
    bool getBundleData = bundleDataStorage.LoadAllData(bundleDatas);
    EXPECT_TRUE(getBundleData) << "get bundle data from database fail!";

    StopBundleMgrService();
    StartBundleMgrService();

    std::shared_ptr<BundleDataMgr> dataMgr = GetBundleDataMgr();
    if (dataMgr == nullptr) {
        EXPECT_TRUE(false);
    }
    ApplicationInfo appInfo;
    for (int i = 0; i < saveTimes; i++) {
        bool getAppInfo =
            dataMgr->GetApplicationInfo(
                bundleNames[i], ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, 0, appInfo);
        if (i == pos) {
            EXPECT_FALSE(getAppInfo) << "Get Application: " + bundleNames[i] + " success!";
        } else {
            EXPECT_TRUE(getAppInfo) << "Get Application: " + bundleNames[i] + " fail!";
            EXPECT_EQ(appInfo.bundleName, bundleNames[i]) << "appinfo bundleName is wrong";
        }
        DeleteBundleDataToStorage(kvStorePtr, bundleNames[i]);
    }
    AppId appId{Constants::APP_ID};
    StoreId storeId{Constants::STORE_ID};
    dataManager.CloseKvStore(appId, storeId);
}
} // OHOS