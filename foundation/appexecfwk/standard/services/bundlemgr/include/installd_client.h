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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_CLIENT_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_CLIENT_H

#include <memory>
#include <mutex>
#include <string>

#include "nocopyable.h"
#include "singleton.h"

#include "appexecfwk_errors.h"
#include "ipc/installd_interface.h"

namespace OHOS {
namespace AppExecFwk {
class InstalldClient : public DelayedSingleton<InstalldClient> {
public:
    /**
     * @brief Create a bundle code directory through an installd proxy object.
     * @param bundleDir Indicates the bundle code directory path that to be created.
     * @return Returns ERR_OK if the bundle directory created successfully; returns error code otherwise.
     */
    ErrCode CreateBundleDir(const std::string &bundleDir);
    /**
     * @brief Extract the files of a HAP module to the code directory.
     * @param srcModulePath Indicates the HAP file path.
     * @param targetPath normal files decompression path.
     * @param targetSoPath so files decompression path.
     * @param cpuAbi cpuAbi.
     * @return Returns ERR_OK if the HAP file extracted successfully; returns error code otherwise.
     */
    ErrCode ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath,
        const std::string &targetSoPath, const std::string &cpuAbi);
    /**
     * @brief Rename the module directory from temporaily path to the real path.
     * @param oldPath Indicates the old path name.
     * @param newPath Indicates the new path name.
     * @return Returns ERR_OK if the module directory renamed successfully; returns error code otherwise.
     */
    ErrCode RenameModuleDir(const std::string &oldPath, const std::string &newPath);
    /**
     * @brief Create a bundle data directory.
     * @param bundleName Indicates bundleName to be set to the directory.
     * @param userid Indicates userid to be set to the directory.
     * @param uid Indicates uid to be set to the directory.
     * @param gid Indicates gid to be set to the directory.
     * @param apl Indicates apl to be set to the directory.
     * @return Returns ERR_OK if the bundle data directory created successfully; returns error code otherwise.
     */
    ErrCode CreateBundleDataDir(const std::string &bundleName,
        const int userid, const int uid, const int gid, const std::string &apl);
    /**
     * @brief Remove a bundle data directory.
     * @param bundleName Indicates the bundleName data directory path that to be created.
     * @param userid Indicates userid to be set to the directory.
     * @return Returns ERR_OK if the bundle data directory created successfully; returns error code otherwise.
     */
    ErrCode RemoveBundleDataDir(const std::string &bundleName, const int userid);
    /**
     * @brief Remove a module data directory.
     * @param ModuleDir Indicates the module data directory path that to be created.
     * @param userid Indicates userid to be set to the directory.
     * @return Returns ERR_OK if the data directories created successfully; returns error code otherwise.
     */
    ErrCode RemoveModuleDataDir(const std::string &ModuleDir, const int userid);
    /**
     * @brief Remove a directory.
     * @param dir Indicates the directory path that to be removed.
     * @return Returns ERR_OK if the  directory removed successfully; returns error code otherwise.
     */
    ErrCode RemoveDir(const std::string &dir);
    /**
     * @brief Clean all files in a bundle data directory.
     * @param bundleDir Indicates the data directory path that to be cleaned.
     * @return Returns ERR_OK if the data directory cleaned successfully; returns error code otherwise.
     */
    ErrCode CleanBundleDataDir(const std::string &bundleDir);
    /**
     * @brief Get bundle Stats.
     * @param bundleName Indicates the bundle name.
     * @param userId Indicates the user Id.
     * @param bundleStats Indicates the bundle Stats.
     * @return Returns ERR_OK if get stats successfully; returns error code otherwise.
     */
    ErrCode GetBundleStats(const std::string &bundleName, const int32_t userId, std::vector<int64_t> &bundleStats);

    /**
     * @brief Reset the installd proxy object when installd service died.
     * @return
     */
    void ResetInstalldProxy();

    /**
     * @brief Set dir apl.
     * @param dir Indicates the data dir.
     * @param bundleName Indicates the bundle name.
     * @param apl Indicates the apl type.
     * @return Returns ERR_OK if set apl successfully; returns error code otherwise.
     */
    ErrCode SetDirApl(const std::string &dir, const std::string &bundleName, const std::string &apl);

    /**
     * @brief Get all cache file path.
     * @param dir Indicates the data dir.
     * @param cachesPath Indicates the cache file path.
     * @return Returns ERR_OK if get cache file path successfully; returns error code otherwise.
     */
    ErrCode GetBundleCachePath(const std::string &dir, std::vector<std::string> &cachePath);

private:
    /**
     * @brief Get the installd proxy object.
     * @return Returns true if the installd proxy object got successfully; returns false otherwise.
     */
    bool GetInstalldProxy();

    template<typename F, typename... Args>
    ErrCode CallService(F func, Args&&... args)
    {
        if (!GetInstalldProxy()) {
            return ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR;
        }
        return (installdProxy_->*func)(std::forward<Args>(args)...);
    }

private:
    std::mutex mutex_;
    sptr<IInstalld> installdProxy_;
    sptr<IRemoteObject::DeathRecipient> recipient_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_CLIENT_H