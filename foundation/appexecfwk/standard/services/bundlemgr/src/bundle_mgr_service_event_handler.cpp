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

#include "bundle_mgr_service_event_handler.h"

#include <future>

#include "app_log_wrapper.h"
#include "bundle_mgr_service.h"
#include "bundle_parser.h"
#include "bundle_permission_mgr.h"
#include "bundle_scanner.h"
#include "bundle_util.h"
#ifdef CONFIG_POLOCY_ENABLE
#include "config_policy_utils.h"
#endif
#include "event_report.h"
#include "perf_profile.h"
#include "system_bundle_installer.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string APP_SUFFIX = "/app";
const std::string PRODUCT_SUFFIX = "/etc/bundle";

std::string GetScanBundleName(const std::string &str)
{
    auto index = str.find_last_of('/');
    if (index != std::string::npos) {
        return str.substr(index + 1);
    }
    
    return "";
}
}

BMSEventHandler::BMSEventHandler(const std::shared_ptr<EventRunner> &runner) : EventHandler(runner)
{
    APP_LOGD("instance is created");
}

BMSEventHandler::~BMSEventHandler()
{
    APP_LOGD("instance is destroyed");
}

void BMSEventHandler::ProcessEvent(const InnerEvent::Pointer &event)
{
    switch (event->GetInnerEventId()) {
        case BUNDLE_SCAN_START: {
            OnBootStartScanning(Constants::DEFAULT_USERID);
            SetAllInstallFlag();
            DelayedSingleton<BundleMgrService>::GetInstance()->RegisterService();
            break;
        }
        case BUNDLE_SCAN_FINISHED:
            break;
        case BMS_START_FINISHED:
            break;
        case BUNDLE_REBOOT_SCAN_START: {
            OnRebootStartScanning();
            SetAllInstallFlag();
            DelayedSingleton<BundleMgrService>::GetInstance()->RegisterService();
            DelayedSingleton<BundleMgrService>::GetInstance()->NotifyBundleScanStatus();
            break;
        }
        default:
            APP_LOGE("the eventId is not supported");
            break;
    }
}

void BMSEventHandler::OnBootStartScanning(int32_t userId)
{
    if (!BundlePermissionMgr::Init()) {
        APP_LOGW("BundlePermissionMgr::Init failed");
    }
    EventReport::SendScanSysEvent(BMSEventType::BOOT_SCAN_START);
#ifdef USE_PRE_BUNDLE_PROFILE
    ProcessBootBundleInstallFromPreBundleProFile(userId);
#else
    ProcessBootBundleInstallFromScan(userId);
#endif
    PerfProfile::GetInstance().Dump();
    EventReport::SendScanSysEvent(BMSEventType::BOOT_SCAN_END);
    BundlePermissionMgr::UnInit();
}

void BMSEventHandler::ProcessBootBundleInstallFromScan(int32_t userId)
{
    APP_LOGD("Process boot bundle install from scan");
    std::list<std::string> bundleDirs;
    GetBundleDirFromScan(bundleDirs);
    for (auto item : bundleDirs) {
        ProcessSystemBundleInstall(item, Constants::AppType::SYSTEM_APP, userId);
    }
}

void BMSEventHandler::GetBundleDirFromScan(std::list<std::string> &bundleDirs)
{
    ProcessScanDir(Constants::SYSTEM_APP_SCAN_PATH, bundleDirs);
    ProcessScanDir(Constants::THIRD_SYSTEM_APP_SCAN_PATH, bundleDirs);
#ifdef CONFIG_POLOCY_ENABLE
    auto cfgDirList = GetCfgDirList();
    if (cfgDirList == nullptr) {
        APP_LOGD("cfgDirList is empty");
        return;
    }

    for (auto cfgDir : cfgDirList->paths) {
        if (!cfgDir) {
            continue;
        }

        APP_LOGD("cfgDir: %{public}s ", cfgDir);
        ProcessScanDir(cfgDir + APP_SUFFIX, bundleDirs);
    }
#endif
    auto iter = std::find(bundleDirs.begin(), bundleDirs.end(), Constants::SYSTEM_RESOURCES_APP_PATH_NEW);
    if (iter != bundleDirs.end()) {
        bundleDirs.erase(iter);
        bundleDirs.insert(bundleDirs.begin(), Constants::SYSTEM_RESOURCES_APP_PATH_NEW);
    } else {
        iter = std::find(bundleDirs.begin(), bundleDirs.end(), Constants::SYSTEM_RESOURCES_APP_PATH);
        if (iter != bundleDirs.end()) {
            bundleDirs.erase(iter);
            bundleDirs.insert(bundleDirs.begin(), Constants::SYSTEM_RESOURCES_APP_PATH);
        }
    }
}

