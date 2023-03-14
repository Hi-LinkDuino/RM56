/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_CLONE_MGR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_CLONE_MGR_H

#include <string>
#include "nocopyable.h"
#include "appexecfwk_errors.h"
#include "inner_bundle_info.h"
#include "install_param.h"
#include "bundle_data_mgr.h"

namespace OHOS {
namespace AppExecFwk {
class BundleCloneMgr {
public:
    BundleCloneMgr();
    ~BundleCloneMgr();
    /**
     * @brief Remove cloned bundle.
     * @param oldName Indicates the ontology application bundle name.
     * @param bundleName Indicates the bundle name of remove cloned bundle.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool RemoveClonedBundle(const std::string &oldName, const std::string &bundleName);
    /**
     * @brief create bundle clone.
     * @param bundleName Indicates the bundle name of create bundle clone.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool BundleClone(const std::string &bundleName);
    /**
     * @brief Set clone name into CloneAppName.json.
     * @param bundleName Indicates the cloned name to be set.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool SetCloneAppName(const std::string &bundleName);
    /**
     * @brief Delete cloned bundle's name from CloneAppName.json.
     * @param bundleName Indicates the cloned bundle's name.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool DeleteCloneAppName(const std::string &bundleName);
    /**
     * @brief The real procedure for bundle clone.
     * @param bundleName Indicates the bundle name of bundle clone.
     * @return Returns ERR_OK if the bundle clone successfully; returns error code otherwise.
     */
    ErrCode ProcessCloneInstall(const std::string &bundleName);
    /**
     * @brief Create the code and data directories of a bundle.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @return Returns ERR_OK if the bundle directories created successfully; returns error code otherwise.
     */
    ErrCode CreateBundleAndDataDir(InnerBundleInfo &info) const;
    /**
     * @brief Update the bundle paths in the InnerBundleInfo object.
     * @param info Indicates the InnerBundleInfo object of a bundle under clone.
     * @param baseDataPath Indicates the data file paths.
     * @return Returns true if the path set successfully; returns false otherwise.
     */
    bool UpdateBundlePaths(InnerBundleInfo &info, const std::string baseDataPath) const;
    /**
     * @brief Modify the clone directory path for different clone type.
     * @param info Indicates the InnerBundleInfo object of a bundle under clone.
     * @return Returns true if the path set successfully; returns false otherwise.
     */
    bool ModifyInstallDirByHapType(InnerBundleInfo &info);
    /**
     * @brief Extract the code to temporilay directory and rename it.
     * @param info Indicates the InnerBundleInfo object of a bundle.
     * @return Returns ERR_OK if the bundle extract and renamed successfully; returns error code otherwise.
     */
    ErrCode ExtractModuleAndRename(InnerBundleInfo &info);
    /**
     * @brief Extract files of the current clone module package.
     * @param info Indicates the InnerBundleInfo object of a bundle under clone.
     * @return Returns ERR_OK if the module files extraced successfully; returns error code otherwise.
     */
    ErrCode ExtractModuleFiles(InnerBundleInfo &info);
    /**
     * @brief Create the data directories of current clone module package.
     * @param info Indicates the InnerBundleInfo object of a bundle under clone.
     * @return Returns ERR_OK if the module directory created successfully; returns error code otherwise.
     */
    ErrCode CreateModuleDataDir(InnerBundleInfo &info) const;
    /**
     * @brief Create clone bundle info.
     * @param bundleName Indicates the bundle Names.
     * @return Returns true if clone bundle is success; returns false otherwise.
     */
    bool CreateCloneBundleInfo(const std::string &bundleName);
    /**
     * @brief Save the cloned info to DB.
     * @return Returns true if save cloned info successfully; returns false otherwise.
     */
    bool SaveNewInfoToDB();
    /**
     * @brief Determine whether the application is in the allow list.
     * @param bundleName Indicates the bundle Names.
     * @return Returns true if bundle name in the allow list successfully; returns false otherwise.
     */
    bool CheckBundleNameInAllowList(const std::string &bundleName);

private:
    std::shared_ptr<BundleDataMgr> dataMgr_ = nullptr;  // this pointer will get when public functions called
    std::string bundleName_;
    std::string moduleTmpDir_;
    std::string modulePath_;
    std::string baseCodePath_;
    std::string baseDataPath_;
    std::string modulePackage_;
    std::string mainAbility_;
    InnerBundleInfo cloneInfo_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_CLONE_MGR_H