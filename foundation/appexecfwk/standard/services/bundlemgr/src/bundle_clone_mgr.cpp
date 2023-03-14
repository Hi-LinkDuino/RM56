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
#include "bundle_clone_mgr.h"

#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

#include "ability_manager_helper.h"
#include "bundle_mgr_service.h"
#include "installd_client.h"
#include "ipc_skeleton.h"
#include "scope_guard.h"

namespace OHOS {
namespace AppExecFwk {
BundleCloneMgr::BundleCloneMgr()
{
    APP_LOGI("");
}

BundleCloneMgr::~BundleCloneMgr()
{
    APP_LOGI("");
}

bool BundleCloneMgr::BundleClone(const std::string &bundleName)
{
    APP_LOGD("BundleClone start");
    auto result = CheckBundleNameInAllowList(bundleName);
    if (result != true) {
        APP_LOGE("The APP is not on the AllowList");
        APP_LOGE("BundleClone fail");
        return false;
    }
    result = SetCloneAppName(bundleName);
    if (result != true) {
        APP_LOGE("BundleClone fail because the APP clone file already exists");
        APP_LOGE("BundleClone fail");
        return false;
    }
    result = CreateCloneBundleInfo(bundleName);
    if (result != true) {
        DeleteCloneAppName(bundleName);
        APP_LOGE("CreateCloneBundleInfo fail");
        APP_LOGE("BundleClone fail");
        return false;
    }
    result = ProcessCloneInstall(bundleName);
    if (result != ERR_OK) {
        DeleteCloneAppName(bundleName);
        APP_LOGE("ProcessCloneInstall fail");
        APP_LOGE("BundleClone fail");
        return false;
    }
    APP_LOGD("clone app success");
    return true;
}

bool BundleCloneMgr::SetCloneAppName(const std::string &bundleName)
{
    APP_LOGD("SetCloneAppName start");
    std::fstream f("/data/clonefile/CloneAppName.json");
    bool isExist = f.good();
    if (isExist) {
        nlohmann::json jParse;
        f.seekg(0, std::ios::end);
        int len = static_cast<int>(f.tellg());
        if (len == 0) {
            jParse[bundleName] = bundleName;
            f << std::setw(Constants::DUMP_INDENT) << jParse << std::endl;
            APP_LOGD("clone bundleName Of file success (first)");
        } else {
            f.seekg(0, std::ios::beg);
            nlohmann::json jsonFile;
            f >> jsonFile;
            if (jsonFile.find(bundleName) != jsonFile.end()) {
                APP_LOGE("clone bundleName = %{public}s is exist", bundleName.c_str());
                f.close();
                return false;
            } else {
                jsonFile[bundleName] = bundleName;
                f.seekp(0, std::ios::beg);
                f << std::setw(Constants::DUMP_INDENT) << jsonFile << std::endl;
                APP_LOGD("clone bundleName = %{public}s Of file success", bundleName.c_str());
            }
        }
    } else {
        APP_LOGE("clone file not exist");
        return false;
    }
    f.close();
    return true;
}

bool BundleCloneMgr::DeleteCloneAppName(const std::string &bundleName)
{
    APP_LOGI("delete clone file");
    bool isEmpty = false;
    std::ifstream i("/data/clonefile/CloneAppName.json", std::ifstream::in);
    nlohmann::json jParse;
    if (!i.is_open()) {
        APP_LOGE("failed to open clone file");
        return false;
    } else {
        i.seekg(0, std::ios::end);
        int len = static_cast<int>(i.tellg());
        if (len != 0) {
            i.seekg(0, std::ios::beg);
            i >> jParse;
            APP_LOGE("find name in the clone file");
            if (jParse.find(bundleName) != jParse.end()) {
                jParse.erase(bundleName);
                APP_LOGE("clone file fstream delete success");
            } else {
                APP_LOGE("not find appName = %{public}s", bundleName.c_str());
            }
        } else {
            APP_LOGE("file is empty appName = %{private}s", bundleName.c_str());
        }
    }
    i.close();

    std::ofstream o("/data/clonefile/CloneAppName.json");
    if (!o.is_open()) {
        APP_LOGE("failed to open clone file");
        return false;
    } else {
        if (!isEmpty) {
            o << std::setw(Constants::DUMP_INDENT) << jParse;
        }
    }
    o.close();
    APP_LOGD("clone file AppName delete success");
    return true;
}

bool BundleCloneMgr::CreateCloneBundleInfo(const std::string &bundleName)
{
    APP_LOGD("CreateCloneBundleInfo start");
    dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr_ == nullptr) {
        APP_LOGE("datamgr is nullptr");
        return false;
    }
    auto result = dataMgr_->GetInnerBundleInfo(bundleName, cloneInfo_);
    if (!result) {
        APP_LOGE("Clone AppName BundleInfo fail");
        return false;
    }
    APP_LOGD("finish CreateCloneBundleInfo");
    cloneInfo_.SetIsCloned(true);
    bool LogisCloned = cloneInfo_.GetisCloned();
    APP_LOGD("cloneInfo_.GetisCloned() is %{public}d", LogisCloned);
    return result;
}

ErrCode BundleCloneMgr::ProcessCloneInstall(const std::string &bundleName)
{
    APP_LOGD("ProcessCloneInstall start");
    ErrCode result = 0;
    dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!ModifyInstallDirByHapType(cloneInfo_)) {
        APP_LOGE("clone modify bundle install dir failed %{public}d", result);
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }
    bundleName_ = cloneInfo_.GetBundleName();
    modulePackage_ = cloneInfo_.GetCurrentModulePackage();
    mainAbility_ = cloneInfo_.GetMainAbility();

