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

#include "launcher_service.h"

#include "bundle_mgr_proxy.h"
#include "common_event_subscribe_info.h"
#include "common_event_support.h"
#include "matching_skills.h"
#include "operation_builder.h"

namespace OHOS {
namespace AppExecFwk {
OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> LauncherService::bundleMgr_ = nullptr;
std::mutex LauncherService::bundleMgrMutex_;

LauncherService::LauncherService()
{
    init();
}

void LauncherService::init()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    bundleMonitor_ = std::make_shared<BundleMonitor>(subscribeInfo);
}

OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> LauncherService::GetBundleMgr()
{
    if (bundleMgr_ == nullptr) {
        std::lock_guard<std::mutex> lock(bundleMgrMutex_);
        if (bundleMgr_ == nullptr) {
            auto systemAbilityManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (systemAbilityManager == nullptr) {
                APP_LOGE("GetBundleMgr GetSystemAbilityManager is null");
                return nullptr;
            }
            auto bundleMgrSa = systemAbilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
            if (bundleMgrSa == nullptr) {
                APP_LOGE("GetBundleMgr GetSystemAbility is null");
                return nullptr;
            }
            auto bundleMgr = OHOS::iface_cast<IBundleMgr>(bundleMgrSa);
            if (bundleMgr == nullptr) {
                APP_LOGE("GetBundleMgr iface_cast get null");
            }
            bundleMgr_ = bundleMgr;
        }
    }
    return bundleMgr_;
}

bool LauncherService::RegisterCallback(const sptr<IBundleStatusCallback> &callback)
{
    APP_LOGI("RegisterCallback called");
    if (bundleMonitor_ == nullptr) {
        APP_LOGE("failed to register callback, bundleMonitor is null");
        return false;
    }

    // check permission
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    if (!iBundleMgr->VerifyCallingPermission(Constants::LISTEN_BUNDLE_CHANGE)) {
        APP_LOGE("register bundle status callback failed due to lack of permission");
        return false;
    }
    return bundleMonitor_->Subscribe(callback);
}

bool LauncherService::UnRegisterCallback()
{
    APP_LOGI("UnRegisterCallback called");
    if (bundleMonitor_ == nullptr) {
        APP_LOGE("failed to unregister callback, bundleMonitor is null");
        return false;
    }

    // check permission
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    if (!iBundleMgr->VerifyCallingPermission(Constants::LISTEN_BUNDLE_CHANGE)) {
        APP_LOGE("register bundle status callback failed due to lack of permission");
        return false;
    }
    return bundleMonitor_->UnSubscribe();
}

bool LauncherService::GetAbilityList(
    const std::string &bundleName, const int userId, std::vector<LauncherAbilityInfo> &launcherAbilityInfos)
{
    APP_LOGI("GetAbilityList called");
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr || bundleName.empty()) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }

    std::vector<std::string> entities;
    entities.push_back(Want::ENTITY_HOME);
    Want want;
    want.SetAction(Want::ACTION_HOME);
    want.AddEntity(Want::ENTITY_HOME);
    ElementName elementName;
    elementName.SetBundleName(bundleName);
    want.SetElement(elementName);
    std::vector<AbilityInfo> abilityInfos;
    if (!iBundleMgr->QueryAllAbilityInfos(want, userId, abilityInfos)) {
        APP_LOGE("Query ability info failed");
        return false;
    }

    BundleFlag flags = BundleFlag::GET_BUNDLE_DEFAULT;
    BundleInfo bundleInfo;
    if (!iBundleMgr->GetBundleInfo(bundleName, flags, bundleInfo, userId)) {
        APP_LOGE("Get bundle info failed");
        return false;
    }

    for (auto &ability : abilityInfos) {
        if (ability.bundleName != bundleName || !ability.enabled) {
            continue;
        }
        LauncherAbilityInfo info;
        info.applicationInfo = ability.applicationInfo;
        info.labelId = ability.labelId;
        info.iconId = ability.iconId;
        ElementName elementName;
        elementName.SetBundleName(ability.bundleName);
        elementName.SetAbilityName(ability.name);
        elementName.SetDeviceID(ability.deviceId);
        info.elementName = elementName;
        info.userId = userId;
        info.installTime = bundleInfo.installTime;
        launcherAbilityInfos.emplace_back(info);
    }

    return true;
}

