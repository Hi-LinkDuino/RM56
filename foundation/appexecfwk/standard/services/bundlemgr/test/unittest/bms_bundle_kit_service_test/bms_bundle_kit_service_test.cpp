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

#include <chrono>
#include <fstream>
#include <gtest/gtest.h>

#include "ability_manager_client.h"
#include "ability_info.h"
#include "bundle_clone_mgr.h"
#include "bundle_data_mgr.h"
#include "bundle_info.h"
#include "bundle_permission_mgr.h"
#include "bundle_mgr_service.h"
#include "bundle_mgr_host.h"
#include "directory_ex.h"
#include "install_param.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "inner_bundle_info.h"
#include "launcher_service.h"
#include "mock_clean_cache.h"
#include "mock_bundle_status.h"
#include "ohos/aafwk/content/want.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using OHOS::AAFwk::Want;

namespace OHOS {
namespace {
const std::string BUNDLE_NAME_TEST = "com.example.bundlekit.test";
const std::string MODULE_NAME_TEST = "com.example.bundlekit.test.entry";
const std::string MODULE_NAME_TEST_1 = "com.example.bundlekit.test.entry_A";
const std::string MODULE_NAME_TEST_2 = "com.example.bundlekit.test.entry_B";
const std::string MODULE_NAME_TEST_3 = "com.example.bundlekit.test.entry_C";
const std::string ABILITY_NAME_TEST1 = ".Reading1";
const std::string ABILITY_NAME_TEST = ".Reading";
const int32_t BASE_TEST_UID = 65535;
const int32_t TEST_UID = 20065535;
const std::string BUNDLE_LABEL = "Hello, OHOS";
const std::string BUNDLE_DESCRIPTION = "example helloworld";
const std::string BUNDLE_VENDOR = "example";
const std::string BUNDLE_VERSION_NAME = "1.0.0.1";
const std::string BUNDLE_MAIN_ABILITY = "com.example.bundlekit.test.entry";
const int32_t BUNDLE_MAX_SDK_VERSION = 0;
const int32_t BUNDLE_MIN_SDK_VERSION = 0;
const std::string BUNDLE_JOINT_USERID = "3";
const uint32_t BUNDLE_VERSION_CODE = 1001;
const std::string BUNDLE_NAME_TEST1 = "com.example.bundlekit.test1";
const std::string BUNDLE_NAME_DEMO = "com.example.bundlekit.demo";
const std::string MODULE_NAME_DEMO = "com.example.bundlekit.demo.entry";
const std::string ABILITY_NAME_DEMO = ".Writing";
const int DEMO_UID = 30001;
const std::string PACKAGE_NAME = "com.example.bundlekit.test.entry";
const std::string PROCESS_TEST = "test.process";
const std::string DEVICE_ID = "PHONE-001";
const int APPLICATION_INFO_FLAGS = 1;
const int DEFAULT_USER_ID_TEST = 100;
const int NEW_USER_ID_TEST = 200;
const std::string LABEL = "hello";
const std::string DESCRIPTION = "mainEntry";
const std::string THEME = "mytheme";
const std::string ICON_PATH = "/data/data/icon.png";
const std::string KIND = "test";
const std::string ACTION = "action.system.home";
const std::string ENTITY = "entity.system.home";
const std::string TARGET_ABILITY = "MockTargetAbility";
const AbilityType ABILITY_TYPE = AbilityType::PAGE;
const DisplayOrientation ORIENTATION = DisplayOrientation::PORTRAIT;
const LaunchMode LAUNCH_MODE = LaunchMode::SINGLETON;
const uint32_t FORM_ENTITY = 1;
const uint32_t BACKGROUND_MODES = 1;
const std::vector<std::string> CONFIG_CHANGES = {"locale"};
const int DEFAULT_FORM_HEIGHT = 100;
const int DEFAULT_FORM_WIDTH = 200;
const std::string META_DATA_DESCRIPTION = "description";
const std::string META_DATA_NAME = "name";
const std::string META_DATA_TYPE = "type";
const std::string META_DATA_VALUE = "value";
const std::string META_DATA_EXTRA = "extra";
const ModuleColorMode COLOR_MODE = ModuleColorMode::AUTO;
const std::string CODE_PATH = "/data/app/el1/bundle/public/com.example.bundlekit.test";
const std::string RESOURCE_PATH = "/data/app/el1/bundle/public/com.example.bundlekit.test/res";
const std::string LIB_PATH = "/data/app/el1/bundle/public/com.example.bundlekit.test/lib";
const bool VISIBLE = true;
const std::string MAIN_ENTRY = "com.example.bundlekit.test.entry";
const uint32_t ABILITY_SIZE_ZERO = 0;
const uint32_t ABILITY_SIZE_ONE = 1;
const uint32_t PERMISSION_SIZE_ZERO = 0;
const uint32_t PERMISSION_SIZE_TWO = 2;
const uint32_t META_DATA_SIZE_ONE = 1;
const uint32_t BUNDLE_NAMES_SIZE_ZERO = 0;
const uint32_t BUNDLE_NAMES_SIZE_ONE = 1;
const uint32_t MODULE_NAMES_SIZE_ONE = 1;
const uint32_t MODULE_NAMES_SIZE_TWO = 2;
const uint32_t MODULE_NAMES_SIZE_THREE = 3;
const std::string EMPTY_STRING = "";
const int INVALID_UID = -1;
const std::string ABILITY_URI = "dataability:///com.example.hiworld.himusic.UserADataAbility/person/10";
const std::string URI = "dataability://com.example.hiworld.himusic.UserADataAbility";
const std::string ERROR_URI = "dataability://";
const std::string HAP_FILE_PATH = "/data/test/resource/bms/bundle_kit/test.hap";
const std::string HAP_FILE_PATH1 = "/data/test/resource/bms/bundle_kit/test1.hap";
const std::string ERROR_HAP_FILE_PATH = "/data/test/resource/bms/bundle_kit/error.hap";
const std::string META_DATA = "name";
const std::string ERROR_META_DATA = "String";
const std::string BUNDLE_DATA_DIR = "/data/app/el2/100/base/com.example.bundlekit.test";
const std::string FILES_DIR = "/data/app/el2/100/base/com.example.bundlekit.test/files";
const std::string TEST_FILE_DIR = "/data/app/el2/100/base/com.example.bundlekit.test/files";
const std::string DATA_BASE_DIR = "/data/app/el2/100/database/com.example.bundlekit.test";
const std::string TEST_DATA_BASE_DIR = "/data/app/el2/100/database/com.example.bundlekit.test";
const std::string CACHE_DIR = "/data/app/el2/100/base/com.example.bundlekit.test/cache";
const std::string TEST_CACHE_DIR = "/data/app/el2/100/base/com.example.bundlekit.test/cache/cache";
const std::string FORM_NAME = "form_js";
const std::string FORM_PATH = "data/app";
const std::string FORM_JS_COMPONENT_NAME = "JS";
const std::string FORM_DESCRIPTION = "description";
const std::string FORM_SCHEDULED_UPDATE_TIME = "11:00";
const std::string FORM_CUSTOMIZE_DATAS_NAME = "customizeDataName";
const std::string FORM_CUSTOMIZE_DATAS_VALUE = "customizeDataValue";
const std::string FORM_PORTRAIT_LAYOUTS1 = "port1";
const std::string FORM_PORTRAIT_LAYOUTS2 = "port2";
const std::string FORM_LANDSCAPE_LAYOUTS1 = "land1";
const std::string FORM_LANDSCAPE_LAYOUTS2 = "land2";
const std::string FORM_SRC = "page/card/index";
constexpr int32_t FORM_JS_WINDOW_DESIGNWIDTH = 720;
const std::string SHORTCUT_TEST_ID = "shortcutTestId";
const std::string SHORTCUT_DEMO_ID = "shortcutDemoId";
const std::string SHORTCUT_HOST_ABILITY = "hostAbility";
const std::string SHORTCUT_ICON = "/data/test/bms_bundle";
const std::string SHORTCUT_LABEL = "shortcutLabel";
const std::string SHORTCUT_DISABLE_MESSAGE = "shortcutDisableMessage";
const std::string SHORTCUT_INTENTS_TARGET_BUNDLE = "targetBundle";
const std::string SHORTCUT_INTENTS_TARGET_CLASS = "targetClass";
const std::string COMMON_EVENT_NAME = ".MainAbililty";
const std::string COMMON_EVENT_PERMISSION = "permission";
const std::string COMMON_EVENT_DATA = "data";
const std::string COMMON_EVENT_TYPE = "type";
const std::string COMMON_EVENT_EVENT = "usual.event.PACKAGE_ADDED";
const std::string COMMON_EVENT_EVENT_ERROR_KEY = "usual.event.PACKAGE_ADDED_D";
const std::string COMMON_EVENT_EVENT_NOT_EXISTS_KEY = "usual.event.PACKAGE_REMOVED";
const int FORMINFO_DESCRIPTIONID = 123;
const std::string ACTION_001 = "action001";
const std::string ACTION_002 = "action002";
const std::string ENTITY_001 = "entity001";
const std::string ENTITY_002 = "entity002";
const std::string TYPE_001 = "type001";
const std::string TYPE_002 = "type002";
const std::string TYPE_IMG_REGEX = "img/*";
const std::string TYPE_IMG_JPEG = "img/jpeg";
const std::string SCHEME_SEPARATOR = "://";
const std::string PORT_SEPARATOR = ":";
const std::string PATH_SEPARATOR = "/";
const std::string SCHEME_001 = "scheme001";
const std::string SCHEME_002 = "scheme002";
const std::string HOST_001 = "host001";
const std::string HOST_002 = "host002";
const std::string PORT_001 = "port001";
const std::string PORT_002 = "port002";
const std::string PATH_001 = "path001";
const std::string PATH_REGEX_001 = ".*";
const std::string URI_PATH_001 = SCHEME_001 + SCHEME_SEPARATOR + HOST_001 +
    PORT_SEPARATOR + PORT_001 + PATH_SEPARATOR + PATH_001;
const std::string URI_PATH_DUPLICATE_001 = SCHEME_001 + SCHEME_SEPARATOR +
    HOST_001 + PORT_SEPARATOR + PORT_001 + PATH_SEPARATOR + PATH_001 + PATH_001;
const std::string URI_PATH_REGEX_001 = SCHEME_001 + SCHEME_SEPARATOR + HOST_001 +
    PORT_SEPARATOR + PORT_001 + PATH_SEPARATOR + PATH_REGEX_001;
const int32_t DEFAULT_USERID = 100;
const int32_t WAIT_TIME = 5; // init mocked bms
}  // namespace

class BmsBundleKitServiceTest : public testing::Test {
public:
    using Want = OHOS::AAFwk::Want;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<BundleDataMgr> GetBundleDataMgr() const;
    std::shared_ptr<LauncherService> GetLauncherService() const;
    std::shared_ptr<BundleCloneMgr> GetBundleCloneMgr() const;
    void MockInnerBundleInfo(const std::string &bundleName, const std::string &moduleName,
        const std::string &abilityName, const std::vector<std::string> &dependencies,
        InnerBundleInfo &innerBundleInfo) const;
    void MockInstallBundle(
        const std::string &bundleName, const std::string &moduleName, const std::string &abilityName,
        bool userDataClearable = true, bool isSystemApp = false) const;
    void MockInstallBundle(
        const std::string &bundleName, const std::vector<std::string> &moduleNameList, const std::string &abilityName,
        bool userDataClearable = true, bool isSystemApp = false) const;
    void MockUninstallBundle(const std::string &bundleName) const;
    AbilityInfo MockAbilityInfo(
        const std::string &bundleName, const std::string &module, const std::string &abilityName) const;
    InnerModuleInfo MockModuleInfo(const std::string &moduleName) const;
    FormInfo MockFormInfo(
        const std::string &bundleName, const std::string &module, const std::string &abilityName) const;
    ShortcutInfo MockShortcutInfo(const std::string &bundleName, const std::string &shortcutId) const;
    CommonEventInfo MockCommonEventInfo(const std::string &bundleName, const int uid) const;
    void CheckBundleInfo(const std::string &bundleName, const std::string &moduleName, const uint32_t abilitySize,
        const BundleInfo &bundleInfo) const;
    void CheckBundleArchiveInfo(const std::string &bundleName, const std::string &moduleName,
        const uint32_t abilitySize, const BundleInfo &bundleInfo) const;
    void CheckBundleList(const std::string &bundleName, const std::vector<std::string> &bundleList) const;
    void CheckApplicationInfo(
        const std::string &bundleName, const uint32_t permissionSize, const ApplicationInfo &appInfo) const;
    void CheckAbilityInfo(const std::string &bundleName, const std::string &abilityName, int32_t flags,
        const AbilityInfo &appInfo) const;
    void CheckAbilityInfos(const std::string &bundleName, const std::string &abilityName, int32_t flags,
        const std::vector<AbilityInfo> &appInfo) const;
    void CheckCompatibleApplicationInfo(
        const std::string &bundleName, const uint32_t permissionSize, const CompatibleApplicationInfo &appInfo) const;
    void CheckCompatibleAbilityInfo(
        const std::string &bundleName, const std::string &abilityName, const CompatibleAbilityInfo &appInfo) const;
    void CheckInstalledBundleInfos(const uint32_t abilitySize, const std::vector<BundleInfo> &bundleInfos) const;
    void CheckInstalledApplicationInfos(const uint32_t permsSize, const std::vector<ApplicationInfo> &appInfos) const;
    void CheckModuleInfo(const HapModuleInfo &hapModuleInfo) const;
    void CreateFileDir() const;
    void CleanFileDir() const;
    void CheckFileExist() const;
    void CheckFileNonExist() const;
    void CheckCacheExist() const;
    void CheckCacheNonExist() const;
    void CheckFormInfoTest(const std::vector<FormInfo> &forms) const;
    void CheckFormInfoDemo(const std::vector<FormInfo> &forms) const;
    void CheckShortcutInfoTest(std::vector<ShortcutInfo> &shortcutInfos) const;
    void CheckCommonEventInfoTest(std::vector<CommonEventInfo> &commonEventInfos) const;
    void CheckShortcutInfoDemo(std::vector<ShortcutInfo> &shortcutInfos) const;
    void AddBundleInfo(const std::string &bundleName, BundleInfo &bundleInfo) const;
    void AddApplicationInfo(const std::string &bundleName, ApplicationInfo &appInfo,
        bool userDataClearable = true, bool isSystemApp = false) const;
    void AddInnerBundleInfoByTest(const std::string &bundleName, const std::string &moduleName,
        const std::string &abilityName, InnerBundleInfo &innerBundleInfo) const;
    void SaveToDatabase(const std::string &bundleName, InnerBundleInfo &innerBundleInfo,
        bool userDataClearable, bool isSystemApp) const;

public:
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
    std::shared_ptr<InstalldService> service_ = std::make_shared<InstalldService>();
    std::shared_ptr<LauncherService> launcherService_ = std::make_shared<LauncherService>();
};

void BmsBundleKitServiceTest::SetUpTestCase()
{}

void BmsBundleKitServiceTest::TearDownTestCase()
{}

void BmsBundleKitServiceTest::SetUp()
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    if (!bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStart();
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
    }
}