void BMSEventHandler::ProcessScanDir(const std::string &dir, std::list<std::string> &bundleDirs)
{
    BundleScanner scanner;
    std::list<std::string> bundleList = scanner.Scan(dir);
    for (auto item : bundleList) {
        auto iter = std::find(bundleDirs.begin(), bundleDirs.end(), item);
        if (iter == bundleDirs.end()) {
            bundleDirs.push_back(item);
        }
    }
}

void BMSEventHandler::ProcessBootBundleInstallFromPreBundleProFile(int32_t userId)
{
    APP_LOGD("Process boot bundle install from pre bundle proFile");
    std::set<PreScanInfo> scanInfos;
    std::set<std::string> uninstallBundleNames;
    std::set<PreBundleConfigInfo> preBundleConfigInfos;
    GetBundleDirFromPreBundleProFile(scanInfos, uninstallBundleNames, preBundleConfigInfos);
    InnerProcessBootPreBundleProFileInstall(scanInfos, uninstallBundleNames, preBundleConfigInfos, userId);
}

void BMSEventHandler::GetBundleDirFromPreBundleProFile(
    std::set<PreScanInfo> &scanInfos,
    std::set<std::string> &uninstallBundleNames,
    std::set<PreBundleConfigInfo> &preBundleConfigInfos)
{
    std::set<std::string> preBundleProFileDirs;
    // Product catalog
#ifdef CONFIG_POLOCY_ENABLE
    auto cfgDirList = GetCfgDirList();
    if (cfgDirList == nullptr) {
        APP_LOGD("cfgDirList is empty");
        return;
    }

    for (auto cfgDir : cfgDirList->paths) {
        if (!cfgDir) {
            continue;
        }

        std::string preBundleProFileDir = cfgDir + PRODUCT_SUFFIX;
        APP_LOGD("preBundleProFileDir: %{public}s ", preBundleProFileDir.c_str());
        if (preBundleProFileDirs.find(preBundleProFileDir) != preBundleProFileDirs.end()) {
            APP_LOGE("preBundleProFileDir(%{public}s) has parsed", preBundleProFileDir.c_str());
            continue;
        }

        preBundleProFileDirs.insert(preBundleProFileDir);
        ParsePreBundleProFile(preBundleProFileDir, scanInfos, uninstallBundleNames, preBundleConfigInfos);
    }
#endif
    // Default catalog
    if (preBundleProFileDirs.find(Constants::DEFAULT_PRE_BUNDLE_PROFILE) == preBundleProFileDirs.end()) {
        ParsePreBundleProFile(
            Constants::DEFAULT_PRE_BUNDLE_PROFILE, scanInfos, uninstallBundleNames, preBundleConfigInfos);
    }
}

void BMSEventHandler::ParsePreBundleProFile(
    std::string dir,
    std::set<PreScanInfo> &scanInfos,
    std::set<std::string> &uninstallBundleNames,
    std::set<PreBundleConfigInfo> &preBundleConfigInfos)
{
    BundleParser bundleParser;
    bundleParser.ParsePreInstallConfig(
        dir + Constants::PREINSTALL_CONFIG, scanInfos);
    bundleParser.ParsePreUnInstallConfig(
        dir + Constants::PREINSTALL_UNINSTALL_CONFIG, uninstallBundleNames);
    bundleParser.ParsePreInstallAbilityConfig(
        dir + Constants::PREINSTALL_ABILITY_CONFIG, preBundleConfigInfos);
}

void BMSEventHandler::InnerProcessBootPreBundleProFileInstall(
    const std::set<PreScanInfo> &scanInfos,
    const std::set<std::string> &uninstallBundleNames,
    const std::set<PreBundleConfigInfo> &preBundleConfigInfos,
    int32_t userId)
{
    for (auto scanInfo : scanInfos) {
        APP_LOGD("Inner process boot preBundle proFile install %{public}s", scanInfo.ToString().c_str());
        std::string scanBundleName = GetScanBundleName(scanInfo.bundleDir);
        if (uninstallBundleNames.find(scanBundleName) != uninstallBundleNames.end()) {
            APP_LOGW("bundle(%{public}s) not allowed installed", scanBundleName.c_str());
            continue;
        }

        PreBundleConfigInfo preBundleConfigInfo;
        preBundleConfigInfo.bundleName = scanBundleName;
        auto iter = preBundleConfigInfos.find(preBundleConfigInfo);
        if (iter != preBundleConfigInfos.end()) {
            preBundleConfigInfo = *iter;
        }

        ProcessSystemBundleInstall(scanInfo, preBundleConfigInfo, userId);
    }
}

