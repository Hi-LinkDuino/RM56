/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "bundle_manager_service.h"

#include "app_verify_pub.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#include <algorithm>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>

#include "appexecfwk_errors.h"
#include "bundle_callback_utils.h"
#include "bundle_common.h"
#include "bundle_daemon_client.h"
#include "bundle_info_utils.h"
#include "bundle_inner_feature.h"
#include "bundle_manager.h"
#include "bundle_message_id.h"
#include "bundle_parser.h"
#include "bundle_util.h"
#include "liteipc_adapter.h"
#include "log.h"
#include "samgr_lite.h"
#include "utils.h"
#include "want.h"

namespace OHOS {
ManagerService::ManagerService()
{
    installer_ = new BundleInstaller(INSTALL_PATH, DATA_PATH);
    bundleMap_ = BundleMap::GetInstance();
}

ManagerService::~ManagerService()
{
    if (installer_ != nullptr) {
        delete installer_;
        installer_ = nullptr;
    }
    sysUidMap_.clear();
    sysVendorUidMap_.clear();
    appUidMap_.clear();
}

static void InnerTransact(uint32_t code, uint8_t resultCode, const char *bundleName)
{
    if (bundleName == nullptr) {
        return;
    }
    IpcIo io;
    char data[IPC_IO_DATA_MAX];
    IpcIo reply;
    uintptr_t ptr;
    IpcIoInit(&io, data, IPC_IO_DATA_MAX, 0);
    IpcIoPushInt32(&io, static_cast<int32_t>(resultCode));
    IpcIoPushString(&io, bundleName);
    if (!IpcIoAvailable(&io)) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS InnerTransact ipc failed");
        return;
    }
    std::vector<SvcIdentity> svcIdentity = ManagerService::GetInstance().GetServiceId();
    if (svcIdentity.empty()) {
        return;
    }
    for (const auto& svc : svcIdentity) {
        int32_t ret = Transact(NULL, svc, code, &io, &reply, LITEIPC_FLAG_ONEWAY, &ptr);
        if (ret != LITEIPC_OK) {
            HILOG_ERROR(HILOG_MODULE_APP, "BundleMS InnerTransact failed %{public}d\n", ret);
            return;
        }
    }
}

static void InnerSelfTransact(uint32_t code, uint8_t resultCode, const SvcIdentity &svc)
{
    IpcIo io;
    char data[IPC_IO_DATA_MAX];
    IpcIo reply;
    IpcIoInit(&io, data, IPC_IO_DATA_MAX, 0);
    IpcIoPushInt32(&io, static_cast<int32_t>(resultCode));
    int32_t ret = Transact(NULL, svc, code, &io, &reply, LITEIPC_FLAG_ONEWAY, NULL);
    if (ret != LITEIPC_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS InnerSelfTransact failed %{public}d\n", ret);
    }
#ifdef __LINUX__
    BinderRelease(svc.ipcContext, svc.handle);
#endif
}

std::vector<SvcIdentity> ManagerService::GetServiceId() const
{
    return svcIdentity_;
}

bool ManagerService::GetAmsInterface(AmsInnerInterface **amsInterface)
{
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(AMS_SERVICE, AMS_INNER_FEATURE);
    if (iUnknown == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS GetAmsInterface failed");
        return false;
    }

    int result = iUnknown->QueryInterface(iUnknown, 0, (void **) amsInterface);
    if (result != 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS Query ams Interface failed: %{public}d\n", result);
        return false;
    }

    return true;
}

uint8_t ManagerService::SetExternalInstallMode(bool enable)
{
    IsExternalInstallMode_ = enable;
    HILOG_INFO(HILOG_MODULE_APP, "current install mode is %d", IsExternalInstallMode_);
    return ERR_OK;
}

bool ManagerService::IsExternalInstallMode() const
{
    return IsExternalInstallMode_;
}