void BmsBundleKitServiceTest::TearDown()
{}

std::shared_ptr<BundleDataMgr> BmsBundleKitServiceTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

std::shared_ptr<BundleCloneMgr> BmsBundleKitServiceTest::GetBundleCloneMgr() const
{
    return bundleMgrService_->GetCloneMgr();
}

std::shared_ptr<LauncherService> BmsBundleKitServiceTest::GetLauncherService() const
{
    return launcherService_;
}

void BmsBundleKitServiceTest::AddBundleInfo(const std::string &bundleName, BundleInfo &bundleInfo) const
{
    bundleInfo.name = bundleName;
    bundleInfo.label = BUNDLE_LABEL;
    bundleInfo.description = BUNDLE_DESCRIPTION;
    bundleInfo.vendor = BUNDLE_VENDOR;
    bundleInfo.versionCode = BUNDLE_VERSION_CODE;
    bundleInfo.versionName = BUNDLE_VERSION_NAME;
    bundleInfo.minSdkVersion = BUNDLE_MIN_SDK_VERSION;
    bundleInfo.maxSdkVersion = BUNDLE_MAX_SDK_VERSION;
    bundleInfo.mainEntry = MAIN_ENTRY;
    bundleInfo.isKeepAlive = true;
    bundleInfo.isDifferentName = true;
    bundleInfo.jointUserId = BUNDLE_JOINT_USERID;
    bundleInfo.singleton = true;
}

void BmsBundleKitServiceTest::AddApplicationInfo(const std::string &bundleName, ApplicationInfo &appInfo,
    bool userDataClearable, bool isSystemApp) const
{
    appInfo.bundleName = bundleName;
    appInfo.name = bundleName;
    appInfo.deviceId = DEVICE_ID;
    appInfo.process = PROCESS_TEST;
    appInfo.label = BUNDLE_LABEL;
    appInfo.description = BUNDLE_DESCRIPTION;
    appInfo.codePath = CODE_PATH;
    appInfo.dataDir = FILES_DIR;
    appInfo.dataBaseDir = DATA_BASE_DIR;
    appInfo.cacheDir = CACHE_DIR;
    appInfo.flags = APPLICATION_INFO_FLAGS;
    appInfo.enabled = true;
    appInfo.isCloned = false;
    appInfo.userDataClearable = userDataClearable;
    appInfo.isSystemApp = isSystemApp;
}

void BmsBundleKitServiceTest::AddInnerBundleInfoByTest(const std::string &bundleName,
    const std::string &moduleName, const std::string &abilityName, InnerBundleInfo &innerBundleInfo) const
{
    std::string keyName = bundleName + "." + moduleName + "." + abilityName;
    FormInfo form = MockFormInfo(bundleName, moduleName, abilityName);
    std::vector<FormInfo> formInfos;
    formInfos.emplace_back(form);
    if (bundleName == BUNDLE_NAME_TEST) {
        ShortcutInfo shortcut = MockShortcutInfo(bundleName, SHORTCUT_TEST_ID);
        std::string shortcutKey = bundleName + moduleName + SHORTCUT_TEST_ID;
        innerBundleInfo.InsertShortcutInfos(shortcutKey, shortcut);
    } else {
        ShortcutInfo shortcut = MockShortcutInfo(bundleName, SHORTCUT_DEMO_ID);
        std::string shortcutKey = bundleName + moduleName + SHORTCUT_DEMO_ID;
        innerBundleInfo.InsertShortcutInfos(shortcutKey, shortcut);
    }
    innerBundleInfo.InsertFormInfos(keyName, formInfos);
    std::string commonEventKey = bundleName + moduleName + abilityName;
    CommonEventInfo eventInfo = MockCommonEventInfo(bundleName, innerBundleInfo.GetUid(DEFAULT_USERID));
    innerBundleInfo.InsertCommonEvents(commonEventKey, eventInfo);
}

void BmsBundleKitServiceTest::MockInstallBundle(
    const std::string &bundleName, const std::string &moduleName, const std::string &abilityName,
    bool userDataClearable, bool isSystemApp) const
{
    InnerModuleInfo moduleInfo = MockModuleInfo(moduleName);
    std::string keyName = bundleName + "." + moduleName + "." + abilityName;
    moduleInfo.entryAbilityKey = keyName;
    AbilityInfo abilityInfo = MockAbilityInfo(bundleName, moduleName, abilityName);
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
    innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
    Skill skill {{ACTION}, {ENTITY}};
    std::vector<Skill> skills;
    skills.emplace_back(skill);
    innerBundleInfo.InsertSkillInfo(keyName, skills);
    SaveToDatabase(bundleName, innerBundleInfo, userDataClearable, isSystemApp);
}

InnerModuleInfo BmsBundleKitServiceTest::MockModuleInfo(const std::string &moduleName) const
{
    InnerModuleInfo moduleInfo;
    RequestPermission reqPermission1;
    reqPermission1.name = "permission1";
    RequestPermission reqPermission2;
    reqPermission2.name = "permission2";
    moduleInfo.requestPermissions = {reqPermission1, reqPermission2};
    moduleInfo.modulePackage = PACKAGE_NAME;
    moduleInfo.moduleName = moduleName;
    moduleInfo.description = BUNDLE_DESCRIPTION;
    moduleInfo.colorMode = COLOR_MODE;
    moduleInfo.label = LABEL;

    AppExecFwk::CustomizeData customizeData {"name", "value", "extra"};
    MetaData metaData {{customizeData}};
    moduleInfo.metaData = metaData;
    return moduleInfo;
}

void BmsBundleKitServiceTest::SaveToDatabase(const std::string &bundleName,
    InnerBundleInfo &innerBundleInfo, bool userDataClearable, bool isSystemApp) const
{
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);

    InnerBundleUserInfo innerBundleUserInfo;
    innerBundleUserInfo.bundleName = bundleName;
    innerBundleUserInfo.bundleUserInfo.enabled = true;
    innerBundleUserInfo.bundleUserInfo.userId = Constants::DEFAULT_USERID;
    innerBundleUserInfo.uid = BASE_TEST_UID;

    InnerBundleUserInfo innerBundleUserInfo1;
    innerBundleUserInfo1.bundleName = bundleName;
    innerBundleUserInfo1.bundleUserInfo.enabled = true;
    innerBundleUserInfo1.bundleUserInfo.userId = DEFAULT_USERID;
    innerBundleUserInfo1.uid = TEST_UID;

    ApplicationInfo appInfo;
    AddApplicationInfo(bundleName, appInfo, userDataClearable, isSystemApp);
    BundleInfo bundleInfo;
    AddBundleInfo(bundleName, bundleInfo);
    innerBundleInfo.SetBaseApplicationInfo(appInfo);
    innerBundleInfo.AddInnerBundleUserInfo(innerBundleUserInfo);
    innerBundleInfo.AddInnerBundleUserInfo(innerBundleUserInfo1);
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    auto accessTokenId = BundlePermissionMgr::CreateAccessTokenId(innerBundleInfo, bundleName, DEFAULT_USERID);
    innerBundleInfo.SetAccessTokenId(accessTokenId, DEFAULT_USERID);
    auto moduleNameVec = innerBundleInfo.GetModuleNameVec();
    auto abilityNameVec = innerBundleInfo.GetAbilityNames();
    if (!moduleNameVec.empty() && !abilityNameVec.empty()) {
        AddInnerBundleInfoByTest(bundleName, moduleNameVec[0], abilityNameVec[0], innerBundleInfo);
    }
    bool startRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::INSTALL_START);
    bool addRet = dataMgr->AddInnerBundleInfo(bundleName, innerBundleInfo);
    bool endRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::INSTALL_SUCCESS);

    EXPECT_TRUE(startRet);
    EXPECT_TRUE(addRet);
    EXPECT_TRUE(endRet);
}

void BmsBundleKitServiceTest::MockInstallBundle(
    const std::string &bundleName, const std::vector<std::string> &moduleNameList, const std::string &abilityName,
    bool userDataClearable, bool isSystemApp) const
{
    if (moduleNameList.empty()) {
        return;
    }
    InnerBundleInfo innerBundleInfo;
    for (const auto &moduleName : moduleNameList) {
        InnerModuleInfo moduleInfo = MockModuleInfo(moduleName);
        std::string keyName = bundleName + "." + moduleName + "." + abilityName;
        AbilityInfo abilityInfo = MockAbilityInfo(bundleName, moduleName, abilityName);
        innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
        innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
        Skill skill {{ACTION}, {ENTITY}};
        std::vector<Skill> skills;
        skills.emplace_back(skill);
        innerBundleInfo.InsertSkillInfo(keyName, skills);
    }
    SaveToDatabase(bundleName, innerBundleInfo, userDataClearable, isSystemApp);
}

FormInfo BmsBundleKitServiceTest::MockFormInfo(
    const std::string &bundleName, const std::string &moduleName, const std::string &abilityName) const
{
    FormInfo formInfo;
    formInfo.name = FORM_NAME;
    formInfo.bundleName = bundleName;
    formInfo.abilityName = abilityName;
    formInfo.moduleName = moduleName;
    formInfo.package = PACKAGE_NAME;
    formInfo.descriptionId = FORMINFO_DESCRIPTIONID;
    formInfo.formConfigAbility = FORM_PATH;
    formInfo.description = FORM_DESCRIPTION;
    formInfo.defaultFlag = false;
    formInfo.type = FormType::JS;
    formInfo.colorMode = FormsColorMode::LIGHT_MODE;
    formInfo.supportDimensions = {1, 2};
    formInfo.portraitLayouts = {FORM_PORTRAIT_LAYOUTS1, FORM_PORTRAIT_LAYOUTS2};
    formInfo.landscapeLayouts = {FORM_LANDSCAPE_LAYOUTS1, FORM_LANDSCAPE_LAYOUTS2};
    formInfo.defaultDimension = 1;
    formInfo.updateDuration = 0;
    formInfo.formVisibleNotify = true;
    formInfo.deepLink = FORM_PATH;
    formInfo.scheduledUpdateTime = FORM_SCHEDULED_UPDATE_TIME;
    formInfo.updateEnabled = true;
    formInfo.jsComponentName = FORM_JS_COMPONENT_NAME;
    formInfo.src = FORM_SRC;
    formInfo.window.autoDesignWidth = true;
    formInfo.window.designWidth = FORM_JS_WINDOW_DESIGNWIDTH;
    for (auto &info : formInfo.customizeDatas) {
        info.name = FORM_CUSTOMIZE_DATAS_NAME;
        info.value = FORM_CUSTOMIZE_DATAS_VALUE;
    }
    return formInfo;
}

ShortcutInfo BmsBundleKitServiceTest::MockShortcutInfo(
    const std::string &bundleName, const std::string &shortcutId) const
{
    ShortcutInfo shortcutInfos;
    shortcutInfos.id = shortcutId;
    shortcutInfos.bundleName = bundleName;
    shortcutInfos.hostAbility = SHORTCUT_HOST_ABILITY;
    shortcutInfos.icon = SHORTCUT_ICON;
    shortcutInfos.label = SHORTCUT_LABEL;
    shortcutInfos.disableMessage = SHORTCUT_DISABLE_MESSAGE;
    shortcutInfos.isStatic = true;
    shortcutInfos.isHomeShortcut = true;
    shortcutInfos.isEnables = true;
    for (auto &info : shortcutInfos.intents) {
        info.targetBundle = SHORTCUT_INTENTS_TARGET_BUNDLE;
        info.targetClass = SHORTCUT_INTENTS_TARGET_CLASS;
    }
    return shortcutInfos;
}

CommonEventInfo BmsBundleKitServiceTest::MockCommonEventInfo(
    const std::string &bundleName, const int uid) const
{
    CommonEventInfo CommonEventInfo;
    CommonEventInfo.name = COMMON_EVENT_NAME;
    CommonEventInfo.bundleName = bundleName;
    CommonEventInfo.uid = uid;
    CommonEventInfo.permission = COMMON_EVENT_PERMISSION;
    CommonEventInfo.data.emplace_back(COMMON_EVENT_DATA);
    CommonEventInfo.type.emplace_back(COMMON_EVENT_TYPE);
    CommonEventInfo.events.emplace_back(COMMON_EVENT_EVENT);
    return CommonEventInfo;
}