void BMSEventHandler::ProcessSystemBundleInstall(
    const PreScanInfo &preScanInfo, const PreBundleConfigInfo &preBundleConfig, int32_t userId)
{
    APP_LOGD("Process system bundle install by preScanInfo(%{public}s)", preScanInfo.bundleDir.c_str());
    SystemBundleInstaller installer;
    if (!installer.InstallSystemBundle(
        preScanInfo.bundleDir, Constants::AppType::SYSTEM_APP, userId)) {
        APP_LOGW("Install System app:%{public}s error", preScanInfo.bundleDir.c_str());
    }
}

void BMSEventHandler::ProcessSystemBundleInstall(
    const std::string &bundleDir, Constants::AppType appType, int32_t userId)
{
    APP_LOGD("Process system bundle install by bundleDir(%{public}s)", bundleDir.c_str());
    SystemBundleInstaller installer;
    if (!installer.InstallSystemBundle(bundleDir, appType, userId)) {
        APP_LOGW("Install System app:%{public}s error", bundleDir.c_str());
    }
}

void BMSEventHandler::SetAllInstallFlag() const
{
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return;
    }

    dataMgr->SetInitialUserFlag(true);
}

void BMSEventHandler::OnRebootStartScanning(int32_t userId)
{
    if (!BundlePermissionMgr::Init()) {
        APP_LOGW("BundlePermissionMgr::Init failed");
    }
    EventReport::SendScanSysEvent(BMSEventType::BOOT_SCAN_START);
    ProcessRebootBundle(userId);
    EventReport::SendScanSysEvent(BMSEventType::BOOT_SCAN_END);
    BundlePermissionMgr::UnInit();
}

void BMSEventHandler::ProcessRebootBundle(int32_t userId)
{
    APP_LOGD("Process reboot bundle start");
    if (!LoadAllPreInstallBundleInfos()) {
        APP_LOGE("Load all preInstall bundleInfos failed.");
        return;
    }

    ProcessRebootBundleInstall();
    ProcessRebootBundleUninstall();
}

bool BMSEventHandler::LoadAllPreInstallBundleInfos()
{
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return false;
    }

    std::vector<PreInstallBundleInfo> preInstallBundleInfos = dataMgr->GetAllPreInstallBundleInfos();
    for (auto &iter : preInstallBundleInfos) {
        APP_LOGD("preInstallBundleInfos: %{public}s ", iter.GetBundleName().c_str());
        loadExistData_.emplace(iter.GetBundleName(), iter);
    }

    return true;
}

void BMSEventHandler::ProcessRebootBundleInstall()
{
    APP_LOGD("Process reboot bundle install start");
#ifdef USE_PRE_BUNDLE_PROFILE
    ProcessRebootBundleInstallFromPreBundleProFile();
#else
    ProcessRebootBundleInstallFromScan();
#endif
}

void BMSEventHandler::ProcessRebootBundleInstallFromPreBundleProFile()
{
    APP_LOGD("Process reboot bundle install from pre bundle proFile");
    std::set<PreScanInfo> scanInfos;
    std::set<std::string> uninstallBundleNames;
    std::set<PreBundleConfigInfo> preBundleConfigInfos;
    GetBundleDirFromPreBundleProFile(scanInfos, uninstallBundleNames, preBundleConfigInfos);
    ProcessReBootPreBundleProFileInstall(scanInfos, uninstallBundleNames, preBundleConfigInfos);
}

void BMSEventHandler::ProcessReBootPreBundleProFileInstall(
    const std::set<PreScanInfo> &scanInfos,
    const std::set<std::string> &uninstallBundleNames,
    const std::set<PreBundleConfigInfo> &preBundleConfigInfos)
{
    std::list<std::string> bundleDirs;
    for (auto scanInfo : scanInfos) {
        APP_LOGD("Process reboot preBundle proFile install %{public}s", scanInfo.ToString().c_str());
        std::string scanBundleName = GetScanBundleName(scanInfo.bundleDir);
        if (uninstallBundleNames.find(scanBundleName) != uninstallBundleNames.end()) {
            APP_LOGW("bundle(%{public}s) not allowed installed", scanBundleName.c_str());
            continue;
        }

        bundleDirs.push_back(scanInfo.bundleDir);
    }

    InnerProcessRebootBundleInstall(bundleDirs, Constants::AppType::SYSTEM_APP);
}

