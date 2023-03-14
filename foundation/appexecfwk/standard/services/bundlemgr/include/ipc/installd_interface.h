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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_INSTALLD_INTERFACE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_INSTALLD_INTERFACE_H

#include <string>
#include <vector>

#include "iremote_broker.h"

#include "appexecfwk_errors.h"

namespace OHOS {
namespace AppExecFwk {
class IInstalld : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.Installd");
    /**
     * @brief Create a bundle code directory.
     * @param bundleDir Indicates the bundle code directory path that to be created.
     * @return Returns ERR_OK if the bundle directory created successfully; returns error code otherwise.
     */
    virtual ErrCode CreateBundleDir(const std::string &bundleDir) = 0;
    /**
     * @brief Extract the files of a HAP module to the code directory.
     * @param srcModulePath Indicates the HAP file path.
     * @param targetPath normal files decompression path.
     * @param targetSoPath so files decompression path.
     * @param cpuAbi cpuAbi.
     * @return Returns ERR_OK if the HAP file extracted successfully; returns error code otherwise.
     */
    virtual ErrCode ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath,
        const std::string &targetSoPath, const std::string &cpuAbi) = 0;
    /**
     * @brief Rename the module directory from temporaily path to the real path.
     * @param oldPath Indicates the old path name.
     * @param newPath Indicates the new path name.
     * @return Returns ERR_OK if the module directory renamed successfully; returns error code otherwise.
     */
    virtual ErrCode RenameModuleDir(const std::string &oldDir, const std::string &newDir) = 0;
    /**
     * @brief Create a bundle data directory.
     * @param bundleName Indicates bundleName to be set to the directory.
     * @param userid Indicates userid to be set to the directory.
     * @param uid Indicates uid to be set to the directory.
     * @param gid Indicates gid to be set to the directory.
     * @param apl Indicates apl to be set to the directory.
     * @return Returns ERR_OK if the bundle data directory created successfully; returns error code otherwise.
     */
    virtual ErrCode CreateBundleDataDir(const std::string &bundleName,
        const int userid, const int uid, const int gid, const std::string &apl) = 0;
    /**
     * @brief Remove a bundle data directory.
     * @param bundleDir Indicates the bundle data directory path that to be created.
     * @param userid Indicates userid to be set to the directory.
     * @return Returns ERR_OK if the bundle data directory created successfully; returns error code otherwise.
     */
    virtual ErrCode RemoveBundleDataDir(const std::string &bundleDir, const int userid) = 0;
    /**
     * @brief Remove a module and it's abilities data directory.
     * @param ModuleDir Indicates the module data directory path that to be created.
     * @param userid Indicates userid to be set to the directory.
     * @return Returns ERR_OK if the data directories created successfully; returns error code otherwise.
     */
    virtual ErrCode RemoveModuleDataDir(const std::string &ModuleDir, const int userid)= 0;
    /**
     * @brief Remove a directory.
     * @param dir Indicates the directory path that to be removed.
     * @return Returns ERR_OK if the  directory removed successfully; returns error code otherwise.
     */
    virtual ErrCode RemoveDir(const std::string &dir) = 0;
    /**
     * @brief Clean all files in a bundle data directory.
     * @param bundleDir Indicates the data directory path that to be cleaned.
     * @return Returns ERR_OK if the data directory cleaned successfully; returns error code otherwise.
     */
    virtual ErrCode CleanBundleDataDir(const std::string &bundleDir) = 0;
    /**
     * @brief Get bundle Stats.
     * @param bundleName Indicates the bundle name.
     * @param userId Indicates the user Id.
     * @param bundleStats Indicates the bundle Stats.
     * @return Returns ERR_OK if get stats successfully; returns error code otherwise.
     */
    virtual ErrCode GetBundleStats(
        const std::string &bundleName, const int32_t userId, std::vector<int64_t> &bundleStats) = 0;
    /**
     * @brief Set dir apl.
     * @param dir Indicates the data dir.
     * @param bundleName Indicates the bundle name.
     * @param apl Indicates the apl type.
     * @return Returns ERR_OK if set apl successfully; returns error code otherwise.
     */
    virtual ErrCode SetDirApl(const std::string &dir, const std::string &bundleName, const std::string &apl) = 0;
    /**
     * @brief Get all cache file path.
     * @param dir Indicates the data dir.
     * @param cachesPath Indicates the cache file path.
     * @return Returns ERR_OK if get cache file path successfully; returns error code otherwise.
     */
    virtual ErrCode GetBundleCachePath(const std::string &dir, std::vector<std::string> &cachePath) = 0;
protected:
    enum Message : uint32_t {
        CREATE_BUNDLE_DIR = 1,
        EXTRACT_MODULE_FILES,
        RENAME_MODULE_DIR,
        CREATE_BUNDLE_DATA_DIR,
        CLEAN_BUNDLE_DATA_DIR,
        REMOVE_BUNDLE_DATA_DIR,
        REMOVE_MODULE_DATA_DIR,
        REMOVE_DIR,
        GET_BUNDLE_STATS,
        SET_DIR_APL,
        GET_BUNDLE_CACHE_PATH
    };
};

#define INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(parcel, token)                        \
    do {                                                                            \
        bool ret = parcel.WriteInterfaceToken((token));                             \
        if (!ret) {                                                                 \
            APP_LOGE("fail to write interface token into the parcel!");             \
            return ERR_APPEXECFWK_PARCEL_ERROR;                                     \
        }                                                                           \
    } while (0)

#define INSTALLD_PARCEL_WRITE(parcel, type, value)                                  \
    do {                                                                            \
        bool ret = parcel.Write##type((value));                                     \
        if (!ret) {                                                                 \
            APP_LOGE("fail to write parameter into the parcel!");                   \
            return ERR_APPEXECFWK_PARCEL_ERROR;                                     \
        }                                                                           \
    } while (0)
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_INSTALLD_INTERFACE_H