void BmsBundleKitServiceTest::MockUninstallBundle(const std::string &bundleName) const
{
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    bool startRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_START);
    bool finishRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_SUCCESS);

    EXPECT_TRUE(startRet);
    EXPECT_TRUE(finishRet);
}

AbilityInfo BmsBundleKitServiceTest::MockAbilityInfo(
    const std::string &bundleName, const std::string &moduleName, const std::string &abilityName) const
{
    AbilityInfo abilityInfo;
    abilityInfo.package = PACKAGE_NAME;
    abilityInfo.name = abilityName;
    abilityInfo.bundleName = bundleName;
    abilityInfo.moduleName = moduleName;
    abilityInfo.deviceId = DEVICE_ID;
    abilityInfo.label = LABEL;
    abilityInfo.labelId = 0;
    abilityInfo.description = DESCRIPTION;
    abilityInfo.theme = THEME;
    abilityInfo.iconPath = ICON_PATH;
    abilityInfo.visible = VISIBLE;
    abilityInfo.kind = KIND;
    abilityInfo.type = ABILITY_TYPE;
    abilityInfo.orientation = ORIENTATION;
    abilityInfo.launchMode = LAUNCH_MODE;
    abilityInfo.configChanges = {"locale"};
    abilityInfo.backgroundModes = 1;
    abilityInfo.formEntity = 1;
    abilityInfo.defaultFormHeight = DEFAULT_FORM_HEIGHT;
    abilityInfo.defaultFormWidth = DEFAULT_FORM_WIDTH;
    abilityInfo.codePath = CODE_PATH;
    abilityInfo.resourcePath = RESOURCE_PATH;
    abilityInfo.libPath = LIB_PATH;
    abilityInfo.uri = URI;
    abilityInfo.enabled = true;
    abilityInfo.supportPipMode = false;
    abilityInfo.targetAbility = TARGET_ABILITY;
    AppExecFwk::CustomizeData customizeData {
        "name",
        "value",
        "extra"
    };
    MetaData metaData {
        {customizeData}
    };
    abilityInfo.metaData = metaData;
    abilityInfo.permissions = {"abilityPerm001", "abilityPerm002"};
    return abilityInfo;
}

void BmsBundleKitServiceTest::MockInnerBundleInfo(const std::string &bundleName, const std::string &moduleName,
    const std::string &abilityName, const std::vector<std::string> &dependencies,
    InnerBundleInfo &innerBundleInfo) const
{
    ApplicationInfo appInfo;
    appInfo.bundleName = bundleName;
    BundleInfo bundleInfo;
    bundleInfo.name = bundleName;
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    InnerModuleInfo moduleInfo;
    moduleInfo.modulePackage = moduleName;
    moduleInfo.moduleName = moduleName;
    moduleInfo.description = BUNDLE_DESCRIPTION;
    moduleInfo.dependencies = dependencies;
    innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
    AbilityInfo abilityInfo = MockAbilityInfo(bundleName, moduleName, abilityName);
    std::string keyName = bundleName + "." + moduleName + "." + abilityName;
    innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
    innerBundleInfo.SetBaseApplicationInfo(appInfo);
}

void BmsBundleKitServiceTest::CheckBundleInfo(const std::string &bundleName, const std::string &moduleName,
    const uint32_t abilitySize, const BundleInfo &bundleInfo) const
{
    EXPECT_EQ(bundleName, bundleInfo.name);
    EXPECT_EQ(BUNDLE_LABEL, bundleInfo.label);
    EXPECT_EQ(BUNDLE_DESCRIPTION, bundleInfo.description);
    EXPECT_EQ(BUNDLE_VENDOR, bundleInfo.vendor);
    EXPECT_EQ(BUNDLE_VERSION_CODE, bundleInfo.versionCode);
    EXPECT_EQ(BUNDLE_VERSION_NAME, bundleInfo.versionName);
    EXPECT_EQ(BUNDLE_MIN_SDK_VERSION, bundleInfo.minSdkVersion);
    EXPECT_EQ(BUNDLE_MAX_SDK_VERSION, bundleInfo.maxSdkVersion);
    EXPECT_EQ(BUNDLE_MAIN_ABILITY, bundleInfo.mainEntry);
    EXPECT_EQ(bundleName, bundleInfo.applicationInfo.name);
    EXPECT_EQ(bundleName, bundleInfo.applicationInfo.bundleName);
    EXPECT_EQ(abilitySize, static_cast<uint32_t>(bundleInfo.abilityInfos.size()));
    EXPECT_EQ(true, bundleInfo.isDifferentName);
    EXPECT_EQ(BUNDLE_JOINT_USERID, bundleInfo.jointUserId);
    EXPECT_TRUE(bundleInfo.isKeepAlive);
    EXPECT_TRUE(bundleInfo.singleton);
}

void BmsBundleKitServiceTest::CheckBundleArchiveInfo(const std::string &bundleName, const std::string &moduleName,
    const uint32_t abilitySize, const BundleInfo &bundleInfo) const
{
    EXPECT_EQ(bundleName, bundleInfo.name);
    EXPECT_EQ(BUNDLE_LABEL, bundleInfo.label);
    EXPECT_EQ(BUNDLE_DESCRIPTION, bundleInfo.description);
    EXPECT_EQ(BUNDLE_VENDOR, bundleInfo.vendor);
    EXPECT_EQ(BUNDLE_VERSION_CODE, bundleInfo.versionCode);
    EXPECT_EQ(BUNDLE_VERSION_NAME, bundleInfo.versionName);
    EXPECT_EQ(BUNDLE_MIN_SDK_VERSION, bundleInfo.minSdkVersion);
    EXPECT_EQ(BUNDLE_MAX_SDK_VERSION, bundleInfo.maxSdkVersion);
    EXPECT_EQ(BUNDLE_MAIN_ABILITY, bundleInfo.mainEntry);
    EXPECT_EQ(bundleName, bundleInfo.applicationInfo.name);
    EXPECT_EQ(bundleName, bundleInfo.applicationInfo.bundleName);
    EXPECT_EQ(abilitySize, static_cast<uint32_t>(bundleInfo.abilityInfos.size()));
}

void BmsBundleKitServiceTest::CheckBundleList(
    const std::string &bundleName, const std::vector<std::string> &bundleList) const
{
    EXPECT_TRUE(std::find(bundleList.begin(), bundleList.end(), bundleName) != bundleList.end());
}

void BmsBundleKitServiceTest::CheckApplicationInfo(
    const std::string &bundleName, const uint32_t permissionSize, const ApplicationInfo &appInfo) const
{
    EXPECT_EQ(bundleName, appInfo.name);
    EXPECT_EQ(bundleName, appInfo.bundleName);
    EXPECT_EQ(BUNDLE_LABEL, appInfo.label);
    EXPECT_EQ(BUNDLE_DESCRIPTION, appInfo.description);
    EXPECT_EQ(DEVICE_ID, appInfo.deviceId);
    EXPECT_EQ(PROCESS_TEST, appInfo.process);
    EXPECT_EQ(CODE_PATH, appInfo.codePath);
    EXPECT_EQ(permissionSize, static_cast<uint32_t>(appInfo.permissions.size()));
    EXPECT_EQ(APPLICATION_INFO_FLAGS, appInfo.flags);
}

void BmsBundleKitServiceTest::CheckAbilityInfo(
    const std::string &bundleName, const std::string &abilityName, int32_t flags, const AbilityInfo &abilityInfo) const
{
    EXPECT_EQ(abilityName, abilityInfo.name);
    EXPECT_EQ(bundleName, abilityInfo.bundleName);
    EXPECT_EQ(LABEL, abilityInfo.label);
    EXPECT_EQ(DESCRIPTION, abilityInfo.description);
    EXPECT_EQ(DEVICE_ID, abilityInfo.deviceId);
    EXPECT_EQ(THEME, abilityInfo.theme);
    EXPECT_EQ(ICON_PATH, abilityInfo.iconPath);
    EXPECT_EQ(CODE_PATH, abilityInfo.codePath);
    EXPECT_EQ(ORIENTATION, abilityInfo.orientation);
    EXPECT_EQ(LAUNCH_MODE, abilityInfo.launchMode);
    EXPECT_EQ(URI, abilityInfo.uri);
    EXPECT_EQ(false, abilityInfo.supportPipMode);
    EXPECT_EQ(TARGET_ABILITY, abilityInfo.targetAbility);
    EXPECT_EQ(CONFIG_CHANGES, abilityInfo.configChanges);
    EXPECT_EQ(BACKGROUND_MODES, abilityInfo.backgroundModes);
    EXPECT_EQ(FORM_ENTITY, abilityInfo.formEntity);
    EXPECT_EQ(DEFAULT_FORM_HEIGHT, abilityInfo.defaultFormHeight);
    EXPECT_EQ(DEFAULT_FORM_WIDTH, abilityInfo.defaultFormWidth);
    if ((flags & GET_ABILITY_INFO_WITH_METADATA) != GET_ABILITY_INFO_WITH_METADATA) {
        EXPECT_EQ(0, abilityInfo.metaData.customizeData.size());
    } else {
        for (auto &info : abilityInfo.metaData.customizeData) {
            EXPECT_EQ(info.name, META_DATA_NAME);
            EXPECT_EQ(info.value, META_DATA_VALUE);
            EXPECT_EQ(info.extra, META_DATA_EXTRA);
        }
    }
}

void BmsBundleKitServiceTest::CheckAbilityInfos(const std::string &bundleName, const std::string &abilityName,
    int32_t flags, const std::vector<AbilityInfo> &abilityInfos) const
{
    for (auto abilityInfo : abilityInfos) {
        EXPECT_EQ(abilityName, abilityInfo.name);
        EXPECT_EQ(bundleName, abilityInfo.bundleName);
        EXPECT_EQ(LABEL, abilityInfo.label);
        EXPECT_EQ(DESCRIPTION, abilityInfo.description);
        EXPECT_EQ(DEVICE_ID, abilityInfo.deviceId);
        EXPECT_EQ(THEME, abilityInfo.theme);
        EXPECT_EQ(ICON_PATH, abilityInfo.iconPath);
        EXPECT_EQ(CODE_PATH, abilityInfo.codePath);
        EXPECT_EQ(ORIENTATION, abilityInfo.orientation);
        EXPECT_EQ(LAUNCH_MODE, abilityInfo.launchMode);
        EXPECT_EQ(URI, abilityInfo.uri);
        EXPECT_EQ(false, abilityInfo.supportPipMode);
        EXPECT_EQ(TARGET_ABILITY, abilityInfo.targetAbility);
        EXPECT_EQ(CONFIG_CHANGES, abilityInfo.configChanges);
        EXPECT_EQ(BACKGROUND_MODES, abilityInfo.backgroundModes);
        EXPECT_EQ(FORM_ENTITY, abilityInfo.formEntity);
        EXPECT_EQ(DEFAULT_FORM_HEIGHT, abilityInfo.defaultFormHeight);
        EXPECT_EQ(DEFAULT_FORM_WIDTH, abilityInfo.defaultFormWidth);
        if ((flags & GET_ABILITY_INFO_WITH_METADATA) != GET_ABILITY_INFO_WITH_METADATA) {
            EXPECT_EQ(0, abilityInfo.metaData.customizeData.size());
        } else {
            for (auto &info : abilityInfo.metaData.customizeData) {
                EXPECT_EQ(info.name, META_DATA_NAME);
                EXPECT_EQ(info.value, META_DATA_VALUE);
                EXPECT_EQ(info.extra, META_DATA_EXTRA);
            }
        }
        if ((flags & GET_ABILITY_INFO_WITH_PERMISSION) != GET_ABILITY_INFO_WITH_PERMISSION) {
            EXPECT_EQ(0, abilityInfo.permissions.size());
        } else {
            EXPECT_EQ(PERMISSION_SIZE_TWO, abilityInfo.permissions.size());
        }
    }
}

void BmsBundleKitServiceTest::CheckCompatibleAbilityInfo(
    const std::string &bundleName, const std::string &abilityName, const CompatibleAbilityInfo &abilityInfo) const
{
    EXPECT_EQ(abilityName, abilityInfo.name);
    EXPECT_EQ(bundleName, abilityInfo.bundleName);
    EXPECT_EQ(LABEL, abilityInfo.label);
    EXPECT_EQ(DESCRIPTION, abilityInfo.description);
    EXPECT_EQ(DEVICE_ID, abilityInfo.deviceId);
    EXPECT_EQ(ICON_PATH, abilityInfo.iconPath);
    EXPECT_EQ(ORIENTATION, abilityInfo.orientation);
    EXPECT_EQ(LAUNCH_MODE, abilityInfo.launchMode);
    EXPECT_EQ(URI, abilityInfo.uri);
    EXPECT_EQ(false, abilityInfo.supportPipMode);
    EXPECT_EQ(TARGET_ABILITY, abilityInfo.targetAbility);
    EXPECT_EQ(FORM_ENTITY, abilityInfo.formEntity);
    EXPECT_EQ(DEFAULT_FORM_HEIGHT, abilityInfo.defaultFormHeight);
    EXPECT_EQ(DEFAULT_FORM_WIDTH, abilityInfo.defaultFormWidth);
}
void BmsBundleKitServiceTest::CheckCompatibleApplicationInfo(
    const std::string &bundleName, const uint32_t permissionSize, const CompatibleApplicationInfo &appInfo) const
{
    EXPECT_EQ(bundleName, appInfo.name);
    EXPECT_EQ(BUNDLE_LABEL, appInfo.label);
    EXPECT_EQ(BUNDLE_DESCRIPTION, appInfo.description);
    EXPECT_EQ(PROCESS_TEST, appInfo.process);
    EXPECT_EQ(permissionSize, static_cast<uint32_t>(appInfo.permissions.size()));
}