void BMSEventHandler::ProcessRebootBundleInstallFromScan()
{
    APP_LOGD("Process reboot bundle install from scan");
    std::list<std::string> bundleDirs;
    GetBundleDirFromScan(bundleDirs);
    InnerProcessRebootBundleInstall(bundleDirs, Constants::AppType::SYSTEM_APP);
}

void BMSEventHandler::InnerProcessRebootBundleInstall(
    const std::list<std::string> &scanPathList, Constants::AppType appType)
{
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return;
    }

    for (auto &scanPathIter : scanPathList) {
        APP_LOGD("reboot scan bundle path: %{public}s ", scanPathIter.c_str());
        std::unordered_map<std::string, InnerBundleInfo> infos;
        if (!ParseHapFiles(scanPathIter, infos) || infos.empty()) {
            APP_LOGE("obtain bundleinfo failed : %{public}s ", scanPathIter.c_str());
            continue;
        }

        auto bundleName = infos.begin()->second.GetBundleName();
        auto hapVersionCode = infos.begin()->second.GetVersionCode();
        AddParseInfosToMap(bundleName, infos);
        auto mapIter = loadExistData_.find(bundleName);
        if (mapIter == loadExistData_.end()) {
            APP_LOGD("OTA Install new bundle(%{public}s) by path(%{private}s).",
                bundleName.c_str(), scanPathIter.c_str());
            std::vector<std::string> filePaths { scanPathIter };
            if (!OTAInstallSystemBundle(filePaths, appType)) {
                APP_LOGE("OTA Install new bundle(%{public}s) error.", bundleName.c_str());
            }

            continue;
        }

        APP_LOGD("OTA process bundle(%{public}s) by path(%{private}s).",
            bundleName.c_str(), scanPathIter.c_str());
        BundleInfo hasInstalledInfo;
        auto hasBundleInstalled = dataMgr->GetBundleInfo(
            bundleName, BundleFlag::GET_BUNDLE_DEFAULT, hasInstalledInfo, Constants::ANY_USERID);
        if (!hasBundleInstalled) {
            APP_LOGW("app(%{public}s) has been uninstalled and do not OTA install.",
                bundleName.c_str());
            continue;
        }

        std::vector<std::string> filePaths;
        for (auto item : infos) {
            auto parserModuleNames = item.second.GetModuleNameVec();
            if (parserModuleNames.empty()) {
                APP_LOGE("module is empty when parser path(%{public}s).", item.first.c_str());
                continue;
            }

            // Used to judge whether the module has been installed.
            bool hasModuleInstalled = std::find(
                hasInstalledInfo.hapModuleNames.begin(), hasInstalledInfo.hapModuleNames.end(),
                parserModuleNames[0]) != hasInstalledInfo.hapModuleNames.end();
            if (HasModuleSavedInPreInstalledDb(bundleName, item.first) && !hasModuleInstalled) {
                APP_LOGW("module(%{public}s) has been uninstalled and do not OTA install",
                    parserModuleNames[0].c_str());
                continue;
            }

            // Generally, when the versionCode of Hap is greater than the installed versionCode,
            // Except for the uninstalled app, they can be installed or upgraded directly by OTA.
            if (hasInstalledInfo.versionCode < hapVersionCode) {
                APP_LOGD("OTA update module(%{public}s) by path(%{private}s)",
                    parserModuleNames[0].c_str(), item.first.c_str());
                    filePaths.emplace_back(item.first);
            }

            // The versionCode of Hap is equal to the installed versionCode.
            // You can only install new modules by OTA
            if (hasInstalledInfo.versionCode == hapVersionCode) {
                if (hasModuleInstalled) {
                    APP_LOGD("module(%{public}s) has been installed and versionCode is same.",
                        parserModuleNames[0].c_str());
                    continue;
                }

                APP_LOGD("OTA install module(%{public}s) by path(%{private}s)",
                    parserModuleNames[0].c_str(), item.first.c_str());
                filePaths.emplace_back(item.first);
            }
        }

        if (filePaths.empty()) {
            continue;
        }

        if (!OTAInstallSystemBundle(filePaths, appType)) {
            APP_LOGE("OTA bundle(%{public}s) failed", bundleName.c_str());
        }
    }
}

