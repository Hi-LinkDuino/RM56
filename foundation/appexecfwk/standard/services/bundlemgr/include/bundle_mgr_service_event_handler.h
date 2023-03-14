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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_EVENT_HANDLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_EVENT_HANDLER_H

#include "bundle_constants.h"
#include "bundle_data_mgr.h"
#include "event_handler.h"
#include "pre_scan_info.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrService;

class BMSEventHandler : public EventHandler {
public:
    explicit BMSEventHandler(const std::shared_ptr<EventRunner> &runner);
    virtual ~BMSEventHandler() override;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     * @return
     */
    virtual void ProcessEvent(const InnerEvent::Pointer &event) override;

    enum {
        BUNDLE_SCAN_START = 1,
        BUNDLE_SCAN_FINISHED,
        BMS_START_FINISHED,
        BUNDLE_REBOOT_SCAN_START,
    };

private:
    /**
     * @brief start boot scan.
     * @param userId Indicates the userId.
     * @return
     */
    void OnBootStartScanning(int32_t userId = Constants::UNSPECIFIED_USERID);
    /**
     * @brief Process boot bundle install from scan.
     * @param userId Indicates the userId.
     * @return
     */
    void ProcessBootBundleInstallFromScan(int32_t userId);
    /**
     * @brief Process boot bundle install from pre bundle profile.
     * @param userId Indicates the userId.
     * @return
     */
    void ProcessBootBundleInstallFromPreBundleProFile(int32_t userId);
    /**
     * @brief Process bundle install by scanInfos.
     * @param scanInfos Indicates the scanInfos contains installed info.
     * @param uninstallBundleNames Indicates the uninstall bundleNames.
     * @param preBundleConfigInfos Indicates the preBundleConfigInfos.
     * @param userId Indicates the userId.
     * @return
     */
    void InnerProcessBootPreBundleProFileInstall(
        const std::set<PreScanInfo> &scanInfos,
        const std::set<std::string> &uninstallBundleNames,
        const std::set<PreBundleConfigInfo> &preBundleConfigInfos,
        int32_t userId);
    /**
     * @brief Install bundles by scanDir.
     * @param scanDir Indicates the scanDir.
     * @param appType Indicates the bundle type.
     * @param userId Indicates userId.
     * @return
     */
    void ProcessSystemBundleInstall(const std::string &scanDir,
        Constants::AppType appType, int32_t userId = Constants::UNSPECIFIED_USERID);
    /**
     * @brief Install bundles by preScanInfo.
     * @param preScanInfo Indicates the preScanInfo.
     * @param preBundleConfig Indicates the preBundleConfig.
     * @param userId Indicates userId.
     * @return
     */
    void ProcessSystemBundleInstall(
        const PreScanInfo &preScanInfo,
        const PreBundleConfigInfo &preBundleConfig,
        int32_t userId);
    /**
     * @brief start reboot scan.
     * @param userId Indicates the userId.
     * @return
     */
    void OnRebootStartScanning(int32_t userId = Constants::UNSPECIFIED_USERID);
    /**
     * @brief Process reboot bundle.
     * @param userId Indicates the userId.
     * @return
     */
    void ProcessRebootBundle(int32_t userId);
    /**
     * @brief Obtains the PreInstallBundleInfo objects.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool LoadAllPreInstallBundleInfos();
    /**
     * @brief Process reboot bundle install.
     * @return
     */
    void ProcessRebootBundleInstall();
    /**
     * @brief Process reboot bundle install from pre bundle profile.
     * @return
     */
    void ProcessRebootBundleInstallFromPreBundleProFile();
    /**
     * @brief Process reboot bundle install by scanInfos.
     * @param scanInfos Indicates the scanInfos contains installed info.
     * @param uninstallBundleNames Indicates the uninstall bundleNames.
     * @param preBundleConfigInfos Indicates the preBundleConfigInfos.
     * @return
     */
    void ProcessReBootPreBundleProFileInstall(
        const std::set<PreScanInfo> &scanInfos,
        const std::set<std::string> &uninstallBundleNames,
        const std::set<PreBundleConfigInfo> &preBundleConfigInfos);
    /**
     * @brief Process reboot bundle install from scan.
     * @return
     */
    void ProcessRebootBundleInstallFromScan();
    /**
     * @brief Process reboot install bundles by bundleList.
     * @param bundleList Indicates store bundle list.
     * @param appType Indicates the bundle type.
     * @return
     */
    void InnerProcessRebootBundleInstall(
        const std::list<std::string> &bundleList, Constants::AppType appType);
    /**
     * @brief Reboot uninstall system and system vendor bundles.
     * @return
     */
    void ProcessRebootBundleUninstall();
    /**
     * @brief Get bundle dir by scan.
     * @param bundleDirs Indicates the return bundleDirs.
     * @return
     */
    void GetBundleDirFromScan(std::list<std::string> &bundleDirs);
    /**
     * @brief Process scan dir.
     * @param dir Indicates the dir.
     * @param bundleDirs Indicates the return bundleDirs.
     * @return
     */
    void ProcessScanDir(const std::string &dir, std::list<std::string> &bundleDirs);
    /**
     * @brief Get bundle dir by pre bundle profile.
     * @param scanInfos Indicates the scanInfos contains installed info.
     * @param uninstallBundleNames Indicates the uninstall bundleNames.
     * @param preBundleConfigInfos Indicates the preBundleConfigInfos.
     * @return
     */
    void GetBundleDirFromPreBundleProFile(
        std::set<PreScanInfo> &scanInfos,
        std::set<std::string> &uninstallBundleNames,
        std::set<PreBundleConfigInfo> &preBundleConfigInfos);
    /**
     * @brief Process parse pre bundle profile.
     * @param scanInfos Indicates the scanInfos contains installed info.
     * @param uninstallBundleNames Indicates the uninstall bundleNames.
     * @param preBundleConfigInfos Indicates the preBundleConfigInfos.
     * @return
     */
    void ParsePreBundleProFile(
        std::string dir,
        std::set<PreScanInfo> &scanInfos,
        std::set<std::string> &uninstallBundleNames,
        std::set<PreBundleConfigInfo> &preBundleConfigInfos);
    /**
     * @brief Set the flag indicates that all system and vendor applications installed.
     * @return
     */
    void SetAllInstallFlag() const;
    /**
     * @brief Get bundleinfo of HAP by path.
     * @param hapFilePath Indicates the absolute file path of the HAP.
     * @param infos Indicates the obtained BundleInfo object.
     * @return Returns true if the BundleInfo is successfully obtained; returns false otherwise.
     */
    bool ParseHapFiles(const std::string &hapFilePath,
        std::unordered_map<std::string, InnerBundleInfo> &infos);
    /**
     * @brief To check the version code and bundleName in all haps.
     * @param infos .Indicates all innerBundleInfo for all haps need to be installed.
     * @return Returns ERR_OK if haps checking successfully; returns error code otherwise.
     */
    ErrCode CheckAppLabelInfo(const std::unordered_map<std::string, InnerBundleInfo> &infos);
    /**
     * @brief OTA Install system app and system vendor bundles.
     * @param filePaths Indicates the filePaths.
     * @param appType Indicates the bundle type.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool OTAInstallSystemBundle(const std::vector<std::string> &filePaths, Constants::AppType appType);
    /**
     * @brief Used to determine whether the module has been installed. If the installation has
     *        been uninstalled, OTA install and upgrade will not be allowed.
     * @param bundleName Indicates the bundleName.
     * @param bundlePath Indicates the bundlePath.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool HasModuleSavedInPreInstalledDb(
        const std::string &bundleName, const std::string &bundlePath);
    /**
     * @brief Delete preInstallInfo to Db.
     * @param bundleName Indicates the bundleName.
     * @param bundlePath Indicates the bundlePath.
     */
    void DeletePreInfoInDb(
        const std::string &bundleName, const std::string &bundlePath, bool bundleLevel);
    /**
     * @brief Add parseInfos to map.
     * @param bundleName Indicates the bundleName.
     * @param infos Indicates the infos.
     */
    void AddParseInfosToMap(const std::string &bundleName,
        const std::unordered_map<std::string, InnerBundleInfo> &infos);

    // Used to save the information parsed by Hap in the scanned directory.
    std::map<std::string, std::unordered_map<std::string, InnerBundleInfo>> hapParseInfoMap_;
    // used to save application information that already exists in the Db.
    std::map<std::string, PreInstallBundleInfo> loadExistData_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_EVENT_HANDLER_H
