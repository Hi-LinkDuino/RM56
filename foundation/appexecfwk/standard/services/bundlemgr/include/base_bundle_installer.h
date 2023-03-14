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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_BUNDLE_INSTALLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_BUNDLE_INSTALLER_H

#include <map>
#include <string>

#include "nocopyable.h"

#include "appexecfwk_errors.h"
#include "bundle_clone_mgr.h"
#include "bundle_data_mgr.h"
#include "bundle_pack_info.h"
#include "bundle_verify_mgr.h"
#include "event_report.h"
#include "inner_bundle_info.h"
#include "install_param.h"

namespace OHOS {
namespace AppExecFwk {
class BaseBundleInstaller {
public:
    BaseBundleInstaller();
    virtual ~BaseBundleInstaller();

protected:
    enum class InstallerState {
        INSTALL_START,
        INSTALL_BUNDLE_CHECKED = 5,
        INSTALL_SYSCAP_CHECKED = 10,
        INSTALL_SIGNATURE_CHECKED = 15,
        INSTALL_PARSED = 20,
        INSTALL_HAP_HASH_PARAM_CHECKED = 25,
        INSTALL_VERSION_AND_BUNDLENAME_CHECKED = 30,
        INSTALL_CREATDIR = 40,
        INSTALL_REMOVE_SANDBOX_APP = 50,
        INSTALL_EXTRACTED = 60,
        INSTALL_INFO_SAVED = 80,
        INSTALL_RENAMED = 90,
        INSTALL_SUCCESS = 100,
        INSTALL_FAILED,
    };

    enum SingletonState {
        DEFAULT,
        SINGLETON_TO_NON = 1,
        NON_TO_SINGLETON = 2,
    };