uint8_t ManagerService::SetDebugMode(bool enable)
{
    int32_t ret = APPVERI_SetDebugMode(enable);
    if (ret < 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "set signature debug mode failed");
        return ERR_APPEXECFWK_SET_DEBUG_MODE_ERROR;
    }
    isDebugMode_ = enable;
    HILOG_INFO(HILOG_MODULE_APP, "current sign debug mode is %d", isDebugMode_);
    return ERR_OK;
}

bool ManagerService::IsDebugMode() const
{
    return isDebugMode_;
}

#ifdef OHOS_DEBUG
uint8_t ManagerService::SetSignMode(bool enable)
{
    isSignMode_ = enable;
    HILOG_INFO(HILOG_MODULE_APP, "current sign mode is %d", isSignMode_);
    return ERR_OK;
}

bool ManagerService::IsSignMode() const
{
    return isSignMode_;
}
#endif

void ManagerService::ServiceMsgProcess(Request* request)
{
    if (request == nullptr) {
        return;
    }

    if (installer_ == nullptr) {
        installer_ = new BundleInstaller(INSTALL_PATH, DATA_PATH);
    }

    switch (request->msgId) {
        case BUNDLE_SERVICE_INITED: {
            if (!BundleDaemonClient::GetInstance().Initialize()) {
                HILOG_ERROR(HILOG_MODULE_APP, "BundleDeamonClient initialize fail");
                return;
            }
            ScanPackages();
            ScanSharedLibPath();
            AmsInnerInterface *amsInterface = nullptr;
            if (!GetAmsInterface(&amsInterface)) {
                return;
            }
            if (amsInterface != nullptr) {
                amsInterface->StartKeepAliveApps();
            }
            break;
        }
        case BUNDLE_UPDATED:
            /* Process update request by Install() */
        case BUNDLE_INSTALLED: {
            auto info = reinterpret_cast<SvcIdentityInfo *>(request->data);
            if (info == nullptr) {
                return;
            }
            if (info->svc == nullptr || info->path == nullptr) {
                AdapterFree(info->path);
                AdapterFree(info->svc);
                return;
            }

            InstallThirdBundle(info->path, *(info->svc), info->installLocation);
            AdapterFree(info->path);
            AdapterFree(info->svc);
            break;
        }
        case BUNDLE_UNINSTALLED: {
            auto info = reinterpret_cast<SvcIdentityInfo *>(request->data);
            if (info == nullptr) {
                return;
            }
            if ((info->bundleName == nullptr) || (info->svc == nullptr)) {
                AdapterFree(info->bundleName);
                AdapterFree(info->svc);
                return;
            }

            InstallParam installParam = {.installLocation = 1, .keepData = info->keepData};
            uint8_t bResult = installer_->Uninstall(info->bundleName, installParam);
            InnerSelfTransact(UNINSTALL_CALLBACK, bResult, *(info->svc));
            InnerTransact(UNINSTALL_CALLBACK, bResult, info->bundleName);
            if (bResult == ERR_OK) {
                RecycleUid(info->bundleName);
            }
            AdapterFree(info->bundleName);
            AdapterFree(info->svc);
            break;
        }
        case BUNDLE_CHANGE_CALLBACK: {
            auto svc = reinterpret_cast<SvcIdentity *>(request->data);
            if (svc == nullptr) {
                return;
            }
            if (request->msgValue) {
                AddCallbackServiceId(*svc);
            } else {
                RemoveCallbackServiceId(*svc);
            }
            break;
        }
        default: {
            break;
        }
    }
}

static bool CompareServiceId(const SvcIdentity &svc1, const SvcIdentity &svc2)
{
    return (svc1.handle == svc2.handle) && (svc1.token == svc2.token);
}

void ManagerService::AddCallbackServiceId(const SvcIdentity &svc)
{
    for (auto it = svcIdentity_.begin(); it != svcIdentity_.end(); ++it) {
        if (CompareServiceId(*it, svc)) {
            return;
        }
    }
    svcIdentity_.emplace_back(svc);
}

