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

#include <benchmark/benchmark.h>

#include "bundle_constants.h"
#include "bundle_mgr_proxy.h"
#include "bundle_status_callback_host.h"
#include "clean_cache_callback_host.h"
#include "common_tool.h"
#include "iservice_registry.h"
#include "status_receiver_host.h"
#include "system_ability_definition.h"

using OHOS::AAFwk::Want;
using namespace std::chrono_literals;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
const std::string SYSTEM_SETTINGS_BUNDLE_NAME = "com.ohos.settings";
const std::string bundleName = "com.ohos.contactsdataability";
const std::string abilityName = "com.ohos.contactsdataability.MainAbility";
const std::string hapFilePath = "/data/test/benchmark/test.hap";
const std::string MODULE_NAME_TEST = "entry";
const std::string COMMON_EVENT_EVENT = "usual.event.PACKAGE_ADDED";
const int32_t DEFAULT_USERID = 100;
class BundleStatusCallbackImpl : public BundleStatusCallbackHost {
public:
    BundleStatusCallbackImpl() = default;
    virtual ~BundleStatusCallbackImpl() override {};
    virtual void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode, const std::string &resultMsg,
        const std::string &bundleName) override;
    virtual void OnBundleAdded(const std::string &bundleName, const int userId) override {};
    virtual void OnBundleUpdated(const std::string &bundleName, const int userId) override {};
    virtual void OnBundleRemoved(const std::string &bundleName, const int userId) override {};

private:
    DISALLOW_COPY_AND_MOVE(BundleStatusCallbackImpl);
};

void BundleStatusCallbackImpl::OnBundleStateChanged(
    const uint8_t installType, const int32_t resultCode, const std::string &resultMsg, const std::string &bundleName)
{}

class CleanCacheCallBackImpl : public CleanCacheCallbackHost {
public:
    CleanCacheCallBackImpl() = default;
    virtual ~CleanCacheCallBackImpl() override {};
    virtual void OnCleanCacheFinished(bool succeeded) override {};

private:
    DISALLOW_COPY_AND_MOVE(CleanCacheCallBackImpl);
};

class StatusReceiverImpl : public StatusReceiverHost {
public:
    StatusReceiverImpl() = default;
    virtual ~StatusReceiverImpl() {};
    virtual void OnStatusNotify(const int progress) override {};
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override {};

private:
    DISALLOW_COPY_AND_MOVE(StatusReceiverImpl);
};

class BundleMgrProxyTest {
public:
    static sptr<IBundleMgr> GetBundleMgrProxy();
};

sptr<IBundleMgr> BundleMgrProxyTest::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        return nullptr;
    }

    return iface_cast<IBundleMgr>(remoteObject);
}