    /**
     * @brief The main function for system and normal bundle install.
     * @param bundlePath Indicates the path for storing the HAP file of the application
     *                   to install or update.
     * @param installParam Indicates the install parameters.
     * @param appType Indicates the application type.
     * @return Returns ERR_OK if the application install successfully; returns error code otherwise.
     */
    ErrCode InstallBundle(
        const std::string &bundlePath, const InstallParam &installParam, const Constants::AppType appType);
    /**
     * @brief The main function for system and normal bundle install.
     * @param bundlePaths Indicates the paths for storing the HAP file sof the application
     *                   to install or update.
     * @param installParam Indicates the install parameters.
     * @param appType Indicates the application type.
     * @return Returns ERR_OK if the application install successfully; returns error code otherwise.
     */
    ErrCode InstallBundle(const std::vector<std::string> &bundlePaths, const InstallParam &installParam,
        const Constants::AppType appType);
    /**
     * @brief The main function for uninstall a bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @return Returns ERR_OK if the application uninstall successfully; returns error code otherwise.
     */
    ErrCode UninstallBundle(const std::string &bundleName, const InstallParam &installParam);
    /**
     * @brief The main function for uninstall a module in a specific bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @return Returns ERR_OK if the application uninstall successfully; returns error code otherwise.
     */
    ErrCode UninstallBundle(
        const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam);
    /**
     * @brief Update the installer state.
     * @attention This function changes the base class state only.
     * @param state Indicates the state to be updated to.
     * @return
     */
    virtual void UpdateInstallerState(const InstallerState state);
    /**
     * @brief Get the installer state.
     * @return The current state of the installer object.
     */
    inline InstallerState GetInstallerState()
    {
        return state_;
    }
    /**
     * @brief Get the installer state.
     * @param state Indicates the state to be updated to.
     * @return
     */
    inline void SetInstallerState(InstallerState state)
    {
        state_ = state;
    }
    /**
     * @brief The main function for bundle install by bundleName.
     * @param bundleName Indicates the bundleName of the application to install.
     * @param installParam Indicates the install parameters.
     * @return Returns ERR_OK if the application install successfully; returns error code otherwise.
     */
    ErrCode Recover(const std::string &bundleName, const InstallParam &installParam);
    /**
     * @brief The main function for bundle install by bundleName.
     * @param bundleName Indicates the bundleName of the application to install.
     * @param installParam Indicates the install parameters.
     * @return Returns ERR_OK if the application install successfully; returns error code otherwise.
     */
    ErrCode InstallBundleByBundleName(const std::string &bundleName, const InstallParam &installParam);
    /**
     * @brief Reset install properties.
     */
    void ResetInstallProperties();
    /**
     * @brief Reset install properties.
     * @param isBootScene Indicates the event occurs in the boot phase.
     */
    void MarkPreBundleSyeEventBootTag(bool isBootScene)
    {
        sysEventInfo_.preBundleScene =
            isBootScene ? InstallScene::BOOT : InstallScene::REBOOT;
    }
    /**
     * @brief Save hap to install path when install.
     * @param moveFileMode Indicates the moveFileMode.
     * @return
     */
    void SaveHapToInstallPath(bool moveFileMode);

private:
    /**
     * @brief The real procedure for system and normal bundle install.
     * @param bundlePath Indicates the path for storing the HAP file of the application
     *                   to install or update.
     * @param installParam Indicates the install parameters.
     * @param appType Indicates the application type.
     * @param uid Indicates the uid of the application.
     * @return Returns ERR_OK if the bundle install successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleInstall(const std::vector<std::string> &bundlePaths, const InstallParam &installParam,
        const Constants::AppType appType, int32_t &uid);

    ErrCode InnerProcessBundleInstall(std::unordered_map<std::string, InnerBundleInfo> &newInfos,
        InnerBundleInfo &oldInfo, const InstallParam &installParam, int32_t &uid);
    /**
     * @brief The real procedure function for uninstall a bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param uid Indicates the uid of the application.
     * @return Returns ERR_OK if the bundle uninstall successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleUninstall(const std::string &bundleName, const InstallParam &installParam, int32_t &uid);
    /**
     * @brief The real procedure for uninstall a module in a specific bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param uid Indicates the uid of the application.
     * @return Returns ERR_OK if the module uninstall successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleUninstall(const std::string &bundleName, const std::string &modulePackage,
        const InstallParam &installParam, int32_t &uid);
    /**
     * @brief The process of installing a new bundle.
     * @param info Indicates the InnerBundleInfo parsed from the config.json in the HAP package.
     * @param uid Indicates the uid of the application.
     * @return Returns ERR_OK if the new bundle install successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleInstallStatus(InnerBundleInfo &info, int32_t &uid);
    /**
     * @brief The process of updating an exist bundle.
     * @param oldInfo Indicates the exist InnerBundleInfo object get from the database.
     * @param newInfo Indicates the InnerBundleInfo object parsed from the config.json in the HAP package.
     * @param isReplace Indicates whether there is the replace flag in the install flag.
     * @return Returns ERR_OK if the bundle updating successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleUpdateStatus(InnerBundleInfo &oldInfo,
        InnerBundleInfo &newInfo, bool isReplace, bool noSkipsKill = true);
    /**
     * @brief Remove a whole bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @param isKeepData Indicates that whether to save data.
     * @return Returns ERR_OK if the bundle removed successfully; returns error code otherwise.
     */
    ErrCode RemoveBundle(InnerBundleInfo &info, bool isKeepData);
    /**
     * @brief Create the code and data directories of a bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @return Returns ERR_OK if the bundle directories created successfully; returns error code otherwise.
     */
    ErrCode CreateBundleAndDataDir(InnerBundleInfo &info) const;
    /**
     * @brief Extract the code to temporilay directory and rename it.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @param modulePath normal files decompression path.
     * @return Returns ERR_OK if the bundle extract and renamed successfully; returns error code otherwise.
     */
    ErrCode ExtractModule(InnerBundleInfo &info, const std::string &modulePath);
    /**
     * @brief Remove the code and data directories of a bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @param isKeepData Indicates that whether to save data.
     * @return Returns ERR_OK if the bundle directories removed successfully; returns error code otherwise.
     */
    ErrCode RemoveBundleAndDataDir(const InnerBundleInfo &info, bool isKeepData) const;
    /**
     * @brief Remove the code and data directories of a module in a bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @param modulePackage Indicates the module to be removed.
     * @param userId Indicates the userId.
     * @param isKeepData Indicates that whether to save data.
     * @return Returns ERR_OK if the bundle directories removed successfully; returns error code otherwise.
     */
    ErrCode RemoveModuleAndDataDir(const InnerBundleInfo &info,
        const std::string &modulePackage, int32_t userId, bool isKeepData) const;
    /**
     * @brief Parse the bundle config.json file.
     * @param bundleFilePath Indicates the HAP file path.
     * @param InnerBundleInfo Indicates the InnerBundleInfo object of a bundle.
     * @return Returns ERR_OK if the bundle parsed successfully; returns error code otherwise.
     */
    ErrCode ParseBundleInfo(const std::string &bundleFilePath, InnerBundleInfo &info, BundlePackInfo &packInfo) const;
    /**
     * @brief Remove the current installing module directory.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @return Returns ERR_OK if the module directory removed successfully; returns error code otherwise.
     */
    ErrCode RemoveModuleDir(const std::string &modulePath) const;
    /**
     * @brief Extract files of the current installing module package.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @param modulePath normal files decompression path.
     * @param targetSoPath so files decompression path.
     * @param cpuAbi cpuAbi.
     * @return Returns ERR_OK if the module files extraced successfully; returns error code otherwise.
     */
    ErrCode ExtractModuleFiles(const InnerBundleInfo &info, const std::string &modulePath,
        const std::string &targetSoPath, const std::string &cpuAbi);
    /**
     * @brief Rename the directory of current installing module package.
     * @param info Indicates the InnerBundleInfo object of a bundle under installing.
     * @return Returns ERR_OK if the module directory renamed successfully; returns error code otherwise.
     */
    ErrCode RenameModuleDir(const InnerBundleInfo &info) const;
    /**
     * @brief The process of install a new module package.
     * @param newInfo Indicates the InnerBundleInfo object parsed from the config.json in the HAP package.
     * @param oldInfo Indicates the exist InnerBundleInfo object get from the database.
     * @return Returns ERR_OK if the new module install successfully; returns error code otherwise.
     */
    ErrCode ProcessNewModuleInstall(InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo);
    /**
     * @brief The process of updating an exist module package.
     * @param newInfo Indicates the InnerBundleInfo object parsed from the config.json in the HAP package.
     * @param oldInfo Indicates the exist InnerBundleInfo object get from the database.
     * @return Returns ERR_OK if the module updating successfully; returns error code otherwise.
     */
    ErrCode ProcessModuleUpdate(InnerBundleInfo &newInfo,
        InnerBundleInfo &oldInfo, bool isReplace, bool noSkipsKill = true);
    /**
     * @brief try to get the bundle info to decide use install or update.
     * @param newInfo Indicates the InnerBundleInfo object parsed from the config.json in the HAP package.
     * @param uid Indicates the uid of the application.
     * @param installFlag Indicates install Flag.
     * @return Returns ERR_OK if the bundle install successfully; returns error code otherwise.
     */
    ErrCode ProcessBundleStatus(InnerBundleInfo &newInfo, int32_t &uid, const InstallFlag &installFlag);
    /**
     * @brief The real procedure for bundle install by bundleName.
     * @param bundleName Indicates the bundleName the application to install.
     * @param installParam Indicates the install parameters.
     * @param uid Indicates the uid of the application.
     * @return Returns ERR_OK if the bundle install successfully; returns error code otherwise.
     */
    ErrCode ProcessRecover(
        const std::string &bundleName, const InstallParam &installParam, int32_t &uid);
    /**
     * @brief The real procedure for bundle install by bundleName.
     * @param bundleName Indicates the bundleName the application to install.
     * @param installParam Indicates the install parameters.
     * @param uid Indicates the uid of the application.
     * @return Returns ERR_OK if the bundle install successfully; returns error code otherwise.
     */
    ErrCode ProcessInstallBundleByBundleName(
        const std::string &bundleName, const InstallParam &installParam, int32_t &uid);
    /**
     * @brief The real procedure for bundle install by bundleName.
     * @param bundleName Indicates the bundleName the application to install.
     * @param installParam Indicates the install parameters.
     * @param uid Indicates the uid of the application.
     * @param recoverMode Indicates the recoverMode or not.
     * @return Returns ERR_OK if the bundle install successfully; returns error code otherwise.
     */
    ErrCode InnerProcessInstallByPreInstallInfo(
        const std::string &bundleName, const InstallParam &installParam, int32_t &uid, bool recoverMode);
    /**
     * @brief Check syscap.
     * @param bundlePaths Indicates the file paths of all HAP packages.
     * @return Returns ERR_OK if the syscap satisfy; returns error code otherwise.
     */
    ErrCode CheckSysCap(const std::vector<std::string> &bundlePaths);
    /**
     * @brief Check signature info of multiple haps.
     * @param bundlePaths Indicates the file paths of all HAP packages.
     * @param installParam Indicates the install parameters.
     * @param hapVerifyRes Indicates the signature info.
     * @return Returns ERR_OK if the every hap has signature info and all haps have same signature info.
     */
    ErrCode CheckMultipleHapsSignInfo(const std::vector<std::string> &bundlePaths, const InstallParam &installParam,
        std::vector<Security::Verify::HapVerifyResult> &hapVerifyRes) const;
    /**
     * @brief To parse hap files and to obtain innerBundleInfo of each hap.
     * @param bundlePaths Indicates the file paths of all HAP packages.
     * @param installParam Indicates the install parameters.
     * @param appType Indicates the app type of the hap.
     * @param hapVerifyRes Indicates all signature info of all haps.
     * @param infos Indicates the innerBundleinfo of each hap.
     * @return Returns ERR_OK if each hap is parsed successfully; returns error code otherwise.
     */
    ErrCode ParseHapFiles(const std::vector<std::string> &bundlePaths, const InstallParam &installParam,
        const Constants::AppType appType, std::vector<Security::Verify::HapVerifyResult> &hapVerifyRes,
        std::unordered_map<std::string, InnerBundleInfo> &infos);
    /**
     * @brief To check the hap hash param.
     * @param infos .Indicates all innerBundleInfo for all haps need to be installed.
     * @param hashParams .Indicates all hashParams in installParam.
     * @return Returns ERR_OK if haps checking successfully; returns error code otherwise.
     */
    ErrCode CheckHapHashParams(
        std::unordered_map<std::string, InnerBundleInfo> &infos,
        std::map<std::string, std::string> hashParams);
    /**
     * @brief To check the version code and bundleName in all haps.
     * @param infos .Indicates all innerBundleInfo for all haps need to be installed.
     * @return Returns ERR_OK if haps checking successfully; returns error code otherwise.
     */
    ErrCode CheckAppLabelInfo(const std::unordered_map<std::string, InnerBundleInfo> &infos);
    /**
     * @brief To roll back when the installation is failed.
     * @param infos .Indicates the innerBundleInfo needs to be roll back.
     * @param oldInfo Indicates the original innerBundleInfo of the bundle.
     * @return Returns ERR_OK if roll back successfully; returns error code otherwise.
     */
    void RollBack(const InnerBundleInfo &info, InnerBundleInfo &oldInfo);
    /**
     * @brief To check the version code and bundleName in all haps.
     * @param newInfos .Indicates all innerBundleInfo for all haps need to be rolled back.
     * @param oldInfo Indicates the original innerBundleInfo of the bundle.
     * @return Returns ERR_OK if roll back successfully; returns error code otherwise.
     */
    void RollBack(const std::unordered_map<std::string, InnerBundleInfo> &newInfos, InnerBundleInfo &oldInfo);
    /**
     * @brief To remove innerBundleInfo or moduleInfo of the corresponding haps.
     * @param bundleName Indicates the bundle name of the bundle which needs to be removed innerBundleInfo or
     *                   moudleInfo.
     * @param packageName Indicates the package name of the hap which needs to be removed the moduleInfo.
     * @return Returns ERR_OK if the removing is successful; returns error code otherwise.
     */
    void RemoveInfo(const std::string &bundleName, const std::string &packageName);
    /**
     * @brief To roll back the moduleInfo of the corresponding haps.
     * @param bundleName Indicates the bundle name of the bundle which needs to be rolled back the moudleInfo.
     * @param oldInfo Indicates the original innerBundleInfo of the bundle.
     * @return Returns ERR_OK if the rollback is successful; returns error code otherwise.
     */
    void RollBackMoudleInfo(const std::string &bundleName, InnerBundleInfo &oldInfo);
    /**
     * @brief To obtain the innerBundleInfo of the corresponding hap.
     * @param info Indicates the innerBundleInfo obtained.
     * @param isAppExist Indicates if the innerBundleInfo is existed or not.
     * @return Returns ERR_OK if the innerBundleInfo is obtained successfully; returns error code otherwise.
     */
    bool GetInnerBundleInfo(InnerBundleInfo &info, bool &isAppExist);
    /**
     * @brief To check whether the version code is compatible for application or not.
     * @param oldInfo Indicates the original innerBundleInfo of the bundle.
     * @return Returns ERR_OK if version code is checked successfully; returns error code otherwise.
     */
    ErrCode CheckVersionCompatibility(const InnerBundleInfo &oldInfo);
    /**
     * @brief To check whether the version code is compatible for application or not.
     * @param oldInfo Indicates the original innerBundleInfo of the bundle.
     * @return Returns ERR_OK if version code is checked successfully; returns error code otherwise.
     */
    ErrCode CheckVersionCompatibilityForApplication(const InnerBundleInfo &oldInfo);
    /**
     * @brief To check whether the version code is compatible for openharmony service or not.
     * @param info Indicates the original innerBundleInfo of the bundle.
     * @return Returns ERR_OK if version code is checked successfully; returns error code otherwise.
     */
    ErrCode CheckVersionCompatibilityForHmService(const InnerBundleInfo &oldInfo);
    /**
     * @brief To uninstall lower version feature haps.
     * @param info Indicates the innerBundleInfo of the bundle.
     * @param packageVec Indicates the array of package names of the high version entry or feature hap.
     * @return Returns ERR_OK if uninstall successfully; returns error code otherwise.
     */
    ErrCode UninstallLowerVersionFeature(const std::vector<std::string> &packageVec);
    /**
     * @brief Check whether the disk path memory is available for installing the hap.
     * @param bundlePath Indicates the file path.
     * @param appType Indicates the bundle type of the application.
     * @return Returns ERR_OK if system memory is adequate; returns error code otherwise.
     */
    ErrCode CheckSystemSize(const std::string &bundlePath, const Constants::AppType appType) const;
    /**
     * @brief To get userId.
     * @param installParam Indicates the installParam of the bundle.
     * @return Returns userId.
     */
    int32_t GetUserId(const int32_t &userId) const;
    /**
     * @brief Remove bundle user data.
     * @param innerBundleInfo Indicates the innerBundleInfo of the bundle.
     * @param needRemoveData Indicates need remove data or not.
     * @return Returns BundleUserMgr.
     */
    ErrCode RemoveBundleUserData(InnerBundleInfo &innerBundleInfo, bool needRemoveData = true);
    /**
     * @brief Create bundle user data.
     * @param innerBundleInfo Indicates the bundle type of the application.
     * @return Returns ERR_OK if result is ok; returns error code otherwise.
     */
    ErrCode CreateBundleUserData(InnerBundleInfo &innerBundleInfo);