void BmsBundleKitServiceTest::CheckModuleInfo(const HapModuleInfo &hapModuleInfo) const
{
    EXPECT_EQ(MODULE_NAME_TEST, hapModuleInfo.name);
    EXPECT_EQ(MODULE_NAME_TEST, hapModuleInfo.moduleName);
    EXPECT_EQ(BUNDLE_DESCRIPTION, hapModuleInfo.description);
    EXPECT_EQ(ICON_PATH, hapModuleInfo.iconPath);
    EXPECT_EQ(LABEL, hapModuleInfo.label);
    EXPECT_EQ(COLOR_MODE, hapModuleInfo.colorMode);
}

void BmsBundleKitServiceTest::CheckInstalledBundleInfos(
    const uint32_t abilitySize, const std::vector<BundleInfo> &bundleInfos) const
{
    bool isContainsDemoBundle = false;
    bool isContainsTestBundle = false;
    bool checkDemoAppNameRet = false;
    bool checkTestAppNameRet = false;
    bool checkDemoAbilitySizeRet = false;
    bool checkTestAbilitySizeRet = false;
    for (auto item : bundleInfos) {
        if (item.name == BUNDLE_NAME_DEMO) {
            isContainsDemoBundle = true;
            checkDemoAppNameRet = item.applicationInfo.name == BUNDLE_NAME_DEMO;
            uint32_t num = static_cast<uint32_t>(item.abilityInfos.size());
            checkDemoAbilitySizeRet = num == abilitySize;
        }
        if (item.name == BUNDLE_NAME_TEST) {
            isContainsTestBundle = true;
            checkTestAppNameRet = item.applicationInfo.name == BUNDLE_NAME_TEST;
            uint32_t num = static_cast<uint32_t>(item.abilityInfos.size());
            checkTestAbilitySizeRet = num == abilitySize;
        }
    }
    EXPECT_TRUE(isContainsDemoBundle);
    EXPECT_TRUE(isContainsTestBundle);
    EXPECT_TRUE(checkDemoAppNameRet);
    EXPECT_TRUE(checkTestAppNameRet);
    EXPECT_TRUE(checkDemoAbilitySizeRet);
    EXPECT_TRUE(checkTestAbilitySizeRet);
}

void BmsBundleKitServiceTest::CheckInstalledApplicationInfos(
    const uint32_t permsSize, const std::vector<ApplicationInfo> &appInfos) const
{
    bool isContainsDemoBundle = false;
    bool isContainsTestBundle = false;
    bool checkDemoAppNameRet = false;
    bool checkTestAppNameRet = false;
    bool checkDemoAbilitySizeRet = false;
    bool checkTestAbilitySizeRet = false;
    for (auto item : appInfos) {
        if (item.name == BUNDLE_NAME_DEMO) {
            isContainsDemoBundle = true;
            checkDemoAppNameRet = item.bundleName == BUNDLE_NAME_DEMO;
            uint32_t num = static_cast<uint32_t>(item.permissions.size());
            checkDemoAbilitySizeRet = num == permsSize;
        }
        if (item.name == BUNDLE_NAME_TEST) {
            isContainsTestBundle = true;
            checkTestAppNameRet = item.bundleName == BUNDLE_NAME_TEST;
            uint32_t num = static_cast<uint32_t>(item.permissions.size());
            checkTestAbilitySizeRet = num == permsSize;
        }
    }
    EXPECT_TRUE(isContainsDemoBundle);
    EXPECT_TRUE(isContainsTestBundle);
    EXPECT_TRUE(checkDemoAppNameRet);
    EXPECT_TRUE(checkTestAppNameRet);
    EXPECT_TRUE(checkDemoAbilitySizeRet);
    EXPECT_TRUE(checkTestAbilitySizeRet);
}