    // Create clone Uid
    result = CreateBundleAndDataDir(cloneInfo_);
    if (result != ERR_OK) {
        APP_LOGE("clone create bundle and data dir failed");
        return result;
    }
    cloneInfo_.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
    if (!dataMgr_->SaveNewInfoToDB(bundleName_, cloneInfo_)) {
        APP_LOGE("clone add bundle %{public}s info failed", bundleName_.c_str());
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    std::string Newbundlename = cloneInfo_.GetDBKeyBundleName();
    dataMgr_->EnableBundle(Newbundlename);
    dataMgr_->EnableBundle(bundleName_);
    APP_LOGD("ProcessCloneInstall finish");
    return ERR_OK;
}

ErrCode BundleCloneMgr::CreateBundleAndDataDir(InnerBundleInfo &info) const
{
    APP_LOGD("CreateBundleAndDataDir start");
    auto cloneCodePath = cloneInfo_.GetAppCodePath();
    APP_LOGI("clone create bundle dir %{private}s", cloneCodePath.c_str());
    InnerBundleUserInfo newInnerBundleUserInfo;
    newInnerBundleUserInfo.bundleUserInfo.userId = Constants::DEFAULT_USERID;
    newInnerBundleUserInfo.bundleName = bundleName_;
    if (!dataMgr_->GenerateUidAndGid(newInnerBundleUserInfo)) {
        APP_LOGE("clone fail to gererate uid and gid");
        return ERR_APPEXECFWK_INSTALL_GENERATE_UID_ERROR;
    }
    info.SetApplicationInfoUid();
    int logUid = info.GetApplicationInfoUid();
    APP_LOGD("info.uid is %{public}d", logUid);

    bool LogisCloned = info.GetisCloned();
    APP_LOGD("info.GetisCloned is %{public}d", LogisCloned);

    info.SetNewBundleName(bundleName_);
    std::string Newbundlename = info.GetDBKeyBundleName();
    APP_LOGI("clone Newbundlename %{public}s", Newbundlename.c_str());
    auto cloneDataPath = baseDataPath_ + Constants::PATH_SEPARATOR + Newbundlename;
    ErrCode result = InstalldClient::GetInstance()->CreateBundleDataDir(
        cloneDataPath, newInnerBundleUserInfo.bundleUserInfo.userId,
        newInnerBundleUserInfo.uid, newInnerBundleUserInfo.uid, info.GetAppPrivilegeLevel());
    if (result != ERR_OK) {
        APP_LOGE("clone fail to create bundle data dir, error is %{public}d", result);
        return result;
    }
    UpdateBundlePaths(info, cloneDataPath);
    APP_LOGD("CreateBundleAndDataDir finish");
    return ERR_OK;
}

bool BundleCloneMgr::ModifyInstallDirByHapType(InnerBundleInfo &info)
{
    auto internalPath = Constants::PATH_SEPARATOR + Constants::USER_ACCOUNT_DIR + Constants::FILE_UNDERLINE +
                        std::to_string(info.GetUserId()) + Constants::PATH_SEPARATOR;
    switch (info.GetAppType()) {
        case Constants::AppType::SYSTEM_APP:
            baseCodePath_ = Constants::SYSTEM_APP_INSTALL_PATH + internalPath + Constants::APP_CODE_DIR;
            baseDataPath_ = Constants::SYSTEM_APP_INSTALL_PATH + internalPath + Constants::APP_DATA_DIR;
            break;
        case Constants::AppType::THIRD_SYSTEM_APP:
            baseCodePath_ = Constants::THIRD_SYSTEM_APP_INSTALL_PATH + internalPath + Constants::APP_CODE_DIR;
            baseDataPath_ = Constants::THIRD_SYSTEM_APP_INSTALL_PATH + internalPath + Constants::APP_DATA_DIR;
            break;
        case Constants::AppType::THIRD_PARTY_APP:
            baseCodePath_ = Constants::THIRD_PARTY_APP_INSTALL_PATH + internalPath + Constants::APP_CODE_DIR;
            baseDataPath_ = Constants::THIRD_PARTY_APP_INSTALL_PATH + internalPath + Constants::APP_DATA_DIR;
            break;
        default:
            APP_LOGE("Clone app type error");
            return false;
    }
    return true;
}

bool BundleCloneMgr::UpdateBundlePaths(InnerBundleInfo &info, const std::string baseDataPath) const
{
    APP_LOGD("Clone UpdateBundlePaths");
    info.SetBaseDataDir(baseDataPath);
    info.SetAppDataDir(baseDataPath);
    info.SetAppDataBaseDir(baseDataPath + Constants::PATH_SEPARATOR + Constants::DATA_BASE_DIR);
    info.SetAppCacheDir(baseDataPath + Constants::PATH_SEPARATOR + Constants::CACHE_DIR);
    return true;
}

bool BundleCloneMgr::RemoveClonedBundle(const std::string &oldName, const std::string &bundleName)
{
    APP_LOGI("begin to process remove %{public}s cloned bundle", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("bundle name empty");
        return false;
    }
    std::shared_ptr<BundleDataMgr> dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr_) {
        APP_LOGE("Get dataMgr shared_ptr nullptr");
        return false;
    }
    InnerBundleInfo newcloneInfo_;
    if (!dataMgr_->GetInnerBundleInfo(bundleName, newcloneInfo_)) {
        APP_LOGE("remove bundle info missing");
        return false;
    }
    auto uid = newcloneInfo_.GetUid();
    std::string Newbundlename = cloneInfo_.GetDBKeyBundleName();
    APP_LOGD("Newbundlename is %{public}s", Newbundlename.c_str());
    ScopeGuard enableCloneGuard([&] { dataMgr_->EnableBundle(Newbundlename); });
    APP_LOGD("bundleName_ is %{public}s", bundleName_.c_str());
    ScopeGuard enableGuard([&] { dataMgr_->EnableBundle(bundleName_); });