void ManagerService::RemoveCallbackServiceId(const SvcIdentity &svc)
{
#ifdef __LINUX__
    BinderRelease(svc.ipcContext, svc.handle);
#endif
    for (auto it = svcIdentity_.begin(); it != svcIdentity_.end(); ++it) {
        if (CompareServiceId(*it, svc)) {
            svcIdentity_.erase(it);
            return;
        }
    }
}

void ManagerService::InstallThirdBundle(const char *path, const SvcIdentity &svc, int32_t installLocation)
{
    if (path == nullptr || installer_ == nullptr) {
        return;
    }
    char *bundleName = nullptr;
    int32_t versionCode = -1;
    int8_t ret = BundleParser::ParseBundleParam(path, &bundleName, versionCode);
    if (ret != ERR_OK) {
        InnerSelfTransact(INSTALL_CALLBACK, ret, svc);
        AdapterFree(bundleName);
        return;
    }
    InstallParam installParam = {.installLocation = installLocation, .keepData = false};
    uint8_t bResult = installer_->Install(path, installParam);
    HILOG_DEBUG(HILOG_MODULE_APP, "BundleMS InstallThirdBundle Install : %{public}d\n", bResult);
    InnerSelfTransact(INSTALL_CALLBACK, bResult, svc);
    InnerTransact(INSTALL_CALLBACK, bResult, bundleName);
}

void ManagerService::InstallAllSystemBundle(int32_t scanFlag)
{
    DIR *dir = nullptr;
    dirent *ent = nullptr;

    dir = (scanFlag == SYSTEM_APP_FLAG) ? opendir(SYSTEM_BUNDLE_PATH) : opendir(THIRD_SYSTEM_BUNDLE_PATH);
    if (dir == nullptr) {
        return;
    }

    while ((ent = readdir(dir)) != nullptr) {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..")) == 0) {
            continue;
        }
        if (scanFlag == SYSTEM_APP_FLAG) {
            InstallSystemBundle(SYSTEM_BUNDLE_PATH, ent->d_name);
        } else {
            InstallSystemBundle(THIRD_SYSTEM_BUNDLE_PATH, ent->d_name);
        }
    }
    closedir(dir);
}

void ManagerService::InstallSystemBundle(const char *fileDir, const char *fileName)
{
    if ((installer_ == nullptr) || (fileDir == nullptr) || (fileName == nullptr)) {
        return;
    }
    std::string systemAppPath = std::string(fileDir) + PATH_SEPARATOR + fileName;
    if (!BundleUtil::IsFile(systemAppPath.c_str()) ||
        !BundleUtil::EndWith(systemAppPath.c_str(), INSTALL_FILE_SUFFIX)) {
        return;
    }
    InstallParam installParam = {.installLocation = 1, .keepData = false};
    uint8_t ret = installer_->Install(systemAppPath.c_str(), installParam);
    HILOG_INFO(HILOG_MODULE_APP, "install system app, result is : %{public}d", ret);
}

void ManagerService::ScanPackages()
{
    // restore uid and gid map
    RestoreUidAndGidMap();

    if (!BundleUtil::IsDir(JSON_PATH)) {
        InstallAllSystemBundle(SYSTEM_APP_FLAG);
        InstallAllSystemBundle(THIRD_SYSTEM_APP_FLAG);
        return;
    }

    // get third system bundle uninstall record
    cJSON *uninstallRecord = BundleUtil::GetJsonStream(UNINSTALL_THIRD_SYSTEM_BUNDLE_JSON);
    if (uninstallRecord == nullptr) {
        BundleDaemonClient::GetInstance().RemoveFile(UNINSTALL_THIRD_SYSTEM_BUNDLE_JSON);
    }

    // scan system apps
    ScanAppDir(SYSTEM_BUNDLE_PATH, nullptr, SYSTEM_APP_FLAG);
    // scan third system apps
    ScanAppDir(THIRD_SYSTEM_BUNDLE_PATH, uninstallRecord, THIRD_SYSTEM_APP_FLAG);
    if (uninstallRecord != nullptr) {
        cJSON_Delete(uninstallRecord);
        uninstallRecord = nullptr;
    }
    // scan third apps
    ScanAppDir(INSTALL_PATH, nullptr, THIRD_APP_FLAG);
    // scan third apps in sdcard if exists
    if (BundleUtil::IsDir(EXTEANAL_INSTALL_PATH)) {
        ScanAppDir(EXTEANAL_INSTALL_PATH, nullptr, THIRD_APP_FLAG);
    }
}