    bool verifyUriPrefix(const InnerBundleInfo &info, int32_t userId, bool isUpdate = false) const;

    ErrCode CheckHapModleOrType(const InnerBundleInfo &innerBundleInfo,
        const std::unordered_map<std::string, InnerBundleInfo> &infos) const;
    void SetEntryInstallationFree(const BundlePackInfo &bundlePackInfo, InnerBundleInfo &innerBundleInfo);

private:
    ErrCode CreateBundleCodeDir(InnerBundleInfo &info) const;
    ErrCode CreateBundleDataDir(InnerBundleInfo &info) const;
    ErrCode RemoveModuleDataDir(const InnerBundleInfo &info, const std::string &modulePackage,
        int32_t userId) const;
    ErrCode RemoveBundleCodeDir(const InnerBundleInfo &info) const;
    ErrCode RemoveBundleDataDir(const InnerBundleInfo &info) const;
    uint32_t CreateAccessTokenId(const InnerBundleInfo &info);
    ErrCode GrantRequestPermissions(const InnerBundleInfo &info, const uint32_t tokenId);
    ErrCode UpdateDefineAndRequestPermissions(const InnerBundleInfo &oldInfo, const InnerBundleInfo &newInfo);
    ErrCode SetDirApl(const InnerBundleInfo &info);
    /**
     * @brief Check to set isRemovable true when install.
     * @param newInfos Indicates all innerBundleInfo for all haps need to be installed.
     * @param oldInfo Indicates the original innerBundleInfo of the bundle.
     * @param userId Indicates the userId.
     * @param isFreeInstallFlag Indicates whether is FREE_INSTALL flag.
     * @param isAppExist Indicates whether app is exist.
     * @return
     */
    void CheckEnableRemovable(std::unordered_map<std::string, InnerBundleInfo> &newInfos,
        InnerBundleInfo &oldInfo, int32_t &userId, bool isFreeInstallFlag, bool isAppExist);
    /**
     * @brief Save oldInfo isRemovable to newInfo isRemovable.
     * @param newModuleInfo Indicates the old innerModuleInfo of the bundle..
     * @param oldInfo Indicates the old innerBundleInfo of the bundle.
     * @param existModule Indicates whether module is exist.
     * @return
     */
    void SaveOldRemovableInfo(InnerModuleInfo &newModuleInfo, InnerBundleInfo &oldInfo, bool existModule);
    /**
     * @brief Save hap path to records.
     * @param isPreInstallApp Indicates isPreInstallApp or not.
     * @param infos Indicates all innerBundleInfo for all haps need to be installed.
     * @return
     */
    void SaveHapPathToRecords(
        bool isPreInstallApp, const std::unordered_map<std::string, InnerBundleInfo> &infos);
    void OnSingletonChange();
    ErrCode UninstallAllSandboxApps(const std::string &bundleName, int32_t userId = Constants::INVALID_USERID);
    ErrCode CheckAppLabel(const InnerBundleInfo &oldInfo, const InnerBundleInfo &newInfo) const;
    void SendBundleSystemEvent(const std::string &bundleName, BundleEventType bundleEventType,
        const InstallParam &installParam, InstallScene preBundleScene, ErrCode errCode);