void BMSEventHandler::AddParseInfosToMap(
    const std::string &bundleName, const std::unordered_map<std::string, InnerBundleInfo> &infos)
{
    auto hapParseInfoMapIter = hapParseInfoMap_.find(bundleName);
    if (hapParseInfoMapIter == hapParseInfoMap_.end()) {
        hapParseInfoMap_.emplace(bundleName, infos);
        return;
    }

    auto iterMap = hapParseInfoMapIter->second;
    for (auto infoIter : infos) {
        iterMap.emplace(infoIter.first, infoIter.second);
    }

    hapParseInfoMap_.at(bundleName) = iterMap;
}

void BMSEventHandler::ProcessRebootBundleUninstall()
{
    APP_LOGD("Reboot scan and OTA uninstall start");
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return;
    }

    for (auto &loadIter : loadExistData_) {
        std::string bundleName = loadIter.first;
        auto listIter = hapParseInfoMap_.find(bundleName);
        if (listIter == hapParseInfoMap_.end()) {
            APP_LOGD("OTA uninstall app(%{public}s).", bundleName.c_str());
            SystemBundleInstaller installer;
            if (!installer.UninstallSystemBundle(bundleName)) {
                APP_LOGE("OTA uninstall app(%{public}s) error", bundleName.c_str());
            } else {
                std::string moduleName;
                DeletePreInfoInDb(bundleName, moduleName, true);
            }

            continue;
        }

        BundleInfo hasInstalledInfo;
        auto hasBundleInstalled = dataMgr->GetBundleInfo(
            bundleName, BundleFlag::GET_BUNDLE_DEFAULT, hasInstalledInfo, Constants::ANY_USERID);
        if (!hasBundleInstalled) {
            APP_LOGW("app(%{public}s) maybe has been uninstall.", bundleName.c_str());
            continue;
        }

        // Check the installed module.
        // If the corresponding Hap does not exist, it should be uninstalled.
        for (auto moduleName : hasInstalledInfo.hapModuleNames) {
            bool hasModuleHapExist = false;
            for (auto parserInfoIter : listIter->second) {
                auto parserModuleNames = parserInfoIter.second.GetModuleNameVec();
                if (!parserModuleNames.empty() && moduleName == parserModuleNames[0]) {
                    hasModuleHapExist = true;
                    break;
                }
            }

            if (!hasModuleHapExist) {
                APP_LOGD("OTA app(%{public}s) uninstall module(%{public}s).",
                    bundleName.c_str(), moduleName.c_str());
                SystemBundleInstaller installer;
                if (!installer.UninstallSystemBundle(bundleName, moduleName)) {
                    APP_LOGE("OTA app(%{public}s) uninstall module(%{public}s) error.",
                        bundleName.c_str(), moduleName.c_str());
                }
            }
        }

        // Check the preInstall path in Db.
        // If the corresponding Hap does not exist, it should be deleted.
        auto parserInfoMap = listIter->second;
        for (auto preBundlePath : loadIter.second.GetBundlePaths()) {
            auto parserInfoIter = parserInfoMap.find(preBundlePath);
            if (parserInfoIter != parserInfoMap.end()) {
                APP_LOGD("OTA uninstall app(%{public}s) module path(%{private}s) exits.",
                    bundleName.c_str(), preBundlePath.c_str());
                continue;
            }

            APP_LOGD("OTA app(%{public}s) delete path(%{private}s).",
                bundleName.c_str(), preBundlePath.c_str());
            DeletePreInfoInDb(bundleName, preBundlePath, false);
        }
    }

    APP_LOGD("Reboot scan and OTA uninstall success");
}

void BMSEventHandler::DeletePreInfoInDb(
    const std::string &bundleName, const std::string &bundlePath, bool bundleLevel)
{
    APP_LOGD("DeletePreInfoInDb bundle(%{public}s)", bundleName.c_str());
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return;
    }

    PreInstallBundleInfo preInstallBundleInfo;
    preInstallBundleInfo.SetBundleName(bundleName);
    if (bundleLevel) {
        APP_LOGD("DeletePreInfoInDb bundle bundleLevel");
        dataMgr->DeletePreInstallBundleInfo(bundleName, preInstallBundleInfo);
        return;
    }

    APP_LOGD("DeletePreInfoInDb bundle not bundleLevel with path(%{private}s)",
        bundlePath.c_str());
    dataMgr->GetPreInstallBundleInfo(bundleName, preInstallBundleInfo);
    preInstallBundleInfo.DeleteBundlePath(bundlePath);
    if (preInstallBundleInfo.GetBundlePaths().empty()) {
        dataMgr->DeletePreInstallBundleInfo(bundleName, preInstallBundleInfo);
    } else {
        dataMgr->SavePreInstallBundleInfo(bundleName, preInstallBundleInfo);
    }
}