void ManagerService::ScanAppDir(const char *appDir, const cJSON *uninstallRecord, uint8_t scanFlag)
{
    dirent *ent = nullptr;
    char *bundleName = nullptr;
    int32_t versionCode = -1;

    if (appDir == nullptr) {
        return;
    }

    DIR *dir = opendir(appDir);
    if (dir == nullptr) {
        return;
    }
    while ((ent = readdir(dir)) != nullptr) {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..")) == 0) {
            continue;
        }
        std::string appPath = std::string(appDir) + PATH_SEPARATOR + ent->d_name;
        if (scanFlag == THIRD_APP_FLAG) {
            if (!BundleUtil::IsDir(appPath.c_str())) {
                continue;
            }
            ReloadEntireBundleInfo(appPath.c_str(), ent->d_name, versionCode, scanFlag);
            continue;
        }

        // scan system app
        bool res = CheckSystemBundleIsValid(appPath.c_str(), &bundleName, versionCode);
        if (!res) {
            AdapterFree(bundleName);
            continue;
        }

        if (scanFlag == THIRD_SYSTEM_APP_FLAG &&
            CheckThirdSystemBundleHasUninstalled(bundleName, uninstallRecord)) {
            AdapterFree(bundleName);
            continue;
        }
        ReloadEntireBundleInfo(appPath.c_str(), bundleName, versionCode, scanFlag);
        AdapterFree(bundleName);
    }
    closedir(dir);
}

bool ManagerService::CheckSystemBundleIsValid(const char *appPath, char **bundleName, int32_t &versionCode)
{
    if (appPath == nullptr || bundleName == nullptr) {
        return false;
    }

    if (!BundleUtil::EndWith(appPath, INSTALL_FILE_SUFFIX)) {
        return false;
    }

    if (BundleParser::ParseBundleParam(appPath, bundleName, versionCode) != 0) {
        return false;
    }

    if (*bundleName != nullptr && strlen(*bundleName) > MAX_BUNDLE_NAME_LEN) {
        return false;
    }
    return true;
}

void ManagerService::ReloadEntireBundleInfo(const char *appPath, const char *bundleName, int32_t versionCode,
    uint8_t scanFlag)
{
    char *codePath = nullptr;
    char *appId = nullptr;
    int32_t oldVersionCode = -1;
    InstallParam installParam = {.installLocation = 1, .keepData = false};

    if (appPath == nullptr || bundleName == nullptr || installer_ == nullptr) {
        return;
    }

    if (QueryBundleInfo(bundleName) != nullptr) {
        return;
    }

    bool res = BundleUtil::CheckBundleJsonIsValid(bundleName, &codePath, &appId, oldVersionCode);
    bool isSystemApp = (scanFlag == SYSTEM_APP_FLAG);
    if (scanFlag != THIRD_APP_FLAG) {
        if (!res) {
            uint8_t ret = installer_->Install(appPath, installParam);
            HILOG_INFO(HILOG_MODULE_APP, "install new system app, result is %d", ret);
            AdapterFree(codePath);
            AdapterFree(appId);
            return;
        }
        if (versionCode > oldVersionCode) {
            ReloadBundleInfo(codePath, appId, bundleName, isSystemApp);
            uint8_t ret = installer_->Install(appPath, installParam);
            HILOG_INFO(HILOG_MODULE_APP, "update system app, result is %d", ret);
            AdapterFree(codePath);
            AdapterFree(appId);
            return;
        }
    } else {
        if (!res) {
            HILOG_ERROR(HILOG_MODULE_APP, "codePath or appid in third app json file is invalid!");
            AdapterFree(codePath);
            AdapterFree(appId);
            return;
        }
    }
    ReloadBundleInfo(codePath, appId, bundleName, isSystemApp);
    AdapterFree(appId);
    AdapterFree(codePath);
}