    InstallerState state_ = InstallerState::INSTALL_START;
    std::shared_ptr<BundleDataMgr> dataMgr_ = nullptr;  // this pointer will get when public functions called
    std::shared_ptr<BundleCloneMgr> cloneMgr_ = nullptr;
    std::string bundleName_;
    std::string moduleTmpDir_;
    std::string modulePath_;
    std::string baseDataPath_;
    std::string modulePackage_;
    std::string mainAbility_;
    // key is package name, value is boolean
    std::unordered_map<std::string, bool> installedModules_;
    bool isAppExist_ = false;
    bool isContainEntry_ = false;
    uint32_t versionCode_ = 0;
    // value is packageName for uninstalling
    bool isFeatureNeedUninstall_ = false;
    std::vector<std::string> uninstallModuleVec_;

    int32_t userId_ = Constants::INVALID_USERID;
    bool hasInstalledInUser_ = false;
    SingletonState singletonState_ = SingletonState::DEFAULT;
    std::map<std::string, std::string> hapPathRecords_;
    // used to record system event infos
    EventInfo sysEventInfo_;

    DISALLOW_COPY_AND_MOVE(BaseBundleInstaller);

#define CHECK_RESULT(errcode, errmsg)                                              \
    do {                                                                           \
        if (errcode != ERR_OK) {                                                   \
            APP_LOGE(errmsg, errcode);                                             \
            return errcode;                                                        \
        }                                                                          \
    } while (0)

#define CHECK_RESULT_WITH_ROLLBACK(errcode, errmsg, newInfos, oldInfo)             \
    do {                                                                           \
        if ((errcode) == ERR_APPEXECFWK_INSTALL_SINGLETON_NOT_SAME ||              \
            (errcode) == ERR_APPEXECFWK_INSTALL_ZERO_USER_WITH_NO_SINGLETON) {     \
            APP_LOGE(errmsg, errcode);                                             \
            return errcode;                                                        \
        }                                                                          \
                                                                                   \
        if (errcode != ERR_OK) {                                                   \
            APP_LOGE(errmsg, errcode);                                             \
            RollBack(newInfos, oldInfo);                                           \
            return errcode;                                                        \
        }                                                                          \
    } while (0)
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_BUNDLE_INSTALLER_H