bool BMSEventHandler::HasModuleSavedInPreInstalledDb(
    const std::string &bundleName, const std::string &bundlePath)
{
    auto preInstallIter = loadExistData_.find(bundleName);
    if (preInstallIter == loadExistData_.end()) {
        APP_LOGE("app(%{public}s) does not save in PreInstalledDb.", bundleName.c_str());
        return false;
    }

    return preInstallIter->second.HasBundlePath(bundlePath);
}

bool BMSEventHandler::OTAInstallSystemBundle(
    const std::vector<std::string> &filePaths, Constants::AppType appType)
{
    if (filePaths.empty()) {
        APP_LOGE("File path is empty");
        return false;
    }

    SystemBundleInstaller installer;
    return installer.OTAInstallSystemBundle(filePaths, appType);
}

bool BMSEventHandler::ParseHapFiles(
    const std::string &hapFilePath, std::unordered_map<std::string, InnerBundleInfo> &infos)
{
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    std::vector<std::string> hapFilePathVec { hapFilePath };
    std::vector<std::string> realPaths;
    auto ret = BundleUtil::CheckFilePath(hapFilePathVec, realPaths);
    if (ret != ERR_OK) {
        APP_LOGE("File path %{private}s invalid", hapFilePath.c_str());
        return false;
    }

    BundleParser bundleParser;
    for (auto realPath : realPaths) {
        InnerBundleInfo innerBundleInfo;
        ret = bundleParser.Parse(realPath, innerBundleInfo);
        if (ret != ERR_OK) {
            APP_LOGE("parse bundle info failed, error: %{public}d", ret);
            continue;
        }

        infos.emplace(realPath, innerBundleInfo);
    }

    ret = CheckAppLabelInfo(infos);
    if (ret != ERR_OK) {
        APP_LOGE("Check APP label failed %{public}d", ret);
        return false;
    }

    return true;
}

ErrCode BMSEventHandler::CheckAppLabelInfo(
    const std::unordered_map<std::string, InnerBundleInfo> &infos)
{
    APP_LOGD("Check APP label");
    ErrCode ret = ERR_OK;
    if (infos.empty()) {
        return ret;
    }

    std::string bundleName = (infos.begin()->second).GetBundleName();
    std::string vendor = (infos.begin()->second).GetVendor();
    auto versionCode = (infos.begin()->second).GetVersionCode();
    std::string versionName = (infos.begin()->second).GetVersionName();
    uint32_t target = (infos.begin()->second).GetTargetVersion();
    uint32_t compatible = (infos.begin()->second).GetCompatibleVersion();
    bool singleton = (infos.begin()->second).IsSingleton();
    Constants::AppType appType = (infos.begin()->second).GetAppType();

    for (const auto &info :infos) {
        // check bundleName
        if (bundleName != info.second.GetBundleName()) {
            return ERR_APPEXECFWK_INSTALL_BUNDLENAME_NOT_SAME;
        }
        // check version
        if (versionCode != info.second.GetVersionCode()) {
            return ERR_APPEXECFWK_INSTALL_VERSIONCODE_NOT_SAME;
        }
        if (versionName != info.second.GetVersionName()) {
            return ERR_APPEXECFWK_INSTALL_VERSIONNAME_NOT_SAME;
        }
        // check vendor
        if (vendor != info.second.GetVendor()) {
            return ERR_APPEXECFWK_INSTALL_VENDOR_NOT_SAME;
        }
        // check release type
        if (target != info.second.GetTargetVersion()) {
            return ERR_APPEXECFWK_INSTALL_RELEASETYPE_TARGET_NOT_SAME;
        }
        if (compatible != info.second.GetCompatibleVersion()) {
            return ERR_APPEXECFWK_INSTALL_RELEASETYPE_COMPATIBLE_NOT_SAME;
        }
        if (singleton != info.second.IsSingleton()) {
            return ERR_APPEXECFWK_INSTALL_SINGLETON_NOT_SAME;
        }
        if (appType != info.second.GetAppType()) {
            return ERR_APPEXECFWK_INSTALL_APPTYPE_NOT_SAME;
        }
    }
    APP_LOGD("finish check APP label");
    return ret;
}
}  // namespace AppExecFwk
}  // namespace OHOS