void ManagerService::ReloadBundleInfo(const char *codePath, const char *appId, const char *bundleName,
    bool isSystemApp)
{
    BundleParser bundleParser;
    dirent *ent = nullptr;

    int32_t uid = BundleUtil::GetValueFromBundleJson(bundleName, JSON_SUB_KEY_UID, INVALID_UID);
    int32_t gid = uid;
    if (uid == INVALID_UID || gid == INVALID_GID) {
        HILOG_ERROR(HILOG_MODULE_APP, "get uid or gid in json file fail!");
        return;
    }

    DIR *dir = opendir(codePath);
    if (dir == nullptr) {
        return;
    }

    while ((ent = readdir(dir)) != nullptr) {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..")) == 0) {
            continue;
        }
        std::string profileDir = codePath + std::string(PATH_SEPARATOR) + ent->d_name;
        BundleInfo *bundleInfo = bundleParser.ParseHapProfile(profileDir.c_str());
        if (bundleInfo != nullptr) {
            bundleInfo->isSystemApp = isSystemApp;
            bundleInfo->appId = Utils::Strdup(appId);
            if (bundleInfo->appId == nullptr) {
                HILOG_ERROR(HILOG_MODULE_APP, "bundleInfo->appId is nullptr when restore bundleInfo!");
                BundleInfoUtils::FreeBundleInfo(bundleInfo);
                continue;
            }
            bundleInfo->uid = static_cast<int32_t>(uid);
            bundleInfo->gid = static_cast<int32_t>(gid);
            // need to update bundleInfo when support many haps install
            bundleMap_->Add(bundleInfo);
        } else {
            BundleDaemonClient::GetInstance().RemoveFile(profileDir.c_str());
            // delete uid and gid info
            BundleUtil::DeleteUidInfoFromJson(bundleName);
        }
    }
    closedir(dir);
}

bool ManagerService::CheckThirdSystemBundleHasUninstalled(const char *bundleName, const cJSON *object)
{
    if (object == nullptr || bundleName == nullptr) {
        return false;
    }

    cJSON *array = cJSON_GetObjectItem(object, JSON_MAIN_KEY);
    if (!cJSON_IsArray(array)) {
        return false;
    }

    cJSON *item = nullptr;
    cJSON_ArrayForEach(item, array) {
        if (!cJSON_IsString(item)) {
            return false;
        }
        if ((item->valuestring != nullptr) && strcmp(bundleName, item->valuestring) == 0) {
            return true;
        }
    }
    return false;
}

BundleInfo *ManagerService::QueryBundleInfo(const char *bundleName)
{
    if (bundleMap_ == nullptr || bundleName == nullptr) {
        return nullptr;
    }
    return bundleMap_->Get(bundleName);
}

void ManagerService::RemoveBundleInfo(const char *bundleName)
{
    if (bundleMap_ == nullptr || bundleName == nullptr) {
        return;
    }
    bundleMap_->Erase(bundleName);
}

void ManagerService::AddBundleInfo(BundleInfo *info)
{
    if (info == nullptr || info->bundleName == nullptr || bundleMap_ == nullptr) {
        return;
    }
    bundleMap_->Add(info);
}

bool ManagerService::UpdateBundleInfo(BundleInfo *info)
{
    if (info == nullptr || info->bundleName == nullptr || bundleMap_ == nullptr) {
        return false;
    }
    return bundleMap_->Update(info);
}

uint8_t ManagerService::GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo &bundleInfo)
{
    if (bundleName == nullptr || bundleMap_ == nullptr) {
        return ERR_APPEXECFWK_QUERY_PARAMETER_ERROR;
    }
    return bundleMap_->GetBundleInfo(bundleName, flags, bundleInfo);
}