void BmsBundleKitServiceTest::CreateFileDir() const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }

    if (access(TEST_FILE_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(TEST_FILE_DIR);
        EXPECT_TRUE(result) << "fail to create file dir";
    }

    if (access(TEST_CACHE_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(TEST_CACHE_DIR);
        EXPECT_TRUE(result) << "fail to create cache dir";
    }
}

void BmsBundleKitServiceTest::CleanFileDir() const
{
    service_->Stop();
    InstalldClient::GetInstance()->ResetInstalldProxy();

    OHOS::ForceRemoveDirectory(BUNDLE_DATA_DIR);
}

void BmsBundleKitServiceTest::CheckFileExist() const
{
    int dataExist = access(TEST_FILE_DIR.c_str(), F_OK);
    EXPECT_EQ(dataExist, 0);
}

void BmsBundleKitServiceTest::CheckFileNonExist() const
{
    int dataExist = access(TEST_FILE_DIR.c_str(), F_OK);
    EXPECT_NE(dataExist, 0);
}

void BmsBundleKitServiceTest::CheckCacheExist() const
{
    int dataExist = access(TEST_CACHE_DIR.c_str(), F_OK);
    EXPECT_EQ(dataExist, 0);
}

void BmsBundleKitServiceTest::CheckCacheNonExist() const
{
    int dataExist = access(TEST_CACHE_DIR.c_str(), F_OK);
    EXPECT_NE(dataExist, 0);
}

void BmsBundleKitServiceTest::CheckFormInfoTest(const std::vector<FormInfo> &formInfos) const
{
    for (auto &formInfo : formInfos) {
        EXPECT_EQ(formInfo.name, FORM_NAME);
        EXPECT_EQ(formInfo.bundleName, BUNDLE_NAME_TEST);
        EXPECT_EQ(formInfo.moduleName, MODULE_NAME_TEST);
        EXPECT_EQ(formInfo.abilityName, ABILITY_NAME_TEST);
        EXPECT_EQ(formInfo.description, FORM_DESCRIPTION);
        EXPECT_EQ(formInfo.formConfigAbility, FORM_PATH);
        EXPECT_EQ(formInfo.defaultFlag, false);
        EXPECT_EQ(formInfo.type, FormType::JS);
        EXPECT_EQ(formInfo.colorMode, FormsColorMode::LIGHT_MODE);
        EXPECT_EQ(formInfo.descriptionId, FORMINFO_DESCRIPTIONID);
        EXPECT_EQ(formInfo.deepLink, FORM_PATH);
        EXPECT_EQ(formInfo.package, PACKAGE_NAME);
        EXPECT_EQ(formInfo.formVisibleNotify, true);
        unsigned i = 2;
        EXPECT_EQ(formInfo.supportDimensions.size(), i);
        EXPECT_EQ(formInfo.defaultDimension, 1);
        EXPECT_EQ(formInfo.updateDuration, 0);
        EXPECT_EQ(formInfo.scheduledUpdateTime, FORM_SCHEDULED_UPDATE_TIME);
        EXPECT_EQ(formInfo.jsComponentName, FORM_JS_COMPONENT_NAME);
        EXPECT_EQ(formInfo.updateEnabled, true);
        for (auto &info : formInfo.customizeDatas) {
            EXPECT_EQ(info.name, FORM_CUSTOMIZE_DATAS_NAME);
            EXPECT_EQ(info.value, FORM_CUSTOMIZE_DATAS_VALUE);
        }
        EXPECT_EQ(formInfo.src, FORM_SRC);
        EXPECT_EQ(formInfo.window.designWidth, FORM_JS_WINDOW_DESIGNWIDTH);
        EXPECT_EQ(formInfo.window.autoDesignWidth, true);
    }
}

void BmsBundleKitServiceTest::CheckFormInfoDemo(const std::vector<FormInfo> &formInfos) const
{
    for (const auto &formInfo : formInfos) {
        EXPECT_EQ(formInfo.name, FORM_NAME);
        EXPECT_EQ(formInfo.bundleName, BUNDLE_NAME_DEMO);
        EXPECT_EQ(formInfo.moduleName, MODULE_NAME_DEMO);
        EXPECT_EQ(formInfo.abilityName, ABILITY_NAME_TEST);
        EXPECT_EQ(formInfo.description, FORM_DESCRIPTION);
        EXPECT_EQ(formInfo.formConfigAbility, FORM_PATH);
        EXPECT_EQ(formInfo.defaultFlag, false);
        EXPECT_EQ(formInfo.type, FormType::JS);
        EXPECT_EQ(formInfo.colorMode, FormsColorMode::LIGHT_MODE);
        EXPECT_EQ(formInfo.descriptionId, FORMINFO_DESCRIPTIONID);
        EXPECT_EQ(formInfo.deepLink, FORM_PATH);
        EXPECT_EQ(formInfo.package, PACKAGE_NAME);
        EXPECT_EQ(formInfo.formVisibleNotify, true);
        unsigned i = 2;
        EXPECT_EQ(formInfo.supportDimensions.size(), i);
        EXPECT_EQ(formInfo.defaultDimension, 1);
        EXPECT_EQ(formInfo.updateDuration, 0);
        EXPECT_EQ(formInfo.scheduledUpdateTime, FORM_SCHEDULED_UPDATE_TIME);
        EXPECT_EQ(formInfo.jsComponentName, FORM_JS_COMPONENT_NAME);
        EXPECT_EQ(formInfo.updateEnabled, true);
        EXPECT_EQ(formInfo.src, FORM_SRC);
        EXPECT_EQ(formInfo.window.designWidth, FORM_JS_WINDOW_DESIGNWIDTH);
        for (auto &info : formInfo.customizeDatas) {
            EXPECT_EQ(info.name, FORM_CUSTOMIZE_DATAS_NAME);
            EXPECT_EQ(info.value, FORM_CUSTOMIZE_DATAS_VALUE);
        }
    }
}

void BmsBundleKitServiceTest::CheckShortcutInfoTest(std::vector<ShortcutInfo> &shortcutInfos) const
{
    for (const auto &shortcutInfo : shortcutInfos) {
        EXPECT_EQ(shortcutInfo.id, SHORTCUT_TEST_ID);
        EXPECT_EQ(shortcutInfo.bundleName, BUNDLE_NAME_TEST);
        EXPECT_EQ(shortcutInfo.hostAbility, SHORTCUT_HOST_ABILITY);
        EXPECT_EQ(shortcutInfo.icon, SHORTCUT_ICON);
        EXPECT_EQ(shortcutInfo.label, SHORTCUT_LABEL);
        EXPECT_EQ(shortcutInfo.disableMessage, SHORTCUT_DISABLE_MESSAGE);
        EXPECT_EQ(shortcutInfo.isStatic, true);
        EXPECT_EQ(shortcutInfo.isHomeShortcut, true);
        EXPECT_EQ(shortcutInfo.isEnables, true);
        for (auto &intent : shortcutInfo.intents) {
            EXPECT_EQ(intent.targetBundle, SHORTCUT_INTENTS_TARGET_BUNDLE);
            EXPECT_EQ(intent.targetClass, SHORTCUT_INTENTS_TARGET_CLASS);
        }
    }
}

void BmsBundleKitServiceTest::CheckCommonEventInfoTest(std::vector<CommonEventInfo> &commonEventInfos) const
{
    for (const auto &commonEventInfo : commonEventInfos) {

        EXPECT_EQ(commonEventInfo.name, COMMON_EVENT_NAME);
        EXPECT_EQ(commonEventInfo.bundleName, BUNDLE_NAME_TEST);
        EXPECT_EQ(commonEventInfo.permission, COMMON_EVENT_PERMISSION);
        for (auto item : commonEventInfo.data) {
            EXPECT_EQ(item, COMMON_EVENT_DATA);
        }
        for (auto item : commonEventInfo.type) {
            EXPECT_EQ(item, COMMON_EVENT_TYPE);
        }
        for (auto item : commonEventInfo.events) {
            EXPECT_EQ(item, COMMON_EVENT_EVENT);
        }
    }
}

void BmsBundleKitServiceTest::CheckShortcutInfoDemo(std::vector<ShortcutInfo> &shortcutInfos) const
{
    for (const auto &shortcutInfo : shortcutInfos) {
        EXPECT_EQ(shortcutInfo.id, SHORTCUT_DEMO_ID);
        EXPECT_EQ(shortcutInfo.bundleName, BUNDLE_NAME_DEMO);
        EXPECT_EQ(shortcutInfo.hostAbility, SHORTCUT_HOST_ABILITY);
        EXPECT_EQ(shortcutInfo.icon, SHORTCUT_ICON);
        EXPECT_EQ(shortcutInfo.label, SHORTCUT_LABEL);
        EXPECT_EQ(shortcutInfo.disableMessage, SHORTCUT_DISABLE_MESSAGE);
        EXPECT_EQ(shortcutInfo.isStatic, true);
        EXPECT_EQ(shortcutInfo.isHomeShortcut, true);
        EXPECT_EQ(shortcutInfo.isEnables, true);
        for (auto &intent : shortcutInfo.intents) {
            EXPECT_EQ(intent.targetBundle, SHORTCUT_INTENTS_TARGET_BUNDLE);
            EXPECT_EQ(intent.targetClass, SHORTCUT_INTENTS_TARGET_CLASS);
        }
    }
}

/**
 * @tc.number: CheckModuleRemovable_0100
 * @tc.name: test can check module removable is enable by no setting
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST);
    EXPECT_FALSE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0200
 * @tc.name: test can check module removable is enable by setting
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, true);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST);
    EXPECT_TRUE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0300
 * @tc.name: test can check module removable is disable by setting
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, false);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST);
    EXPECT_FALSE(testRet1);

    bool testRet2 = GetBundleDataMgr()->SetModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, true);
    EXPECT_TRUE(testRet2);
    bool testRet3 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST);
    EXPECT_TRUE(testRet3);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0400
 * @tc.name: test can check module removable is disable by no install
 * @tc.desc: 1.system run normally
 *           2.check the module removable failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0400, Function | SmallTest | Level1)
{
    bool testRet = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST);
    EXPECT_FALSE(testRet);
}

/**
 * @tc.number: CheckModuleRemovable_0500
 * @tc.name: test can check module removable is able by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetModuleRemovable("", "", true);
    EXPECT_FALSE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST);
    EXPECT_FALSE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0600
 * @tc.name: test can check module removable is disable by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the module removable failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, true);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsModuleRemovable("", "");
    EXPECT_FALSE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfo_0100
 * @tc.name: test can get the bundleName's bundle info
 * @tc.desc: 1.system run normal
 *           2.get bundle info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    BundleInfo testResult;
    bool testRet = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_TEST, BundleFlag::GET_BUNDLE_DEFAULT, testResult,
        DEFAULT_USERID);
    EXPECT_TRUE(testRet);
    CheckBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_SIZE_ZERO, testResult);
    BundleInfo demoResult;
    bool demoRet = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_DEMO, BundleFlag::GET_BUNDLE_WITH_ABILITIES,
        demoResult, DEFAULT_USERID);
    EXPECT_TRUE(demoRet);
    CheckBundleInfo(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_SIZE_ONE, demoResult);
    EXPECT_EQ(PERMISSION_SIZE_ZERO, demoResult.reqPermissions.size());

    BundleInfo bundleInfo;
    bool ret = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_DEMO, GET_BUNDLE_WITH_ABILITIES |
        GET_BUNDLE_WITH_REQUESTED_PERMISSION, bundleInfo, DEFAULT_USERID);
    EXPECT_TRUE(ret);
    CheckBundleInfo(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_SIZE_ONE, bundleInfo);
    EXPECT_EQ(PERMISSION_SIZE_TWO, bundleInfo.reqPermissions.size());

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfo_0200
 * @tc.name: test can not get the bundleName's bundle info which not exist in system
 * @tc.desc: 1.system run normal
 *           2.get bundle info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    BundleInfo result;
    bool ret = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_DEMO, BundleFlag::GET_BUNDLE_DEFAULT, result,
        DEFAULT_USERID);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.label);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfo_0300
 * @tc.name: test can not get bundle info with empty bundle name
 * @tc.desc: 1.system run normal
 *           2.get bundle info failed with empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0300, Function | SmallTest | Level0)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    BundleInfo result;
    bool ret = GetBundleDataMgr()->GetBundleInfo(EMPTY_STRING, BundleFlag::GET_BUNDLE_DEFAULT, result, DEFAULT_USERID);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfo_0400
 * @tc.name: test can not get the bundleName's bundle info with no bundle in system
 * @tc.desc: 1.system run normally and without any bundle
 *           2.get bundle info failed with no bundle in system
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0400, Function | SmallTest | Level0)
{
    BundleInfo bundleInfo;
    bool ret = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_TEST, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo,
        DEFAULT_USERID);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, bundleInfo.name);
    EXPECT_EQ(EMPTY_STRING, bundleInfo.label);
}

/**
 * @tc.number: GetBundleInfos_0100
 * @tc.name: test can get the installed bundles's bundle info with nomal flag
 * @tc.desc: 1.system run normally
 *           2.get all installed bundle info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool ret = GetBundleDataMgr()->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos, DEFAULT_USERID);
    EXPECT_TRUE(ret);
    CheckInstalledBundleInfos(ABILITY_SIZE_ZERO, bundleInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfos_0200
 * @tc.name: test can get the installed bundles's bundle info with abilities
 * @tc.desc: 1.system run normally
 *           2.get all installed bundle info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfos_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool ret = GetBundleDataMgr()->GetBundleInfos(BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfos, DEFAULT_USERID);
    EXPECT_TRUE(ret);
    CheckInstalledBundleInfos(ABILITY_SIZE_ONE, bundleInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfo_0100
 * @tc.name: test can get the appName's application info
 * @tc.desc: 1.system run normally
 *           2.get application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    ApplicationInfo testResult;
    bool testRet = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult);
    EXPECT_TRUE(testRet);
    CheckApplicationInfo(BUNDLE_NAME_TEST, PERMISSION_SIZE_ZERO, testResult);

    ApplicationInfo demoResult;
    bool demoRet = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_DEMO, ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, DEFAULT_USER_ID_TEST, demoResult);
    EXPECT_TRUE(demoRet);
    CheckApplicationInfo(BUNDLE_NAME_DEMO, PERMISSION_SIZE_TWO, demoResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfo_0200
 * @tc.name: test can not get the appName's application info which not exist in system
 * @tc.desc: 1.system run normally
 *           2.get application info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo result;
    bool ret = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_DEMO, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, result);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetApplicationInfo_0300
 * @tc.name: test can not get application info with empty appName
 * @tc.desc: 1.system run normally
 *           2.get application info failed with empty appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0300, Function | SmallTest | Level0)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo result;
    bool ret = GetBundleDataMgr()->GetApplicationInfo(
        EMPTY_STRING, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, result);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetApplicationInfo_0400
 * @tc.name: test can not get the appName's application info with no bundle in system
 * @tc.desc: 1.system run normally
 *           2.get application info failed with no bundle in system
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0400, Function | SmallTest | Level0)
{
    ApplicationInfo result;
    bool ret = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, result);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.name);
    EXPECT_EQ(EMPTY_STRING, result.label);
}

/**
 * @tc.number: GetApplicationInfo_0600
 * @tc.name: test can parceable application info(permissions and metaData)
 * @tc.desc: 1.system run normally
 *           2.get application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo testResult;
    bool testRet = GetBundleDataMgr()->GetApplicationInfo(BUNDLE_NAME_TEST,
        GET_APPLICATION_INFO_WITH_PERMISSION | GET_APPLICATION_INFO_WITH_METADATA, DEFAULT_USER_ID_TEST, testResult);
    EXPECT_TRUE(testRet);
    EXPECT_EQ(PERMISSION_SIZE_TWO, testResult.permissions.size());
    EXPECT_EQ(META_DATA_SIZE_ONE, testResult.metaData.size());

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfos_0100
 * @tc.name: test can get the installed bundles's application info with basic info flag
 * @tc.desc: 1.system run normally
 *           2.get all installed application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<ApplicationInfo> appInfos;
    bool ret = GetBundleDataMgr()->GetApplicationInfos(
        ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, appInfos);
    EXPECT_TRUE(ret);
    CheckInstalledApplicationInfos(PERMISSION_SIZE_ZERO, appInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfos_0200
 * @tc.name: test can get the installed bundles's application info with permissions
 * @tc.desc: 1.system run normally
 *           2.get all installed application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfos_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<ApplicationInfo> appInfos;
    bool ret = GetBundleDataMgr()->GetApplicationInfos(
        ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, DEFAULT_USER_ID_TEST, appInfos);
    EXPECT_TRUE(ret);
    CheckInstalledApplicationInfos(PERMISSION_SIZE_TWO, appInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetAbilityLabel_0100
 * @tc.name: test can get the ability's label by bundleName and abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability label successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testRet = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, EMPTY_STRING, ABILITY_NAME_TEST);
    EXPECT_EQ(LABEL, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0200
 * @tc.name: test can not get the ability's label if bundle doesn't install
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0200, Function | SmallTest | Level1)
{
    std::string testRet = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, EMPTY_STRING, ABILITY_NAME_TEST);
    EXPECT_EQ(EMPTY_STRING, testRet);
}

/**
 * @tc.number: GetAbilityLabel_0300
 * @tc.name: test can not get the ability's label if bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testRet = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_DEMO, EMPTY_STRING, ABILITY_NAME_TEST);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0400
 * @tc.name: test can not get the ability's label if ability doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testRet = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, EMPTY_STRING, ABILITY_NAME_DEMO);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0500
 * @tc.name: test can not get the ability's label if module doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 * @tc.require: SR000H48EM
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testRet = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, MODULE_NAME_TEST_1, ABILITY_NAME_TEST);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0600
 * @tc.name: test can not get the ability's label if module and ability exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 * @tc.require: AR000H4931
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testRet = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    EXPECT_EQ(LABEL, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0700
 * @tc.name: test can not get the ability's label if module exist and ability doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 * @tc.require: AR000H4931
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testRet = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_DEMO);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0100
 * @tc.name: test can get the ability info by want
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: SR000GM5QO
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);

    flags = GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0200
 * @tc.name: test can not get the ability info by want in which element name is wrong
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    Want want;
    want.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_TEST);
    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, result);
    EXPECT_EQ(false, testRet);

    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_DEMO);
    testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0300
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0300, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfo_0400
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.require: SR000H48EM
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0400, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfo_0500
 * @tc.name: test can get the ability info by want with bundleName, moduleName, abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: SR000GM5QO
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    AbilityInfo result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST, result.moduleName);

    flags = GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST, result.moduleName);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0600
 * @tc.name: test can get the ability info by want with bundleName, moduleName, abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: SR000GM5QO
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0600, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    AbilityInfo result;
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST, result.moduleName);

    want.SetModuleName(MODULE_NAME_TEST_1);
    testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST_1, result.moduleName);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0700
 * @tc.name: test can get the ability info by want with implicit query
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: SR000GM5QO
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0700, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", MODULE_NAME_TEST_1);
    AbilityInfo result;
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST_1, result.moduleName);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0100
 * @tc.name: test can get the ability info of list by want
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: AR000GM5QP
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    result.clear();

    flags = GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    result.clear();

    flags = GET_ABILITY_INFO_WITH_PERMISSION | GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0200
 * @tc.name: test can not get the ability info by want in which bundle name is wrong
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    Want want;
    want.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, 0, 0, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0300
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0300, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, 0, 0, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfos_0400
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 * @tc.require: AR000H4931
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0400, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, 0, 0, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfos_0500
 * @tc.name: test can get the ability info of list by want with bundleName, moduleName and abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: AR000H4931
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    std::vector<AbilityInfo> result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST, ret.moduleName);
    }
    result.clear();

    flags = GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST, ret.moduleName);
    }
    result.clear();

    flags = GET_ABILITY_INFO_WITH_PERMISSION | GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST, ret.moduleName);
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0600
 * @tc.name: test can get the ability info of list by want with bundleName, moduleName and abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: AR000H4931
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0600, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    std::vector<AbilityInfo> result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST, ret.moduleName);
    }
    result.clear();

    want.SetModuleName(MODULE_NAME_TEST_1);
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST_1, ret.moduleName);
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0700
 * @tc.name: test can get the ability info by want with implicit query
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: SR000GM5QO
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0700, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> result;
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    EXPECT_EQ(result.size(), MODULE_NAMES_SIZE_THREE);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0800
 * @tc.name: test can get the ability info by want with implicit query
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 * @tc.require: SR000GM5QO
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0800, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", MODULE_NAME_TEST_1);
    std::vector<AbilityInfo> result;
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    EXPECT_EQ(result.size(), MODULE_NAMES_SIZE_ONE);
    if (!result.empty()) {
        EXPECT_EQ(MODULE_NAME_TEST_1, result[0].moduleName);
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfosForClone_0100
 * @tc.name: test can get the ability info of list by want
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosForClone_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfosForClone(want, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST,
        GET_ABILITY_INFO_WITH_PERMISSION | GET_ABILITY_INFO_WITH_METADATA, result);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfosForClone_0200
 * @tc.name: test can not get the ability info by want in which element name is wrong
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosForClone_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    Want want;
    want.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfosForClone(want, result);
    EXPECT_EQ(false, testRet);

    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_DEMO);
    testRet = GetBundleDataMgr()->QueryAbilityInfosForClone(want, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfosForClone_0300
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosForClone_0300, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfosForClone(want, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: GetLaunchWantForBundle_0100
 * @tc.name: test can get the launch want of a bundle
 * @tc.desc: 1.system run normally
 *           2.get launch want successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    bool testRet = GetBundleDataMgr()->GetLaunchWantForBundle(BUNDLE_NAME_TEST, want);
    EXPECT_EQ(true, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetLaunchWantForBundle_0200
 * @tc.name: test can not get the launch want of a bundle which is not exist
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    bool testRet = GetBundleDataMgr()->GetLaunchWantForBundle(BUNDLE_NAME_DEMO, want);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetLaunchWantForBundle_0300
 * @tc.name: test can not get the launch want of a bundle which its mainability is not exist
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0300, Function | SmallTest | Level1)
{
    std::string bundleName = BUNDLE_NAME_DEMO;
    std::string moduleName = MODULE_NAME_DEMO;
    std::string abilityName = ABILITY_NAME_DEMO;
    InnerModuleInfo moduleInfo = MockModuleInfo(moduleName);
    std::string keyName = bundleName + "." + moduleName + "." + abilityName;
    AbilityInfo abilityInfo = MockAbilityInfo(bundleName, moduleName, abilityName);
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
    innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
    Skill skill;
    std::vector<Skill> skills;
    skills.emplace_back(skill);
    innerBundleInfo.InsertSkillInfo(keyName, skills);
    SaveToDatabase(bundleName, innerBundleInfo, true, false);

    Want want;
    bool testRet = GetBundleDataMgr()->GetLaunchWantForBundle(BUNDLE_NAME_DEMO, want);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetLaunchWantForBundle_0400
 * @tc.name: test can not get the launch want of empty name
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    Want want;
    bool testRet = GetBundleDataMgr()->GetLaunchWantForBundle(EMPTY_STRING, want);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetLaunchWantForBundle_0500
 * @tc.name: test can not get the launch want when no bundle installed
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0500, Function | SmallTest | Level1)
{
    Want want;
    bool testRet = GetBundleDataMgr()->GetLaunchWantForBundle(BUNDLE_NAME_TEST, want);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: GetBundleList_0100
 * @tc.name: test can get all installed bundle names
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names successfully with correct names
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleList_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundleList(testResult);
    EXPECT_TRUE(testRet);
    CheckBundleList(BUNDLE_NAME_TEST, testResult);
    CheckBundleList(BUNDLE_NAME_DEMO, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleNameForUid_0100
 * @tc.name: test can get the bundle names with bundle installed
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleNameForUid_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetBundleNameForUid(TEST_UID, testResult);
    EXPECT_TRUE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleNameForUid_0200
 * @tc.name: test can not get not installed bundle name
 * @tc.desc: 1.system run normally
 *           2.get installed bundle name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleNameForUid_0200, Function | SmallTest | Level1)
{
    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetBundleNameForUid(TEST_UID, testResult);
    EXPECT_FALSE(testRet);
}

/**
 * @tc.number: GetBundleNameForUid_0300
 * @tc.name: test can not get installed bundle name by incorrect uid
 * @tc.desc: 1.system run normally
 *           2.get installed bundle name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleNameForUid_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetBundleNameForUid(DEMO_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleNameForUid_0400
 * @tc.name: test can not get installed bundle name by invalid uid
 * @tc.desc: 1.system run normally
 *           2.get installed bundle name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleNameForUid_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetBundleNameForUid(INVALID_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);
    EXPECT_NE(BUNDLE_NAME_DEMO, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: CheckIsSystemAppByUid_0100
 * @tc.name: test can check the installed bundle whether system app or not by uid
 * @tc.desc: 1.system run normally
 *           2.check the installed bundle successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckIsSystemAppByUid_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    bool testRet = GetBundleDataMgr()->CheckIsSystemAppByUid(TEST_UID);
    EXPECT_FALSE(testRet);

    testRet = GetBundleDataMgr()->CheckIsSystemAppByUid(DEMO_UID);
    EXPECT_FALSE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: CheckIsSystemAppByUid_0200
 * @tc.name: test can check the installed bundle whether system app or not by uid
 * @tc.desc: 1.system run normally
 *           2.check the installed bundle successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckIsSystemAppByUid_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->CheckIsSystemAppByUid(INVALID_UID);
    EXPECT_FALSE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: DUMP_0100
 * @tc.name: Dump bundlelist, all bundle info, bundle info for bundleName
 * @tc.desc: 1.system run normally
 *           2.dump info with one mock installed bundles
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string allInfoResult;
    bool allInfoRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_ALL_BUNDLE_INFO, EMPTY_STRING, DEFAULT_USERID, allInfoResult);
    EXPECT_TRUE(allInfoRet);
    EXPECT_NE(std::string::npos, allInfoResult.find(BUNDLE_NAME_TEST));
    EXPECT_NE(std::string::npos, allInfoResult.find(MODULE_NAME_TEST));
    EXPECT_NE(std::string::npos, allInfoResult.find(ABILITY_NAME_TEST));

    std::string infoResult;
    bool infoRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, BUNDLE_NAME_TEST, DEFAULT_USERID, infoResult);
    EXPECT_TRUE(infoRet);
    EXPECT_NE(std::string::npos, infoResult.find(BUNDLE_NAME_TEST));
    EXPECT_NE(std::string::npos, infoResult.find(MODULE_NAME_TEST));
    EXPECT_NE(std::string::npos, infoResult.find(ABILITY_NAME_TEST));

    std::string bundleNames;
    bool listRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_LIST, EMPTY_STRING, DEFAULT_USERID, bundleNames);
    EXPECT_TRUE(listRet);
    EXPECT_NE(std::string::npos, bundleNames.find(BUNDLE_NAME_TEST));

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_TEST, ABILITY_NAME_DEMO);

    std::string names;
    bool namesRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_LIST, EMPTY_STRING, DEFAULT_USERID, names);
    EXPECT_TRUE(namesRet);
    EXPECT_NE(std::string::npos, names.find(BUNDLE_NAME_DEMO));

    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: DUMP_0200
 * @tc.name: Dump empty bundle info for empty bundle name
 * @tc.desc: 1.system run normally
 *           2.dump with empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0200, Function | SmallTest | Level0)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string emptyResult;
    bool emptyRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, EMPTY_STRING, DEFAULT_USERID, emptyResult);
    EXPECT_FALSE(emptyRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: DUMP_0300
 * @tc.name: Dump bundlelist, all bundle info, bundle info for bundleName
 * @tc.desc: 1.system run normally
 *           2.dump info with 2 installed bundles
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string bundleNames;
    bool listRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_LIST, EMPTY_STRING, DEFAULT_USERID, bundleNames);
    EXPECT_TRUE(listRet);
    EXPECT_NE(std::string::npos, bundleNames.find(BUNDLE_NAME_DEMO));
    EXPECT_NE(std::string::npos, bundleNames.find(BUNDLE_NAME_TEST));

    std::string allBundleInfos;
    bool allInfoRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_ALL_BUNDLE_INFO, EMPTY_STRING, DEFAULT_USERID, allBundleInfos);
    EXPECT_TRUE(allInfoRet);
    EXPECT_NE(std::string::npos, allBundleInfos.find(BUNDLE_NAME_TEST));
    EXPECT_NE(std::string::npos, allBundleInfos.find(BUNDLE_NAME_DEMO));
    EXPECT_NE(std::string::npos, allBundleInfos.find(MODULE_NAME_TEST));
    EXPECT_NE(std::string::npos, allBundleInfos.find(MODULE_NAME_DEMO));
    EXPECT_NE(std::string::npos, allBundleInfos.find(ABILITY_NAME_TEST));
    EXPECT_NE(std::string::npos, allBundleInfos.find(ABILITY_NAME_DEMO));

    std::string bundleInfo;
    bool infoRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, BUNDLE_NAME_TEST, DEFAULT_USERID, bundleInfo);
    EXPECT_TRUE(infoRet);
    EXPECT_NE(std::string::npos, allBundleInfos.find(BUNDLE_NAME_TEST));
    EXPECT_NE(std::string::npos, allBundleInfos.find(BUNDLE_NAME_DEMO));

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0100
 * @tc.name: test can get the ability info by uri
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        ABILITY_URI, DEFAULT_USERID, result);
    EXPECT_EQ(true, testRet);
    EXPECT_EQ(ABILITY_NAME_TEST, result.name);
    EXPECT_NE(ABILITY_NAME_DEMO, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0200
 * @tc.name: test can get the ability infos by uri
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        ABILITY_URI, DEFAULT_USERID, result);
    EXPECT_EQ(true, testRet);
    EXPECT_EQ(ABILITY_NAME_TEST, result.name);
    EXPECT_NE(ABILITY_NAME_DEMO, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0300
 * @tc.name: test can not get the ability info by uri which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */

HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0300, Function | SmallTest | Level1)
{
    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        ABILITY_URI, DEFAULT_USERID, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0400
 * @tc.name: test can not get the ability info by empty uri
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        "",  DEFAULT_USERID, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0500
 * @tc.name: test can not get the ability info by error uri
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        ERROR_URI,  DEFAULT_USERID, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryKeepAliveBundleInfos_0100
 * @tc.name: test can get the keep alive bundle infos
 * @tc.desc: 1.system run normally
 *           2.get all keep alive bundle infos successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryKeepAliveBundleInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool ret = GetBundleDataMgr()->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_EQ(true, ret);
    CheckInstalledBundleInfos(ABILITY_SIZE_ONE, bundleInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: QueryKeepAliveBundleInfos_0200
 * @tc.name: test can not get the keep alive bundle info which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get bundle info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryKeepAliveBundleInfos_0200, Function | SmallTest | Level1)
{
    std::vector<BundleInfo> bundleInfos;
    bool ret = GetBundleDataMgr()->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: GetBundleArchiveInfo_0200
 * @tc.name: test can not get the bundle archive info by empty hap file path
 * @tc.desc: 1.system run normally
 *           2.get the bundle archive info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleArchiveInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundleInfo testResult;
    bool listRet = hostImpl->GetBundleArchiveInfo("", BundleFlag::GET_BUNDLE_DEFAULT, testResult);
    EXPECT_FALSE(listRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleArchiveInfo_0300
 * @tc.name: test can not get the bundle archive info by no exist hap file path
 * @tc.desc: 1.system run normally
 *           2.get the bundle archive info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleArchiveInfo_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundleInfo testResult;
    bool listRet = hostImpl->GetBundleArchiveInfo(ERROR_HAP_FILE_PATH, BundleFlag::GET_BUNDLE_DEFAULT, testResult);
    EXPECT_FALSE(listRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0100
 * @tc.name: test can get the hap module info
 * @tc.desc: 1.system run normally
 *           2.get the hap module info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.package = PACKAGE_NAME;

    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo);
    EXPECT_EQ(true, ret);
    CheckModuleInfo(hapModuleInfo);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0200
 * @tc.name: test can not get the hap module info by no exist bundleName
 * @tc.desc: 1.system run normally
 *           2.get the hap module info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME_DEMO;
    abilityInfo.package = PACKAGE_NAME;

    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo);
    EXPECT_EQ(false, ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0300
 * @tc.name: test can not get the hap module info by no exist package
 * @tc.desc: 1.system run normally
 *           2.get the hap module info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.package = BUNDLE_NAME_DEMO;

    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo);
    EXPECT_EQ(false, ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0400
 * @tc.name: test get the hap module info by userId
 * @tc.desc: 1.system run normally
 *           2.get the hap module info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.package = PACKAGE_NAME;
    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo, DEFAULT_USER_ID_TEST);
    EXPECT_EQ(true, ret);

    ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo, NEW_USER_ID_TEST);
    EXPECT_EQ(false, ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0100
 * @tc.name: test can check bundle status is enable by no setting
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST);
    EXPECT_TRUE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0200
 * @tc.name: test can check bundle status is enable by setting
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetApplicationEnabled(BUNDLE_NAME_TEST, true);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST);
    EXPECT_TRUE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0300
 * @tc.name: test can check bundle status is disable by setting
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetApplicationEnabled(BUNDLE_NAME_TEST, false);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST);
    EXPECT_FALSE(testRet1);

    bool testRet2 = GetBundleDataMgr()->SetApplicationEnabled(BUNDLE_NAME_TEST, true);
    EXPECT_TRUE(testRet2);
    bool testRet3 = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST);
    EXPECT_TRUE(testRet3);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0400
 * @tc.name: test can check bundle status is disable by no install
 * @tc.desc: 1.system run normally
 *           2.check the bundle status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0400, Function | SmallTest | Level1)
{
    bool testRet = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST);
    EXPECT_FALSE(testRet);
}