/**
 * @tc.name: BenchmarkTestGetApplicationInfo
 * @tc.desc: Testcase for testing GetApplicationInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetApplicationInfoByFlag(benchmark::State &state)
{
    int userId = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    ApplicationInfo appInfo;
    appInfo.name = "com.ohos.contactsdataability";
    appInfo.bundleName = "com.ohos.contactsdataability";
    appInfo.versionName = "1.0";
    appInfo.iconPath = "$media:icon";
    appInfo.description = "dataability_description";
    appInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    appInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    appInfo.apiReleaseType = "Release";
    appInfo.deviceId = "PHONE-001";
    appInfo.entityType = "unsppecified";
    appInfo.vendor = "ohos";
    appInfo.nativeLibraryPath = "libs/arm";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
    }
}

/**
 * @tc.name: BenchmarkTestGetApplicationInfo
 * @tc.desc: Testcase for testing GetApplicationInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetApplicationInfoByUserId(benchmark::State &state)
{
    int userId = Constants::DEFAULT_USERID;
    int32_t flags = 0;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    ApplicationInfo appInfo;
    appInfo.name = "com.ohos.contactsdataability";
    appInfo.bundleName = "com.ohos.contactsdataability";
    appInfo.versionName = "1.0";
    appInfo.iconPath = "$media:icon";
    appInfo.description = "dataability_description";
    appInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    appInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    appInfo.apiReleaseType = "Release";
    appInfo.deviceId = "PHONE-001";
    appInfo.entityType = "unsppecified";
    appInfo.vendor = "ohos";
    appInfo.nativeLibraryPath = "libs/arm";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetApplicationInfo(bundleName, flags, userId, appInfo);
    }
}

/**
 * @tc.name: BenchmarkTestGetApplicationInfosByApplicationFlag
 * @tc.desc: Testcase for testing GetApplicationInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetApplicationInfosByApplicationFlag(benchmark::State &state)
{
    int userId = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<ApplicationInfo> appInfos;
    ApplicationInfo appInfo;
    appInfo.name = "com.ohos.contactsdataability";
    appInfo.bundleName = "com.ohos.contactsdataability";
    appInfo.versionName = "1.0";
    appInfo.iconPath = "$media:icon";
    appInfo.description = "dataability_description";
    appInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    appInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    appInfo.apiReleaseType = "Release";
    appInfo.deviceId = "PHONE-001";
    appInfo.entityType = "unsppecified";
    appInfo.vendor = "ohos";
    appInfo.nativeLibraryPath = "libs/arm";
    appInfos.push_back(appInfo);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetApplicationInfos(ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, appInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetApplicationInfosByFlags
 * @tc.desc: Testcase for testing GetApplicationInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetApplicationInfosByFlags(benchmark::State &state)
{
    int userId = Constants::DEFAULT_USERID;
    int32_t flags = 0;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<ApplicationInfo> appInfos;
    ApplicationInfo appInfo;
    appInfo.name = "com.ohos.contactsdataability";
    appInfo.bundleName = "com.ohos.contactsdataability";
    appInfo.versionName = "1.0";
    appInfo.iconPath = "$media:icon";
    appInfo.description = "dataability_description";
    appInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    appInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    appInfo.apiReleaseType = "Release";
    appInfo.deviceId = "PHONE-001";
    appInfo.entityType = "unsppecified";
    appInfo.vendor = "ohos";
    appInfo.nativeLibraryPath = "libs/arm";
    appInfos.push_back(appInfo);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetApplicationInfos(flags, userId, appInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleInfoByBundleFlag
 * @tc.desc: Testcase for testing GetBundleInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleInfoByBundleFlag(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, info);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleInfoByFlags
 * @tc.desc: Testcase for testing GetBundleInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleInfoByFlags(benchmark::State &state)
{
    int32_t flags = 0;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleInfo(bundleName, flags, info);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleInfosByBundleFlag
 * @tc.desc: Testcase for testing GetBundleInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleInfosByBundleFlag(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<BundleInfo> bundleInfos;
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    bundleInfos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleInfosByFlags
 * @tc.desc: Testcase for testing GetBundleInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleInfosByFlags(benchmark::State &state)
{
    int32_t flags = 0;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<BundleInfo> bundleInfos;
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    bundleInfos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleInfos(flags, bundleInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetUidByBundleName
 * @tc.desc: Obtains the application ID based on the given bundle name and user ID.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetUidByBundleName(benchmark::State &state)
{
    int userId = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetUidByBundleName(bundleName, userId);
    }
}

/**
 * @tc.name: BenchmarkTestGetAppIdByBundleName
 * @tc.desc: Obtains the application ID based on the given bundle name and user ID.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetAppIdByBundleName(benchmark::State &state)
{
    int userId = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetAppIdByBundleName(bundleName, userId);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleNameForUid
 * @tc.desc: Testcase for testing GetBundleNameForUid.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleNameForUid(benchmark::State &state)
{
    int uid = Constants::INVALID_UID;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::string bundleName = "com.ohos.contactsdataability";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleNameForUid(uid, bundleName);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundlesForUid
 * @tc.desc: Testcase for testing GetBundlesForUid.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundlesForUid(benchmark::State &state)
{
    int uid = Constants::INVALID_UID;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<std::string> bundleNames;
    bundleNames.push_back(bundleName);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundlesForUid(uid, bundleNames);
    }
}

/**
 * @tc.name: BenchmarkTestGetNameForUid
 * @tc.desc: Testcase for testing GetNameForUid.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetNameForUid(benchmark::State &state)
{
    int uid = Constants::INVALID_UID;
    std::string name;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetNameForUid(uid, name);
    }
}

/**
 * @tc.name: BenchmarkTestGetAppType
 * @tc.desc: Testcase for testing GetAppType.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetAppType(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetAppType(SYSTEM_SETTINGS_BUNDLE_NAME);
    }
}

/**
 * @tc.name: BenchmarkTestCheckIsSystemAppByUid
 * @tc.desc: Testcase for testing CheckIsSystemAppByUid.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestCheckIsSystemAppByUid(benchmark::State &state)
{
    int uid = Constants::DEFAULT_USERID;
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->CheckIsSystemAppByUid(uid);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleInfosByMetaData
 * @tc.desc: Testcase for testing GetBundleInfosByMetaData.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleInfosByMetaData(benchmark::State &state)
{
    std::vector<BundleInfo> bundleInfos;
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    bundleInfos.push_back(info);
    std::string metadata = "string";
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleInfosByMetaData(metadata, bundleInfos);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfo
 * @tc.desc: Testcase for testing QueryAbilityInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfo(benchmark::State &state)
{
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);

    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAbilityInfo(want, info);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfoByFlags
 * @tc.desc: Testcase for testing QueryAbilityInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfoByFlags(benchmark::State &state)
{
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);

    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAbilityInfo(want, 0, 0, info);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfos
 * @tc.desc: Testcase for testing QueryAbilityInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfos(benchmark::State &state)
{
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);

    std::vector<AbilityInfo> abilityInfos;
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    abilityInfos.push_back(info);
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAbilityInfos(want, abilityInfos);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfosByFlags
 * @tc.desc: Testcase for testing QueryAbilityInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfosByFlags(benchmark::State &state)
{
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);

    std::vector<AbilityInfo> abilityInfos;
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    abilityInfos.push_back(info);
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAbilityInfos(want, 0, 0, abilityInfos);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfosForClone
 * @tc.desc: Testcase for testing QueryAbilityInfosForClone.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfosForClone(benchmark::State &state)
{
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);

    std::vector<AbilityInfo> abilityInfos;
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    abilityInfos.push_back(info);
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAbilityInfosForClone(want, abilityInfos);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfosById
 * @tc.desc: Testcase for testing QueryAllAbilityInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfosById(benchmark::State &state)
{
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);

    std::vector<AbilityInfo> abilityInfos;
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    abilityInfos.push_back(info);
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAllAbilityInfos(want, 0, abilityInfos);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfoByUri
 * @tc.desc: Testcase for testing QueryAbilityInfoByUri.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfoByUri(benchmark::State &state)
{
    std::string abilityUri = "err://com.test.demo.weatherfa.UserADataAbility";
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAbilityInfoByUri(abilityUri, info);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfoByUriAndId
 * @tc.desc: Testcase for testing QueryAbilityInfoByUri.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfoByUriAndId(benchmark::State &state)
{
    std::string abilityUri = "err://com.test.demo.weatherfa.UserADataAbility";
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAbilityInfoByUri(abilityUri, 0, info);
    }
}

/**
 * @tc.name: BenchmarkTestQueryAbilityInfosByUri
 * @tc.desc: Testcase for testing QueryAbilityInfosByUri.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryAbilityInfosByUri(benchmark::State &state)
{
    std::string abilityUri = "err://com.test.demo.weatherfa.UserADataAbility";
    std::vector<AbilityInfo> abilityInfos;
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    abilityInfos.push_back(info);
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryAbilityInfosByUri(abilityUri, abilityInfos);
    }
}

/**
 * @tc.name: BenchmarkTestQueryKeepAliveBundleInfos
 * @tc.desc: Testcase for testing QueryKeepAliveBundleInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryKeepAliveBundleInfos(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<BundleInfo> bundleInfos;
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    bundleInfos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryKeepAliveBundleInfos(bundleInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetAbilityLabel
 * @tc.desc: Testcase for testing GetAbilityLabel.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetAbilityLabel(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetAbilityLabel(bundleName, abilityName);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleArchiveInfo
 * @tc.desc: Testcase for testing GetBundleArchiveInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleArchiveInfo(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, 0, info);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleArchiveInfoByFlag
 * @tc.desc: Testcase for testing GetBundleArchiveInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleArchiveInfoByFlag(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleArchiveInfo(hapFilePath, BundleFlag::GET_BUNDLE_DEFAULT, info);
    }
}

/**
 * @tc.name: BenchmarkTestGetLaunchWantForBundle
 * @tc.desc: Testcase for testing GetLaunchWantForBundle.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetLaunchWantForBundle(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    Want want;
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetLaunchWantForBundle(bundleName, want);
    }
}

/**
 * @tc.name: BenchmarkTestGetPermissionDef
 * @tc.desc: Testcase for testing GetPermissionDef.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetPermissionDef(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::string permissionName;
    PermissionDef info;
    info.bundleName = "com.ohos.contactsdataability";
    info.description = "dataability_description";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetPermissionDef(permissionName, info);
    }
}

/**
 * @tc.name: BenchmarkTestHasSystemCapability
 * @tc.desc: Testcase for testing HasSystemCapability.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestHasSystemCapability(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->HasSystemCapability(abilityName);
    }
}

/**
 * @tc.name: BenchmarkTestGetSystemAvailableCapabilities
 * @tc.desc: Testcase for testing GetSystemAvailableCapabilities.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetSystemAvailableCapabilities(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<std::string> systemCaps = {"bmsSystemBundle_A1"};
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetSystemAvailableCapabilities(systemCaps);
    }
}

/**
 * @tc.name: BenchmarkTestIsSafeMode
 * @tc.desc: Testcase for testing IsSafeMode.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestIsSafeMode(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->IsSafeMode();
    }
}

/**
 * @tc.name: BenchmarkTestCleanBundleDataFiles
 * @tc.desc: Testcase for testing CleanBundleDataFiles.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestCleanBundleDataFiles(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->CleanBundleDataFiles(bundleName, 0);
    }
}

/**
 * @tc.name: BenchmarkTestRegisterBundleStatusCallback
 * @tc.desc: Testcase for testing RegisterBundleStatusCallback.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestRegisterBundleStatusCallback(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
    bundleStatusCallback->SetBundleName(bundleName);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->RegisterBundleStatusCallback(bundleStatusCallback);
    }
}

/**
 * @tc.name: BenchmarkTestClearBundleStatusCallback
 * @tc.desc: Testcase for testing ClearBundleStatusCallback.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestClearBundleStatusCallback(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    sptr<BundleStatusCallbackImpl> bundleStatusCallback = (new (std::nothrow) BundleStatusCallbackImpl());
    bundleStatusCallback->SetBundleName(bundleName);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->ClearBundleStatusCallback(bundleStatusCallback);
    }
}

/**
 * @tc.name: BenchmarkTestUnregisterBundleStatusCallback
 * @tc.desc: Testcase for testing UnregisterBundleStatusCallback.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestUnregisterBundleStatusCallback(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->UnregisterBundleStatusCallback();
    }
}

/**
 * @tc.name: BenchmarkTestDumpInfos
 * @tc.desc: Testcase for testing DumpInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestDumpInfos(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    const std::string EMPTY_STRING = "";
    std::string allInfoResult;
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
    bundleMgrProxy->DumpInfos(
        DumpFlag::DUMP_ALL_BUNDLE_INFO, EMPTY_STRING, DEFAULT_USERID, allInfoResult);
    }
}

/**
 * @tc.name: BenchmarkTestIsApplicationEnabled
 * @tc.desc: Testcase for testing IsApplicationEnabled.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestIsApplicationEnabled(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->IsApplicationEnabled(bundleName);
    }
}

/**
 * @tc.name: BenchmarkTestSetApplicationEnabled
 * @tc.desc: Testcase for testing SetApplicationEnabled.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestSetApplicationEnabled(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->SetApplicationEnabled(bundleName, false);
    }
}

/**
 * @tc.name: BenchmarkTestIsAbilityEnabled
 * @tc.desc: Testcase for testing IsAbilityEnabled.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestIsAbilityEnabled(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->IsAbilityEnabled(info);
    }
}

/**
 * @tc.name: BenchmarkTestSetAbilityEnabled
 * @tc.desc: Testcase for testing SetAbilityEnabled.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestSetAbilityEnabled(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->SetAbilityEnabled(info, false);
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleInstaller
 * @tc.desc: Testcase for testing GetBundleInstaller.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleInstaller(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleInstaller();
    }
}

/**
 * @tc.name: BenchmarkTestGetBundleUserMgr
 * @tc.desc: Testcase for testing GetBundleUserMgr.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetBundleUserMgr(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetBundleUserMgr();
    }
}

/**
 * @tc.name: BenchmarkTestGetAllFormsInfo
 * @tc.desc: Testcase for testing GetAllFormsInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetAllFormsInfo(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<FormInfo> formInfos;
    FormInfo info;
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.name = "com.ohos.contactsdataability";
    info.description = "dataability_description";
    formInfos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetAllFormsInfo(formInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetFormsInfoByApp
 * @tc.desc: Testcase for testing GetFormsInfoByApp.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetFormsInfoByApp(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<FormInfo> formInfos;
    FormInfo info;
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.name = "com.ohos.contactsdataability";
    info.description = "dataability_description";
    formInfos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetFormsInfoByApp(bundleName, formInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetFormsInfoByModule
 * @tc.desc: Testcase for testing GetFormsInfoByModule.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetFormsInfoByModule(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<FormInfo> formInfos;
    FormInfo info;
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.name = "com.ohos.contactsdataability";
    info.description = "dataability_description";
    formInfos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetFormsInfoByModule(bundleName, MODULE_NAME_TEST, formInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetShortcutInfos
 * @tc.desc: Testcase for testing GetShortcutInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetShortcutInfos(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<ShortcutInfo> shortcutInfos;
    ShortcutInfo info;
    info.bundleName = "com.ohos.contactsdataability";
    info.iconId = 0;
    info.labelId = 0;
    shortcutInfos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetShortcutInfos(bundleName, shortcutInfos);
    }
}

/**
 * @tc.name: BenchmarkTestGetAllCommonEventInfo
 * @tc.desc: Testcase for testing GetAllCommonEventInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetAllCommonEventInfo(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<CommonEventInfo> commonEventInfos;
    CommonEventInfo info;
    info.name = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    commonEventInfos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetAllCommonEventInfo(COMMON_EVENT_EVENT, commonEventInfos);
    }
}


/**
 * @tc.name: BenchmarkTestRemoveClonedBundle
 * @tc.desc: Testcase for testing RemoveClonedBundle.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestRemoveClonedBundle(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->RemoveClonedBundle(bundleName, DEFAULT_USERID);
    }
}

/**
 * @tc.name: BenchmarkTestCheckBundleNameInAllowList
 * @tc.desc: Testcase for testing CheckBundleNameInAllowList.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestCheckBundleNameInAllowList(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->CheckBundleNameInAllowList(bundleName);
    }
}

/**
 * @tc.name: BenchmarkTestGetDistributedBundleInfo
 * @tc.desc: Testcase for testing GetDistributedBundleInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetDistributedBundleInfo(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::string networkId;
    DistributedBundleInfo distributedBundleInfo;
    distributedBundleInfo.bundleName = "com.ohos.contactsdataability";
    distributedBundleInfo.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetDistributedBundleInfo(networkId, bundleName, distributedBundleInfo);
    }
}

/**
 * @tc.name: BenchmarkTestGetAppPrivilegeLevel
 * @tc.desc: Testcase for testing GetAppPrivilegeLevel.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetAppPrivilegeLevel(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetAppPrivilegeLevel(bundleName);
    }
}

/**
 * @tc.name: BenchmarkTestQueryExtensionAbilityInfosByWant
 * @tc.desc: Testcase for testing QueryExtensionAbilityInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryExtensionAbilityInfosByWant(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);
    int32_t flags = 0;
    std::vector<ExtensionAbilityInfo> infos;
    ExtensionAbilityInfo info;
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.name = "com.ohos.contactsdataability";
    info.description = "dataability_description";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.uri = "dataability://com.ohos.callogability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    infos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryExtensionAbilityInfos(want, flags, DEFAULT_USERID, infos);
    }
}

/**
 * @tc.name: BenchmarkTestQueryExtensionAbilityInfosByType
 * @tc.desc: Testcase for testing QueryExtensionAbilityInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryExtensionAbilityInfosByType(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::vector<ExtensionAbilityInfo> infos;
    ExtensionAbilityInfo info;
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.name = "com.ohos.contactsdataability";
    info.description = "dataability_description";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.uri = "dataability://com.ohos.callogability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    infos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryExtensionAbilityInfos(ExtensionAbilityType::FORM, DEFAULT_USERID, infos);
    }
}

/**
 * @tc.name: BenchmarkTestQueryExtensionAbilityInfos
 * @tc.desc: Testcase for testing QueryExtensionAbilityInfos.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryExtensionAbilityInfos(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    Want want;
    ElementName name;
    name.SetAbilityName(abilityName);
    name.SetBundleName(bundleName);
    want.SetElement(name);
    int32_t flags = 0;
    std::vector<ExtensionAbilityInfo> infos;
    ExtensionAbilityInfo info;
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.name = "com.ohos.contactsdataability";
    info.description = "dataability_description";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.uri = "dataability://com.ohos.callogability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    infos.push_back(info);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryExtensionAbilityInfos(want, ExtensionAbilityType::FORM, flags, DEFAULT_USERID, infos);
    }
}

/**
 * @tc.name: BenchmarkTestVerifyCallingPermission
 * @tc.desc: Testcase for testing VerifyCallingPermission.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestVerifyCallingPermission(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    std::string appPermission = "USER_GRANT";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->VerifyCallingPermission(appPermission);
    }
}

/**
 * @tc.name: BenchmarkTestGetAccessibleAppCodePaths
 * @tc.desc: Testcase for testing GetAccessibleAppCodePaths.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetAccessibleAppCodePaths(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetAccessibleAppCodePaths(DEFAULT_USERID);
    }
}

/**
 * @tc.name: BenchmarkTestQueryExtensionAbilityInfoByUri
 * @tc.desc: Testcase for testing QueryExtensionAbilityInfoByUri.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestQueryExtensionAbilityInfoByUri(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    ExtensionAbilityInfo info;
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.name = "com.ohos.contactsdataability";
    info.description = "dataability_description";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.uri = "dataability://com.ohos.callogability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "PHONE-001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    const std::string URI = "dataability://com.example.hiworld.himusic.UserADataAbility";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->QueryExtensionAbilityInfoByUri(URI, DEFAULT_USERID, info);
    }
}

/**
 * @tc.name: BenchmarkTestGetAbilityInfo
 * @tc.desc: Testcase for testing GetAbilityInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestGetAbilityInfo(benchmark::State &state)
{
    sptr<IBundleMgr> bundleMgrProxy = BundleMgrProxyTest::GetBundleMgrProxy();
    AbilityInfo info;
    info.name = "allLogAbility";
    info.description = "dataability_description";
    info.iconPath = "$media:icon";
    info.labelId = 0;
    info.descriptionId = 0;
    info.iconId = 0;
    info.kind = "data";
    info.deviceTypes = {"smartVision"};
    info.uri = "dataability://com.ohos.callogability";
    info.readPermission = "ohos.permission.READ_CALL_LOG";
    info.writePermission = "ohos.permission.WRITE_CALL_LOG";
    info.package = "com.ohos.contactsdataability";
    info.bundleName = "com.ohos.contactsdataability";
    info.moduleName = "entry";
    info.applicationName = "com.ohos.contactsdataability";
    info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
        "/com.ohos.contactsdataability/assets/entry/resources.index";
    info.versionName = "1.0";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.cpuAbi = "armeabi";
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        bundleMgrProxy->GetAbilityInfo(bundleName, abilityName, info);
    }
}

BENCHMARK(BenchmarkTestGetApplicationInfoByFlag)->Iterations(1000);
BENCHMARK(BenchmarkTestGetApplicationInfoByUserId)->Iterations(1000);
BENCHMARK(BenchmarkTestGetApplicationInfosByApplicationFlag)->Iterations(1000);
BENCHMARK(BenchmarkTestGetApplicationInfosByFlags)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleInfoByBundleFlag)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleInfoByFlags)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleInfosByBundleFlag)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleInfosByFlags)->Iterations(1000);
BENCHMARK(BenchmarkTestGetUidByBundleName)->Iterations(1000);
BENCHMARK(BenchmarkTestGetAppIdByBundleName)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleNameForUid)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundlesForUid)->Iterations(1000);
BENCHMARK(BenchmarkTestGetNameForUid)->Iterations(1000);
BENCHMARK(BenchmarkTestGetAppType)->Iterations(1000);
BENCHMARK(BenchmarkTestCheckIsSystemAppByUid)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleInfosByMetaData)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfo)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfoByFlags)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfos)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfosByFlags)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfosForClone)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfosById)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfoByUriAndId)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfoByUri)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryAbilityInfosByUri)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryKeepAliveBundleInfos)->Iterations(1000);
BENCHMARK(BenchmarkTestGetAbilityLabel)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleArchiveInfo)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleArchiveInfoByFlag)->Iterations(1000);
BENCHMARK(BenchmarkTestGetLaunchWantForBundle)->Iterations(1000);
BENCHMARK(BenchmarkTestGetPermissionDef)->Iterations(1000);
BENCHMARK(BenchmarkTestHasSystemCapability)->Iterations(1000);
BENCHMARK(BenchmarkTestGetSystemAvailableCapabilities)->Iterations(1000);
BENCHMARK(BenchmarkTestIsSafeMode)->Iterations(1000);
BENCHMARK(BenchmarkTestCleanBundleDataFiles)->Iterations(1000);
BENCHMARK(BenchmarkTestRegisterBundleStatusCallback)->Iterations(1000);
BENCHMARK(BenchmarkTestClearBundleStatusCallback)->Iterations(1000);
BENCHMARK(BenchmarkTestUnregisterBundleStatusCallback)->Iterations(1000);
BENCHMARK(BenchmarkTestDumpInfos)->Iterations(1000);
BENCHMARK(BenchmarkTestIsApplicationEnabled)->Iterations(1000);
BENCHMARK(BenchmarkTestSetApplicationEnabled)->Iterations(1000);
BENCHMARK(BenchmarkTestIsAbilityEnabled)->Iterations(1000);
BENCHMARK(BenchmarkTestSetAbilityEnabled)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleInstaller)->Iterations(1000);
BENCHMARK(BenchmarkTestGetBundleUserMgr)->Iterations(1000);
BENCHMARK(BenchmarkTestGetAllFormsInfo)->Iterations(1000);
BENCHMARK(BenchmarkTestGetFormsInfoByApp)->Iterations(1000);
BENCHMARK(BenchmarkTestGetFormsInfoByModule)->Iterations(1000);
BENCHMARK(BenchmarkTestGetShortcutInfos)->Iterations(1000);
BENCHMARK(BenchmarkTestGetAllCommonEventInfo)->Iterations(1000);
BENCHMARK(BenchmarkTestRemoveClonedBundle)->Iterations(1000);
BENCHMARK(BenchmarkTestCheckBundleNameInAllowList)->Iterations(1000);
BENCHMARK(BenchmarkTestGetDistributedBundleInfo)->Iterations(1000);
BENCHMARK(BenchmarkTestGetAppPrivilegeLevel)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryExtensionAbilityInfosByWant)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryExtensionAbilityInfos)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryExtensionAbilityInfosByType)->Iterations(1000);
BENCHMARK(BenchmarkTestVerifyCallingPermission)->Iterations(1000);
BENCHMARK(BenchmarkTestGetAccessibleAppCodePaths)->Iterations(1000);
BENCHMARK(BenchmarkTestQueryExtensionAbilityInfoByUri)->Iterations(1000);
BENCHMARK(BenchmarkTestGetAbilityInfo)->Iterations(1000);
}  // namespace

BENCHMARK_MAIN();