bool ManagerService::HasSystemCapability(const char *sysCapName)
{
    if (sysCapName == nullptr) {
        return ERR_APPEXECFWK_QUERY_PARAMETER_ERROR;
    }
    return SAMGR_GetInstance()->HasSystemCapability(sysCapName);
}

uint8_t ManagerService::GetSystemAvailableCapabilities(char syscap[][MAX_SYSCAP_NAME_LEN], int32_t *len)
{
    if (syscap == nullptr || len == nullptr) {
        return ERR_APPEXECFWK_QUERY_PARAMETER_ERROR;
    }
    int32_t res = SAMGR_GetInstance()->GetSystemAvailableCapabilities(syscap, len);
    if (res != ERR_OK) {
        return ERR_APPEXECFWK_QUERY_PARAMETER_ERROR;
    }
    return ERR_OK;
}

uint8_t ManagerService::GetBundleInfos(int32_t flags, BundleInfo **bundleInfos, int32_t *len)
{
    if (bundleMap_ == nullptr) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    return bundleMap_->GetBundleInfos(flags, bundleInfos, len);
}

uint32_t ManagerService::GetBundleSize(const char *bundleName)
{
    if (bundleName == nullptr) {
        return 0;
    }
    BundleInfo *installedInfo = bundleMap_->Get(bundleName);
    if (installedInfo == nullptr) {
        return 0;
    }
    char *codePath = installedInfo->codePath;
    uint32_t codeBundleSize = BundleUtil::GetFileFolderSize(codePath);
    if (codeBundleSize == 0) {
        return 0;
    }
    char *dataPath = installedInfo->dataPath;
    uint32_t dataBundleSize = BundleUtil::GetFileFolderSize(dataPath);
    HILOG_INFO(HILOG_MODULE_APP, "bundle size is %{public}d\n", codeBundleSize + dataBundleSize);
    return codeBundleSize + dataBundleSize;
}

std::string ManagerService::GetCodeDirPath() const
{
    std::string codeDirPath;
    if (installer_ == nullptr) {
        return codeDirPath;
    }
    return installer_->GetCodeDirPath();
}

std::string ManagerService::GetDataDirPath() const
{
    std::string dataDirPath;
    if (installer_ == nullptr) {
        return dataDirPath;
    }
    return installer_->GetDataDirPath();
}

static int32_t GenerateInnerUid(std::map<int, std::string> &innerMap, const std::string &bundleName,
    int8_t bundleStyle, int32_t baseUid)
{
    if (innerMap.empty()) {
        innerMap.emplace(0, bundleName);
        return baseUid;
    }
    int32_t ret = 0;
    for (int32_t i = 0; i < innerMap.rbegin()->first; ++i) {
        if (innerMap.find(i) == innerMap.end()) {
            innerMap.emplace(i, bundleName);
            ret = i + baseUid;
            return ret;
        }
    }

    if ((bundleStyle == SYSTEM_APP_FLAG) && (innerMap.rbegin()->first == BASE_SYS_VEN_UID - 1)) {
        return INVALID_UID;
    }

    if ((bundleStyle == THIRD_SYSTEM_APP_FLAG) && (innerMap.rbegin()->first == MAX_SYS_VEN_UID)) {
        return INVALID_UID;
    }
    innerMap.emplace((innerMap.rbegin()->first + 1), bundleName);
    ret = innerMap.rbegin()->first + baseUid;
    return ret;
}

int32_t ManagerService::GenerateUid(const char *bundleName, int8_t bundleStyle)
{
    if (bundleName == nullptr) {
        return INVALID_UID;
    }

    if (bundleStyle == THIRD_SYSTEM_APP_FLAG) {
        return GenerateInnerUid(sysVendorUidMap_, bundleName, THIRD_SYSTEM_APP_FLAG, BASE_SYS_VEN_UID);
    } else if (bundleStyle == THIRD_APP_FLAG) {
        return GenerateInnerUid(appUidMap_, bundleName, THIRD_APP_FLAG, BASE_APP_UID);
    } else if (bundleStyle == SYSTEM_APP_FLAG) {
        return GenerateInnerUid(sysUidMap_, bundleName, SYSTEM_APP_FLAG, BASE_SYS_UID);
    } else {
        return INVALID_UID;
    }
}