    if (!AbilityManagerHelper::UninstallApplicationProcesses(newcloneInfo_.GetApplicationName(), uid)) {
        APP_LOGE("can not kill process");
        return false;
    }
    auto result = InstalldClient::GetInstance()->RemoveDir(newcloneInfo_.GetBaseDataDir());
    if (result != ERR_OK) {
        APP_LOGE("fail to remove bundle data dir, error is %{public}d", result);
        return false;
    }
    if (!dataMgr_->RemoveClonedBundleInfo(bundleName)) {
        APP_LOGE("fail to remove bundle data dir");
        return false;
    }
    result = DeleteCloneAppName(oldName);
    if (result != true) {
        APP_LOGE("fail to DeleteCloneAppNameDeleteCloneAppName");
        return false;
    }
    APP_LOGI("finish to process cloned %{public}s bundle remove", bundleName.c_str());
    return true;
}

bool BundleCloneMgr::CheckBundleNameInAllowList(const std::string &bundleName)
{
    nlohmann::json jsonObject = nullptr;
    std::ifstream ifs("/data/clonefile/BundleNameAllowList.json", std::ifstream::in);
    if (ifs.is_open()) {
        jsonObject=nlohmann::json::parse(ifs);
        for (auto item : jsonObject) {
            if (item ["bundlename"] == bundleName) {
                ifs.close();
                return true;
            }
        }
        ifs.close();
        return false;
    }
    return false;
}
}  // namespace AppExecFwk
}  // namespace OHOS