bool LauncherService::GetAllLauncherAbilityInfos(int32_t userId, std::vector<LauncherAbilityInfo> &launcherAbilityInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }

    Want want;
    want.SetAction(Want::ACTION_HOME);
    want.AddEntity(Want::ENTITY_HOME);

    std::vector<AbilityInfo> abilityInfos;
    if (!iBundleMgr->QueryAllAbilityInfos(want, userId, abilityInfos)) {
        APP_LOGE("Query ability info failed");
        return false;
    }

    for (const auto& ability : abilityInfos) {
        if (ability.applicationInfo.isLauncherApp || !ability.enabled) {
            continue;
        }
        LauncherAbilityInfo info;
        BundleInfo bundleInfo;
        BundleFlag flags = BundleFlag::GET_BUNDLE_DEFAULT;
        if (!iBundleMgr->GetBundleInfo(ability.bundleName, flags, bundleInfo, userId)) {
            APP_LOGE("Get bundle info failed for %{public}s",  ability.bundleName.c_str());
            continue;
        }
        info.installTime = bundleInfo.installTime;
        info.applicationInfo = ability.applicationInfo;
        info.labelId = ability.labelId;
        info.iconId = ability.iconId;
        info.userId = userId;
        ElementName elementName;
        elementName.SetBundleName(ability.bundleName);
        elementName.SetModuleName(ability.moduleName);
        elementName.SetAbilityName(ability.name);
        elementName.SetDeviceID(ability.deviceId);
        info.elementName = elementName;
        launcherAbilityInfos.emplace_back(info);
    }
    return true;
}

bool LauncherService::GetAbilityInfo(const Want &want, const int userId, LauncherAbilityInfo &launcherAbilityInfo)
{
    APP_LOGI("GetAbilityInfo called");
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    ElementName element = want.GetElement();
    if (element.GetBundleName().empty() || element.GetAbilityName().empty()) {
        APP_LOGE("GetAbilityInfo elementName is empty");
        return false;
    }
    AbilityInfo abilityInfo;
    if (!iBundleMgr->QueryAbilityInfo(want, abilityInfo)) {
        APP_LOGE("Query AbilityInfo failed");
        return false;
    }
    if (!abilityInfo.enabled) {
        APP_LOGE("Query AbilityInfo is disabled");
        return false;
    }
    std::string bundleName = element.GetBundleName();
    int32_t iconId;
    ApplicationInfo appInfo;
    ApplicationFlag flag = ApplicationFlag::GET_BASIC_APPLICATION_INFO;
    if (!iBundleMgr->GetApplicationInfo(bundleName, flag, userId, appInfo)) {
        APP_LOGE("Get application info failed");
        return false;
    }
    iconId = appInfo.iconId;

    int64_t installTime = 0;
    BundleFlag flags = BundleFlag::GET_BUNDLE_DEFAULT;
    flags = BundleFlag::GET_BUNDLE_WITH_ABILITIES;
    BundleInfo bundleInfo;
    if (!iBundleMgr->GetBundleInfo(bundleName, flags, bundleInfo, userId)) {
        APP_LOGE("Get bundle info failed");
        return false;
    }
    installTime = bundleInfo.installTime;

    LauncherAbilityInfo info;
    info.applicationInfo = abilityInfo.applicationInfo;
    info.labelId = abilityInfo.labelId;
    ElementName elementName;
    elementName.SetBundleName(abilityInfo.bundleName);
    elementName.SetModuleName(abilityInfo.moduleName);
    elementName.SetAbilityName(abilityInfo.name);
    elementName.SetDeviceID(abilityInfo.deviceId);
    info.elementName = elementName;
    info.iconId = iconId;
    info.userId = userId;
    info.installTime = installTime;
    launcherAbilityInfo = info;

    return true;
}

bool LauncherService::GetApplicationInfo(
    const std::string &bundleName, const ApplicationFlag &flags, const int userId, ApplicationInfo &applicationInfo)
{
    APP_LOGI("GetApplicationInfo called");
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    if (bundleName.empty()) {
        APP_LOGE("GetApplicationInfo bundleName is empty");
        return false;
    }

    if (!iBundleMgr->GetApplicationInfo(bundleName, flags, userId, applicationInfo)) {
        APP_LOGE("Get application info failed");
        return false;
    }

    return true;
}

bool LauncherService::IsBundleEnabled(const std::string &bundleName)
{
    APP_LOGI("IsBundleEnabled called");
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    if (bundleName.empty()) {
        APP_LOGE("bundleName is empty");
        return false;
    }

    return iBundleMgr->IsApplicationEnabled(bundleName);
}

bool LauncherService::IsAbilityEnabled(const AbilityInfo &abilityInfo)
{
    APP_LOGI("IsAbilityEnabled called");
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }

    return iBundleMgr->IsAbilityEnabled(abilityInfo);
}

bool LauncherService::GetShortcutInfos(
    const std::string &bundleName, std::vector<ShortcutInfo> &shortcutInfos)
{
    APP_LOGI("GetShortcutInfos called");
    if (bundleName.empty()) {
        APP_LOGE("bundleName is empty");
        return false;
    }
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }

    std::vector<ShortcutInfo> infos;
    if (!iBundleMgr->GetShortcutInfos(bundleName, infos)) {
        APP_LOGE("Get shortcut infos failed");
        return false;
    }
    if (infos.size() == 0) {
        APP_LOGE("ShortcutInfo is not exist in system");
        return false;
    }

    for (ShortcutInfo shortcutInfo : infos) {
        if (bundleName == shortcutInfo.bundleName) {
            shortcutInfos.emplace_back(shortcutInfo);
        }
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS