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

#include "bundle_data_mgr.h"

#include <chrono>
#include <cinttypes>

#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
#include "installd/installd_operator.h"
#include "os_account_info.h"
#endif
#include "account_helper.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_data_storage_database.h"
#include "bundle_mgr_service.h"
#include "bundle_status_callback_death_recipient.h"
#include "bundle_util.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#ifdef BUNDLE_FRAMEWORK_GRAPHICS
#include "image_source.h"
#endif
#include "ipc_skeleton.h"
#include "json_serializer.h"
#include "nlohmann/json.hpp"
#include "free_install_params.h"
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
BundleDataMgr::BundleDataMgr()
{
    InitStateTransferMap();
    dataStorage_ = std::make_shared<BundleDataStorageDatabase>();
    preInstallDataStorage_ = std::make_shared<PreInstallDataStorage>();
    distributedDataStorage_ = DistributedDataStorage::GetInstance();
    sandboxDataMgr_ = std::make_shared<BundleSandboxDataMgr>();
    bundleStateStorage_ = std::make_shared<BundleStateStorage>();
    APP_LOGI("BundleDataMgr instance is created");
}

BundleDataMgr::~BundleDataMgr()
{
    APP_LOGI("BundleDataMgr instance is destroyed");
    installStates_.clear();
    transferStates_.clear();
    bundleInfos_.clear();
}

bool BundleDataMgr::LoadDataFromPersistentStorage()
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    // Judge whether bundleState json db exists.
    // If it does not exist, create it and return the judgment result.
    bool bundleStateDbExist = bundleStateStorage_->HasBundleUserInfoJsonDb();
    if (!dataStorage_->LoadAllData(bundleInfos_)) {
        APP_LOGE("LoadAllData failed");
        return false;
    }

    if (bundleInfos_.empty()) {
        APP_LOGW("persistent data is empty");
        return false;
    }

    for (const auto &item : bundleInfos_) {
        std::lock_guard<std::mutex> lock(stateMutex_);
        installStates_.emplace(item.first, InstallState::INSTALL_SUCCESS);
    }

    LoadAllPreInstallBundleInfos(preInstallBundleInfos_);
    RestoreUidAndGid();
    if (!bundleStateDbExist) {
        // Compatible old bundle status in kV db
        CompatibleOldBundleStateInKvDb();
    } else {
        ResetBundleStateData();
        // Load all bundle status from json db.
        LoadAllBundleStateDataFromJsonDb();
    }

    SetInitialUserFlag(true);
    return true;
}

void BundleDataMgr::CompatibleOldBundleStateInKvDb()
{
    for (auto& bundleInfoItem : bundleInfos_) {
        for (auto& innerBundleUserInfoItem : bundleInfoItem.second.GetInnerBundleUserInfos()) {
            auto& bundleUserInfo = innerBundleUserInfoItem.second.bundleUserInfo;
            if (bundleUserInfo.IsInitialState()) {
                continue;
            }

            // save old bundle state to json db
            bundleStateStorage_->SaveBundleStateStorage(
                bundleInfoItem.first, bundleUserInfo.userId, bundleUserInfo);
        }
    }
}

void BundleDataMgr::LoadAllBundleStateDataFromJsonDb()
{
    APP_LOGD("Load all bundle state start");
    std::map<std::string, std::map<int32_t, BundleUserInfo>> bundleStateInfos;
    if (!bundleStateStorage_->LoadAllBundleStateData(bundleStateInfos) || bundleStateInfos.empty()) {
        APP_LOGE("Load all bundle state failed");
        return;
    }

    for (auto& bundleState : bundleStateInfos) {
        auto infoItem = bundleInfos_.find(bundleState.first);
        if (infoItem == bundleInfos_.end()) {
            APP_LOGE("BundleName(%{public}s) not exist in cache", bundleState.first.c_str());
            continue;
        }

        InnerBundleInfo& newInfo = infoItem->second;
        for (auto& bundleUserState : bundleState.second) {
            auto& tempUserInfo = bundleUserState.second;
            newInfo.SetApplicationEnabled(tempUserInfo.enabled, bundleUserState.first);
            for (auto& disabledAbility : tempUserInfo.disabledAbilities) {
                newInfo.SetAbilityEnabled(bundleState.first, "", disabledAbility, false, bundleUserState.first);
            }
        }
    }

    APP_LOGD("Load all bundle state end");
}

void BundleDataMgr::ResetBundleStateData()
{
    for (auto& bundleInfoItem : bundleInfos_) {
        bundleInfoItem.second.ResetBundleState(Constants::ALL_USERID);
    }
}

bool BundleDataMgr::UpdateBundleInstallState(const std::string &bundleName, const InstallState state)
{
    if (bundleName.empty()) {
        APP_LOGW("update result:fail, reason:bundle name is empty");
        return false;
    }

    // always keep lock bundleInfoMutex_ before locking stateMutex_ to avoid deadlock
    std::lock_guard<std::mutex> lck(bundleInfoMutex_);
    std::lock_guard<std::mutex> lock(stateMutex_);
    auto item = installStates_.find(bundleName);
    if (item == installStates_.end()) {
        if (state == InstallState::INSTALL_START) {
            installStates_.emplace(bundleName, state);
            APP_LOGD("update result:success, state:INSTALL_START");
            return true;
        }
        APP_LOGW("update result:fail, reason:incorrect state");
        return false;
    }

    auto stateRange = transferStates_.equal_range(state);
    for (auto previousState = stateRange.first; previousState != stateRange.second; ++previousState) {
        if (item->second == previousState->second) {
            APP_LOGD("update result:success, current:%{public}d, state:%{public}d", previousState->second, state);
            if (IsDeleteDataState(state)) {
                installStates_.erase(item);
                DeleteBundleInfo(bundleName, state);
                return true;
            }
            item->second = state;
            return true;
        }
    }
    APP_LOGW("update result:fail, reason:incorrect current:%{public}d, state:%{public}d", item->second, state);
    return false;
}

bool BundleDataMgr::AddInnerBundleInfo(const std::string &bundleName, InnerBundleInfo &info)
{
    APP_LOGD("to save info:%{public}s", info.GetBundleName().c_str());
    if (bundleName.empty()) {
        APP_LOGW("save info fail, empty bundle name");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem != bundleInfos_.end()) {
        APP_LOGE("bundle info already exist");
        return false;
    }
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGE("save info fail, app:%{public}s is not installed", bundleName.c_str());
        return false;
    }
    if (statusItem->second == InstallState::INSTALL_START) {
        APP_LOGD("save bundle:%{public}s info", bundleName.c_str());
        if (dataStorage_->SaveStorageBundleInfo(info)) {
            APP_LOGI("write storage success bundle:%{public}s", bundleName.c_str());
            bundleInfos_.emplace(bundleName, info);
            return true;
        }
    }
    return false;
}

bool BundleDataMgr::SaveNewInfoToDB(const std::string &bundleName, InnerBundleInfo &info)
{
    APP_LOGD("SaveNewInfoToDB start");
    std::string Newbundlename = info.GetDBKeyBundleName();
    APP_LOGI("to save clone newinfo to DB info:%{public}s", Newbundlename.c_str());
    if (bundleName.empty()) {
        APP_LOGW("clone newinfo save info fail, empty bundle name");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(Newbundlename);
    if (infoItem != bundleInfos_.end()) {
        APP_LOGE("clone newinfo bundle info already exist");
        return false;
    }
    int64_t time =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    APP_LOGI("the clone newinfo bundle install time is %{public}" PRId64, time);
    info.SetBundleInstallTime(time);
    if (dataStorage_->SaveStorageBundleInfo(info)) {
        APP_LOGI("clone newinfo write storage success bundle:%{public}s", Newbundlename.c_str());
        bundleInfos_.emplace(Newbundlename, info);
        return true;
    }
    APP_LOGD("SaveNewInfoToDB finish");
    return false;
}

bool BundleDataMgr::AddNewModuleInfo(
    const std::string &bundleName, const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGD("add new module info module name %{public}s ", newInfo.GetCurrentModulePackage().c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("bundle info not exist");
        return false;
    }
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGE("save info fail, app:%{public}s is not updated", bundleName.c_str());
        return false;
    }
    if (statusItem->second == InstallState::UPDATING_SUCCESS) {
        APP_LOGD("save bundle:%{public}s info", bundleName.c_str());
        if (!oldInfo.HasEntry() || oldInfo.GetEntryInstallationFree()) {
            oldInfo.UpdateBaseBundleInfo(newInfo.GetBaseBundleInfo(), newInfo.HasEntry());
            oldInfo.UpdateBaseApplicationInfo(newInfo.GetBaseApplicationInfo());
            oldInfo.SetAppPrivilegeLevel(newInfo.GetAppPrivilegeLevel());
            oldInfo.SetAllowedAcls(newInfo.GetAllowedAcls());
        }
        oldInfo.SetBundlePackInfo(newInfo.GetBundlePackInfo());
        oldInfo.AddModuleInfo(newInfo);
        oldInfo.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
        if (dataStorage_->SaveStorageBundleInfo(oldInfo)) {
            APP_LOGI("update storage success bundle:%{public}s", bundleName.c_str());
            bundleInfos_.at(bundleName) = oldInfo;
            return true;
        }
    }
    return false;
}

bool BundleDataMgr::RemoveModuleInfo(
    const std::string &bundleName, const std::string &modulePackage, InnerBundleInfo &oldInfo)
{
    APP_LOGD("remove module info:%{public}s/%{public}s", bundleName.c_str(), modulePackage.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("bundle info not exist");
        return false;
    }
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGE("save info fail, app:%{public}s is not updated", bundleName.c_str());
        return false;
    }
    if (statusItem->second == InstallState::UNINSTALL_START || statusItem->second == InstallState::ROLL_BACK) {
        APP_LOGD("save bundle:%{public}s info", bundleName.c_str());
        oldInfo.RemoveModuleInfo(modulePackage);
        oldInfo.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
        if (dataStorage_->SaveStorageBundleInfo(oldInfo)) {
            APP_LOGI("update storage success bundle:%{public}s", bundleName.c_str());
            bundleInfos_.at(bundleName) = oldInfo;
            return true;
        }
        APP_LOGD("after delete modulePackage:%{public}s info", modulePackage.c_str());
    }
    return true;
}

bool BundleDataMgr::AddInnerBundleUserInfo(
    const std::string &bundleName, const InnerBundleUserInfo& newUserInfo)
{
    APP_LOGD("AddInnerBundleUserInfo:%{public}s", bundleName.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("bundle info not exist");
        return false;
    }

    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto& info = bundleInfos_.at(bundleName);
    info.AddInnerBundleUserInfo(newUserInfo);
    info.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
    if (!dataStorage_->SaveStorageBundleInfo(info)) {
        APP_LOGE("update storage failed bundle:%{public}s", bundleName.c_str());
        return false;
    }
    return true;
}

bool BundleDataMgr::RemoveInnerBundleUserInfo(
    const std::string &bundleName, int32_t userId)
{
    APP_LOGD("RemoveInnerBundleUserInfo:%{public}s", bundleName.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("bundle info not exist");
        return false;
    }

    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto& info = bundleInfos_.at(bundleName);
    info.RemoveInnerBundleUserInfo(userId);
    info.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
    if (!dataStorage_->SaveStorageBundleInfo(info)) {
        APP_LOGE("update storage failed bundle:%{public}s", bundleName.c_str());
        return false;
    }

    bundleStateStorage_->DeleteBundleState(bundleName, userId);
    return true;
}

bool BundleDataMgr::UpdateInnerBundleInfo(
    const std::string &bundleName, const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGD("UpdateInnerBundleInfo:%{public}s", bundleName.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("bundle info not exist");
        return false;
    }
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGE("save info fail, app:%{public}s is not updated", bundleName.c_str());
        return false;
    }
    // ROLL_BACK and USER_CHANGE should not be here
    if (statusItem->second == InstallState::UPDATING_SUCCESS
        || statusItem->second == InstallState::ROLL_BACK
        || statusItem->second == InstallState::USER_CHANGE) {
        APP_LOGD("begin to update, bundleName : %{public}s, moduleName : %{public}s",
            bundleName.c_str(), newInfo.GetCurrentModulePackage().c_str());
        // 1.exist entry, update entry.
        // 2.only exist feature, update feature.
        if (newInfo.HasEntry() || !oldInfo.HasEntry() || oldInfo.GetEntryInstallationFree()) {
            oldInfo.UpdateBaseBundleInfo(newInfo.GetBaseBundleInfo(), newInfo.HasEntry());
            oldInfo.UpdateBaseApplicationInfo(newInfo.GetBaseApplicationInfo());
            oldInfo.SetAppType(newInfo.GetAppType());
            oldInfo.SetAppFeature(newInfo.GetAppFeature());
            oldInfo.SetAppPrivilegeLevel(newInfo.GetAppPrivilegeLevel());
            oldInfo.SetAllowedAcls(newInfo.GetAllowedAcls());
        }
        oldInfo.SetBundlePackInfo(newInfo.GetBundlePackInfo());
        oldInfo.UpdateModuleInfo(newInfo);
        oldInfo.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
        if (!dataStorage_->SaveStorageBundleInfo(oldInfo)) {
            APP_LOGE("update storage failed bundle:%{public}s", bundleName.c_str());
            return false;
        }
        APP_LOGI("update storage success bundle:%{public}s", bundleName.c_str());
        bundleInfos_.at(bundleName) = oldInfo;
        return true;
    }
    return false;
}

bool BundleDataMgr::QueryAbilityInfo(const Want &want, int32_t flags, int32_t userId, AbilityInfo &abilityInfo) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    ElementName element = want.GetElement();
    std::string bundleName = element.GetBundleName();
    std::string abilityName = element.GetAbilityName();
    std::string moduleName = element.GetModuleName();
    APP_LOGD("QueryAbilityInfo bundle name:%{public}s, moduleName:%{public}s, ability name:%{public}s",
        bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
    // explicit query
    if (!bundleName.empty() && !abilityName.empty()) {
        bool ret = ExplicitQueryAbilityInfo(bundleName, moduleName, abilityName, flags, requestUserId, abilityInfo);
        if (!ret) {
            APP_LOGE("explicit queryAbilityInfo error");
            return false;
        }
        return true;
    }
    std::vector<AbilityInfo> abilityInfos;
    bool ret = ImplicitQueryAbilityInfos(want, flags, requestUserId, abilityInfos);
    if (!ret) {
        APP_LOGE("implicit queryAbilityInfos error");
        return false;
    }
    if (abilityInfos.size() == 0) {
        APP_LOGE("no matching abilityInfo");
        return false;
    }
    abilityInfo = abilityInfos[0];
    return true;
}

bool BundleDataMgr::QueryAbilityInfos(
    const Want &want, int32_t flags, int32_t userId, std::vector<AbilityInfo> &abilityInfos) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    ElementName element = want.GetElement();
    std::string bundleName = element.GetBundleName();
    std::string abilityName = element.GetAbilityName();
    std::string moduleName = element.GetModuleName();
    APP_LOGD("QueryAbilityInfos bundle name:%{public}s, moduleName:%{public}s, ability name:%{public}s",
        bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
    // explicit query
    if (!bundleName.empty() && !abilityName.empty()) {
        AbilityInfo abilityInfo;
        bool ret = ExplicitQueryAbilityInfo(
            bundleName, moduleName, abilityName, flags, requestUserId, abilityInfo);
        if (!ret) {
            APP_LOGE("explicit queryAbilityInfo error");
            return false;
        }
        abilityInfos.emplace_back(abilityInfo);
        return true;
    }
    // implicit query
    bool ret = ImplicitQueryAbilityInfos(want, flags, requestUserId, abilityInfos);
    if (!ret) {
        APP_LOGE("implicit queryAbilityInfos error");
        return false;
    }
    if (abilityInfos.size() == 0) {
        APP_LOGE("no matching abilityInfo");
        return false;
    }
    return true;
}

bool BundleDataMgr::ExplicitQueryAbilityInfo(const std::string &bundleName, const std::string &moduleName,
    const std::string &abilityName, int32_t flags, int32_t userId, AbilityInfo &abilityInfo) const
{
    APP_LOGD("ExplicitQueryAbilityInfo bundleName:%{public}s, moduleName:%{public}s, abilityName:%{public}s",
        bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
    APP_LOGD("flags:%{public}d, userId:%{public}d", flags, userId);
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoWithFlags(bundleName, flags, innerBundleInfo, requestUserId)) {
        APP_LOGE("ExplicitQueryAbilityInfo failed");
        return false;
    }

    int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
    auto ability = innerBundleInfo.FindAbilityInfo(bundleName, moduleName, abilityName, responseUserId);
    if (!ability) {
        APP_LOGE("ability not found");
        return false;
    }
    if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_SYSTEMAPP_ONLY) == GET_ABILITY_INFO_SYSTEMAPP_ONLY &&
        !innerBundleInfo.IsSystemApp()) {
        return false;
    }
    if (!(static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_DISABLE)) {
        if (!innerBundleInfo.IsAbilityEnabled((*ability), responseUserId)) {
            APP_LOGE("ability:%{public}s is disabled", ability->name.c_str());
            return false;
        }
    }
    if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_PERMISSION) != GET_ABILITY_INFO_WITH_PERMISSION) {
        ability->permissions.clear();
    }
    if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_METADATA) != GET_ABILITY_INFO_WITH_METADATA) {
        ability->metaData.customizeData.clear();
        ability->metadata.clear();
    }
    abilityInfo = (*ability);
    if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_APPLICATION) == GET_ABILITY_INFO_WITH_APPLICATION) {
        innerBundleInfo.GetApplicationInfo(
            ApplicationFlag::GET_BASIC_APPLICATION_INFO, responseUserId, abilityInfo.applicationInfo);
    }
    return true;
}

bool BundleDataMgr::QueryAbilityInfosForClone(const Want &want, std::vector<AbilityInfo> &abilityInfo)
{
    ElementName element = want.GetElement();
    std::string abilityName = element.GetAbilityName();
    std::string bundleName = element.GetBundleName();
    if (bundleName.empty()) {
        return false;
    }
    std::string keyName = bundleName + abilityName;
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ is empty");
        return false;
    }
    std::string cloneBundleName = bundleName;
    std::string name = bundleName + "#";
    for (auto it = bundleInfos_.begin(); it != bundleInfos_.end();) {
        if (it->first.find(name) != std::string::npos) {
            cloneBundleName = it->first;
            APP_LOGI("new name is %{public}s", cloneBundleName.c_str());
            break;
        } else {
            ++it;
        }
    }
    if (cloneBundleName != bundleName) {
        auto itemClone = bundleInfos_.find(cloneBundleName);
        if (itemClone == bundleInfos_.end()) {
            APP_LOGI("bundle:%{public}s not find", bundleName.c_str());
            return false;
        }
        InnerBundleInfo &infoClone = itemClone->second;
        if (infoClone.IsDisabled()) {
            return false;
        }

        int32_t responseUserId = infoClone.GetResponseUserId(GetUserId());
        infoClone.FindAbilityInfosForClone(bundleName, abilityName, responseUserId, abilityInfo);
    }

    auto item = bundleInfos_.find(bundleName);
    if (item == bundleInfos_.end()) {
        APP_LOGI("bundle:%{public}s not find", bundleName.c_str());
        return false;
    }
    InnerBundleInfo &info = item->second;
    int32_t responseUserId = info.GetResponseUserId(GetUserId());
    info.FindAbilityInfosForClone(bundleName, abilityName, responseUserId, abilityInfo);
    if (abilityInfo.size() == 0) {
        return false;
    }
    return true;
}

void BundleDataMgr::FilterAbilityInfosByModuleName(const std::string &moduleName,
    std::vector<AbilityInfo> &abilityInfos) const
{
    APP_LOGD("BundleDataMgr::FilterAbilityInfosByModuleName moduleName: %{public}s", moduleName.c_str());
    if (moduleName.empty()) {
        return;
    }
    for (auto iter = abilityInfos.begin(); iter != abilityInfos.end();) {
        if (iter->moduleName != moduleName) {
            iter = abilityInfos.erase(iter);
        } else {
            ++iter;
        }
    }
}

bool BundleDataMgr::ImplicitQueryAbilityInfos(
    const Want &want, int32_t flags, int32_t userId, std::vector<AbilityInfo> &abilityInfos) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    if (want.GetAction().empty() && want.GetEntities().empty()
        && want.GetUriString().empty() && want.GetType().empty()) {
        APP_LOGE("param invalid");
        return false;
    }
    APP_LOGD("action:%{public}s, uri:%{private}s, type:%{public}s",
        want.GetAction().c_str(), want.GetUriString().c_str(), want.GetType().c_str());
    APP_LOGD("flags:%{public}d, userId:%{public}d", flags, userId);
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ is empty");
        return false;
    }
    std::string bundleName = want.GetElement().GetBundleName();
    if (!bundleName.empty()) {
        // query in current bundleName
        InnerBundleInfo innerBundleInfo;
        if (!GetInnerBundleInfoWithFlags(bundleName, flags, innerBundleInfo, requestUserId)) {
            APP_LOGE("ImplicitQueryAbilityInfos failed");
            return false;
        }
        int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
        GetMatchAbilityInfos(want, flags, innerBundleInfo, responseUserId, abilityInfos);
        FilterAbilityInfosByModuleName(want.GetElement().GetModuleName(), abilityInfos);
    } else {
        // query all
        for (const auto &item : bundleInfos_) {
            InnerBundleInfo innerBundleInfo;
            if (!GetInnerBundleInfoWithFlags(
                item.first, flags, innerBundleInfo, requestUserId)) {
                APP_LOGE("ImplicitQueryAbilityInfos failed");
                continue;
            }

            int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
            GetMatchAbilityInfos(want, flags, innerBundleInfo, responseUserId, abilityInfos);
        }
    }
    // sort by priority, descending order.
    if (abilityInfos.size() > 1) {
        std::stable_sort(abilityInfos.begin(), abilityInfos.end(),
            [](AbilityInfo a, AbilityInfo b) { return a.priority > b.priority; });
    }
    return true;
}

void BundleDataMgr::GetMatchAbilityInfos(const Want &want, int32_t flags,
    const InnerBundleInfo &info, int32_t userId, std::vector<AbilityInfo> &abilityInfos) const
{
    if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_SYSTEMAPP_ONLY) == GET_ABILITY_INFO_SYSTEMAPP_ONLY &&
        !info.IsSystemApp()) {
        return;
    }
    std::map<std::string, std::vector<Skill>> skillInfos = info.GetInnerSkillInfos();
    for (const auto &abilityInfoPair : info.GetInnerAbilityInfos()) {
        auto skillsPair = skillInfos.find(abilityInfoPair.first);
        if (skillsPair == skillInfos.end()) {
            continue;
        }
        for (const Skill &skill : skillsPair->second) {
            if (skill.Match(want)) {
                AbilityInfo abilityinfo = abilityInfoPair.second;
                if (!(static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_DISABLE)) {
                    if (!info.IsAbilityEnabled(abilityinfo, GetUserId(userId))) {
                        APP_LOGW("GetMatchAbilityInfos %{public}s is disabled", abilityinfo.name.c_str());
                        continue;
                    }
                }
                if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_APPLICATION) ==
                    GET_ABILITY_INFO_WITH_APPLICATION) {
                    info.GetApplicationInfo(
                        ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, abilityinfo.applicationInfo);
                }
                if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_PERMISSION) !=
                    GET_ABILITY_INFO_WITH_PERMISSION) {
                    abilityinfo.permissions.clear();
                }
                if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_METADATA) != GET_ABILITY_INFO_WITH_METADATA) {
                    abilityinfo.metaData.customizeData.clear();
                    abilityinfo.metadata.clear();
                }
                abilityInfos.emplace_back(abilityinfo);
                break;
            }
        }
    }
}

void BundleDataMgr::GetMatchLauncherAbilityInfos(const Want& want,
    const InnerBundleInfo& info, std::vector<AbilityInfo>& abilityInfos, int32_t userId) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return;
    }

    int32_t responseUserId = info.GetResponseUserId(requestUserId);
    std::map<std::string, std::vector<Skill>> skillInfos = info.GetInnerSkillInfos();
    for (const auto& abilityInfoPair : info.GetInnerAbilityInfos()) {
        auto skillsPair = skillInfos.find(abilityInfoPair.first);
        if (skillsPair == skillInfos.end()) {
            continue;
        }
        for (const Skill& skill : skillsPair->second) {
            if (skill.MatchLauncher(want)) {
                AbilityInfo abilityinfo = abilityInfoPair.second;
                info.GetApplicationInfo(ApplicationFlag::GET_BASIC_APPLICATION_INFO,
                    responseUserId, abilityinfo.applicationInfo);
                abilityInfos.emplace_back(abilityinfo);
                break;
            }
        }
    }
}

bool BundleDataMgr::QueryLauncherAbilityInfos(
    const Want& want, uint32_t userId, std::vector<AbilityInfo>& abilityInfos) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ is empty");
        return false;
    }

    ElementName element = want.GetElement();
    std::string bundleName = element.GetBundleName();
    if (bundleName.empty()) {
        // query all launcher ability
        for (const auto &item : bundleInfos_) {
            const InnerBundleInfo &info = item.second;
            if (info.IsDisabled()) {
                APP_LOGI("app %{public}s is disabled", info.GetBundleName().c_str());
                continue;
            }
            GetMatchLauncherAbilityInfos(want, info, abilityInfos, requestUserId);
        }
        return true;
    } else {
        // query definite abilitys by bundle name
        auto item = bundleInfos_.find(bundleName);
        if (item == bundleInfos_.end()) {
            APP_LOGE("no bundleName %{public}s found", bundleName.c_str());
            return false;
        }
        GetMatchLauncherAbilityInfos(want, item->second, abilityInfos, requestUserId);
        FilterAbilityInfosByModuleName(element.GetModuleName(), abilityInfos);
        return true;
    }
}

bool BundleDataMgr::QueryAbilityInfoByUri(
    const std::string &abilityUri, int32_t userId, AbilityInfo &abilityInfo) const
{
    APP_LOGD("abilityUri is %{private}s", abilityUri.c_str());
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    if (abilityUri.empty()) {
        return false;
    }
    if (abilityUri.find(Constants::DATA_ABILITY_URI_PREFIX) == std::string::npos) {
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }
    std::string noPpefixUri = abilityUri.substr(Constants::DATA_ABILITY_URI_PREFIX.size());
    auto posFirstSeparator = noPpefixUri.find(Constants::DATA_ABILITY_URI_SEPARATOR);
    if (posFirstSeparator == std::string::npos) {
        return false;
    }
    auto posSecondSeparator = noPpefixUri.find(Constants::DATA_ABILITY_URI_SEPARATOR, posFirstSeparator + 1);
    std::string uri;
    if (posSecondSeparator == std::string::npos) {
        uri = noPpefixUri.substr(posFirstSeparator + 1, noPpefixUri.size() - posFirstSeparator - 1);
    } else {
        uri = noPpefixUri.substr(posFirstSeparator + 1, posSecondSeparator - posFirstSeparator - 1);
    }

    for (const auto &item : bundleInfos_) {
        const InnerBundleInfo &info = item.second;
        if (info.IsDisabled()) {
            APP_LOGE("app %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }

        int32_t responseUserId = info.GetResponseUserId(requestUserId);
        if (!info.GetApplicationEnabled(responseUserId)) {
            continue;
        }

        auto ability = info.FindAbilityInfoByUri(uri);
        if (!ability) {
            continue;
        }

        abilityInfo = (*ability);
        info.GetApplicationInfo(
            ApplicationFlag::GET_BASIC_APPLICATION_INFO, responseUserId, abilityInfo.applicationInfo);
        return true;
    }

    APP_LOGE("query abilityUri(%{private}s) failed.", abilityUri.c_str());
    return false;
}

bool BundleDataMgr::QueryAbilityInfosByUri(const std::string &abilityUri, std::vector<AbilityInfo> &abilityInfos)
{
    APP_LOGI("abilityUri is %{private}s", abilityUri.c_str());
    if (abilityUri.empty()) {
        return false;
    }
    if (abilityUri.find(Constants::DATA_ABILITY_URI_PREFIX) == std::string::npos) {
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    std::string noPpefixUri = abilityUri.substr(Constants::DATA_ABILITY_URI_PREFIX.size());
    auto posFirstSeparator = noPpefixUri.find(Constants::DATA_ABILITY_URI_SEPARATOR);
    if (posFirstSeparator == std::string::npos) {
        return false;
    }
    auto posSecondSeparator = noPpefixUri.find(Constants::DATA_ABILITY_URI_SEPARATOR, posFirstSeparator + 1);
    std::string uri;
    if (posSecondSeparator == std::string::npos) {
        uri = noPpefixUri.substr(posFirstSeparator + 1, noPpefixUri.size() - posFirstSeparator - 1);
    } else {
        uri = noPpefixUri.substr(posFirstSeparator + 1, posSecondSeparator - posFirstSeparator - 1);
    }

    for (auto &item : bundleInfos_) {
        InnerBundleInfo &info = item.second;
        if (info.IsDisabled()) {
            APP_LOGI("app %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }
        info.FindAbilityInfosByUri(uri, abilityInfos, GetUserId());
    }
    if (abilityInfos.size() == 0) {
        return false;
    }

    return true;
}

bool BundleDataMgr::GetApplicationInfo(
    const std::string &appName, int32_t flags, const int userId, ApplicationInfo &appInfo) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoWithFlags(appName, flags, innerBundleInfo, requestUserId)) {
        APP_LOGE("GetApplicationInfo failed");
        return false;
    }

    int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
    innerBundleInfo.GetApplicationInfo(flags, responseUserId, appInfo);
    return true;
}

bool BundleDataMgr::GetApplicationInfos(
    int32_t flags, const int userId, std::vector<ApplicationInfo> &appInfos) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    bool find = false;
    for (const auto &item : bundleInfos_) {
        const InnerBundleInfo &info = item.second;
        if (info.IsDisabled()) {
            APP_LOGE("app %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }
        int32_t responseUserId = info.GetResponseUserId(requestUserId);
        if (!(static_cast<uint32_t>(flags) & GET_APPLICATION_INFO_WITH_DISABLE)
            && !info.GetApplicationEnabled(responseUserId)) {
            APP_LOGD("bundleName: %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }
        ApplicationInfo appInfo;
        info.GetApplicationInfo(flags, responseUserId, appInfo);
        appInfos.emplace_back(appInfo);
        find = true;
    }
    APP_LOGD("get installed bundles success");
    return find;
}

bool BundleDataMgr::GetBundleInfo(
    const std::string &bundleName, int32_t flags, BundleInfo &bundleInfo, int32_t userId) const
{
    std::vector<InnerBundleUserInfo> innerBundleUserInfos;
    if (userId == Constants::ANY_USERID) {
        if (!GetInnerBundleUserInfos(bundleName, innerBundleUserInfos)) {
            APP_LOGE("no userInfos for this bundle(%{public}s)", bundleName.c_str());
            return false;
        }
        userId = innerBundleUserInfos.begin()->bundleUserInfo.userId;
    }

    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoWithFlags(bundleName, flags, innerBundleInfo, requestUserId)) {
        APP_LOGE("GetBundleInfo failed");
        return false;
    }

    int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
    innerBundleInfo.GetBundleInfo(flags, bundleInfo, responseUserId);
    APP_LOGD("get bundleInfo(%{public}s) successfully in user(%{public}d)", bundleName.c_str(), userId);
    return true;
}

bool BundleDataMgr::GetBundlePackInfo(
    const std::string &bundleName, int32_t flags, BundlePackInfo &bundlePackInfo) const
{
    APP_LOGD("Service BundleDataMgr GetBundlePackInfo start");
    int32_t requestUserId = GetUserIdByCallingUid();
    if (requestUserId == Constants::INVALID_USERID) {
        APP_LOGE("getBundlePackInfo userId is invalid");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoWithFlags(bundleName, flags, innerBundleInfo, requestUserId)) {
        APP_LOGE("GetBundlePackInfo failed");
        return false;
    }
    BundlePackInfo innerBundlePackInfo = innerBundleInfo.GetBundlePackInfo();
    if (static_cast<uint32_t>(flags) & GET_PACKAGES) {
        bundlePackInfo.packages = innerBundlePackInfo.packages;
        return true;
    }
    if (static_cast<uint32_t>(flags) & GET_BUNDLE_SUMMARY) {
        bundlePackInfo.summary.app = innerBundlePackInfo.summary.app;
        return true;
    }
    if (static_cast<uint32_t>(flags) & GET_MODULE_SUMMARY) {
        bundlePackInfo.summary.modules = innerBundlePackInfo.summary.modules;
        return true;
    }
    bundlePackInfo = innerBundlePackInfo;
    return true;
}

bool BundleDataMgr::GetBundleInfosByMetaData(
    const std::string &metaData, std::vector<BundleInfo> &bundleInfos) const
{
    if (metaData.empty()) {
        APP_LOGE("bundle name is empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    bool find = false;
    int32_t requestUserId = GetUserId();
    for (const auto &item : bundleInfos_) {
        const InnerBundleInfo &info = item.second;
        if (info.IsDisabled()) {
            APP_LOGW("app %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }
        if (info.CheckSpecialMetaData(metaData)) {
            BundleInfo bundleInfo;
            int32_t responseUserId = info.GetResponseUserId(requestUserId);
            info.GetBundleInfo(
                BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo, responseUserId);
            bundleInfos.emplace_back(bundleInfo);
            find = true;
        }
    }
    return find;
}

bool BundleDataMgr::GetBundleList(
    std::vector<std::string> &bundleNames, int32_t userId) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    bool find = false;
    for (const auto &infoItem : bundleInfos_) {
        InnerBundleInfo innerBundleInfo;
        if (!GetInnerBundleInfoWithFlags(infoItem.first, BundleFlag::GET_BUNDLE_DEFAULT,
            innerBundleInfo, requestUserId)) {
            continue;
        }

        bundleNames.emplace_back(infoItem.first);
        find = true;
    }
    APP_LOGD("user(%{public}d) get installed bundles list result(%{public}d)", userId, find);
    return find;
}

bool BundleDataMgr::GetBundleInfos(
    int32_t flags, std::vector<BundleInfo> &bundleInfos, int32_t userId) const
{
    if (userId == Constants::ALL_USERID) {
        return GetAllBundleInfos(flags, bundleInfos);
    }

    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    bool find = false;
    for (const auto &item : bundleInfos_) {
        InnerBundleInfo innerBundleInfo;
        if (!GetInnerBundleInfoWithFlags(
            item.first, flags, innerBundleInfo, requestUserId)) {
            continue;
        }

        BundleInfo bundleInfo;
        int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
        innerBundleInfo.GetBundleInfo(flags, bundleInfo, responseUserId);
        bundleInfos.emplace_back(bundleInfo);
        find = true;
    }
    APP_LOGD("get bundleInfos result(%{public}d) in user(%{public}d).", find, userId);
    return find;
}

bool BundleDataMgr::GetAllBundleInfos(int32_t flags, std::vector<BundleInfo> &bundleInfos) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    bool find = false;
    for (const auto &item : bundleInfos_) {
        const InnerBundleInfo &info = item.second;
        if (info.IsDisabled()) {
            APP_LOGD("app %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }
        BundleInfo bundleInfo;
        info.GetBundleInfo(flags, bundleInfo, Constants::ALL_USERID);
        bundleInfos.emplace_back(bundleInfo);
        find = true;
    }

    APP_LOGD("get all bundleInfos result(%{public}d).", find);
    return find;
}

bool BundleDataMgr::GetBundleNameForUid(const int uid, std::string &bundleName) const
{
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoByUid(uid, innerBundleInfo)) {
        APP_LOGE("get innerBundleInfo by uid failed.");
        return false;
    }

    bundleName = innerBundleInfo.GetBundleName();
    return true;
}

bool BundleDataMgr::GetInnerBundleInfoByUid(const int uid, InnerBundleInfo &innerBundleInfo) const
{
    int32_t userId = GetUserIdByUid(uid);
    if (userId == Constants::UNSPECIFIED_USERID || userId == Constants::INVALID_USERID) {
        APP_LOGE("the uid %{public}d is illegal when get bundleName by uid.", uid);
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    for (const auto &item : bundleInfos_) {
        const InnerBundleInfo &info = item.second;
        if (info.IsDisabled()) {
            APP_LOGW("app %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }
        if (info.GetUid(userId) == uid) {
            innerBundleInfo = info;
            return true;
        }
    }

    APP_LOGD("the uid(%{public}d) is not exists.", uid);
    return false;
}

#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
int64_t BundleDataMgr::GetBundleSpaceSize(const std::string &bundleName) const
{
    int32_t userId = AccountHelper::GetCurrentActiveUserId();
    int64_t curSize = 0;
    int64_t spaceSize = 0;
    BundleInfo bundleInfo;

    if (userId != Constants::INVALID_USERID
        && GetBundleInfo(bundleName, GET_ALL_APPLICATION_INFO, bundleInfo, userId) == true) {
        if (!bundleInfo.applicationInfo.codePath.empty()) {
            curSize = InstalldOperator::GetDiskUsage(bundleInfo.applicationInfo.codePath);
            spaceSize += curSize;
            APP_LOGI("Code %{public}s:%{public}" PRId64, bundleInfo.applicationInfo.codePath.c_str(), curSize);
        }
        if (!bundleInfo.applicationInfo.dataDir.empty()) {
            curSize = InstalldOperator::GetDiskUsage(bundleInfo.applicationInfo.dataDir);
            spaceSize += curSize;
            APP_LOGI("Data %{public}s:%{public}" PRId64, bundleInfo.applicationInfo.dataDir.c_str(), curSize);
        }
    }

    APP_LOGI("%{public}s spaceSize:%{public}" PRId64, bundleName.c_str(), spaceSize);
    return spaceSize;
}

int64_t BundleDataMgr::GetAllFreeInstallBundleSpaceSize() const
{
    int32_t userId = AccountHelper::GetCurrentActiveUserId();
    int64_t allSize = 0;
    int64_t curSize = 0;
    std::vector<BundleInfo> bundleInfos;

    if (userId != Constants::INVALID_USERID
        && GetBundleInfos(GET_ALL_APPLICATION_INFO, bundleInfos, userId) == true) {
        for (const auto &item : bundleInfos) {
            APP_LOGI("%{public}s freeInstall:%{public}d", item.name.c_str(), item.applicationInfo.isFreeInstallApp);
            if (item.applicationInfo.isFreeInstallApp) {
                if (!item.applicationInfo.codePath.empty()) {
                    curSize = InstalldOperator::GetDiskUsage(item.applicationInfo.codePath);
                    allSize += curSize;
                    APP_LOGI("Code %{public}s:%{public}" PRId64, item.applicationInfo.codePath.c_str(), curSize);
                }
                if (!item.applicationInfo.dataDir.empty()) {
                    curSize = InstalldOperator::GetDiskUsage(item.applicationInfo.dataDir);
                    allSize += curSize;
                    APP_LOGI("Data %{public}s:%{public}" PRId64, item.applicationInfo.dataDir.c_str(), curSize);
                }
            }
        }
    }

    APP_LOGI("All sfreeInstall:%{public}" PRId64, allSize);
    return allSize;
}
#endif

bool BundleDataMgr::GetBundlesForUid(const int uid, std::vector<std::string> &bundleNames) const
{
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoByUid(uid, innerBundleInfo)) {
        APP_LOGE("get innerBundleInfo by uid failed.");
        return false;
    }

    bundleNames.emplace_back(innerBundleInfo.GetBundleName());
    return true;
}

bool BundleDataMgr::GetNameForUid(const int uid, std::string &name) const
{
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoByUid(uid, innerBundleInfo)) {
        APP_LOGE("get innerBundleInfo by uid failed.");
        return false;
    }

    name = innerBundleInfo.GetBundleName();
    return true;
}

bool BundleDataMgr::GetBundleGids(const std::string &bundleName, std::vector<int> &gids) const
{
    int32_t requestUserId = GetUserId();
    InnerBundleUserInfo innerBundleUserInfo;
    if (!GetInnerBundleUserInfoByUserId(bundleName, requestUserId, innerBundleUserInfo)) {
        APP_LOGE("the user(%{public}d) is not exists in bundleName(%{public}s) .",
            requestUserId, bundleName.c_str());
        return false;
    }

    gids = innerBundleUserInfo.gids;
    return true;
}

bool BundleDataMgr::GetBundleGidsByUid(
    const std::string &bundleName, const int &uid, std::vector<int> &gids) const
{
    return true;
}

bool BundleDataMgr::QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    int32_t requestUserId = GetUserId();
    for (const auto &info : bundleInfos_) {
        if (info.second.IsDisabled()) {
            APP_LOGW("app %{public}s is disabled", info.second.GetBundleName().c_str());
            continue;
        }
        if (info.second.GetIsKeepAlive()) {
            BundleInfo bundleInfo;
            int32_t responseUserId = info.second.GetResponseUserId(requestUserId);
            info.second.GetBundleInfo(BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo, responseUserId);
            if (bundleInfo.name == "") {
                continue;
            }
            bundleInfos.emplace_back(bundleInfo);
        }
    }
    return !(bundleInfos.empty());
}

std::string BundleDataMgr::GetAbilityLabel(const std::string &bundleName, const std::string &moduleName,
    const std::string &abilityName) const
{
#ifdef GLOBAL_RESMGR_ENABLE
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGW("bundleInfos_ data is empty");
        return Constants::EMPTY_STRING;
    }
    APP_LOGI("GetAbilityLabel %{public}s", bundleName.c_str());
    auto item = bundleInfos_.find(bundleName);
    if (item == bundleInfos_.end()) {
        return Constants::EMPTY_STRING;
    }
    const InnerBundleInfo &innerBundleInfo = item->second;
    if (innerBundleInfo.IsDisabled()) {
        APP_LOGW("app %{public}s is disabled", innerBundleInfo.GetBundleName().c_str());
        return Constants::EMPTY_STRING;
    }
    auto ability = innerBundleInfo.FindAbilityInfo(bundleName, moduleName, abilityName, GetUserId());
    if (!ability) {
        return Constants::EMPTY_STRING;
    }
    if ((*ability).labelId == 0) {
        return (*ability).label;
    }
    std::string abilityLabel;
    BundleInfo bundleInfo;
    innerBundleInfo.GetBundleInfo(0, bundleInfo, GetUserIdByCallingUid());
    std::shared_ptr<OHOS::Global::Resource::ResourceManager> resourceManager = GetResourceManager(bundleInfo);
    if (resourceManager == nullptr) {
        APP_LOGE("InitResourceManager failed");
        return Constants::EMPTY_STRING;
    }
    OHOS::Global::Resource::RState errval =
        resourceManager->GetStringById(static_cast<uint32_t>((*ability).labelId), abilityLabel);
    if (errval != OHOS::Global::Resource::RState::SUCCESS) {
        return Constants::EMPTY_STRING;
    } else {
        return abilityLabel;
    }
#else
    APP_LOGW("GLOBAL_RESMGR_ENABLE is false");
    return Constants::EMPTY_STRING;
#endif
}

bool BundleDataMgr::GetHapModuleInfo(
    const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo, int32_t userId) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    APP_LOGD("GetHapModuleInfo %{public}s", abilityInfo.bundleName.c_str());
    auto infoItem = bundleInfos_.find(abilityInfo.bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }

    const InnerBundleInfo &innerBundleInfo = infoItem->second;
    if (innerBundleInfo.IsDisabled()) {
        APP_LOGE("app %{public}s is disabled", innerBundleInfo.GetBundleName().c_str());
        return false;
    }

    int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
    auto module = innerBundleInfo.FindHapModuleInfo(abilityInfo.package, responseUserId);
    if (!module) {
        APP_LOGE("can not find module %{public}s", abilityInfo.package.c_str());
        return false;
    }
    hapModuleInfo = *module;
    return true;
}

bool BundleDataMgr::GetLaunchWantForBundle(const std::string &bundleName, Want &want) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    InnerBundleInfo innerBundleInfo;

    if (!GetInnerBundleInfoWithFlags(bundleName, BundleFlag::GET_BUNDLE_DEFAULT,
        innerBundleInfo, GetUserIdByCallingUid())) {
        APP_LOGE("GetLaunchWantForBundle failed");
        return false;
    }
    std::string mainAbility = innerBundleInfo.GetMainAbility();
    if (mainAbility.empty()) {
        APP_LOGE("no main ability in the bundle %{public}s", bundleName.c_str());
        return false;
    }
    want.SetElementName("", bundleName, mainAbility);
    want.SetAction(Constants::INTENT_ACTION_HOME);
    want.AddEntity(Constants::INTENT_ENTITY_HOME);
    return true;
}

bool BundleDataMgr::CheckIsSystemAppByUid(const int uid) const
{
    // If the value of uid is 0 (ROOT_UID) or 1000 (BMS_UID),
    // the uid should be the system uid.
    if (uid == Constants::ROOT_UID || uid == Constants::BMS_UID) {
        return true;
    }

    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoByUid(uid, innerBundleInfo)) {
        return false;
    }

    return innerBundleInfo.IsSystemApp();
}

void BundleDataMgr::InitStateTransferMap()
{
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::INSTALL_START);
    transferStates_.emplace(InstallState::INSTALL_FAIL, InstallState::INSTALL_START);
    transferStates_.emplace(InstallState::UNINSTALL_START, InstallState::INSTALL_SUCCESS);
    transferStates_.emplace(InstallState::UNINSTALL_START, InstallState::INSTALL_START);
    transferStates_.emplace(InstallState::UNINSTALL_START, InstallState::UPDATING_SUCCESS);
    transferStates_.emplace(InstallState::UNINSTALL_FAIL, InstallState::UNINSTALL_START);
    transferStates_.emplace(InstallState::UNINSTALL_SUCCESS, InstallState::UNINSTALL_START);
    transferStates_.emplace(InstallState::UPDATING_START, InstallState::INSTALL_SUCCESS);
    transferStates_.emplace(InstallState::UPDATING_SUCCESS, InstallState::UPDATING_START);
    transferStates_.emplace(InstallState::UPDATING_FAIL, InstallState::UPDATING_START);
    transferStates_.emplace(InstallState::UPDATING_FAIL, InstallState::INSTALL_START);
    transferStates_.emplace(InstallState::UPDATING_START, InstallState::INSTALL_START);
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::UPDATING_START);
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::UPDATING_SUCCESS);
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::UNINSTALL_START);
    transferStates_.emplace(InstallState::UPDATING_START, InstallState::UPDATING_SUCCESS);
    transferStates_.emplace(InstallState::ROLL_BACK, InstallState::UPDATING_START);
    transferStates_.emplace(InstallState::ROLL_BACK, InstallState::UPDATING_SUCCESS);
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::ROLL_BACK);
    transferStates_.emplace(InstallState::UNINSTALL_START, InstallState::USER_CHANGE);
    transferStates_.emplace(InstallState::UPDATING_START, InstallState::USER_CHANGE);
    transferStates_.emplace(InstallState::INSTALL_SUCCESS, InstallState::USER_CHANGE);
    transferStates_.emplace(InstallState::UPDATING_SUCCESS, InstallState::USER_CHANGE);
    transferStates_.emplace(InstallState::USER_CHANGE, InstallState::INSTALL_SUCCESS);
    transferStates_.emplace(InstallState::USER_CHANGE, InstallState::UPDATING_SUCCESS);
    transferStates_.emplace(InstallState::USER_CHANGE, InstallState::UPDATING_START);
}

bool BundleDataMgr::IsDeleteDataState(const InstallState state) const
{
    return (state == InstallState::INSTALL_FAIL || state == InstallState::UNINSTALL_FAIL ||
            state == InstallState::UNINSTALL_SUCCESS || state == InstallState::UPDATING_FAIL);
}

bool BundleDataMgr::IsDisableState(const InstallState state) const
{
    if (state == InstallState::UPDATING_START || state == InstallState::UNINSTALL_START) {
        return true;
    }
    return false;
}

void BundleDataMgr::DeleteBundleInfo(const std::string &bundleName, const InstallState state)
{
    if (InstallState::INSTALL_FAIL == state) {
        APP_LOGW("del fail, bundle:%{public}s has no installed info", bundleName.c_str());
        return;
    }

    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem != bundleInfos_.end()) {
        APP_LOGD("del bundle name:%{public}s", bundleName.c_str());
        const InnerBundleInfo &innerBundleInfo = infoItem->second;
        RecycleUidAndGid(innerBundleInfo);
        bool ret = dataStorage_->DeleteStorageBundleInfo(innerBundleInfo);
        if (!ret) {
            APP_LOGW("delete storage error name:%{public}s", bundleName.c_str());
        }
        bundleInfos_.erase(bundleName);
    }
}

bool BundleDataMgr::IsAppOrAbilityInstalled(const std::string &bundleName) const
{
    if (bundleName.empty()) {
        APP_LOGW("name:%{public}s empty", bundleName.c_str());
        return false;
    }

    std::lock_guard<std::mutex> lock(stateMutex_);
    auto statusItem = installStates_.find(bundleName);
    if (statusItem == installStates_.end()) {
        APP_LOGW("name:%{public}s not find", bundleName.c_str());
        return false;
    }

    if (statusItem->second == InstallState::INSTALL_SUCCESS) {
        return true;
    }

    APP_LOGW("name:%{public}s not install success", bundleName.c_str());
    return false;
}

bool BundleDataMgr::GetInnerBundleInfoWithFlags(const std::string &bundleName,
    const int32_t flags, InnerBundleInfo &info, int32_t userId) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }
    APP_LOGD("GetInnerBundleInfoWithFlags: %{public}s", bundleName.c_str());
    auto item = bundleInfos_.find(bundleName);
    if (item == bundleInfos_.end()) {
        APP_LOGE("GetInnerBundleInfoWithFlags: bundleName not find");
        return false;
    }
    const InnerBundleInfo &innerBundleInfo = item->second;
    if (innerBundleInfo.IsDisabled()) {
        APP_LOGE("bundleName: %{public}s status is disabled", innerBundleInfo.GetBundleName().c_str());
        return false;
    }

    int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
    if (!(static_cast<uint32_t>(flags) & GET_APPLICATION_INFO_WITH_DISABLE)
        && !innerBundleInfo.GetApplicationEnabled(responseUserId)) {
        APP_LOGE("bundleName: %{public}s is disabled", innerBundleInfo.GetBundleName().c_str());
        return false;
    }
    info = innerBundleInfo;
    return true;
}

bool BundleDataMgr::GetInnerBundleInfo(const std::string &bundleName, InnerBundleInfo &info)
{
    APP_LOGD("GetInnerBundleInfo %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName is empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    infoItem->second.SetBundleStatus(InnerBundleInfo::BundleStatus::DISABLED);
    info = infoItem->second;
    return true;
}

bool BundleDataMgr::DisableBundle(const std::string &bundleName)
{
    APP_LOGD("DisableBundle %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    infoItem->second.SetBundleStatus(InnerBundleInfo::BundleStatus::DISABLED);
    return true;
}

bool BundleDataMgr::EnableBundle(const std::string &bundleName)
{
    APP_LOGD("EnableBundle %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    infoItem->second.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
    return true;
}

bool BundleDataMgr::IsApplicationEnabled(const std::string &bundleName) const
{
    APP_LOGD("IsApplicationEnabled %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    int32_t responseUserId = infoItem->second.GetResponseUserId(GetUserId());
    return infoItem->second.GetApplicationEnabled(responseUserId);
}

bool BundleDataMgr::SetApplicationEnabled(const std::string &bundleName, bool isEnable, int32_t userId)
{
    APP_LOGD("SetApplicationEnabled %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }

    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        APP_LOGE("Request userId is invalid");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }

    InnerBundleInfo& newInfo = infoItem->second;
    newInfo.SetApplicationEnabled(isEnable, requestUserId);
    InnerBundleUserInfo innerBundleUserInfo;
    if (!newInfo.GetInnerBundleUserInfo(requestUserId, innerBundleUserInfo)) {
        APP_LOGE("can not find request userId %{public}d when get userInfo", requestUserId);
        return false;
    }

    if (innerBundleUserInfo.bundleUserInfo.IsInitialState()) {
        bundleStateStorage_->DeleteBundleState(bundleName, requestUserId);
    } else {
        bundleStateStorage_->SaveBundleStateStorage(
            bundleName, requestUserId, innerBundleUserInfo.bundleUserInfo);
    }

    return true;
}

bool BundleDataMgr::SetModuleRemovable(const std::string &bundleName, const std::string &moduleName, bool isEnable)
{
    if (bundleName.empty() || moduleName.empty()) {
        APP_LOGE("bundleName or moduleName is empty");
        return false;
    }
    int32_t userId = AccountHelper::GetCurrentActiveUserId();
    if (userId == Constants::INVALID_USERID) {
        APP_LOGE("get a invalid userid");
        return false;
    }
    APP_LOGD("bundleName:%{public}s, moduleName:%{public}s, userId:%{public}d",
        bundleName.c_str(), moduleName.c_str(), userId);
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    InnerBundleInfo newInfo = infoItem->second;
    bool ret = newInfo.SetModuleRemovable(moduleName, isEnable, userId);
    if (ret && dataStorage_->SaveStorageBundleInfo(newInfo)) {
        ret = infoItem->second.SetModuleRemovable(moduleName, isEnable, userId);
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
        if (isEnable) {
            // call clean task
            APP_LOGD("bundle:%{public}s isEnable:%{public}d ret:%{public}d call clean task",
                bundleName.c_str(), isEnable, ret);
            DelayedSingleton<BundleMgrService>::GetInstance()->GetAgingMgr()->Start(
                BundleAgingMgr::AgingTriggertype::UPDATE_REMOVABLE_FLAG);
        }
#endif
        return ret;
    } else {
        APP_LOGE("bundle:%{public}s SetModuleRemoved failed", bundleName.c_str());
        return false;
    }
}

bool BundleDataMgr::IsModuleRemovable(const std::string &bundleName, const std::string &moduleName) const
{
    if (bundleName.empty() || moduleName.empty()) {
        APP_LOGE("bundleName or moduleName is empty");
        return false;
    }
    int32_t userId = AccountHelper::GetCurrentActiveUserId();
    if (userId == Constants::INVALID_USERID) {
        APP_LOGE("get a invalid userid");
        return false;
    }
    APP_LOGD("bundleName:%{public}s, moduleName:%{public}s, userId:%{public}d",
        bundleName.c_str(), moduleName.c_str(), userId);
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    InnerBundleInfo newInfo = infoItem->second;
    return newInfo.IsModuleRemovable(moduleName, userId);
}


bool BundleDataMgr::IsAbilityEnabled(const AbilityInfo &abilityInfo) const
{
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    AbilityInfo abilityInfoIn;
    return ExplicitQueryAbilityInfo(abilityInfo.bundleName, abilityInfo.moduleName, abilityInfo.name,
        flags, GetUserId(), abilityInfoIn);
}

bool BundleDataMgr::SetAbilityEnabled(const AbilityInfo &abilityInfo, bool isEnabled, int32_t userId)
{
    APP_LOGD("SetAbilityEnabled %{public}s", abilityInfo.name.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }

    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        APP_LOGE("Request userId is invalid");
        return false;
    }

    auto infoItem = bundleInfos_.find(abilityInfo.bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", abilityInfo.bundleName.c_str());
        return false;
    }

    InnerBundleInfo& newInfo = infoItem->second;
    if (!newInfo.SetAbilityEnabled(
        abilityInfo.bundleName, abilityInfo.moduleName, abilityInfo.name, isEnabled, requestUserId)) {
        APP_LOGE("SetAbilityEnabled %{public}s failed", abilityInfo.bundleName.c_str());
        return false;
    }

    InnerBundleUserInfo innerBundleUserInfo;
    if (!newInfo.GetInnerBundleUserInfo(requestUserId, innerBundleUserInfo)) {
        APP_LOGE("can not find request userId %{public}d when get userInfo", requestUserId);
        return false;
    }

    if (innerBundleUserInfo.bundleUserInfo.IsInitialState()) {
        bundleStateStorage_->DeleteBundleState(abilityInfo.bundleName, requestUserId);
    } else {
        bundleStateStorage_->SaveBundleStateStorage(
            abilityInfo.bundleName, requestUserId, innerBundleUserInfo.bundleUserInfo);
    }
    
    return true;
}

#ifdef BUNDLE_FRAMEWORK_GRAPHICS
std::shared_ptr<Media::PixelMap> BundleDataMgr::GetAbilityPixelMapIcon(const std::string &bundleName,
    const std::string &moduleName, const std::string &abilityName) const
{
#ifdef GLOBAL_RESMGR_ENABLE
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGW("bundleInfos_ data is empty");
        return nullptr;
    }
    APP_LOGD("GetAbilityPixelMapIcon %{public}s", bundleName.c_str());
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return nullptr;
    }
    auto ability = infoItem->second.FindAbilityInfo(bundleName, moduleName, abilityName, GetUserId());
    if (!ability) {
        APP_LOGE("abilityName:%{public}s not find", abilityName.c_str());
        return nullptr;
    }
    BundleInfo bundleInfo;
    int32_t flags = 0;
    infoItem->second.GetBundleInfo(flags, bundleInfo, GetUserId());
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager = GetResourceManager(bundleInfo);
    if (resourceManager == nullptr) {
        APP_LOGE("InitResourceManager failed");
        return nullptr;
    }
    std::string iconPath;
    OHOS::Global::Resource::RState iconPathErrval =
        resourceManager->GetMediaById(static_cast<uint32_t>((*ability).iconId), iconPath);
    if (iconPathErrval != OHOS::Global::Resource::RState::SUCCESS) {
        APP_LOGE("GetMediaById iconPath failed");
        return nullptr;
    }
    APP_LOGD("GetMediaById iconPath: %{private}s", iconPath.c_str());
    auto pixelMapPtr = LoadImageFile(iconPath);
    if (!pixelMapPtr) {
        APP_LOGE("LoadImageFile failed");
        return nullptr;
    }
    return pixelMapPtr;
#else
    APP_LOGW("GLOBAL_RESMGR_ENABLE is false");
    return nullptr;
#endif
}
#endif

std::shared_ptr<BundleSandboxDataMgr> BundleDataMgr::GetSandboxDataMgr() const
{
    return sandboxDataMgr_;
}
bool BundleDataMgr::RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    APP_LOGD("RegisterBundleStatusCallback %{public}s", bundleStatusCallback->GetBundleName().c_str());
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbackList_.emplace_back(bundleStatusCallback);
    if (bundleStatusCallback->AsObject() != nullptr) {
        sptr<BundleStatusCallbackDeathRecipient> deathRecipient =
            new (std::nothrow) BundleStatusCallbackDeathRecipient();
        if (deathRecipient == nullptr) {
            APP_LOGE("deathRecipient is null");
            return false;
        }
        bundleStatusCallback->AsObject()->AddDeathRecipient(deathRecipient);
    }
    return true;
}

bool BundleDataMgr::ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    APP_LOGD("ClearBundleStatusCallback %{public}s", bundleStatusCallback->GetBundleName().c_str());
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbackList_.erase(std::remove_if(callbackList_.begin(),
        callbackList_.end(),
        [&](const sptr<IBundleStatusCallback> &callback) {
            return callback->AsObject() == bundleStatusCallback->AsObject();
        }),
        callbackList_.end());
    return true;
}

bool BundleDataMgr::UnregisterBundleStatusCallback()
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbackList_.clear();
    return true;
}

bool BundleDataMgr::GenerateUidAndGid(InnerBundleUserInfo &innerBundleUserInfo)
{
    if (innerBundleUserInfo.bundleName.empty()) {
        APP_LOGE("bundleName is null.");
        return false;
    }

    int32_t bundleId = Constants::INVALID_BUNDLEID;
    if (!GenerateBundleId(innerBundleUserInfo.bundleName, bundleId)) {
        APP_LOGE("Generate bundleId failed.");
        return false;
    }

    innerBundleUserInfo.uid = innerBundleUserInfo.bundleUserInfo.userId * Constants::BASE_USER_RANGE
        + bundleId % Constants::BASE_USER_RANGE;
    innerBundleUserInfo.gids.emplace_back(innerBundleUserInfo.uid);
    return true;
}

bool BundleDataMgr::GenerateBundleId(const std::string &bundleName, int32_t &bundleId)
{
    std::lock_guard<std::mutex> lock(bundleIdMapMutex_);
    if (bundleIdMap_.empty()) {
        APP_LOGI("first app install");
        bundleId = Constants::BASE_APP_UID;
        bundleIdMap_.emplace(bundleId, bundleName);
        return true;
    }

    for (const auto &innerBundleId : bundleIdMap_) {
        if (innerBundleId.second == bundleName) {
            bundleId = innerBundleId.first;
            return true;
        }
    }

    for (int32_t i = Constants::BASE_APP_UID; i < bundleIdMap_.rbegin()->first; ++i) {
        if (bundleIdMap_.find(i) == bundleIdMap_.end()) {
            APP_LOGI("the %{public}d app install", i);
            bundleId = i;
            bundleIdMap_.emplace(bundleId, bundleName);
            BundleUtil::MakeHmdfsConfig(bundleName, bundleId);
            return true;
        }
    }

    if (bundleIdMap_.rbegin()->first == Constants::MAX_APP_UID) {
        APP_LOGE("the bundleId exceeding the maximum value.");
        return false;
    }

    bundleId = bundleIdMap_.rbegin()->first + 1;
    bundleIdMap_.emplace(bundleId, bundleName);
    BundleUtil::MakeHmdfsConfig(bundleName, bundleId);
    return true;
}

bool BundleDataMgr::SetModuleUpgradeFlag(const std::string &bundleName,
    const std::string &moduleName, const int32_t upgradeFlag)
{
    APP_LOGD("SetModuleUpgradeFlag %{public}d", upgradeFlag);
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    InnerBundleInfo newInfo = infoItem->second;
    newInfo.SetModuleUpgradeFlag(moduleName, upgradeFlag);
    if (dataStorage_->SaveStorageBundleInfo(newInfo)) {
        return infoItem->second.SetModuleUpgradeFlag(moduleName, upgradeFlag);
    }
    APP_LOGD("dataStorage SetModuleUpgradeFlag %{public}s failed", bundleName.c_str());
    return false;
}

int32_t BundleDataMgr::GetModuleUpgradeFlag(const std::string &bundleName, const std::string &moduleName) const
{
    APP_LOGD("bundleName is bundleName:%{public}s, moduleName:%{public}s", bundleName.c_str(), moduleName.c_str());
    if (bundleName.empty() || moduleName.empty()) {
        APP_LOGE("bundleName or moduleName is empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    InnerBundleInfo newInfo = infoItem->second;
    return newInfo.GetModuleUpgradeFlag(moduleName);
}

void BundleDataMgr::StoreSandboxPersistentInfo(const std::string &bundleName, const SandboxAppPersistentInfo &info)
{
    if (bundleName.empty()) {
        APP_LOGW("StoreSandboxPersistentInfo bundleName is empty");
        return;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.find(bundleName) == bundleInfos_.end()) {
        APP_LOGW("can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    bundleInfos_[bundleName].AddSandboxPersistentInfo(info);
    SaveInnerBundleInfo(bundleInfos_[bundleName]);
}

void BundleDataMgr::DeleteSandboxPersistentInfo(const std::string &bundleName, const SandboxAppPersistentInfo &info)
{
    if (bundleName.empty()) {
        APP_LOGW("DeleteSandboxPersistentInfo bundleName is empty");
        return;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.find(bundleName) == bundleInfos_.end()) {
        APP_LOGW("can not find bundle %{public}s", bundleName.c_str());
        return;
    }
    bundleInfos_[bundleName].RemoveSandboxPersistentInfo(info);
    SaveInnerBundleInfo(bundleInfos_[bundleName]);
}

void BundleDataMgr::RecycleUidAndGid(const InnerBundleInfo &info)
{
    auto userInfos = info.GetInnerBundleUserInfos();
    if (userInfos.empty()) {
        return;
    }

    auto innerBundleUserInfo = userInfos.begin()->second;
    int32_t bundleId = innerBundleUserInfo.uid -
        innerBundleUserInfo.bundleUserInfo.userId * Constants::BASE_USER_RANGE;
    std::lock_guard<std::mutex> lock(bundleIdMapMutex_);
    auto infoItem = bundleIdMap_.find(bundleId);
    if (infoItem == bundleIdMap_.end()) {
        return;
    }

    bundleIdMap_.erase(bundleId);
    BundleUtil::RemoveHmdfsConfig(innerBundleUserInfo.bundleName);
}

bool BundleDataMgr::GenerateCloneUid(InnerBundleInfo &info)
{
    return true;
}

bool BundleDataMgr::RestoreUidAndGid()
{
    for (const auto &info : bundleInfos_) {
        bool onlyInsertOne = false;
        for (auto infoItem : info.second.GetInnerBundleUserInfos()) {
            auto innerBundleUserInfo = infoItem.second;
            AddUserId(innerBundleUserInfo.bundleUserInfo.userId);
            if (!onlyInsertOne) {
                onlyInsertOne = true;
                int32_t bundleId = innerBundleUserInfo.uid -
                    innerBundleUserInfo.bundleUserInfo.userId * Constants::BASE_USER_RANGE;
                std::lock_guard<std::mutex> lock(bundleIdMapMutex_);
                auto infoItem = bundleIdMap_.find(bundleId);
                if (infoItem == bundleIdMap_.end()) {
                    bundleIdMap_.emplace(bundleId, innerBundleUserInfo.bundleName);
                } else {
                    bundleIdMap_[bundleId] = innerBundleUserInfo.bundleName;
                }
                BundleUtil::MakeHmdfsConfig(innerBundleUserInfo.bundleName, bundleId);
            }
        }
    }
    return true;
}

bool BundleDataMgr::NotifyBundleStatus(const std::string& bundleName, const std::string& modulePackage,
    const std::string& abilityName, const ErrCode resultCode, const NotifyType type, const int32_t& uid)
{
    APP_LOGD("notify type %{public}d with %{public}d for %{public}s-%{public}s in %{public}s", type, resultCode,
        modulePackage.c_str(), abilityName.c_str(), bundleName.c_str());
    uint8_t installType = [&]() -> uint8_t {
        if ((type == NotifyType::UNINSTALL_BUNDLE) || (type == NotifyType::UNINSTALL_MODULE)) {
            return static_cast<uint8_t>(InstallType::UNINSTALL_CALLBACK);
        }
        return static_cast<uint8_t>(InstallType::INSTALL_CALLBACK);
    }();
    {
        std::lock_guard<std::mutex> lock(callbackMutex_);
        for (const auto& callback : callbackList_) {
            if (callback->GetBundleName() == bundleName) {
                // if the msg needed, it could convert in the proxy node
                callback->OnBundleStateChanged(installType, resultCode, Constants::EMPTY_STRING, bundleName);
            }
        }
    }

    if (resultCode != ERR_OK) {
        return true;
    }
    std::string eventData = [type]() -> std::string {
        switch (type) {
            case NotifyType::INSTALL:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED;
            case NotifyType::UNINSTALL_BUNDLE:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED;
            case NotifyType::UNINSTALL_MODULE:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED;
            case NotifyType::UPDATE:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED;
            case NotifyType::ABILITY_ENABLE:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED;
            case NotifyType::APPLICATION_ENABLE:
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED;
            default:
                APP_LOGE("event type error");
                return EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED;
        }
    }();
    APP_LOGD("will send event data %{public}s", eventData.c_str());
    Want want;
    want.SetAction(eventData);
    ElementName element;
    element.SetBundleName(bundleName);
    element.SetAbilityName(abilityName);
    want.SetElement(element);
    want.SetParam(Constants::UID, uid);
    want.SetParam(Constants::USER_ID, GetUserIdByUid(uid));
    want.SetParam(Constants::ABILTY_NAME.data(), abilityName);
    EventFwk::CommonEventData commonData { want };
    EventFwk::CommonEventManager::PublishCommonEvent(commonData);
    return true;
}

std::mutex &BundleDataMgr::GetBundleMutex(const std::string &bundleName)
{
    bundleMutex_.lock_shared();
    auto it = bundleMutexMap_.find(bundleName);
    if (it == bundleMutexMap_.end()) {
        bundleMutex_.unlock_shared();
        std::unique_lock lock {bundleMutex_};
        return bundleMutexMap_[bundleName];
    }
    bundleMutex_.unlock_shared();
    return it->second;
}

bool BundleDataMgr::GetProvisionId(const std::string &bundleName, std::string &provisionId) const
{
    APP_LOGD("GetProvisionId %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    provisionId = infoItem->second.GetProvisionId();
    return true;
}

bool BundleDataMgr::GetAppFeature(const std::string &bundleName, std::string &appFeature) const
{
    APP_LOGD("GetAppFeature %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundleName empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", bundleName.c_str());
        return false;
    }
    appFeature = infoItem->second.GetAppFeature();
    return true;
}

void BundleDataMgr::SetInitialUserFlag(bool flag)
{
    APP_LOGD("SetInitialUserFlag %{public}d", flag);
    if (!initialUserFlag_ && flag && bundlePromise_ != nullptr) {
        bundlePromise_->NotifyAllTasksExecuteFinished();
    }

    initialUserFlag_ = flag;
}

int BundleDataMgr::CheckPublicKeys(const std::string &firstBundleName, const std::string &secondBundleName) const
{
    APP_LOGD("CheckPublicKeys %{public}s and %{public}s", firstBundleName.c_str(), secondBundleName.c_str());
    if (firstBundleName.empty() || secondBundleName.empty()) {
        APP_LOGE("bundleName empty");
        return Constants::SIGNATURE_UNKNOWN_BUNDLE;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto firstInfoItem = bundleInfos_.find(firstBundleName);
    if (firstInfoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", firstBundleName.c_str());
        return Constants::SIGNATURE_UNKNOWN_BUNDLE;
    }
    auto secondInfoItem = bundleInfos_.find(secondBundleName);
    if (secondInfoItem == bundleInfos_.end()) {
        APP_LOGE("can not find bundle %{public}s", secondBundleName.c_str());
        return Constants::SIGNATURE_UNKNOWN_BUNDLE;
    }
    auto firstProvisionId = firstInfoItem->second.GetProvisionId();
    auto secondProvisionId = secondInfoItem->second.GetProvisionId();
    return (firstProvisionId == secondProvisionId) ? Constants::SIGNATURE_MATCHED : Constants::SIGNATURE_NOT_MATCHED;
}

std::shared_ptr<IBundleDataStorage> BundleDataMgr::GetDataStorage() const
{
    return dataStorage_;
}

bool BundleDataMgr::GetAllFormsInfo(std::vector<FormInfo> &formInfos) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }
    auto result = false;
    for (const auto &item : bundleInfos_) {
        if (item.second.IsDisabled()) {
            APP_LOGW("app %{public}s is disabled", item.second.GetBundleName().c_str());
            continue;
        }
        item.second.GetFormsInfoByApp(formInfos);
        result = true;
    }
    APP_LOGD("all the form infos find success");
    return result;
}

bool BundleDataMgr::GetFormsInfoByModule(
    const std::string &bundleName, const std::string &moduleName, std::vector<FormInfo> &formInfos) const
{
    if (bundleName.empty()) {
        APP_LOGW("bundle name is empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    if (infoItem->second.IsDisabled()) {
        APP_LOGE("app %{public}s is disabled", infoItem->second.GetBundleName().c_str());
        return false;
    }
    infoItem->second.GetFormsInfoByModule(moduleName, formInfos);
    if (formInfos.empty()) {
        return false;
    }
    APP_LOGE("module forminfo find success");
    return true;
}

bool BundleDataMgr::GetFormsInfoByApp(const std::string &bundleName, std::vector<FormInfo> &formInfos) const
{
    if (bundleName.empty()) {
        APP_LOGW("bundle name is empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    if (infoItem->second.IsDisabled()) {
        APP_LOGE("app %{public}s is disabled", infoItem->second.GetBundleName().c_str());
        return false;
    }
    infoItem->second.GetFormsInfoByApp(formInfos);
    APP_LOGE("App forminfo find success");
    return true;
}

bool BundleDataMgr::GetShortcutInfos(
    const std::string &bundleName, int32_t userId, std::vector<ShortcutInfo> &shortcutInfos) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoWithFlags(
        bundleName, BundleFlag::GET_BUNDLE_DEFAULT, innerBundleInfo, requestUserId)) {
        APP_LOGE("GetLaunchWantForBundle failed");
        return false;
    }
    innerBundleInfo.GetShortcutInfos(shortcutInfos);
    return true;
}

bool BundleDataMgr::GetAllCommonEventInfo(const std::string &eventKey,
    std::vector<CommonEventInfo> &commonEventInfos) const
{
    if (eventKey.empty()) {
        APP_LOGW("event key is empty");
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGI("bundleInfos_ data is empty");
        return false;
    }
    for (const auto &item : bundleInfos_) {
        const InnerBundleInfo &info = item.second;
        if (info.IsDisabled()) {
            APP_LOGI("app %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }
        info.GetCommonEvents(eventKey, commonEventInfos);
    }
    if (commonEventInfos.size() == 0) {
        APP_LOGE("commonEventInfos is empty");
        return false;
    }
    APP_LOGE("commonEventInfos find success");
    return true;
}

bool BundleDataMgr::RemoveClonedBundleInfo(const std::string &bundleName)
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem != bundleInfos_.end()) {
        APP_LOGI("del bundle name:%{public}s", bundleName.c_str());
        const InnerBundleInfo &innerBundleInfo = infoItem->second;
        RecycleUidAndGid(innerBundleInfo);
        bool ret = dataStorage_->DeleteStorageBundleInfo(innerBundleInfo);
        if (!ret) {
            APP_LOGW("delete storage error name:%{public}s", bundleName.c_str());
            return false;
        }
        bundleInfos_.erase(bundleName);
    }
    return true;
}

bool BundleDataMgr::GetClonedBundleName(const std::string &bundleName, std::string &newName)
{
    APP_LOGI("GetCloneBundleName start");
    std::string name = bundleName + "#";
    for (auto it = bundleInfos_.begin(); it != bundleInfos_.end();) {
        if (it->first.find(name) != std::string::npos) {
            newName = it->first;
            APP_LOGI("new name is %{public}s", newName.c_str());
            return true;
        } else {
            ++it;
        }
    }
    return false;
}

bool BundleDataMgr::SavePreInstallBundleInfo(
    const std::string &bundleName, const PreInstallBundleInfo &preInstallBundleInfo)
{
    std::lock_guard<std::mutex> lock(preInstallInfoMutex_);
    if (!preInstallDataStorage_) {
        return false;
    }

    if (preInstallDataStorage_->SavePreInstallStorageBundleInfo(preInstallBundleInfo)) {
        auto info = std::find_if(
            preInstallBundleInfos_.begin(), preInstallBundleInfos_.end(), preInstallBundleInfo);
        if (info != preInstallBundleInfos_.end()) {
            *info = preInstallBundleInfo;
        } else {
            preInstallBundleInfos_.emplace_back(preInstallBundleInfo);
        }
        APP_LOGD("write storage success bundle:%{public}s", bundleName.c_str());
        return true;
    }

    return false;
}

bool BundleDataMgr::DeletePreInstallBundleInfo(
    const std::string &bundleName, const PreInstallBundleInfo &preInstallBundleInfo)
{
    std::lock_guard<std::mutex> lock(preInstallInfoMutex_);
    if (!preInstallDataStorage_) {
        return false;
    }

    if (preInstallDataStorage_->DeletePreInstallStorageBundleInfo(preInstallBundleInfo)) {
        auto info = std::find_if(
            preInstallBundleInfos_.begin(), preInstallBundleInfos_.end(), preInstallBundleInfo);
        if (info != preInstallBundleInfos_.end()) {
            preInstallBundleInfos_.erase(info);
        }
        APP_LOGI("Delete PreInstall Storage success bundle:%{public}s", bundleName.c_str());
        return true;
    }

    return false;
}

bool BundleDataMgr::GetPreInstallBundleInfo(
    const std::string &bundleName, PreInstallBundleInfo &preInstallBundleInfo)
{
    std::lock_guard<std::mutex> lock(preInstallInfoMutex_);
    if (bundleName.empty()) {
        APP_LOGE("bundleName is empty");
        return false;
    }

    preInstallBundleInfo.SetBundleName(bundleName);
    auto info = std::find_if(
        preInstallBundleInfos_.begin(), preInstallBundleInfos_.end(), preInstallBundleInfo);
    if (info != preInstallBundleInfos_.end()) {
        preInstallBundleInfo = *info;
        return true;
    }

    APP_LOGE("get preInstall bundleInfo failed by bundle(%{public}s).", bundleName.c_str());
    return false;
}

bool BundleDataMgr::LoadAllPreInstallBundleInfos(std::vector<PreInstallBundleInfo> &preInstallBundleInfos)
{
    if (!preInstallDataStorage_) {
        return false;
    }

    if (preInstallDataStorage_->LoadAllPreInstallBundleInfos(preInstallBundleInfos)) {
        APP_LOGD("load all storage success");
        return true;
    }

    return false;
}

bool BundleDataMgr::SaveInnerBundleInfo(const InnerBundleInfo &info) const
{
    APP_LOGD("write install InnerBundleInfo to storage with bundle:%{public}s", info.GetBundleName().c_str());
    if (dataStorage_->SaveStorageBundleInfo(info)) {
        APP_LOGD("save install InnerBundleInfo successfully");
        return true;
    }
    APP_LOGE("save install InnerBundleInfo failed!");
    return false;
}


bool BundleDataMgr::GetInnerBundleUserInfoByUserId(const std::string &bundleName,
    int32_t userId, InnerBundleUserInfo &innerBundleUserInfo) const
{
    APP_LOGD("get user info start: bundleName: (%{public}s)  userId: (%{public}d) ",
        bundleName.c_str(), userId);
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    if (bundleName.empty()) {
        APP_LOGW("bundle name is empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos data is empty");
        return false;
    }

    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    if (infoItem->second.IsDisabled()) {
        APP_LOGE("app %{public}s is disabled", infoItem->second.GetBundleName().c_str());
        return false;
    }

    return infoItem->second.GetInnerBundleUserInfo(requestUserId, innerBundleUserInfo);
}

int32_t BundleDataMgr::GetUserId(int32_t userId) const
{
    if (userId == Constants::ANY_USERID || userId == Constants::ALL_USERID) {
        return userId;
    }

    if (userId == Constants::UNSPECIFIED_USERID) {
        userId = GetUserIdByCallingUid();
    }

    if (!HasUserId(userId)) {
        APP_LOGE("user is not existed.");
        userId = Constants::INVALID_USERID;
    }

    return userId;
}

int32_t BundleDataMgr::GetUserIdByUid(int32_t uid) const
{
    return BundleUtil::GetUserIdByUid(uid);
}

void BundleDataMgr::AddUserId(int32_t userId)
{
    std::lock_guard<std::mutex> lock(multiUserIdSetMutex_);
    auto item = multiUserIdsSet_.find(userId);
    if (item != multiUserIdsSet_.end()) {
        return;
    }

    multiUserIdsSet_.insert(userId);
}

void BundleDataMgr::RemoveUserId(int32_t userId)
{
    std::lock_guard<std::mutex> lock(multiUserIdSetMutex_);
    auto item = multiUserIdsSet_.find(userId);
    if (item == multiUserIdsSet_.end()) {
        return;
    }

    multiUserIdsSet_.erase(item);
}

bool BundleDataMgr::HasUserId(int32_t userId) const
{
    std::lock_guard<std::mutex> lock(multiUserIdSetMutex_);
    return multiUserIdsSet_.find(userId) != multiUserIdsSet_.end();
}

int32_t BundleDataMgr::GetUserIdByCallingUid() const
{
    return BundleUtil::GetUserIdByCallingUid();
}

std::set<int32_t> BundleDataMgr::GetAllUser() const
{
    std::lock_guard<std::mutex> lock(multiUserIdSetMutex_);
    return multiUserIdsSet_;
}

bool BundleDataMgr::GetDistributedBundleInfo(const std::string &networkId, const std::string &bundleName,
    DistributedBundleInfo &distributedBundleInfo)
{
    return distributedDataStorage_->QueryStroageDistributeInfo(bundleName, networkId, distributedBundleInfo);
}

bool BundleDataMgr::GetInnerBundleUserInfos(
    const std::string &bundleName, std::vector<InnerBundleUserInfo> &innerBundleUserInfos) const
{
    APP_LOGD("get all user info in bundle(%{public}s)", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGW("bundle name is empty");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos data is empty");
        return false;
    }

    auto infoItem = bundleInfos_.find(bundleName);
    if (infoItem == bundleInfos_.end()) {
        return false;
    }
    if (infoItem->second.IsDisabled()) {
        APP_LOGE("app %{public}s is disabled", infoItem->second.GetBundleName().c_str());
        return false;
    }

    for (auto userInfo : infoItem->second.GetInnerBundleUserInfos()) {
        innerBundleUserInfos.emplace_back(userInfo.second);
    }

    return !innerBundleUserInfos.empty();
}

std::string BundleDataMgr::GetAppPrivilegeLevel(const std::string &bundleName, int32_t userId)
{
    APP_LOGD("GetAppPrivilegeLevel:%{public}s, userId:%{public}d", bundleName.c_str(), userId);
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    InnerBundleInfo info;
    if (!GetInnerBundleInfoWithFlags(bundleName, 0, info, userId)) {
        return Constants::EMPTY_STRING;
    }

    return info.GetAppPrivilegeLevel();
}

bool BundleDataMgr::QueryExtensionAbilityInfos(const Want &want, int32_t flags, int32_t userId,
    std::vector<ExtensionAbilityInfo> &extensionInfos) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }

    ElementName element = want.GetElement();
    std::string bundleName = element.GetBundleName();
    std::string moduleName = element.GetModuleName();
    std::string extensionName = element.GetAbilityName();
    APP_LOGD("bundle name:%{public}s, moduleName:%{public}s, extension name:%{public}s",
        bundleName.c_str(), moduleName.c_str(), extensionName.c_str());
    // explicit query
    if (!bundleName.empty() && !extensionName.empty()) {
        ExtensionAbilityInfo info;
        bool ret = ExplicitQueryExtensionInfo(bundleName, moduleName, extensionName, flags, requestUserId, info);
        if (!ret) {
            APP_LOGE("explicit queryExtensionInfo error");
            return false;
        }
        extensionInfos.emplace_back(info);
        return true;
    }

    bool ret = ImplicitQueryExtensionInfos(want, flags, requestUserId, extensionInfos);
    if (!ret) {
        APP_LOGE("implicit queryExtensionAbilityInfos error");
        return false;
    }
    if (extensionInfos.size() == 0) {
        APP_LOGE("no matching abilityInfo");
        return false;
    }
    APP_LOGD("query extensionAbilityInfo successfully");
    return true;
}

bool BundleDataMgr::ExplicitQueryExtensionInfo(const std::string &bundleName, const std::string &moduleName,
    const std::string &extensionName, int32_t flags, int32_t userId, ExtensionAbilityInfo &extensionInfo) const
{
    APP_LOGD("bundleName:%{public}s, moduleName:%{public}s, abilityName:%{public}s",
        bundleName.c_str(), moduleName.c_str(), extensionName.c_str());
    APP_LOGD("flags:%{public}d, userId:%{public}d", flags, userId);
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    InnerBundleInfo innerBundleInfo;
    if (!GetInnerBundleInfoWithFlags(bundleName, flags, innerBundleInfo, requestUserId)) {
        APP_LOGE("ExplicitQueryExtensionInfo failed");
        return false;
    }
    auto extension = innerBundleInfo.FindExtensionInfo(bundleName, moduleName, extensionName);
    if (!extension) {
        APP_LOGE("extensionAbility not found or disabled");
        return false;
    }
    if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_PERMISSION) != GET_ABILITY_INFO_WITH_PERMISSION) {
        extension->permissions.clear();
    }
    if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_METADATA) != GET_ABILITY_INFO_WITH_METADATA) {
        extension->metadata.clear();
    }
    extensionInfo = (*extension);
    if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_APPLICATION) == GET_ABILITY_INFO_WITH_APPLICATION) {
        int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
        innerBundleInfo.GetApplicationInfo(
            ApplicationFlag::GET_BASIC_APPLICATION_INFO, responseUserId, extensionInfo.applicationInfo);
    }
    return true;
}

void BundleDataMgr::FilterExtensionAbilityInfosByModuleName(const std::string &moduleName,
    std::vector<ExtensionAbilityInfo> &extensionInfos) const
{
    APP_LOGD("BundleDataMgr::FilterExtensionAbilityInfosByModuleName moduleName: %{public}s", moduleName.c_str());
    if (moduleName.empty()) {
        return;
    }
    for (auto iter = extensionInfos.begin(); iter != extensionInfos.end();) {
        if (iter->moduleName != moduleName) {
            iter = extensionInfos.erase(iter);
        } else {
            ++iter;
        }
    }
}

bool BundleDataMgr::ImplicitQueryExtensionInfos(
    const Want &want, int32_t flags, int32_t userId, std::vector<ExtensionAbilityInfo> &extensionInfos) const
{
    if (want.GetAction().empty() && want.GetEntities().empty()
        && want.GetUriString().empty() && want.GetType().empty()) {
        APP_LOGE("param invalid");
        return false;
    }
    APP_LOGD("action:%{public}s, uri:%{private}s, type:%{public}s",
        want.GetAction().c_str(), want.GetUriString().c_str(), want.GetType().c_str());
    APP_LOGD("flags:%{public}d, userId:%{public}d", flags, userId);

    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    std::string bundleName = want.GetElement().GetBundleName();
    if (!bundleName.empty()) {
        // query in current bundle
        InnerBundleInfo innerBundleInfo;
        if (!GetInnerBundleInfoWithFlags(bundleName, flags, innerBundleInfo, requestUserId)) {
            APP_LOGE("ImplicitQueryExtensionAbilityInfos failed");
            return false;
        }
        int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
        GetMatchExtensionInfos(want, flags, responseUserId, innerBundleInfo, extensionInfos);
        FilterExtensionAbilityInfosByModuleName(want.GetElement().GetModuleName(), extensionInfos);
    } else {
        // query all
        for (const auto &item : bundleInfos_) {
            InnerBundleInfo innerBundleInfo;
            if (!GetInnerBundleInfoWithFlags(item.first, flags, innerBundleInfo, requestUserId)) {
                APP_LOGE("ImplicitQueryExtensionAbilityInfos failed");
                continue;
            }
            int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
            GetMatchExtensionInfos(want, flags, responseUserId, innerBundleInfo, extensionInfos);
        }
    }
    // sort by priority, descending order.
    if (extensionInfos.size() > 1) {
        std::stable_sort(extensionInfos.begin(), extensionInfos.end(),
            [](ExtensionAbilityInfo a, ExtensionAbilityInfo b) { return a.priority > b.priority; });
    }
    return true;
}

void BundleDataMgr::GetMatchExtensionInfos(const Want &want, int32_t flags, const int32_t &userId,
    const InnerBundleInfo &info, std::vector<ExtensionAbilityInfo> &infos) const
{
    auto extensionSkillInfos = info.GetExtensionSkillInfos();
    auto extensionInfos = info.GetInnerExtensionInfos();
    for (const auto &skillInfos : extensionSkillInfos) {
        for (const auto &skill : skillInfos.second) {
            if (!skill.Match(want)) {
                continue;
            }
            if (extensionInfos.find(skillInfos.first) == extensionInfos.end()) {
                APP_LOGW("cannot find the extension info with %{public}s", skillInfos.first.c_str());
                break;
            }
            ExtensionAbilityInfo extensionInfo = extensionInfos[skillInfos.first];
            if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_APPLICATION) ==
                GET_ABILITY_INFO_WITH_APPLICATION) {
                info.GetApplicationInfo(
                    ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, extensionInfo.applicationInfo);
            }
            if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_PERMISSION) !=
                GET_ABILITY_INFO_WITH_PERMISSION) {
                extensionInfo.permissions.clear();
            }
            if ((static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_METADATA) != GET_ABILITY_INFO_WITH_METADATA) {
                extensionInfo.metadata.clear();
            }
            infos.emplace_back(extensionInfo);
            break;
        }
    }
}

bool BundleDataMgr::QueryExtensionAbilityInfos(const ExtensionAbilityType &extensionType, const int32_t &userId,
    std::vector<ExtensionAbilityInfo> &extensionInfos) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        return false;
    }
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    for (const auto &item : bundleInfos_) {
        InnerBundleInfo innerBundleInfo;
        if (!GetInnerBundleInfoWithFlags(item.first, 0, innerBundleInfo, requestUserId)) {
            APP_LOGE("QueryExtensionAbilityInfos failed");
            continue;
        }
        auto innerExtensionInfos = innerBundleInfo.GetInnerExtensionInfos();
        int32_t responseUserId = innerBundleInfo.GetResponseUserId(requestUserId);
        for (const auto &info : innerExtensionInfos) {
            if (info.second.type == extensionType) {
                ExtensionAbilityInfo extensionAbilityInfo = info.second;
                innerBundleInfo.GetApplicationInfo(
                    ApplicationFlag::GET_BASIC_APPLICATION_INFO, responseUserId, extensionAbilityInfo.applicationInfo);
                extensionInfos.emplace_back(extensionAbilityInfo);
            }
        }
    }
    return true;
}

std::vector<std::string> BundleDataMgr::GetAccessibleAppCodePaths(int32_t userId) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    std::vector<std::string> vec;
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ is empty");
        return vec;
    }

    for (const auto &item : bundleInfos_) {
        const InnerBundleInfo &info = item.second;
        auto userInfoMap = info.GetInnerBundleUserInfos();
        for (const auto &userInfo : userInfoMap) {
            auto innerUserId = userInfo.second.bundleUserInfo.userId;
            if (((innerUserId == 0) || (innerUserId == userId)) && info.IsAccessible()) {
                vec.emplace_back(info.GetAppCodePath());
            }
        }
    }
    return vec;
}

bool BundleDataMgr::QueryExtensionAbilityInfoByUri(const std::string &uri, int32_t userId,
    ExtensionAbilityInfo &extensionAbilityInfo) const
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        APP_LOGE("invalid userId -1");
        return false;
    }
    if (uri.empty()) {
        APP_LOGE("uri empty");
        return false;
    }
    // example of valid param uri : fileShare:///com.example.FileShare/person/10
    // example of convertUri : fileShare://com.example.FileShare
    size_t schemePos = uri.find(Constants::PARAM_URI_SEPARATOR);
    if (schemePos == uri.npos) {
        APP_LOGE("uri not include :///, invalid");
        return false;
    }
    size_t cutPos = uri.find(Constants::SEPARATOR, schemePos + Constants::PARAM_URI_SEPARATOR_LEN);
    // 1. cut string
    std::string convertUri = uri;
    if (cutPos != uri.npos) {
        convertUri = uri.substr(0, cutPos);
    }
    // 2. replace :/// with ://
    convertUri.replace(schemePos, Constants::PARAM_URI_SEPARATOR_LEN,
        Constants::URI_SEPARATOR);
    APP_LOGD("convertUri : %{private}s", convertUri.c_str());

    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ data is empty");
        return false;
    }
    for (const auto &item : bundleInfos_) {
        const InnerBundleInfo &info = item.second;
        if (info.IsDisabled()) {
            APP_LOGE("app %{public}s is disabled", info.GetBundleName().c_str());
            continue;
        }

        int32_t responseUserId = info.GetResponseUserId(requestUserId);
        if (!info.GetApplicationEnabled(responseUserId)) {
            continue;
        }

        bool ret = info.FindExtensionAbilityInfoByUri(convertUri, extensionAbilityInfo);
        if (!ret) {
            continue;
        }
        info.GetApplicationInfo(
            ApplicationFlag::GET_BASIC_APPLICATION_INFO, responseUserId, extensionAbilityInfo.applicationInfo);
        return true;
    }
    APP_LOGE("QueryExtensionAbilityInfoByUri (%{private}s) failed.", uri.c_str());
    return false;
}

void BundleDataMgr::GetAllUriPrefix(std::vector<std::string> &uriPrefixList, int32_t userId,
    const std::string &excludeModule) const
{
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    APP_LOGD("begin to GetAllUriPrefix, userId : %{public}d, excludeModule : %{public}s",
        userId, excludeModule.c_str());
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ is empty");
        return;
    }
    for (const auto &item : bundleInfos_) {
        item.second.GetUriPrefixList(uriPrefixList, userId, excludeModule);
        item.second.GetUriPrefixList(uriPrefixList, Constants::DEFAULT_USERID, excludeModule);
    }
}

#ifdef GLOBAL_RESMGR_ENABLE
std::shared_ptr<Global::Resource::ResourceManager> BundleDataMgr::GetResourceManager(
    const AppExecFwk::BundleInfo &bundleInfo) const
{
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    for (auto moduleResPath : bundleInfo.moduleResPaths) {
        if (!moduleResPath.empty()) {
            APP_LOGD("DistributedBms::InitResourceManager, moduleResPath: %{private}s", moduleResPath.c_str());
            if (!resourceManager->AddResource(moduleResPath.c_str())) {
                APP_LOGE("DistributedBms::InitResourceManager AddResource failed");
            }
        }
    }

    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    resConfig->SetLocaleInfo("zh", "Hans", "CN");
    resourceManager->UpdateResConfig(*resConfig);
    return resourceManager;
}
#endif

#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
bool BundleDataMgr::GetRemovableBundleNameVec(std::map<std::string, int>& bundlenameAndUids)
{
    if (bundleInfos_.empty()) {
        APP_LOGE("bundleInfos_ is data is empty.");
    }
    for (auto &it : bundleInfos_) {
        APP_LOGD("bundleName: %{public}s", it.first.c_str());
        int32_t userId = AccountHelper::GetCurrentActiveUserId();
        APP_LOGD("bundle userId is %{public}d, userId= %{public}d", it.second.GetUserId(), userId);
        if (!it.second.HasInnerBundleUserInfo(userId)) {
            continue;
        }
        if (it.second.IsBundleRemovable(userId)) {
            bundlenameAndUids.emplace(it.first, it.second.GetUid(it.second.GetUserId()));
        }
    }
    return true;
}
#endif

bool BundleDataMgr::QueryAllDeviceIds(std::vector<std::string> &deviceIds)
{
    return distributedDataStorage_->QueryAllDeviceIds(deviceIds);
}

const std::vector<PreInstallBundleInfo>& BundleDataMgr::GetAllPreInstallBundleInfos()
{
    std::lock_guard<std::mutex> lock(preInstallInfoMutex_);
    return preInstallBundleInfos_;
}

bool BundleDataMgr::ImplicitQueryInfoByPriority(const Want &want, int32_t flags, int32_t userId,
    AbilityInfo &abilityInfo, ExtensionAbilityInfo &extensionInfo)
{
    int32_t requestUserId = GetUserId(userId);
    if (requestUserId == Constants::INVALID_USERID) {
        APP_LOGE("invalid userId");
        return false;
    }
    std::vector<AbilityInfo> abilityInfos;
    bool abilityValid =
        ImplicitQueryAbilityInfos(want, flags, requestUserId, abilityInfos) && (abilityInfos.size() > 0);
    std::vector<ExtensionAbilityInfo> extensionInfos;
    bool extensionValid =
        ImplicitQueryExtensionInfos(want, flags, requestUserId, extensionInfos) && (extensionInfos.size() > 0);
    if (!abilityValid && !extensionValid) {
        // both invalid
        APP_LOGE("can't find target AbilityInfo or ExtensionAbilityInfo");
        return false;
    }
    if (abilityValid && extensionValid) {
        // both valid
        if (abilityInfos[0].priority >= extensionInfos[0].priority) {
            APP_LOGD("find target AbilityInfo with higher priority, name : %{public}s", abilityInfos[0].name.c_str());
            abilityInfo = abilityInfos[0];
        } else {
            APP_LOGD("find target ExtensionAbilityInfo with higher priority, name : %{public}s",
                extensionInfos[0].name.c_str());
            extensionInfo = extensionInfos[0];
        }
    } else if (abilityValid) {
        // only ability valid
        APP_LOGD("find target AbilityInfo, name : %{public}s", abilityInfos[0].name.c_str());
        abilityInfo = abilityInfos[0];
    } else {
        // only extension valid
        APP_LOGD("find target ExtensionAbilityInfo, name : %{public}s", extensionInfos[0].name.c_str());
        extensionInfo = extensionInfos[0];
    }
    return true;
}

bool BundleDataMgr::GetAllDependentModuleNames(const std::string &bundleName, const std::string &moduleName,
    std::vector<std::string> &dependentModuleNames)
{
    APP_LOGD("GetAllDependentModuleNames bundleName: %{public}s, moduleName: %{public}s",
        bundleName.c_str(), moduleName.c_str());
    std::lock_guard<std::mutex> lock(bundleInfoMutex_);
    auto item = bundleInfos_.find(bundleName);
    if (item == bundleInfos_.end()) {
        APP_LOGE("GetAllDependentModuleNames: bundleName not find");
        return false;
    }
    const InnerBundleInfo &innerBundleInfo = item->second;
    dependentModuleNames = innerBundleInfo.GetAllDependentModuleNames(moduleName);
    return true;
}

#ifdef BUNDLE_FRAMEWORK_GRAPHICS
std::shared_ptr<Media::PixelMap> BundleDataMgr::LoadImageFile(const std::string &path) const
{
    APP_LOGD("BundleDataMgr::LoadImageFile IN");
    uint32_t errorCode = 0;
    Media::SourceOptions opts;
    std::unique_ptr<Media::ImageSource> imageSource = Media::ImageSource::CreateImageSource(path,
                                                                                            opts,
                                                                                            errorCode);
    if (errorCode != 0) {
        APP_LOGE("Failed to create image source path %{private}s err %{public}d",
            path.c_str(), errorCode);
        return nullptr;
    }

    Media::DecodeOptions decodeOpts;
    auto pixelMapPtr = imageSource->CreatePixelMap(decodeOpts, errorCode);
    if (errorCode != 0) {
        APP_LOGE("Failed to create pixelmap path %{private}s err %{public}d",
            path.c_str(), errorCode);
        return nullptr;
    }
    APP_LOGD("BundleDataMgr::LoadImageFile OUT");
    return std::shared_ptr<Media::PixelMap>(std::move(pixelMapPtr));
}
#endif
}  // namespace AppExecFwk
}  // namespace OHOS