/**
 * @tc.number: CheckApplicationEnabled_0500
 * @tc.name: test can check bundle status is able by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetApplicationEnabled("", true);
    EXPECT_FALSE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST);
    EXPECT_TRUE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0600
 * @tc.name: test can check bundle status is disable by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the bundle status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetApplicationEnabled(BUNDLE_NAME_TEST, true);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsApplicationEnabled("");
    EXPECT_FALSE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfosByMetaData_0100
 * @tc.name: test can get the bundle infos by metadata
 * @tc.desc: 1.system run normally
 *           2.get bundle infos successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfosByMetaData_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool testRet = GetBundleDataMgr()->GetBundleInfosByMetaData(META_DATA, bundleInfos);
    EXPECT_EQ(true, testRet);
    CheckInstalledBundleInfos(ABILITY_SIZE_ONE, bundleInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfosByMetaData_0200
 * @tc.name: test can not get the bundle infos by empty metadata
 * @tc.desc: 1.system run normally
 *           2.get bundle infos failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfosByMetaData_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool testRet = GetBundleDataMgr()->GetBundleInfosByMetaData("", bundleInfos);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfosByMetaData_0300
 * @tc.name: test can not get the bundle infos by no exist metadata
 * @tc.desc: 1.system run normally
 *           2.get bundle infos failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfosByMetaData_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool testRet = GetBundleDataMgr()->GetBundleInfosByMetaData(ERROR_META_DATA, bundleInfos);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: CleanBundleDataFiles_0100
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the bundle data files successfully
 * @tc.require: AR000GJUJ8
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0200
 * @tc.name: test can clean the bundle data files by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the bundle data files failed
 * @tc.require: AR000GJUJ8
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles("", DEFAULT_USERID);
    EXPECT_FALSE(testRet);
    CheckFileExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0300
 * @tc.name: test can clean the bundle data files by no exist bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the bundle data files failed
 * @tc.require: AR000GJUJ8
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_DEMO, DEFAULT_USERID);
    EXPECT_FALSE(testRet);
    CheckFileExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0400
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is false, isSystemApp is false
 *           3.clean the cache files succeed
 * @tc.require: SR000H00TH
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, false);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0500
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is true, isSystemApp is false
 *           3.clean the cache files succeed
 * @tc.require: SR000H00TH
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, true, false);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0600
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is false, isSystemApp is true
 *           3.clean the cache files failed
 * @tc.require: AR000H035G
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, true);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_FALSE(testRet);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0700
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is true, isSystemApp is true
 *           3.clean the cache files failed
 * @tc.require: AR000H035G
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, true, true);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0200
 * @tc.name: test can clean the cache files by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the cache files failed
 * @tc.require: AR000GJUJ8
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->CleanBundleCacheFiles("", cleanCache);
    EXPECT_FALSE(result);
    CheckCacheExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0300
 * @tc.name: test can clean the cache files by no exist bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the cache files failed
 * @tc.require: AR000GJUJ8
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->CleanBundleCacheFiles("", cleanCache);
    EXPECT_FALSE(result);
    CheckCacheExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0400
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is true, isSystemApp is false
 *           3.clean the cache files succeed
 * @tc.require: SR000H00TH
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, true, false);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_TRUE(result);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0500
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is false, isSystemApp is false
 *           3.clean the cache files succeed
 * @tc.require: SR000H00TH
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, false);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_TRUE(result);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0600
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is true, isSystemApp is true
 *           3.clean the cache files succeed
 * @tc.require: AR000H035G
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, true, true);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_TRUE(result);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0700
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is false, isSystemApp is true
 *           3.clean the cache files failed
 * @tc.require: AR000H035G
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, true);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_FALSE(result);
    CheckCacheExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: RegisterBundleStatus_0100
 * @tc.name: test can register the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback successfully
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleStatus_0100, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);

    bool resultNotify = GetBundleDataMgr()->NotifyBundleStatus(
        HAP_FILE_PATH, HAP_FILE_PATH, ABILITY_NAME_DEMO, ERR_OK, NotifyType::INSTALL, Constants::INVALID_UID);
    EXPECT_TRUE(resultNotify);
    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_OK);
}

/**
 * @tc.number: RegisterBundleStatus_0200
 * @tc.name: test can register the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed by empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleStatus_0200, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);

    bool resultNotify = GetBundleDataMgr()->NotifyBundleStatus(
        "", HAP_FILE_PATH, ABILITY_NAME_DEMO, ERR_OK, NotifyType::INSTALL, Constants::INVALID_UID);
    EXPECT_TRUE(resultNotify);

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_TIMED_OUT);
}

/**
 * @tc.number: RegisterBundleStatus_0300
 * @tc.name: test can register the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed by no exist bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleStatus_0300, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);

    bool resultNotify = GetBundleDataMgr()->NotifyBundleStatus(
        ERROR_HAP_FILE_PATH, HAP_FILE_PATH, ABILITY_NAME_DEMO, ERR_OK, NotifyType::INSTALL, Constants::INVALID_UID);
    EXPECT_TRUE(resultNotify);

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_TIMED_OUT);
}

/**
 * @tc.number: ClearBundleStatus_0100
 * @tc.name: test can clear the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed by cleared bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, ClearBundleStatus_0100, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback1 = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback1->SetBundleName(HAP_FILE_PATH1);
    bool result1 = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback1);
    EXPECT_TRUE(result1);

    bool result2 = GetBundleDataMgr()->ClearBundleStatusCallback(bundleStatusCallback1);
    EXPECT_TRUE(result2);

    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);

    bool resultNotify = GetBundleDataMgr()->NotifyBundleStatus(
        HAP_FILE_PATH, HAP_FILE_PATH, ABILITY_NAME_DEMO, ERR_OK, NotifyType::INSTALL, Constants::INVALID_UID);
    EXPECT_TRUE(resultNotify);

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_OK);

    bool resultNotify1 = GetBundleDataMgr()->NotifyBundleStatus(
        HAP_FILE_PATH1, HAP_FILE_PATH, ABILITY_NAME_DEMO, ERR_OK, NotifyType::INSTALL, Constants::INVALID_UID);
    EXPECT_TRUE(resultNotify1);

    int32_t callbackResult1 = bundleStatusCallback1->GetResultCode();
    EXPECT_EQ(callbackResult1, ERR_TIMED_OUT);
}

/**
 * @tc.number: UnregisterBundleStatus_0100
 * @tc.name: test can unregister the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed by unregister bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, UnregisterBundleStatus_0100, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);

    sptr<MockBundleStatus> bundleStatusCallback1 = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback1->SetBundleName(HAP_FILE_PATH1);
    bool result1 = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback1);
    EXPECT_TRUE(result1);

    bool result2 = GetBundleDataMgr()->UnregisterBundleStatusCallback();
    EXPECT_TRUE(result2);

    bool resultNotify = GetBundleDataMgr()->NotifyBundleStatus(
        HAP_FILE_PATH, HAP_FILE_PATH, ABILITY_NAME_DEMO, ERR_OK, NotifyType::INSTALL, Constants::INVALID_UID);
    EXPECT_TRUE(resultNotify);

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_TIMED_OUT);

    bool resultNotify1 = GetBundleDataMgr()->NotifyBundleStatus(
        HAP_FILE_PATH1, HAP_FILE_PATH, ABILITY_NAME_DEMO, ERR_OK, NotifyType::INSTALL, Constants::INVALID_UID);
    EXPECT_TRUE(resultNotify1);

    int32_t callbackResult1 = bundleStatusCallback1->GetResultCode();
    EXPECT_EQ(callbackResult1, ERR_TIMED_OUT);
}

/**
 * @tc.number: GetBundlesForUid_0100
 * @tc.name: test can get the bundle names with bundle installed
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundlesForUid(TEST_UID, testResult);
    EXPECT_TRUE(testRet);
    EXPECT_EQ(BUNDLE_NAMES_SIZE_ONE, testResult.size());
    if (testResult.size() > 0) {
        EXPECT_EQ(BUNDLE_NAME_TEST, testResult[0]);
    }

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundlesForUid_0200
 * @tc.name: test can not get not installed bundle names
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0200, Function | SmallTest | Level1)
{
    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundlesForUid(TEST_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_EQ(BUNDLE_NAMES_SIZE_ZERO, testResult.size());
}

/**
 * @tc.number: GetBundlesForUid_0300
 * @tc.name: test can not get installed bundle names by incorrect uid
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundlesForUid(DEMO_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_EQ(BUNDLE_NAMES_SIZE_ZERO, testResult.size());

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundlesForUid_0400
 * @tc.name: test can not get installed bundle names by invalid uid
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundlesForUid(INVALID_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_EQ(BUNDLE_NAMES_SIZE_ZERO, testResult.size());

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetNameForUid_0100
 * @tc.name: test can get the uid name with bundle installed
 * @tc.desc: 1.system run normally
 *           2.get installed uid name successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetNameForUid(TEST_UID, testResult);
    EXPECT_TRUE(testRet);
    EXPECT_EQ(BUNDLE_NAME_TEST, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetNameForUid_0200
 * @tc.name: test can not get not installed uid name
 * @tc.desc: 1.system run normally
 *           2.get installed uid name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0200, Function | SmallTest | Level1)
{
    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetNameForUid(TEST_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);
}

/**
 * @tc.number: GetNameForUid_0300
 * @tc.name: test can not get installed uid name by incorrect uid
 * @tc.desc: 1.system run normally
 *           2.get installed uid name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetNameForUid(DEMO_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetNameForUid_0400
 * @tc.name: test can not get installed uid name by invalid uid
 * @tc.desc: 1.system run normally
 *           2.get installed uid name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetNameForUid(INVALID_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);
    EXPECT_NE(BUNDLE_NAME_DEMO, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: CheckAbilityEnabled_0100
 * @tc.name: test can check ability status is enable by no setting
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_TRUE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0200
 * @tc.name: test can check ability status is enable by setting
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_TRUE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0300
 * @tc.name: test can check ability status is disable by setting and can be enabled again
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, false);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_FALSE(testRet1);
    bool testRet2 = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true);
    EXPECT_TRUE(testRet2);
    bool testRet3 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_TRUE(testRet3);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0400
 * @tc.name: test can check ability status is disable by no install
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0400, Function | SmallTest | Level1)
{
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_FALSE(testRet);
}

/**
 * @tc.number: CheckAbilityEnabled_0500
 * @tc.name: test can check ability status is able by empty AbilityInfo
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfoEmpty;
    bool testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfoEmpty, false);
    EXPECT_FALSE(testRet);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_TRUE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0600
 * @tc.name: test can check ability status is disable by empty AbilityInfo
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true);
    EXPECT_TRUE(testRet);
    AbilityInfo abilityInfoEmpty;
    bool testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfoEmpty);
    EXPECT_FALSE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0700
 * @tc.name: test can check ability status is enable by empty moduleName
 * @tc.require: AR000H4931
 * @tc.desc: 1.system run normally
 *           2.check the ability status successful
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    bool testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_TRUE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0800
 * @tc.name: test can check ability status is disable by moduleName
 * @tc.require: AR000H4931
 * @tc.desc: 1.system run normally
 *           2.check the ability status successful
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.moduleName = MODULE_NAME_TEST;
    bool testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true);
    EXPECT_TRUE(testRet);
    bool testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_TRUE(testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0900
 * @tc.name: test can check ability status is disable by moduleName
 * @tc.require: AR000H4931
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0900, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.moduleName = MODULE_NAME_TEST_1;
    bool testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true);
    EXPECT_FALSE(testRet);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1000
 * @tc.name: test can check ability status is enable by no setting
 * @tc.require: AR000H4931
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1000, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    bool testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_TRUE(testRet);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1100
 * @tc.name: test can check ability status is enable by no setting
 * @tc.require: AR000H4931
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.moduleName = MODULE_NAME_TEST;
    bool testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_TRUE(testRet);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1200
 * @tc.name: test can check ability status is enable by wrong moduleName
 * @tc.require: AR000H4931
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.moduleName = MODULE_NAME_TEST_1;
    bool testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo);
    EXPECT_FALSE(testRet);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0100
 * @tc.name: test can  get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get formInfo by moduleName successful
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto result = GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, MODULE_NAME_TEST, formInfos);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfos.empty());
    CheckFormInfoTest(formInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0200
 * @tc.name: test can get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get forms info in different bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo1;
    auto result = GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, MODULE_NAME_TEST, formInfo1);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfo1.empty());
    CheckFormInfoTest(formInfo1);

    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo2;
    auto result1 = GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, formInfo2);
    EXPECT_EQ(result1, true);
    EXPECT_FALSE(formInfo2.empty());
    CheckFormInfoDemo(formInfo2);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetFormInfoByModule_0400
 * @tc.name: test can not get the formInfo
 * @tc.desc:1.system run normally
 *          2.get form info with error moduleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, MODULE_NAME_DEMO, formInfos);
    EXPECT_TRUE(formInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0400
 * @tc.name: test can not get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get form info with empty moduleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_DEMO);
    std::vector<FormInfo> formInfos;
    GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, EMPTY_STRING, formInfos);
    EXPECT_TRUE(formInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0500
 * @tc.name: test can not get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get form info with non bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0500, Function | SmallTest | Level1)
{
    std::vector<FormInfo> formInfos;
    GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, EMPTY_STRING, formInfos);
    EXPECT_TRUE(formInfos.empty());
}

/**
 * @tc.number: GetFormInfoByApp_0100
 * @tc.name: test can get the formInfo by bundlename
 * @tc.desc: 1.system run normally
 *           2.get form info by bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto result = GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_TEST, formInfos);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfos.empty());
    CheckFormInfoTest(formInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByApp_0200
 * @tc.name: test can get the formInfo in different app by bundlename
 * @tc.desc: 1.system run normally
 *           2.get form info by different bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo1;
    auto result = GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_TEST, formInfo1);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfo1.empty());
    CheckFormInfoTest(formInfo1);

    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo2;
    auto result1 = GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_DEMO, formInfo2);
    EXPECT_EQ(result1, true);
    EXPECT_FALSE(formInfo2.empty());
    CheckFormInfoDemo(formInfo2);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetFormInfoByApp_0300
 * @tc.name: test can't get the formInfo in app by error app name
 * @tc.desc: 1.system run normally
 *           2.get form info by error bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo1;
    GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_DEMO, formInfo1);
    EXPECT_TRUE(formInfo1.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByApp_0400
 * @tc.name: test can't get the formInfo in app by empty app name
 * @tc.desc: 1.system run normally
 *           2.get form info by empty bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo1;
    GetBundleDataMgr()->GetFormsInfoByApp(EMPTY_STRING, formInfo1);
    EXPECT_TRUE(formInfo1.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByApp_0500
 * @tc.name: test can't get the formInfo have no bundle
 * @tc.desc: 1.system run normally
 *           2.get form info with non bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0500, Function | SmallTest | Level1)
{
    std::vector<FormInfo> formInfo1;
    GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_DEMO, formInfo1);
    EXPECT_TRUE(formInfo1.empty());
}

/**
 * @tc.number: GetAllFormInfo_0100
 * @tc.name: test can get all the formInfo
 * @tc.desc: 1.system run normally
 *           2.get forms by all the bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllFormInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto result = GetBundleDataMgr()->GetAllFormsInfo(formInfos);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfos.empty());
    for (const auto &info : formInfos) {
        if (info.bundleName == BUNDLE_NAME_TEST) {
            std::vector<FormInfo> formInfo;
            formInfo.emplace_back(info);
            CheckFormInfoTest(formInfo);
            break;
        }
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllFormInfo_0200
 * @tc.name: test can get all the formInfo
 * @tc.desc: 1.system run normally
 *           2.get forms by all the bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllFormInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto result = GetBundleDataMgr()->GetAllFormsInfo(formInfos);
    EXPECT_FALSE(formInfos.empty());
    EXPECT_EQ(result, true);
    for (const auto &info : formInfos) {
        if (info.bundleName == MODULE_NAME_DEMO) {
            std::vector<FormInfo> formInfo1;
            formInfo1.emplace_back(info);
            CheckFormInfoDemo(formInfo1);
        } else if (info.bundleName == MODULE_NAME_TEST){
            std::vector<FormInfo> formInfo2;
            formInfo2.emplace_back(info);
            CheckFormInfoTest(formInfo2);
        }
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetShortcutInfos_0100
 * @tc.name: test can get shortcutInfo by bundleName
 * @tc.desc: 1.can get shortcutInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    auto result = GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_TEST,  DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(result);
    CheckShortcutInfoTest(shortcutInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfos_0200
 * @tc.name: test have two bundle can get shortcutInfo by bundleName
 * @tc.desc: 1.can get shortcutInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfo1;
    auto result1 = GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_TEST,  DEFAULT_USERID, shortcutInfo1);
    EXPECT_TRUE(result1);
    CheckShortcutInfoTest(shortcutInfo1);
    std::vector<ShortcutInfo> shortcutInfo2;
    auto result2 = GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_DEMO,  DEFAULT_USERID, shortcutInfo2);
    EXPECT_TRUE(result2);
    CheckShortcutInfoDemo(shortcutInfo2);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetShortcutInfos_0300
 * @tc.name: test can't get the shortcutInfo in app by error app name
 * @tc.desc: 1.have not get shortcutinfo by appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_DEMO,  DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(shortcutInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfos_0400
 * @tc.name: test can't get the shortcutInfo in app by null app name
 * @tc.desc: 1.have not get shortcutinfo by null app name
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    GetBundleDataMgr()->GetShortcutInfos(
        EMPTY_STRING,  DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(shortcutInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfos_0500
 * @tc.name: test can't get the shortcutInfo have no bundle
 * @tc.desc: 1.have not get shortcutInfo by appName
 *           2.can't get shortcutInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0500, Function | SmallTest | Level1)
{
    std::vector<ShortcutInfo> shortcutInfos;
    GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_TEST, DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(shortcutInfos.empty());
}



/**
 * @tc.number: Ability_0100
 * @tc.name: test can get the compatibleAbilityInfo by one bundle
 * @tc.desc: 1.can get compatibleAbilityInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Ability_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, abilityInfo);
    EXPECT_TRUE(testRet);
    CompatibleAbilityInfo compatibleAbilityInfo;
    abilityInfo.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
    CheckCompatibleAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, compatibleAbilityInfo);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Ability_0200
 * @tc.name: test can get the compatibleAbilityInfo by two bundle
 * @tc.desc: 1.can get compatibleAbilityInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Ability_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    Want want1;
    want1.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo1;
    bool testRet1 = GetBundleDataMgr()->QueryAbilityInfo(want1, 0, 0, abilityInfo1);
    EXPECT_TRUE(testRet1);
    CompatibleAbilityInfo compatibleAbilityInfo1;
    abilityInfo1.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo1);
    CheckCompatibleAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, compatibleAbilityInfo1);
    Want want2;
    want2.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_DEMO);
    AbilityInfo abilityInfo2;
    bool testRet2 = GetBundleDataMgr()->QueryAbilityInfo(want2, 0, 0, abilityInfo2);
    EXPECT_TRUE(testRet2);
    CompatibleAbilityInfo compatibleAbilityInfo2;
    abilityInfo2.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo2);
    CheckCompatibleAbilityInfo(BUNDLE_NAME_DEMO, ABILITY_NAME_DEMO, compatibleAbilityInfo2);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: Application_0100
 * @tc.name: test can get the compatibleApplicationInfo by one bundle
 * @tc.desc: 1.can get compatibleApplicationInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Application_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo testResult;
    bool testRet = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult);
    EXPECT_TRUE(testRet);
    CompatibleApplicationInfo appInfo;
    testResult.ConvertToCompatibleApplicationInfo(appInfo);
    CheckCompatibleApplicationInfo(BUNDLE_NAME_TEST, PERMISSION_SIZE_ZERO, appInfo);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Application_0100
 * @tc.name: test can get the compatibleApplicationInfo by two bundle
 * @tc.desc: 1.can get compatibleApplicationInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Application_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    ApplicationInfo testResult1;
    bool testRet1 = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult1);
    EXPECT_TRUE(testRet1);
    CompatibleApplicationInfo appInfo1;
    testResult1.ConvertToCompatibleApplicationInfo(appInfo1);
    CheckCompatibleApplicationInfo(BUNDLE_NAME_TEST, PERMISSION_SIZE_ZERO, appInfo1);

    ApplicationInfo testResult2;
    bool testRet2 = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_DEMO, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult2);
    EXPECT_TRUE(testRet2);
    CompatibleApplicationInfo appInfo2;
    testResult2.ConvertToCompatibleApplicationInfo(appInfo2);
    CheckCompatibleApplicationInfo(BUNDLE_NAME_DEMO, PERMISSION_SIZE_ZERO, appInfo2);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: QueryAllAbilityInfos
 * @tc.name: test can get the All AbilityInfo
 * @tc.desc: 1.can get All AbilityInfo
 * @tc.require: AR000GJUJ8
 */