bool ManagerService::RecycleInnerUid(const std::string &bundleName, std::map<int, std::string> &innerMap)
{
    auto it = innerMap.begin();
    while (it != innerMap.end()) {
        if (it->second == bundleName) {
            innerMap.erase(it);
            return true;
        }
        it++;
    }
    return false;
}

void ManagerService::RecycleUid(const char *bundleName)
{
    if (bundleName == nullptr) {
        return;
    }

    if (RecycleInnerUid(bundleName, appUidMap_) || RecycleInnerUid(bundleName, sysVendorUidMap_) ||
        RecycleInnerUid(bundleName, sysUidMap_)) {
        return;
    }
}

void ManagerService::ScanSharedLibPath()
{
    BundleInfo *bundleInfos = nullptr;
    int32_t len = 0;
    if (bundleMap_->GetBundleInfos(1, &bundleInfos, &len) != ERR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "ScanSharedLibPath GetBundleInfos is error");
        return;
    }
    if (bundleInfos == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "ScanSharedLibPath bundleInfos is unavailable");
        return;
    }
    for (int32_t index = 0; index < len; ++index) {
        if (!(bundleInfos + index)->isSystemApp) {
            continue;
        }
        std::string path = (bundleInfos + index)->codePath;
        path = path + PATH_SEPARATOR + (bundleInfos + index)->moduleInfos[0].moduleName + PATH_SEPARATOR +
               SHARED_LIB_NAME;
        if (!BundleUtil::IsDir(path.c_str())) {
            continue;
        }

        if (BundleDaemonClient::GetInstance().MoveFile(path.c_str(), SHARED_LIB_PATH) != EC_SUCCESS) {
            HILOG_WARN(HILOG_MODULE_APP, "ScanSharedLibPath move file to share library failed");
        }
    }
    BundleInfoUtils::FreeBundleInfos(bundleInfos, len);
}

void ManagerService::RestoreUidAndGidMap()
{
    std::string uidJsonPath = std::string(JSON_PATH) + UID_GID_MAP + JSON_SUFFIX;
    cJSON *object = BundleUtil::GetJsonStream(uidJsonPath.c_str());
    if (object == nullptr) {
        return;
    }

    cJSON *uids = cJSON_GetObjectItemCaseSensitive(object, "uid_and_gid");
    if (!(cJSON_IsArray(uids))) {
        cJSON_Delete(object);
        return;
    }
    cJSON *uid = nullptr;
    cJSON_ArrayForEach(uid, uids) {
        cJSON *innerUid = cJSON_GetObjectItemCaseSensitive(uid, JSON_SUB_KEY_UID);
        cJSON *innerBundleName = cJSON_GetObjectItemCaseSensitive(uid, JSON_SUB_KEY_PACKAGE);
        if ((!cJSON_IsNumber(innerUid)) || (!cJSON_IsString(innerBundleName))) {
            continue;
        }
        uint32_t uidValue = innerUid->valueint;
        if ((uidValue < BASE_SYS_VEN_UID) && (uidValue >= BASE_SYS_UID)) {
            sysUidMap_[uidValue - BASE_SYS_UID] = innerBundleName->valuestring;
        } else if ((uidValue >= BASE_SYS_VEN_UID) && (uidValue <= MAX_SYS_VEN_UID)) {
            sysVendorUidMap_[uidValue - BASE_SYS_VEN_UID] = innerBundleName->valuestring;
        } else if (uidValue > MAX_SYS_VEN_UID) {
            appUidMap_[uidValue - BASE_APP_UID] = innerBundleName->valuestring;
        } else {
            continue;
        }
    }
    cJSON_Delete(object);
    return;
}
} // namespace OHOS