HWTEST_F(BmsBundleKitServiceTest, AllAbility_001, Function | SmallTest | Level1)
{
    std::vector<AbilityInfo> abilityInfos;
    Want want1;
    want1.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_DEMO);
    bool testRet1 = GetBundleDataMgr()->QueryLauncherAbilityInfos(want1, DEFAULT_USER_ID_TEST, abilityInfos);
    EXPECT_FALSE(testRet1);
}

/**
 * @tc.number: GetAllCommonEventInfo_0100
 * @tc.name: test can get CommonEventInfo by event key
 * @tc.desc: 1.can get CommonEventInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    auto result = GetBundleDataMgr()->GetAllCommonEventInfo(COMMON_EVENT_EVENT, commonEventInfos);
    EXPECT_TRUE(result);
    CheckCommonEventInfoTest(commonEventInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllCommonEventInfo_0200
 * @tc.name: test can't get the commonEventInfo have no bundle
 * @tc.desc: 1.have not get commonEventInfo by event key
 *           2.can't get commonEventInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0200, Function | SmallTest | Level1)
{
    std::vector<CommonEventInfo> commonEventInfos;
    GetBundleDataMgr()->GetAllCommonEventInfo(COMMON_EVENT_EVENT, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
}

/**
 * @tc.number: GetAllCommonEventInfo_0300
 * @tc.name: test can't get the commonEventInfo in app by empty event key
 * @tc.desc: 1.have not get commonEventInfo by appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    GetBundleDataMgr()->GetAllCommonEventInfo(EMPTY_STRING, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllCommonEventInfo_0400
 * @tc.name: test can't get the commonEventInfo in app by error event key
 * @tc.desc: 1.have not get commonEventInfo by appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    GetBundleDataMgr()->GetAllCommonEventInfo(COMMON_EVENT_EVENT_ERROR_KEY, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllCommonEventInfo_0500
 * @tc.name: test can't get the commonEventInfo in app by not exists event key
 * @tc.desc: 1.have not get commonEventInfo by appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    GetBundleDataMgr()->GetAllCommonEventInfo(COMMON_EVENT_EVENT_NOT_EXISTS_KEY, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: BundleClone_0200.
 * @tc.name: test the error bundle name can't be cloned.
 * @tc.desc: 1.the bundle name is error.
 *           2.the bundle can't be cloned success.
 */
HWTEST_F(BmsBundleKitServiceTest, BundleClone_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto result = GetBundleCloneMgr()->BundleClone(BUNDLE_NAME_DEMO);
    EXPECT_FALSE(result);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: RemoveClonedBundle_0200.
 * @tc.name: test the cloned bundle can't be removed when the bundle name is error.
 * @tc.desc: 1. the bundle is already cloned.
 *           2. the cloned bundle can't be removed successfully.
 */
HWTEST_F(BmsBundleKitServiceTest, RemoveClonedBundle_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string cloneName;
    GetBundleDataMgr()->GetClonedBundleName(BUNDLE_NAME_TEST, cloneName);
    auto result = GetBundleCloneMgr()->RemoveClonedBundle(BUNDLE_NAME_TEST, cloneName);
    EXPECT_FALSE(result);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want empty; skill empty
 * @tc.desc: expect false
 * @tc.require: SR000GGT3C
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_001, Function | SmallTest | Level1)
{
    struct Skill skill;
    Want want;
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want not empty; skill empty
 * @tc.desc: expect false
 * @tc.require: SR000GGT3C
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_002, Function | SmallTest | Level1)
{
    struct Skill skill;
    Want want;
    want.SetAction(ACTION_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want empty; skill not empty
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_003, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want not empty; skill not empty; skill contains want
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_004, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    want.SetAction(ACTION_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want not empty; skill not empty; skill not contains want
 * @tc.desc: expect false
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_005, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    want.SetAction(ACTION_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want empty; skill empty;
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_001, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want not empty; skill empty;
 * @tc.desc: expect false
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_002, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    want.AddEntity(ENTITY_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want empty; skill not empty;
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_003, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    skill.entities.emplace_back(ENTITY_001);
    Want want;
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want not empty; skill not empty; skill contains want
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_004, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    skill.entities.emplace_back(ENTITY_001);
    Want want;
    want.AddEntity(ENTITY_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want not empty; skill not empty; skill contains want
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_005, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    skill.entities.emplace_back(ENTITY_001);
    skill.entities.emplace_back(ENTITY_002);
    Want want;
    want.AddEntity(ENTITY_001);
    want.AddEntity(ENTITY_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want not empty; skill not empty; skill not contains want
 * @tc.desc: expect false
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_006, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    skill.entities.emplace_back(ENTITY_001);
    Want want;
    want.AddEntity(ENTITY_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want empty; skill empty
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_001, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    want.SetUri("");
    want.SetType("");
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri empty, type not empty; skill uri empty, type not empty
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_002, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.type = TYPE_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetType(TYPE_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri empty, type not empty; skill uri empty, type not empty; type not equal
 * @tc.desc: expect false
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_003, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.type = TYPE_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetType(TYPE_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_004, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(SCHEME_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty; uri not equal
 * @tc.desc: expect false
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_005, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(SCHEME_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri empty, type not empty; skill uri empty, type not empty; regex
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_006, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.type = TYPE_IMG_REGEX;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetType(TYPE_IMG_JPEG);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty
 *           uri path match.
 * @tc.desc: expect true
 * @tc.require: AR000GHO34
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_007, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skillUri.host = HOST_001;
    skillUri.port = PORT_001;
    skillUri.path = PATH_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(URI_PATH_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty
 *           uri pathStartWith match.
 * @tc.desc: expect true
 * @tc.require: AR000GHO3B
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_008, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skillUri.host = HOST_001;
    skillUri.port = PORT_001;
    skillUri.pathStartWith = PATH_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(URI_PATH_DUPLICATE_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty
 *           uri pathRegex match.
 * @tc.desc: expect true
 * @tc.require: AR000GHO3B
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_009, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skillUri.host = HOST_001;
    skillUri.port = PORT_001;
    skillUri.pathRegex = PATH_REGEX_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(URI_PATH_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: GetAlldependentModuleNames
 * @tc.name: no dependencies
 * @tc.desc: expect true
 * @tc.require: SR000H00TB
 */
HWTEST_F(BmsBundleKitServiceTest, GetAlldependentModuleNames_001, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    std::vector<std::string> dependencies;
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    auto res = innerBundleInfo.GetAllDependentModuleNames(MODULE_NAME_TEST);
    EXPECT_TRUE(res.empty());
}

/**
 * @tc.number: GetAlldependentModuleNames
 * @tc.name: one dependent module
 * @tc.desc: expect true
 * @tc.require: SR000H0DUR
 */
HWTEST_F(BmsBundleKitServiceTest, GetAlldependentModuleNames_002, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    std::vector<std::string> dependencies;
    dependencies.push_back(MODULE_NAME_TEST_1);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_1, ABILITY_NAME_TEST, dependencies, innerBundleInfo);

    auto res = innerBundleInfo.GetAllDependentModuleNames(MODULE_NAME_TEST);
    EXPECT_EQ(res.size(), MODULE_NAMES_SIZE_ONE);
    if (!res.empty()) {
        EXPECT_EQ(res[0], MODULE_NAME_TEST_1);
    }
}

/**
 * @tc.number: GetAlldependentModuleNames
 * @tc.name: more than one dependent module
 * @tc.desc: expect true
 * @tc.require: AR000H0362
 */
HWTEST_F(BmsBundleKitServiceTest, GetAlldependentModuleNames_003, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    std::vector<std::string> dependencies;
    dependencies.push_back(MODULE_NAME_TEST_1);
    dependencies.push_back(MODULE_NAME_TEST_2);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_1, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_2, ABILITY_NAME_TEST, dependencies, innerBundleInfo);

    auto res = innerBundleInfo.GetAllDependentModuleNames(MODULE_NAME_TEST);
    EXPECT_EQ(res.size(), MODULE_NAMES_SIZE_TWO);
    if (res.size() == MODULE_NAMES_SIZE_TWO) {
        EXPECT_EQ(res[0], MODULE_NAME_TEST_1);
        EXPECT_EQ(res[MODULE_NAMES_SIZE_ONE], MODULE_NAME_TEST_2);
    }
}

/**
 * @tc.number: GetAlldependentModuleNames
 * @tc.name: Multiple dependent modules
 * @tc.desc: expect true
 * @tc.require: AR000HDTN
 */
HWTEST_F(BmsBundleKitServiceTest, GetAlldependentModuleNames_004, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    std::vector<std::string> dependencies;
    dependencies.push_back(MODULE_NAME_TEST_1);
    dependencies.push_back(MODULE_NAME_TEST_2);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_1, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    dependencies.push_back(MODULE_NAME_TEST_3);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_2, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_3, ABILITY_NAME_TEST, dependencies, innerBundleInfo);

    auto res = innerBundleInfo.GetAllDependentModuleNames(MODULE_NAME_TEST);
    EXPECT_EQ(res.size(), MODULE_NAMES_SIZE_THREE);
    if (res.size() == MODULE_NAMES_SIZE_THREE) {
        EXPECT_EQ(res[0], MODULE_NAME_TEST_1);
        EXPECT_EQ(res[MODULE_NAMES_SIZE_ONE], MODULE_NAME_TEST_2);
        EXPECT_EQ(res[MODULE_NAMES_SIZE_TWO], MODULE_NAME_TEST_3);
    }
}
}